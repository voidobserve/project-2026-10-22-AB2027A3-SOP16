#include "include.h"
#include "ble_service.h"
#include "ble_bond_management_service.h"

#define BOND_MANAGEMENT_SVC_DEBUG_EN 1

#if BOND_MANAGEMENT_SVC_DEBUG_EN
#define BOND_MANAGEMENT_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BOND_MANAGEMENT_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BOND_MANAGEMENT_SVC_DEBUG(...)
#define BOND_MANAGEMENT_SVC_DEBUG_R(...)
#endif

static ble_bond_management_var_t ble_bond_management_var;
static att_service_handler_t     ble_bond_management_service;
static uint16_t                  ble_bond_management_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

//static const uint8_t profile_data[] =
//{
//    // 0x0013 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BOND_MANAGEMENT
//    0x0a, 0x00, 0x02, 0x00, 0x13, 0x00, 0x00, 0x28, 0x1e, 0x18,
//    // 0x0014 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_CONTROL_POINT - DYNAMIC | WRITE
//    0x0d, 0x00, 0x02, 0x00, 0x14, 0x00, 0x03, 0x28, 0x08, 0x15, 0x00, 0xa4, 0x2a,
//    // 0x0015 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_CONTROL_POINT - DYNAMIC | WRITE
//    // WRITE_ANYBODY
//    0x08, 0x00, 0x08, 0x01, 0x15, 0x00, 0xa4, 0x2a,
//    // 0x0016 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_FEATURE - DYNAMIC | READ
//    0x0d, 0x00, 0x02, 0x00, 0x16, 0x00, 0x03, 0x28, 0x02, 0x17, 0x00, 0xa5, 0x2a,
//    // 0x0017 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_FEATURE - DYNAMIC | READ
//    // READ_ANYBODY
//    0x08, 0x00, 0x02, 0x01, 0x17, 0x00, 0xa5, 0x2a,
//
//    // END
//    0x00, 0x00,
//};

static void ble_bond_management_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_bond_management_connect_handle, &param[7], 2);
            BOND_MANAGEMENT_SVC_DEBUG("-->BLE_BOND_MANAGEMENT_EVENT_CONNECTED:%x\n",ble_bond_management_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_bond_management_connect_handle, &param[0], 2);
            BOND_MANAGEMENT_SVC_DEBUG("-->BLE_BOND_MANAGEMENT_EVENT_DISCONNECTED:%x\n",ble_bond_management_connect_handle);
            ble_bond_management_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_bond_management_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    BOND_MANAGEMENT_SVC_DEBUG("ble bond_management read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_bond_management_var.feature);
            if (buffer) {
                memcpy(buffer, ble_bond_management_var.feature, read_len);
            }
        } break;

        default:
            break;
    }

    BOND_MANAGEMENT_SVC_DEBUG("read_len:%d\n", read_len);
    BOND_MANAGEMENT_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_bond_management_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    BOND_MANAGEMENT_SVC_DEBUG("ble bond_management write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOND_MANAGEMENT_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_bond_management_service_init(void)
{
    BOND_MANAGEMENT_SVC_DEBUG("ble_bond_management_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BOND_MANAGEMENT_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BOND_MANAGEMENT_END_HANDLE;

    // register service with ATT Server
	ble_bond_management_service.start_handle   = start_handle;
	ble_bond_management_service.end_handle     = end_handle;
	ble_bond_management_service.read_callback  = &ble_bond_management_service_read_callback;
	ble_bond_management_service.write_callback = &ble_bond_management_service_write_callback;
	ble_bond_management_service.event_handler  = &ble_bond_management_service_event_packet_handle;
	att_server_register_service_handler(&ble_bond_management_service);

    ble_bond_management_connect_handle = 0;
}

void ble_bond_management_service_proc(void)
{
    if (ble_bond_management_connect_handle == 0) {
        return;
    }
}


