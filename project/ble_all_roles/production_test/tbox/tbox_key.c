#include "include.h"
#include "tbox_key.h"
#include "prod_test_tbox.h"

#if (FUNC_TBOX_EN && PROD_TEST_KEY_EN)

AT(.text.prod_test.tbox_key_ntf)
static int16_t tbox_key_notify_for_handle(uint16_t att_handle, uint8_t *buf, uint16_t buf_size)
{
    if (!tbox_var.key_client_config) {
        printf("[err] %s, clien config err!\n", __func__);
        return -1;
    }

    int16_t ret = ble_notify_for_handle(tbox_var.conn_handle, att_handle, buf, buf_size);
    return ret;
}

AT(.text.prod_test.tbox_key_ntf)
static void tbox_key_notify_do(uint8_t tbox_key_msg)
{
    uint8_t key_cmd = TBOX_KEY_NONE_CMD;

    switch (tbox_key_msg) {
        case TBOX_KEY_MSG_RELEASE:
            key_cmd = TBOX_KEY_RELEASE_CMD;
            break;
        case TBOX_KEY_MSG_UP:
            key_cmd = TBOX_KEY_UP_CMD;
            break;
        case TBOX_KEY_MSG_DOWN:
            key_cmd = TBOX_KEY_DOWN_CMD;
            break;
        case TBOX_KEY_MSG_LEFT:
            key_cmd = TBOX_KEY_LEFT_CMD;
            break;
        case TBOX_KEY_MSG_RIGHT:
            key_cmd = TBOX_KEY_RIGHT_CMD;
            break;
        case TBOX_KEY_MSG_MEUN:
            key_cmd = TBOX_KEY_MEUN_CMD;
            break;
        case TBOX_KEY_MSG_HOME:
            key_cmd = TBOX_KEY_HOME_CMD;
            break;
        case TBOX_KEY_MSG_OK:
            key_cmd = TBOX_KEY_OK_CMD;
            break;
        case TBOX_KEY_MSG_BACK:
            key_cmd = TBOX_KEY_BACK_CMD;
            break;
        case TBOX_KEY_MSG_VOL_UP:
            key_cmd = TBOX_KEY_VOL_UP_CMD;
            break;
        case TBOX_KEY_MSG_VOL_DOWN:
            key_cmd = TBOX_KEY_VOL_DOWN_CMD;
            break;
        case TBOX_KEY_MSG_POWER:
            key_cmd = TBOX_KEY_PWR_CMD;
            break;
            
#if (PROD_TEST_MIC_EN)
        case TBOX_KEY_MSG_VOICE_KEY_TEST:
            key_cmd = TBOX_KEY_VOICE_KEY_CMD;
            break;
        case TBOX_KEY_MSG_VOICE_ON_TEST:
            key_cmd = TBOX_KEY_VOICE_ON_CMD;
            break;
        case TBOX_KEY_MSG_VOICE_OFF_TEST:
            key_cmd = TBOX_KEY_VOICE_OFF_CMD;
            break;
#endif  //(PROD_TEST_MIC_EN)

        default:
            break;
    }

    if (key_cmd != TBOX_KEY_NONE_CMD) {
#if (PROD_TEST_MIC_EN)
        //保留开关语音测试消息，最后由语音那边清除
        if ((key_cmd != TBOX_KEY_VOICE_ON_CMD) && (key_cmd != TBOX_KEY_VOICE_OFF_CMD)) 
#endif  //(PROD_TEST_MIC_EN)
        {
            tbox_set_key_msg(TBOX_KEY_MSG_NONE);
        }
        tbox_key_notify_for_handle(ATT_CHARACTERISTIC_8001_01_VALUE_HANDLE, &key_cmd, sizeof(key_cmd));
    }
}

AT(.text.app.proc.tbox_key)
void tbox_key_proc(void)
{
    if (tbox_var.conn_handle == 0) {
        return;
    }

    uint8_t tbox_key_msg = tbox_get_key_msg();
    if (tbox_key_msg != TBOX_KEY_MSG_NONE){
        tbox_key_notify_do(tbox_key_msg);
    }

}

#endif //(FUNC_TBOX_EN && PROD_TEST_KEY_EN)
