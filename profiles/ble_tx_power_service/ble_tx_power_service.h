#ifndef __BLE_TX_POWER_SERVICE_H
#define __BLE_TX_POWER_SERVICE_H
#include "include.h"

// Tx Power  0x1804
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_TX_POWER
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TX_POWER_LEVEL, DYNAMIC | READ,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_TX_POWER_START_HANDLE 0x00be
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_TX_POWER_END_HANDLE 0x00c0

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TX_POWER_LEVEL_01_VALUE_HANDLE 0x00c0

typedef struct{
    uint8_t level;
}ble_tx_power_var_t;

void ble_tx_power_service_init(void);
void ble_tx_power_service_proc(void);
#endif
