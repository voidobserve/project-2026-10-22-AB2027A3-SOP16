#ifndef __BLE_AMS_CLIENT_H
#define __BLE_AMS_CLIENT_H

#define MAX_SERVICE_NUM              1
#define MAX_CHARACTERISTIC_NUM       3

enum {
    AMS_ENTITY_ID_PLAYER = 0,       //播放器
    AMS_ENTITY_ID_QUEUE,            //队列
    AMS_ENTITY_ID_TRACK,            //音轨
};

enum {
    AMS_PLAYER_ATT_ID_NAME = 0,     //播放器名称
    AMS_PLAYER_ATT_ID_BACK_INFO,    //播放信息，包含播放状态，播放速率，已用时间
    AMS_PLAYER_ATT_ID_VOL,          //音量
};

enum {
    AMS_QUEUE_ATT_ID_INDEX = 0,     //队列索引
    AMS_QUEUE_ATT_ID_COUNT,         //队列中项目总数
    AMS_QUEUE_ATT_ID_SHUFFLE_MODE,  //随机播放模式
    AMS_QUEUE_ATT_ID_REPEAT_MODE,   //重复模式
};

enum {
    AMS_TRACK_ATT_ID_ARTIST = 0,    //艺术家姓名
    AMS_TRACK_ATT_ID_ALBUM,         //唱片集名称
    AMS_TRACK_ATT_ID_TITLE,         //曲目标题
    AMS_TRACK_ATT_ID_DURATION,      //轨道总持续时间
};

enum {
    AMS_REM_CMD_ID_PLAY = 0,
    AMS_REM_CMD_ID_PAUSE,
    AMS_REM_CMD_ID_PLAY_PAUSE,
    AMS_REM_CMD_ID_NEXT,
    AMS_REM_CMD_ID_PREV,
    AMS_REM_CMD_ID_VOL_UP,
    AMS_REM_CMD_ID_VOL_DOWN,
    AMS_REM_CMD_ID_REPEAT,
    AMS_REM_CMD_ID_SHUFFLE,
    AMS_REM_CMD_ID_PORWARD,
    AMS_REM_CMD_ID_BACKWARD,
    AMS_REM_CMD_ID_LIKE_TRACK,
    AMS_REM_CMD_ID_SLIKE_TRACK,
    AMS_REM_CMD_ID_BOOKMARK_TRACK,
};

enum {
    AMS_PLAY_STA_PAUSE = '0',
    AMS_PLAY_STA_PLAYING,
    AMS_PLAY_STA_REWINDING,
    AMS_PLAY_STA_FAST_FORWARD,
};


enum {
    AMS_SHUFFLE_OFF = 0,
    AMS_SHUFFLE_ONE,
    AMS_SHUFFLE_ALL,
};

enum {
    AMS_REPEAT_OFF = 0,
    AMS_REPEAT_ONE,
    AMS_REPEAT_ALL,
};


typedef struct {
    gatt_client_service_t service[MAX_SERVICE_NUM];
    gatt_client_characteristic_t characteristic[MAX_CHARACTERISTIC_NUM];
    uint8_t service_cnt;
    uint8_t characteristic_cnt;
    uint8_t service_index;
    uint8_t characteristic_index;
    uint16_t conn_handle;
} server_info_t;


void ble_ams_client_init(void);
void ble_ams_remote_cmd_send(uint8_t cmd);


#endif // __BLE_AMS_CLIENT_H
