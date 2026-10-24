#include "include.h"
#include "driver_gpio.h"
#include "bsp_led.h"

#if BSP_LED_EN

void bsp_led_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = GPIO_PIN_2;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;

    gpio_init(GPIOB_REG, &gpio_init_structure);
}

void bsp_led_process(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, 500)) {
        delay_cnt = tick_get();
        gpio_toggle_bits(GPIOB_REG, GPIO_PIN_2);
    }
}

#endif
