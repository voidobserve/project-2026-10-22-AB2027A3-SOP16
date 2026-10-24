#include "include.h"
#include "driver_gpio.h"
#include "driver_uart.h"
#include "bsp_uart_debug.h"

#if BSP_UART_DEBUG_EN

void bsp_uart_debug_init(void)
{
    uart_init_typedef uart_init_struct;
    gpio_init_typedef gpio_init_structure;

    /************************** 时钟使能 ***************************/
    clk_gate0_cmd(CLK_GATE0_UART, CLK_EN);

    /************************* GPIO初始化 *************************/
    gpio_init_structure.gpio_pin = UART_DEBUG_PIN_SEL;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_PER;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_MAP;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init(UART_DEBUG_PORT_SEL, &gpio_init_structure);

    /*********************** Crossbar映射 ************************/
    gpio_func_mapping_config(UART_DEBUG_PORT_SEL, UART_DEBUG_PIN_SEL, GPIO_CROSSBAR_IN_UARTRX);
    gpio_func_mapping_config(UART_DEBUG_PORT_SEL, UART_DEBUG_PIN_SEL, GPIO_CROSSBAR_OUT_UARTTX);

    /************************** 使能串口 **************************/
    uart_init_struct.baud = UART_DEBUG_BAUD;
    uart_init_struct.mode = UART_SEND_RECV_MODE;
    uart_init_struct.word_len = UART_WORD_LENGTH_8b;
    uart_init_struct.stop_bits = UART_STOP_BIT_1BIT;
    uart_init_struct.one_line_enable = UART_ONE_LINE;
    uart_init_struct.clock_source = UART_SYSTEM_CLOCK;
    uart_init(UART, &uart_init_struct);

    uart_cmd(UART, ENABLE);
}

#endif // BSP_UART_DEBUG_EN
