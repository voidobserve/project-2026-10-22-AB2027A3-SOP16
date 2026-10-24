/*
 * @File name    : driver_lpwm.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-03-06
 * @Description  : This file contains all the functions prototypes for the LPWM library.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_LPWM_H
#define _DRIVER_LPWM_H

#include "driver_com.h"

/**
 * @brief LPWM Pend Flag Definition
 */
typedef enum {
    LPWM_FLAG_L_STAGE       = 0x01,
} LPWM_FLAG_TYPEDEF;

/**
 * @brief LPWM Stage Select Enumeration
 */
typedef enum {
    LPWM_STAGE_A            = 0x01,
    LPWM_STAGE_B            = 0x02,
    LPWM_STAGE_C            = 0x04,
    LPWM_STAGE_D            = 0x08,
    LPWM_STAGE_E            = 0x10,
    LPWM_STAGE_F            = 0x20,
    LPWM_STAGE_L            = 0x40,
    LPWM_STAGE_H            = 0x80,
} LPWM_STAGE_TYPEDEF;

/**
 * @brief LPWM Mode Select Enumeration
 */
typedef enum {
    LPWM_MODE_NORMAL        = 0x00000000,
    LPWM_MODE_BREATH        = 0x00000002,
} LPWM_MODE_TYPEDEF;

/**
 * @brief LPWM Breath Mode Change Dir Select Enumeration
 */
typedef enum {
    LPWM_DIR_INCREASE       = 0x00000000,
    LPWM_DIR_DECREASE       = 0x00000800,
} LPWM_DIR_TYPEDEF;

/**
 * @brief LPWM Stage Init Structure
 */
typedef struct {
    uint8_t             step;
    uint16_t            repeat;
    uint16_t            start_duty;
} lpwm_stage_init_typedef;

/**
 * @brief LPWM Init Structure
 */
typedef struct {
    LPWM_MODE_TYPEDEF   mode;
    LPWM_DIR_TYPEDEF    breath_start_dir;
    uint16_t            period;
    uint16_t            first_duty;
} lpwm_base_init_typedef;

/********************* Function used to config LPWM peripheral *********************/
void lpwm_init(lpwm_base_init_typedef *lpwm_base_init_struct);
void lpwm_deinit(void);
void lpwm_stage_config(LPWM_STAGE_TYPEDEF stage, lpwm_stage_init_typedef *stage_init_struct, FUNCTIONAL_STATE state);
void lpwm_cmd(FUNCTIONAL_STATE state);
FLAG_STATE lpwm_get_flag(LPWM_FLAG_TYPEDEF flag_type);
void lpwm_clear_flag(LPWM_FLAG_TYPEDEF flag_type);

#endif // _DRIVER_LPWM_H
