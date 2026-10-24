/*
 * @File name    : driver_lowpwr.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Version      : v1.0
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_LOWPWR_H
#define _DRIVER_LOWPWR_H

#include "driver_com.h"

typedef enum {
    WK_EDGE_RISING        = 0x00,
    WK_EDGE_FALLING       = 0x01,
} WK_EDGE_SEL;

typedef enum {
    LP_TYPE_POWOFF          = 0x00,
    LP_TYPE_SLEEP           = 0x01,
} LP_TYPE_SEL;

typedef enum {
    WK_LP_ALARM             = BIT(0),
    WK_LP_RTC_1S            = BIT(1),
    WK_LP_WK0               = BIT(2),
    WK_LP_VUSB              = BIT(3),
    WK_LP_INBOX             = BIT(4),
    WK_LP_BT                = BIT(5),
    WK_LP_TOUCH             = BIT(6),
    WK_LP_GPIO              = BIT(7),
    WK_LP_KEYSCAN           = BIT(12),
    WK_LP_SRC_ALL           = 0xFFFF,
} WK_LP_SRC_SEL;

typedef struct {
    u32 piccon;
    u32 picen;
    u32 picpr;
    u32 nmicon;
    u32 clkgat0;
    u32 clkgat1;
    u32 clkgat2;

    u32 pwrcon0;
    u32 pwrcon1;
    u32 pwrcon2;

    u32 xosccon;
    u32 pll0con;
    u32 pll0con1;

    u32 clkcon0;
    u32 clkcon1;
    u32 clkdivcon0;

    u16 gpioa_fen;
    u16 gpiob_fen;
    u16 gpioa_fdiren;
    u16 gpiob_fdiren;
    u16 gpioa_drv;
    u16 gpiob_drv;

    u16 rstcon0;
    u32 rtccon;
    u32 rtccon0;
    u32 rtccon12;
    u32 rtccon5;
} lowpwr_sfr_backup_t;

typedef struct {
    LP_TYPE_SEL lp_type;
    WK_LP_SRC_SEL source;
    WK_EDGE_SEL edge;
    gpio_typedef *gpiox;
    uint16_t gpio_pin;
    uint16_t gpio_pupd;
} lowpwr_wakeup_typedef;

u32 lowpwr_get_wakeup_source(void);
void lowpwr_wakeup_disable(void);
bool lowpwr_is_wakeup_pending(u32 ignore_src);
void lowpwr_clr_wakeup_pending(void);
void lowpwr_wakeup_config(lowpwr_wakeup_typedef *config);
void lowpwr_wakeup_config_2(lowpwr_wakeup_typedef *config);
void lowpwr_wakeup_source_enable(WK_LP_SRC_SEL wk_src);
void lowpwr_wakeup_source_disable(WK_LP_SRC_SEL wk_src);
void lowpwr_wk0_wakeup_config(lowpwr_wakeup_typedef *config);
void lowpwr_gpio_wakeup_config(lowpwr_wakeup_typedef *config);
void lowpwr_sleep_proc(u8 lpclk_type);
uint32_t lowpwr_sleep_get_io_wksrc(void);
void lowpwr_sleep_clr_io_wksrc(void);

/**
  * @brief  only rtc digital, vddio is disable, only wk0 can wakeup system
  */
void lowpwr_pwroff_mode1_do(void);

/**
  * @brief  rtc digital,low power and vddio is enable, wk0, keyscan, touch and gpio can wakeup system
  */
void lowpwr_pwroff_mode2_do(void);


#endif
