/*
 * @File name: driver_tick.c
 * @Author: Bluetrum IOT Team
 * @Date: 2023-12-27
 * @Description: This file provides functions to manage the most functionalities
 *               of the TICK peripheral.
 * 
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#include "driver_tick.h"


/**
  * @brief  Initializes the tick base peripheral according to the specified
  *         parameters in the tick_base_init_typedef.
  * @param  tick_base_init_struct: pointer to a tick_base_init_typedef structure that
  *         contains the configuration information for the specified TICK peripheral.
  * @retval None
  */
void tick_base_init(tick_base_init_typedef *tick_base_init_struct)
{
    uint32_t tmp_reg;

    /* Disable peripheral before config */
    TICK->con &= ~TICK_CON_EN;

    /* Configure clock sel */
    tmp_reg = TICK->con;
    tmp_reg &= ~(TICK_CON_INCSRC | TICK_CON_INCSEL);
    tmp_reg |= (uint32_t)(tick_base_init_struct->counter_source & TICK_CON_INCSRC);
    tmp_reg |= (uint32_t)(tick_base_init_struct->clock_source & TICK_CON_INCSEL);
    TICK->con = tmp_reg;

    /* Configure tick period */
    TICK->pr = (uint16_t)tick_base_init_struct->period;
}

/**
 * @brief  De-initialize the specified tick peripheral.
 * @retval None
 */
void tick_deinit(void)
{
    TICK->con = 0;
    CLKGAT1 &= ~(0x01 << 16);
}

/**
  * @brief  Set the TICK counter register value.
  * @param  cnt: specifies the counter register new value.
  * @retval None
  */
AT(.com_periph.tick.config)
void tick_set_counter(uint16_t cnt)
{
    TICK->cnt = cnt;
}

/**
  * @brief  Set the TICK period register value.
  * @param  period: specifies the period register new value.
  * @retval None
  */
AT(.com_periph.tick.config)
void tick_set_period(uint16_t period)
{
    TICK->pr = period;
}

/**
  * @brief  Get the TICK counter register value.
  * @retval specifies the counter register value.
  */
AT(.com_periph.tick.query)
uint16_t tick_get_counter(void)
{
    return (uint16_t)(TICK->cnt);
}

/**
  * @brief  Get the TICK period register value.
  * @retval specifies the period register value.
  */
AT(.com_periph.tick.query)
uint16_t tick_get_period(void)
{
    return (uint16_t)(TICK->pr);
}

/**
  * @brief  Enable or disable the specified TICK peripheral.
  * @param  state: new state of the TICK peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
AT(.com_periph.tick.ctrl)
void tick_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        TICK->con |= TICK_CON_EN;
    } else {
        TICK->con &= ~TICK_CON_EN;
    }
}

/**
  * @brief  Enable or disable the specified TICK interrupt.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrup_type: specifies the TICK interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg TICK_IT_OV: overflow interrupt enable bit.
  * @param  state: the state of the TICK peripheral interrupt.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void tick_pic_config(isr_t isr, int pr, TICK_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_en_bit = 0;
    uint32_t all_interrupt_type_mask = TICK_IT_OV;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & TICK_IT_OV) {
        interrupt_en_bit |= TICK_CON_TIE;
    }

    if (state != DISABLE) {
        sys_irq_init(IRQn_TICK0, pr, isr);
        TICK->con |= interrupt_en_bit;
    } else {
        TICK->con &= ~interrupt_en_bit;
        if (interrupt_type == all_interrupt_type_mask) {
            PICEN &= ~BIT(IRQn_TICK0);
        }
    }
}

/**
  * @brief  Get the TICK interrupt pending.
  * @param  interrupt_type: specifies the IT pending need to get.
  *         this parameter can be one of the following values:
  *             @arg TICK_IT_OV: overflow interrupt.
  * @retval The state of interrupt_type (SET or RESET).
  */
AT(.com_periph.tick.query)
FLAG_STATE tick_get_flag(TICK_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & TICK_IT_OV) {
        interrupt_pending_bit |= TICK_CON_TPND;
    }

    if ((TICK->con & interrupt_pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the TICK interrupt pending.
  * @param  interrupt_type: specifies the IT pending need to clear.
  *         this parameter can be one of the following values:
  *             @arg TICK_IT_OV: overflow interrupt.
  * @retval None
  */
AT(.com_periph.tick.config)
void tick_clear_flag(TICK_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & TICK_IT_OV) {
        interrupt_pending_bit |= TICK_CPND_TPCLR;
    }

    TICK->cpnd |= interrupt_pending_bit;
}
