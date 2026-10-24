#ifndef __BLE_CURRENT_TIME_SERVICE_H
#define __BLE_CURRENT_TIME_SERVICE_H
#include "include.h"

// Current Time 0x1805
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_CURRENT_TIME
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME, DYNAMIC | READ | NOTIFY,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CURRENT_TIME_START_HANDLE 0x0032
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_CURRENT_TIME_END_HANDLE 0x0035

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_VALUE_HANDLE 0x0034
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_CURRENT_TIME_01_CLIENT_CONFIGURATION_HANDLE 0x0035

typedef struct{
    uint8_t time_data[10];
    uint16_t client_cfg;
}ble_current_time_var_t;

void ble_current_time_service_init(void);
void ble_current_time_service_proc(void);
void ble_current_time_service_notify(uint8_t *buf, uint16_t buf_size);
#endif
