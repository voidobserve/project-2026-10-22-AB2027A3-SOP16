#include "io_key_apply.h"
#include "io_key_driver.h"

extern const io_key_handle_func_t io_key_event_handle_table[1];

void io_key_apply(void)
{
    uint8_t io_key_event;
    io_key_handle_func_t io_key_handle_func_ptr;

    io_key_event = io_convert_key_event(io_scan_param.latest_key_val,
                                        io_scan_param.latest_key_event);
    io_scan_param.latest_key_val = NO_KEY; // 获取到按键事件后，清空按键值
	io_scan_param.latest_key_event = KEY_EVENT_NONE;
    if (IO_KEY_EVENT_NONE == io_key_event) {
        return;
    }

    io_key_handle_func_ptr = io_key_event_handle_table[io_key_event];
    if (NULL == io_key_handle_func_ptr) {
        return;
    }

    io_key_handle_func_ptr();
}

void io_key1_click_handle(void)
{
    printf("io key1 click\n");
}

const io_key_handle_func_t io_key_event_handle_table[1] = {
    [IO_KEY_EVENT_KEY1_CLICK] = io_key1_click_handle,
};
