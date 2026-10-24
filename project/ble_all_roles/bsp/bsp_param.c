#include "include.h"

#define param_read8(a, b)       b = cm_read8(PAGE0(a))
#define param_write8(a, b)      cm_write8(PAGE0(a), b)
#define param_read16(a, b)      b = cm_read16(PAGE0(a))
#define param_write16(a, b)     cm_write16(PAGE0(a), b)
#define param_read32(a, b)      b = cm_read32(PAGE0(a))
#define param_write32(a, b)     cm_write32(PAGE0(a), b)
#define param_read(a, b, c)     cm_read(a, PAGE0(b), c)
#define param_write(a, b, c)    cm_write(a, PAGE0(b), c)
#define param_sync_do()         cm_sync()


AT(.text.bsp.param)
void bsp_param_init(void)
{
    cm_init(MAX_CM_PAGE, CM_START, CM_SIZE);
    //printf("CM: %x\n", cm_read8(PAGE0(0)));
    //printf("CM: %x\n", cm_read8(PAGE1(0)));
}

AT(.text.bsp.param)
void bsp_param_write(u8 *buf, u32 addr, uint len)
{
    param_write(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_read(u8 *buf, u32 addr, uint len)
{
    param_read(buf, addr, len);
}

AT(.text.bsp.param)
void bsp_param_sync(void)
{
    param_sync_do();
}

AT(.text.bsp.param.fota)
void param_fot_addr_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_ADDR, PARAM_FOT_ADDR_LEN);
    param_sync_do();
}

AT(.text.bsp.param.fota)
void param_fot_addr_read(u8 *param)
{
    param_read(param, PARAM_FOT_ADDR, PARAM_FOT_ADDR_LEN);
}

AT(.text.bsp.param.fota)
void param_fot_remote_ver_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_REMOTE_VER, PARAM_FOT_REMOTE_VER_LEN);
    param_sync_do();
}

AT(.text.bsp.param.fota)
void param_fot_remote_ver_read(u8 *param)
{
    param_read(param, PARAM_FOT_REMOTE_VER, PARAM_FOT_REMOTE_VER_LEN);
}

AT(.text.bsp.param.fota)
void param_fot_head_info_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HEAD_INFO, PARAM_FOT_HEAD_INFO_LEN);
    param_sync_do();
}

AT(.text.bsp.param.fota)
void param_fot_head_info_read(u8 *param)
{
    param_read(param, PARAM_FOT_HEAD_INFO, PARAM_FOT_HEAD_INFO_LEN);
}

AT(.text.bsp.param.fota)
void param_fot_hash_write(u8 *param)
{
    param_write((u8 *)param, PARAM_FOT_HASH, PARAM_FOT_HASH_LEN);
}

AT(.text.bsp.param.fota)
void param_fot_hash_read(u8 *param)
{
    param_read(param, PARAM_FOT_HASH, PARAM_FOT_HASH_LEN);
}
