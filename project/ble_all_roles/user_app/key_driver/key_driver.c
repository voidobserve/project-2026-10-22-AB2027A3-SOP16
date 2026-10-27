#include "key_driver.h"

static volatile uint8_t is_key_active; // 按键是否有效的计数值

//=======================================================//
// 按键扫描函数: 扫描所有注册的按键驱动
//=======================================================//
// xdata volatile struct key_driver_para *scan_para; // 要让指针指向xdata区域的全局变量，需要同样将指针变为全局变量
// 要让指针指向xdata区域的全局变量，需要同样将指针变为全局变量
volatile struct key_driver_para *scan_para;
void key_driver_scan(key_driver_param_t *_scan_para)
{

#if 1
    uint8_t key_event = KEY_EVENT_NONE; // 存放待发送的按键事件
    uint8_t cur_key_value = NO_KEY;     // 存放待发送的按键键值
    uint8_t key_value = NO_KEY;

    scan_para = (struct key_driver_para *)_scan_para;

    // 如果扫描时间未到来，不执行按键扫描
    if (scan_para->cur_scan_times < scan_para->scan_times) {
        return;
    }

    scan_para->cur_scan_times = 0; // 清空扫描时间

    cur_key_value = scan_para->get_value(); // 调用用户自定义的获取键值函数

    // if (cur_key_value != NO_KEY)
    //     printf("key id %bu\n", cur_key_value); // 打印获取到的键值

    // 判断按键是否有效
    if (cur_key_value != NO_KEY) {
        is_key_active = 35; // 35*10Ms
    } else if (is_key_active) {
        is_key_active--;
    }

    // if (cur_key_value != NO_KEY)
    // {
    //     //     printf("scan_times %bu\n", scan_para->scan_times);
    //     //     printf("cur_scan_times %bu\n", scan_para->cur_scan_times);
    //     //     printf("last_key %bu\n", scan_para->last_key);
    //     //     printf("filter_value %bu\n", scan_para->filter_value);
    //     //     printf("filter_cnt %bu\n", scan_para->filter_cnt);
    //     //     printf("filter_time %bu\n", scan_para->filter_time);
    //     printf("long_time %bu\n", ad_key_para.long_time);
    //     printf("long_time %bu\n", (*scan_para).long_time);
    //     //     printf("hold_time %bu\n", scan_para->hold_time);
    //     //     printf("press_cnt %bu\n", scan_para->press_cnt);

    //     //     printf("click_cnt %bu\n", scan_para->click_cnt);
    //     //     printf("click_delay_cnt %bu\n", scan_para->click_delay_cnt);
    //     //     printf("click_delay_time %bu\n", scan_para->click_delay_time);
    //     //     printf("notify_value %bu\n", scan_para->notify_value);
    //     //     printf("key_type %bu\n", scan_para->key_type);

    //     //     printf("latest_key_val %bu\n", scan_para->latest_key_val);
    //     //     printf("latest_key_event %bu\n", scan_para->latest_key_event);

    //     printf("ad key long time addr %p\n", &ad_key_para.long_time);
    //     // printf("scan para long time addr %p\n", &(scan_para->long_time));
    //     printf("scan para long time addr %p\n", (*scan_para).long_time);
    // }

    //===== 按键消抖处理
    if (cur_key_value != scan_para->filter_value && scan_para->filter_time) {
        // 当前按键值与上一次按键值如果不相等, 重新消抖处理, 注意filter_time != 0;
        scan_para->filter_cnt = 0;               // 消抖次数清0, 重新开始消抖
        scan_para->filter_value = cur_key_value; // 记录上一次的按键值
        return;                                  // 第一次检测, 返回不做处理
    }

    // 当前按键值与上一次按键值相等, filter_cnt开始累加;
    if (scan_para->filter_cnt < scan_para->filter_time) {
        scan_para->filter_cnt++;
        return;
    }

    //===== 按键消抖结束, 开始判断按键类型(单击, 双击, 长按, 多击, HOLD, (长按/HOLD)抬起)
    if (cur_key_value != scan_para->last_key) {
        /*
            如果当前的键值为空，上一次的键值又与当前的键值不一样，说明按键松开
            cur_key = NO_KEY; last_key = valid_key -> 按键被抬起
        */
        // printf("key event up\n");

        if (cur_key_value == NO_KEY) {
            if (scan_para->press_cnt >= scan_para->long_time) {
                // 长按/HOLD状态之后被按键抬起;
                key_event = KEY_EVENT_UP;
                key_value = scan_para->last_key;
                goto _notify; // 发送抬起消息
            }

            scan_para->click_delay_cnt = 1; // 按键等待下次连击延时开始
        } else {
            /*
                如果当前的键值不为空，上一次的键值又与当前的键值不一样，说明按键按下
                cur_key = valid_key, last_key = NO_KEY -> 按键被按下
            */
            // 用于判断long和hold事件的计数器重新开始计时;
            scan_para->press_cnt = 1;
            if (cur_key_value != scan_para->notify_value) {
                // 第一次单击/连击时按下的是不同按键, 单击次数重新开始计数
                scan_para->click_cnt = 1;
                scan_para->notify_value = cur_key_value;

                key_event = KEY_EVENT_PRESS;
                scan_para->press_cnt++;
                key_value = cur_key_value;
                goto _notify;
            } else {
                scan_para->click_cnt++; // 单击次数累加
            }
        }

        goto _scan_end; // 返回, 等待延时时间到
    } else {
        /*
            如果当前获取的键值与上次获取的键值一样，说明按键在长按或是按键未按下
            cur_key = last_key -> 没有按键按下/按键长按(HOLD)
        */
        if (cur_key_value == NO_KEY) {
            // last_key = NO_KEY; cur_key = NO_KEY -> 没有按键按下
            if (scan_para->click_cnt > 0) { // 有按键需要消息需要处理

                // 如果只响应单击事件，可以在这里添加/修改功能

                if (scan_para->click_delay_cnt >
                    scan_para->click_delay_time) { // 按键被抬起后延时到
                    // TODO: 在此可以添加任意多击事件
                    // if (scan_para->click_cnt >= 5)
                    // {
                    //     key_event = KEY_EVENT_FIRTH_CLICK; // 五击
                    // }
                    // else if (scan_para->click_cnt >= 4)
                    // {
                    //     key_event = KEY_EVENT_FOURTH_CLICK; // 4击
                    // }
                    // else if (scan_para->click_cnt >= 3)
                    // {
                    //     key_event = KEY_EVENT_TRIPLE_CLICK; // 三击
                    // }
                    // else if (scan_para->click_cnt >= 2)
                    // if (scan_para->click_cnt >= 2)
                    // {
                    //     key_event = KEY_EVENT_DOUBLE_CLICK; // 双击
                    // }
                    // else
                    {
                        key_event = KEY_EVENT_CLICK; // 单击
                        // printf("click \n");
                    }
                    key_value = scan_para->notify_value;

                    goto _notify;
                } else {
                    // 按键抬起后等待下次延时时间未到
                    scan_para->click_delay_cnt++;
                    goto _scan_end; // 按键抬起后延时时间未到, 返回
                }
            } else {
                goto _scan_end; // 没有按键需要处理
            }
        } else {
            // last_key = valid_key; cur_key = valid_key, press_cnt累加用于判断long和hold
            if (scan_para->press_cnt < 255) {
                scan_para->press_cnt++;
            }

            // printf("long time %bu\n", scan_para->long_time);
            // printf("press cnt %bu\n", scan_para->press_cnt);
            if (scan_para->press_cnt == scan_para->long_time) {
                key_event = KEY_EVENT_LONG;

                // printf("long time %bu\n", scan_para->long_time);

                // printf("key event long\n");
            } else if (scan_para->press_cnt == scan_para->hold_time) {
                key_event = KEY_EVENT_HOLD;
                // 下一次scan_para->press_cnt++,还是会进入到这里
                scan_para->press_cnt = scan_para->long_time;
                // printf("key event hold\n");
            } else {
                goto _scan_end; // press_cnt没到长按和HOLD次数, 返回
            }

            // press_cnt没到长按和HOLD次数, 发消息
            key_value = cur_key_value;
            goto _notify;
        }
    }

_notify:

    // USER_TO_DO 根据按键类型，更新按键键值和事件
    // if (KEY_TYPE_24G_REMOTE == scan_para->key_type) {
    //     // 如果是2.4G遥控按键

    scan_para->click_cnt = 0;         // 按键按下次数清0
    scan_para->notify_value = NO_KEY; // 在延时的待发送按键值清零

    scan_para->latest_key_val = key_value;   // 存放得到的按键键值
    scan_para->latest_key_event = key_event; // 存放得到的按键事件
    // }
    // else if (KEY_TYPE_TOUCH == scan_para->key_type) // 如果是触摸按键
    // {
    //     scan_para->click_cnt = 0;         // 按下次数清0
    //     scan_para->notify_value = NO_KEY; // 在延时的待发送按键值清零

    //     scan_para->latest_key_val = key_value;   // 存放得到的按键键值
    //     scan_para->latest_key_event = key_event; // 存放得到的按键事件
    // }

_scan_end:
    scan_para->last_key = cur_key_value;
    return;
#endif
}
