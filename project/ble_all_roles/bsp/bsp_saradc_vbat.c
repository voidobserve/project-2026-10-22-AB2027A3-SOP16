#include "include.h"
#include "driver_saradc.h"
#include "driver_charge.h"
#include "bsp_saradc_vbat.h"

#if BSP_VBAT_DETECT_EN

#define VBAT_FILTER_DIFF_VAL    30  //30mV
#define VBAT_FILTER_BUF_LEN     5
#define PWROFF_FILTER_NUM	    5	//检测到连续n次（500ms一次）低电量后，自动关机

static u32 vbat_voltage = 0;

// 电池电量相关参数
const vbat_param_struct vbat_param = {
    .vbat_max_level = 100,
    .vbat_min_power = 2500,
    .vbat_max_power = 4200,
};

// 初始化Vbat检测
void bsp_vbat_detect_init(void)
{
    saradc_channel_init_typedef saradc_channel_init_struct;

    saradc_channel_init_struct.channel = ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL;
    saradc_channel_init_struct.pullup_en = SARADC_PULLUP_DIS;
    saradc_channel_init_struct.setup_time = SARADC_ST_8_CLK;
    saradc_channel_init_struct.interrupt_en = DISABLE;

    saradc_channel_init(&saradc_channel_init_struct);

    saradc_kick_start(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL);

    while(!saradc_get_channel_flag(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL));

    bsp_vbat_proc();
}

uint32_t bsp_vbat_filter(uint32_t voltage)
{
    static uint8_t div = 1;
    static uint8_t index = 0;
    static uint32_t filter_buf[VBAT_FILTER_BUF_LEN] = {0};
    uint32_t total_vol = 0;
    uint32_t result = 0;

    filter_buf[index] = voltage;
    for (uint8_t i=0; i<VBAT_FILTER_BUF_LEN; i++) {
        total_vol += filter_buf[i];
    }
    result =  total_vol / div;
    div = (div >= VBAT_FILTER_BUF_LEN) ? VBAT_FILTER_BUF_LEN : (div+1);
    index = (index >= (VBAT_FILTER_BUF_LEN-1)) ? 0 : (index+1);

    return result;
}

AT(.text.app.proc.vbat)
void bsp_vbat_lowpwr_detect(void)
{
    static u8 auto_off_count = 0;
    bool charge_in = false;

#if BSP_CHARGE_EN
    charge_in = (charge_get_status() > CHARGE_STA_OFF) ? true : false;
#endif

    if (bsp_get_vbat_level(1) <= LP_PWROFF_VAL && !charge_in) {
        if (auto_off_count < PWROFF_FILTER_NUM) {
            auto_off_count++;
        } else {
            printf("[BAT] low battery level auto power off\n");
            func_cb.sta = FUNC_PWROFF;
        }
    } else {
        auto_off_count = 0;
    }
}

AT(.text.app.proc.vbat)
bool bsp_vbat_proc(void)
{
    uint32_t adc_val;
    uint32_t adc_vbg;
    uint32_t vbat_cur;
    uint16_t diff;

    if (saradc_get_channel_flag(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL)) {
        adc_val = saradc_get_data(ADC_CHANNEL_VBAT);
        adc_vbg = saradc_get_data(VBAT_DETECT_ASSIST_CHANNEL);
        vbat_cur = adc_val * VBAT_DETECT_ASSIST_VOLTAGE / adc_vbg;
        vbat_cur = bsp_vbat_filter(vbat_cur);
        if(vbat_voltage > vbat_cur) {
            diff = vbat_voltage - vbat_cur;
        } else {
            diff = vbat_cur - vbat_voltage;
        }
        if (diff >= VBAT_FILTER_DIFF_VAL) {
            vbat_voltage = vbat_cur;
        }
        saradc_kick_start(ADC_CHANNEL_VBAT | VBAT_DETECT_ASSIST_CHANNEL);

        #if LP_PWROFF_EN
        bsp_vbat_lowpwr_detect();
        #endif

        return true;
    }

    return false;
}

// 获取电池电量： 1：电压(mv)  0：电量等级(%)
uint16_t bsp_get_vbat_level(u8 mode)
{
    uint8_t vbat_level;
    if (mode == 0) {
        if (vbat_param.vbat_min_power > vbat_voltage) {
            return 0;
        }
        vbat_level = vbat_param.vbat_max_level * \
        (vbat_voltage - vbat_param.vbat_min_power) / (vbat_param.vbat_max_power - vbat_param.vbat_min_power);
        vbat_level = (vbat_level > vbat_param.vbat_max_level) ? vbat_param.vbat_max_level : vbat_level;
        return vbat_level;
    } else {
        return vbat_voltage;
    }
}

#endif // BSP_VBAT_DETECT_EN
