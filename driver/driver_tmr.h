/*
 * @File name    : driver_tmr.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file contains all the functions prototypes for the TMR library.
 * 
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#ifndef _DRIVER_TMR_H
#define _DRIVER_TMR_H

#include "driver_com.h"

/**
 * @brief TMR Interrupt Flag Definition
 */
typedef enum {
    TMR_IT_UPDATE           = 0x01,
    TMR_IT_CAPTURE          = 0x02,
} TMR_IT_TYPEDEF;

/**
 * @brief Clock Source Selection
 */
typedef enum {
    TMR_INC_SOURCE          = 0x00000000,
    TMR_PIN_SOURCE          = 0x00000040,
} TMR_COUNTER_TYPEDEF;

/**
 * @brief Capture Edge Selection
 */
typedef enum {
    TMR_CAP_NULL            = 0x00000000,
    TMR_CAP_RISING          = 0x00000010,
    TMR_CAP_FALLING         = 0x00000020,
    TMR_CAP_EDGE            = 0x00000030,
} TMR_CAP_EDGE_TYPEDEF;

/**
 * @brief Clock Triggle Selection
 */
typedef enum {
    TMR_SYSTEM_CLOCK        = 0x00000000,
    TMR_COUNTER_RISING      = 0x00000004,
    TMR_COUNTER_FALLING     = 0x00000008,
    TMR_COUNTER_EDGE        = 0x0000000c,
} TMR_CLOCK_TYPEDEF;

/**
 * @brief PWM Mode Selection
 */
typedef enum {
    TMR_PWM0                = 0x00000200,
    TMR_PWM1                = 0x00000400,
    TMR_PWM2                = 0x00000800,
} TMR_PWM_TYPEDDEF;

/**
 * @brief Timer Init Structure
 */
typedef struct {
    uint32_t                period;
    uint32_t                prescale;
    TMR_COUNTER_TYPEDEF     counter_source;
    TMR_CLOCK_TYPEDEF       clock_source;
} tmr_base_init_typedef;

/********************** Function used to configure timer base **********************/
void tmr_base_init(tmr_typedef *tmrx, tmr_base_init_typedef *tmr_base_init_struct);
void tmr_deinit(tmr_typedef *tmrx);
void tmr_set_counter(tmr_typedef *tmrx, uint32_t cnt);
void tmr_set_period(tmr_typedef *tmrx, uint32_t period);
void tmr_set_prescale(tmr_typedef *tmrx, uint16_t psc);
uint32_t tmr_get_counter(tmr_typedef *tmrx);
uint32_t tmr_get_period(tmr_typedef *tmrx);
uint16_t tmr_get_prescale(tmr_typedef *tmrx);
void tmr_cmd(tmr_typedef *tmrx, FUNCTIONAL_STATE state);
/*************************** Function used to set pwm ******************************/
void tmr_pwm_duty_config(tmr_typedef *tmrx, TMR_PWM_TYPEDDEF pwm, uint16_t duty);
void tmr_pwm_cmd(tmr_typedef *tmrx, TMR_PWM_TYPEDDEF pwm, FUNCTIONAL_STATE state);
/************************** Function used to set capture ***************************/
uint32_t tmr_get_capture(tmr_typedef *tmrx);
void tmr_capture_config(tmr_typedef *tmrx, TMR_CAP_EDGE_TYPEDEF edge_select);
void tmr_capture_cmd(tmr_typedef *tmrx, FUNCTIONAL_STATE state);
/********************** Function used to configure interrupt ***********************/
void tmr_pic_config(tmr_typedef *tmrx, isr_t isr, int pr, TMR_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE tmr_get_flag(tmr_typedef *tmrx, TMR_IT_TYPEDEF interrupt_type);
void tmr_clear_flag(tmr_typedef *tmrx, TMR_IT_TYPEDEF interrupt_type);

#endif // _DRIVER_TMR_H
