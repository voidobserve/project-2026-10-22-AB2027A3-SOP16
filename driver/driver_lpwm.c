/*
 * @File name    : driver_lpwm.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-03-06
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the LPWM peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_lpwm.h"

/**
 * @brief  Initializes the LPWM peripheral according to the specified
 *         parameters in the lpwm_init_struct.
 * @param  lpwm_init_struct: pointer to a lpwm_base_init_typedef structure that
 *         contains the configuration information for the specified LPWM peripheral.
 * @retval None
 */
void lpwm_init(lpwm_base_init_typedef *lpwm_base_init_struct)
{
    /* Disable peripheral before config */
    LPWM->start_con &= ~LPWMSTCON_LPWM_EN;

    /* Configure mode */
    LPWM->start_con &= ~LPWMSTCON_BREATH_MODE;
    LPWM->start_con |= lpwm_base_init_struct->mode & LPWMSTCON_BREATH_MODE;

    /* Configure period and duty of normal mode or first duty of breath mode */
    if ((lpwm_base_init_struct->mode & LPWMSTCON_BREATH_MODE) != LPWM_MODE_NORMAL) {
        LPWM->start_con &= ~LPWMSTCON_LPWM_DIR;
        LPWM->start_con |= lpwm_base_init_struct->breath_start_dir & LPWMSTCON_LPWM_DIR;

        LPWM->cnt_max_con &= ~LPWMCMCON_BRT_CNTMAX;
        LPWM->cnt_max_con |= (lpwm_base_init_struct->period * LPWMCMCON_BRT_CNTMAX_0) & LPWMCMCON_BRT_CNTMAX;

        LPWM->start_con &= ~LPWMSTCON_ST_DUTY;
        LPWM->start_con |= (lpwm_base_init_struct->first_duty * LPWMSTCON_ST_DUTY_0) & LPWMSTCON_ST_DUTY;
    } else {
        LPWM->cnt_max_con &= ~LPWMCMCON_SWT_CNTMAX;
        LPWM->cnt_max_con |= (lpwm_base_init_struct->period * LPWMCMCON_SWT_CNTMAX_0) & LPWMCMCON_SWT_CNTMAX;

        LPWM->switch_con &= ~LPWMSWCON_DUTY_ON;
        LPWM->switch_con |= (lpwm_base_init_struct->first_duty * LPWMSWCON_DUTY_ON_0) & LPWMSWCON_DUTY_ON;
    }
}

/**
 * @brief  De-initialize the specified LPWM peripheral.
 * @retval None
 */
void lpwm_deinit(void)
{
    u8 i;
    volatile uint32_t *lpwm_con_base = &(LPWM->stageA_con);

    for (i = 0; i < 6; i++) {
        lpwm_con_base[i] &= ~LPWMxCON_EN;
    }

    LPWM->stageHL_con &= ~(LPWMHLCON_L_EN | LPWMHLCON_H_EN);

    LPWM->start_con = 0;

    clk_gate1_cmd(CLK_GATE1_LPWM, CLK_EN);
}

/**
 * @brief  Configure the LPWM stage that needs to be enabled or disabled.
 * @param  stage: the stage that need to config.
 * @param  stage_init_struct: pointer to a lpwm_stage_init_typedef structure that
 *         contains the configuration information for the specified LPWM peripheral.
 * @param  state: state of the stage of LPWM peripheral.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void lpwm_stage_config(LPWM_STAGE_TYPEDEF stage, lpwm_stage_init_typedef *stage_init_struct, FUNCTIONAL_STATE state)
{
    uint8_t i;
    volatile uint32_t *lpwm_con_base = &(LPWM->stageA_con);

    /* Configure A..F LPWM stage register */
    for (i = 0; i < 6; i++) {
        if (stage & (1 << i)) {
            if (state != DISABLE) {
                lpwm_con_base[i] &= ~LPWMxCON_EN;

                lpwm_con_base[i] &= ~LPWMxCON_REPEAT;
                lpwm_con_base[i] |= (stage_init_struct->repeat * LPWMxCON_REPEAT_0) & LPWMxCON_REPEAT;

                lpwm_con_base[i] &= ~LPWMxCON_STEP;
                lpwm_con_base[i] |= (stage_init_struct->step * LPWMxCON_STEP_0) & LPWMxCON_STEP;

                lpwm_con_base[i] &= ~LPWMxCON_ST_DUTY;
                lpwm_con_base[i] |= (stage_init_struct->start_duty * LPWMxCON_ST_DUTY_0) & LPWMxCON_ST_DUTY;

                lpwm_con_base[i] |= LPWMxCON_EN;
            } else {
                lpwm_con_base[i] &= ~LPWMxCON_EN;
            }
        }
    }

    /* Configure max hold time */
    if (stage & LPWM_STAGE_L) {
        if (state != DISABLE) {
            LPWM->stageHL_con &= ~LPWMHLCON_L_EN;

            LPWM->stageHL_con &= ~LPWMHLCON_L_REPEAT;
            LPWM->stageHL_con |= (stage_init_struct->repeat * LPWMHLCON_L_REPEAT_0) & LPWMHLCON_L_REPEAT;

            LPWM->stageHL_con |= LPWMHLCON_L_EN;
        } else {
            LPWM->stageHL_con &= ~LPWMHLCON_L_EN;
        }
    }

    /* Configure min hold time */
    if (stage & LPWM_STAGE_H) {
        if (state != DISABLE) {
            LPWM->stageHL_con &= ~LPWMHLCON_H_EN;

            LPWM->stageHL_con &= ~LPWMHLCON_H_REPEAT;
            LPWM->stageHL_con |= (stage_init_struct->repeat * LPWMHLCON_H_REPEAT_0) & LPWMHLCON_H_REPEAT;

            LPWM->stageHL_con |= LPWMHLCON_H_EN;
        } else {
            LPWM->stageHL_con &= ~LPWMHLCON_H_EN;
        }
    }
}

/**
 * @brief  Enable or disable LPWM peripheral.
 * @param  state: State of LPWM peripheral.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void lpwm_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        LPWM->start_con |= LPWMSTCON_LPWM_EN;
    } else {
        LPWM->start_con &= ~LPWMSTCON_LPWM_EN;
    }
}

/**
 * @brief  Get the LPWM flag state.
 * @param  flag_type: specifies the flag to get.
 * @retval state of the specified flag (SET or RESET)
 */
FLAG_STATE lpwm_get_flag(LPWM_FLAG_TYPEDEF flag_type)
{
    uint32_t pending_bit = 0;

    if (flag_type & LPWM_FLAG_L_STAGE) {
        pending_bit |= LPWMPND_LPWM_PND;
    }

    if ((LPWM->pend & pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
 * @brief  Clear LPWM pending flag.
 * @param  flag_type: specifies the flag to clear.
 *         this parameter can be one of the following values:
 *             @arg LPWM_FLAG_L_STAGE: stage L begin pending.
 * @retval None
 */
void lpwm_clear_flag(LPWM_FLAG_TYPEDEF flag_type)
{
    uint32_t pending_bit = 0;

    if (flag_type & LPWM_FLAG_L_STAGE) {
        pending_bit |= LPWMPND_LPWM_PND;
    }

    LPWM->pend |= pending_bit;
}
