#ifndef __API_SDDAC_H
#define __API_SDDAC_H
#include "driver_sddac.h"

typedef void (*sddac_done_callback)(SDDAC_IT_TYPEDEF type);

/**
  * @brief  Set the sddac output completion callback function
            This callback function will be called after sddac_done_proc_kick is executed
  * @param  func:callback function
  */
void sddac_done_callback_set(sddac_done_callback func);

/**
  * @brief  Triggers the aupcm thread to handle the sddac callback function, which will be set by sddac_done_callback_set.
            This function is usually called inside the sddac interrupt function
  * @param  type:SDDAC_IT_ALL_DONE or SDDAC_IT_HALF_DONE
  */
void sddac_done_proc_kick(SDDAC_IT_TYPEDEF type);

#endif //__API_SDDAC_H
