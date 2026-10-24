#ifndef _BSP_SYS_H
#define _BSP_SYS_H


typedef struct {
    u32 rst_reason;
    u32 wakeup_reason;
#if SYS_SLEEP_EN
    u32 sleep_time;
    u32 sleep_delay;
    u8  sleep_sta;
    bool sleep_prevent;
#endif
#if SYS_OFF_TIME
    u32 pwroff_time;
    u32 pwroff_delay;
#endif
} sys_cb_t;
extern sys_cb_t sys_cb;

void bsp_sys_init(void);
void bsp_periph_init(void);
void sys_ram_info_dump(void);
void usr_tmr5ms_thread_do(void);

#endif // _BSP_SYS_H

