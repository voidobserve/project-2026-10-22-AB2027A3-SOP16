#include "include.h"
#include "ble_service.h"
#include "ble_hid_controller_service.h"

static att_service_handler_t    hids_service;
static uint16_t                 hids_connect_handle;

static uint16_t                 hids_report_1_client_config;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)
const uint8_t profile_data[] =
{
    // 0x0001 PRIMARY_SERVICE-1812
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x12, 0x18,
    // 0x0002 CHARACTERISTIC-2a4a - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x4a, 0x2a,
    // 0x0003 VALUE CHARACTERISTIC-2a4a - READ -'11 01 00 00'
    // READ_ANYBODY
    0x0c, 0x00, 0x02, 0x00, 0x03, 0x00, 0x4a, 0x2a, 0x11, 0x01, 0x00, 0x00,
    // 0x0004 CHARACTERISTIC-2a4d - READ | WRITE | NOTIFY | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x1a, 0x05, 0x00, 0x4d, 0x2a,
    // 0x0005 VALUE CHARACTERISTIC-2a4d - READ | WRITE | NOTIFY | DYNAMIC
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x05, 0x00, 0x4d, 0x2a,
    // 0x0006 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x06, 0x00, 0x02, 0x29, 0x00, 0x00,
    // 0x0007 REPORT_REFERENCE-READ-1-1
    0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x08, 0x29, 0x1, 0x1,
    // 0x0008 CHARACTERISTIC-2a4b - READ | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x02, 0x09, 0x00, 0x4b, 0x2a,
    // 0x0009 VALUE CHARACTERISTIC-2a4b - READ | DYNAMIC
    // READ_ANYBODY
    0x08, 0x00, 0x02, 0xf1, 0x09, 0x00, 0x4b, 0x2a,
    // 0x000a CHARACTERISTIC-2a4c - WRITE_WITHOUT_RESPONSE
    0x0d, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x03, 0x28, 0x04, 0x0b, 0x00, 0x4c, 0x2a,
    // 0x000b VALUE CHARACTERISTIC-2a4c - WRITE_WITHOUT_RESPONSE -''
    // WRITE_ANYBODY
    0x08, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x4c, 0x2a,

    // END
    0x00, 0x00,
}; // total size 64 bytes

const uint8_t ble_hid_descriptor[] = {
    0x05, 0x0C, // Usage Page (Consumer)
    0x09, 0x01, // Usage (Consumer Control)
    0xA1, 0x01, // Collection (Apakcation)
    0x85, 0x01, // Report Id (2)
    0x15, 0x00, // Logical minimum (0)
    0x25, 0x01, // Logical maximum (1)
    0x75, 0x01, // Report Size (1)
    0x95, 0x08, // Report Count (8)
    0x09, 0xE9, // Usage (Volme increment)
    0x09, 0xEA, // Usage (Volme Decrement)
    0x09, 0x40, // Usage (Menu)
    0x09, 0x30, // Usage (Power)
    0x09, 0xB5, // Usage (Scan Next Track)
    0x09, 0xB6, // Usage (Scan Previous Track)
    0x0A, 0x23, 0x02, // Usage (AC Home)
    0x09, 0xCD, // Usage (Play/Pause)
    0x81, 0x02, // Input (Data, Value,Absolute Bit field)
    0x75, 0x01, // Report Size (1)
    0x95, 0x01, // Report Count (1)
    0x05, 0x0B, // Usage Page (Telephony)
    0x09, 0x21, // Usage (flash)
    0x81, 0x02, // Input (Data, Value,Absolute,Bit Field)
    0x75, 0x07, // Report Size (7)
    0x95, 0x01, // Report Count (1)
    0x81, 0x03, // Input (Constant, Value,Absolute,Bit Field)
    0xC0 // End Collection
};


static void hids_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            memcpy(&hids_connect_handle, &param[7], 2);
            printf("-->BLE_EVENT_CONNECTED:%x\n",hids_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint16_t con_handle;
            memcpy(&con_handle, &param[0], 2);
            printf("-->BLE_EVENT_DISCONNECTED:%x\n",con_handle);
            hids_connect_handle = 0;
            hids_report_1_client_config = 0;
        } break;

        default:
            break;
    }
}

static uint16_t hids_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("hids_service_read_callback\n");
    u8 hid_report_map_len = sizeof(ble_hid_descriptor);
    u8 data_len = 0;

    if (attribute_handle == ATT_CHARACTERISTIC_2a4b_01_VALUE_HANDLE) {
        if (buffer) {
            data_len = (buffer_size < (hid_report_map_len - offset))? buffer_size: (hid_report_map_len - offset);
            memcpy(buffer, ble_hid_descriptor + offset, data_len);
            return data_len;
        }
        return hid_report_map_len;
    }

	return 0;
}


static int hids_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("hids_service_write_callback\n");

    if(attribute_handle == ATT_CHARACTERISTIC_2a4d_01_CLIENT_CONFIGURATION_HANDLE){
        hids_report_1_client_config = GET_LE16(&buffer[0]);
    }

	return ATT_ERR_NO_ERR;
}

void ble_hid_send_consumer_cmd(u16 cmd, u8 hold)
{
    u8 cmd_buf[2];

    memset(cmd_buf, 0, 2);
    cmd_buf[0] = cmd & 0xff;
    cmd_buf[1] = (cmd & 0xff00) >> 8;

    if (hids_report_1_client_config) {
        ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2a4d_01_VALUE_HANDLE, cmd_buf, sizeof(cmd_buf));
        if (!hold) {
            memset(cmd_buf, 0, 2);
            ble_notify_for_handle(hids_connect_handle, ATT_CHARACTERISTIC_2a4d_01_VALUE_HANDLE, cmd_buf, sizeof(cmd_buf));
        }
    }
}

void ble_hids_service_init(void)
{
    printf("ble_hids_service_init\n");

    // register service with ATT Server
	hids_service.start_handle   = ATT_SERVICE_1812_START_HANDLE;
	hids_service.end_handle     = ATT_SERVICE_1812_END_HANDLE;
	hids_service.read_callback  = &hids_service_read_callback;
	hids_service.write_callback = &hids_service_write_callback;
	hids_service.event_handler  = &hids_service_event_packet_handle;
	att_server_register_service_handler(&hids_service);
}
