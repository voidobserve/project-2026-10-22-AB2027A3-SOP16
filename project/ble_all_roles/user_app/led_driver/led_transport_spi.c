#include "led_transport_spi.h"
#include "include.h"
#include "driver_gpio.h"
#include "driver_spi.h"

/* SPI 时钟频率：1 MHz，1 bit = 1 us，1 字节 = 8 us */
#define LED_TRANSPORT_SPI_BAUD         1000000UL
/* 每个字节包含的 bit 数 */
#define LED_TRANSPORT_BITS_PER_BYTE    8U
/* 逻辑 0 的高电平时间 24 us，对应 3 个 0xFF 字节 */
#define LED_TRANSPORT_BIT0_HIGH_BYTES  3U
/* 逻辑 1 的高电平时间 56 us，对应 7 个 0xFF 字节 */
#define LED_TRANSPORT_BIT1_HIGH_BYTES  7U
/* 帧间空闲 1250 us；实测原机中位约 1248 us，远大于解码阈值 200 us */
#define LED_TRANSPORT_FRAME_GAP_US     1250U
/* 初始化后保持空闲高电平 300 us，确保灯串芯片从帧边界开始 */
#define LED_TRANSPORT_INIT_IDLE_US     300U
/* 波形缓冲最大字节数：48 bit × (1 低 + 7 高) + 帧尾 2 字节 = 386 */
#define LED_TRANSPORT_WAVE_MAX_BYTES   386U
/* 低电平字节值：0x00 持续 8 bit，即 8 us */
#define LED_TRANSPORT_LOW_BYTE         0x00U
/* 高电平字节值：0xFF 持续 8 bit，即 8 us */
#define LED_TRANSPORT_HIGH_BYTE        0xFFU
/* MSB 掩码：用于判断当前协议 bit 是 0 还是 1 */
#define LED_TRANSPORT_MSB_MASK         0x80U
/* 灯串数据引脚：PA3 */
#define LED_TRANSPORT_GPIO_PORT        GPIOA_REG
#define LED_TRANSPORT_GPIO_PIN         GPIO_PIN_3
/* 测试灯串的灯数 */
#define TEST_LED_COUNT                 12U

/* SPI 波形缓冲区，静态分配，避免动态内存 */
static uint8_t led_transport_wave_buf[LED_TRANSPORT_WAVE_MAX_BYTES];

/**
 * @brief 将 6 字节协议帧编码为 SPI 波形字节
 * @param frame 指向 6 字节协议帧的指针
 * @param wave 指向波形缓冲区的指针
 * @return 编码后的波形字节数
 */
static uint16_t led_transport_encode_frame(const uint8_t *frame, uint8_t *wave)
{
    uint16_t wave_len = 0; // 当前波形缓冲区写入位置
    uint8_t byte_index; // 协议帧字节索引
    uint8_t bit_index; // 当前字节内的 bit 索引
    uint8_t high_bytes; // 当前 bit 的高电平字节数
    uint8_t data; // 当前处理的协议字节

    for (byte_index = 0; byte_index < LED_TRANSPORT_FRAME_BYTES;
         byte_index++) {
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

    wave[wave_len++] = LED_TRANSPORT_LOW_BYTE; // 帧尾 8 us 低电平
    wave[wave_len++] = LED_TRANSPORT_HIGH_BYTE; // 回到空闲高电平

    return wave_len;
}

/**
 * @brief 初始化 LED 灯串的 SPI 传输层
 * @param 无
 * @return 无
 */
void led_transport_spi_init(void)
{
    gpio_init_typedef gpio_init_struct; // GPIO 初始化参数
    spi_init_typedef spi_init_struct; // SPI 初始化参数

    clk_gate1_cmd(CLK_GATE1_SPI1, CLK_EN);

    /* 先把数据脚配置为 GPIO 输出高，保证上电空闲电平为高 */
    gpio_init_struct.gpio_pin = LED_TRANSPORT_GPIO_PIN;
    gpio_init_struct.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_struct.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init_struct.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_struct.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_struct.gpio_drv = GPIO_DRV_6MA;
    gpio_init(LED_TRANSPORT_GPIO_PORT, &gpio_init_struct);
    gpio_set_bits(LED_TRANSPORT_GPIO_PORT, LED_TRANSPORT_GPIO_PIN);

    /* 把 PA3 映射为 SPI1 的 MOSI（D0）输出 */
    gpio_init_struct.gpio_fen = GPIO_FEN_PER;
    gpio_init_struct.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init(LED_TRANSPORT_GPIO_PORT, &gpio_init_struct);
    gpio_func_mapping_config(LED_TRANSPORT_GPIO_PORT, LED_TRANSPORT_GPIO_PIN,
                             GPIO_CROSSBAR_OUT_SPI1D0);

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

    /* 先发一个高电平字节，确保 SPI 空闲时数据脚为高 */
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    spi_send_data(SPI1, LED_TRANSPORT_HIGH_BYTE);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING)) {
    }

    /* 保持一段空闲高电平，避免初始化时的短脉冲被当作数据位 */
    delay_us(LED_TRANSPORT_INIT_IDLE_US);
}

/**
 * @brief 通过 SPI 发送一帧 48 bit 的灯串数据，并保证帧间空闲高电平
 * @param frame 指向 6 字节协议帧的指针
 * @return 无
 * @note 该函数会阻塞到本帧发送完成并等待帧间空闲结束，只能在主循环/任务上下文调用
 */
void led_transport_spi_send_frame(const uint8_t *frame)
{
    uint16_t wave_len; // 编码后的波形字节数

    wave_len = led_transport_encode_frame(frame, led_transport_wave_buf);

    spi_dir_sel(SPI1, SPI_DIR_TX);
    spi_clear_flag(SPI1, SPI_FLAG_PENDING);
    spi_set_dma_addr(SPI1, (uint32_t)led_transport_wave_buf);
    spi_set_dma_cnt(SPI1, (uint32_t)wave_len);
    while (!spi_get_flag(SPI1, SPI_FLAG_PENDING)) {
    }

    /* 帧尾之后保持高电平，形成大于 200 us 的帧间空闲 */
    delay_us(LED_TRANSPORT_FRAME_GAP_US);
}

/* 测试用单灯颜色结构，字段顺序与协议帧的 B、G、R、W 一致 */
typedef struct {
    uint8_t b; // 蓝色分量
    uint8_t g; // 绿色分量
    uint8_t r; // 红色分量
    uint8_t w; // 白色分量
} test_led_color_t;

/* 12 盏灯的静态测试颜色，按灯号 1~12 排列 */
static const test_led_color_t test_led_colors[TEST_LED_COUNT] = {
    {0x00, 0x00, 0xFF, 0x00}, // 1 红
    {0x00, 0xFF, 0x00, 0x00}, // 2 绿
    {0xFF, 0x00, 0x00, 0x00}, // 3 蓝
    {0x00, 0xFF, 0xFF, 0x00}, // 4 黄
    {0xFF, 0xFF, 0x00, 0x00}, // 5 青
    {0xFF, 0x00, 0xFF, 0x00}, // 6 紫
    {0x00, 0x80, 0xFF, 0x00}, // 7 橙
    {0x50, 0xB4, 0xFF, 0x00}, // 8 暖白
    {0xFF, 0xC8, 0xB4, 0x00}, // 9 冷白
    {0xA0, 0x50, 0xFF, 0x00}, // 10 粉
    {0xFF, 0xFF, 0xFF, 0x00}, // 11 RGB 白
    {0x00, 0x00, 0x00, 0xFF}, // 12 W 白
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
    uint8_t lamp_index; // 灯索引 0~11

    for (lamp_index = 0; lamp_index < TEST_LED_COUNT; lamp_index++) {
        frame[0] = (uint8_t)(lamp_index + 1U); // 灯号从 1 开始
        frame[1] = test_led_colors[lamp_index].b;
        frame[2] = test_led_colors[lamp_index].g;
        frame[3] = test_led_colors[lamp_index].r;
        frame[4] = test_led_colors[lamp_index].w;
        frame[5] = (uint8_t)(frame[0] + frame[1] + frame[2] + frame[3] +
                             frame[4]); // 校验：前 5 字节累加和

        led_transport_spi_send_frame(frame);
    }
}
