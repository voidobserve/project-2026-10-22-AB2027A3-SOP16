#include "include.h"
#include "driver_lowpwr.h"
#include "bsp_saradc_key.h"

xcfg_cb_t xcfg_cb;
sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
u16 tmr5ms_cnt;

void sys_ram_info_dump(void)
{
    extern u32 __comm_size;
    extern u32 __bss_size;
    extern u32 __bram_size;
    extern u32 __heap_size;

    extern u32 __bram_vma;
    extern u32 __bram_ram_size;
    extern u32 __sram_vma;
    extern u32 __sram_ram_size;

    u32 sram_free  = (u32)&__sram_ram_size - (u32)&__comm_size - (u32)&__bss_size - (u32)&__heap_size;
    u32 bram_free = (u32)&__bram_ram_size - (u32)&__bram_size;
    u32 ram_total = sram_free + bram_free;

    printf("Ram_info:\n");
    printf("sram:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", sram_free,  (u32)&__sram_ram_size,  (u32)&__sram_vma, (u32)&__sram_vma + (u32)&__sram_ram_size);
    printf("bram: remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", bram_free, (u32)&__bram_ram_size, (u32)&__bram_vma, (u32)&__bram_vma + (u32)&__bram_ram_size);
    printf("remain_total: %dK\n",ram_total / 1024);
}


//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_thread_do(void)
{
    tmr5ms_cnt++;

#if BSP_KEY_EN
    bsp_key_scan(0);
#endif

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0) {
        lowpwr_tout_ticks();
    }

    //500ms timer process
    if ((tmr5ms_cnt % 100) == 0) {
        msg_enqueue(MSG_SYS_500MS);
    }

    //1s timer process
    if ((tmr5ms_cnt % 200) == 0) {
        msg_enqueue(MSG_SYS_1S);
    }
}

AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
#if SYS_SLEEP_EN
    sys_cb.sleep_time = -1L;
    sys_cb.sleep_delay = -1L;

    if (SYS_SLEEP_TIME != 0) {
        sys_cb.sleep_time = (u32)SYS_SLEEP_TIME * 10;   //100ms为单位
        sys_cb.sleep_delay = sys_cb.sleep_time;
    }
#endif

#if SYS_OFF_TIME
    sys_cb.pwroff_time = -1L;
    sys_cb.pwroff_delay = -1L;

    if (SYS_OFF_TIME != 0) {
        sys_cb.pwroff_time = (u32)SYS_OFF_TIME * 10;    //100ms为单位
        sys_cb.pwroff_delay = sys_cb.pwroff_time;
    }
#endif


    if(!xcfg_cb.ft_osc_cap_en || (xcfg_cb.osci_cap == 0 && xcfg_cb.osco_cap == 0)) {        //没有过产测时，使用自定义OSC电容
        xcfg_cb.osci_cap = xcfg_cb.uosci_cap;
        xcfg_cb.osco_cap = xcfg_cb.uosco_cap;
        xcfg_cb.osc_both_cap = xcfg_cb.uosc_both_cap;
    }

    msg_queue_init();
}

#if LP_PWROFF_EN
static void power_on_vbat_check(void)
{
    u8 vbat_check_cnt = 20;

    while (vbat_check_cnt) {
        if (bsp_vbat_proc()) {
            vbat_check_cnt--;
        }
    }
    if (func_cb.sta == FUNC_PWROFF) {
        func_pwroff();
    }
}
#endif

void power_on_check(void)
{
    u32 power_on_check_time = SYS_PWRUP_TIME;
    u32 wakeup_reason = WK_LP_WK0;

#if BSP_TOUCH_KEY_EN
    wakeup_reason = WK_LP_TOUCH;
#endif

#if LP_PWROFF_EN
    power_on_vbat_check();
#endif

    if (!(sys_cb.wakeup_reason & wakeup_reason) || power_on_check_time == 0) {
        return;
    }

#if BSP_KEY_EN

    u32 tick = tick_get();
    u16 key;
    u16 cnt_up = 0;

    while (1) {
        WDT_CLR();
        delay_5ms(1);

        key = bsp_key_scan(1);

        if (!tick_check_expire(tick, power_on_check_time)) {
            if (key != KEY_ID_PP) {
                cnt_up++;
                if(cnt_up > 5){
                    func_pwroff();
                }
            }else{
                cnt_up = 0;
            }
        } else {
            bsp_key_set_power_on_flag(true);
            return;
        }
    }
#endif
}

AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    uint8_t pmu_cfg = BUCK_MODE_EN * PMU_CFG_BUCK_EN;

#if !BSP_CHARGE_EN
    pmu_cfg |= PMU_CFG_CHARGE_DIS;
#endif

    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    bsp_var_init();

#if BSP_SARADC_EN
    bsp_saradc_init();
#endif

#if BSP_KEY_EN
    bsp_key_init();
#endif

    // power init
    pmu_init(pmu_cfg);

#if BSP_CHARGE_EN
    bsp_charge_init();
#endif

#if BSP_VBAT_DETECT_EN
    bsp_vbat_detect_init();
#endif // BSP_VBAT_DETECT_EN

    power_on_check();

    // clock init
    sys_clk_set(SYS_CLK_SEL);

    bsp_param_init();

    xosc_init();

    // enable user timer callback
    sys_set_tmr_enable(1, 1);

    ble_app_env_init();

#if BSP_LED_EN
    bsp_led_init();
#endif

#if SYS_SLEEP_EN
    lowpwr_sleep_wakeup_config();
#endif

    user_init();

    printf("%s\n",__func__);

    func_cb.sta = FUNC_BT;
}

void bsp_periph_init(void)
{
#if BSP_UART_DEBUG_EN
    bsp_uart_debug_init();
#endif

#if BSP_SARADC_EN
    bsp_saradc_init();
#endif

#if BSP_KEY_EN
    bsp_key_init();
#endif

#if BSP_VBAT_DETECT_EN
    bsp_vbat_detect_init();
#endif
}
