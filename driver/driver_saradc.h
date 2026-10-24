/*
 * @File name    : driver_saradc.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file contains all the functions prototypes for the SARADC library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_SARADC_H
#define _DRIVER_SARADC_H

#include "driver_com.h"

/**
 * @brief ADC Channel
 */
#define ADC_CHANNEL_PA2                 0x00000001  // channel 0
#define ADC_CHANNEL_PA3                 0x00000002  // channel 1
#define ADC_CHANNEL_PA4                 0x00000004  // channel 2
#define ADC_CHANNEL_PA5                 0x00000008  // channel 3
#define ADC_CHANNEL_PA6                 0x00000010  // channel 4
#define ADC_CHANNEL_PA7                 0x00000020  // channel 5
#define ADC_CHANNEL_PB0                 0x00000040  // channel 6
#define ADC_CHANNEL_PB1                 0x00000080  // channel 7
#define ADC_CHANNEL_PB3                 0x00000100  // channel 8
#define ADC_CHANNEL_PB4                 0x00000200  // channel 9
#define ADC_CHANNEL_PB8                 0x00000400  // channel 10
#define ADC_CHANNEL_VRTC                0x00000800  // channel 11
#define ADC_CHANNEL_WK0                 0x00001000  // channel 12
#define ADC_CHANNEL_BG                  0x00002000  // channel 13, please do not use this channel, abandon, can use VRTC channel
#define ADC_CHANNEL_VBAT                0x00004000  // channel 14
#define ADC_CHANNEL_VUSB                0x00008000  // channel 15
#define ADC_CHANNEL_TSEN                0x00010000  // channel 15'

/**
 * @brief SARADC ANALOG BIT
 */
typedef enum {
    SARADC_ANALOG_BIT_VBAT      = 0x01,
    SARADC_ANALOG_BIT_BG        = 0x02,
    SARADC_ANALOG_BIT_WK0       = 0x04 ,
    SARADC_ANALOG_BIT_VRTC      = 0x08,
    SARADC_ANALOG_BIT_VUSB      = 0x10,
} SARADC_ANALOG_BIT_TYPEDEF;

/**
 * @brief SARADC Flag Definition
 */
typedef enum {
    SARADC_IT_FINISHED                  = 0x01,
} SARADC_IT_TYPEDEF;

/**
 * @brief SARADC Analog Mode Enumeration
 */
typedef enum {
    SARADC_MODE_NORMAL                  = 0x00000000,
    SARADC_MODE_TRIM                    = 0x00100000,
} SARADC_MODE_TYPEDEF;

/**
 * @brief SARADC Auto Enable Analog Enumeration
 */
typedef enum {
    SARADC_AUTO_ENABLE_ANALOG_DIS       = 0x00000000,
    SARADC_AUTO_ENABLE_ANALOG_EN        = 0x00080000,
} SARADC_AUTO_ENABLE_ANALOG_TYPEDEF;

/**
 * @brief SARADC Auto Enable IO Analog Enumeration
 */
typedef enum {
    SARADC_AUTO_ENABLE_ANALOG_IO_DIS    = 0x00000000,
    SARADC_AUTO_ENABLE_ANALOG_IO_EN     = 0x00040000,
} SARADC_AUTO_ENABLE_ANALOG_IO_TYPEDEF;

/**
 * @brief SARADC Internel Pullup Enumeration
 */
typedef enum {
    SARADC_PULLUP_DIS                   = 0x00000000,
    SARADC_PULLUP_EN                    = 0x00000001,
} SARADC_PULLUP_TYPEDEF;

/**
 * @brief SARADC setup timing enumeration
 */
typedef enum {
    SARADC_ST_0_CLK                     = 0x00000000,
    SARADC_ST_2_CLK                     = 0x00000001,
    SARADC_ST_4_CLK                     = 0x00000010,
    SARADC_ST_8_CLK                     = 0x00000011,
} SARADC_SETUP_TYPEDEF;

/**
 * @brief SARADC Base Init Structure
 */
typedef struct {
    uint32_t                                baud;
    SARADC_MODE_TYPEDEF                     mode;
    SARADC_AUTO_ENABLE_ANALOG_TYPEDEF       auto_analog;
    SARADC_AUTO_ENABLE_ANALOG_IO_TYPEDEF    auto_analog_io;
} saradc_base_init_typedef;

/**
 * @brief SARADC Channel Init structure
 */
typedef struct {
    uint32_t                                channel;
    FUNCTIONAL_STATE                        interrupt_en;
    SARADC_PULLUP_TYPEDEF                   pullup_en;
    SARADC_SETUP_TYPEDEF                    setup_time;
} saradc_channel_init_typedef;

void saradc_init(saradc_base_init_typedef* saradc_init_struct);
uint8_t saradc_deinit(void);
void saradc_channel_init(saradc_channel_init_typedef *saradc_channel_init_struct);
void saradc_cmd(FUNCTIONAL_STATE state);
void saradc_kick_start(uint32_t adc_ch);
void saradc_set_baud(uint32_t baud);
void saradc_pic_config(isr_t isr, int pr, SARADC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE saradc_get_flag(SARADC_IT_TYPEDEF interrupt_type);
FLAG_STATE saradc_get_channel_flag(uint32_t adc_ch);
void saradc_clear_flag(SARADC_IT_TYPEDEF interrupt_type);
uint32_t saradc_get_data(uint32_t adc_chx);

#endif  // _DRIVER_SARADC_H
