#include "include.h"
#include "bsp_io_key.h"

#if BSP_IOKEY_EN

#define GPIO_SET_DIR_OUT(x, y)                  {x->dir&=~y;x->drv&=~y;delay_us(30);}
#define GPIO_SET_DIR_IN(x, y)                   {x->dir|=y;x->pupd[0]&=~y;x->pupd[1]|=y;delay_us(30);}

key_io_typedef key_io_table_column[IO_KEY_COL_TABLE_SIZE] = {
    // {GPIOA_REG, GPIO_PIN_6},
    // {GPIOA_REG, GPIO_PIN_7},
};

#if IO_KEY_SCAN_MODE
key_io_typedef key_io_table_row[IO_KEY_ROW_TABLE_SIZE] = {
    {GPIOA_REG, GPIO_PIN_0},
    {GPIOA_REG, GPIO_PIN_1},
    {GPIOA_REG, GPIO_PIN_2},
    {GPIOA_REG, GPIO_PIN_3},
    {GPIOA_REG, GPIO_PIN_4},
    {GPIOA_REG, GPIO_PIN_5},
};
#endif // IO_KEY_SCAN_MODE

void bsp_io_key_init(void)
{
    gpio_init_typedef gpio_init_structure;

    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_DIGITAL;
#if IO_KEY_SCAN_MODE
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PD10K;  //软件矩阵扫描默认下拉检测高电平
#else
    gpio_init_structure.gpio_pupd = GPIO_PUPD_PU10K;  //普通IO KEY默认上拉检测低电平
#endif

#if IO_KEY_WK0_EN
    /* KEY0(WK0/PP) Init */
    wko_io_config(ENABLE, DISABLE, ENABLE);
#endif

    /* KEYx Init */
    for (u8 i = 0; i < IO_KEY_COL_TABLE_SIZE; i++) {
        gpio_init_structure.gpio_pin = key_io_table_column[i].gpio_pin;
        gpio_init(key_io_table_column[i].gpiox, &gpio_init_structure);
    }

#if IO_KEY_SCAN_MODE
    gpio_init_structure.gpio_drv = GPIO_DRV_6MA;
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    for (u8 i = 0; i < IO_KEY_ROW_TABLE_SIZE; i++) {
        gpio_init_structure.gpio_pin = key_io_table_row[i].gpio_pin;
        gpio_init(key_io_table_row[i].gpiox, &gpio_init_structure);
        gpio_set_bits(key_io_table_row[i].gpiox, key_io_table_row[i].gpio_pin);
    }
#endif // IO_KEY_SCAN_MODE
}

AT(.com_text.key.scan)
u8 bsp_get_io_key_id(void)
{
#if IO_KEY_WK0_EN
    if (wko_io_read_bit() == RESET) {                                   // KEY0(WK0/PP)
        return KEY_ID_PP;
    }
#endif

#if IO_KEY_SCAN_MODE
    u8 column = 0;
    u8 row = 0;

    for (u8 i = 0; i < IO_KEY_COL_TABLE_SIZE; i++) {
        GPIO_SET_DIR_IN(key_io_table_column[i].gpiox, key_io_table_column[i].gpio_pin);
        column |= (gpio_read_bit(key_io_table_column[i].gpiox, key_io_table_column[i].gpio_pin))? (0x01 << i) : 0x00;
        GPIO_SET_DIR_OUT(key_io_table_column[i].gpiox, key_io_table_column[i].gpio_pin);
        gpio_set_bits(key_io_table_column[i].gpiox, key_io_table_column[i].gpio_pin);
    }

    for (u8 i = 0; i < IO_KEY_ROW_TABLE_SIZE; i++) {
        GPIO_SET_DIR_IN(key_io_table_row[i].gpiox, key_io_table_row[i].gpio_pin);
        row |= (gpio_read_bit(key_io_table_row[i].gpiox, key_io_table_row[i].gpio_pin))? (0x01 << i) : 0x00;
        GPIO_SET_DIR_OUT(key_io_table_row[i].gpiox, key_io_table_row[i].gpio_pin);
        gpio_set_bits(key_io_table_row[i].gpiox, key_io_table_row[i].gpio_pin);
    }

    /* no combination key case */
    switch (column) {
        case 0x01:
            switch (row) {
                case 0x01:
                    return KEY_ID_K1;

                case 0x02:
                    return KEY_ID_K2;

                case 0x04:
                    break;

                case 0x08:
                    break;

                case 0x10:
                    break;

                case 0x20:
                    break;

                default:
                    break;
            }
            break;

        case 0x02:
            switch (row) {
                case 0x01:
                    return KEY_ID_K3;

                case 0x02:
                    return KEY_ID_K4;

                case 0x04:
                    break;

                case 0x08:
                    break;

                case 0x10:
                    break;

                case 0x20:
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
#else // !IO_KEY_SCAN_MODE
    /* Assume that the IO definition order in key_io_table_column corresponds to the desired key_id order */
    /* Or adjust the list based on actual requirements, for example, define a list corresponding to key_io_table_column */
    u8 key_id = KEY_ID_K1;
    for (u8 i = 0; i < IO_KEY_COL_TABLE_SIZE; i++) {
        if (gpio_read_bit(key_io_table_column[i].gpiox, key_io_table_column[i].gpio_pin) == RESET) {
            return key_id;
        }
        key_id++;
    }
#endif // IO_KEY_SCAN_MODE

    return KEY_ID_NO;
}

#endif // BSP_IOKEY_EN
