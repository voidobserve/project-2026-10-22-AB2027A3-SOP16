#ifndef _BSP_IO_KEY_H
#define _BSP_IO_KEY_H


#include "driver_gpio.h"

/* WK0 --> KEY0(PP)  by default, no matter which way, see initialization in file 'bsp_io_key.c'. */
#define IO_KEY_SCAN_MODE            0       // 0: Judge the level and determine the key status.
                                            // 1: Matrix row/column scanning.

#define IO_KEY_COL_TABLE_SIZE       0
#define IO_KEY_ROW_TABLE_SIZE       6

#define IO_KEY_WK0_EN               1


typedef struct {
    gpio_typedef *gpiox;
    uint16_t gpio_pin;
} key_io_typedef;


extern key_io_typedef key_io_table_column[IO_KEY_COL_TABLE_SIZE];
extern key_io_typedef key_io_table_row[IO_KEY_ROW_TABLE_SIZE];


void bsp_io_key_init(void);
u8 bsp_get_io_key_id(void);


#endif // _BSP_IO_KEY_H
