#include "include.h"
#include "ble_ancs_client.h"
#include "api_btstack.h"

#define ANCS_CLIENT_DEBUG_EN 1

#if ANCS_CLIENT_DEBUG_EN
#define log_info(...)                  printf(__VA_ARGS__)
#define log_info_r(...)                print_r(__VA_ARGS__)
#else
#define log_info(...)
#define log_info_r(...)
#endif

static ancs_client_data_src_t data_src AT(.ble_buf.ancs);
static ancs_client_ntf_src_t ntf_src AT(.ble_buf.ancs);

static ancs_server_info_t ancs_server_info AT(.ble_buf.ancs);
static att_client_handler_t  ancs_client_handler AT(.ble_buf.ancs);
static ancs_client_state_t ancs_tc_state = STA_IDLE;

static const uint8_t ancs_service_uuid[] =             {0x79,0x05,0xF4,0x31,0xB5,0xCE,0x4E,0x99,0xA4,0x0F,0x4B,0x1E,0x12,0x2D,0x00,0xD0};
static const uint8_t ancs_notification_source_uuid[] = {0x9F,0xBF,0x12,0x0D,0x63,0x01,0x42,0xD9,0x8C,0x58,0x25,0xE6,0x99,0xA2,0x1D,0xBD};
static const uint8_t ancs_control_point_uuid[] =       {0x69,0xD1,0xD8,0xF3,0x45,0xE1,0x49,0xA8,0x98,0x21,0x9B,0xBD,0xFD,0xAA,0xD9,0xD9};
static const uint8_t ancs_data_source_uuid[] =         {0x22,0xEA,0xC6,0xE9,0x24,0xD6,0x4B,0xB5,0xBE,0x44,0xB3,0x6A,0xCE,0x7C,0x7B,0xFB};

static const char *ancs_app_identifier[APP_IDENTIFIER_MAX] = {
    [WEIXIN]        =   "com.tencent.xin",
    [MOBILE_SMS]    =   "com.apple.MobileSMS",
    [MOBILE_PHONE]  =   "com.apple.mobilephone",
    [MQQ]           =   "com.tencent.mqq",
    [QQ]            =   "com.tencent.qq",
};

static uint16_t ble_ancs_client_little_endian_read_16(const uint8_t * buffer, int pos)
{
    return ((uint16_t) buffer[pos]) | (((uint16_t)buffer[(pos)+1]) << 8);
}

static uint32_t ble_ancs_client_little_endian_read_32(const uint8_t * buffer, int pos)
{
    return ((uint32_t) buffer[pos]) | (((uint32_t)buffer[(pos)+1]) << 8) | (((uint32_t)buffer[(pos)+2]) << 16) | (((uint32_t) buffer[(pos)+3]) << 24);
}

static void ble_ancs_client_little_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = value;
    buffer[pos++] = value >> 8;
    buffer[pos++] = value >> 16;
    buffer[pos++] = value >> 24;
}

static void ble_ancs_client_print_utf8(uint8_t *data, uint16_t data_len)
{
    uint16_t index = 0;
    log_info("len:%d, data:",data_len);
    while (index < data_len) {
        log_info("%c", data[index]);
        index++;
    }
    log_info("\n");
}

static ANCS_CLIENT_APP_IDENTIFIER ble_ancs_client_match_app_identifier(uint8_t *buf, uint16_t buf_len)
{
    uint8_t app_identifier_index = 0;
    while (app_identifier_index < APP_IDENTIFIER_MAX) {
        if (memcmp(buf, ancs_app_identifier[app_identifier_index], buf_len) == 0) {
            return app_identifier_index;
        }
        app_identifier_index++;
    }
    return app_identifier_index;
}

static uint8_t ble_ancs_client_ctl_point_write(uint8_t *buf, uint16_t buf_len)
{
    uint8_t ret = ble_write_req_for_character(ancs_server_info.conn_handle, &ancs_server_info.characteristic[ancs_server_info.ctl_point_characteristic_index], buf, buf_len);
    return ret;
}

void ble_ancs_client_perform_ntf_act(uint32_t ntf_uuid, ANCS_CLIENT_ACTION_ID action_id)
{
    uint8_t offset = 0;
    ntf_src.rsp[offset++] = ANCS_CMD_ID_PERFORM_NOTIFICATION_ACTION; //cmd id
    ble_ancs_client_little_endian_store_32(ntf_src.rsp, offset, ntf_uuid); //ntf_uid
    offset += 4;
    ntf_src.rsp[offset++] = action_id;
    log_info("[ctl point ==>] cmd_id:0x%x, uuid:0x%x\n", ntf_src.rsp[0], ntf_uuid);
    ble_ancs_client_ctl_point_write(ntf_src.rsp, offset);
}

static void ble_ancs_client_get_ntf_att(uint32_t ntf_uuid)
{
    uint16_t offset = 0;
    uint16_t max_att_len;
    gatt_client_get_mtu(ancs_server_info.conn_handle, &max_att_len);
    max_att_len -= 3;
    ntf_src.rsp[offset++] = ANCS_CMD_ID_GET_NOTIFICATION_ATTR; //cmd id
    ble_ancs_client_little_endian_store_32(ntf_src.rsp, offset, ntf_uuid); //ntf_uid
    offset += 4;
    ntf_src.rsp[offset++] = NTF_ATT_ID_APP_IDENTIFIER;
    ntf_src.rsp[offset++] = NTF_ATT_ID_TITLE;
    ntf_src.rsp[offset++] = (max_att_len & 0xff);
    ntf_src.rsp[offset++] = (max_att_len & 0xff00)>>8;

//    ntf_src.rsp[offset++] = NTF_ATT_ID_SUBTITLE;
//    ntf_src.rsp[offset++] = (max_att_len & 0xff);
//    ntf_src.rsp[offset++] = (max_att_len & 0xff00)>>8;

    ntf_src.rsp[offset++] = NTF_ATT_ID_MSG;
    ntf_src.rsp[offset++] = (max_att_len & 0xff);
    ntf_src.rsp[offset++] = (max_att_len & 0xff00)>>8;

    ntf_src.rsp[offset++] = NTF_ATT_ID_MSG_SIZE;
    ntf_src.rsp[offset++] = NTF_ATT_ID_DATE;
    ntf_src.rsp[offset++] = NTF_ATT_ID_POSITIVE_ACT;
    ntf_src.rsp[offset++] = NTF_ATT_ID_NEGATIVE_ACT;
    log_info("[ctl point ==>] cmd_id:0x%x, uuid:0x%x\n", ntf_src.rsp[0], ntf_uuid);
    ble_ancs_client_ctl_point_write(ntf_src.rsp, offset);
}

static void ble_ancs_client_recv_ntf_src(uint8_t *packet, uint16_t packet_size)
{
    if(packet_size != BLE_ANCS_CLIENT_NTF_LEN) return;
    memcpy(&ntf_src.event_id, packet, packet_size-4);
    ntf_src.ntf_uuid = ble_ancs_client_little_endian_read_32(packet, 4);
    log_info("[ntf src <==] event_id:0x%x,event_flags:0x%x,category_id:0x%x,category_cnt:0x%x,ntf_uid:0x%x\n",ntf_src.event_id ,\
            ntf_src.event_flags,ntf_src.category_id,ntf_src.category_cnt,ntf_src.ntf_uuid);

    if (ntf_src.event_id == EVENT_ID_NOTIFICATION_REMOVED) {
        switch (ntf_src.category_id) {
            case CATGRY_ID_INCOMING_CALL:
                log_info("CATGRY_ID_INCOMING_CALL\n");
                ancs_server_info.sta_flag &= ~ANCS_FLAG_CALL_INCOMING;
                return;

            case CATGRY_ID_ACTIVE_CALL:
                log_info("CATGRY_ID_ACTIVE_CALL\n");
                ancs_server_info.sta_flag &= ~ANCS_FLAG_CALL_ACTIVE;
                return;

            default:
                return;
        }
    } else {
        switch (ntf_src.category_id) {
            case CATGRY_ID_SOCIAL:
                log_info("CATGRY_ID_SOCIAL\n");
                ancs_server_info.social_ntf_uuid = ntf_src.ntf_uuid;
                ble_ancs_client_get_ntf_att(ancs_server_info.social_ntf_uuid);
                break;

            case CATGRY_ID_INCOMING_CALL:
                log_info("CATGRY_ID_INCOMING_CALL\n");
                ancs_server_info.sta_flag |= ANCS_FLAG_CALL_INCOMING;
                ancs_server_info.call_ntf_uuid = ntf_src.ntf_uuid;
                ble_ancs_client_get_ntf_att(ancs_server_info.call_ntf_uuid);
                break;

            case CATGRY_ID_ACTIVE_CALL:
                log_info("CATGRY_ID_ACTIVE_CALL\n");
                ancs_server_info.sta_flag |= ANCS_FLAG_CALL_ACTIVE;
                ancs_server_info.call_active_uuid = ntf_src.ntf_uuid;
                break;

            default:
                break;
        }
    }
}

static void ble_ancs_client_recv_data_src(uint8_t *packet, uint16_t packet_size)
{
    uint16_t offset = 0;
    uint16_t att_len = 0;
    ANCS_CLIENT_APP_IDENTIFIER app_identifier;
    memcpy(data_src.buf, packet, packet_size);
    data_src.event_id = data_src.buf[offset++];
    data_src.buf_len = packet_size;
    data_src.data_uid = ble_ancs_client_little_endian_read_32(data_src.buf, 1);
    offset += 4;
    log_info("[data src <==] evt_id:%d,uid:%x,len:%d\n", data_src.event_id, data_src.data_uid, data_src.buf_len);
    while (offset < data_src.buf_len) {
        switch (data_src.buf[offset++]) {
            case NTF_ATT_ID_APP_IDENTIFIER:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                app_identifier = ble_ancs_client_match_app_identifier(data_src.buf+offset, att_len);
                log_info("NTF_ATT_ID_APP_IDENTIFIER, app_identifier:0x%x\n", app_identifier);
                offset += att_len;
                break;
            case NTF_ATT_ID_TITLE:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_TITLE,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_SUBTITLE:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_SUBTITLE,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_MSG:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_MSG,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_MSG_SIZE:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_MSG_SIZE,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_DATE:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_DATE,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_POSITIVE_ACT:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_POSITIVE_ACT,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            case NTF_ATT_ID_NEGATIVE_ACT:
                att_len = ble_ancs_client_little_endian_read_16(data_src.buf, offset);
                offset += 2;
                log_info("NTF_ATT_ID_NEGATIVE_ACT,");
                ble_ancs_client_print_utf8(data_src.buf+offset, att_len);
                offset += att_len;
                break;
            default:
                log_info("ERRR\r\n");
                break;
        }
    }
}

void ble_ancs_incoming_call_positive(void)
{
    if (ancs_server_info.sta_flag & ANCS_FLAG_CALL_INCOMING) {
        ble_ancs_client_perform_ntf_act(ancs_server_info.call_ntf_uuid, ANCS_ACT_ID_POSITIVE);
    }
}

void ble_ancs_incoming_call_negative(void)
{
    if (ancs_server_info.sta_flag & ANCS_FLAG_CALL_INCOMING) {
        ble_ancs_client_perform_ntf_act(ancs_server_info.call_ntf_uuid, ANCS_ACT_ID_NEGATIVE);
    }
}

void ble_ancs_call_active_negative(void)
{
    if (ancs_server_info.sta_flag & ANCS_FLAG_CALL_ACTIVE) {
        ble_ancs_client_perform_ntf_act(ancs_server_info.call_active_uuid, ANCS_ACT_ID_NEGATIVE);
    }
}

static void ble_ancs_client_event_packet_handle(uint8_t event_type, uint8_t *packet, uint16_t size)
{
    switch (event_type) {
        case BLE_EVT_CONNECT:
            log_info("BLE_EVT_CONNECT\n");
            memcpy(&ancs_server_info.conn_handle, &packet[7], 2);
            ancs_tc_state = STA_W4_ENCRYPTED_CONNECTION;
            ble_sm_send_security_request(ancs_server_info.conn_handle);
            return;

        case BLE_EVT_ENCRYPTION_CHANGE:
            log_info("BLE_EVT_ENCRYPTION_CHANGE\n");
            if (memcmp(&ancs_server_info.conn_handle, &packet[1], 2)) return;
            ancs_server_info.connection_encrypted = packet[3];
            if (!ancs_server_info.connection_encrypted) return;
            if (ancs_tc_state != STA_W4_ENCRYPTED_CONNECTION) return;

            log_info("ancs Client - connected, discover ANCS service\n");
            ancs_tc_state = STA_W4_SERVICE_RESULT;
            ancs_server_info.service_cnt = 0;
            ble_client_discover_primary_services_by_uuid128(ancs_server_info.conn_handle, ancs_service_uuid);
            return;

        case BLE_EVT_DISCONNECT:
            log_info("BLE_EVT_DISCONNECT\n");
            memset(&ancs_server_info, 0, sizeof(ancs_server_info));
            return;

        case BLE_EVT_CONNECT_PARAM_UPDATE_DONE:
            log_info("BLE_EVT_CONNECT_PARAM_UPDATE_DONE\n");
            return;
    }

    switch(ancs_tc_state){
        case STA_W4_SERVICE_RESULT:
            switch(event_type){
                case BLE_EVT_SERVICE_FIND_RESULT:
                    log_info("BLE_EVT_SERVICE_FIND_RESULT\n");
                    ble_service_query_result_get_service(packet, &ancs_server_info.service[ancs_server_info.service_cnt++]);
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    ancs_server_info.service_index = 0;
                    if (ancs_server_info.service_cnt != 0) {
                        log_info("ancs service discover complete:%d\n",ancs_server_info.service_cnt);
                        for (uint8_t cnt=0; cnt<ancs_server_info.service_cnt; cnt++) {
                            log_info("service[%d]->uuid16:%04x,start handle:%04x,stop handle:%04x\n",cnt,ancs_server_info.service[cnt].uuid16,\
                                ancs_server_info.service[cnt].start_group_handle,ancs_server_info.service[cnt].end_group_handle);
                        }
                        log_info("client discovery characteristic start\n");
                        ancs_tc_state = STA_W4_CHARACTERISTIC_RESULT;
                        ancs_server_info.characteristic_cnt = 0;
                        ble_client_discover_characteristics_for_service(ancs_server_info.conn_handle,&ancs_server_info.service[0]);
                    } else {
                        ancs_tc_state = STA_IDLE;
                        ancs_server_info.service_cnt = 0;
                        ancs_server_info.characteristic_cnt = 0;
                        log_info("client discovery service failed->no service\n");
                    }
                    break;

                default:
                    break;
            }
            break;

        case STA_W4_CHARACTERISTIC_RESULT:
            switch (event_type) {
                case BLE_EVT_CHARACTERISTIC_FIND_RESULT:
                    ble_characteristic_query_result_get_characteristic(packet,&ancs_server_info.characteristic[ancs_server_info.characteristic_cnt]);
                    if (memcmp(ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].uuid128, ancs_notification_source_uuid, 16) == 0) {
                        log_info("ancs notification source found, attribute handle %04x\n", ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].value_handle);
                        ancs_server_info.ntf_src_characteristic_index = ancs_server_info.characteristic_cnt;
                        ancs_server_info.characteristic_cnt++;
                        break;
                    } else if (memcmp(ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].uuid128, ancs_control_point_uuid, 16) == 0) {
                        log_info("ancs control point found, attribute handle %04x\n", ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].value_handle);
                        ancs_server_info.ctl_point_characteristic_index = ancs_server_info.characteristic_cnt;
                        ancs_server_info.characteristic_cnt++;
                        break;
                    } else if (memcmp(ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].uuid128, ancs_data_source_uuid, 16) == 0) {
                        log_info("ancs data source found, attribute handle %04x\n", ancs_server_info.characteristic[ancs_server_info.characteristic_cnt].value_handle);
                        ancs_server_info.data_src_characteristic_index = ancs_server_info.characteristic_cnt;
                        ancs_server_info.characteristic_cnt++;
                        break;
                    }
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    log_info("all characteristic discover complete:%d\n",ancs_server_info.characteristic_cnt);
                    ancs_tc_state = STA_W4_DATA_SOURCE_SUBSCRIBED;
                    ancs_server_info.conn_sta = ANCS_CLIENT_CONN;
                    ble_client_write_client_characteristic_configuration(ancs_server_info.conn_handle,&ancs_server_info.characteristic[ancs_server_info.data_src_characteristic_index], \
                        GATT_CCC_NOTIFICATION);

                    break;

                default:
                    break;
            }
            break;

        case STA_W4_DATA_SOURCE_SUBSCRIBED:
            switch (event_type) {
                case BLE_EVT_PROFILE_FIND_DONE:
                    log_info("STA_W4_DATA_SOURCE_SUBSCRIBED\n");
                    ancs_tc_state = STA_W4_NOTIFICATION_SOURCE_SUBSCRIBED;
                    ble_client_write_client_characteristic_configuration(ancs_server_info.conn_handle,&ancs_server_info.characteristic[ancs_server_info.ntf_src_characteristic_index], \
                        GATT_CCC_NOTIFICATION);
                    break;

                default:
                    break;
            }
            break;

        case STA_W4_NOTIFICATION_SOURCE_SUBSCRIBED:
            switch (event_type) {
                case BLE_EVT_PROFILE_FIND_DONE:
                    log_info("STA_W4_NOTIFICATION_SOURCE_SUBSCRIBED\n");
                    ancs_tc_state = STA_SUBSCRIBED;
                    break;

                default:
                    break;
            }
            break;

        case STA_SUBSCRIBED:
            break;

        default:
            break;
    }
}

static uint16_t ble_ancs_client_notify_callback (uint16_t con_handle, uint16_t attribute_handle, uint8_t *buffer, uint16_t buffer_size)
{
    log_info("ble_ancs_client_notify_callback, con_handle:0x%x, attribute_handle:0x%x\n", con_handle, attribute_handle);
    if (attribute_handle == ancs_server_info.characteristic[ancs_server_info.ntf_src_characteristic_index].value_handle) {
        ble_ancs_client_recv_ntf_src(buffer, buffer_size);
    } else if (attribute_handle == ancs_server_info.characteristic[ancs_server_info.data_src_characteristic_index].value_handle) {
        ble_ancs_client_recv_data_src(buffer, buffer_size);
    } else {
       log_info("ancs unknown source!\n");
    }
    return 0;
}


static uint16_t ble_ancs_att_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    log_info("ble_ancs_att_read_callback attribute_handle:%x\n",attribute_handle);
    return 0;
}

void ble_ancs_client_init(void)
{
    memset(&data_src, 0, sizeof(ancs_client_data_src_t));
    memset(&ntf_src, 0, sizeof(ancs_client_ntf_src_t));

    memset(&ancs_server_info, 0, sizeof(ancs_server_info));

    ancs_client_handler.event_handler = ble_ancs_client_event_packet_handle;
    ancs_client_handler.notify_callback = ble_ancs_client_notify_callback;
    ancs_client_handler.read_callback = ble_ancs_att_read_callback;
    att_client_register_handler(&ancs_client_handler);
}

