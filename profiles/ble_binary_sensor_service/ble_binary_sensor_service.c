#include "include.h"
#include "ble_service.h"
#include "ble_binary_sensor_service.h"

#define BINARY_SENSOR_SVC_DEBUG_EN 1

#if BINARY_SENSOR_SVC_DEBUG_EN
#define BINARY_SENSOR_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BINARY_SENSOR_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BINARY_SENSOR_SVC_DEBUG(...)
#define BINARY_SENSOR_SVC_DEBUG_R(...)
#endif

static ble_binary_sensor_var_t  ble_binary_sensor_var;
static att_service_handler_t    ble_binary_sensor_service;
static uint16_t                 ble_binary_sensor_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0018 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BINARY_SENSOR
//     0x0a, 0x00, 0x02, 0x00, 0x18, 0x00, 0x00, 0x28, 0x3b, 0x18,
//     // 0x0019 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BSS_CONTROL_POINT - DYNAMIC | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0x19, 0x00, 0x03, 0x28, 0x08, 0x1a, 0x00, 0x2b, 0x2b,
//     // 0x001a VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BSS_CONTROL_POINT - DYNAMIC | WRITE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x1a, 0x00, 0x2b, 0x2b,
//     // 0x001b CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE - DYNAMIC | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x1b, 0x00, 0x03, 0x28, 0x20, 0x1c, 0x00, 0x2c, 0x2b,
//     // 0x001c VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE - DYNAMIC | INDICATE
//     //
//     0x08, 0x00, 0x00, 0x01, 0x1c, 0x00, 0x2c, 0x2b,
//     // 0x001d CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x1d, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_binary_sensor_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_binary_sensor_connect_handle, &param[7], 2);
            BINARY_SENSOR_SVC_DEBUG("-->BLE_BINARY_SENSOR_EVENT_CONNECTED:%x\n",ble_binary_sensor_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_binary_sensor_connect_handle, &param[0], 2);
            BINARY_SENSOR_SVC_DEBUG("-->BLE_BINARY_SENSOR_EVENT_DISCONNECTED:%x\n",ble_binary_sensor_connect_handle);
            ble_binary_sensor_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_binary_sensor_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    BINARY_SENSOR_SVC_DEBUG("ble binary_sensor read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_binary_sensor_var.rsp_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_binary_sensor_var.rsp_client_cfg);
            }
        } break;

        default:
            break;
    }

    BINARY_SENSOR_SVC_DEBUG("read_len:%d\n", read_len);
    print_r(buffer, read_len);
	return read_len;
}

static int ble_binary_sensor_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    BINARY_SENSOR_SVC_DEBUG("ble binary_sensor write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BSS_CONTROL_POINT_01_VALUE_HANDLE:{
            ble_binary_sensor_var.ctr_point = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE_01_VALUE_HANDLE:{
            ble_binary_sensor_var.rsp_val = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_binary_sensor_var.rsp_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_binary_sensor_service_init(void)
{
    BINARY_SENSOR_SVC_DEBUG("ble_binary_sensor_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BINARY_SENSOR_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BINARY_SENSOR_END_HANDLE;

    // register service with ATT Server
	ble_binary_sensor_service.start_handle   = start_handle;
	ble_binary_sensor_service.end_handle     = end_handle;
	ble_binary_sensor_service.read_callback  = &ble_binary_sensor_service_read_callback;
	ble_binary_sensor_service.write_callback = &ble_binary_sensor_service_write_callback;
	ble_binary_sensor_service.event_handler  = &ble_binary_sensor_service_event_packet_handle;
	att_server_register_service_handler(&ble_binary_sensor_service);

    ble_binary_sensor_connect_handle = 0;
}

void ble_binary_sensor_service_proc(void)
{
    if (ble_binary_sensor_connect_handle == 0) {
        return;
    }
}

void ble_binary_sensor_service_response_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_binary_sensor_var.rsp_client_cfg) {
        ble_indicate_for_handle(ble_binary_sensor_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BSS_RESPONSE_01_VALUE_HANDLE, buf, buf_size);
    } else {
        BINARY_SENSOR_SVC_DEBUG("bss response indicate not subscribed!\n");
    }
}


