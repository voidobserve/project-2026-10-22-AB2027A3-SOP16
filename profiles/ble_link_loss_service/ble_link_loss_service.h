#ifndef __BLE_LINK_LOSS_SERVICE_H
#define __BLE_LINK_LOSS_SERVICE_H
#include "include.h"

// Link Loss  0x1803
// PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_LINK_LOSS
// CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL, DYNAMIC | READ | WRITE,

//
// list service handle ranges
//
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_LINK_LOSS_START_HANDLE 0x0088
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_LINK_LOSS_END_HANDLE 0x008a

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_ALERT_LEVEL_02_VALUE_HANDLE 0x008a

typedef struct{
    uint8_t alert_level;
}ble_link_loss_var_t;

void ble_link_loss_service_init(void);
void ble_link_loss_service_proc(void);
#endif
