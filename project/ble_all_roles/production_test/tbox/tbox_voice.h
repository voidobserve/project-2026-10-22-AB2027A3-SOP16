#ifndef __TBOX_VOICE_H
#define __TBOX_VOICE_H
#include "include.h"


#if (FUNC_TBOX_EN && PROD_TEST_MIC_EN)
   
#define TBOX_VOICE_RING_BUF_LEN               (10*132)  //one pack 132 byte
#define TBOX_VOICE_ENC_DATA_LEN               (4+128)   //2byte presample + 2byte index + 128byte偏差值    

typedef enum {
    VOICE_STATUS_STOP          = 0x00,
    VOICE_STATUS_START         = 0x01,
} VOICE_STATUS_TYPEDEF;

typedef struct{
    uint8_t voice_status;
    uint8_t exit_clk;
}tbox_voice_var_t;

void tbox_voice_stop(void);
void tbox_voice_proc(void);
void tbox_voice_init(void);
#endif  //(FUNC_TBOX_EN && PROD_TEST_MIC_EN)

#endif
