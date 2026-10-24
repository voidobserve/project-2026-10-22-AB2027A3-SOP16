#include "include.h"
#include "ble_service.h"
#include "ble_cycling_power_service.h"

#define CYCLING_POWER_SVC_DEBUG_EN 1

#if CYCLING_POWER_SVC_DEBUG_EN
#define CYCLING_POWER_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define CYCLING_POWER_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define CYCLING_POWER_SVC_DEBUG(...)
#define CYCLING_POWER_SVC_DEBUG_R(...)
#endif

static ble_cycling_power_var_t  ble_cycling_power_var;
static att_service_handler_t    ble_cycling_power_service;
static uint16_t                 ble_cycling_power_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x001e PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_CYCLING_POWER
//     0x0a, 0x00, 0x02, 0x00, 0x1e, 0x00, 0x00, 0x28, 0x18, 0x18,
//     // 0x001f CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT - DYNAMIC | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x1f, 0x00, 0x03, 0x28, 0x10, 0x20, 0x00, 0x63, 0x2a,
//     // 0x0020 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT - DYNAMIC | NOTIFY
//     //
//     0x08, 0x00, 0x00, 0x01, 0x20, 0x00, 0x63, 0x2a,
//     // 0x0021 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x21, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x0022 SERVER_CONFIGURATION-READ | WRITE-
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x22, 0x00, 0x03, 0x29,
//     // 0x0023 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x23, 0x00, 0x03, 0x28, 0x02, 0x24, 0x00, 0x65, 0x2a,
//     // 0x0024 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x24, 0x00, 0x65, 0x2a,
//     // 0x0025 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x25, 0x00, 0x03, 0x28, 0x02, 0x26, 0x00, 0x5d, 0x2a,
//     // 0x0026 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x26, 0x00, 0x5d, 0x2a,

//     // END
//     0x00, 0x00,
// };

static void ble_cycling_power_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_cycling_power_connect_handle, &param[7], 2);
            CYCLING_POWER_SVC_DEBUG("-->BLE_CYCLING_POWER_EVENT_CONNECTED:%x\n",ble_cycling_power_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cycling_power_connect_handle, &param[0], 2);
            CYCLING_POWER_SVC_DEBUG("-->BLE_CYCLING_POWER_EVENT_DISCONNECTED:%x\n",ble_cycling_power_connect_handle);
            ble_cycling_power_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_cycling_power_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    CYCLING_POWER_SVC_DEBUG("ble cycling_power read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_cycling_power_var.client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_cycling_power_var.client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_01_SERVER_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_cycling_power_var.server_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_cycling_power_var.server_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_cycling_power_var.feature);
            if (buffer) {
                memcpy(buffer, ble_cycling_power_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

    CYCLING_POWER_SVC_DEBUG("read_len:%d\n", read_len);
    CYCLING_POWER_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_cycling_power_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    CYCLING_POWER_SVC_DEBUG("ble cycling_power write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_cycling_power_var.client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_01_SERVER_CONFIGURATION_HANDLE:{
            ble_cycling_power_var.server_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_cycling_power_service_init(void)
{
    CYCLING_POWER_SVC_DEBUG("ble_cycling_power_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CYCLING_POWER_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CYCLING_POWER_END_HANDLE;

    // register service with ATT Server
	ble_cycling_power_service.start_handle   = start_handle;
	ble_cycling_power_service.end_handle     = end_handle;
	ble_cycling_power_service.read_callback  = &ble_cycling_power_service_read_callback;
	ble_cycling_power_service.write_callback = &ble_cycling_power_service_write_callback;
	ble_cycling_power_service.event_handler  = &ble_cycling_power_service_event_packet_handle;
	att_server_register_service_handler(&ble_cycling_power_service);

    ble_cycling_power_connect_handle = 0;
}

void ble_cycling_power_service_proc(void)
{
    if (ble_cycling_power_connect_handle == 0) {
        return;
    }
}

void ble_cycling_power_service_client_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_cycling_power_var.client_cfg) {
        ble_notify_for_handle(ble_cycling_power_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CYCLING_POWER_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        CYCLING_POWER_SVC_DEBUG("cycling power not subscribed\n");
    }
}

