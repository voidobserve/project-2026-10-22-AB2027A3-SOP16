/*
 * @File name    : driver_wdt.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the WDT peripheral.
 * 
 * Copyright (c) by Bluetrum, All Rights reserved.
 */
#include "driver_wdt.h"

/**
  * @brief  Enable or disable the WDT peripheral.
  * @param  state: the state of the WDT peripheral.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void wdt_cmd(FUNCTIONAL_STATE state)
{
    uint32_t temp_reg = WDT->con;

    if (state) {
        temp_reg |= WDTCON_WDTEN_WR_0;
    } else {
        temp_reg &= ~WDTCON_WDTEN_WR;
        temp_reg |= (uint32_t)(0x0a << 4);
    }

    WDT->con = temp_reg;
}

/**
  * @brief  Enable or disable the WDT reset.
  * @param  state: the state of the WDT reset.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void wdt_reset_cmd(FUNCTIONAL_STATE state)
{
    uint32_t temp_reg = WDT->con;

    if (state) {
        temp_reg |= WDTCON_RSTEN_WR_0;
    } else {
        temp_reg &= ~WDTCON_RSTEN_WR;
        temp_reg |= (uint32_t)(0x0a << 8);
    }

    WDT->con = temp_reg;
}

/**
  * @brief  Enable or disable the WDT interrupt.
  * @param  state: the state of the WDT interrupt.
  *         this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void wdt_it_cmd(FUNCTIONAL_STATE state)
{
    uint32_t temp_reg = WDT->con;

    temp_reg &= ~WDTCON_WDTIE_WR;

    if (state) {
        temp_reg |= (uint32_t)(0x05 << 12);
    } else {
        temp_reg |= (uint32_t)(0x0a << 12);
    }

    WDT->con = temp_reg;
}

/**
  * @brief  Secect the WDT clock.
  * @param  clk: The WDT clock.
  *         this parameter can be one of the following values:
  *             @arg WDT_CLK_RC32K: RC32K.
  *             @arg WDT_CLK_X32K: X32K from 26M divider.
  * @retval None.
  */
void wdt_clk_select(WDT_CLK_TYPEDEF clk)
{
    uint32_t temp_reg = WDT->con;

    temp_reg &= ~WDTCON_WDTCSEL_WR;
    temp_reg |= clk;

    if(clk) {
        temp_reg |= (uint32_t)(0x05 << 16);
    } else {
        temp_reg |= (uint32_t)(0x0a << 16);
    }

    WDT->con = temp_reg;
}

/**
  * @brief  Secect the WDT time.
  * @param  time: The WDT time.
  *         this parameter can be one of the following values:
  *             @arg WDT_TIME_1MS: 1ms.
  *             @arg WDT_TIME_256MS: 256ms.
  *             @arg WDT_TIME_512MS: 512ms.
  *             @arg WDT_TIME_1024MS: 1024ms.
  *             @arg WDT_TIME_2048MS: 2048ms.
  *             @arg WDT_TIME_4096MS: 4096ms.
  *             @arg WDT_TIME_8192MS: 8192ms.
  *             @arg WDT_TIME_16384MS: 16384ms.
  * @retval None.
  */
void wdt_time_select(WDT_TIME_TYPEDEF time)
{
    uint32_t temp_reg;

    temp_reg = WDT->con;

    temp_reg &= ~(WDTCON_TMRSEL | WDTCON_TMRSEL_WR);
    temp_reg |= time;
    temp_reg |= (0x0a << 24);

    WDT->con = temp_reg;
}

/**
  * @brief  Get the WDT flag.
  * @param  wdt_flag: specifies the flag to set.
  *         this parameter can be one of the following values:
  *             @arg WDT_FLAG_PENDING: WDT pending.
  * @retval The state of wdt_flag (SET or RESET).
  */
FLAG_STATE wdt_get_flag(uint32_t wdt_flag)
{
    if ((WDT->con & WDT_FLAG_PENDING) != RESET) {
        return SET;
    } else {
        return RESET;
    }
}

/**
  * @brief  Clear the WDT pending.
  * @param  None
  * @retval None
  */
void wdt_clear(void)
{
    WDT->con |= (uint32_t)0x0a;
}
