#ifndef __LED_APP_H__
#define __LED_APP_H__

/*
 * 文件说明：
 * 应用状态层接口。遥控器和机械按键只调用这里的命令，
 * 由本层统一处理电源、亮度、模式、Timer 和 NVM。
 */

#include <stdint.h>
#include <stdbool.h>

#include "led_driver/led_animation.h"

/* 关灯状态标志，供 TMR3 ISR 判断是否释放 driver 信号量 */
extern volatile uint8_t led_app_off_interrupt_flag;

/* 2h/4h timer 类型 */
typedef enum {
    APP_TIMER_2H = 0,
    APP_TIMER_4H
} app_timer_t;

/* 初始化应用层：读取 NVM、恢复模式/颜色/开关、初始化动画 */
void led_app_init(uint32_t now_ms);

/* 每帧轮询：timer 倒计时、NVM 延迟保存；关灯状态也要调用 */
void led_app_poll(uint32_t now_ms);

/* 更新应用层当前 1 ms 时间，供遥控 handler 使用 */
void led_app_set_current_time_ms(uint32_t now_ms);
uint32_t led_app_get_current_time_ms(void);

/* 电源 */
bool led_app_power_is_on(void);
void led_app_power_toggle(uint32_t now_ms);
/* 机械按键开灯入口；应在 LED driver 线程/用户主循环中调用 */
void led_app_power_on_external(uint32_t now_ms);

/* 模式/颜色 */
void led_app_set_mode(led_animation_mode_t mode, uint32_t now_ms);
void led_app_set_static_color(uint8_t color_index, uint32_t now_ms);
void led_app_set_icon(uint8_t icon, uint32_t now_ms);
void led_app_modes_next(uint32_t now_ms);
void led_app_white_color_next(uint32_t now_ms);

/* 亮度：5 挡循环 */
void led_app_brightness_next(void);

/* 2h/4h timer */
void led_app_timer_start(app_timer_t timer_type, uint32_t now_ms);

/* 逐灯取最终输出（已乘亮度、已应用 timer 覆盖） */
void led_app_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                            led_color_t *output_color);

#endif
