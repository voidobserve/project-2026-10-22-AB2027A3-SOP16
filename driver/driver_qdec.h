#ifndef __DRIVER_QDEC_H__
#define __DRIVER_QDEC_H__

#include "driver_com.h"

/**
 * @brief Quadrature Decoder clock source Definition
 */
typedef enum {
    QDEC_RC32K_CLK,
    QDEC_X24M_32K_CLK,
} QDEC_CLK_SEL_TYPEDEF;

/**
 * @brief Quadrature Decoder count mode Definition
 */
typedef enum {
    QDEC_CM_X1      = 0x01,
    QDEC_CM_X2      = 0x02,
    QDEC_CM_X4      = 0x03,
} QDEC_CM_SEL_TYPEDEF;

/**
 * @brief Quadrature Decoder samples number Definition
 */
typedef enum {
    QDEC_SMP_NUM_64,
    QDEC_SMP_NUM_640,
    QDEC_SMP_NUM_2560,
    QDEC_SMP_NUM_5120,
    QDEC_SMP_NUM_7680,
    QDEC_SMP_NUM_10240,
    QDEC_SMP_NUM_12800,
    QDEC_SMP_NUM_16360,
    QDEC_SMP_NUM_17920,
} QDEC_SMP_NUM_SEL_TYPEDEF;

/**
 * @brief Quadrature Decoder wakeup sleep enable Definition
 */
typedef enum {
    QDEC_WAKEUP_DISABLE,
    QDEC_WAKEUP_ENABLE,
} QDEC_WAKEUP_SEL_TYPEDEF;

/**
 * @brief Quadrature Decoder timer mode enable Definition
 */
typedef enum {
    QDEC_TM_DISABLE,
    QDEC_TM_ENABLE,
} QDEC_TM_SEL_TYPEDEF;

/**
 * @brief Quadrature Decoder Interrupt Flag Definition
 */
typedef enum {
    /*Setting*/
    QDEC_IT_SAMPLE_COUNT            = (0x01 << 0),
    QDEC_IT_REVERSE_DIR             = (0x01 << 1),
    QDEC_IT_FORWARD_DIR             = (0x01 << 2),

    /*Getting*/
    QDEC_REVERSE_DIR_PND            = (0x01 << 3),
    QDEC_FORWARD_DIR_PND            = (0x01 << 4),
    QDEC_OV_PND                     = (0x01 << 5),
    QDEC_SMP_PND                    = (0x01 << 6),
    QDEC_TERR_PND                   = (0x01 << 7),
} QDEC_IT_TYPEDEF;

/**
 * @brief QDEC Init Structure
 */
typedef struct{
    QDEC_CLK_SEL_TYPEDEF clk_source;        //select QDEC clock source.
    QDEC_SMP_NUM_SEL_TYPEDEF smp_num_sel;   //select numbers of samples. 
    QDEC_CM_SEL_TYPEDEF cm_sel;             //QDEC count mode select.
    QDEC_WAKEUP_SEL_TYPEDEF wakup_en;       //QDEC wakeup sleep mode enable.
    QDEC_TM_SEL_TYPEDEF tm_en;              //QDEC timer mode enable.
    u8 fil_len;                             //QDEC filter length.
}qdec_init_typedef;

void qdec_init(qdec_typedef *qdec, qdec_init_typedef *qdec_init_struct);
void qdec_deinit(qdec_typedef *qdec);
void qdec_cmd(qdec_typedef *qdec, FUNCTIONAL_STATE state);
uint32_t qdec_get_data(qdec_typedef *qdec);
void qdec_clear_data(qdec_typedef *qdec);
void qdec_pic_config(qdec_typedef *qdec, isr_t isr, int pr, QDEC_IT_TYPEDEF interrup_type, FUNCTIONAL_STATE state);
FLAG_STATE qdec_get_flag(qdec_typedef *qdec, QDEC_IT_TYPEDEF interrupt_type);
void qdec_clear_flag(qdec_typedef *qdec, QDEC_IT_TYPEDEF interrupt_type);

#endif // __DRIVER_QDEC_H__
