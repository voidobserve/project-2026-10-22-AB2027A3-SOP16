/*
 * @File name    : driver_com.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-10
 * @Description  : AB202X Device Peripheral Access Layer Header File.
 *                 This file contains all the peripheral register's definitions, bits
 *                 definitions and memory mapping for AB202X devices.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_COM_H
#define _DRIVER_COM_H

#include "global.h"
#include "api.h"
#include "driver_clk.h"

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FUNCTIONAL_STATE;
typedef enum {RESET = 0, SET = !RESET} FLAG_STATE, IT_STATE;
typedef enum {FAILED = 0, SUCCESS = !FAILED} STATUS_STATE;

//--------------------------- SFR BASE DECLARATION ---------------------------//
#define SFR0_BASE                           (SFR_BASE + 0x000)
#define SFR1_BASE                           (SFR_BASE + 0x100)
#define SFR2_BASE                           (SFR_BASE + 0x200)
#define SFR3_BASE                           (SFR_BASE + 0x300)
#define SFR4_BASE                           (SFR_BASE + 0x400)
#define SFR5_BASE                           (SFR_BASE + 0x500)
#define SFR6_BASE                           (SFR_BASE + 0x600)
#define SFR7_BASE                           (SFR_BASE + 0x700)
#define SFR8_BASE                           (SFR_BASE + 0x800)
#define SFR9_BASE                           (SFR_BASE + 0x900)
#define SFR10_BASE                          (SFR_BASE + 0xa00)
#define SFR11_BASE                          (SFR_BASE + 0xb00)
#define SFR12_BASE                          (SFR_BASE + 0xc00)
#define SFR13_BASE                          (SFR_BASE + 0xd00)
#define SFR14_BASE                          (SFR_BASE + 0xe00)
#define SFR15_BASE                          (SFR_BASE + 0xf00)

//-------------------------------- SFR Group0 --------------------------------//
#define TICK0_BASE                          (SFR0_BASE + 0x01*4)
#define TICK1_BASE                          (SFR0_BASE + 0x05*4)
#define UART0_BASE                          (SFR0_BASE + 0x10*4)
#define TMR0_BASE                           (SFR0_BASE + 0x14*4)
#define WDT_BASE                            (SFR0_BASE + 0x1a*4)
#define SD0_BASE                            (SFR0_BASE + 0x20*4)
#define SPI0_BASE                           (SFR0_BASE + 0x2a*4)
#define UART1_BASE                          (SFR0_BASE + 0x30*4)
#define TMR1_BASE                           (SFR0_BASE + 0x35*4)
#define TMR2_BASE                           (SFR0_BASE + 0x3a*4)
#define QDECX_BASE                          (SFR13_BASE + 0x00*4)
#define QDECY_BASE                          (SFR13_BASE + 0x01*4)
#define QDECZ_BASE                          (SFR13_BASE + 0x02*4)

//---------------------------------- SFR Group1 -----------------------------------//

//---------------------------------- SFR Group3 -----------------------------------//
#define HSUART_BASE                         (SFR3_BASE + 0x30*4)

//---------------------------------- SFR Group4 -----------------------------------//

//---------------------------------- SFR Group5 -----------------------------------//
#define IIC_BASE                            (SFR5_BASE + 0x07*4)
#define SARADC_BASE                         (SFR5_BASE + 0x20*4)

//---------------------------------- SFR Group6 -----------------------------------//
#define GPIOA_BASE                          (SFR6_BASE + 0x00*4)
#define GPIOB_BASE                          (SFR6_BASE + 0x10*4)
#define FUNCMCON_BASE                       (SFR6_BASE + 0x30*4)

//---------------------------------- SFR Group7 -----------------------------------//
#define GPIOG_BASE                          (SFR7_BASE + 0x00*4)
#define IRTX_BASE                           (SFR7_BASE + 0x20*4)
#define IRRX_BASE                           (SFR7_BASE + 0x2f*4)

//---------------------------------- SFR Group8 -----------------------------------//
#define KEYSCAN_BASE                        (SFR8_BASE + 0x00*4)

//---------------------------------- SFR Group9 -----------------------------------//
#define TMR3_BASE                           (SFR9_BASE + 0x00*4)
#define UDET_BASE                           (SFR9_BASE + 0x18*4)
#define SPI1_BASE                           (SFR9_BASE + 0x20*4)

//---------------------------------- SFR Group10 -----------------------------------//
#define TOUCH_KEY_BASE                      (SFR10_BASE + 0x02*4)

//---------------------------------- SFR Group12 -----------------------------------//
#define TIM_BASE                            (SFR12_BASE + 0x00*4)
#define LEDC_BASE                           (SFR12_BASE + 0x30*4)

//---------------------------------- SFR Group13 -----------------------------------//
#define SDADC_BASE                          (SFR13_BASE + 0x09*4)
#define SDDAC_BASE                          (SFR13_BASE + 0x13*4)
#define LPWM_BASE                           (SFR13_BASE + 0x20*4)

/**
 * @brief PERIPHERAL DECLARATION
 */
#define GPIOA_REG                           ((gpio_typedef *) GPIOA_BASE)
#define GPIOB_REG                           ((gpio_typedef *) GPIOB_BASE)
#define FUNCMAP                             ((func_map_typedef *) FUNCMCON_BASE)
#define TMR0                                ((tmr_typedef *) TMR0_BASE)
#define TMR1                                ((tmr_typedef *) TMR1_BASE)
#define TMR2                                ((tmr_typedef *) TMR2_BASE)
#define TMR3                                ((tmr_typedef *) TMR3_BASE)
#define TIM1                                ((tim_typedef *) TIM_BASE)
#define UART                                ((uart_typedef *) UART0_BASE)
#define UDET                                ((uart_typedef *) UDET_BASE)
#define HSUART                              ((hsuart_typedef *) HSUART_BASE)
#define IIC                                 ((iic_typedef *) IIC_BASE)
#define SPI0                                ((spi_typedef *) SPI0_BASE)
#define SPI1                                ((spi_typedef *) SPI1_BASE)
#define WDT                                 ((wdt_typedef *) WDT_BASE)
#define SARADC                              ((sadc_typedef *) SARADC_BASE)
#define LPWM                                ((lpwm_typedef *) LPWM_BASE)
#define KEYSCAN                             ((keyscan_typedef *) KEYSCAN_BASE)
#define IRTX                                ((irtx_typedef *) IRTX_BASE)
#define IRRX                                ((irrx_typedef *) IRRX_BASE)
#define TOUCH_KEY                           ((touch_key_typedef *) TOUCH_KEY_BASE)
#define SDADC                               ((sdadc_typedef *) SDADC_BASE)
#define SDDAC                               ((sddac_typedef *) SDDAC_BASE)
#define LEDC                                ((ledc_typedef *) LEDC_BASE)
#define TICK                                ((tick_typedef *) TICK0_BASE)
#define QDECX                               ((qdec_typedef *) QDECX_BASE)
#define QDECY                               ((qdec_typedef *) QDECY_BASE)
#define QDECZ                               ((qdec_typedef *) QDECZ_BASE)

/*---------------------------------------------------------------------------------*/
/*                                     TMR                                         */
/*---------------------------------------------------------------------------------*/
/******************** Bit definition for TMR[0.1.2]CON register ********************/
#define TMRxCON_TMREN                       ((uint32_t)0x00000001)

#define TMRxCON_INCSEL                      ((uint32_t)0x0000000c)
#define TMRxCON_INCSEL_0                    ((uint32_t)0x00000004)
#define TMRxCON_INCSEL_1                    ((uint32_t)0x00000008)

#define TMRxCON_INCSRC                      ((uint32_t)0x00000040)
#define TMRxCON_TIE                         ((uint32_t)0x00000080)
#define TMRxCON_TPND                        ((uint32_t)0x00000200)

/*********************** Bit definition for TMR3CON register ***********************/
#define TMR3CON_TMREN                       ((uint32_t)0x00000001)
#define TMR3CON_CPTEN                       ((uint32_t)0x00000002)

#define TMR3CON_INCSEL                      ((uint32_t)0x0000000c)
#define TMR3CON_INCSEL_0                    ((uint32_t)0x00000004)
#define TMR3CON_INCSEL_1                    ((uint32_t)0x00000008)

#define TMR3CON_CPTEDSEL                    ((uint32_t)0x00000030)
#define TMR3CON_CPTEDSEL_0                  ((uint32_t)0x00000010)
#define TMR3CON_CPTEDSEL_1                  ((uint32_t)0x00000020)

#define TMR3CON_INCSRC                      ((uint32_t)0x00000040)
#define TMR3CON_TIE                         ((uint32_t)0x00000080)
#define TMR3CON_CIE                         ((uint32_t)0x00000100)
#define TMR3CON_PWM0EN                      ((uint32_t)0x00000200)
#define TMR3CON_PWM1EN                      ((uint32_t)0x00000400)
#define TMR3CON_PWM2EN                      ((uint32_t)0x00000800)
#define TMR3CON_TIMS                        ((uint32_t)0x00001000)
#define TMR3CON_TIS                         ((uint32_t)0x00002000)
#define TMR3CON_CPT_CLREN                   ((uint32_t)0x00004000)
#define TMR3CON_CPT_FLTEN                   ((uint32_t)0x00008000)
#define TMR3CON_TPND                        ((uint32_t)0x00010000)
#define TMR3CON_CPND                        ((uint32_t)0x00020000)

/******************** Bit definition for TMR[0.1.2]CPND register *******************/
#define TMRxCPND_TPCLR                      ((uint32_t)0x00000200)

/*********************** Bit definition for TMR3CPND register **********************/
#define TMR3CPND_TPCLR                      ((uint32_t)0x00010000)
#define TMR3CPND_CPCLR                      ((uint32_t)0x00020000)

/*---------------------------------------------------------------------------------*/
/*                                     UART                                        */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for UARTxCON register **********************/
#define UARTxCON_UTEN                       ((uint32_t)0x00000001)
#define UARTxCON_BIT9EN                     ((uint32_t)0x00000002)
#define UARTxCON_RXIE                       ((uint32_t)0x00000004)
#define UARTxCON_TXIE                       ((uint32_t)0x00000008)
#define UARTxCON_SB2EN                      ((uint32_t)0x00000010)
#define UARTxCON_CLKSRC                     ((uint32_t)0x00000020)
/* UDETCON Register */
#define UARTxCON_FIXBAUD                    ((uint32_t)0x00000020)
#define UARTxCON_ONELINE                    ((uint32_t)0x00000040)
#define UARTxCON_RXEN                       ((uint32_t)0x00000080)
#define UARTxCON_TXPND                      ((uint32_t)0x00000100)
#define UARTxCON_RXPND                      ((uint32_t)0x00000200)
#define UARTxCON_RX4BUFEN                   ((uint32_t)0x00000400)
/* UDETCON Register */
#define UARTxCON_KEYMAT                     ((uint32_t)0x00000400)

#define UARTxCON_RXBCNT                     ((uint32_t)0x00003800)
#define UARTxCON_RXBCNT_0                   ((uint32_t)0x00000800)
#define UARTxCON_RXBCNT_1                   ((uint32_t)0x00001000)
#define UARTxCON_RXBCNT_2                   ((uint32_t)0x00002000)

#define UARTxCON_RX4BUFPND                  ((uint32_t)0x00004000)
#define UARTxCON_RX4BUFERROR                ((uint32_t)0x00008000)

/*********************** Bit definition for UARTxCPND register *********************/
#define UARTxCPND_ALL                       ((uint32_t)0x00038300)
#define UARTxCPND_CTXPND                    ((uint32_t)0x00000100)
#define UARTxCPND_CRXPND                    ((uint32_t)0x00000200)
/* UDETCPND Register */
#define UARTxCPND_CKEYMAT                   ((uint32_t)0x00000400)
#define UARTxCPND_RX4BUF_ERR_CLR            ((uint32_t)0x00008000)
#define UARTxCPND_CKEYPND                   ((uint32_t)0x00010000)
#define UARTxCPND_CRSTKEYPND                ((uint32_t)0x00020000)
/* UDETCPND Register */
#define UARTxCPND_CBDCFM                    ((uint32_t)0x00040000)

/*---------------------------------------------------------------------------------*/
/*                                    HUART                                        */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for HSUT0CON register **********************/
#define HSUT0CON_URXEN                      ((uint32_t)0x00000001)
#define HSUT0CON_UTXEN                      ((uint32_t)0x00000002)
#define HSUT0CON_RXIE                       ((uint32_t)0x00000004)
#define HSUT0CON_TXIE                       ((uint32_t)0x00000008)
#define HSUT0CON_RXTRSMODE                  ((uint32_t)0x00000010)
#define HSUT0CON_RXBITSEL                   ((uint32_t)0x00000020)
#define HSUT0CON_RXLPBUFEN                  ((uint32_t)0x00000040)
#define HSUT0CON_TXTRSMODE                  ((uint32_t)0x00000080)
#define HSUT0CON_TXBITSEL                   ((uint32_t)0x00000100)
#define HSUT0CON_SPBITSEL                   ((uint32_t)0x00000200)
#define HSUT0CON_HSUTTMREN                  ((uint32_t)0x00000400)
#define HSUT0CON_RXHF_PND                   ((uint32_t)0x00000800)
#define HSUT0CON_RXPND                      ((uint32_t)0x00001000)
#define HSUT0CON_TXPND                      ((uint32_t)0x00002000)
#define HSUT0CON_RXFAIL                     ((uint32_t)0x00004000)
#define HSUT0CON_TMROV                      ((uint32_t)0x00008000)
#define HSUT0CON_RXOVE_PND                  ((uint32_t)0x00010000)
#define HSUT0CON_RXHF_IE                    ((uint32_t)0x00020000)
#define HSUT0CON_ONELINE                    ((uint32_t)0x00040000)

/*********************** Bit definition for HSUT0CPND register *********************/
#define HSUT0CPND_CUTRX                     ((uint32_t)0x00000001)
#define HSUT0CPND_CUTTX                     ((uint32_t)0x00000002)
#define HSUT0CPND_CCTSPND                   ((uint32_t)0x00000400)
#define HSUT0CPND_CRXHFPND                  ((uint32_t)0x00000800)
#define HSUT0CPND_CRXPND                    ((uint32_t)0x00001000)
#define HSUT0CPND_CTXPND                    ((uint32_t)0x00002000)
#define HSUT0CPND_CRXFAIL                   ((uint32_t)0x00004000)
#define HSUT0CPND_CTMROV                    ((uint32_t)0x00008000)
#define HSUT0CPND_CRXOVEPND                 ((uint32_t)0x00010000)
#define HSUT0CPND_CRXLBBUF                  ((uint32_t)0x00020000)

/*********************** Bit definition for HSUT0FCCON register ********************/
#define HSUT0CCON_UCTS_EN                   ((uint32_t)0x00000001)
#define HSUT0CCON_URTS_EN                   ((uint32_t)0x00000002)
#define HSUT0CCON_CTSIE                     ((uint32_t)0x00000004)
#define HSUT0CCON_RTS_MODE                  ((uint32_t)0x00000008)
#define HSUT0CCON_CTSPND                    ((uint32_t)0x00000010)
#define HSUT0CCON_CTSS                      ((uint32_t)0x00000020)
#define HSUT0CCON_RTSS                      ((uint32_t)0x00000040)
#define HSUT0CCON_RTS_SW                    ((uint32_t)0x00000080)

/*---------------------------------------------------------------------------------*/
/*                                     IIC                                         */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for IICCON0 register **********************/
#define IICCON0_DONE                        ((uint32_t)0x80000000)
#define IICCON0_ACKSTATUS                   ((uint32_t)0x40000000)
#define IICCON0_CLR_DONE                    ((uint32_t)0x20000000)
#define IICCON0_KS                          ((uint32_t)0x10000000)
#define IICCON0_CLR_ALL                     ((uint32_t)0x08000000)
#define IICCON0_RX_NACK_EN                  ((uint32_t)0x00004000)
#define IICCON0_SMP_SEL                     ((uint32_t)0x00002000)
#define IICCON0_MODE                        ((uint32_t)0x00001000)

#define IICCON0_POSDIV                      ((uint32_t)0x000003f0)

#define IICCON0_HOLDCNT                     ((uint32_t)0x0000000c)

#define IICCON0_INTEN                       ((uint32_t)0x00000002)
#define IICCON0_IICEN                       ((uint32_t)0x00000001)

/************************ Bit definition for IICCON1 register **********************/
#define IICCON1_DATA_CNT                    ((uint32_t)0x00000007)

#define IICCON1_START0_EN                   ((uint32_t)0x00000008)
#define IICCON1_CTL0_EN                     ((uint32_t)0x00000010)
#define IICCON1_ADR0_EN                     ((uint32_t)0x00000020)
#define IICCON1_ADR1_EN                     ((uint32_t)0x00000040)
#define IICCON1_START1_EN                   ((uint32_t)0x00000080)
#define IICCON1_CTL1_EN                     ((uint32_t)0x00000100)
#define IICCON1_RDAT_EN                     ((uint32_t)0x00000200)
#define IICCON1_WDAT_EN                     ((uint32_t)0x00000400)
#define IICCON1_STOP_EN                     ((uint32_t)0x00000800)
#define IICCON1_TXNAK_EN                    ((uint32_t)0x00001000)

/************************ Bit definition for IICSSTS register **********************/
#define IICSSTS_RXVLD                       ((uint32_t)0x00000001)
#define IICSSTS_TXVLD                       ((uint32_t)0x00000002)
#define IICSSTS_START                       ((uint32_t)0x00000004)
#define IICSSTS_RESTART                     ((uint32_t)0x00000008)
#define IICSSTS_STOP                        ((uint32_t)0x00000010)
#define IICSSTS_SRX                         ((uint32_t)0x00000020)
#define IICSSTS_STX                         ((uint32_t)0x00000040)

#define IICSSTS_DATA_CNT                    ((uint32_t)0x00000700)

#define IICSSTS_RXVLD_SET                   ((uint32_t)0x00010000)
#define IICSSTS_TXVLD_SET                   ((uint32_t)0x00020000)
#define IICSSTS_START_CLR                   ((uint32_t)0x00040000)
#define IICSSTS_RESTART_CLR                 ((uint32_t)0x00080000)
#define IICSSTS_STOP_CLR                    ((uint32_t)0x00100000)

/************************ Bit definition for SPICON register **********************/
#define SPIxCON_IE                          ((uint32_t)0x00000080)

/*---------------------------------------------------------------------------------*/
/*                                     RTC                                         */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for RTCCON register ***********************/
#define RTCCON_VRTCVSEL                     ((uint32_t)0x00000001)

#define RTCCON_BAUDSEL                      ((uint32_t)0x00000006)

#define RTCCON_RTC1SIE                      ((uint32_t)0x00000008)
#define RTCCON_ALMIE                        ((uint32_t)0x00000010)
#define RTCCON_WKUPRSTEN                    ((uint32_t)0x00000020)
#define RTCCON_VUSBRSTEN                    ((uint32_t)0x00000040)
#define RTCCON_RTC_WKSLPEN                  ((uint32_t)0x00000080)
#define RTCCON_ALM_WKEN                     ((uint32_t)0x00000100)
#define RTCCON_VUSBON_WKSLPEN               ((uint32_t)0x00000200)
#define RTCCON_INBOX_WKSLPEN                ((uint32_t)0x00000400)
#define RTCCON_VUSBONIE                     ((uint32_t)0x00000800)
#define RTCCON_INBOXIE                      ((uint32_t)0x00001000)
#define RTCCON_RTC_ON                       ((uint32_t)0x00010000)
#define RTCCON_ALMPND                       ((uint32_t)0x00020000)
#define RTCCON_RTCWKSLPPND                  ((uint32_t)0x00040000)
#define RTCCON_RTCWKP                       ((uint32_t)0x00080000)
#define RTCCON_VUSBONLINE                   ((uint32_t)0x00100000)
#define RTCCON_VUSBOFF                      ((uint32_t)0x00200000)
#define RTCCON_INBOX                        ((uint32_t)0x00400000)

/************************ Bit definition for RTCCPND register **********************/
#define RTCCPND_TKTSOE                      ((uint32_t)0x00000003)

#define RTCCPND_CALMPND                     ((uint32_t)0x00020000)
#define RTCCPND_CWKSLPPND                   ((uint32_t)0x00040000)

#define RTCCPND_VBCNTDIS                    ((uint32_t)0x30000000)

#define RTCCPND_VBCNTKST                    ((uint32_t)0x40000000)
#define RTCCPND_VBCNTCLR                    ((uint32_t)0x80000000)

/************************ Bit definition for RTCCON11 register *********************/
#define RTCCON11_RTCWKSLPEN                 ((uint32_t)0x00000400)

/*---------------------------------------------------------------------------------*/
/*                                     SPI                                         */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for SPIxCON register **********************/
#define SPIxCON_SPIEN                       ((uint32_t)0x00000001)
#define SPIxCON_SPISM                       ((uint32_t)0x00000002)

#define SPIxCON_BUSMODE                     ((uint32_t)0x0000000c)
#define SPIxCON_BUSMODE_0                   ((uint32_t)0x00000004)
#define SPIxCON_BUSMODE_1                   ((uint32_t)0x00000008)

#define SPIxCON_SPISEL                      ((uint32_t)0x00000010)
#define SPIxCON_CLKIDS                      ((uint32_t)0x00000020)
#define SPIxCON_SMPS                        ((uint32_t)0x00000040)
#define SPIxCON_SPIIE                       ((uint32_t)0x00000080)
#define SPIxCON_SPILF_EN                    ((uint32_t)0x00000100)
#define SPIxCON_SPIMBEN                     ((uint32_t)0x00000200)
#define SPIxCON_SPIOSS                      ((uint32_t)0x00000400)
#define SPIxCON_HOLDENRX                    ((uint32_t)0x00000800)
#define SPIxCON_HOLDENTX                    ((uint32_t)0x00001000)
#define SPIxCON_HOLDENSW                    ((uint32_t)0x00002000)
#define SPIxCON_SPIPND                      ((uint32_t)0x00010000)

/*---------------------------------------------------------------------------------*/
/*                                     WDT                                         */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for WDTCON register ***********************/
#define WDTCON_WDTCLR                       ((uint32_t)0x0000000f)
#define WDTCON_WDTCLR_0                     ((uint32_t)0x00000001)
#define WDTCON_WDTCLR_1                     ((uint32_t)0x00000002)
#define WDTCON_WDTCLR_2                     ((uint32_t)0x00000004)
#define WDTCON_WDTCLR_3                     ((uint32_t)0x00000008)

#define WDTCON_WDTEN_WR                     ((uint32_t)0x000000f0)
#define WDTCON_WDTEN_WR_0                   ((uint32_t)0x00000010)
#define WDTCON_WDTEN_WR_1                   ((uint32_t)0x00000020)
#define WDTCON_WDTEN_WR_2                   ((uint32_t)0x00000040)
#define WDTCON_WDTEN_WR_3                   ((uint32_t)0x00000080)

#define WDTCON_RSTEN_WR                     ((uint32_t)0x00000f00)
#define WDTCON_RSTEN_WR_0                   ((uint32_t)0x00000100)
#define WDTCON_RSTEN_WR_1                   ((uint32_t)0x00000200)
#define WDTCON_RSTEN_WR_2                   ((uint32_t)0x00000400)
#define WDTCON_RSTEN_WR_3                   ((uint32_t)0x00000800)

#define WDTCON_WDTIE_WR                     ((uint32_t)0x0000f000)
#define WDTCON_WDTIE_WR_0                   ((uint32_t)0x00001000)
#define WDTCON_WDTIE_WR_1                   ((uint32_t)0x00002000)
#define WDTCON_WDTIE_WR_2                   ((uint32_t)0x00004000)
#define WDTCON_WDTIE_WR_3                   ((uint32_t)0x00008000)

#define WDTCON_WDTCSEL_WR                   ((uint32_t)0x000f0000)
#define WDTCON_WDTCSEL_WR_0                 ((uint32_t)0x00010000)
#define WDTCON_WDTCSEL_WR_1                 ((uint32_t)0x00020000)
#define WDTCON_WDTCSEL_WR_2                 ((uint32_t)0x00040000)
#define WDTCON_WDTCSEL_WR_3                 ((uint32_t)0x00080000)

#define WDTCON_TMRSEL                       ((uint32_t)0x00700000)
#define WDTCON_TMRSEL_0                     ((uint32_t)0x00100000)
#define WDTCON_TMRSEL_1                     ((uint32_t)0x00200000)
#define WDTCON_TMRSEL_2                     ((uint32_t)0x00400000)

#define WDTCON_TMRSEL_WR                    ((uint32_t)0x0f000000)
#define WDTCON_TMRSEL_WR_0                  ((uint32_t)0x01000000)
#define WDTCON_TMRSEL_WR_1                  ((uint32_t)0x02000000)
#define WDTCON_TMRSEL_WR_2                  ((uint32_t)0x04000000)
#define WDTCON_TMRSEL_WR_3                  ((uint32_t)0x08000000)

#define WDTCON_WDTPND                       ((uint32_t)0x80000000)

/*---------------------------------------------------------------------------------*/
/*                                    SARADC                                       */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for SADCCON register **********************/
#define SADCCON_CH0PUEN                     ((uint32_t)0x00000001)
#define SADCCON_CH1PUEN                     ((uint32_t)0x00000002)
#define SADCCON_CH2PUEN                     ((uint32_t)0x00000004)
#define SADCCON_CH3PUEN                     ((uint32_t)0x00000008)
#define SADCCON_CH4PUEN                     ((uint32_t)0x00000010)
#define SADCCON_CH5PUEN                     ((uint32_t)0x00000020)
#define SADCCON_CH6PUEN                     ((uint32_t)0x00000040)
#define SADCCON_CH7PUEN                     ((uint32_t)0x00000080)
#define SADCCON_CH8PUEN                     ((uint32_t)0x00000100)
#define SADCCON_CH9PUEN                     ((uint32_t)0x00000200)
#define SADCCON_CH10PUEN                    ((uint32_t)0x00000400)
#define SADCCON_CH11PUEN                    ((uint32_t)0x00000800)
#define SADCCON_CH12PUEN                    ((uint32_t)0x00001000)
#define SADCCON_CH13PUEN                    ((uint32_t)0x00002000)
#define SADCCON_CH14PUEN                    ((uint32_t)0x00004000)
#define SADCCON_CH15PUEN                    ((uint32_t)0x00008000)
#define SADCCON_ADCEN                       ((uint32_t)0x00010000)
#define SADCCON_ADCIE                       ((uint32_t)0x00020000)
#define SADCCON_ADCANGIOEN                  ((uint32_t)0x00040000)
#define SADCCON_ADCAEN                      ((uint32_t)0x00080000)
#define SADCCON_ADCMODE                     ((uint32_t)0x00100000)

/*---------------------------------------------------------------------------------*/
/*                                    LPWM                                         */
/*---------------------------------------------------------------------------------*/
/********************** Bit definition for LPWMSTCON register **********************/
#define LPWMSTCON_LPWM_DIR                  ((uint32_t)0x00000800)

#define LPWMSTCON_ST_DUTY                   ((uint32_t)0x000007fc)
#define LPWMSTCON_ST_DUTY_0                 ((uint32_t)0x00000004)

#define LPWMSTCON_BREATH_MODE               ((uint32_t)0x00000002)
#define LPWMSTCON_LPWM_EN                   ((uint32_t)0x00000001)

/********************** Bit definition for LPWMSWCON register **********************/
#define LPWMSWCON_DUTY_ON                   ((uint32_t)0x000000ff)
#define LPWMSWCON_DUTY_ON_0                 ((uint32_t)0x00000001)

/********************** Bit definition for LPWMCMCON register **********************/
#define LPWMCMCON_SWT_CNTMAX                ((uint32_t)0x00ff0000)
#define LPWMCMCON_SWT_CNTMAX_0              ((uint32_t)0x00010000)

#define LPWMCMCON_BRT_CNTMAX                ((uint32_t)0x000001ff)
#define LPWMCMCON_BRT_CNTMAX_0              ((uint32_t)0x00000001)

/********************** Bit definition for LPWMxCON register ***********************/
#define LPWMxCON_EN                         ((uint32_t)0x02000000)

#define LPWMxCON_REPEAT                     ((uint32_t)0x01fe0000)
#define LPWMxCON_REPEAT_0                   ((uint32_t)0x00020000)

#define LPWMxCON_STEP                       ((uint32_t)0x0001fe00)
#define LPWMxCON_STEP_0                     ((uint32_t)0x00000200)

#define LPWMxCON_ST_DUTY                    ((uint32_t)0x000001ff)
#define LPWMxCON_ST_DUTY_0                  ((uint32_t)0x00000001)

/********************** Bit definition for LPWMHLCON register **********************/
#define LPWMHLCON_H_EN                      ((uint32_t)0x00200000)
#define LPWMHLCON_L_EN                      ((uint32_t)0x00100000)

#define LPWMHLCON_H_REPEAT                  ((uint32_t)0x000ffc00)
#define LPWMHLCON_H_REPEAT_0                ((uint32_t)0x00000400)

#define LPWMHLCON_L_REPEAT                  ((uint32_t)0x000003ff)
#define LPWMHLCON_L_REPEAT_0                ((uint32_t)0x00000001)

/*********************** Bit definition for LPWMPND register ***********************/
#define LPWMPND_LPWM_PND                    ((uint32_t)0x00000001)

/*---------------------------------------------------------------------------------*/
/*                                   KEYSCAN                                       */
/*---------------------------------------------------------------------------------*/
/********************* Bit definition for KEYSCAN_CON0 register ********************/
#define KEYSCAN_CON0_KEYVALID_PND           ((uint32_t)0x80000000)
#define KEYSCAN_CON0_KEYINVALID_PND         ((uint32_t)0x40000000)

#define KEYSCAN_CON0_KEYPORT_EN             ((uint32_t)0x0000ff00)
#define KEYSCAN_CON0_KEYPORT_EN_0           ((uint32_t)0x00000100)

#define KEYSCAN_CON0_KEYSCAN_SWRSTN         ((uint32_t)0x00000080)
#define KEYSCAN_CON0_KEYINVALID_INT_EN      ((uint32_t)0x00000020)
#define KEYSCAN_CON0_KEYVALID_INT_EN        ((uint32_t)0x00000010)
#define KEYSCAN_CON0_KEYVALID_WKUP_EN       ((uint32_t)0x00000008)
#define KEYSCAN_CON0_KEYSCAN_LP             ((uint32_t)0x00000004)
#define KEYSCAN_CON0_KEYSCAN_MODE           ((uint32_t)0x00000002)
#define KEYSCAN_CON0_KEYSCAN_EN             ((uint32_t)0x00000001)

/********************* Bit definition for KEYSCAN_CON1 register ********************/
#define KEYSCAN_CON1_KEYPORT_STAB_TIME      ((uint32_t)0x00ff0000)
#define KEYSCAN_CON1_KEYPORT_STAB_TIME_0    ((uint32_t)0x00010000)

#define KEYSCAN_CON1_KEY_DEBO_TIMES         ((uint32_t)0x0000ff00)
#define KEYSCAN_CON1_KEY_DEBO_TIMES_0       ((uint32_t)0x00000100)

#define KEYSCAN_CON1_KEY_WAIT_TIMES         ((uint32_t)0x000000ff)
#define KEYSCAN_CON1_KEY_WAIT_TIMES_0       ((uint32_t)0x00000001)

/********************* Bit definition for KEYSCAN_CLR register *********************/
#define KEYSCAN_CLR_KEYVALID_PND_CLR        ((uint32_t)0x80000000)
#define KEYSCAN_CLR_KEYINVALID_PND_CLR      ((uint32_t)0x40000000)

/********************* Bit definition for KEYSCAN_INFO0 register *******************/
#define KEYSCAN_INFO0_KD7_KD0               ((uint32_t)0xff000000)
#define KEYSCAN_INFO0_KD7_KD0_0             ((uint32_t)0x01000000)

#define KEYSCAN_INFO0_KC7_KC0               ((uint32_t)0x00ff0000)
#define KEYSCAN_INFO0_KC7_KC0_0             ((uint32_t)0x00010000)

#define KEYSCAN_INFO0_KB7_KB0               ((uint32_t)0x0000ff00)
#define KEYSCAN_INFO0_KB7_KB0_0             ((uint32_t)0x00000100)

#define KEYSCAN_INFO0_KA7_KA0               ((uint32_t)0x000000ff)
#define KEYSCAN_INFO0_KA7_KA0_0             ((uint32_t)0x00000001)

/********************* Bit definition for KEYSCAN_INFO1 register *******************/
#define KEYSCAN_INFO1_KH7_KH0               ((uint32_t)0xff000000)
#define KEYSCAN_INFO1_KH7_KH0_0             ((uint32_t)0x01000000)

#define KEYSCAN_INFO1_KG7_KG0               ((uint32_t)0x00ff0000)
#define KEYSCAN_INFO1_KG7_KG0_0             ((uint32_t)0x00010000)

#define KEYSCAN_INFO1_KF7_KF0               ((uint32_t)0x0000ff00)
#define KEYSCAN_INFO1_KF7_KF0_0             ((uint32_t)0x00000100)

#define KEYSCAN_INFO1_KE7_KE0               ((uint32_t)0x000000ff)
#define KEYSCAN_INFO1_KE7_KE0_0             ((uint32_t)0x00000001)

/********************* Bit definition for KEYSCAN_INFO2 register *******************/
#define KEYSCAN_INFO2_KI7_KI0               ((uint32_t)0x0000ff00)
#define KEYSCAN_INFO2_KI7_KI0_0             ((uint32_t)0x00000100)

#define KEYSCAN_INFO2_KJ7_KJ0               ((uint32_t)0x000000ff)
#define KEYSCAN_INFO2_KJ7_KJ0_0             ((uint32_t)0x00000001)


/*---------------------------------------------------------------------------------*/
/*                                    SDADC                                        */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for SDADC_CON register *********************/
#define SDADC_CON_GET_DC_FLAG               ((uint32_t)0x80000000)
#define SDADC_CON_PHT_SP_SEL                ((uint32_t)0x00008000)
#define SDADC_CON_ADC_SP_SEL                ((uint32_t)0x00004000)
#define SDADC_CON_RMDC_FILTER_SEL           ((uint32_t)0x00001000)

#define SDADC_CON_GETDC_SEL                 ((uint32_t)0x00000c00)
#define SDADC_CON_GETDC_SEL_0               ((uint32_t)0x00000400)
#define SDADC_CON_GETDC_SEL_1               ((uint32_t)0x00000800)

#define SDADC_CON_RMDC_SEL                  ((uint32_t)0x00000300)
#define SDADC_CON_RMDC_SEL_0                ((uint32_t)0x00000100)
#define SDADC_CON_RMDC_SEL_1                ((uint32_t)0x00000200)

#define SDADC_CON_SW_DC_EN                  ((uint32_t)0x00000080)
#define SDADC_CON_GETDC_EN                  ((uint32_t)0x00000040)
#define SDADC_CON_RMDC_EN                   ((uint32_t)0x00000020)
#define SDADC_CON_GAIN_EN                   ((uint32_t)0x00000010)

#define SDADC_CON_FSOUT_SEL                 ((uint32_t)0x0000000e)
#define SDADC_CON_FSOUT_SEL_0               ((uint32_t)0x00000002)
#define SDADC_CON_FSOUT_SEL_1               ((uint32_t)0x00000004)
#define SDADC_CON_FSOUT_SEL_2               ((uint32_t)0x00000008)

#define SDADC_CON_ADC_EN                    ((uint32_t)0x00000001)

/********************* Bit definition for SDADC_GETDCCON register ******************/
#define SDADC_GETDCCON_GETDC_SIZE           ((uint32_t)0xffff0000)
#define SDADC_GETDCCON_SW_DC                ((uint32_t)0x0000ffff)

/********************* Bit definition for SDADC_GAINCON register *******************/
#define SDADC_GAINCON_GAIN                  ((uint32_t)0x0000ffff)

/********************* Bit definition for SDADC_DMACON register ********************/
#define SDADC_DMACON_SAMPLES_IE             ((uint32_t)0x00000400)
#define SDADC_DMACON_DMA_ALLDONE_IE         ((uint32_t)0x00000200)
#define SDADC_DMACON_DMA_HALFDONE_IE        ((uint32_t)0x00000100)
#define SDADC_DMACON_DMA_EN                 ((uint32_t)0x00000001)

/********************* Bit definition for SDADC_DMAPND register ********************/
#define SDADC_DMAPND_SAMPLEDONE            ((uint32_t)0x00000004)
#define SDADC_DMAPND_ALLDONE               ((uint32_t)0x00000002)
#define SDADC_DMAPND_HALFDONE              ((uint32_t)0x00000001)

/********************* Bit definition for SDADC_DMACPND register *******************/
#define SDADC_DMACPND_SAMPLEDONE_CLR        ((uint32_t)0x00000004)
#define SDADC_DMACPND_ALLDONE_CLR           ((uint32_t)0x00000002)
#define SDADC_DMACPND_HALFDONE_CLR          ((uint32_t)0x00000001)


/*---------------------------------------------------------------------------------*/
/*                                    SDDAC                                        */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for SDDACCON register **********************/
#define SDDAC_DAC_EN                        ((uint32_t)0x00000001)
#define SDDAC_FSIN_SEL                      ((uint32_t)0x0000000e)
#define SDDAC_FSIN_SEL_0                    ((uint32_t)0x00000002)
#define SDDAC_OSR_MODE                      ((uint32_t)0x00000010)

/********************* Bit definition for DACDMAINCON register ********************/
#define SDDAC_DMA_EN                        ((uint32_t)0x00000001)
#define SDDAC_DMA_MODE                      ((uint32_t)0x00000002)
#define SDDAC_DMA_HPEND_IE                  ((uint32_t)0x00000004)
#define SDDAC_DMA_APEND_IE                  ((uint32_t)0x00000008)

/********************* Bit definition for DACDMAINPEND register ********************/
#define SDDAC_DMA_HALFPEND                  ((uint32_t)0x00000001)
#define SDDAC_DMA_ALLPEND                   ((uint32_t)0x00000002)

/********************* Bit definition for DACDMAINCPND register ********************/
#define SDDAC_DMA_HALFPEND_CLR              ((uint32_t)0x00000001)
#define SDDAC_DMA_ALLPEND_CLR               ((uint32_t)0x00000002)

/*---------------------------------------------------------------------------------*/
/*                                    LEDC                                         */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for LEDCCON register ***********************/
#define LEDC_CON_DMA_INT_EN                 ((uint32_t)0x04000000)
#define LEDC_CON_OUT_INV                    ((uint32_t)0x02000000)
#define LEDC_CON_RST_INT_EN                 ((uint32_t)0x01000000)

#define LEDC_CON_BAUD                       ((uint32_t)0x00ff0000)

#define LEDC_CON_DELAY                      ((uint32_t)0x0000ff00)

#define LEDC_CON_IN_FORMAT                  ((uint32_t)0x00000080)
#define LEDC_CON_INT_EN                     ((uint32_t)0x00000040)
#define LEDC_CON_IDLE_LEVEL                 ((uint32_t)0x00000020)
#define LEDC_CON_VLD_INV                    ((uint32_t)0x00000010)

#define LEDC_CON_BYTE_INV                   ((uint32_t)0x0000000c)
#define LEDC_CON_BYTE_INV_0                 ((uint32_t)0x00000004)
#define LEDC_CON_BYTE_INV_1                 ((uint32_t)0x00000008)

#define LEDC_CON_LEDC_KICK                  ((uint32_t)0x00000002)
#define LEDC_CON_EN                         ((uint32_t)0x00000001)

/*********************** Bit definition for LEDCPEND register **********************/
#define LEDC_PEND_DMA_PND                   ((uint32_t)0x00000004)
#define LEDC_PEND_RST_PND                   ((uint32_t)0x00000002)
#define LEDC_PEND_LEDC_PND                  ((uint32_t)0x00000001)

/*---------------------------------------------------------------------------------*/
/*                                     QDEC                                         */
/*---------------------------------------------------------------------------------*/
/************************ Bit definition for QDECxCON register **********************/
#define QDECCON_REVERSE_PND                 ((uint32_t)0x80000000)
#define QDECCON_FORWARD_PND                 ((uint32_t)0x40000000)
#define QDECCON_DAT_CLR                     ((uint32_t)0x00400000)
#define QDECCON_OV_PND                      ((uint32_t)0x00200000)
#define QDECCON_SMP_PND                     ((uint32_t)0x00100000)
#define QDECxCON_SMP                        ((uint32_t)0x000F0000)
#define QDECxCON_TIME_MODE                  ((uint32_t)0x00008000)
#define QDECCON_TERR_PND                    ((uint32_t)0x00004000)
#define QDECxCON_CM                         ((uint32_t)0x00003000)
#define QDECxCON_FIL_LEN                    ((uint32_t)0x00000FE0)
#define QDECxCON_WKUP_EN                    ((uint32_t)0x00000010)
#define QDECCON_SMPIE                       ((uint32_t)0x00000008)
#define QDECCON_RDIE                        ((uint32_t)0x00000004)
#define QDECCON_FDIE                        ((uint32_t)0x00000002)
#define QDECCON_EN                          ((uint32_t)0x00000001)
/*---------------------------------------------------------------------------------*/
/*                                    TICK                                         */
/*---------------------------------------------------------------------------------*/
/*********************** Bit definition for TICK0CON register **********************/
#define TICK_CON_TPND                       ((uint32_t)0x00000200)
#define TICK_CON_TIE                        ((uint32_t)0x00000080)
#define TICK_CON_INCSRC                     ((uint32_t)0x00000040)

#define TICK_CON_INCSEL                     ((uint32_t)0x0000000c)
#define TICK_CON_INCSEL_0                   ((uint32_t)0x00000004)
#define TICK_CON_INCSEL_1                   ((uint32_t)0x00000008)

#define TICK_CON_EN                         ((uint32_t)0x00000001)

/*********************** Bit definition for TICK0CPND register *********************/
#define TICK_CPND_TPCLR                     ((uint32_t)0x00000200)


/*---------------------------------------------------------------------------------*/
/*                                    TIM                                          */
/*---------------------------------------------------------------------------------*/
/*******************  Bit definition for TIM_CR1 register  ********************/
#define  TIM_CR1_CEN                         ((uint16_t)0x0001)            /*!< Counter enable */
#define  TIM_CR1_UDIS                        ((uint16_t)0x0002)            /*!< Update disable */
#define  TIM_CR1_URS                         ((uint16_t)0x0004)            /*!< Update request source */
#define  TIM_CR1_OPM                         ((uint16_t)0x0008)            /*!< One pulse mode */
#define  TIM_CR1_DIR                         ((uint16_t)0x0010)            /*!< Direction */

#define  TIM_CR1_CMS                         ((uint16_t)0x0060)            /*!< CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CR1_CMS_0                       ((uint16_t)0x0020)            /*!< Bit 0 */
#define  TIM_CR1_CMS_1                       ((uint16_t)0x0040)            /*!< Bit 1 */

#define  TIM_CR1_ARPE                        ((uint16_t)0x0080)            /*!< Auto-reload preload enable */

#define  TIM_CR1_CKD                         ((uint16_t)0x0300)            /*!< CKD[1:0] bits (clock division) */
#define  TIM_CR1_CKD_0                       ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_CR1_CKD_1                       ((uint16_t)0x0200)            /*!< Bit 1 */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define  TIM_CR2_CCPC                        ((uint16_t)0x0001)            /*!< Capture/Compare Preloaded Control */
#define  TIM_CR2_CCUS                        ((uint16_t)0x0004)            /*!< Capture/Compare Control Update Selection */
#define  TIM_CR2_CCDS                        ((uint16_t)0x0008)            /*!< Capture/Compare DMA Selection */

#define  TIM_CR2_MMS                         ((uint16_t)0x0070)            /*!< MMS[2:0] bits (Master Mode Selection) */
#define  TIM_CR2_MMS_0                       ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_CR2_MMS_1                       ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_CR2_MMS_2                       ((uint16_t)0x0040)            /*!< Bit 2 */

#define  TIM_CR2_TI1S                        ((uint16_t)0x0080)            /*!< TI1 Selection */
#define  TIM_CR2_OIS1                        ((uint16_t)0x0100)            /*!< Output Idle state 1 (OC1 output) */
#define  TIM_CR2_OIS1N                       ((uint16_t)0x0200)            /*!< Output Idle state 1 (OC1N output) */
#define  TIM_CR2_OIS2                        ((uint16_t)0x0400)            /*!< Output Idle state 2 (OC2 output) */
#define  TIM_CR2_OIS2N                       ((uint16_t)0x0800)            /*!< Output Idle state 2 (OC2N output) */
#define  TIM_CR2_OIS3                        ((uint16_t)0x1000)            /*!< Output Idle state 3 (OC3 output) */
#define  TIM_CR2_OIS3N                       ((uint16_t)0x2000)            /*!< Output Idle state 3 (OC3N output) */
#define  TIM_CR2_OIS4                        ((uint16_t)0x4000)            /*!< Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define  TIM_SMCR_SMS                        ((uint16_t)0x0007)            /*!< SMS[2:0] bits (Slave mode selection) */
#define  TIM_SMCR_SMS_0                      ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_SMCR_SMS_1                      ((uint16_t)0x0002)            /*!< Bit 1 */
#define  TIM_SMCR_SMS_2                      ((uint16_t)0x0004)            /*!< Bit 2 */

#define  TIM_SMCR_TS                         ((uint16_t)0x0070)            /*!< TS[2:0] bits (Trigger selection) */
#define  TIM_SMCR_TS_0                       ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_SMCR_TS_1                       ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_SMCR_TS_2                       ((uint16_t)0x0040)            /*!< Bit 2 */

#define  TIM_SMCR_MSM                        ((uint16_t)0x0080)            /*!< Master/slave mode */

#define  TIM_SMCR_ETF                        ((uint16_t)0x0F00)            /*!< ETF[3:0] bits (External trigger filter) */
#define  TIM_SMCR_ETF_0                      ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_SMCR_ETF_1                      ((uint16_t)0x0200)            /*!< Bit 1 */
#define  TIM_SMCR_ETF_2                      ((uint16_t)0x0400)            /*!< Bit 2 */
#define  TIM_SMCR_ETF_3                      ((uint16_t)0x0800)            /*!< Bit 3 */

#define  TIM_SMCR_ETPS                       ((uint16_t)0x3000)            /*!< ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_SMCR_ETPS_0                     ((uint16_t)0x1000)            /*!< Bit 0 */
#define  TIM_SMCR_ETPS_1                     ((uint16_t)0x2000)            /*!< Bit 1 */

#define  TIM_SMCR_ECE                        ((uint16_t)0x4000)            /*!< External clock enable */
#define  TIM_SMCR_ETP                        ((uint16_t)0x8000)            /*!< External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define  TIM_DIER_UIE                        ((uint16_t)0x0001)            /*!< Update interrupt enable */
#define  TIM_DIER_CC1IE                      ((uint16_t)0x0002)            /*!< Capture/Compare 1 interrupt enable */
#define  TIM_DIER_CC2IE                      ((uint16_t)0x0004)            /*!< Capture/Compare 2 interrupt enable */
#define  TIM_DIER_CC3IE                      ((uint16_t)0x0008)            /*!< Capture/Compare 3 interrupt enable */
#define  TIM_DIER_CC4IE                      ((uint16_t)0x0010)            /*!< Capture/Compare 4 interrupt enable */
#define  TIM_DIER_COMIE                      ((uint16_t)0x0020)            /*!< COM interrupt enable */
#define  TIM_DIER_TIE                        ((uint16_t)0x0040)            /*!< Trigger interrupt enable */
#define  TIM_DIER_BIE                        ((uint16_t)0x0080)            /*!< Break interrupt enable */
#define  TIM_DIER_UDE                        ((uint16_t)0x0100)            /*!< Update DMA request enable */
#define  TIM_DIER_CC1DE                      ((uint16_t)0x0200)            /*!< Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE                      ((uint16_t)0x0400)            /*!< Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE                      ((uint16_t)0x0800)            /*!< Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE                      ((uint16_t)0x1000)            /*!< Capture/Compare 4 DMA request enable */
#define  TIM_DIER_COMDE                      ((uint16_t)0x2000)            /*!< COM DMA request enable */
#define  TIM_DIER_TDE                        ((uint16_t)0x4000)            /*!< Trigger DMA request enable */

/********************  Bit definition for TIM_SR register  ********************/
#define  TIM_SR_UIF                          ((uint16_t)0x0001)            /*!< Update interrupt Flag */
#define  TIM_SR_CC1IF                        ((uint16_t)0x0002)            /*!< Capture/Compare 1 interrupt Flag */
#define  TIM_SR_CC2IF                        ((uint16_t)0x0004)            /*!< Capture/Compare 2 interrupt Flag */
#define  TIM_SR_CC3IF                        ((uint16_t)0x0008)            /*!< Capture/Compare 3 interrupt Flag */
#define  TIM_SR_CC4IF                        ((uint16_t)0x0010)            /*!< Capture/Compare 4 interrupt Flag */
#define  TIM_SR_COMIF                        ((uint16_t)0x0020)            /*!< COM interrupt Flag */
#define  TIM_SR_TIF                          ((uint16_t)0x0040)            /*!< Trigger interrupt Flag */
#define  TIM_SR_BIF                          ((uint16_t)0x0080)            /*!< Break interrupt Flag */
#define  TIM_SR_CC1OF                        ((uint16_t)0x0200)            /*!< Capture/Compare 1 Overcapture Flag */
#define  TIM_SR_CC2OF                        ((uint16_t)0x0400)            /*!< Capture/Compare 2 Overcapture Flag */
#define  TIM_SR_CC3OF                        ((uint16_t)0x0800)            /*!< Capture/Compare 3 Overcapture Flag */
#define  TIM_SR_CC4OF                        ((uint16_t)0x1000)            /*!< Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_EGR register  ********************/
#define  TIM_EGR_UG                          ((uint8_t)0x01)               /*!< Update Generation */
#define  TIM_EGR_CC1G                        ((uint8_t)0x02)               /*!< Capture/Compare 1 Generation */
#define  TIM_EGR_CC2G                        ((uint8_t)0x04)               /*!< Capture/Compare 2 Generation */
#define  TIM_EGR_CC3G                        ((uint8_t)0x08)               /*!< Capture/Compare 3 Generation */
#define  TIM_EGR_CC4G                        ((uint8_t)0x10)               /*!< Capture/Compare 4 Generation */
#define  TIM_EGR_COMG                        ((uint8_t)0x20)               /*!< Capture/Compare Control Update Generation */
#define  TIM_EGR_TG                          ((uint8_t)0x40)               /*!< Trigger Generation */
#define  TIM_EGR_BG                          ((uint8_t)0x80)               /*!< Break Generation */

/******************  Bit definition for TIM_CCMR1 register  *******************/
#define  TIM_CCMR1_CC1S                      ((uint16_t)0x0003)            /*!< CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CCMR1_CC1S_0                    ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_CCMR1_CC1S_1                    ((uint16_t)0x0002)            /*!< Bit 1 */

#define  TIM_CCMR1_OC1FE                     ((uint16_t)0x0004)            /*!< Output Compare 1 Fast enable */
#define  TIM_CCMR1_OC1PE                     ((uint16_t)0x0008)            /*!< Output Compare 1 Preload enable */

#define  TIM_CCMR1_OC1M                      ((uint16_t)0x0070)            /*!< OC1M[2:0] bits (Output Compare 1 Mode) */
#define  TIM_CCMR1_OC1M_0                    ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_CCMR1_OC1M_1                    ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_CCMR1_OC1M_2                    ((uint16_t)0x0040)            /*!< Bit 2 */

#define  TIM_CCMR1_OC1CE                     ((uint16_t)0x0080)            /*!< Output Compare 1Clear Enable */

#define  TIM_CCMR1_CC2S                      ((uint16_t)0x0300)            /*!< CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CCMR1_CC2S_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_CCMR1_CC2S_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  TIM_CCMR1_OC2FE                     ((uint16_t)0x0400)            /*!< Output Compare 2 Fast enable */
#define  TIM_CCMR1_OC2PE                     ((uint16_t)0x0800)            /*!< Output Compare 2 Preload enable */

#define  TIM_CCMR1_OC2M                      ((uint16_t)0x7000)            /*!< OC2M[2:0] bits (Output Compare 2 Mode) */
#define  TIM_CCMR1_OC2M_0                    ((uint16_t)0x1000)            /*!< Bit 0 */
#define  TIM_CCMR1_OC2M_1                    ((uint16_t)0x2000)            /*!< Bit 1 */
#define  TIM_CCMR1_OC2M_2                    ((uint16_t)0x4000)            /*!< Bit 2 */

#define  TIM_CCMR1_OC2CE                     ((uint16_t)0x8000)            /*!< Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR1_IC1PSC                    ((uint16_t)0x000C)            /*!< IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_CCMR1_IC1PSC_0                  ((uint16_t)0x0004)            /*!< Bit 0 */
#define  TIM_CCMR1_IC1PSC_1                  ((uint16_t)0x0008)            /*!< Bit 1 */

#define  TIM_CCMR1_IC1F                      ((uint16_t)0x00F0)            /*!< IC1F[3:0] bits (Input Capture 1 Filter) */
#define  TIM_CCMR1_IC1F_0                    ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_CCMR1_IC1F_1                    ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_CCMR1_IC1F_2                    ((uint16_t)0x0040)            /*!< Bit 2 */
#define  TIM_CCMR1_IC1F_3                    ((uint16_t)0x0080)            /*!< Bit 3 */

#define  TIM_CCMR1_IC2PSC                    ((uint16_t)0x0C00)            /*!< IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define  TIM_CCMR1_IC2PSC_0                  ((uint16_t)0x0400)            /*!< Bit 0 */
#define  TIM_CCMR1_IC2PSC_1                  ((uint16_t)0x0800)            /*!< Bit 1 */

#define  TIM_CCMR1_IC2F                      ((uint16_t)0xF000)            /*!< IC2F[3:0] bits (Input Capture 2 Filter) */
#define  TIM_CCMR1_IC2F_0                    ((uint16_t)0x1000)            /*!< Bit 0 */
#define  TIM_CCMR1_IC2F_1                    ((uint16_t)0x2000)            /*!< Bit 1 */
#define  TIM_CCMR1_IC2F_2                    ((uint16_t)0x4000)            /*!< Bit 2 */
#define  TIM_CCMR1_IC2F_3                    ((uint16_t)0x8000)            /*!< Bit 3 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define  TIM_CCMR2_CC3S                      ((uint16_t)0x0003)            /*!< CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define  TIM_CCMR2_CC3S_0                    ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_CCMR2_CC3S_1                    ((uint16_t)0x0002)            /*!< Bit 1 */

#define  TIM_CCMR2_OC3FE                     ((uint16_t)0x0004)            /*!< Output Compare 3 Fast enable */
#define  TIM_CCMR2_OC3PE                     ((uint16_t)0x0008)            /*!< Output Compare 3 Preload enable */

#define  TIM_CCMR2_OC3M                      ((uint16_t)0x0070)            /*!< OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_CCMR2_OC3M_0                    ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_CCMR2_OC3M_1                    ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_CCMR2_OC3M_2                    ((uint16_t)0x0040)            /*!< Bit 2 */

#define  TIM_CCMR2_OC3CE                     ((uint16_t)0x0080)            /*!< Output Compare 3 Clear Enable */

#define  TIM_CCMR2_CC4S                      ((uint16_t)0x0300)            /*!< CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CCMR2_CC4S_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_CCMR2_CC4S_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  TIM_CCMR2_OC4FE                     ((uint16_t)0x0400)            /*!< Output Compare 4 Fast enable */
#define  TIM_CCMR2_OC4PE                     ((uint16_t)0x0800)            /*!< Output Compare 4 Preload enable */

#define  TIM_CCMR2_OC4M                      ((uint16_t)0x7000)            /*!< OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_CCMR2_OC4M_0                    ((uint16_t)0x1000)            /*!< Bit 0 */
#define  TIM_CCMR2_OC4M_1                    ((uint16_t)0x2000)            /*!< Bit 1 */
#define  TIM_CCMR2_OC4M_2                    ((uint16_t)0x4000)            /*!< Bit 2 */

#define  TIM_CCMR2_OC4CE                     ((uint16_t)0x8000)            /*!< Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/

#define  TIM_CCMR2_IC3PSC                    ((uint16_t)0x000C)            /*!< IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_CCMR2_IC3PSC_0                  ((uint16_t)0x0004)            /*!< Bit 0 */
#define  TIM_CCMR2_IC3PSC_1                  ((uint16_t)0x0008)            /*!< Bit 1 */

#define  TIM_CCMR2_IC3F                      ((uint16_t)0x00F0)            /*!< IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_CCMR2_IC3F_0                    ((uint16_t)0x0010)            /*!< Bit 0 */
#define  TIM_CCMR2_IC3F_1                    ((uint16_t)0x0020)            /*!< Bit 1 */
#define  TIM_CCMR2_IC3F_2                    ((uint16_t)0x0040)            /*!< Bit 2 */
#define  TIM_CCMR2_IC3F_3                    ((uint16_t)0x0080)            /*!< Bit 3 */

#define  TIM_CCMR2_IC4PSC                    ((uint16_t)0x0C00)            /*!< IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_CCMR2_IC4PSC_0                  ((uint16_t)0x0400)            /*!< Bit 0 */
#define  TIM_CCMR2_IC4PSC_1                  ((uint16_t)0x0800)            /*!< Bit 1 */

#define  TIM_CCMR2_IC4F                      ((uint16_t)0xF000)            /*!< IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_CCMR2_IC4F_0                    ((uint16_t)0x1000)            /*!< Bit 0 */
#define  TIM_CCMR2_IC4F_1                    ((uint16_t)0x2000)            /*!< Bit 1 */
#define  TIM_CCMR2_IC4F_2                    ((uint16_t)0x4000)            /*!< Bit 2 */
#define  TIM_CCMR2_IC4F_3                    ((uint16_t)0x8000)            /*!< Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CCER_CC1E                       ((uint16_t)0x0001)            /*!< Capture/Compare 1 output enable */
#define  TIM_CCER_CC1P                       ((uint16_t)0x0002)            /*!< Capture/Compare 1 output Polarity */
#define  TIM_CCER_CC1NE                      ((uint16_t)0x0004)            /*!< Capture/Compare 1 Complementary output enable */
#define  TIM_CCER_CC1NP                      ((uint16_t)0x0008)            /*!< Capture/Compare 1 Complementary output Polarity */
#define  TIM_CCER_CC2E                       ((uint16_t)0x0010)            /*!< Capture/Compare 2 output enable */
#define  TIM_CCER_CC2P                       ((uint16_t)0x0020)            /*!< Capture/Compare 2 output Polarity */
#define  TIM_CCER_CC2NE                      ((uint16_t)0x0040)            /*!< Capture/Compare 2 Complementary output enable */
#define  TIM_CCER_CC2NP                      ((uint16_t)0x0080)            /*!< Capture/Compare 2 Complementary output Polarity */
#define  TIM_CCER_CC3E                       ((uint16_t)0x0100)            /*!< Capture/Compare 3 output enable */
#define  TIM_CCER_CC3P                       ((uint16_t)0x0200)            /*!< Capture/Compare 3 output Polarity */
#define  TIM_CCER_CC3NE                      ((uint16_t)0x0400)            /*!< Capture/Compare 3 Complementary output enable */
#define  TIM_CCER_CC3NP                      ((uint16_t)0x0800)            /*!< Capture/Compare 3 Complementary output Polarity */
#define  TIM_CCER_CC4E                       ((uint16_t)0x1000)            /*!< Capture/Compare 4 output enable */
#define  TIM_CCER_CC4P                       ((uint16_t)0x2000)            /*!< Capture/Compare 4 output Polarity */
#define  TIM_CCER_CC4NP                      ((uint16_t)0x8000)            /*!< Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT_CNT                         ((uint16_t)0xFFFF)            /*!< Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC_PSC                         ((uint16_t)0xFFFF)            /*!< Prescaler Value */

/*******************  Bit definition for TIM_ARR register  ********************/
#define  TIM_ARR_ARR                         ((uint16_t)0xFFFF)            /*!< actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define  TIM_RCR_REP                         ((uint8_t)0xFF)               /*!< Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define  TIM_CCR1_CCR1                       ((uint16_t)0xFFFF)            /*!< Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define  TIM_CCR2_CCR2                       ((uint16_t)0xFFFF)            /*!< Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define  TIM_CCR3_CCR3                       ((uint16_t)0xFFFF)            /*!< Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define  TIM_CCR4_CCR4                       ((uint16_t)0xFFFF)            /*!< Capture/Compare 4 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_BDTR_DTG                        ((uint16_t)0x00FF)            /*!< DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_BDTR_DTG_0                      ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_BDTR_DTG_1                      ((uint16_t)0x0002)            /*!< Bit 1 */
#define  TIM_BDTR_DTG_2                      ((uint16_t)0x0004)            /*!< Bit 2 */
#define  TIM_BDTR_DTG_3                      ((uint16_t)0x0008)            /*!< Bit 3 */
#define  TIM_BDTR_DTG_4                      ((uint16_t)0x0010)            /*!< Bit 4 */
#define  TIM_BDTR_DTG_5                      ((uint16_t)0x0020)            /*!< Bit 5 */
#define  TIM_BDTR_DTG_6                      ((uint16_t)0x0040)            /*!< Bit 6 */
#define  TIM_BDTR_DTG_7                      ((uint16_t)0x0080)            /*!< Bit 7 */

#define  TIM_BDTR_LOCK                       ((uint16_t)0x0300)            /*!< LOCK[1:0] bits (Lock Configuration) */
#define  TIM_BDTR_LOCK_0                     ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_BDTR_LOCK_1                     ((uint16_t)0x0200)            /*!< Bit 1 */

#define  TIM_BDTR_OSSI                       ((uint16_t)0x0400)            /*!< Off-State Selection for Idle mode */
#define  TIM_BDTR_OSSR                       ((uint16_t)0x0800)            /*!< Off-State Selection for Run mode */
#define  TIM_BDTR_BKE                        ((uint16_t)0x1000)            /*!< Break enable */
#define  TIM_BDTR_BKP                        ((uint16_t)0x2000)            /*!< Break Polarity */
#define  TIM_BDTR_AOE                        ((uint16_t)0x4000)            /*!< Automatic Output enable */
#define  TIM_BDTR_MOE                        ((uint16_t)0x8000)            /*!< Main Output enable */

/*******************  Bit definition for TIM_DCR register  ********************/
#define  TIM_DCR_DBA                         ((uint16_t)0x001F)            /*!< DBA[4:0] bits (DMA Base Address) */
#define  TIM_DCR_DBA_0                       ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_DCR_DBA_1                       ((uint16_t)0x0002)            /*!< Bit 1 */
#define  TIM_DCR_DBA_2                       ((uint16_t)0x0004)            /*!< Bit 2 */
#define  TIM_DCR_DBA_3                       ((uint16_t)0x0008)            /*!< Bit 3 */
#define  TIM_DCR_DBA_4                       ((uint16_t)0x0010)            /*!< Bit 4 */

#define  TIM_DCR_DBL                         ((uint16_t)0x1F00)            /*!< DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DCR_DBL_0                       ((uint16_t)0x0100)            /*!< Bit 0 */
#define  TIM_DCR_DBL_1                       ((uint16_t)0x0200)            /*!< Bit 1 */
#define  TIM_DCR_DBL_2                       ((uint16_t)0x0400)            /*!< Bit 2 */
#define  TIM_DCR_DBL_3                       ((uint16_t)0x0800)            /*!< Bit 3 */
#define  TIM_DCR_DBL_4                       ((uint16_t)0x1000)            /*!< Bit 4 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define  TIM_DMAR_DMAB                       ((uint16_t)0xFFFF)            /*!< DMA register for burst accesses */

/*******************  Bit definition for TIM_SARAD register  *******************/
#define  TIM_SARAD_MASK                      ((uint16_t)0xFFFF)            /*!< TIM_SARAD register for burst accesses */
#define  TIM_SARAD_CH0                       ((uint16_t)0x0001)            /*!< Bit 0 */
#define  TIM_SARAD_CH1                       ((uint16_t)0x0002)            /*!< Bit 1 */
#define  TIM_SARAD_CH2                       ((uint16_t)0x0004)            /*!< Bit 2 */
#define  TIM_SARAD_CH3                       ((uint16_t)0x0008)            /*!< Bit 3 */
#define  TIM_SARAD_CH4                       ((uint16_t)0x0010)            /*!< Bit 4 */
#define  TIM_SARAD_CH5                       ((uint16_t)0x0020)            /*!< Bit 5 */
#define  TIM_SARAD_CH6                       ((uint16_t)0x0040)            /*!< Bit 6 */
#define  TIM_SARAD_CH7                       ((uint16_t)0x0080)            /*!< Bit 7 */
#define  TIM_SARAD_CH8                       ((uint16_t)0x0100)            /*!< Bit 8 */
#define  TIM_SARAD_CH9                       ((uint16_t)0x0200)            /*!< Bit 9 */
#define  TIM_SARAD_CH10                      ((uint16_t)0x0400)            /*!< Bit 10 */
#define  TIM_SARAD_CH11                      ((uint16_t)0x0800)            /*!< Bit 11 */
#define  TIM_SARAD_CH12                      ((uint16_t)0x1000)            /*!< Bit 12 */
#define  TIM_SARAD_CH13                      ((uint16_t)0x2000)            /*!< Bit 13 */
#define  TIM_SARAD_CH14                      ((uint16_t)0x4000)            /*!< Bit 14 */
#define  TIM_SARAD_CH15                      ((uint16_t)0x8000)            /*!< Bit 15 */
#define  TIM_SARAD_TRGOEN                    ((uint32_t)0x10000)           /*!< Bit 16 */


/**
 * @brief GPIO Register Structure
 */
typedef struct {
    volatile u32 io_set;
    volatile u32 io_clr;
    volatile u32 data;
    volatile u32 dir;
    volatile u32 de;
    volatile u32 fen;
    volatile u32 fdir;
    volatile u32 drv;
    volatile u32 pupd[6];
} gpio_typedef;

/**
 * @brief Function and IO Mapping Register Structure
 */
typedef struct {
    volatile u32 func_output_map[8];
    volatile u32 func_input_map[7];
} func_map_typedef;

/**
 * @brief Timer Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 cpnd;
    volatile u32 cnt;
    volatile u32 period;
    volatile u32 capture_val;
    volatile u32 duty0;
    volatile u32 duty1;
    volatile u32 duty2;
    volatile u32 psc;
} tmr_typedef;

/**
 * @brief Uart Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 cpnd;
    volatile u32 baud;
    volatile u32 data;
} uart_typedef;

/**
 * @brief High Speed UART Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 cpnd;
    volatile u32 baud;
    volatile u32 data;
    volatile u32 tx_cnt;
    volatile u32 tx_adr;
    volatile u32 rx_cnt;
    const    u32 reserved0;
    const    u32 reserved1;
    volatile u32 rx_adr;
    volatile u32 fifo_cnt;
    volatile u32 fifo;
    volatile u32 fifo_adr;
    volatile u32 tmr_cnt;
    volatile u32 flow_ctrl;
} hsuart_typedef;

/**
 * @brief IIC Register Structure
 */
typedef struct {
    volatile u32 con0;
    volatile u32 con1;
    volatile u32 cmd_addr;
    volatile u32 data;
    volatile u32 dma_addr;
    volatile u32 dma_cnt;
    volatile u32 slave_sta;
} iic_typedef;

/**
 * @brief SPI Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 buf;
    volatile u32 baud;
    volatile u32 cpnd;
    volatile u32 dma_cnt;
    volatile u32 dma_adr;
} spi_typedef;

/**
 * @brief WDT Register Structure
 */
typedef struct {
    volatile u32 con;
} wdt_typedef;

/**
 * @brief SARADC Register Structure
 */
typedef struct {
    volatile u32 dat[16];
    volatile u32 con;
    volatile u32 ch;
    volatile u32 cst;
    volatile u32 baud;
    const    u32 reserved0;
    const    u32 reserved1;
    const    u32 reserved2;
    const    u32 reserved3;
    const    u32 reserved4;
    volatile u32 ch_int;
    volatile u32 ch_pnd;
} sadc_typedef;

/**
 * @brief LPWM Register Structure
 */
typedef struct {
    volatile u32 start_con;
    volatile u32 switch_con;
    volatile u32 cnt_max_con;
    volatile u32 stageA_con;
    volatile u32 stageB_con;
    volatile u32 stageC_con;
    volatile u32 stageD_con;
    volatile u32 stageE_con;
    volatile u32 stageF_con;
    volatile u32 stageHL_con;
    volatile u32 pend;
} lpwm_typedef;

/**
 * @brief KEYSCAN Register Structure
 */
typedef struct {
    volatile u32 con0;
    volatile u32 con1;
    volatile u32 clr;
    volatile u32 info0;
    volatile u32 info1;
    volatile u32 info2;
} keyscan_typedef;

/**
 * @brief IRRX Register Structure
 */
typedef struct {
    // IR RX Control Register
    volatile u32 con;
    // IR RX Data Register
    volatile u32 rxdata;
    // IR RX Clear Pending Register
    volatile u32 cpnd;
    // IR RX Error Config Register0
    volatile u32 err0;
    // IR RX Error Config Register1
    volatile u32 err1;
    // IR RX Period Config Register0
    volatile u32 period0;
    // IR RX Period Config Register1
    volatile u32 period1;
} irrx_typedef;

/**
 * @brief IRTX Register Structure
 */
typedef struct {
    // IR One Code Time Register
    volatile u32 otime;
    // IR One Zero Time Register
    volatile u32 ztime;
    // IR Start Code Time Register
    volatile u32 stime;
    // IR Repeat Code Time Register
    volatile u32 rtime;
    // IR Repeat Code Control Register
    volatile u32 rctrl;
    // IR TX Control Register
    volatile u32 con;
    // IR TX Data Register
    volatile u32 txdata;
    // IR TX Data Length Register
    volatile u32 txlen;
    // IR TX Pending Register
    volatile u32 pend;
    // IR TX DMA Control Register
    volatile u32 dma_con;
    // IR TX DMA Input Address Register
    volatile u32 dma_iadr;
    // IR TX DMA Input Size Register
    volatile u32 dma_isize;
    // IR TX DMA Output Address Register
    volatile u32 dma_oadr;
    // IR TX DMA Output Size Register
    volatile u32 dma_osize;
    // IR TX DMA Pending Register
    volatile u32 dma_pend;
} irtx_typedef;

/**
 * @brief Touch Key Register Structure
 */
typedef struct {
    // Touch Key Control Register
    volatile u32 tkcon;
    // Touch Key Control Register 1
    volatile u32 tkcon1;
    // Touch Key Counter Register
    volatile u32 tkcnt;
    // Touch Key Charge And Discharge Period Register 0
    volatile u32 tkcdpr0;
    // Touch Key Charge And Discharge Period Register 1
    volatile u32 tkcdpr1;
    // Touch Key Times Register
    volatile u32 tktmr;
    // Touch Ear Times Register
    volatile u32 tetmr;
    // Touch Key Base Counter Register
    volatile u32 tkbcnt;
    // Touch Key Press Threshold Register
    volatile u32 tkpthd;
    // Touch Key Exception Threshold Register
    volatile u32 tkethd;
    // Touch Ear Base Counter Register
    volatile u32 tebcnt;
    // Touch Ear Press Threshold Register
    volatile u32 tepthd;
    // Touch Ear Exception Threshold Register
    volatile u32 teethd;
    // Touch Key Control Register 2
    volatile u32 tkcon2;
    // Touch Key Variance Register
    volatile u32 tkvari;
    // Touch Key Variance Threshold Register
    volatile u32 tkvarithd;
    // Touch Key Analog Control Register 0
    volatile u32 tkacon0;
    // Touch Key Analog Control Register 1
    volatile u32 tkacon1;
} touch_key_typedef;

/**
 * @brief SDADC Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 gain_con;
    volatile u32 get_dc_con;
    volatile u32 dma_con;
    volatile u32 dma_addr;
    volatile u32 dma_size;
    volatile u32 dma_pend;
    volatile u32 dma_cpnd;
    volatile u32 ram_test_addr;
    volatile u32 ram_test_data;
} sdadc_typedef;

/**
 * @brief SDDAC Register Structure
 */
typedef struct {
    volatile u32 dma_con;
    volatile u32 dma_addr;
    volatile u32 dma_size;
    volatile u32 dma_pend;
    volatile u32 dma_cpnd;
    volatile u32 con;
} sddac_typedef;

/**
 * @brief LEDC Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 fd;
    volatile u32 lp;
    volatile u32 tix;
    volatile u32 rstx;
    volatile u32 addr;
    volatile u32 cnt;
    volatile u32 pend;
} ledc_typedef;

/**
 * @brief TICK0 Register Structure
 */
typedef struct {
    volatile u32 con;
    volatile u32 cpnd;
    volatile u32 cnt;
    volatile u32 pr;
} tick_typedef;

/**
 * @brief Quadrature Decoder Register Structure
 */
typedef struct {
    volatile u32 con;
    const    u32 reserve1;
    const    u32 reserve2;
    volatile u32 cpnd;
    const    u32 reserve3;
    const    u32 reserve4;
    volatile u32 dat;
} qdec_typedef;
#endif // _DRIVER_COM_H
