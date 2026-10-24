/*
 * @File name    : driver_clk.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-04-04
 * @Description  : This file contains all the functions prototypes for the CLK library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_CLK_H
#define _DRIVER_CLK_H

#include "global.h"
#include "api.h"


#define CLK_GATE0_ROM0                  ((uint32_t)(0x00000001))        /* CLKGAT0[0] */
#define CLK_GATE0_ROM1                  ((uint32_t)(0x00000002))        /* CLKGAT0[1] */
#define CLK_GATE0_RAM0                  ((uint32_t)(0x00000004))        /* CLKGAT0[2] */
#define CLK_GATE0_RAM1                  ((uint32_t)(0x00000008))        /* CLKGAT0[3] */
#define CLK_GATE0_RAM2                  ((uint32_t)(0x00000010))        /* CLKGAT0[4] */
#define CLK_GATE0_RAM3                  ((uint32_t)(0x00000020))        /* CLKGAT0[5] */
#define CLK_GATE0_RAM4                  ((uint32_t)(0x00000040))        /* CLKGAT0[6] */
#define CLK_GATE0_TMR0                  ((uint32_t)(0x00000100))        /* CLKGAT0[8] */
#define CLK_GATE0_SD0                   ((uint32_t)(0x00000200))        /* CLKGAT0[9] */
#define CLK_GATE0_UART                  ((uint32_t)(0x00000400))        /* CLKGAT0[10] */
#define CLK_GATE0_HSUART                ((uint32_t)(0x00000800))        /* CLKGAT0[11] */
#define CLK_GATE0_DAC                   ((uint32_t)(0x00001000))        /* CLKGAT0[12] */
#define CLK_GATE0_SARADC                ((uint32_t)(0x00002000))        /* CLKGAT0[13] */
#define CLK_GATE0_USB                   ((uint32_t)(0x00004000))        /* CLKGAT0[14] */
#define CLK_GATE0_LP                    ((uint32_t)(0x00008000))        /* CLKGAT0[15] */
#define CLK_GATE0_AUDEC                 ((uint32_t)(0x00010000))        /* CLKGAT0[16] */
#define CLK_GATE0_POTR                  ((uint32_t)(0x00020000))        /* CLKGAT0[17] */
#define CLK_GATE0_MBIST                 ((uint32_t)(0x00040000))        /* CLKGAT0[18] */
#define CLK_GATE0_SPI0                  ((uint32_t)(0x00080000))        /* CLKGAT0[19] */
#define CLK_GATE0_BT                    ((uint32_t)(0x00100000))        /* CLKGAT0[20] */
#define CLK_GATE0_UDET                  ((uint32_t)(0x00200000))        /* CLKGAT0[21] */
#define CLK_GATE0_SDADCL                ((uint32_t)(0x00400000))        /* CLKGAT0[22] */
#define CLK_GATE0_SDADCR                ((uint32_t)(0x00800000))        /* CLKGAT0[23] */
#define CLK_GATE0_TMR1                  ((uint32_t)(0x01000000))        /* CLKGAT0[24] */
#define CLK_GATE0_TMR2                  ((uint32_t)(0x02000000))        /* CLKGAT0[25] */
#define CLK_GATE0_RTCC                  ((uint32_t)(0x04000000))        /* CLKGAT0[26] */
#define CLK_GATE0_RECSRC                ((uint32_t)(0x08000000))        /* CLKGAT0[27] */
#define CLK_GATE0_SDADCM                ((uint32_t)(0x10000000))        /* CLKGAT0[28] */
#define CLK_GATE0_SPF                   ((uint32_t)(0x20000000))        /* CLKGAT0[29] */
#define CLK_GATE0_AEC                   ((uint32_t)(0x40000000))        /* CLKGAT0[30] */
#define CLK_GATE0_CVSD                  ((uint32_t)(0x80000000))        /* CLKGAT0[31] */

#define CLK_GATE1_IRTX                  ((uint32_t)(0x00000004))        /* CLKGAT1[2] */
#define CLK_GATE1_IRRX                  ((uint32_t)(0x00000008))        /* CLKGAT1[3] */
#define CLK_GATE1_SBCEC                 ((uint32_t)(0x00000020))        /* CLKGAT1[5] */
#define CLK_GATE1_LPBK                  ((uint32_t)(0x00000040))        /* CLKGAT1[6] */
#define CLK_GATE1_PLC                   ((uint32_t)(0x00000080))        /* CLKGAT1[7] */
#define CLK_GATE1_TMR3                  ((uint32_t)(0x00000100))        /* CLKGAT1[8] */
#define CLK_GATE1_UDET                  ((uint32_t)(0x00000800))        /* CLKGAT1[11] */
#define CLK_GATE1_SPI1                  ((uint32_t)(0x00001000))        /* CLKGAT1[12] */
#define CLK_GATE1_QDECX                 ((uint32_t)(0x00002000))        /* CLKGAT1[13] */
#define CLK_GATE1_QDECY                 ((uint32_t)(0x00004000))        /* CLKGAT1[14] */
#define CLK_GATE1_PIANO                 ((uint32_t)(0x00008000))        /* CLKGAT1[15] */
#define CLK_GATE1_TICK0                 ((uint32_t)(0x00010000))        /* CLKGAT1[16] */
#define CLK_GATE1_QDECZ                 ((uint32_t)(0x00020000))        /* CLKGAT1[17] */
#define CLK_GATE1_LPWM                  ((uint32_t)(0x00040000))        /* CLKGAT1[18] */
#define CLK_GATE1_AECRAM                ((uint32_t)(0x00800000))        /* CLKGAT1[23] */
#define CLK_GATE1_ROM2                  ((uint32_t)(0x01000000))        /* CLKGAT1[24] */
#define CLK_GATE1_ROM3                  ((uint32_t)(0x02000000))        /* CLKGAT1[25] */
#define CLK_GATE1_FREQDET               ((uint32_t)(0x04000000))        /* CLKGAT1[26] */
#define CLK_GATE1_PBF                   ((uint32_t)(0x08000000))        /* CLKGAT1[27] */
#define CLK_GATE1_DBG                   ((uint32_t)(0x10000000))        /* CLKGAT1[28] */
#define CLK_GATE1_X24M                  ((uint32_t)(0x20000000))        /* CLKGAT1[29] */
#define CLK_GATE1_M2M                   ((uint32_t)(0x40000000))        /* CLKGAT1[30] */
#define CLK_GATE1_EFUS                  ((uint32_t)(0x80000000))        /* CLKGAT1[31] */

#define CLK_GATE2_IIC                   ((uint32_t)(0x00000001))        /* CLKGAT2[0] */
#define CLK_GATE2_IRFLT                 ((uint32_t)(0x00000002))        /* CLKGAT2[1] */
#define CLK_GATE2_LEDC                  ((uint32_t)(0x00000004))        /* CLKGAT2[2] */
#define CLK_GATE2_SYSX24M               ((uint32_t)(0x00000100))        /* CLKGAT2[8] */
#define CLK_GATE2_X48M                  ((uint32_t)(0x00000200))        /* CLKGAT2[9] */
#define CLK_GATE2_BSP                   ((uint32_t)(0x00000400))        /* CLKGAT2[10] */
#define CLK_GATE2_CLKOUTDIV2            ((uint32_t)(0x00000800))        /* CLKGAT2[11] */
#define CLK_GATE2_CLKOUTDIV4            ((uint32_t)(0x00001000))        /* CLKGAT2[12] */
#define CLK_GATE2_SYSDIV                ((uint32_t)(0x00002000))        /* CLKGAT2[13] */
#define CLK_GATE2_ADDIV                 ((uint32_t)(0x00004000))        /* CLKGAT2[14] */
#define CLK_GATE2_DAC_CLK               ((uint32_t)(0x00008000))        /* CLKGAT2[15] */
#define CLK_GATE2_DAC_CLK25             ((uint32_t)(0x00010000))        /* CLKGAT2[16] */
#define CLK_GATE2_DAC_DIV2SEL           ((uint32_t)(0x00020000))        /* CLKGAT2[17] */
#define CLK_GATE2_DRC_PCLK              ((uint32_t)(0x00040000))        /* CLKGAT2[18] */
#define CLK_GATE2_RNN                   ((uint32_t)(0x00080000))        /* CLKGAT2[19] */
#define CLK_GATE2_RDFT                  ((uint32_t)(0x00100000))        /* CLKGAT2[20] */
#define CLK_GATE2_HWMATH                ((uint32_t)(0x00200000))        /* CLKGAT2[21] */

/**
 * @brief Clock Status Enumeration.
 */
typedef enum {
    CLK_DIS                         = 0,
    CLK_EN                          = 1,
} CLK_STATE;

/**
 * @brief Clock Info Type Enumeration.
 */
typedef enum {
    CLK_VALUE_MODE_IDX              = 0,
    CLK_VALUE_MODE_FREQ             = 1,
} CLK_VALUE_MODE_TYPEDEF;

/**
 * @brief Clock of CLKOUT Enumeration.
 */
typedef enum {
    CLK_CLKOUT_XOSC24M              = 1,
    CLK_CLKOUT_X24M_32K,
    CLK_CLKOUT_OSC32K,
    CLK_CLKOUT_PLLDIV4,
    CLK_CLKOUT_XOSC48M,
    CLK_CLKOUT_DAC_CLKPPP,
    CLK_CLKOUT_RC2M,
    CLK_CLKOUT_RTC_RC2M,
    CLK_CLKOUT_SYS_CLK,
    CLK_CLKOUT_BT24M_CLK,
    CLK_CLKOUT_TKA_OUT,
    CLK_CLKOUT_BTLP_CLK,
} CLK_CLKOUT_TYPEDEF;

/**
 * @brief Clock of CLK32K_RTC Enumeration.
 */
typedef enum {
    CLK_CLK32K_RTC_1BIT             = 0,
    CLK_CLK32K_RTC_RC2M_RTC,
    CLK_CLK32K_RTC_SNF_RC_RTC,
    CLK_CLK32K_RTC_X24MDIV12_RTC,
} CLK_CLK32K_RTC_TYPEDEF;

/**
 * @brief Clock of HSUART_CLK Enumeration.
 */
typedef enum {
    CLK_HSUT0_CLK_XOSC24M_CLK       = 0,
    CLK_HSUT0_CLK_XOSC48M,
    CLK_HSUT0_CLK_PLLDIV2_CLK,
    CLK_HSUT0_CLK_UDET_CLKPP,
} CLK_HSUART_CLK_TYPEDEF;

/**
 * @brief Clock of SARADC_CLK Enumeration.
 */
typedef enum {
    CLK_SARADC_CLK_RC2M_CLK         = 0,
    CLK_SARADC_CLK_X24M_CLKDIV4,
    CLK_SARADC_CLK_X24M_CLKDIV2,
    CLK_SARADC_CLK_X24Md32K_CLK,
} CLK_SARADC_CLK_TYPEDEF;

/**
 * @brief Clock of UDET_CLK Enumeration.
 */
typedef enum {
    CLK_UDET_CLK_RC2M_CLK           = 0,
    CLK_UDET_CLK_RTC_RC2M,
    XOSC24M_CLK,
    XOSC48M,
} CLK_UDET_CLK_TYPEDEF;

/**
 * @brief Clock of TMR_INC Enumeration.
 */
typedef enum {
    CLK_TMR_INC_OSC32K              = 0,
    CLK_TMR_INC_CLKOUOT_PIN,
    CLK_TMR_INC_X24M_DIV_CLK,
    CLK_TMR_INC_RC2M_D0,
} CLK_TMR_INC_TYPEDEF;

/**
 * @brief Clock of UART_INC Enumeration.
 */
typedef enum {
    CLK_UART_INC_X24M_DIV_CLK       = 0,
    CLK_UART_INC_X24M_CLKDIV4,
} CLK_UART_INC_TPYEDEF;

/**
 * @brief Clock of CLK2M_KS(KeyScan) Enumeration.
 */
typedef enum {
    CLK_CLK2M_KS_1BIT               = 0,
    CLK_CLK2M_KS_RC2M_RTC,
    CLK_CLK2M_KS_SNF_RC_RTC,
    CLK_CLK2M_KS_X24MDIV12_RTC,
} CLK_CLK2M_KS_TYPEDEF;

/**
 * @brief Clock of SDADDA_CLK Enumeration.
 */
typedef enum {
    CLK_SDADDA_CLK_1BIT             = 0,
    CLK_SDADDA_CLK_ADDA_CLK24_A,
    CLK_SDADDA_CLK_XOSC24M_CLK,
    CLK_SDADDA_CLK_PB4,
    CLK_SDADDA_CLK_XOSC24M_CLK_2PLL = 5,
} CLK_SDADDA_CLK_TYPEDEF;

/**
 * @brief Clock of LEDC_CLK Enumeration.
 */
typedef enum {
    CLK_LEDC_CLK_XOSC24M            = 0,
    CLK_LEDC_CLK_X24M_CLKDIV2,
} CLK_LEDC_CLK_TYPEDEF;

/**
 * @brief Clock of TOUCH_KEY Enumeration.
 */
typedef enum {
    CLK_TOUCH_KEY_RC2M              = 1,
    CLK_TOUCH_KEY_CLK_XOSC24M       = 3,
} CLK_TOUCH_KEY_TYPEDEF;

/**
 * @brief Clock of TTMR_INC(tick0) Enumeration.
 */
typedef enum {
    CLK_TTMR_INC_OSC_32K             = 0,
    CLK_TTMR_INC_CLKOUT_PIN,
    CLK_TTMR_INC_X24M_DIV_CLK,
    CLK_TTMR_INC_RC2M_D0,
} CLK_TTMR_INC_TYPEDEF;

/**
 * @brief Clock of QDEC Enumeration.
 */
typedef enum {
    CLK_QDEC_RC32K             = 0,
    CLK_QDEC_X24M_32K,
} CLK_QDEC_TYPEDEF;

void clk_gate0_cmd(uint32_t clock_gate, CLK_STATE state);
void clk_gate1_cmd(uint32_t clock_gate, CLK_STATE state);
void clk_gate2_cmd(uint32_t clock_gate, CLK_STATE state);

void clk_clkout_set(CLK_CLKOUT_TYPEDEF clk_sel, uint8_t div, CLK_STATE state);
void clk_clk32k_rtc_set(CLK_CLK32K_RTC_TYPEDEF clk_sel);
void clk_hsut0_clk_set(CLK_HSUART_CLK_TYPEDEF clk_sel, uint8_t div);
void clk_saradc_clk_set(CLK_SARADC_CLK_TYPEDEF clk_sel);
void clk_udet_clk_set(CLK_UDET_CLK_TYPEDEF clk_sel);
void clk_tmr_inc_set(CLK_TMR_INC_TYPEDEF clk_sel);
void clk_uart_inc_set(CLK_UART_INC_TPYEDEF clk_sel);
void clk_clk2m_ks_set(CLK_CLK2M_KS_TYPEDEF clk_sel);
void clk_sdadda_clk_set(CLK_SDADDA_CLK_TYPEDEF clk_sel, uint8_t div);
void clk_ledc_clk_set(CLK_LEDC_CLK_TYPEDEF clk_sel);
void clk_touch_key_clk_set(CLK_TOUCH_KEY_TYPEDEF clk_sel);
void clk_ttmr_inc_set(CLK_TTMR_INC_TYPEDEF clk_sel);

uint32_t clk_clk32k_rtc_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_hsut0_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_saradc_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_udet_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_tmr_inc_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_uart_inc_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_clk2m_ks_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_sdadda_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_ledc_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_touch_key_clk_get(CLK_VALUE_MODE_TYPEDEF mode);
uint32_t clk_ttmr_inc_get(CLK_VALUE_MODE_TYPEDEF mode);
void clk_qdec_set(CLK_QDEC_TYPEDEF clk_sel);

#endif // _DRIVER_CLK_H
