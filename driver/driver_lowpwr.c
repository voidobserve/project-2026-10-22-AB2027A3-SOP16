/*
 * @File name    : driver_lowpwr.c
 * @Author       : Bluetrum IOT Team
 * @Date         : 2023-02-15
 * @Version      : v2.0
 *
 * Copyright (c) by Bluetrum, All Rights Reserved.
 */

#include "driver_gpio.h"
#include "driver_saradc.h"
#include "driver_lowpwr.h"

extern void io_sleep(void);
extern void io_resume(void);


extern u32 __comm_vma;

static lowpwr_sfr_backup_t sfr_backup;
static uint32_t wkupedg_bak = 0;

AT(.com_sleep.lv2_get_wksrc)
uint32_t lowpwr_sleep_get_io_wksrc(void)
{
    return wkupedg_bak;
}

AT(.com_sleep.lv2_clr_wksrc)
void lowpwr_sleep_clr_io_wksrc(void)
{
    wkupedg_bak = 0;
}

u32 lowpwr_get_wakeup_source(void)
{
    u32 reason = RTCCON9 | RTCCON10;

    if(reason){
        if(reason & WK_LP_KEYSCAN){
            printf("KEY_SCAN_WK\n");
        } else if(reason & WK_LP_GPIO){
            printf("GPIO_WK\n");
        } else if(reason & WK_LP_TOUCH){
            printf("TOUCH_WK\n");
        } else if(reason & WK_LP_INBOX){
            printf("INBOX_WK\n");
        } else if(reason & WK_LP_VUSB){
            printf("VUSB_WK\n");
        } else if(reason & WK_LP_WK0){
            printf("WKO_WK\n");
        } else if(reason & WK_LP_RTC_1S){
            printf("RTC1S_WK\n");
        } else if(reason & WK_LP_ALARM){
            printf("ALMWKP_WK\n");
        }
        
        RTCCON9 = 0x1fff;                                //Clr wakeup pending
        RTCCON10 = 0xffff;
    }


    return (reason & 0xffff);

}

void lowpwr_wakeup_disable(void)
{
    RTCCON9 = 0x1fff;                           //Clr wakeup pending
    RTCCON10 = 0xffff;
    RTCCON3 &= ~(0x7ff00);

    WKUPCPND = 0xff << 16;                      //clear wakeup pending
    WKUPEDG = 0;
    WKUPCON = 0;
    PORTINTEDG = 0;
    PORTINTEN = 0;
}

void lowpwr_clr_wakeup_pending(void)
{
    WKUPCPND = 0xff << 16;                      //clear wakeup pending

    RTCCON9 = 0x1fff;                           //Clr wakeup pending
    RTCCON10 = 0xffff;
}

AT(.com_periph.lowpwr.sleep)
bool lowpwr_is_wakeup_pending(u32 ignore_src)
{
    u32 wk_pending = 0x1fff;

    if(ignore_src){
        wk_pending &= ~ignore_src;
    }

    return ((RTCCON9 | RTCCON10) & wk_pending);
}

void lowpwr_wk0_wakeup_config(lowpwr_wakeup_typedef *config)
{
    if (config->lp_type == LP_TYPE_SLEEP) {
        wk0_edge_capture_config(config->edge);
        WKUPCON |= BIT(17);             //wakeup sleep mode enable
        RTCCON3 |= BIT(17);
    } else if(config->lp_type == LP_TYPE_POWOFF) {
        if (config->edge == WK_EDGE_FALLING) {
            wko_io_config(1, 0, 1);
            RTCCON1 &= ~BIT(6);         //wk pin low level wakeup
        } else {
            wko_io_config(0, 1, 1);
            RTCCON1 |= BIT(6);          //wk pin high level wakeup
        }
        RTCCON3 |= BIT(10);
    }
}

void lowpwr_gpio_wakeup_config(lowpwr_wakeup_typedef *config)
{
    if((config->gpiox == GPIOA_REG) || (config->gpiox == GPIOB_REG)){
        gpio_edge_cap_typedef gpio_cfg;

        gpio_cfg.edge = config->edge;
        gpio_cfg.gpio_pin = config->gpio_pin;
        gpio_cfg.gpiox = config->gpiox;
        gpio_cfg.gpio_pupd = config->gpio_pupd;
        gpio_edge_capture_config(&gpio_cfg);

        RTCCON3 |= BIT(17);
        WKUPCON |= BIT(17);                         //wakeup sleep mode enable
        WKUPCPND = 0xff << 16;                      //clear wakeup pending
    }
}

void lowpwr_wakeup_config_do(lowpwr_wakeup_typedef *config)
{
    if(config){
        if(!(config->source & WK_LP_TOUCH)){
            RTCCON0 &= ~BIT(4);     //disable touch key
        }

        if(config->source & WK_LP_WK0){
            lowpwr_wk0_wakeup_config(config);
        }

        if(config->source & WK_LP_TOUCH){
            RTCCON0 |= BIT(4);
            RTCCON1 |= BIT(8);
            RTCCON1 |= BIT(9);          //VDDTK enable for touch key logic
            RTCCON3 |= BIT(14);         // Touch key long press wakeup enable
        }

        if(config->source & WK_LP_BT){
            RTCCON3 |= BIT(13);
        }

        if(config->source & WK_LP_ALARM){
            RTCCON0 |= BIT(19);
            RTCCON3 |= BIT(8);
        }

        if(config->source & WK_LP_RTC_1S){
            RTCCON3 |= BIT(9);
        }

        if(config->source & WK_LP_VUSB){
            RTCCON3 |= BIT(11);
        }

        if(config->source & WK_LP_KEYSCAN){
            RTCCON3 |= BIT(16);
        }

        if(config->source & WK_LP_INBOX){
           RTCCON3 |= BIT(12);

        }

        if(config->source & WK_LP_GPIO){
            lowpwr_gpio_wakeup_config(config);
        }
    }
}

void lowpwr_wakeup_config(lowpwr_wakeup_typedef *config)
{
    if(config){
        RTCCON3 &= 0xFF;
        lowpwr_wakeup_config_do(config);
    }
}

void lowpwr_wakeup_config_2(lowpwr_wakeup_typedef *config)
{
    if(config){
        lowpwr_wakeup_config_do(config);
    }
}

void lowpwr_wakeup_source_enable(WK_LP_SRC_SEL wk_src)
{
    if(wk_src & WK_LP_WK0){
        RTCCON3 |= BIT(17);
    }

    if(wk_src & WK_LP_TOUCH){
        RTCCON3 |= BIT(14);
    }

    if(wk_src & WK_LP_BT){
        RTCCON3 |= BIT(13);
    }

    if(wk_src & WK_LP_ALARM){
        RTCCON3 |= BIT(8);
    }

    if(wk_src & WK_LP_RTC_1S){
        RTCCON3 |= BIT(9);
    }

    if(wk_src & WK_LP_VUSB){
        RTCCON3 |= BIT(11);
    }

    if(wk_src & WK_LP_KEYSCAN){
        RTCCON3 |= BIT(16);
    }

    if(wk_src & WK_LP_INBOX){
       RTCCON3 |= BIT(12);

    }

    if(wk_src & WK_LP_GPIO){
        RTCCON3 |= BIT(17);
    }
}

void lowpwr_wakeup_source_disable(WK_LP_SRC_SEL wk_src)
{
    if (wk_src == WK_LP_SRC_ALL) {
        RTCCON3 &= 0xFF;
        return;
    }

    if(wk_src & WK_LP_WK0){
        RTCCON3 &= ~BIT(17);
    }

    if(wk_src & WK_LP_TOUCH){
        RTCCON3 &= ~BIT(14);         // Touch key long press wakeup enable
    }

    if(wk_src & WK_LP_BT){
        RTCCON3 &= ~BIT(13);
    }

    if(wk_src & WK_LP_ALARM){
        RTCCON3 &= ~BIT(8);
    }

    if(wk_src & WK_LP_RTC_1S){
        RTCCON3 &= ~BIT(9);
    }

    if(wk_src & WK_LP_VUSB){
        RTCCON3 &= ~BIT(11);
    }

    if(wk_src & WK_LP_KEYSCAN){
        RTCCON3 &= ~BIT(16);
    }

    if(wk_src & WK_LP_INBOX){
       RTCCON3 &= ~BIT(12);

    }

    if(wk_src & WK_LP_GPIO){
        RTCCON3 &= ~BIT(17);
    }
}

ALIGNED(512)
void lowpwr_pwroff_mode1_do(void)
{
    u32 rtccon3;

    saradc_deinit();         //disable saradc

    PICCONCLR = BIT(0);     //disable global interrupt
    NMICON = 0;

    RTCCON3 &= ~BIT(17);    //disable gpio wakeup

    RTCCON9 = 0x1fff;       //Clr wakeup pending

    if(RTCCON3 & BIT(14)){                          //Touch need rc2m clk
        RTCCON0 |= BIT(19);                         //RC2M_RTC enable
        asm("nop");asm("nop");asm("nop");
        RTCCON0 = (RTCCON0 & ~(0x03 << 12)) | (0x02 << 12); //CLK2M in Touch select SNF_RC_RTC
    }else{
        RTCCON0  &= ~BIT(19);                       //RC2M_RTC disable
    }

    RTCCON4 &= ~BIT(17);    //FGS
    RTCCON4 &= ~BIT(18);    //FGBUF
    RTCCON4 &= ~BIT(15);    //VIO_PD not work

    RTCCON4 = (RTCCON4 & ~(0x3f<<24)) | (5<<24) | (6<<27);  //no sfr access, bit26~bit24, VRTC11 is 1.025V; sfr access, bit29~27, VRTC11 is 1.09V

    CLKCON0 &= ~0xc;        //sys_clk switch to RC
    asm("nop");
    asm("nop");
    asm("nop");

    XOSCCON = 0;
    PLL0CON = 0;

    rtccon3 = RTCCON3 & ~(0xFF);
    rtccon3 |= BIT(5) | BIT(6);      //core power down enable

    WDT_DIS();

    RTCCON |= BIT(5);       //RTC wake up power down mode reset system enable
    RTCCON3 = rtccon3;

    LPMCON |= BIT(0);	    //sleep mode
    LPMCON |= BIT(1);       //idle mode

    asm("nop");asm("nop");asm("nop");

    while(1);
}

//in pwroff_mode2_final, can't use subfunction
AT(.com_text.pwroff.mode2)
static void pwroff_mode2_final(void)
{
    u32 rtccon3 = RTCCON3;
    u32 rtccon0 = RTCCON0;

    RTCCON4 &= ~(BIT(17) | BIT(18));    //FGS, FGBUF

    if(RTCCON3 & (BIT(8) | BIT(14) | BIT(16))){     //wakeup source need rc2m clk
        RTCCON0 |= BIT(19);                         //RC2M_RTC enable
    }else{
        RTCCON0  &= ~BIT(19);                       //RC2M_RTC disable
    }

    PWRCON1 = (PWRCON1 & ~(0XF<<14)) | (0X0<<14); //DI_SW DISABLE

    RTCCON8 &= ~BIT(29);
    RTCCON4 |= BIT(11);     //VDDIO_AON voltage
    RTCCON3 |= BIT(7);     //enable vddio aon ldo

    RTCCON4 = (RTCCON4 & ~(0x3f<<24)) | (5<<24) | (6<<27);  //no sfr access, bit26~bit24, VRTC11 is 1.025V; sfr access, bit29~27, VRTC11 is 1.09V

    rtccon0 = RTCCON0;

    if(RTCCON3 & BIT(14)){
        rtccon0 = (rtccon0 & ~(0x03 << 12)) | (0x02 << 12); //CLK2M in Touch select SNF_RC_RTC
    }

    if(RTCCON3 & BIT(16)){
        rtccon0 = (rtccon0 & ~(0x03 << 14)) | (0x02 << 14); //CLK2M in Keyscan select SNF_RC_RTC
    }

    RTCCON0 = rtccon0;

    CLKCON0 &= ~0xc;        //sys_clk switch to RC
    asm("nop");
    asm("nop");
    asm("nop");

    XOSCCON = 0;
    PLL0CON = 0;

//  when switch power, reduce lp_top power, disable ram0/1 clock
    CLKGAT0 &= ~((3<<2) | BIT(15)); // sram0/1 & LP disable clock

    RTCCON5 &= ~(1<<11); // dis sram if

    RTCCON5 |= (BIT(15) | BIT(16));   //RAM0 RAM1 shutdown
    //RTCCON5 |= BIT(13); //RAM0/1 light sleep
    //RTCCON5 |= BIT(14); //RAM0/1 deep sleep

    // pmu power down
    LVDCON &= ~(BIT(3) | BIT(5));      //disable LVD reset

    rtccon3 = RTCCON3 & ~BIT(5);    //disable C2RL
    rtccon3 |= (BIT(4) | BIT(3));   //PD CORE2, disable all analog model except IO

    rtccon3 &= ~0x7;     //disable VDDCORE & VDDIO & VDDBT LDO

    //disable WDT
    WDT_DIS();

    RTCCON |= BIT(5);   //RTC wake up power down mode reset system enable

    RTCCON3 = rtccon3;
    LPMCON |= BIT(0);	//sleep mode
    LPMCON |= BIT(1);   //idle mode
    while(1);
}

AT(.com_text.lp_port_init)
void lp_port_init(void)
{
    //config IO low power state
    //when RTCCON5 bit12=0, will select LPGPIO; otherwise, select GPIO
    //save IO

    if(pmu_vddio_to_flash()){
        GPIOGDE = BIT(2);
        GPIOGPU = BIT(2);
    }else{
        GPIOGDE = 0;
        GPIOGPU = 0;
    }

    CLKGAT0 |= BIT(15);         //lp_clk enable

    io_sleep();
}

AT(.com_text.pwroff_mode2)
void lowpwr_pwroff_mode2_do(void)
{
    u8 *src, *dst;
    u32 i;
    void (*p_func)(void);

    p_func = (void (*)(void))CACHE_ADDR;

    saradc_deinit();

    PICCONCLR = BIT(0);     //disable global interrupt
    NMICON = 0;

    RTCCON9 = 0x1fff;       //Clr wakeup pending
    RTCCON10 = 0xffff;

    PWRCON1 &= ~(0xf<<14);  //disable flash power gate
    PWRCON1 |= BIT(18);

    lp_port_init();

    RTCCON5 &= ~(1<<12);   //CLEAR LP FLAG
    WKUPCPND = 0xFF<<16;

    //copy pwroff_mode2_final to cache ram

    dst = (u8*) CACHE_ADDR;
    src = (u8*) (&pwroff_mode2_final);
    for(i = 0; i < 0x300; i++){
        *dst++ = *src++;
    }

    //jmp to run lp_sniff_final()
    p_func();
}


AT(.com_sleep.lv2)
WEAK void lowpwr_sleep_sfr_user_save(void)
{

}

AT(.com_sleep.lv2)
WEAK void lowpwr_sleep_sfr_user_resume(void)
{

}


AT(.com_sleep.lv2_do)
void lp_sleep_lv2_do(void)
{
    u32 rtccon3 = RTCCON3;

    // 1. disable clock
    CLKGAT0 &= ~(BIT(15) | BIT(3) | BIT(2)); //lp clk, sram0/1 disable clock
    //RTCCON4 &= ~(0x3<<22);                //VRTC24 to 1.7V

    // 2. disable interface
    //    disable if, then deep sleep memory, can not do in 1 cycle
    RTCCON5 &= ~(1<<11);    // dis sram if
    //RTCCON5 = (RTCCON5 & ~(0x7<<13)) | (1<<13);
    RTCCON5 |=  (2<<13);    // 1xx : shutdown
                            // 01x : deep sleep
                            // 001 : light sleep
                            // 000 : normal
    RTCCON5 |= (1<<17);     // set lp_flag

    RTCCON5 |= BIT(0);

    WDT_DIS();

    // pmu power down ...
    rtccon3 &= ~(1<<5);     // VDDCORE and VLCORE
    rtccon3 |= ((1<<3) | (1<<4));      // VLCORE LDO enable, RI_PD_CORE2 from rtc
    rtccon3 &= ~(0x7<<0);   // VDDBT/VDDIO/VDDCORE LDO disable

    RTCCON |= BIT(5);       //RTC wake up power down mode reset system enable
    RTCCON3 = rtccon3;
//    RTCCON3 |= (1<<6);          // PD CORE

    LPMCON |= BIT(0);
    LPMCON |= BIT(1);       //idle mode
    asm("nop");
    asm("nop");
    asm("nop");
    while(1);
}

AT(.com_sleep.lv2_proc)
void lp_sleep_lv2_proc(void)
{
    u32 rtccon0 = RTCCON0;
    u32 *dst = (u32 *)0x70000;
    u32 *src = (u32 *)&lp_sleep_lv2_do;

    for(uint i=0; i<512/4; i++) {
        *dst++ = *src++;
    }

    // 1, save IO
    CLKGAT0 |= (1<<15);         // LP_CLK Enable
    io_sleep();
    WKUPCPND = 0xFF<<16;
    wkupedg_bak = 0;

    sfr_backup.gpioa_fen = GPIOAFEN;
    sfr_backup.gpiob_fen = GPIOBFEN;
    sfr_backup.gpioa_fdiren = GPIOAFDIREN;
    sfr_backup.gpiob_fdiren = GPIOBFDIREN;
    sfr_backup.gpioa_drv = GPIOADRV;
    sfr_backup.gpiob_drv = GPIOBDRV;

    RTCCON9 = 0x1fff;           //Clr wakeup pending
    RTCCON10 = 0xffff;

    // 2, analog disable
    RTCCON4 &= ~(BIT(17) | BIT(18));    //FGS,FGBUF

    RTCCON8 &= ~BIT(29);
    RTCCON3 |= BIT(7);      //enable vddio aon ldo

    RTCCON4 = (RTCCON4 & ~(0x3f<<24)) | (6<<24) | (6<<27);                  //no sfr access, bit26~bit24, VRTC11 is 0.83V; sfr access, bit29~27, VRTC11 is 1.1V

    // PG OFF
    PWRCON1 |= BIT(9) | BIT(18);            // SD/FS PG PD
    PWRCON1  = PWRCON1 & ~((0x3f << 10)) ;  // PG off
    // ... other analog pd

    if(RTCCON3 & BIT(14)){
        rtccon0 = (rtccon0 & ~(0x03 << 12)) | (0x02 << 12); //CLK2M in Touch select SNF_RC_RTC
    }

    if(RTCCON3 & BIT(16)){
        rtccon0 = (rtccon0 & ~(0x03 << 14)) | (0x02 << 14); //CLK2M in Keyscan select SNF_RC_RTC
    }

    rtccon0 = (rtccon0 & ~(0x7<<22)) | (1<<22);     //sys reset delay

    RTCCON0 = rtccon0;

    // 3, select sniff rc clock and disable pll,xosc
    CLKCON0  = (CLKCON0 & ~(3<<2)) | (0<<2); // sel rc2m
    asm("nop");
    asm("nop");
    asm("nop");

    // 4, PLL, XOSC, CLK2M_BT off
    PLL0CON  = 0;
    PLL0CON1 = 0;
    XOSCCON = 0;

    //RTCCON3 |= BIT(3) | BIT(5);             //enable RL2VLC & C2VLC
    //PWRCON0 = (PWRCON0 & ~0x1f) | 0x8;      //VDDCORE is 0.9V

    asm("j 0x70000");
};

AT(.com_sleep.lv2_wakeup)
void lp_wakeup_lv2_proc(void)
{
    bool gpio_pending = true;

    RTCCON4 &= ~BIT(6);                     //VDDBT to VCORE dis

    RTCCON4 |= BIT(5) ;                     //VCORE from VDDIO

    PICADR = (u32)&__comm_vma;

    if(!(sfr_backup.rtccon5 & BIT(0))){
        PWRCON1 = (PWRCON1 & ~(0X3<<2)) | (0X1<<2);       // DI_CLIMIT_S   109:85:366mA
        RTCCON5 &= ~BIT(0);
    }

    PWRCON0 |= BIT(20);                     //pmu normal

    wkupedg_bak = WKUPEDG;

    //pending is generated due to burrs after the gpio is wakeup from sleep. In this case, manually clear pending
    if(!(RTCCON9 & WK_LP_GPIO) && !(WKUPEDG & 0xff0000)){
        gpio_pending = false;
    }

    //resume IO
    io_resume();

    if (gpio_pending == false) {
        WKUPCPND = 0xFF<<16;
    }

    GPIOAFEN = sfr_backup.gpioa_fen;
    GPIOBFEN = sfr_backup.gpiob_fen;
    GPIOAFDIREN = sfr_backup.gpioa_fdiren;
    GPIOBFDIREN = sfr_backup.gpiob_fdiren;
    GPIOADRV = sfr_backup.gpioa_drv;
    GPIOBDRV = sfr_backup.gpiob_drv;


    // 2, analog init
    RTCCON3 |= BIT(0);                                          //VDDDBT
    PWRCON0 = (PWRCON0 & ~0x1f) | pmu_get_vddcore();            //VDDCORE is 1.15V
    asm("nop");

    if(((RTCCON9 & WK_LP_ALARM) == 0) && ((RTCCON10 & WK_LP_ALARM) == 0)){
        RTCCPND |= BIT(17) | BIT(18);
    }

    PLL0CON |= BIT(12);                         //DI_EN_LDO
    delay(10);

    RSTCON0 = sfr_backup.rstcon0;               //pllsdm reset release
    delay(25);                                  //delay 100us, by rc


    // 3, PLL,XOSC,CLK2M_BT, on
    XOSCCON = sfr_backup.xosccon;
    PLL0CON  = sfr_backup.pll0con;
    PLL0CON1 = sfr_backup.pll0con1;

    delay(700);                                 //delay 1ms - 2ms for xosc to work

    CLKDIVCON0 = sfr_backup.clkdivcon0;

    // 4, resume CLK
    CLKCON1 = sfr_backup.clkcon1;               //xosc_mux_clk select xosc24m
    asm("nop");
    asm("nop");
    asm("nop");

    CLKCON0 = sfr_backup.clkcon0;               //sysclk select xosc_mux_clk
    asm("nop");
    asm("nop");

    if (CLKGAT0 & BIT(22)) {
        AUANGCON0 = 0x100;                                // MICLDO bypass AVDD enable
        AUANGCON1 = 0x400;                                // VBG_ADV = 0.92VBG
    } else {
        CLKGAT0 |= BIT(22);                                 // Enable sdadcl_clken
        delay(10);
        AUANGCON0 = 0x100;                                // MICLDO bypass AVDD enable
        AUANGCON1 = 0x400;                                // VBG_ADV = 0.92VBG
        CLKGAT0 &= ~BIT(22);                                // Disable sdadcl_clken
    }
    LVDCON &= ~BIT(30);                         //Enable reset pending
}

AT(.com_sleep.lv2_proc)
WEAK void lowpwr_sleep_lv2_proc(void)
{
    sfr_backup.piccon  = PICCON;
    sfr_backup.picen   = PICEN;
    sfr_backup.picpr   = PICPR;
    sfr_backup.nmicon  = NMICON;

    sfr_backup.clkgat0 = CLKGAT0;
    sfr_backup.clkgat1 = CLKGAT1;
    sfr_backup.clkgat2 = CLKGAT2;

    sfr_backup.pwrcon0 = PWRCON0 & ~BIT(20);
    sfr_backup.pwrcon1 = PWRCON1;
    sfr_backup.pwrcon2 = PWRCON2;

    sfr_backup.clkcon0 = CLKCON0;
    sfr_backup.clkcon1 = CLKCON1;
    sfr_backup.clkdivcon0 = CLKDIVCON0;

    sfr_backup.xosccon = XOSCCON;
    sfr_backup.pll0con = PLL0CON;
    sfr_backup.pll0con1 = PLL0CON1;

    sfr_backup.rstcon0 = RSTCON0;
    sfr_backup.rtccon = RTCCON;
    sfr_backup.rtccon5 = RTCCON5;

    lowpwr_sleep_sfr_save();
    lowpwr_sleep_sfr_user_save();

    u32 gpiogde = GPIOGDE;
    u32 gpiogpu = GPIOGPU;
    if(pmu_vddio_to_flash()){
        GPIOGDE = BIT(2);                      //SPICS
        GPIOGPU = BIT(2);
    }else{
        GPIOGDE = 0;
        GPIOGPU = 0;
        PWRCON1 &= ~(0XF<<14);                          //DI_SW DISABLE
    }

    sfr_backup.rtccon0 = RTCCON0;
    sfr_backup.rtccon12 = RTCCON12;

    if(RTCCON3 & (BIT(8) | BIT(9) | BIT(13) | BIT(14) | BIT(16))){      //wakeup source need rc2m clk
        RTCCON0 |= BIT(19);                         //RC2M_RTC enable
    }else{
        RTCCON0  &= ~BIT(19);                       //RC2M_RTC disable
    }

    lp_sleep(lp_sleep_lv2_proc, lp_wakeup_lv2_proc);

    RTCCON4 |= BIT(5) | BIT(6);                     //VCORE from VDDBT & VDDIO

    RTCCON12 = sfr_backup.rtccon12;
    RTCCON0 = sfr_backup.rtccon0;
    asm("nop");
    asm("nop");

    PWRCON1 = sfr_backup.pwrcon1 | BIT(7);
    PWRCON2 = sfr_backup.pwrcon2;
    PWRCON0 = sfr_backup.pwrcon0 | BIT(20);

    CLKGAT0 = sfr_backup.clkgat0;
    CLKGAT1 = sfr_backup.clkgat1;
    CLKGAT2 = sfr_backup.clkgat2;

	lowpwr_sleep_sfr_resume();
    lowpwr_sleep_sfr_user_resume();

    GPIOGDE = gpiogde;
    GPIOGPU = gpiogpu;

    RTCCON = sfr_backup.rtccon;

	PICCON  = sfr_backup.piccon;
	PICPR   = sfr_backup.picpr;
	PICEN   = sfr_backup.picen;
	NMICON  = sfr_backup.nmicon;

    if(sfr_backup.rtccon5 & BIT(0)){
        RTCCON4 &= ~BIT(6);                         //LDO Mode,VCORE disable VDDBT, from VDDIO
    }else{
        RTCCON4 &= ~BIT(5);                         //Buck Mode,VCORE disable VDDIO, from VDDBT
    }

    PWRCON1 &= ~BIT(7);


}

AT(.com_sleep.sleep)
void lowpwr_sleep_proc(u8 lpclk_type)
{
    lowpwr_sleep_lv2_proc();
}

