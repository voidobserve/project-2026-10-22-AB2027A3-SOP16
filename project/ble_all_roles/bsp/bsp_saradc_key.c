#include "include.h"
#include "driver_saradc.h"
#include "bsp_saradc_key.h"
#include "bsp_key.h"

#if BSP_ADKEY_EN

AT(.com_rodata.key)
const key_val_mapping_typedef key_val_mapping_table[6] = {
		{KEY_VALUE_PP,      KEY_ID_PP},
		{KEY_VALUE_KEY1,    KEY_ID_K1},
		{KEY_VALUE_KEY2,    KEY_ID_K2},
		{KEY_VALUE_KEY3,    KEY_ID_K3},
		{KEY_VALUE_KEY4,    KEY_ID_K4},
		{KEY_VALUE_NULL,    KEY_ID_NO},
};


// 初始化ADC按键
void bsp_adkey_init(void)
{
    saradc_channel_init_typedef saradc_channel_init_struct;

    saradc_channel_init_struct.channel = ADC_CHANNEL_SEL;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
    saradc_channel_init_struct.interrupt_en = DISABLE;

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_kick_start(ADC_CHANNEL_SEL);
}

// 读取按键的ADC采样值并根据数值返回键值
AT(.com_text.adkey.get)
u8 bsp_get_adkey_id(void)
{
    u8 num = 0;
    u16 key_adc_val;

    if (!saradc_get_channel_flag(ADC_CHANNEL_SEL)) {
        return 0;
    }

    key_adc_val = saradc_get_data(ADC_CHANNEL_SEL);

    while (key_adc_val > key_val_mapping_table[num].adc_val) {
        num++;
    }

    saradc_kick_start(ADC_CHANNEL_SEL);

    return key_val_mapping_table[num].usage_id;
}

#endif // BSP_ADKEY_EN
