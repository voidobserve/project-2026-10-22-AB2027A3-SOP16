#include "include.h"
#include "tbox_key.h"
#include "tbox_voice.h"
#include "prod_test_tbox.h"

#if FUNC_TBOX_EN
///////////////////////////////////////////////////////////////////////////

#define MAX_NOTIFY_NUM          10
#define MAX_NOTIFY_LEN          132
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

tbox_var_t tbox_var AT(.tbox.var);
static att_service_handler_t  tbox_service AT(.tbox.service);
static tbox_rx_cb_t tbox_rx_cb AT(.tbox.rx);
static uint8_t tbox_notify_tx_pool[NOTIFY_POOL_SIZE] AT(.tbox.att);

// const uint8_t tbox_profile_data[] =
const uint8_t tbox_profile_data[] =
{
    //TBOX
    // 0x0001 PRIMARY_SERVICE-FF80
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x80, 0xff,
    // 0x0002 CHARACTERISTIC-8000 - READ | WRITE_WITHOUT_RESPONSE | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x06, 0x03, 0x00, 0x00, 0x80,
    // 0x0003 VALUE CHARACTERISTIC-8000 - READ | WRITE_WITHOUT_RESPONSE | DYNAMIC
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x80,
    // 0x0004 CHARACTERISTIC-8001 - NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x10, 0x05, 0x00, 0x01, 0x80,
    // 0x0005 VALUE CHARACTERISTIC-8001 - NOTIFY -''
    //
    0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x80,
    // 0x0006 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x06, 0x00, 0x02, 0x29, 0x00, 0x00,
    // 0x0007 CHARACTERISTIC-8002 - NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x07, 0x00, 0x03, 0x28, 0x10, 0x08, 0x00, 0x02, 0x80,
    // 0x0008 VALUE CHARACTERISTIC-8002 - NOTIFY -''
    //
    0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x80,
    // 0x0009 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x09, 0x00, 0x02, 0x29, 0x00, 0x00,

    // END
    0x00, 0x00,
}; // total size 51 bytes

//----------------------------------------------------------------------------
AT(.text.app.proc.ble)
void tbox_set_key_msg(uint8_t msg)
{
    tbox_var.key_msg = msg;
}

AT(.text.app.proc.ble)
uint8_t tbox_get_key_msg(void)
{
    return tbox_var.key_msg;
}

AT(.text.prod_test.tbox_mode)
bool is_prod_test_tbox_mode(void)
{
    return tbox_var.state;
}

AT(.text.prod_test.txpkt_init)
static void tbox_txpkt_init(void)
{
    memset(&ble_tx, 0x00, sizeof(ble_tx));
    txpkt_init(&ble_tx, tbox_notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    ble_tx.send_kick = ble_send_kick;
}

AT(.text.prod_test.var_init)
static void tbox_service_var_init(void)
{
    memset(&tbox_var, 0, sizeof(tbox_var_t));
#if (PROD_TEST_KEY_EN)    
    tbox_var.key_msg = TBOX_KEY_MSG_NONE;
#endif //(PROD_TEST_KEY_EN)   
}

AT(.text.prod_test.event_cb)
static void tbox_service_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            //产测模式没有BLE_EVT_CONNECT事件
            // memcpy(&tbox_var.conn_handle, &param[7], 2);
            // printf("-->BLE_VOICE_EVENT_CONNECTED:%x\n",tbox_var.conn_handle);
        } break;

        case BLE_EVT_DISCONNECT:{
            uint8_t disc_reason = param[2];
            memcpy(&tbox_var.conn_handle, &param[0], 2);
            printf("-->BLE_VOICE_EVENT_DISCONNECTED:%x, disc_reason:0x%x\n",tbox_var.conn_handle, disc_reason);
            tbox_var.state = 0;
#if (PROD_TEST_MIC_EN)
            tbox_voice_stop();
#endif //(PROD_TEST_MIC_EN)
            func_cb.sta = FUNC_BT;
        } break;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            uint8_t status = param[0];
            uint16_t handle = param[1] | (param[2] << 8);
            uint16_t interval = param[3] | (param[4] << 8);
            uint16_t latency = param[5] | (param[6] << 8);
            uint16_t timeout = param[7] | (param[8] << 8);
            printf("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
        } break;

        default:
            break;
    }
}

AT(.text.prod_test.read_cb)
static uint16_t tbox_service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    uint16_t read_len = 0;
    printf("tbox read cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE:{
            printf("READ ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE\n");
        } break;

        default:
            break;
    }

	return read_len;
}

AT(.text.prod_test.write_cb)
static int tbox_service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("tbox write cb, handle:0x%x, offset:0x%x, buf_size:0x%x\n", attribute_handle, offset, buffer_size);
    switch (attribute_handle) {
        case ATT_CHARACTERISTIC_8001_01_CLIENT_CONFIGURATION_HANDLE:
            tbox_var.conn_handle = con_handle;   //产测模式没有BLE_EVT_CONNECT事件，con_handle在此处赋值
            tbox_var.key_client_config = GET_LE16(&buffer[0]);
            ble_update_conn_param(con_handle, 26, 0, 300);  //32ms
            break;

        case ATT_CHARACTERISTIC_8002_01_CLIENT_CONFIGURATION_HANDLE:
            tbox_var.voice_client_config = GET_LE16(&buffer[0]);
            ble_update_conn_param(con_handle, 26, 0, 300);  //32ms
            break;

        case ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE:
            printf("WRITE ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE\n");
            break;

        default:
            break;
    }

	return ATT_ERR_NO_ERR;
}

AT(.text.app.proc.tbox_rx_cmd)
static void tbox_service_rx_cmd_process(void)
{
    if (tbox_rx_cb.cmd_rptr == tbox_rx_cb.cmd_wptr) {
        return;
    }
    // uint8_t rptr = tbox_rx_cb.cmd_rptr & BLE_TBOX_CMD_BUF_MASK;
    // uint8_t *buf = tbox_rx_cb.cmd[rptr].buf;
    // uint8_t buf_size = tbox_rx_cb.cmd[rptr].len;
    // uint16_t handle = tbox_rx_cb.cmd[rptr].handle;
    // tbox_rx_cb.cmd_rptr++;
    // switch (handle) {
    //     case ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE:
    //         break;

    //     default:
    //         break;
    // }
}

AT(.text.app.proc.tbox)
void prod_test_tbox_proc(void)
{
    if (is_prod_test_tbox_mode() == 0) {
        return;
    }

#if PROD_TEST_KEY_EN
    tbox_key_proc();
#endif //PROD_TEST_KEY_EN

#if PROD_TEST_MIC_EN
    tbox_voice_proc();
#endif //PROD_TEST_MIC_EN
    tbox_service_rx_cmd_process();
}

AT(.text.prod_test.service_init)
static void tbox_service_init(void)
{
    printf("tbox service init\n");
    // get service handle range
	uint16_t start_handle = ATT_SERVICE_FF80_01_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_FF80_01_END_HANDLE;

    // register service with ATT Server
	tbox_service.start_handle   = start_handle;
	tbox_service.end_handle     = end_handle;
	tbox_service.read_callback  = &tbox_service_read_callback;
	tbox_service.write_callback = &tbox_service_write_callback;
	tbox_service.event_handler  = &tbox_service_event_callback;
	att_server_register_service_handler(&tbox_service);
}

AT(.text.prod_test.tbox_init)
static void tbox_init(void)
{
    tbox_service_var_init();
    tbox_var.state = 1;
    tbox_txpkt_init();
    att_server_init(tbox_profile_data);
    tbox_service_init();
#if PROD_TEST_MIC_EN
    tbox_voice_init();
#endif //PROD_TEST_MIC_EN
}

AT(.text.prod_test.tbox_init)
void prod_test_tbox_init(void)
{
    tbox_service_var_init();
    ble_vs_tbox_init_callback_register(tbox_init);
}

#endif  //FUNC_TBOX_EN
