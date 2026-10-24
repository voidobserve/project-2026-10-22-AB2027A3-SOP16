#ifndef __BLE_SERVICE_H
#define __BLE_SERVICE_H

#define CCCD_DFT                    0           //定义client config的默认值,对于有些应用，主机不会进行订阅的情况，可以设置成1

//
// list service handle ranges
//
#define ATT_SERVICE_FF12_START_HANDLE 0x0001
#define ATT_SERVICE_FF12_END_HANDLE 0x0006
#define ATT_SERVICE_FF00_START_HANDLE 0x0007
#define ATT_SERVICE_FF00_END_HANDLE 0x000c

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_FF15_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_FF14_01_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_FF14_01_CLIENT_CONFIGURATION_HANDLE 0x0006
#define ATT_CHARACTERISTIC_FF01_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_FF02_01_VALUE_HANDLE 0x000b
#define ATT_CHARACTERISTIC_FF02_01_CLIENT_CONFIGURATION_HANDLE 0x000c

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size);
u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size);
const u8 *ble_get_profile_data(void);
void ble_service_init(void);


#endif
