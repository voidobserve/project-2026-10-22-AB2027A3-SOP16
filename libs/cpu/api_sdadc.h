#ifndef _API_SDADC_H
#define _API_SDADC_H

typedef enum{
    SDADC_ALL_DONE,
    SDADC_HALF_DONE,
}SDADC_DONE_TYPEDEF;

typedef enum {
    MIC_ANALOG_CFG_NOT_R = 1,
    MIC_ANALOG_CFG_NORMAL,
    MIC_ANALOG_CFG_DIRECT,
} MIC_ADC_CFG;

typedef void (*sdadc_done_callback)(SDADC_DONE_TYPEDEF type);

/**
  * @brief  Set the sdadc sampling completion callback function
            This callback function will be called after sdadc_done_proc_kick is executed
  * @param  func:callback function
  */
void sdadc_done_callback_set(sdadc_done_callback func);

/**
  * @brief  Triggers the adpcm thread to handle the sdadc callback function, which will be set by sdadc_done_callback_set.
            This function is usually called inside the sdadc interrupt function
  * @param  type:SDADC_ALL_DONE or SDADC_HALF_DONE
  */
void sdadc_done_proc_kick(SDADC_DONE_TYPEDEF type);

void mic_adc_init(MIC_ADC_CFG mic_type);

void mic_adc_deinit(void);

/**
  * @brief  Enable and set the bias voltage of the mic pin, mic_bias pin only map to PB1.
            Before use, you need to set the PB1 pin to analog output, turn off the pull up and down
            This function must be called after sdadc_init
  * @param  level: [0,7], bias voltage = 1.8V + level * 0.2V
  * @param  res: [8,4,2,1], res = 8k, 4k, 2k, 1k
  */
void mic_bias_open(u8 level, u8 res);

void mic_bias_close(void);

/**
  * @brief  Enable and set the bias voltage of the mic pin, mic_bias pin only map to PB1.
            Before use, you need to set the PB1 pin to analog output, turn off the pull up and down
            This function must be called after sdadc_init
  * @param  level: [0,7], bias voltage = 1.8V + level * 0.2V
  */
void mic_bias_level_set(u8 level);

/**
  * @brief  Set mic analog gain
  * @param  level: [0,7], analog gain = -6dB + level * 6dB
  */
void mic_analog_gain_set(u8 level);

#endif //_API_SDADC_H
