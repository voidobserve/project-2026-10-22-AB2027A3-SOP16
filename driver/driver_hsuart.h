/*
 * @File name    : driver_hsuart.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file contains all the functions prototypes for the HSUART library.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_HSUART_H
#define _DRIVER_HSUART_H

#include "driver_com.h"

/**
 * @brief HSUART IT Flag Definition
 */
typedef enum {
    HSUART_IT_RX_DMA_HF     = 0x01,
    HSUART_IT_RX_TMR_OV     = 0x02,
    HSUART_IT_TX            = 0x04,
    HSUART_IT_RX            = 0x08,
} HSUART_IT_TYPEDEF;

/**
 * @brief TX Stop Bit Enumeration
 */
typedef enum {
    HSUT_STOP_BIT_1BIT      = 0x00000000,
    HSUT_STOP_BIT_2BIT      = 0x00000200,
} HSUT_TX_STOP_BIT_TYPEDEF;

/**
 * @brief TX Word Length Enumeration
 */
typedef enum {
    HSUT_TX_LENGTH_8b       = 0x00000000,
    HSUT_TX_LENGTH_9b       = 0x00000100,
} HSUT_TX_WORD_LENGTH;

/**
 * @brief TX Transmit Mode Enumeration
 */
typedef enum {
    HSUT_TX_BUF_MODE        = 0x00000000,
    HSUT_TX_DMA_MODE        = 0x00000080,
} HSUT_TX_MODE;

/**
 * @brief RX Word Length Enumeration
 */
typedef enum {
    HSUT_RX_LENGTH_8b       = 0x00000000,
    HSUT_RX_LENGTH_9b       = 0x00000020,
} HSUT_RX_WORD_LENGTH;

/**
 * @brief RX Transmit Mode Enumeration
 */
typedef enum {
    HSUT_RX_BUF_MODE        = 0x00000000,
    HSUT_RX_DMA_MODE        = 0x00000010,
} HSUT_RX_MODE;

/**
 * @brief Receive or Transmit Enumeration
 */
typedef enum {
    HSUT_RECEIVE            = 0x00000001,
    HSUT_TRANSMIT           = 0x00000002,
} HSUT_REC_TRA_CMD;

/**
 * @brief One-line Mode Enumeration
 */
typedef enum {
    HSUT_ONELINE_DIS        = 0x00000000,
    HSUT_ONELINE_EN         = 0x00040000,
} HSUT_ONELINE_TYPEDEF;

/**
 * @brief Rx DMA Mode Loop Buf Enumeration
 */
typedef enum {
    HSUT_DMA_BUF_SINGLE     = 0x00000000,
    HSUT_DMA_BUF_LOOP       = 0x00000040,
} HSUT_DMA_BUF_TYPEDEF;

/**
 * @brief Clock Selection
 */
typedef enum {
    HSUT_24M_CLK,
    HSUT_48M_CLK,
    HSUT_PLLDIV4_CLK,
} HSUT_CLK_TYPEDEF;

/**
 * @brief HSUART Init Structure
 */
typedef struct {
    uint32_t                    baud;
    HSUT_TX_STOP_BIT_TYPEDEF    tx_stop_bit;
    HSUT_TX_WORD_LENGTH         tx_word_len;
    HSUT_RX_WORD_LENGTH         rx_word_len;
    HSUT_TX_MODE                tx_mode;
    HSUT_RX_MODE                rx_mode;
    HSUT_DMA_BUF_TYPEDEF        rx_dma_buf_mode;
    HSUT_CLK_TYPEDEF            clk_source;
    HSUT_ONELINE_TYPEDEF        one_line_en;
} hsuart_init_typedef;

//--------------- Function used to configure hsuart ----------------//
void hsuart_init(hsuart_init_typedef *hsuart_init_struct);
void hsuart_deinit(void);
void hsuart_cmd(HSUT_REC_TRA_CMD rec_tra_sel, FUNCTIONAL_STATE state);
void hsuart_send_data(uint16_t data);
uint16_t hsuart_receive_data(void);
STATUS_STATE hsuart_receive_data_from_fifo(uint8_t *buf, uint8_t len);
uint16_t hsuart_get_fifo_counter(void);
void hsuart_clear_fifo_counter(void);
void hsuart_baud_config(uint32_t baud);
//---------- Function used to configure hsuart interrupt -----------//
void hsuart_pic_config(isr_t isr, int pr, HSUART_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE hsuart_get_flag(HSUART_IT_TYPEDEF interrupt_type);
void hsuart_clear_flag(HSUART_IT_TYPEDEF interrupt_type);
void hsuart_rx_idle_config(uint16_t idle_time, FUNCTIONAL_STATE state);
//------------- Function used to configure hsuart DMA --------------//
void hsuart_dma_start(HSUT_REC_TRA_CMD rec_tra_sel, uint32_t addr, uint16_t len);
void hsuart_dma_stop(HSUT_REC_TRA_CMD rec_tra_sel);

#endif // _DRIVER_HSUART_H
