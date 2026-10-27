/*
 * 文件说明：
 * LED 灯串的 SPI/DMA 传输层。负责把 6 字节协议帧编码成
 * “低电平 + 高电平”波形，并在关灯时把 PA3 保持为低电平。
 */

#include "led_transport_spi.h"
#include "include.h"
#include "driver_gpio.h"
#include "driver_spi.h"
#include "driver_tmr.h"

#include "user_config.h"

/* SPI 时钟频率：1 MHz，1 bit = 1 us，1 字节 = 8 us */
#define LED_TRANSPORT_SPI_BAUD 1000000UL
/* 每个字节包含的 bit 数 */
#define LED_TRANSPORT_BITS_PER_BYTE 8U
/* 逻辑 0 的高电平时间 24 us，对应 3 个 0xFF 字节 */
#define LED_TRANSPORT_BIT0_HIGH_BYTES 3U
/* 逻辑 1 的高电平时间 56 us，对应 7 个 0xFF 字节 */
#define LED_TRANSPORT_BIT1_HIGH_BYTES 7U
/* 帧间隔 1320 us；1 MHz SPI 下 1 字节 = 8 us，163 字节 = 1304 us，加帧尾 16 us = 1320 us */
#define LED_TRANSPORT_GAP_BYTES 163U
/* 在 DMA 结束前提前切 GPIO 高的时间，按 TMR1 的 10 us 分辨率取 500 us
   帧间隔有 1312 us，提前量可以大幅增加，不会切到数据区 */
#define LED_TRANSPORT_GAP_SWITCH_MARGIN_US 500U
/* 实测 TMR1 计数 10 us 对应 1600 个计数（160 MHz 系统时钟） */
#define LED_TRANSPORT_TMR1_10US_TICKS 1600U
/* 初始化后保持空闲高电平 300 us，确保灯串芯片从帧边界开始 */
#define LED_TRANSPORT_INIT_IDLE_US 300U
/* 单帧波形最大字节数：48 bit × (1 低 + 7 高) + 帧尾 2 字节 = 386 */
#define LED_TRANSPORT_WAVE_MAX_BYTES 386U
/* 单帧缓冲最大字节数：386 + 163 = 549 */
#define LED_TRANSPORT_FRAME_BUFFER_BYTES 549U
/* 低电平字节值：0x00 持续 8 bit，即 8 us */
#define LED_TRANSPORT_LOW_BYTE 0x00U
/* 高电平字节值：0xFF 持续 8 bit，即 8 us */
#define LED_TRANSPORT_HIGH_BYTE 0xFFU
/* MSB 掩码：用于判断当前协议 bit 是 0 还是 1 */
#define LED_TRANSPORT_MSB_MASK 0x80U
/* 灯串数据引脚：PA3 */
#define LED_TRANSPORT_GPIO_PORT GPIOA_REG
#define LED_TRANSPORT_GPIO_PIN  GPIO_PIN_3
/* 测试灯串的灯数 */
#define TEST_LED_COUNT 12U

/* 单帧 SPI 波形缓冲区，静态分配，避免动态内存 */
static uint8_t led_transport_frame_buffer[LED_TRANSPORT_FRAME_BUFFER_BYTES];

volatile uint32_t spi_interrupt_count; // SPI 完成中断进入次数，调试用
static volatile uint8_t led_transport_frame_complete_flag; // 单帧发送完成标志
static volatile uint8_t led_transport_power_on_flag = 1U; // 1: 开灯输出, 0: 控制脚低

/**
 * @brief SPI DMA 完成中断
 * @param 无
 * @return 无
 * @note SPI 中断源就是 DMA 完成，这里不再判断 pending，直接清标志并置完成
 */
AT(.com_text.isr)
void led_transport_spi_isr(void)
{
#if USER_DEBUG_ENABLE
    // USER_DEBUG_IO_TOGGLE(); // 调试：确认 SPI 中断是否每帧进入一次
#endif
    spi_interrupt_count++; // 记录中断次数

    /* 清 SPI pending，避免中断重复进入 */
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    /* 通知等待中的 driver 线程：本帧发送完成 */
    led_transport_frame_complete_flag = 1;
    thread_driver_sem_release();
}

/**
 * @brief 将 6 字节协议帧编码为 SPI 波形字节
 * @param frame 指向 6 字节协议帧的指针
 * @param wave 指向波形缓冲区的指针
 * @return 编码后的波形字节数
 */
static uint16_t led_transport_encode_frame(const uint8_t *frame, uint8_t *wave)
{
    uint16_t wave_len = 0; // 当前波形缓冲区写入位置
    uint8_t byte_index;    // 协议帧字节索引
    uint8_t bit_index;     // 当前字节内的 bit 索引
    uint8_t high_bytes;    // 当前 bit 的高电平字节数
    uint8_t data;          // 当前处理的协议字节

    for (byte_index = 0; byte_index < LED_TRANSPORT_FRAME_BYTES; byte_index++) {
        data = frame[byte_index];

        for (bit_index = 0; bit_index < LED_TRANSPORT_BITS_PER_BYTE;
             bit_index++) {
            wave[wave_len++] = LED_TRANSPORT_LOW_BYTE; // 8 us 低电平

            if (data & LED_TRANSPORT_MSB_MASK) {
                high_bytes = LED_TRANSPORT_BIT1_HIGH_BYTES;
            } else {
                high_bytes = LED_TRANSPORT_BIT0_HIGH_BYTES;
            }

            memset(&wave[wave_len], LED_TRANSPORT_HIGH_BYTE, high_bytes);
            wave_len += high_bytes;
            data <<= 1; // 准备下一个 bit，MSB first
        }
    }

    wave[wave_len++] = LED_TRANSPORT_LOW_BYTE;  // 帧尾 8 us 低电平
    wave[wave_len++] = LED_TRANSPORT_HIGH_BYTE; // 回到空闲高电平

    return wave_len;
}

/**
 * @brief 把 PA3 切换为 GPIO 输出高，作为空闲电平
 * @param 无
 * @return 无
 * @note 只切 FEN/FDIR，SPI 映射保留；可在主循环或 SPI 中断中调用
 */
static void led_transport_spi_set_idle_high(void)
{
    LED_TRANSPORT_GPIO_PORT->io_set = (uint32_t)LED_TRANSPORT_GPIO_PIN;
    LED_TRANSPORT_GPIO_PORT->fdir &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
    LED_TRANSPORT_GPIO_PORT->fen &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
}

/**
 * @brief 把 PA3 切换为 SPI1D0 输出
 * @param 无
 * @return 无
 * @note 映射已在初始化时配置好，这里只切 FEN/FDIR
 */
static void led_transport_spi_set_spi_output(void)
{
    LED_TRANSPORT_GPIO_PORT->fdir |= (uint32_t)LED_TRANSPORT_GPIO_PIN;
    LED_TRANSPORT_GPIO_PORT->fen |= (uint32_t)LED_TRANSPORT_GPIO_PIN;
}

/**
 * @brief 关闭 LED 输出：停 TMR1/SPI 中断，PA3 持续低
 */
void led_transport_spi_power_off(void)
{
    spi_it_cmd(SPI1, DISABLE);
    tmr_cmd(TMR1, DISABLE);
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    led_transport_frame_complete_flag = 0U;

    /* 先输出低，再保持 GPIO 输出；关灯期间不能有空闲高 */
    LED_TRANSPORT_GPIO_PORT->io_clr = (uint32_t)LED_TRANSPORT_GPIO_PIN;
    LED_TRANSPORT_GPIO_PORT->fdir &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
    LED_TRANSPORT_GPIO_PORT->fen &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
    led_transport_power_on_flag = 0U;
}

/**
 * @brief 打开 LED 输出：PA3 空闲高，等待稳定后恢复发帧
 */
void led_transport_spi_power_on(void)
{
    led_transport_power_on_flag = 1U;
    led_transport_spi_set_idle_high();
    /* 保证第一帧前空闲高稳定，避免被灯串芯片当成数据位 */
    delay_us(LED_TRANSPORT_INIT_IDLE_US);
}

/**
 * @brief 查询 LED 输出是否处于开灯状态
 */
bool led_transport_spi_is_power_on(void)
{
    return (led_transport_power_on_flag != 0U);
}

/**
 * @brief 计算提前切 GPIO 高的 TMR1 计数值
 * @param wave_len 本帧波形总字节数
 * @return TMR1 周期计数值
 * @note TMR1 按 10 us 分辨率工作，不再使用 clk_tmr_inc_get 换算
 */
static uint32_t led_transport_gap_ticks(uint16_t wave_len)
{
    uint32_t us;    // DMA 总时间，单位 us
    uint32_t units; // 10 us 的个数

    us = (uint32_t)wave_len * 8U;
    if (us <= LED_TRANSPORT_GAP_SWITCH_MARGIN_US) {
        return 0U;
    }
    us -= LED_TRANSPORT_GAP_SWITCH_MARGIN_US;

    units = us / 10U;
    if (units == 0U) {
        return 0U;
    }

    return units * LED_TRANSPORT_TMR1_10US_TICKS;
}

// /**
//  * @brief TMR1 中断：在 DMA 结束前把 PA3 切回 GPIO 高
//  * @param 无
//  * @return 无
//  * @note 此时 SPI 还在输出 0xFF 高电平，切换是高到高，不产生低电平
//  */
// AT(.com_text.isr)
// void led_transport_gap_isr(void)
// {
//     if (tmr_get_flag(TMR1, TMR_IT_UPDATE) != RESET) {
//         tmr_clear_flag(TMR1, TMR_IT_UPDATE);
//         tmr_cmd(TMR1, DISABLE);
//         led_transport_spi_set_idle_high();
//         // USER_DEBUG_IO_TOGGLE();
//     }
// }

AT(.com_text.isr)
void led_transport_gap_isr(void)
{
    if (tmr_get_flag(TMR1, TMR_IT_UPDATE) != RESET) {
        tmr_cmd(TMR1, DISABLE);
        tmr_clear_flag(TMR1, TMR_IT_UPDATE);

        /* 直接操作寄存器，避免在 ISR 中调用 Flash 函数 */
        GPIOA_REG->io_set = (uint32_t)LED_TRANSPORT_GPIO_PIN;
        GPIOA_REG->fdir &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
        GPIOA_REG->fen &= ~(uint32_t)LED_TRANSPORT_GPIO_PIN;
    }
}

/**
 * @brief 初始化 LED 灯串的 SPI 传输层
 * @param 无
 * @return 无
 * @note 初始化后 PA3 保持 GPIO 高电平，发送时再映射到 SPI1D0
 */
void led_transport_spi_init(void)
{
    gpio_init_typedef gpio_init_struct;      // GPIO 初始化参数
    spi_init_typedef spi_init_struct;        // SPI 初始化参数
    tmr_base_init_typedef timer_init_struct; // TMR1 初始化参数

    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);

    /* 先把 PA3 配置为 GPIO 输出高，保证上电空闲电平为高 */
    gpio_init_struct.gpio_pin = LED_TRANSPORT_GPIO_PIN;
    gpio_init_struct.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_struct.gpio_drv = GPIO_DRV_6MA;
    gpio_init(LED_TRANSPORT_GPIO_PORT, &gpio_init_struct);
    gpio_set_bits(LED_TRANSPORT_GPIO_PORT, LED_TRANSPORT_GPIO_PIN);
    led_transport_power_on_flag = 1U;

    /* 映射只配置一次；后面只切 FEN/FDIR，不重复清/写映射 */
    gpio_func_mapping_config(LED_TRANSPORT_GPIO_PORT, LED_TRANSPORT_GPIO_PIN,
                             GPIO_CROSSBAR_OUT_SPI1D0);
    led_transport_spi_set_idle_high();

    /* SPI1 主机、3 线模式、1 MHz、时钟空闲高、下降沿输出数据 */
    spi_init_struct.baud = LED_TRANSPORT_SPI_BAUD;
    spi_init_struct.role_mode = SPI_ROLE_MODE_MASTER;
    spi_init_struct.bus_mode = SPI_3WIRE_MODE;
    spi_init_struct.output_data_edge = SPI_OUTPUT_DATA_EDGE_FALLING;
    spi_init_struct.sample_clk_edge = SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF;
    spi_init_struct.clkids = SPI_CLOCK_IDLE_HIGH;
    spi_init(SPI1, &spi_init_struct);

    spi_dir_sel(SPI1, SPI_DIR_TX);
    spi_cmd(SPI1, ENABLE);

    /* 先把 SPI 移位寄存器置为高，减少切换映射时的低电平 */
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    spi_send_data(SPI1, LED_TRANSPORT_HIGH_BYTE);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING)) {
    }

    /* dummy 字节结束后再注册 SPI 中断，避免它触发一次假完成 */
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    led_transport_frame_complete_flag = 0;
    /* SPI 完成中断优先级必须为 0，与 TMR1/TMR3 保持一致 */
    spi_pic_config(SPI1, led_transport_spi_isr, 0, ENABLE);

#if 1
    /* TMR1：在 DMA 结束前提前把 PA3 切回 GPIO 高 */
    clk_gate0_cmd(CLK_GATE0_TMR1, CLK_EN);
    // 用系统时钟计数（SYS_160M = 160 MHz），0.00625 us 一个计数
    timer_init_struct.clock_source = TMR_SYSTEM_CLOCK;
    timer_init_struct.counter_source = TMR_INC_SOURCE;
    // tmr1 的预分频无效，默认为0
    // timer_init_struct.prescale = 10 - 1;
    // timer_init_struct.prescale = 0U;
    /* 计数 10us == 1600 * 0.00625 us */
    timer_init_struct.period = LED_TRANSPORT_TMR1_10US_TICKS - 1U;
    tmr_base_init(TMR1, &timer_init_struct);
    tmr_pic_config(TMR1, led_transport_gap_isr, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR1, DISABLE);
#endif

    /* 保持一段空闲高电平，避免初始化时的短脉冲被当作数据位 */
    delay_us(LED_TRANSPORT_INIT_IDLE_US);
}

/**
 * @brief 初始化逐帧发送接口
 * @param 无
 * @return 无
 * @note 方案 A 只把 PA3 置为空闲高电平；方案 B 可在这里初始化双缓冲/队列
 */
void led_transport_spi_frame_init(void)
{
    led_transport_power_on_flag = 1U;
    led_transport_spi_set_idle_high();
}

/**
 * @brief 查询是否可以提交下一帧
 * @param 无
 * @return true 可以提交；false 需要等待
 * @note 方案 A 恒返回 true；方案 B 返回是否有空闲缓冲
 */
bool led_transport_spi_frame_ready(void)
{
    return (led_transport_power_on_flag != 0U);
}

/**
 * @brief 提交一帧协议数据
 * @param frame 指向 6 字节协议帧的指针
 * @return 无
 * @note 方案 A：阻塞到本帧和帧间隔发送完成；方案 B：入队后立即返回
 */
void led_transport_spi_frame_submit(const uint8_t *frame)
{
    uint16_t wave_len;  // 编码后的波形字节数
    uint32_t gap_ticks; // TMR1 提前切换计数值

    if (led_transport_power_on_flag == 0U) {
        return;
    }

    wave_len = led_transport_encode_frame(frame, led_transport_frame_buffer);

    /* 帧间隔也放进 DMA，由 0xFF 字节产生高电平 */
    memset(&led_transport_frame_buffer[wave_len], LED_TRANSPORT_HIGH_BYTE,
           LED_TRANSPORT_GAP_BYTES);
    wave_len += LED_TRANSPORT_GAP_BYTES;

    /* 清旧 pending，SPI 完成由中断置位并释放 driver 信号量 */
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    led_transport_frame_complete_flag = 0;

    led_transport_spi_set_spi_output();

    spi_dir_sel(SPI1, SPI_DIR_TX);
    spi_set_dma_addr(SPI1, (uint32_t)led_transport_frame_buffer);
    spi_set_dma_cnt(SPI1, (uint32_t)wave_len);
    /* 使能 SPI 中断，等待 DMA 完成中断置完成标志 */
    spi_it_cmd(SPI1, ENABLE);

#if 1
    /* TMR1 在 DMA 结束前约 500 us 触发，提前切 GPIO 高 */
    gap_ticks = led_transport_gap_ticks(wave_len);
    tmr_cmd(TMR1, DISABLE);
    if (gap_ticks > 0U) {
        tmr_clear_flag(TMR1, TMR_IT_UPDATE);
        tmr_set_period(TMR1, gap_ticks - 1U);
        tmr_set_counter(TMR1, 0U);
        tmr_cmd(TMR1, ENABLE);
    }
#endif

    /* 阻塞 driver 线程，DMA 期间让蓝牙线程运行 */
    thread_driver_sem_take();

    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    tmr_cmd(TMR1, DISABLE);
    led_transport_spi_set_idle_high();
}

/**
 * @brief 传输层轮询
 * @param 无
 * @return 无
 * @note 方案 A 为空实现；方案 B 用于处理 DMA 完成并启动下一帧
 */
void led_transport_spi_frame_poll(void)
{
}

/* 测试用单灯颜色结构，字段顺序与协议帧的 B、G、R、W 一致 */
typedef struct
{
    uint8_t b; // 蓝色分量
    uint8_t g; // 绿色分量
    uint8_t r; // 红色分量
    uint8_t w; // 白色分量
} test_led_color_t;

/* 12 盏灯的静态测试颜色，按灯号 1~12 排列 */
static const test_led_color_t test_led_colors[TEST_LED_COUNT] = {
    {0x00, 0x00, 0xFF - 1, 0x00}, // 1 红
    // {0x00, 0x00, 0x00, 0xFF}, // 1 白
    {0x00, 0xFF - 1, 0x00, 0x00},         // 2 绿
    {0xFF - 1, 0x00, 0x00, 0x00},         // 3 蓝
    {0x00, 0xFF - 1, 0xFF - 1, 0x00},     // 4 黄
    {0xFF - 1, 0xFF - 1, 0x00, 0x00},     // 5 青
    {0xFF - 1, 0x00, 0xFF - 1, 0x00},     // 6 紫
    {0x00, 0x80 - 1, 0xFF - 1, 0x00},     // 7 橙
    {0x50 - 1, 0xB4 - 1, 0xFF - 1, 0x00}, // 8 暖白
    {0xFF - 1, 0xC8 - 1, 0xB4 - 1, 0x00}, // 9 冷白
    {0xA0 - 1, 0x50 - 1, 0xFF - 1, 0x00}, // 10 粉
    {0xFF - 1, 0xFF - 1, 0xFF - 1, 0x00}, // 11 RGB 白
    {0x00, 0x00, 0x00, 0xFF - 1},         // 12 W 白
};

/**
 * @brief 测试函数：发送 12 盏灯的静态颜色数据
 * @param 无
 * @return 无
 * @note 每盏灯发送一帧，共 12 帧；函数会阻塞到 12 帧全部发送完成
 */
void test_led_transport_spi_send_static_12(void)
{
    uint8_t frame[LED_TRANSPORT_FRAME_BYTES]; // 协议帧：灯号、B、G、R、W、校验
    uint8_t lamp_index;                       // 灯索引 0~11

    for (lamp_index = 0; lamp_index < TEST_LED_COUNT; lamp_index++) {
        frame[0] = (uint8_t)(lamp_index + 1U); // 灯号从 1 开始
        frame[1] = test_led_colors[lamp_index].b;
        frame[2] = test_led_colors[lamp_index].g;
        frame[3] = test_led_colors[lamp_index].r;
        frame[4] = test_led_colors[lamp_index].w;
        frame[5] = (uint8_t)(frame[0] + frame[1] + frame[2] + frame[3] +
                             frame[4]); // 校验

        led_transport_spi_frame_submit(frame);
    }
}
