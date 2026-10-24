#include "user_debug_io.h"
#include "driver_gpio.h"

#if USER_DEBUG_ENABLE
void user_debug_io_init(void)
{
    // 初始化 测试时使用到的io
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = USER_DEBUG_IO_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(USER_DEBUG_IO_PORT, &gpio_init_structure);
    gpio_reset_bits(USER_DEBUG_IO_PORT, USER_DEBUG_IO_PIN); // 输出低电平
}
#endif
