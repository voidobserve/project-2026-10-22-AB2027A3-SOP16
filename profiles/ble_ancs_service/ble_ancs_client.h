#ifndef __BLE_ANCS_CLIENT_H
#define __BLE_ANCS_CLIENT_H

#define BLE_ANCS_CLIENT_DATA_SRC_BUF_LEN 512 //MTU
#define BLE_ANCS_CLIENT_NTF_LEN          8
#define BLE_ANCS_CLIENT_NTF_RSP_LEN      19

#define ANCS_MAX_SERVICE_NUM              1
#define ANCS_MAX_CHARACTERISTIC_NUM       3 //ctl point、ntf、data

#define ANCS_FLAG_CALL_INCOMING     BIT(0)
#define ANCS_FLAG_CALL_ACTIVE       BIT(1)

#define EVT_FLAG_SILENT         BIT(0)
#define EVT_FLAG_IMPORTANT      BIT(1)
#define EVT_FLAG_PRE_EXSITING   BIT(2)
#define EVT_FLAG_POSITIVE       BIT(3)
#define EVT_FLAG_NEGATIVE       BIT(4)

typedef enum {
    EVENT_ID_NOTIFICATION_ADD = 0,  /**< The arrival of a new iOS notification on the NP */
    EVENT_ID_NOTIFICATION_MODIFIED, /**< The modification of an iOS notification on the NP */
    EVENT_ID_NOTIFICATION_REMOVED,  /**< The removal of an iOS notification on the NP */
    EVENT_ID_NOTIFICATION_RESERVED = 0xff,
} ANCS_CLIENT_EVENT_ID;

typedef enum {
    CATGRY_ID_OTHER = 0,
    CATGRY_ID_INCOMING_CALL,
    CATGRY_ID_MISS_CALL,
    CATGRY_ID_VOICE_MAIL,
    CATGRY_ID_SOCIAL,
    CATGRY_ID_SCHEDULE,
    CATGRY_ID_EMAIL,
    CATGRY_ID_NEWS,
    CATGRY_ID_HEALTH,
    CATGRY_ID_BUSINESS,
    CATGRY_ID_LOCATION,
    CATGRY_ID_ENTERTAINMENT,
    CATGRY_ID_ACTIVE_CALL,
}ANCS_CLIENT_GATGRY_ID;

typedef enum {
    NTF_ATT_ID_APP_IDENTIFIER = 0,
    NTF_ATT_ID_TITLE,
    NTF_ATT_ID_SUBTITLE,
    NTF_ATT_ID_MSG,
    NTF_ATT_ID_MSG_SIZE,
    NTF_ATT_ID_DATE,
    NTF_ATT_ID_POSITIVE_ACT,
    NTF_ATT_ID_NEGATIVE_ACT,
}ANCS_CLIENT_NTF_ATT_ID;

typedef enum {
    ANCS_CMD_ID_GET_NOTIFICATION_ATTR = 0,
    ANCS_CMD_ID_GET_APP_ATTR = 1,
    ANCS_CMD_ID_PERFORM_NOTIFICATION_ACTION = 2,
    ANCS_CMD_ID_RESERVED = 0xff,
} ANCS_CLIENT_CTL_POINT_CMD_ID;

typedef enum {
    ANCS_ACT_ID_POSITIVE = 0,
    ANCS_ACT_ID_NEGATIVE = 1,
    ANCS_ACT_ID_RESERVED  = 0xff,
} ANCS_CLIENT_ACTION_ID;

typedef enum {
    ANCS_CLIENT_DISCONN = 0,
    ANCS_CLIENT_CONN,
} ANCS_CLIENT_CONN_STA;

typedef enum {
    WEIXIN = 0,
    MOBILE_SMS,
    MOBILE_PHONE,
    MQQ,
    QQ,
    APP_IDENTIFIER_MAX,
} ANCS_CLIENT_APP_IDENTIFIER;

typedef struct {
    uint8_t buf[BLE_ANCS_CLIENT_DATA_SRC_BUF_LEN];
    uint8_t event_id;
    uint16_t buf_len;
    uint32_t data_uid;
} ancs_client_data_src_t;

typedef struct {
    uint8_t rsp[BLE_ANCS_CLIENT_NTF_RSP_LEN];
    uint8_t event_id;
    uint8_t event_flags;
    uint8_t category_id;
    uint8_t category_cnt;
    uint32_t ntf_uuid;
} ancs_client_ntf_src_t;

typedef enum {
    STA_IDLE,
    STA_W4_ENCRYPTED_CONNECTION,
    STA_W4_SERVICE_RESULT,
    STA_W4_CHARACTERISTIC_RESULT,
    STA_W4_DATA_SOURCE_SUBSCRIBED,
    STA_W4_NOTIFICATION_SOURCE_SUBSCRIBED,
    STA_SUBSCRIBED,
    STA_W4_DISCONNECT
} ancs_client_state_t;

typedef struct {
    gatt_client_service_t  service[ANCS_MAX_SERVICE_NUM];
    gatt_client_characteristic_t characteristic[ANCS_MAX_CHARACTERISTIC_NUM];
    uint8_t conn_sta;
    uint8_t service_cnt;
    uint8_t characteristic_cnt;
    uint8_t service_index;
    uint8_t characteristic_index;
    uint8_t ctl_point_characteristic_index;
    uint8_t ntf_src_characteristic_index;
    uint8_t data_src_characteristic_index;
    uint16_t conn_handle;
    uint16_t connection_encrypted;
    uint32_t call_ntf_uuid;
    uint32_t social_ntf_uuid;
    uint32_t call_active_uuid;
    uint8_t sta_flag;
} ancs_server_info_t;

void ble_ancs_client_init(void);

void ble_ancs_incoming_call_positive(void);
void ble_ancs_incoming_call_negative(void);
void ble_ancs_call_active_negative(void);

#endif
