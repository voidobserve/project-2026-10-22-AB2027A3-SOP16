/*
 * @File name    : driver_qdec.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2025-07-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the QDEC peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#include "driver_qdec.h"

/**
  * @brief  Initializes the qdec peripheral according to the specified
  *         parameters in the qdec_init_typedef.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @param  qdec_init_struct: pointer to a qdec_init_struct structure that
  *         contains the configuration information for the specified QDEC peripheral.
  * @retval None
  */
void qdec_init(qdec_typedef *qdec, qdec_init_typedef *qdec_init_struct)
{
    //---------- qdec controller register ----------//
    if (QDEC_RC32K_CLK == qdec_init_struct->clk_source) {
        clk_qdec_set(CLK_QDEC_RC32K);
    } else {
        clk_qdec_set(CLK_QDEC_X24M_32K);
    }

    if (qdec == QDECX) {
        clk_gate1_cmd(CLK_GATE1_QDECX, CLK_EN);
    } else if (qdec == QDECY) {
        clk_gate1_cmd(CLK_GATE1_QDECY, CLK_EN);
    } else if (qdec == QDECZ) {
        clk_gate1_cmd(CLK_GATE1_QDECZ, CLK_EN);
    }

    qdec->con &= ~(uint32_t)QDECxCON_TIME_MODE;
    qdec->con |= ((qdec_init_struct->tm_en) << 15) & QDECxCON_TIME_MODE;

    qdec->con &= ~(uint32_t)QDECxCON_CM;
    qdec->con |= ((qdec_init_struct->cm_sel) << 12) & QDECxCON_CM;

    qdec->con &= ~(uint32_t)QDECxCON_WKUP_EN;
    qdec->con |= ((qdec_init_struct->wakup_en) << 4) & QDECxCON_WKUP_EN;

    if (QDEC_TM_ENABLE == qdec_init_struct->tm_en) {
        qdec->con &= ~(uint32_t)QDECxCON_SMP;
        qdec->con |= ((qdec_init_struct->smp_num_sel) << 16) & QDECxCON_SMP;
    }

    qdec->con &= ~(uint32_t)QDECxCON_FIL_LEN;
    qdec->con |= ((qdec_init_struct->fil_len) << 5) & QDECxCON_FIL_LEN;
}

/**
 * @brief  De-initialize the specified QDEC peripheral.
 * @retval None
 */
void qdec_deinit(qdec_typedef *qdec)
{
    qdec->con = 0;

    if (qdec == QDECX) {
        clk_gate1_cmd(CLK_GATE1_QDECX, CLK_DIS);
    } else if (qdec == QDECY) {
        clk_gate1_cmd(CLK_GATE1_QDECY, CLK_DIS);
    } else if (qdec == QDECZ) {
        clk_gate1_cmd(CLK_GATE1_QDECZ, CLK_DIS);
    }
}

/**
  * @brief  Enable or disable the specified QDEC peripheral.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @param  state: state of the QDEC peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
AT(.com_periph.qdec.cmd)
void qdec_cmd(qdec_typedef *qdec, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        qdec->con |= QDECCON_EN;
    } else {
        qdec->con &= (uint32_t)(~QDECCON_EN);
    }
}


/**
  * @brief  Get the QDEC data value.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @retval specifies QDEC data register value.
  */
AT(.com_periph.qdec.get_data)
uint32_t qdec_get_data(qdec_typedef *qdec)
{
    return (uint32_t)(qdec->dat);
}

/**
  * @brief  Get the TMR capture register value.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @retval specifies the capture register value.
  */
void qdec_clear_data(qdec_typedef *qdec)
{
    qdec->con |= QDECCON_DAT_CLR;
}

/**
  * @brief  Enable or disable the specified QDECx interrupt.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrupt_type: specifies the IT to clear.
  *         this parameter can be one of the following values:
  *             @arg QDEC_IT_SAMPLE_COUNT:  QDEC sample count interrupt pending.
  *             @arg QDEC_IT_REVERSE_DIR:  QDEC reverse direction interrupt pending.
  *             @arg QDEC_IT_FORWARD_DIR:  QDEC forward direction interrupt pending.
  * @param  state: the state of the QDEC peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void qdec_pic_config(qdec_typedef *qdec, isr_t isr, int pr, QDEC_IT_TYPEDEF interrup_type, FUNCTIONAL_STATE state)
{
    uint8_t irq_vector;
    uint32_t qdec_interrupt_bit = 0;
    uint32_t all_interrupt_type_mask = QDEC_IT_SAMPLE_COUNT | QDEC_IT_REVERSE_DIR | QDEC_IT_FORWARD_DIR;

    if (interrup_type == 0) {
        return;
    }

    /*--- Get interrupt vector ---*/
    irq_vector = IRQn_IR_QDEC_LEDC;

    /*--- Get interrupt ctrl bit ---*/
    if (interrup_type & QDEC_IT_SAMPLE_COUNT) {
        qdec_interrupt_bit |= QDECCON_SMPIE;
    }

    if (interrup_type & QDEC_IT_REVERSE_DIR) {
        qdec_interrupt_bit |= QDECCON_RDIE;
    }

    if (interrup_type & QDEC_IT_FORWARD_DIR) {
        qdec_interrupt_bit |= QDECCON_FDIE;
    }
    /*--- Execute configured ---*/
    if (state != DISABLE) {
        qdec->con |= qdec_interrupt_bit;
        sys_irq_init(irq_vector, pr, isr);
    } else {
        qdec->con &= ~qdec_interrupt_bit;
        if (interrup_type == all_interrupt_type_mask) {
            PICEN &= ~BIT(irq_vector);
        }
    }
}

/**
  * @brief  Get the QDEC interrupt pending.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @param  interrupt_type: specifies the IT to clear.
  *         this parameter can be one of the following values:
  *             @arg QDEC_REVERSE_DIR_PND:  QDEC reverse direction pending.
  *             @arg QDEC_FORWARD_DIR_PND:  QDEC forward direction pending.
  *             @arg QDEC_OV_PND:           QDEC overflow pending.
  *             @arg QDEC_SMP_PND:          QDEC sample pending.
  *             @arg QDEC_TERR_PND:         QDEC transition error pending.
  * @retval The state of interrupt_type (SET or RESET).
  */
AT(.com_periph.qdec.get)
FLAG_STATE qdec_get_flag(qdec_typedef *qdec, QDEC_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type & QDEC_REVERSE_DIR_PND) {
        interrupt_bit |= QDECCON_REVERSE_PND;
    }

    if (interrupt_type & QDEC_FORWARD_DIR_PND) {
        interrupt_bit |= QDECCON_FORWARD_PND;
    }

    if (interrupt_type & QDEC_OV_PND) {
        interrupt_bit |= QDECCON_OV_PND;
    }

    if (interrupt_type & QDEC_SMP_PND) {
        interrupt_bit |= QDECCON_SMP_PND;
    }
    
    if (interrupt_type & QDEC_TERR_PND) {
        interrupt_bit |= QDECCON_TERR_PND;
    }
    
    if ((qdec->con & interrupt_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the QDEC interrupt pending.
  * @param  qdec: it can be QDECX/QDEXY/QDEXZ
  * @param  interrupt_type: specifies the IT to clear.
  *         this parameter can be one of the following values:
  *             @arg QDEC_REVERSE_DIR_PND:  QDEC reverse direction pending.
  *             @arg QDEC_FORWARD_DIR_PND:  QDEC forward direction pending.
  *             @arg QDEC_OV_PND:           QDEC overflow pending.
  *             @arg QDEC_SMP_PND:          QDEC sample pending.
  *             @arg QDEC_TERR_PND:         QDEC transition error pending.
  * @retval None
  */
AT(.com_periph.qdec.clear)
void qdec_clear_flag(qdec_typedef *qdec, QDEC_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type & QDEC_REVERSE_DIR_PND) {
        interrupt_bit |= QDECCON_REVERSE_PND;
    }

    if (interrupt_type & QDEC_FORWARD_DIR_PND) {
        interrupt_bit |= QDECCON_FORWARD_PND;
    }

    if (interrupt_type & QDEC_OV_PND) {
        interrupt_bit |= QDECCON_OV_PND;
    }

    if (interrupt_type & QDEC_SMP_PND) {
        interrupt_bit |= QDECCON_SMP_PND;
    }
    
    if (interrupt_type & QDEC_TERR_PND) {
        interrupt_bit |= QDECCON_TERR_PND;
    }

    qdec->cpnd |= interrupt_bit;
}

