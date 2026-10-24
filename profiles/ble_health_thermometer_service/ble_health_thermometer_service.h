#ifndef __BLE_HLT_TH_SERVICE_H
#define __BLE_HLT_TH_SERVICE_H
#include "include.h"

// Health Thermometer  0x1809
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_HEALTH_THERMOMETER
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_MEASUREMENT, INDICATE,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HEALTH_THERMOMETER_START_HANDLE 0x007b
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_HEALTH_THERMOMETER_END_HANDLE 0x007e

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_MEASUREMENT_01_VALUE_HANDLE 0x007d
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_MEASUREMENT_01_CLIENT_CONFIGURATION_HANDLE 0x007e

typedef struct{
    uint16_t meas_client_cfg;
}ble_hlt_th_var_t;

void ble_hlt_th_service_init(void);
void ble_hlt_th_service_proc(void);
void ble_hlt_th_service_measurement_indicate(uint8_t *buf, uint16_t buf_size);
#endif
