#ifndef _BSP_UART_DEBUG_H
#define _BSP_UART_DEBUG_H


#define UART_DEBUG_PORT_SEL             GPIO_PORT_GET(BSP_UART_DEBUG_EN)
#define UART_DEBUG_PIN_SEL              GPIO_PIN_GET(BSP_UART_DEBUG_EN)
#define UART_DEBUG_BAUD                 1500000

void bsp_uart_debug_init(void);


#endif // _BSP_UART_DEBUG_H
