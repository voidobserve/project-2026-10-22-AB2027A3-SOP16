#ifndef _API_CM_H
#define _API_CM_H

#define PAGE_DATA_SIZE      250

//每个Page 250 Byte
#define PAGE0(x)            (x)
#define PAGE1(x)            (0x100 + (x))
#define PAGE2(x)            (0x200 + (x))
#define PAGE3(x)            (0x300 + (x))
#define PAGE4(x)            (0x400 + (x))
#define PAGE5(x)            (0x500 + (x))
#define PAGE6(x)            (0x600 + (x))
#define PAGE7(x)            (0x700 + (x))

#define MAX_CM_PAGE         3       //根据需要定义大小,最大15个

#define SYS_CM_PAGE(x)      PAGE0(x)
#define BLE_CM_PAGE(x)      PAGE1(x)
#define BLE_CM_1_PAGE(x)    PAGE2(x)

/**
  * @brief  参数区初始化，用于保存一些掉电需要记忆的参数到内置的flash里面
  * @param  page总数
  * @param  参数区的起始地址，参数区一般设置在flash的最后那块区域
  * @param  参数区总大小,至少8k
  */
void cm_init(uint max_page, u32 addr, uint len);    //

/**
  * @brief  将write写入cache的内容同步到spiflash中
  */
void cm_sync(void);
void cm_read(void *buf, u32 addr, uint len);
u8 cm_read8(u32 addr);
u16 cm_read16(u32 addr);
u32 cm_read32(u32 addr);

/**
  * @brief write数据到cache，需要调用cm_sync才会真正写入到spiflash中
  */
void cm_write(void *buf, u32 addr, uint len);

void cm_write8(u32 addr, u8 val);
void cm_write16(u32 addr, u16 val);
void cm_write32(u32 addr, u32 val);

/**
  * @brief 清除CM的一整个Page
  * @param 要清除的page起始地址
  */
void cm_clear(u32 addr);

/**
  * @brief 配置区(setting)初始化
  */
bool xcfg_init(void *xcfg, uint len);

#endif
