#include "rf24g_remoter_apply.h"
#include "rf24g_remoter_driver.h"

extern const rf24_key_handle_func_t
    rf24g_key_event_handle_table[RF24G_KEY_EVENT_MAX - 1];
/**
 * @brief 2.4g遥控器应用层
 * 
 * @return * void 
 */
void rf24g_remoter_apply(void)
{
    uint8_t rf24g_key_event;
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = NULL;
    rf24g_key_event = rf24g_convert_key_event(rf24g_scan_para.latest_key_val,
                                              rf24g_scan_para.latest_key_event);
    rf24g_scan_para.latest_key_val = NO_KEY; // 获取到按键事件后，清空按键值
    if (RF24G_KEY_EVENT_NONE == rf24g_key_event) {
        return;
    }

    rf24g_key_handle_func_ptr = rf24g_key_event_handle_table[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) {
        return;
    }

    // 按键处理函数
    rf24g_key_handle_func_ptr();
}

void rf24g_key_event_r1c1_press_handle(void)
{
    printf("r1c1 press\n");
}

void rf24g_key_event_r2c1_press_handle(void)
{
    printf("r2c1 press\n");
}

void rf24g_key_event_r2c2_press_handle(void)
{
    printf("r2c2 press\n");
}

void rf24g_key_event_r2c3_press_handle(void)
{
    printf("r2c3 press\n");
}

void rf24g_key_event_r3c1_press_handle(void)
{
    printf("r3c1 press\n");
}

void rf24g_key_event_r3c2_press_handle(void)
{
    printf("r3c2 press\n");
}

void rf24g_key_event_r3c3_press_handle(void)
{
    printf("r3c3 press\n");
}

void rf24g_key_event_r4c1_press_handle(void)
{
    printf("r4c1 press\n");
}

void rf24g_key_event_r4c2_press_handle(void)
{
    printf("r4c2 press\n");
}

void rf24g_key_event_r4c3_press_handle(void)
{
    printf("r4c3 press\n");
}

void rf24g_key_event_r5c1_press_handle(void)
{
    printf("r5c1 press\n");
}

void rf24g_key_event_r5c2_press_handle(void)
{
    printf("r5c2 press\n");
}

void rf24g_key_event_r5c3_press_handle(void)
{
    printf("r5c3 press\n");
}

void rf24g_key_event_r6c1_press_handle(void)
{
    printf("r6c1 press\n");
}

void rf24g_key_event_r6c2_press_handle(void)
{
    printf("r6c2 press\n");
}

void rf24g_key_event_r6c3_press_handle(void)
{
    printf("r6c3 press\n");
}

void rf24g_key_event_r6c4_press_handle(void)
{
    printf("r6c4 press\n");
}

void rf24g_key_event_r7c1_press_handle(void)
{
    printf("r7c1 press\n");
}

void rf24g_key_event_r7c2_press_handle(void)
{
    printf("r7c2 press\n");
}

void rf24g_key_event_r7c3_press_handle(void)
{
    printf("r7c3 press\n");
}

void rf24g_key_event_r7c4_press_handle(void)
{
    printf("r7c4 press\n");
}

void rf24g_key_event_r8c1_press_handle(void)
{
    printf("r8c1 press\n");
}

void rf24g_key_event_r8c2_press_handle(void)
{
    printf("r8c2 press\n");
}

void rf24g_key_event_r8c3_press_handle(void)
{
    printf("r8c3 press\n");
}

void rf24g_key_event_r8c4_press_handle(void)
{
    printf("r8c4 press\n");
}

void rf24g_key_event_r9c1_press_handle(void)
{
    printf("r9c1 press\n");
}

void rf24g_key_event_r9c2_press_handle(void)
{
    printf("r9c2 press\n");
}

void rf24g_key_event_r9c3_press_handle(void)
{
    printf("r9c3 press\n");
}

void rf24g_key_event_r9c4_press_handle(void)
{
    printf("r9c4 press\n");
}

/*
 * @brief 按键事件处理函数表
 * RF24G_KEY_EVENT_MAX - 1: 有效的按键事件数量，这里减一代表减去了 RF24G_KEY_EVENT_NONE
*/
const rf24_key_handle_func_t
    rf24g_key_event_handle_table[RF24G_KEY_EVENT_MAX - 1] = {
        [RF24G_KEY_EVENT_R1C1_PRESS] = rf24g_key_event_r1c1_press_handle,

        [RF24G_KEY_EVENT_R2C1_PRESS] = rf24g_key_event_r2c1_press_handle,
        [RF24G_KEY_EVENT_R2C2_PRESS] = rf24g_key_event_r2c2_press_handle,
        [RF24G_KEY_EVENT_R2C3_PRESS] = rf24g_key_event_r2c3_press_handle,

        [RF24G_KEY_EVENT_R3C1_PRESS] = rf24g_key_event_r3c1_press_handle,
        [RF24G_KEY_EVENT_R3C2_PRESS] = rf24g_key_event_r3c2_press_handle,
        [RF24G_KEY_EVENT_R3C3_PRESS] = rf24g_key_event_r3c3_press_handle,

        [RF24G_KEY_EVENT_R4C1_PRESS] = rf24g_key_event_r4c1_press_handle,
        [RF24G_KEY_EVENT_R4C2_PRESS] = rf24g_key_event_r4c2_press_handle,
        [RF24G_KEY_EVENT_R4C3_PRESS] = rf24g_key_event_r4c3_press_handle,

        [RF24G_KEY_EVENT_R5C1_PRESS] = rf24g_key_event_r5c1_press_handle,
        [RF24G_KEY_EVENT_R5C2_PRESS] = rf24g_key_event_r5c2_press_handle,
        [RF24G_KEY_EVENT_R5C3_PRESS] = rf24g_key_event_r5c3_press_handle,

        [RF24G_KEY_EVENT_R6C1_PRESS] = rf24g_key_event_r6c1_press_handle,
        [RF24G_KEY_EVENT_R6C2_PRESS] = rf24g_key_event_r6c2_press_handle,
        [RF24G_KEY_EVENT_R6C3_PRESS] = rf24g_key_event_r6c3_press_handle,
        [RF24G_KEY_EVENT_R6C4_PRESS] = rf24g_key_event_r6c4_press_handle,

        [RF24G_KEY_EVENT_R7C1_PRESS] = rf24g_key_event_r7c1_press_handle,
        [RF24G_KEY_EVENT_R7C2_PRESS] = rf24g_key_event_r7c2_press_handle,
        [RF24G_KEY_EVENT_R7C3_PRESS] = rf24g_key_event_r7c3_press_handle,
        [RF24G_KEY_EVENT_R7C4_PRESS] = rf24g_key_event_r7c4_press_handle,

        [RF24G_KEY_EVENT_R8C1_PRESS] = rf24g_key_event_r8c1_press_handle,
        [RF24G_KEY_EVENT_R8C2_PRESS] = rf24g_key_event_r8c2_press_handle,
        [RF24G_KEY_EVENT_R8C3_PRESS] = rf24g_key_event_r8c3_press_handle,
        [RF24G_KEY_EVENT_R8C4_PRESS] = rf24g_key_event_r8c4_press_handle,

        [RF24G_KEY_EVENT_R9C1_PRESS] = rf24g_key_event_r9c1_press_handle,
        [RF24G_KEY_EVENT_R9C2_PRESS] = rf24g_key_event_r9c2_press_handle,
        [RF24G_KEY_EVENT_R9C3_PRESS] = rf24g_key_event_r9c3_press_handle,
        [RF24G_KEY_EVENT_R9C4_PRESS] = rf24g_key_event_r9c4_press_handle,
};