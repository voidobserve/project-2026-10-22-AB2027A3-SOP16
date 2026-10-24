#include "include.h"
#include "ble_service.h"
#include "ble_user_service.h"
#include "ble_fota_service.h"

///////////////////////////////////////////////////////////////////////////
const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
    // Manufacturer Specific Data
    // 0x03, 0xff, 0x42, 0x06,

    // 广播头：可以根据需要修改这部分
    0x09, 0xFF,
    0x5A, 0x44, 0x00, 0x03,
    0x00, 0x03, 0x01, 0x14, 
};

const uint8_t scan_data_const[] = {
};

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);
    
    //读取BLE配置的蓝牙名称
    int name_len;
    name_len = strlen(xcfg_cb.le_name);
    if ((data_len + name_len + 2) > 31) {
        printf("[err] scan response buffer insufficient!!!\n");
        return data_len;
    }
    scan_buf[data_len+1] = 0x09; //ad type local name
    if (name_len > 0) {
        memcpy(&scan_buf[data_len+2], xcfg_cb.le_name, name_len);
        scan_buf[data_len] = name_len + 1;  //ad length
        data_len = data_len + 2 + name_len;
    }

    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    //读取BLE配置的蓝牙名称
    int name_len;
    name_len = strlen(xcfg_cb.le_name);
    if ((data_len + name_len + 2) > 31) {
        printf("[err] adv buffer space insufficient!!!\n");
        return data_len;
    }
    adv_buf[data_len+1] = 0x09; //ad type local name
    if (name_len > 0) {
        memcpy(&adv_buf[data_len+2], xcfg_cb.le_name, name_len);
        adv_buf[data_len] = name_len + 1;  //ad length
        data_len = data_len + 2 + name_len;
    }

    return data_len;
}

const uint8_t profile_data[] =
{
#if LE_FOTA_EN
    // 0x0001 PRIMARY_SERVICE-FF12
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x12, 0xff,
    // 0x0002 CHARACTERISTIC-FF15-READ | WRITE_WITHOUT_RESPONSE | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x06, 0x03, 0x00, 0x15, 0xff,
    // 0x0003 VALUE-FF15-READ | WRITE_WITHOUT_RESPONSE | DYNAMIC-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x06, 0x01, 0x03, 0x00, 0x15, 0xff,
    // 0x0004 CHARACTERISTIC-FF14-NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x10, 0x05, 0x00, 0x14, 0xff,
    // 0x0005 VALUE-FF14-NOTIFY-''
    //
    0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x14, 0xff,
    // 0x0006 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0a, 0x01, 0x06, 0x00, 0x02, 0x29, 0x00, 0x00,
#endif
    // 0x0007 PRIMARY_SERVICE-FF00
    0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x00, 0xff,
    // 0x0008 CHARACTERISTIC-FF01-READ | WRITE | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x0a, 0x09, 0x00, 0x01, 0xff,
    // 0x0009 VALUE-FF01-READ | WRITE | DYNAMIC-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x09, 0x00, 0x01, 0xff,
    // 0x000a CHARACTERISTIC-FF02-NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x03, 0x28, 0x10, 0x0b, 0x00, 0x02, 0xff,
    // 0x000b VALUE-FF02-NOTIFY-''
    //
    0x08, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x02, 0xff,
    // 0x000c CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0a, 0x01, 0x0c, 0x00, 0x02, 0x29, 0x00, 0x00,

    // END
    0x00, 0x00,
};


//----------------------------------------------------------------------------
//
const u8 *ble_get_profile_data(void)
{
    return profile_data;
}


void ble_service_init(void)
{
    printf("ble_service_init\n");

    ble_user_service_init();
    
#if LE_FOTA_EN
    ble_fota_service_init();
#endif
}

