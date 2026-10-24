#include "include.h"
#include "ble_service.h"
#include "ble_device_information_service.h"

#define DEVICE_INFORMATION_SVC_DEBUG_EN 1

#if DEVICE_INFORMATION_SVC_DEBUG_EN
#define DEVICE_INFORMATION_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define DEVICE_INFORMATION_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DEVICE_INFORMATION_SVC_DEBUG(...)
#define DEVICE_INFORMATION_SVC_DEBUG_R(...)
#endif

static ble_device_information_var_t ble_dev_info_var;
static att_service_handler_t        ble_device_information_service;
static uint16_t                     ble_device_information_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0036 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_DEVICE_INFORMATION
//     0x0a, 0x00, 0x02, 0x00, 0x36, 0x00, 0x00, 0x28, 0x0a, 0x18,
//     // 0x0037 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_MANUFACTURER_NAME_STRING - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x37, 0x00, 0x03, 0x28, 0x02, 0x38, 0x00, 0x29, 0x2a,
//     // 0x0038 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_MANUFACTURER_NAME_STRING - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x38, 0x00, 0x29, 0x2a,
//     // 0x0039 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_MODEL_NUMBER_STRING - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x39, 0x00, 0x03, 0x28, 0x02, 0x3a, 0x00, 0x24, 0x2a,
//     // 0x003a VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_MODEL_NUMBER_STRING - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x3a, 0x00, 0x24, 0x2a,
//     // 0x003b CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SYSTEM_ID - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x3b, 0x00, 0x03, 0x28, 0x02, 0x3c, 0x00, 0x23, 0x2a,
//     // 0x003c VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SYSTEM_ID - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x3c, 0x00, 0x23, 0x2a,
//     // 0x003d CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PNP_ID - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x3d, 0x00, 0x03, 0x28, 0x02, 0x3e, 0x00, 0x50, 0x2a,
//     // 0x003e VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PNP_ID - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x3e, 0x00, 0x50, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_device_information_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_device_information_connect_handle, &param[7], 2);
            DEVICE_INFORMATION_SVC_DEBUG("-->BLE_DEVICE_INFORMATION_EVENT_CONNECTED:%x\n",ble_device_information_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_device_information_connect_handle, &param[0], 2);
            DEVICE_INFORMATION_SVC_DEBUG("-->BLE_DEVICE_INFORMATION_EVENT_DISCONNECTED:%x\n",ble_device_information_connect_handle);
            ble_device_information_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_device_information_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    DEVICE_INFORMATION_SVC_DEBUG("ble device_information read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_MANUFACTURER_NAME_STRING_01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_info_var.name);
            if (buffer) {
                memcpy(buffer, ble_dev_info_var.name, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_MODEL_NUMBER_STRING_01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_info_var.number);
            if (buffer) {
                memcpy(buffer, ble_dev_info_var.number, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SYSTEM_ID_01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_info_var.system_id);
            if (buffer) {
                memcpy(buffer, ble_dev_info_var.system_id, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PNP_ID_01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_info_var.pnp_id);
            if (buffer) {
                memcpy(buffer, ble_dev_info_var.pnp_id, read_len);
            }
        } break;

        default:
            break;
    }

    DEVICE_INFORMATION_SVC_DEBUG("read_len:%d\n", read_len);
    DEVICE_INFORMATION_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_device_information_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    DEVICE_INFORMATION_SVC_DEBUG("ble device_information write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_device_information_service_init(void)
{
    DEVICE_INFORMATION_SVC_DEBUG("ble_device_information_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_DEVICE_INFORMATION_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_DEVICE_INFORMATION_END_HANDLE;

    // register service with ATT Server
	ble_device_information_service.start_handle   = start_handle;
	ble_device_information_service.end_handle     = end_handle;
	ble_device_information_service.read_callback  = &ble_device_information_service_read_callback;
	ble_device_information_service.write_callback = &ble_device_information_service_write_callback;
	ble_device_information_service.event_handler  = &ble_device_information_service_event_packet_handle;
	att_server_register_service_handler(&ble_device_information_service);

    ble_device_information_connect_handle = 0;
}

void ble_device_information_service_proc(void)
{
    if (ble_device_information_connect_handle == 0) {
        return;
    }
}

