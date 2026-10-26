#include "user_config.h"
#include "driver_uart.h"
#include "driver_tmr.h"
#include "led_driver/led_transport_spi.h"
#include "led_driver/led_protocol.h"
#include "led_driver/led_animation.h"

// #include "driver_gpio.h" // 初始化 测试时使用的gpio，需要引用该头文件

volatile user_data_t user_data = {0};

#define USER_DATE_SAVE_START_ADDR 0x00 // 起始地址

/* 1 ms 定时器中断生产的节拍计数，中断与主循环共享，必须 volatile */
static volatile uint32_t led_timer_tick = 0;

AT(.com_text.isr)
void timer3_isq(void)
{
    if (tmr_get_flag(TMR3, TMR_IT_UPDATE) != RESET) {
        tmr_clear_flag(TMR3, TMR_IT_UPDATE);
        led_timer_tick++;
        // USER_DEBUG_IO_TOGGLE();
    }
}

/**
 * @brief 初始化 1 ms 硬件定时器
 * @param 无
 * @return 无
 * 
 */
static void tmr3_init(void)
{
    tmr_base_init_typedef timer_init_struct; // 定时器初始化参数

    clk_gate1_cmd(CLK_GATE1_TMR3, CLK_EN);

    /* TMR3 用系统时钟计数（SYS_160M = 160 MHz），不是 clk_tmr_inc_get() 的 TMR_INC（1 MHz）
       中断周期 = (prescale + 1) * (period + 1) / 计数时钟
       1600 分频后每 10 us 计数一次，计 100 次 = 1 ms
       注意：以下固定值与 SYS_160M 对应，改 SYS_CLK_SEL 需重新计算 */
    timer_init_struct.clock_source = TMR_SYSTEM_CLOCK;
    timer_init_struct.counter_source = TMR_INC_SOURCE;
    /* TMR3 计数时钟：160 MHz / 1600 = 100 kHz，即 10 us 一个计数 */
    timer_init_struct.prescale = 1600U - 1U;
    /* 1 ms = 100 × 10 us */
    timer_init_struct.period = 100U - 1U;
    tmr_base_init(TMR3, &timer_init_struct);

    tmr_pic_config(TMR3, timer3_isq, 0, TMR_IT_UPDATE, ENABLE);
    tmr_cmd(TMR3, ENABLE);
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
    led_transport_spi_frame_init();
    led_protocol_init();
    led_anim_init(0);
    led_anim_set_mode(LED_ANIM_FADE, 0);
    tmr3_init();

    delay_ms(10);
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
    uint32_t now;                   // 当前 1 ms 时间
    uint8_t lamp_idx;               // 当前灯号
    led_color_t color;              // 当前灯颜色
    uint8_t frame[LED_FRAME_BYTES]; // 协议帧

#if 1
    led_transport_spi_frame_poll();

    if (!led_transport_spi_frame_ready()) {
        return;
    }

    now = led_timer_tick;
    lamp_idx = led_protocol_current_lamp();
    led_anim_get_lamp_color(now, lamp_idx, &color);
    led_protocol_build_frame(lamp_idx, &color, frame);

    led_transport_spi_frame_submit(frame);

    led_protocol_advance_lamp();
#endif
}
