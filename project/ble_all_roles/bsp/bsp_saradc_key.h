#ifndef _BSP_SARADC_KEY_H
#define _BSP_SARADC_KEY_H


/**
 * @brief  ADC Channel for key
 */
#define ADC_CHANNEL_SEL     ADC_CHANNEL_PB0

/**
 * @brief ADC value of each key
 */
#define KEY_VALUE_PP            0x0005
#define KEY_VALUE_KEY1          0x0061
#define KEY_VALUE_KEY2          0x00ae
#define KEY_VALUE_KEY3          0x0104
#define KEY_VALUE_KEY4          0x015c
#define KEY_VALUE_NULL          0xffff


/**
 * @brief Mapping relationship between ADC value with key ID
 */
typedef struct {
    u16 adc_val;
    u8 usage_id;
} key_val_mapping_typedef;


/******************************* Function of ad_key ********************************/
void bsp_adkey_init(void);
u8 bsp_get_adkey_id(void);

#endif // _BSP_SARADC_KEY_H
