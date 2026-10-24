#include "include.h"
#include "ble_service.h"
#include "ble_reference_time_service.h"

#define REFERENCE_TIME_SVC_DEBUG_EN 1

#if REFERENCE_TIME_SVC_DEBUG_EN
#define REFERENCE_TIME_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define REFERENCE_TIME_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define REFERENCE_TIME_SVC_DEBUG(...)
#define REFERENCE_TIME_SVC_DEBUG_R(...)
#endif

static ble_reference_time_var_t ble_reference_time_var;
static att_service_handler_t    ble_reference_time_service;
static uint16_t                 ble_reference_time_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00b3 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_REFERENCE_TIME_UPDATE
//     0x0a, 0x00, 0x02, 0x00, 0xb3, 0x00, 0x00, 0x28, 0x06, 0x18,
//     // 0x00b4 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0xb4, 0x00, 0x03, 0x28, 0x04, 0xb5, 0x00, 0x16, 0x2a,
//     // 0x00b5 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x04, 0x01, 0xb5, 0x00, 0x16, 0x2a,
//     // 0x00b6 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_STATE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xb6, 0x00, 0x03, 0x28, 0x02, 0xb7, 0x00, 0x17, 0x2a,
//     // 0x00b7 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_STATE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xb7, 0x00, 0x17, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_reference_time_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_reference_time_connect_handle, &param[7], 2);
            REFERENCE_TIME_SVC_DEBUG("-->BLE_REFERENCE_TIME_EVENT_CONNECTED:%x\n",ble_reference_time_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_reference_time_connect_handle, &param[0], 2);
            REFERENCE_TIME_SVC_DEBUG("-->BLE_REFERENCE_TIME_EVENT_DISCONNECTED:%x\n",ble_reference_time_connect_handle);
            ble_reference_time_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_reference_time_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    REFERENCE_TIME_SVC_DEBUG("ble reference_time read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_01_VALUE_HANDLE:{
            read_len = sizeof(ble_reference_time_var.update_val);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_reference_time_var.update_val);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_STATE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_reference_time_var.update_sta);
            if (buffer) {
               PUT_LE16(&buffer[0], ble_reference_time_var.update_sta);
            }
        } break;

        default:
            break;
    }

    REFERENCE_TIME_SVC_DEBUG("read_len:%d\n", read_len);
    REFERENCE_TIME_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_reference_time_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    REFERENCE_TIME_SVC_DEBUG("ble reference_time write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_01_VALUE_HANDLE:{
            ble_reference_time_var.update_val = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_reference_time_service_init(void)
{
    REFERENCE_TIME_SVC_DEBUG("ble_reference_time_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_REFERENCE_TIME_UPDATE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_REFERENCE_TIME_UPDATE_END_HANDLE;

    // register service with ATT Server
	ble_reference_time_service.start_handle   = start_handle;
	ble_reference_time_service.end_handle     = end_handle;
	ble_reference_time_service.read_callback  = &ble_reference_time_service_read_callback;
	ble_reference_time_service.write_callback = &ble_reference_time_service_write_callback;
	ble_reference_time_service.event_handler  = &ble_reference_time_service_event_packet_handle;
	att_server_register_service_handler(&ble_reference_time_service);

    ble_reference_time_connect_handle = 0;
}

void ble_reference_time_service_proc(void)
{
    if (ble_reference_time_connect_handle == 0) {
        return;
    }
}

