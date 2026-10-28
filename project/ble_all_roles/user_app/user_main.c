/*
 * 文件说明：
 * 用户应用入口。当前版本中 user_main() 由 LED driver 线程循环调用，
 * 负责按键扫描、应用层轮询和逐灯发送 LED 帧。
 */

#include "user_config.h"
#include "driver_uart.h"
#include "driver_tmr.h"
#include "led_driver/led_transport_spi.h"
#include "led_driver/led_protocol.h"
#include "led_driver/led_animation.h"
#include "led_app.h"

#include "key_driver.h"
#include "rf24g_remoter_driver.h"
#include "rf24g_remoter_apply.h"

#include "io_key_driver.h"
#include "io_key_apply.h"


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

        // 2.4G遥控器按键扫描的计时器
        if (rf24g_scan_para.cur_scan_times < UINT8_MAX) {
            rf24g_scan_para.cur_scan_times++;
        }

        // io按键扫描的计时器
        if (io_scan_param.cur_scan_times < UINT8_MAX) {
            io_scan_param.cur_scan_times++;
        }

        /* 关灯后让 driver 线程每 1 ms 阻塞等待，避免高优先级空转压住蓝牙 */
        if (led_app_off_interrupt_flag != 0U) {
            thread_driver_sem_release();
        }

        // TEST ONLY
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

/* driver 线程的 LED 帧消息 */
#define LED_THREAD_MSG_TICK 0x81U

/* LED 线程回调是否注册成功 */
static volatile uint8_t led_thread_registered;
/* LED 线程是否已经启动，避免重复投递 */
static volatile uint8_t led_thread_started;

/**
 * @brief driver 线程的 LED 事件回调
 * @param msg 消息号
 * @return 无
 * @note 每发完一帧再投递下一帧，避免阻塞 driver 线程消息循环
 */
AT(.com_text.thread.driver)
static void led_thread_event_callback(uint8_t msg)
{
    if (msg != LED_THREAD_MSG_TICK) {
        return;
    }

    user_main();

    /* 继续投递下一帧；失败则退回主循环，避免灯效完全停止 */
    if (!thread_driver_msg_post(LED_THREAD_MSG_TICK)) {
        led_thread_registered = 0;
    }
}

/**
 * @brief 启动 LED 独立线程
 * @param 无
 * @return 无
 * @note 蓝牙开启时在蓝牙初始化完成后调用；蓝牙关闭时在 user_init 中调用
 */
void led_thread_start(void)
{
    if ((led_thread_registered == 0U) || (led_thread_started != 0U)) {
        return;
    }

    if (thread_driver_msg_post(LED_THREAD_MSG_TICK)) {
        led_thread_started = 1;
    } else {
        /* 投递失败时退回主循环，避免灯效完全停止 */
        led_thread_registered = 0;
    }
}

/**
 * @brief 查询 LED 独立线程是否可用
 * @param 无
 * @return true 可独立运行；false 注册失败，需要主循环兜底
 */
bool led_thread_available(void)
{
    return (led_thread_registered != 0U);
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
#if USER_DEBUG_ENABLE
    user_debug_io_init();
#endif

    io_key_init();

    led_transport_spi_init();
    led_transport_spi_frame_init();
    led_protocol_init();
    tmr3_init();

    /* 读取 NVM，恢复开关/模式/颜色，并初始化动画状态 */
    led_app_init(led_timer_tick);

    /* 注册 driver 线程回调；蓝牙开启时等蓝牙初始化完成后再启动 */
    led_thread_registered = (uint8_t)thread_driver_user_callback_register(
        led_thread_event_callback);

#if !FUNC_BT_EN
    /* 蓝牙关闭时直接启动 LED 独立线程 */
    led_thread_start();
#endif

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

// 由 driver 线程的 LED 消息回调调用
// 会循环投递，所以该函数内部不用写 while(1)
void user_main(void)
{
    uint32_t current_time_ms;                 // 当前 1 ms 时间
    uint8_t current_lamp_index;               // 当前要发送的灯号
    led_color_t lamp_color;                   // 当前灯最终颜色
    uint8_t protocol_frame[LED_FRAME_BYTES];  // 协议帧

    current_time_ms = led_timer_tick;
    led_app_set_current_time_ms(current_time_ms);

    /* 2.4G 遥控器按键扫描与业务处理 */
    key_driver_scan(&rf24g_scan_para);
    rf24g_remoter_apply();

    /* 机械按键扫描与业务处理；机械按键代码由用户维护 */
    key_driver_scan(&io_scan_param);
    io_key_apply();

    /* timer 到点、NVM 延迟保存；关灯状态下也必须执行 */
    led_app_poll(current_time_ms);

    if (!led_app_power_is_on()) {
        /* TMR3 每 1 ms 释放一次信号量，driver 线程在这里让出 CPU */
        thread_driver_sem_take();
        return;
    }

#if 1
    led_transport_spi_frame_poll();

    if (!led_transport_spi_frame_ready()) {
        return;
    }

    current_lamp_index = led_protocol_current_lamp();
    led_app_get_lamp_color(current_time_ms, current_lamp_index, &lamp_color);
    led_protocol_build_frame(current_lamp_index, &lamp_color, protocol_frame);

    led_transport_spi_frame_submit(protocol_frame);

    led_protocol_advance_lamp();
#endif
}
