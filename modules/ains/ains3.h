#ifndef _AINS3_H
#define _AINS3_H

/*
    nt:降噪等级，值越大降噪越明显
*/
void ains3_init(void);
void ains3_exit(void);
void ains3_process(s16 *buf);

#endif
