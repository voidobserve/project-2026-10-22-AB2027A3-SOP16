/*
 * @File name    : driver_uart.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file contains all the functions prototypes for the UART library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_UART_H
#define _DRIVER_UART_H

#include "driver_com.h"

/**
 * @brief UART IT Flag Definition
 */
typedef enum {
    UART_IT_TX          = 0x01,
    UART_IT_RX          = 0x02,
    UART_IT_ERR         = 0x04,
    UART_IT_KEY         = 0x08,
    UART_IT_RSTKEY      = 0x10,
} UART_IT_TYPEDEF;

/**
 * @brief Rx Enable enumeration
 */
typedef enum {
    UART_SEND_MODE      = 0x00000000,
    UART_SEND_RECV_MODE = 0x00000080,
} UART_MODE_TYPEDEF;

/**
 * @brief One-line Mode Enumeration
 */
typedef enum {
    UART_SEPARATE       = 0x00000000,
    UART_ONE_LINE       = 0x00000040,
} UART_ONELINE_TYPEDEF;

/**
 * @brief Clock select Enumeration
 */
typedef enum {
    UART_SYSTEM_CLOCK   = 0x00000000,
    UART_INC_CLOCK      = 0x00000020,
} UART_CLOCK_TYPEDEF;

/**
 * @brief Stop Bit Select Enumeration
 */
typedef enum {
    UART_STOP_BIT_1BIT  = 0x00000000,
    UART_STOP_BIT_2BIT  = 0x00000010,
} UART_STOP_BIT_TYPEDEF;

/**
 * @brief Word Length Enumeration
 */
typedef enum {
    UART_WORD_LENGTH_8b = 0x00000000,
    UART_WORD_LENGTH_9b = 0x00000002,
} UART_WORD_LENGTH;

/**
 * @brief Baud Fix Enumeration, only UDET Can Use.
 */
typedef enum {
    UART_BAUD_AUTO      = 0x00000000,
    UART_BAUD_FIX       = 0x00000020,
} UART_BAUD_FIX_TYPEDEF;

/**
 * @brief UART Init Structure
 */
typedef struct {
    uint32_t                baud;
    UART_MODE_TYPEDEF       mode;
    UART_ONELINE_TYPEDEF    one_line_enable;
    UART_CLOCK_TYPEDEF      clock_source;
    UART_STOP_BIT_TYPEDEF   stop_bits;
    UART_WORD_LENGTH        word_len;
    /* Only UDET */
    UART_BAUD_FIX_TYPEDEF   baud_fix;
} uart_init_typedef;

/************************* Function used to configure UART *************************/
void uart_init(uart_typedef *uartx, uart_init_typedef *uart_init_struct);
void uart_deinit(uart_typedef *uartx);
void uart_cmd(uart_typedef *uartx, FUNCTIONAL_STATE state);
void uart_send_data(uart_typedef *uartx, uint16_t data);
uint16_t uart_receive_data(uart_typedef *uartx);
void uart_pic_config(uart_typedef *uartx, isr_t isr, int pr, UART_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE uart_get_flag(uart_typedef *uartx, UART_IT_TYPEDEF interrupt_type);
void uart_clear_flag(uart_typedef *uartx, UART_IT_TYPEDEF interrupt_type);
void uart_baud_config(uart_typedef *uartx, uint32_t baud);

#endif // _DRIVER_UART_H
