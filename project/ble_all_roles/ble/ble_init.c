#include "include.h"
#include "ble_client.h"


///////////////////////////////////////////////////////////////////////////
#define MAX_TX_NUM          8
#define MAX_TX_LEN          128     //max=512
#define TX_POOL_SIZE       (MAX_TX_LEN + sizeof(struct txbuf_tag)) * MAX_TX_NUM

ble_cb_t ble_cb[MAX_BLE_CONNECTIONS] AT(.ble_buf.var);
BT_WORK_MODE cfg_bt_work_mode            = MODE_NORMAL;

AT(.ble_cache.att)
uint8_t ble_tx_pool[TX_POOL_SIZE];

void ble_info_init(void)
{
    memset(&ble_cb, 0, sizeof(ble_cb));

    for (uint8_t i = 0; i < MAX_BLE_CONNECTIONS; i ++) {
        ble_cb[i].role = ROLE_UNKNOWN;
    }
}

ble_cb_t *ble_get_ble_info(void)
{
    return ble_cb;
}

uint8_t ble_get_free_conidx(uint16_t conhdl)
{
    uint8_t conidx = ble_get_conidx(conhdl);

    if (conidx != INVALID_CONIDX) {
        return conidx;
    }

    // Find first available connection index
    for (conidx = 0; conidx < MAX_BLE_CONNECTIONS; conidx ++) {
        // Find first index which is free.
        if (ble_cb[conidx].role == ROLE_UNKNOWN) {
            break;
        }
    }

    // No connection index found
    if (conidx == MAX_BLE_CONNECTIONS) {
        // Error, return wrong connection index
        conidx = INVALID_CONIDX;
    }

    return conidx;
}

uint8_t ble_get_conidx(uint16_t conhdl)
{
    uint8_t conidx;

    // Find first available connection index
    for (conidx = 0; conidx < MAX_BLE_CONNECTIONS; conidx ++) {
        // Find first index which is free.
        if (ble_cb[conidx].role != ROLE_UNKNOWN && ble_cb[conidx].conhdl == conhdl) {
            break;
        }
    }

    // No connection index found
    if (conidx == MAX_BLE_CONNECTIONS) {
        // Error, return wrong connection index
        conidx = INVALID_CONIDX;
    }

    return conidx;
}

uint8_t ble_get_role(uint8_t conidx)
{
    uint8_t ret = ROLE_UNKNOWN;

    if ((conidx != INVALID_CONIDX) && (ble_cb[conidx].role != ROLE_UNKNOWN)) {
        ret = ble_cb[conidx].role;
    }

    return ret;
}

void ble_set_work_mode(BT_WORK_MODE mode)
{
    cfg_bt_work_mode = mode;
}

BT_WORK_MODE ble_get_work_mode(void)
{
    return cfg_bt_work_mode;
}
void ble_txpkt_init(void)
{
    memset(&ble_tx, 0x00, sizeof(ble_tx));

    txpkt_init(&ble_tx, ble_tx_pool, MAX_TX_NUM, MAX_TX_LEN);
    ble_tx.send_kick = ble_send_kick;
}

void ble_sm_param_init(void)
{
    uint8_t security_req_en = 0;
    uint8_t auth_req = SM_AUTHREQ_BONDING | SM_AUTHREQ_MITM_PROTECTION;
    uint8_t io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT;

    ble_sm_init(security_req_en, io_capability, auth_req);
}

void ble_adv_param_init(void)
{
    adv_param_t adv_info;
    memset(&adv_info, 0, sizeof(adv_param_t));
    adv_info.adv_len = ble_get_adv_data(adv_info.adv_data, sizeof(adv_info.adv_data));
    adv_info.scan_rsp_len = ble_get_scan_data(adv_info.scan_rsp, sizeof(adv_info.scan_rsp));
    adv_info.adv_int_min = adv_info.adv_int_max = 0x30; // 广播间隔设置为0x30 (48 * 0.625ms = 30ms)
    adv_info.adv_type = ADV_TYPE_IND; // 广播类型：可连接的非定向广播
    adv_info.channel_map = ADV_ALL_CHNLS_EN; // 启用所有广播通道 (37, 38, 39)
    adv_info.adv_en = 1;

    ble_set_adv_param(&adv_info);
}

void ble_scan_param_init(void)
{
    scan_param_t scan_param;

    scan_param.scan_type = 1;
    scan_param.scan_interval = 500;
    scan_param.scan_window = 250;
    ble_set_scan_param(&scan_param);
    ble_scan_en();
}

void ble_all_roles_init(void)
{
    ble_txpkt_init();
#if LE_SM_EN
    ble_sm_param_init();
#endif
    gatt_client_init();
    gap_random_address_set_mode(GAP_PUBLIC_ADDRESS_TYPE);
    att_server_init(ble_get_profile_data());
    ble_service_init();
    ble_client_init();

#if (MAX_SLAVE_NR > 0)
    ble_adv_param_init();
#endif

#if (MAX_MASTER_NR > 0)
    ble_scan_param_init();
#endif

    ble_info_init();
}

void bsp_ble_init(void)
{
    bt_init();

    ble_init_callback_register(ble_all_roles_init);

    bt_setup();
}
