#ifndef __LED_TRANSPORT_SPI_H__
#define __LED_TRANSPORT_SPI_H__

#include <stdint.h>

/* 传输层的协议帧长度：灯号、B、G、R、W、校验，共 6 字节 */
#define LED_TRANSPORT_FRAME_BYTES 6

/**
 * @brief 初始化 LED 灯串的 SPI 传输层
 * @param 无
 * @return 无
 */
void led_transport_spi_init(void);

/**
 * @brief 通过 SPI 发送一帧 48 bit 的灯串数据，并保证帧间空闲高电平
 * @param frame 指向 6 字节协议帧的指针；传输层不解释字节含义，按 MSB first 原样发送
 * @return 无
 * @note 该函数会阻塞到本帧发送完成并等待帧间空闲结束，只能在主循环/任务上下文调用
 */
void led_transport_spi_send_frame(const uint8_t *frame);

/**
 * @brief 测试函数：发送 12 盏灯的静态颜色数据
 * @param 无
 * @return 无
 * @note 每盏灯发送一帧，共 12 帧；函数会阻塞到 12 帧全部发送完成
 */
void test_led_transport_spi_send_static_12(void);

#endif
