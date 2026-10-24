#include "include.h"
#include "ble_service.h"
#include "ble_scan_param_service.h"

#define SCAN_PARAM_SVC_DEBUG_EN 1

#if SCAN_PARAM_SVC_DEBUG_EN
#define SCAN_PARAM_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define SCAN_PARAM_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define SCAN_PARAM_SVC_DEBUG(...)
#define SCAN_PARAM_SVC_DEBUG_R(...)
#endif

static ble_scan_param_var_t     ble_scan_param_var;
static att_service_handler_t    ble_scan_param_service;
static uint16_t                 ble_scan_param_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00b8 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_SCAN_PARAMETERS
//     0x0a, 0x00, 0x02, 0x00, 0xb8, 0x00, 0x00, 0x28, 0x13, 0x18,
//     // 0x00b9 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SCAN_INTERVAL_WINDOW - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0xb9, 0x00, 0x03, 0x28, 0x04, 0xba, 0x00, 0x4f, 0x2a,
//     // 0x00ba VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SCAN_INTERVAL_WINDOW - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x04, 0x01, 0xba, 0x00, 0x4f, 0x2a,
//     // 0x00bb CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SCAN_REFRESH - DYNAMIC | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0xbb, 0x00, 0x03, 0x28, 0x10, 0xbc, 0x00, 0x31, 0x2a,
//     // 0x00bc VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SCAN_REFRESH - DYNAMIC | NOTIFY
//     //
//     0x08, 0x00, 0x00, 0x01, 0xbc, 0x00, 0x31, 0x2a,
//     // 0x00bd CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xbd, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_scan_param_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_scan_param_connect_handle, &param[7], 2);
            SCAN_PARAM_SVC_DEBUG("-->BLE_SCAN_PARAM_EVENT_CONNECTED:%x\n",ble_scan_param_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_scan_param_connect_handle, &param[0], 2);
            SCAN_PARAM_SVC_DEBUG("-->BLE_SCAN_PARAM_EVENT_DISCONNECTED:%x\n",ble_scan_param_connect_handle);
            ble_scan_param_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_scan_param_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    SCAN_PARAM_SVC_DEBUG("ble scan_param read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SCAN_INTERVAL_WINDOW_01_VALUE_HANDLE:{
            read_len = sizeof(ble_scan_param_var.scan_interval);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_scan_param_var.scan_interval);
            }
        } break;

        default:
            break;
    }

    SCAN_PARAM_SVC_DEBUG("read_len:%d\n", read_len);
    SCAN_PARAM_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_scan_param_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    SCAN_PARAM_SVC_DEBUG("ble scan_param write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SCAN_INTERVAL_WINDOW_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_scan_param_service_init(void)
{
    SCAN_PARAM_SVC_DEBUG("ble_scan_param_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_SCAN_PARAMETERS_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_SCAN_PARAMETERS_END_HANDLE;

    // register service with ATT Server
	ble_scan_param_service.start_handle   = start_handle;
	ble_scan_param_service.end_handle     = end_handle;
	ble_scan_param_service.read_callback  = &ble_scan_param_service_read_callback;
	ble_scan_param_service.write_callback = &ble_scan_param_service_write_callback;
	ble_scan_param_service.event_handler  = &ble_scan_param_service_event_packet_handle;
	att_server_register_service_handler(&ble_scan_param_service);

    ble_scan_param_connect_handle = 0;
}

void ble_scan_param_service_proc(void)
{
    if (ble_scan_param_connect_handle == 0) {
        return;
    }
}


