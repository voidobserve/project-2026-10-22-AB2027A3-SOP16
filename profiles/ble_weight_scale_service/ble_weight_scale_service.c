#include "include.h"
#include "ble_service.h"
#include "ble_weight_scale_service.h"

#define WEIGHT_SCALE_SVC_DEBUG_EN 1

#if WEIGHT_SCALE_SVC_DEBUG_EN
#define WEIGHT_SCALE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define WEIGHT_SCALE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define WEIGHT_SCALE_SVC_DEBUG(...)
#define WEIGHT_SCALE_SVC_DEBUG_R(...)
#endif

static ble_weight_scale_var_t   ble_weight_scale_var;
static att_service_handler_t    ble_weight_scale_service;
static uint16_t                 ble_weight_scale_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00c1 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_WEIGHT_SCALE
//     0x0a, 0x00, 0x02, 0x00, 0xc1, 0x00, 0x00, 0x28, 0x1d, 0x18,
//     // 0x00c2 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xc2, 0x00, 0x03, 0x28, 0x02, 0xc3, 0x00, 0x98, 0x2a,
//     // 0x00c3 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xc3, 0x00, 0x98, 0x2a,
//     // 0x00c4 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_MEASUREMENT - INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0xc4, 0x00, 0x03, 0x28, 0x20, 0xc5, 0x00, 0x9d, 0x2a,
//     // 0x00c5 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_MEASUREMENT - INDICATE -''
//     //
//     0x08, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x9d, 0x2a,
//     // 0x00c6 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xc6, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x00c7 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_SCALE_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xc7, 0x00, 0x03, 0x28, 0x02, 0xc8, 0x00, 0x9e, 0x2a,
//     // 0x00c8 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_SCALE_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xc8, 0x00, 0x9e, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_weight_scale_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_weight_scale_connect_handle, &param[7], 2);
            WEIGHT_SCALE_SVC_DEBUG("-->BLE_WEIGHT_SCALE_EVENT_CONNECTED:%x\n",ble_weight_scale_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_weight_scale_connect_handle, &param[0], 2);
            WEIGHT_SCALE_SVC_DEBUG("-->BLE_WEIGHT_SCALE_EVENT_DISCONNECTED:%x\n",ble_weight_scale_connect_handle);
            ble_weight_scale_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_weight_scale_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    WEIGHT_SCALE_SVC_DEBUG("ble weight_scale read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_SCALE_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_weight_scale_var.feature);
            if (buffer) {
                memcpy(buffer, ble_weight_scale_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = 2;
            if (buffer) {
                PUT_LE16(&buffer[0], ble_weight_scale_var.measurement_client_cfg);
            }
        } break;

        default:
            break;
    }

    WEIGHT_SCALE_SVC_DEBUG("read_len:%d\n", read_len);
    WEIGHT_SCALE_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_weight_scale_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    WEIGHT_SCALE_SVC_DEBUG("ble weight_scale write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_weight_scale_var.measurement_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_weight_scale_service_init(void)
{
    WEIGHT_SCALE_SVC_DEBUG("ble_weight_scale_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_WEIGHT_SCALE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_WEIGHT_SCALE_END_HANDLE;

    // register service with ATT Server
	ble_weight_scale_service.start_handle   = start_handle;
	ble_weight_scale_service.end_handle     = end_handle;
	ble_weight_scale_service.read_callback  = &ble_weight_scale_service_read_callback;
	ble_weight_scale_service.write_callback = &ble_weight_scale_service_write_callback;
	ble_weight_scale_service.event_handler  = &ble_weight_scale_service_event_packet_handle;
	att_server_register_service_handler(&ble_weight_scale_service);

    ble_weight_scale_connect_handle = 0;
}

void ble_weight_scale_service_proc(void)
{
    if (ble_weight_scale_connect_handle == 0) {
        return;
    }
}

void ble_weight_scale_service_measurement_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_weight_scale_var.measurement_client_cfg) {
        ble_indicate_for_handle(ble_weight_scale_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_WEIGHT_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        WEIGHT_SCALE_SVC_DEBUG("weight measurement indicate not subscribed!\n");
    }
}
