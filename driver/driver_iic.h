/*
 * @File name    : driver_iic.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file contains all the functions prototypes for the IIC library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_IIC_H
#define _DRIVER_IIC_H

#include "driver_com.h"

/**
 * @brief IIC IT and Flag Definition
 */
typedef enum {
    IIC_IT_DONE             = 0x01,
    IIC_IT_ACK              = 0x02,
} IIC_IT_TYPEDEF;

/**
 * @brief IIC Clock Source Enumeration
 */
typedef enum {
    IIC_CLK_RC2M            = 0x00000000,
    IIC_CLK_X24MDIV8        = 0x00800000,
} IIC_CLK_SOURCE_TYPEDEF;

/**
 * @brief IIC Role Mode Enumeration
 */
typedef enum {
    IIC_ROLE_MODE_MASTER                    = 0,
    IIC_ROLE_MODE_SLAVE                     = 1,
} IIC_ROLE_MODE_TYPEDEF;

/**
 * @brief IIC Data Sample Edge Selection In Slave Mode.
 */
typedef enum {
    IIC_SMP_SEL_FALLING                     = 0,
    IIC_SMP_SEL_RISING                      = 1,
} IIC_SMP_SEL_TYPEDEF;

/**
 * @brief IIC Transmit Direction Enumeration
 */
typedef enum {
    IIC_SEND_MODE           = 1,
    IIC_RECEIVE_MODE        = 2,
} IIC_TRANSMIT_MODE;

/**
 * @brief IIC Init Structure
 */
typedef struct {
    uint8_t                 scl_pose_div;
    uint8_t                 sda_hold_cnt;
    IIC_CLK_SOURCE_TYPEDEF  clk_source;
    IIC_ROLE_MODE_TYPEDEF   mode_sel;
} iic_init_typedef;


#define iic_slave_status_clear() {IICSSTS |= IICSSTS_START_CLR | IICSSTS_RESTART_CLR | IICSSTS_STOP_CLR | IICSSTS_RXVLD_SET;}

//--------------- Function used to configure iic or transmit data ---------------//
void iic_init(iic_init_typedef *iic_init_struct);
void iic_deinit(void);
void iic_cmd(FUNCTIONAL_STATE state);
STATUS_STATE iic_send_data(uint8_t dev_addr, uint16_t reg_addr, uint32_t data, uint8_t len, uint8_t timeout_ms);
STATUS_STATE iic_receive_data(uint8_t dev_addr, uint16_t reg_addr, uint32_t *data, uint8_t len, uint8_t timeout_ms);
STATUS_STATE iic_dma_start(IIC_TRANSMIT_MODE transmit_mode, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t len);
//------------------- Function used to configure interrupt -------------------//
void iic_pic_config(isr_t isr, int pr, IIC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
FLAG_STATE iic_get_flag(IIC_IT_TYPEDEF interrupt_type);
void iic_clear_all_flag(void);
void iic_send_nack_en(FUNCTIONAL_STATE state);
void iic_smp_sel(IIC_SMP_SEL_TYPEDEF edge);
void iic_slave_addr_set(uint8_t dev_addr);
uint32_t iic_slave_status_get(void);
uint8_t iic_slave_data_len_get(void);
void iic_clear_done_flag(void);
void iic_clear_slave_tx_status(void);
void iic_clear_slave_rx_status(void);
uint32_t iic_slave_data_get(void);
void iic_slave_data_send(uint32_t data);
FLAG_STATE iic_slave_is_rx_status(uint32_t slave_sta);
FLAG_STATE iic_slave_is_tx_status(uint32_t slave_sta);
FLAG_STATE iic_slave_rx_stop_sign(void);


#endif // _DRIVER_IIC_H
