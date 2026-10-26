#ifndef __LED_ANIMATION_H__
#define __LED_ANIMATION_H__

#include <stdint.h>

#include "led_protocol.h"

/* 动画模式 */
typedef enum {
    LED_ANIM_OFF = 0,
    LED_ANIM_STATIC_COLOR,
    LED_ANIM_FADE,
} led_anim_mode_t;

/**
 * @brief 初始化动画层
 * @param now_ms 当前 1 ms 时间
 * @return 无
 */
void led_anim_init(uint32_t now_ms);

/**
 * @brief 切换动画模式
 * @param mode 目标模式
 * @param now_ms 当前 1 ms 时间
 * @return 无
 * @note 只重置动画起点，不修改协议层灯号游标
 */
void led_anim_set_mode(led_anim_mode_t mode, uint32_t now_ms);

/**
 * @brief 设置静态模式颜色
 * @param color 指向 100% 基准颜色的指针
 * @return 无
 */
void led_anim_set_static_color(const led_color_t *color);

/**
 * @brief 获取指定灯当前颜色
 * @param now_ms 当前 1 ms 时间
 * @param lamp 灯号 1~12
 * @param out 输出颜色
 * @return 无
 * @note 每帧发送前调用，所以颜色变化会在下一帧生效
 */
void led_anim_get_lamp_color(uint32_t now_ms, uint8_t lamp, led_color_t *out);

#endif
