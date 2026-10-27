#ifndef __RF24G_REMOTER_DRIVER_H__
#define __RF24G_REMOTER_DRIVER_H__

#include "api_btstack.h"

#include "key_driver.h"

//
/* 定义按键键值
	实际测试发现，客户的遥控器按下后，先发 (0x8F & 键值)
	过一小段时间后，遥控器发送的是：0x80 | 键值

	哪怕只短按了一下，也是这种发送规律，
	短按刚开始，最高位是0，低7位是键值
	一小段时间后，最高位是1，低7位是键值
*/
enum
{
    RF24G_KEY_VAL_R1C1 = 0x0E, // 第一行第一列按键

    RF24G_KEY_VAL_R2C1 = 0x10, // 第二行第一列按键
    RF24G_KEY_VAL_R2C2 = 0x0F,
    RF24G_KEY_VAL_R2C3 = 0x0D,

    RF24G_KEY_VAL_R3C1 = 0x12, // 第三行第一列按键
    RF24G_KEY_VAL_R3C2 = 0x18,
    RF24G_KEY_VAL_R3C3 = 0x17,

    RF24G_KEY_VAL_R4C1 = 0x1E,
    RF24G_KEY_VAL_R4C2 = 0x0C,
    RF24G_KEY_VAL_R4C3 = 0x06,

    RF24G_KEY_VAL_R5C1 = 0x1D,
    RF24G_KEY_VAL_R5C2 = 0x0B,
    RF24G_KEY_VAL_R5C3 = 0x05,

    RF24G_KEY_VAL_R6C1 = 0x1C,
    RF24G_KEY_VAL_R6C2 = 0x0A,
    RF24G_KEY_VAL_R6C3 = 0x04,
    RF24G_KEY_VAL_R6C4 = 0x16,

    RF24G_KEY_VAL_R7C1 = 0x1B,
    RF24G_KEY_VAL_R7C2 = 0x09,
    RF24G_KEY_VAL_R7C3 = 0x03,
    RF24G_KEY_VAL_R7C4 = 0x15,

    RF24G_KEY_VAL_R8C1 = 0x1A,
    RF24G_KEY_VAL_R8C2 = 0x08,
    RF24G_KEY_VAL_R8C3 = 0x02,
    RF24G_KEY_VAL_R8C4 = 0x14,

    RF24G_KEY_VAL_R9C1 = 0x19,
    RF24G_KEY_VAL_R9C2 = 0x07,
    RF24G_KEY_VAL_R9C3 = 0x01,
    RF24G_KEY_VAL_R9C4 = 0x13,
};

// 定义按键事件
enum
{
    RF24G_KEY_EVENT_R1C1_PRESS = 0x00,

    RF24G_KEY_EVENT_R2C1_PRESS,
    RF24G_KEY_EVENT_R2C2_PRESS,
    RF24G_KEY_EVENT_R2C3_PRESS,

    RF24G_KEY_EVENT_R3C1_PRESS,
    RF24G_KEY_EVENT_R3C2_PRESS,
    RF24G_KEY_EVENT_R3C3_PRESS,

    RF24G_KEY_EVENT_R4C1_PRESS,
    RF24G_KEY_EVENT_R4C2_PRESS,
    RF24G_KEY_EVENT_R4C3_PRESS,

    RF24G_KEY_EVENT_R5C1_PRESS,
    RF24G_KEY_EVENT_R5C2_PRESS,
    RF24G_KEY_EVENT_R5C3_PRESS,

    RF24G_KEY_EVENT_R6C1_PRESS,
    RF24G_KEY_EVENT_R6C2_PRESS,
    RF24G_KEY_EVENT_R6C3_PRESS,
    RF24G_KEY_EVENT_R6C4_PRESS,

    RF24G_KEY_EVENT_R7C1_PRESS,
    RF24G_KEY_EVENT_R7C2_PRESS,
    RF24G_KEY_EVENT_R7C3_PRESS,
    RF24G_KEY_EVENT_R7C4_PRESS,

    RF24G_KEY_EVENT_R8C1_PRESS,
    RF24G_KEY_EVENT_R8C2_PRESS,
    RF24G_KEY_EVENT_R8C3_PRESS,
    RF24G_KEY_EVENT_R8C4_PRESS,

    RF24G_KEY_EVENT_R9C1_PRESS,
    RF24G_KEY_EVENT_R9C2_PRESS,
    RF24G_KEY_EVENT_R9C3_PRESS,
    RF24G_KEY_EVENT_R9C4_PRESS,

    RF24G_KEY_EVENT_NONE, // 无按键事件/无效按键事件
    RF24G_KEY_EVENT_MAX,
};

#define RF24G_KEY_SCAN_TIME_MS (10) // 2.4G遥控器按键扫描频率，单位：ms
#define RF24G_KEY_LONG_TIME_MS (1500)
#define RF24G_KEY_HOLD_TIME_MS (500)
#define RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS                                     \
    (0) // 2.4G遥控器按键被抬起后等待连击延时数量
#define RF24G_KEY_SCAN_FILTER_TIME_MS (0) // 2.4G遥控器按键消抖延时

// 给【指向遥控器按键事件处理函数的指针类型】起别名
typedef void (*rf24_key_handle_func_t)(void);

extern volatile key_driver_param_t rf24g_scan_para;
extern volatile uint8_t rf24g_key_driver_value;
extern volatile uint8_t rf24g_key_driver_event;
// extern const rf24_key_handle_func_t rf24_28keys_handle_func_buff[RF24G_28_KEY_EVENT_MAX];

void rf24g_remoter_data_process(adv_report_t *adv_report);

uint8_t rf24g_convert_key_event(uint8_t key_driver_val,
                                uint8_t key_driver_event);

#endif
