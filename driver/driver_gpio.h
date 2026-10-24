/*
 * @File name    : driver_gpio.h
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-13
 * @Description  : This file contains all the functions prototypes for the GPIO library.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#ifndef _DRIVER_GPIO_H
#define _DRIVER_GPIO_H

#include "driver_com.h"

/**
 * @brief GPIO_Pins_Definition
 */
#define GPIO_PIN_ALL                ((uint16_t)0xffff)      /* All PIN bits definition */
#define GPIO_PIN_0                  ((uint16_t)0x0001)      /* PIN 0 bit definition */
#define GPIO_PIN_1                  ((uint16_t)0x0002)      /* PIN 1 bit definition */
#define GPIO_PIN_2                  ((uint16_t)0x0004)      /* PIN 2 bit definition */
#define GPIO_PIN_3                  ((uint16_t)0x0008)      /* PIN 3 bit definition */
#define GPIO_PIN_4                  ((uint16_t)0x0010)      /* PIN 4 bit definition */
#define GPIO_PIN_5                  ((uint16_t)0x0020)      /* PIN 5 bit definition */
#define GPIO_PIN_6                  ((uint16_t)0x0040)      /* PIN 6 bit definition */
#define GPIO_PIN_7                  ((uint16_t)0x0080)      /* PIN 7 bit definition */
#define GPIO_PIN_8                  ((uint16_t)0x0100)      /* PIN 8 bit definition */
#define GPIO_PIN_9                  ((uint16_t)0x0200)      /* PIN 9 bit definition */
#define GPIO_PIN_10                 ((uint16_t)0x0400)      /* PIN 10 bit definition */
#define GPIO_PIN_11                 ((uint16_t)0x0800)      /* PIN 11 bit definition */
#define GPIO_PIN_12                 ((uint16_t)0x1000)      /* PIN 12 bit definition */
#define GPIO_PIN_13                 ((uint16_t)0x2000)      /* PIN 13 bit definition */
#define GPIO_PIN_14                 ((uint16_t)0x4000)      /* PIN 14 bit definition */
#define GPIO_PIN_15                 ((uint16_t)0x8000)      /* PIN 15 bit definition */


/**
 * @brief GPIO Function Mapping Crossbar Peripheral Definition
 */
typedef enum {
    GPIO_CROSSBAR_OUT_I2CSCL = 5,
    GPIO_CROSSBAR_OUT_I2CSDA,
    GPIO_CROSSBAR_OUT_T3PWM0,
    GPIO_CROSSBAR_OUT_T3PWM1,
    GPIO_CROSSBAR_OUT_T3PWM2,
    GPIO_CROSSBAR_OUT_TOC1,
    GPIO_CROSSBAR_OUT_TOC1N,
    GPIO_CROSSBAR_OUT_TOC2,
    GPIO_CROSSBAR_OUT_TOC2N,
    GPIO_CROSSBAR_OUT_TOC3,
    GPIO_CROSSBAR_OUT_TOC3N,
    GPIO_CROSSBAR_OUT_TOC4,
    GPIO_CROSSBAR_OUT_UARTTX,
    GPIO_CROSSBAR_OUT_UDETTX,
    GPIO_CROSSBAR_OUT_HURTX,
    GPIO_CROSSBAR_OUT_HURRTS,
    GPIO_CROSSBAR_OUT_SPI1D0,
    GPIO_CROSSBAR_OUT_SPI1D1,
    GPIO_CROSSBAR_OUT_SPI1D2,
    GPIO_CROSSBAR_OUT_SPI1D3,
    GPIO_CROSSBAR_OUT_SPI1CLK,
    GPIO_CROSSBAR_OUT_LPWM,
    GPIO_CROSSBAR_OUT_LEDCDAT,
    GPIO_CROSSBAR_OUT_IRTX,
    GPIO_CROSSBAR_OUT_CLKOUT,

    GPIO_CROSSBAR_PERIPHERAL_INPUT_BASE,
    GPIO_CROSSBAR_IN_UARTRX,
    GPIO_CROSSBAR_IN_UDETRX,
    GPIO_CROSSBAR_IN_SPI1CLK,
    GPIO_CROSSBAR_IN_SPI1DI0,
    GPIO_CROSSBAR_IN_SPI1DI1,
    GPIO_CROSSBAR_IN_HURRX,
    GPIO_CROSSBAR_IN_HURCTS,
    GPIO_CROSSBAR_IN_T0CAP,
    GPIO_CROSSBAR_IN_T1CAP,
    GPIO_CROSSBAR_IN_T2CAP,
    GPIO_CROSSBAR_IN_T3CAP,
    GPIO_CROSSBAR_IN_TBKIN,
    GPIO_CROSSBAR_IN_TETR,
    GPIO_CROSSBAR_IN_I2CSCL,
    GPIO_CROSSBAR_IN_I2CSDA,
    GPIO_CROSSBAR_IN_IRRX,
    GPIO_CROSSBAR_IN_QDECXDI0,
    GPIO_CROSSBAR_IN_QDECXDI1,
    GPIO_CROSSBAR_IN_QDECYDI0,
    GPIO_CROSSBAR_IN_QDECYDI1,
    GPIO_CROSSBAR_IN_QDECZDI0,
    GPIO_CROSSBAR_IN_QDECZDI1,
    GPIO_CROSSBAR_IN_SDCMD,
    GPIO_CROSSBAR_IN_SDDAT0,
    GPIO_CROSSBAR_PERIPHERAL_MAX_IDX,
} GPIO_CROSSBAR_PERIPHERAL_DEF;

/**
 * @brief 10s reset source for chip
 */
typedef enum {
    GPIO_RSTSEL_MCLR,
    GPIO_RSTSEL_PA0,
    GPIO_RSTSEL_PA1,
    GPIO_RSTSEL_PA2,
    GPIO_RSTSEL_PA3,
    GPIO_RSTSEL_PA4,
    GPIO_RSTSEL_PA5,
    GPIO_RSTSEL_PA6,
    GPIO_RSTSEL_PA7,
    GPIO_RSTSEL_PA8,
    GPIO_RSTSEL_PA9,
    GPIO_RSTSEL_PA10,
    GPIO_RSTSEL_PA11,
    GPIO_RSTSEL_PA12,
    GPIO_RSTSEL_PA13,
    GPIO_RSTSEL_PA14,
    GPIO_RSTSEL_PA15,
    GPIO_RSTSEL_PB0,
    GPIO_RSTSEL_PB1,
    GPIO_RSTSEL_PB2,
    GPIO_RSTSEL_PB3,
    GPIO_RSTSEL_PB4,
    GPIO_RSTSEL_PB5,
    GPIO_RSTSEL_PB6,
    GPIO_RSTSEL_PB7,
    GPIO_RSTSEL_PB8,
    GPIO_RSTSEL_PB9,
    GPIO_RSTSEL_WK0,
} GPIO_RSTSEL_DEF;

/**
 * @brief Bits detinition for GPIO
 */
typedef enum {
    BIT_RESET           = 0,
    BIT_SET,
} BIT_ACTION;

/**
 * @brief GPIO direction enumeration
 */
typedef enum {
    GPIO_DIR_OUTPUT     = 0x00,
    GPIO_DIR_INPUT      = 0x01,
} GPIO_DIR_TYPEDEF;

/**
 * @brief GPIO input pull resistance enumeration
 */
typedef enum {
    GPIO_PUPD_NULL      = 0x00,
    GPIO_PUPD_PU10K     = 0x01,
    GPIO_PUPD_PD10K     = 0x02,
    GPIO_PUPD_PU200K    = 0x03,
    GPIO_PUPD_PD200K    = 0x04,
    GPIO_PUPD_PU300     = 0x05,
    GPIO_PUPD_PD300     = 0x06,
} GPIO_PUPD_TYPEDEF;

/**
 * @brief GPIO port type enumeration
 */
typedef enum {
    GPIO_MODE_ANALOG    = 0x00,
    GPIO_MODE_DIGITAL   = 0x01,
} GPIO_MODE_TYPEDEF;

/**
 * @brief GPIO function mapping enable enumeration
 */
typedef enum {
    GPIO_FEN_GPIO       = 0x00,
    GPIO_FEN_PER        = 0x01,
} GPIO_FEN_TYPEDEF;

/**
 * @brief GPIO function mapping direction select register
 */
typedef enum {
    GPIO_FDIR_SELF      = 0x00,
    GPIO_FDIR_MAP       = 0x01,
} GPIO_FDIR_TYPEDEF;

/**
 * @brief GPIO output current enumeration, specially PB4 can output 64mA.
 */
typedef enum {
    GPIO_DRV_6MA        = 0x00,
    GPIO_DRV_24MA       = 0x01,
    GPIO_DRV_64MA       = (3 << 10),
} GPIO_DRV_TYPEDEF;

typedef enum {
    GPIO_EDGE_RISING        = 0x00,
    GPIO_EDGE_FALLING       = 0x01,
} GPIO_EDGE_SEL;

typedef enum {
    WAKEUP_IDX_PA7,
    WAKEUP_IDX_PB1,
    WAKEUP_IDX_PB2,
    WAKEUP_IDX_PB3,
    WAKEUP_IDX_PB4,
    WAKEUP_IDX_WK0,
    WAKEUP_IDX_PORT_FALL,
    WAKEUP_IDX_PORT_RISE,
    WAKEUP_IDX_MAX,
} GPIO_WAKEUP_IDX;

/**
 * @brief GPIO Init Structure
 */
typedef struct {
    uint32_t              gpio_pin;
    GPIO_DIR_TYPEDEF      gpio_dir;
    GPIO_PUPD_TYPEDEF     gpio_pupd;
    GPIO_MODE_TYPEDEF     gpio_mode;
    GPIO_FEN_TYPEDEF      gpio_fen;
    GPIO_FDIR_TYPEDEF     gpio_fdir;
    GPIO_DRV_TYPEDEF      gpio_drv;
} gpio_init_typedef;

typedef struct {
    GPIO_EDGE_SEL edge;
    gpio_typedef *gpiox;
    uint16_t gpio_pin;
    GPIO_PUPD_TYPEDEF     gpio_pupd;
} gpio_edge_cap_typedef;

#define GPIO_SUPPORT_STRONG_CURRENT(drv, gpiox, pin_idx)           ((GPIO_DRV_64MA == drv) && (GPIOB_REG == gpiox) && (GPIO_PIN_4 == (1 << pin_idx)))


/************************ Function used to set or R/W GPIO *************************/
void gpio_init(gpio_typedef *gpiox, gpio_init_typedef *gpio_init_struct);
uint32_t gpio_deinit(gpio_typedef *gpiox, uint16_t gpio_pin);
void gpio_set_bits(gpio_typedef *gpiox, uint16_t gpio_pin);
void gpio_reset_bits(gpio_typedef *gpiox, uint16_t gpio_pin);
void gpio_write_data(gpio_typedef *gpiox, uint16_t port_val);
void gpio_toggle_bits(gpio_typedef *gpiox, uint16_t gpio_pin);
uint16_t gpio_read_data(gpio_typedef *gpiox);
uint8_t gpio_read_bit(gpio_typedef *gpiox, uint16_t gpio_pin);
void gpio_func_mapping_config(gpio_typedef* gpiox, uint16_t gpio_pin, GPIO_CROSSBAR_PERIPHERAL_DEF func_idx);
void gpio_func_mapping_clear(gpio_typedef* gpiox, uint16_t gpio_pin);
void wko_io_config(FUNCTIONAL_STATE pull_up_en, FUNCTIONAL_STATE pull_down_en, FUNCTIONAL_STATE input_en);
FLAG_STATE wko_io_read_bit(void);
void wk0_edge_capture_config(GPIO_EDGE_SEL edge);
void gpio_edge_capture_config(gpio_edge_cap_typedef* config);
bool gpio_is_edge_pending(void);
void gpio_edge_pending_clear(void);
void gpio_edge_pic_config(isr_t isr, int pr);
void gpio_edge_pic_disable(void);
void gpio_reset_init(GPIO_RSTSEL_DEF rst_pin, FUNCTIONAL_STATE enable);
bool gpio_reset_10s_sel(GPIO_RSTSEL_DEF rst_pin, uint8_t level);

#endif  // _DRIVER_GPIO_H
