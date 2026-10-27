#ifndef __LED_ANIMATION_H__
#define __LED_ANIMATION_H__

/*
 * 文件说明：
 * 动画层接口。输入当前 1 ms 时间和灯号，输出该灯的 100% 基准 RGBW。
 * 不处理协议组帧、SPI 发送、按键和 NVM。
 */

#include <stdint.h>
#include <stdbool.h>

#include "led_protocol.h"

/* 动画/模式类型 */
typedef enum {
    LED_ANIMATION_MODE_OFF = 0,
    LED_ANIMATION_MODE_STATIC_COLOR,
    LED_ANIMATION_MODE_FADE,
    LED_ANIMATION_MODE_STROBE,
    LED_ANIMATION_MODE_MODES,
    LED_ANIMATION_MODE_ICON,
    LED_ANIMATION_MODE_WHITE_SELECT,
} led_animation_mode_t;

/* 15 种静态单色，顺序与 NVM 的 color 字段一致 */
typedef enum {
    LED_STATIC_ORANGE = 0,
    LED_STATIC_GREEN_203,
    LED_STATIC_BLUE_203,
    LED_STATIC_RED,
    LED_STATIC_AMBER,
    LED_STATIC_TEAL_GREEN,
    LED_STATIC_PURPLE,
    LED_STATIC_PURE_GREEN,
    LED_STATIC_BEIGE,
    LED_STATIC_BLUE_GREEN,
    LED_STATIC_MAGENTA,
    LED_STATIC_PURE_BLUE,
    LED_STATIC_WARM_WHITE,
    LED_STATIC_COOL_WHITE,
    LED_STATIC_FUCHSIA,
    LED_STATIC_COLOR_COUNT
} led_static_color_id_t;

/* 4 种静态图标 */
typedef enum {
    LED_ICON_US_FLAG = 0,
    LED_ICON_CLOVER,
    LED_ICON_PUMPKIN,
    LED_ICON_CHRISTMAS,
    LED_ICON_COUNT
} led_icon_id_t;

/* White Select 的 3 种颜色 */
typedef enum {
    LED_WHITE_WHITE = 0,
    LED_WHITE_COOL,
    LED_WHITE_LIGHT_BLUE,
    LED_WHITE_COUNT
} led_white_id_t;

/* Modes 状态：0 彩虹，1~7 A~G，8 H */
#define LED_MODES_RAINBOW 0U
#define LED_MODES_A 1U
#define LED_MODES_B 2U
#define LED_MODES_C 3U
#define LED_MODES_D 4U
#define LED_MODES_E 5U
#define LED_MODES_F 6U
#define LED_MODES_G 7U
#define LED_MODES_H 8U
#define LED_MODES_STATE_COUNT 9U

/* 初始化动画层 */
void led_animation_init(uint32_t now_ms);

/* 切换顶层动画/模式；只重置起点，不改协议灯号游标 */
void led_animation_set_mode(led_animation_mode_t mode, uint32_t now_ms);

/* 当前顶层模式 */
led_animation_mode_t led_animation_get_mode(void);

/* 设置静态单色索引，索引会限幅 */
void led_animation_set_static_color_index(uint8_t static_color_index);

/* 设置图标索引，索引会限幅 */
void led_animation_set_icon(uint8_t icon_index);

/* 设置 White Select 颜色索引，索引会限幅 */
void led_animation_set_white_color_index(uint8_t white_color_index);
uint8_t led_animation_get_white_color_index(void);

/* 进入 Modes：从索引 0 彩虹开始 */
void led_animation_modes_enter(uint32_t now_ms);

/* Modes 前进一个索引，最大到 H；不自动进入关灯 */
void led_animation_modes_next(uint32_t now_ms);

/* 当前 Modes 索引 */
uint8_t led_animation_get_modes_state(void);

/* 当前是否处于 Modes 的 H 状态 */
bool led_animation_modes_is_h(void);

/* 动画暂停/恢复；关灯/开灯使用，不重置模式 */
void led_animation_pause(uint32_t now_ms);
void led_animation_resume(uint32_t now_ms);
bool led_animation_is_paused(void);

/* 获取指定灯当前颜色（100% 基准，未乘全局亮度） */
void led_animation_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                                  led_color_t *output_color);

#endif
