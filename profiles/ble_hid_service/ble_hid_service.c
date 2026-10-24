#include "include.h"
#include "ble_service.h"
#include "ble_hid_service.h"

#define HID_SVC_DEBUG_EN 1

#if HID_SVC_DEBUG_EN
#define HID_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define HID_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define BAT_SVC_DEBUG(...)
#define BAT_SVC_DEBUG_R(...)
#endif

static att_service_handler_t    ble_hid_service;
static uint16_t                 ble_hid_connect_handle;
ble_hid_var_t                   ble_hid_var AT(.ble_hid.var);

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0056 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE
//     0x0a, 0x00, 0x02, 0x00, 0x56, 0x00, 0x00, 0x28, 0x12, 0x18,
//     // 0x0057 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PROTOCOL_MODE - DYNAMIC | READ | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0x57, 0x00, 0x03, 0x28, 0x06, 0x58, 0x00, 0x4e, 0x2a,
//     // 0x0058 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PROTOCOL_MODE - DYNAMIC | READ | WRITE_WITHOUT_RESPONSE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x06, 0x01, 0x58, 0x00, 0x4e, 0x2a,
//     // 0x0059 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     0x0d, 0x00, 0x02, 0x00, 0x59, 0x00, 0x03, 0x28, 0x1a, 0x5a, 0x00, 0x4d, 0x2a,
//     // 0x005a VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     // READ_ENCRYPTED, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x08, 0x00, 0x0b, 0xf5, 0x5a, 0x00, 0x4d, 0x2a,
//     // 0x005b CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x0a, 0x00, 0x0f, 0xf1, 0x5b, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // fixed report id = 1, type = Input (1)
//     // 0x005c REPORT_REFERENCE-READ-1-1
//     0x0a, 0x00, 0x02, 0x00, 0x5c, 0x00, 0x08, 0x29, 0x1, 0x1,
//     // 0x005d CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     0x0d, 0x00, 0x02, 0x00, 0x5d, 0x00, 0x03, 0x28, 0x1a, 0x5e, 0x00, 0x4d, 0x2a,
//     // 0x005e VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     // READ_ENCRYPTED, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x08, 0x00, 0x0b, 0xf5, 0x5e, 0x00, 0x4d, 0x2a,
//     // 0x005f CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x0a, 0x00, 0x0f, 0xf1, 0x5f, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // fixed report id = 2, type = Output (2)
//     // 0x0060 REPORT_REFERENCE-READ-2-2
//     0x0a, 0x00, 0x02, 0x00, 0x60, 0x00, 0x08, 0x29, 0x2, 0x2,
//     // 0x0061 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     0x0d, 0x00, 0x02, 0x00, 0x61, 0x00, 0x03, 0x28, 0x1a, 0x62, 0x00, 0x4d, 0x2a,
//     // 0x0062 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT - DYNAMIC | READ | WRITE | NOTIFY | ENCRYPTION_KEY_SIZE_16
//     // READ_ENCRYPTED, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x08, 0x00, 0x0b, 0xf5, 0x62, 0x00, 0x4d, 0x2a,
//     // 0x0063 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ENCRYPTED, ENCRYPTION_KEY_SIZE=16
//     0x0a, 0x00, 0x0f, 0xf1, 0x63, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // fixed report id = 3, type = Feature (3)
//     // 0x0064 REPORT_REFERENCE-READ-3-3
//     0x0a, 0x00, 0x02, 0x00, 0x64, 0x00, 0x08, 0x29, 0x3, 0x3,
//     // 0x0065 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT_MAP - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x65, 0x00, 0x03, 0x28, 0x02, 0x66, 0x00, 0x4b, 0x2a,
//     // 0x0066 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_REPORT_MAP - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x66, 0x00, 0x4b, 0x2a,
//     // 0x0067 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT - DYNAMIC | READ | WRITE | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x67, 0x00, 0x03, 0x28, 0x1a, 0x68, 0x00, 0x22, 0x2a,
//     // 0x0068 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT - DYNAMIC | READ | WRITE | NOTIFY
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x68, 0x00, 0x22, 0x2a,
//     // 0x0069 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x69, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x006a CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_OUTPUT_REPORT - DYNAMIC | READ | WRITE | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0x6a, 0x00, 0x03, 0x28, 0x0e, 0x6b, 0x00, 0x32, 0x2a,
//     // 0x006b VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_OUTPUT_REPORT - DYNAMIC | READ | WRITE | WRITE_WITHOUT_RESPONSE
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0e, 0x01, 0x6b, 0x00, 0x32, 0x2a,
//     // 0x006c CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_MOUSE_INPUT_REPORT - DYNAMIC | READ | WRITE | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x6c, 0x00, 0x03, 0x28, 0x1a, 0x6d, 0x00, 0x33, 0x2a,
//     // 0x006d VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BOOT_MOUSE_INPUT_REPORT - DYNAMIC | READ | WRITE | NOTIFY
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x08, 0x00, 0x0a, 0x01, 0x6d, 0x00, 0x33, 0x2a,
//     // 0x006e CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x6e, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // bcdHID = 0x101 (v1.0.1), bCountryCode 0, remote wakeable = 0 | normally connectable 2
//     // 0x006f CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HID_INFORMATION - READ
//     0x0d, 0x00, 0x02, 0x00, 0x6f, 0x00, 0x03, 0x28, 0x02, 0x70, 0x00, 0x4a, 0x2a,
//     // 0x0070 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HID_INFORMATION - READ -'01 01 00 02'
//     // READ_ANYBODY
//     0x0c, 0x00, 0x02, 0x00, 0x70, 0x00, 0x4a, 0x2a, 0x01, 0x01, 0x00, 0x02,
//     // 0x0071 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HID_CONTROL_POINT - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     0x0d, 0x00, 0x02, 0x00, 0x71, 0x00, 0x03, 0x28, 0x04, 0x72, 0x00, 0x4c, 0x2a,
//     // 0x0072 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_HID_CONTROL_POINT - DYNAMIC | WRITE_WITHOUT_RESPONSE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x04, 0x01, 0x72, 0x00, 0x4c, 0x2a,

//     // END
//     0x00, 0x00,
// };

const uint8_t ble_hid_descriptor[] = {
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x01, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00,
    0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x05,
    0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x05, 0x91, 0x02, 0x95, 0x01, 0x75, 0x03, 0x91, 0x01,
    0x95, 0x06, 0x75, 0x08, 0x15, 0x28, 0x25, 0xFE, 0x05, 0x07, 0x19, 0x28, 0x29, 0xFE, 0x81, 0x00,
    0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x85, 0xF1, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x18,
    0x09, 0xB5, 0x09, 0xB6, 0x09, 0xB7, 0x09, 0xCD, 0x09, 0xE2, 0x09, 0xE5, 0x09, 0xE7, 0x09, 0xE9,
    0x09, 0xEA, 0x0A, 0x52, 0x01, 0x0A, 0x53, 0x01, 0x0A, 0x54, 0x01, 0x0A, 0x55, 0x01, 0x0A, 0x83,
    0x01, 0x0A, 0x8A, 0x01, 0x0A, 0x92, 0x01, 0x0A, 0x94, 0x01, 0x0A, 0x21, 0x02, 0x0A, 0x23, 0x02,
    0x0A, 0x24, 0x02, 0x0A, 0x25, 0x02, 0x0A, 0x26, 0x02, 0x0A, 0x27, 0x02, 0x0A, 0x2A, 0x02, 0x81,
    0x02, 0xC0, 0xC0,
};

const uint8_t ble_hid_information[] = {
    0x00, 0x00,     //Hid Version
    0x00,           //Country Code
    0x01,           //feature
};

static void ble_hid_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_hid_connect_handle, &param[7], 2);
            HID_SVC_DEBUG("-->BLE_HID_EVENT_CONNECTED:%x\n",ble_hid_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_hid_connect_handle, &param[0], 2);
            HID_SVC_DEBUG("-->BLE_HID_EVENT_DISCONNECTED:%x\n",ble_hid_connect_handle);
            ble_hid_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_hid_service_read_cb_proc(uint16_t data_size, const uint8_t *data, uint16_t offset, uint8_t *read_buf, uint16_t read_buf_size)
{
    uint16_t read_size = data_size;
    read_size = read_size - offset;
    read_size = (read_size > read_buf_size) ? read_buf_size : read_size;
    memcpy(read_buf, data+offset, read_size);
    return read_size;
}

static uint16_t ble_hid_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    HID_SVC_DEBUG("hid read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch(attribute_handle){
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_HID_INFORMATION_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_information);
            if (buffer) {
                read_len= ble_hid_service_read_cb_proc(read_len, ble_hid_information, offset, buffer, buffer_size);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_MAP_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_descriptor);
            if (buffer) {
                read_len = ble_hid_service_read_cb_proc(read_len, ble_hid_descriptor, offset, buffer, buffer_size);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_01_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_var.input_value);
            if (buffer) {
                buffer[0] = ble_hid_var.input_value;
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_02_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_var.output_value);
            if (buffer) {
                buffer[0] = ble_hid_var.output_value;
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_03_VALUE_HANDLE:{
            read_len = sizeof(ble_hid_var.feature_value);
            if (buffer) {
                buffer[0] = ble_hid_var.feature_value;
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_hid_var.input_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_hid_var.input_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_02_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_hid_var.output_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_hid_var.output_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_03_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_hid_var.feature_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_hid_var.feature_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_hid_var.keyboard_input_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_hid_var.keyboard_input_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_MOUSE_INPUT_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_hid_var.mouse_input_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_hid_var.mouse_input_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PROTOCOL_MODE_01_VALUE_HANDLE:{
            read_len = 1;
            if (buffer) {
                buffer[0] = ble_hid_var.mouse_input_value;
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT_01_VALUE_HANDLE:{
            read_len = 1;
            if (buffer) {
                buffer[0] = ble_hid_var.keyboard_input_value;
            }
        } break;

        default:
            break;
    }

    HID_SVC_DEBUG("read_len:%d\n", read_len);
    HID_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_hid_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    HID_SVC_DEBUG("hid write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_01_VALUE_HANDLE:{
            ble_hid_var.input_value = buffer[0];
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_02_VALUE_HANDLE:{
            ble_hid_var.output_value = buffer[0];
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_03_VALUE_HANDLE:{
            ble_hid_var.feature_value = buffer[0];
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_hid_var.input_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_02_CLIENT_CONFIGURATION_HANDLE:{
            ble_hid_var.output_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_03_CLIENT_CONFIGURATION_HANDLE:{
            ble_hid_var.feature_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_hid_var.keyboard_input_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_MOUSE_INPUT_REPORT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_hid_var.mouse_input_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT_01_VALUE_HANDLE:{
            ble_hid_var.keyboard_input_value  = buffer[0];
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PROTOCOL_MODE_01_VALUE_HANDLE:{
            ble_hid_var.mouse_input_value  = buffer[0];
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_hid_service_init(void)
{
    HID_SVC_DEBUG("ble_ble_hid_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE_END_HANDLE;

    // register service with ATT Server
	ble_hid_service.start_handle   = start_handle;
	ble_hid_service.end_handle     = end_handle;
	ble_hid_service.read_callback  = &ble_hid_service_read_callback;
	ble_hid_service.write_callback = &ble_hid_service_write_callback;
	ble_hid_service.event_handler  = &ble_hid_service_event_packet_handle;
	att_server_register_service_handler(&ble_hid_service);

    ble_hid_connect_handle = 0;
}

void ble_hid_service_proc(void)
{
    if (ble_hid_connect_handle == 0) {
        return;
    }
}

void ble_hid_service_report_input_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_hid_var.input_client_cfg) {
        ble_notify_for_handle(ble_hid_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        HID_SVC_DEBUG("hid report input notify not subscribed!\n");
    }
}

void ble_hid_service_report_output_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_hid_var.output_client_cfg) {
        ble_notify_for_handle(ble_hid_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_02_VALUE_HANDLE, buf, buf_size);
    } else {
        HID_SVC_DEBUG("hid report output notify not subscribed!\n");
    }
}

void ble_hid_service_report_feature_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_hid_var.feature_client_cfg) {
        ble_notify_for_handle(ble_hid_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_REPORT_03_VALUE_HANDLE, buf, buf_size);
    } else {
        HID_SVC_DEBUG("hid report feature notify not subscribed!\n");
    }
}

void ble_hid_service_keyboard_input_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_hid_var.keyboard_input_client_cfg) {
        ble_notify_for_handle(ble_hid_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_KEYBOARD_INPUT_REPORT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        HID_SVC_DEBUG("hid keyboard input notify not subscribed!\n");
    }
}

void ble_hid_service_mouse_input_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_hid_var.mouse_input_client_cfg) {
        ble_notify_for_handle(ble_hid_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BOOT_MOUSE_INPUT_REPORT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        HID_SVC_DEBUG("hid mouse input notify not subscribed!\n");
    }
}


