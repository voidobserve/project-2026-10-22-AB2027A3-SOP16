/*
 * @File name    : driver_saradc.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the SARADC peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_saradc.h"


/**
 * @brief  This function is used to resolve bugs in the current version.
 *         we will delete this function when the bugs are fixed in the future.
 * @param  channel: the adc channel that need to patch.
 * @retval None
 */
static void saradc_patch(uint32_t channel)
{
    /* Disable auto_analog_io */
    SARADC->con &= ~BIT(18);
    /* Set GPIO to Analog */
    GPIOA_REG->de &= ~((channel & 0x3f) << 2);
    GPIOB_REG->de &= ~((channel & 0xc0) >> 6);
    GPIOB_REG->de &= ~((channel & 0x0300) >> 5);
    GPIOB_REG->de &= ~((channel & 0x0400) >> 2);
}


/**
 * @brief  Initializes the saradc peripheral according to the specified
 *         parameters in the saradc_base_init_struct.
 * @param  saradc_base_init_struct: pointer to a sadc_init_typedef structure that
 *         contains the configuration information for the specified SARADC peripheral.
 * @retval None
 */
void saradc_init(saradc_base_init_typedef* saradc_base_init_struct)
{
    u16 baud_reg;
    u32 saradc_clk;

    saradc_clk = clk_saradc_clk_get(CLK_VALUE_MODE_FREQ);

    /* Configure Baud Register */
    baud_reg = (saradc_clk / (2 * saradc_base_init_struct->baud - 1)) & 0x3ff;
    SARADC->baud = baud_reg;

    /* Configure SARADC Mode */
    SARADC->con &= ~SADCCON_ADCMODE;
    SARADC->con |=  saradc_base_init_struct->mode & SADCCON_ADCMODE;

    /* Configure SARADC auto enable analog enable bit */
    SARADC->con &= ~SADCCON_ADCAEN;
    SARADC->con |= saradc_base_init_struct->auto_analog & SADCCON_ADCAEN;

    /* Configure SARADC auto enable analog IO enable bit */
    SARADC->con &= ~SADCCON_ADCANGIOEN;
    SARADC->con |= saradc_base_init_struct->auto_analog_io & SADCCON_ADCANGIOEN;
}

/**
 * @brief  Config saradc specified channel.
 * @param  saradc_channel_init_struct: the struct including the info of specified channel.
 * @retval None
 */
void saradc_channel_init(saradc_channel_init_typedef *saradc_channel_init_struct)
{
    u8 i = 0;

    /* Battery channel config */
    if (saradc_channel_init_struct->channel & ADC_CHANNEL_VBAT) {
        PWRCON0  |= (uint32_t)(1 << 10);        // VBATDIV2 ADC enable
    }

    /* Bandgap channel config */
    if (saradc_channel_init_struct->channel & ADC_CHANNEL_BG) {
        PWRCON0  |= (uint32_t)(1 << 22);        // BG output ADC enable
    }

    /* PWRKEY channel config */
    if (saradc_channel_init_struct->channel & ADC_CHANNEL_WK0) {
        RTCCON1 |= (uint32_t)(1 << 5);          // WK0 output ADC enable
        if (RTCCON1 & BIT(1)) {                 // Sure this bit is enable,
            RTCCON11 |= BIT(4);                 // and recommends it is enabled normally.
        }
    }

    /* VDDRTC channel config */
    if (saradc_channel_init_struct->channel & ADC_CHANNEL_VRTC) {
        RTCCON1 |= (uint32_t)(1 << 7);          // VRTC output ADC enable
    }

    /* VUSB channel config */
    if (saradc_channel_init_struct->channel & ADC_CHANNEL_VUSB) {
        RTCCON8 |= (uint32_t)(1 << 15);         // VUSB output ADC enable
        PWRCON2 &= ~(uint32_t)(0x03 << 5);      // TSEN disable
    /* TSEN channel config */
    } else if (saradc_channel_init_struct->channel & ADC_CHANNEL_TSEN) {
        RTCCON8 &= ~(uint32_t)(1 << 15);
        PWRCON2 |= (uint32_t)(0x03 << 5);
        saradc_channel_init_struct->channel &= ~ADC_CHANNEL_TSEN;
        saradc_channel_init_struct->channel |= ADC_CHANNEL_VUSB;
    }

    /* Configure each channel */
    for (i = 0; i < 16; i++) {
        if (saradc_channel_init_struct->channel & (0x01 << i)) {
            SARADC->cst &= ~(0x03 << (i * 2));
            SARADC->cst |= (saradc_channel_init_struct->setup_time) << (i * 2);

            SARADC->con &= ~(0x01 << i);
            SARADC->con |= (saradc_channel_init_struct->pullup_en & 0x01) << i;

            if (saradc_channel_init_struct->interrupt_en != DISABLE) {
                SARADC->ch_int |= (0x01 << i);
            } else {
                SARADC->ch_int &= ~(0x01 << i);
            }
        }
    }

    saradc_patch(saradc_channel_init_struct->channel);
}

/**
 * @brief  Deinit all channel config.
 * @retval the enumeration of channels that has enable.
 */
uint8_t saradc_deinit(void)
{
    uint8_t saradc_analog_bits = 0;

    /* Battery channel config */
    if (PWRCON0 & (uint32_t)(1 << 10)) {
        PWRCON0 &= ~((uint32_t)(1 << 10));
        saradc_analog_bits |= SARADC_ANALOG_BIT_VBAT;
    }

    /* Bandgap channel config */
    if (PWRCON0 & (uint32_t)(1 << 22)) {
        PWRCON0 &= ~((uint32_t)(1 << 22));
        saradc_analog_bits |= SARADC_ANALOG_BIT_BG;
    }

    /* PWRKEY channel config */
    if (RTCCON1 & (uint32_t)(1 << 5)) {
        RTCCON1 &= ~((uint32_t)(1 << 5));
        saradc_analog_bits |= SARADC_ANALOG_BIT_WK0;
    }

    /* VDDRTC channel config */
    if (RTCCON1 & (uint32_t)(1 << 7)) {
        RTCCON1 &= ~((uint32_t)(1 << 7));
        saradc_analog_bits |= SARADC_ANALOG_BIT_VRTC;
    }

    /* VUSB channel config */
    if (RTCCON8 & (uint32_t)(1 << 15)) {
        RTCCON8 &= ~((uint32_t)(1 << 15));
        saradc_analog_bits |= SARADC_ANALOG_BIT_VUSB;
    }

    /* TSEN channel config */
    if (PWRCON2 & (0x03 << 5)) {
        PWRCON2 &= ~(0x03 << 5);
        saradc_analog_bits |= ADC_CHANNEL_TSEN;
    }

    /* Clear saradc register config */
    SARADC->con = 0;
    SARADC->ch = 0;

    /* Disable SARADC clock */
    clk_gate0_cmd(CLK_GATE0_SARADC, CLK_DIS);

    return saradc_analog_bits;
}

/**
  * @brief  Enable or disable the specified SARADC peripheral.
  * @param  state: state of the SARADC peripheral.
            This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void saradc_cmd(FUNCTIONAL_STATE state)
{
    if(state) {
        SARADC->con |= SADCCON_ADCEN;
    } else {
        SARADC->con &= (uint32_t)~SADCCON_ADCEN;
    }
}

/**
  * @brief  SARADC start kick.
  * @param  adc_ch: the adc channels.
  * @retval None
  */
AT(.com_periph.saradc.kick)
void saradc_kick_start(uint32_t adc_ch)
{
    if (!adc_ch) {
        return;
    }

    if (adc_ch & ADC_CHANNEL_TSEN) {
        adc_ch &= ~ADC_CHANNEL_TSEN;
        adc_ch |= ADC_CHANNEL_VUSB;
    }

    while(SARADC->ch & 0xffff);

    SARADC->ch = (adc_ch & 0xffff);
}

/**
  * @brief  Set the SARADC baud register value.
  * @param  baud: specifies the baud register new value.
  * @retval None
  */
void saradc_set_baud(uint32_t baud)
{
    u16 baud_reg;
    u32 saradc_clk;

    saradc_clk = clk_saradc_clk_get(CLK_VALUE_MODE_FREQ);

    /* Configure Baud Register */
    baud_reg = (saradc_clk / (2 * baud - 1)) & 0x3ff;
    SARADC->baud = baud_reg;
}

/**
 * @brief  Enable or disable the specified SARADC interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  interrupt_type: specifies the SARADC interrupt sources to be enable or disable.
 *         this parameter can be one of the following values:
 *             @arg SARADC_IT_FINISHED: All channel finished flag.
 * @param  state: state of the SARADC peripheral.
 *         this parameter can be: EANBLE or DISABLE.
 * @retval None
 */
void saradc_pic_config(isr_t isr, int pr, SARADC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    uint32_t interrupt_bit = 0;

    /* Get interrupt ctrl bit */
    if (interrupt_type & SARADC_IT_FINISHED) {
        interrupt_bit |= SADCCON_ADCIE;
    }

    if(state != DISABLE) {
        SARADC->con |= interrupt_bit;
        sys_irq_init(IRQn_SARADC, pr, isr);
    } else {
        SARADC->con &= ~interrupt_bit;
        PICEN &= ~BIT(IRQn_SARADC);
    }
}

/**
 * @brief  Get the SARADC interrupt pending.
 * @param  interrupt_type: specifies the IT to get.
 *         this parameter can be on of the following values:
 *             @arg SARADC_IT_FINISHED: SARADC all channel finished interrupt.
 * @retval The state of interrupt_type (SET or RESET).
 */
AT(.com_periph.saradc.get)
FLAG_STATE saradc_get_flag(SARADC_IT_TYPEDEF interrupt_type)
{
    uint32_t flag_bit = 0;

    if (interrupt_type & SARADC_IT_FINISHED) {
        flag_bit |= (0x01 << 16);
    }

    if ((SARADC->ch & flag_bit) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Get the SARADC specifies channel finished statue.
  * @param  adc_ch: the channel that wants to query.
  * @retval The state of adc_ch finish (SET or RESET).
  */
AT(.com_periph.saradc.get_channel)
FLAG_STATE saradc_get_channel_flag(uint32_t adc_ch)
{
    if (adc_ch & ADC_CHANNEL_TSEN) {
        adc_ch &= ~ADC_CHANNEL_TSEN;
        adc_ch |= ADC_CHANNEL_VUSB;
    }

    if (((SARADC->ch) & adc_ch) != RESET) {
        return RESET;
    } else {
        return SET;
    }
}

/**
 * @brief  Clear the SARADC interrupt pending.
 * @param  interrupt_type: specifies the IT to clear.
 *         this parameter can be on of the following values:
 *             @arg SARADC_IT_FINISHED: SARADC all channel finished interrupt.
 * @retval None
 */
AT(.com_periph.saradc.clear)
void saradc_clear_flag(SARADC_IT_TYPEDEF interrupt_type)
{
    uint32_t flag_bit = 0;

    if (interrupt_type & SARADC_IT_FINISHED) {
        flag_bit |= (0x01 << 16);
    }

    SARADC->ch |= flag_bit;
}

/**
  * @brief  Get the SARADC specifies channel value.
  * @param  adc_chx: the channel that wants to query.
  * @retval saradc data.
  */
AT(.com_periph.saradc.get_data)
uint32_t saradc_get_data(uint32_t adc_chx)
{
    u8 i = 0;

    while (i < 17) {
        if (adc_chx & (0x01 << i)) {
            break;
        }
        i++;
    }

    if ((0x01 << i) == ADC_CHANNEL_VBAT) {
        return SARADC->dat[i] * pmu_get_vbatdet() / 1000;
    } else if ((0x01 << i) == ADC_CHANNEL_VUSB) {
        return SARADC->dat[i] * 3;
    } else if ((0x01 << i) == ADC_CHANNEL_TSEN) {
        return SARADC->dat[15];
    } else if (i < 16) {
        return SARADC->dat[i];
    } else {
        return 0;
    }
}
