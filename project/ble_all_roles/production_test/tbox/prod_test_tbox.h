#ifndef __PROD_TEST_TBOX_H
#define __PROD_TEST_TBOX_H
#include "include.h"

//***************************************************************************************************************//
//***************************************************************************************************************//
//                                                                                                               //
//                                               测试盒产测说明                                                   //
// 1.根据需求开启测试项，按键产测：PROD_TEST_KEY_EN， MIC产测：PROD_TEST_MIC_EN。                                    //
// 2.测试盒产测模式使用另一套按键消息枚举、命令和语音协议，需要在tbox_key.h添加产测专用的按键消息枚举和命令，同时也要在    //
//   msg_tbox.c中添加产测按键消息相关的case，最后在测试盒key_pair.csv文件中添加相对应的按键命令和按键名。               //
// 3.统一规定使用单击测试按键，长按语音键测试语音，松开语音键停止语音。                                                //
// 4.测试语音需要在测试盒接上3.5mm的耳机，语音实时播放。                                                             //
//                                                                                                               //
//***************************************************************************************************************//
//***************************************************************************************************************//

#if FUNC_TBOX_EN

#define BLE_TBOX_CMD_BUF_LEN                4
#define BLE_TBOX_CMD_BUF_MASK               (BLE_TBOX_CMD_BUF_LEN - 1)
#define BLE_TBOX_RX_BUF_LEN                 20

//
// list service handle ranges
//
#define ATT_SERVICE_FF80_01_START_HANDLE 0x0001
#define ATT_SERVICE_FF80_01_END_HANDLE   0x0009

//
// list mapping between characteristics and handles
//
#define ATT_CHARACTERISTIC_8000_01_VALUE_HANDLE 0x0003					//GENERAL READ、WRITE WITHOUT RESPONSE
#define ATT_CHARACTERISTIC_8001_01_VALUE_HANDLE 0x0005					//KEY NOTIFY
#define ATT_CHARACTERISTIC_8001_01_CLIENT_CONFIGURATION_HANDLE 0x0006
#define ATT_CHARACTERISTIC_8002_01_VALUE_HANDLE 0x0008					//VOICE NOTIFY
#define ATT_CHARACTERISTIC_8002_01_CLIENT_CONFIGURATION_HANDLE 0x0009

//RX
typedef struct{
    uint8_t len;
    uint8_t buf[BLE_TBOX_RX_BUF_LEN];
    uint16_t handle;
}tbox_rx_cmd_t;

typedef struct{
    tbox_rx_cmd_t cmd[BLE_TBOX_CMD_BUF_LEN];
    uint8_t cmd_rptr;
    uint8_t cmd_wptr;
}tbox_rx_cb_t;

//TX
typedef struct{
}tbox_tx_t;

typedef struct{
    bool state;
    uint8_t key_msg;
    uint16_t conn_handle;
    uint16_t key_client_config;
    uint16_t voice_client_config;
}tbox_var_t;

extern tbox_var_t tbox_var;

void prod_test_tbox_init(void);
void prod_test_tbox_proc(void);
bool is_prod_test_tbox_mode();
void tbox_set_key_msg(uint8_t msg);
uint8_t tbox_get_key_msg(void);

#endif  //FUNC_TBOX_EN
#endif  //__PROD_TEST_TBOX_H
