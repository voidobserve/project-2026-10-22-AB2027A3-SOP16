/*
 * @File name: driver_tick.h
 * @Author: Bluetrum IOT Team
 * @Date: 2023-12-27
 * @Description: This file contains all the functions prototypes for the TICK library.
 * 
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#ifndef _DRIVER_TICK_H
#define _DRIVER_TICK_H

#include "driver_com.h"


/**
 * @brief TICK Interrupt Flag Definition
 */
typedef enum {
    TICK_IT_OV              = 0x01,
} TICK_IT_TYPEDEF;

/**
 * @brief Clock Source Selection
 */
typedef enum {
    TICK_INCSRC_INC         = 0x00000000,
    TICK_INCSRC_PIN         = 0x00000040,
} TICK_INCSRC_TYPEDEF;

/**
 * @brief Clock Trigger Selection
 */
typedef enum {
    TICK_INCSEL_SYSCLK      = 0x00000000,
    TICK_INCSEL_RISING      = 0x00000004,
    TICK_INCSEL_FALLING     = 0x00000008,
    TICK_INCSEL_EDGE        = 0x0000000c,
} TICK_INCSEL_TYPEDEF;

/**
 * @brief Tick Init Structure
 */
typedef struct {
    uint16_t                period;
    TICK_INCSRC_TYPEDEF     counter_source;
    TICK_INCSEL_TYPEDEF     clock_source;
} tick_base_init_typedef;


/********************** Function used to configure tick base ***********************/
void tick_base_init(tick_base_init_typedef *tick_base_init_struct);
void tick_deinit(void);
void tick_set_counter(uint16_t cnt);
void tick_set_period(uint16_t period);
uint16_t tick_get_counter(void);
uint16_t tick_get_period(void);
void tick_cmd(FUNCTIONAL_STATE state);
/********************** Function used to configure interrupt ***********************/
void tick_pic_config(isr_t isr, int pr, TICK_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE tick_get_flag(TICK_IT_TYPEDEF interrupt_type);
void tick_clear_flag(TICK_IT_TYPEDEF interrupt_type);

#endif // _DRIVER_TICK_H
