/*
 * @File name    : driver_uart.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the UART peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_uart.h"

/**
  * @brief  Initializes the uart peripheral according to the specified
  *         parameters in the uart_init_struct.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  uart_init_struct: pointer to a uart_init_typedef structure that
  *         contains the configuration information for the specified UART peripheral.
  * @retval None
  */
void uart_init(uart_typedef *uartx, uart_init_typedef* uart_init_struct)
{
    uint32_t tmp_reg, sys_clk_val, baud_reg;

    //---------- UART Controller Register ----------//
    tmp_reg = uartx->con;
    tmp_reg &= (uint32_t)(~UARTxCON_RXEN);
    tmp_reg &= (uint32_t)(~UARTxCON_ONELINE);
    tmp_reg &= (uint32_t)(~UARTxCON_BIT9EN);
    tmp_reg &= (uint32_t)(~UARTxCON_SB2EN);
    tmp_reg |= (uint32_t)(uart_init_struct->mode |
                          uart_init_struct->one_line_enable |
                          uart_init_struct->stop_bits |
                          uart_init_struct->word_len);

    if (uartx == UART) {
        tmp_reg &= (uint32_t)(~UARTxCON_CLKSRC);
        tmp_reg |= (uint32_t)(uart_init_struct->clock_source);
    } else if (uartx == UDET) {
        tmp_reg &= (uint32_t)(~UARTxCON_FIXBAUD);
        tmp_reg |= (uint32_t)(uart_init_struct->baud_fix);
    }

    uartx->con = tmp_reg;

    if (uartx == UART) {
        //---------- UART Baud Configuration ----------//
        sys_clk_val = sys_clk_nhz_get();
        if (uart_init_struct->clock_source == UART_SYSTEM_CLOCK) {
            baud_reg = (uint16_t)((sys_clk_val / uart_init_struct->baud) - 1);
        } else {
            // uart clk is double frequency of the clock source when select uart_inc
            sys_clk_val = 2 * clk_uart_inc_get(CLK_VALUE_MODE_FREQ);
            baud_reg = (uint16_t)((sys_clk_val / uart_init_struct->baud) - 1);
        }
        tmp_reg = (uint32_t)((baud_reg << 16) | baud_reg);
        uartx->baud = tmp_reg;
    } else if (uartx == UDET && uart_init_struct->baud_fix == UART_BAUD_FIX) {
        sys_clk_val = clk_udet_clk_get(CLK_VALUE_MODE_FREQ);
        baud_reg = (uint16_t)((sys_clk_val / uart_init_struct->baud) - 1);
        uartx->baud = baud_reg;
    }
}

/**
 * @brief  De-initialize the specified uart peripheral.
 * @param  uartx: which uart need to config. it should the one of [UART or UDET].
 * @retval None
 */
void uart_deinit(uart_typedef *uartx)
{
    uartx->con = 0;

    if (uartx == UART) {
        clk_gate0_cmd(CLK_GATE0_UART, CLK_DIS);
    } else if (uartx == UDET) {
        clk_gate0_cmd(CLK_GATE0_UDET, CLK_DIS);
    }
}

/**
  * @brief  Enable or disable the specified UART peripheral.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  state: the state of the UARTx peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void uart_cmd(uart_typedef *uartx, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        uartx->con |= UARTxCON_UTEN;
    } else {
        uartx->con &= (uint32_t)(~UARTxCON_UTEN);
    }
}

/**
  * @brief  Transmits data through the UARTx peripheral.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  data: the data to transmit.
  * @retval None
  */
AT(.com_periph.uart.send)
void uart_send_data(uart_typedef *uartx, uint16_t data)
{
    uartx->data = (data & (uint16_t)0xff);
}

/**
  * @brief  Return the received data lastly by the UARTx peripheral.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @retval The received data.
  */
AT(.com_periph.uart.recv)
uint16_t uart_receive_data(uart_typedef *uartx)
{
    if (uartx->con & UARTxCON_BIT9EN) {
        return (uartx->data & (uint16_t)0x1ff);
    } else {
        return (uartx->data & (uint16_t)0xff);
    }
}

/**
  * @brief  Enable or disable the specified UARTx interrupt.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrupt_type: specifies the UART interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg UART_IT_TX: Transmit single data finish interrupt enable bit.
  *             @arg UART_IT_RX: Receive single data finish interrupt enable bit.
  *             @arg UART_IT_ERR: Raise error interrupt enable bit.
  *             @arg UART_IT_KEY: Match key interrupt enable bit.
  *             @arg UART_IT_RSTKEY: Reset match key interrupt enable bit.
  * @param  state: the state of the UARTx peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void uart_pic_config(uart_typedef *uartx, isr_t isr, int pr, UART_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_con_bit = 0;
    uint32_t all_interrupt_type_mask = UART_IT_TX | UART_IT_RX | UART_IT_ERR | UART_IT_KEY | UART_IT_RSTKEY;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & UART_IT_TX) {
        interrupt_con_bit |= UARTxCON_TXIE;
    }

    if (interrupt_type & UART_IT_RX) {
        interrupt_con_bit |= UARTxCON_RXIE;
    }

    if (interrupt_type & UART_IT_ERR) {
    }

    if (interrupt_type & UART_IT_KEY) {
    }

    if (interrupt_type & UART_IT_RSTKEY) {
    }

    if (state != DISABLE) {
        sys_irq_init(IRQn_UART0_UDET, pr, isr);
        uartx->con |= interrupt_con_bit;
    } else {
        uartx->con &= ~interrupt_con_bit;
        if (interrupt_type == all_interrupt_type_mask) {
            PICEN &= ~BIT(IRQn_UART0_UDET);
        }
    }
}

/**
  * @brief  Check the specified UART flag is set or not.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  interrupt_type: specifies the flag to check.
  *         this parameter can be one of the following values:
  *             @arg UART_IT_TX: Transmit single data finish interrupt enable bit.
  *             @arg UART_IT_RX: Receive single data finish interrupt enable bit.
  *             @arg UART_IT_ERR: Raise error interrupt enable bit.
  *             @arg UART_IT_KEY: Match key interrupt enable bit.
  *             @arg UART_IT_RSTKEY: Reset match key interrupt enable bit.
  * @retval The state of uart_flag (SET or RESET).
  */
AT(.com_periph.uart.get_flag)
FLAG_STATE uart_get_flag(uart_typedef *uartx, UART_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_con_bit = 0;

    if (interrupt_type & UART_IT_TX) {
        interrupt_con_bit |= UARTxCON_TXPND;
    }

    if (interrupt_type & UART_IT_RX) {
        interrupt_con_bit |= UARTxCON_RXPND;
    }

    if (interrupt_type & UART_IT_ERR) {
    }

    if (interrupt_type & UART_IT_KEY) {
    }

    if (interrupt_type & UART_IT_RSTKEY) {
    }

    if ((uartx->con & interrupt_con_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the UART's pending flag.
  * @param  uartx: where x can be (0..2) to select the uartx peripheral.
  * @param  interrupt_type: specifies the flag to check.
  *         this parameter can be one of the following values:
  *             @arg UART_IT_TX: Transmit single data finish interrupt enable bit.
  *             @arg UART_IT_RX: Receive single data finish interrupt enable bit.
  *             @arg UART_IT_ERR: Raise error interrupt enable bit.
  *             @arg UART_IT_KEY: Match key interrupt enable bit.
  *             @arg UART_IT_RSTKEY: Reset match key interrupt enable bit.
  * @retval None
  */
AT(.com_periph.uart.clear_flag)
void uart_clear_flag(uart_typedef *uartx, UART_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_con_bit = 0;

    if (interrupt_type & UART_IT_TX) {
        interrupt_con_bit |= UARTxCPND_CTXPND;
    }

    if (interrupt_type & UART_IT_RX) {
        interrupt_con_bit |= UARTxCPND_CRXPND;
    }

    if (interrupt_type & UART_IT_ERR) {
        interrupt_con_bit |= UARTxCPND_RX4BUF_ERR_CLR;
    }

    if (interrupt_type & UART_IT_KEY) {
        interrupt_con_bit |= UARTxCPND_CKEYPND;
    }

    if (interrupt_type & UART_IT_RSTKEY) {
        interrupt_con_bit |= UARTxCPND_CRSTKEYPND;
    }

    uartx->cpnd |= interrupt_con_bit;
}

/**
 * @brief  Config the baud rate of the serial port.
 * @param  uartx: The uart that need to config.
 * @param  baud: The baudrate value need to set.
 * @retval None
 */
void uart_baud_config(uart_typedef *uartx, uint32_t baud)
{
    uint32_t clk_val, baud_reg;

    if (uartx->con & UARTxCON_UTEN)
    while ((uartx->con & UARTxCON_TXPND) == 0);

    if (uartx == UART) {
        if ((uartx->con & UARTxCON_CLKSRC) == 0) {                  // if select is sys clk
            clk_val = sys_clk_nhz_get();
        } else {                                                    // if select is inc clk
            clk_val = 2 * clk_uart_inc_get(CLK_VALUE_MODE_FREQ);    // uart clk is double frequency of the clock source when select uart_inc
        }

        baud_reg = (uint16_t)((clk_val / baud) - 1);
        uartx->baud = (uint32_t)((baud_reg << 16) | baud_reg);

    } else if (uartx == UDET && (uartx->con & UARTxCON_FIXBAUD)) {
        clk_val = clk_udet_clk_get(CLK_VALUE_MODE_FREQ);
        baud_reg = (uint16_t)((clk_val / baud) - 1);
        uartx->baud = baud_reg;
    }
}
