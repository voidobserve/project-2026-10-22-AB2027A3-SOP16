#include "led_protocol.h"

/* 当前灯号游标：0~11，对应灯号 1~12 */
static uint8_t lamp_cursor_index;

/**
 * @brief 初始化协议层
 * @param 无
 * @return 无
 * @note 只复位灯号游标，不影响动画层
 */
void led_protocol_init(void)
{
    lamp_cursor_index = 0;
}

/**
 * @brief 获取当前要发送的灯号
 * @param 无
 * @return 当前灯号 1~12
 */
uint8_t led_protocol_current_lamp(void)
{
    return (uint8_t)(lamp_cursor_index + 1U);
}

/**
 * @brief 按灯号和颜色组一帧协议数据
 * @param lamp_idx 灯号 1~12
 * @param color 指向当前灯颜色的指针
 * @param frame 输出 6 字节协议帧
 * @return 无
 * @note 帧格式：灯号、B、G、R、W、校验
 */
void led_protocol_build_frame(uint8_t lamp_idx, const led_color_t *color,
                              uint8_t frame[LED_FRAME_BYTES])
{
    frame[0] = lamp_idx;
    frame[1] = color->b;
    frame[2] = color->g;
    frame[3] = color->r;
    frame[4] = color->w;
    frame[5] = (uint8_t)(frame[0] + frame[1] + frame[2] + frame[3] +
                         frame[4]); // 校验：前 5 字节累加和
}

/**
 * @brief 灯号游标前进到下一个
 * @param 无
 * @return 无
 * @note 灯号 1~12 循环，模式切换不能复位该游标
 */
void led_protocol_advance_lamp(void)
{
    lamp_cursor_index = (uint8_t)((lamp_cursor_index + 1U) % LED_LAMP_COUNT);
}
