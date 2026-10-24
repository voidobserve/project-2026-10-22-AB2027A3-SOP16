#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "include.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define USER_DEBUG_ENABLE 1

// 校验，用于验证是不是第一次上电，之前写入的数据是否有效
#define USER_DATA_VALID_VAL        0xC5
#define USER_DATA_SAVE_INTERVAL_MS 2000 // 每次保存数据的间隔时间

// 需要掉电保存的数据
typedef struct __attribute__((packed))
{
    u8 valid; // 校验，用于验证是不是第一次上电，之前写入的数据是否有效
} user_data_t;

extern volatile user_data_t user_data;

void user_data_write(void);
void user_data_read(void);

void user_init(void);
void user_main(void);

#if 0
void ble_user_server_message_deal(u8 *buffer, u16 len);
#endif

#if USER_DEBUG_ENABLE
#include "user_debug_io.h"
#endif

#endif