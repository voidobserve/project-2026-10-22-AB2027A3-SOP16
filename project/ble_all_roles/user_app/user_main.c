#include "user_config.h"
#include "driver_uart.h"

// #include "driver_gpio.h" // 初始化 测试时使用的gpio，需要引用该头文件

volatile user_data_t user_data = {0};

#define USER_DATE_SAVE_START_ADDR 0x00 // 起始地址

void user_data_write(void)
{
    bsp_param_write(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
    bsp_param_sync(); // 同步数据到flash
}

void user_data_read(void)
{
    bsp_param_read(&user_data, USER_DATE_SAVE_START_ADDR, sizeof(user_data_t));
#if USER_DEBUG_ENABLE
    // my_printf("save.color == 0x%lx\n", user_data.color);
#endif
}

// 初始化函数，在 bsp_sys.c -> bsp_sys_init() 中调用
void user_init(void)
{
#if 0
    user_data_read(); // 读取flash保存的数据
    if (user_data.valid != USER_DATA_VALID_VAL) {
        // 如果读出来的数据，校验不通过，说明是第一次上电
        // 初始化变量
        user_data.valid = USER_DATA_VALID_VAL;

        user_data_write(); // 将数据写回flash
#if USER_DEBUG_ENABLE
        my_printf("is first pwr on\n");
#endif
    } else {
#if USER_DEBUG_ENABLE
        my_printf("is not first pwr on\n");
#endif
    }
#endif

#if USER_DEBUG_ENABLE
    user_debug_io_init();
#endif

    // delay_ms(10);
}

// 处理蓝牙服务发送过来的数据
void ble_user_server_message_deal(u8 *buff, u16 len)
{
    if (len < 6) {
        return;
    }
}

// 在 project/ble_all_roles/app/func.c -> func_process() 中调用
// 会循环调用，所以该函数内部不用写 while(1)
void user_main(void)
{
    // printf("main\n");
    USER_DEBUG_IO_TOGGLE();
}
