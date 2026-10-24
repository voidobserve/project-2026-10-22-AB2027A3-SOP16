/*
 * @File name    : driver_ledc.c
 * @Author       : Bluetrum IOT Team K.
 * @Date         : 2023-07-10
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the LEDC peripheral.
 * @WARNING      : Due to IC logical error, we should make sure no other peripherals 
                   use the same DMA address as LEDC(SRAM0), or dma of ledc would be error.
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_ledc.h"


/**
 * @brief  Initializes the LEDC peripheral according to the specified
 *         parameters in the ledc_timing_init_struct.
 * @param  ledc_timing_init_struct: pointer to a ledc_init_typedef structure that
 *         contains the configuration information for the specified LEDC peripheral.
 * @retval None
 */
void ledc_init(ledc_init_typedef *ledc_timing_init_struct)
{
    u32 tmp_reg;

    /* Load the register value to variable */
    tmp_reg = LEDC->con;

    /* Clear value need to configure */
    tmp_reg &= ~(uint32_t)(LEDC_CON_OUT_INV | \
                           LEDC_CON_BAUD | \
                           LEDC_CON_DELAY | \
                           LEDC_CON_IN_FORMAT | \
                           LEDC_CON_IDLE_LEVEL | \
                           LEDC_CON_VLD_INV);

    /* Config output data all invert */
    tmp_reg |= ledc_timing_init_struct->output_invert & LEDC_CON_OUT_INV;
    /* Config input format */
    tmp_reg |= ledc_timing_init_struct->format & LEDC_CON_IN_FORMAT;
    /* Config idle level */
    tmp_reg |= ledc_timing_init_struct->idle_level & LEDC_CON_IDLE_LEVEL;
    /* Config valid data direct or invert */
    tmp_reg |= ledc_timing_init_struct->valid_data_mode & LEDC_CON_VLD_INV;
    tmp_reg &= ~LEDC_CON_BYTE_INV;
    tmp_reg |= ledc_timing_init_struct->byte_inv_number & LEDC_CON_BYTE_INV;

    /* Restore the value to register */
    LEDC->con = tmp_reg;
}

/**
 * @brief  Initialize the code element timing accordingto the specified
 *         parameters in the ledc_timing_init_struct.
 * @param  ledc_timing_init_struct: pointer to a ledc_timing_init_typedef structure that
 *         contains the configuration information for the specified LEDC peripheral.
 * @retval None
 */
void ledc_timing_init(ledc_timing_init_typedef *ledc_timing_init_struct)
{
    u32 tmp_reg;
    ledc_timing_init_typedef *p = ledc_timing_init_struct;

    /* Load the register value to variable */
    tmp_reg = LEDC->con;
    /* Config baud */
    tmp_reg |= (ledc_timing_init_struct->baud << 16) & LEDC_CON_BAUD;
    /* Config delay */
    tmp_reg |= (ledc_timing_init_struct->delay << 8) & LEDC_CON_DELAY;
    /* Restore the value to register */
    LEDC->con = tmp_reg;

    /* Config code element timing */
    tmp_reg = 0;
    if ((p->code_1_high <= p->baud) && (p->code_0_high <= p->baud)) {
        tmp_reg = (p->code_1_high << 24) | \
                  ((p->baud - p->code_1_high) << 16) | \
                  (p->code_0_high << 8) | \
                  ((p->baud - p->code_0_high));
    }
    LEDC->tix = tmp_reg;

    /* Config reset timing */
    tmp_reg = (p->reset_low << 16) | p->reset_high;
    LEDC->rstx = tmp_reg;
}

/**
 * @brief  De-initialize the specified LEDC peripheral.
 * @retval None
 */
void ledc_deinit(void)
{
    LEDC->con &= ~(LEDC_CON_DMA_INT_EN | \
                   LEDC_CON_RST_INT_EN | \
                   LEDC_CON_INT_EN | \
                   LEDC_CON_EN);
    clk_gate2_cmd(CLK_GATE2_LEDC, CLK_DIS);
}

/**
 * @brief  Enable or disable the specified LEDC peripheral.
 * @param  state: the state of the LEDC peripheral.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
AT(.com_text.ledc.cmd)
void ledc_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        LEDC->con |= LEDC_CON_EN;
    } else {
        LEDC->con &= ~LEDC_CON_EN;
    }
}

/**
 * @brief  Enable the LEDC DMA function.
 * @param  addr: DMA start address.
 * @param  len: DMA transmit len.
 * @retval None
 */
AT(.com_periph.ledc.transport)
void ledc_dma_kick(uint32_t addr, uint16_t len)
{
    LEDC->addr = addr;
    LEDC->cnt = (0x01 << 31) | len;
}

/**
 * @brief  Start ledc transmit.
 * @retval None
 */
AT(.com_periph.ledc.transport)
void ledc_kick(void)
{
    LEDC->con |= LEDC_CON_LEDC_KICK;
}

/**
 * @brief  Enable or disable the specified LEDC interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  flag_type: specifies the LEDC interrupt sources to be enable or disable.
 *         This parameter can be one of the following values:
 *             @arg LEDC_FLAG_DONE: One frame has tx done.
 *             @arg LEDC_FLAG_RST: One frame has tx done and reset time has finished.
 *             @arg LEDC_FLAG_DMA: One DMA transmit finished.
 * @param  state: the new state need to config.
 * @retval None
 */
void ledc_pic_config(isr_t isr, int pr, LEDC_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state)
{
    u32 flag_pending_bit = 0;

    if (flag_type == 0) {
        return;
    }

    if (flag_type & LEDC_FLAG_DONE) {
        flag_pending_bit |= LEDC_CON_INT_EN;
    }

    if (flag_type & LEDC_FLAG_RST) {
        flag_pending_bit |= LEDC_CON_RST_INT_EN;
    }

    if (flag_type & LEDC_FLAG_DMA) {
        flag_pending_bit |= LEDC_CON_DMA_INT_EN;
    }

    if (state != DISABLE) {
        LEDC->con |= flag_pending_bit;
        sys_irq_init(IRQn_IR_QDEC_LEDC, pr, isr);
    } else {
        LEDC->con &= ~flag_pending_bit;
    }
}

/**
 * @brief  Check the specified LEDC flag is set or not.
 * @param  flag_type: specifies the flag to check.
 *         the parameter can be one of the following values:
 *             @arg LEDC_FLAG_DONE: One frame has tx done.
 *             @arg LEDC_FLAG_RST: One frame has tx done and reset time has finished.
 *             @arg LEDC_FLAG_DMA: One DMA transmit finished.
 * @retval The state of flag_type (SET or RESET).
 */
AT(.com_periph.ledc.flag)
FLAG_STATE ledc_get_flag(LEDC_FLAG_TYPEDEF flag_type)
{
    u32 flag_pending_bit = 0;

    if (flag_type & LEDC_FLAG_DONE) {
        flag_pending_bit |= LEDC_PEND_LEDC_PND;
    }

    if (flag_type & LEDC_FLAG_RST) {
        flag_pending_bit |= LEDC_PEND_RST_PND;
    }

    if (flag_type & LEDC_FLAG_DMA) {
        flag_pending_bit |= LEDC_PEND_DMA_PND;
    }

    if ((LEDC->pend & flag_pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
 * @brief  Clear the LEDC pending.
 * @param  flag_type: specified flag to clear.
 *             @arg LEDC_FLAG_DONE: One frame has tx done.
 *             @arg LEDC_FLAG_RST: One frame has tx done and reset time has finished.
 *             @arg LEDC_FLAG_DMA: One DMA transmit finished.
 * @retval None
 */
AT(.com_periph.ledc.flag)
void ledc_clear_flag(LEDC_FLAG_TYPEDEF flag_type)
{
    u32 flag_pending_bit = 0;

    if (flag_type & LEDC_FLAG_DONE) {
        flag_pending_bit |= LEDC_PEND_LEDC_PND;
    }

    if (flag_type & LEDC_FLAG_RST) {
        flag_pending_bit |= LEDC_PEND_RST_PND;
    }

    if (flag_type & LEDC_FLAG_DMA) {
        flag_pending_bit |= LEDC_PEND_DMA_PND;
    }

    LEDC->pend = flag_pending_bit;
}

/**
 * @brief  Set the length of frame length.
 * @param  frame_length: the length param of frame that need to config.
 * @retval None
 */
void ledc_set_frame_length(uint16_t frame_length)
{
    LEDC->fd = frame_length & 0xfff;
}

/**
 * @brief  Set the number of loop.
 * @param  loop_n: the number param of loop that need to config.
 * @retval None
 */
void ledc_set_loop_number(uint16_t loop_n)
{
    LEDC->lp = loop_n & 0x7ff;
}

/**
 * @brief  Get the length of frame length.
 * @retval None
 */
uint16_t ledc_get_frame_length(void)
{
    return (LEDC->fd & 0xfff);
}

/**
 * @brief  Get the number of loop.
 * @retval None
 */
uint16_t ledc_get_loop_number(void)
{
    return (LEDC->lp & 0x7ff);
}
