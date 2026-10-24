#ifndef _PROD_TEST_IODM_H
#define _PROD_TEST_IODM_H

#define IODM_DATA_LEN                 40
#define IODM_PACKET_HEADER_LEN        5

#define FLAG_IODM_RECEIVE_EVENT      BIT(0)

#define IODM_HEADER       0xAA55
#define IODM_DISTINGUISH  0xFF

typedef enum {
    IODM_CMD_DEV_RST = 0,               //复位系统
    IODM_CMD_SET_LE_ADDR,               //设置蓝牙地址
    IODM_CMD_GET_LE_ADDR,               //获取蓝牙地址
    IODM_CMD_SET_LE_NAME,               //设置蓝牙名字
    IODM_CMD_GET_LE_NAME,               //获取蓝牙名字
    IODM_CMD_LE_FCC_TEST_ENTER,         //进入LE FCC 测试
    IODM_CMD_LE_FCC_SET_PARAM,          //设置LE FCC 参数
    IODM_CMD_LE_FCC_TEST_EXIT,          //退出LE FCC 模式
    IODM_CMD_LE_BQB_RF_ENTER,           //进入LE BQB RF测试模式
    IODM_CMD_LE_BQB_RF_TX_V2_PRAM,      //设置tx mode参数并进入tx mode（v2）
    IODM_CMD_LE_BQB_RF_RX_V2_PRAM,      //设置rx mode参数并进入rx mode（v2）
    IODM_CMD_LE_BQB_RF_STOP,            //暂停当前tx或rx模式，rx模式返回接收到的packet数量
    IODM_CMD_LE_BQB_RF_EXIT,            //退出LE BQB RF测试模式
    IODM_CMD_SET_XOSC_CAP,              //设置频偏参数
    IODM_CMD_GET_XOSC_CAP,              //获取频偏参数
    IODM_CMD_GET_VER_INFO,              //获取固件版本号
    IODM_CMD_GET_INFO,                  //获取耳机的基本信息
    IODM_CMD_MIC_LOOKBACK_ENTER,        //进入mic直通
    IODM_CMD_MIC_LOOKBACK_EXIT,         //退出mic直通
    IODM_CMD_PROTOCOL_VER,              //获取协议版本号
    IODM_CMD_MAX,
} IODM_CMD_TYPE;

typedef enum {
    IODM_RES_SUCCESS  = 0,   
    IODM_RES_FAIL     = 1,   
    IODM_RES_COMPLETE = 2,   
} IODM_RES_TYPE;

typedef struct {
    uint16_t header;
    uint8_t distinguish;                  
    uint8_t cmd;
    uint8_t length;
    uint8_t data[IODM_DATA_LEN];
    uint8_t checksum;
} iodm_packet_t;

typedef struct {
    uint8_t do_flag;
} iodm_var_t;

void prod_test_iodm_set_do_flag(uint8_t flag);
void prod_test_iodm_packet_receive(uint8_t *buf, uint8_t buf_len);
void prod_test_iodm_init(void);
void prod_test_iodm_process(void);

#endif // _PROD_TEST_IODM_H
