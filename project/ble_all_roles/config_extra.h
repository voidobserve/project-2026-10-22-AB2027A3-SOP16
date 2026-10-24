#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if BSP_ADKEY_EN || BSP_IOKEY_EN || BSP_TOUCH_KEY_EN
    #define BSP_KEY_EN              1
#endif

#if BSP_ADKEY_EN || BSP_VBAT_DETECT_EN
    #define BSP_SARADC_EN           1
#endif

#if !BSP_VBAT_DETECT_EN
    #undef LP_PWROFF_EN
    #define LP_PWROFF_EN            0
#endif

#if !SYS_SLEEP_EN
    #undef SYS_SLEEP_TIME
    #define SYS_SLEEP_TIME          0
#endif

#if (FLASH_RESERVE_SIZE % 4096) != 0
    #error "FLASH_RESERVE_SIZE is not a multiple of 4096!"
#endif

#if (AB_FOT_EN && (AB_FOT_TYPE == AB_FOT_TYPE_PACK))
    #if (FOT_PACK_START % 4096) != 0
        #error "FOT_PACK_START is not a multiple of 4096!"
    #endif
#endif

/*****************************************************************************
 * Module    : 线程配置
 *****************************************************************************/
#define OS_THREAD_SIZE_CAL(stack, msg, en)  (en ? (stack + 108 + msg * 112) : 0)
#define OS_HEAP_SIZE    768 +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_BT_STACK, 1, OS_THREAD_BT_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_AUPCM_STACK, 1, OS_THREAD_AUPCM_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_DRIVER_STACK, 1, OS_THREAD_DRIVER_STACK_EN) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_TIMER_STACK, 0, 1) +\
                        OS_THREAD_SIZE_CAL(OS_THREAD_MAIN_STACK, 0, 1)


/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/

#if (FUNC_LE_BQB_RF_EN || FUNC_LE_FCC_EN || LE_FCC_TEST_EN || LE_BQB_RF_EN)
    #define LE_DUT_UART_EN          1
#else
    #define LE_DUT_UART_EN          0
#endif

#if LE_FCC_TEST_EN || LE_BQB_RF_EN
    #undef SYS_SLEEP_TIME
    #define SYS_SLEEP_TIME          0

    #undef SYS_OFF_TIME
    #define SYS_OFF_TIME            0

    #undef SYS_PWRUP_TIME
    #define SYS_PWRUP_TIME          0

    #undef WK0_10S_RESET
    #define WK0_10S_RESET           0
#endif

#if LE_FCC_TEST_EN || FUNC_LE_FCC_EN
    #if (LE_FCC_TEST_HSUART_IO_SEL == BSP_UART_DEBUG_EN)
    #error "LE FCC TEST: LE_FCC_TEST_HSUART_IO_SEL conflicts with BSP_UART_DEBUG_EN!\n"
    #endif  //(LE_FCC_TEST_HSUART_IO_SEL == BSP_UART_DEBUG_EN)
#endif

#if (FUNC_LE_FCC_EN && LE_FCC_TEST_EN)
#error "LE FCC TEST: The FUNC_LE_FCC_EN and LE_FCC_TEST_EN cannot be enabled at the same time!\n"
#endif

#if (FUNC_LE_BQB_RF_EN && LE_BQB_RF_EN)
#error "LE BQB RF TEST: The FUNC_LE_BQB_RF_EN and LE_BQB_RF_EN cannot be enabled at the same time!\n"
#endif


/*****************************************************************************
 * Module    : UART打印功能配置
 *****************************************************************************/
#if !BSP_UART_DEBUG_EN
    //关闭所有打印信息 
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)

#endif


#endif // __CONFIG_EXTRA_H__

