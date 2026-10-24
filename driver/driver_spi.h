/*
 * @File name    : driver_spi.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file contains all the functions prototypes for the SPI library.
 * 
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_SPI_H
#define _DRIVER_SPI_H

#include "driver_com.h"

//--------------- SPI[0..1] Flag Definition ---------------//
#define SPI_FLAG_PENDING                    ((uint32_t)0x00010000)

/**
 * @brief SPI Role Mode Enumeration
 */
typedef enum {
    SPI_ROLE_MODE_MASTER                    = 0x00000000,
    SPI_ROLE_MODE_SLAVE                     = 0x00000002,
} SPI_ROLE_MODE_TYPEDEF;

/**
 * @brief SPI Bus Mode Enumeration
 */
typedef enum {
    SPI_3WIRE_MODE                          = 0x00000000,
    SPI_2WIRE_MODE                          = 0x00000004,
    SPI_2BIT_BID_MODE                       = 0x00000008,
    SPI_4BIT_BID_MODE                       = 0x0000000c,
} SPI_BUS_MODE_TYPEDEF;

/**
 * @brief SPI Output Data Edge Enumeration
 */
typedef enum {
    SPI_OUTPUT_DATA_EDGE_FALLING            = 0x00000000,
    SPI_OUTPUT_DATA_EDGE_RISING             = 0x00000040,
} SPI_OUTPUT_DATA_EDGE_TYPEDEF;

/**
 * @brief SPI Sample Clock Enumeration
 */
typedef enum {
    SPI_SAMPLE_AND_OUTPUT_CLOCK_DIFF        = 0x00000000,
    SPI_SAMPLE_AND_OUTPUT_CLOCK_SAME        = 0x00000400,
} SPI_SAMPLE_AND_OUTPUT_CLOCK_TYPEDEF;

/**
 * @brief SPI Direction
 */
typedef enum {
    SPI_DIR_TX                              = 0x00,
    SPI_DIR_RX                              = 0x01,
} SPI_DIR_TYPEDEF;

/**
 * @brief SPI Clock State When Idle
 */
typedef enum {
    SPI_CLOCK_IDLE_LOW                      = 0x00000000,
    SPI_CLOCK_IDLE_HIGH                     = 0x00000020,
} SPI_CLOCK_IDLE_STATE_TYPEDEF;

/* SPI Init Structure */
typedef struct{
    uint32_t baud;
    SPI_ROLE_MODE_TYPEDEF role_mode;
    SPI_BUS_MODE_TYPEDEF bus_mode;
    SPI_OUTPUT_DATA_EDGE_TYPEDEF output_data_edge;
    SPI_SAMPLE_AND_OUTPUT_CLOCK_TYPEDEF sample_clk_edge;
    SPI_CLOCK_IDLE_STATE_TYPEDEF clkids;    //if SPI1BAUD is equal to 0, then CLKIDS forces 0 by designed
} spi_init_typedef;

void spi_init(spi_typedef* spix, spi_init_typedef* spi_init_struct);
void spi_deinit(spi_typedef* spix);
void spi_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_dir_sel(spi_typedef* spix, SPI_DIR_TYPEDEF direction);
void spi_it_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_lfsr_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_multi_bit_bus_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_hold_rx_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_hold_tx_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
void spi_soft_hold_cmd(spi_typedef* spix, FUNCTIONAL_STATE state);
FLAG_STATE spi_get_flag(spi_typedef* spix, uint32_t spi_flag);
void spi_clear_flag(spi_typedef* spix, uint32_t spi_flag);
void spi_set_dma_cnt(spi_typedef* spix, uint32_t cnt);
void spi_set_dma_addr(spi_typedef* spix, uint32_t addr);
uint8_t spi_receive_data(spi_typedef* spix);
void spi_send_data(spi_typedef* spix, uint8_t data);
void spi_pic_config(spi_typedef* spix, isr_t isr, int pr, FUNCTIONAL_STATE state);

#endif // _DRIVER_SPI_H
