/**
******************************************************************************************************************************
*
*@file touch_key_example.h
*
*@brief Header file for Touch Key example
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _BSP_TOUCH_KEY_H
#define _BSP_TOUCH_KEY_H


#include "driver_com.h"


#ifdef __cplusplus
extern "C" {
#endif

//touch key参数
#define CDPR            30                 //Touch key charge and discharge period (0 ~ 1024), 每个通道可以独立配置。
#define FIL_LOW         4                   //Touch key state low state filter length(FIL_LOW + 1)
#define FIL_HIGH        4                   //Touch key state high state filter length(FIL_HIGH + 1)    //越大检测稳定时间越长，灵敏度越低
#define FIL_EXCEPT      10                  //Touch key state exception filter length(FIL_EXCEPT + 1)
#define FIL_VAL         8
#define TO_EXCEPT       3000                //1250*8ms = 10S, 定时校准
#define PTO_EXCEPT      15                  //Touch press timeout(0 ~ 15), timeout = (15 << 8) * 8ms = 30S
#define TKPTHRESH       18                  //Touch key press threshold
#define TKRTHRESH       9                  //Touch key release threshold
#define SMALLTHD        3                   //update TKBCNT, TKCNT < (TKBCNT - SMALLTHD)
#define LARGETHD        (TKPTHRESH * 2)    //update TKBCNT, TKCNT > (TKBCNT + LARGETHD)

#define TO_STRING(X)            #X
#define TOUCH_KEY_NUM           4
#define ABS(a,b)        (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))


typedef enum
{
    TOUCH_KEY_STA_PRESSED,
    TOUCH_KEY_STA_RELEASED,
    TOUCH_KEY_STA_ERROR = 0xff,
}touch_key_state_t;

typedef enum
{
    TOUCH_KEY_DET_NO_KEY,
    TOUCH_KEY_DET_CHANGE,
}touch_key_detect_t;

typedef struct {
	volatile bool sta;
	volatile u32 bcnt;
} mult_cb_t;

void bsp_touch_key_init(void);
u8 bsp_get_touch_key_id(void);


#ifdef __cplusplus
}
#endif


#endif
