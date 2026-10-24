#include "user_config.h"
#include "driver_uart.h"
#include "driver_tmr.h"
#include "led_driver/led_transport_spi.h"

// #include "driver_gpio.h" // 初始化 测试时使用的gpio，需要引用该头文件

volatile user_data_t user_data = {0};

#define USER_DATE_SAVE_START_ADDR 0x00 // 起始地址

/* 测试定时器周期：10 ms */
#define TEST_LED_TIMER_PERIOD_MS     10U
/* 1 秒对应的毫秒数 */
#define TEST_LED_MS_PER_SECOND       1000U

/* 10 ms 定时器中断生产的节拍计数，中断与主循环共享，必须 volatile */
static volatile uint32_t test_led_timer_tick = 0;
/* 消费者上一次处理的节拍计数 */
static uint32_t test_led_last_tick = 0;

/**
 * @brief 10 ms 硬件定时器中断：生产者，只递增节拍计数
 * @param 无
 * @return 无
 */
AT(.com_text.isr)
void test_led_timer_isr(void)
{
    if (tmr_get_flag(TMR0, TMR_IT_UPDATE)) {
        tmr_clear_flag(TMR0, TMR_IT_UPDATE);
        test_led_timer_tick++;
    }
}

/**
 * @brief 初始化 10 ms 硬件定时器
 * @param 无
 * @return 无
 * @note TMR0 使用当前 TMR_INC 计数时钟，按实际频率折算 10 ms
 */
static void test_led_timer_init(void)
{
    tmr_base_init_typedef timer_init_struct; // TMR0 初始化参数
    uint32_t tmr_inc_hz; // TMR_INC 当前计数频率
    uint32_t tick_divider; // 10 ms 对应的计数分频值（1000 / 10）

    tmr_inc_hz = clk_tmr_inc_get(CLK_VALUE_MODE_FREQ);
    tick_divider = TEST_LED_MS_PER_SECOND / TEST_LED_TIMER_PERIOD_MS;
    if (tmr_inc_hz < tick_divider) {
        return; // 时钟频率异常，避免定时器产生过密中断
    }

    timer_init_struct.period = (tmr_inc_hz / tick_divider) - 1U;
    timer_init_struct.prescale = 0;
    timer_init_struct.counter_source = TMR_INC_SOURCE;
    timer_init_struct.clock_source = TMR_SYSTEM_CLOCK;

    clk_gate0_cmd(CLK_GATE0_TMR0, CLK_EN);
    tmr_base_init(TMR0, &timer_init_struct);
    tmr_pic_config(TMR0, test_led_timer_isr, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR0, ENABLE);
}

void user_data_write(void)
{
    bsp_param_write(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
    bsp_param_sync(); // 同步数据到flash
}

void user_data_read(void)
{
    bsp_param_read(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
#if USER_DEBUG_ENABLE
    // my_printf("save.color == 0x%lx\n", user_data.color);
#endif
}

// 初始化函数，在 bsp_sys.c -> bsp_sys_init() 中调用
void user_init(void)
{
#if 0
    user_data_read(); // 读取flash保存的数据
    if (user_data.valid != USER_DATA_VALID_VAL) {
        // 如果读出来的数据，校验不通过，说明是第一次上电
        // 初始化变量
        user_data.valid = USER_DATA_VALID_VAL;

        user_data_write(); // 将数据写回flash
#if USER_DEBUG_ENABLE
        my_printf("is first pwr on\n");
#endif
    } else {
#if USER_DEBUG_ENABLE
        my_printf("is not first pwr on\n");
#endif
    }
#endif

#if USER_DEBUG_ENABLE
    user_debug_io_init();
#endif

    led_transport_spi_init();
    test_led_timer_init();

    // delay_ms(10);
}

#if 0
// 处理蓝牙服务发送过来的数据
void ble_user_server_message_deal(u8 *buff, u16 len)
{
    if (len < 6) {
        return;
    }
}
#endif

// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
// 会循环调用，所以该函数内部不用写 while(1)
void user_main(void)
{
    uint32_t tick; // 本次读取到的 10 ms 节拍

    tick = test_led_timer_tick;
    if (tick != test_led_last_tick) {
        test_led_last_tick = tick;
        /* 一次 12 帧约 36 ms，发送期间产生的 10 ms 节拍会合并到下一次 */
        test_led_transport_spi_send_static_12();
    }

    // printf("main\n");
    USER_DEBUG_IO_TOGGLE();
}
