#ifndef _USB_DEFINE_H
#define _USB_DEFINE_H

//EP Type
#define EP_TYPE_CTRL                        0
#define EP_TYPE_ISOC                        1
#define EP_TYPE_BULK                        2
#define EP_TYPE_INTR                        3

//EP Dir
#define EP_DIR_IN                           1           //device to host
#define EP_DIR_OUT                          0           //host to device

//Recipient
#define ST_REQ_DEVICE                   0
#define ST_REQ_INTERFACE                1
#define ST_REQ_ENDPOINT                 2

//Type
#define ST_TYPE_STANDARD                0
#define ST_TYPE_CLASS                   1
#define ST_TYPE_VENDOR                  2

//standard request
#define UR_GET_STATUS                   0x00
#define UR_CLEAR_FEATURE                0x01
#define UR_SET_FEATURE                  0x03
#define UR_SET_ADDRESS                  0x05
#define UR_GET_DESCRIPTOR               0x06
#define UR_GET_CONFIGURATION            0x08
#define UR_SET_CONFIGURATION            0x09
#define UR_GET_INTERFACE                0x0a
#define UR_SET_INTERFACE                0x0b

//get status
#define DS_SELF_PWR                     (1 << 6)
#define DS_REM_WAKEUP                   (1 << 5)

//hid request
#define HR_SET_PROTOCOL                 0x0b
#define HR_GET_PROTOCOL                 0x03
#define HR_SET_IDLE                     0x0a
#define HR_GET_IDLE                     0x02
#define HR_SET_REPORT                   0x09
#define HR_GET_REPORT                   0x01

//Feature
#define FT_EP_STALL                     0
#define FT_DEV_REM_WAKEUP               1


//Device Descriptor
#define DEVICE_DESCRIPTOR                   1           //设备描述符
#define CONFIGURATION_DESCRIPTOR            2           //配置描述符
#define STRING_DESCRIPTOR                   3           //字符串描述符
#define INTERFACE_DESCRIPTOR                4           //接口描述符
#define ENDPOINT_DESCRIPTOR                 5           //端点描述符
#define DEVICE_QUALIFIER_DESCRIPTOR         6

#define HID_DESCRIPTOR                      0x21        //HID描述符
#define HID_REPORT_DESCRIPTOR               0x22        //HID报表描述符
#define AUDIO_INTERFACE_DESCRIPTOR          0x24        //音频类接口描述符
#define AUDIO_ENDPOINT_DESCRIPTOR           0x25        //音频类传输端点描述符

//USB CDC
#define CS_INTERFACE                        0x24
#define HEADER_FUNC_DESCRIPTOR              0x00
#define CALL_MANAGE_DESCRIPTOR              0x01
#define ABSTRACT_CTL_DESCRIPTOR             0x02
#define UNION_FUNC_DESCRIPTOR               0x06

#define CDC_SET_LINE_CODING                 0x20
#define CDC_GET_LINE_CODING                 0x21
#define CDC_SET_LINE_STATE                  0x22

//USB Audio Class-Specific

#define UA_HEADER_DESCRIPTOR                0x01        //音频类头接口描述符
#define UA_INPUT_TERM_DESCRIPTOR            0x02        //音频类输入终端描述符
#define UA_OUTPUT_TERM_DESCRIPTOR           0x03        //音频类输出终端描述符
#define UA_FEATURE_UNIT_DESCRIPTOR          0x06        //音频类特征单元描述符

#define UA_STREAM_GENERAL_DESCRIPTOR        0x01        //音频流接口描述符
#define UA_STREAM_FORMAT_DESCRIPTOR         0x02        //音频流数据格式描述符

#define UA_SET_CUR                          0x01
#define UA_SET_MIN                          0x02
#define UA_SET_MAX                          0x03
#define UA_SET_RES                          0x04
#define UA_GET_CUR                          0x81        //Current setting attribute
#define UA_GET_MIN                          0x82        //Minimum setting attribute
#define UA_GET_MAX                          0x83        //Maximum setting attribute
#define UA_GET_RES                          0x84        //Resolution attribute

//control for feature unit
#define UA_MUTE_CTL                         0x01
#define UA_VOL_CTL                          0x02

#endif
