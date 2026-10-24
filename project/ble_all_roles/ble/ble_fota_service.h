#ifndef __BLE_FOTA_SERVICE_H
#define __BLE_FOTA_SERVICE_H

/*
PRIMARY_SERVICE, FF12
CHARACTERISTIC, FF15, READ | WRITE_WITHOUT_RESPONSE | DYNAMIC,
CHARACTERISTIC, FF14, NOTIFY,
*/


void ble_fota_service_init(void);
void ble_fota_process(void);
void ble_fota_pause(void);
void ble_fota_continue(void);
u8 ble_fota_is_start(void);


#endif
