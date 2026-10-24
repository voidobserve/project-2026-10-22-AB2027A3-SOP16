#ifndef _USB_API_H
#define _USB_API_H

#include "usb_define.h"

#define EP_CNT                  4

#define USB_EP0_MAX_SIZE        64
#define USB_EP1_MAX_SIZE        64
#define USB_EP2_MAX_SIZE        256
#define USB_EP3_MAX_SIZE        1023


enum {
    USBCHK_ONLY_HOST,
    USBCHK_ONLY_DEVICE,
    USBCHK_OTG,
};

enum {
    USB_DISCONNECT,
    USB_DEV_CONNECTED,
    USB_HOST_CONNECTED,
    USB_DETECTING,
};

enum {
    ///usb device message.
    MSG_USB_EP_RESET = 32,          //usb device ep0 reset message.
    MSG_USB_EP0_CTL_FLOW,           //usb device control flow message.
    MSG_USB_EP1_IN,
    MSG_USB_EP1_OUT,
    MSG_USB_EP2_IN,
    MSG_USB_EP2_OUT,
    MSG_USB_EP3_IN,
    MSG_USB_EP3_OUT,
    MSG_USB_SUSPEND,
    MSG_USB_RESUME,
};

typedef struct _epcb_t {
    psfr_t sfr;

    u8 type;                        //EP Type: Ctrl, Bulk, Int, ISOC
    u8 index;                       //EP Index
    volatile u8  error      : 1,    //USB通信出错
        dir                 : 1,    //EP的方向
        halt                : 1,    //Halt标志
        first_pkt           : 1;    //Transfer first packet

    u8 *buf;                        //EP的BUF地址

    const u8 *xptr;                 //当前发送/接收指针
    u16 xlen;                       //当前发送/接收剩余
    u16 xcnt;                       //已发送/接收统计
    u16 epsize;                     //EP Size
} epcb_t;

typedef struct _spb_wrap_t {
    union {
        struct {
            u8 recipient        : 5,
                type            : 2,
                dir             : 1;
        };
        u8 reqtype;
    };
    u8 req;
    u16 val;
    u16 index;
    u16 len;
} spb_wrap_t;

typedef struct _uhe_t {
    u8 *buf;                        //EP0 buf地址

    u8 devaddr;                     //当前设备地址
    u8 cfg_val;                     //当前配置
    u8 epsize;                      //EP0 的PKT SIZE

    u8 xlen;                        //TX RX DATA 剩余数据
    u8 *xptr;                       //TX RX DATA处理指针

    u8  set_faddr           : 1,    //Enum Set Faddr
        tx_first            : 1,    //TX 第一包
        rx_first            : 1,    //RX 第一包
        rx_end              : 1,    //RX到最后一包DATA
        stall               : 1;    //接收到stall标志

volatile u8 valid;                  //enum ok
volatile u8 error;                  //异常标志
volatile u8 suspend;                //suspend flag

    u32 timeout;
    spb_wrap_t spb;                 //Setup Packet Block
} uhe_t;


typedef bool (*usb_init_handler)(void);
typedef bool (*usb_evt_handler)(uint8_t event, spb_wrap_t *spb, uint8_t *data, uint8_t data_len);

typedef struct {
    usb_init_handler init;
    usb_evt_handler evt;
}usb_handler_cb_t;

void usb_handler_register(usb_handler_cb_t *handler);
void usb_sof_hook_register(void (*callback)(void));

u8 usbchk_connect(u8 mode);
u8 usbchk_host_connect(void);
u8 usbchk_host_connect_2(void);
u8 usbchk_device_connect(void);

void usb_init(void);
void usb_init_2(bool host_mode);
void usb_disable(void);
void usb_interrupt_init(void);

void usb_device_enter(void);
void usb_device_exit(void);
bool usb_device_is_suspend(void);
void usb_device_suspend_enable(void);
void usb_device_resume(void);

bool usb_host_enter(uhe_t *uhe);
void usb_host_exit(void);
void usb_host_reset(void);
void usb_host_suspend(void);
bool usb_host_resume(void);
void usb_host_ep0_setup_transfer(uhe_t *uhe);
void usb_host_ep0_out_transfer(uhe_t *uhe);
void usb_host_ep0_in_transfer(uhe_t *uhe);
void usb_device_addr_set(uint8_t addr);
void usb_host_ep_type_set(epcb_t *epcb);
void usb_host_session_start(void);
void usb_host_ep_in_transfer(epcb_t *epcb);

void usb_ep_init(epcb_t *epcb);
uint16_t usb_ep_get_max_len(uint8_t ep_index);
uint16_t usb_ep_get_rx_len(epcb_t *epcb);
void usb_ep_clear_rx_fifo(epcb_t *epcb);
bool usb_ep_transfer(epcb_t *epcb);
bool usb_ep_start_transfer(epcb_t *epcb, uint len);
bool usb_ep0_start_transfer(void *buf, uint len);
epcb_t * usb_ep_get_for_index(uint8_t index, uint8_t dir);
void usb_ep_reset(epcb_t *epcb);
void usb_ep_clear(epcb_t *epcb);
void usb_ep_halt(epcb_t *epcb);

#endif // _USB_API_H
