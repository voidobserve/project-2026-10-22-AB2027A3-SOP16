/*
 * @File name    : driver_sdadc.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-07-03
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the SDADC peripheral.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_sdadc.h"
#include "driver_clk.h"

/**
 * @brief  Initializes the sdadc peripheral according to the specified
 *         parameters in the sdadc_init_struct.
 * @param  sdadc_init_struct: pointer to a sdadc_init_typedef structure that
 *         contains the configuration information for the specified SDADC peripheral.
 * @retval None
 */
void sdadc_init(sdadc_init_typedef *sdadc_init_struct)
{
    /* Config sampling rate */
    SDADC->con &= ~SDADC_CON_FSOUT_SEL;
    SDADC->con |= sdadc_init_struct->sample_rate & SDADC_CON_FSOUT_SEL;

    /* Config MIC analog register */
    mic_adc_init(sdadc_init_struct->mic_analog_cfg);
    
    /* Config sw_sub dc status */
    if (sdadc_init_struct->subtract_soft_dc_en != DISABLE) {
        SDADC->con |= SDADC_CON_SW_DC_EN;
    } else {
        SDADC->con &= ~SDADC_CON_SW_DC_EN;
    }

    /* Config getdc status */
    if (sdadc_init_struct->get_dc_en != DISABLE) {
        SDADC->con |= SDADC_CON_GETDC_EN;
    } else {
        SDADC->con &= ~SDADC_CON_GETDC_EN;
    }

    /* Config remove dc status */
    if (sdadc_init_struct->remove_dc_en != DISABLE) {
        SDADC->con |= SDADC_CON_RMDC_EN;
    } else {
        SDADC->con &= ~SDADC_CON_RMDC_EN;
    }

    /* Config gain status */
    if (sdadc_init_struct->dig_gain_en != DISABLE) {
        SDADC->con |= SDADC_CON_GAIN_EN;
    } else {
        SDADC->con &= ~SDADC_CON_GAIN_EN;
    }
}

/**
 * @brief  De-initialize the specified SDADC peripheral.
 * @retval None
 */
void sdadc_deinit(void)
{    
    SDADC->con &= ~(SDADC_CON_SW_DC_EN | \
                    SDADC_CON_GETDC_EN | \
                    SDADC_CON_RMDC_EN | \
                    SDADC_CON_GAIN_EN | \
                    SDADC_CON_ADC_EN);

    SDADC->dma_con &= ~SDADC_DMACON_DMA_EN;
    
    mic_adc_deinit();

    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_DIS);
}

/**
 * @brief  Enable or disable the specified SDADC peripheral.
 * @param  state: the state of the SDADC peripheral.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void sdadc_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        SDADC->con |= SDADC_CON_ADC_EN;
    } else {
        SDADC->con &= ~SDADC_CON_ADC_EN;
    }
}

/**
 * @brief  Enable or disable the SDADC DMA function.
 * @param  addr: DMA start address.
 * @param  len: DMA transmit len.
 * @param  state: the new state need to config.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void sdadc_dma_cmd(u32 addr, u16 len, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        /* Config DMA address and size */
        SDADC->dma_addr = addr;
        SDADC->dma_size = len;
        SDADC->dma_con |= SDADC_DMACON_DMA_EN;
    } else {
        SDADC->dma_con &= ~SDADC_DMACON_DMA_EN;
    }
}

/**
 * @brief  Enable or disable the specified SDADC interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  flag_type: specifies the SDADC interrupt sources to be enable or disable.
 *         this parameter can be one of the following values:
 *             @arg SDADC_FLAG_SINGLE_SAMPLE: Single sampling complete interrupt.
 *             @arg SDADC_FLAG_DMA_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDADC_FLAG_DMA_HALF_DONE: DMA half-transfer interrupt.
 * @param  state: the state of the SDADC peripheral's interrupt.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void sdadc_pic_config(isr_t isr, int pr, SDADC_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state)
{
    u32 flag_pending_bit = 0;
    u32 all_flag_mask = (SDADC_DMACON_SAMPLES_IE | SDADC_DMACON_DMA_ALLDONE_IE | SDADC_DMACON_DMA_HALFDONE_IE);

    if (flag_type == 0) {
        return;
    }

    if (flag_type & SDADC_FLAG_SINGLE_SAMPLE) {
        flag_pending_bit |= SDADC_DMACON_SAMPLES_IE;
    }

    if (flag_type & SDADC_FLAG_DMA_ALL_DONE) {
        flag_pending_bit |= SDADC_DMACON_DMA_ALLDONE_IE;
    }

    if (flag_type & SDADC_FLAG_DMA_HALF_DONE) {
        flag_pending_bit |= SDADC_DMACON_DMA_HALFDONE_IE;
    }

    if (state != DISABLE) {
        SDADC->dma_con |= flag_pending_bit;
        sys_irq_init(IRQn_SDADC_DMA, pr, isr);
    } else {
        SDADC->dma_con &= ~flag_pending_bit;
        if (flag_pending_bit & all_flag_mask) {
            PICEN &= ~BIT(IRQn_SDADC_DMA);
        }
    }
}

/**
 * @brief  Check the specified SDADC flag is set or not.
 * @param  flag_type: specifies the flag to check.
 *         the parameter can be one of the following values:
 *             @arg SDADC_FLAG_SINGLE_SAMPLE: Single sampling complete interrupt.
 *             @arg SDADC_FLAG_DMA_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDADC_FLAG_DMA_HALF_DONE: DMA half-transfer interrupt.
 * @retval the state of flag_type (SET or RESET).
 */
AT(.com_periph.sdadc.flag)
FLAG_STATE sdadc_get_flag(SDADC_FLAG_TYPEDEF flag_type)
{
    u32 flag_pending_bit = 0;

    if (flag_type & SDADC_FLAG_SINGLE_SAMPLE) {
        flag_pending_bit |= SDADC_DMAPND_SAMPLEDONE;
    }

    if (flag_type & SDADC_FLAG_DMA_ALL_DONE) {
        flag_pending_bit |= SDADC_DMAPND_ALLDONE;
    }

    if (flag_type & SDADC_FLAG_DMA_HALF_DONE) {
        flag_pending_bit |= SDADC_DMAPND_HALFDONE;
    }

    if ((SDADC->dma_pend & flag_pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
 * @brief  Clear the SDADC pending.
 * @param  flag_type: specifies flag to clear.
 *         the parameter can be one of the following values:
 *             @arg SDADC_FLAG_SINGLE_SAMPLE: Single sampling complete interrupt.
 *             @arg SDADC_FLAG_DMA_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDADC_FLAG_DMA_HALF_DONE: DMA half-transfer interrupt.
 * @retval None
 */
AT(.com_periph.sdadc.flag)
void sdadc_clear_flag(SDADC_FLAG_TYPEDEF flag_type)
{
    u32 flag_pending_bit = 0;

    if (flag_type & SDADC_FLAG_SINGLE_SAMPLE) {
        flag_pending_bit |= SDADC_DMACPND_SAMPLEDONE_CLR;
    }

    if (flag_type & SDADC_FLAG_DMA_ALL_DONE) {
        flag_pending_bit |= SDADC_DMACPND_ALLDONE_CLR;
    }

    if (flag_type & SDADC_FLAG_DMA_HALF_DONE) {
        flag_pending_bit |= SDADC_DMACPND_HALFDONE_CLR;
    }

    SDADC->dma_cpnd |= flag_pending_bit;
}

/**
 * @brief  Set digital gain function.
 * @param  dig_gain: the value of digital gain need to set.
 * @retval None
 */
void sdadc_digital_gain_set(SDADC_DIGITAL_GAIN_TYPEDEF dig_gain)
{
    SDADC->gain_con = dig_gain & SDADC_GAINCON_GAIN;
}

/**
 * @brief  Set removeDC bits_len function.
 * @param  removeDC_bits_sel: the enum value of removeDC need to set.
 * @retval None
 */
void sdadc_removeDC_bits_set(SDADC_RMDC_BITS_TYPEDEF removeDC_bits_sel)
{
    u32 reg = SDADC->con;

    reg &= ~SDADC_CON_GETDC_SEL;
    reg |= removeDC_bits_sel & SDADC_CON_GETDC_SEL;

    SDADC->con = reg;
}

/**
 * @brief  Set getDC bits_len function.
 * @param  getDC_bits_sel: the enum value of getDC need to set.
 * @retval None
 */
void sdadc_getDC_bits_set(SDADC_GETDC_BITS_TYPEDEF getDC_bits_sel)
{
    u32 reg = SDADC->con;

    reg &= ~SDADC_CON_RMDC_SEL;
    reg |= getDC_bits_sel & SDADC_CON_RMDC_SEL;

    SDADC->con = reg;
}

/**
 * @brief  Get digital gain function.
 * @retval the value of digital gain.
 */
uint16_t sdadc_digital_gain_get(void)
{
    return (SDADC->gain_con & SDADC_GAINCON_GAIN);
}

/**
 * @brief  Get getDC bits_len function.
 * @retval the enum value of getDC_bits.
 */
SDADC_GETDC_BITS_TYPEDEF sdadc_getDC_bits_get(void)
{
    return (SDADC->con & SDADC_CON_GETDC_SEL);
}

/**
 * @brief  Get removeDC bits_len function.
 * @retval the enum value of removeDC_bits.
 */
SDADC_RMDC_BITS_TYPEDEF sdadc_removeDC_bits_get(void)
{
    return (SDADC->con & SDADC_CON_RMDC_SEL);
}
