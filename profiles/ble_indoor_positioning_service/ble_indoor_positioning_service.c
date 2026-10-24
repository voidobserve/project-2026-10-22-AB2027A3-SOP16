#include "include.h"
#include "ble_service.h"
#include "ble_indoor_positioning_service.h"

#define INDOOR_POSITIONING_SVC_DEBUG_EN 1

#if INDOOR_POSITIONING_SVC_DEBUG_EN
#define INDOOR_POSITIONING_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define INDOOR_POSITIONING_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define INDOOR_POSITIONING_SVC_DEBUG(...)
#define INDOOR_POSITIONING_SVC_DEBUG_R(...)
#endif

static ble_indoor_positioning_var_t ble_indoor_positioning_var;
static att_service_handler_t        ble_indoor_positioning_service;
static uint16_t                     ble_indoor_positioning_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0082 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_INDOOR_POSITIONING
//     0x0a, 0x00, 0x02, 0x00, 0x82, 0x00, 0x00, 0x28, 0x21, 0x18,
//     // 0x0083 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_BIKE_DATA - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x83, 0x00, 0x03, 0x28, 0x02, 0x84, 0x00, 0xd2, 0x2a,
//     // 0x0084 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_BIKE_DATA - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x84, 0x00, 0xd2, 0x2a,
//     // 0x0085 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_POSITIONING_CONFIGURATION - DYNAMIC | READ | WRITE | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x85, 0x00, 0x03, 0x28, 0x1a, 0x86, 0x00, 0xad, 0x2a,
//     // 0x0086 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_POSITIONING_CONFIGURATION - DYNAMIC | READ | WRITE | NOTIFY
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x86, 0x00, 0xad, 0x2a,
//     // 0x0087 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x87, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_indoor_positioning_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_indoor_positioning_connect_handle, &param[7], 2);
            INDOOR_POSITIONING_SVC_DEBUG("-->BLE_INDOOR_POSITIONING_EVENT_CONNECTED:%x\n",ble_indoor_positioning_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_indoor_positioning_connect_handle, &param[0], 2);
            INDOOR_POSITIONING_SVC_DEBUG("-->BLE_INDOOR_POSITIONING_EVENT_DISCONNECTED:%x\n",ble_indoor_positioning_connect_handle);
            ble_indoor_positioning_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_indoor_positioning_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    INDOOR_POSITIONING_SVC_DEBUG("===>ble indoor_positioning read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_BIKE_DATA_01_VALUE_HANDLE:{
            read_len = sizeof(ble_indoor_positioning_var.bik_data);
            if (buffer) {
                memcpy(buffer, ble_indoor_positioning_var.bik_data, sizeof(ble_indoor_positioning_var.bik_data));
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_POSITIONING_CONFIGURATION_01_VALUE_HANDLE:{
            read_len = sizeof(ble_indoor_positioning_var.positoning_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_indoor_positioning_var.positoning_client_cfg);
            }
        } break;

        default:
            break;
    }

    INDOOR_POSITIONING_SVC_DEBUG("read_len:%d\n", read_len);
    INDOOR_POSITIONING_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_indoor_positioning_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    INDOOR_POSITIONING_SVC_DEBUG("===>ble indoor_positioning write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_POSITIONING_CONFIGURATION_01_VALUE_HANDLE:{
            ble_indoor_positioning_var.positoning_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_indoor_positioning_service_init(void)
{
    INDOOR_POSITIONING_SVC_DEBUG("ble_indoor_positioning_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_INDOOR_POSITIONING_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_INDOOR_POSITIONING_END_HANDLE;

    // register service with ATT Server
	ble_indoor_positioning_service.start_handle   = start_handle;
	ble_indoor_positioning_service.end_handle     = end_handle;
	ble_indoor_positioning_service.read_callback  = &ble_indoor_positioning_service_read_callback;
	ble_indoor_positioning_service.write_callback = &ble_indoor_positioning_service_write_callback;
	ble_indoor_positioning_service.event_handler  = &ble_indoor_positioning_service_event_packet_handle;
	att_server_register_service_handler(&ble_indoor_positioning_service);

    ble_indoor_positioning_connect_handle = 0;
}

void ble_indoor_positioning_service_proc(void)
{
    if (ble_indoor_positioning_connect_handle == 0) {
        return;
    }
}

void ble_indoor_positioning_service_indoor_positioning_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_indoor_positioning_var.positoning_client_cfg) {
        ble_notify_for_handle(ble_indoor_positioning_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_INDOOR_POSITIONING_CONFIGURATION_01_VALUE_HANDLE, buf, buf_size);
    } else {
        INDOOR_POSITIONING_SVC_DEBUG("indoor positioning notify not subscribed!\n");
    }
}
