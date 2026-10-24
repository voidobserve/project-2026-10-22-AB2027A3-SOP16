/*
 * @File name    : driver_keyscan.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-03-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the KEYSCAN peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_keyscan.h"

/**
 * @brief  Initializes the KEYSCAN peripheral according to the specified
 *         parameters in the keyscan_init_struct.
 * @param keyscan_init_struct: pointer to a keyscan_init_typedef structure that
 *         contains the configuration information for the specified KEYSCAN peripheral.
 * @retval
 */
void keyscan_init(keyscan_init_typedef *keyscan_init_struct)
{
    /* Configure Control0 Register */
    KEYSCAN->con1 &= ~(KEYSCAN_CON1_KEYPORT_STAB_TIME | KEYSCAN_CON1_KEY_DEBO_TIMES | KEYSCAN_CON1_KEY_WAIT_TIMES);
    KEYSCAN->con1 |= (keyscan_init_struct->stable_times * KEYSCAN_CON1_KEYPORT_STAB_TIME_0) & KEYSCAN_CON1_KEYPORT_STAB_TIME;
    KEYSCAN->con1 |= (keyscan_init_struct->debounce_times * KEYSCAN_CON1_KEY_DEBO_TIMES_0) & KEYSCAN_CON1_KEY_DEBO_TIMES;
    KEYSCAN->con1 |= (keyscan_init_struct->wait_times * KEYSCAN_CON1_KEY_WAIT_TIMES_0) & KEYSCAN_CON1_KEY_WAIT_TIMES;

    /* Configure Key Scan Mode for Row/Column Scan */
    KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYSCAN_MODE;
    KEYSCAN->con0 |= keyscan_init_struct->key_mode & KEYSCAN_CON0_KEYSCAN_MODE;

    /* Configure Low Power Mode */
    KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYSCAN_LP;
    KEYSCAN->con0 |= keyscan_init_struct->low_power_mode & KEYSCAN_CON0_KEYSCAN_LP;

    /* Configure Key Valid Wakeup System */
    KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYVALID_WKUP_EN;
    KEYSCAN->con0 |= keyscan_init_struct->wakeup & KEYSCAN_CON0_KEYVALID_WKUP_EN;

    /* Configure Key Scan Software rstn */
    KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYSCAN_SWRSTN;
    KEYSCAN->con0 |= keyscan_init_struct->sw_reset & KEYSCAN_CON0_KEYSCAN_SWRSTN;

    /* Configure Port Mask */
    KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYPORT_EN;
    KEYSCAN->con0 |= (keyscan_init_struct->key_mask * KEYSCAN_CON0_KEYPORT_EN_0) & KEYSCAN_CON0_KEYPORT_EN;
}

/**
 * @brief  De-initialize the specified KeyScan peripheral.
 * @retval None
 */
void keyscan_deinit(void)
{
    KEYSCAN->con0 = 0;
    KEYSCAN->con1 = 0;

    /* The clock is not unique to this module */
    //clk_gate0_cmd(CLK_GATE0_LP, CLK_DIS);
}

/**
 * @brief  Enable or disable the KEYSCAN peripheral.
 * @param  state: state of the KEYSCAN peripheral.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None.
 */
AT(.com_periph.keyscan.en_ctrl)
void keyscan_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        KEYSCAN->con0 |= KEYSCAN_CON0_KEYSCAN_EN;
    } else {
        KEYSCAN->con0 &= ~KEYSCAN_CON0_KEYSCAN_EN;
    }
}

/**
 * @brief  Enable or disable the specified KEYSCAN interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  interrupt_type: specifies the KEYSCAN interrupt sources to be enable or disable.
 *         this parameter can be one of the following values:
 *             @arg KEYSCAN_IT_VALID: Any key has pressed interrupt.
 *             @arg KEYSCAN_IT_INVALID: All key release interrupt.
 * @param  state: the state of the KEYSCAN peripheral.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None.
 */
void keyscan_pic_config(isr_t isr, int pr, KEYSCAN_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & KEYSCAN_IT_VALID) {
        interrupt_pending_bit |= KEYSCAN_CON0_KEYVALID_INT_EN;
    }

    if (interrupt_type & KEYSCAN_IT_INVALID) {
        interrupt_pending_bit |= KEYSCAN_CON0_KEYINVALID_INT_EN;
    }

    if (state != DISABLE) {
        KEYSCAN->con0 |= interrupt_pending_bit;
        sys_irq_init(IRQn_KEY_SCAN, pr, isr);
    } else {
        KEYSCAN->con0 &= ~interrupt_pending_bit;
    }
}

/**
 * @brief  Get interrupt pendding function.
 * @param  interrupt_type: specifies the interrupt to get.
 *         this parameter can be one of the following values:
 *             @arg KEYSCAN_IT_VALID: Any key has pressed interrupt.
 *             @arg KEYSCAN_IT_INVALID: All key release interrupt.
 * @retval  The state of interrupt_type (SET or RESET).
 */
AT(.com_periph.keyscan.flag)
FLAG_STATE keyscan_get_flag(KEYSCAN_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & KEYSCAN_IT_VALID) {
        interrupt_pending_bit |= KEYSCAN_CLR_KEYVALID_PND_CLR;
    }

    if (interrupt_type & KEYSCAN_IT_INVALID) {
        interrupt_pending_bit |= KEYSCAN_CLR_KEYINVALID_PND_CLR;
    }

    if (KEYSCAN->con0 & interrupt_pending_bit) {
        return SET;
    } else {
        return RESET;
    }
}

/**
 * @brief  Clear interrupt pending function.
 * @param  interrupt_type: specifies the interrupt to clear.
 *         this parameter can be one of the following values:
 *             @arg KEYSCAN_IT_VALID: Any key has pressed interrupt.
 *             @arg KEYSCAN_IT_INVALID: All key release interrupt.
 * @retval None.
 */
AT(.com_periph.keyscan.flag)
void keyscan_clear_flag(KEYSCAN_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & KEYSCAN_IT_VALID) {
        interrupt_pending_bit |= KEYSCAN_CON0_KEYVALID_PND;
    }

    if (interrupt_type & KEYSCAN_IT_INVALID) {
        interrupt_pending_bit |= KEYSCAN_CON0_KEYINVALID_PND;
    }

    KEYSCAN->clr |= interrupt_pending_bit;
}

/**
 * @brief  Get Group A Key ID that be triggered.
 * @retval Key ID. All Key ID please see enumeration "KEYSCAN_GROUP_A_KEY_ID_TYPEDEF".
 */
AT(.com_periph.keyscan.query)
uint32_t keyscan_get_groupA_key_id(void)
{
    uint8_t  idx;
    uint8_t  *p;
    uint32_t info_reg[2];
    uint32_t key_val = 0;
    uint32_t key_val_img = 0;

    info_reg[0] = KEYSCAN->info0;
    info_reg[1] = KEYSCAN->info1;
    p = (u8 *)info_reg;

    key_val |= ((info_reg[0] & 0xfe) >> 1) * KA1;
    key_val |= ((info_reg[0] & 0xfc00) >> 10) * KB2;
    key_val |= ((info_reg[0] & 0xf80000) >> 19) * KC3;
    key_val |= ((info_reg[0] & 0xf0000000) >> 28) * KD4;
    key_val |= ((info_reg[1] & 0xe0) >> 5) * KE5;
    key_val |= ((info_reg[1] & 0xc000) >> 14) * KF6;
    key_val |= ((info_reg[1] & 0x800000) >> 23) * KG7;

    if (KEYSCAN->con0 & KEYSCAN_CON0_KEYSCAN_MODE) {
        //Resolve matrix keystroke conflicts
        for (u8 i = 0; i < 8; i++) {
            idx = i - 1;
            for (u8 j = 0; j < i; j++) {
                key_val_img |= (p[i] & (0x01  << j))? (0x01 << idx): 0;
                idx = idx + 6 - j;
            }
        }
    } else {
        key_val_img = 0xffffffff;
    }

    return key_val & key_val_img;
}

/**
 * @brief  Get Group B Key ID that be triggered.
 * @retval Key ID. All Key ID please see enumeration "KEYSCAN_GROUP_B_KEY_ID_TYPEDEF".
 */
AT(.com_periph.keyscan.query)
uint32_t keyscan_get_groupB_key_id(void)
{
    uint32_t info2_reg;
    uint32_t key_val = 0;

    info2_reg = KEYSCAN->info2;

    key_val |= (info2_reg & 0xff) * KJ0;
    key_val |= ((info2_reg & 0xff00) >> 8) * KI0;

    return key_val;
}

/**
 * @brief  Get low power mode configuration.
 * @retval 1:low power mode en, 0:low power mode dis.
 */
AT(.com_periph.keyscan.lowpwr_mode)
bool keyscan_is_lowpwr_mode(void)
{
    bool mode = (KEYSCAN->con0 & KEYSCAN_CON0_KEYSCAN_LP) >> 2;

    return mode;
}

