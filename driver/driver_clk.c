/*
 * @File name    : driver_clk.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-04-04
 * @Description  : This file provides functions to manage the most functionalities
 *                 of the CLK peripheral.
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */
#include "driver_clk.h"

void clk_gate0_cmd(uint32_t clock_gate, CLK_STATE state)
{
    if (state != CLK_DIS) {
        CLKGAT0 |= clock_gate;
    } else {
        CLKGAT0 &= ~clock_gate;
    }
}

void clk_gate1_cmd(uint32_t clock_gate, CLK_STATE state)
{
    if (state != CLK_DIS) {
        CLKGAT1 |= clock_gate;
    } else {
        CLKGAT1 &= ~clock_gate;
    }
}

void clk_gate2_cmd(uint32_t clock_gate, CLK_STATE state)
{
    if (state != CLK_DIS) {
        CLKGAT2 |= clock_gate;
    } else {
        CLKGAT2 &= ~clock_gate;
    }
}

void clk_clkout_set(CLK_CLKOUT_TYPEDEF clk_sel, uint8_t div, CLK_STATE state)
{
    if (state != CLK_DIS) {
        CLKCON0 &= ~(0x0f << 13);
        CLKCON0 |= ((clk_sel & 0x0f) << 13);

        CLKDIVCON0 &= ~(0x1f << 17);
        CLKDIVCON0 |= ((div & 0x1f) << 17);
    } else {
        CLKCON0 &= ~(0x0f << 13);
        CLKDIVCON0 &= ~(0x1f << 17);
    }
}

void clk_clk32k_rtc_set(CLK_CLK32K_RTC_TYPEDEF clk_sel)
{
    uint32_t rtccon0 = RTCCON0;

    if ((CLK_CLK32K_RTC_RC2M_RTC == clk_sel) || (CLK_CLK32K_RTC_SNF_RC_RTC == clk_sel)) {
        rtccon0  |= BIT(19);                                //RC2M_RTC enable
    }

    rtccon0 &= ~(0x03 << 8);
    rtccon0 |= ((clk_sel & 0x03) << 8);

    RTCCON0 = rtccon0;
}

uint32_t clk_clk32k_rtc_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t rtccon0 = RTCCON0;
    u8 index;

    index = (rtccon0 >> 8) & 0x03;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_CLK32K_RTC_1BIT:
                return -1L;

            case CLK_CLK32K_RTC_RC2M_RTC:
            case CLK_CLK32K_RTC_SNF_RC_RTC:
                return rc2m_clk_nhz_get(1) / 64;

            case CLK_CLK32K_RTC_X24MDIV12_RTC:
                return 2000000 / 64;

            default:
                return 0;
        }
    }
}

void clk_hsut0_clk_set(CLK_HSUART_CLK_TYPEDEF clk_sel, uint8_t div)
{
    uint32_t clkcon0 = CLKCON0;

    clkcon0 &= ~(0x03 << 17);
    clkcon0 |= ((clk_sel & 0x03) << 17);

    CLKCON0 = clkcon0;

    if (clk_sel >= CLK_HSUT0_CLK_PLLDIV2_CLK) {
        CLKDIVCON0 &= ~(0x03 << 22);
        CLKDIVCON0 |= ((div & 0x03) << 22);
        PLL0CON |= BIT(13);
    }
}

uint32_t clk_hsut0_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon0 = CLKCON0;
    u8 index;

    index = ((clkcon0 >> 17) & 0x03);

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_HSUT0_CLK_XOSC24M_CLK:
                return 24000000;

            case CLK_HSUT0_CLK_XOSC48M:
                return 48000000;

            case CLK_HSUT0_CLK_PLLDIV2_CLK:
                return pll_clk_nhz_get() / 2 / (((CLKDIVCON0 >> 22) & 0x03) + 1);

            case CLK_HSUT0_CLK_UDET_CLKPP:
                return clk_udet_clk_get(CLK_VALUE_MODE_FREQ) / (((CLKDIVCON0 >> 22) & 0x03) + 1);

            default:
                return 0;
        }
    }
}

void clk_saradc_clk_set(CLK_SARADC_CLK_TYPEDEF clk_sel)
{
    uint32_t clkcon0 = CLKCON0;

    clkcon0 &= ~(0x03 << 28);
    clkcon0 |= ((clk_sel & 0x03) << 28);

    CLKCON0 = clkcon0;
}

uint32_t clk_saradc_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon0 = CLKCON0;
    u8 index;

    index = ((clkcon0 >> 28) & 0x03);

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_SARADC_CLK_RC2M_CLK:
                return rc2m_clk_nhz_get(0);

            case CLK_SARADC_CLK_X24M_CLKDIV4:
                return 6000000;

            case CLK_SARADC_CLK_X24M_CLKDIV2:
                return 12000000;

            case CLK_SARADC_CLK_X24Md32K_CLK:
                return 32000;

            default:
                return 0;
        }
    }
}

void clk_udet_clk_set(CLK_UDET_CLK_TYPEDEF clk_sel)
{
    uint32_t clkcon1 = CLKCON1;

    clkcon1 &= ~(0x03 << 28);
    clkcon1 |= ((clk_sel & 0x03) << 28);

    CLKCON1 = clkcon1;
}

uint32_t clk_udet_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon1 = CLKCON1;
    u8 index;

    index = ((clkcon1 >> 28) & 0x03);

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_UDET_CLK_RC2M_CLK:
                return rc2m_clk_nhz_get(0);

            case CLK_UDET_CLK_RTC_RC2M:
                return rc2m_clk_nhz_get(1);

            case XOSC24M_CLK:
                return 24000000;

            case XOSC48M:
                return 48000000;

            default:
                return 0;
        }
    }
}

void clk_tmr_inc_set(CLK_TMR_INC_TYPEDEF clk_sel)
{
    uint32_t clkcon0 = CLKCON0;

    clkcon0 &= ~(0x03 << 23);
    clkcon0 |= ((clk_sel & 0x03) << 23);

    CLKCON0 = clkcon0;
}

uint32_t clk_tmr_inc_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon0 = CLKCON0;
    u8 index;

    index = (clkcon0 >> 23) & 0x03;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_TMR_INC_OSC32K:
                return clk_clk32k_rtc_get(CLK_VALUE_MODE_FREQ);

            case CLK_TMR_INC_CLKOUOT_PIN:
                return -1L;

            case CLK_TMR_INC_X24M_DIV_CLK:
                return 24000000 / (((CLKDIVCON0 >> 24) & 0xff) + 1);

            case CLK_TMR_INC_RC2M_D0:
                return rc2m_clk_nhz_get(0) / 2;

            default:
                return 0;
        }
    }
}

void clk_uart_inc_set(CLK_UART_INC_TPYEDEF clk_sel)
{
    uint32_t clkcon1 = CLKCON1;

    clkcon1 &= ~(0x01 << 14);
    clkcon1 |= ((clk_sel & 0x01) << 14);

    CLKCON1 = clkcon1;
}

uint32_t clk_uart_inc_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon1 = CLKCON1;
    u8 index;

    index = (clkcon1 >> 14) & 0x01;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_UART_INC_X24M_DIV_CLK:
                return 24000000 / (((CLKDIVCON0 >> 24) & 0xff) + 1);

            case CLK_UART_INC_X24M_CLKDIV4:
                return 6000000;

            default:
                return 0;
        }
    }
}

void clk_clk2m_ks_set(CLK_CLK2M_KS_TYPEDEF clk_sel)
{
    uint32_t rtccon0 = RTCCON0;

    rtccon0 &= ~(0x03 << 14);
    rtccon0 |= ((clk_sel & 0x03) << 14);

    RTCCON0 = rtccon0;
}

uint32_t clk_clk2m_ks_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t rtccon0 = RTCCON0;
    u8 index;

    index = (rtccon0 >> 14) & 0x03;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_CLK2M_KS_1BIT:
                return -1L;

            case CLK_CLK2M_KS_RC2M_RTC:
                return rc2m_clk_nhz_get(1);

            case CLK_CLK2M_KS_SNF_RC_RTC:
                return rc2m_clk_nhz_get(1);

            case CLK_CLK2M_KS_X24MDIV12_RTC:
                return 2000000;

            default:
                return 0;
        }
    }
}

void clk_sdadda_clk_set(CLK_SDADDA_CLK_TYPEDEF clk_sel, uint8_t div)
{
    u32 clkcon1 = CLKCON1;
    u32 clkdivcon0 = CLKDIVCON0;

    if (clk_sel == 4 || clk_sel > 5) {
        return;
    }

    clkcon1 &= ~0x07;
    clkcon1 |= clk_sel;
    CLKCON1 = clkcon1;

    if (clk_sel == CLK_SDADDA_CLK_ADDA_CLK24_A) {
        CLKGAT2 |= BIT(14);
        clkdivcon0 &= ~(0x0f << 4);
        clkdivcon0 |= ((div & 0x0f) << 4);
        CLKDIVCON0 = clkdivcon0;
    }
}

uint32_t clk_sdadda_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    u32 clkcon1 = CLKCON1;
    u8 div = ((CLKDIVCON0 >> 4) & 0x0f) + 1;
    u8 index;

    index = clkcon1 & 0x07;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_SDADDA_CLK_ADDA_CLK24_A:
                return (pll_clk_nhz_get() / 2 / div / 2);

            case CLK_SDADDA_CLK_XOSC24M_CLK:
                return 24000000;

            case CLK_SDADDA_CLK_XOSC24M_CLK_2PLL:
                return 24000000;

            default:
                return 0;
        }
    }
}

void clk_ledc_clk_set(CLK_LEDC_CLK_TYPEDEF clk_sel)
{
    u32 clkcon1 = CLKCON1;

    clkcon1 &= ~(0x01 << 27);
    clkcon1 |= (clk_sel & 0x01) << 27;

    CLKCON1 = clkcon1;
}

uint32_t clk_ledc_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    u32 clkcon1 = CLKCON1;
    u8 index;

    index = (clkcon1 >> 27) & 0x01;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_LEDC_CLK_XOSC24M:
                return 24000000;

            case CLK_LEDC_CLK_X24M_CLKDIV2:
                return 12000000;

            default:
                return 0;
        }
    }
}

void clk_touch_key_clk_set(CLK_TOUCH_KEY_TYPEDEF clk_sel)
{
    if (CLK_TOUCH_KEY_CLK_XOSC24M == clk_sel) {
        XOSCCON |= BIT(10);     //xosc enable
        CLKCON4 &= ~BIT(26);    //select xosc24m_p
    	CLKGAT1 |= BIT(29);     //x24m_clk enable
    } else if (CLK_TOUCH_KEY_RC2M == clk_sel) {
    	RTCCON0 |= BIT(19);                          //RC2M_RTC EN
    }

    RTCCON0 = (RTCCON0 & ~(3 << 12)) | (clk_sel << 12);

	//RTCCON &= ~BIT(0);	   //VRTC11 voltage select RTCCON4 bit26~24
	//RTCCON4 = (RTCCON4 & ~(0x07 << 24)) | (5 << 24);		//vrtc11 voltage select 0.7+0.065*5 = 1.025V
}

uint32_t clk_touch_key_clk_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    u32 rtccon0 = RTCCON0;
    u8 index;

    index = (rtccon0 >> 12) & 0x03;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_TOUCH_KEY_RC2M:
                return rc2m_clk_nhz_get(1);

            case CLK_TOUCH_KEY_CLK_XOSC24M:
                return 2000000;

            default:
                return 0;
        }
    }
}

void clk_ttmr_inc_set(CLK_TTMR_INC_TYPEDEF clk_sel)
{
    uint32_t clkcon1 = CLKCON1;

    clkcon1 &= ~(0x03 << 12);
    clkcon1 |= ((clk_sel & 0x03) << 12);

    CLKCON1 = clkcon1;
}

uint32_t clk_ttmr_inc_get(CLK_VALUE_MODE_TYPEDEF mode)
{
    uint32_t clkcon1 = CLKCON1;
    uint8_t index;

    index = (clkcon1 >> 12) & 0x03;

    if (mode == CLK_VALUE_MODE_IDX) {
        return index;
    } else {
        switch (index) {
            case CLK_TTMR_INC_OSC_32K:
                return clk_clk32k_rtc_get(CLK_VALUE_MODE_FREQ);

            case CLK_TTMR_INC_CLKOUT_PIN:
                /* TODO: */
                return -1L;

            case CLK_TTMR_INC_X24M_DIV_CLK:
                return 24000000 / (((CLKDIVCON0 >> 24) & 0xff) + 1);

            case CLK_TTMR_INC_RC2M_D0:
                return rc2m_clk_nhz_get(0) / 2;

            default:
                return 0;
        }
    }
}

void clk_qdec_set(CLK_QDEC_TYPEDEF clk_sel)
{
    uint32_t clkcon1 = CLKCON1;

    clkcon1 &= ~(0x01 << 8);
    clkcon1 |= ((clk_sel & 0x01) << 8);

    CLKCON1 = clkcon1;
}
