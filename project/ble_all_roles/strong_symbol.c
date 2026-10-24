/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"
#include "driver_uart.h"

uint8_t cfg_spiflash_speed_up_en = SPIFLASH_SPEED_UP_EN;

uint8_t cfg_bt_rf_def_txpwr;

bool cfg_bt_hsec_wakeup_en   = false;

u8 mem_heap[OS_HEAP_SIZE] AT(.mem_heap);

static const struct thread_cfg_cb thread_cfg =
{
    /*                  Enable                              Priority    Stack   */
    .bt_hw      =       {0,                                 6,          1024},
    .bt_stack   =       {OS_THREAD_BT_STACK_EN,             23,         OS_THREAD_BT_STACK},
    .aupcm      =       {OS_THREAD_AUPCM_STACK_EN,          17,         OS_THREAD_AUPCM_STACK},
    .dirver     =       {OS_THREAD_DRIVER_STACK_EN,         11,         OS_THREAD_DRIVER_STACK},
    .timer      =       {1,                                 16,         OS_THREAD_TIMER_STACK},
    .main       =       {1,                                 25,         OS_THREAD_MAIN_STACK},
    // .main       =       {1,                                 0,         OS_THREAD_MAIN_STACK}, // 优先级改成比蓝牙还高，会导致蓝牙功能无法正常使用
};

uint32_t mem_heap_size_get(void)
{
    return sizeof(mem_heap);
}

struct thread_cfg_cb* thread_cfg_get(void)
{
    return (struct thread_cfg_cb*)&thread_cfg;
}

void thread_create_callback(void)
{
    if (thread_cfg.dirver.enable) {
        thread_driver_create();
    }

    if (thread_cfg.aupcm.enable) {
        thread_aupcm_create();
    }

    if (thread_cfg.bt_stack.enable) {
        thread_btstack_create();
    }
}

void ble_get_local_bd_addr(u8 *addr)
{
    memcpy(addr, xcfg_cb.le_addr, 6);
}

const uint8_t *bt_rf_get_param(void)
{
    cfg_bt_rf_def_txpwr = xcfg_cb.bt_rf_pwrdec;

    if(xcfg_cb.bt_rf_param_en) {
        cfg_bt_rf_def_txpwr = 0;
        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
    } else {
        if (bt_get_ft_trim_value(&xcfg_cb.rf_pa_gain)) {
            return (const uint8_t *)&xcfg_cb.rf_pa_gain;
        }
    }
    return NULL;
}

u8 bt_rf_get_tx_pwr(void)
{
    return xcfg_cb.rf_tx_pwr;
}

void xosc_get_xcfg(u8 *osci_cap, u8 *osco_cap, u8 *both_cap)
{
    *osci_cap = xcfg_cb.osci_cap;
    *osco_cap = xcfg_cb.osco_cap;
    *both_cap = xcfg_cb.osc_both_cap;
}

void param_xosc_read(u8 *param)
{
    bsp_param_read(param, PARAM_BT_XOSC_CAP, PARAM_BT_XOSC_CAP_LEN);
}

void param_xosc_write(u8 *param)
{
    bsp_param_write(param, PARAM_BT_XOSC_CAP, PARAM_BT_XOSC_CAP_LEN);
    bsp_param_sync();
}

AT(.com_text.spiflash)
bool spiflash_data_protect_en(void)
{
    return FLASH_PROTECT_EN;
}

AT(.com_text.spiflash)
bool os_spiflash_addr_check(u32 addr, uint len)
{
    if ((addr >= CM_START) && ((addr + len) <= (CM_START + CM_SIZE))) {
        return true;
    }

#if AB_FOT_EN
#if (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    if ((addr >= FOT_PACK_START) && ((addr + len) <= (FOT_PACK_START + FOT_PACK_SIZE))) {
        return true;
    }
    if ((addr >= 0x1000) && ((addr + len) <= 0x2000)) {
        return true;
    }
#endif
#endif

    printf("-->os_spiflash_addr_err:0x%x\n", addr);

    return false;
}

#if !FLASH_PROTECT_EN
AT(.com_text.spiflash.protect.strong)
void os_spiflash_data_protect(uint8_t en)
{
}

AT(.com_text.spiflash.protect.strong)
bool os_spiflash_data_is_protect(void)
{
    return false;
}
#endif

#if (!LE_DUT_UART_EN)
void hci_fcc_init(void){}
bool bt_uart_isr(void) {return false;}
#endif

AT(.com_text.timer)
void usr_tmr5ms_thread_callback(void)
{
    usr_tmr5ms_thread_do();
}

void sys_clk_change_callback(void)
{
#if BSP_UART_DEBUG_EN
    uart_baud_config(UART, UART_DEBUG_BAUD);
#endif // BSP_UART_DEBUG_EN
}

#if BSP_UART_DEBUG_EN
void uart_debug_init_callback(void)
{
    bsp_uart_debug_init();
}
#else
AT(.com_text.printf)
void uart_debug_putchar(char ch)
{

}

void uart_debug_init_callback(void)
{
    my_printf_init(uart_debug_putchar);
}
#endif // BSP_UART_DEBUG_EN


#if !LE_SM_EN
void ble_sm_server_event_do(uint8_t *packet, uint16_t size){}
void ble_sm_client_event_do(uint8_t *packet, uint16_t size){}
void sm_send_security_request(uint16_t con_handle){}
void sm_request_pairing(uint16_t con_handle){}
void sm_passkey_input(uint16_t con_handle, uint32_t passkey){}
uint32_t sm_passkey_input_get(void){return 0;}
#endif

void ble_get_link_info(void *buf, u16 addr, u16 size)
{
    cm_read(buf, BLE_CM_PAGE(addr), size);
}

void ble_put_link_info(void *buf, u16 addr, u16 size)
{
    cm_write(buf, BLE_CM_PAGE(addr), size);
    cm_sync();
}

bool ble_mtu_exchange_enabled(void)
{
    return LE_MTU_EXCHANGE_EN;
}

uint16_t ble_sm_send_security_req_time_get(void)
{
    return LE_SM_SEND_SECURITY_REQ_TIME;
}

bool ble_sm_always_accept_ltk_req(void)
{
    return LE_SM_ALWAYS_ACCEPT_LTK_REQ_EN;
}

bool ble_sm_pairing_support(void)
{
    return LE_SM_EN;
}

bool ble_disconnected_restart_adv(void)
{
    return LE_DISCONN_RESTART_ADV_EN;
}

bool ble_rx_exit_sleep(void)
{
    return LE_RX_EXIT_SLEEP_EN;
}

AT(.com_text.ble.phy)
bool ble_phy_2M_enable(void)
{
    return LE_PHY_2M_EN;
}

bool ble_feature_exchange_enabled(void)
{
    return LE_FEATURE_EXCHANGE_EN;
}

bool ble_length_request_enabled(void)
{
    return LE_LENGTH_REQ_EN;
}

bool ble_phy_update_enable(void)
{
    return LE_PHY_UPDATE_EN;
}

