/*
 * @File name    : driver_sdadc.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-06-08
 * @Description  : This file contains all the functions prototypes for the SDADC library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_SDADC_H
#define _DRIVER_SDADC_H

#include "driver_com.h"


/**
 * @brief Flag Enumeration.
 */
typedef enum {
    SDADC_FLAG_SINGLE_SAMPLE    = 0x01,
    SDADC_FLAG_DMA_ALL_DONE     = 0x02,
    SDADC_FLAG_DMA_HALF_DONE    = 0x04,
} SDADC_FLAG_TYPEDEF;

/**
 * @brief Digital Gain Select Enumeration.
 */
typedef enum {
    DIG_GAIN_400                = 0x0400,
    DIG_GAIN_800                = 0x0800,
    DIG_GAIN_1600               = 0x1600,
    DIG_GAIN_3200               = 0x3200,
    DIG_GAIN_4800               = 0x4800,
    DIG_GAIN_6400               = 0x6400,
    DIG_GAIN_8000               = 0x8000,
} SDADC_DIGITAL_GAIN_TYPEDEF;

/**
 * @brief Sample Rate Enumeration.
 */
typedef enum {
    SPR_48000                   = 0x00000000,
    SPR_32000                   = 0x00000002,
    SPR_24000                   = 0x00000004,
    SPR_16000                   = 0x00000006,
    SPR_12000                   = 0x00000008,
    SPR_8000                    = 0x0000000a,
} SDADC_SAMPLING_TYPEDEF;

/**
 * @brief GETDC Bits Enumeration.
 */
typedef enum{
    SDADC_GETDC_BITS_13BITS     = 0x00000000,
    SDADC_GETDC_BITS_12BITS     = 0x00000400,
    SDADC_GETDC_BITS_11BITS     = 0x00000800,
    SDADC_GETDC_BITS_10BITS     = 0x00000c00,
} SDADC_GETDC_BITS_TYPEDEF;

/**
 * @brief RemoveDC Bits Enumeration.
 */
typedef enum{
    SDADC_RMDC_BITS_10BITS      = 0x00000000,
    SDADC_RMDC_BITS_9BITS       = 0x00000100,
    SDADC_RMDC_BITS_8BITS       = 0x00000200,
    SDADC_RMDC_BITS_7BITS       = 0x00000300,
} SDADC_RMDC_BITS_TYPEDEF;

/**
 * @brief SDADC Initialization Struct.
 */
typedef struct{
    SDADC_SAMPLING_TYPEDEF sample_rate;
    MIC_ADC_CFG mic_analog_cfg;
    FUNCTIONAL_STATE subtract_soft_dc_en;
    FUNCTIONAL_STATE get_dc_en;
    FUNCTIONAL_STATE remove_dc_en;
    FUNCTIONAL_STATE dig_gain_en;
} sdadc_init_typedef;


void sdadc_init(sdadc_init_typedef *sdadc_init_struct);
void sdadc_deinit(void);
void sdadc_cmd(FUNCTIONAL_STATE state);
void sdadc_dma_cmd(u32 addr, u16 len, FUNCTIONAL_STATE state);
void sdadc_pic_config(isr_t isr, int pr, SDADC_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state);
FLAG_STATE sdadc_get_flag(SDADC_FLAG_TYPEDEF flag_type);
void sdadc_clear_flag(SDADC_FLAG_TYPEDEF flag_type);
void sdadc_digital_gain_set(SDADC_DIGITAL_GAIN_TYPEDEF dig_gain);
void sdadc_removeDC_bits_set(SDADC_RMDC_BITS_TYPEDEF removeDC_bits_sel);
void sdadc_getDC_bits_set(SDADC_GETDC_BITS_TYPEDEF getDC_bits_sel);
uint16_t sdadc_digital_gain_get(void);
SDADC_GETDC_BITS_TYPEDEF sdadc_getDC_bits_get(void);
SDADC_RMDC_BITS_TYPEDEF sdadc_removeDC_bits_get(void);

#endif
