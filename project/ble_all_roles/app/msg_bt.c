#include "include.h"

AT(.text.func.msg)
void func_bt_message_do(u16 msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            //printf("MSG_SYS_1S\n");
            break;

        default:
            func_message(msg);
            break;
    }
}


AT(.text.app.proc.msg)
void func_bt_message(u16 msg)
{
    if(msg){
        func_bt_message_do(msg);
    }
}
