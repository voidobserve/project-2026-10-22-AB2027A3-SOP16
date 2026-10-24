/*
 * @File name    : driver_iic.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-14
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the IIC peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_iic.h"

/**
  * @brief  Initializes the IIC peripheral according to the specified
  *         parameters in the iic_init_typedef.
  * @param  iic_init_struct: pointer to a iic_init_typedef structure that
  *         contains the configuration information for the specified IIC peripheral.
  * @retval None
  */
void iic_init(iic_init_typedef *iic_init_struct)
{
    /*--- Configure the iic clock source ---*/
    CLKCON1 &= ~(1 << 23);
    CLKCON1 |= iic_init_struct->clk_source;

    /*--- Configure the iic SCL pose div conter ---*/
    IIC->con0 &= ~IICCON0_POSDIV;
    IIC->con0 |= ((iic_init_struct->scl_pose_div) << 4) & IICCON0_POSDIV;

    /*--- Configure the iic SDA hold cnt when SCL falling ---*/
    IIC->con0 &= ~IICCON0_HOLDCNT;
    IIC->con0 |= ((iic_init_struct->sda_hold_cnt) << 2) & IICCON0_HOLDCNT;

    /*--- Configure the iic controller work mode ---*/
    IIC->con0 &= ~IICCON0_MODE;
    IIC->con0 |= ((iic_init_struct->mode_sel) << 12) & IICCON0_MODE;

    /*--- Enable model function ---*/
    RSTCON0 |= (1 << 3);
}

/**
 * @brief  De-initialize the specified IIC peripheral.
 * @retval None
 */
void iic_deinit(void)
{
    IIC->con0 = 0;
    clk_gate2_cmd(CLK_GATE2_IIC, CLK_DIS);
}

/**
  * @brief  Enable or disable the specified IIC peripheral.
  * @param  state: state of the IIC peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void iic_cmd(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        IIC->con0 |= IICCON0_IICEN;
    } else {
        IIC->con0 &= ~IICCON0_IICEN;
    }
}

/**
  * @brief  Initializes the IIC control regisger according to param.
  * @param  transmit_mode: select the iic transmit direction.
  *         This parameter can be: IIC_SEND_MODE or IIC_RECEIVE_MODE.
  * @param  dev_addr: the slave device address.
  * @param  reg_addr: the address of the slave device register.
  * @param  len: the numbers of bytes of data.
  * @retval None
  */
static void iic_transmit_config(IIC_TRANSMIT_MODE transmit_mode, uint8_t dev_addr, uint16_t reg_addr, uint8_t len)
{
    IIC->con1 = 0;
    IIC->con1 |= IICCON1_STOP_EN;

    IIC->con1 |= IICCON1_ADR0_EN;
    IIC->con1 |= IICCON1_CTL0_EN;
    IIC->con1 |= IICCON1_START0_EN;
    IIC->con1 |= len & IICCON1_DATA_CNT;
    if (reg_addr & 0xff00) {
        IIC->con1 |= IICCON1_ADR1_EN;
    }

    IIC->cmd_addr = (dev_addr & 0xfe) | (reg_addr << 8);

    if (transmit_mode == IIC_SEND_MODE) {
        IIC->con1 |= IICCON1_WDAT_EN;
    } else if (transmit_mode == IIC_RECEIVE_MODE) {
        IIC->con1 |= IICCON1_TXNAK_EN;
        IIC->con1 |= IICCON1_RDAT_EN;
        IIC->con1 |= IICCON1_CTL1_EN;
        IIC->con1 |= IICCON1_START1_EN;
        IIC->cmd_addr |= ((dev_addr | 0x01) << 24);
    }
    IIC->dma_cnt &= ~(0x01 << 0);
}

/**
  * @brief  Send data by iic bus, this function will block program.
  * @param  dev_addr: the slave device address.
  * @param  reg_addr: the address of the slave device register.
  * @param  data: the data needed to transmit.
  * @param  len: the numbers of bytes of data. A maximum of 4 bytes cna be
  *              transmitted at a time. So len should less than or equal to 4.
  * @param  timeout_ms: timeout period for waiting for a result.
  * @retval The status of result. It can be (FAILED or SUCCESS).
  */
STATUS_STATE iic_send_data(uint8_t dev_addr, uint16_t reg_addr, uint32_t data, uint8_t len, uint8_t timeout_ms)
{
    iic_transmit_config(IIC_SEND_MODE, dev_addr, reg_addr, len);

    IIC->data = data;

    IIC->con0 |= IICCON0_KS;

    if (timeout_ms == 0) {
        while ((IIC->con0 & IICCON0_DONE) == RESET);
        IIC->con0 |= IICCON0_CLR_DONE;
        return SUCCESS;
    } else {
        while (timeout_ms) {
            if ((IIC->con0 & IICCON0_DONE) != RESET) {
                IIC->con0 |= IICCON0_CLR_DONE;
                return SUCCESS;
            }
            timeout_ms--;
            delay_ms(1);
        }
    }

    return FAILED;
}

AT(.com_periph.iic.dma)
STATUS_STATE iic_dma_start(IIC_TRANSMIT_MODE transmit_mode, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t len)
{
    if (!len) {
        return FAILED;
    }

    IIC->con1 = 0;

    IIC->con1 |= IICCON1_CTL0_EN;
    IIC->con1 |= IICCON1_START0_EN;
    IIC->con1 |= IICCON1_ADR0_EN;

    if (reg_addr & 0xff00) {
        IIC->con1 |= IICCON1_ADR1_EN;
    }

    IIC->con1 |= IICCON1_STOP_EN;
    IIC->con1 |= len & IICCON1_DATA_CNT;

    IIC->cmd_addr = (dev_addr & 0xfe) | (reg_addr << 8);

    /* DMA Config */
    IIC->dma_addr = ALIGN4_HI((u32)buf);
    IIC->dma_cnt  = ((len - 1) << 16);
    IIC->dma_cnt &= ~(0x01 << 1);            // dma enable
    IIC->dma_cnt |= (0x01 << 0);            // dma enable

#if 1
    if (transmit_mode == IIC_SEND_MODE) {
        /* Write Mode Config */
        IIC->con1 |= IICCON1_WDAT_EN;
        IIC->dma_cnt |= (0x01 << 1);        //dma read enable
    } else if (transmit_mode == IIC_RECEIVE_MODE) {
        /* Read Mode Config */
        IIC->con1 |= IICCON1_CTL1_EN;
        IIC->con1 |= IICCON1_RDAT_EN;
        IIC->con1 |= IICCON1_START1_EN;
        IIC->con1 |= IICCON1_TXNAK_EN;
        IIC->cmd_addr |= (dev_addr | 0x01) << 24;
    }
#endif
    IIC->con0 |= IICCON0_KS;

    return SUCCESS;
}

/**
  * @brief  Receive data by iic bus, this function will block program.
  * @param  dev_addr: the slave device address.
  * @param  reg_addr: the address of the slave device.
  * @param  len: the numbers of bytes of data.
  * @param  timeout_ms: timeout period for waiting for a result.
  * @retval The status of result. It can be (FAILED or SUCCESS).
  */
STATUS_STATE iic_receive_data(uint8_t dev_addr, uint16_t reg_addr, uint32_t *data, uint8_t len, uint8_t timeout_ms)
{
    iic_transmit_config(IIC_RECEIVE_MODE, dev_addr, reg_addr, len);

    IIC->con0 |= IICCON0_KS;
    if (timeout_ms == 0) {
        while ((IIC->con0 & IICCON0_DONE) == RESET);
        IIC->con0 |= IICCON0_CLR_DONE;
        *data = IIC->data;
        return SUCCESS;
    } else {
        while (timeout_ms) {
            if ((IIC->con0 & IICCON0_DONE) != RESET) {
                IIC->con0 |= IICCON0_CLR_DONE;
                *data = IIC->data;
                return SUCCESS;
            }
            timeout_ms--;
            delay_ms(1);
        }
    }

    return FAILED;
}

/**
  * @brief  Enable or disable the specified IIC interrupt.
  * @param  isr: Function to be executed for service interruption.
  * @param  pr: Priority of service interruption.
  * @param  interrupt_type: specifies the IIC interrupt sources to be enable or disable.
  *         this parameter can be one of the following values:
  *             @arg IIC_IT_DONE: IIC transmit finished interrupt.
  * @param  state: the state of the IIC peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void iic_pic_config(isr_t isr, int pr, IIC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_bit = 0;
    uint32_t all_interrupt_type_mask = IIC_IT_DONE;

    if (interrupt_type == 0) {
        return;
    }

    if (interrupt_type & IIC_IT_DONE) {
        interrupt_bit |= IICCON0_INTEN;
    }

    if (state != DISABLE) {
        IIC->con0 |= interrupt_bit;
        sys_irq_init(IRQn_IIC, pr, isr);
    } else {
        IIC->con0 &= ~(uint32_t)interrupt_bit;
        if (interrupt_type == all_interrupt_type_mask) {
            PICEN &= ~BIT(IRQn_IIC);
        }
    }
}

/**
  * @brief  Get the IIC interrupt pending.
  * @param  interrupt_type: specifies the flag to get.
  *         this parameter can be one of the following values:
  *             @arg IIC_IT_DONE: IIC transmit finished flag.
  *             @arg IIC_IT_ACK: IIC receive slave device ack flag.
  * @retval The state of iic_flag (SET or RESET).
  */
AT(.com_periph.iic.get)
FLAG_STATE iic_get_flag(IIC_IT_TYPEDEF interrupt_type)
{
    uint32_t interrupt_bit = 0;

    if (interrupt_type & IIC_IT_DONE) {
        interrupt_bit |= IICCON0_DONE;
    }

    if (interrupt_type & IIC_IT_ACK) {
        interrupt_bit |= IICCON0_ACKSTATUS;
    }

    if ((IIC->con0 & interrupt_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear all status.
  * @retval None
  */
AT(.com_periph.iic.clear)
void iic_clear_all_flag(void)
{
    IIC->con0 |= IICCON0_CLR_ALL;
}

/**
  * @brief  Send NACK to the master when in slave mode.
  * @retval None
  */
AT(.com_periph.iic.nack_en)
void iic_send_nack_en(FUNCTIONAL_STATE state)
{
    if (state != DISABLE) {
        IIC->con0 |= IICCON0_RX_NACK_EN;
    } else {
        IIC->con0 &= ~IICCON0_RX_NACK_EN;
    }
}

/**
  * @brief  Data sample edge selection in slave mode.
  * @retval None
  */
AT(.com_periph.iic.smp_sel)
void iic_smp_sel(IIC_SMP_SEL_TYPEDEF edge)
{
    if (edge != IIC_SMP_SEL_FALLING) {
        IIC->con0 |= IICCON0_SMP_SEL;
    } else {
        IIC->con0 &= ~IICCON0_SMP_SEL;
    }
}

/**
  * @brief  When the device is a slave, set the slave device address.
  * @param  dev_addr: iic slave device address.
  * @retval None
  */

void iic_slave_addr_set(uint8_t dev_addr)
{
    IIC->cmd_addr= dev_addr;
}

/**
  * @brief  Get IIC slave device status.
  * @retval None
  */
AT(.com_periph.iic.slave_status_get)
uint32_t iic_slave_status_get(void)
{
   return IIC->slave_sta;
}

/**
  * @brief  Get IIC slave device data length.
  * @retval Data length
  */
AT(.com_periph.iic.slave_data_len_get)
uint8_t iic_slave_data_len_get(void)
{
   return (IIC->slave_sta>>8) & 0x07;
}
/**
  * @brief  Get IIC slave device data length.
  * @retval Data length
  */
AT(.com_periph.iic.slave_data_get)
uint32_t iic_slave_data_get(void)
{
   return IIC->data;
}

/**
  * @brief  IIC slave device send data.
  * @param  data: iic slave device send data value.
  * @retval None
  */
AT(.com_periph.iic.slave_data_send)
void  iic_slave_data_send(uint32_t data)
{
   IIC->data=data;
   IIC->con1 |=4;
}
/**
  * @brief  Clear iic device done flag.
  * @retval None
  */
AT(.com_periph.iic.clear_done_flag)
void iic_clear_done_flag(void)
{
    IIC->con0 |= IICCON0_CLR_DONE;
}
/**
  * @brief  Clear iic the START flag,RESTART flag,STOP flag and TXVLD flag.
  * @retval None
*/

AT(.com_periph.iic.clear_slave_tx_status)
void iic_clear_slave_tx_status(void)
{
   IIC->slave_sta |= IICSSTS_START_CLR | IICSSTS_RESTART_CLR | IICSSTS_STOP_CLR | IICSSTS_TXVLD_SET;
}
/**
  * @brief  Clear iic the START flag,RESTART flag,STOP flag and RXVLD flag.
  * @retval None
*/

AT(.com_periph.iic.clear_slave_rx_status)
void iic_clear_slave_rx_status(void)
{
   IIC->slave_sta |= IICSSTS_START_CLR | IICSSTS_RESTART_CLR | IICSSTS_STOP_CLR | IICSSTS_RXVLD_SET;
}

/**
  * @brief  Check iic slave is receiver mode.
  * The state of iic_flag (SET or RESET).
*/
AT(.com_periph.iic.slave_is_rx_status)
FLAG_STATE iic_slave_is_rx_status(uint32_t slave_sta)
{
   if(slave_sta & IICSSTS_SRX){
       return SET;
   }else{
       return RESET;
   }
}

/**
  * @brief  Check iic slave is transfer mode.
  * The state of iic_flag (SET or RESET).
*/
AT(.com_periph.iic.slave_is_tx_status)
FLAG_STATE iic_slave_is_tx_status(uint32_t slave_sta)
{
   if(slave_sta & IICSSTS_STX){
       return SET;
   }else{
       return RESET;
   }
}

/**
  * @brief  Check master send a STOP sign.
  * The state of iic_flag (SET or RESET).
*/

AT(.com_periph.iic.slave_rx_stop_sign)
FLAG_STATE iic_slave_rx_stop_sign(void)
{
   if(IIC->slave_sta & IICSSTS_STOP){
       return SET;
   }else{
       return RESET;
   }
}




