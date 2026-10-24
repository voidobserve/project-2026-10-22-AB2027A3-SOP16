#include "include.h"
#include "ble_service.h"
#include "ble_body_composition_service.h"

#define BODY_COMPOSITION_SVC_DEBUG_EN 1

#if BODY_COMPOSITION_SVC_DEBUG_EN
#define BODY_COMPOSITION_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BODY_COMPOSITION_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BODY_COMPOSITION_SVC_DEBUG(...)
#define BODY_COMPOSITION_SVC_DEBUG_R(...)
#endif

static ble_body_composition_var_t ble_body_comp_var;
static att_service_handler_t      ble_body_composition_service;
static uint16_t                   ble_body_composition_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0007 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BODY_COMPOSITION
//     0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x1b, 0x18,
//     // 0x0008 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT - DYNAMIC | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x20, 0x09, 0x00, 0x9c, 0x2a,
//     // 0x0009 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT - DYNAMIC | INDICATE
//     //
//     0x08, 0x00, 0x00, 0x01, 0x09, 0x00, 0x9c, 0x2a,
//     // 0x000a CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x0a, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x000b CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x03, 0x28, 0x02, 0x0c, 0x00, 0x9b, 0x2a,
//     // 0x000c VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x0c, 0x00, 0x9b, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_body_composition_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_body_composition_connect_handle, &param[7], 2);
            BODY_COMPOSITION_SVC_DEBUG("-->BLE_BODY_COMPOSITION_EVENT_CONNECTED:%x\n",ble_body_composition_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_body_composition_connect_handle, &param[0], 2);
            BODY_COMPOSITION_SVC_DEBUG("-->BLE_BODY_COMPOSITION_EVENT_DISCONNECTED:%x\n",ble_body_composition_connect_handle);
            ble_body_composition_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_body_composition_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    BODY_COMPOSITION_SVC_DEBUG("ble body_composition read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_body_comp_var.meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_body_comp_var.meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_body_comp_var.feature);
            if (buffer) {
                memcpy(buffer, ble_body_comp_var.feature, sizeof(ble_body_comp_var.feature));
            }
        } break;

        default:
            break;
    }

    BODY_COMPOSITION_SVC_DEBUG("read_len:%d\n", read_len);
    BODY_COMPOSITION_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_body_composition_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    BODY_COMPOSITION_SVC_DEBUG("ble body_composition write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_body_comp_var.meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_body_composition_service_init(void)
{
    BODY_COMPOSITION_SVC_DEBUG("ble_body_composition_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BODY_COMPOSITION_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BODY_COMPOSITION_END_HANDLE;

    // register service with ATT Server
	ble_body_composition_service.start_handle   = start_handle;
	ble_body_composition_service.end_handle     = end_handle;
	ble_body_composition_service.read_callback  = &ble_body_composition_service_read_callback;
	ble_body_composition_service.write_callback = &ble_body_composition_service_write_callback;
	ble_body_composition_service.event_handler  = &ble_body_composition_service_event_packet_handle;
	att_server_register_service_handler(&ble_body_composition_service);

    ble_body_composition_connect_handle = 0;
}

void ble_body_composition_service_proc(void)
{
    if (ble_body_composition_connect_handle == 0) {
        return;
    }
}

void ble_body_composition_service_measurement_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_body_comp_var.meas_client_cfg) {
        ble_indicate_for_handle(ble_body_composition_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BODY_COMPOSITION_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        BODY_COMPOSITION_SVC_DEBUG("body composition measurement indicate not subscribed!\n");
    }
}

