#include "include.h"

#if LE_FOTA_EN

static uint16_t fota_conhdl = 0;

#define FOT_DEBUG_EN                    0

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif

#define FOT_STA_INIT            BIT(0)
#define FOT_STA_START           BIT(1)
#define FOT_STA_PAUSE           BIT(2)

#define FOT_FLAG_UPDATE_OK      BIT(0)
#define FOT_FLAG_SYS_RESET      BIT(1)
#define FOT_FLAG_APP_CONNECT    BIT(2)
#define FOT_FLAG_CLK_SET        BIT(3)
#define FOT_FLAG_UPDATE_EXIT    BIT(4)


#define FOT_NOTIFY_STA          0x90
#define FOT_GET_INFO            0x91
#define FOT_GET_INFO_TLV        0x92
#define FOT_OUT_DATA_START      0xA0
#define FOT_OUT_DATA_CONTINUE   0x20


#define FOT_CMD_POS             0
#define FOT_SEQ_POS             1
#define FOT_ADDR_POS            2
#define FOT_DATA_LEN_POS        6
#define DATA_START_POS          10
#define DATA_CONTINUE_POS       2

#define FOT_BLOCK_LEN           512

typedef enum{
    INFO_DEV_VER  = 1,
    INFO_UPDATE_REQ,
    INFO_DEV_FEATURE,
    INFO_DEV_CONNECT_STA,
    INFO_EDR_ADDR,
    INFO_DEV_CHANNEL,
}DEV_INFO_E;

typedef struct __attribute__((packed)){
    u8 seq;
    u8 remote_seq;
    u8 sta;
    u8 flag;
    u16 con_sta;
    u16 dev_ver;
    u16 remote_ver;
    u32 total_len;
    u32 remain_len;
    u32 hash;
    u16 data_pos;
    u8 recv_ok;
    u32 addr;
    u32 tick;
}fot_s;

fot_s fot_var;

static u8 fot_data[FOT_BLOCK_LEN] AT(.fot_data.buf);

static att_service_handler_t       fota_service;
static uint16_t fota_client_config;

static const u8 fot_auth_data[] = {0xCC, 0xAA, 0x55, 0xEE, 0x12, 0x19, 0xE4};

void ab_fota_write(void *buf, u32 addr, u32 len)
{
    ota_pack_write(buf);
}

u8 ab_fota_get_err(void)
{
    return ota_pack_get_err();
}


bool ab_fota_breakpoint_info_read(void)
{
    return ota_pack_breakpoint_info_read();
}

void ab_fota_init(void)
{
    ota_pack_init();
}

void ab_fota_deinit(void)
{
    ota_pack_deinit();
}

u32 ab_fota_get_curaddr(void)
{
    return ota_pack_get_curaddr();
}

bool ab_fota_is_update_success(void)
{
    return ota_pack_is_update_success();
}

AT(.text.fot.update)
void ble_fota_init(void)
{
    u8  dev_version_str[] = SW_VERSION;
    u16 version_temp = 0;

    memset(&fot_var, 0, sizeof(fot_var));

    fot_var.dev_ver = 0x00;
    version_temp = dev_version_str[1]-'0';
    version_temp <<= 12;
    fot_var.dev_ver |= version_temp;

    version_temp = dev_version_str[3]-'0';
    version_temp <<= 8;
    fot_var.dev_ver |= version_temp;

    version_temp = dev_version_str[5]-'0';
    version_temp <<= 4;
    fot_var.dev_ver |= version_temp;

    fot_var.sta = FOT_STA_INIT;
}

void ble_fota_exit(void)
{
    if(fot_var.flag & FOT_FLAG_APP_CONNECT){
        fot_var.sta = 0;
        fot_var.flag &= ~FOT_FLAG_APP_CONNECT;
        ab_fota_deinit();
    }
}

AT(.com_text.fot)
static u8 fot_app_connect_auth(uint8_t *packet, uint16_t size)
{
    if(!fot_var.sta){
        if((size == sizeof(fot_auth_data)) && !memcmp(fot_auth_data, packet, sizeof(fot_auth_data))){
            ble_fota_init();
            fot_var.flag |= FOT_FLAG_APP_CONNECT;
            return 0;
        }
    }

    return fot_var.sta;
}


AT(.text.fot.cache)
static void fot_sent_proc(u8 *buf,u8 len)
{
    if((fot_var.flag & FOT_FLAG_APP_CONNECT) == 0){
        return;
    }

    if(fota_client_config){
        FOT_DEBUG("fot tx:");
        FOT_DEBUG_R(buf,len);

        ble_notify_for_handle(fota_conhdl, ATT_CHARACTERISTIC_FF14_01_VALUE_HANDLE, buf, len);
    }
}

AT(.text.fot.update)
static void fot_reply_info_tlv(u8 *buf,u8 len)
{
    u8 read_offset = 0;
    u8 write_offset = 0;
    u8 rsp[32];
    u8 val_len = 0;

    if((buf == NULL) || (len == 0)){
        return;
    }

    rsp[write_offset++] = FOT_GET_INFO_TLV;
    rsp[write_offset++] = fot_var.seq++;

    while(read_offset < len){
        switch(buf[read_offset]){
            case INFO_DEV_VER:
                FOT_DEBUG("INFO_DEV_VER\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_VER;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_var.dev_ver & 0xff;
                rsp[write_offset++] = (fot_var.dev_ver >> 8) & 0xff;
            break;

            case INFO_DEV_FEATURE:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                {
                    u16 dev_ability = 0;
                    val_len = buf[read_offset + 1];
                    rsp[write_offset++] = INFO_DEV_FEATURE;
                    rsp[write_offset++] = 2;
                    rsp[write_offset++] = dev_ability & 0xff;
                    rsp[write_offset++] = (dev_ability >> 8) & 0xff;
                }
            break;

            case INFO_DEV_CONNECT_STA:
                FOT_DEBUG("INFO_DEV_CONNECT_STA\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_CONNECT_STA;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_var.con_sta & 0xff;
                rsp[write_offset++] = (fot_var.con_sta >> 8) & 0xff;
                break;

            default:
                val_len = buf[read_offset + 1];
                break;
        }
        read_offset += (2 + val_len);
    }

    if(write_offset > sizeof(rsp)){
        printf("fot:rsp buf overflow!!!\n");
        while(1);
    }

    fot_sent_proc(rsp, write_offset);
}


AT(.text.fot.update)
static void fot_reply_dev_version(void)
{
    u8 data[5];

    data[0] = FOT_GET_INFO;
    data[1] = fot_var.seq++;
    data[2] = INFO_DEV_VER;
    memcpy(&data[3], &fot_var.dev_ver, 2);

    fot_sent_proc(data,5);
}

AT(.text.fot.cache)
static void fot_dev_notify_sta(u8 sta)
{
    u8 buf[3];

    buf[0] = FOT_NOTIFY_STA;
    buf[1] = fot_var.seq++;
    buf[2] = sta;

    fot_sent_proc(buf,3);
}

AT(.com_text.fot.update)
u8 ble_fota_is_start(void)
{
    return (fot_var.sta & FOT_STA_START) ? 1:0;
}

AT(.text.fot.update)
void ble_fota_pause(void)
{
    if(fot_var.sta & FOT_STA_START){
        fot_var.sta = FOT_STA_PAUSE;
        fot_dev_notify_sta(FOT_UPDATE_PAUSE);
    }
}

AT(.text.fot.update)
void ble_fota_continue(void)
{
    if(fot_var.sta & FOT_STA_PAUSE){
        fot_var.sta = FOT_STA_INIT;
        fot_var.total_len = 0;
        fot_var.remain_len = 0;
        fot_var.data_pos = 0;
        fot_dev_notify_sta(FOT_UPDATE_CONTINUE);
    }
}


AT(.text.fot.update)
static void fot_reply_update_request(void)
{
    u32 hash;
    u32 addr = 0;
    u32 block_len = FOT_BLOCK_LEN;
    u16 packet_len = 0;
    u8 data[14];
    u8 need_update = 1;

    att_server_get_mtu(fota_conhdl, &packet_len);

    packet_len -= 3;

    if(packet_len > (FOT_BLOCK_LEN + DATA_START_POS)){
        packet_len = FOT_BLOCK_LEN + DATA_START_POS;
    }

    ble_update_conn_param(fota_conhdl, 12, 0, 400);


    printf("fot_packet_len:%d\n",packet_len);

    ab_fota_init();

    fot_var.sta |= FOT_STA_START;

    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);

    if((fot_var.hash != 0xFFFFFFFF) && (hash == fot_var.hash)){
        if(ab_fota_breakpoint_info_read() == true){
            addr = ab_fota_get_curaddr();
        }
    }else{
        param_fot_hash_write((u8*)&fot_var.hash);
    }


    data[0] = FOT_GET_INFO;
    data[1] = fot_var.seq++;
    data[2] = INFO_UPDATE_REQ;
    memcpy(&data[3], &addr, 4);
    memcpy(&data[7], &block_len, 4);
    memcpy(&data[11], &packet_len, 2);
    data[13] = need_update;

    fot_sent_proc(data,14);
}


void fota_ble_disconnect_callback(void)
{
    FOT_DEBUG("--->fota_ble_disconnect_callback\n");

    fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
}

void fota_ble_connect_callback(void)
{
    FOT_DEBUG("--->fota_ble_connect_callback\n");
}


//------------------------------------------------------------------------------
AT(.text.fot.cache)
void fot_recv_proc(u8 *buf, u16 len)
{
    u32 addr;
    u32 recv_data_len;

    if(fot_var.remote_seq != buf[FOT_SEQ_POS]){
        printf("remote seq err:%d,%d\n",fot_var.remote_seq, buf[FOT_SEQ_POS]);
        fot_dev_notify_sta(FOT_ERR_SEQ);
        fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
        return;
    }

    fot_var.remote_seq++;

    if((fot_var.sta & FOT_STA_INIT) == 0){
        return;
    }

    if(buf[FOT_CMD_POS] == FOT_GET_INFO_TLV){
        fot_reply_info_tlv(&buf[2], len - 2);
    }else if(buf[FOT_CMD_POS] == FOT_GET_INFO){
        if(buf[2] == INFO_DEV_VER){
            fot_reply_dev_version();
        }else if(buf[2] == INFO_UPDATE_REQ){
            memcpy(&fot_var.remote_ver, &buf[3], 2);
            memcpy(&fot_var.hash,&buf[5], 4);
            fot_reply_update_request();
        }
    }else if(buf[FOT_CMD_POS] == FOT_OUT_DATA_START){
        if(fot_var.remain_len){
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
            fot_dev_notify_sta(FOT_ERR_DATA_LEN);
            fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            return;
        }
        recv_data_len = len-DATA_START_POS;
        memcpy(&fot_var.total_len, &buf[FOT_DATA_LEN_POS], 4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&addr, &buf[FOT_ADDR_POS], 4);
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_START_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }else if(buf[FOT_CMD_POS] == FOT_OUT_DATA_CONTINUE){
        recv_data_len = len - DATA_CONTINUE_POS;
        if(fot_var.remain_len < recv_data_len){
           recv_data_len = fot_var.remain_len;
        }
        fot_var.remain_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos], &buf[DATA_CONTINUE_POS], recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
            fot_var.recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }
}

static void fota_event_callback(uint8_t event_type, uint8_t *param, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_CONNECT:{
                uint8_t status = packet[10];
                uint8_t role = packet[9];
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
                    printf("-->conhdl %x, conidx %x\n", ble_cb_node[conidx].conhdl, conidx);
                    ble_cb_node[conidx].role = param[9];
                    ble_conn_state_set(1, conidx, param[9]);
                    fota_conhdl = ble_cb_node[conidx].conhdl;
                } else {
                    printf("%s conidx(%d) must less than (%d)\n", conidx, MAX_BLE_CONNECTIONS);
                }

                fota_ble_connect_callback();
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
            fota_conhdl = 0;
            fota_client_config = CCCD_DFT;
            fota_ble_disconnect_callback();
        } break;

        default:
            break;
    }
}

static uint16_t fota_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    if(attribute_handle == ATT_CHARACTERISTIC_FF15_01_VALUE_HANDLE){
        u8 read_data[] = "FOTA";
        if(buffer){
            memcpy(buffer, read_data, sizeof(read_data));
        }
        return sizeof(read_data);
    }

	return 0;
}

static int fota_write_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    if(attribute_handle == ATT_CHARACTERISTIC_FF14_01_CLIENT_CONFIGURATION_HANDLE){
        fota_client_config = GET_LE16(&buffer[0]);
    }else if(attribute_handle == ATT_CHARACTERISTIC_FF15_01_VALUE_HANDLE){
        if(fot_app_connect_auth(buffer,buffer_size)){
            fot_recv_proc(buffer, buffer_size);
        }
    }

	return ATT_ERR_NO_ERR;
}

void ble_fota_service_init(void)
{
    printf("ble_fota_service_init\n");

    // get service handle range
	uint16_t start_handle = ATT_SERVICE_FF12_START_HANDLE;
	uint16_t end_handle   = ATT_SERVICE_FF12_END_HANDLE;

    // register service with ATT Server
	fota_service.start_handle   = start_handle;
	fota_service.end_handle     = end_handle;
	fota_service.read_callback  = &fota_read_callback;
	fota_service.write_callback = &fota_write_callback;
	fota_service.event_handler  = &fota_event_callback;
	att_server_register_service_handler(&fota_service);

	fota_client_config = CCCD_DFT;
}

AT(.text.fot.cache)
void ble_fota_process_do(void)
{
    if(fot_var.recv_ok){
        //FOT_DEBUG("--->fot_recv_ok\n");
        fot_var.recv_ok = 0;
        ab_fota_write(fot_data, ab_fota_get_curaddr(), fot_var.total_len);

        if(ab_fota_is_update_success()){
            FOT_DEBUG("--->fot update success\n");
            fot_var.flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
            fot_var.tick = tick_get();
            fot_dev_notify_sta(FOT_UPDATE_DONE);
        }else{
            fot_dev_notify_sta(ab_fota_get_err());
            if(ab_fota_get_err()){
                fot_var.flag |= FOT_FLAG_UPDATE_EXIT;
            }
        }
    }
}

AT(.text.fot.cache)
void ble_fota_flag_do(void)
{
    if(fot_var.flag & FOT_FLAG_SYS_RESET){
        if(tick_check_expire(fot_var.tick, 3000)){
            fot_var.flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("-->fota update ok,sys reset\n");
            WDT_RST();
        }
    }

    if(fot_var.flag & FOT_FLAG_UPDATE_EXIT){
        ble_fota_exit();
        fot_var.flag &= ~FOT_FLAG_UPDATE_EXIT;
    }
}

AT(.text.app.proc.fot)
void ble_fota_process(void)
{
    if(fot_var.sta & FOT_STA_START){
        lowpwr_pwroff_delay_reset();
        lowpwr_sleep_delay_reset();
        ble_fota_process_do();
    }

    if(fot_var.flag){
        ble_fota_flag_do();
    }
}
#endif
