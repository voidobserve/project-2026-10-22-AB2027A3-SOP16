#include "include.h"
#include "ble_service.h"
#include "ble_immediate_alert_service.h"

#define IAS_SVC_DEBUG_EN 1

#if IAS_SVC_DEBUG_EN
#define IAS_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define IAS_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define IAS_SVC_DEBUG(...)
#define IAS_SVC_DEBUG_R(...)
#endif

static ble_ias_var_t            ble_ias_var;
static att_service_handler_t    ble_ias_service;
static uint16_t                 ble_ias_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x007f PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT
//     0x0a, 0x00, 0x02, 0x00, 0x7f, 0x00, 0x00, 0x28, 0x02, 0x18,
//     // 0x0080 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0x80, 0x00, 0x03, 0x28, 0x04, 0x81, 0x00, 0x06, 0x2a,
//     // 0x0081 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x04, 0x01, 0x81, 0x00, 0x06, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_ias_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_ias_connect_handle, &param[7], 2);
            IAS_SVC_DEBUG("-->BLE_IAS_EVENT_CONNECTED:%x\n",ble_ias_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_ias_connect_handle, &param[0], 2);
            IAS_SVC_DEBUG("-->BLE_IAS_EVENT_DISCONNECTED:%x\n",ble_ias_connect_handle);
            ble_ias_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_ias_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    IAS_SVC_DEBUG("ble ias read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

    IAS_SVC_DEBUG("read_len:%d\n", read_len);
    IAS_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_ias_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    IAS_SVC_DEBUG("ble ias write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_ias_service_init(void)
{
    IAS_SVC_DEBUG("ble_ias_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT_END_HANDLE;

    // register service with ATT Server
	ble_ias_service.start_handle   = start_handle;
	ble_ias_service.end_handle     = end_handle;
	ble_ias_service.read_callback  = &ble_ias_service_read_callback;
	ble_ias_service.write_callback = &ble_ias_service_write_callback;
	ble_ias_service.event_handler  = &ble_ias_service_event_packet_handle;
	att_server_register_service_handler(&ble_ias_service);

    ble_ias_connect_handle = 0;
}

void ble_ias_service_proc(void)
{
    if (ble_ias_connect_handle == 0) {
        return;
    }
}
