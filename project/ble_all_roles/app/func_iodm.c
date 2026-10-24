#include "include.h"
#include "func.h"

#if FUNC_IODM_EN

AT(.text.func.iodm)
void func_iodm_sub_process(void)
{
    // iodm_app_proc();
}

AT(.text.func.iodm)
void func_iodm_process(void)
{
    func_process();
    func_iodm_sub_process();
}

AT(.text.func.iodm)
void func_iodm_init(void)
{
    // bsp_ble_init();
}

AT(.text.func.iodm)
void func_iodm_enter(void)
{
    msg_queue_clear();
    lowpwr_pwroff_auto_dis();
    ble_set_work_mode(MODE_IODM_TEST);
    func_iodm_init();
}

AT(.text.func.iodm)
void func_iodm_exit(void)
{
    bt_off();
    lowpwr_pwroff_auto_en();
    ble_set_work_mode(MODE_NORMAL);
    func_cb.last = FUNC_IODM;
}

AT(.text.func.iodm)
void func_iodm(void)
{
    printf("%s\n", __func__);

    func_iodm_enter();

    while (func_cb.sta == FUNC_IODM) {
        func_iodm_process();
        func_iodm_message(msg_dequeue());
    }

    func_iodm_exit();
}

#endif //FUNC_IODM_EN
