#include "include.h"
#include "ble_service.h"
#include "ble_glucose_service.h"

#define GLUCOSE_SVC_DEBUG_EN 1

#if GLUCOSE_SVC_DEBUG_EN
#define GLUCOSE_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define GLUCOSE_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define GLUCOSE_SVC_DEBUG(...)
#define GLUCOSE_SVC_DEBUG_R(...)
#endif

static ble_glucose_var_t        ble_glucose_var;
static att_service_handler_t    ble_glucose_service;
static uint16_t                 ble_glucose_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x004d PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_GLUCOSE
//     0x0a, 0x00, 0x02, 0x00, 0x4d, 0x00, 0x00, 0x28, 0x08, 0x18,
//     // 0x004e CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x4e, 0x00, 0x03, 0x28, 0x02, 0x4f, 0x00, 0x51, 0x2a,
//     // 0x004f VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x4f, 0x00, 0x51, 0x2a,
//     // 0x0050 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_MEASUREMENT - DYNAMIC | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x50, 0x00, 0x03, 0x28, 0x10, 0x51, 0x00, 0x18, 0x2a,
//     // 0x0051 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_MEASUREMENT - DYNAMIC | NOTIFY
//     //
//     0x08, 0x00, 0x00, 0x01, 0x51, 0x00, 0x18, 0x2a,
//     // 0x0052 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x52, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0053 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x53, 0x00, 0x03, 0x28, 0x28, 0x54, 0x00, 0x52, 0x2a,
//     // 0x0054 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x54, 0x00, 0x52, 0x2a,
//     // 0x0055 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x55, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_glucose_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_glucose_connect_handle, &param[7], 2);
            GLUCOSE_SVC_DEBUG("-->BLE_GLUCOSE_EVENT_CONNECTED:%x\n",ble_glucose_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_glucose_connect_handle, &param[0], 2);
            GLUCOSE_SVC_DEBUG("-->BLE_GLUCOSE_EVENT_DISCONNECTED:%x\n",ble_glucose_connect_handle);
            ble_glucose_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_glucose_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    GLUCOSE_SVC_DEBUG("ble glucose read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_glucose_var.feature);
            if (buffer) {
                memcpy(buffer, ble_glucose_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_glucose_var.meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_glucose_var.meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_glucose_var.ctr_point_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_glucose_var.ctr_point_client_cfg);
            }
        } break;

        default:
            break;
    }

    GLUCOSE_SVC_DEBUG("read_len:%d\n", read_len);
    GLUCOSE_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_glucose_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    GLUCOSE_SVC_DEBUG("ble glucose write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_glucose_var.meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_glucose_var.ctr_point_client_cfg = GET_LE16(&buffer[0]);

        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_glucose_service_init(void)
{
    GLUCOSE_SVC_DEBUG("ble_glucose_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_GLUCOSE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_GLUCOSE_END_HANDLE;

    // register service with ATT Server
	ble_glucose_service.start_handle   = start_handle;
	ble_glucose_service.end_handle     = end_handle;
	ble_glucose_service.read_callback  = &ble_glucose_service_read_callback;
	ble_glucose_service.write_callback = &ble_glucose_service_write_callback;
	ble_glucose_service.event_handler  = &ble_glucose_service_event_packet_handle;
	att_server_register_service_handler(&ble_glucose_service);

    ble_glucose_connect_handle = 0;
}

void ble_glucose_service_proc(void)
{
    if (ble_glucose_connect_handle == 0) {
        return;
    }
}

void ble_glucose_service_measurement_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_glucose_var.meas_client_cfg) {
        ble_notify_for_handle(ble_glucose_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_GLUCOSE_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        GLUCOSE_SVC_DEBUG("glucose measurement notify not subscribed!\n");
    }
}

void ble_glucose_service_record_access_control_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_glucose_var.ctr_point_client_cfg) {
        ble_indicate_for_handle(ble_glucose_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_RECORD_ACCESS_CONTROL_POINT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        GLUCOSE_SVC_DEBUG("glucose record access control point indicate not subscribed!\n");
    }
}
