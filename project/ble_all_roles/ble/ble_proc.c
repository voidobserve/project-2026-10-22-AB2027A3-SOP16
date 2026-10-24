#include "include.h"
#include "ble_user_service.h"
#include "ble_fota_service.h"
#include "ble_app.h"

#define BLE_CON_SLEEP_INTERVAL    400   //500ms
#define BLE_ADV_SLEEP_INTERVAL    800   //500ms
#define BLE_CON_SLEEP_LATENCY     8
#define BLE_CON_SLEEP_TIMEOUT     1000   //10s

static struct ble_param_t {
    uint16_t adv_interval;
    uint16_t conn_interval[MAX_BLE_CONNECTIONS];
    uint16_t conn_latency[MAX_BLE_CONNECTIONS];
    uint16_t conn_timeout[MAX_BLE_CONNECTIONS];
    uint16_t conhdl[MAX_BLE_CONNECTIONS];
} ble_param;

static void ble_update_param_before_enter_sleep(void)
{
    ble_cb_t *p_info = ble_get_ble_info();

    for (uint8_t i = 0; i < MAX_BLE_CONNECTIONS; i++) {
        if (p_info[i].conhdl) {
            //uint16_t timeout;
            ble_enable_latency(p_info[i].conhdl);
            ble_get_conn_param(p_info[i].conhdl, &ble_param.conn_interval[i], &ble_param.conn_latency[i], &ble_param.conn_timeout[i]);
            //timeout = 25 * ble_param.conn_interval * BLE_CON_SLEEP_LATENCY / 20 ;
            bool ret = ble_update_conn_param(p_info[i].conhdl, BLE_CON_SLEEP_INTERVAL, BLE_CON_SLEEP_LATENCY, BLE_CON_SLEEP_TIMEOUT);
            printf("ret %d, conhdl %d, intv = %x, latency %x\n", ret, p_info[i].conhdl, ble_param.conn_interval[i], ble_param.conn_latency[i]);

            delay_ms(2);
        }
    }

    uint16_t slave_cnt = ble_get_conn_cnt(ROLE_SLAVE);

    if (slave_cnt < MAX_SLAVE_NR) {
        ble_get_adv_interval(&ble_param.adv_interval);
        ble_set_adv_interval(BLE_ADV_SLEEP_INTERVAL);
    }
}

static void ble_update_param_after_exit_sleep(void)
{
    ble_cb_t *p_info = ble_get_ble_info();

    for (uint8_t i = 0; i < MAX_BLE_CONNECTIONS; i++) {
        if (p_info[i].conhdl) {
            ble_disable_latency(p_info[i].conhdl);

            bool ret = ble_update_conn_param(p_info[i].conhdl, ble_param.conn_interval[i], ble_param.conn_latency[i], ble_param.conn_timeout[i]);
            printf("ret %d, conhdl %d, intv = %x, latency %x\n", ret, p_info[i].conhdl, ble_param.conn_interval[i], ble_param.conn_latency[i]);

            delay_ms(2);
        }
    }

    uint16_t slave_cnt = ble_get_conn_cnt(ROLE_SLAVE);

    if (slave_cnt < MAX_SLAVE_NR) {
        ble_set_adv_interval(ble_param.adv_interval);
    }
}

void ble_enter_sleep_proc(void)
{
    ble_scan_dis();

    delay_ms(2);

    ble_update_param_before_enter_sleep();
}

void ble_exit_sleep_proc(void)
{
    ble_adv_scan_state_correct();

    ble_update_param_after_exit_sleep();
}

AT(.text.app.proc.ble)
void ble_app_proc(void)
{
    ble_user_service_proc();

#if LE_FOTA_EN
    ble_fota_process();
#endif
}

AT(.com_sleep.ble.sleep)
bool ble_proc_pending(void)
{
    bool res = false;

#if LE_FOTA_EN
    res = ble_fota_is_start();
#endif

    if(!res){
        res = ble_user_service_pending();
    }

    return res;
}
