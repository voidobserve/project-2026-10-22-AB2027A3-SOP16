#include "include.h"
#include "func.h"

#if FUNC_LE_FCC_EN

AT(.text.func.le_fcc)
void func_le_fcc_sub_process(void)
{
    // le_fcc_app_proc();
}

AT(.text.func.le_fcc)
void func_le_fcc_process(void)
{
    func_process();
    func_le_fcc_sub_process();
}

AT(.text.func.le_fcc)
void func_le_fcc_init(void)
{
    bsp_ble_init();
}

AT(.text.func.le_fcc)
void func_le_fcc_enter(void)
{
    msg_queue_clear();
    lowpwr_pwroff_auto_dis();
    ble_set_work_mode(MODE_FCC_TEST);
    func_le_fcc_init();
}

AT(.text.func.le_fcc)
void func_le_fcc_exit(void)
{
    bt_off();
    lowpwr_pwroff_auto_en();
    ble_set_work_mode(MODE_NORMAL);
    func_cb.last = FUNC_LE_FCC;
}

AT(.text.func.le_fcc)
void func_le_fcc(void)
{
    printf("%s\n", __func__);

    func_le_fcc_enter();

    while (func_cb.sta == FUNC_LE_FCC) {
        func_le_fcc_process();
        func_le_fcc_message(msg_dequeue());
    }

    func_le_fcc_exit();
}

#endif //FUNC_LE_FCC_EN
