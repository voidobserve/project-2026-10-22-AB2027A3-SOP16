/**
******************************************************************************************************************************
*
*@file driver_touch_key.h
*
*@brief Header file for Touch Key Driver
*
*@Create date: 2023-03-23
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _TOUCH_KEY_H_
#define _TOUCH_KEY_H_

#ifdef __cpluscplus
extern "C" {
#endif

/*
 * INCLUDE FILES
 ***************************************************************************
 */
#include "driver_com.h"

/*
 * VARIABLE DEFINITION
 ***************************************************************************
 */

#define SUPPORT_TOUCH_KEY_NUM           4

typedef struct {
    u16 cdpr;
    u8  type;                           //0: disable channel, 1: touch key channel, 2: touch ear channel
    u8  ctrim;                          //cur channel ctrim select
    u8  itrim;                          //cur channel itrim select
    u16 tkpthd;                         // TK Press Threshold
    u16 tkrthd;                         // TK Release Threshold
} tkey_ch_t;

typedef struct {
    const tkey_ch_t *key[SUPPORT_TOUCH_KEY_NUM];
    union {
        struct {
            u32 fil_low     :  4;       //touch key state low state filter length, fil_low + 1
            u32 fil_high    :  4;       //touch key state high state filter length, fil_high + 1
            u32 fil_except  :  8;       //touch key state exception filter length, fil_except + 1
            u32 fil_val     :  4;       //touch key base_cnt valid filter length, fil_val + 1
            u32 to_except   :  12;      //touch key state high timeout length
        };
        u32 reg_tktmr;
    };
    union {
        struct {
            u32 tkpthd      :   12;     //touch key press threshold
            u32 resv0       :   4;
            u32 tkrthd      :   12;     //touch key release threshold
            u32 pto_except  :   4;      //touch key press time out length. (pto_except << 8)
        };
        u32 reg_tkpthd;
    };
    union {
        struct {
            u32 tksthd      :   12;     //touch key smaller threshold
            u32 resv1       :   4;
            u32 tklthd      :   12;     //touch key larger threshold
            u32 resv2       :   4;
        };
        u32 reg_tkethd;
    };

    union {
        struct {
            u32 tkvthd      :   16;     //touch key variance threshold
            u32 val         :   16;     //touch key variance
        };
        u32 reg_tkvari;
    };

    union {
        struct {
            u32 tkarthd     :   12;     //touch key average range threshold
            u32 tkaethd     :   4;      //touch key average equal threshold
            u32 tkvfil      :   8;      //touch key variance filter count
            u32 tkbadd      :   8;      //touch key base counter adder value
        };
        u32 reg_tkvarithd;
    };

    union {
        struct {
            u32 press_vari_en   :   1;  //press variance enable bit
            u32 rels_vari_en    :   1;  //release variance enable bit
            u32 press_fil_sel   :   1;  //press variance after filter
            u32 rels_fil_sel    :   1;  //release variance after filter
            u32 press_vari_thd  :   12; //press variance threshold
            u32 rels_vari_thd   :   12; //release variance threshold
            u32 to_bcnt_thd     :   4;  //time out base counter threshold
        };
        u32 reg_tkcon2;
    };


    //in ear
    union {
        struct {
            u32 ear_fil_low     :  4;
            u32 ear_fil_high    :  4;
            u32 ear_fil_except  :  8;
            u32 ear_fil_val     :  4;
            u32 tkpwup          :  6;
        };
        u32 reg_tetmr;
    };
    union {
        struct {
            u32 tepthd      :   12;     //touch ear press threshold
            u32 resv3       :   4;
            u32 terthd      :   12;     //touch ear release threshold
            u32 tefathd     :   4;
        };
        u32 reg_tepthd;
    };
    union {
        struct {
            u32 testhd      :   12;     //touch ear smaller threshold
            u32 resv5       :   4;
            u32 telthd      :   12;     //touch ear larger threshold
            u32 resv6       :   4;
        };
        u32 reg_teethd;
    };
} tkey_cfg_t;

typedef struct {
    u8  ch;                     //channel index
    u8  cnt;
    u8  limit;                  //方差阈值
    u8  stable_cnt;
    u8  te_flag;                //是否为入耳检测
    u8  range_thresh;           //rang校准的上限阈值
    u16 avg;                    //平均值
    u16 buf[8];
    u32 anov_cnt;               //满足方差条件计数
    psfr_t bcnt_sfr;            //BCNT寄存器
    u8  fil_except;
    u8  range_en;
    u16 temp_tkcnt;
    u32 tick;
    u16 to_cnt;                 //定时校准时间
    u8 soft_stable_cnt;         // 用于软件校准，稳定次数
    u8  resv[6];
    u8 touch_key_sta[SUPPORT_TOUCH_KEY_NUM];
} tk_cb_t;

/*
 * REGISTER DEFINITION
 ***************************************************************************
 */
// Touch Key Control Register 0
#define ADDR_REG_TKCON                          *(volatile unsigned long*) (SFR10_BASE + 0x02*4)
// Touch Key base_cnt Valid
#define POS_REG_TKCON_TKBCNTVAL                 27
#define BIT_REG_TKCON_TKBCNTVAL                 0x8000000
#define SETF_REG_TKCON_TKBCNTVAL()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKBCNTVAL
#define CLRF_REG_TKCON_TKBCNTVAL()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKBCNTVAL
#define GETF_REG_TKCON_TKBCNTVAL()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKBCNTVAL) >> POS_REG_TKCON_TKBCNTVAL)
// Touch Key Timeout Pending Update base_cnt Enable
#define POS_REG_TKCON_TKTOUPDBEN                26
#define BIT_REG_TKCON_TKTOUPDBEN                0x4000000
#define SETF_REG_TKCON_TKTOUPDBEN()             ADDR_REG_TKCON |= BIT_REG_TKCON_TKTOUPDBEN
#define CLRF_REG_TKCON_TKTOUPDBEN()             ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKTOUPDBEN
#define GETF_REG_TKCON_TKTOUPDBEN()             ((ADDR_REG_TKCON & BIT_REG_TKCON_TKTOUPDBEN) >> POS_REG_TKCON_TKTOUPDBEN)
// Touch Key Range Pending Update base_cnt Enable
#define POS_REG_TKCON_TKRUPDBEN                 25
#define BIT_REG_TKCON_TKRUPDBEN                 0x2000000
#define SETF_REG_TKCON_TKRUPDBEN()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKRUPDBEN
#define CLRF_REG_TKCON_TKRUPDBEN()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKRUPDBEN
#define GETF_REG_TKCON_TKRUPDBEN()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKRUPDBEN) >> POS_REG_TKCON_TKRUPDBEN)
// Touch Key Large Pending Update base_cnt Enable
#define POS_REG_TKCON_TKLUPDBEN                 24
#define BIT_REG_TKCON_TKLUPDBEN                 0x1000000
#define SETF_REG_TKCON_TKLUPDBEN()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKLUPDBEN
#define CLRF_REG_TKCON_TKLUPDBEN()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKLUPDBEN
#define GETF_REG_TKCON_TKLUPDBEN()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKLUPDBEN) >> POS_REG_TKCON_TKLUPDBEN)
// Touch Key Small Pending Update base_cnt Enable
#define POS_REG_TKCON_TKSUPDBEN                 23
#define BIT_REG_TKCON_TKSUPDBEN                 0x800000
#define SETF_REG_TKCON_TKSUPDBEN()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKSUPDBEN
#define CLRF_REG_TKCON_TKSUPDBEN()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKSUPDBEN
#define GETF_REG_TKCON_TKSUPDBEN()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKSUPDBEN) >> POS_REG_TKCON_TKSUPDBEN)
// Touch Key State Wakeup Enable
#define POS_REG_TKCON_TKSTAWKEN                 21
#define BIT_REG_TKCON_TKSTAWKEN                 0x200000
#define SETF_REG_TKCON_TKSTAWKEN()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKSTAWKEN
#define CLRF_REG_TKCON_TKSTAWKEN()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKSTAWKEN
#define GETF_REG_TKCON_TKSTAWKEN()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKSTAWKEN) >> POS_REG_TKCON_TKSTAWKEN)
// Touch Key Pending Wakeup Enable
#define POS_REG_TKCON_TKPNDWKEN                 20
#define BIT_REG_TKCON_TKPNDWKEN                 0x100000
#define SETF_REG_TKCON_TKPNDWKEN()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKPNDWKEN
#define CLRF_REG_TKCON_TKPNDWKEN()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKPNDWKEN
#define GETF_REG_TKCON_TKPNDWKEN()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKPNDWKEN) >> POS_REG_TKCON_TKPNDWKEN)
// Touch Key Press Timeout Pending Update base_cnt Enable
#define POS_REG_TKCON_TKPTOUPDBEN               19
#define BIT_REG_TKCON_TKPTOUPDBEN               0x80000
#define SETF_REG_TKCON_TKPTOUPDBEN()            ADDR_REG_TKCON |= BIT_REG_TKCON_TKPTOUPDBEN
#define CLRF_REG_TKCON_TKPTOUPDBEN()            ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKPTOUPDBEN
#define GETF_REG_TKCON_TKPTOUPDBEN()            ((ADDR_REG_TKCON & BIT_REG_TKCON_TKPTOUPDBEN) >> POS_REG_TKCON_TKPTOUPDBEN)
// Touch Key Mode Select
#define POS_REG_TKCON_TK_MODE                   18
#define BIT_REG_TKCON_TK_MODE                   0x40000
#define SETF_REG_TKCON_TK_MODE()                ADDR_REG_TKCON |= BIT_REG_TKCON_TK_MODE
#define CLRF_REG_TKCON_TK_MODE()                ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK_MODE
#define GETF_REG_TKCON_TK_MODE()                ((ADDR_REG_TKCON & BIT_REG_TKCON_TK_MODE) >> POS_REG_TKCON_TK_MODE)
// Touch Key Software Control Select
#define POS_REG_TKCON_TKSWSEL                   17
#define BIT_REG_TKCON_TKSWSEL                   0x20000
#define SETF_REG_TKCON_TKSWSEL()                ADDR_REG_TKCON |= BIT_REG_TKCON_TKSWSEL
#define CLRF_REG_TKCON_TKSWSEL()                ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKSWSEL
#define GETF_REG_TKCON_TKSWSEL()                ((ADDR_REG_TKCON & BIT_REG_TKCON_TKSWSEL) >> POS_REG_TKCON_TKSWSEL)
// Touch Key Analog Fix Channel Enable
#define POS_REG_TKCON_TKCH_FIX                  16
#define BIT_REG_TKCON_TKCH_FIX                  0x10000
#define SETF_REG_TKCON_TKCH_FIX()               ADDR_REG_TKCON |= BIT_REG_TKCON_TKCH_FIX
#define CLRF_REG_TKCON_TKCH_FIX()               ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKCH_FIX
#define GETF_REG_TKCON_TKCH_FIX()               ((ADDR_REG_TKCON & BIT_REG_TKCON_TKCH_FIX) >> POS_REG_TKCON_TKCH_FIX)
// Touch Key Analog Reset Disable
#define POS_REG_TKCON_TKRST_DIS                 15
#define BIT_REG_TKCON_TKRST_DIS                 0x8000
#define SETF_REG_TKCON_TKRST_DIS()              ADDR_REG_TKCON |= BIT_REG_TKCON_TKRST_DIS
#define CLRF_REG_TKCON_TKRST_DIS()              ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKRST_DIS
#define GETF_REG_TKCON_TKRST_DIS()              ((ADDR_REG_TKCON & BIT_REG_TKCON_TKRST_DIS) >> POS_REG_TKCON_TKRST_DIS)
// Touch Key Analog Hardware Auto Enable Select
#define POS_REG_TKCON_TKAEN_ATSEL               14
#define BIT_REG_TKCON_TKAEN_ATSEL               0x4000
#define SETF_REG_TKCON_TKAEN_ATSEL()            ADDR_REG_TKCON |= BIT_REG_TKCON_TKAEN_ATSEL
#define CLRF_REG_TKCON_TKAEN_ATSEL()            ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKAEN_ATSEL
#define GETF_REG_TKCON_TKAEN_ATSEL()            ((ADDR_REG_TKCON & BIT_REG_TKCON_TKAEN_ATSEL) >> POS_REG_TKCON_TKAEN_ATSEL)
// Touch Key Internal Channel 3 Enable
#define POS_REG_TKCON_TK3_EN                    13
#define BIT_REG_TKCON_TK3_EN                    0x2000
#define SETF_REG_TKCON_TK3_EN()                 ADDR_REG_TKCON |= BIT_REG_TKCON_TK3_EN
#define CLRF_REG_TKCON_TK3_EN()                 ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK3_EN
#define GETF_REG_TKCON_TK3_EN()                 ((ADDR_REG_TKCON & BIT_REG_TKCON_TK3_EN) >> POS_REG_TKCON_TK3_EN)
// Touch Key Internal Channel 2 Enable
#define POS_REG_TKCON_TK2_EN                    12
#define BIT_REG_TKCON_TK2_EN                    0x1000
#define SETF_REG_TKCON_TK2_EN()                 ADDR_REG_TKCON |= BIT_REG_TKCON_TK2_EN
#define CLRF_REG_TKCON_TK2_EN()                 ADDR_REG_TKCON &=~BIT_REG_TKCON_TK2_EN
#define GETF_REG_TKCON_TK2_EN()                  ((ADDR_REG_TKCON & BIT_REG_TKCON_TK2_EN) >> POS_REG_TKCON_TK2_EN)
// Touch Key Internal Channel 1 Enable
#define POS_REG_TKCON_TK1_EN                    11
#define BIT_REG_TKCON_TK1_EN                    0x800
#define SETF_REG_TKCON_TK1_EN()                 ADDR_REG_TKCON |= BIT_REG_TKCON_TK1_EN
#define CLRF_REG_TKCON_TK1_EN()                 ADDR_REG_TKCON &=~BIT_REG_TKCON_TK1_EN
#define GETF_REG_TKCON_TK1_EN()                 ((ADDR_REG_TKCON & BIT_REG_TKCON_TK1_EN) >> POS_REG_TKCON_TK1_EN)
// Touch Key Internal Channel 0 Enable
#define POS_REG_TKCON_TK0_EN                    10
#define BIT_REG_TKCON_TK0_EN                    0x400
#define SETF_REG_TKCON_TK0_EN()                 ADDR_REG_TKCON |= BIT_REG_TKCON_TK0_EN
#define CLRF_REG_TKCON_TK0_EN()                 ADDR_REG_TKCON &=~BIT_REG_TKCON_TK0_EN
#define GETF_REG_TKCON_TK0_EN()                 ((ADDR_REG_TKCON & BIT_REG_TKCON_TK0_EN) >> POS_REG_TKCON_TK0_EN)
// Touch Key 3 Pending Enable
#define POS_REG_TKCON_TK3_PND_EN                9
#define BIT_REG_TKCON_TK3_PND_EN                0x200
#define SETF_REG_TKCON_TK3_PND_EN()             ADDR_REG_TKCON |= BIT_REG_TKCON_TK3_PND_EN
#define CLRF_REG_TKCON_TK3_PND_EN()             ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK3_PND_EN
#define GETF_REG_TKCON_TK3_PND_EN()             ((ADDR_REG_TKCON & BIT_REG_TKCON_TK3_PND_EN) >> POS_REG_TKCON_TK3_PND_EN)
// Touch Key 2 Pending Enable
#define POS_REG_TKCON_TK2_PND_EN                8
#define BIT_REG_TKCON_TK2_PND_EN                0x100
#define SETF_REG_TKCON_TK2_PND_EN()             ADDR_REG_TKCON |= BIT_REG_TKCON_TK2_PND_EN
#define CLRF_REG_TKCON_TK2_PND_EN()             ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK2_PND_EN
#define GETF_REG_TKCON_TK2_PND_EN()             ((ADDR_REG_TKCON & BIT_REG_TKCON_TK2_PND_EN) >> POS_REG_TKCON_TK2_PND_EN)
// Touch Key 1 Pending Enable
#define POS_REG_TKCON_TK1_PND_EN                7
#define BIT_REG_TKCON_TK1_PND_EN                0x80
#define SETF_REG_TKCON_TK1_PND_EN()             ADDR_REG_TKCON |= BIT_REG_TKCON_TK1_PND_EN
#define CLRF_REG_TKCON_TK1_PND_EN()             ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK1_PND_EN
#define GETF_REG_TKCON_TK1_PND_EN()             ((ADDR_REG_TKCON & BIT_REG_TKCON_TK1_PND_EN) >> POS_REG_TKCON_TK1_PND_EN)
// Touch Key 0 Pending Enable
#define POS_REG_TKCON_TK0_PND_EN                6
#define BIT_REG_TKCON_TK0_PND_EN                0x40
#define SETF_REG_TKCON_TK0_PND_EN()             ADDR_REG_TKCON |= BIT_REG_TKCON_TK0_PND_EN
#define CLRF_REG_TKCON_TK0_PND_EN()             ADDR_REG_TKCON &= ~BIT_REG_TKCON_TK0_PND_EN
#define GETF_REG_TKCON_TK0_PND_EN()             ((ADDR_REG_TKCON & BIT_REG_TKCON_TK0_PND_EN) >> POS_REG_TKCON_TK0_PND_EN)
// Touch Key Pin Select
#define POS_REG_TKCON_TKSEL                     4
#define BIT_REG_TKCON_TKSEL                     0x30
#define SET_REG_TKCON_TKSEL(val)                ADDR_REG_TKCON = ((ADDR_REG_TKCON & (~BIT_REG_TKCON_TKSEL)) | ((val) << POS_REG_TKCON_TKSEL))
#define GET_REG_TKCON_TKSEL()                   ((ADDR_REG_TKCON & BIT_REG_TKCON_TKSEL) >> POS_REG_TKCON_TKSEL)
// Touch Key Or Touch Ear New base_cnt Select BIT For Small Update
#define POS_REG_TKCON_BCNTSEL                   1
#define BIT_REG_TKCON_BCNTSEL                   0x2
#define SETF_REG_TKCON_BCNTSEL()                ADDR_REG_TKCON |= BIT_REG_TKCON_BCNTSEL
#define CLRF_REG_TKCON_BCNTSEL()                ADDR_REG_TKCON &= ~BIT_REG_TKCON_BCNTSEL
#define GETF_REG_TKCON_BCNTSEL()                ((ADDR_REG_TKCON & BIT_REG_TKCON_BCNTSEL) >> POS_REG_TKCON_BCNTSEL)
// Touch Key Enable
#define POS_REG_TKCON_TKEN                      0
#define BIT_REG_TKCON_TKEN                      0x1
#define SETF_REG_TKCON_TKEN()                   ADDR_REG_TKCON |= BIT_REG_TKCON_TKEN
#define CLRF_REG_TKCON_TKEN()                   ADDR_REG_TKCON &= ~BIT_REG_TKCON_TKEN
#define GETF_REG_TKCON_TKEN()                   ((ADDR_REG_TKCON & BIT_REG_TKCON_TKEN) >> POS_REG_TKCON_TKEN)

// Touch Key Control Register 1
#define ADDR_REG_TKCON1                          *(volatile unsigned long*) (SFR10_BASE + 0x03*4)
// Touch Key CLK2M Divide 2 Select
#define POS_REG_TKCON1_DIV2SEL                  25
#define BIT_REG_TKCON1_DIV2SEL                  0x2000000
#define SETF_REG_TKCON1_DIV2SEL()               ADDR_REG_TKCON1 |= BIT_REG_TKCON1_DIV2SEL
#define CLRF_REG_TKCON1_DIV2SEL()               ADDR_REG_TKCON1 &= ~BIT_REG_TKCON1_DIV2SEL
#define GETF_REG_TKCON1_DIV2SEL()               ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_DIV2SEL) >> POS_REG_TKCON1_DIV2SEL)
// Touch Key Clock Enable
#define POS_REG_TKCON1_TKCEN                    23
#define BIT_REG_TKCON1_TKCEN                    0x800000
#define SETF_REG_TKCON1_TKCEN()                 ADDR_REG_TKCON1 |= BIT_REG_TKCON1_TKCEN
#define CLRF_REG_TKCON1_TKCEN()                 ADDR_REG_TKCON1 &= ~BIT_REG_TKCON1_TKCEN
#define GETF_REG_TKCON1_TKCEN()                 ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_TKCEN) >> POS_REG_TKCON1_TKCEN)
// Touch Key Variance Clock Enable
#define POS_REG_TKCON1_TKVCEN                   22
#define BIT_REG_TKCON1_TKVCEN                   0x400000
#define SETF_REG_TKCON1_TKVCEN()                ADDR_REG_TKCON1 |= BIT_REG_TKCON1_TKVCEN
#define CLRF_REG_TKCON1_TKVCEN()                ADDR_REG_TKCON1 &= ~BIT_REG_TKCON1_TKVCEN
#define GETF_REG_TKCON1_TKVCEN()                ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_TKVCEN) >> POS_REG_TKCON1_TKVCEN)
// Touch Key Each Channel Time. Counter Clock Is 256 Divider From CLK2M.
#define POS_REG_TKCON1_TKCLK2MDIV               16
#define BIT_REG_TKCON1_TKCLK2MDIV               0x1F0000
#define SET_REG_TKCON1_TKCLK2MDIV(val)          ADDR_REG_TKCON1 = ((ADDR_REG_TKCON1 & (~BIT_REG_TKCON1_TKCLK2MDIV)) | ((val) << POS_REG_TKCON1_TKCLK2MDIV))
#define GET_REG_TKCON1_TKCLK2MDIV()             ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_TKCLK2MDIV) >> POS_REG_TKCON1_TKCLK2MDIV)
// Touch Key Analog Enable
#define POS_REG_TKCON1_TKAEN                    13
#define BIT_REG_TKCON1_TKAEN                    0x2000
#define SETF_REG_TKCON1_TKAEN()                 ADDR_REG_TKCON1 |= BIT_REG_TKCON1_TKAEN
#define CLRF_REG_TKCON1_TKAEN()                 ADDR_REG_TKCON1 &= ~BIT_REG_TKCON1_TKAEN
#define GETF_REG_TKCON1_TKAEN()                 ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_TKAEN) >> POS_REG_TKCON1_TKAEN)
// Touch Key---Reference to pinlist, TK Reference Voltage Select Bit, 0x00/0x01/0x10/0x11-1.0/1.1/1.2/1.3
#define POS_REG_TKCON1_TKARTRIM                 0
#define BIT_REG_TKCON1_TKARTRIM                 0x3
#define SET_REG_TKCON1_TKARTRIM(val)            ADDR_REG_TKCON1 = ((ADDR_REG_TKCON1 & (~BIT_REG_TKCON1_TKARTRIM)) | ((val) << POS_REG_TKCON1_TKARTRIM))
#define GET_REG_TKCON1_TKARTRIM()               ((ADDR_REG_TKCON1 & BIT_REG_TKCON1_TKARTRIM) >> POS_REG_TKCON1_TKARTRIM)

// Touch Key Charge And Discharge Period Register 0, ###Read only
#define ADDR_REG_TKCDPR0                        *(volatile unsigned long*) (SFR10_BASE + 0x05*4)
// Touch Key Channel 1 Charge And Discharge Period, Can't Be Longer Than 500Hz
// (TKCDPR + 1) * (One Cycle TKCNT) * (CLK2M_TK Frequency)
#define POS_REG_TKCDPR0_TKCDPR1                 16
#define BIT_REG_TKCDPR0_TKCDPR1                 0x3FF0000
//#define SET_REG_TKCDPR0_TKCDPR1(val)            ADDR_REG_TKCDPR0 = ((ADDR_REG_TKCDPR0 & (~BIT_REG_TKCDPR0_TKCDPR1)) | ((val) << POS_REG_TKCDPR0_TKCDPR1))
//#define GET_REG_TKCDPR0_TKCDPR1()               ((ADDR_REG_TKCDPR0 & BIT_REG_TKCDPR0_TKCDPR1) >> POS_REG_TKCDPR0_TKCDPR1)
// Touch Key Channel 0 Charge And Discharge Period, Can't Be Longer Than 500Hz
// (TKCDPR + 1) * (One Cycle TKCNT) * (CLK2M_TK Frequency)
#define POS_REG_TKCDPR0_TKCDPR0                 0
#define BIT_REG_TKCDPR0_TKCDPR0                 0x3FF
//#define SET_REG_TKCDPR0_TKCDPR0(val)            ADDR_REG_TKCDPR0 = ((ADDR_REG_TKCDPR0 & (~BIT_REG_TKCDPR0_TKCDPR0)) | ((val) << POS_REG_TKCDPR0_TKCDPR0))
//#define GET_REG_TKCDPR0_TKCDPR0()               ((ADDR_REG_TKCDPR0 & BIT_REG_TKCDPR0_TKCDPR0) >> POS_REG_TKCDPR0_TKCDPR0)

// Touch Key Charge And Discharge Period Register 1, ###Read only
#define ADDR_REG_TKCDPR1                        *(volatile unsigned long*) (SFR10_BASE + 0x06*4)
// Touch Key Channel 3 Charge And Discharge Period, Can't Be Longer Than 500Hz
// (TKCDPR + 1) * (One Cycle TKCNT) * (CLK2M_TK Frequency)
#define POS_REG_TKCDPR1_TKCDPR3                 16
#define BIT_REG_TKCDPR1_TKCDPR3                 0x3FF0000
//#define SET_REG_TKCDPR1_TKCDPR3(val)            ADDR_REG_TKCDPR1 = ((ADDR_REG_TKCDPR1 & (~BIT_REG_TKCDPR1_TKCDPR3)) | ((val) << POS_REG_TKCDPR1_TKCDPR3))
//#define GET_REG_TKCDPR1_TKCDPR3()               ((ADDR_REG_TKCDPR1 & BIT_REG_TKCDPR1_TKCDPR3) >> POS_REG_TKCDPR1_TKCDPR3)
// Touch Key Channel 2 Charge And Discharge Period, Can't Be Longer Than 500Hz
// (TKCDPR + 1) * (One Cycle TKCNT) * (CLK2M_TK Frequency)
#define POS_REG_TKCDPR1_TKCDPR2                 0
#define BIT_REG_TKCDPR1_TKCDPR2                 0x3FF
//#define SET_REG_TKCDPR1_TKCDPR2(val)            ADDR_REG_TKCDPR1 = ((ADDR_REG_TKCDPR1 & (~BIT_REG_TKCDPR1_TKCDPR2)) | ((val) << POS_REG_TKCDPR1_TKCDPR2))
//#define GET_REG_TKCDPR1_TKCDPR2()               ((ADDR_REG_TKCDPR1 & BIT_REG_TKCDPR1_TKCDPR2) >> POS_REG_TKCDPR1_TKCDPR2)

// Touch Key Base Counter Register
#define ADDR_REG_TKBCNT                         *(volatile unsigned long*) (SFR10_BASE + 0x09*4)
// Touch Key Base Counter Average Value
#define POS_REG_TKBCNT_TKBCNT_AVG               12
#define BIT_REG_TKBCNT_TKBCNT_AVG               0xFFF000
#define SET_REG_TKBCNT_TKBCNT_AVG(val)          ADDR_REG_TKBCNT = ((ADDR_REG_TKBCNT & (~BIT_REG_TKBCNT_TKBCNT_AVG)) | ((val) << POS_REG_TKBCNT_TKBCNT_AVG))
#define GET_REG_TKBCNT_TKBCNT_AVG()             ((ADDR_REG_TKBCNT & BIT_REG_TKBCNT_TKBCNT_AVG) >> POS_REG_TKBCNT_TKBCNT_AVG)
// Touch Key Base Counter
#define POS_REG_TKBCNT_TKBCNT                   0
#define BIT_REG_TKBCNT_TKBCNT                   0xFFF
#define SET_REG_TKBCNT_TKBCNT(val)              ADDR_REG_TKBCNT = ((ADDR_REG_TKBCNT & (~BIT_REG_TKBCNT_TKBCNT)) | ((val) << POS_REG_TKBCNT_TKBCNT))
#define GET_REG_TKBCNT_TKBCNT()                 ((ADDR_REG_TKBCNT & BIT_REG_TKBCNT_TKBCNT) >> POS_REG_TKBCNT_TKBCNT)

// Touch Key Press Threshold Register
#define ADDR_REG_TKPTHD                         *(volatile unsigned long*) (SFR10_BASE + 0x0a*4)
// Touch Key Press Time Out
#define POS_REG_TKPTHD_PTO                      28
#define BIT_REG_TKPTHD_PTO                      0xF0000000
#define SET_REG_TKPTHD_PTO(val)                 ADDR_REG_TKPTHD = ((ADDR_REG_TKPTHD & (~BIT_REG_TKPTHD_PTO)) | ((val) << POS_REG_TKPTHD_PTO))
#define GET_REG_TKPTHD_PTO()                    ((ADDR_REG_TKPTHD & BIT_REG_TKPTHD_PTO) >> POS_REG_TKPTHD_PTO)
// Touch Key Release Thresohld
#define POS_REG_TKPTHD_RTHD                     16
#define BIT_REG_TKPTHD_RTHD                     0xFFF0000
#define SET_REG_TKPTHD_RTHD(val)                ADDR_REG_TKPTHD = ((ADDR_REG_TKPTHD & (~BIT_REG_TKPTHD_RTHD)) | ((val) << POS_REG_TKPTHD_RTHD))
#define GET_REG_TKPTHD_RTHD()                   ((ADDR_REG_TKPTHD & BIT_REG_TKPTHD_RTHD) >> POS_REG_TKPTHD_RTHD)
// Touch Key Press Thresohld
#define POS_REG_TKPTHD_PTHD                     0
#define BIT_REG_TKPTHD_PTHD                     0xFFF
#define SET_REG_TKPTHD_PTHD(val)                ADDR_REG_TKPTHD = ((ADDR_REG_TKPTHD & (~BIT_REG_TKPTHD_PTHD)) | ((val) << POS_REG_TKPTHD_PTHD))
#define GET_REG_TKPTHD_PTHD()                   ((ADDR_REG_TKPTHD & BIT_REG_TKPTHD_PTHD) >> POS_REG_TKPTHD_PTHD)

// Touch Key Exception Threshold Register
#define ADDR_REG_TKETHD                         *(volatile unsigned long*) (SFR10_BASE + 0x0b*4)
// Touch Key Larger Threshold
#define POS_REG_TKETHD_LTHD                     16
#define BIT_REG_TKETHD_LTHD                     0xFFF0000
#define SET_REG_TKETHD_LTHD(val)                ADDR_REG_TKETHD = ((ADDR_REG_TKETHD & (~BIT_REG_TKETHD_LTHD)) | ((val) << POS_REG_TKETHD_LTHD))
#define GET_REG_TKETHD_LTHD()                   ((ADDR_REG_TKETHD & BIT_REG_TKETHD_LTHD) >> POS_REG_TKETHD_LTHD)
// Touch Key Smaller Threshold
#define POS_REG_TKETHD_STHD                     0
#define BIT_REG_TKETHD_STHD                     0xFFF
#define SET_REG_TKETHD_STHD(val)                ADDR_REG_TKETHD = ((ADDR_REG_TKETHD & (~BIT_REG_TKETHD_STHD)) | ((val) << POS_REG_TKETHD_STHD))
#define GET_REG_TKETHD_STHD()                   ((ADDR_REG_TKETHD & BIT_REG_TKETHD_STHD) >> POS_REG_TKETHD_STHD)

// Touch Key Analog Control Register 0
#define ADDR_REG_TKACON0                        *(volatile unsigned long*) (SFR10_BASE + 0x12*4)
// Touch Key Channel 1 ENCHN
#define POS_REG_TKACON0_ENCHN1                  23
#define BIT_REG_TKACON0_ENCHN1                  0x800000
#define SETF_REG_TKACON0_ENCHN1()               ADDR_REG_TKACON0 |= BIT_REG_TKACON0_ENCHN1
#define CLRF_REG_TKACON0_ENCHN1()               ADDR_REG_TKACON0 &= ~BIT_REG_TKACON0_ENCHN1
#define GETF_REG_TKACON0_ENCHN1()               ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_ENCHN1) >> POS_REG_TKACON0_ENCHN1)
// Touch Key Channel 1 ITRIM Select, TK Charge Current Select Signal, 3.84u/1.92u/0.96u/0.48u/0.24u
#define POS_REG_TKACON0_ITRIM1                  18
#define BIT_REG_TKACON0_ITRIM1                  0x7C0000
#define SET_REG_TKACON0_ITRIM1(val)             ADDR_REG_TKACON0 = ((ADDR_REG_TKACON0 & (~BIT_REG_TKACON0_ITRIM1)) | ((val) << POS_REG_TKACON0_ITRIM1))
#define GET_REG_TKACON0_ITRIM1()                ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_ITRIM1) >> POS_REG_TKACON0_ITRIM1)
// Touch Key Channel 1 CTRIM Select, TK Internal Capacitance Select, 3.2p/1.6p/800f/400f/200f/200f
#define POS_REG_TKACON0_CTRIM1                  12
#define BIT_REG_TKACON0_CTRIM1                  0x3F000
#define SET_REG_TKACON0_CTRIM1(val)             ADDR_REG_TKACON0 = ((ADDR_REG_TKACON0 & (~BIT_REG_TKACON0_CTRIM1)) | ((val) << POS_REG_TKACON0_CTRIM1))
#define GET_REG_TKACON0_CTRIM1()                ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_CTRIM1) >> POS_REG_TKACON0_CTRIM1)
// Touch Key Channel 0 ENCHN
#define POS_REG_TKACON0_ENCHN0                  11
#define BIT_REG_TKACON0_ENCHN0                  0x800
#define SETF_REG_TKACON0_ENCHN0()               ADDR_REG_TKACON0 |= BIT_REG_TKACON0_ENCHN0
#define CLRF_REG_TKACON0_ENCHN0()               ADDR_REG_TKACON0 &= ~BIT_REG_TKACON0_ENCHN0
#define GETF_REG_TKACON0_ENCHN0()               ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_ENCHN0) >> POS_REG_TKACON0_ENCHN0)
// Touch Key Channel 0 ITRIM Select, TK Charge Current Select Signal, 3.84u/1.92u/0.96u/0.48u/0.24u
#define POS_REG_TKACON0_ITRIM0                  6
#define BIT_REG_TKACON0_ITRIM0                  0x7C0
#define SET_REG_TKACON0_ITRIM0(val)             ADDR_REG_TKACON0 = ((ADDR_REG_TKACON0 & (~BIT_REG_TKACON0_ITRIM0)) | ((val) << POS_REG_TKACON0_ITRIM0))
#define GET_REG_TKACON0_ITRIM0()                  ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_ITRIM0) >> POS_REG_TKACON0_ITRIM0)
// Touch Key Channel 0 CTRIM Select, 3.2p/1.6p/800f/400f/200f/200f
#define POS_REG_TKACON0_CTRIM0                  0
#define BIT_REG_TKACON0_CTRIM0                  0x3F
#define SET_REG_TKACON0_CTRIM0(val)             ADDR_REG_TKACON0 = ((ADDR_REG_TKACON0 & (~BIT_REG_TKACON0_CTRIM0)) | ((val) << POS_REG_TKACON0_CTRIM0))
#define GET_REG_TKACON0_CTRIM0()                ((ADDR_REG_TKACON0 & BIT_REG_TKACON0_CTRIM0) >> POS_REG_TKACON0_CTRIM0)

// Touch Key Analog Control Register 1
#define ADDR_REG_TKACON1                        *(volatile unsigned long*) (SFR10_BASE + 0x13*4)
// Touch Key Channel 3 ENCHN
#define POS_REG_TKACON1_ENCHN3                  23
#define BIT_REG_TKACON1_ENCHN3                  0x800000
#define SETF_REG_TKACON1_ENCHN3()               ADDR_REG_TKACON1 |= BIT_REG_TKACON1_ENCHN3
#define CLRF_REG_TKACON1_ENCHN3()               ADDR_REG_TKACON1 &= ~BIT_REG_TKACON1_ENCHN3
#define GETF_REG_TKACON1_ENCHN3()               ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_ENCHN3) >> POS_REG_TKACON1_ENCHN3)
// Touch Key Channel 3 ITRIM Select, TK Charge Current Select Signal, 3.84u/1.92u/0.96u/0.48u/0.24u
#define POS_REG_TKACON1_ITRIM3                  18
#define BIT_REG_TKACON1_ITRIM3                  0x7C0000
#define SET_REG_TKACON1_ITRIM3(val)             ADDR_REG_TKACON1 = ((ADDR_REG_TKACON1 & (~BIT_REG_TKACON1_ITRIM3)) | ((val) << POS_REG_TKACON1_ITRIM3))
#define GET_REG_TKACON1_ITRIM3()                ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_ITRIM3) >> POS_REG_TKACON1_ITRIM3)
// Touch Key Channel 3 CTRIM Select, 3.2p/1.6p/800f/400f/200f/200f
#define POS_REG_TKACON1_CTRIM3                  12
#define BIT_REG_TKACON1_CTRIM3                  0x3F000
#define SET_REG_TKACON1_CTRIM3(val)             ADDR_REG_TKACON1 = ((ADDR_REG_TKACON1 & (~BIT_REG_TKACON1_CTRIM3)) | ((val) << POS_REG_TKACON1_CTRIM3))
#define GET_REG_TKACON1_CTRIM3()                ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_CTRIM3) >> POS_REG_TKACON1_CTRIM3)
// Touch Key Channel 2 ENCHN
#define POS_REG_TKACON1_ENCHN2                  11
#define BIT_REG_TKACON1_ENCHN2                  0x800
#define SETF_REG_TKACON1_ENCHN2()               ADDR_REG_TKACON1 |= BIT_REG_TKACON1_ENCHN2
#define CLRF_REG_TKACON1_ENCHN2()               ADDR_REG_TKACON1 &= ~BIT_REG_TKACON1_ENCHN2
#define GETF_REG_TKACON1_ENCHN2()               ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_ENCHN2) >> POS_REG_TKACON1_ENCHN2)
// Touch Key Channel 2 ITRIM Select, TK Charge Current Select Signal, 3.84u/1.92u/0.96u/0.48u/0.24u
#define POS_REG_TKACON1_ITRIM2                  6
#define BIT_REG_TKACON1_ITRIM2                  0x7C0
#define SET_REG_TKACON1_ITRIM2(val)             ADDR_REG_TKACON1 = ((ADDR_REG_TKACON1 & (~BIT_REG_TKACON1_ITRIM2)) | ((val) << POS_REG_TKACON1_ITRIM2))
#define GET_REG_TKACON1_ITRIM2()                ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_ITRIM2) >> POS_REG_TKACON1_ITRIM2)
// Touch Key Channel 2 CTRIM Select, 3.2p/1.6p/800f/400f/200f/200f
#define POS_REG_TKACON1_CTRIM2                  0
#define BIT_REG_TKACON1_CTRIM2                  0x3F
#define SET_REG_TKACON1_CTRIM2(val)             ADDR_REG_TKACON1 = ((ADDR_REG_TKACON1 & (~BIT_REG_TKACON1_CTRIM2)) | ((val) << POS_REG_TKACON1_CTRIM2))
#define GET_REG_TKACON1_CTRIM2()                ((ADDR_REG_TKACON1 & BIT_REG_TKACON1_CTRIM2) >> POS_REG_TKACON1_CTRIM2)

/*
 * FUNCTION DECLARATION
 ***************************************************************************
 */
int touch_key_init(touch_key_typedef *touch_key_reg, void *tkey_cfg, u32 first_pwron,   \
                                CLK_TOUCH_KEY_TYPEDEF clk_sel, uint8_t tkclk_div2_enable);
void touch_key_tkcnt_isr(u32 tk_ch, u16 tkcnt);
void touch_key_range_isr(u32 tk_ch, u16 tkcnt);
u8 touch_key_get_key(void);
void tkey_bcnt_range_exception(tk_cb_t *s, u16 tkcnt, uint8_t tk_ch);
void touch_key_sw_reset(void);

#ifdef __cpluscplus
}
#endif

#endif
