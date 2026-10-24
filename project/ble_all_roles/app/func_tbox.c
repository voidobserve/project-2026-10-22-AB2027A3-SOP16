#include "include.h"
#include "func.h"
#include "func_tbox.h"
#include "prod_test_tbox.h"

#if FUNC_TBOX_EN

AT(.text.app.proc.tbox)
void func_tbox_process(void)
{
    func_process();
}

AT(.text.func.tbox)
void func_tbox_init(void)
{

}

AT(.text.func.tbox)
void func_tbox_enter(void)
{
#if SYS_SLEEP_TIME
    lowpwr_sleep_auto_dis();
#endif

#if SYS_OFF_TIME
    lowpwr_pwroff_auto_dis();
#endif

    msg_queue_clear();
    func_tbox_init();
}

AT(.text.func.tbox)
void func_tbox_exit(void)
{
#if SYS_SLEEP_TIME
    lowpwr_sleep_auto_en();
#endif

#if SYS_OFF_TIME
    lowpwr_pwroff_auto_en();
#endif

    func_cb.last = FUNC_TBOX;
}

AT(.text.func.tbox)
void func_tbox(void)
{
    printf("%s\n", __func__);

    func_tbox_enter();

    while (func_cb.sta == FUNC_TBOX) {
        func_tbox_process();
        func_tbox_message(msg_dequeue());
    }

    func_tbox_exit();
}

#endif //FUNC_TBOX_EN
