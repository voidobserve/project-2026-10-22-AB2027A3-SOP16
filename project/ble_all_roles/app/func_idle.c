#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN

AT(.text.func.idle)
void func_idle_process(void)
{
    func_process();
}

static void func_idle_enter(void)
{

}

static void func_idle_exit(void)
{
    func_cb.last = FUNC_IDLE;
}

AT(.text.func.idle)
void func_idle(void)
{
    printf("%s\n", __func__);

    func_idle_enter();

    while (func_cb.sta == FUNC_IDLE) {
        func_idle_process();
        func_idle_message(msg_dequeue());
    }

    func_idle_exit();
}

#endif
