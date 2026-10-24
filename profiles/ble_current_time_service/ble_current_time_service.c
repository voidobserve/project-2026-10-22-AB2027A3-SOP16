#include "include.h"
#include "ble_service.h"
#include "ble_current_time_service.h"

#define CURRENT_TIME_SVC_DEBUG_EN 1

#if CURRENT_TIME_SVC_DEBUG_EN
#define CURRENT_TIME_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define CURRENT_TIME_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define CURRENT_TIME_SVC_DEBUG(...)
#define CURRENT_TIME_SVC_DEBUG_R(...)
#endif

static ble_current_time_var_t   ble_cur_time_var;
static att_service_handler_t    ble_current_time_service;
static uint16_t                 ble_current_time_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//    // 0x0032 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_CURRENT_TIME
//    0x0a, 0x00, 0x02, 0x00, 0x32, 0x00, 0x00, 0x28, 0x05, 0x18,
//    // 0x0033 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME - DYNAMIC | READ | NOTIFY
//    0x0d, 0x00, 0x02, 0x00, 0x33, 0x00, 0x03, 0x28, 0x12, 0x34, 0x00, 0x2b, 0x2a,
//    // 0x0034 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME - DYNAMIC | READ | NOTIFY
//    // READ_ANYBODY
//    0x08, 0x00, 0x02, 0x01, 0x34, 0x00, 0x2b, 0x2a,
//    // 0x0035 CLIENT_CHARACTERISTIC_CONFIGURATION
//    // READ_ANYBODY, WRITE_ANYBODY
//    0x0a, 0x00, 0x0e, 0x01, 0x35, 0x00, 0x02, 0x29, 0x00, 0x00,

//    // END
//    0x00, 0x00,
// };

static void ble_current_time_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_current_time_connect_handle, &param[7], 2);
            CURRENT_TIME_SVC_DEBUG("-->BLE_CURRENT_TIME_EVENT_CONNECTED:%x\n",ble_current_time_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_current_time_connect_handle, &param[0], 2);
            CURRENT_TIME_SVC_DEBUG("-->BLE_CURRENT_TIME_EVENT_DISCONNECTED:%x\n",ble_current_time_connect_handle);
            ble_current_time_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_current_time_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    CURRENT_TIME_SVC_DEBUG("ble current_time read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_VALUE_HANDLE:{
            read_len = sizeof(ble_cur_time_var.time_data);
            if (buffer) {
                memcpy(buffer, ble_cur_time_var.time_data, sizeof(ble_cur_time_var.time_data));
            }

        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_cur_time_var.client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_cur_time_var.client_cfg);
            }
        } break;

        default:
            break;
    }

    CURRENT_TIME_SVC_DEBUG("read_len:%d\n", read_len);
    CURRENT_TIME_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_current_time_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    CURRENT_TIME_SVC_DEBUG("ble current_time write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_cur_time_var.client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_current_time_service_init(void)
{
    CURRENT_TIME_SVC_DEBUG("ble_current_time_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CURRENT_TIME_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CURRENT_TIME_END_HANDLE;

    // register service with ATT Server
	ble_current_time_service.start_handle   = start_handle;
	ble_current_time_service.end_handle     = end_handle;
	ble_current_time_service.read_callback  = &ble_current_time_service_read_callback;
	ble_current_time_service.write_callback = &ble_current_time_service_write_callback;
	ble_current_time_service.event_handler  = &ble_current_time_service_event_packet_handle;
	att_server_register_service_handler(&ble_current_time_service);

    ble_current_time_connect_handle = 0;
}

void ble_current_time_service_proc(void)
{
    if (ble_current_time_connect_handle == 0) {
        return;
    }
}

void ble_current_time_service_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_cur_time_var.client_cfg) {
        ble_notify_for_handle(ble_current_time_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_VALUE_HANDLE, buf, buf_size);
    } else {
        CURRENT_TIME_SVC_DEBUG("current time notify not subscribed!\n");
    }
}




