#include "include.h"
#include "bsp_huart_iodm.h"
#include "driver_hsuart.h"
#include "prod_test_iodm.h"
#include "driver_gpio.h"

#if FUNC_IODM_EN

static uint8_t huart_iodm_rx_buffer[HUART_IODM_BUF_LEN] = {0}; 
static uint8_t huart_iodm_tx_buffer[HUART_IODM_BUF_LEN] = {0}; 

///HUART IODM 等发送空闲
AT(.com_text.huart_iodm.text)
static void bsp_huart_iodm_wait_txdone(void)
{
    while (hsuart_get_flag(HSUART_IT_TX) == RESET);
}

///HUART IODM 发送数据
void bsp_huart_iodm_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len)
{
    bsp_huart_iodm_wait_txdone();
    memcpy(huart_iodm_tx_buffer, dma_buf, dma_buf_len);
    hsuart_dma_start(HSUT_TRANSMIT, DMA_ADR(huart_iodm_tx_buffer), dma_buf_len);
}

///HUART IODM DMA 中断服务函数
AT(.com_text.huart_iodm.dma_rx_isr)
static void bsp_huart_iodm_dma_rx_isr(void)
{
    if (hsuart_get_flag(HSUART_IT_RX) != RESET) {
        prod_test_iodm_packet_receive(huart_iodm_rx_buffer, HUART_IODM_BUF_LEN);
        prod_test_iodm_set_do_flag(FLAG_IODM_RECEIVE_EVENT);
        hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
        hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(huart_iodm_rx_buffer), HUART_IODM_BUF_LEN);
    }
}

void bsp_huart_iodm_init(void)
{
    gpio_init_typedef gpio_init_structure;
    hsuart_init_typedef hsuart_init_struct;

    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_EN);

/************************************ GPIO Init ************************************/
    gpio_init_structure.gpio_pin = HSUART_IODM_PIN_SEL;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(HSUART_IODM_PORT_SEL, &gpio_init_structure);

    /* NOTE: make sure printf_io isn't using PB9(VUSB) */
    gpio_func_mapping_clear(HSUART_IODM_PORT_SEL, HSUART_IODM_PIN_SEL);
    gpio_func_mapping_config(HSUART_IODM_PORT_SEL, HSUART_IODM_PIN_SEL, GPIO_CROSSBAR_IN_HURRX);
    gpio_func_mapping_config(HSUART_IODM_PORT_SEL, HSUART_IODM_PIN_SEL, GPIO_CROSSBAR_OUT_HURTX);

/*********************************** HSUART Init ***********************************/
    hsuart_init_struct.baud = HSUART_IODM_BAUD;
    hsuart_init_struct.one_line_en = HSUT_ONELINE_EN;
    hsuart_init_struct.tx_mode = HSUT_TX_DMA_MODE;
    hsuart_init_struct.rx_mode = HSUT_RX_DMA_MODE;
    hsuart_init_struct.tx_stop_bit = HSUT_STOP_BIT_2BIT;
    hsuart_init_struct.tx_word_len = HSUT_TX_LENGTH_8b;
    hsuart_init_struct.rx_word_len = HSUT_RX_LENGTH_8b;
    hsuart_init_struct.clk_source = HSUT_24M_CLK;
    hsuart_init_struct.rx_dma_buf_mode = HSUT_DMA_BUF_SINGLE;
    hsuart_init(&hsuart_init_struct);
    hsuart_clear_flag(HSUART_IT_RX | HSUART_IT_RX_TMR_OV);
    hsuart_rx_idle_config(HSUART_IODM_OV_CNT, ENABLE);
    hsuart_pic_config(bsp_huart_iodm_dma_rx_isr, 0, HSUART_IT_RX, ENABLE);
    hsuart_cmd(HSUT_TRANSMIT | HSUT_RECEIVE, ENABLE);
    hsuart_dma_start(HSUT_RECEIVE, DMA_ADR(huart_iodm_rx_buffer), HUART_IODM_BUF_LEN);
}

void bsp_huart_iodm_exit(void)
{
}
#endif
