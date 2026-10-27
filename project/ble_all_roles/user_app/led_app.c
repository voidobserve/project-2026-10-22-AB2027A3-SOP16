#include "led_app.h"

#include "user_config.h"
#include "led_driver/led_transport_spi.h"

/*
 * 文件说明：
 * 本文件是遥控器/机械按键与动画层之间的应用状态层。
 * 负责电源、亮度、模式切换、Timer、NVM 延迟保存和逐灯输出。
 */

/* 5 挡亮度：20/40/60/80/100% 对应的 0~255 缩放值 */
#define BRIGHTNESS_LEVEL_COUNT 5U
static const uint8_t brightness_scale_table[BRIGHTNESS_LEVEL_COUNT] = {
    49U, 99U, 149U, 199U, 255U,
};

/* NVM 延迟保存时间 */
#define NVM_SAVE_DELAY_MS 2000U

/* Timer 实际倒计时 */
#define TIMER_2H_DURATION_MS 7200000UL
#define TIMER_4H_DURATION_MS 14400000UL

/* Timer 提示输出类型 */
typedef enum {
    TIMER_OUT_OFF = 0,
    TIMER_OUT_BASE,
    TIMER_OUT_RED
} timer_output_type_t;

typedef struct {
    uint16_t duration_ms;
    uint8_t output;
} timer_step_t;

/*
 * Timer 提示按“单次按键”拆开分析：
 * - 2h：1 次提示 = 关灯 500 ms + 红色 500 ms，然后回原本模式；
 * - 4h：1 次提示 = 关灯/红/关灯/红，共 2 次红色，然后回原本模式。
 *
 * 原始 2h 采集里的 3 次红色，间隔 2.35 s / 1.55 s 不一致，
 * 是连续按了 3 次 2h；原始 4h 采集里的 4 次红色分成两对，
 * 中间间隔 2.85 s，是连续按了 2 次 4h。单次按键不应播放整段长序列。
 */
static const timer_step_t timer_2h_flash_steps[] = {
    {500U, TIMER_OUT_OFF},
    {500U, TIMER_OUT_RED},
};

static const timer_step_t timer_4h_flash_steps[] = {
    {500U, TIMER_OUT_OFF},
    {500U, TIMER_OUT_RED},
    {500U, TIMER_OUT_OFF},
    {500U, TIMER_OUT_RED},
};

/* 当前 1 ms 时间，遥控 handler 通过它取得同一帧时间 */
static uint32_t led_app_current_time_ms;
/* 1 表示当前开灯，0 表示关灯 */
static uint8_t led_app_power_on_flag;
/* 全局亮度挡位：0~4，对应 20%~100% */
static uint8_t led_app_brightness_level;
/* 当前静态单色索引，用于掉电恢复 */
static uint8_t led_app_static_color_index;
/* 当前图标索引，用于掉电恢复 */
static uint8_t led_app_icon_index;
/* 1 表示 NVM 有未保存变化 */
static uint8_t led_app_save_dirty_flag;
/* NVM 延迟保存的到期时间 */
static uint32_t led_app_save_due_time_ms;
/* Modes 的 H 状态按到关灯后，下一次开灯从 Modes 索引 0 重新开始 */
static uint8_t led_app_off_from_modes_flag;

/* TMR3 ISR 读取：关灯时每 1 ms 释放一次驱动信号量，避免 driver 线程空转 */
volatile uint8_t led_app_off_interrupt_flag;

/* Timer 是否已启动，以及当前是 2h 还是 4h */
static uint8_t led_app_timer_active_flag;
static app_timer_t led_app_timer_kind;
static uint32_t led_app_timer_start_time_ms;

/**
 * @brief 当前是否静态类模式（使用全局亮度）
 */
static bool led_app_is_static_mode(led_animation_mode_t mode)
{
    return (mode == LED_ANIMATION_MODE_STATIC_COLOR) ||
           (mode == LED_ANIMATION_MODE_ICON) ||
           (mode == LED_ANIMATION_MODE_WHITE_SELECT);
}

/**
 * @brief 按亮度挡位缩放一个通道
 */
static uint8_t led_app_brightness_apply(uint8_t channel_value)
{
    uint8_t brightness_level_index = led_app_brightness_level;

    if (brightness_level_index >= BRIGHTNESS_LEVEL_COUNT) {
        brightness_level_index = BRIGHTNESS_LEVEL_COUNT - 1U;
    }

    return (uint8_t)(((uint16_t)channel_value *
                          brightness_scale_table[brightness_level_index] +
                      127U) /
                     255U);
}

/**
 * @brief 标记 NVM 需要延迟保存
 */
static void led_app_mark_save(uint32_t now_ms)
{
    led_app_save_dirty_flag = 1U;
    led_app_save_due_time_ms = now_ms + NVM_SAVE_DELAY_MS;
}

/**
 * @brief 将当前状态同步到 user_data，供延迟保存使用
 */
static void led_app_sync_user_data(void)
{
    led_animation_mode_t mode = led_animation_get_mode();

    user_data.valid = USER_DATA_VALID_VAL;
    user_data.version = USER_DATA_VERSION;
    user_data.power = led_app_power_on_flag ? 1U : 0U;
    user_data.mode = (uint8_t)mode;

    if (mode == LED_ANIMATION_MODE_STATIC_COLOR) {
        user_data.color = led_app_static_color_index;
    } else if (mode == LED_ANIMATION_MODE_ICON) {
        user_data.color = led_app_icon_index;
    } else {
        user_data.color = 0U;
    }
}

/**
 * @brief 恢复 NVM 中的模式/颜色
 * @return true 表示 NVM 有效并已恢复
 */
static bool led_app_restore_user_data(uint32_t now_ms)
{
    led_animation_mode_t mode;

    if ((user_data.valid != USER_DATA_VALID_VAL) ||
        (user_data.version != USER_DATA_VERSION)) {
        return false;
    }

    mode = (led_animation_mode_t)user_data.mode;
    if (mode > LED_ANIMATION_MODE_WHITE_SELECT) {
        return false;
    }

    if (mode == LED_ANIMATION_MODE_STATIC_COLOR) {
        if (user_data.color >= (uint8_t)LED_STATIC_COLOR_COUNT) {
            led_app_static_color_index = (uint8_t)LED_STATIC_RED;
        } else {
            led_app_static_color_index = user_data.color;
        }
        led_animation_set_static_color_index(led_app_static_color_index);
        led_animation_set_mode(LED_ANIMATION_MODE_STATIC_COLOR, now_ms);
    } else if (mode == LED_ANIMATION_MODE_ICON) {
        if (user_data.color >= (uint8_t)LED_ICON_COUNT) {
            led_app_icon_index = (uint8_t)LED_ICON_US_FLAG;
        } else {
            led_app_icon_index = user_data.color;
        }
        led_animation_set_icon(led_app_icon_index);
        led_animation_set_mode(LED_ANIMATION_MODE_ICON, now_ms);
    } else if (mode == LED_ANIMATION_MODE_FADE) {
        led_animation_set_mode(LED_ANIMATION_MODE_FADE, now_ms);
    } else if (mode == LED_ANIMATION_MODE_STROBE) {
        led_animation_set_mode(LED_ANIMATION_MODE_STROBE, now_ms);
    } else if (mode == LED_ANIMATION_MODE_MODES) {
        led_animation_modes_enter(now_ms);
    } else if (mode == LED_ANIMATION_MODE_WHITE_SELECT) {
        /* White Select 的颜色索引不保存，上电从白色开始 */
        led_animation_set_white_color_index((uint8_t)LED_WHITE_WHITE);
        led_animation_set_mode(LED_ANIMATION_MODE_WHITE_SELECT, now_ms);
    } else {
        return false;
    }

    return true;
}

/**
 * @brief 首次上电/存储无效时的默认值：RED、开灯、100%
 */
static void led_app_set_default(uint32_t now_ms)
{
    led_app_power_on_flag = 1U;
    led_app_brightness_level = BRIGHTNESS_LEVEL_COUNT - 1U;
    led_app_static_color_index = (uint8_t)LED_STATIC_RED;
    led_app_icon_index = (uint8_t)LED_ICON_US_FLAG;

    led_animation_set_static_color_index(led_app_static_color_index);
    led_animation_set_mode(LED_ANIMATION_MODE_STATIC_COLOR, now_ms);

    user_data.valid = USER_DATA_VALID_VAL;
    user_data.version = USER_DATA_VERSION;
    user_data.power = 1U;
    user_data.mode = (uint8_t)LED_ANIMATION_MODE_STATIC_COLOR;
    user_data.color = led_app_static_color_index;

    led_app_mark_save(now_ms);
}

/**
 * @brief 启动 timer 提示/倒计时
 * @note 同一个 timer 重复按忽略；2h/4h 之间切换会重播一次提示，
 *       并把倒计时重置为新选择的时长。
 */
void led_app_timer_start(app_timer_t timer_type, uint32_t now_ms)
{
    if (led_app_power_on_flag == 0U) {
        return;
    }

    /* 同一个 timer 重复按不响应；2h/4h 之间切换则重新计时并重播提示 */
    if ((led_app_timer_active_flag != 0U) &&
        (led_app_timer_kind == timer_type)) {
        return;
    }

    led_app_timer_kind = timer_type;
    led_app_timer_start_time_ms = now_ms;
    led_app_timer_active_flag = 1U;
}

/**
 * @brief 取 timer 提示的当前覆盖类型
 * @param now_ms 当前 1 ms 时间
 * @return OFF / RED / BASE
 * @note 2h 闪 1 次，4h 闪 2 次；每种 timer 每次按键只跑一遍。
 */
static timer_output_type_t led_app_get_timer_prompt_output(uint32_t now_ms)
{
    uint32_t timer_elapsed_ms;
    uint32_t elapsed_accumulated_ms = 0U;
    uint8_t timer_step_index;
    uint8_t timer_step_count;
    const timer_step_t *timer_steps;

    if (led_app_timer_active_flag == 0U) {
        return TIMER_OUT_BASE;
    }

    timer_elapsed_ms = now_ms - led_app_timer_start_time_ms;

    if (led_app_timer_kind == APP_TIMER_2H) {
        timer_steps = timer_2h_flash_steps;
        timer_step_count = (uint8_t)(sizeof(timer_2h_flash_steps) /
                                     sizeof(timer_2h_flash_steps[0]));
    } else {
        timer_steps = timer_4h_flash_steps;
        timer_step_count = (uint8_t)(sizeof(timer_4h_flash_steps) /
                                     sizeof(timer_4h_flash_steps[0]));
    }

    for (timer_step_index = 0U; timer_step_index < timer_step_count;
         timer_step_index++) {
        elapsed_accumulated_ms += timer_steps[timer_step_index].duration_ms;
        if (timer_elapsed_ms < elapsed_accumulated_ms) {
            return (timer_output_type_t)timer_steps[timer_step_index].output;
        }
    }

    return TIMER_OUT_BASE;
}

/**
 * @brief 初始化应用层
 */
void led_app_init(uint32_t now_ms)
{
    bool is_user_data_valid;

    led_app_current_time_ms = now_ms;
    led_app_power_on_flag = 1U;
    led_app_brightness_level = BRIGHTNESS_LEVEL_COUNT - 1U;
    led_app_static_color_index = (uint8_t)LED_STATIC_RED;
    led_app_icon_index = (uint8_t)LED_ICON_US_FLAG;
    led_app_save_dirty_flag = 0U;
    led_app_save_due_time_ms = now_ms;
    led_app_off_from_modes_flag = 0U;
    led_app_timer_active_flag = 0U;
    led_app_timer_kind = APP_TIMER_2H;
    led_app_timer_start_time_ms = now_ms;

    led_animation_init(now_ms);
    user_data_read();

    is_user_data_valid = led_app_restore_user_data(now_ms);
    if (!is_user_data_valid) {
        led_app_set_default(now_ms);
    } else {
        led_app_power_on_flag = (user_data.power != 0U) ? 1U : 0U;
    }

    if (led_app_power_on_flag != 0U) {
        led_app_off_interrupt_flag = 0U;
        led_transport_spi_power_on();
    } else {
        led_app_off_interrupt_flag = 1U;
        led_transport_spi_power_off();
        led_animation_pause(now_ms);
    }

    led_app_sync_user_data();
}

/**
 * @brief 更新应用层当前时间
 */
void led_app_set_current_time_ms(uint32_t now_ms)
{
    led_app_current_time_ms = now_ms;
}

/**
 * @brief 当前时间
 */
uint32_t led_app_get_current_time_ms(void)
{
    return led_app_current_time_ms;
}

/**
 * @brief 查询是否开灯
 */
bool led_app_power_is_on(void)
{
    return (led_app_power_on_flag != 0U);
}

/**
 * @brief 关灯：暂停动画、控制脚低、清除 timer、延迟保存开关状态
 * @note 关灯后 2h/4h 定时取消，重新开灯不会恢复之前的倒计时。
 */
static void led_app_power_off(uint32_t now_ms)
{
    if (led_app_power_on_flag == 0U) {
        led_app_timer_active_flag = 0U;
        led_app_timer_start_time_ms = now_ms;
        return;
    }

    led_app_power_on_flag = 0U;
    led_app_off_interrupt_flag = 1U;
    led_animation_pause(now_ms);
    led_transport_spi_power_off();

    /* 关灯即取消 2h/4h 定时，避免关灯期间继续倒计时 */
    led_app_timer_active_flag = 0U;
    led_app_timer_start_time_ms = now_ms;

    user_data.power = 0U;
    led_app_mark_save(now_ms);
}

/**
 * @brief 开灯：控制脚高、恢复动画、延迟保存开关状态
 * @note 如果是 Modes 的 H 按到关灯，开灯时从 Modes 索引 0 重新开始；
 *       其他关灯原因则从暂停处继续。
 */
static void led_app_power_on(uint32_t now_ms)
{
    if (led_app_power_on_flag != 0U) {
        return;
    }

    led_app_power_on_flag = 1U;

    /* 关灯期间 TMR3 可能已经留下一次信号量；先消费掉，避免开灯后
       第一帧误以为 DMA 完成而过早启动下一帧。 */
    if (led_app_off_interrupt_flag != 0U) {
        led_app_off_interrupt_flag = 1U;
        thread_driver_sem_take();
    }

    /* 关中断再清标志，避免 TMR3 在这一小段又留下一次信号量 */
    GLOBAL_INT_DISABLE();
    led_app_off_interrupt_flag = 0U;
    GLOBAL_INT_RESTORE();

    led_transport_spi_power_on();

    if (led_app_off_from_modes_flag != 0U) {
        /* Modes 的 H 关灯后，R1C1/机械开灯都从索引 0 重新开始 */
        led_app_off_from_modes_flag = 0U;
        led_animation_resume(now_ms);
        led_animation_modes_enter(now_ms);
        user_data.mode = (uint8_t)LED_ANIMATION_MODE_MODES;
        user_data.color = 0U;
    } else {
        led_animation_resume(now_ms);
    }

    user_data.power = 1U;
    led_app_mark_save(now_ms);
}

/**
 * @brief R1C1 开关切换
 */
void led_app_power_toggle(uint32_t now_ms)
{
    if (led_app_power_on_flag != 0U) {
        led_app_power_off(now_ms);
    } else {
        led_app_power_on(now_ms);
    }
}

/**
 * @brief 机械按键开灯入口：只负责开灯，机械模式/索引由机械代码设置
 * @note 建议在 LED driver 线程/用户主循环中调用，不要在中断里调用。
 */
void led_app_power_on_external(uint32_t now_ms)
{
    led_app_power_on(now_ms);
}

/**
 * @brief 切换 Fade/Strobe
 */
void led_app_set_mode(led_animation_mode_t mode, uint32_t now_ms)
{
    if ((led_app_power_on_flag == 0U) ||
        ((mode != LED_ANIMATION_MODE_FADE) &&
         (mode != LED_ANIMATION_MODE_STROBE))) {
        return;
    }

    if (led_animation_get_mode() == mode) {
        return;
    }

    led_animation_set_mode(mode, now_ms);
    user_data.mode = (uint8_t)mode;
    user_data.color = 0U;
    led_app_mark_save(now_ms);
}

/**
 * @brief 切换静态单色
 */
void led_app_set_static_color(uint8_t color_index, uint32_t now_ms)
{
    if (led_app_power_on_flag == 0U) {
        return;
    }

    if (color_index >= (uint8_t)LED_STATIC_COLOR_COUNT) {
        color_index = (uint8_t)LED_STATIC_RED;
    }

    if ((led_animation_get_mode() == LED_ANIMATION_MODE_STATIC_COLOR) &&
        (led_app_static_color_index == color_index)) {
        return;
    }

    led_app_static_color_index = color_index;
    led_animation_set_static_color_index(color_index);
    led_animation_set_mode(LED_ANIMATION_MODE_STATIC_COLOR, now_ms);
    user_data.mode = (uint8_t)LED_ANIMATION_MODE_STATIC_COLOR;
    user_data.color = color_index;
    led_app_mark_save(now_ms);
}

/**
 * @brief 切换图标
 */
void led_app_set_icon(uint8_t icon, uint32_t now_ms)
{
    if (led_app_power_on_flag == 0U) {
        return;
    }

    if (icon >= (uint8_t)LED_ICON_COUNT) {
        icon = (uint8_t)LED_ICON_US_FLAG;
    }

    if ((led_animation_get_mode() == LED_ANIMATION_MODE_ICON) &&
        (led_app_icon_index == icon)) {
        return;
    }

    led_app_icon_index = icon;
    led_animation_set_icon(icon);
    led_animation_set_mode(LED_ANIMATION_MODE_ICON, now_ms);
    user_data.mode = (uint8_t)LED_ANIMATION_MODE_ICON;
    user_data.color = icon;
    led_app_mark_save(now_ms);
}

/**
 * @brief Modes 每次 PRESS 前进；H 后关灯
 * @note H 关灯后，下一次 R1C1/机械开灯从 Modes 索引 0 重新开始。
 */
void led_app_modes_next(uint32_t now_ms)
{
    if (led_app_power_on_flag == 0U) {
        return;
    }

    if (led_animation_get_mode() != LED_ANIMATION_MODE_MODES) {
        led_animation_modes_enter(now_ms);
        user_data.mode = (uint8_t)LED_ANIMATION_MODE_MODES;
        user_data.color = 0U;
        led_app_mark_save(now_ms);
        return;
    }

    if (led_animation_modes_is_h()) {
        led_app_off_from_modes_flag = 1U;
        led_app_power_off(now_ms);
        return;
    }

    led_animation_modes_next(now_ms);
}

/**
 * @brief White Select：从其他模式进入先选白色，之后每次循环
 */
void led_app_white_color_next(uint32_t now_ms)
{
    uint8_t white_color_index;

    if (led_app_power_on_flag == 0U) {
        return;
    }

    if (led_animation_get_mode() != LED_ANIMATION_MODE_WHITE_SELECT) {
        led_animation_set_white_color_index((uint8_t)LED_WHITE_WHITE);
        led_animation_set_mode(LED_ANIMATION_MODE_WHITE_SELECT, now_ms);
        user_data.mode = (uint8_t)LED_ANIMATION_MODE_WHITE_SELECT;
        user_data.color = 0U;
        led_app_mark_save(now_ms);
        return;
    }

    white_color_index = led_animation_get_white_color_index();
    white_color_index =
        (uint8_t)((white_color_index + 1U) % (uint8_t)LED_WHITE_COUNT);
    led_animation_set_white_color_index(white_color_index);
}

/**
 * @brief 亮度 5 挡循环
 */
void led_app_brightness_next(void)
{
    if (led_app_power_on_flag == 0U) {
        return;
    }

    led_app_brightness_level =
        (uint8_t)((led_app_brightness_level + 1U) % BRIGHTNESS_LEVEL_COUNT);
}

/**
 * @brief 逐灯取最终输出
 */
void led_app_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                            led_color_t *output_color)
{
    led_animation_mode_t mode;
    timer_output_type_t timer_output;

    if (led_app_power_on_flag == 0U) {
        output_color->r = 0U;
        output_color->g = 0U;
        output_color->b = 0U;
        output_color->w = 0U;
        return;
    }

    led_animation_get_lamp_color(now_ms, lamp, output_color);

    mode = led_animation_get_mode();
    if (led_app_is_static_mode(mode)) {
        output_color->r = led_app_brightness_apply(output_color->r);
        output_color->g = led_app_brightness_apply(output_color->g);
        output_color->b = led_app_brightness_apply(output_color->b);
        output_color->w = led_app_brightness_apply(output_color->w);
    }

    timer_output = led_app_get_timer_prompt_output(now_ms);
    if (timer_output == TIMER_OUT_OFF) {
        output_color->r = 0U;
        output_color->g = 0U;
        output_color->b = 0U;
        output_color->w = 0U;
    } else if (timer_output == TIMER_OUT_RED) {
        output_color->r = 255U;
        output_color->g = 0U;
        output_color->b = 0U;
        output_color->w = 0U;
    }
}

/**
 * @brief 每帧轮询：timer 到点、NVM 延迟保存
 */
void led_app_poll(uint32_t now_ms)
{
    uint32_t timer_duration_ms;

    led_app_current_time_ms = now_ms;

    if (led_app_timer_active_flag != 0U) {
        timer_duration_ms = (led_app_timer_kind == APP_TIMER_2H)
                                ? TIMER_2H_DURATION_MS
                                : TIMER_4H_DURATION_MS;
        if ((uint32_t)(now_ms - led_app_timer_start_time_ms) >=
            timer_duration_ms) {
            led_app_timer_active_flag = 0U;
            if (led_app_power_on_flag != 0U) {
                led_app_power_off(now_ms);
            }
        }
    }

    if ((led_app_save_dirty_flag != 0U) &&
        ((int32_t)(now_ms - led_app_save_due_time_ms) >= 0)) {
        led_app_sync_user_data();
        user_data_write();
        led_app_save_dirty_flag = 0U;
    }
}
