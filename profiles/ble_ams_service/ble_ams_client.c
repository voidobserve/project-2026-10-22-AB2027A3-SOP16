#include "include.h"
#include "ble_ams_client.h"

#define TRACE_EN                    1
#if TRACE_EN
#define TRACE(...)                  printf("[ASM] ");printf(__VA_ARGS__)
#define TRACE_R(...)                print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif


typedef enum {
    AMS_IDLE,
    AMS_W4_ENCRYPTED_CONNECTION,
    AMS_W4_SERVICE_RESULT,
    AMS_W4_CHARACTERISTIC_RESULT,
    AMS_W4_ENTITY_UPDATE_NOTIFICATION,
    AMS_W4_ENTITYID_PLAYER_SUBSCRIBED,
    AMS_SUBSCRIBED,
} ams_client_state_t;

static server_info_t                server_info AT(.ble_buf.data);
static gatt_client_service_t        *ams_primary_service AT(.ble_buf.data);
static gatt_client_characteristic_t *ams_remote_conmmand_characteristic AT(.ble_buf.data);
static gatt_client_characteristic_t *ams_entity_update_characteristic AT(.ble_buf.data);
static gatt_client_characteristic_t *ams_entity_attribute_characteristic AT(.ble_buf.data);

static att_client_handler_t         ams_client_handler AT(.ble_buf.data);
static ams_client_state_t           ams_tc_state AT(.ble_buf.data);

static const uint8_t entity_update_player[] = { AMS_ENTITY_ID_PLAYER,
                                                AMS_PLAYER_ATT_ID_NAME,
                                                AMS_PLAYER_ATT_ID_BACK_INFO,
                                                AMS_PLAYER_ATT_ID_VOL
                                        };

static const uint8_t entity_update_track[] = {  AMS_ENTITY_ID_TRACK,
                                                AMS_TRACK_ATT_ID_ARTIST,
                                                AMS_TRACK_ATT_ID_ALBUM,
                                                AMS_TRACK_ATT_ID_TITLE,
                                                AMS_TRACK_ATT_ID_DURATION
                                        };

// 89D3502B-0F36-433A-8EF4-C502AD55F8DC
static const uint8_t ams_service_uuid[] = {
    0x89, 0xD3, 0x50, 0x2B, 0x0F, 0x36, 0x43, 0x3A,
    0x8E, 0xF4, 0xC5, 0x02, 0xAD, 0x55, 0xF8, 0xDC
};

// 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2
static const uint8_t ams_remote_conmmand_uuid[] = {
    0x9B, 0x3C, 0x81, 0xD8, 0x57, 0xB1, 0x4A, 0x8A,
    0xB8, 0xDF, 0x0E, 0x56, 0xF7, 0xCA, 0x51, 0xC2,
};

// 2F7CABCE-808D-411F-9A0C-BB92BA96C102
static const uint8_t ams_entity_update_uuid[] = {
    0x2F, 0x7C, 0xAB, 0xCE, 0x80, 0x8D, 0x41, 0x1F,
    0x9A, 0x0C, 0xBB, 0x92, 0xBA, 0x96, 0xC1, 0x02,
};

// C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7
static const uint8_t ams_entity_attribute_uuid[] = {
    0xC6, 0xB2, 0xF3, 0x8C, 0x23, 0xAB, 0x46, 0xD8,
    0xA6, 0xAB, 0xA3, 0xA8, 0x70, 0xBB, 0xD5, 0xD7,
};

void ble_ams_client_var_init(void);

void ble_ams_remote_cmd_send(uint8_t cmd)
{
    if (ams_remote_conmmand_characteristic) {
        ble_write_req_for_character(server_info.conn_handle, ams_remote_conmmand_characteristic, &cmd, 1);
    }
}

static void ble_ams_player_update(uint8_t attribute_id, uint8_t entity_update_flags, uint8_t *value, uint16_t size)
{
    switch (attribute_id) {
        case AMS_PLAYER_ATT_ID_NAME:
            TRACE("-->AMS_PLAYER_ATT_ID_NAME: ");
            TRACE_R(value, size);
            break;

        case AMS_PLAYER_ATT_ID_BACK_INFO:
            TRACE("-->AMS_PLAYER_ATT_ID_BACK_INFO: ");
            switch (value[0]) {
                case AMS_PLAY_STA_PAUSE:
                    TRACE("-->AMS_PLAY_STA_PAUSE\n");
                    break;

                case AMS_PLAY_STA_PLAYING:
                    TRACE("-->AMS_PLAY_STA_PLAYING\n");
                    break;

                case AMS_PLAY_STA_REWINDING:
                    TRACE("-->AMS_PLAY_STA_REWINDING\n");
                    break;

                case AMS_PLAY_STA_FAST_FORWARD:
                    TRACE("-->AMS_PLAY_STA_FAST_FORWARD\n");
                    break;
                default:
                    break;
            }
            break;

        case AMS_PLAYER_ATT_ID_VOL:
            TRACE("-->AMS_PLAYER_ATT_ID_VOL: ");
            TRACE_R(value, size);
            break;

        default:
            break;
    }
}

static void ble_ams_queue_update(uint8_t attribute_id, uint8_t entity_update_flags, uint8_t *value, uint16_t size)
{
    switch (attribute_id) {

        case AMS_QUEUE_ATT_ID_INDEX:
            break;

        case AMS_QUEUE_ATT_ID_COUNT:
            break;

        case AMS_QUEUE_ATT_ID_SHUFFLE_MODE:
            break;

        case AMS_QUEUE_ATT_ID_REPEAT_MODE:
            break;

        default:
            break;
    }
}

static void ble_ams_track_update(uint8_t attribute_id, uint8_t entity_update_flags, uint8_t *value, uint16_t size)
{
    switch (attribute_id) {
        case AMS_TRACK_ATT_ID_ARTIST:
            TRACE("--->AMS_TRACK_ATT_ID_ARTIST\n");
            TRACE_R(value, size);
            break;

        case AMS_TRACK_ATT_ID_ALBUM:
            TRACE("--->AMS_TRACK_ATT_ID_ALBUM\n");
            TRACE_R(value, size);
            break;

        case AMS_TRACK_ATT_ID_TITLE:
            TRACE("--->AMS_TRACK_ATT_ID_TITLE\n");
            TRACE_R(value, size);
            break;

        case AMS_TRACK_ATT_ID_DURATION:
            TRACE("--->AMS_TRACK_ATT_ID_DURATION\n");
            TRACE_R(value, size);
            break;

        default:
            break;
    }
}

static void ble_ams_entity_update_callback(uint8_t event_id, uint8_t attribute_id, uint8_t entity_update_flags, uint8_t *value, uint16_t size)
{
    TRACE("Entity Update received: EventID %02x, AttributeID %02x, EntityUpdateFlags %02x, value:\n", event_id, attribute_id, entity_update_flags);

    TRACE_R(value, size);

    switch (event_id) {
    case AMS_ENTITY_ID_PLAYER:
        ble_ams_player_update(attribute_id, entity_update_flags, value, size);
        break;

    // Queue
    case AMS_ENTITY_ID_QUEUE:
        ble_ams_queue_update(attribute_id, entity_update_flags, value, size);
        break;

    // Track
    case AMS_ENTITY_ID_TRACK:
        ble_ams_track_update(attribute_id, entity_update_flags, value, size);
        break;

    default:
        break;
    }
}

static void ams_client_event_packet_handle(uint8_t event_type, uint8_t *packet, uint16_t size)
{
    switch(event_type){
        case BLE_EVT_DISCONNECT:
            ble_ams_client_var_init();
            return;

        case BLE_EVT_CONNECT:
            TRACE("BLE_EVT_CONNECT\n");
            memcpy(&server_info.conn_handle, &packet[7], 2);
            ams_tc_state = AMS_W4_ENCRYPTED_CONNECTION;
            ble_sm_send_security_request(server_info.conn_handle);
            return;

        case BLE_EVT_ENCRYPTION_CHANGE:
            TRACE("BLE_EVT_ENCRYPTION_CHANGE\n");
            if (memcmp(&server_info.conn_handle, &packet[1], 2)) return;
            if (!packet[3]) return;
            if (ams_tc_state != AMS_W4_ENCRYPTED_CONNECTION) return;

            TRACE("ancs Client - connected, discover ANCS service\n");
            ams_tc_state = AMS_W4_SERVICE_RESULT;
            server_info.service_cnt = 0;
            ble_client_discover_primary_services_by_uuid128(server_info.conn_handle, ams_service_uuid);
            return;
    }

    switch(ams_tc_state){
        case AMS_W4_SERVICE_RESULT:
            switch(event_type){
                case BLE_EVT_SERVICE_FIND_RESULT:
                    ble_service_query_result_get_service(packet, &server_info.service[server_info.service_cnt]);
                    if (0 == memcmp(server_info.service[server_info.service_cnt].uuid128, ams_service_uuid, 16)) {
                        ams_primary_service = &server_info.service[server_info.service_cnt];
                    }
                    server_info.service_cnt++;
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    if (ams_primary_service != NULL) {
                        TRACE("Discover characteristics for AMS SERVICE\n");
                        ams_tc_state = AMS_W4_CHARACTERISTIC_RESULT;
                        ble_client_discover_characteristics_for_service(server_info.conn_handle, ams_primary_service);
                    } else {
                        ams_tc_state = AMS_IDLE;
                        TRACE("Service not found\n");
                    }
                    break;

                default:
                    break;
            }
            break;

        case AMS_W4_CHARACTERISTIC_RESULT:
            switch (event_type) {
                case BLE_EVT_CHARACTERISTIC_FIND_RESULT:
                    ble_characteristic_query_result_get_characteristic(packet, &server_info.characteristic[server_info.characteristic_cnt]);
                    if (memcmp(server_info.characteristic[server_info.characteristic_cnt].uuid128, ams_remote_conmmand_uuid, 16) == 0) {
                        TRACE("Remote Connand found, attribute handle %u\n", server_info.characteristic[server_info.characteristic_cnt].value_handle);
                        ams_remote_conmmand_characteristic = &server_info.characteristic[server_info.characteristic_cnt];
                    } else if (memcmp(server_info.characteristic[server_info.characteristic_cnt].uuid128, ams_entity_update_uuid, 16) == 0) {
                        TRACE("Entity Update found, attribute handle %u\n", server_info.characteristic[server_info.characteristic_cnt].value_handle);
                        ams_entity_update_characteristic = &server_info.characteristic[server_info.characteristic_cnt];
                    } else if (memcmp(server_info.characteristic[server_info.characteristic_cnt].uuid128, ams_entity_attribute_uuid, 16) == 0) {
                        TRACE("Entity Attribute found, attribute handle %u\n", server_info.characteristic[server_info.characteristic_cnt].value_handle);
                        ams_entity_attribute_characteristic = &server_info.characteristic[server_info.characteristic_cnt];
                    }
                    server_info.characteristic_cnt++;
                    break;

                case BLE_EVT_PROFILE_FIND_DONE:
                    TRACE("Characteristcs count %u\n",server_info.characteristic_cnt);
                    ams_tc_state = AMS_W4_ENTITY_UPDATE_NOTIFICATION;
                    ble_client_write_client_characteristic_configuration(server_info.conn_handle,
                        ams_entity_update_characteristic, GATT_CCC_NOTIFICATION);
                    break;

                default:
                    break;
            }
            break;

        case AMS_W4_ENTITY_UPDATE_NOTIFICATION:
            switch (event_type) {
                case BLE_EVT_PROFILE_FIND_DONE:
                    ams_tc_state = AMS_W4_ENTITYID_PLAYER_SUBSCRIBED;
                    ble_write_req_for_character(server_info.conn_handle,
                        ams_entity_update_characteristic, (uint8_t*)entity_update_player, sizeof(entity_update_player));
                    break;

                default:
                    break;
            }
            break;

        case AMS_W4_ENTITYID_PLAYER_SUBSCRIBED:
            switch (event_type) {
                case BLE_EVT_PROFILE_FIND_DONE:
                    ams_tc_state = AMS_SUBSCRIBED;
                    ble_write_req_for_character(server_info.conn_handle,
                        ams_entity_update_characteristic, (uint8_t*)entity_update_track, sizeof(entity_update_track));
                    break;

                default:
                    break;
            }
            break;

        case AMS_SUBSCRIBED:
            break;

        default:
            break;
    }
}

static uint16_t ams_client_notify_callback (uint16_t con_handle, uint16_t attribute_handle, uint8_t * buffer, uint16_t buffer_size)
{
    if (attribute_handle == ams_remote_conmmand_characteristic->value_handle) {
    } else if (attribute_handle == ams_entity_update_characteristic->value_handle) {
        ble_ams_entity_update_callback(buffer[0], buffer[1], buffer[2], &buffer[3], buffer_size - 3);
    }

    return 0;
}

static uint16_t ams_att_read_callback(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
    TRACE("%s: ", __func__);
    TRACE_R(buffer, buffer_size);

    return 0;
}

void ble_ams_client_var_init(void)
{
    memset(&server_info, 0, sizeof(server_info));

    ams_primary_service = NULL;
    ams_remote_conmmand_characteristic = NULL;
    ams_entity_update_characteristic = NULL;
    ams_entity_attribute_characteristic = NULL;
    ams_tc_state = AMS_IDLE;

    ams_client_handler.event_handler = ams_client_event_packet_handle;
    ams_client_handler.notify_callback = ams_client_notify_callback;
    ams_client_handler.read_callback = ams_att_read_callback;
}

void ble_ams_client_init(void)
{
    TRACE("%s\n", __func__);

    ble_ams_client_var_init();

    att_client_register_handler(&ams_client_handler);
}
