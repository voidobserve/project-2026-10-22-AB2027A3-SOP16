#ifndef __USER_DEBUG_IO_H__
#define __USER_DEBUG_IO_H__

#include "include.h"
#include "driver_gpio.h"
#include "user_config.h"

#if USER_DEBUG_ENABLE 

// 测试时使用到的引脚，用于在逻辑分析仪观察波形
#define USER_DEBUG_IO_PORT GPIOA_REG
#define USER_DEBUG_IO_PIN GPIO_PIN_5
#define USER_DEBUG_IO_SET()  gpio_set_bits(USER_DEBUG_IO_PORT, USER_DEBUG_IO_PIN)
#define USER_DEBUG_IO_RESET() gpio_reset_bits(USER_DEBUG_IO_PORT, USER_DEBUG_IO_PIN)
#define USER_DEBUG_IO_TOGGLE() gpio_toggle_bits(USER_DEBUG_IO_PORT, USER_DEBUG_IO_PIN)

void user_debug_io_init(void);
#endif

#endif
