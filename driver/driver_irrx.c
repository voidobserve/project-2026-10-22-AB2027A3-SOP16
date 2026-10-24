/**
******************************************************************************************************************************
*
*@file driver_irrx.c
*
*@brief Source file for IR rx driver
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "driver_ir.h"

/**
  * @brief  Set the clock,and clock/rx io init.Set irrx register.
  * @param  irrx_reg: irrx register address
  * @param  irrx_param: irrx params,include wakup_en/clock/int_en/data_format.
  * @retval None
  */
void irrx_base_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param)
{
    irrx_decode_config(irrx_reg, irrx_param);

    irrx_init(irrx_reg, irrx_param);
}

/**
  * @brief  Set irrx wakup_en/int_en/data_format.
  * @param  irrx_reg: irrx register address
  * @param  irrx_param: irrx params,include wakup_en/clock/int_en/data_format.
  * @retval None
  */
void irrx_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param)
{
    // IR RX Wake Up Sleep Mode Enable
    if (IRRX_WAKUP_SLEEP_ENABLE == irrx_param->wakup_en) {
        SETF_REG_IRRXCON_IRWKEN();
    }
    else if (IRRX_WAKUP_SLEEP_DISABLE == irrx_param->wakup_en) {
        CLRF_REG_IRRXCON_IRWKEN();
    }

    // IR RX Data Select 16 Bit Data
    if (IRRX_DATA_SELECT_16BIT == irrx_param->data_format) {
        SETF_REG_IRRXCON_IRRXSEL();
    }
    else if (IRRX_DATA_SELECT_32BIT == irrx_param->data_format) {
        CLRF_REG_IRRXCON_IRRXSEL();
    }

    // IR RX Interrupt Enable
    if (IRRX_INT_DISABLE == irrx_param->int_en) {
        CLRF_REG_IRRXCON_IRIE();
    }
    else if (IRRX_INT_ENABLE == irrx_param->int_en) {
        SETF_REG_IRRXCON_IRIE();
    }

    // IR RX Enable
    if (IRRX_DISABLE == irrx_param->irrx_en) {
        CLRF_REG_IRRXCON_IREN();
    }
    else if (IRRX_ENABLE == irrx_param->irrx_en) {
        SETF_REG_IRRXCON_IREN();
    }


}

/**
  * @brief  irrx enable and set irrx period0/period1/err0/err1 value.
  * @param  irrx_reg: irrx register address
  * @retval None
  */
void irrx_decode_config(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param)
{
    double prescale;
    volatile u32 timing_prescale, period0, period1, err0, err1;

    timing_prescale = ir_timing_freq_get(irrx_param->clk_sel);

    prescale = ((double)timing_prescale/1000000);

    period0       = BIT(16) * (uint16_t)(RPTPR_CNT   * prescale) |
                                (uint16_t)(DATPR_CNT   * prescale);
    period1       = BIT(16) * (uint16_t)(DATA_1_CNT  * prescale) |
                                (uint16_t)(DATA_0_CNT  * prescale);
    err0          = BIT(16) * (uint16_t)(RPTERR_CNT  * prescale) |
                                (uint16_t)(DATERR_CNT  * prescale);
    err1          = BIT(20) * (uint16_t)(TOPR_CNT    * prescale) |
                    BIT(10) * (uint16_t)(ONEERR_CNT  * prescale) |
                                (uint16_t)(ZEROERR_CNT * prescale);

    irrx_reg->period0   = period0;
    irrx_reg->period1   = period1;
    irrx_reg->err0      = err0;
    irrx_reg->err1      = err1;
    //SETF_REG_IRRXCON_IREN();
}

void irrx_decode_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param)
{
    ir_clk_init(irrx_param->clk_sel);

    clk_gate1_cmd(CLK_GATE1_IRRX, CLK_EN);

    irrx_base_init(irrx_reg, irrx_param);
}

void irrx_wake_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param)
{
    ir_clk_init(irrx_param->clk_sel);

    clk_gate1_cmd(CLK_GATE1_IRRX, CLK_EN);

    irrx_base_init(irrx_reg, irrx_param);
}

