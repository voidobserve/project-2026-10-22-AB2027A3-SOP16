#include "include.h"
#include "ble_service.h"
#include "ble_object_transfer_service.h"

#define OBJECT_TRANSFER_SVC_DEBUG_EN 1

#if OBJECT_TRANSFER_SVC_DEBUG_EN
#define OBJECT_TRANSFER_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define OBJECT_TRANSFER_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define OBJECT_TRANSFER_SVC_DEBUG(...)
#define OBJECT_TRANSFER_SVC_DEBUG_R(...)
#endif

static ble_object_transfer_var_t ble_object_transfer_var;
static att_service_handler_t     ble_object_transfer_service;
static uint16_t                  ble_object_transfer_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x008b PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_OBJECT_TRANSFER
//     0x0a, 0x00, 0x02, 0x00, 0x8b, 0x00, 0x00, 0x28, 0x25, 0x18,
//     // 0x008c CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OTS_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x8c, 0x00, 0x03, 0x28, 0x02, 0x8d, 0x00, 0xbd, 0x2a,
//     // 0x008d VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OTS_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x8d, 0x00, 0xbd, 0x2a,
//     // 0x008e CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x8e, 0x00, 0x03, 0x28, 0x28, 0x8f, 0x00, 0xc5, 0x2a,
//     // 0x008f VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x8f, 0x00, 0xc5, 0x2a,
//     // 0x0090 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x90, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0091 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED - DYNAMIC | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x91, 0x00, 0x03, 0x28, 0x20, 0x92, 0x00, 0xc8, 0x2a,
//     // 0x0092 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED - DYNAMIC | INDICATE
//     //
//     0x08, 0x00, 0x00, 0x01, 0x92, 0x00, 0xc8, 0x2a,
//     // 0x0093 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x93, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0094 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_FIRST_CREATED - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x94, 0x00, 0x03, 0x28, 0x0a, 0x95, 0x00, 0xc1, 0x2a,
//     // 0x0095 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_FIRST_CREATED - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x95, 0x00, 0xc1, 0x2a,
//     // 0x0096 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ID - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x96, 0x00, 0x03, 0x28, 0x02, 0x97, 0x00, 0xc3, 0x2a,
//     // 0x0097 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ID - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x97, 0x00, 0xc3, 0x2a,
//     // 0x0098 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LAST_MODIFIED - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x98, 0x00, 0x03, 0x28, 0x0a, 0x99, 0x00, 0xc2, 0x2a,
//     // 0x0099 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LAST_MODIFIED - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x99, 0x00, 0xc2, 0x2a,
//     // 0x009a CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x9a, 0x00, 0x03, 0x28, 0x28, 0x9b, 0x00, 0xc6, 0x2a,
//     // 0x009b VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x9b, 0x00, 0xc6, 0x2a,
//     // 0x009c CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x9c, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x009d CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_FILTER - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x9d, 0x00, 0x03, 0x28, 0x0a, 0x9e, 0x00, 0xc7, 0x2a,
//     // 0x009e VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_FILTER - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x9e, 0x00, 0xc7, 0x2a,
//     // 0x009f CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_NAME - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x9f, 0x00, 0x03, 0x28, 0x0a, 0xa0, 0x00, 0xbe, 0x2a,
//     // 0x00a0 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_NAME - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0xa0, 0x00, 0xbe, 0x2a,
//     // 0x00a1 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_PROPERTIES - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0xa1, 0x00, 0x03, 0x28, 0x0a, 0xa2, 0x00, 0xc4, 0x2a,
//     // 0x00a2 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_PROPERTIES - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0xa2, 0x00, 0xc4, 0x2a,
//     // 0x00a3 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_SIZE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xa3, 0x00, 0x03, 0x28, 0x02, 0xa4, 0x00, 0xc0, 0x2a,
//     // 0x00a4 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_SIZE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xa4, 0x00, 0xc0, 0x2a,
//     // 0x00a5 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_TYPE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xa5, 0x00, 0x03, 0x28, 0x02, 0xa6, 0x00, 0xbf, 0x2a,
//     // 0x00a6 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_TYPE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xa6, 0x00, 0xbf, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_object_transfer_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_object_transfer_connect_handle, &param[7], 2);
            OBJECT_TRANSFER_SVC_DEBUG("-->BLE_OBJECT_TRANSFER_EVENT_CONNECTED:%x\n",ble_object_transfer_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_object_transfer_connect_handle, &param[0], 2);
            OBJECT_TRANSFER_SVC_DEBUG("-->BLE_OBJECT_TRANSFER_EVENT_DISCONNECTED:%x\n",ble_object_transfer_connect_handle);
            ble_object_transfer_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_object_transfer_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    OBJECT_TRANSFER_SVC_DEBUG("===>ble object_transfer read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OTS_FEATURE_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_FIRST_CREATED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ID_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LAST_MODIFIED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_FILTER_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_NAME_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_PROPERTIES_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_SIZE_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_TYPE_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = 2;
            if (buffer) {
                PUT_LE16(&buffer[0], ble_object_transfer_var.action_ctr_point_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = 2;
            if (buffer) {
                PUT_LE16(&buffer[0], ble_object_transfer_var.changed_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = 2;
            if (buffer) {
                PUT_LE16(&buffer[0], ble_object_transfer_var.list_ctr_point_client_cfg);
            }
        } break;

        default:
            break;
    }

    OBJECT_TRANSFER_SVC_DEBUG("read_len:%d\n", read_len);
    OBJECT_TRANSFER_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_object_transfer_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    OBJECT_TRANSFER_SVC_DEBUG("===>ble object_transfer write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_FIRST_CREATED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LAST_MODIFIED_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_FILTER_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_NAME_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_PROPERTIES_01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_object_transfer_var.action_ctr_point_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_object_transfer_var.changed_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_object_transfer_var.list_ctr_point_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_object_transfer_service_init(void)
{
    OBJECT_TRANSFER_SVC_DEBUG("ble_object_transfer_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_OBJECT_TRANSFER_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_OBJECT_TRANSFER_END_HANDLE;

    // register service with ATT Server
	ble_object_transfer_service.start_handle   = start_handle;
	ble_object_transfer_service.end_handle     = end_handle;
	ble_object_transfer_service.read_callback  = &ble_object_transfer_service_read_callback;
	ble_object_transfer_service.write_callback = &ble_object_transfer_service_write_callback;
	ble_object_transfer_service.event_handler  = &ble_object_transfer_service_event_packet_handle;
	att_server_register_service_handler(&ble_object_transfer_service);

    ble_object_transfer_connect_handle = 0;
}

void ble_object_transfer_service_proc(void)
{
    if (ble_object_transfer_connect_handle == 0) {
        return;
    }
}

void ble_object_transfer_service_action_ctr_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_object_transfer_var.action_ctr_point_client_cfg) {
        ble_indicate_for_handle(ble_object_transfer_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_ACTION_CONTROL_POINT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        OBJECT_TRANSFER_SVC_DEBUG("object action ctr point indicate not subscribed!\n");
    }
}

void ble_object_transfer_service_list_ctr_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_object_transfer_var.list_ctr_point_client_cfg) {
        ble_indicate_for_handle(ble_object_transfer_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_LIST_CONTROL_POINT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        OBJECT_TRANSFER_SVC_DEBUG("object list ctr point indicate not subscribed!\n");
    }
}

void ble_object_transfer_service_change_ctr_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_object_transfer_var.changed_client_cfg) {
        ble_indicate_for_handle(ble_object_transfer_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_OBJECT_CHANGED_01_VALUE_HANDLE, buf, buf_size);
    } else {
        OBJECT_TRANSFER_SVC_DEBUG("object change indicate not subscribed!\n");
    }
}

