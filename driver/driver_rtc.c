/*
 * @File name    : driver_rtc.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the RTC Base peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_rtc.h"

/**
  * @brief  Set the RTC counter register value.
  * @param  cnt: specifies the counter register new value.
  * @retval None
  */
AT(.com_periph.rtc.cnt_set)
void rtc_set_counter(uint32_t cnt)
{
    RTCCNT = (uint32_t)cnt;
}

/**
  * @brief  Get the RTC counter register value.
  * @param  None
  * @retval specifies the counter register value.
  */
AT(.com_periph.rtc.cnt_get)
uint32_t rtc_get_counter(void)
{
    return (uint32_t)RTCCNT;
}

/**
  * @brief  Set the RTC alarm register value.
  * @param  alm: specifies the alarm register new value. RTCALM = (real time) - 1.
  * @retval None
  */
AT(.com_periph.rtc.alarm_set)
void rtc_set_alarm(uint32_t alarm)
{
    RTCALM = (uint32_t)alarm;
}

/**
  * @brief  Get the RTC alarm register value.
  * @param  None
  * @retval specifies the alarm register value. (real time) = RTCALM + 1.
  */
AT(.com_periph.rtc.alarm_get)
uint32_t rtc_get_alarm(void)
{
    return (uint32_t)RTCALM;
}

/**
 * @brief  Config predivision factor for 1Hz counter.
 * @param  psc: Predivision factor configuration function of 1hz second counter.
 * @retval None
 */
void rtc_prescale_set(uint32_t psc)
{
    u32 reg_1s_period_mask = 0x1ffff;
    u32 rtccon2 = RTCCON2;

    rtccon2 &= ~reg_1s_period_mask;
    rtccon2 |= psc & reg_1s_period_mask;;
    RTCCON2 = rtccon2;
}

/**
 * @brief  Get the RTC counter 1Hz prescale value.
 * @retval the prescale value.
 */
uint32_t rtc_prescale_get(void)
{
    return RTCCON2 & 0x1ffff;
}

/**
  * @brief  Enable or disable the RTC interrupt.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrupt_type: specifies the RTC interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg RTC_IT_ALM: RTC alarm interrupt enable bit.
  *             @arg RTC_IT_1S: RTC 1S interrupt enable bit.
  * @param  state: the state of the RTC peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void rtc_pic_config(isr_t isr, int pr, RTC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & RTC_IT_ALM) {
        interrupt_bit |= RTCCON_ALMIE;
    }

    if (interrupt_type & RTC_IT_1S) {
        interrupt_bit |= RTCCON_RTC1SIE;
    }

    if (state != DISABLE) {
        RTCCON |= interrupt_bit;
        sys_irq_init(IRQn_RTC_SECOND_ALARM_LVD_WDT, pr, isr);
        if (interrupt_type & RTC_IT_1S){
            RTCCON11 |= RTCCON11_RTCWKSLPEN;
        }
    } else {
        RTCCON &= ~interrupt_bit;
        if (interrupt_type & RTC_IT_1S){
            RTCCON11 &= ~RTCCON11_RTCWKSLPEN;
        }
    }
}

/**
  * @brief  Get the RTC interrupt pending.
  * @param  interrupt_type: specifies the flag to get.
  *         this parameter can be one of the following values:
  *             @arg RTC_IT_1S: RTC 1S interrupt pending bit.
  *             @arg RTC_IT_ALM: RTC alarm interrupt pending bit.
  * @retval The state of rtc_flag (SET or RESET).
  */
AT(.com_periph.rct.get)
FLAG_STATE rtc_get_flag(RTC_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type & RTC_IT_ALM) {
        interrupt_bit |= RTCCON_ALMPND;
    }

    if (interrupt_type & RTC_IT_1S) {
        interrupt_bit |= RTCCON_RTCWKSLPPND;
    }

    if ((RTCCON & interrupt_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the RTC interrupt pending.
  * @param  rtc_flag: specifies the flag to clear.
  *         this parameter can be one of the following values:
  *             @arg RTC_FLAG_1S: RTC 1S interrupt pending bit.
  *             @arg RTC_FLAG_ALM: RTC alarm interrupt pending bit.
  * @retval None
  */
AT(.com_periph.rct.clear)
void rtc_clear_flag(RTC_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type & RTC_IT_ALM) {
        interrupt_bit |= RTCCON_ALMPND;
    }

    if (interrupt_type & RTC_IT_1S) {
        interrupt_bit |= RTCCON_RTCWKSLPPND;
    }

    RTCCPND |= (uint32_t)interrupt_bit;
}

/**
  * @brief  Get the RTC count overflow flag.
  * @param  interrupt_type: specifies the flag to get.
  *         this parameter can be one of the following values:
  *             @arg RTC_IT_ALM: RTC alarm interrupt pending bit.
  * @retval The state of rtc_flag (SET or RESET).
  */
AT(.com_periph.rct.get)
FLAG_STATE rtc_get_overflow_flag(RTC_IT_TYPEDEF interrupt_type)
{
    if (interrupt_type & RTC_IT_ALM) {
        if (RTCCNT >= RTCALM) {
            return SET;
        }
    }

    return RESET;
}
