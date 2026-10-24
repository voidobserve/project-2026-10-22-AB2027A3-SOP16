/*
 * @File name    : driver_gpio.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-13
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the GPIO peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_gpio.h"

const gpio_edge_cap_typedef separate_wakeup_io[WAKEUP_IDX_MAX] = {
    /*edge is useless from PA7 to PB4 */
    [WAKEUP_IDX_PA7] =          {GPIO_EDGE_RISING,      GPIOA_REG,  GPIO_PIN_7},
    [WAKEUP_IDX_PB1] =          {GPIO_EDGE_RISING,      GPIOB_REG,  GPIO_PIN_1},
    [WAKEUP_IDX_PB2] =          {GPIO_EDGE_RISING,      GPIOB_REG,  GPIO_PIN_2},
    [WAKEUP_IDX_PB3] =          {GPIO_EDGE_RISING,      GPIOB_REG,  GPIO_PIN_3},
    [WAKEUP_IDX_PB4] =          {GPIO_EDGE_RISING,      GPIOB_REG,  GPIO_PIN_4},

    /*WK0 no used in this array*/
    [WAKEUP_IDX_WK0] =          {GPIO_EDGE_RISING,      NULL,       0},

    [WAKEUP_IDX_PORT_FALL] =    {GPIO_EDGE_FALLING,     NULL,       0},
    [WAKEUP_IDX_PORT_RISE] =    {GPIO_EDGE_RISING,      NULL,       0},
};

/**
  * @brief  Initializes the gpiox peripheral according to the specified
  *         parameters in the gpio_init_struct.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  gpio_init_struct: pointer to a gpio_init_typedef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void gpio_init(gpio_typedef* gpiox, gpio_init_typedef* gpio_init_struct)
{
    u32 reg;
    uint32_t pin_idx = 0, pin_bit_offset = 0x00;
    uint8_t i = 0x00;

    /*--- Configure the port pins ---*/
    for (pin_idx = 0; pin_idx < 16; pin_idx++) {
        pin_bit_offset = ((uint32_t)0x01) << pin_idx;
        if (gpio_init_struct->gpio_pin & pin_bit_offset) {
            /*--- Input or Output Configuration ---*/
            reg = gpiox->dir;
            reg &= ~pin_bit_offset;
            reg |= ((uint32_t)(gpio_init_struct->gpio_dir) << pin_idx);
            gpiox->dir = reg;

            /*--- Digital or Analog Configuration ---*/
            reg = gpiox->de;
            reg &= ~pin_bit_offset;
            reg |= ((uint32_t)(gpio_init_struct->gpio_mode) << pin_idx);
            gpiox->de = reg;

            /*--- Function Mapping Enable Configuration ---*/
            reg = gpiox->fen;
            reg &= ~pin_bit_offset;
            reg |= ((uint32_t)(gpio_init_struct->gpio_fen) << pin_idx);
            gpiox->fen = reg;

            /*--- GPIO Direction Select Configguratiopn ---*/
            reg = gpiox->fdir;
            reg &= ~pin_bit_offset;
            reg |= ((uint32_t)(gpio_init_struct->gpio_fdir) << pin_idx);
            gpiox->fdir = reg;

            /*--- Pull Up or Pull Down Configuration and clear driving cfg ---*/
            if (gpio_init_struct->gpio_dir == GPIO_DIR_INPUT) {
                gpiox->drv &= ~pin_bit_offset;
                for (i = 0; i < 6; i++) {
                    reg = gpiox->pupd[i];
                    reg &= ~pin_bit_offset;
                    if ((i + 1) == gpio_init_struct->gpio_pupd) {
                        reg |= pin_bit_offset;
                    }
                    gpiox->pupd[i] = reg;
                }
            /*--- Output driving Configuration and clear pupu cfg ---*/
            } else if (gpio_init_struct->gpio_dir == GPIO_DIR_OUTPUT) {
                for (i = 0; i < 6; i++) {
                    gpiox->pupd[i] &= ~pin_bit_offset;
                }
                if (gpio_init_struct->gpio_mode == GPIO_MODE_DIGITAL) {
                    if (GPIO_SUPPORT_STRONG_CURRENT(gpio_init_struct->gpio_drv, gpiox, pin_idx)) {
                        reg = gpiox->drv;
                        reg |= ((uint32_t)(gpio_init_struct->gpio_drv));
                        gpiox->drv = reg;
                    } else {
                        reg = gpiox->drv;
                        reg &= ~pin_bit_offset;
                        reg |= ((uint32_t)(gpio_init_struct->gpio_drv) << pin_idx);
                        gpiox->drv = reg;
                    }
                }
            }
        }
    }
}

/**
 * @brief  De-initialize the specified GPIO peripheral.
 * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
 * @param  gpio_pin: specifies the port bits to be written. This parameter
 *         can be any combination of GPIO_PIN_x where x can be (0..15).
 * @retval None
 */
uint32_t gpio_deinit(gpio_typedef *gpiox, uint16_t gpio_pin)
{
    u32 reg_original_sta;
    if (gpiox == GPIOB_REG) {
        gpio_pin &= 0x3ff;
    }

    reg_original_sta = gpiox->de;
    gpiox->de &= ~gpio_pin;

    return reg_original_sta;
}

/**
  * @brief  Sets the selected data port bits.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  gpio_pin: specifies the port bits to be written. This parameter
  *         can be any combination of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
AT(.com_periph.gpio.set)
void gpio_set_bits(gpio_typedef* gpiox, uint16_t gpio_pin)
{
    gpiox->io_set = gpio_pin;
}

/**
  * @brief  Clears the selected data port bits.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  gpio_pin: specifies the port bits to be written.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
AT(.com_periph.gpio.reset)
void gpio_reset_bits(gpio_typedef* gpiox, uint16_t gpio_pin)
{
    gpiox->io_clr = gpio_pin;
}

/**
  * @brief  Writes the specified GPIO output data port.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  port_val: specifies the port bits to be written. This parameter
  *         can be any combination of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
AT(.com_periph.gpio.write)
void gpio_write_data(gpio_typedef *gpiox, uint16_t port_val)
{
    gpiox->data = port_val;
}

/**
  * @brief  Toggles the selected output data port bits.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  gpio_pin: specifies the port bits to be written. This parameter
  *         can be any combination of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
AT(.com_periph.gpio.toggle)
void gpio_toggle_bits(gpio_typedef *gpiox, uint16_t gpio_pin)
{
    gpiox->data ^= gpio_pin;
}

/**
  * @brief  Reads the specified GPIO input data port.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @retval GPIO output data port value.
  */
AT(.com_periph.gpio.read_data)
uint16_t gpio_read_data(gpio_typedef* gpiox)
{
    return ((uint16_t)(gpiox->data));
}

/**
  * @brief  Reads the specified input port pin.
  * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
  * @param  gpio_pin: specifies the port bit to read.
  *         This parameter can be gpio_pin where x can be (0..15).
  * @retval The input port pin value.
  */
AT(.com_periph.gpio.read_bit)
uint8_t gpio_read_bit(gpio_typedef* gpiox, uint16_t gpio_pin)
{
    uint8_t bit_status;

    if ((gpiox->data & gpio_pin) != BIT_RESET) {
        bit_status = (uint8_t)SET;
    } else {
        bit_status = (uint8_t)RESET;
    }

    return bit_status;
}

/**
 * @brief  Configure GPIO function mapping.
 * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
 * @param  gpio_pin: specifies the port bit to read.
 *         This parameter can be gpio_pin where x can be (0..15).
 * @param  func_idx: Peripheral index that need to config mapping. The value of this param
 *         see enumeration "GPIO_CROSSBAR_PERIPHERAL_DEF" in the file "driver_gpio.h".
 * @retval None
 */
void gpio_func_mapping_config(gpio_typedef* gpiox, uint16_t gpio_pin, GPIO_CROSSBAR_PERIPHERAL_DEF func_idx)
{
    u8 i;
    u8 pin_idx;
    uint8_t register_idx, register_offset;
    u8 gpio_crossbar_idx_PA0 = 1;
    u8 gpio_crossbar_idx_PB0 = 17;

    /*--- Calculate the pin_dix for function mapping ---*/
    for (i = 0; i < 16; i++) {
        if ((1 << i) == gpio_pin) {
            break;
        }
    }
    if (gpiox == GPIOA_REG) {
        pin_idx = gpio_crossbar_idx_PA0 + i;
    } else {
        pin_idx = gpio_crossbar_idx_PB0 + i;
    }

    /*--- Check parameter validity ---*/
    if (func_idx >= GPIO_CROSSBAR_PERIPHERAL_MAX_IDX) {
        return;
    }
    if ((gpiox == GPIOB_REG) && (gpio_pin > GPIO_PIN_9)) {
        return;
    }

    /*--- function mapping config ---*/
    if (func_idx > GPIO_CROSSBAR_PERIPHERAL_INPUT_BASE) {
        register_idx = (func_idx - GPIO_CROSSBAR_PERIPHERAL_INPUT_BASE - 1) / 4;
        register_offset = (func_idx - GPIO_CROSSBAR_PERIPHERAL_INPUT_BASE - 1) % 4 * 8;

        /* Do nothing if the current peripheral is already mapped to the corresponding IO */
        if (((FUNCMAP->func_input_map[register_idx] >> register_offset) & 0x1f) == pin_idx) {
            return;
        }

        FUNCMAP->func_input_map[register_idx] |= (uint32_t)(0xff << register_offset);
        FUNCMAP->func_input_map[register_idx] |= (uint32_t)(pin_idx << register_offset);
    } else if (func_idx < GPIO_CROSSBAR_PERIPHERAL_INPUT_BASE) {
        register_idx = (pin_idx - 1) / 4;
        register_offset = (pin_idx - 1) % 4 * 8;

        /* Do nothing if the current peripheral is already mapped to the corresponding IO */
        if (((FUNCMAP->func_output_map[register_idx] >> register_offset) & 0x1f) == func_idx) {
            return;
        }

        FUNCMAP->func_output_map[register_idx] |= (uint32_t)(0xff << register_offset);
        FUNCMAP->func_output_map[register_idx] |= (uint32_t)(func_idx << register_offset);
    }
}

/**
 * @brief  Clear GPIO function mapping config.
 * @param  gpiox: where x can be (A.B) to select the GPIO peripheral.
 * @param  gpio_pin: specifies the port bit to read.
 *         This parameter can be gpio_pin where x can be (0..15).
 * @retval None
 */
void gpio_func_mapping_clear(gpio_typedef* gpiox, uint16_t gpio_pin)
{
    uint8_t register_idx, register_offset;
    uint8_t i, j;
    u8 pin_idx;
    u8 gpio_crossbar_idx_PA0 = 1;
    u8 gpio_crossbar_idx_PB0 = 17;

    /* Calculate the pin_dix for function mapping */
    for (i = 0; i < 16; i++) {
        if ((1 << i) == gpio_pin) {
            break;
        }
    }
    if (gpiox == GPIOA_REG) {
        pin_idx = gpio_crossbar_idx_PA0 + i;
    } else {
        pin_idx = gpio_crossbar_idx_PB0 + i;
    }

    /* Clear output mapping */
    register_idx = (pin_idx - 1) / 4;
    register_offset = (pin_idx - 1) % 4 * 8;
    FUNCMAP->func_output_map[register_idx] |= (uint32_t)(0xff << register_offset);

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 32; j += 8)
        if (((FUNCMAP->func_input_map[i] >> j) & 0x1f) == pin_idx) {
            FUNCMAP->func_input_map[i] |= (uint32_t)(0xff << j);
        }
    }
}

/**
 * @brief  WK0 IO level detect config.
 * @param  pull_up_en: Whether WK0 internal pull up enable.
 * @param  pull_down_en: Whether WK0 internal pull down enable.
 * @param  input_en: Whether WK0 input enable.
 * @retval None
 */
void wko_io_config(FUNCTIONAL_STATE pull_up_en, FUNCTIONAL_STATE pull_down_en, FUNCTIONAL_STATE input_en)
{
    if (pull_up_en) {
        RTCCON1 |= (uint32_t)(1 << 4);
    } else {
        RTCCON1 &= ~(uint32_t)(1 << 4);
    }

    if (pull_down_en) {
        RTCCON1 |= (uint32_t)(1 << 1);
        RTCCON11 |= (uint32_t)(1 << 4);     // Sure this bit is enable, and recommends it is enabled normally.
    } else {
        RTCCON1 &= ~(uint32_t)(1 << 1);
    }

    if (input_en) {
        RTCCON1 |= (uint32_t)(1 << 0);
    } else {
        RTCCON1 &= ~(uint32_t)(1 << 0);
    }
}

/**
 * @brief  Read WK0 level.
 * @retval The state of interrupt_type (SET or RESET).
 */
AT(.com_periph.wk0.read_bit)
FLAG_STATE wko_io_read_bit(void)
{
    if (RTCCON & (1 << 19)) {
        return SET;
    } else {
        return RESET;
    }
}

void wk0_edge_capture_config(GPIO_EDGE_SEL edge)
{
    clk_gate0_cmd(CLK_GATE0_LP, CLK_EN);

    if(edge == GPIO_EDGE_FALLING){
        wko_io_config(1, 0, 1);
        WKUPEDG |= BIT(5);
    }else{
        wko_io_config(0, 1, 1);
        WKUPEDG &= ~BIT(5);
    }

    WKUPCON |= BIT(5);

    WKUPCPND = 0xff << 16;                  //clear wakeup pending
}

uint32_t gpio_edge_capture_idx_get(gpio_edge_cap_typedef* config)
{
    uint32_t ret_idx = WAKEUP_IDX_MAX;

    /*we first considered wakeup source is general IO */
    for (uint8_t i = WAKEUP_IDX_PORT_FALL; i < WAKEUP_IDX_MAX; i++) {
        if (config->edge == separate_wakeup_io[i].edge) {
            ret_idx = i;
            break;
        }
    }

    /*check if wakeup source is separate IO*/
    for (uint8_t i = WAKEUP_IDX_PA7; i < WAKEUP_IDX_WK0; i++) {
        if ((config->gpiox == separate_wakeup_io[i].gpiox) && (config->gpio_pin == separate_wakeup_io[i].gpio_pin)) {
            ret_idx = i;
            break;
        }
    }

    return ret_idx;
}

void gpio_edge_capture_config(gpio_edge_cap_typedef* config)
{
    clk_gate0_cmd(CLK_GATE0_LP, CLK_EN);

    gpio_init_typedef gpio_config;
    uint32_t gpio_pin = config->gpio_pin;
    if(config->gpiox == GPIOB_REG){
        gpio_pin = ((uint32_t)config->gpio_pin) << 16;
    }
    gpio_config.gpio_pin = config->gpio_pin;
    gpio_config.gpio_dir = GPIO_DIR_INPUT;
    gpio_config.gpio_fen = GPIO_FEN_GPIO;
    gpio_config.gpio_fdir = GPIO_FDIR_SELF;
    gpio_config.gpio_mode = GPIO_MODE_DIGITAL;
    gpio_config.gpio_pupd = config->gpio_pupd;

    gpio_init(config->gpiox, &gpio_config);

    uint32_t wakeup_idx = gpio_edge_capture_idx_get(config);
    
    if(config->edge == GPIO_EDGE_RISING){
        PORTINTEDG &= ~gpio_pin;
        PORTINTEN |= gpio_pin;
        WKUPEDG &= ~BIT(wakeup_idx);
        WKUPCON |= BIT(wakeup_idx);
    }else{
        PORTINTEDG |= gpio_pin;
        PORTINTEN |= gpio_pin;
        WKUPEDG |= BIT(wakeup_idx);
        WKUPCON |= BIT(wakeup_idx);
    }

    WKUPCPND = 0xff << 16;
}

AT(.com_periph.gpio.pending_clr)
void gpio_edge_pending_clear(void)
{
    WKUPCPND = 0xff << 16;
}

AT(.com_periph.gpio.pending_is)
bool gpio_is_edge_pending(void)
{
   return ((WKUPEDG>>16) & 0xff) ? true : false;
}


void gpio_edge_pic_config(isr_t isr, int pr)
{
    sys_irq_init(IRQn_PORT, pr, isr);

    WKUPCPND = 0xff << 16;
    WKUPCON |= BIT(16);
}

void gpio_edge_pic_disable(void)
{
    PICEN &= ~BIT(IRQn_PORT);
    WKUPCON &= ~BIT(16);
}

/**
 * @brief Select the 10s reset source.
 * @param  rst_pin: the reset source we choose.
 * @param  enable: whether enable 10s reset or not.
 * @retval None
 */
void gpio_reset_init(GPIO_RSTSEL_DEF rst_pin, FUNCTIONAL_STATE enable)
{
    if (ENABLE == enable) {
        WKUPCON |= (WKUPCON & ~(0x1f << 24)) | (rst_pin << 24) | BIT(29);

        if (GPIO_RSTSEL_WK0 == rst_pin) {
            RTCCON12 &= ~0x0f;                          //wk0 10S Reset Enable
            RTCCON12 |= (3 << 6) | (0x03 << 4);         //gpio 10S Reset disable, RTC WDT disable
        } else {
            RTCCON12 |= 0x0a;                   //wk0 10S Reset Disable
            RTCCON12 |= (3 << 6) | (1 << 4);  //gpio 10S Reset Enable, RTC WDT disable
        }
    } else {
        WKUPCON |= (WKUPCON & ~(0x1f << 24)) | (0 << 24) | BIT(29);
        
        if (GPIO_RSTSEL_WK0 == rst_pin) {
            RTCCON12 = (3<<6) | 0x0a;   //10S Reset Disable, RTC WDT disable
        } else {
            RTCCON12 |= (3 << 4);   //gpio 10S Reset Disable
        }
    }
}

/**
 * @brief Select the period of 10s reset.
 * @param  rst_pin: the reset source we choose.
 * @param  level:reset time from 0 to 7. 
 *         reset time = (262144 + level * 65535) / Frc, the units is second.
 * @retval None
 */
bool gpio_reset_10s_sel(GPIO_RSTSEL_DEF rst_pin, uint8_t level)
{
    if (level > 0x07) {
        return false;
    }

    RTCCON12 = (RTCCON12 &~(0x07 << 8)) | (level << 8) | BIT(11);
    RTCCON12 = (RTCCON12 &~(0x07 << 12)) | (level << 12) | BIT(15);

    return true;
}
