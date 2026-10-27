#ifndef __IO_KEY_DRIVER_H__
#define __IO_KEY_DRIVER_H__

#include "key_driver.h"

#define IO_KEY_PORT GPIOA_REG
#define IO_KEY_PIN  GPIO_PIN_0

// 定义按键键值
enum
{
    IO_KEY_VAL_NONE = 0x00,
    IO_KEY_VAL_KEY1 = 0x01,
};

// 定义按键事件
enum
{
    IO_KEY_EVENT_KEY1_CLICK,
    IO_KEY_EVENT_NONE,
    IO_KEY_EVENT_MAX,
};

#define IO_KEY_SCAN_TIME_MS             (10) // 遥控器按键扫描频率，单位：ms
#define IO_KEY_LONG_TIME_MS             (1500)
#define IO_KEY_HOLD_TIME_MS             (500)
#define IO_KEY_SCAN_CLICK_DELAY_TIME_MS (0)  // 按键被抬起后等待连击延时数量
#define IO_KEY_SCAN_FILTER_TIME_MS      (30) // 按键消抖延时

// 给【指向按键事件处理函数的指针类型】起别名
typedef void (*io_key_handle_func_t)(void);

extern volatile key_driver_param_t io_scan_param; 

void io_key_init(void);
uint8_t io_convert_key_event(uint8_t key_driver_val, uint8_t key_driver_event);

#endif
