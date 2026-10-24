#include "include.h"
#include "ble_service.h"
#include "ble_pulse_oximeter_service.h"

#define PULSE_OXIMETER_SVC_DEBUG_EN 1

#if PULSE_OXIMETER_SVC_DEBUG_EN
#define PULSE_OXIMETER_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define PULSE_OXIMETER_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define PULSE_OXIMETER_SVC_DEBUG(...)
#define PULSE_OXIMETER_SVC_DEBUG_R(...)
#endif

static ble_pulse_oximeter_var_t ble_pulse_oximeter_var;
static att_service_handler_t    ble_pulse_oximeter_service;
static uint16_t                 ble_pulse_oximeter_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x00a7 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_PULSE_OXIMETER
//     0x0a, 0x00, 0x02, 0x00, 0xa7, 0x00, 0x00, 0x28, 0x22, 0x18,
//     // 0x00a8 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_CONTINUOUS_MEASUREMENT - NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0xa8, 0x00, 0x03, 0x28, 0x10, 0xa9, 0x00, 0x5f, 0x2a,
//     // 0x00a9 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_CONTINUOUS_MEASUREMENT - NOTIFY -''
//     //
//     0x08, 0x00, 0x00, 0x00, 0xa9, 0x00, 0x5f, 0x2a,
//     // 0x00aa CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xaa, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x00ab CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_FEATURES - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0xab, 0x00, 0x03, 0x28, 0x02, 0xac, 0x00, 0x60, 0x2a,
//     // 0x00ac VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_FEATURES - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0xac, 0x00, 0x60, 0x2a,
//     // 0x00ad CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_SPOT_CHECK_MEASUREMENT - INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0xad, 0x00, 0x03, 0x28, 0x20, 0xae, 0x00, 0x5e, 0x2a,
//     // 0x00ae VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PLX_SPOT_CHECK_MEASUREMENT - INDICATE -''
//     //
//     0x08, 0x00, 0x00, 0x00, 0xae, 0x00, 0x5e, 0x2a,
//     // 0x00af CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xaf, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x00b0 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PULSE_OXIMETRY_CONTROL_POINT - DYNAMIC | INDICATE | WRITE
//     0x0d, 0x00, 0x02, 0x00, 0xb0, 0x00, 0x03, 0x28, 0x28, 0xb1, 0x00, 0x62, 0x2a,
//     // 0x00b1 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_PULSE_OXIMETRY_CONTROL_POINT - DYNAMIC | INDICATE | WRITE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0xb1, 0x00, 0x62, 0x2a,
//     // 0x00b2 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0xb2, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_pulse_oximeter_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_pulse_oximeter_connect_handle, &param[7], 2);
            PULSE_OXIMETER_SVC_DEBUG("-->BLE_PULSE_OXIMETER_EVENT_CONNECTED:%x\n",ble_pulse_oximeter_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_pulse_oximeter_connect_handle, &param[0], 2);
            PULSE_OXIMETER_SVC_DEBUG("-->BLE_PULSE_OXIMETER_EVENT_DISCONNECTED:%x\n",ble_pulse_oximeter_connect_handle);
            ble_pulse_oximeter_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_pulse_oximeter_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    PULSE_OXIMETER_SVC_DEBUG("ble pulse_oximeter read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_FEATURES_01_VALUE_HANDLE:{
            read_len = sizeof(ble_pulse_oximeter_var.feature);
            if (buffer) {
                memcpy(buffer, ble_pulse_oximeter_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_CONTINUOUS_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_pulse_oximeter_var.ctn_meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_pulse_oximeter_var.ctn_meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_SPOT_CHECK_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_pulse_oximeter_var.check_meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_pulse_oximeter_var.check_meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PULSE_OXIMETRY_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_pulse_oximeter_var.ctl_point_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_pulse_oximeter_var.ctl_point_client_cfg);
            }
        } break;

        default:
            break;
    }

    PULSE_OXIMETER_SVC_DEBUG("read_len:%d\n", read_len);
    PULSE_OXIMETER_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_pulse_oximeter_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    PULSE_OXIMETER_SVC_DEBUG("ble pulse_oximeter write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_CONTINUOUS_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_pulse_oximeter_var.ctn_meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_SPOT_CHECK_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_pulse_oximeter_var.check_meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PULSE_OXIMETRY_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_pulse_oximeter_var.ctl_point_client_cfg = GET_LE16(&buffer[0]);
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_pulse_oximeter_service_init(void)
{
    PULSE_OXIMETER_SVC_DEBUG("ble_pulse_oximeter_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_PULSE_OXIMETER_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_PULSE_OXIMETER_END_HANDLE;

    // register service with ATT Server
	ble_pulse_oximeter_service.start_handle   = start_handle;
	ble_pulse_oximeter_service.end_handle     = end_handle;
	ble_pulse_oximeter_service.read_callback  = &ble_pulse_oximeter_service_read_callback;
	ble_pulse_oximeter_service.write_callback = &ble_pulse_oximeter_service_write_callback;
	ble_pulse_oximeter_service.event_handler  = &ble_pulse_oximeter_service_event_packet_handle;
	att_server_register_service_handler(&ble_pulse_oximeter_service);

    ble_pulse_oximeter_connect_handle = 0;
}

void ble_pulse_oximeter_service_proc(void)
{
    if (ble_pulse_oximeter_connect_handle == 0) {
        return;
    }
}

void ble_pulse_oximeter_service_comtinuous_masurement_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_pulse_oximeter_var.ctn_meas_client_cfg) {
        ble_notify_for_handle(ble_pulse_oximeter_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_CONTINUOUS_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        PULSE_OXIMETER_SVC_DEBUG("plx continuous measurement notify not subscribed!\n");
    }
}

void ble_pulse_oximeter_service_spot_check_masurement_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_pulse_oximeter_var.check_meas_client_cfg) {
        ble_indicate_for_handle(ble_pulse_oximeter_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PLX_SPOT_CHECK_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        PULSE_OXIMETER_SVC_DEBUG("plx spot check measurement indicate not subscribed!\n");
    }
}

void ble_pulse_oximeter_service_control_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_pulse_oximeter_var.ctl_point_client_cfg) {
        ble_indicate_for_handle(ble_pulse_oximeter_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_PULSE_OXIMETRY_CONTROL_POINT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        PULSE_OXIMETER_SVC_DEBUG("pulse oximetry control point indicate not subscribed!\n");
    }
}

