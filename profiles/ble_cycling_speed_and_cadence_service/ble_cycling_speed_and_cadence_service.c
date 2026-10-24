#include "include.h"
#include "ble_service.h"
#include "ble_cycling_speed_and_cadence_service.h"

#define CYC_SP_CAD_SVC_DEBUG_EN 1

#if CYC_SP_CAD_SVC_DEBUG_EN
#define CYC_SP_CAD_SVC_DEBUG(...)                  printf(__VA_ARGS__)
#define CYC_SP_CAD_SVC_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define CYC_SP_CAD_SVC_DEBUG(...)
#define CYC_SP_CAD_SVC_DEBUG_R(...)
#endif

static ble_cyc_sp_cad_var_t     ble_cyc_sp_cad_var;
static att_service_handler_t    ble_cyc_sp_cad_service;
static uint16_t                 ble_cyc_sp_cad_connect_handle;

// Primary:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Secondary:       size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  uuid(2B/16B)
// Include:         size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  start_handle(2B)          end_handle(2B)    service_uuid(2B/16B)
// Characteristic:  size(2B)  properties(2B)  handle(2B)  base_uuid(2B)  character_properties(1B)  value_handle(2B)  uuid(2B/16B)
// Value:           size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   value(...)
// Descriptor:      size(2B)  properties(2B)  handle(2B)  uuid(2B/16B)   param(...)

// static const uint8_t profile_data[] =
// {
//     // 0x0027 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_CYCLING_SPEED_AND_CADENCE
//     0x0a, 0x00, 0x02, 0x00, 0x27, 0x00, 0x00, 0x28, 0x16, 0x18,
//     // 0x0028 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CSC_MEASUREMENT - DYNAMIC | NOTIFY
//     0x0d, 0x00, 0x02, 0x00, 0x28, 0x00, 0x03, 0x28, 0x10, 0x29, 0x00, 0x5b, 0x2a,
//     // 0x0029 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CSC_MEASUREMENT - DYNAMIC | NOTIFY
//     //
//     0x08, 0x00, 0x00, 0x01, 0x29, 0x00, 0x5b, 0x2a,
//     // 0x002a CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x2a, 0x00, 0x02, 0x29, 0x00, 0x00,
//     // 0x002b CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CSC_FEATURE - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x2b, 0x00, 0x03, 0x28, 0x02, 0x2c, 0x00, 0x5c, 0x2a,
//     // 0x002c VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_CSC_FEATURE - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x2c, 0x00, 0x5c, 0x2a,
//     // 0x002d CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION - DYNAMIC | READ
//     0x0d, 0x00, 0x02, 0x00, 0x2d, 0x00, 0x03, 0x28, 0x02, 0x2e, 0x00, 0x5d, 0x2a,
//     // 0x002e VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION - DYNAMIC | READ
//     // READ_ANYBODY
//     0x08, 0x00, 0x02, 0x01, 0x2e, 0x00, 0x5d, 0x2a,
//     // 0x002f CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     0x0d, 0x00, 0x02, 0x00, 0x2f, 0x00, 0x03, 0x28, 0x28, 0x30, 0x00, 0x55, 0x2a,
//     // 0x0030 VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT - DYNAMIC | WRITE | INDICATE
//     // WRITE_ANYBODY
//     0x08, 0x00, 0x08, 0x01, 0x30, 0x00, 0x55, 0x2a,
//     // 0x0031 CLIENT_CHARACTERISTIC_CONFIGURATION
//     // READ_ANYBODY, WRITE_ANYBODY
//     0x0a, 0x00, 0x0e, 0x01, 0x31, 0x00, 0x02, 0x29, 0x00, 0x00,

//     // END
//     0x00, 0x00,
// };

static void ble_cyc_sp_cad_service_event_packet_handle(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:{
            memcpy(&ble_cyc_sp_cad_connect_handle, &param[7], 2);
            CYC_SP_CAD_SVC_DEBUG("-->BLE_CYC_SP_CAD_EVENT_CONNECTED:%x\n",ble_cyc_sp_cad_connect_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            memcpy(&ble_cyc_sp_cad_connect_handle, &param[0], 2);
            CYC_SP_CAD_SVC_DEBUG("-->BLE_CYC_SP_CAD_EVENT_DISCONNECTED:%x\n",ble_cyc_sp_cad_connect_handle);
            ble_cyc_sp_cad_connect_handle = 0;
        } break;

        default:
            break;
    }
}

static uint16_t ble_cyc_sp_cad_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    CYC_SP_CAD_SVC_DEBUG("ble cyc_sp_cad read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CSC_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_cyc_sp_cad_var.meas_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_cyc_sp_cad_var.meas_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            read_len = sizeof(ble_cyc_sp_cad_var.ctr_point_client_cfg);
            if (buffer) {
                PUT_LE16(&buffer[0], ble_cyc_sp_cad_var.ctr_point_client_cfg);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CSC_FEATURE_01_VALUE_HANDLE:{
            read_len = sizeof(ble_cyc_sp_cad_var.feature);
            if (buffer) {
                memcpy(buffer, ble_cyc_sp_cad_var.feature, read_len);
            }
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SENSOR_LOCATION_02_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

    CYC_SP_CAD_SVC_DEBUG("read_len:%d\n", read_len);
    CYC_SP_CAD_SVC_DEBUG_R(buffer, read_len);
	return read_len;
}

static int ble_cyc_sp_cad_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    CYC_SP_CAD_SVC_DEBUG("ble cyc_sp_cad write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CSC_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_cyc_sp_cad_var.meas_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT_01_CLIENT_CONFIGURATION_HANDLE:{
            ble_cyc_sp_cad_var.ctr_point_client_cfg = GET_LE16(&buffer[0]);
        } break;

        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT_01_VALUE_HANDLE:{
        } break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

void ble_cyc_sp_cad_service_init(void)
{
    CYC_SP_CAD_SVC_DEBUG("ble_cyc_sp_cad_service_init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CYCLING_SPEED_AND_CADENCE_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CYCLING_SPEED_AND_CADENCE_END_HANDLE;

    // register service with ATT Server
	ble_cyc_sp_cad_service.start_handle   = start_handle;
	ble_cyc_sp_cad_service.end_handle     = end_handle;
	ble_cyc_sp_cad_service.read_callback  = &ble_cyc_sp_cad_service_read_callback;
	ble_cyc_sp_cad_service.write_callback = &ble_cyc_sp_cad_service_write_callback;
	ble_cyc_sp_cad_service.event_handler  = &ble_cyc_sp_cad_service_event_packet_handle;
	att_server_register_service_handler(&ble_cyc_sp_cad_service);

    ble_cyc_sp_cad_connect_handle = 0;
}

void ble_cyc_sp_cad_service_proc(void)
{
    if (ble_cyc_sp_cad_connect_handle == 0) {
        return;
    }
}

void ble_cyc_sp_cad_service_measurement_notify(uint8_t *buf, uint16_t buf_size)
{
    if (ble_cyc_sp_cad_var.meas_client_cfg) {
        ble_notify_for_handle(ble_cyc_sp_cad_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CSC_MEASUREMENT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        CYC_SP_CAD_SVC_DEBUG("cycling speed and cadence measurement notify not subscribed!\n");
    }
}

void ble_cyc_sp_cad_service_control_point_indicate(uint8_t *buf, uint16_t buf_size)
{
    if (ble_cyc_sp_cad_var.ctr_point_client_cfg) {
        ble_indicate_for_handle(ble_cyc_sp_cad_connect_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_SC_CONTROL_POINT_01_VALUE_HANDLE, buf, buf_size);
    } else {
        CYC_SP_CAD_SVC_DEBUG("cycling speed and cadence control point indicate not subscribed!\n");
    }
}



