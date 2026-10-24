#ifndef _API_DEVICE_H
#define _API_DEVICE_H

enum {
    DEV_USB           = 0,
    DEV_TOTAL_NUM,
};

void dev_init(u16 online_cnt, u16 offline_cnt);
bool dev_is_online(u8 dev_num);
bool dev_online_filter(u8 dev_num);
bool dev_offline_filter(u8 dev_num);
void dev_delay_times(u8 dev_num, u16 times);          //同时设置, online去抖延时 和 offline 去抖延时, 它们的延时一样的,兼容以前.
void dev_delay_online_times(u8 dev_num, u16 times);   //单独设置online去抖延时
void dev_delay_offline_times(u8 dev_num, u16 times);  //单独设置offline 去抖延时
void dev_clr_sta(u8 dev_num);

#endif // _API_DEVICE_H
