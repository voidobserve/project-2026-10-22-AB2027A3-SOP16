#include "include.h"
#include "func.h"

#if FUNC_BT_EN

AT(.text.app.proc.bt)
void func_bt_sub_process(void)
{
    ble_app_proc();
}

AT(.text.app.proc.bt)
void func_bt_process(void)
{
    func_process();
    func_bt_sub_process();

#if FUNC_TBOX_EN
    if (is_prod_test_tbox_mode()) {
        func_cb.sta = FUNC_TBOX;
        return;
    }
#endif

#if SYS_OFF_TIME
    if(sys_cb.pwroff_delay == 0) {
        func_cb.sta = FUNC_PWROFF;
        return;
    }
#endif

#if SYS_SLEEP_EN
    if(sleep_process(bt_is_sleep)) {

    }
#endif
}

AT(.text.func.bt)
void func_bt_init(void)
{
    bsp_ble_init();
}

AT(.text.func.bt)
void func_bt_enter(void)
{
    msg_queue_clear();

#if LE_FCC_TEST_EN
    ble_set_work_mode(MODE_FCC_TEST);
#elif LE_BQB_RF_EN
    ble_set_work_mode(MODE_BQB_RF_BLE);
#else
    ble_set_work_mode(MODE_NORMAL);
#endif

    func_bt_init();
}

AT(.text.func.bt)
void func_bt_exit(void)
{
#if FUNC_TBOX_EN
    if (is_prod_test_tbox_mode() == 0)
#endif //FUNC_TBOX_EN
    {
        bt_off();
    }

    ble_set_work_mode(MODE_NORMAL);

    func_cb.last = FUNC_BT;
}

AT(.text.func.bt)
void func_bt(void)
{
    printf("%s\n", __func__);

    func_bt_enter();

    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }

    func_bt_exit();
}

#endif //FUNC_BT_EN
