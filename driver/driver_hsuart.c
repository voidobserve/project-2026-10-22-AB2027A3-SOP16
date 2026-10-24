/*
 * @File name    : driver_hsuart.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the HSUART peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_hsuart.h"

/**
  * @brief  Initializes the hsuart peripheral according to the specified
  *         parameters in the hsuart_init_struct.
  * @param  hsuart_init_struct: pointer to a hsuart_init_typedef structure that
  *         contains the configuration information for the specified HSUART peripheral.
  * @retval None
  */
void hsuart_init(hsuart_init_typedef *hsuart_init_struct)
{
    uint32_t tmp_reg, sys_clk_val, baud_reg;

    /*--- Enable model function ---*/
    RSTCON0 |= 1<<2;

    tmp_reg = HSUART->con;
    //---------- hsuart Tx configuration ----------//
    tmp_reg &= ~(uint32_t)(HSUT0CON_SPBITSEL | HSUT0CON_TXBITSEL | HSUT0CON_TXTRSMODE);
    tmp_reg |= hsuart_init_struct->tx_stop_bit;
    tmp_reg |= hsuart_init_struct->tx_word_len;
    tmp_reg |= hsuart_init_struct->tx_mode;

    //---------- hsuart Rx configuration ----------//
    tmp_reg &= ~(HSUT0CON_RXBITSEL | HSUT0CON_RXTRSMODE);
    tmp_reg |= hsuart_init_struct->rx_word_len;
    tmp_reg |= hsuart_init_struct->rx_mode;

    /* hsuart one line configuration */
    tmp_reg &= ~HSUT0CON_ONELINE;
    tmp_reg |= hsuart_init_struct->one_line_en;

    /* hsuart Rx dma mode loop buf configuration */
    if (hsuart_init_struct->rx_mode == HSUT_RX_DMA_MODE) {
        tmp_reg &= ~HSUT0CON_RXLPBUFEN;
        tmp_reg |= hsuart_init_struct->rx_dma_buf_mode;
    }

    HSUART->con = tmp_reg;

    //---------- hsuart baud and clock configuration ----------//
    if (hsuart_init_struct->clk_source == HSUT_24M_CLK) {
        clk_hsut0_clk_set(CLK_HSUT0_CLK_XOSC24M_CLK, 0);
    } else if (hsuart_init_struct->clk_source == HSUT_48M_CLK) {
        clk_hsut0_clk_set(CLK_HSUT0_CLK_XOSC48M, 0);
    } else if (hsuart_init_struct->clk_source == HSUT_PLLDIV4_CLK) {
        clk_hsut0_clk_set(CLK_HSUT0_CLK_PLLDIV2_CLK, 1);
    }

    sys_clk_val = clk_hsut0_clk_get(CLK_VALUE_MODE_FREQ);

    if (hsuart_init_struct->baud == 0) {
        return;
    }

    baud_reg = (sys_clk_val / hsuart_init_struct->baud) - 1;
    tmp_reg = (uint32_t)((baud_reg << 16) | baud_reg);
    HSUART->baud = tmp_reg;
}

/**
 * @brief  De-initialize the specified HSUART peripheral.
 * @retval None
 */
void hsuart_deinit(void)
{
    HSUART->con = 0;
    clk_gate0_cmd(CLK_GATE0_HSUART, CLK_DIS);
}

/**
  * @brief  Enable or disable the specified HSUART peripheral.
  * @param  rec_tra_sel: select whether Rx or Tx needs to be configured.
  *         this parameter cna be: HSUT_TRANSMIT or HSUT_RECEIVE.
  * @param  state: the state of the HSUART peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void hsuart_cmd(HSUT_REC_TRA_CMD rec_tra_sel, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        HSUART->con |= (uint32_t)rec_tra_sel;
    } else {
        HSUART->con &= ~(uint32_t)rec_tra_sel;
    }
}

/**
  * @brief  Transmits data through the HSUART buf mode.
  * @param  data: the data to transmit.
  * @retval None
  */
AT(.com_periph.hsuart.send)
void hsuart_send_data(uint16_t data)
{
    if (HSUART->con & HSUT0CON_TXBITSEL) {
        HSUART->data = (uint32_t)(data & 0x1ff);
    } else {
        HSUART->data = (uint32_t)(data & 0xff);
    }
}

/**
  * @brief  Return the received data lastly by the buf mode.
  * @retval The received data.
  */
AT(.com_periph.hsuart.recv)
uint16_t hsuart_receive_data(void)
{
    return (HSUART->fifo & 0xff);
}

/**
 * @brief  Get the receive data from Rx DMA FIFO.
 * @param  buf: A buf pointer used to return data.
 * @param  len: The data number need to read.
 * @retval Result of func execution. It will be SUCCESS or FAILED.
 */
AT(.com_periph.hsuart.receive_data)
STATUS_STATE hsuart_receive_data_from_fifo(uint8_t *buf, uint8_t len)
{
    u8 i = 0;

    if (len > HSUART->fifo_cnt || !buf) {
        return FAILED;
    }

    while (len--) {
        HSUART->fifo |= (1 << 8);
        while (!(HSUART->fifo & (1 << 9)));
        buf[i++] = HSUART->fifo & 0xff;
    }

    return SUCCESS;
}

/**
 * @brief  Get Rx DMA mode FIFO counter value.
 * @retval The value of FIFO counter.
 */
AT(.com_periph.hsuart.fifo_get)
uint16_t hsuart_get_fifo_counter(void)
{
    return HSUART->fifo_cnt;
}

/**
 * @brief  Clear fifo counter.
 * @retval None
 */
void hsuart_clear_fifo_counter(void)
{
    HSUART->cpnd |= (uint32_t)(1 << 17);
}

/**
 * @brief  Change the HSUART baud.
 * @param  baud: Specifies the baud rateof the serial port.
 * @retval None.
 */
void hsuart_baud_config(uint32_t baud)
{
    u32 baud_reg, sys_clk_val;

    //---------- hsuart baud and clock configuration ----------//
    sys_clk_val = clk_hsut0_clk_get(CLK_VALUE_MODE_FREQ);

    baud_reg = (sys_clk_val / baud) - 1;
    HSUART->baud = (uint32_t)((baud_reg << 16) | baud_reg);
}

/**
  * @brief  Enable or disable the specified HSUART interrupt.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrup_type: specifies the HSUART interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg HSUART_IT_RX_DMA_HF: RX DMA half-full interrupt enable bit.
  *             @arg HSUART_IT_TX: Transmit single or n bytes data finish interrupt enable bit.
  *             @arg HSUART_IT_RX: Receive single or n bytes data finish interrupt enable bit.
  * @param  state: the state of the HSUART peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void hsuart_pic_config(isr_t isr, int pr, HSUART_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_pending_bit = 0;
    uint32_t all_interrupt_type_mask = HSUART_IT_RX_DMA_HF | HSUART_IT_TX | HSUART_IT_RX;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & HSUART_IT_RX) {
        interrupt_pending_bit |= HSUT0CON_RXIE;
    }

    if (interrupt_type & HSUART_IT_TX) {
        interrupt_pending_bit |= HSUT0CON_TXIE;
    }

    if (interrupt_type & HSUART_IT_RX_DMA_HF) {
        interrupt_pending_bit |= HSUT0CON_RXHF_IE;
    }

    if (state != DISABLE) {
        sys_irq_init(IRQn_HSUART, pr, isr);
        HSUART->con |= interrupt_pending_bit;
    } else {
        HSUART->con &= ~interrupt_pending_bit;
        if (interrupt_type == all_interrupt_type_mask) {
            PICEN &= ~BIT(IRQn_HSUART);
        }
    }
}

/**
  * @brief  Check the specified HSUART flag is set or not.
  * @param  interrupt_type: specifies the flag to check.
  *         this parameter can be one of the following values:
  *             @arg HSUART_IT_RX_DMA_HF: RX DMA half-full interrupt pending bit.
  *             @arg HSUART_IT_RX_TMR_OV: RX timer overflow interrupt pending bit.
  *             @arg HSUART_IT_TX: Transmit single or n bytes data finish interrupt pending bit.
  *             @arg HSUART_IT_RX: Receive single or n bytes data finish interrupt pending bit.
  * @retval The state of uart_flag (SET or RESET).
  */
AT(.com_periph.hsuart.get_flag)
FLAG_STATE hsuart_get_flag(HSUART_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & HSUART_IT_RX) {
        interrupt_pending_bit |= HSUT0CON_RXPND;
    }

    if (interrupt_type & HSUART_IT_TX){
        interrupt_pending_bit |= HSUT0CON_TXPND;
    }

    if (interrupt_type & HSUART_IT_RX_TMR_OV){
        interrupt_pending_bit |= HSUT0CON_TMROV;
    }

    if (interrupt_type & HSUART_IT_RX_DMA_HF){
        interrupt_pending_bit |= HSUT0CON_RXHF_PND;
    }

    if ((HSUART->con & interrupt_pending_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the HSUART's pending flag.
  * @param  hsuart_flag: specifies the flag to check.
  *         this parameter can be one of the following values:
  *             @arg HSUART_IT_RX_DMA_HF: RX DMA half-full interrupt pending bit.
  *             @arg HSUART_IT_RX_TMR_OV: RX timer overflow interrupt pending bit.
  *             @arg HSUART_IT_TX: Transmit single or n bytes data finish interrupt pending bit.
  *             @arg HSUART_IT_RX: Receive single or n bytes data finish interrupt pending bit.
  * @retval None
  */
AT(.com_periph.hsuart.clear)
void hsuart_clear_flag(HSUART_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_pending_bit = 0;

    if (interrupt_type & HSUART_IT_RX) {
        interrupt_pending_bit |= HSUT0CPND_CRXPND;
    }

    if (interrupt_type & HSUART_IT_TX){
        interrupt_pending_bit |= HSUT0CPND_CTXPND;
    }

    if (interrupt_type & HSUART_IT_RX_TMR_OV){
        interrupt_pending_bit |= HSUT0CPND_CTMROV;
    }

    if (interrupt_type & HSUART_IT_RX_DMA_HF){
        interrupt_pending_bit |= HSUT0CPND_CRXHFPND;
    }

    HSUART->cpnd |= interrupt_pending_bit;
}

/**
 * @brief  Config idle timer that can trigger an interrupt when Rx idle.
 * @param  idle_time: The maximum number of idle clocks.
 * @param  state: the state of the HSUART peripheral.
*                 this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void hsuart_rx_idle_config(uint16_t idle_time, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        HSUART->tmr_cnt = idle_time;
        HSUART->con |= HSUT0CON_HSUTTMREN;
    } else {
        HSUART->con &= ~HSUT0CON_HSUTTMREN;
    }
}

/**
  * @brief  Start send or receive dma. When receiving data of indefinite
  *         length, need to enable the idle timer.
  * @param  rec_tra_sel: Tx or Rx to configure.
  *         this parameter can be: (HSUT_TRANSMIT or HSUT_RECEIVE)
  * @param  addr: DMA start address.
  * @param  len: DMA transmit len.
  * @retval None
  */
AT(.com_periph.hsuart.dma)
void hsuart_dma_start(HSUT_REC_TRA_CMD rec_tra_sel, uint32_t addr, uint16_t len)
{
    if (rec_tra_sel == HSUT_RECEIVE) {
        HSUART->rx_adr = addr;
        HSUART->rx_cnt = (uint32_t)len;
    } else if (rec_tra_sel == HSUT_TRANSMIT) {
        HSUART->tx_adr = addr;
        HSUART->tx_cnt = (uint32_t)len;
    }
}

/**
  * @brief  stop send or receive dma.
  * @param  rec_tra_sel: Tx or Rx to configure.
  *         this parameter can be: (HSUT_TRANSMIT or HSUT_RECEIVE)
  * @retval None
  */
AT(.com_periph.hsuart.stop)
void hsuart_dma_stop(HSUT_REC_TRA_CMD rec_tra_sel)
{
    if (rec_tra_sel == HSUT_RECEIVE) {
        HSUART->rx_cnt = 0;
    } else if (rec_tra_sel == HSUT_TRANSMIT) {
        HSUART->tx_cnt = 0;
    }
}
