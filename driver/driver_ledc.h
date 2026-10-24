/*
 * @File name    : driver_ledc.h
 * @Author       : Bluetrum IOT Team K.
 * @Date         : 2023-07-12
 * @Description  : This file contains all the functions protorypes for the LEDC library.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_LEDC_H
#define _DRIVER_LEDC_H

#include "driver_com.h"

/**
 * @brief Flag Enumeration.
 */
typedef enum {
    LEDC_FLAG_DONE          = 0x01,
    LEDC_FLAG_RST           = 0x02,
    LEDC_FLAG_DMA           = 0x04,
} LEDC_FLAG_TYPEDEF;

/**
 * @brief Output Invert Enumeration.
 */
typedef enum {
    LEDC_OUT_INV_DIS        = 0x00000000,
    LEDC_OUT_INV_EN         = 0x02000000,
} LEDC_OUT_INV_TYPEDEF;

/**
 * @brief DMA Buffer Data Format Enumeration.
 */
typedef enum {
    LEDC_IN_FORMAT_RGB      = 0x00000000,
    LEDC_IN_FORMAT_WRGB     = 0x00000080,
} LEDC_IN_FORMAT_TYPEDEF;

/**
 * @brief Idle Level Enumeration.
 */
typedef enum {
    LEDC_IDLE_LEVEL_LOW     = 0x00000000,
    LEDC_IDLE_LEVEL_HIGH    = 0x00000020,
} LEDC_IDLE_LEVEL_TYPEDEF;

/**
 * @brief Valid Data Direct or Invert Enumeration.
 */
typedef enum {
    LEDC_VALID_DATA_DIR     = 0x00000000,
    LEDC_VALID_DATA_INV     = 0x00000010,
} LEDC_VALID_DATA_TYPEDEF;

/**
 * @brief Valid Data Invert Byte Number Enumeration.
 */
typedef enum {
    LEDC_BYTE_INV_NO        = 0x00000000,
    LEDC_BYTE_INV_1B        = 0x00000004,
    LEDC_BYTE_INV_2B        = 0x00000008,
    LEDC_BYTE_INV_4B        = 0x0000000c,
} LEDC_BYTE_INV_TYPEDEF;

/**
 * @brief LEDC Ctrl Initialization Struct.
 */
typedef struct {
    LEDC_OUT_INV_TYPEDEF    output_invert;
    LEDC_IN_FORMAT_TYPEDEF  format;
    LEDC_IDLE_LEVEL_TYPEDEF idle_level;
    LEDC_VALID_DATA_TYPEDEF valid_data_mode;
    LEDC_BYTE_INV_TYPEDEF   byte_inv_number;
} ledc_init_typedef;

/**
 * @brief LEDC Code Element Timing Initialization Struct.
 */
typedef struct {
    uint8_t                 baud;
    uint8_t                 delay;
    uint8_t                 code_0_high;
    uint8_t                 code_1_high;
    uint16_t                reset_high;
    uint16_t                reset_low;
} ledc_timing_init_typedef;


void ledc_init(ledc_init_typedef *ledc_init_struct);
void ledc_timing_init(ledc_timing_init_typedef *ledc_timing_init_struct);
void ledc_deinit(void);
void ledc_cmd(FUNCTIONAL_STATE state);
void ledc_dma_kick(uint32_t addr, uint16_t len);
void ledc_kick(void);
void ledc_pic_config(isr_t isr, int pr, LEDC_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state);
FLAG_STATE ledc_get_flag(LEDC_FLAG_TYPEDEF flag_type);
void ledc_clear_flag(LEDC_FLAG_TYPEDEF flag_type);
void ledc_set_frame_length(uint16_t frame_length);
void ledc_set_loop_number(uint16_t loop_n);
uint16_t ledc_get_frame_length(void);
uint16_t ledc_get_loop_number(void);

#endif // _DRIVER_LEDC_H
