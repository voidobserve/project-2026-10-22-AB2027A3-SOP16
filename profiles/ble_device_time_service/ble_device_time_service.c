#include "include.h"
#include "ble_service.h"
#include "ble_device_time_service.h"

#define DEVICE_TIME_SVC_DEBUG_EN 1

#if DEVICE_TIME_SVC_DEBUG_EN
#define DEVICE_TIME_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define DEVICE_TIME_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DEVICE_TIME_SVC_DEBUG(...)
#define DEVICE_TIME_SVC_DEBUG_R(...)
#endif

static ble_device_time_var_t ble_dev_time_var;
static att_service_handler_t    ble_device_time_service;
static uint16_t                 ble_device_time_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0042 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_DEVICE_TIME
//     0x0a, 0x00, 0x02, 0x00, 0x42, 0x00, 0x00, 0x28, 0x47, 0x18,
//     // 0x0043 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_FEATURE_ - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x43, 0x00, 0x03, 0x28, 0x02, 0x44, 0x00, 0x8e, 0x2b,
//     // 0x0044 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_FEATURE_ - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x44, 0x00, 0x8e, 0x2b,
//     // 0x0045 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_PARAMETERS_ - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x45, 0x00, 0x03, 0x28, 0x02, 0x46, 0x00, 0x8f, 0x2b,
//     // 0x0046 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_PARAMETERS_ - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x46, 0x00, 0x8f, 0x2b,
//     // 0x0047 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT_ - DYNAMIC | WRITE | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x47, 0x00, 0x03, 0x28, 0x28, 0x48, 0x00, 0x91, 0x2b,
//     // 0x0048 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT_ - DYNAMIC | WRITE | INDICATE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x48, 0x00, 0x91, 0x2b,
//     // 0x0049 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x49, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x004a CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME - DYNAMIC | READ | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x4a, 0x00, 0x03, 0x28, 0x22, 0x4b, 0x00, 0x90, 0x2b,
//     // 0x004b VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME - DYNAMIC | READ | INDICATE
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x4b, 0x00, 0x90, 0x2b,
//     // 0x004c CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x4c, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_device_time_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_device_time_connect_handle, &param[7], 2);
            DEVICE_TIME_SVC_DEBUG("-->BLE_DEVICE_TIME_EVENT_CONNECTED:%x\n",ble_device_time_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_device_time_connect_handle, &param[0], 2);
            DEVICE_TIME_SVC_DEBUG("-->BLE_DEVICE_TIME_EVENT_DISCONNECTED:%x\n",ble_device_time_connect_handle);
            ble_device_time_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_device_time_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    DEVICE_TIME_SVC_DEBUG("ble device_time read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_FEATURE__01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_time_var.feature);
            if (buffer) {
                memcpy(buffer, ble_dev_time_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_PARAMETERS__01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_time_var.dt_parameters);
            if (buffer) {
                memcpy(buffer, ble_dev_time_var.dt_parameters, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT__01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_dev_time_var.ctr_point_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_dev_time_var.ctr_point_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_dev_time_var.time_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_dev_time_var.time_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_01_VALUE_HANDLE:{
            read_len = sizeof(ble_dev_time_var.dev_time);
            if (buffer) {
                memcpy(buffer, ble_dev_time_var.dev_time, read_len);
            }
        } break;

        default:
            break;
    }

    DEVICE_TIME_SVC_DEBUG("read_len:%d\n", read_len);
    DEVICE_TIME_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_device_time_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    DEVICE_TIME_SVC_DEBUG("ble device_time write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    DEVICE_TIME_SVC_DEBUG_R(buffer, buffer_size);
    switch (attribute_handle) {

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT__01_CLIENT_CONFIGURATION_HANDLE:{
            ble_dev_time_var.ctr_point_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT__01_VALUE_HANDLE:{
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_dev_time_var.time_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_device_time_service_init(void)
{
    DEVICE_TIME_SVC_DEBUG("ble_device_time_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_DEVICE_TIME_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_DEVICE_TIME_END_HANDLE;

    // register service with ATT Server
	ble_device_time_service.start_handle   = start_handle;
	ble_device_time_service.end_handle     = end_handle;
	ble_device_time_service.read_callback  = &ble_device_time_service_read_callback;
	ble_device_time_service.write_callback = &ble_device_time_service_write_callback;
	ble_device_time_service.event_handler  = &ble_device_time_service_event_packet_handle;
	att_server_register_service_handler(&ble_device_time_service);

    ble_device_time_connect_handle = 0;
}

void ble_device_time_service_proc(void)
{
    if (ble_device_time_connect_handle == 0) {
        return;
    }
}

void ble_device_time_service_control_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_dev_time_var.ctr_point_client_cfg) {
        ble_indicate_for_handle(ble_device_time_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_DEVICE_TIME_CONTROL_POINT__01_VALUE_HANDLE, buf, buf_size);
    } else {
        DEVICE_TIME_SVC_DEBUG("cycling speed and cadence control point indicate not subscribed\n");
    }
}
