/**
******************************************************************************************************************************
*
*@file driver_touch_key.c
*
*@brief Source file for Touch Key Driver
*
*@Create date: 2023-03-23
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "driver_touch_key.h"
#include "driver_gpio.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if TRACE_EN
AT(.com_rodata.irq)
const char tkey_dbg_smaller[] = "smaller pnd, %08x(%d) %d %x\n";
AT(.com_rodata.irq)
const char tkey_dbg_large[] = "large pnd, %08x(%d) %d\n";
AT(.com_rodata.irq)
const char tkey_dbg_range[] = "range pnd, %08x(%d) %d\n";
AT(.com_rodata.irq)
const char tkey_dbg_to[] = "timeout pnd, %08x(%d) %d\n";
AT(.com_rodata.irq)
const char tkey_dbg_pto[] = "press timeout pnd, %08x(%d) %d\n";
AT(.com_rodata.irq)
const char tkey_dbg_str5[] = "inpcon %08x, %d %d\n";
AT(.com_rodata.irq)
const char tkey_dbg_str6[] = "%x %x %x %x %x %X %x %x %x %x %x %x\n";
#endif

void touch_key_dump(void)
{
    TRACE  ("\n");
    TRACE ("INPCON:  %08X\n", INPCON );
    TRACE ("INPCPND: %08X\n", INPCPND);
    TRACE ("TKCON:   %08X\n", TKCON);
    TRACE ("TKCON1:  %08X\n", TKCON1);
    TRACE ("TKCON2:  %08X\n", TKCON2);
    TRACE ("TKACON0:  %08X\n", TKACON0);
    TRACE ("TKACON1:  %08X\n", TKACON1);
    TRACE ("TKCNT:   %08X\n", TKCNT);
    TRACE ("TKBCNT:  %08X\n", TKBCNT);
    TRACE ("TKCDPR0: %08X\n",TKCDPR0);
    TRACE ("TKCDPR1: %08X\n",TKCDPR1);
    TRACE ("TKVARI:   %08X\n", TKVARI);
    TRACE ("TKTMR:   %08X\n", TKTMR);

    if (GETF_REG_TKCON_TK0_PND_EN() & GETF_REG_TKCON_TK0_EN()) {
        TRACE("touch key ch0 enable\n");
    }

    if (GETF_REG_TKCON_TK1_PND_EN() & GETF_REG_TKCON_TK1_EN()) {
        TRACE("touch key ch1 enable\n");
    }

    if (GETF_REG_TKCON_TK2_PND_EN() & GETF_REG_TKCON_TK2_EN()) {
        TRACE("touch key ch2 enable\r");
    }

    if (GETF_REG_TKCON_TK3_PND_EN() & GETF_REG_TKCON_TK3_EN()) {
        TRACE("touch key ch3 enable\r");
    }

    TRACE("touch key ch = 0x%02x\r\n", GET_REG_TKCON_TKSEL());
}

AT(.com_periph.touch.get_key)
u8 touch_key_get_key(void)
{
    if (!GETF_REG_TKCON_TKEN()) {
        return 0;
    }

    if (INPCON & BIT(19)) {            //touch key state
        return 1;
    }
    
    return 0;
}

AT(.com_periph.tkey.isr)
u32 tkey_tcnt_variance_calc(tk_cb_t *s, u16 tkcnt)
{
    u32 i;
    u32 sum = 0, dela, variance = 0;

    s->buf[s->cnt++] = tkcnt;
    if (s->cnt > 7) {
        s->cnt = 0;
    }
    for (i = 0; i < 8; i++) {
        sum += s->buf[i];
    }
    s->avg = sum >> 3;
    for (i = 0; i < 8; i++) {
        if (s->buf[i] > s->avg) {
            dela = s->buf[i] - s->avg;
        } else {
            dela = s->avg - s->buf[i];
        }
        variance += (dela * dela);
    }
    return variance;
}

//根据方差检测进行range exception校准
uint32_t  g_variance[4];
uint32_t  g_avg[4];

AT(.com_periph.tkey.isr)
void tkey_bcnt_range_exception(tk_cb_t *s, u16 tkcnt, uint8_t tk_ch)
{
    u16 bcnt;
    u32 variance = tkey_tcnt_variance_calc(s, tkcnt);
    g_variance[tk_ch] = variance;
    g_avg[tk_ch] = s->avg;
    if (variance < s->limit) {
        s->anov_cnt++;
        bcnt = s->bcnt_sfr[0] & 0xfff;
        if ((s->avg > (bcnt+3)) && (s->avg < (bcnt + s->range_thresh))) {
            s->stable_cnt++;
            if (s->stable_cnt > s->fil_except) {
                if (s->range_en) {
                    s->bcnt_sfr[0] = bcnt + 3;
                }
                s->stable_cnt = 0;
            }
        } else {
            s->stable_cnt = 0;
        }

        s16 mult_bcnt = s->bcnt_sfr[0];
        if ((mult_bcnt - s->avg) > 5) { //软件校准，变小跟随
            s->soft_stable_cnt++;
            if (s->soft_stable_cnt > s->fil_except) {
                if (s->range_en) {
                    s->bcnt_sfr[0] = tkcnt + 3;
                }
                s->soft_stable_cnt = 0;
            }
        } else {
            s->soft_stable_cnt = 0;
        }
    } else {
        s->anov_cnt = 0;
        s->stable_cnt = 0;
        s->soft_stable_cnt = 0;
    }
}

AT(.com_periph.tkey.isr)
void touch_key_isr(void)
{
    u32 inpcon = INPCON;
    u32 tk_exp_pnd = (inpcon >> 21) & (inpcon >> 5) & 0xf;
    u32 tk_pto_pnd = (inpcon >> 29) & (inpcon >> 13) & 0x1;
    u32 tk_pnd = (inpcon >> 16) & (inpcon >> 0) & 0x1;

    tk_exp_pnd |= (tk_pto_pnd << 4);                        //bit0-smaller exception pending
                                                            //bit1-larger exception pending
                                                            //bit2-range exception pending
                                                            //bit3-timeout exception pending
                                                            //bit4-press timeout exception pending

    if (tk_exp_pnd) {                                       //touch key exception pending
    #if TRACE_EN
        if (tk_exp_pnd & BIT(0)) {            
            printf(tkey_dbg_smaller, TKBCNT, TKBCNT & 0xfff, TKCNT, inpcon);
            printf(tkey_dbg_str6, INPCON, INPCPND, TKCON, TKCON1, TKCON2, TKACON0, TKACON1, TKCNT, TKBCNT, TKCDPR0, TKCDPR1, TKVARI);
        }
        if (tk_exp_pnd & BIT(1)) {            
            printf(tkey_dbg_large, TKBCNT, TKBCNT & 0xfff, TKCNT);
        }
        if (tk_exp_pnd & BIT(2)) {            
            printf(tkey_dbg_range, TKBCNT, TKBCNT & 0xfff, TKCNT);
        }
        if (tk_exp_pnd & BIT(3)) {            
            printf(tkey_dbg_to, TKBCNT, TKBCNT & 0xfff, TKCNT);
        }
        if (tk_exp_pnd & BIT(4)) {            
            printf(tkey_dbg_pto, TKBCNT, TKBCNT & 0xfff, TKCNT);
        }
    #endif
        INPCPND = BIT(21);
    }

    if (tk_pnd) {                                           //touch key timer pending
        INPCPND = BIT(16);
        touch_key_range_isr(inpcon >> 30, TKCNT & 0xfff);
#if TRACE_EN
        //printf(tkey_dbg_str5, inpcon, TKBCNT & 0xfff, TKCNT);
#endif
    }
}


/**
  * @brief  Touch key interrup enable and isr registered.
  *
  * @params[in] touch_pin       To makesure touch key pin data from APP is legal.
  *
  * @retval none
  */
void touch_key_isr_init(u32 touch_pin)
{
    u32 inpcon;

    sys_irq_init(IRQn_FREQUENCY_DET_TOUCH_KEY, 0, touch_key_isr);

    inpcon = BIT(0);                                        //touch key pending interrupt enable
    if (touch_pin & 0x0f) {
        //inpcon |= BIT(4)                                  //touch key bcnt valid pending interrupt enable
        inpcon |= (0xf << 5);                               //touch key exception pending interrupt enable
        inpcon |= BIT(13);                                  //tkey press timeout interrupt enable
        //inpcon |= BIT(14);                                //touch key variance interrupt enable
    }

    INPCON = inpcon;
}


/**
  * @brief  To reset touch key,so that we can change REGISTER value without power off.
  *
  * @retval none
  */
void touch_key_sw_reset(void)
{
    RTCCON0 |= BIT(4);
    INPCPND = BIT(0);               //inpwr_swrst
    RTCCON0 &= ~BIT(19);            //osc core disable
}

/**
  * @brief  To init touch key clock.
  *
  * @params[in] touch_key_reg   Touch Key Register Base Address
  * @params[in] clk_sel         Touch Key Clock Select.
  * @params[in] tkclk_div2_enable     CLK2M_TK to CLK2M_P divide
  *
  * @retval none
  */
void touch_key_clk_init(touch_key_typedef *touch_key_reg, CLK_TOUCH_KEY_TYPEDEF clk_sel, uint8_t tkclk_div2_enable)
{
    u32 clk_freq;

    clk_touch_key_clk_set(clk_sel);

    clk_freq = clk_touch_key_clk_get(CLK_VALUE_MODE_FREQ);

    INPCPND = BIT(0);                           //inpwr_swrst

    touch_key_reg->tkcon1 = 0;

    if (tkclk_div2_enable) {
        SETF_REG_TKCON1_DIV2SEL();                                  //CLK2M_TK to CLK2M_P divide
        SET_REG_TKCON1_TKCLK2MDIV((u32)(clk_freq / (125 * 2048)));  //CLK2M to 32k divide
    } else {
        SET_REG_TKCON1_TKCLK2MDIV((u32)(clk_freq / (125 * 1024)));  //CLK2M to 32k divide
    }

    SETF_REG_TKCON1_TKAEN();          //touch key analog enable
    SETF_REG_TKCON1_TKCEN();          //touch key clock enable
    SETF_REG_TKCON1_TKVCEN();         //touch key variance clock enable

    TRACE("%s: %x, %d\n", __func__, touch_key_reg->tkcon1, clk_freq);
}

/**
  * @brief  To init touch key configuration,including clock and other registers.
  *
  * @params[in] touch_key_reg   Touch Key Register Base Address
  * @params[in] tkey_cfg        Touch Key Params From APP.
  * @params[in] first_pwron     To Tell The Driver Whether Touch Key Is First Time Init Or Not.
  * @params[in] clk_sel         Touch Key Clock Select.
  * @params[in] tkclk_div2_enable     CLK2M_TK to CLK2M_P divide, typical value is 0.
  *
  * @retval     -1:     If failed
                0:      If success
  */
int touch_key_init(touch_key_typedef *touch_key_reg, void *tkey_cfg, u32 first_pwron, CLK_TOUCH_KEY_TYPEDEF clk_sel, uint8_t tkclk_div2_enable)
{
    int ret_code = 1;
    u32 touch_pin = 0;
    u16 tkbcnt;

    uint32_t tkcdpr0 = 0, tkcdpr1 = 0;

    if (tkey_cfg == NULL) {
        return -1;
    }

    tkey_cfg_t *p = (tkey_cfg_t *)tkey_cfg;

    INPCPND = BIT(0);
    // BGUV Reset TKSWRSTN disable
    RTCCON0 &= ~BIT(21);
    // Between Core Interface Enable, SNF_RC_EN
    RTCCON0 |= BIT(4) | BIT(19);

    if ((first_pwron) || (!(RTCCON0 & BIT(5)))) {
        //Touch Key Reset, or it will cause register write failure
        RTCCON0 |= BIT(5);
        touch_key_clk_init(touch_key_reg, clk_sel, tkclk_div2_enable);

        touch_key_reg->tkbcnt = 0xfff;
        touch_key_reg->tkcon = 0;
        ret_code = 0;
    }

    tkbcnt = touch_key_reg->tkbcnt & 0xfff;

    for (u8 i = 0; i < 4; i++) {
        tkey_ch_t *ch = (tkey_ch_t *)p->key[i];
        if (ch != NULL) {
            if (i == 0) {
                tkcdpr0 |= ((tkcdpr0 & (~BIT_REG_TKCDPR0_TKCDPR0)) | ((ch->cdpr) << POS_REG_TKCDPR0_TKCDPR0));

                SET_REG_TKACON0_CTRIM0(ch->ctrim);
                SET_REG_TKACON0_ITRIM0(ch->itrim);
                SETF_REG_TKCON_TK0_EN();                //touch key 0 enable
                SETF_REG_TKCON_TK0_PND_EN();            //touch key 0 pending enable

                RTCCON1 &= ~(1 << 4);                   //WK pull up disable, or may cause TKCNT error
                RTCCON1 |= BIT(8);                      //10s reset source 0:WKO 1 touch key

                if (ch->type) {
                    SETF_REG_TKACON0_ENCHN0();
                }
            } else if (i == 1) {
                tkcdpr0 |= ((tkcdpr0 & (~BIT_REG_TKCDPR0_TKCDPR1)) | ((ch->cdpr) << POS_REG_TKCDPR0_TKCDPR1));

                SET_REG_TKACON0_CTRIM1(ch->ctrim);
                SET_REG_TKACON0_ITRIM1(ch->itrim);
                SETF_REG_TKCON_TK1_EN();              //touch key 1 enable
                SETF_REG_TKCON_TK1_PND_EN();          //touch key 1 pending enable

                if (ch->type) {
                    SETF_REG_TKACON0_ENCHN1();
                }
            } else if (i == 2) {
                tkcdpr1 |= ((tkcdpr1 & (~BIT_REG_TKCDPR1_TKCDPR2)) | ((ch->cdpr) << POS_REG_TKCDPR1_TKCDPR2));
                SET_REG_TKACON1_CTRIM2(ch->ctrim);
                SET_REG_TKACON1_ITRIM2(ch->itrim);
                SETF_REG_TKCON_TK2_EN();              //touch key 2 enable
                SETF_REG_TKCON_TK2_PND_EN();          //touch key 2 pending enable

                if (ch->type) {
                    SETF_REG_TKACON1_ENCHN2();
                }
            } else if (i == 3) {
                tkcdpr1 |= ((tkcdpr1 & (~BIT_REG_TKCDPR1_TKCDPR3)) | ((ch->cdpr) << POS_REG_TKCDPR1_TKCDPR3));
                SET_REG_TKACON1_CTRIM3(ch->ctrim);
                SET_REG_TKACON1_ITRIM3(ch->itrim);
                SETF_REG_TKCON_TK3_EN();              //touch key 3 enable
                SETF_REG_TKCON_TK3_PND_EN();          //touch key 3 pending enable

                if (ch->type) {
                    SETF_REG_TKACON1_ENCHN3();
                }
            }

            if (ch->type == 1) {
                SET_REG_TKCON_TKSEL(0);             //Select WKO for hardware wakeup

                // Touch Key Pin Select
                touch_pin |= BIT(i);
            }
        }
    }

    SET_REG_TKCON1_TKARTRIM(2);

    //touch key configure
    if (touch_pin & 0x0f) {
        touch_key_reg->tkbcnt = tkbcnt;
        touch_key_reg->tktmr = p->reg_tktmr;
        touch_key_reg->tkpthd = p->reg_tkpthd;
        touch_key_reg->tkethd = p->reg_tkethd;
        touch_key_reg->tkvari = p->reg_tkvari;
        touch_key_reg->tkvarithd = p->reg_tkvarithd;
        touch_key_reg->tkcon2 = p->reg_tkcon2;
        touch_key_reg->tkcdpr0 = tkcdpr0;
        touch_key_reg->tkcdpr1 = tkcdpr1;

        SETF_REG_TKCON_TKPTOUPDBEN();     //press timeout update base_cnt enable
        SETF_REG_TKCON_TKSTAWKEN();       //touch key wakeup enable
        SETF_REG_TKCON_TKSUPDBEN();       //small pending update base_cnt enable
        SETF_REG_TKCON_TKLUPDBEN();       //large pending update base_cnt enable
        SETF_REG_TKCON_TKRUPDBEN();       //range pending update base_cnt enable
        //SETF_REG_TKCON_TKTOUPDBEN();      //timerout pending update base_cnt enable
        SETF_REG_TKCON_TKBCNTVAL();       //touch key base_cnt valid enable
    }

    SETF_REG_TKCON_TKEN();                //touch key enable
    SETF_REG_TKCON_TKAEN_ATSEL();         //touch key auto enable
    SETF_REG_TKCON_TK_MODE();

    delay_5ms(10);

    touch_key_isr_init(touch_pin);

    touch_key_dump();

    return ret_code;
}

