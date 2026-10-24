#include "include.h"
#include "func.h"

#if FUNC_LE_BQB_RF_EN

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf_sub_process(void)
{
    // le_bqb_rf_app_proc();
}

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf_process(void)
{
    func_process();
    func_le_bqb_rf_sub_process();
}

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf_init(void)
{
    bsp_ble_init();
}

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf_enter(void)
{
    msg_queue_clear();
    lowpwr_pwroff_auto_dis();
    ble_set_work_mode(MODE_BQB_RF_BLE);
    func_le_bqb_rf_init();
}

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf_exit(void)
{
    bt_off();
    lowpwr_pwroff_auto_en();
    ble_set_work_mode(MODE_NORMAL);
    func_cb.last = FUNC_LE_BQB_RF;
}

AT(.text.func.le_bqb_rf)
void func_le_bqb_rf(void)
{
    printf("%s\n", __func__);

    func_le_bqb_rf_enter();

    while (func_cb.sta == FUNC_LE_BQB_RF) {
        func_le_bqb_rf_process();
        func_le_bqb_rf_message(msg_dequeue());
    }

    func_le_bqb_rf_exit();
}

#endif //FUNC_LE_BQB_RF_EN
