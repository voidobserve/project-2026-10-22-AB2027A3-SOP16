#ifndef __VOICE_SDADC_H
#define __VOICE_SDADC_H
#include "api_sdadc.h"

#if (FUNC_TBOX_EN && PROD_TEST_MIC_EN)
void tbox_voice_sdadc_process(SDADC_DONE_TYPEDEF type);
void tbox_voice_sdadc_init(void);
void tbox_voice_sdadc_start(void);
void tbox_voice_sdadc_stop(void);
#endif  //(FUNC_TBOX_EN && PROD_TEST_MIC_EN)
#endif  //__VOICE_SDADC_H
