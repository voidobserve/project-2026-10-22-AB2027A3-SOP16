#ifndef _BSP_SARADC_VBAT_H
#define _BSP_SARADC_VBAT_H


#define VBAT_DETECT_ASSIST_CHANNEL      ADC_CHANNEL_VRTC
#define VBAT_DETECT_ASSIST_VOLTAGE      pmu_get_vrtc24_mv()

typedef struct {
    u16 vbat_min_power;
    u16 vbat_max_power;
    u8  vbat_max_level;
    u8  res[4];
} vbat_param_struct;

void bsp_vbat_detect_init(void);
uint16_t bsp_get_vbat_level(u8 mode);
bool bsp_vbat_proc(void);

#endif // _BSP_SARADC_VBAT_H
