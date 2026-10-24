#include "include.h"

AT(.text.func.le_fcc.msg)
void func_le_fcc_message(uint16_t msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            //printf("MSG_SYS_1S\n");
        default:
            func_message(msg);
            break;
    }
}
