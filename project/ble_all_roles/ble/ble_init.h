#ifndef __BLE_INIT_H
#define __BLE_INIT_H

#define INVALID_CONIDX                  (0xff)

#define MAX_MASTER_NR                       2
#define MAX_SLAVE_NR                        1

#define MAX_BLE_CONNECTIONS                 (MAX_MASTER_NR + MAX_SLAVE_NR)

#if (MAX_BLE_CONNECTIONS > 3)
    #error "MAX_BLE_CONNECTIONS must must less than 3"
#endif


typedef struct _ble_cb_t {
    ble_con_handle_t conhdl;

    // Connection role.
    uint8_t role;
} ble_cb_t;

extern ble_cb_t ble_cb[MAX_BLE_CONNECTIONS];

void bsp_ble_init(void);
void ble_set_work_mode(BT_WORK_MODE mode);
BT_WORK_MODE ble_get_work_mode(void);
ble_cb_t *ble_get_ble_info(void);
uint8_t ble_get_free_conidx(uint16_t conhdl);
uint8_t ble_get_conidx(uint16_t conhdl);
uint8_t ble_get_role(uint8_t conidx);

#endif
