#ifndef _BSP_HUART_IODM_H
#define _BSP_HUART_IODM_H


#define HUART_IODM_BUF_LEN           sizeof(iodm_packet_t)

#define HSUART_IODM_BAUD             115200
#define HSUART_IODM_PORT_SEL         GPIOB_REG
#define HSUART_IODM_PIN_SEL          GPIO_PIN_9  //VUSB
#define HSUART_IODM_OV_CNT           0x20        //unit: time for transmitting 1 bit

void bsp_huart_iodm_init(void);
void bsp_huart_iodm_dma_tx(const uint8_t *dma_buf, uint8_t dma_buf_len);

#endif
