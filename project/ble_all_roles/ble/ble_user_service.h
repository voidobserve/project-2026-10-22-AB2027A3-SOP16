#ifndef __BLE_USER_SERVICE_H
#define __BLE_USER_SERVICE_H

/*
PRIMARY_SERVICE, FF00
CHARACTERISTIC, FF01, READ | WRITE | DYNAMIC,
CHARACTERISTIC, FF02, NOTIFY,
*/

void ble_user_service_init(void);
void ble_user_service_proc(void);
bool ble_user_service_pending(void);

#endif
