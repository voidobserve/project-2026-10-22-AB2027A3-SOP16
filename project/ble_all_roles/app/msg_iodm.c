#include "include.h"

AT(.text.func.iodm.msg)
void func_iodm_message(uint16_t msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            //printf("MSG_SYS_1S\n");
        default:
            func_message(msg);
            break;
    }
}
