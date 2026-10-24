#ifndef __DRIVER_ADV_TIMER_H__
#define __DRIVER_ADV_TIMER_H__

/* Includes ------------------------------------------------------------------*/
#include "driver_com.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define assert_param(expr) ((void)0)
/** @addtogroup TIM
  * @{
  */

#define     __IO    volatile


/** @defgroup TIM_Exported_constants
  * @{
  */

#define IS_TIM_ALL_PERIPH(PERIPH) (((PERIPH) == TIM1)

/* LIST1: TIM 1 and 8 */
#define IS_TIM_LIST1_PERIPH(PERIPH)  (((PERIPH) == TIM1)

/* LIST2: TIM 1, 8, 15 16 and 17 */
#define IS_TIM_LIST2_PERIPH(PERIPH) (((PERIPH) == TIM1)
/* LIST3: TIM 1, 2, 3, 4, 5 and 8 */
#define IS_TIM_LIST3_PERIPH(PERIPH) (((PERIPH) == TIM1)

/* LIST4: TIM 1, 2, 3, 4, 5, 8, 15, 16 and 17 */
#define IS_TIM_LIST4_PERIPH(PERIPH) (((PERIPH) == TIM1)


/* LIST5: TIM 1, 2, 3, 4, 5, 8 and 15 */
#define IS_TIM_LIST5_PERIPH(PERIPH) (((PERIPH) == TIM1)

/* LIST6: TIM 1, 2, 3, 4, 5, 8, 9, 12 and 15 */
#define IS_TIM_LIST6_PERIPH(PERIPH)  (((PERIPH) == TIM1)

/* LIST7: TIM 1, 2, 3, 4, 5, 6, 7, 8, 9, 12 and 15 */
#define IS_TIM_LIST7_PERIPH(PERIPH)  (((PERIPH) == TIM1)

/* LIST8: TIM 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 16 and 17 */
#define IS_TIM_LIST8_PERIPH(PERIPH)  (((PERIPH) == TIM1)

/* LIST9: TIM 1, 2, 3, 4, 5, 6, 7, 8, 15, 16, and 17 */
#define IS_TIM_LIST9_PERIPH(PERIPH)  (((PERIPH) == TIM1)

/**
  * @}
  */

/** @defgroup TIM_Output_Compare_and_PWM_modes
  * @{
  */

#define TIM_OCMODE_TIMING                  ((uint16_t)0X0000)
#define TIM_OCMODE_ACTIVE                  ((uint16_t)0X0010)
#define TIM_OCMODE_INACTIVE                ((uint16_t)0X0020)
#define TIM_OCMODE_TOGGLE                  ((uint16_t)0X0030)
#define TIM_OCMODE_PWM1                    ((uint16_t)0X0060)
#define TIM_OCMODE_PWM2                    ((uint16_t)0X0070)
#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMODE_FROZEN) || \
                              ((MODE) == TIM_OCMODE_ACTIVE) || \
                              ((MODE) == TIM_OCMODE_INACTIVE) || \
                              ((MODE) == TIM_OCMODE_TOGGLE)|| \
                              ((MODE) == TIM_OCMODE_PWM1) || \
                              ((MODE) == TIM_OCMODE_PWM2))
#define IS_TIM_OCM(MODE) (((MODE) == TIM_OCMODE_FROZEN) || \
                          ((MODE) == TIM_OCMODE_ACTIVE) || \
                          ((MODE) == TIM_OCMODE_INACTIVE) || \
                          ((MODE) == TIM_OCMODE_TOGGLE)|| \
                          ((MODE) == TIM_OCMODE_PWM1) || \
                          ((MODE) == TIM_OCMODE_PWM2) ||	\
                          ((MODE) == TIM_FORCEDACTION_ACTIVE) || \
                          ((MODE) == TIM_FORCEDACTION_INACTIVE))
/**
  * @}
  */

/** @defgroup TIM_One_Pulse_Mode
  * @{
  */

#define TIM_OPMODE_SINGLE                  ((uint16_t)0X0008)
#define TIM_OPMODE_REPETITIVE              ((uint16_t)0X0000)
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMODE_SINGLE) || \
                               ((MODE) == TIM_OPMODE_REPETITIVE))
/**
  * @}
  */

/** @defgroup TIM_Channel
  * @{
  */

#define TIM_CHANNEL_1                      ((uint16_t)0X0000)
#define TIM_CHANNEL_2                      ((uint16_t)0X0004)
#define TIM_CHANNEL_3                      ((uint16_t)0X0008)
#define TIM_CHANNEL_4                      ((uint16_t)0X000C)
#define IS_TIM_CHANNEL(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                 ((CHANNEL) == TIM_CHANNEL_2) || \
                                 ((CHANNEL) == TIM_CHANNEL_3) || \
                                 ((CHANNEL) == TIM_CHANNEL_4))
#define IS_TIM_PWMI_CHANNEL(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                      ((CHANNEL) == TIM_CHANNEL_2))
#define IS_TIM_COMPLEMENTARY_CHANNEL(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                               ((CHANNEL) == TIM_CHANNEL_2) || \
                                               ((CHANNEL) == TIM_CHANNEL_3))
/**
  * @}
  */

/** @defgroup TIM_Clock_Division_CKD
  * @{
  */

#define TIM_CKD_DIV1                       ((uint16_t)0X0000)
#define TIM_CKD_DIV2                       ((uint16_t)0X0100)
#define TIM_CKD_DIV4                       ((uint16_t)0X0200)
#define IS_TIM_CKD_DIV(DIV) (((DIV) == TIM_CKD_DIV1) || \
                             ((DIV) == TIM_CKD_DIV2) || \
                             ((DIV) == TIM_CKD_DIV4))
/**
  * @}
  */

/** @DEFGROUP TIM_COUNTER_MODE
  * @{
  */

#define TIM_COUNTERMODE_UP                 ((uint16_t)0X0000)
#define TIM_COUNTERMODE_DOWN               ((uint16_t)0X0010)
#define TIM_COUNTERMODE_CENTERALIGNED1     ((uint16_t)0X0020)
#define TIM_COUNTERMODE_CENTERALIGNED2     ((uint16_t)0X0040)
#define TIM_COUNTERMODE_CENTERALIGNED3     ((uint16_t)0X0060)
#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_COUNTERMODE_UP) ||  \
                                   ((MODE) == TIM_COUNTERMODE_DOWN) || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED1) || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED2) || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED3))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_POLARITY
  * @{
  */

#define TIM_OCPOLARITY_HIGH                ((uint16_t)0X0000)
#define TIM_OCPOLARITY_LOW                 ((uint16_t)0X0002)
#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPOLARITY_HIGH) || \
                                      ((POLARITY) == TIM_OCPOLARITY_LOW))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_N_POLARITY
  * @{
  */

#define TIM_OCNPOLARITY_HIGH               ((uint16_t)0X0000)
#define TIM_OCNPOLARITY_LOW                ((uint16_t)0X0008)
#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPOLARITY_HIGH) || \
                                       ((POLARITY) == TIM_OCNPOLARITY_LOW))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_STATE
  * @{
  */

#define TIM_OUTPUTSTATE_DISABLE            ((uint16_t)0X0000)
#define TIM_OUTPUTSTATE_ENABLE             ((uint16_t)0X0001)
#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OUTPUTSTATE_DISABLE) || \
                                    ((STATE) == TIM_OUTPUTSTATE_ENABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_N_STATE
  * @{
  */

#define TIM_OUTPUTNSTATE_DISABLE           ((uint16_t)0X0000)
#define TIM_OUTPUTNSTATE_ENABLE            ((uint16_t)0X0004)
#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OUTPUTNSTATE_DISABLE) || \
                                     ((STATE) == TIM_OUTPUTNSTATE_ENABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_CAPTURE_COMPARE_STATE
  * @{
  */

#define TIM_CCX_ENABLE                      ((uint16_t)0X0001)
#define TIM_CCX_DISABLE                     ((uint16_t)0X0000)
#define IS_TIM_CCX(CCX) (((CCX) == TIM_CCX_ENABLE) || \
                         ((CCX) == TIM_CCX_DISABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_CAPTURE_COMPARE_N_STATE
  * @{
  */

#define TIM_CCXN_ENABLE                     ((uint16_t)0X0004)
#define TIM_CCXN_DISABLE                    ((uint16_t)0X0000)
#define IS_TIM_CCXN(CCXN) (((CCXN) == TIM_CCXN_ENABLE) || \
                           ((CCXN) == TIM_CCXN_DISABLE))
/**
  * @}
  */

/** @DEFGROUP BREAK_INPUT_ENABLE_DISABLE
  * @{
  */

#define TIM_BREAK_ENABLE                   ((uint16_t)0X1000)
#define TIM_BREAK_DISABLE                  ((uint16_t)0X0000)
#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_BREAK_ENABLE) || \
                                   ((STATE) == TIM_BREAK_DISABLE))
/**
  * @}
  */

/** @DEFGROUP BREAK_POLARITY
  * @{
  */

#define TIM_BREAKPOLARITY_LOW              ((uint16_t)0X0000)
#define TIM_BREAKPOLARITY_HIGH             ((uint16_t)0X2000)
#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BREAKPOLARITY_LOW) || \
                                         ((POLARITY) == TIM_BREAKPOLARITY_HIGH))
/**
  * @}
  */

/** @DEFGROUP TIM_AOE_BIT_SET_RESET
  * @{
  */

#define TIM_AUTOMATICOUTPUT_ENABLE         ((uint16_t)0X4000)
#define TIM_AUTOMATICOUTPUT_DISABLE        ((uint16_t)0X0000)
#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AUTOMATICOUTPUT_ENABLE) || \
                                              ((STATE) == TIM_AUTOMATICOUTPUT_DISABLE))
/**
  * @}
  */

/** @DEFGROUP LOCK_LEVEL
  * @{
  */

#define TIM_LOCKLEVEL_OFF                  ((uint16_t)0X0000)
#define TIM_LOCKLEVEL_1                    ((uint16_t)0X0100)
#define TIM_LOCKLEVEL_2                    ((uint16_t)0X0200)
#define TIM_LOCKLEVEL_3                    ((uint16_t)0X0300)
#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLEVEL_OFF) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_1) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_2) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_3))
/**
  * @}
  */

/** @DEFGROUP OSSI_OFF_STATE_SELECTION_FOR_IDLE_MODE_STATE
  * @{
  */

#define TIM_OSSISTATE_ENABLE               ((uint16_t)0X0400)
#define TIM_OSSISTATE_DISABLE              ((uint16_t)0X0000)
#define IS_TIM_OSSI_STATE(STATE) (((STATE) == TIM_OSSISTATE_ENABLE) || \
                                  ((STATE) == TIM_OSSISTATE_DISABLE))
/**
  * @}
  */

/** @DEFGROUP OSSR_OFF_STATE_SELECTION_FOR_RUN_MODE_STATE
  * @{
  */

#define TIM_OSSRSTATE_ENABLE               ((uint16_t)0X0800)
#define TIM_OSSRSTATE_DISABLE              ((uint16_t)0X0000)
#define IS_TIM_OSSR_STATE(STATE) (((STATE) == TIM_OSSRSTATE_ENABLE) || \
                                  ((STATE) == TIM_OSSRSTATE_DISABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_IDLE_STATE
  * @{
  */

#define TIM_OCIDLESTATE_SET                ((uint16_t)0X0100)
#define TIM_OCIDLESTATE_RESET              ((uint16_t)0X0000)
#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIDLESTATE_SET) || \
                                    ((STATE) == TIM_OCIDLESTATE_RESET))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_N_IDLE_STATE
  * @{
  */

#define TIM_OCNIDLESTATE_SET               ((uint16_t)0X0200)
#define TIM_OCNIDLESTATE_RESET             ((uint16_t)0X0000)
#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIDLESTATE_SET) || \
                                     ((STATE) == TIM_OCNIDLESTATE_RESET))
/**
  * @}
  */

/** @DEFGROUP TIM_INPUT_CAPTURE_POLARITY
  * @{
  */

#define  TIM_ICPOLARITY_RISING             ((uint16_t)0X0000)
#define  TIM_ICPOLARITY_FALLING            ((uint16_t)0X0002)
#define  TIM_ICPOLARITY_BOTHEDGE           ((uint16_t)0X000A)
#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPOLARITY_RISING) || \
                                      ((POLARITY) == TIM_ICPOLARITY_FALLING))
#define IS_TIM_IC_POLARITY_LITE(POLARITY) (((POLARITY) == TIM_ICPOLARITY_RISING) || \
                                           ((POLARITY) == TIM_ICPOLARITY_FALLING)|| \
                                           ((POLARITY) == TIM_ICPOLARITY_BOTHEDGE))
/**
  * @}
  */

/** @DEFGROUP TIM_INPUT_CAPTURE_SELECTION
  * @{
  */

#define TIM_ICSELECTION_DIRECTTI           ((uint16_t)0X0001) /*!< TIM INPUT 1, 2, 3 OR 4 IS SELECTED TO BE
                                                                   CONNECTED TO IC1, IC2, IC3 OR IC4, RESPECTIVELY */
#define TIM_ICSELECTION_INDIRECTTI         ((uint16_t)0X0002) /*!< TIM INPUT 1, 2, 3 OR 4 IS SELECTED TO BE
                                                                   CONNECTED TO IC2, IC1, IC4 OR IC3, RESPECTIVELY. */
#define TIM_ICSELECTION_TRC                ((uint16_t)0X0003) /*!< TIM INPUT 1, 2, 3 OR 4 IS SELECTED TO BE CONNECTED TO TRC. */
#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSELECTION_DIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_INDIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_TRC))
/**
  * @}
  */

/** @DEFGROUP TIM_INPUT_CAPTURE_PRESCALER
  * @{
  */

#define TIM_ICPSC_DIV1                     ((uint16_t)0X0000) /*!< CAPTURE PERFORMED EACH TIME AN EDGE IS DETECTED ON THE CAPTURE INPUT. */
#define TIM_ICPSC_DIV2                     ((uint16_t)0X0004) /*!< CAPTURE PERFORMED ONCE EVERY 2 EVENTS. */
#define TIM_ICPSC_DIV4                     ((uint16_t)0X0008) /*!< CAPTURE PERFORMED ONCE EVERY 4 EVENTS. */
#define TIM_ICPSC_DIV8                     ((uint16_t)0X000C) /*!< CAPTURE PERFORMED ONCE EVERY 8 EVENTS. */
#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))
/**
  * @}
  */

/** @DEFGROUP TIM_INTERRUPT_SOURCES
  * @{
  */

#define TIM_IT_UPDATE                      ((uint16_t)0X0001)
#define TIM_IT_CC1                         ((uint16_t)0X0002)
#define TIM_IT_CC2                         ((uint16_t)0X0004)
#define TIM_IT_CC3                         ((uint16_t)0X0008)
#define TIM_IT_CC4                         ((uint16_t)0X0010)
#define TIM_IT_COM                         ((uint16_t)0X0020)
#define TIM_IT_TRIGGER                     ((uint16_t)0X0040)
#define TIM_IT_BREAK                       ((uint16_t)0X0080)
#define IS_TIM_IT(IT) ((((IT) & (uint16_t)0XFF00) == 0X0000) && ((IT) != 0X0000))

#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_UPDATE) || \
                           ((IT) == TIM_IT_CC1) || \
                           ((IT) == TIM_IT_CC2) || \
                           ((IT) == TIM_IT_CC3) || \
                           ((IT) == TIM_IT_CC4) || \
                           ((IT) == TIM_IT_COM) || \
                           ((IT) == TIM_IT_TRIGGER) || \
                           ((IT) == TIM_IT_BREAK))
/**
  * @}
  */

/** @DEFGROUP TIM_DMA_BASE_ADDRESS
  * @{
  */

#define TIM_DMABASE_CR1                    ((uint16_t)0X0000)
#define TIM_DMABASE_CR2                    ((uint16_t)0X0001)
#define TIM_DMABASE_SMCR                   ((uint16_t)0X0002)
#define TIM_DMABASE_DIER                   ((uint16_t)0X0003)
#define TIM_DMABASE_SR                     ((uint16_t)0X0004)
#define TIM_DMABASE_EGR                    ((uint16_t)0X0005)
#define TIM_DMABASE_CCMR1                  ((uint16_t)0X0006)
#define TIM_DMABASE_CCMR2                  ((uint16_t)0X0007)
#define TIM_DMABASE_CCER                   ((uint16_t)0X0008)
#define TIM_DMABASE_CNT                    ((uint16_t)0X0009)
#define TIM_DMABASE_PSC                    ((uint16_t)0X000A)
#define TIM_DMABASE_ARR                    ((uint16_t)0X000B)
#define TIM_DMABASE_RCR                    ((uint16_t)0X000C)
#define TIM_DMABASE_CCR1                   ((uint16_t)0X000D)
#define TIM_DMABASE_CCR2                   ((uint16_t)0X000E)
#define TIM_DMABASE_CCR3                   ((uint16_t)0X000F)
#define TIM_DMABASE_CCR4                   ((uint16_t)0X0010)
#define TIM_DMABASE_BDTR                   ((uint16_t)0X0011)
#define TIM_DMABASE_DCR                    ((uint16_t)0X0012)
#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABASE_CR1) || \
                               ((BASE) == TIM_DMABASE_CR2) || \
                               ((BASE) == TIM_DMABASE_SMCR) || \
                               ((BASE) == TIM_DMABASE_DIER) || \
                               ((BASE) == TIM_DMABASE_SR) || \
                               ((BASE) == TIM_DMABASE_EGR) || \
                               ((BASE) == TIM_DMABASE_CCMR1) || \
                               ((BASE) == TIM_DMABASE_CCMR2) || \
                               ((BASE) == TIM_DMABASE_CCER) || \
                               ((BASE) == TIM_DMABASE_CNT) || \
                               ((BASE) == TIM_DMABASE_PSC) || \
                               ((BASE) == TIM_DMABASE_ARR) || \
                               ((BASE) == TIM_DMABASE_RCR) || \
                               ((BASE) == TIM_DMABASE_CCR1) || \
                               ((BASE) == TIM_DMABASE_CCR2) || \
                               ((BASE) == TIM_DMABASE_CCR3) || \
                               ((BASE) == TIM_DMABASE_CCR4) || \
                               ((BASE) == TIM_DMABASE_BDTR) || \
                               ((BASE) == TIM_DMABASE_DCR))
/**
  * @}
  */

/** @DEFGROUP TIM_DMA_BURST_LENGTH
  * @{
  */

#define TIM_DMABURSTLENGTH_1TRANSFER           ((uint16_t)0X0000)
#define TIM_DMABURSTLENGTH_2TRANSFERS          ((uint16_t)0X0100)
#define TIM_DMABURSTLENGTH_3TRANSFERS          ((uint16_t)0X0200)
#define TIM_DMABURSTLENGTH_4TRANSFERS          ((uint16_t)0X0300)
#define TIM_DMABURSTLENGTH_5TRANSFERS          ((uint16_t)0X0400)
#define TIM_DMABURSTLENGTH_6TRANSFERS          ((uint16_t)0X0500)
#define TIM_DMABURSTLENGTH_7TRANSFERS          ((uint16_t)0X0600)
#define TIM_DMABURSTLENGTH_8TRANSFERS          ((uint16_t)0X0700)
#define TIM_DMABURSTLENGTH_9TRANSFERS          ((uint16_t)0X0800)
#define TIM_DMABURSTLENGTH_10TRANSFERS         ((uint16_t)0X0900)
#define TIM_DMABURSTLENGTH_11TRANSFERS         ((uint16_t)0X0A00)
#define TIM_DMABURSTLENGTH_12TRANSFERS         ((uint16_t)0X0B00)
#define TIM_DMABURSTLENGTH_13TRANSFERS         ((uint16_t)0X0C00)
#define TIM_DMABURSTLENGTH_14TRANSFERS         ((uint16_t)0X0D00)
#define TIM_DMABURSTLENGTH_15TRANSFERS         ((uint16_t)0X0E00)
#define TIM_DMABURSTLENGTH_16TRANSFERS         ((uint16_t)0X0F00)
#define TIM_DMABURSTLENGTH_17TRANSFERS         ((uint16_t)0X1000)
#define TIM_DMABURSTLENGTH_18TRANSFERS         ((uint16_t)0X1100)
#define IS_TIM_DMA_LENGTH(LENGTH) (((LENGTH) == TIM_DMABURSTLENGTH_1TRANSFER) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_2TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_3TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_4TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_5TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_6TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_7TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_8TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_9TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_10TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_11TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_12TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_13TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_14TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_15TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_16TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_17TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_18TRANSFERS))
/**
  * @}
  */

/** @DEFGROUP TIM_DMA_SOURCES
  * @{
  */

#define TIM_DMA_UPDATE                     ((uint16_t)0X0100)
#define TIM_DMA_CC1                        ((uint16_t)0X0200)
#define TIM_DMA_CC2                        ((uint16_t)0X0400)
#define TIM_DMA_CC3                        ((uint16_t)0X0800)
#define TIM_DMA_CC4                        ((uint16_t)0X1000)
#define TIM_DMA_COM                        ((uint16_t)0X2000)
#define TIM_DMA_TRIGGER                    ((uint16_t)0X4000)
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & (uint16_t)0X80FF) == 0X0000) && ((SOURCE) != 0X0000))

/**
  * @}
  */

/** @DEFGROUP TIM_EXTERNAL_TRIGGER_PRESCALER
  * @{
  */

#define TIM_EXTTRGPSC_OFF                  ((uint16_t)0X0000)
#define TIM_EXTTRGPSC_DIV2                 ((uint16_t)0X1000)
#define TIM_EXTTRGPSC_DIV4                 ((uint16_t)0X2000)
#define TIM_EXTTRGPSC_DIV8                 ((uint16_t)0X3000)
#define IS_TIM_EXT_PRESCALER(PRESCALER) (((PRESCALER) == TIM_EXTTRGPSC_OFF) || \
                                         ((PRESCALER) == TIM_EXTTRGPSC_DIV2) || \
                                         ((PRESCALER) == TIM_EXTTRGPSC_DIV4) || \
                                         ((PRESCALER) == TIM_EXTTRGPSC_DIV8))
/**
  * @}
  */

/** @DEFGROUP TIM_INTERNAL_TRIGGER_SELECTION
  * @{
  */

#define TIM_TS_ITR0                        ((uint16_t)0X0000)
#define TIM_TS_ITR1                        ((uint16_t)0X0010)
#define TIM_TS_ITR2                        ((uint16_t)0X0020)
#define TIM_TS_ITR3                        ((uint16_t)0X0030)
#define TIM_TS_TI1F_ED                     ((uint16_t)0X0040)
#define TIM_TS_TI1FP1                      ((uint16_t)0X0050)
#define TIM_TS_TI2FP2                      ((uint16_t)0X0060)
#define TIM_TS_ETRF                        ((uint16_t)0X0070)
#define IS_TIM_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                             ((SELECTION) == TIM_TS_ITR1) || \
                                             ((SELECTION) == TIM_TS_ITR2) || \
                                             ((SELECTION) == TIM_TS_ITR3) || \
                                             ((SELECTION) == TIM_TS_TI1F_ED) || \
                                             ((SELECTION) == TIM_TS_TI1FP1) || \
                                             ((SELECTION) == TIM_TS_TI2FP2) || \
                                             ((SELECTION) == TIM_TS_ETRF))
#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                      ((SELECTION) == TIM_TS_ITR1) || \
                                                      ((SELECTION) == TIM_TS_ITR2) || \
                                                      ((SELECTION) == TIM_TS_ITR3))
/**
  * @}
  */

/** @DEFGROUP TIM_TIX_EXTERNAL_CLOCK_SOURCE
  * @{
  */

#define TIM_TIXEXTERNALCLK1SOURCE_TI1      ((uint16_t)0X0050)
#define TIM_TIXEXTERNALCLK1SOURCE_TI2      ((uint16_t)0X0060)
#define TIM_TIXEXTERNALCLK1SOURCE_TI1ED    ((uint16_t)0X0040)
#define IS_TIM_TIXCLK_SOURCE(SOURCE) (((SOURCE) == TIM_TIXEXTERNALCLK1SOURCE_TI1) || \
                                      ((SOURCE) == TIM_TIXEXTERNALCLK1SOURCE_TI2) || \
                                      ((SOURCE) == TIM_TIXEXTERNALCLK1SOURCE_TI1ED))
/**
  * @}
  */

/** @DEFGROUP TIM_EXTERNAL_TRIGGER_POLARITY
  * @{
  */
#define TIM_EXTTRGPOLARITY_INVERTED        ((uint16_t)0X8000)
#define TIM_EXTTRGPOLARITY_NONINVERTED     ((uint16_t)0X0000)
#define IS_TIM_EXT_POLARITY(POLARITY) (((POLARITY) == TIM_EXTTRGPOLARITY_INVERTED) || \
                                       ((POLARITY) == TIM_EXTTRGPOLARITY_NONINVERTED))
/**
  * @}
  */

/** @DEFGROUP TIM_PRESCALER_RELOAD_MODE
  * @{
  */

#define TIM_PSCRELOADMODE_UPDATE           ((uint16_t)0X0000)
#define TIM_PSCRELOADMODE_IMMEDIATE        ((uint16_t)0X0001)
#define IS_TIM_PRESCALER_RELOAD(RELOAD) (((RELOAD) == TIM_PSCRELOADMODE_UPDATE) || \
                                         ((RELOAD) == TIM_PSCRELOADMODE_IMMEDIATE))
/**
  * @}
  */

/** @DEFGROUP TIM_FORCED_ACTION
  * @{
  */

#define TIM_FORCEDACTION_ACTIVE            ((uint16_t)0X0050)
#define TIM_FORCEDACTION_INACTIVE          ((uint16_t)0X0040)
#define IS_TIM_FORCED_ACTION(ACTION) (((ACTION) == TIM_FORCEDACTION_ACTIVE) || \
                                      ((ACTION) == TIM_FORCEDACTION_INACTIVE))
/**
  * @}
  */

/** @DEFGROUP TIM_ENCODER_MODE
  * @{
  */

#define TIM_ENCODERMODE_TI1                ((uint16_t)0X0001)
#define TIM_ENCODERMODE_TI2                ((uint16_t)0X0002)
#define TIM_ENCODERMODE_TI12               ((uint16_t)0X0003)
#define IS_TIM_ENCODER_MODE(MODE) (((MODE) == TIM_ENCODERMODE_TI1) || \
                                   ((MODE) == TIM_ENCODERMODE_TI2) || \
                                   ((MODE) == TIM_ENCODERMODE_TI12))
/**
  * @}
  */


/** @DEFGROUP TIM_EVENT_SOURCE
  * @{
  */

#define TIM_EVENTSOURCE_UPDATE             ((uint16_t)0X0001)
#define TIM_EVENTSOURCE_CC1                ((uint16_t)0X0002)
#define TIM_EVENTSOURCE_CC2                ((uint16_t)0X0004)
#define TIM_EVENTSOURCE_CC3                ((uint16_t)0X0008)
#define TIM_EVENTSOURCE_CC4                ((uint16_t)0X0010)
#define TIM_EVENTSOURCE_COM                ((uint16_t)0X0020)
#define TIM_EVENTSOURCE_TRIGGER            ((uint16_t)0X0040)
#define TIM_EVENTSOURCE_BREAK              ((uint16_t)0X0080)
#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & (uint16_t)0XFF00) == 0X0000) && ((SOURCE) != 0X0000))

/**
  * @}
  */

/** @DEFGROUP TIM_UPDATE_SOURCE
  * @{
  */

#define TIM_UPDATESOURCE_GLOBAL            ((uint16_t)0X0000) /*!< SOURCE OF UPDATE IS THE COUNTER OVERFLOW/UNDERFLOW
                                                                   OR THE SETTING OF UG BIT, OR AN UPDATE GENERATION
                                                                   THROUGH THE SLAVE MODE CONTROLLER. */
#define TIM_UPDATESOURCE_REGULAR           ((uint16_t)0X0001) /*!< SOURCE OF UPDATE IS COUNTER OVERFLOW/UNDERFLOW. */
#define IS_TIM_UPDATE_SOURCE(SOURCE) (((SOURCE) == TIM_UPDATESOURCE_GLOBAL) || \
                                      ((SOURCE) == TIM_UPDATESOURCE_REGULAR))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_PRELOAD_STATE
  * @{
  */

#define TIM_OCPRELOAD_ENABLE               ((uint16_t)0X0008)
#define TIM_OCPRELOAD_DISABLE              ((uint16_t)0X0000)
#define IS_TIM_OCPRELOAD_STATE(STATE) (((STATE) == TIM_OCPRELOAD_ENABLE) || \
                                       ((STATE) == TIM_OCPRELOAD_DISABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_FAST_STATE
  * @{
  */

#define TIM_OCFAST_ENABLE                  ((uint16_t)0X0004)
#define TIM_OCFAST_DISABLE                 ((uint16_t)0X0000)
#define IS_TIM_OCFAST_STATE(STATE) (((STATE) == TIM_OCFAST_ENABLE) || \
                                    ((STATE) == TIM_OCFAST_DISABLE))

/**
  * @}
  */

/** @DEFGROUP TIM_OUTPUT_COMPARE_CLEAR_STATE
  * @{
  */

#define TIM_OCCLEAR_ENABLE                 ((uint16_t)0X0080)
#define TIM_OCCLEAR_DISABLE                ((uint16_t)0X0000)
#define IS_TIM_OCCLEAR_STATE(STATE) (((STATE) == TIM_OCCLEAR_ENABLE) || \
                                     ((STATE) == TIM_OCCLEAR_DISABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_TRIGGER_OUTPUT_SOURCE
  * @{
  */

#define TIM_TRGOSOURCE_RESET               ((uint16_t)0X0000)
#define TIM_TRGOSOURCE_ENABLE              ((uint16_t)0X0010)
#define TIM_TRGOSOURCE_UPDATE              ((uint16_t)0X0020)
#define TIM_TRGOSOURCE_OC1                 ((uint16_t)0X0030)
#define TIM_TRGOSOURCE_OC1REF              ((uint16_t)0X0040)
#define TIM_TRGOSOURCE_OC2REF              ((uint16_t)0X0050)
#define TIM_TRGOSOURCE_OC3REF              ((uint16_t)0X0060)
#define TIM_TRGOSOURCE_OC4REF              ((uint16_t)0X0070)
#define IS_TIM_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM_TRGOSOURCE_RESET) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_ENABLE) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_UPDATE) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_OC1) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_OC1REF) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_OC2REF) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_OC3REF) || \
                                    ((SOURCE) == TIM_TRGOSOURCE_OC4REF))
/**
  * @}
  */

/** @DEFGROUP TIM_SLAVE_MODE
  * @{
  */

#define TIM_SLAVEMODE_RESET                ((uint16_t)0X0004)
#define TIM_SLAVEMODE_GATED                ((uint16_t)0X0005)
#define TIM_SLAVEMODE_TRIGGER              ((uint16_t)0X0006)
#define TIM_SLAVEMODE_EXTERNAL1            ((uint16_t)0X0007)
#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SLAVEMODE_RESET) || \
                                 ((MODE) == TIM_SLAVEMODE_GATED) || \
                                 ((MODE) == TIM_SLAVEMODE_TRIGGER) || \
                                 ((MODE) == TIM_SLAVEMODE_EXTERNAL1))
/**
  * @}
  */

/** @DEFGROUP TIM_MASTER_SLAVE_MODE
  * @{
  */

#define TIM_MASTERSLAVEMODE_ENABLE         ((uint16_t)0X0080)
#define TIM_MASTERSLAVEMODE_DISABLE        ((uint16_t)0X0000)
#define IS_TIM_MSM_STATE(STATE) (((STATE) == TIM_MASTERSLAVEMODE_ENABLE) || \
                                 ((STATE) == TIM_MASTERSLAVEMODE_DISABLE))
/**
  * @}
  */

/** @DEFGROUP TIM_FLAGS
  * @{
  */

#define TIM_FLAG_UPDATE                    ((uint16_t)0X0001)
#define TIM_FLAG_CC1                       ((uint16_t)0X0002)
#define TIM_FLAG_CC2                       ((uint16_t)0X0004)
#define TIM_FLAG_CC3                       ((uint16_t)0X0008)
#define TIM_FLAG_CC4                       ((uint16_t)0X0010)
#define TIM_FLAG_COM                       ((uint16_t)0X0020)
#define TIM_FLAG_TRIGGER                   ((uint16_t)0X0040)
#define TIM_FLAG_BREAK                     ((uint16_t)0X0080)
#define TIM_FLAG_CC1OF                     ((uint16_t)0X0200)
#define TIM_FLAG_CC2OF                     ((uint16_t)0X0400)
#define TIM_FLAG_CC3OF                     ((uint16_t)0X0800)
#define TIM_FLAG_CC4OF                     ((uint16_t)0X1000)
#define IS_TIM_GET_FLAG(FLAG) (((FLAG) == TIM_FLAG_UPDATE) || \
                               ((FLAG) == TIM_FLAG_CC1) || \
                               ((FLAG) == TIM_FLAG_CC2) || \
                               ((FLAG) == TIM_FLAG_CC3) || \
                               ((FLAG) == TIM_FLAG_CC4) || \
                               ((FLAG) == TIM_FLAG_COM) || \
                               ((FLAG) == TIM_FLAG_TRIGGER) || \
                               ((FLAG) == TIM_FLAG_BREAK) || \
                               ((FLAG) == TIM_FLAG_CC1OF) || \
                               ((FLAG) == TIM_FLAG_CC2OF) || \
                               ((FLAG) == TIM_FLAG_CC3OF) || \
                               ((FLAG) == TIM_FLAG_CC4OF))


#define IS_TIM_CLEAR_FLAG(TIM_FLAG) ((((TIM_FLAG) & (uint16_t)0XE100) == 0X0000) && ((TIM_FLAG) != 0X0000))
/**
  * @}
  */

/** @DEFGROUP TIM_INPUT_CAPTURE_FILER_VALUE
  * @{
  */

#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0XF)
/**
  * @}
  */

/** @DEFGROUP TIM_EXTERNAL_TRIGGER_FILTER
  * @{
  */

#define IS_TIM_EXT_FILTER(EXTFILTER) ((EXTFILTER) <= 0XF)
/**
  * @}
  */

/** @DEFGROUP TIM_LEGACY
  * @{
  */

#define TIM_DMABURSTLENGTH_1BYTE           TIM_DMABURSTLENGTH_1TRANSFER
#define TIM_DMABURSTLENGTH_2BYTES          TIM_DMABURSTLENGTH_2TRANSFERS
#define TIM_DMABURSTLENGTH_3BYTES          TIM_DMABURSTLENGTH_3TRANSFERS
#define TIM_DMABURSTLENGTH_4BYTES          TIM_DMABURSTLENGTH_4TRANSFERS
#define TIM_DMABURSTLENGTH_5BYTES          TIM_DMABURSTLENGTH_5TRANSFERS
#define TIM_DMABURSTLENGTH_6BYTES          TIM_DMABURSTLENGTH_6TRANSFERS
#define TIM_DMABURSTLENGTH_7BYTES          TIM_DMABURSTLENGTH_7TRANSFERS
#define TIM_DMABURSTLENGTH_8BYTES          TIM_DMABURSTLENGTH_8TRANSFERS
#define TIM_DMABURSTLENGTH_9BYTES          TIM_DMABURSTLENGTH_9TRANSFERS
#define TIM_DMABURSTLENGTH_10BYTES         TIM_DMABURSTLENGTH_10TRANSFERS
#define TIM_DMABURSTLENGTH_11BYTES         TIM_DMABURSTLENGTH_11TRANSFERS
#define TIM_DMABURSTLENGTH_12BYTES         TIM_DMABURSTLENGTH_12TRANSFERS
#define TIM_DMABURSTLENGTH_13BYTES         TIM_DMABURSTLENGTH_13TRANSFERS
#define TIM_DMABURSTLENGTH_14BYTES         TIM_DMABURSTLENGTH_14TRANSFERS
#define TIM_DMABURSTLENGTH_15BYTES         TIM_DMABURSTLENGTH_15TRANSFERS
#define TIM_DMABURSTLENGTH_16BYTES         TIM_DMABURSTLENGTH_16TRANSFERS
#define TIM_DMABURSTLENGTH_17BYTES         TIM_DMABURSTLENGTH_17TRANSFERS
#define TIM_DMABURSTLENGTH_18BYTES         TIM_DMABURSTLENGTH_18TRANSFERS

/** @defgroup tim_exported_types
  * @{
  */

/**
  * @brief  tim time base init structure definition
  * @note   this structure is used with all TIMX except for tim6 and tim7.
  */
typedef struct
{
  __IO uint32_t cr1;
//  uint16_t  RESERVED0;
  __IO uint32_t cr2;
//  uint16_t  RESERVED1;
  __IO uint32_t smcr;
//  uint16_t  RESERVED2;
  __IO uint32_t dier;
//  uint16_t  RESERVED3;
  __IO uint32_t sr;
//  uint16_t  RESERVED4;
  __IO uint32_t egr;
//  uint16_t  RESERVED5;
  __IO uint32_t ccmr1;
//  uint16_t  RESERVED6;
  __IO uint32_t ccmr2;
//  uint16_t  RESERVED7;
  __IO uint32_t ccer;
//  uint16_t  RESERVED8;
  __IO uint32_t cnt;
//  uint16_t  RESERVED9;
  __IO uint32_t psc;
//  uint16_t  RESERVED10;
  __IO uint32_t arr;
//  uint16_t  RESERVED11;
  __IO uint32_t rcr;
//  uint16_t  RESERVED12;
  __IO uint32_t ccr1;
//  uint16_t  RESERVED13;
  __IO uint32_t ccr2;
//  uint16_t  RESERVED14;
  __IO uint32_t ccr3;
//  uint16_t  RESERVED15;
  __IO uint32_t ccr4;
//  uint16_t  RESERVED16;
  __IO uint32_t bdtr;
//  uint16_t  RESERVED17;
  __IO uint32_t sarad;
//  __IO uint32_t DCR;
////  uint16_t  RESERVED18;
//  __IO uint32_t DMAR;
////  uint16_t  RESERVED19;
} tim_typedef;

typedef struct
{
  uint32_t tim_clock_source;      /*!< tim clock source config*/

  uint16_t tim_prescaler;         /*!< specifies the prescaler value used to divide the tim clock.
                                       this parameter can be a number between 0x0000 and 0xffff */

  uint16_t tim_countermode;       /*!< specifies the counter mode.
                                       this parameter can be a value of @ref tim_counter_mode */

  uint16_t tim_period;            /*!< specifies the period value to be loaded into the active
                                       auto-reload register at the next update event.
                                       this parameter must be a number between 0x0000 and 0xffff.  */

  uint16_t tim_clockdivision;     /*!< specifies the clock division.
                                      this parameter can be a value of @ref tim_clock_division_ckd */

  uint8_t tim_repetitioncounter;  /*!< specifies the repetition counter value. each time the rcr downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the rcr value (n).
                                       this means in pwm mode that (n+1) corresponds to:
                                          - the number of pwm periods in edge-aligned mode
                                          - the number of half pwm period in center-aligned mode
                                       this parameter must be a number between 0x00 and 0xff.
                                       @note this parameter is valid only for tim1 and tim8. */
} tim_timebaseinittypedef;

/**
  * @brief  tim output compare init structure definition
  */

typedef struct
{
  uint16_t tim_ocmode;        /*!< specifies the tim mode.
                                   this parameter can be a value of @ref tim_output_compare_and_pwm_modes */

  uint16_t tim_outputstate;   /*!< specifies the tim output compare state.
                                   this parameter can be a value of @ref tim_output_compare_state */

  uint16_t tim_outputnstate;  /*!< specifies the tim complementary output compare state.
                                   this parameter can be a value of @ref tim_output_compare_n_state
                                   @note this parameter is valid only for tim1 and tim8. */

  uint16_t tim_pulse;         /*!< specifies the pulse value to be loaded into the capture compare register.
                                   this parameter can be a number between 0x0000 and 0xffff */

  uint16_t tim_ocpolarity;    /*!< specifies the output polarity.
                                   this parameter can be a value of @ref tim_output_compare_polarity */

  uint16_t tim_ocnpolarity;   /*!< specifies the complementary output polarity.
                                   this parameter can be a value of @ref tim_output_compare_n_polarity
                                   @note this parameter is valid only for tim1 and tim8. */

  uint16_t tim_ocidlestate;   /*!< specifies the tim output compare pin state during idle state.
                                   this parameter can be a value of @ref tim_output_compare_idle_state
                                   @note this parameter is valid only for tim1 and tim8. */

  uint16_t tim_ocnidlestate;  /*!< specifies the tim output compare pin state during idle state.
                                   this parameter can be a value of @ref tim_output_compare_n_idle_state
                                   @note this parameter is valid only for tim1 and tim8. */
} tim_ocinittypedef;

/**
  * @brief  tim input capture init structure definition
  */

typedef struct
{

  uint16_t tim_channel;      /*!< specifies the tim channel.
                                  this parameter can be a value of @ref tim_channel */

  uint16_t tim_icpolarity;   /*!< specifies the active edge of the input signal.
                                  this parameter can be a value of @ref tim_input_capture_polarity */

  uint16_t tim_icselection;  /*!< specifies the input.
                                  this parameter can be a value of @ref tim_input_capture_selection */

  uint16_t tim_icprescaler;  /*!< specifies the input capture prescaler.
                                  this parameter can be a value of @ref tim_input_capture_prescaler */

  uint16_t tim_icfilter;     /*!< specifies the input capture filter.
                                  this parameter can be a number between 0x0 and 0xf */
} tim_icinittypedef;

/**
  * @brief  bdtr structure definition
  * @note   this structure is used only with tim1 and tim8.
  */

typedef struct
{

  uint16_t tim_ossrstate;        /*!< specifies the off-state selection used in run mode.
                                      this parameter can be a value of @ref ossr_off_state_selection_for_run_mode_state */

  uint16_t tim_ossistate;        /*!< specifies the off-state used in idle state.
                                      this parameter can be a value of @ref ossi_off_state_selection_for_idle_mode_state */

  uint16_t tim_locklevel;        /*!< specifies the lock level parameters.
                                      this parameter can be a value of @ref lock_level */

  uint16_t tim_deadtime;         /*!< specifies the delay time between the switching-off and the
                                      switching-on of the outputs.
                                      this parameter can be a number between 0x00 and 0xff  */

  uint16_t tim_break;            /*!< specifies whether the tim break input is enabled or not.
                                      this parameter can be a value of @ref break_input_enable_disable */

  uint16_t tim_breakpolarity;    /*!< specifies the tim break input pin polarity.
                                      this parameter can be a value of @ref break_polarity */

  uint16_t tim_automaticoutput;  /*!< specifies whether the tim automatic output feature is enabled or not.
                                      this parameter can be a value of @ref tim_aoe_bit_set_reset */
} tim_bdtrinittypedef;



/**
 * @brief Clock Triggle Selection
 */
typedef enum {
    TIM_SYSTEM_CLOCK        = 0x00000000,
    TIM_COUNTER_RISING      = 0x00010000,
    TIM_COUNTER_FALLING     = 0x00020000,
    TIM_COUNTER_EDGE        = 0x00030000,
} TIM_CLOCK_TYPEDEF;



/**
  * @}
  */

/**
  * @}
  */

/** @defgroup tim_exported_macros
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_exported_functions
  * @{
  */

void tim_timebaseinit(tim_typedef* TIMX, tim_timebaseinittypedef* tim_timebaseinitstruct);
void tim_oc1init(tim_typedef* TIMX, tim_ocinittypedef* tim_ocinitstruct);
void tim_oc2init(tim_typedef* TIMX, tim_ocinittypedef* tim_ocinitstruct);
void tim_oc3init(tim_typedef* TIMX, tim_ocinittypedef* tim_ocinitstruct);
void tim_oc4init(tim_typedef* TIMX, tim_ocinittypedef* tim_ocinitstruct);
void tim_bdtrconfig(tim_typedef* TIMX, tim_bdtrinittypedef *tim_bdtrinitstruct);
void tim_cmd(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_ctrlpwmoutputs(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_itconfig(tim_typedef* TIMX, uint16_t tim_it, FUNCTIONAL_STATE newstate);
void tim_generateevent(tim_typedef* TIMX, uint16_t tim_eventsource);
void tim_etrclockmode2config(tim_typedef* TIMX, uint16_t tim_exttrgprescaler,uint16_t tim_exttrgpolarity, uint16_t exttrgfilter);
void tim_etrconfig(tim_typedef* TIMX, uint16_t tim_exttrgprescaler, uint16_t tim_exttrgpolarity,uint16_t exttrgfilter);
void tim_selectinputtrigger(tim_typedef* TIMX, uint16_t tim_inputtriggersource);
void tim_arrpreloadconfig(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_ccpreloadcontrol(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_oc1preloadconfig(tim_typedef* TIMX, uint16_t tim_ocpreload);
void tim_oc2preloadconfig(tim_typedef* TIMX, uint16_t tim_ocpreload);
void tim_oc3preloadconfig(tim_typedef* TIMX, uint16_t tim_ocpreload);
void tim_oc4preloadconfig(tim_typedef* TIMX, uint16_t tim_ocpreload);
void tim_oc1npolarityconfig(tim_typedef* TIMX, uint16_t tim_ocnpolarity);
void tim_oc2npolarityconfig(tim_typedef* TIMX, uint16_t tim_ocnpolarity);
void tim_oc3npolarityconfig(tim_typedef* TIMX, uint16_t tim_ocnpolarity);
void tim_ccxcmd(tim_typedef* TIMX, uint16_t tim_channel, uint16_t tim_ccx);
void tim_ccxncmd(tim_typedef* TIMX, uint16_t tim_channel, uint16_t tim_ccxn);
void tim_selectocxm(tim_typedef* TIMX, uint16_t tim_channel, uint16_t tim_ocmode);
void tim_selectonepulsemode(tim_typedef* TIMX, uint16_t tim_opmode);
void tim_setcompare1(tim_typedef* TIMX, uint16_t compare1);
void tim_setcompare2(tim_typedef* TIMX, uint16_t compare2);
void tim_setcompare3(tim_typedef* TIMX, uint16_t compare3);
void tim_setcompare4(tim_typedef* TIMX, uint16_t compare4);
void tim_setic1prescaler(tim_typedef* TIMX, uint16_t tim_icpsc);
void tim_setic2prescaler(tim_typedef* TIMX, uint16_t tim_icpsc);
void tim_setic3prescaler(tim_typedef* TIMX, uint16_t tim_icpsc);
void tim_setic4prescaler(tim_typedef* TIMX, uint16_t tim_icpsc);
FLAG_STATE tim_getflagstatus(tim_typedef* TIMX, uint16_t tim_flag);
void tim_clearflag(tim_typedef* TIMX, uint16_t tim_flag);
IT_STATE tim_getitstatus(tim_typedef* TIMX, uint16_t tim_it);
void tim_clearitpendingbit(tim_typedef* TIMX, uint16_t tim_it);
void tim_etrclockmode1config(tim_typedef* TIMX, uint16_t tim_exttrgprescaler, uint16_t tim_exttrgpolarity,uint16_t exttrgfilter);
void tim_prescalerconfig(tim_typedef* TIMX, uint16_t prescaler, uint16_t tim_pscreloadmode);
void tim_countermodeconfig(tim_typedef* TIMX, uint16_t tim_countermode);
void tim_encoderinterfaceconfig(tim_typedef* TIMX, uint16_t tim_encodermode,uint16_t tim_ic1polarity, uint16_t tim_ic2polarity);
void tim_forcedoc1config(tim_typedef* TIMX, uint16_t tim_forcedaction);
void tim_forcedoc2config(tim_typedef* TIMX, uint16_t tim_forcedaction);
void tim_forcedoc3config(tim_typedef* TIMX, uint16_t tim_forcedaction);
void tim_forcedoc4config(tim_typedef* TIMX, uint16_t tim_forcedaction);
void tim_selectcom(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_internalclockconfig(tim_typedef* TIMX);
void tim_clearoc1ref(tim_typedef* TIMX, uint16_t tim_occlear);
void tim_clearoc2ref(tim_typedef* TIMX, uint16_t tim_occlear);
void tim_clearoc3ref(tim_typedef* TIMX, uint16_t tim_occlear);
void tim_clearoc4ref(tim_typedef* TIMX, uint16_t tim_occlear);
void tim_oc1polarityconfig(tim_typedef* TIMX, uint16_t tim_ocpolarity);
void tim_oc2polarityconfig(tim_typedef* TIMX, uint16_t tim_ocpolarity);
void tim_oc3polarityconfig(tim_typedef* TIMX, uint16_t tim_ocpolarity);
void tim_oc4polarityconfig(tim_typedef* TIMX, uint16_t tim_ocpolarity);
void tim_updatedisableconfig(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
void tim_updaterequestconfig(tim_typedef* TIMX, uint16_t tim_updatesource);
void tim_selectoutputtrigger(tim_typedef* TIMX, uint16_t tim_trgosource);
void tim_selectslavemode(tim_typedef* TIMX, uint16_t tim_slavemode);
void tim_setcounter(tim_typedef* TIMX, uint16_t counter);
void tim_setautoreload(tim_typedef* TIMX, uint16_t autoreload);
void tim_setclockdivision(tim_typedef* TIMX, uint16_t tim_ckd);
uint16_t tim_getcounter(tim_typedef* TIMX);
uint16_t tim_getprescaler(tim_typedef* TIMX);
void tim_timebasestructinit(tim_timebaseinittypedef* tim_timebaseinitstruct);
void tim_ocstructinit(tim_ocinittypedef* tim_ocinitstruct);
void tim_trgosaradc_config(tim_typedef* TIMX,FUNCTIONAL_STATE newstate);
void tim_bdtrstructinit(tim_bdtrinittypedef* tim_bdtrinitstruct);
void tim_itrxexternalclockconfig(tim_typedef* TIMX, uint16_t tim_inputtriggersource);
void tim_tixexternalclockconfig(tim_typedef* TIMX, uint16_t tim_tixexternalclksource,uint16_t tim_icpolarity, uint16_t icfilter);



 /* 预留 */
//void tim_deinit(tim_typedef* TIMX);
//void tim_icinit(tim_typedef* TIMX, tim_icinittypedef* tim_icinitstruct);
//void tim_pwmiconfig(tim_typedef* TIMX, tim_icinittypedef* tim_icinitstruct);
//void tim_icstructinit(tim_icinittypedef* tim_icinitstruct);
//void tim_dmaconfig(tim_typedef* TIMX, uint16_t tim_dmabase, uint16_t tim_dmaburstlength);
//void tim_selectccdma(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
//void tim_oc1fastconfig(tim_typedef* TIMX, uint16_t tim_ocfast);
//void tim_oc2fastconfig(tim_typedef* TIMX, uint16_t tim_ocfast);
//void tim_oc3fastconfig(tim_typedef* TIMX, uint16_t tim_ocfast);
//void tim_oc4fastconfig(tim_typedef* TIMX, uint16_t tim_ocfast);
//void tim_selecthallsensor(tim_typedef* TIMX, FUNCTIONAL_STATE newstate);
//void tim_selectmasterslavemode(tim_typedef* TIMX, uint16_t tim_masterslavemode);
//void tim_dmacmd(tim_typedef* TIMX, uint16_t tim_dmasource, FUNCTIONAL_STATE newstate);
//uint16_t tim_getcapture1(tim_typedef* TIMX);
//uint16_t tim_getcapture2(tim_typedef* TIMX);
//uint16_t tim_getcapture3(tim_typedef* TIMX);
//uint16_t tim_getcapture4(tim_typedef* TIMX);

/*  */

#ifdef __cplusplus
}
#endif

#endif // __DRIVER_ADV_TIMER_H__




