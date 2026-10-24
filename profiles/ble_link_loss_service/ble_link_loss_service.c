#include "include.h"
#include "ble_service.h"
#include "ble_link_loss_service.h"

#define LINK_LOSS_SVC_DEBUG_EN 1

#if LINK_LOSS_SVC_DEBUG_EN
#define LINK_LOSS_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define LINK_LOSS_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define LINK_LOSS_SVC_DEBUG(...)
#define LINK_LOSS_SVC_DEBUG_R(...)
#endif

static ble_link_loss_var_t      ble_link_loss_var;
static att_service_handler_t    ble_link_loss_service;
static uint16_t                 ble_link_loss_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0088 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_LINK_LOSS
//     0x0a, 0x00, 0x02, 0x00, 0x88, 0x00, 0x00, 0x28, 0x03, 0x18,
//     // 0x0089 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL - DYNAMIC | READ | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x89, 0x00, 0x03, 0x28, 0x0a, 0x8a, 0x00, 0x06, 0x2a,
//     // 0x008a VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL - DYNAMIC | READ | WRITE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x8a, 0x00, 0x06, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_link_loss_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_link_loss_connect_handle, &param[7], 2);
            LINK_LOSS_SVC_DEBUG("-->BLE_LINK_LOSS_EVENT_CONNECTED:%x\n",ble_link_loss_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_link_loss_connect_handle, &param[0], 2);
            LINK_LOSS_SVC_DEBUG("-->BLE_LINK_LOSS_EVENT_DISCONNECTED:%x\n",ble_link_loss_connect_handle);
            ble_link_loss_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_link_loss_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    LINK_LOSS_SVC_DEBUG("ble link_loss read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_02_VALUE_HANDLE:{
            read_len = 1;
            if (buffer) {
                buffer[0] = ble_link_loss_var.alert_level;
            }
        } break;

        default:
            break;
    }

    LINK_LOSS_SVC_DEBUG("read_len:%d\n", read_len);
    LINK_LOSS_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_link_loss_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    LINK_LOSS_SVC_DEBUG("ble link_loss write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_02_VALUE_HANDLE:{
            ble_link_loss_var.alert_level = buffer[0];
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_link_loss_service_init(void)
{
    LINK_LOSS_SVC_DEBUG("ble_link_loss_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_LINK_LOSS_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_LINK_LOSS_END_HANDLE;

    // register service with ATT Server
	ble_link_loss_service.start_handle   = start_handle;
	ble_link_loss_service.end_handle     = end_handle;
	ble_link_loss_service.read_callback  = &ble_link_loss_service_read_callback;
	ble_link_loss_service.write_callback = &ble_link_loss_service_write_callback;
	ble_link_loss_service.event_handler  = &ble_link_loss_service_event_packet_handle;
	att_server_register_service_handler(&ble_link_loss_service);

    ble_link_loss_connect_handle = 0;
}

void ble_link_loss_service_proc(void)
{
    if (ble_link_loss_connect_handle == 0) {
        return;
    }
}

