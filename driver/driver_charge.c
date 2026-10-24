/*
 * @File name    : driver_charge.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-06-20
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the Charge module.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_charge.h"


static void charge_DC_online_handle(void);
static bool charge_detect_off_condition(void);

// charging process parameter struct
static struct charge_status_def{
    u8 sta;
    u8 mode;
    u8 cutoff_vol;
    u8 cutoff_cur;
    u8 const_cur;
    u8 trickle_en;
    u8 trickle_cur;
} charge_status;

// charging process control struct
static struct charge_ctrl_def{
    u8 dc_insert;
    u8 dc_in_debounce_cnt;
    u8 reach_trick_curr_state_condition_debounce_cnt;
    u8 reach_const_curr_state_condition_debounce_cnt;
    u8 reach_cutoff_curr_debounce_cnt;
    u8 reach_cutoff_vol_debounce_cnt;
    u8 reach_cutoff_debounce_cnt;
    u16 cutoff_vol_2_stop_time;
    u16 cutoff_vol_2_stop_time_cnt;
    u32 vddio_tmp;
} charge_ctrl;

/**
  * @brief  Initializes the charge according to the specified
  *         parameters in the charge_init_struct.
  * @param  charge_init_struct: pointer to a charge_init_typedef structure that
  *         contains the configuration information for the charge register.
  * @retval None
  */
void charge_init(charge_init_typedef *charge_init_struct)
{
    charge_init_struct->cutoff_curr = charge_cutoff_curr_trim(charge_init_struct->cutoff_curr >> 11) << 11;
    charge_init_struct->const_curr = charge_const_curr_trim(charge_init_struct->const_curr);

    RTCCON8 &= ~((uint32_t)(0x03 << 8));
    RTCCON8 |= (uint32_t)(charge_init_struct->cutoff_volt);

    RTCCON7 &= ~((uint32_t)(0x0f << 11));
    RTCCON7 |= (uint32_t)(charge_init_struct->cutoff_curr);

    RTCCON &= ~((uint32_t)(0x01 << 6));
    RTCCON |= (uint32_t)charge_init_struct->dcin_reset;

    // set VUSB leakage current.
    RTCCON8 &= ~(0x07 << 16);
    RTCCON8 |= (0x02 << 16);

    memset(&charge_status, 0, sizeof(struct charge_status_def));
    memset(&charge_ctrl, 0, sizeof(struct charge_ctrl_def));

    // storage the configure value of charge.
    charge_status.trickle_en = charge_init_struct->trick_curr_en;
    charge_status.trickle_cur = charge_init_struct->trick_curr;
    charge_status.const_cur = charge_init_struct->const_curr;
    charge_status.mode = charge_init_struct->mode;

    // set the charge initial status to CHARGE_STA_OFF.
    charge_status.sta = CHARGE_STA_OFF;
}

/**
 * @brief  De-initialize the specified uart peripheral.
 * @retval None
 */
void charge_deinit(void)
{
    RTCCON8 &= ~(0x01 << 6);
    RTCCON8 |= (0x01 << 1);
    charge_status.sta = CHARGE_STA_UNINIT;
}

/**
  * @brief  process state switch during charging, this func should be called continuously
  *         during charging.
  * @param  None.
  * @retval None
  */
AT(.text.app.proc.charge)
void charge_process(void)
{
    // exit if charge uninitial.
    if (charge_status.sta == CHARGE_STA_UNINIT) {
        return;
    }

    // detect whether DC insert
    if ((charge_detect_dc()) != charge_ctrl.dc_insert) {
        charge_ctrl.dc_in_debounce_cnt++;
        if (charge_ctrl.dc_in_debounce_cnt > 5) {
            charge_ctrl.dc_insert = charge_detect_dc();
            charge_ctrl.dc_in_debounce_cnt = 0;
        }
    } else {
        charge_ctrl.dc_in_debounce_cnt = 0;
    }

    if (charge_ctrl.dc_insert == CHARGE_DC_STATUS_ONLINE) {
        charge_DC_online_handle();
    } else {
        if (charge_status.sta != CHARGE_STA_OFF) {
            charge_status_update(CHARGE_STA_OFF);
        }
    }
}

/**
  * @brief  Process event when VUSB insert.
  * @param  None.
  * @retval None
  */
static void charge_DC_online_handle(void)
{
    // if the charging is not begin.
    if (charge_status.sta == CHARGE_STA_OFF) {
        if (charge_status.trickle_en == DISABLE || (PWRCON1 & (1 << 29))) {         // trickle disable or battery higher then trickle voltage.
            if (charge_ctrl.reach_const_curr_state_condition_debounce_cnt > 10) {
                charge_ctrl.reach_const_curr_state_condition_debounce_cnt = 0;
                charge_status_update(CHARGE_STA_ON_CON_CURR);                       // constant current charging.
            } else {
                charge_ctrl.reach_const_curr_state_condition_debounce_cnt++;
            }
            charge_ctrl.reach_trick_curr_state_condition_debounce_cnt = 0;
        } else {
            if (charge_ctrl.reach_trick_curr_state_condition_debounce_cnt > 10) {
                charge_ctrl.reach_trick_curr_state_condition_debounce_cnt = 0;
                charge_status_update(CHARGE_STA_ON_TRICKLE);                        // trickle current charging.
            } else {
                charge_ctrl.reach_trick_curr_state_condition_debounce_cnt++;
            }
            charge_ctrl.reach_const_curr_state_condition_debounce_cnt = 0;
        }
    // if the charging is begin.
    } else if (charge_status.sta == CHARGE_STA_ON_TRICKLE) {
        if (PWRCON1 & (1 << 29)) {
            charge_ctrl.reach_const_curr_state_condition_debounce_cnt++;
        } else {
            charge_ctrl.reach_const_curr_state_condition_debounce_cnt = 0;
        }

        if (charge_ctrl.reach_const_curr_state_condition_debounce_cnt > 10) {
            charge_status_update(CHARGE_STA_ON_CON_CURR);
        }
    // if the charging at the state[con_curr, con_vol], detect stop condition.
    } else if (charge_status.sta > CHARGE_STA_ON_TRICKLE) {
        if (charge_detect_off_condition()) {
            charge_status_update(CHARGE_STA_OFF_BUT_DC_IN);
        }
    }
}

/**
  * @brief  Update the charging status if needed, and perform relevant configuration.
  * @param  status: the charging status that need to update.
  *         this parameter can be one of the following values:
  *             @arg CHARGE_STA_UNINIT: charge parameter uninitialized flag.
  *             @arg CHARGE_STA_OFF: charge off flag.
  *             @arg CHARGE_STA_OFF_BUT_DC_IN: charge off but VUSB still insert flag.
  *             @arg CHARGE_STA_ON_TRICKLE: charge on trickle stage flag.
  *             @arg CHARGE_STA_ON_CON_CURR: charge on const current stage flag.
  *             @arg CHARGE_STA_ON_CON_VOL: charge on const voltage stage flag.
  * @retval None
  */
void charge_status_update(u8 status)
{
    u32 reg_tmp;

    if (status == CHARGE_STA_OFF) {
        // Restore VDDIO Trim
        PWRCON0 = (PWRCON0 & ~(0x0f<<5)) | (charge_ctrl.vddio_tmp << 5);

        // VUSB to VDDIO LDO Disable
        RTCCON8 &= ~(1 << 0);

        // Stop Charge
		RTCCON8 &= ~(1 << 6);
        RTCCON8 |= (1 << 1);
    } else if (status == CHARGE_STA_OFF_BUT_DC_IN) {
        if (charge_status.mode == CHARGE_MODE_FULL_DISCONNECT) {
            RTCCON8 |= (1 << 1);
        }
    } else if (status == CHARGE_STA_ON_TRICKLE || status == CHARGE_STA_ON_CON_CURR) {
        reg_tmp = RTCCON7;

        if ((charge_status.sta != CHARGE_STA_ON_TRICKLE) && (charge_status.sta != CHARGE_STA_ON_CON_CURR)) {
            charge_ctrl.vddio_tmp = (PWRCON0 >> 5) & 0x0f;
        }

        // Enable VUSB to VDDIO LDO
        RTCCON8 |= (1 << 0);

        if (status == CHARGE_STA_ON_TRICKLE) {
            // Config VUSB to VDDIO LDO Voltage,Ensure VUSB_VDDIO > VBAT_VDDIO
            // 2.6:0.1:3.3  0x02 means 2.8V
            RTCCON8 = (RTCCON8 & ~(0x07<<20)) | (2 << 20);

            PWRCON0 &= ~(0x0f << 5);        //VBAT_VDDIO 2.4V

            reg_tmp &= ~(uint32_t)0x3f;
            reg_tmp |= charge_status.trickle_cur & 0x3f;
        } else {
            // Config VUSB to VDDIO LDO Voltage,Ensure VUSB_VDDIO > VBAT_VDDIO
            // 2.6:0.1:3.3  0x05 means 3.1V
            RTCCON8 = (RTCCON8 & ~(0x07<<20)) | (5 << 20);

            PWRCON0 = (PWRCON0 & ~(0x0f<<5)) | (4 << 5);    //VBAT_VDDIO 2.8V

            reg_tmp &= ~(uint32_t)0x3f;
            reg_tmp |= charge_status.const_cur & 0x3f;
        }
        charge_ctrl.reach_cutoff_curr_debounce_cnt = 0;
        charge_ctrl.reach_cutoff_vol_debounce_cnt = 0;

        RTCCON7 = reg_tmp;
        RTCCON8 |= (0x01 << 6);
        RTCCON8 &= ~(0x01 << 1);
    } else if (status == CHARGE_STA_ON_CON_VOL) {
		RTCCON8 |= (0x01 << 6);
        RTCCON8 &= ~(0x01 << 1);
    }

    charge_status.sta = status;
}

/**
  * @brief  detect charge off condition and enter const voltage stage condition.
  * @param  None.
  * @retval true or false that can stop charge.
  */
static bool charge_detect_off_condition(void)
{
    if (charge_ctrl.reach_cutoff_debounce_cnt && (charge_status.sta != CHARGE_STA_ON_CON_VOL)) {
        charge_ctrl.reach_cutoff_debounce_cnt--;
        // judge if the charging current lower then setting val.
        if (PWRCON1 & (1 << 30)) { //I
            charge_ctrl.reach_cutoff_curr_debounce_cnt++;
        }
        // judge if the charging voltage higher then setting val.
        if (PWRCON1 & (1 << 28)) { //V
            charge_ctrl.reach_cutoff_vol_debounce_cnt++;
        }
    }

    if (charge_ctrl.reach_cutoff_curr_debounce_cnt >= 98) {
        return true;
    } else if (charge_ctrl.reach_cutoff_vol_debounce_cnt >= 98) {
        if (charge_status.sta != CHARGE_STA_ON_CON_VOL) {
            charge_status_update(CHARGE_STA_ON_CON_VOL);
            charge_ctrl.reach_cutoff_debounce_cnt = 0;
        }

        if (charge_ctrl.reach_cutoff_debounce_cnt == 0) {
            charge_ctrl.reach_cutoff_debounce_cnt = 100;
            charge_ctrl.reach_cutoff_curr_debounce_cnt = 0;
        } else {
            charge_ctrl.reach_cutoff_debounce_cnt--;
            if (PWRCON1 & (1 << 30)) {            //I
                charge_ctrl.reach_cutoff_curr_debounce_cnt++;
            }
        }

        if (charge_ctrl.cutoff_vol_2_stop_time) {
            charge_ctrl.cutoff_vol_2_stop_time_cnt++;
            if (charge_ctrl.cutoff_vol_2_stop_time_cnt > charge_ctrl.cutoff_vol_2_stop_time) {
                charge_ctrl.cutoff_vol_2_stop_time_cnt = 0;
                return true;
            }
        }
    } else {
        if (charge_ctrl.reach_cutoff_debounce_cnt == 0) {
            charge_ctrl.reach_cutoff_debounce_cnt = 100;
            charge_ctrl.reach_cutoff_curr_debounce_cnt = 0;
            charge_ctrl.reach_cutoff_vol_debounce_cnt = 0;
        }
    }

    return false;
}

/**
  * @brief  Update the charging status if needed, and perform relevant configuration.
  * @param  None.
  * @retval the status of charge stage. the value will be one in the CHARGE_STATUS_TYPEDEF.
  */
AT(.com_periph.charge)
u8 charge_get_status(void) {
    return charge_status.sta;
}

/**
  * @brief  select the event to do when charge is finish.
  * @param  mode: charge mode.
  *         this parameter can be one of the following values:
  *             @arg CHARGE_MODE_FULL_KEEP: keep VUSB connect when charge finish.
  *             @arg CHARGE_MODE_FULL_DISCONNECT: disconnect VUSB when charge finish.
  * @retval None
  */
void charge_change_mode(u8 mode)
{
    if ((mode == CHARGE_MODE_FULL_DISCONNECT) || (mode == CHARGE_MODE_FULL_KEEP)) {
        charge_status.mode = mode;
    }
}

/**
  * @brief  Update the charging mode if needed, and perform relevant configuration.
  * @param  None.
  * @retval mode of charge stage. the value will be one in the CHARGE_MODE_TYPEDEF.
  */
u8 charge_get_mode(void)
{
    return charge_status.mode;
}

/**
  * @brief  get state VUSB insert.
  * @param  None.
  * @retval VUSB insertion statue.
  */
AT(.com_periph.charge)
CHARGE_DC_STATUS_TYPEDEF charge_detect_dc(void)
{
    u8 status = (RTCCON >> 19 & 0x02) | (RTCCON >> 22 & 0x01);

    if (status > 1) {
        return CHARGE_DC_STATUS_ONLINE;
    } else {
        return status & 0x03;
    }
}

/**
  * @brief  Judge if the charging current reach the cutoff current.
  * @param  None.
  * @retval the condition SET of RESET.
  */
FLAG_STATE charge_cutoff_cur_condition(void)
{
    if ((PWRCON1 & (1 << 30)) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Judge if the charging voltage reach the cutoff voltage.
  * @param  None.
  * @retval the condition SET of RESET.
  */
FLAG_STATE charge_cutoff_vol_condition(void)
{
    if ((PWRCON1 & (1 << 28)) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Judge if the charging voltage reach the trickle voltage.
  * @param  None.
  * @retval the condition SET of RESET.
  */
FLAG_STATE charge_trickle_vol_condition(void)
{
    if ((PWRCON1 & (1 << 29)) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Set the time from the cut-off voltage to stop charging.
  * @param  time, unit:100ms.
  */
void charge_cutoff_vol_to_stop_time_set(uint16_t time)
{
    charge_ctrl.cutoff_vol_2_stop_time = time;
}

/**
  * @brief  Select charge current when power up charge.
  */
void charge_pwrup_charge_cur_sel(CHARGE_PWRUP_CHARGE_CUR_TYPEDEF pwrup_charge_cur)
{
    RTCCON7 = (RTCCON7 & ~(0x03<<6)) | (pwrup_charge_cur);
}

/**
  * @brief  Triming Cutoff Voltage.(X)mV per step. Vcutoff / trming = Vnew_cutoff / (triming + step), X = (Vnew_cutoff - Vcutoff) / step.
  * @retval false - trim failed, keep original value.
  *         true - trim successful.
  */
bool charge_cutoff_vol_triming(s8 step)
{
    uint8_t bg_charger_triming = (RTCCON8 >> 10) & 0x1f;

    if ((bg_charger_triming + step) > 0x1f) {
        return false;
    }

    bg_charger_triming  += step;
    RTCCON8 = (RTCCON8 & ~(0x1f << 10)) | (bg_charger_triming << 10);

    return true;
}
