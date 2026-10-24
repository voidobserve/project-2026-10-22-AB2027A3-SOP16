/*
 * @File name    : driver_charge.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-06-20
 * @Description  : This file contains all the functions prototypes for the Charge library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_CHARGE_H
#define _DRIVER_CHARGE_H

#include "driver_com.h"


/**
 * @brief Charge Mode Enum
 */
typedef enum {
    CHARGE_MODE_FULL_KEEP       = 0x00,
    CHARGE_MODE_FULL_DISCONNECT = 0x01,
} CHARGE_MODE_TYPEDEF;

/**
 * @brief Charge DC Detect Enum
 */
typedef enum {
    CHARGE_DC_STATUS_OFF = 0x00,
    CHARGE_DC_STATUS_ONLINE_BUT_ERR = 0x01,
    CHARGE_DC_STATUS_ONLINE = 0x02,
} CHARGE_DC_STATUS_TYPEDEF;

/**
 * @brief Charge Status Enum
 */
typedef enum {
    CHARGE_STA_UNINIT           = 0x00,     // uninitialized.
    CHARGE_STA_OFF              = 0x01,     // charge off.
    CHARGE_STA_OFF_BUT_DC_IN    = 0x02,     // charge off but still connect to DC5V.
    CHARGE_STA_ON_TRICKLE       = 0x03,     // trickle charging.
    CHARGE_STA_ON_CON_CURR      = 0x04,     // constant current charging.
    CHARGE_STA_ON_CON_VOL       = 0x05,     // constant voltage charging.
} CHARGE_STATUS_TYPEDEF;

/**
 * @brief Charge DC In Reset System Enum
 */
typedef enum {
    CHARGE_DCIN_RESET_DIS       = 0x0000,
    CHARGE_DCIN_RESET_EN        = 0x0040,
} CHARGE_DCIN_RESERT_TYPEDEF;

/**
 * @brief Current Charging Enum
 */
typedef enum {
    CHARGE_CUR_5mA        = 0x0000,
    CHARGE_CUR_10mA,
    CHARGE_CUR_15mA,
    CHARGE_CUR_20mA,
    CHARGE_CUR_25mA,
    CHARGE_CUR_30mA,
    CHARGE_CUR_35mA,
    CHARGE_CUR_40mA,
    CHARGE_CUR_45mA,
    CHARGE_CUR_50mA,
    CHARGE_CUR_55mA,
    CHARGE_CUR_60mA,
    CHARGE_CUR_65mA,
    CHARGE_CUR_70mA,
    CHARGE_CUR_75mA,
    CHARGE_CUR_80mA,
    CHARGE_CUR_85mA,
    CHARGE_CUR_90mA,
    CHARGE_CUR_95mA,
    CHARGE_CUR_100mA,
    CHARGE_CUR_105mA,
    CHARGE_CUR_110mA,
    CHARGE_CUR_115mA,
    CHARGE_CUR_120mA,
    CHARGE_CUR_125mA,
    CHARGE_CUR_130mA,
    CHARGE_CUR_135mA,
    CHARGE_CUR_140mA,
    CHARGE_CUR_145mA,
    CHARGE_CUR_150mA,
    CHARGE_CUR_155mA,
    CHARGE_CUR_160mA,
    CHARGE_CUR_165mA,
    CHARGE_CUR_170mA,
    CHARGE_CUR_175mA,
    CHARGE_CUR_180mA,
    CHARGE_CUR_185mA,
    CHARGE_CUR_190mA,
    CHARGE_CUR_195mA,
    CHARGE_CUR_200mA,
    CHARGE_CUR_205mA,
    CHARGE_CUR_210mA,
    CHARGE_CUR_215mA,
    CHARGE_CUR_220mA,
    CHARGE_CUR_225mA,
    CHARGE_CUR_230mA,
    CHARGE_CUR_235mA,
    CHARGE_CUR_240mA,
    CHARGE_CUR_245mA,
    CHARGE_CUR_250mA,
    CHARGE_CUR_255mA,
    CHARGE_CUR_260mA,
    CHARGE_CUR_265mA,
    CHARGE_CUR_270mA,
    CHARGE_CUR_275mA,
    CHARGE_CUR_280mA,
    CHARGE_CUR_285mA,
    CHARGE_CUR_290mA,
    CHARGE_CUR_295mA,
    CHARGE_CUR_300mA,
    CHARGE_CUR_305mA,
    CHARGE_CUR_310mA,
    CHARGE_CUR_315mA,
    CHARGE_CUR_320mA,
} CHARGE_CUR_TYPEDEF;

/**
 * @brief Current of Cutoff charging Enum
 */
typedef enum {
    CHARGE_CUTOFF_CUR_0m        = 0x0000,
    CHARGE_CUTOFF_CUR_2mA5      = 0x0800,
    CHARGE_CUTOFF_CUR_5mA       = 0x1000,
    CHARGE_CUTOFF_CUR_7mA5      = 0x1800,
    CHARGE_CUTOFF_CUR_10mA      = 0x2000,
    CHARGE_CUTOFF_CUR_12mA5     = 0x2800,
    CHARGE_CUTOFF_CUR_15mA      = 0x3000,
    CHARGE_CUTOFF_CUR_17mA5     = 0x3800,
    CHARGE_CUTOFF_CUR_20mA      = 0x4000,
    CHARGE_CUTOFF_CUR_22mA5     = 0x4800,
    CHARGE_CUTOFF_CUR_25mA      = 0x5000,
    CHARGE_CUTOFF_CUR_27mA5     = 0x5800,
    CHARGE_CUTOFF_CUR_30mA      = 0x6000,
    CHARGE_CUTOFF_CUR_32mA5     = 0x6800,
    CHARGE_CUTOFF_CUR_35mA      = 0x7000,
    CHARGE_CUTOFF_CUR_37mA5     = 0x7800,
} CHARGE_CUTOFF_CUR_TYPEDEF;

/**
 * @brief Voltage of Cutoff Charging Enum
 */
typedef enum {
    CHARGE_CUTOFF_VOL_4V2       = 0x0000,
    CHARGE_CUTOFF_VOL_4V35      = 0x0100,
    CHARGE_CUTOFF_VOL_4V4       = 0x0200,
    CHARGE_CUTOFF_VOL_4V45      = 0x0300,
} CHARGE_CUTOFF_VOL_TYPEDEF;

/**
 * @brief Charge Current When Power Up Charge
 */
typedef enum {
    CHARGE_PWRUP_CUR_5mA        = 0x0000,
    CHARGE_PWRUP_CUR_10mA       = 0x0040,
    CHARGE_PWRUP_CUR_15mA       = 0x0080,
    CHARGE_PWRUP_CUR_20mA       = 0x00c0,
} CHARGE_PWRUP_CHARGE_CUR_TYPEDEF;


/**
 * @brief Charging Initial Struct
 */
typedef struct {
    CHARGE_DCIN_RESERT_TYPEDEF  dcin_reset;             // reset if DC insert.
    CHARGE_CUR_TYPEDEF          const_curr;             // the current of the constant currrent charging.
    CHARGE_CUR_TYPEDEF          trick_curr;             // the current of the trickle charging.
    CHARGE_CUTOFF_CUR_TYPEDEF   cutoff_curr;            // charging cutoff current.
    CHARGE_CUTOFF_VOL_TYPEDEF   cutoff_volt;            // charging cutoff voltage.
    FUNCTIONAL_STATE            trick_curr_en;          // trickle charging enable.
    CHARGE_MODE_TYPEDEF         mode;                   // disconnect or not when charge full.
} charge_init_typedef;

//--------------- Function used to configure charging ---------------//
void charge_init(charge_init_typedef *charge_init_struct);
void charge_deinit(void);
void charge_process(void);
CHARGE_DC_STATUS_TYPEDEF charge_detect_dc(void);
uint8_t charge_get_status(void);
void charge_status_update(u8 status);
void charge_change_mode(u8 mode);
u8 charge_get_mode(void);
FLAG_STATE charge_cutoff_cur_condition(void);
FLAG_STATE charge_cutoff_vol_condition(void);
FLAG_STATE charge_trickle_vol_condition(void);
void charge_cutoff_vol_to_stop_time_set(uint16_t time);
void charge_pwrup_charge_cur_sel(CHARGE_PWRUP_CHARGE_CUR_TYPEDEF pwrup_charge_cur);
bool charge_cutoff_vol_triming(s8 step);

#endif // _DRIVER_CHARGE_H
