#ifndef __TBOX_KEY_H
#define __TBOX_KEY_H
#include "include.h"

#if (FUNC_TBOX_EN && PROD_TEST_KEY_EN)

typedef enum {
    TBOX_KEY_MSG_RELEASE             = 0x00,  //按键抬起
    TBOX_KEY_MSG_UP                  = 0x01,
    TBOX_KEY_MSG_DOWN                = 0x02,
    TBOX_KEY_MSG_LEFT                = 0x03,
    TBOX_KEY_MSG_RIGHT               = 0x04,
    TBOX_KEY_MSG_MEUN                = 0x05,
    TBOX_KEY_MSG_HOME                = 0x06,
    TBOX_KEY_MSG_OK                  = 0x07,
    TBOX_KEY_MSG_BACK                = 0x08,
    TBOX_KEY_MSG_VOL_UP              = 0x09,
    TBOX_KEY_MSG_VOL_DOWN            = 0x0a,
    TBOX_KEY_MSG_POWER               = 0x0b,
    TBOX_KEY_MSG_VOICE_KEY_TEST      = 0x0c,  //产测语音按键测试(单击)
    TBOX_KEY_MSG_VOICE_ON_TEST       = 0x0d,  //产测语音测试开始(长按)
    TBOX_KEY_MSG_VOICE_OFF_TEST      = 0x0e,  //产测语音测试结束(长按抬起)
    
    TBOX_KEY_MSG_NONE                = 0xff,
} TBOX_KEY_MSG_TYPEDEF;  //范围：0x00~0xff

//按键测试命令  范围：0x00~0xff
#define TBOX_KEY_RELEASE_CMD         0x00
#define TBOX_KEY_UP_CMD              0x01
#define TBOX_KEY_DOWN_CMD            0x02
#define TBOX_KEY_LEFT_CMD            0x03
#define TBOX_KEY_RIGHT_CMD           0x04
#define TBOX_KEY_MEUN_CMD            0x05
#define TBOX_KEY_HOME_CMD            0x06
#define TBOX_KEY_OK_CMD              0x07
#define TBOX_KEY_BACK_CMD            0x08
#define TBOX_KEY_VOL_UP_CMD          0x09
#define TBOX_KEY_VOL_DOWN_CMD        0x0a
#define TBOX_KEY_PWR_CMD             0x0b
#define TBOX_KEY_VOICE_KEY_CMD       0x0c
//语音测试命令
#define TBOX_KEY_VOICE_ON_CMD        0x0d
#define TBOX_KEY_VOICE_OFF_CMD       0x0e

#define TBOX_KEY_NONE_CMD            0xff      

void tbox_key_proc(void);
#endif  //(FUNC_TBOX_EN && PROD_TEST_KEY_EN)
#endif  //__TBOX_KEY_H
