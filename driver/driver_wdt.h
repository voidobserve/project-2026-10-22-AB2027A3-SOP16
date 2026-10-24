/*
 * @File name    : driver_wdt.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file contains all the functions prototypes for the WDT library.
 * 
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#ifndef _DRIVER_WDT_H
#define _DRIVER_WDT_H

#include "driver_com.h"

//--------------- WDT Flag Definition ---------------//
#define WDT_FLAG_PENDING            ((uint32_t)0x80000000)

//clk select enumeration
typedef enum {
    WDT_CLK_RC32K       = 0x00010000,
    WDT_CLK_X32K        = 0x00000000,
} WDT_CLK_TYPEDEF;

//time select enumeration
typedef enum {
    WDT_TIME_1MS        = 0x00000000,
    WDT_TIME_256MS      = 0x00100000,
    WDT_TIME_512MS      = 0x00200000,
    WDT_TIME_1024MS     = 0x00300000,
    WDT_TIME_2048MS     = 0x00400000,
    WDT_TIME_4096MS     = 0x00500000,
    WDT_TIME_8192MS     = 0x00600000,
    WDT_TIME_16384MS    = 0x00700000,
} WDT_TIME_TYPEDEF;

void wdt_cmd(FUNCTIONAL_STATE state);
void wdt_reset_cmd(FUNCTIONAL_STATE state);
void wdt_it_cmd(FUNCTIONAL_STATE state);
void wdt_clk_select(WDT_CLK_TYPEDEF clk);
void wdt_time_select(WDT_TIME_TYPEDEF time);
FLAG_STATE wdt_get_flag(uint32_t wdt_flag);
void wdt_clear(void);

#endif // _DRIVER_WDT_H
