/*
 * 文件说明：
 * 把 2.4G 遥控器的 PRESS 事件转换成 led_app 应用命令。
 * 本文件不直接操作动画、协议或 GPIO。
 */

#include "rf24g_remoter_apply.h"
#include "rf24g_remoter_driver.h"
#include "led_app.h"

extern const rf24_key_handle_func_t
    rf24g_key_event_handle_table[RF24G_KEY_EVENT_MAX - 1];

/**
 * @brief 2.4g遥控器应用层
 */
void rf24g_remoter_apply(void)
{
    uint8_t rf24g_key_event;
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = NULL;

    rf24g_key_event = rf24g_convert_key_event(rf24g_scan_para.latest_key_val,
                                              rf24g_scan_para.latest_key_event);
    rf24g_scan_para.latest_key_val = NO_KEY; // 获取到按键事件后，清空按键值
    if (RF24G_KEY_EVENT_NONE == rf24g_key_event) {
        return;
    }

    rf24g_key_handle_func_ptr = rf24g_key_event_handle_table[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) {
        return;
    }

    rf24g_key_handle_func_ptr();
}

/* 第一行：开关 */
void rf24g_key_event_r1c1_press_handle(void)
{
    led_app_power_toggle(led_app_get_current_time_ms());
}

/* 第二行：Fade / Dimming / Strobe */
void rf24g_key_event_r2c1_press_handle(void)
{
    led_app_set_mode(LED_ANIMATION_MODE_FADE, led_app_get_current_time_ms());
}

void rf24g_key_event_r2c2_press_handle(void)
{
    led_app_brightness_next();
}

void rf24g_key_event_r2c3_press_handle(void)
{
    led_app_set_mode(LED_ANIMATION_MODE_STROBE, led_app_get_current_time_ms());
}

/* Color1 / Color2：本型号无效果，保留空处理 */
void rf24g_key_event_r3c1_press_handle(void)
{
}

void rf24g_key_event_r4c1_press_handle(void)
{
}

/* 第三行：Modes / 2h timer */
void rf24g_key_event_r3c2_press_handle(void)
{
    led_app_modes_next(led_app_get_current_time_ms());
}

void rf24g_key_event_r3c3_press_handle(void)
{
    led_app_timer_start(APP_TIMER_2H, led_app_get_current_time_ms());
}

/* 第四行：图标 / 4h timer */
void rf24g_key_event_r4c2_press_handle(void)
{
    led_app_set_icon((uint8_t)LED_ICON_US_FLAG,
                      led_app_get_current_time_ms());
}

void rf24g_key_event_r4c3_press_handle(void)
{
    led_app_timer_start(APP_TIMER_4H, led_app_get_current_time_ms());
}

/* 第五行：图标 */
void rf24g_key_event_r5c1_press_handle(void)
{
    led_app_set_icon((uint8_t)LED_ICON_CLOVER,
                      led_app_get_current_time_ms());
}

void rf24g_key_event_r5c2_press_handle(void)
{
    led_app_set_icon((uint8_t)LED_ICON_PUMPKIN,
                      led_app_get_current_time_ms());
}

void rf24g_key_event_r5c3_press_handle(void)
{
    led_app_set_icon((uint8_t)LED_ICON_CHRISTMAS,
                      led_app_get_current_time_ms());
}

/* 第六行：橙 / 绿 / 蓝 / 红 */
void rf24g_key_event_r6c1_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_ORANGE,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r6c2_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_GREEN_203,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r6c3_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_BLUE_203,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r6c4_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_RED,
                             led_app_get_current_time_ms());
}

/* 第七行：琥珀 / 青绿 / 紫 / 纯绿 */
void rf24g_key_event_r7c1_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_AMBER,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r7c2_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_TEAL_GREEN,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r7c3_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_PURPLE,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r7c4_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_PURE_GREEN,
                             led_app_get_current_time_ms());
}

/* 第八行：米色 / 蓝绿 / 紫红 / 纯蓝 */
void rf24g_key_event_r8c1_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_BEIGE,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r8c2_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_BLUE_GREEN,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r8c3_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_MAGENTA,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r8c4_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_PURE_BLUE,
                             led_app_get_current_time_ms());
}

/* 第九行：暖白 / 冷白 / 品红 / White Select */
void rf24g_key_event_r9c1_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_WARM_WHITE,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r9c2_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_COOL_WHITE,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r9c3_press_handle(void)
{
    led_app_set_static_color((uint8_t)LED_STATIC_FUCHSIA,
                             led_app_get_current_time_ms());
}

void rf24g_key_event_r9c4_press_handle(void)
{
    led_app_white_color_next(led_app_get_current_time_ms());
}

/*
 * @brief 按键事件处理函数表
 * RF24G_KEY_EVENT_MAX - 1: 有效的按键事件数量，这里减一代表减去了 RF24G_KEY_EVENT_NONE
 */
const rf24_key_handle_func_t
    rf24g_key_event_handle_table[RF24G_KEY_EVENT_MAX - 1] = {
        [RF24G_KEY_EVENT_R1C1_PRESS] = rf24g_key_event_r1c1_press_handle,

        [RF24G_KEY_EVENT_R2C1_PRESS] = rf24g_key_event_r2c1_press_handle,
        [RF24G_KEY_EVENT_R2C2_PRESS] = rf24g_key_event_r2c2_press_handle,
        [RF24G_KEY_EVENT_R2C3_PRESS] = rf24g_key_event_r2c3_press_handle,

        [RF24G_KEY_EVENT_R3C1_PRESS] = rf24g_key_event_r3c1_press_handle,
        [RF24G_KEY_EVENT_R3C2_PRESS] = rf24g_key_event_r3c2_press_handle,
        [RF24G_KEY_EVENT_R3C3_PRESS] = rf24g_key_event_r3c3_press_handle,

        [RF24G_KEY_EVENT_R4C1_PRESS] = rf24g_key_event_r4c1_press_handle,
        [RF24G_KEY_EVENT_R4C2_PRESS] = rf24g_key_event_r4c2_press_handle,
        [RF24G_KEY_EVENT_R4C3_PRESS] = rf24g_key_event_r4c3_press_handle,

        [RF24G_KEY_EVENT_R5C1_PRESS] = rf24g_key_event_r5c1_press_handle,
        [RF24G_KEY_EVENT_R5C2_PRESS] = rf24g_key_event_r5c2_press_handle,
        [RF24G_KEY_EVENT_R5C3_PRESS] = rf24g_key_event_r5c3_press_handle,

        [RF24G_KEY_EVENT_R6C1_PRESS] = rf24g_key_event_r6c1_press_handle,
        [RF24G_KEY_EVENT_R6C2_PRESS] = rf24g_key_event_r6c2_press_handle,
        [RF24G_KEY_EVENT_R6C3_PRESS] = rf24g_key_event_r6c3_press_handle,
        [RF24G_KEY_EVENT_R6C4_PRESS] = rf24g_key_event_r6c4_press_handle,

        [RF24G_KEY_EVENT_R7C1_PRESS] = rf24g_key_event_r7c1_press_handle,
        [RF24G_KEY_EVENT_R7C2_PRESS] = rf24g_key_event_r7c2_press_handle,
        [RF24G_KEY_EVENT_R7C3_PRESS] = rf24g_key_event_r7c3_press_handle,
        [RF24G_KEY_EVENT_R7C4_PRESS] = rf24g_key_event_r7c4_press_handle,

        [RF24G_KEY_EVENT_R8C1_PRESS] = rf24g_key_event_r8c1_press_handle,
        [RF24G_KEY_EVENT_R8C2_PRESS] = rf24g_key_event_r8c2_press_handle,
        [RF24G_KEY_EVENT_R8C3_PRESS] = rf24g_key_event_r8c3_press_handle,
        [RF24G_KEY_EVENT_R8C4_PRESS] = rf24g_key_event_r8c4_press_handle,

        [RF24G_KEY_EVENT_R9C1_PRESS] = rf24g_key_event_r9c1_press_handle,
        [RF24G_KEY_EVENT_R9C2_PRESS] = rf24g_key_event_r9c2_press_handle,
        [RF24G_KEY_EVENT_R9C3_PRESS] = rf24g_key_event_r9c3_press_handle,
        [RF24G_KEY_EVENT_R9C4_PRESS] = rf24g_key_event_r9c4_press_handle,
};
