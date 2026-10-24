#ifndef _DRIVER_SDDAC_H
#define _DRIVER_SDDAC_H

#include "driver_com.h"

enum {
    SDDAC_NORMAL_SPR_48k      = 0x00,
    SDDAC_NORMAL_SPR_24k,
    SDDAC_NORMAL_SPR_16k,
    SDDAC_NORMAL_SPR_12k,
    SDDAC_NORMAL_SPR_8k,

    SDDAC_HIGH_SPR_240k,
    SDDAC_HIGH_SPR_120k,
    SDDAC_HIGH_SPR_80k,
    SDDAC_HIGH_SPR_60k,
    SDDAC_HIGH_SPR_48k,
};


typedef enum {
    SDDAC_IT_ALL_DONE       = BIT(0),
    SDDAC_IT_HALF_DONE      = BIT(1),
} SDDAC_IT_TYPEDEF;


typedef struct{
    u8  sample_rate;
    u8  *dma_buf;
    u16  dma_size;
} sddac_init_typedef;


void sddac_init(sddac_init_typedef *config);
void sddac_clk_init(void);
void sddac_pic_config(isr_t isr, int pr, SDDAC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state);
void sddac_cmd(FUNCTIONAL_STATE state);
FLAG_STATE sddac_get_flag(SDDAC_IT_TYPEDEF interrupt_type);
void sddac_clear_flag(SDDAC_IT_TYPEDEF interrupt_type);
void sddac_dma_kick_start(uint8_t *buf, uint16_t size);
void sddac_dma_stop(void);
bool sddac_spr_set(uint8_t spr);
uint8_t sddac_spr_get(void);
void sddac_deinit(void);

#endif
