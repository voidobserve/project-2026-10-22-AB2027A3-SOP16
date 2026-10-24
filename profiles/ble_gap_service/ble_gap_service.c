#include "include.h"
#include "ble_service.h"
#include "ble_gap_service.h"

#define GAP_SVC_DEBUG_EN 1

#if GAP_SVC_DEBUG_EN
#define GAP_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define GAP_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define GAP_SVC_DEBUG(...)
#define GAP_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_gap_service;
static uint16_t                 ble_gap_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00d8 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_GENERIC_ACCESS
//     0x0a, 0x00, 0x02, 0x00, 0xd8, 0x00, 0x00, 0x28, 0x00, 0x18,
//     // 0x00d9 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ENCRYPTED_DATA - READ | INDICATE | DYNAMIC
//     0x0d, 0x00, 0x02, 0x00, 0xd9, 0x00, 0x03, 0x28, 0x22, 0xda, 0x00, 0x88, 0x2b,
//     // 0x00da VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ENCRYPTED_DATA - READ | INDICATE | DYNAMIC
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xda, 0x00, 0x88, 0x2b,
//     // 0x00db CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xdb, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static uint8_t encrypted_material[] = {
                                        /*session key*/0x19, 0x6a,0x0a, 0xd1, 0x2a, 0x61, 0x20, 0x1e, 0x13, 0x6e, 0x2e, 0xd1, 0x12, 0xda, 0xa9, 0x57,
                                        /*sinit_vector*/0x9e, 0x7a, 0x00, 0xef, 0xb1, 0x7a, 0xe7, 0x46
                                      };

static void ble_gap_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_gap_connect_handle, &param[7], 2);
            GAP_SVC_DEBUG("-->BLE_GAP_EVENT_CONNECTED:%x\n",ble_gap_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_gap_connect_handle, &param[0], 2);
            GAP_SVC_DEBUG("-->BLE_GAP_EVENT_DISCONNECTED:%x\n",ble_gap_connect_handle);
            ble_gap_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_gap_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    GAP_SVC_DEBUG("ble gap read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ENCRYPTED_DATA_01_VALUE_HANDLE:{
            read_len = sizeof(encrypted_material);

            if(buffer){
                read_len = (buffer_size < (read_len - offset))? buffer_size: (read_len - offset);
                memcpy(buffer, encrypted_material + offset, read_len);
            }
        } break;

        default:
            break;
    }

    GAP_SVC_DEBUG("read_len:%d\n", read_len);
    GAP_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_gap_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    GAP_SVC_DEBUG("ble gap write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_gap_service_init(void)
{
    GAP_SVC_DEBUG("ble_gap_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_GENERIC_ACCESS_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_GENERIC_ACCESS_END_HANDLE;

    // register service with ATT Server
	ble_gap_service.start_handle   = start_handle;
	ble_gap_service.end_handle     = end_handle;
	ble_gap_service.read_callback  = &ble_gap_service_read_callback;
	ble_gap_service.write_callback = &ble_gap_service_write_callback;
	ble_gap_service.event_handler  = &ble_gap_service_event_packet_handle;
	att_server_register_service_handler(&ble_gap_service);

    ble_gap_connect_handle = 0;
}

void ble_gap_service_proc(void)
{
    if (ble_gap_connect_handle == 0) {
        return;
    }
}


