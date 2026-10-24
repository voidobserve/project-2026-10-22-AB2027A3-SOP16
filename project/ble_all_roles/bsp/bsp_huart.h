#ifndef _BSP_HUART_H
#define _BSP_HUART_H

void bsp_huart_init(void);

void huart_exit(void);
void huart_set_baudrate(uint baudrate);
void huart_putchar(const char ch);
void huart_tx(const void *buf, uint len);
uint huart_get_rxcnt(void);
char huart_getchar(void);

#endif
