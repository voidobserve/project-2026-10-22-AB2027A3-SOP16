#ifndef _API_KEY_H
#define _API_KEY_H

// Key Type
#define KEY_SHORT               0x0000
#define KEY_SHORT_UP            0x0800
#define KEY_LONG                0x0A00
#define KEY_LONG_UP             0x0C00
#define KEY_HOLD                0x0E00
#define KEY_DOUBLE              0x1800      //2击
#define KEY_THREE               0x1900      //3击
#define KEY_FOUR                0x1A00      //4击
#define KEY_FIVE                0x1B00      //5击


void multiple_key_init(u16 internal, u16 *allow_multiple_key_table, u8 count_of_multiple_key);
u16 key_multi_press_process(u16 key_val);

#endif // _API_KEY_H
