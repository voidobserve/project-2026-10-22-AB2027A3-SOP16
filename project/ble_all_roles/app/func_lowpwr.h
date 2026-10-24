#ifndef __FUNC_LOWPWR_H
#define __FUNC_LOWPWR_H

#if SYS_OFF_TIME

#define lowpwr_pwroff_auto_en()         if(sys_cb.pwroff_delay == -1L) {sys_cb.pwroff_delay = sys_cb.pwroff_time;}

#define lowpwr_pwroff_auto_dis()        sys_cb.pwroff_delay = -1L;

#define lowpwr_pwroff_delay_reset()     if(sys_cb.pwroff_delay != -1L) {sys_cb.pwroff_delay = sys_cb.pwroff_time;}

#else

#define lowpwr_pwroff_auto_en()

#define lowpwr_pwroff_auto_dis()

#define lowpwr_pwroff_delay_reset()

#endif

#if SYS_SLEEP_EN

#define lowpwr_sleep_delay_reset()      if(sys_cb.sleep_delay != -1L) {sys_cb.sleep_delay = sys_cb.sleep_time;}

#define lowpwr_sleep_auto_en()          if(sys_cb.sleep_delay == -1L) {sys_cb.sleep_delay = sys_cb.sleep_time;}

#define lowpwr_sleep_auto_dis()         sys_cb.sleep_delay = -1L;

#else

#define lowpwr_sleep_delay_reset()

#define lowpwr_sleep_auto_en()

#define lowpwr_sleep_auto_dis()

#endif

enum{
    LOWPWR_SLEEP_EXIT,
    LOWPWR_SLEEP_PREPARE,
    LOWPWR_SLEEP_ENTER,
};

typedef bool (*is_sleep_func)(void);

bool sleep_process(is_sleep_func is_sleep);
void lowpwr_tout_ticks(void);
void func_pwroff(void);
void lowpwr_sleep_wakeup_config(void);

#endif // __FUNC_LOWPWR_H
