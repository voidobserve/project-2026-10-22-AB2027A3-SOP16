#ifndef __LED_PROTOCOL_H__
#define __LED_PROTOCOL_H__

#include <stdint.h>

/* 灯串的灯数：灯号 1~12 */
#define LED_LAMP_COUNT 12U
/* 协议帧长度：灯号、B、G、R、W、校验 */
#define LED_FRAME_BYTES 6U

/* 单个灯的颜色，100% 基准值 */
typedef struct {
    uint8_t r; // 红色分量
    uint8_t g; // 绿色分量
    uint8_t b; // 蓝色分量
    uint8_t w; // 白色分量
} led_color_t;

/**
 * @brief 初始化协议层
 * @param 无
 * @return 无
 * @note 只复位灯号游标，不影响动画层
 */
void led_protocol_init(void);

/**
 * @brief 获取当前要发送的灯号
 * @param 无
 * @return 当前灯号 1~12
 */
uint8_t led_protocol_current_lamp(void);

/**
 * @brief 按灯号和颜色组一帧协议数据
 * @param lamp 灯号 1~12
 * @param color 指向当前灯颜色的指针
 * @param frame 输出 6 字节协议帧
 * @return 无
 */
void led_protocol_build_frame(uint8_t lamp, const led_color_t *color,
                              uint8_t frame[LED_FRAME_BYTES]);

/**
 * @brief 灯号游标前进到下一个
 * @param 无
 * @return 无
 * @note 灯号 1~12 循环，模式切换不能复位该游标
 */
void led_protocol_advance_lamp(void);

#endif
