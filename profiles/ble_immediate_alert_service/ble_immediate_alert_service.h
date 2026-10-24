#ifndef __BLE_IAS_SERVICE_H
#define __BLE_IAS_SERVICE_H
#include "include.h"

// Immediate alert  0x1802
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL, DYNAMIC | WRITE_WITHOUT_RESPONSE,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT_START_HANDLE 0x007f
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_IMMEDIATE_ALERT_END_HANDLE 0x0081

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_01_VALUE_HANDLE 0x0081

typedef struct{
    uint8_t level_value[2];
}ble_ias_var_t;

void ble_ias_service_init(void);
void ble_ias_service_proc(void);
#endif
