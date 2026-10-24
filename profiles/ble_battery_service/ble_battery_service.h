#ifndef __BLE_BATTERY_SERVICE_H
#define __BLE_BATTERY_SERVICE_H
#include "include.h"

// Battery Service 0x180F
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL, DYNAMIC | READ,
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS, DYNAMIC | READ | NOTIFY,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE_START_HANDLE 0x0001
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE_END_HANDLE 0x0006

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL_STATUS_01_CLIENT_CONFIGURATION_HANDLE 0x0006

typedef struct{ 
    uint8_t level_sta[4];
    uint16_t level_sta_client_cfg;
}ble_bat_svc_var_t;

void ble_battery_service_init(void);
void ble_battery_service_proc(void);
void ble_battery_service_level_status_set(uint8_t *lv_sta);
void ble_battery_service_level_status_notify(uint8_t *buf, uint16_t buf_size);
#endif
