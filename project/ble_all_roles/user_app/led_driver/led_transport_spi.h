#ifndef __LED_TRANSPORT_SPI_H__
#define __LED_TRANSPORT_SPI_H__

#include <stdint.h>
#include <stdbool.h>

/* 传输层的协议帧长度：灯号、B、G、R、W、校验，共 6 字节 */
#define LED_TRANSPORT_FRAME_BYTES 6

/**
 * @brief 初始化 LED 灯串的 SPI 传输层
 * @param 无
 * @return 无
 */
void led_transport_spi_init(void);

/**
 * @brief 初始化逐帧发送接口
 * @param 无
 * @return 无
 * @note 方案 A 只把 PA3 置为空闲高电平；方案 B 可在这里初始化双缓冲/队列
 */
void led_transport_spi_frame_init(void);

/**
 * @brief 查询是否可以提交下一帧
 * @param 无
 * @return true 可以提交；false 需要等待
 * @note 方案 A 恒返回 true；方案 B 返回是否有空闲缓冲
 */
bool led_transport_spi_frame_ready(void);

/**
 * @brief 提交一帧协议数据
 * @param frame 指向 6 字节协议帧的指针
 * @return 无
 * @note 方案 A：阻塞到本帧和帧间隔发送完成；方案 B：入队后立即返回
 */
void led_transport_spi_frame_submit(const uint8_t *frame);

/**
 * @brief 传输层轮询
 * @param 无
 * @return 无
 * @note 方案 A 为空实现；方案 B 用于处理 DMA 完成并启动下一帧
 */
void led_transport_spi_frame_poll(void);

/**
 * @brief 测试函数：发送 12 盏灯的静态颜色数据
 * @param 无
 * @return 无
 * @note 每盏灯发送一帧，共 12 帧；函数会阻塞到 12 帧全部发送完成
 */
void test_led_transport_spi_send_static_12(void);

#endif
