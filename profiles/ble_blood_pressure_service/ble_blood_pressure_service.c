#include "include.h"
#include "ble_service.h"
#include "ble_blood_pressure_service.h"

#define BLOOD_PRESSURE_SVC_DEBUG_EN 1

#if BLOOD_PRESSURE_SVC_DEBUG_EN
#define BLOOD_PRESSURE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BLOOD_PRESSURE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BLOOD_PRESSURE_SVC_DEBUG(...)
#define BLOOD_PRESSURE_SVC_DEBUG_R(...)
#endif

static ble_blood_pressure_var_t ble_blood_pressure_var;
static att_service_handler_t    ble_blood_pressure_service;
static uint16_t                 ble_blood_pressure_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x000d PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BLOOD_PRESSURE
//     0x0a, 0x00, 0x02, 0x00, 0x0d, 0x00, 0x00, 0x28, 0x10, 0x18,
//     // 0x000e CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_MEASUREMENT - DYNAMIC | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x0e, 0x00, 0x03, 0x28, 0x20, 0x0f, 0x00, 0x35, 0x2a,
//     // 0x000f VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_MEASUREMENT - DYNAMIC | INDICATE
//     //
//     0x08, 0x00, 0x00, 0x01, 0x0f, 0x00, 0x35, 0x2a,
//     // 0x0010 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x10, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0011 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x11, 0x00, 0x03, 0x28, 0x02, 0x12, 0x00, 0x49, 0x2a,
//     // 0x0012 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x12, 0x00, 0x49, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_blood_pressure_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_blood_pressure_connect_handle, &param[7], 2);
            BLOOD_PRESSURE_SVC_DEBUG("-->BLE_BLOOD_PRESSURE_EVENT_CONNECTED:%x\n",ble_blood_pressure_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_blood_pressure_connect_handle, &param[0], 2);
            BLOOD_PRESSURE_SVC_DEBUG("-->BLE_BLOOD_PRESSURE_EVENT_DISCONNECTED:%x\n",ble_blood_pressure_connect_handle);
            ble_blood_pressure_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_blood_pressure_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    BLOOD_PRESSURE_SVC_DEBUG("ble blood_pressure read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_blood_pressure_var.meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_blood_pressure_var.meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_blood_pressure_var.feature);
            if (buffer) {
                memcpy(buffer, ble_blood_pressure_var.feature, read_len);
            }
        } break;

        default:
            break;
    }

    BLOOD_PRESSURE_SVC_DEBUG("read_len:%d\n", read_len);
    BLOOD_PRESSURE_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_blood_pressure_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    BLOOD_PRESSURE_SVC_DEBUG("ble blood_pressure write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_blood_pressure_var.meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_blood_pressure_service_init(void)
{
    BLOOD_PRESSURE_SVC_DEBUG("ble_blood_pressure_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BLOOD_PRESSURE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BLOOD_PRESSURE_END_HANDLE;

    // register service with ATT Server
	ble_blood_pressure_service.start_handle   = start_handle;
	ble_blood_pressure_service.end_handle     = end_handle;
	ble_blood_pressure_service.read_callback  = &ble_blood_pressure_service_read_callback;
	ble_blood_pressure_service.write_callback = &ble_blood_pressure_service_write_callback;
	ble_blood_pressure_service.event_handler  = &ble_blood_pressure_service_event_packet_handle;
	att_server_register_service_handler(&ble_blood_pressure_service);

    ble_blood_pressure_connect_handle = 0;
}

void ble_blood_pressure_service_proc(void)
{
    if (ble_blood_pressure_connect_handle == 0) {
        return;
    }
}

void ble_blood_pressure_service_meas_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_blood_pressure_var.meas_client_cfg) {
        ble_indicate_for_handle(ble_blood_pressure_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BLOOD_PRESSURE_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        BLOOD_PRESSURE_SVC_DEBUG("blood pressure measurement not subscribed!\n");
    }
}

