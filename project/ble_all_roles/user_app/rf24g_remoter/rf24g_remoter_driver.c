#include "rf24g_remoter_driver.h"
#include <stdint.h>
#include "global.h" // SDK的类型定义、宏函数定义

static volatile u8 flag_rf24g_key_val_update = 0; // 是否收到了新的数据
volatile u8 rf24g_key_val = NO_KEY;               // 存放按键键值

volatile u8 rf24g_key_driver_event = 0; // 由key_driver_scan() 更新
volatile u8 rf24g_key_driver_value = 0; // 由key_driver_scan() 更新

// 获取按键键值的函数声明
static uint8_t get_key_value(void);
volatile key_driver_param_t rf24g_scan_para = {
    .scan_times = RF24G_KEY_SCAN_TIME_MS, // 按键扫描频率, 单位: ms
    .last_key = NO_KEY, // 上一次get_value按键值, 初始化为NO_KEY;
    .filter_time = RF24G_KEY_SCAN_FILTER_TIME_MS, // 按键消抖延时;

    // 按键判定长按数量
    .long_time = RF24G_KEY_LONG_TIME_MS / RF24G_KEY_SCAN_TIME_MS,
    // 按键判定HOLD数量
    .hold_time = (RF24G_KEY_LONG_TIME_MS + RF24G_KEY_HOLD_TIME_MS) /
                 RF24G_KEY_SCAN_TIME_MS,
    // 按键被抬起后等待连击延时数量
    .click_delay_time = RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS,
    .key_type = KEY_TYPE_24G_REMOTE,
    .get_value = get_key_value,
};

const uint8_t rf24g_key_table[][2] = {
    {RF24G_KEY_VAL_R1C1, RF24G_KEY_EVENT_R1C1_PRESS},

    {RF24G_KEY_VAL_R2C1, RF24G_KEY_EVENT_R2C1_PRESS},
    {RF24G_KEY_VAL_R2C2, RF24G_KEY_EVENT_R2C2_PRESS},
    {RF24G_KEY_VAL_R2C3, RF24G_KEY_EVENT_R2C3_PRESS},

    {RF24G_KEY_VAL_R3C1, RF24G_KEY_EVENT_R3C1_PRESS},
    {RF24G_KEY_VAL_R3C2, RF24G_KEY_EVENT_R3C2_PRESS},
    {RF24G_KEY_VAL_R3C3, RF24G_KEY_EVENT_R3C3_PRESS},

    {RF24G_KEY_VAL_R4C1, RF24G_KEY_EVENT_R4C1_PRESS},
    {RF24G_KEY_VAL_R4C2, RF24G_KEY_EVENT_R4C2_PRESS},
    {RF24G_KEY_VAL_R4C3, RF24G_KEY_EVENT_R4C3_PRESS},

    {RF24G_KEY_VAL_R5C1, RF24G_KEY_EVENT_R5C1_PRESS},
    {RF24G_KEY_VAL_R5C2, RF24G_KEY_EVENT_R5C2_PRESS},
    {RF24G_KEY_VAL_R5C3, RF24G_KEY_EVENT_R5C3_PRESS},

    {RF24G_KEY_VAL_R6C1, RF24G_KEY_EVENT_R6C1_PRESS},
    {RF24G_KEY_VAL_R6C2, RF24G_KEY_EVENT_R6C2_PRESS},
    {RF24G_KEY_VAL_R6C3, RF24G_KEY_EVENT_R6C3_PRESS},
    {RF24G_KEY_VAL_R6C4, RF24G_KEY_EVENT_R6C4_PRESS},

    {RF24G_KEY_VAL_R7C1, RF24G_KEY_EVENT_R7C1_PRESS},
    {RF24G_KEY_VAL_R7C2, RF24G_KEY_EVENT_R7C2_PRESS},
    {RF24G_KEY_VAL_R7C3, RF24G_KEY_EVENT_R7C3_PRESS},
    {RF24G_KEY_VAL_R7C4, RF24G_KEY_EVENT_R7C4_PRESS},

    {RF24G_KEY_VAL_R8C1, RF24G_KEY_EVENT_R8C1_PRESS},
    {RF24G_KEY_VAL_R8C2, RF24G_KEY_EVENT_R8C2_PRESS},
    {RF24G_KEY_VAL_R8C3, RF24G_KEY_EVENT_R8C3_PRESS},
    {RF24G_KEY_VAL_R8C4, RF24G_KEY_EVENT_R8C4_PRESS},

    {RF24G_KEY_VAL_R9C1, RF24G_KEY_EVENT_R9C1_PRESS},
    {RF24G_KEY_VAL_R9C2, RF24G_KEY_EVENT_R9C2_PRESS},
    {RF24G_KEY_VAL_R9C3, RF24G_KEY_EVENT_R9C3_PRESS},
    {RF24G_KEY_VAL_R9C4, RF24G_KEY_EVENT_R9C4_PRESS},
};

/**
 * @brief 处理接收到的数据，更新键值
 * 
 * @param adv_report 
 */
void rf24g_remoter_data_process(adv_report_t *adv_report)
{
    if (17 == adv_report->report_len && adv_report->report[0] == 0x10 &&
        adv_report->report[1] == 0xFF) {
        // printf("evt_type %x, addr_type %x, Adv Addr:", adv_report->event_type,
        //        adv_report->addr_type);
        // print_r(adv_report->addr, 6);
        // printf("Adv Data: %d###", adv_report->report_len);
        // print_r(adv_report->report, adv_report->report_len);
        // printf("adv_report->rssi == %d\n", (int)(int8_t)adv_report->rssi);

        // printf("key val == %02x\n", adv_report->report[16]);
        rf24g_key_val = adv_report->report[16];
        flag_rf24g_key_val_update = 1;
    }
}

uint8_t get_key_value(void)
{
    uint8_t ret_key_val = NO_KEY;
    /*
     * 按键键值保持（可重触发的保持计数）
     * 遥控器的键值随广播包到达，包与包之间存在间隔。收到键值后，
     * 在保持时间内持续上报同一键值，填补无数据的空档，避免键值在
     * 有效值与 NO_KEY 之间跳变，使按键状态机把一次按键误判为多次
     * 按下、连续触发对应的 press 事件。每收到一次新键值都重新装载
     * 计数（可重触发），持续按住时不会中途误判为松开。
     */
    static uint8_t key_keep_cnt = 0;
    static uint8_t last_key_val = 0;

    if (flag_rf24g_key_val_update) {
        flag_rf24g_key_val_update = 0;

        ret_key_val = rf24g_key_val;
        // 按键扫描周期 RF24G_KEY_SCAN_TIME_MS(10ms) 调用一次本函数，
        // 装载 10 次计数，即键值保持 10 * 10ms = 100ms
        key_keep_cnt = 10;
        last_key_val = ret_key_val;
        return ret_key_val;
    }

    // 保持计数未耗尽：继续返回上次的键值；耗尽后返回 NO_KEY，视为按键松开
    if (key_keep_cnt > 0) {
        key_keep_cnt--;
        return last_key_val;
    }

    return ret_key_val;
}

uint8_t rf24g_convert_key_event(uint8_t key_driver_val,
                                uint8_t key_driver_event)
{
    uint8_t i;
    uint8_t rf24g_key_event = RF24G_KEY_EVENT_NONE;

    if (key_driver_event == KEY_EVENT_PRESS) {
        for (i = 0; i < sizeof(rf24g_key_table) / sizeof(rf24g_key_table[0]);
             i++) {
            if (key_driver_val == rf24g_key_table[i][0]) {
                rf24g_key_event = rf24g_key_table[i][1];
                break;
            }
        }
    } else {
        // 未使用的按键事件
    }

    return rf24g_key_event;
}