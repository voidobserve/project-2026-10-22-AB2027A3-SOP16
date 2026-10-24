#include "include.h"
#include "driver_com.h"
#include "bsp_saradc_key.h"
#include "bsp_io_key.h"

#if BSP_KEY_EN

static key_status_typedef key_status;

AT(.com_rodata.key)
u16 allow_multiple_key_table[] = {
    KEY_ID_PP | KEY_SHORT_UP,
    KEY_ID_K1 | KEY_SHORT_UP,
    KEY_ID_K2 | KEY_SHORT_UP,
    KEY_ID_K3 | KEY_SHORT_UP,
};


// 处理短按、长按、连按
AT(.com_text.key.process)
u16 key_process(u16 key_id)
{
    u16 key_return = KEY_ID_NO;

    if (key_id == KEY_ID_NO || key_id != key_status.key_id) {               // 抬键消抖
        if (key_status.release_debounce_cnt < KEY_UP_TIMES) {
            key_status.release_debounce_cnt++;
        } else {
            if (key_status.key_press_cnt >= KEY_LONG_TIMES) {               // 长按抬键
                key_return = key_status.key_id | KEY_LONG_UP;
            } else if (key_status.key_press_cnt >= KEY_SCAN_TIMES) {        // 短按抬键
                key_return = key_status.key_id | KEY_SHORT_UP;
            }

            key_status.key_id = key_id;
            key_status.key_press_cnt = 0;
        }
    } else {
        key_status.key_press_cnt++;
        if (key_status.key_press_cnt == KEY_SCAN_TIMES) {               // 去抖
            key_status.release_debounce_cnt = 0;
            key_return = key_status.key_id;
        } else if (key_status.key_press_cnt == KEY_LONG_TIMES) {        // 长按
            key_return = key_status.key_id | KEY_LONG;
        } else if (key_status.key_press_cnt == KEY_LONG_HOLD_TIMES) {   // 连按
            key_return = key_status.key_id | KEY_HOLD;
            key_status.key_press_cnt = KEY_LONG_TIMES;
        }
    }

    return key_return;
}

// mode: 0-事件检测  1-状态检测
AT(.com_text.key.scan)
u16 bsp_key_scan(u8 mode)
{
    u16 key, key_id = KEY_ID_NO;

#if BSP_IOKEY_EN
    key_id = bsp_get_io_key_id();
#endif // BSP_IOKEY_EN

#if BSP_ADKEY_EN
    if (key_id == KEY_ID_NO) {
        key_id = bsp_get_adkey_id();            // 根据ADC采样值返回键值
    }
#endif // BSP_ADKEY_EN

#if BSP_TOUCH_KEY_EN
    if (key_id == KEY_ID_NO) {
        key_id = bsp_get_touch_key_id();
    }
#endif // BSP_TOUCH_KEY_EN

    if (mode) {
        return key_id;
    }

    if (bsp_key_get_power_on_flag()) {
        if (key_id == KEY_ID_PP) {
            key_id = KEY_ID_NO;
        } else {
            bsp_key_set_power_on_flag(false);
        }
    }

    key = key_process(key_id);              // 短按、长按、连按处理
    key = key_multi_press_process(key);     // 多击处理

    if(key){
        msg_enqueue(key);
    }
    return key;
}

void bsp_key_init(void)
{
    memset(&key_status, 0, sizeof(key_status));

    multiple_key_init(100, allow_multiple_key_table, sizeof(allow_multiple_key_table) / sizeof(u16));

#if BSP_ADKEY_EN
    bsp_adkey_init();
#endif // BSP_ADKEY_EN

#if BSP_IOKEY_EN
    bsp_io_key_init();
#endif // BSP_IOKEY_EN

#if BSP_TOUCH_KEY_EN
    bsp_touch_key_init();
#endif // BSP_TOUCH_KEY_EN
}

AT(.com_text.key.set.power_on)
void bsp_key_set_power_on_flag(bool flag)
{
    key_status.power_on_flag = flag;
}

AT(.com_text.key.get.power_on)
bool bsp_key_get_power_on_flag(void)
{
    return key_status.power_on_flag;
}

#endif // BSP_KEY_EN
