#include "include.h"
#include "ble_service.h"
#include "ble_heart_rate_service.h"

#define HEART_RATE_SVC_DEBUG_EN 1

#if HEART_RATE_SVC_DEBUG_EN
#define HEART_RATE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define HEART_RATE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define HEART_RATE_SVC_DEBUG(...)
#define HEART_RATE_SVC_DEBUG_R(...)
#endif

static ble_heart_rate_var_t     ble_heart_rate_var;
static att_service_handler_t    ble_heart_rate_service;
static uint16_t                 ble_heart_rate_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0073 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_HEART_RATE
//     0x0a, 0x00, 0x02, 0x00, 0x73, 0x00, 0x00, 0x28, 0x0d, 0x18,
//     // 0x0074 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT - DYNAMIC | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x74, 0x00, 0x03, 0x28, 0x10, 0x75, 0x00, 0x37, 0x2a,
//     // 0x0075 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT - DYNAMIC | NOTIFY
//     //
//     0x08, 0x00, 0x00, 0x01, 0x75, 0x00, 0x37, 0x2a,
//     // 0x0076 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x76, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0077 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_SENSOR_LOCATION - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x77, 0x00, 0x03, 0x28, 0x02, 0x78, 0x00, 0x38, 0x2a,
//     // 0x0078 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BODY_SENSOR_LOCATION - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x78, 0x00, 0x38, 0x2a,
//     // 0x0079 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_CONTROL_POINT - DYNAMIC | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x79, 0x00, 0x03, 0x28, 0x08, 0x7a, 0x00, 0x39, 0x2a,
//     // 0x007a VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_CONTROL_POINT - DYNAMIC | WRITE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x7a, 0x00, 0x39, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_heart_rate_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_heart_rate_connect_handle, &param[7], 2);
            HEART_RATE_SVC_DEBUG("-->BLE_HEART_RATE_EVENT_CONNECTED:%x\n",ble_heart_rate_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_heart_rate_connect_handle, &param[0], 2);
            HEART_RATE_SVC_DEBUG("-->BLE_HEART_RATE_EVENT_DISCONNECTED:%x\n",ble_heart_rate_connect_handle);
            ble_heart_rate_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_heart_rate_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    HEART_RATE_SVC_DEBUG("ble heart_rate read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT_01_VALUE_HANDLE:{
            read_len = sizeof(ble_heart_rate_var.meas_val);
            if (buffer) {
                memcpy(buffer, ble_heart_rate_var.meas_val, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_heart_rate_var.meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_heart_rate_var.meas_client_cfg);
            }
        } break;

        default:
            break;
    }

    HEART_RATE_SVC_DEBUG("read_len:%d\n", read_len);
    HEART_RATE_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_heart_rate_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    HEART_RATE_SVC_DEBUG("ble heart_rate write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_heart_rate_var.meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_heart_rate_service_init(void)
{
    HEART_RATE_SVC_DEBUG("ble_heart_rate_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HEART_RATE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HEART_RATE_END_HANDLE;

    // register service with ATT Server
	ble_heart_rate_service.start_handle   = start_handle;
	ble_heart_rate_service.end_handle     = end_handle;
	ble_heart_rate_service.read_callback  = &ble_heart_rate_service_read_callback;
	ble_heart_rate_service.write_callback = &ble_heart_rate_service_write_callback;
	ble_heart_rate_service.event_handler  = &ble_heart_rate_service_event_packet_handle;
	att_server_register_service_handler(&ble_heart_rate_service);

    ble_heart_rate_connect_handle = 0;
}

void ble_heart_rate_service_proc(void)
{
    if (ble_heart_rate_connect_handle == 0) {
        return;
    }
}

void ble_heart_rate_service_measurement_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_heart_rate_var.meas_client_cfg) {
        ble_notify_for_handle(ble_heart_rate_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        HEART_RATE_SVC_DEBUG("heart rate measurement notify not subscribed!\n");
    }
}
