#include "include.h"
#include "bsp_huart.h"
#include "api.h"
#include "driver_com.h"
#include "driver_hsuart.h"
#include "ble_init.h"
#include "driver_gpio.h"
#include "config.h"

#if LE_DUT_UART_EN

//#define LE_FCC_TONE_EN
#ifdef LE_FCC_TONE_EN
static fcc_cfg_t fcc_cfg;
#endif

static struct _ble_dut_cfg{
    u8 vddcore;
} ble_dut_cfg;

AT(.dut.huart_buf)
u8 huart_buffer[128];


AT(.com_text.huart.text)
void huart_wait_txdone(void)
{
    while (hsuart_get_flag(HSUART_IT_TX) == RESET);
}

///设置HUART波特率
AT(.text.huart.huart_set_baudrate)
void huart_set_baudrate(uint baud_rate)
{
    hsuart_baud_config(baud_rate);
}

///HUART 发送1Byte数据
AT(.com_text.huart.text)
void huart_putchar(const char ch)
{
    while (hsuart_get_flag(HSUART_IT_TX) == RESET);
    hsuart_send_data((uint16_t)ch);

}

///HUART 获取RXFIFO中的数据个数
AT(.com_text.huart.huart_getchar)
uint huart_get_rxcnt(void)
{
    return hsuart_get_fifo_counter();
}

///HUART 获取1Byte数据
AT(.com_text.huart.huart_getchar)
char huart_getchar(void)
{
    char data = 0;

    hsuart_receive_data_from_fifo((uint8_t *)&data, 1);

    return data;
}

///HUART 发送数据
AT(.com_text.huart.text)
void huart_tx(const void *buf, uint len)
{
    huart_wait_txdone();

    if ((u32)buf >= 0x10000000) {
        HSUT0CON &= ~BIT(7);        //buf mode
        const char *txbuf = buf;
        while (len--) {
            huart_putchar(*txbuf++);
        }
    } else {
        HSUT0CON |= BIT(7);         //dma mode
        hsuart_dma_start(HSUT_TRANSMIT, DMA_ADR(buf), len);
    }
}

void bt_uart_init(void)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    ble_dut_cfg.vddcore = pmu_get_vddcore();
    pmu_set_vddcore(0x1A);

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

/************************************ GPIO Init ************************************/
#if (FUNC_LE_FCC_EN || LE_FCC_TEST_EN)
    gpio_init_structure.gpio_pin = GPIO_PIN_GET(LE_FCC_TEST_HSUART_IO_SEL);
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(GPIO_PORT_GET(LE_FCC_TEST_HSUART_IO_SEL), &gpio_init_structure);

    /* NOTE: make sure printf_io isn't using PB3 */
    gpio_func_mapping_clear(GPIO_PORT_GET(LE_FCC_TEST_HSUART_IO_SEL), GPIO_PIN_GET(LE_FCC_TEST_HSUART_IO_SEL));
    gpio_func_mapping_config(GPIO_PORT_GET(LE_FCC_TEST_HSUART_IO_SEL), GPIO_PIN_GET(LE_FCC_TEST_HSUART_IO_SEL), GPIO_CROSSBAR_IN_HURRX);
    gpio_func_mapping_config(GPIO_PORT_GET(LE_FCC_TEST_HSUART_IO_SEL), GPIO_PIN_GET(LE_FCC_TEST_HSUART_IO_SEL), GPIO_CROSSBAR_OUT_HURTX);
#elif (FUNC_LE_BQB_RF_EN || LE_BQB_RF_EN)
    gpio_init_structure.gpio_pin = GPIO_PIN_GET(LE_BQB_RF_HSUART_RX_IO_SEL);
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(GPIO_PORT_GET(LE_BQB_RF_HSUART_RX_IO_SEL), &gpio_init_structure);
    gpio_init_structure.gpio_pin = GPIO_PIN_GET(LE_BQB_RF_HSUART_TX_IO_SEL);
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(GPIO_PORT_GET(LE_BQB_RF_HSUART_TX_IO_SEL), &gpio_init_structure);
    gpio_func_mapping_config(GPIO_PORT_GET(LE_BQB_RF_HSUART_RX_IO_SEL), GPIO_PIN_GET(LE_BQB_RF_HSUART_RX_IO_SEL), GPIO_CROSSBAR_IN_HURRX);
    gpio_func_mapping_config(GPIO_PORT_GET(LE_BQB_RF_HSUART_TX_IO_SEL), GPIO_PIN_GET(LE_BQB_RF_HSUART_TX_IO_SEL), GPIO_CROSSBAR_OUT_HURTX);
#endif

/*********************************** HSUART Init ***********************************/
#if (FUNC_LE_FCC_EN || LE_FCC_TEST_EN)
    hsuart_init_struct.baud = LE_FCC_TEST_HSUART_BAUD;
    hsuart_init_struct.one_line_en = HSUT_ONELINE_EN;
#elif (FUNC_LE_BQB_RF_EN || LE_BQB_RF_EN)
    hsuart_init_struct.baud = LE_BQB_RF_HSUART_BAUD;
    hsuart_init_struct.one_line_en = HSUT_ONELINE_DIS;
#endif
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_2BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_24M_CLK;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_LOOP;
    hsuart_init(&hsuart_init_struct);

    hsuart_clear_flag(HSUART_IT_RX);

    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);

    hsuart_dma_start(HSUT_RECEIVE, (uint32_t)huart_buffer, 128);

#ifdef LE_FCC_TONE_EN
#if (FUNC_LE_FCC_EN || LE_FCC_TEST_EN)
    fcc_cfg.rf_mode = FCC_TX_TONE;
    fcc_cfg.tx_power = 7;
    fcc_cfg.tx_freq = 1;
    fcc_cfg.todo = 1;
    ble_fcc_cfg_init(&fcc_cfg);
#endif
#endif
}

void bt_uart_exit(void)
{
    pmu_set_vddcore(ble_dut_cfg.vddcore);
}
#endif
