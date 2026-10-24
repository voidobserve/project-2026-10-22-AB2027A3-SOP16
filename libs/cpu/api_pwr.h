#ifndef _API_PWR_H_
#define _API_PWR_H_

typedef enum{
    PMU_CFG_BUCK_EN = BIT(0),
    PMU_CFG_CHARGE_DIS = BIT(1),
    PMU_CFG_VUSB_TO_VDDIO = BIT(7),
}PMU_CFG_ENUM;

/**
  * @brief  pmu initialization
  * @param  pmu_cfg: PMU_CFG_ENUM
  */
void pmu_init(u8 pmu_cfg);

/**
  * @brief  get vddio level, vddio = 2.4V + 0.1V * level
  * @return  level: 0 ~ 0xF
  */
u8 pmu_get_vddio(void);

/**
  * @brief  get vrtc24 voltage values, uint:mV
  * @return  vrtc24 voltage value, about 2400mv
  */
u16 pmu_get_vrtc24_mv(void);

/**
  * @brief  set vddio level, vddio = 2.4V + 0.1V * level
  * @param  level: 0 ~ 0xF
  */
void pmu_set_vddio(u8 level);

/**
  * @brief  get vddcore level, vddcore = 0.7 + 0.025 * x
  * @return  level: 0 ~ 0x1F
  */
u8 pmu_get_vddcore(void);

/**
  * @brief  set vddcore level, vddcore = 0.7 + 0.025 * x
  * @param  level: 0 ~ 0x1F
  */
void pmu_set_vddcore(uint8_t level);

/**
  * @brief  get vbat saradc voltage distribution coefficient
  * @return  vbat saradc voltage distribution coefficient, about 2 * 1000
  */
u16 pmu_get_vbatdet(void);

/**
  * @brief  get lvd level
  * @return  level: 0 ~ 0x7
  */
u8 pmu_get_lvd(void);

/**
  * @brief  set lvd level
    0: 1.7V
    1: 1.8V
    2-7: 2.0V + 0.2V * (level - 2)
  * @param  level: 0 ~ 0x7
  */
void pmu_set_lvd(u8 level);

/**
  * @brief  set vddbt level
  * @param  level: 0 ~ 0xf, vddbt=0.85 + 0.05*level
  */
void pmu_set_vddbt(uint8_t level);

/**
  * @brief  get vddbt level
  * @return  level: 0 ~ 0xf
  */
uint8_t pmu_get_vddbt(void);


/**
  * @brief  enable buck mode or disable buck mode
  * @param 1:enable buck mode 0:disable buck mode, pmu work in ldo mode
  */
void pmu_set_buck_mode(u8 buck_en);

bool pmu_vddio_to_flash(void);

u8 charge_const_curr_trim(u8 val);
u8 charge_cutoff_curr_trim(u8 val);

void lowpwr_sleep_sfr_save(void);
void lowpwr_sleep_sfr_resume(void);

void lp_sleep(void (*sleep_cb)(void), void (*wakeup_cb)(void));

#endif // _API_PWR_H_

