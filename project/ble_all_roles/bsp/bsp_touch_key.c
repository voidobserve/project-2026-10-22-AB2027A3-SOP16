#include "include.h"
#include "driver_touch_key.h"
#include "bsp_touch_key.h"

#if BSP_TOUCH_KEY_EN

//static uint8_t touch_key_id_value = 0;
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_check[] = "INPCON %08x, TKVARI %x, tk0bcnt %d,tk1bcnt %d,tk2bcnt %d,tk3bcnt %d,tkcnt %d,%d,%d,%d,\n";
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_sta[] = "sta1 %d,sta2 %d,sta3 %d\n";
uint8_t touch_key_check(tk_cb_t *s, u16 tkcnt);

AT(.com_rodata.touch_key_0)
const tkey_ch_t tkey0 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x07,//0x14
    .tkpthd = 18,
    .tkrthd = 12,
};

AT(.com_rodata.touch_key_1)
const tkey_ch_t tkey1 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x07,//0x13
    .tkpthd = 20,//17,
    .tkrthd = 12,
};

AT(.com_rodata.touch_key_2)
const tkey_ch_t tkey2 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x07,//0x12
    .tkpthd = 20,//18,
    .tkrthd = 12,//10,//12,
};

AT(.com_rodata.touch_key_3)
const tkey_ch_t tkey3 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x07,//0x11
    .tkpthd = 16,//16,
    .tkrthd = 12,//10,
};

AT(.com_rodata.touch_key_cfg)
const tkey_cfg_t tkey_cfg = {
    .key[0] = &tkey0,
    //.key[0] = NULL,
    .key[1] = &tkey1,
    //.key[1] = NULL,
    .key[2] = &tkey2,
    //.key[2] = NULL,
    .key[3] = &tkey3,
    //.key[3] = NULL,

    //TKTMR Register
    .fil_low = FIL_LOW,
    .fil_high = FIL_HIGH,
    .fil_except = FIL_EXCEPT,
    .fil_val = FIL_VAL,
    .to_except = TO_EXCEPT,

    //TKPTHD Register
    .tkpthd = TKPTHRESH,
    .tkrthd = TKRTHRESH,
    .pto_except = PTO_EXCEPT,

    //TKETHD Register
    .tksthd = SMALLTHD,
    .tklthd = LARGETHD,

    //TKVARI Register
    .tkvthd = 20,

    //TKVARITHD Register
    .tkarthd = 20,
    .tkaethd = SMALLTHD,
    .tkvfil  = 7,
    .tkbadd  = SMALLTHD,

    //TKCON2 Register
     .press_vari_en = 0,
     .rels_vari_en = 0,
     .press_fil_sel = 1,
     .rels_fil_sel = 1,
     .press_vari_thd = 20,
     .rels_vari_thd = 20,
     .to_bcnt_thd = 10,
};

tk_cb_t tk_cb;
tk_cb_t tk1_cb;
tk_cb_t tk2_cb;
tk_cb_t tk3_cb;
u16 tkcnt_tmr[4] = {0};       //每个通路的实时TKCNT

mult_cb_t tk1_mult_cb;
mult_cb_t tk2_mult_cb;
mult_cb_t tk3_mult_cb;

AT(.com_text.tkey.isr)
void tkey_mult_set_status(mult_cb_t *t, u16 tkcnt)
{
    s16 difference_val = tkcnt - t->bcnt;
    if( difference_val > TKPTHRESH) {
        t->sta = true;
    } else {
        t->sta = false;
    }
}

AT(.com_text.tkey)
bool tkey_is_pressed(void)
{
    if (tk_cb.ch == 0xff) {
        return false;
    }
    if (!touch_key_get_key()) {
        return false;
    }

    return true;
}

AT(.com_text.touch_key.check_status)
u8 bsp_tkey_hard_mult_check_status(void)
{
    if (tkey_is_pressed()){
        return 1;
    } else if(tk1_mult_cb.sta) {
        return 2;
    } else if(tk2_mult_cb.sta) {
        return 3;
    } else if(tk3_mult_cb.sta) {
        return 4;
    } else {
        return 0;					//没有按下
    }
}

//中断调用
AT(.com_text.touch_key.isr)
void touch_key_range_isr(u32 tk_ch, u16 tkcnt)
{
    tkcnt_tmr[tk_ch] = tkcnt;

    if(tk_ch == tk_cb.ch)
    {

    }
    else if(tk_ch == tk1_cb.ch)//tk1_cb.ch)
    {
        tkey_bcnt_range_exception(&tk1_cb,tkcnt,tk_ch);
        tkey_mult_set_status(&tk1_mult_cb,tkcnt);
    }
    else if(tk_ch == tk2_cb.ch)//tk2_cb.ch)
    {
        tkey_bcnt_range_exception(&tk2_cb,tkcnt,tk_ch);
        tkey_mult_set_status(&tk2_mult_cb,tkcnt);
    }
    else if(tk_ch == tk3_cb.ch)
    {
        tkey_bcnt_range_exception(&tk3_cb,tkcnt,tk_ch);
        tkey_mult_set_status(&tk3_mult_cb,tkcnt);
    }

}

void touch_key_var_init(void)
{
    memset(&tk_cb, 0, sizeof(tk_cb_t));
    memset(&tk1_cb, 0, sizeof(tk_cb_t));
    memset(&tk2_cb, 0, sizeof(tk_cb_t));
    memset(&tk3_cb, 0, sizeof(tk_cb_t));

    memset(&tk1_mult_cb,0,sizeof(mult_cb_t));
    memset(&tk2_mult_cb,0,sizeof(mult_cb_t));
    memset(&tk3_mult_cb,0,sizeof(mult_cb_t));

    tk_cb.ch = 0xff;
    tk_cb.limit = 100;
    tk_cb.range_thresh = 20;
    tk_cb.ch = 0xff;
    tk_cb.fil_except = FIL_EXCEPT;
    tk_cb.bcnt_sfr = (psfr_t)(&TKBCNT);
    tk_cb.range_en = 1;                                 //软件range校准是否使能
    tk_cb.to_cnt = TO_EXCEPT;                           //10秒

    memcpy(&tk1_cb, &tk_cb, sizeof(tk_cb_t));
    memcpy(&tk2_cb, &tk_cb, sizeof(tk_cb_t));
    memcpy(&tk3_cb, &tk_cb, sizeof(tk_cb_t));

    tk1_cb.bcnt_sfr = (psfr_t)(&tk1_mult_cb.bcnt);
    tk2_cb.bcnt_sfr = (psfr_t)(&tk2_mult_cb.bcnt);
    tk3_cb.bcnt_sfr = (psfr_t)(&tk3_mult_cb.bcnt);

    tk1_cb.range_en = 1;                                 //软件range校准是否使能
    tk2_cb.range_en = 1;                                 //软件range校准是否使能
    tk3_cb.range_en = 1;                                 //软件range校准是否使能

	printf("range_en1 %d, range_en2 %d, range_en3 %d\n", tk1_cb.range_en, tk2_cb.range_en, tk3_cb.range_en);
    memset(tk_cb.touch_key_sta, TOUCH_KEY_STA_RELEASED, sizeof(tk_cb.touch_key_sta));
}

void touch_key_channel_set(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        tkey_ch_t *ch = (tkey_ch_t *)tkey_cfg.key[i];
        if (ch) {
            if (ch->type == 1) {
                if((tk_cb.ch == 0xff) && (i == 0)) {
                   tk_cb.ch = i;
                } else if((tk1_cb.ch == 0xff) && (i == 1)) {
                   tk1_cb.ch = i;
                } else if((tk2_cb.ch == 0xff) && (i == 2)) {
                   tk2_cb.ch = i;
                } else if((tk3_cb.ch == 0xff) && (i == 3)) {
                   tk3_cb.ch = i;
                }
            }
        }
    }
}

void bsp_touch_key_init(void)
{
    touch_key_var_init();

	uint8_t first_pwron_flag = 0;

	if (sys_cb.rst_reason & BIT(31)) {
		printf("touch key first pwron!!\n");
		first_pwron_flag = 1;
	}
    int res;
    tkey_cfg_t tkey_cfg_v;

    memcpy(&tkey_cfg_v, &tkey_cfg, sizeof(tkey_cfg_t));

    touch_key_channel_set();
    //touch_key_sw_reset();
    res = touch_key_init(TOUCH_KEY, (void *)&tkey_cfg_v, first_pwron_flag, CLK_TOUCH_KEY_RC2M, 0);
    (void)res;
	printf("ch0 %d, ch1 %d, ch2 %d, ch3 %d\n", tk_cb.ch, tk1_cb.ch, tk2_cb.ch, tk3_cb.ch);

    u32 ticks = tick_get();
    while(tk1_mult_cb.bcnt == 0 || tk2_mult_cb.bcnt == 0 || tk3_mult_cb.bcnt == 0) {
        tk1_mult_cb.bcnt = tkcnt_tmr[tk1_cb.ch];
        tk2_mult_cb.bcnt = tkcnt_tmr[tk2_cb.ch];
        tk3_mult_cb.bcnt = tkcnt_tmr[tk3_cb.ch];
        if (tick_check_expire(ticks,50)) {
            printf("USER_TKEY_MULT BCNT INIT FALSE\n");
            break;
        }
    }
}

extern uint32_t  g_variance[4];
extern uint32_t  g_avg[4];
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_vari[] = "g_variance:%d,%d,%d,%d------g_avg:%d,%d,%d,%d, sta %d\n";
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_cnt[] = "cnt %d,%d,%d, bcnt %d,%d,%d tkpthd %d,%d\n";
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_cnt1[] = "TOUCH_KEY_STA_PRESSED,cnt %d,%d,%d, bcnt %d, tkpthd %d\n";
AT(.com_rodata.tkey_dbg)
const char tkey_dbg_cnt2[] = "TOUCH_KEY_STA_RELEASED,cnt %d,%d,%d, bcnt %d, tkpthd %d\n";

AT(.com_text.touch_key.isr)
uint8_t touch_key_check(tk_cb_t *s, u16 tkcnt)
{
    uint8_t key_sta_change = 0;
    uint16_t tkbcnt;
	#if 1
    static uint32_t dbg_tick = 0;
	static uint8_t dbg_en = 0;
	if (tick_check_expire(dbg_tick, 200) && dbg_en) {
		dbg_tick = tick_get();
		printf(tkey_dbg_check, INPCON, TKVARI, TKBCNT & 0xfff, tk1_mult_cb.bcnt, tk2_mult_cb.bcnt, tk3_mult_cb.bcnt, \
					tkcnt_tmr[0], tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3]);
		printf(tkey_dbg_vari, g_variance[0],g_variance[1],g_variance[2],g_variance[3],g_avg[0],g_avg[1],g_avg[2],g_avg[3], tk_cb.touch_key_sta[1]);
		printf(tkey_dbg_cnt, tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3],  tk1_mult_cb.bcnt,tk2_mult_cb.bcnt,tk3_mult_cb.bcnt, tkey_cfg.key[1]->tkpthd,tkey_cfg.key[2]->tkpthd);
	}
	#endif

    for (uint8_t ch_idx = 1; ch_idx < SUPPORT_TOUCH_KEY_NUM; ch_idx ++) {
		if (1 == ch_idx) tkbcnt = tk1_mult_cb.bcnt;
		if (2 == ch_idx) tkbcnt = tk2_mult_cb.bcnt;
		if (3 == ch_idx) tkbcnt = tk3_mult_cb.bcnt;

        if (tkcnt_tmr[ch_idx] > tkey_cfg.key[ch_idx]->tkpthd + tkbcnt) {
            // Key Pressed
            key_sta_change = 1;
            tk_cb.touch_key_sta[ch_idx] =  TOUCH_KEY_STA_PRESSED;
			//if (ch_idx == 1)
			//printf(tkey_dbg_cnt1, tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3],  tkbcnt, tkey_cfg.key[1]->tkpthd);
        //} else if (ABS(tkcnt_tmr[ch_idx], tkbcnt) < tkey_cfg.key[ch_idx]->tkrthd) {
        } else if (tkcnt_tmr[ch_idx] < tkey_cfg.key[ch_idx]->tkrthd + tkbcnt) {
            // Key Released
            key_sta_change = 1;
            tk_cb.touch_key_sta[ch_idx] = TOUCH_KEY_STA_RELEASED;
			//if (ch_idx == 1)
			//printf(tkey_dbg_cnt2, tkcnt_tmr[1], tkcnt_tmr[2], tkcnt_tmr[3],	tkbcnt, tkey_cfg.key[1]->tkpthd);
        }
    }

    if (key_sta_change) {
        return TOUCH_KEY_DET_CHANGE;
    }
    return TOUCH_KEY_DET_NO_KEY;
}

AT(.com_rodata.tkey_dbg)
const char tkey_dbg_get_key_id[] = "ch %d, sel ch %d\n";

AT(.com_periph.touch.get_key_id)
u8 bsp_get_touch_key_id(void)
{
    uint8_t key_value = touch_key_check(NULL, 0);

    if (TOUCH_KEY_DET_NO_KEY == key_value) {
        return KEY_ID_NO;
    }

    for (uint8_t ch_idx = 0; ch_idx < SUPPORT_TOUCH_KEY_NUM; ch_idx ++) {
			if (0 == ch_idx) {
				if (tkey_is_pressed()) {
					return KEY_ID_PP;
				}
			}
            else if (TOUCH_KEY_STA_PRESSED == tk_cb.touch_key_sta[ch_idx]) {
				return KEY_ID_PP + ch_idx;
            }
    }
    return KEY_ID_NO;
}

#endif // BSP_TOUCH_KEY_EN
