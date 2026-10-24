/*
 * @File name    : driver_keyscan.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-03-15
 * @Description  : This file contains all the functions prototypes for the KEYSCAN library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_KEYSCAN_H
#define _DRIVER_KEYSCAN_H

#include "driver_com.h"

/**
 * @brief KEYSCAN Group A Key ID Enumeration
 */
typedef enum {
    KA1     = 0x00000001,
    KA2     = 0x00000002,
    KA3     = 0x00000004,
    KA4     = 0x00000008,
    KA5     = 0x00000010,
    KA6     = 0x00000020,
    KA7     = 0x00000040,
    KB2     = 0x00000080,
    KB3     = 0x00000100,
    KB4     = 0x00000200,
    KB5     = 0x00000400,
    KB6     = 0x00000800,
    KB7     = 0x00001000,
    KC3     = 0x00002000,
    KC4     = 0x00004000,
    KC5     = 0x00008000,
    KC6     = 0x00010000,
    KC7     = 0x00020000,
    KD4     = 0x00040000,
    KD5     = 0x00080000,
    KD6     = 0x00100000,
    KD7     = 0x00200000,
    KE5     = 0x00400000,
    KE6     = 0x00800000,
    KE7     = 0x01000000,
    KF6     = 0x02000000,
    KF7     = 0x04000000,
    KG7     = 0x08000000,
} KEYSCAN_GROUP_A_KEY_ID_TYPEDEF;

/**
 * @brief KEYSCAN Group B Key ID Enumeration
 */
typedef enum {
    KJ0     = 0x00000001,
    KJ1     = 0x00000002,
    KJ2     = 0x00000004,
    KJ3     = 0x00000008,
    KJ4     = 0x00000010,
    KJ5     = 0x00000020,
    KJ6     = 0x00000040,
    KJ7     = 0x00000080,
    KI0     = 0x00000100,
    KI1     = 0x00000200,
    KI2     = 0x00000400,
    KI3     = 0x00000800,
    KI4     = 0x00001000,
    KI5     = 0x00002000,
    KI6     = 0x00004000,
    KI7     = 0x00008000,
} KEYSCAN_GROUP_B_KEY_ID_TYPEDEF;

/**
 * @brief KEYSCAN Interrupt Flag Definition
 */
typedef enum {
    KEYSCAN_IT_VALID                = 0x01,
    KEYSCAN_IT_INVALID              = 0x02,
} KEYSCAN_IT_TYPEDEF;

/**
 * @brief Key Scan Software Release Reset Enumeration
 */
typedef enum {
    KEYSCAN_SWRSTN_RESET            = 0x00000000,
    KEYSCAN_SWRSTN_RELEASE_RESET    = 0x00000080,
} KEYSCAN_SWRSTN_TYPEDEF;

/**
 * @brief Key Valid Wakeup System Enumeration
 */
typedef enum {
    KEYSCAN_WKUP_DIS                = 0x00000000,
    KEYSCAN_WKUP_EN                 = 0x00000008,
} KEYSCAN_WKUP_TYPEDEF;

/**
 * @brief Key Scan Lowpower Mode Enumeration
 */
typedef enum {
    KEYSCAN_LOWPWR_DIS              = 0x00000000,
    KEYSCAN_LOWPWR_EN               = 0x00000004,
} KEYSCAN_LOWPWR_TYPEDEF;

/**
 * @brief Key Scan Mode Enumeration
 */
typedef enum {
    KEYSCAN_MODE_NORMAL             = 0x00000000,
    KEYSCAN_MODE_MATRIX             = 0x00000002,
} KEYSCAN_MODE_TYPEDEF;

/**
 * @brief KEYSCAN Init Structure
 */
typedef struct {
    uint8_t                     key_mask;
    uint8_t                     stable_times;
    uint8_t                     debounce_times;
    uint8_t                     wait_times;
    KEYSCAN_MODE_TYPEDEF        key_mode;
    KEYSCAN_LOWPWR_TYPEDEF      low_power_mode;
    KEYSCAN_SWRSTN_TYPEDEF      sw_reset;
    KEYSCAN_WKUP_TYPEDEF        wakeup;
} keyscan_init_typedef;

/********************** Function used to configure key_scan base **********************/
void keyscan_init(keyscan_init_typedef *keyscan_init_struct);
void keyscan_deinit(void);
void keyscan_cmd(FUNCTIONAL_STATE state);
void keyscan_pic_config(isr_t isr, int pr, KEYSCAN_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE keyscan_get_flag(KEYSCAN_IT_TYPEDEF interrupt_type);
void keyscan_clear_flag(KEYSCAN_IT_TYPEDEF interrupt_type);
uint32_t keyscan_get_groupA_key_id(void);
uint32_t keyscan_get_groupB_key_id(void);
bool keyscan_is_lowpwr_mode(void);

#endif // _DRIVER_KEYSCAN_H
