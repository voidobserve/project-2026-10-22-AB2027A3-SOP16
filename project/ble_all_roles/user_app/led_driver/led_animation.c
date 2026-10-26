#include "led_animation.h"

/* 每段渐变时间：2304 ms */
#define FADE_SEG_MS 2304U
/* 关键帧数量 */
#define FADE_KF_NUM 8U
/* 一个完整循环：8 × 2304 = 18432 ms */
#define FADE_CYCLE_MS (FADE_KF_NUM * FADE_SEG_MS)

/* fade 关键帧：红、黄、绿、青、蓝、品红、红、白 */
static const led_color_t kFadeKeyframes[FADE_KF_NUM] = {
    {255U, 0U, 0U, 0U},   // 红
    {255U, 255U, 0U, 0U}, // 黄
    {0U, 255U, 0U, 0U},   // 绿
    {0U, 255U, 255U, 0U}, // 青
    {0U, 0U, 255U, 0U},   // 蓝
    {255U, 0U, 255U, 0U}, // 品红
    {255U, 0U, 0U, 0U},   // 红
    {255U, 255U, 255U, 0U}, // 白
};

/* 当前动画模式 */
static led_anim_mode_t s_mode;
/* 当前动画起始时间 */
static uint32_t s_start_ms;
/* 静态模式颜色 */
static led_color_t s_static_color;

/**
 * @brief 整数线性插值
 * @param from 起始值
 * @param to 目标值
 * @param pos 当前段内位置，0~FADE_SEG_MS
 * @return 插值结果
 */
static uint8_t fade_lerp(uint8_t from, uint8_t to, uint32_t pos)
{
    int32_t delta = (int32_t)to - (int32_t)from; // 通道差值
    int32_t value = delta * (int32_t)pos;        // 当前增量

    if (value >= 0) {
        value += (int32_t)FADE_SEG_MS / 2;
    } else {
        value -= (int32_t)FADE_SEG_MS / 2;
    }

    return (uint8_t)((int32_t)from + value / (int32_t)FADE_SEG_MS);
}

/**
 * @brief 计算 fade 当前颜色
 * @param now_ms 当前 1 ms 时间
 * @param out 输出颜色
 * @return 无
 */
static void fade_get_color(uint32_t now_ms, led_color_t *out)
{
    uint32_t elapsed = now_ms - s_start_ms; // 距动画开始的毫秒数
    uint32_t t = elapsed % FADE_CYCLE_MS;   // 循环内时间
    uint32_t seg = t / FADE_SEG_MS;         // 当前段索引
    uint32_t pos = t - seg * FADE_SEG_MS;   // 段内位置
    const led_color_t *from = &kFadeKeyframes[seg];
    const led_color_t *to = &kFadeKeyframes[(seg + 1U) % FADE_KF_NUM];

    out->r = fade_lerp(from->r, to->r, pos);
    out->g = fade_lerp(from->g, to->g, pos);
    out->b = fade_lerp(from->b, to->b, pos);
    out->w = 0U;
}

/**
 * @brief 初始化动画层
 * @param now_ms 当前 1 ms 时间
 * @return 无
 */
void led_anim_init(uint32_t now_ms)
{
    s_mode = LED_ANIM_OFF;
    s_start_ms = now_ms;
    s_static_color.r = 255U;
    s_static_color.g = 0U;
    s_static_color.b = 0U;
    s_static_color.w = 0U;
}

/**
 * @brief 切换动画模式
 * @param mode 目标模式
 * @param now_ms 当前 1 ms 时间
 * @return 无
 * @note 只重置动画起点，不修改协议层灯号游标
 */
void led_anim_set_mode(led_anim_mode_t mode, uint32_t now_ms)
{
    s_mode = mode;
    s_start_ms = now_ms;
}

/**
 * @brief 设置静态模式颜色
 * @param color 指向 100% 基准颜色的指针
 * @return 无
 */
void led_anim_set_static_color(const led_color_t *color)
{
    s_static_color = *color;
}

/**
 * @brief 获取指定灯当前颜色
 * @param now_ms 当前 1 ms 时间
 * @param lamp_idx 灯号 1~12
 * @param out 输出颜色
 * @return 无
 * @note 每帧发送前调用，所以颜色变化会在下一帧生效
 */
void led_anim_get_lamp_color(uint32_t now_ms, uint8_t lamp_idx, led_color_t *out)
{
    (void)lamp_idx; // 当前模式与灯号无关，后续图标模式再使用

    switch (s_mode) {
        case LED_ANIM_STATIC_COLOR:
            *out = s_static_color;
            break;

        case LED_ANIM_FADE:
            fade_get_color(now_ms, out);
            break;

        case LED_ANIM_OFF:
        default:
            out->r = 0U;
            out->g = 0U;
            out->b = 0U;
            out->w = 0U;
            break;
    }
}
