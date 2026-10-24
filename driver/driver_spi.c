/*
 * @File name    : driver_spi.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the SPI peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_spi.h"

/**
  * @brief  Initializes the spix peripheral according to the specified
  *         parameters in the spi_init_struct.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  spi_init_struct: pointer to a spi_init_typedef structure that
  *         contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void spi_init(spi_typedef* spix, spi_init_typedef* spi_init_struct)
{
    uint32_t temp = 0;

    /* SPI_CLK freq configuration */
    spix->baud = sys_clk_nhz_get() / spi_init_struct->baud - 1;

    temp |= (uint32_t)(spi_init_struct->bus_mode & SPIxCON_BUSMODE);
    temp |= (uint32_t)(spi_init_struct->role_mode & SPIxCON_SPISM);
    temp |= (uint32_t)(spi_init_struct->output_data_edge & SPIxCON_SMPS);
    temp |= (uint32_t)(spi_init_struct->sample_clk_edge & SPIxCON_SPIOSS);
    temp |= (uint32_t)(spi_init_struct->clkids & SPIxCON_CLKIDS);

    spix->con = temp;
}

/**
 * @brief  De-initialize the specified SPI peripheral.
 * @param  spix: where x can be (0..1) to select the SPI peripheral.
 * @retval None
 */
void spi_deinit(spi_typedef* spix)
{
    if (spix == SPI0) {
        /* Assert: This release disables disabling SPI0 peripherals using this API */
    } else if (spix == SPI1) {
        spix->con = 0;
        clk_gate1_cmd(CLK_GATE1_SPI1, CLK_DIS);
    }
}

/**
  * @brief  Enable or disable the specified SPI peripheral.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_SPIEN;
    } else {
        spix->con &= (uint32_t)~SPIxCON_SPIEN;
    }
}

/**
  * @brief  Select the specified SPI direction when in DMA mode or 2-wire mode.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  direction: transmit or receive.
            This parameter can be: SPI_DIR_TX or SPI_DIR_RX.
  * @retval None
  */
AT(.com_periph.spi)
void spi_dir_sel(spi_typedef* spix, SPI_DIR_TYPEDEF direction)
{
    if(direction == SPI_DIR_RX) {
        spix->con |= BIT(4);
    } else {
        spix->con &= ~BIT(4);
    }
}

/**
  * @brief  Enable or disable the specified SPI interrupts.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
AT(.com_periph.spi)
void spi_it_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_SPIIE;
    } else {
        spix->con &= (uint32_t)~SPIxCON_SPIIE;
    }
}

/**
  * @brief  Enable or disable the specified SPI LFSR.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_lfsr_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_SPILF_EN;
    } else {
        spix->con &= (uint32_t)~SPIxCON_SPILF_EN;
    }
}

/**
  * @brief  Enable or disable the specified SPI multiple bit bus.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_multi_bit_bus_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_SPIMBEN;
    } else {
        spix->con &= (uint32_t)~SPIxCON_SPIMBEN;
    }
}

/**
  * @brief  Enable or disable the specified SPI hold when bt rx.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_hold_rx_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_HOLDENRX;
    } else {
        spix->con &= (uint32_t)~SPIxCON_HOLDENRX;
    }
}

/**
  * @brief  Enable or disable the specified SPI hold when bt tx.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_hold_tx_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_HOLDENTX;
    } else {
        spix->con &= (uint32_t)~SPIxCON_HOLDENTX;
    }
}

/**
  * @brief  Enable or disable the specified SPI software hold.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  state: state of the SPIx interrupts.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_soft_hold_cmd(spi_typedef* spix, FUNCTIONAL_STATE state)
{
    if (state) {
        spix->con |= (uint32_t)SPIxCON_HOLDENSW;
    } else {
        spix->con &= (uint32_t)~SPIxCON_HOLDENSW;
    }
}

/**
  * @brief  Get the SPI pending.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  spi_flag: specifies the flag to set.
  *         this parameter can be one of the following values:
  *             @arg SPI_FLAG_PENDING: SPI(0..1) SPI pending.
  * @retval The state of spi_flag (SET or RESET).
  */
AT(.com_periph.spi)
FLAG_STATE spi_get_flag(spi_typedef* spix, uint32_t spi_flag)
{
    if ((spix->con & spi_flag) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the SPI pending.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  spi_flag: specifies the flag to clear.
  *         this parameter can be one of the following values:
  *             @arg SPI_FLAG_PENDING: SPI(0..1) SPI pending.
  * @retval None
  */
AT(.com_periph.spi)
void spi_clear_flag(spi_typedef* spix, uint32_t spi_flag)
{
    spix->cpnd |= (uint32_t)spi_flag;
}

/**
  * @brief  Set the SPI counter register value.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  cnt: specifies the counter register new value.
  * @retval None
  */
AT(.com_periph.spi)
void spi_set_dma_cnt(spi_typedef* spix, uint32_t cnt)
{
    spix->dma_cnt = cnt;
}

/**
  * @brief  Set the SPI DMA address register value.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  addr: specifies the SPI DMA address register new value.
  * @retval None
  */
AT(.com_periph.spi)
void spi_set_dma_addr(spi_typedef* spix, uint32_t addr)
{
    spix->dma_adr = (uint32_t)addr;
}

/**
  * @brief  Get the spi receive data.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @retval spi receive data.
  */
AT(.com_periph.spi)
uint8_t spi_receive_data(spi_typedef* spix)
{
    return (uint8_t)spix->buf;
}

/**
  * @brief  Send the spi receive data.
  * @param  spix: where x can be (0..1) to select the SPI peripheral.
  * @param  data: specifies the SPI data to be sent.
  * @retval None.
  */
AT(.com_periph.spi)
void spi_send_data(spi_typedef* spix, uint8_t data)
{
    spix->buf = data;
}

/**
  * @brief  Enable or disable the specified SPIx interrupt.
  * @param  uartx: where x can be (0,1) to select the SPI peripheral.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  state: the state of the SPIx peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void spi_pic_config(spi_typedef* spix, isr_t isr, int pr, FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        sys_irq_init(IRQn_SPIx, pr, isr);
        spix->con |= SPIxCON_IE;
    } else {
        spix->con &= ~SPIxCON_IE;
        PICEN &= ~BIT(IRQn_SPIx);
    }
}
