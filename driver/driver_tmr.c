/*
 * @File name    : driver_tmr.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the TMR peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#include "driver_tmr.h"

/**
  * @brief  Initializes the timer base peripheral according to the specified
  *         parameters in the tmr_base_init_typedef.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  tmr_base_init_struct: pointer to a tmr_base_init_typedef structure that
  *         contains the configuration information for the specified TMR peripheral.
  * @retval None
  */
void tmr_base_init(tmr_typedef *tmrx, tmr_base_init_typedef *tmr_base_init_struct)
{
    //---------- tmr controller register ----------//
    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        tmrx->con &= ~(uint32_t)TMRxCON_INCSEL;
        tmrx->con |= (uint32_t)tmr_base_init_struct->clock_source;

        tmrx->con &= ~(uint32_t)TMRxCON_INCSRC;
        tmrx->con |= (uint32_t)tmr_base_init_struct->counter_source;
    } else if (tmrx == TMR3) {
        tmrx->con &= ~(uint32_t)TMR3CON_INCSEL;
        tmrx->con |= (uint32_t)tmr_base_init_struct->clock_source;

        tmrx->con &= ~(uint32_t)TMR3CON_INCSRC;
        tmrx->con |= (uint32_t)tmr_base_init_struct->counter_source;
        tmrx->psc = (uint16_t)(tmr_base_init_struct->prescale);
    }
    tmrx->period = (uint32_t)(tmr_base_init_struct->period);
}

/**
 * @brief  De-initialize the specified tmr peripheral.
 * @retval None
 */
void tmr_deinit(tmr_typedef *tmrx)
{
    tmrx->con = 0;

    if (tmrx == TMR0) {
        clk_gate0_cmd(CLK_GATE0_TMR0, CLK_DIS);
    } else if (tmrx == TMR1) {
        clk_gate0_cmd(CLK_GATE0_TMR1, CLK_DIS);
    } else if (tmrx == TMR2) {
        clk_gate0_cmd(CLK_GATE0_TMR2, CLK_DIS);
    } else if (tmrx == TMR3) {
        clk_gate1_cmd(CLK_GATE1_TMR3, CLK_DIS);
    }
}

/**
  * @brief  Set the TMR counter register value.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  cnt: specifies the counter register new value.
  * @retval None
  */
void tmr_set_counter(tmr_typedef *tmrx, uint32_t cnt)
{
    tmrx->cnt = cnt;
}

/**
  * @brief  Set the TMR period register value.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  period: specifies the period register new value.
  * @retval None
  */
AT(.com_periph.tmr.set_period)
void tmr_set_period(tmr_typedef *tmrx, uint32_t period)
{
    tmrx->period = period;
}

/**
 * @brief  Set the TMR prescale register value.
 * @param  tmrx: where x can be (3) to select the TMR peripheral.
 * @param  psc: specifies the prescale register new value.
 * @retval None
 */
void tmr_set_prescale(tmr_typedef *tmrx, uint16_t psc)
{
	if (tmrx == TMR3) {
	    tmrx->psc = (uint16_t)psc;
	}
}

/**
  * @brief  Get the TMR counter register value.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @retval specifies the counter register value.
  */
uint32_t tmr_get_counter(tmr_typedef *tmrx)
{
    return (uint32_t)(tmrx->cnt);
}

/**
  * @brief  Get the TMR period register value.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @retval specifies the period register value.
  */
uint32_t tmr_get_period(tmr_typedef *tmrx)
{
    return (uint32_t)(tmrx->period);
}

/**
 * @brief  Get the TMR prescale register value.
 * @param  tmrx: where x can be (3) to select the TMR peripheral.
 * @retval Specifies the prescale register value.
 */
uint16_t tmr_get_prescale(tmr_typedef *tmrx)
{
	if (tmrx == TMR3) {
	    return (uint16_t)(tmrx->psc);
	}
	return 0;
}

/**
  * @brief  Enable or disable the specified TMR peripheral.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  state: state of the TMRx peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
AT(.com_periph.tmr.cmd)
void tmr_cmd(tmr_typedef *tmrx, FUNCTIONAL_STATE state)
{
    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        if (state != DISABLE) {
            tmrx->con |= TMRxCON_TMREN;
        } else {
            tmrx->con &= (uint32_t)(~TMRxCON_TMREN);
        }
    } else if (tmrx == TMR3) {
        if (state != DISABLE) {
            tmrx->con |= TMR3CON_TMREN;
        } else {
            tmrx->con &= (uint32_t)(~TMR3CON_TMREN);
        }
    }
}

/**
 * @brief  Config pwm duty of specified TMR periperal.
 * @param  tmrx: where x can be (3) to select the TMR peripheral.
 * @param  pwm: the pwm channel that wants to config.
 *         this parameter can be one of the following values:
 *             @arg TMR_PWM0: pwm0 channel.
 *             @arg TMR_PWM1: pwm1 channel.
 *             @arg TMR_PWM2: pwm2 channel.
 * @param  duty: The high level time of the pwm to be configured.
 * @retval
 */
void tmr_pwm_duty_config(tmr_typedef *tmrx, TMR_PWM_TYPEDDEF pwm, uint16_t duty)
{
    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        return;
    }

    if (pwm == TMR_PWM0) {
        tmrx->duty0 = duty;
    } else if (pwm == TMR_PWM1) {
        tmrx->duty1 = duty;
    } else if (pwm == TMR_PWM2) {
        tmrx->duty2 = duty;
    }
}

/**
  * @brief  Enable or disable the PWM of specified TMR peripheral.
  * @param  tmrx: where x can be (3) to select the TMR peripheral.
  * @param  pwm: The pwm channel that wants to config.
  *         this parameter can be one of the following values:
  *             @arg TMR_PWM0: pwm0 channel.
  *             @arg TMR_PWM1: pwm1 channel.
  *             @arg TMR_PWM2: pwm2 channel.
  * @param  state: state of the TMRx peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void tmr_pwm_cmd(tmr_typedef *tmrx, TMR_PWM_TYPEDDEF pwm, FUNCTIONAL_STATE state)
{
    uint32_t pwm_con_mask = TMR3CON_PWM0EN | TMR3CON_PWM1EN | TMR3CON_PWM2EN;

    if (tmrx == TMR3) {
        if (state != DISABLE) {
            tmrx->con |= pwm & pwm_con_mask;
        } else {
            tmrx->con &= (uint32_t)~(pwm & pwm_con_mask);
        }
    }
}

/**
  * @brief  Get the TMR capture register value.
  * @param  tmrx: where x can be (3) to select the TMR peripheral.
  * @retval specifies the capture register value.
  */
uint32_t tmr_get_capture(tmr_typedef *tmrx)
{
    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        return ~((uint32_t)0);
    }
    return (uint32_t)(tmrx->capture_val);
}

/**
  * @brief  Configure input capture for TMR.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  edge_select: Captured edge selection.
  *         this parameter can be one of the following values:
  *             @arg TMR_CAP_NULL: no capture.
  *             @arg TMR_CAP_RISING: capture rising edge.
  *             @arg TMR_CAP_FALLING: capture falling edge.
  *             @arg TMR_CAP_EDGE: capture bilateral edge.
  * @retval None
  */
void tmr_capture_config(tmr_typedef *tmrx, TMR_CAP_EDGE_TYPEDEF edge_select)
{
    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        return;
    }

    tmrx->con &= ~((uint32_t)TMR3CON_CPTEDSEL);
    tmrx->con |= (uint32_t)edge_select;
}

/**
  * @brief  Enable or disable the capture function of specified TMR peripheral.
  * @param  tmrx: where x can be (3) to select the TMR peripheral.
  * @param  state: state of the TMRx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void tmr_capture_cmd(tmr_typedef *tmrx, FUNCTIONAL_STATE state)
{
    if (tmrx == TMR3) {
        if (state != DISABLE) {
            tmrx->con |= TMR3CON_CPTEN;
        } else {
            tmrx->con &= (uint32_t)(~TMR3CON_CPTEN);
        }
    }
}

/**
  * @brief  Enable or disable the specified TMRx interrupt.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrup_type: specifies the TRMx interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg TMR_IT_UPDATE1: TMR(1..2) overflow interrupt enable bit.
  *             @arg TMR_IT_CAP: TMR(3..5) capture interrupt enable bit.
  *             @arg TMR_IT_UPDATE2: TMR(3..5) overflow interrupt enable bit.
  * @param  state: the state of the TMRx peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void tmr_pic_config(tmr_typedef *tmrx, isr_t isr, int pr, TMR_IT_TYPEDEF interrup_type, FUNCTIONAL_STATE state)
{
    uint8_t irq_vector;
    uint32_t tmr_interrupt_bit = 0;
    uint32_t all_interrupt_type_mask = TMR_IT_UPDATE | TMR_IT_CAPTURE;

    if (interrup_type == 0) {
        return;
    }

    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        if (interrup_type & TMR_IT_CAPTURE) {
            interrup_type &= ~TMR_IT_CAPTURE;
        }
    }

    /*--- Get interrupt vector ---*/
    if (tmrx == TMR0) {
        irq_vector = IRQn_TMR0;
    }else if (tmrx == TMR1) {
        irq_vector = IRQn_TMR1;
    } else if (tmrx == TMR2) {
        irq_vector = IRQn_TMR2;
    } else if (tmrx == TMR3) {
        irq_vector = IRQn_TMR3;
    } else {
        return;
    }

    /*--- Get interrupt ctrl bit ---*/
    if (interrup_type & TMR_IT_UPDATE) {
        tmr_interrupt_bit |= TMRxCON_TIE;
    }

    if (interrup_type & TMR_IT_CAPTURE) {
        tmr_interrupt_bit |= TMR3CON_CIE;
    }

    /*--- Execute configured ---*/
    if (state != DISABLE) {
        tmrx->con |= tmr_interrupt_bit;
        sys_irq_init(irq_vector, pr, isr);
    } else {
        tmrx->con &= ~tmr_interrupt_bit;
        if ((tmrx != TMR3) || (interrup_type == all_interrupt_type_mask)) {
            PICENCLR = BIT(irq_vector);
        }
    }
}

/**
  * @brief  Get the TMR interrupt pending.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  interrupt_type: specifies the IT to clear.
  *         this parameter can be one of the following values:
  *             @arg TMR_IT_UPDATE: TMR overflow interrupt.
  *             @arg TMR_IT_CAPTURE: TMR capture interrupt.
  * @retval The state of interrupt_type (SET or RESET).
  */
AT(.com_periph.tmr.get)
FLAG_STATE tmr_get_flag(tmr_typedef *tmrx, TMR_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        if (interrupt_type & TMR_IT_CAPTURE) {
            return RESET;
        } else if (interrupt_type & TMR_IT_UPDATE) {
            interrupt_pending_bit |= TMRxCON_TPND;
        }
    } else if (tmrx == TMR3) {
        if (interrupt_type & TMR_IT_CAPTURE) {
            interrupt_pending_bit |= TMR3CON_CPND;
        }
        if (interrupt_type & TMR_IT_UPDATE) {
            interrupt_pending_bit |= TMR3CON_TPND;
        }
    } else {
        return RESET;
    }

    if ((tmrx->con & interrupt_pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the TMR interrupt pending.
  * @param  tmrx: where x can be (0..3) to select the TMR peripheral.
  * @param  interrupt_type: specifies the interrupt to clear.
  *         this parameter can be one of the following values:
  *             @arg TMR_IT_UPDATE: TMR overflow interrupt.
  *             @arg TMR_IT_CAPTURE: TMR capture interrupt.
  * @retval None
  */
AT(.com_periph.tmr.clear)
void tmr_clear_flag(tmr_typedef *tmrx, TMR_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (tmrx == TMR0 || tmrx == TMR1 || tmrx == TMR2) {
        if (interrupt_type & TMR_IT_UPDATE) {
            interrupt_pending_bit |= TMRxCPND_TPCLR;
        }
    } else if (tmrx == TMR3) {
        if (interrupt_type & TMR_IT_CAPTURE) {
            interrupt_pending_bit |= TMR3CPND_CPCLR;
        } else if (interrupt_type & TMR_IT_UPDATE) {
            interrupt_pending_bit |= TMR3CPND_TPCLR;
        }
    } else {
        return;
    }

    tmrx->cpnd |= interrupt_pending_bit;
}
