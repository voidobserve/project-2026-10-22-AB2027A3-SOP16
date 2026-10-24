/**
******************************************************************************************************************************
*
*@file driver_irtx.c
*
*@brief Source file for IR tx driver
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "driver_ir.h"
#include "driver_gpio.h"
#include "macro.h"

void ir_clk_init(uint8_t clksel)
{
    // irrx/irrx clock config
    if ((IR_CLK_OSC32K == clksel) || (IR_CLK_X24M32K == clksel) || (IR_CLK_X24MDIV == clksel)) {
        XOSCCON |= BIT(10);                                         //XOSC24M enable
        CLKCON4 &= ~BIT(26);                                        //xoscrc24m_sel chose XOSC24M
        clk_gate1_cmd(CLK_GATE1_X24M, CLK_EN);                      //XOSC24M gate open

        if (IR_CLK_OSC32K == clksel) {
            CLKCON1 &= ~BIT(17);                                    // k32_tscsel chose osc32k_p(CLK32K_RTC)
            RTCCON0 |= (BIT(2));                                    // RTC output clock to core enable
            RTCCON0 &= ~(BIT(19));                                  // osc core disable

            RTCCON0 = (RTCCON0 & ~(3 << 8)) | (3 << 8);             // CLK32K_RTC source select x24mdiv12_rtc(x24m_clkdiv12)
            CLKCON0 = (CLKCON0 & ~(0x0f << 13)) | (3 << 13);        // clkout sel, osc32k
        } else if (IR_CLK_X24M32K == clksel) {
            CLKCON0 = (CLKCON0 & ~(0x0f << 13)) | (2 << 13);        // clkout sel, x24m_32K
        } else if (IR_CLK_X24MDIV == clksel) {
            CLKCON0 = (CLKCON0 & ~(0x0f << 13)) | (1 << 13);        // clkout sel, x24mdiv unvisible,we can check xosc24m
        }
    } else if (IR_CLK_RC32K == clksel) {
        CLKCON0 |= BIT(0);                                          //RC2M enable
        CLKCON0 = (CLKCON0 & ~(0x0f << 13)) | (7 << 13);            // clkout sel, rc32k unvisible,we can check rc2m
    }

    //irrx clock select
    CLKCON1 = (CLKCON1 & ~(3 << 4)) | (clksel << 4);            // 0:x24m_32k 1:x24m_div_clk 2:osc32k 3:rc32k_clk
}

void irtx_clk_init (uint8_t clksel)
{
    ir_clk_init(clksel);
    clk_gate1_cmd(CLK_GATE1_IRTX, CLK_EN);
    clk_gate2_cmd(CLK_GATE2_IRFLT, CLK_EN);
}

/*
 * timing[0] : one     [31:16]mark  m1  [15:0]space  s1
 * timing[1] : zeor    [31:16]mark  m0  [15:0]space  s0
 * timing[2] : start   [31:16]1     t1  [15:0]0      t0
 * timing[3] : repteat [31:16]1     r1  [15:0]0      r0
 * timing[4] : IRREPEATCON          rt et sy rm
 * timing[5] : IRTXCON              o  z
 * timing[6] : IRTXLEN              l
*/
const IRTX_timingTypeDef Nec        = IRTX_TIMING_NEC;
const IRTX_timingTypeDef Tc9012     = IRTX_TIMING_TC9012;
const IRTX_timingTypeDef Rc5        = IRTX_TIMING_RC5;
const IRTX_timingTypeDef Rc6        = IRTX_TIMING_RC6;

void irtx_timing_set(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param, const IRTX_timingTypeDef timing)
{
    double prescale;
    volatile u32 timing_prescale;

    timing_prescale = ir_timing_freq_get(irtx_param->clk_sel);
    prescale = ((double)timing_prescale/1000000);

    irtx_reg->con   = BIT(8) *  timing.em |
                      BIT(5) *  timing.o  |
                      BIT(4) *  timing.z  |
                      BIT(0);
    irtx_reg->otime = BIT(16) * (uint16_t)(timing.m1 * prescale) |
                      BIT(0) *  (uint16_t)(timing.s1 * prescale);
    irtx_reg->ztime = BIT(16) * (uint16_t)(timing.m0 * prescale) |
                      BIT(0) *  (uint16_t)(timing.s0 * prescale);
    irtx_reg->stime = BIT(16) * (uint16_t)(timing.t1 * prescale) |
                      BIT(0) *  (uint16_t)(timing.t0 * prescale);
    irtx_reg->rtime = BIT(16) * (uint16_t)(timing.r1 * prescale) |
                      BIT(0) *  (uint16_t)(timing.r0 * prescale);
    irtx_reg->rctrl = BIT(13) * (uint32_t)(timing.rt * prescale) |
                      BIT(3) *  (uint16_t)(timing.et * prescale) |
                      BIT(2) *  timing.sy |
                      BIT(1) *  timing.rm;
    irtx_reg->txlen = timing.l;
}

void irtx_timing_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    switch(irtx_param->encode_format)
    {
        case IRTX_ENCODE_FORMAT_NEC:
            irtx_timing_set(irtx_reg, irtx_param, Nec);
            break;
        case IRTX_ENCODE_FORMAT_TC9012:
            irtx_timing_set(irtx_reg, irtx_param, Tc9012);
            break;
        case IRTX_ENCODE_FORMAT_RC5:
            irtx_timing_set(irtx_reg, irtx_param, Rc5);
            break;
        case IRTX_ENCODE_FORMAT_RC6:
            irtx_timing_set(irtx_reg, irtx_param, Rc6);
            break;
        default:
            break;
    }
}

//edge set 0 to choose fall edge
void irtx_capture_edge (irtx_typedef *irtx_reg, uint32_t edge)
{
    if (edge) {
        SETF_REG_IRTXCON_IREDGE_SEL();
    }else {
        CLRF_REG_IRTXCON_IREDGE_SEL();
    }
}

void irtx_carrier_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    if (irtx_param->carrier_config.mod_clksel == IRTX_MODE_CLKSEL_3M) {
        CLKCON1 &= ~BIT(9);//1:4m 0:3m
    } else {
        CLKCON1 |= BIT(9);//1:4m 0:3m
    }

    if (irtx_param->carrier_config.carrier_en) {
        SETF_REG_IRTXCON_IRTX_CARRIER_EN();
        SET_REG_IRTXCON_IRCW_LENGTH(irtx_param->carrier_config.ircw_length);
        SET_REG_IRTXCON_IRCW_DUTY(irtx_param->carrier_config.ircw_duty);
    }else {
        CLRF_REG_IRTXCON_IRTX_CARRIER_EN();
    }
}

void irtx_dma_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    if (irtx_param->dma_config.rdma_mode == IRTX_DMA_MODE_16BIT) {
        SETF_REG_IRDMACON_IR_RDMA_MODE();
    } else {
        CLRF_REG_IRDMACON_IR_RDMA_MODE();
    }

    if (irtx_param->dma_config.wdma_mode == IRTX_DMA_MODE_16BIT) {
        SETF_REG_IRDMACON_IR_WDMA_MODE();
    } else {
        CLRF_REG_IRDMACON_IR_WDMA_MODE();
    }
}

void irtx_data_set (irtx_typedef *irtx_reg, uint32_t code)
{
    irtx_reg->txdata = code;
}

void irtx_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    irtx_data_set(irtx_reg, irtx_param->tx_data);
    irtx_carrier_init(irtx_reg, irtx_param);

    if (IRTX_SYNC_OR_TRAILER_CODE_1 == irtx_param->sync_code) {
        SETF_REG_IRREPEATCON_IRTX_SYC();
    }else {
        CLRF_REG_IRREPEATCON_IRTX_SYC();
    }
}

void irtx_set_repeat(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    if (IRTX_KEY_PRESSED == irtx_param->key) {
        SETF_REG_IRREPEATCON_IRTX_KEY();
    }else {
        CLRF_REG_IRREPEATCON_IRTX_KEY();
    }
}

void irfilter_set(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    if (IRTX_FILTER_ENABLE == irtx_param->filter_config.filter_en) {
        SETF_REG_IRFLTCON_IRFLT_EN();
        SET_REG_IRFLTCON_IRFLT_LEN(irtx_param->filter_config.filter_len);
    } else {
        CLRF_REG_IRFLTCON_IRFLT_EN();
    }
}

//KICK
inline void irtx_kick_do (void)
{
    SETF_REG_IRTXCON_IRTX_KST();
}

inline void irtx_wait(void)
{
    while (!GETF_REG_IRTXPEND_IRTX_PND());
    SETF_REG_IRTXPEND_IRTX_PND();
}

void irtx_kick_wait(void)
{
    irtx_kick_do();
    irtx_wait();
}

//CAPTURE DMA
void irtx_capture_dma_init(irtx_typedef *irtx_reg, void *buf, uint32_t len)
{
    irtx_reg->dma_oadr = (uint32_t)buf;
    irtx_reg->dma_osize = len - 1;

    SETF_REG_IRDMACON_IR_WDMA_EN();
    SETF_REG_IRDMACON_IR_WDMA_MODE();
    //SETF_REG_IRDMACON_IR_WDMA_APND_IE();
}

void irtx_capture_dma_kick(irtx_typedef *irtx_reg)
{
    SETF_REG_IRTXCON_IR_CAPEN();
}

void irtx_learn_dma_init(irtx_typedef *irtx_reg, void *buf, uint32_t len)
{
    irtx_reg->dma_iadr = (uint32_t)buf;
    irtx_reg->dma_isize = len - 1;
}

void irtx_learn_kick(irtx_typedef *irtx_reg)
{
    //SETF_REG_IRTXCON_IRTXLEN_EN();
    SETF_REG_IRTXCON_IRTX_KST();
}

inline void irtx_set_invert(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    if (IRTX_INVERT_OUTPUT_ENABLE == irtx_param->invert_en) {
        SETF_REG_IRTXCON_IRTX_INV();
    }else {
        CLRF_REG_IRTXCON_IRTX_INV();
    }
}

/**
  * @brief  Set the clock,and clock/rx io init.Set irrx register.
  * @param  irrx_reg: irrx register address
  * @param  irrx_param: irrx params,include wakup_en/clock/int_en/data_format.
  * @retval None
  */
void irtx_base_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    irtx_clk_init(irtx_param->clk_sel);
    irtx_timing_init(irtx_reg, irtx_param);
    irfilter_set(irtx_reg, irtx_param);
    irtx_set_invert(irtx_reg, irtx_param);

    irtx_init(irtx_reg, irtx_param);
    irtx_set_repeat(irtx_reg, irtx_param);
}

static bool irtx_is_repeat(void)
{
    bool ret = true;

    if (GETF_REG_IRREPEATCON_IRTX_KEY()) {
        ret = false;
    }

    return ret;
}

void irtx_repeat_send(void)
{
    if (irtx_is_repeat() && GETF_REG_IRFLTCON_IRFLT_EN()) {
        irtx_kick_wait();
    }
}

void irtx_send(void)
{
    irtx_kick_wait();
}

void irtx_capture_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    irtx_clk_init(irtx_param->clk_sel);

    irtx_capture_edge(irtx_reg, irtx_param->cap_edge); //edge set 1 to choose rising edge
}

void irtx_capture_buf_init(irtx_typedef *irtx_reg, void *buf, uint32_t len)
{
    irtx_capture_dma_init(irtx_reg, buf, len);
    irtx_capture_dma_kick(irtx_reg);
}

void irtx_learn_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param)
{
    irtx_clk_init(irtx_param->clk_sel);

    irtx_carrier_init(irtx_reg, irtx_param);

    irtx_dma_init(irtx_reg, irtx_param);

    SETF_REG_IRDMACON_IR_RDMA_EN();
    SETF_REG_IRTXCON_IRTXLEN_EN();
}

void irtx_learn_buf_init(irtx_typedef *irtx_reg, void *buf, uint32_t len)
{
    irtx_learn_dma_init(irtx_reg, buf, len);
    irtx_learn_kick(irtx_reg);
}

/**
 * @brief  Enable or disable the specified IRTX interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  flag_type: specifies the IRTX interrupt sources to be enable or disable.
 *         This parameter can be one of the following values:
 *             @arg IRTX_FLAG_WDMA_HPND: ir write DMA half interrupt.
 *             @arg IRTX_FLAG_WDMA_APND: ir write DMA all interrupt.
 *             @arg IRTX_FLAG_RDMA_HPND: ir read DMA half interrupt.
 *             @arg IRTX_FLAG_RDMA_APND: ir read DMA all interrupt.
 * @param  state: the new state need to config.
 * @retval None
 */
void irtx_pic_config(isr_t isr, int pr, IRTX_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state)
{
    u32 flag_pending_bit = 0;

    if (flag_type == 0) {
        return;
    }

    if (flag_type & IRTX_FLAG_WDMA_HPND) {
        flag_pending_bit |= BIT_REG_IRDMACON_IR_WDMA_HPND_IE;
    }

    if (flag_type & IRTX_FLAG_WDMA_APND) {
        flag_pending_bit |= BIT_REG_IRDMACON_IR_WDMA_APND_IE;
    }

    if (flag_type & IRTX_FLAG_RDMA_HPND) {
        flag_pending_bit |= BIT_REG_IRDMACON_IR_RDMA_HPND_IE;
    }

    if (flag_type & IRTX_FLAG_RDMA_APND) {
        flag_pending_bit |= BIT_REG_IRDMACON_IR_RDMA_APND_IE;
    }

    if (state != DISABLE) {
        IRTX->dma_con |= flag_pending_bit;
        sys_irq_init(IRQn_IR_QDEC_LEDC, pr, isr);
    } else {
        IRTX->dma_con &= ~flag_pending_bit;
    }
}

