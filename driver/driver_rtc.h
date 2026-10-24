/*
 * @File name    : driver_rtc.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file contains all the functions prototypes for the RTC Base library.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_RTC_H
#define _DRIVER_RTC_H

#include "driver_com.h"

/**
 * @brief RTC IT and Flag Definition
 */
typedef enum {
    RTC_IT_ALM              = 0x01,
    RTC_IT_1S               = 0x02,
} RTC_IT_TYPEDEF;

//--------------- Function used to configure or get get time ---------------//
void rtc_set_counter(uint32_t cnt);
uint32_t rtc_get_counter(void);
void rtc_set_alarm(uint32_t alarm);
uint32_t rtc_get_alarm(void);
//------------- Function used to configure clock or interrupt --------------//
void rtc_prescale_set(uint32_t psc);
uint32_t rtc_prescale_get(void);
void rtc_pic_config(isr_t isr, int pr, RTC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE rtc_get_flag(RTC_IT_TYPEDEF interrupt_type);
void rtc_clear_flag(RTC_IT_TYPEDEF interrupt_type);
FLAG_STATE rtc_get_overflow_flag(RTC_IT_TYPEDEF interrupt_type);

#endif // _DRIVER_RTC_H
