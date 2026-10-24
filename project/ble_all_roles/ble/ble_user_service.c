#include "include.h"
#include "ble_service.h"
#include "ble_init.h"
#include "user_config.h"

/*This macro is used to test the stability of multiple connections in att communication*/
//#define BLE_CONNECTION_STABILITY_TEST

static att_service_handler_t       ff00_service;
static uint16_t ff02_client_config;

#define BLE_CMD_BUF_LEN     4
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)
#define BLE_RX_BUF_LEN      20


struct ble_cmd_t{
    u8 len;
    u8 buf[BLE_RX_BUF_LEN];
    uint16_t handle;
};

struct ble_user_cb_t {
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
    bool pending;
} ble_user_cb;

AT(.com_sleep.ble.sleep)
bool ble_user_service_pending(void)
{
    return ble_user_cb.pending;
}

static void ble_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
            uint8_t status = param[10];
            uint8_t role = param[9];
            /*if peer device is slave, ignore this event*/
            if (role == ROLE_MASTER) {
                return;
            }

            if (status) {
                printf("%s, connection failed\n", __func__);
                return;
            }

            printf("%s BLE_EVT_CONNECT, %s\n", __func__, param[9] ?  "ROLE_SLAVE": "ROLE_MASTER");

            uint16_t con_handle;

            memcpy(&con_handle, &param[7], 2);

            uint8_t conidx = ble_get_free_conidx(con_handle);

            if (conidx != INVALID_CONIDX) {
                ble_cb_t *ble_cb_node = ble_get_ble_info();

                memcpy(&ble_cb_node[conidx].conhdl, &param[7], 2);
                printf("-->conhdl %x, conidx %x\n",  ble_cb_node[conidx].conhdl, conidx);
                ble_cb_node[conidx].role = param[9];
                ble_conn_state_set(1, conidx, param[9]);
            } else {
                printf("%s conidx(%d) must less than (%d)\n", conidx, MAX_BLE_CONNECTIONS);
            }
        } break;

        case BLE_EVT_DISCONNECT:{
            uint16_t con_handle;
            uint8_t disc_reason = param[2];
            memcpy(&con_handle, &param[0], 2);

            uint8_t conidx = ble_get_conidx(con_handle);

            if (conidx == INVALID_CONIDX) {
                return;
            }

            uint8_t role = ble_get_role(conidx);

            /*if peer device is slave, ignore this event*/
            if (role == ROLE_MASTER) {
                return;
            }

            printf("%s BLE_EVT_DISCONNECT, role %x, conidx %d, conhdl %x, disc_reason %x\n", __func__, role, conidx, con_handle, disc_reason);

            ble_cb_t *ble_cb_node = ble_get_ble_info();

            ble_conn_state_set(0, conidx, ROLE_UNKNOWN);

            ble_cb_node[conidx].conhdl = 0;
            ble_cb_node[conidx].role = ROLE_UNKNOWN;
            ff02_client_config = CCCD_DFT;
        } break;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE: {
            printf("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            u8 status = param[0];
            u16 handle = param[1] | (param[2] << 8);
            u16 interval = param[3] | (param[4] << 8);
            u16 latency = param[5] | (param[6] << 8);
            u16 timeout = param[7] | (param[8] << 8);
            printf("%d, %d, %d, %d, %d\n", status, handle, interval, latency, timeout);
        } break;

        default:
            break;
    }
}

static uint16_t service_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    printf("service_read_callback\n");

    if(attribute_handle == ATT_CHARACTERISTIC_FF01_01_VALUE_HANDLE){
        u8 read_data[] = "hello";
        u8 data_len = sizeof(read_data) - 1;
        if(buffer){
            data_len = (buffer_size < (data_len - offset))? buffer_size: (data_len - offset);
            memcpy(buffer, read_data + offset, data_len);
        }
        return data_len;
    }

	return 0;
}

static int service_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    printf("service_write_callback\n");

    if(attribute_handle == ATT_CHARACTERISTIC_FF02_01_CLIENT_CONFIGURATION_HANDLE){
        ff02_client_config = GET_LE16(&buffer[0]);
    }else if(attribute_handle == ATT_CHARACTERISTIC_FF01_01_VALUE_HANDLE){
        printf("BLE RX [%d]: \n", buffer_size);
        print_r(buffer, buffer_size);

#if USER_DEBUG_ENABLE
#if 0
        // 打印接收到的数据：
        u32 i;
        my_printf("recv form service_write: \n");
        for (i = 0; i <buffer_size; i++) {
            my_printf("%02x ", buffer[i]);
        }
        my_printf("\n");
#endif
#endif

        ble_user_server_message_deal(buffer, buffer_size);

        // u8 wptr = ble_user_cb.cmd_wptr & BLE_CMD_BUF_MASK;
        // ble_user_cb.cmd_wptr++;
        // if (buffer_size > BLE_RX_BUF_LEN) {
        //     buffer_size = BLE_RX_BUF_LEN;
        // }
        // memcpy(ble_user_cb.cmd[wptr].buf, buffer, buffer_size);
        // ble_user_cb.cmd[wptr].len = buffer_size;
        // ble_user_cb.cmd[wptr].handle = attribute_handle;
        // ble_user_cb.pending = 1;

        #if (SYS_SLEEP_TIME != 0)
        lowpwr_sleep_delay_reset();
        #endif

        #if (SYS_OFF_TIME != 0)
        lowpwr_pwroff_delay_reset();
        #endif
    }

	return ATT_ERR_NO_ERR;
}

void ble_user_service_init(void)
{
    printf("ble_user_service_init\n");

    // get service handle range
	uint16_t start_handle = ATT_SERVICE_FF00_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_FF00_END_HANDLE;

    // register service with ATT Server
	ff00_service.start_handle   = start_handle;
	ff00_service.end_handle     = end_handle;
	ff00_service.read_callback  = &service_read_callback;
	ff00_service.write_callback = &service_write_callback;
	ff00_service.event_handler  = &ble_event_callback;
	att_server_register_service_handler(&ff00_service);

	ff02_client_config = CCCD_DFT;
}

#ifdef BLE_CONNECTION_STABILITY_TEST
int ble_user_send_data(uint8_t conidx, uint8_t* buf, uint16_t len)
{
    int ret = 1;
    ble_cb_t *ble_cb_node = ble_get_ble_info();

    /*send data mobile phone*/
    if (ble_cb_node[conidx].role == ROLE_SLAVE) {
        ret = ble_notify_for_handle(ble_cb_node[conidx].conhdl, ATT_CHARACTERISTIC_FF02_01_VALUE_HANDLE, buf, len);
    } else if (ble_cb_node[conidx].role == ROLE_MASTER) {
        /*send data to slave*/
        uint16_t uuid = 0xff01;

        server_info_t *svc_info = ble_get_server_info(conidx);
        uint8_t num = svc_info->characteristic_cnt;

        gatt_client_characteristic_t *found = ble_get_characteristic_for_uuid16(svc_info->characteristic, num, uuid);

        ret = ble_write_req_for_character(ble_cb_node[conidx].conhdl, found, buf, len);
    }

    printf("%s:role %x, conidx %d, conhdl %d, ret %u\n", __func__, ble_cb_node[conidx].role, conidx, ble_cb_node[conidx].conhdl, ret);

    return ret;
}

void ble_connect_stability_test(void)
{
    static uint8_t step = 0;
    static uint32_t stability_test_tick = 0;

    if (!tick_check_expire(stability_test_tick, 1000)) {
        return;
    }

    uint8_t send_buf[8];

    for (uint8_t i = 0; i < sizeof(send_buf) / sizeof(send_buf[0]); i++) {
        send_buf[i] = i;
    }

    stability_test_tick = tick_get();

    ble_user_send_data(step, send_buf, 8);
    step += 1;

    if (step >= MAX_BLE_CONNECTIONS) {
        step = 0;
    }
}
#endif

void ble_user_service_proc(void)
{
    #ifdef BLE_CONNECTION_STABILITY_TEST
    ble_connect_stability_test();
    #endif

    if (ble_user_cb.cmd_rptr == ble_user_cb.cmd_wptr) {
        ble_user_cb.pending = 0;
        return;
    }
    u8 rptr = ble_user_cb.cmd_rptr & BLE_CMD_BUF_MASK;
    ble_user_cb.cmd_rptr++;
    u8 *ptr = ble_user_cb.cmd[rptr].buf;
    u8 len = ble_user_cb.cmd[rptr].len;
    //uint16_t handle = ble_user_cb.cmd[rptr].handle;

    if(ff02_client_config){
        printf("BLE TX [%d]: \n", len);
        print_r(ptr, len);
        uint8_t conidx;
        ble_cb_t *ble_cb_node = ble_get_ble_info();

        // Find first slave connection handle
        for (conidx = 0; conidx < MAX_BLE_CONNECTIONS; conidx ++) {

            if (ble_cb_node[conidx].role == ROLE_MASTER) {
                break;
            }
        }

        // No connection index found
        if (conidx == MAX_BLE_CONNECTIONS) {
            // Error, return wrong connection index
            conidx = INVALID_CONIDX;
        }

        if (conidx != INVALID_CONIDX) {
            printf("BLE TX handle[%d]\n", ble_cb_node[conidx].conhdl);
            ble_notify_for_handle(ble_cb_node[conidx].conhdl, ATT_CHARACTERISTIC_FF02_01_VALUE_HANDLE, ptr, len);
        }
    }
}
