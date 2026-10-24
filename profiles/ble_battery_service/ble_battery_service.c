#include "include.h"
#include "ble_service.h"
#include "ble_battery_service.h"

#define BAT_SVC_DEBUG_EN 1

#if BAT_SVC_DEBUG_EN
#define BAT_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define BAT_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_battery_service;
static ble_bat_svc_var_t        ble_bat_svc_var;
static uint16_t                 ble_battery_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0001 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE
//     0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x0f, 0x18,
//     // 0x0002 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x19, 0x2a,
//     // 0x0003 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x03, 0x00, 0x19, 0x2a,
//     // 0x0004 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS - DYNAMIC | READ | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x12, 0x05, 0x00, 0xed, 0x2b,
//     // 0x0005 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS - DYNAMIC | READ | NOTIFY
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x05, 0x00, 0xed, 0x2b,
//     // 0x0006 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x06, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_battery_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_battery_connect_handle, &param[7], 2);
            BAT_SVC_DEBUG("-->BLE_BAT_EVENT_CONNECTED:%x\n",ble_battery_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_battery_connect_handle, &param[0], 2);
            BAT_SVC_DEBUG("-->BLE_BAT_EVENT_DISCONNECTED:%x\n",ble_battery_connect_handle);
            ble_battery_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint32_t ble_battery_service_get_bat_level(uint8_t *buf, uint16_t buf_size)
{
    uint32_t ret = 0;
    uint8_t vbat_level[1] = {0};
#if BSP_VBAT_DETECT_EN
        vbat_level[0] = bsp_get_vbat_level(0);
#else
        vbat_level[0] = 100;
#endif // BSP_VBAT_DETECT_EN
    memcpy(buf, vbat_level, buf_size);
    ret = buf_size;
    return ret;
}

static uint16_t ble_battery_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    BAT_SVC_DEBUG("ble battery read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_01_VALUE_HANDLE:{
            read_len = 1;
            if (buffer) {
                read_len = ble_battery_service_get_bat_level(buffer, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_VALUE_HANDLE:{
            read_len = sizeof(ble_bat_svc_var.level_sta);
            if (buffer) {
                memcpy(buffer, ble_bat_svc_var.level_sta, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_bat_svc_var.level_sta_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_bat_svc_var.level_sta_client_cfg);
            }
        } break;

        default:
            break;
    }

    BAT_SVC_DEBUG("read_len:%d\n", read_len);
    BAT_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_battery_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    uint16_t write_len = 0;
    BAT_SVC_DEBUG("ble battery write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_bat_svc_var.level_sta_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }
	return write_len;
}

void ble_battery_service_init(void)
{
    BAT_SVC_DEBUG("ble_battery_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE_END_HANDLE;

    // register service with ATT Server
	ble_battery_service.start_handle   = start_handle;
	ble_battery_service.end_handle     = end_handle;
	ble_battery_service.read_callback  = &ble_battery_service_read_callback;
	ble_battery_service.write_callback = &ble_battery_service_write_callback;
	ble_battery_service.event_handler  = &ble_battery_service_event_packet_handle;
	att_server_register_service_handler(&ble_battery_service);

    ble_battery_connect_handle = 0;
}

void ble_battery_service_proc(void)
{
    if (ble_battery_connect_handle == 0) {
        return;
    }
}

void ble_battery_service_level_status_set(uint8_t *lv_sta)
{
    memcpy(ble_bat_svc_var.level_sta, lv_sta, sizeof(ble_bat_svc_var.level_sta));
}

void ble_battery_service_level_status_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_bat_svc_var.level_sta_client_cfg) {
        ble_notify_for_handle(ble_battery_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_VALUE_HANDLE, buf, buf_size);
    } else {
        BAT_SVC_DEBUG("battery level status notify not subscribed!\n");
    }
}
