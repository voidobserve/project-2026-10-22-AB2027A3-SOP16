#ifndef __BLE_HID_CONTROLLER_SERVICE_H
#define __BLE_HID_CONTROLLER_SERVICE_H
#include "include.h"

#define HID_CTRL_INC            0x01
#define HID_CTRL_DEC            0x02
#define HID_CTRL_MENU           0x04
#define HID_CTRL_POWER          0x08
#define HID_CTRL_NEXT           0x10
#define HID_CTRL_PREV           0x20
#define HID_CTRL_HOME           0x40
#define HID_CTRL_PLAY_PAUSE     0x80

//
// list service handle ranges
//
#define ATT_SERVICE_1812_START_HANDLE 0x0001
#define ATT_SERVICE_1812_END_HANDLE 0x000b
#define ATT_SERVICE_1812_01_START_HANDLE 0x0001
#define ATT_SERVICE_1812_01_END_HANDLE 0x000b

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_2a4a_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_2a4d_01_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_2a4d_01_CLIENT_CONFIGURATION_HANDLE 0x0006
#define ATT_CHARACTERISTIC_2a4b_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_2a4c_01_VALUE_HANDLE 0x000b

void ble_hids_service_init(void);
void ble_hid_send_consumer_cmd(u16 cmd, u8 hold);
#endif
