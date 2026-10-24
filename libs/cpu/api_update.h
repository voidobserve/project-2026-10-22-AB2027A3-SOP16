#ifndef _API_UPDATE_H
#define _API_UPDATE_H

typedef enum {
    FOT_ERR_OK,
    FOT_ERR_NO_UPDATA,
    FOT_ERR_KEY,
    FOT_ERR_FILE_FORMAT,
    FOT_ERR_FILE_TAG,
    FOT_ERR_FILE_DATA,
    FOT_ERR_MAC_SIZE,
    FOT_ERR_START,
    FOT_ERR_DOWN_START,
    FOT_ERR_HEAD2_START,
    FOT_ERR_DOWN_LAST,
    FOT_ERR_CRC_VERIFY,
    FOT_ERR_AREA_SIZE,
    FOT_ERR_USER_CHECK_FAILED,
    /*以上err类型和库里面对应,通过fot_get_err()获取到*/

    FOT_ERR_SEQ = 0x40,
    FOT_ERR_DATA_LEN,

    FOT_UPDATE_PAUSE = 0xfd,
    FOT_UPDATE_CONTINUE = 0xfe,
    FOT_UPDATE_DONE = 0xff,
} FOT_ERR_ENUM;

typedef struct __attribute__((packed)){
	u32 flag;				//固定，取值0x4E494255，用于标识该文件
	u16 head_size;			//userbin的头部信息大小，即该结构体的大小，可根据该大小算出userbin有效数据的偏移，目前头部信息大小为32字节
	u32 version;			//userbin当前版本号，可由user.xm获取到
	u32 down_area_start;	//userbin需要写到的flash地址，可由user.xm获取到
	u32 down_area_size;		//开辟给userbin存储的flash大小，可由user.xm获取到
    u32 data_size;			//userbin的有效数据大小，即userbin文件大小减去头部信息大小（head_size）
	u32 data_crc;			//userbin有效数据的CRC，每512字节算一个CRC32，算出来的CRC32作为下一个512字节参与CRC运算的seed
	u32 res;				//保留,默认0
    u16 head_crc;			//头信息对应的CRC16，即除了该字段，其他字段算出来的CRC16
}fot_ubin_head_t;

//FOTA压缩升级
void ota_pack_init(void);
void ota_pack_deinit(void);
bool ota_pack_write(u8 *buf);
u8 ota_pack_get_err(void);
bool ota_pack_is_update_success(void);
bool ota_pack_breakpoint_info_read(void);
u32 ota_pack_get_curaddr(void);

//userbin单独升级
bool fot_ubin_is_valid(uint32_t addr);
uint32_t fot_ubin_get_version(uint32_t addr);
void fot_ubin_init(void);
bool fot_ubin_write(void *buf, u32 addr, u32 len);
u32 fot_ubin_get_curaddr(void);
u8 fot_ubin_get_err(void);
bool fot_ubin_breakpoint_read(uint32_t ubin_start_addr);
bool fot_ubin_is_update_success(void);

#endif
