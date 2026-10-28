# LED 三层架构与调用逻辑

本文以当前工程代码为准，描述 LED 灯串程序的**输入层、应用层、动画层、协议层、传输层**，以及调用顺序、数据流、电源/亮度/NVM/Timer 行为和时序约束。

> 早期文档里的 `led_anim_*`、`led_anim_mode_t` 已废弃；当前代码统一使用 `led_animation_*`、`led_animation_mode_t`。

---

## 0. 必须遵守的硬约束

1. **TMR1、TMR3、SPI 中断优先级必须都为 0**，不能把 TMR1 设为 1。
2. **灯串控制信号空闲时不能有负脉冲**。帧间隔高电平后必须直接接下一帧的 8~9 us 低电平，中间不能出现 0.5 us 等低脉冲。
3. 动画、应用层不在中断里执行；中断只做计数、引脚切换、置标志。
4. 不使用动态内存，不使用递归；所有缓冲区静态分配。

---

## 1. 分层

| 层次 | 文件 | 主要职责 | 不负责 |
|---|---|---|---|
| 输入层 | `key_driver.c`、`rf24g_remoter_driver.c`、`io_key_driver.c` | 扫描 2.4G 按键、机械按键，产生 CLICK/PRESS 事件 | 颜色、电源、NVM |
| 应用层 | `led_app.c/.h` | 电源、亮度、模式、Timer、NVM、机械索引、逐灯最终输出 | SPI 波形、协议位序 |
| 动画层 | `led_animation.c/.h` | 按绝对时间/机械索引计算每盏灯 100% 基准 RGBW | 协议帧、SPI、按键 |
| 协议层 | `led_protocol.c/.h` | 灯号游标、6 字节组帧、累加和校验 | 颜色渐变、SPI 波形 |
| 传输层 | `led_transport_spi.c/.h` | SPI/DMA、波形编码、帧间隔、PA3 空闲高、关灯低 | 颜色含义、灯号选择 |
| 调度 | `user_main.c` | 1 ms 时基、按键扫描、应用层轮询、逐帧发送 | 具体颜色/协议细节 |

---

## 2. 总体调用链

```text
TMR3 1 ms
  → led_timer_tick++

user_main()
  → led_app_set_current_time_ms(led_timer_tick)

  → key_driver_scan(&rf24g_scan_para)
  → rf24g_remoter_apply()
      → led_app_* 命令

  → key_driver_scan(&io_scan_param)
  → io_key_apply()
      → io_key1_click_handle()
      → led_app_mechanical_next()

  → led_app_poll(current_time_ms)
      → Timer 到点
      → NVM 延迟保存

  → 如果关灯：
        thread_driver_sem_take()
        return

  → led_transport_spi_frame_poll()
  → led_transport_spi_frame_ready()
  → led_protocol_current_lamp()
  → led_app_get_lamp_color(current_time_ms, lamp, &color)
  → led_protocol_build_frame(lamp, &color, frame)
  → led_transport_spi_frame_submit(frame)
  → led_protocol_advance_lamp()
```

说明：

- `user_main()` 由 LED driver 线程循环调用，不写 `while(1)`；
- 按键处理在逐灯发送之前；
- 模式切换在下一帧生效；
- 灯号游标不会被模式/颜色切换复位；
- 关灯状态不发送任何帧，靠 TMR3 每 1 ms 释放一次 driver 信号量，避免线程空转。

---

## 3. 应用层 `led_app`

### 3.1 状态

```text
led_app_power_on_flag
led_app_brightness_level
led_app_static_color_index
led_app_icon_index
led_app_off_from_modes_flag
led_app_mechanical_used_flag
led_app_timer_active_flag / kind / start_time_ms
led_app_save_dirty_flag / due_time_ms
led_app_current_time_ms
```

### 3.2 对外接口

```c
void led_app_init(uint32_t now_ms);
void led_app_poll(uint32_t now_ms);

void led_app_set_current_time_ms(uint32_t now_ms);
uint32_t led_app_get_current_time_ms(void);

bool led_app_power_is_on(void);
void led_app_power_toggle(uint32_t now_ms);
void led_app_power_on_external(uint32_t now_ms);
void led_app_mechanical_next(uint32_t now_ms);

void led_app_set_mode(led_animation_mode_t mode, uint32_t now_ms);
void led_app_set_static_color(uint8_t color_index, uint32_t now_ms);
void led_app_set_icon(uint8_t icon_index, uint32_t now_ms);
void led_app_modes_next(uint32_t now_ms);
void led_app_white_color_next(uint32_t now_ms);
void led_app_brightness_next(void);
void led_app_timer_start(app_timer_t timer_type, uint32_t now_ms);

void led_app_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                            led_color_t *output_color);
```

### 3.3 应用层职责

- 2.4G 与机械按键统一调用应用层；
- 2.4G 模式与机械 20 模式相互独立；
- 电源开关、PA3 输出、Timer 取消；
- 静态亮度缩放；
- NVM 2 s 延迟保存；
- 逐灯输出时叠加 Timer 提示。

---

## 4. 动画层 `led_animation`

### 4.1 顶层模式

```c
typedef enum {
    LED_ANIMATION_MODE_OFF = 0,
    LED_ANIMATION_MODE_STATIC_COLOR,
    LED_ANIMATION_MODE_FADE,
    LED_ANIMATION_MODE_STROBE,
    LED_ANIMATION_MODE_MODES,
    LED_ANIMATION_MODE_ICON,
    LED_ANIMATION_MODE_WHITE_SELECT,
    LED_ANIMATION_MODE_MECHANICAL,
} led_animation_mode_t;
```

### 4.2 主要接口

```c
void led_animation_init(uint32_t now_ms);
void led_animation_set_mode(led_animation_mode_t mode, uint32_t now_ms);
led_animation_mode_t led_animation_get_mode(void);

void led_animation_set_static_color_index(uint8_t static_color_index);
void led_animation_set_icon(uint8_t icon_index);
void led_animation_set_white_color_index(uint8_t white_color_index);
uint8_t led_animation_get_white_color_index(void);

void led_animation_set_mechanical_index(uint8_t mechanical_index);
uint8_t led_animation_get_mechanical_index(void);
bool led_animation_mechanical_uses_global_brightness(void);

void led_animation_modes_enter(uint32_t now_ms);
void led_animation_modes_next(uint32_t now_ms);
uint8_t led_animation_get_modes_state(void);
bool led_animation_modes_is_h(void);

void led_animation_pause(uint32_t now_ms);
void led_animation_resume(uint32_t now_ms);
bool led_animation_is_paused(void);

void led_animation_get_lamp_color(uint32_t now_ms, uint8_t lamp,
                                  led_color_t *output_color);
```

### 4.3 输出内容

- 静态单色：15 色表；
- Fade：8 个关键帧，18.432 s 周期；
- Strobe：7 色，每色 606 ms；
- Modes：A~H，H 有进入过程；
- 图标：美国国旗、三叶草、南瓜头、圣诞树；
- White Select：白、冷白、浅蓝；
- 机械 20 模式：见 [机械按键功能](./机械按键功能.md)。

### 4.4 亮度

动画层只输出 **100% 基准 RGBW**，不乘全局亮度。

应用层决定是否缩放：

- 2.4G 静态色、图标、White Select：缩放；
- 2.4G Fade、Strobe、Modes：100%；
- 机械 0~2：固定 W254/W149/W49，不再缩放；
- 机械 3 Fade、18 高变化：100%；
- 机械 4~17 静态色/图案：缩放；
- 机械 19：关灯。

---

## 5. 协议层 `led_protocol`

```c
void led_protocol_init(void);
uint8_t led_protocol_current_lamp(void);
void led_protocol_build_frame(uint8_t lamp, const led_color_t *color,
                              uint8_t frame[LED_FRAME_BYTES]);
void led_protocol_advance_lamp(void);
```

帧格式：

| 字节 | 内容 |
|---:|---|
| 0 | 灯号 1~12 |
| 1 | B |
| 2 | G |
| 3 | R |
| 4 | W |
| 5 | 前 5 字节累加和 `& 0xFF` |

规则：

- 灯号 `1,2,...,12,1,...` 持续循环；
- 切换模式/颜色不能复位灯号；
- 模式切换在下一帧生效。

---

## 6. 传输层 `led_transport_spi`

```c
void led_transport_spi_init(void);
void led_transport_spi_frame_init(void);
bool led_transport_spi_frame_ready(void);
void led_transport_spi_frame_submit(const uint8_t *frame);
void led_transport_spi_frame_poll(void);
void led_transport_spi_power_off(void);
void led_transport_spi_power_on(void);
bool led_transport_spi_is_power_on(void);
```

### 6.1 波形

SPI1 1 MHz，1 字节 = 8 us：

| 逻辑 | SPI 字节 | 时间 |
|---|---|---|
| 0 | `0x00 + 3×0xFF` | 8 us 低 + 24 us 高 |
| 1 | `0x00 + 7×0xFF` | 8 us 低 + 56 us 高 |

一帧：

```text
48 bit（MSB first）
+ 帧尾 0x00（8 us 低）
+ 帧尾 0xFF（8 us 高）
+ 163 字节 0xFF（约 1.304 ms 高）
```

帧间隔约 1.32 ms。

### 6.2 空闲高电平

- 初始化时 PA3 为 GPIO 高；
- 发送时切成 SPI MOSI；
- TMR1 在 DMA 结束前约 500 us 切回 GPIO 高，避免负脉冲；
- SPI 完成中断置完成标志；
- 主循环保留 pending 轮询和超时兜底；
- 关灯时 PA3 保持低，且不再提交帧。

### 6.3 关灯

`led_app_power_off()` 调用：

```c
led_transport_spi_power_off();
```

- 清发送完成标志；
- PA3 拉低；
- 后续 `frame_submit()` 直接丢弃，不产生波形。

开灯调用 `led_transport_spi_power_on()`，PA3 恢复高。

---

## 7. 电源、亮度、NVM、Timer

### 7.1 电源

关灯：

- 动画暂停；
- PA3 低；
- Timer 取消；
- 2 s 后保存 power=0；
- TMR3 每 1 ms 释放 driver 信号量，让 `user_main()` 在关灯状态继续轮询按键/NVM。

开灯：

- PA3 高；
- 动画恢复；
- 亮度不变；
- R1C1 从暂停处继续；
- Modes H 关灯后，R1C1/机械开灯从 Modes 索引 0 开始；
- 机械开灯从机械索引 0 开始。

### 7.2 亮度

5 挡：

```text
20% / 40% / 60% / 80% / 100%
```

- 只作用于静态类输出；
- 上电 100%；
- 亮度不写 NVM；
- 远程 Dimming 只改全局挡位；
- 机械 0~2 是固定白光亮度，不乘全局挡位。

### 7.3 NVM

`user_data_t`：

```c
uint8_t valid;
uint8_t version;
uint8_t power;
uint8_t mode;
uint8_t color;
```

保存：

- power；
- mode；
- static color / icon 索引。

不保存：

- 亮度；
- Timer；
- Modes A~H 状态；
- White Select 索引；
- 机械索引；
- 机械 `mechanical_used_flag`。

2 s 防抖保存；上电恢复后：

- 模式/颜色恢复；
- 亮度 100%；
- 机械模式回到机械白光 0；
- Timer 无效。

### 7.4 Timer

- 2h：提示 1 次红色（OFF 500 ms → RED 500 ms）；
- 4h：提示 2 次红色（OFF/RED/OFF/RED）；
- 提示结束后回当前模式；
- 同一个 Timer 重复按忽略；
- 2h/4h 互相切换时重新计时并重播提示；
- 关灯立即取消 Timer；
- Timer 不写 NVM；
- Timer 倒计时 2h/4h 到点自动关灯。

---

## 8. 按键输入

### 8.1 2.4G

`key_driver_scan(&rf24g_scan_para)` → `rf24g_remoter_apply()` → `led_app_*`。

功能见 [遥控器界面和功能](./遥控器界面和功能.md)。

### 8.2 机械

`key_driver_scan(&io_scan_param)` → `io_key_apply()` → `io_key1_click_handle()` → `led_app_mechanical_next()`。

- 只处理 CLICK；
- 机械索引独立于 2.4G 模式；
- 详细模式、亮度、电源、Timer 行为见 [机械按键功能](./机械按键功能.md)。

---

## 9. 中断与时序

| 中断 | 作用 | 优先级 |
|---|---|---:|
| TMR3 | 1 ms 时基 | 0 |
| TMR1 | DMA 结束前约 500 us 切 PA3 高 | 0 |
| SPI1 | DMA 完成，置完成标志 | 0 |

- TMR1 不能设为 1；
- TMR1 的 prescale 无效，按实测 10 us = 1600 ticks；
- 帧内低电平约 8~9 us；
- 帧内高电平 24 us / 56 us；
- 帧间隔约 1.32 ms；
- 帧间隔后不能再出现低脉冲。

---

## 10. 文件清单

| 文件 | 说明 |
|---|---|
| `user_app/user_main.c` | 1 ms 调度、按键扫描、逐帧发送 |
| `user_app/led_app.c/.h` | 应用状态、电源、亮度、Timer、NVM、机械索引 |
| `user_app/led_driver/led_animation.c/.h` | 动画与机械模式表 |
| `user_app/led_driver/led_protocol.c/.h` | 协议组帧 |
| `user_app/led_driver/led_transport_spi.c/.h` | SPI/DMA 波形 |
| `user_app/rf24g_remoter/rf24g_remoter_apply.c/.h` | 2.4G 按键映射 |
| `user_app/io_key/io_key_driver.c/.h` | 机械按键检测 |
| `user_app/io_key/io_key_apply.c/.h` | 机械按键事件处理 |

Code::Blocks 需要包含：

- `user_app/led_app.c`
- `user_app/rf24g_remoter/rf24g_remoter_apply.c`
- `user_app/io_key/io_key_driver.c`
- `user_app/io_key/io_key_apply.c`
- `user_app/led_driver/led_animation.c`
- `user_app/led_driver/led_protocol.c`
- `user_app/led_driver/led_transport_spi.c`

---

## 11. 常见问题

### 11.1 Timer 提示已经结束，但仍看到旧颜色

- Timer 只是覆盖输出，不切换动画模式；
- 提示结束后 BASE 取当前模式；
- 如果当前模式已切换，应回到新模式，不是旧模式。

### 11.2 关灯后 Timer 还在走

- 关灯必须调用 `led_app_power_off()`；
- 该函数会清 `led_app_timer_active_flag`；
- 机械模式 19、R1C1 关灯、Timer 到点都走这条路径。

### 11.3 机械第一次按键不是白光

- 机械 `mechanical_used_flag` 初始为 0；
- 第一次机械键应落索引 0（W254）；
- 2.4G 切模式不会清机械索引，但会保留 `mechanical_used_flag`；
- 上电/R1C1 关灯后机械开灯会回到索引 0。

### 11.4 静态图案不受 Dimming 影响

- 检查 `led_animation_mechanical_uses_global_brightness()`；
- 机械 4~17 应返回 true；
- 机械 0~2、3、18、19 返回 false。

---

## 12. 验证清单

1. 上电默认红色、100%；
2. R1C1 开关灯，PA3 高/低正确；
3. 关灯无帧、无 Timer、按键可唤醒；
4. Fade 18.432 s 循环；
5. Strobe 7 色；
6. Modes A~H，H 后关灯；
7. 2h 提示 1 次、4h 提示 2 次；
8. Timer 切换/关灯取消；
9. 机械 0 W254、1 W149、2 W49、3 Fade、4~17、18 高变化、19 关灯；
10. 机械关灯/R1C1 关灯后回索引 0；
11. NVM 模式/颜色恢复，亮度上电 100%；
12. 空闲无负脉冲，TMR1/TMR3/SPI 优先级都为 0。
