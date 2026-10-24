#include "include.h"
#include "ble_service.h"
#include "ble_tx_power_service.h"

#define TX_POWER_SVC_DEBUG_EN 1

#if TX_POWER_SVC_DEBUG_EN
#define TX_POWER_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define TX_POWER_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define TX_POWER_SVC_DEBUG(...)
#define TX_POWER_SVC_DEBUG_R(...)
#endif

static ble_tx_power_var_t       ble_tx_power_var;
static att_service_handler_t    ble_tx_power_service;
static uint16_t                 ble_tx_power_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00be PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_TX_POWER
//     0x0a, 0x00, 0x02, 0x00, 0xbe, 0x00, 0x00, 0x28, 0x04, 0x18,
//     // 0x00bf CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TX_POWER_LEVEL - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xbf, 0x00, 0x03, 0x28, 0x02, 0xc0, 0x00, 0x07, 0x2a,
//     // 0x00c0 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TX_POWER_LEVEL - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xc0, 0x00, 0x07, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_tx_power_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_tx_power_connect_handle, &param[7], 2);
            TX_POWER_SVC_DEBUG("-->BLE_TX_POWER_EVENT_CONNECTED:%x\n",ble_tx_power_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_tx_power_connect_handle, &param[0], 2);
            TX_POWER_SVC_DEBUG("-->BLE_TX_POWER_EVENT_DISCONNECTED:%x\n",ble_tx_power_connect_handle);
            ble_tx_power_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_tx_power_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    TX_POWER_SVC_DEBUG("ble tx_power read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TX_POWER_LEVEL_01_VALUE_HANDLE:{
            read_len = 1;
            if (buffer) {
                buffer[0] = ble_tx_power_var.level;
            }
        } break;

        default:
            break;
    }

    TX_POWER_SVC_DEBUG("read_len:%d\n", read_len);
    TX_POWER_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_tx_power_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    TX_POWER_SVC_DEBUG("ble tx_power write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_tx_power_service_init(void)
{
    TX_POWER_SVC_DEBUG("ble_tx_power_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_TX_POWER_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_TX_POWER_END_HANDLE;

    // register service with ATT Server
	ble_tx_power_service.start_handle   = start_handle;
	ble_tx_power_service.end_handle     = end_handle;
	ble_tx_power_service.read_callback  = &ble_tx_power_service_read_callback;
	ble_tx_power_service.write_callback = &ble_tx_power_service_write_callback;
	ble_tx_power_service.event_handler  = &ble_tx_power_service_event_packet_handle;
	att_server_register_service_handler(&ble_tx_power_service);

    ble_tx_power_connect_handle = 0;
}

void ble_tx_power_service_proc(void)
{
    if (ble_tx_power_connect_handle == 0) {
        return;
    }
}



