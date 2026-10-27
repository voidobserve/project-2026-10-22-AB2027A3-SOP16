#include "io_key_driver.h"

#include "key_driver.h"

// 获取按键键值的函数声明
static uint8_t get_key_value(void);
volatile key_driver_param_t io_scan_param = {
    .scan_times = IO_KEY_SCAN_TIME_MS, // 按键扫描频率, 单位: ms
    .last_key = NO_KEY,                // 上一次get_value按键值, 初始化为NO_KEY;
    .filter_time = IO_KEY_SCAN_FILTER_TIME_MS, // 按键消抖延时;

    // 按键判定长按数量
    .long_time = IO_KEY_LONG_TIME_MS / IO_KEY_SCAN_TIME_MS,
    // 按键判定HOLD数量
    .hold_time =
        (IO_KEY_LONG_TIME_MS + IO_KEY_HOLD_TIME_MS) / IO_KEY_SCAN_TIME_MS,
    // 按键被抬起后等待连击延时数量
    .click_delay_time = IO_KEY_SCAN_CLICK_DELAY_TIME_MS,
    .key_type = KEY_TYPE_IO,
    .get_value = get_key_value,
};

const uint8_t io_key_table[][2] = {
    {IO_KEY_VAL_KEY1, IO_KEY_EVENT_KEY1_CLICK},
};

void io_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_pin = IO_KEY_PIN;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init(IO_KEY_PORT, &gpio_init_structure);
}

static uint8_t get_key_value(void)
{
    uint8_t key_val = NO_KEY;

    if (0 == gpio_read_bit(IO_KEY_PORT, IO_KEY_PIN)) {
        key_val = IO_KEY_VAL_KEY1;
    } else {
    }

#if USER_DEBUG_ENABLE
    // printf("key val == %u\n", (uint16_t)key_val);
#endif

    return key_val;
}

uint8_t io_convert_key_event(uint8_t key_driver_val, uint8_t key_driver_event)
{
    uint8_t i;
    uint8_t key_event = IO_KEY_EVENT_NONE;

    if (key_driver_event == KEY_EVENT_CLICK) {
        for (i = 0; i < sizeof(io_key_table) / sizeof(io_key_table[0]); i++) {
            if (io_key_table[i][0] == key_driver_val) {
                key_event = io_key_table[i][1];
                break;
            }
        }
    } else {
        // 未使用的按键事件
    }

    return key_event;
}