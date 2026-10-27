#include "led_animation.h"

/*
 * 文件说明：
 * 本文件只负责“时间 + 灯号 -> RGBW”，不处理协议组帧、SPI 发送和按键。
 * 所有模式都按灯号取色：同一时刻 12 盏灯可能处于不同相位/图案。
 */

/* Fade：8 个关键帧，每段 2304 ms，18.432 s 一轮 */
#define FADE_SEGMENT_MS 2304U
#define FADE_KEYFRAME_COUNT 8U
#define FADE_CYCLE_MS (FADE_KEYFRAME_COUNT * FADE_SEGMENT_MS)

/* Strobe：7 色，每色 606 ms */
#define STROBE_COLOR_COUNT 7U
#define STROBE_STEP_MS 606U

/* 彩虹：6 色轮，每段与 Fade 一样 2304 ms */
#define RAINBOW_COLOR_COUNT 6U
#define RAINBOW_CYCLE_MS (RAINBOW_COLOR_COUNT * FADE_SEGMENT_MS)

/* Modes H 进入：H 初始图案与最终图案的逐灯切换时间 */
#define MODES_H_ENTRY_LAMP_COUNT 12U

/* Fade 8 个关键帧：红、黄、绿、青、蓝、品红、红、白 */
static const led_color_t fade_keyframe_colors[FADE_KEYFRAME_COUNT] = {
    {255U, 0U, 0U, 0U},     // 红
    {255U, 255U, 0U, 0U},   // 黄
    {0U, 255U, 0U, 0U},     // 绿
    {0U, 255U, 255U, 0U},   // 青
    {0U, 0U, 255U, 0U},     // 蓝
    {255U, 0U, 255U, 0U},   // 品红
    {255U, 0U, 0U, 0U},     // 红
    {255U, 255U, 255U, 0U}, // 白
};

/* 15 种静态单色，100% 基准，顺序与 led_static_color_id_t 一致 */
static const led_color_t static_color_table[LED_STATIC_COLOR_COUNT] = {
    {203U, 50U, 0U, 0U},   // 橙
    {0U, 203U, 50U, 0U},   // 绿(203)
    {50U, 0U, 203U, 0U},   // 蓝(203)
    {254U, 0U, 0U, 0U},    // 红
    {169U, 84U, 0U, 0U},   // 琥珀
    {0U, 169U, 84U, 0U},   // 青绿
    {84U, 0U, 169U, 0U},   // 紫
    {0U, 254U, 0U, 0U},    // 纯绿
    {145U, 108U, 0U, 0U},  // 米色
    {0U, 145U, 108U, 0U},  // 蓝绿
    {108U, 0U, 145U, 0U},  // 紫红
    {0U, 0U, 254U, 0U},    // 纯蓝
    {127U, 127U, 0U, 0U},  // 暖白
    {0U, 127U, 127U, 0U},  // 冷白
    {127U, 0U, 127U, 0U},  // 品红
};

/* 图标：美国国旗、三叶草、南瓜头、圣诞树 */
static const led_color_t us_flag_colors[3] = {
    {254U, 0U, 0U, 0U},   // 红
    {0U, 0U, 254U, 0U},   // 蓝
    {84U, 84U, 84U, 0U},  // 灰
};
static const led_color_t clover_colors[2] = {
    {0U, 254U, 0U, 0U},   // 绿
    {84U, 84U, 84U, 0U},  // 灰
};
static const led_color_t pumpkin_colors[2] = {
    {203U, 50U, 0U, 0U},  // 橙
    {127U, 0U, 127U, 0U}, // 紫
};
static const led_color_t christmas_colors[2] = {
    {254U, 0U, 0U, 0U},   // 红
    {0U, 254U, 0U, 0U},   // 绿
};

/* White Select：白、冷白、浅蓝 */
static const led_color_t white_color_table[LED_WHITE_COUNT] = {
    {0U, 0U, 0U, 254U},
    {0U, 0U, 127U, 127U},
    {0U, 0U, 84U, 169U},
};

/* Strobe 7 色 */
static const led_color_t strobe_color_table[STROBE_COLOR_COUNT] = {
    {255U, 0U, 0U, 0U},
    {128U, 128U, 0U, 0U},
    {0U, 255U, 0U, 0U},
    {0U, 128U, 128U, 0U},
    {0U, 0U, 255U, 0U},
    {200U, 10U, 50U, 0U},
    {85U, 85U, 85U, 0U},
};

/* 彩虹 6 色轮 */
static const led_color_t rainbow_color_table[RAINBOW_COLOR_COUNT] = {
    {255U, 0U, 0U, 0U},
    {255U, 255U, 0U, 0U},
    {0U, 255U, 0U, 0U},
    {0U, 255U, 255U, 0U},
    {0U, 0U, 255U, 0U},
    {255U, 0U, 255U, 0U},
};

/* Modes A~G 静态图案：A 3 色、B/C 6 色、D/E/F 2 色、G 7 色 */
static const led_color_t modes_a_colors[3] = {
    {149U, 0U, 0U, 0U},   // 红
    {0U, 149U, 0U, 0U},   // 绿
    {0U, 0U, 0U, 149U},   // 暖白
};
static const led_color_t modes_b_colors[6] = {
    {117U, 5U, 29U, 0U},  // 粉红
    {0U, 85U, 63U, 0U},   // 青绿
    {0U, 0U, 149U, 0U},   // 蓝
    {49U, 49U, 49U, 0U},  // 灰
    {75U, 75U, 0U, 0U},   // 黄
    {0U, 119U, 29U, 0U},  // 绿
};
static const led_color_t modes_c_colors[6] = {
    {0U, 0U, 0U, 149U},   // 暖白
    {0U, 75U, 75U, 0U},   // 青
    {0U, 149U, 0U, 0U},   // 绿
    {75U, 0U, 75U, 0U},   // 紫
    {117U, 5U, 29U, 0U},  // 粉红
    {75U, 75U, 0U, 0U},   // 黄
};
static const led_color_t modes_d_colors[2] = {
    {0U, 0U, 149U, 0U},   // 蓝
    {149U, 0U, 0U, 0U},   // 红
};
static const led_color_t modes_e_colors[2] = {
    {149U, 0U, 0U, 0U},   // 红
    {0U, 0U, 0U, 149U},   // 暖白
};
static const led_color_t modes_f_colors[2] = {
    {0U, 0U, 149U, 0U},   // 蓝
    {0U, 0U, 0U, 149U},   // 暖白
};
static const led_color_t modes_g_colors[7] = {
    {119U, 29U, 0U, 0U},  // 橙
    {75U, 75U, 0U, 0U},   // 黄
    {0U, 149U, 0U, 0U},   // 绿
    {0U, 75U, 75U, 0U},   // 青
    {0U, 0U, 149U, 0U},   // 蓝
    {117U, 5U, 29U, 0U},  // 粉红
    {149U, 0U, 0U, 0U},   // 红
};

/* H 最终图案 */
static const led_color_t modes_h_final_colors[MODES_H_ENTRY_LAMP_COUNT] = {
    {0U, 127U, 127U, 0U},
    {0U, 0U, 254U, 0U},
    {254U, 0U, 0U, 0U},
    {203U, 50U, 0U, 0U},
    {0U, 254U, 0U, 0U},
    {0U, 0U, 254U, 0U},
    {254U, 0U, 0U, 0U},
    {127U, 127U, 0U, 0U},
    {0U, 127U, 127U, 0U},
    {199U, 9U, 49U, 0U},
    {203U, 50U, 0U, 0U},
    {0U, 254U, 0U, 0U},
};

/* H 进入的初始图案 */
static const led_color_t modes_h_initial_colors[MODES_H_ENTRY_LAMP_COUNT] = {
    {0U, 254U, 0U, 0U},
    {0U, 127U, 127U, 0U},
    {199U, 9U, 49U, 0U},
    {254U, 0U, 0U, 0U},
    {127U, 127U, 0U, 0U},
    {0U, 127U, 127U, 0U},
    {199U, 9U, 49U, 0U},
    {203U, 50U, 0U, 0U},
    {0U, 254U, 0U, 0U},
    {0U, 0U, 254U, 0U},
    {254U, 0U, 0U, 0U},
    {127U, 127U, 0U, 0U},
};

/* H 初始图案逐灯切换时间（相对 H 起始，ms） */
static const uint16_t modes_h_initial_offset_ms[MODES_H_ENTRY_LAMP_COUNT] = {
    10U, 14U, 17U, 20U, 23U, 26U, 30U, 33U, 37U, 0U, 3U, 6U,
};

/* H 最终图案逐灯切换时间（相对 H 起始，ms） */
static const uint16_t modes_h_final_offset_ms[MODES_H_ENTRY_LAMP_COUNT] = {
    257U, 421U, 465U, 551U, 596U, 682U, 764U, 974U, 1018U, 1062U, 1066U, 1110U,
};

/* 当前模式、时间、静态色/图标/White/Modes 状态 */
static led_animation_mode_t led_animation_mode; // 当前顶层模式
static uint32_t led_animation_start_time_ms;    // 当前模式/相位的起点
static uint8_t led_animation_static_color_index; // 当前静态单色索引
static uint8_t led_animation_icon_index;         // 当前图标索引
static uint8_t led_animation_white_color_index;  // 当前 White Select 索引
static uint8_t led_animation_modes_state;        // Modes 的 0~8 状态
static uint8_t led_animation_paused_flag;        // 1 表示动画已暂停
static uint32_t led_animation_pause_time_ms;     // 暂停发生的时刻

/**
 * @brief 整数线性插值
 * @param start_color 起始值
 * @param end_color 目标值
 * @param segment_position 段内位置
 * @param segment_duration_ms 段长
 * @return 插值结果
 */
static uint8_t interpolate_segment(uint8_t start_color, uint8_t end_color,
                                   uint32_t segment_position,
                                   uint32_t segment_duration_ms)
{
    int32_t channel_delta = (int32_t)end_color - (int32_t)start_color;
    int32_t interpolated_value = channel_delta * (int32_t)segment_position;

    if (interpolated_value >= 0) {
        interpolated_value += (int32_t)segment_duration_ms / 2;
    } else {
        interpolated_value -= (int32_t)segment_duration_ms / 2;
    }

    return (uint8_t)((int32_t)start_color +
                     interpolated_value / (int32_t)segment_duration_ms);
}

/**
 * @brief 取 Fade 当前颜色
 */
static void fade_get_lamp_color(uint32_t now_ms, led_color_t *output_color)
{
    uint32_t elapsed_time_ms = now_ms - led_animation_start_time_ms;
    uint32_t cycle_position_ms = elapsed_time_ms % FADE_CYCLE_MS;
    uint32_t segment_index = cycle_position_ms / FADE_SEGMENT_MS;
    uint32_t segment_position =
        cycle_position_ms - segment_index * FADE_SEGMENT_MS;
    const led_color_t *start_color = &fade_keyframe_colors[segment_index];
    const led_color_t *end_color =
        &fade_keyframe_colors[(segment_index + 1U) % FADE_KEYFRAME_COUNT];

    output_color->r = interpolate_segment(start_color->r, end_color->r,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->g = interpolate_segment(start_color->g, end_color->g,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->b = interpolate_segment(start_color->b, end_color->b,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->w = 0U;
}

/**
 * @brief 取 Strobe 当前颜色
 */
static void strobe_get_lamp_color(uint32_t now_ms, led_color_t *output_color)
{
    uint32_t elapsed_time_ms = now_ms - led_animation_start_time_ms;
    uint32_t color_index =
        (elapsed_time_ms / STROBE_STEP_MS) % STROBE_COLOR_COUNT;

    *output_color = strobe_color_table[color_index];
}

/**
 * @brief 取彩虹当前颜色，按灯号错开相位
 */
static void rainbow_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                                   led_color_t *output_color)
{
    uint32_t phase_offset_ms =
        ((uint32_t)(lamp - 1U) * RAINBOW_CYCLE_MS) /
        (uint32_t)LED_LAMP_COUNT;
    uint32_t cycle_position_ms =
        (now_ms - led_animation_start_time_ms + phase_offset_ms) %
        RAINBOW_CYCLE_MS;
    uint32_t segment_index = cycle_position_ms / FADE_SEGMENT_MS;
    uint32_t segment_position =
        cycle_position_ms - segment_index * FADE_SEGMENT_MS;
    const led_color_t *start_color = &rainbow_color_table[segment_index];
    const led_color_t *end_color =
        &rainbow_color_table[(segment_index + 1U) % RAINBOW_COLOR_COUNT];

    output_color->r = interpolate_segment(start_color->r, end_color->r,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->g = interpolate_segment(start_color->g, end_color->g,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->b = interpolate_segment(start_color->b, end_color->b,
                                          segment_position, FADE_SEGMENT_MS);
    output_color->w = 0U;
}

/**
 * @brief 取 Modes A~G 的静态图案颜色
 */
static void modes_get_static_lamp_color(uint8_t state, uint8_t lamp,
                                         led_color_t *output_color)
{
    uint8_t lamp_index = (uint8_t)(lamp - 1U);

    switch (state) {
        case LED_MODES_A:
            *output_color = modes_a_colors[lamp_index % 3U];
            break;
        case LED_MODES_B:
            *output_color = modes_b_colors[lamp_index % 6U];
            break;
        case LED_MODES_C:
            *output_color = modes_c_colors[lamp_index % 6U];
            break;
        case LED_MODES_D:
            *output_color = modes_d_colors[lamp_index % 2U];
            break;
        case LED_MODES_E:
            *output_color = modes_e_colors[lamp_index % 2U];
            break;
        case LED_MODES_F:
            *output_color = modes_f_colors[lamp_index % 2U];
            break;
        case LED_MODES_G:
            *output_color = modes_g_colors[lamp_index % 7U];
            break;
        default:
            output_color->r = 0U;
            output_color->g = 0U;
            output_color->b = 0U;
            output_color->w = 0U;
            break;
    }
}

/**
 * @brief 取 Modes H 进入/最终颜色
 * @note H 进入前先保留 G 的颜色，再按逐灯时间切到 H 初始/最终色。
 */
static void modes_get_h_lamp_color(uint8_t lamp, uint32_t now_ms,
                                   led_color_t *output_color)
{
    uint8_t lamp_index = (uint8_t)(lamp - 1U);
    uint32_t elapsed_time_ms = now_ms - led_animation_start_time_ms;

    if (elapsed_time_ms < modes_h_initial_offset_ms[lamp_index]) {
        *output_color = modes_g_colors[lamp_index % 7U];
    } else if (elapsed_time_ms < modes_h_final_offset_ms[lamp_index]) {
        *output_color = modes_h_initial_colors[lamp_index];
    } else {
        *output_color = modes_h_final_colors[lamp_index];
    }
}

/**
 * @brief 初始化动画层
 */
void led_animation_init(uint32_t now_ms)
{
    led_animation_mode = LED_ANIMATION_MODE_OFF;
    led_animation_start_time_ms = now_ms;
    led_animation_static_color_index = (uint8_t)LED_STATIC_RED;
    led_animation_icon_index = (uint8_t)LED_ICON_US_FLAG;
    led_animation_white_color_index = (uint8_t)LED_WHITE_WHITE;
    led_animation_modes_state = LED_MODES_RAINBOW;
    led_animation_paused_flag = 0U;
    led_animation_pause_time_ms = now_ms;
}

/**
 * @brief 切换顶层模式
 */
void led_animation_set_mode(led_animation_mode_t mode, uint32_t now_ms)
{
    led_animation_mode = mode;
    led_animation_start_time_ms = now_ms;
}

/**
 * @brief 取当前顶层模式
 */
led_animation_mode_t led_animation_get_mode(void)
{
    return led_animation_mode;
}

/**
 * @brief 设置静态单色索引
 */
void led_animation_set_static_color_index(uint8_t static_color_index)
{
    if (static_color_index >= (uint8_t)LED_STATIC_COLOR_COUNT) {
        static_color_index = (uint8_t)LED_STATIC_RED;
    }
    led_animation_static_color_index = static_color_index;
}

/**
 * @brief 设置图标索引
 */
void led_animation_set_icon(uint8_t icon_index)
{
    if (icon_index >= (uint8_t)LED_ICON_COUNT) {
        icon_index = (uint8_t)LED_ICON_US_FLAG;
    }
    led_animation_icon_index = icon_index;
}

/**
 * @brief 设置 White Select 索引
 */
void led_animation_set_white_color_index(uint8_t white_color_index)
{
    if (white_color_index >= (uint8_t)LED_WHITE_COUNT) {
        white_color_index = (uint8_t)LED_WHITE_WHITE;
    }
    led_animation_white_color_index = white_color_index;
}

/**
 * @brief 取 White Select 索引
 */
uint8_t led_animation_get_white_color_index(void)
{
    return led_animation_white_color_index;
}

/**
 * @brief 进入 Modes，从索引 0 彩虹开始
 */
void led_animation_modes_enter(uint32_t now_ms)
{
    led_animation_mode = LED_ANIMATION_MODE_MODES;
    led_animation_modes_state = LED_MODES_RAINBOW;
    led_animation_start_time_ms = now_ms;
}

/**
 * @brief Modes 前进一个索引，最大到 H
 */
void led_animation_modes_next(uint32_t now_ms)
{
    if (led_animation_modes_state < LED_MODES_H) {
        led_animation_modes_state++;
    }
    led_animation_start_time_ms = now_ms;
}

/**
 * @brief 当前 Modes 索引
 */
uint8_t led_animation_get_modes_state(void)
{
    return led_animation_modes_state;
}

/**
 * @brief 当前是否 Modes H
 */
bool led_animation_modes_is_h(void)
{
    return (led_animation_mode == LED_ANIMATION_MODE_MODES) &&
           (led_animation_modes_state == LED_MODES_H);
}

/**
 * @brief 暂停动画
 */
void led_animation_pause(uint32_t now_ms)
{
    if (led_animation_paused_flag == 0U) {
        led_animation_pause_time_ms = now_ms;
        led_animation_paused_flag = 1U;
    }
}

/**
 * @brief 恢复动画
 */
void led_animation_resume(uint32_t now_ms)
{
    if (led_animation_paused_flag != 0U) {
        led_animation_start_time_ms += now_ms - led_animation_pause_time_ms;
        led_animation_paused_flag = 0U;
    }
}

/**
 * @brief 当前是否暂停
 */
bool led_animation_is_paused(void)
{
    return (led_animation_paused_flag != 0U);
}

/**
 * @brief 获取指定灯当前颜色（100% 基准）
 */
void led_animation_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                                  led_color_t *output_color)
{
    uint8_t lamp_index;

    if ((lamp == 0U) || (lamp > (uint8_t)LED_LAMP_COUNT)) {
        lamp = 1U;
    }
    lamp_index = (uint8_t)(lamp - 1U);

    switch (led_animation_mode) {
        case LED_ANIMATION_MODE_STATIC_COLOR:
            *output_color =
                static_color_table[led_animation_static_color_index];
            break;

        case LED_ANIMATION_MODE_FADE:
            fade_get_lamp_color(now_ms, output_color);
            break;

        case LED_ANIMATION_MODE_STROBE:
            strobe_get_lamp_color(now_ms, output_color);
            break;

        case LED_ANIMATION_MODE_MODES:
            if (led_animation_modes_state == LED_MODES_RAINBOW) {
                rainbow_get_lamp_color(now_ms, lamp, output_color);
            } else if (led_animation_modes_state == LED_MODES_H) {
                modes_get_h_lamp_color(lamp, now_ms, output_color);
            } else {
                modes_get_static_lamp_color(led_animation_modes_state, lamp,
                                            output_color);
            }
            break;

        case LED_ANIMATION_MODE_ICON:
            switch (led_animation_icon_index) {
                case LED_ICON_US_FLAG:
                    *output_color = us_flag_colors[lamp_index % 3U];
                    break;
                case LED_ICON_CLOVER:
                    *output_color = clover_colors[lamp_index % 2U];
                    break;
                case LED_ICON_PUMPKIN:
                    *output_color = pumpkin_colors[lamp_index % 2U];
                    break;
                case LED_ICON_CHRISTMAS:
                    *output_color = christmas_colors[lamp_index % 2U];
                    break;
                default:
                    *output_color = us_flag_colors[lamp_index % 3U];
                    break;
            }
            break;

        case LED_ANIMATION_MODE_WHITE_SELECT:
            *output_color = white_color_table[led_animation_white_color_index];
            break;

        case LED_ANIMATION_MODE_OFF:
        default:
            output_color->r = 0U;
            output_color->g = 0U;
            output_color->b = 0U;
            output_color->w = 0U;
            break;
    }
}
