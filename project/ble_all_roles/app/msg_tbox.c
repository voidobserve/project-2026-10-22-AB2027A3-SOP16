#include "include.h"

#if FUNC_TBOX_EN

#if PROD_TEST_KEY_EN
#include "tbox_key.h"
#endif //PROD_TEST_KEY_EN

//tbox message process
AT(.text.func.msg)
static void tbox_message(u16 msg)
{
    switch (msg) {

#if (BSP_KEY_EN && PROD_TEST_KEY_EN)
        case KEY_ID_K1 | KEY_SHORT:                        // K1按键 向上按键测试
            printf("K1 key_up short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_UP);
            break;
        case KEY_ID_K2 | KEY_SHORT:                        // K2按键 向下按键测试
            printf("K2 key_down short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_DOWN);
            break;
        case KEY_ID_K3 | KEY_SHORT:                        // K3按键 向左按键测试
            printf("K3 key_left short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_LEFT);
            break;
        case KEY_ID_K4 | KEY_SHORT:                        // K4按键 向右按键测试
            printf("K4 key_right short key\n");
            tbox_set_key_msg(TBOX_KEY_MSG_RIGHT);
            break;

        default: {
            uint8_t key_id = msg & 0xff;
            uint16_t key_up = msg & 0xff00;
             if ((key_id <= KEY_ID_K4) && (key_id > KEY_ID_NO)) {
                if ((key_up == KEY_SHORT_UP) || (key_up == KEY_LONG_UP)) {
                    // printf("key up\n");
                    tbox_set_key_msg(TBOX_KEY_MSG_RELEASE);
                }
            }
        } break;
#else

        default:
            break;
#endif
    }
}

AT(.text.func.msg)
static void func_tbox_message_do(u16 msg)
{
    switch (msg) {
        case MSG_SYS_1S:
            //printf("MSG_SYS_1S\n");
        default:
            tbox_message(msg);
            break;
    }
}

AT(.text.app.proc.msg)
void func_tbox_message(u16 msg)
{
    if(msg){
        func_tbox_message_do(msg);
    }
}
#endif  //FUNC_TBOX_EN
