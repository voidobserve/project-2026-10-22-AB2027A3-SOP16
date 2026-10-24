#include "driver_adv_timer.h"



/* ---------------------- TIM registers bit mask ------------------------ */
#define SMCR_ETR_MASK               ((uint16_t)0x00FF)
#define CCMR_OFFSET                 ((uint16_t)0x0018)
#define CCER_CCE_SET                ((uint16_t)0x0001)
#define	CCER_CCNE_SET               ((uint16_t)0x0004)

/**
  * @}
  */

/** @defgroup tim_private_macros
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_private_variables
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_private_functionprototypes
  * @{
  */

static void ti1_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
                       uint16_t tim_icfilter);
static void ti2_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
                       uint16_t tim_icfilter);
//static void ti3_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
//                       uint16_t tim_icfilter);
//static void ti4_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
//                       uint16_t tim_icfilter);
/**
  * @}
  */

/** @defgroup tim_private_macros
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_private_variables
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_private_functionprototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup tim_private_functions
  * @{
  */

/**
  * @brief  deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @retval none
  */


/**
  * @brief  initializes the TIMx time base unit peripheral according to
  *         the specified parameters in the tim_timebaseinitstruct.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_timebaseinitstruct: pointer to a tim_timebaseinittypedef
  *         structure that contains the configuration information for the
  *         specified tim peripheral.
  * @retval none
  */
void tim_timebaseinit(tim_typedef* TIMx, tim_timebaseinittypedef* tim_timebaseinitstruct)
{
  uint32_t tmpcr1 = 0;

  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_COUNTER_MODE(tim_timebaseinitstruct->tim_countermode));
  assert_param(IS_TIM_CKD_DIV(tim_timebaseinitstruct->tim_clockdivision));

  tmpcr1 = TIMx->cr1;

  if(TIMx == TIM1)
  {
    /* select the counter mode */
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
    tmpcr1 |= (uint32_t)tim_timebaseinitstruct->tim_countermode;

  }

//  if((TIMx != tim6) && (TIMx != tim7))
//  {
//    /* set the clock division */
   tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CR1_CKD));
   tmpcr1 |= (uint32_t)tim_timebaseinitstruct->tim_clockdivision;
   tmpcr1 |= (uint32_t)tim_timebaseinitstruct->tim_clock_source;
//  }

  TIMx->cr1 = tmpcr1;

  /* set the autoreload value */
  TIMx->arr = tim_timebaseinitstruct->tim_period ;

  /* set the prescaler value */
  TIMx->psc = tim_timebaseinitstruct->tim_prescaler;

  if (TIMx == TIM1)
  {
    /* set the repetition counter value */
    TIMx->rcr = tim_timebaseinitstruct->tim_repetitioncounter;
  }

  /* generate an update event to reload the prescaler and the repetition counter
     values immediately */
  TIMx->egr = TIM_PSCRELOADMODE_IMMEDIATE;
}

/**
  * @brief  initializes the TIMx channel1 according to the specified
  *         parameters in the tim_ocinitstruct.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_ocinitstruct: pointer to a tim_ocinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
void tim_oc1init(tim_typedef* TIMx, tim_ocinittypedef* tim_ocinitstruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(tim_ocinitstruct->tim_ocmode));
  assert_param(IS_TIM_OUTPUT_STATE(tim_ocinitstruct->tim_outputstate));
  assert_param(IS_TIM_OC_POLARITY(tim_ocinitstruct->tim_ocpolarity));
 /* DISABLE the channel 1: reset the cc1e bit */
  TIMx->ccer &= (uint16_t)(~(uint16_t)TIM_CCER_CC1E);
  /* get the TIMx ccer register value */
  tmpccer = TIMx->ccer;
  /* get the TIMx cr2 register value */
  tmpcr2 =  TIMx->cr2;

  /* get the TIMx ccmr1 register value */
  tmpccmrx = TIMx->ccmr1;

  /* reset the output compare mode bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_OC1M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_CC1S)); // 预留 ■

  /* select the output compare mode */
  tmpccmrx |= tim_ocinitstruct->tim_ocmode;

  /* reset the output polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1P));
  /* set the output compare polarity */
  tmpccer |= tim_ocinitstruct->tim_ocpolarity;

  /* set the output state */
  tmpccer |= tim_ocinitstruct->tim_outputstate;

  if(TIMx == TIM1)
  {
    assert_param(IS_TIM_OUTPUTN_STATE(tim_ocinitstruct->tim_outputnstate));
    assert_param(IS_TIM_OCN_POLARITY(tim_ocinitstruct->tim_ocnpolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(tim_ocinitstruct->tim_ocnidlestate));
    assert_param(IS_TIM_OCIDLE_STATE(tim_ocinitstruct->tim_ocidlestate));

    /* reset the output n polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1NP));
    /* set the output n polarity */
    tmpccer |= tim_ocinitstruct->tim_ocnpolarity;

    /* reset the output n state */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1NE));
    /* set the output n state */
    tmpccer |= tim_ocinitstruct->tim_outputnstate;

    /* reset the output compare and output compare n idle state */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS1));   // 预留 ■

    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS1N)); 	// 预留 ■

    /* set the output idle state */
    tmpcr2 |= tim_ocinitstruct->tim_ocidlestate;
    /* set the output n idle state */
    tmpcr2 |= tim_ocinitstruct->tim_ocnidlestate;
  }
  /* write to TIMx cr2 */
  TIMx->cr2 = tmpcr2;

  /* write to TIMx ccmr1 */
  TIMx->ccmr1 = tmpccmrx;

  /* set the capture compare register value */
  TIMx->ccr1 = tim_ocinitstruct->tim_pulse;

  /* write to TIMx ccer */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  initializes the TIMx channel2 according to the specified
  *         parameters in the tim_ocinitstruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the tim peripheral.
  * @param  tim_ocinitstruct: pointer to a tim_ocinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
void tim_oc2init(tim_typedef* TIMx, tim_ocinittypedef* tim_ocinitstruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(tim_ocinitstruct->tim_ocmode));
  assert_param(IS_TIM_OUTPUT_STATE(tim_ocinitstruct->tim_outputstate));
  assert_param(IS_TIM_OC_POLARITY(tim_ocinitstruct->tim_ocpolarity));
   /* DISABLE the channel 2: reset the cc2e bit */
  TIMx->ccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2E));

  /* get the TIMx ccer register value */
  tmpccer = TIMx->ccer;
  /* get the TIMx cr2 register value */
  tmpcr2 =  TIMx->cr2;

  /* get the TIMx ccmr1 register value */
  tmpccmrx = TIMx->ccmr1;

  /* reset the output compare mode and capture/compare selection bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_OC2M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_CC2S)); // 预留 ■

  /* select the output compare mode */
  tmpccmrx |= (uint16_t)(tim_ocinitstruct->tim_ocmode << 8);

  /* reset the output polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2P));
  /* set the output compare polarity */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_ocpolarity << 4);

  /* set the output state */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_outputstate << 4);

  if(TIMx == TIM1)
  {
    assert_param(IS_TIM_OUTPUTN_STATE(tim_ocinitstruct->tim_outputnstate));
    assert_param(IS_TIM_OCN_POLARITY(tim_ocinitstruct->tim_ocnpolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(tim_ocinitstruct->tim_ocnidlestate));
    assert_param(IS_TIM_OCIDLE_STATE(tim_ocinitstruct->tim_ocidlestate));

    /* reset the output n polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2NP));
    /* set the output n polarity */
    tmpccer |= (uint16_t)(tim_ocinitstruct->tim_ocnpolarity << 4);

    /* reset the output n state */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2NE));
    /* set the output n state */
    tmpccer |= (uint16_t)(tim_ocinitstruct->tim_outputnstate << 4);

    /* reset the output compare and output compare n idle state */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS2));
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS2N));

    /* set the output idle state */
    tmpcr2 |= (uint16_t)(tim_ocinitstruct->tim_ocidlestate << 2);
    /* set the output n idle state */
    tmpcr2 |= (uint16_t)(tim_ocinitstruct->tim_ocnidlestate << 2);
  }
  /* write to TIMx cr2 */
  TIMx->cr2 = tmpcr2;

  /* write to TIMx ccmr1 */
  TIMx->ccmr1 = tmpccmrx;

  /* set the capture compare register value */
  TIMx->ccr2 = tim_ocinitstruct->tim_pulse;

  /* write to TIMx ccer */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  initializes the TIMx channel3 according to the specified
  *         parameters in the tim_ocinitstruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocinitstruct: pointer to a tim_ocinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
void tim_oc3init(tim_typedef* TIMx, tim_ocinittypedef* tim_ocinitstruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(tim_ocinitstruct->tim_ocmode));
  assert_param(IS_TIM_OUTPUT_STATE(tim_ocinitstruct->tim_outputstate));
  assert_param(IS_TIM_OC_POLARITY(tim_ocinitstruct->tim_ocpolarity));
  /* DISABLE the channel 2: reset the cc2e bit */
  TIMx->ccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3E));

  /* get the TIMx ccer register value */
  tmpccer = TIMx->ccer;
  /* get the TIMx cr2 register value */
  tmpcr2 =  TIMx->cr2;

  /* get the TIMx ccmr2 register value */
  tmpccmrx = TIMx->ccmr2;

  /* reset the output compare mode and capture/compare selection bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_OC3M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_CC3S));// 预留 ■
  /* select the output compare mode */
  tmpccmrx |= tim_ocinitstruct->tim_ocmode;

  /* reset the output polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3P));
  /* set the output compare polarity */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_ocpolarity << 8);

  /* set the output state */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_outputstate << 8);

  if(TIMx == TIM1)
  {
    assert_param(IS_TIM_OUTPUTN_STATE(tim_ocinitstruct->tim_outputnstate));
    assert_param(IS_TIM_OCN_POLARITY(tim_ocinitstruct->tim_ocnpolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(tim_ocinitstruct->tim_ocnidlestate));
    assert_param(IS_TIM_OCIDLE_STATE(tim_ocinitstruct->tim_ocidlestate));

    /* reset the output n polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3NP));
    /* set the output n polarity */
    tmpccer |= (uint16_t)(tim_ocinitstruct->tim_ocnpolarity << 8);
    /* reset the output n state */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3NE));

    /* set the output n state */
    tmpccer |= (uint16_t)(tim_ocinitstruct->tim_outputnstate << 8);
    /* reset the output compare and output compare n idle state */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS3));
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS3N));
    /* set the output idle state */
    tmpcr2 |= (uint16_t)(tim_ocinitstruct->tim_ocidlestate << 4);
    /* set the output n idle state */
    tmpcr2 |= (uint16_t)(tim_ocinitstruct->tim_ocnidlestate << 4);
  }
  /* write to TIMx cr2 */
  TIMx->cr2 = tmpcr2;

  /* write to TIMx ccmr2 */
  TIMx->ccmr2 = tmpccmrx;

  /* set the capture compare register value */
  TIMx->ccr3 = tim_ocinitstruct->tim_pulse;

  /* write to TIMx ccer */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  initializes the TIMx channel4 according to the specified
  *         parameters in the tim_ocinitstruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocinitstruct: pointer to a tim_ocinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
void tim_oc4init(tim_typedef* TIMx, tim_ocinittypedef* tim_ocinitstruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(tim_ocinitstruct->tim_ocmode));
  assert_param(IS_TIM_OUTPUT_STATE(tim_ocinitstruct->tim_outputstate));
  assert_param(IS_TIM_OC_POLARITY(tim_ocinitstruct->tim_ocpolarity));
  /* DISABLE the channel 2: reset the cc4e bit */
  TIMx->ccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC4E));

  /* get the TIMx ccer register value */
  tmpccer = TIMx->ccer;
  /* get the TIMx cr2 register value */
  tmpcr2 =  TIMx->cr2;

  /* get the TIMx ccmr2 register value */
  tmpccmrx = TIMx->ccmr2;

  /* reset the output compare mode and capture/compare selection bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_OC4M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_CC4S)); // 预留 ■

  /* select the output compare mode */
  tmpccmrx |= (uint16_t)(tim_ocinitstruct->tim_ocmode << 8);

  /* reset the output polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC4P));
  /* set the output compare polarity */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_ocpolarity << 12);

  /* set the output state */
  tmpccer |= (uint16_t)(tim_ocinitstruct->tim_outputstate << 12);

  if(TIMx == TIM1)
  {
    assert_param(IS_TIM_OCIDLE_STATE(tim_ocinitstruct->tim_ocidlestate));
    /* reset the output compare idle state */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS4));
    /* set the output idle state */
    tmpcr2 |= (uint16_t)(tim_ocinitstruct->tim_ocidlestate << 6);
  }
  /* write to TIMx cr2 */
  TIMx->cr2 = tmpcr2;

  /* write to TIMx ccmr2 */
  TIMx->ccmr2 = tmpccmrx;

  /* set the capture compare register value */
  TIMx->ccr4 = tim_ocinitstruct->tim_pulse;

  /* write to TIMx ccer */
  TIMx->ccer = tmpccer;
}



/**
  * @brief  initializes the tim peripheral according to the specified
  *         parameters in the tim_icinitstruct.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_icinitstruct: pointer to a tim_icinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
//void tim_icinit(tim_typedef* TIMx, tim_icinittypedef* tim_icinitstruct)  // 预留
//{
//  /* check the parameters */
//  assert_param(IS_TIM_CHANNEL(tim_icinitstruct->tim_channel));
//  assert_param(IS_TIM_IC_SELECTION(tim_icinitstruct->tim_icselection));
//  assert_param(IS_TIM_IC_PRESCALER(tim_icinitstruct->tim_icprescaler));
//  assert_param(IS_TIM_IC_FILTER(tim_icinitstruct->tim_icfilter));
//
//  if(TIMx == TIM1)
//  {
//    assert_param(IS_TIM_IC_POLARITY(tim_icinitstruct->tim_icpolarity));
//  }
//  else
//  {
//    assert_param(IS_TIM_IC_POLARITY_LITE(tim_icinitstruct->tim_icpolarity));
//  }
//  if (tim_icinitstruct->tim_channel == TIM_CHANNEL_1)
//  {
//    assert_param(IS_TIM_LIST8_PERIPH(TIMx));
//    /* ti1 configuration */
//    ti1_config(TIMx, tim_icinitstruct->tim_icpolarity,
//               tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic1prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//  else if (tim_icinitstruct->tim_channel == TIM_CHANNEL_2)
//  {
//    assert_param(IS_TIM_LIST6_PERIPH(TIMx));
//    /* ti2 configuration */
//    ti2_config(TIMx, tim_icinitstruct->tim_icpolarity,
//               tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic2prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//  else if (tim_icinitstruct->tim_channel == TIM_CHANNEL_3)
//  {
//    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//    /* ti3 configuration */
//    ti3_config(TIMx,  tim_icinitstruct->tim_icpolarity,
//               tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic3prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//  else
//  {
//    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//    /* ti4 configuration */
//    ti4_config(TIMx, tim_icinitstruct->tim_icpolarity,
//               tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic4prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//}

/**
  * @brief  configures the tim peripheral according to the specified
  *         parameters in the tim_icinitstruct to measure an external pwm signal.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_icinitstruct: pointer to a tim_icinittypedef structure
  *         that contains the configuration information for the specified tim peripheral.
  * @retval none
  */
//void tim_pwmiconfig(tim_typedef* TIMx, tim_icinittypedef* tim_icinitstruct) // 预留
//{
//  uint16_t icoppositepolarity = TIM_ICPOLARITY_RISING;
//  uint16_t icoppositeselection = TIM_ICSELECTION_DIRECTTI;
//  /* check the parameters */
//  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
//  /* select the opposite input polarity */
//  if (tim_icinitstruct->tim_icpolarity == TIM_ICPOLARITY_RISING)
//  {
//    icoppositepolarity = TIM_ICPOLARITY_FALLING;
//  }
//  else
//  {
//    icoppositepolarity = TIM_ICPOLARITY_RISING;
//  }
//  /* select the opposite input */
//  if (tim_icinitstruct->tim_icselection == TIM_ICSELECTION_DIRECTTI)
//  {
//    icoppositeselection = TIM_ICSELECTION_INDIRECTTI;
//  }
//  else
//  {
//    icoppositeselection = TIM_ICSELECTION_DIRECTTI;
//  }
//  if (tim_icinitstruct->tim_channel == TIM_CHANNEL_1)
//  {
//    /* ti1 configuration */
//    ti1_config(TIMx, tim_icinitstruct->tim_icpolarity, tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic1prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//    /* ti2 configuration */
//    ti2_config(TIMx, icoppositepolarity, icoppositeselection, tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic2prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//  else
//  {
//    /* ti2 configuration */
//    ti2_config(TIMx, tim_icinitstruct->tim_icpolarity, tim_icinitstruct->tim_icselection,
//               tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic2prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//    /* ti1 configuration */
//    ti1_config(TIMx, icoppositepolarity, icoppositeselection, tim_icinitstruct->tim_icfilter);
//    /* set the input capture prescaler value */
//    tim_setic1prescaler(TIMx, tim_icinitstruct->tim_icprescaler);
//  }
//}

/**
  * @brief  configures the: break feature, dead time, lock level, the ossi,
  *         the ossr state and the aoe(automatic output enable).
  * @param  TIMx: where x can be  1 or 8 to select the tim
  * @param  tim_bdtrinitstruct: pointer to a tim_bdtrinittypedef structure that
  *         contains the bdtr register configuration  information for the tim peripheral.
  * @retval none
  */
void tim_bdtrconfig(tim_typedef* TIMx, tim_bdtrinittypedef *tim_bdtrinitstruct)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_OSSR_STATE(tim_bdtrinitstruct->tim_ossrstate));
  assert_param(IS_TIM_OSSI_STATE(tim_bdtrinitstruct->tim_ossistate));
  assert_param(IS_TIM_LOCK_LEVEL(tim_bdtrinitstruct->tim_locklevel));
  assert_param(IS_TIM_BREAK_STATE(tim_bdtrinitstruct->tim_break));
  assert_param(IS_TIM_BREAK_POLARITY(tim_bdtrinitstruct->tim_breakpolarity));
  assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(tim_bdtrinitstruct->tim_automaticoutput));
  /* set the lock level, the break enable bit and the ploarity, the ossr state,
     the ossi state, the dead time value and the automatic output enable bit */
  TIMx->bdtr = (uint32_t)tim_bdtrinitstruct->tim_ossrstate | tim_bdtrinitstruct->tim_ossistate |
             tim_bdtrinitstruct->tim_locklevel | tim_bdtrinitstruct->tim_deadtime |
             tim_bdtrinitstruct->tim_break | tim_bdtrinitstruct->tim_breakpolarity |
             tim_bdtrinitstruct->tim_automaticoutput;
}

/**
  * @brief  fills each tim_timebaseinitstruct member with its default value.
  * @param  tim_timebaseinitstruct : pointer to a tim_timebaseinittypedef
  *         structure which will be initialized.
  * @retval none
  */
void tim_timebasestructinit(tim_timebaseinittypedef* tim_timebaseinitstruct)
{
  /* set the default configuration */
  tim_timebaseinitstruct->tim_period = 0xffff;
  tim_timebaseinitstruct->tim_prescaler = 0x0000;
  tim_timebaseinitstruct->tim_clockdivision = TIM_CKD_DIV1;
  tim_timebaseinitstruct->tim_countermode = TIM_COUNTERMODE_UP;
  tim_timebaseinitstruct->tim_repetitioncounter = 0x0000;
}

/**
  * @brief  fills each tim_ocinitstruct member with its default value.
  * @param  tim_ocinitstruct : pointer to a tim_ocinittypedef structure which will
  *         be initialized.
  * @retval none
  */
void tim_ocstructinit(tim_ocinittypedef* tim_ocinitstruct)
{
  /* set the default configuration */
  tim_ocinitstruct->tim_ocmode = TIM_OCMODE_TIMING;
  tim_ocinitstruct->tim_outputstate = TIM_OUTPUTSTATE_DISABLE;
  tim_ocinitstruct->tim_outputnstate = TIM_OUTPUTNSTATE_DISABLE;
  tim_ocinitstruct->tim_pulse = 0x0000;
  tim_ocinitstruct->tim_ocpolarity = TIM_OCPOLARITY_HIGH;
  tim_ocinitstruct->tim_ocnpolarity = TIM_OCNPOLARITY_HIGH;
  tim_ocinitstruct->tim_ocidlestate = TIM_OCIDLESTATE_RESET;
  tim_ocinitstruct->tim_ocnidlestate = TIM_OCNIDLESTATE_RESET;
}

/**
  * @brief  fills each tim_icinitstruct member with its default value.
  * @param  tim_icinitstruct: pointer to a tim_icinittypedef structure which will
  *         be initialized.
  * @retval none
  */
//void tim_icstructinit(tim_icinittypedef* tim_icinitstruct) // 预留
//{
//  /* set the default configuration */
//  tim_icinitstruct->tim_channel = TIM_CHANNEL_1;
//  tim_icinitstruct->tim_icpolarity = TIM_ICPOLARITY_RISING;
//  tim_icinitstruct->tim_icselection = TIM_ICSELECTION_DIRECTTI;
//  tim_icinitstruct->tim_icprescaler = TIM_ICPSC_DIV1;
//  tim_icinitstruct->tim_icfilter = 0x00;
//}

/**
  * @brief  fills each tim_bdtrinitstruct member with its default value.
  * @param  tim_bdtrinitstruct: pointer to a tim_bdtrinittypedef structure which
  *         will be initialized.
  * @retval none
  */
void tim_bdtrstructinit(tim_bdtrinittypedef* tim_bdtrinitstruct)
{
  /* set the default configuration */
  tim_bdtrinitstruct->tim_ossrstate = TIM_OSSRSTATE_DISABLE;
  tim_bdtrinitstruct->tim_ossistate = TIM_OSSISTATE_DISABLE;
  tim_bdtrinitstruct->tim_locklevel = TIM_LOCKLEVEL_OFF;
  tim_bdtrinitstruct->tim_deadtime = 0x00;
  tim_bdtrinitstruct->tim_break = TIM_BREAK_DISABLE;
  tim_bdtrinitstruct->tim_breakpolarity = TIM_BREAKPOLARITY_LOW;
  tim_bdtrinitstruct->tim_automaticoutput = TIM_AUTOMATICOUTPUT_DISABLE;
}

/**
  * @brief  enables or DISABLEs the specified tim peripheral.
  * @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
  * @param  newstate: new state of the TIMx peripheral.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_text.tim.cmd)
void tim_cmd(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(newstate));

  if (newstate != DISABLE)
  {
    /* enable the tim counter */
    TIMx->cr1 |= TIM_CR1_CEN;
  }
  else
  {
    /* DISABLE the tim counter */
    TIMx->cr1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }
}

/**
  * @brief  enables or DISABLEs the tim peripheral main outputs.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the TIMx peripheral.
  * @param  newstate: new state of the tim peripheral main outputs.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_text.tim.ctrlpwmoutputs)
void tim_ctrlpwmoutputs(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(newstate));
  if (newstate != DISABLE)
  {
    /* enable the tim main output */
    TIMx->bdtr |= TIM_BDTR_MOE;
  }
  else
  {
    /* DISABLE the tim main output */
    TIMx->bdtr &= (uint32_t)(~((uint32_t)TIM_BDTR_MOE));
  }
}

/**
  * @brief  enables or DISABLEs the specified tim interrupts.
  * @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
  * @param  tim_it: specifies the tim interrupts sources to be enabled or DISABLEd.
  *   this parameter can be any combination of the following values:
  *     @arg tim_it_update: tim update interrupt source
  *     @arg tim_it_cc1: tim capture compare 1 interrupt source
  *     @arg tim_it_cc2: tim capture compare 2 interrupt source
  *     @arg tim_it_cc3: tim capture compare 3 interrupt source
  *     @arg tim_it_cc4: tim capture compare 4 interrupt source
  *     @arg tim_it_com: tim commutation interrupt source
  *     @arg tim_it_trigger: tim trigger interrupt source
  *     @arg tim_it_break: tim break interrupt source
  * @note
  *   - tim6 and tim7 can only generate an update interrupt.
  *   - tim9, TIM12 and TIM15 can have only tim_it_update, tim_it_cc1,
  *      tim_it_cc2 or tim_it_trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have tim_it_update or tim_it_cc1.
  *   - tim_it_break is used only with TIM1, tim8 and TIM15.
  *   - tim_it_com is used only with TIM1, tim8, TIM15, TIM16 and TIM17.
  * @param  newstate: new state of the tim interrupts.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
void tim_itconfig(tim_typedef* TIMx, uint16_t tim_it, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_IT(tim_it));
  assert_param(IS_FUNCTIONAL_STATE(newstate));

  if (newstate != DISABLE)
  {
    /* enable the interrupt sources */
    TIMx->dier |= tim_it;
  }
  else
  {
    /* DISABLE the interrupt sources */
    TIMx->dier &= (uint16_t)~tim_it;
  }
}

/**
  * @brief  configures the TIMx event to be generate by software.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_eventsource: specifies the event source.
  *   this parameter can be one or more of the following values:
  *     @arg tim_eventsource_update: timer update event source
  *     @arg tim_eventsource_cc1: timer capture compare 1 event source
  *     @arg tim_eventsource_cc2: timer capture compare 2 event source
  *     @arg tim_eventsource_cc3: timer capture compare 3 event source
  *     @arg tim_eventsource_cc4: timer capture compare 4 event source
  *     @arg tim_eventsource_com: timer com event source
  *     @arg tim_eventsource_trigger: timer trigger event source
  *     @arg tim_eventsource_break: timer break event source
  * @note
  *   - tim6 and tim7 can only generate an update event.
  *   - tim_eventsource_com and tim_eventsource_break are used only with TIM1 and tim8.
  * @retval none
  */
AT(.com_periph.tim.generateevent)
void tim_generateevent(tim_typedef* TIMx, uint16_t tim_eventsource)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_EVENT_SOURCE(tim_eventsource));

  /* set the event sources */
  TIMx->egr = tim_eventsource;
}

///**
//  * @brief  configures the TIMx's dma interface.
//  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 15, 16 or 17 to select
//  *   the tim peripheral.
//  * @param  tim_dmabase: dma base address.
//  *   this parameter can be one of the following values:
//  *     @arg tim_dmabase_cr, tim_dmabase_CR2, tim_dmabase_smcr,
//  *          tim_dmabase_dier, TIM1_dmabase_sr, tim_dmabase_egr,
//  *          tim_dmabase_CCMR1, tim_dmabase_ccmr2, tim_dmabase_CCER,
//  *          tim_dmabase_cnt, tim_dmabase_psc, tim_dmabase_arr,
//  *          tim_dmabase_rcr, tim_dmabase_CCR1, tim_dmabase_CCR2,
//  *          tim_dmabase_ccr3, tim_dmabase_ccr4, tim_dmabase_bdtr,
//  *          tim_dmabase_dcr.
//  * @param  tim_dmaburstlength: dma burst length.
//  *   this parameter can be one value between:
//  *   tim_dmaburstlength_1transfer and tim_dmaburstlength_18transfers.
//  * @retval none
//  */
//void tim_dmaconfig(tim_typedef* TIMx, uint16_t tim_dmabase, uint16_t tim_dmaburstlength)
//{
//  /* check the parameters */
//  assert_param(is_tim_list4_periph(TIMx));
//  assert_param(is_tim_dma_base(tim_dmabase));
//  assert_param(is_tim_dma_length(tim_dmaburstlength));
//  /* set the dma base and the dma burst length */
//  TIMx->dcr = tim_dmabase | tim_dmaburstlength;
//}

/**
  * @brief  enables or DISABLEs the TIMx's dma requests.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 6, 7, 8, 15, 16 or 17
  *   to select the tim peripheral.
  * @param  tim_dmasource: specifies the dma request sources.
  *   this parameter can be any combination of the following values:
  *     @arg tim_dma_update: tim update interrupt source
  *     @arg tim_dma_cc1: tim capture compare 1 dma source
  *     @arg tim_dma_cc2: tim capture compare 2 dma source
  *     @arg tim_dma_cc3: tim capture compare 3 dma source
  *     @arg tim_dma_cc4: tim capture compare 4 dma source
  *     @arg tim_dma_com: tim commutation dma source
  *     @arg tim_dma_trigger: tim trigger dma source
  * @param  newstate: new state of the dma request sources.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
//void tim_dmacmd(tim_typedef* TIMx, uint16_t tim_dmasource, FUNCTIONAL_STATE newstate)
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST9_PERIPH(TIMx));
//  assert_param(IS_TIM_DMA_SOURCE(tim_dmasource));
//  assert_param(IS_FUNCTIONAL_STATE(newstate));
//
//  if (newstate != DISABLE)
//  {
//    /* enable the dma sources */
//    TIMx->dier |= tim_dmasource;
//  }
//  else
//  {
//    /* DISABLE the dma sources */
//    TIMx->dier &= (uint16_t)~tim_dmasource;
//  }
//}

/**
  * @brief  configures the TIMx internal clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15
  *         to select the tim peripheral.
  * @retval none
  */
AT(.com_periph.tim.internalclock)
void tim_internalclockconfig(tim_typedef* TIMx)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* DISABLE slave mode to clock the prescaler directly with the internal clock */
  TIMx->smcr &=  (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
}

/**
  * @brief  configures the TIMx internal trigger as external clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_itrsource: trigger source.
  *   this parameter can be one of the following values:
  * @param  tim_ts_itr0: internal trigger 0
  * @param  tim_ts_itr1: internal trigger 1
  * @param  tim_ts_itr2: internal trigger 2
  * @param  tim_ts_itr3: internal trigger 3
  * @retval none
  */
AT(.com_periph.tim.itrxexternalclock)
void tim_itrxexternalclockconfig(tim_typedef* TIMx, uint16_t tim_inputtriggersource)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_INTERNAL_TRIGGER_SELECTION(tim_inputtriggersource));
  /* select the internal trigger */
  tim_selectinputtrigger(TIMx, tim_inputtriggersource);
  /* select the external clock mode1 */
  TIMx->smcr |= TIM_SLAVEMODE_EXTERNAL1;
}

/**
  * @brief  configures the TIMx trigger as external clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_tixexternalclksource: trigger source.
  *   this parameter can be one of the following values:
  *     @arg tim_tixexternalclk1source_ti1ed: ti1 edge detector
  *     @arg tim_tixexternalclk1source_ti1: filtered timer input 1
  *     @arg TIM_TIXEXTERNALCLK1SOURCE_TI2: filtered timer input 2
  * @param  tim_icpolarity: specifies the tix polarity.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_RISING
  *     @arg TIM_ICPOLARITY_FALLING
  * @param  icfilter : specifies the filter value.
  *   this parameter must be a value between 0x0 and 0xf.
  * @retval none
  */
AT(.com_periph.tim.tixexternalclock)
void tim_tixexternalclockconfig(tim_typedef* TIMx, uint16_t tim_tixexternalclksource,
                                uint16_t tim_icpolarity, uint16_t icfilter)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_TIXCLK_SOURCE(tim_tixexternalclksource));
  assert_param(IS_TIM_IC_POLARITY(tim_icpolarity));
  assert_param(IS_TIM_IC_FILTER(icfilter));
  /* configure the timer input clock source */
  if (tim_tixexternalclksource == TIM_TIXEXTERNALCLK1SOURCE_TI2)
  {
    ti2_config(TIMx, tim_icpolarity, TIM_ICSELECTION_DIRECTTI, icfilter);
  }
  else
  {
    ti1_config(TIMx, tim_icpolarity, TIM_ICSELECTION_DIRECTTI, icfilter);
  }
  /* select the trigger source */
  tim_selectinputtrigger(TIMx, tim_tixexternalclksource);
  /* select the external clock mode1 */
  TIMx->smcr |= TIM_SLAVEMODE_EXTERNAL1;
}

/**
  * @brief  configures the external clock mode1
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_exttrgprescaler: the external trigger prescaler.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpsc_off: etrp prescaler off.
  *     @arg tim_exttrgpsc_div2: etrp frequency divided by 2.
  *     @arg tim_exttrgpsc_div4: etrp frequency divided by 4.
  *     @arg tim_exttrgpsc_div8: etrp frequency divided by 8.
  * @param  tim_exttrgpolarity: the external trigger polarity.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpolarity_inverted: active low or falling edge active.
  *     @arg tim_exttrgpolarity_noninverted: active high or rising edge active.
  * @param  exttrgfilter: external trigger filter.
  *   this parameter must be a value between 0x00 and 0x0f
  * @retval none
  */
AT(.com_periph.etrclockmode1)
void tim_etrclockmode1config(tim_typedef* TIMx, uint16_t tim_exttrgprescaler, uint16_t tim_exttrgpolarity,
                             uint16_t exttrgfilter)
{
  uint16_t tmpsmcr = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(tim_exttrgprescaler));
  assert_param(IS_TIM_EXT_POLARITY(tim_exttrgpolarity));
  assert_param(IS_TIM_EXT_FILTER(exttrgfilter));
  /* configure the etr clock source */
  tim_etrconfig(TIMx, tim_exttrgprescaler, tim_exttrgpolarity, exttrgfilter);

  /* get the TIMx smcr register value */
  tmpsmcr = TIMx->smcr;
  /* reset the sms bits */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
  /* select the external clock mode1 */
  tmpsmcr |= TIM_SLAVEMODE_EXTERNAL1;
  /* select the trigger selection : etrf */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_TS));
  tmpsmcr |= TIM_TS_ETRF;
  /* write to TIMx smcr */
  TIMx->smcr = tmpsmcr;
}

/**
  * @brief  configures the external clock mode2
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_exttrgprescaler: the external trigger prescaler.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpsc_off: etrp prescaler off.
  *     @arg tim_exttrgpsc_div2: etrp frequency divided by 2.
  *     @arg tim_exttrgpsc_div4: etrp frequency divided by 4.
  *     @arg tim_exttrgpsc_div8: etrp frequency divided by 8.
  * @param  tim_exttrgpolarity: the external trigger polarity.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpolarity_inverted: active low or falling edge active.
  *     @arg tim_exttrgpolarity_noninverted: active high or rising edge active.
  * @param  exttrgfilter: external trigger filter.
  *   this parameter must be a value between 0x00 and 0x0f
  * @retval none
  */
AT(.com_periph.tim.etrclockmode2)
void tim_etrclockmode2config(tim_typedef* TIMx, uint16_t tim_exttrgprescaler,
                             uint16_t tim_exttrgpolarity, uint16_t exttrgfilter)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(tim_exttrgprescaler));
  assert_param(IS_TIM_EXT_POLARITY(tim_exttrgpolarity));
  assert_param(IS_TIM_EXT_FILTER(exttrgfilter));
  /* configure the etr clock source */
  tim_etrconfig(TIMx, tim_exttrgprescaler, tim_exttrgpolarity, exttrgfilter);
  /* enable the external clock mode2 */
  TIMx->smcr |= TIM_SMCR_ECE;
}

/**
  * @brief  configures the TIMx external trigger (etr).
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_exttrgprescaler: the external trigger prescaler.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpsc_off: etrp prescaler off.
  *     @arg tim_exttrgpsc_div2: etrp frequency divided by 2.
  *     @arg tim_exttrgpsc_div4: etrp frequency divided by 4.
  *     @arg tim_exttrgpsc_div8: etrp frequency divided by 8.
  * @param  tim_exttrgpolarity: the external trigger polarity.
  *   this parameter can be one of the following values:
  *     @arg tim_exttrgpolarity_inverted: active low or falling edge active.
  *     @arg tim_exttrgpolarity_noninverted: active high or rising edge active.
  * @param  exttrgfilter: external trigger filter.
  *   this parameter must be a value between 0x00 and 0x0f
  * @retval none
  */
AT(.com_periph.tim.etrconfig)
void tim_etrconfig(tim_typedef* TIMx, uint16_t tim_exttrgprescaler, uint16_t tim_exttrgpolarity,
                   uint16_t exttrgfilter)
{
  uint16_t tmpsmcr = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(tim_exttrgprescaler));
  assert_param(IS_TIM_EXT_POLARITY(tim_exttrgpolarity));
  assert_param(IS_TIM_EXT_FILTER(exttrgfilter));
  tmpsmcr = TIMx->smcr;
  /* reset the etr bits */
  tmpsmcr &= SMCR_ETR_MASK;
  /* set the PRESCALER, the filter value and the polarity */
  tmpsmcr |= (uint16_t)(tim_exttrgprescaler | (uint16_t)(tim_exttrgpolarity | (uint16_t)(exttrgfilter << (uint16_t)8)));
  /* write to TIMx smcr */
  TIMx->smcr = tmpsmcr;
}

/**
  * @brief  configures the TIMx prescaler.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  prescaler: specifies the prescaler register value
  * @param  tim_pscreloadmode: specifies the tim prescaler reload mode
  *   this parameter can be one of the following values:
  *     @arg tim_pscreloadmode_update: the prescaler is loaded at the update event.
  *     @arg TIM_PSCRELOADMODE_IMMEDIATE: the prescaler is loaded immediately.
  * @retval none
  */
AT(.com_periph.tim.prescaler)
void tim_prescalerconfig(tim_typedef* TIMx, uint16_t prescaler, uint16_t tim_pscreloadmode)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_PRESCALER_RELOAD(tim_pscreloadmode));
  /* set the prescaler value */
  TIMx->psc = prescaler;
  /* set or reset the ug bit */
  TIMx->egr = tim_pscreloadmode;
}

/**
  * @brief  specifies the TIMx counter mode to be used.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_countermode: specifies the counter mode to be used
  *   this parameter can be one of the following values:
  *     @arg TIM_COUNTERMODE_UP: tim up counting mode
  *     @arg tim_countermode_down: tim down counting mode
  *     @arg tim_countermode_centeraligned1: tim center aligned mode1
  *     @arg tim_countermode_centeraligned2: tim center aligned mode2
  *     @arg tim_countermode_centeraligned3: tim center aligned mode3
  * @retval none
  */
AT(.com_periph.tim.countermode)
void tim_countermodeconfig(tim_typedef* TIMx, uint16_t tim_countermode)
{
  uint16_t tmpcr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_COUNTER_MODE(tim_countermode));
  tmpcr1 = TIMx->cr1;
  /* reset the cms and dir bits */
  tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
  /* set the counter mode */
  tmpcr1 |= tim_countermode;
  /* write to TIMx cr1 register */
  TIMx->cr1 = tmpcr1;
}

/**
  * @brief  selects the input trigger source
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_inputtriggersource: the input trigger source.
  *   this parameter can be one of the following values:
  *     @arg tim_ts_itr0: internal trigger 0
  *     @arg tim_ts_itr1: internal trigger 1
  *     @arg tim_ts_itr2: internal trigger 2
  *     @arg tim_ts_itr3: internal trigger 3
  *     @arg tim_ts_ti1f_ed: ti1 edge detector
  *     @arg tim_ts_ti1fp1: filtered timer input 1
  *     @arg tim_ts_ti2fp2: filtered timer input 2
  *     @arg TIM_TS_ETRF: external trigger input
  * @retval none
  */
AT(.com_periph.tim.selectinputtrigger)
void tim_selectinputtrigger(tim_typedef* TIMx, uint16_t tim_inputtriggersource)
{
  uint16_t tmpsmcr = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_TRIGGER_SELECTION(tim_inputtriggersource));
  /* get the TIMx smcr register value */
  tmpsmcr = TIMx->smcr;
  /* reset the ts bits */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_TS));
  /* set the input trigger source */
  tmpsmcr |= tim_inputtriggersource;
  /* write to TIMx smcr */
  TIMx->smcr = tmpsmcr;
}

/**
  * @brief  configures the TIMx encoder interface.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_encodermode: specifies the TIMx encoder mode.
  *   this parameter can be one of the following values:
  *     @arg tim_encodermode_ti1: counter counts on ti1fp1 edge depending on ti2fp2 level.
  *     @arg tim_encodermode_ti2: counter counts on ti2fp2 edge depending on ti1fp1 level.
  *     @arg tim_encodermode_ti12: counter counts on both ti1fp1 and ti2fp2 edges depending
  *                                on the level of the other input.
  * @param  tim_ic1polarity: specifies the ic1 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_FALLING: ic falling edge.
  *     @arg TIM_ICPOLARITY_RISING: ic rising edge.
  * @param  tim_ic2polarity: specifies the ic2 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_FALLING: ic falling edge.
  *     @arg TIM_ICPOLARITY_RISING: ic rising edge.
  * @retval none
  */
void tim_encoderinterfaceconfig(tim_typedef* TIMx, uint16_t tim_encodermode,
                                uint16_t tim_ic1polarity, uint16_t tim_ic2polarity)
{
  uint16_t tmpsmcr = 0;
  uint16_t tmpccmr1 = 0;
  uint16_t tmpccer = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));
  assert_param(IS_TIM_ENCODER_MODE(tim_encodermode));
  assert_param(IS_TIM_IC_POLARITY(tim_ic1polarity));
  assert_param(IS_TIM_IC_POLARITY(tim_ic2polarity));

  /* get the TIMx smcr register value */
  tmpsmcr = TIMx->smcr;

  /* get the TIMx ccmr1 register value */
  tmpccmr1 = TIMx->ccmr1;

  /* get the TIMx ccer register value */
  tmpccer = TIMx->ccer;

  /* set the encoder mode */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
  tmpsmcr |= tim_encodermode;

  /* select the capture compare 1 and the capture compare 2 as input */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC1S)) & (uint16_t)(~((uint16_t)TIM_CCMR1_CC2S))); // 预留 ■
  tmpccmr1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // 预留 ■

  /* set the ti1 and the ti2 polarities */
  tmpccer &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCER_CC1P)) & ((uint16_t)~((uint16_t)TIM_CCER_CC2P)));
  tmpccer |= (uint16_t)(tim_ic1polarity | (uint16_t)(tim_ic2polarity << (uint16_t)4));

  /* write to TIMx smcr */
  TIMx->smcr = tmpsmcr;
  /* write to TIMx ccmr1 */
  TIMx->ccmr1 = tmpccmr1;
  /* write to TIMx ccer */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  forces the TIMx output 1 waveform to active or inactive level.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_forcedaction: specifies the forced action to be set to the output waveform.
  *   this parameter can be one of the following values:
  *     @arg tim_forcedaction_active: force active level on oc1ref
  *     @arg tim_forcedaction_inactive: force inactive level on oc1ref.
  * @retval none
  */
AT(.com_periph.tim.forcedoc1)
void tim_forcedoc1config(tim_typedef* TIMx, uint16_t tim_forcedaction)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(is_tim_list8_periph(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(tim_forcedaction));
  tmpccmr1 = TIMx->ccmr1;
  /* reset the oc1m bits */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1M);
  /* configure the forced output mode */
  tmpccmr1 |= tim_forcedaction;
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  forces the TIMx output 2 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_forcedaction: specifies the forced action to be set to the output waveform.
  *   this parameter can be one of the following values:
  *     @arg tim_forcedaction_active: force active level on oc2ref
  *     @arg tim_forcedaction_inactive: force inactive level on oc2ref.
  * @retval none
  */
AT(.com_periph.tim.forcedoc2)
void tim_forcedoc2config(tim_typedef* TIMx, uint16_t tim_forcedaction)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(tim_forcedaction));
  tmpccmr1 = TIMx->ccmr1;
  /* reset the oc2m bits */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2M);
  /* configure the forced output mode */
  tmpccmr1 |= (uint16_t)(tim_forcedaction << 8);
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  forces the TIMx output 3 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_forcedaction: specifies the forced action to be set to the output waveform.
  *   this parameter can be one of the following values:
  *     @arg tim_forcedaction_active: force active level on oc3ref
  *     @arg tim_forcedaction_inactive: force inactive level on oc3ref.
  * @retval none
  */
AT(.com_periph.tim.forcedoc3)
void tim_forcedoc3config(tim_typedef* TIMx, uint16_t tim_forcedaction)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(tim_forcedaction));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc1m bits */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3M);
  /* configure the forced output mode */
  tmpccmr2 |= tim_forcedaction;
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  forces the TIMx output 4 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_forcedaction: specifies the forced action to be set to the output waveform.
  *   this parameter can be one of the following values:
  *     @arg tim_forcedaction_active: force active level on oc4ref
  *     @arg tim_forcedaction_inactive: force inactive level on oc4ref.
  * @retval none
  */
AT(.com_periph.tim.forcedoc4)
void tim_forcedoc4config(tim_typedef* TIMx, uint16_t tim_forcedaction)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(tim_forcedaction));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc2m bits */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4M);
  /* configure the forced output mode */
  tmpccmr2 |= (uint16_t)(tim_forcedaction << 8);
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  enables or DISABLEs TIMx peripheral preload register on arr.
  * @param  TIMx: where x can be  1 to 17 to select the tim peripheral.
  * @param  newstate: new state of the TIMx peripheral preload register
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_periph.tim.arrpreload)
void tim_arrpreloadconfig(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(is_FUNCTIONAL_STATE(newstate));
  if (newstate != DISABLE)
  {
    /* set the arr preload bit */
    TIMx->cr1 |= TIM_CR1_ARPE;
  }
  else
  {
    /* reset the arr preload bit */
    TIMx->cr1 &= (uint16_t)~((uint16_t)TIM_CR1_ARPE);
  }
}

/**
  * @brief  selects the tim peripheral commutation event.
  * @param  TIMx: where x can be  1, 8, 15, 16 or 17 to select the TIMx peripheral
  * @param  newstate: new state of the commutation event.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_periph.tim.selectcom)
void tim_selectcom(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(is_FUNCTIONAL_STATE(newstate));
  if (newstate != DISABLE)
  {
    /* set the com bit */
    TIMx->cr2 |= TIM_CR2_CCUS;
  }
  else
  {
    /* reset the com bit */
    TIMx->cr2 &= (uint16_t)~((uint16_t)TIM_CR2_CCUS);
  }
}

/**
  * @brief  selects the TIMx peripheral capture compare dma source.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 15, 16 or 17 to select
  *         the tim peripheral.
  * @param  newstate: new state of the capture compare dma source
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
//void tim_selectccdma(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)预留
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST4_PERIPH(TIMx));
//  assert_param(is_FUNCTIONAL_STATE(newstate));
//  if (newstate != DISABLE)
//  {
//    /* set the ccds bit */
//    TIMx->cr2 |= TIM_CR2_CCDS; // 预留 ■
//  }
//  else
//  {
//    /* reset the ccds bit */
//    TIMx->cr2 &= (uint16_t)~((uint16_t)TIM_CR2_CCDS); // 预留 ■
//  }
//}

/**
  * @brief  sets or resets the tim peripheral capture compare preload control bit.
  * @param  TIMx: where x can be   1, 2, 3, 4, 5, 8 or 15
  *         to select the TIMx peripheral
  * @param  newstate: new state of the capture compare preload control bit
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_periph.tim.ccpreloadcontrol)
void tim_ccpreloadcontrol(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));
  assert_param(is_FUNCTIONAL_STATE(newstate));
  if (newstate != DISABLE)
  {
    /* set the ccpc bit */
    TIMx->cr2 |= TIM_CR2_CCPC;
  }
  else
  {
    /* reset the ccpc bit */
    TIMx->cr2 &= (uint16_t)~((uint16_t)TIM_CR2_CCPC);
  }
}

/**
  * @brief  enables or DISABLEs the TIMx peripheral preload register on ccr1.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_ocpreload: new state of the TIMx peripheral preload register
  *   this parameter can be one of the following values:
  *     @arg tim_ocpreload_enable
  *     @arg tim_ocpreload_DISABLE
  * @retval none
  */
AT(.com_periph.tim.oc1preload)
void tim_oc1preloadconfig(tim_typedef* TIMx, uint16_t tim_ocpreload)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(tim_ocpreload));
  tmpccmr1 = TIMx->ccmr1;
  /* reset the oc1pe bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1PE);
  /* enable or DISABLE the output compare preload feature */
  tmpccmr1 |= tim_ocpreload;
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  enables or DISABLEs the TIMx peripheral preload register on ccr2.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the tim peripheral.
  * @param  tim_ocpreload: new state of the TIMx peripheral preload register
  *   this parameter can be one of the following values:
  *     @arg tim_ocpreload_enable
  *     @arg tim_ocpreload_DISABLE
  * @retval none
  */
AT(.com_periph.tim.oc2preload)
void tim_oc2preloadconfig(tim_typedef* TIMx, uint16_t tim_ocpreload)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(tim_ocpreload));
  tmpccmr1 = TIMx->ccmr1;
  /* reset the oc2pe bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2PE);
  /* enable or DISABLE the output compare preload feature */
  tmpccmr1 |= (uint16_t)(tim_ocpreload << 8);
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  enables or DISABLEs the TIMx peripheral preload register on ccr3.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocpreload: new state of the TIMx peripheral preload register
  *   this parameter can be one of the following values:
  *     @arg tim_ocpreload_enable
  *     @arg tim_ocpreload_DISABLE
  * @retval none
  */
AT(.com_periph.tim.oc3preload)
void tim_oc3preloadconfig(tim_typedef* TIMx, uint16_t tim_ocpreload)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  assert_param(IS_TIM_OCPRELOAD_STATE(tim_ocpreload));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc3pe bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3PE);
  /* enable or DISABLE the output compare preload feature */
  tmpccmr2 |= tim_ocpreload;
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  enables or DISABLEs the TIMx peripheral preload register on ccr4.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocpreload: new state of the TIMx peripheral preload register
  *   this parameter can be one of the following values:
  *     @arg tim_ocpreload_enable
  *     @arg tim_ocpreload_DISABLE
  * @retval none
  */
AT(.com_periph.tim.oc4preload)
void tim_oc4preloadconfig(tim_typedef* TIMx, uint16_t tim_ocpreload)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  assert_param(IS_TIM_OCPRELOAD_STATE(tim_ocpreload));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc4pe bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4PE);
  /* enable or DISABLE the output compare preload feature */
  tmpccmr2 |= (uint16_t)(tim_ocpreload << 8);
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  configures the TIMx output compare 1 fast feature.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_ocfast: new state of the output compare fast enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_ocfast_enable: tim output compare fast enable
  *     @arg tim_ocfast_DISABLE: tim output compare fast DISABLE
  * @retval none
  */
//void tim_oc1fastconfig(tim_typedef* TIMx, uint16_t tim_ocfast) 预留
//{
//  uint16_t tmpccmr1 = 0;
//  /* check the parameters */
//  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
//  assert_param(IS_TIM_OCFAST_STATE(tim_ocfast));
//  /* get the TIMx ccmr1 register value */
//  tmpccmr1 = TIMx->ccmr1;
//  /* reset the oc1fe bit */
//  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1FE); // 预留 ■
//  /* enable or DISABLE the output compare fast bit */
//  tmpccmr1 |= tim_ocfast;
//  /* write to TIMx ccmr1 */
//  TIMx->ccmr1 = tmpccmr1;
//}

/**
  * @brief  configures the TIMx output compare 2 fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the tim peripheral.
  * @param  tim_ocfast: new state of the output compare fast enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_ocfast_enable: tim output compare fast enable
  *     @arg tim_ocfast_DISABLE: tim output compare fast DISABLE
  * @retval none
  */
//void tim_oc2fastconfig(tim_typedef* TIMx, uint16_t tim_ocfast) 预留
//{
//  uint16_t tmpccmr1 = 0;
//  /* check the parameters */
////  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
////  assert_param(IS_TIM_OCFAST_STATE(tim_ocfast));
//  /* get the TIMx ccmr1 register value */
//  tmpccmr1 = TIMx->ccmr1;
//  /* reset the oc2fe bit */
//  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2FE); // 预留 ■
//  /* enable or DISABLE the output compare fast bit */
//  tmpccmr1 |= (uint16_t)(tim_ocfast << 8);
//  /* write to TIMx ccmr1 */
//  TIMx->ccmr1 = tmpccmr1;
//}

/**
  * @brief  configures the TIMx output compare 3 fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocfast: new state of the output compare fast enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_ocfast_enable: tim output compare fast enable
  *     @arg tim_ocfast_DISABLE: tim output compare fast DISABLE
  * @retval none
  */
//void tim_oc3fastconfig(tim_typedef* TIMx, uint16_t tim_ocfast)  预留
//{
//  uint16_t tmpccmr2 = 0;
//  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  assert_param(IS_TIM_OCFAST_STATE(tim_ocfast));
//  /* get the TIMx ccmr2 register value */
//  tmpccmr2 = TIMx->ccmr2;
//  /* reset the oc3fe bit */
//  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3FE); // 预留 ■
//  /* enable or DISABLE the output compare fast bit */
//  tmpccmr2 |= tim_ocfast;
//  /* write to TIMx ccmr2 */
//  TIMx->ccmr2 = tmpccmr2;
//}

/**
  * @brief  configures the TIMx output compare 4 fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocfast: new state of the output compare fast enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_ocfast_enable: tim output compare fast enable
  *     @arg tim_ocfast_DISABLE: tim output compare fast DISABLE
  * @retval none
  */
//void tim_oc4fastconfig(tim_typedef* TIMx, uint16_t tim_ocfast) 预留
//{
//  uint16_t tmpccmr2 = 0;
//  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  assert_param(IS_TIM_OCFAST_STATE(tim_ocfast));
//  /* get the TIMx ccmr2 register value */
//  tmpccmr2 = TIMx->ccmr2;
//  /* reset the oc4fe bit */
//  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4FE); // 预留 ■
//  /* enable or DISABLE the output compare fast bit */
//  tmpccmr2 |= (uint16_t)(tim_ocfast << 8);
//  /* write to TIMx ccmr2 */
//  TIMx->ccmr2 = tmpccmr2;
//}

/**
  * @brief  clears or safeguards the ocref1 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_occlear: new state of the output compare clear enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_occlear_enable: tim output clear enable
  *     @arg tim_occlear_DISABLE: tim output clear DISABLE
  * @retval none
  */
AT(.com_periph.tim.clearoc1)
void tim_clearoc1ref(tim_typedef* TIMx, uint16_t tim_occlear)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(tim_occlear));

  tmpccmr1 = TIMx->ccmr1;

  /* reset the oc1ce bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1CE);
  /* enable or DISABLE the output compare clear bit */
  tmpccmr1 |= tim_occlear;
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  clears or safeguards the ocref2 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_occlear: new state of the output compare clear enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_occlear_enable: tim output clear enable
  *     @arg tim_occlear_DISABLE: tim output clear DISABLE
  * @retval none
  */
AT(.com_periph.tim.clearoc2)
void tim_clearoc2ref(tim_typedef* TIMx, uint16_t tim_occlear)
{
  uint16_t tmpccmr1 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(tim_occlear));
  tmpccmr1 = TIMx->ccmr1;
  /* reset the oc2ce bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2CE);
  /* enable or DISABLE the output compare clear bit */
  tmpccmr1 |= (uint16_t)(tim_occlear << 8);
  /* write to TIMx ccmr1 register */
  TIMx->ccmr1 = tmpccmr1;
}

/**
  * @brief  clears or safeguards the ocref3 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_occlear: new state of the output compare clear enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_occlear_enable: tim output clear enable
  *     @arg tim_occlear_DISABLE: tim output clear DISABLE
  * @retval none
  */
AT(.com_periph.tim.clearoc3)
void tim_clearoc3ref(tim_typedef* TIMx, uint16_t tim_occlear)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(tim_occlear));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc3ce bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3CE);
  /* enable or DISABLE the output compare clear bit */
  tmpccmr2 |= tim_occlear;
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  clears or safeguards the ocref4 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_occlear: new state of the output compare clear enable bit.
  *   this parameter can be one of the following values:
  *     @arg tim_occlear_enable: tim output clear enable
  *     @arg tim_occlear_DISABLE: tim output clear DISABLE
  * @retval none
  */
AT(.com_periph.tim.clearoc4)
void tim_clearoc4ref(tim_typedef* TIMx, uint16_t tim_occlear)
{
  uint16_t tmpccmr2 = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(tim_occlear));
  tmpccmr2 = TIMx->ccmr2;
  /* reset the oc4ce bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4CE);
  /* enable or DISABLE the output compare clear bit */
  tmpccmr2 |= (uint16_t)(tim_occlear << 8);
  /* write to TIMx ccmr2 register */
  TIMx->ccmr2 = tmpccmr2;
}

/**
  * @brief  configures the TIMx channel 1 polarity.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_ocpolarity: specifies the oc1 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_OCPOLARITY_HIGH: output compare active high
  *     @arg tim_ocpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.tim.oc1polarity)
void tim_oc1polarityconfig(tim_typedef* TIMx, uint16_t tim_ocpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(tim_ocpolarity));
  tmpccer = TIMx->ccer;
  /* set or reset the cc1p bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC1P);
  tmpccer |= tim_ocpolarity;
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 1n polarity.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the tim peripheral.
  * @param  tim_ocnpolarity: specifies the oc1n polarity
  *   this parameter can be one of the following values:
  *     @arg tim_ocnpolarity_high: output compare active high
  *     @arg tim_ocnpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc1npolarity)
void tim_oc1npolarityconfig(tim_typedef* TIMx, uint16_t tim_ocnpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(tim_ocnpolarity));

  tmpccer = TIMx->ccer;
  /* set or reset the cc1np bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC1NP);
  tmpccer |= tim_ocnpolarity;
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 2 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_ocpolarity: specifies the oc2 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_OCPOLARITY_HIGH: output compare active high
  *     @arg tim_ocpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc2polarity)
void tim_oc2polarityconfig(tim_typedef* TIMx, uint16_t tim_ocpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(tim_ocpolarity));
  tmpccer = TIMx->ccer;
  /* set or reset the cc2p bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC2P);
  tmpccer |= (uint16_t)(tim_ocpolarity << 4);
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 2n polarity.
  * @param  TIMx: where x can be 1 or 8 to select the tim peripheral.
  * @param  tim_ocnpolarity: specifies the oc2n polarity
  *   this parameter can be one of the following values:
  *     @arg tim_ocnpolarity_high: output compare active high
  *     @arg tim_ocnpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc2npolarity)
void tim_oc2npolarityconfig(tim_typedef* TIMx, uint16_t tim_ocnpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(tim_ocnpolarity));

  tmpccer = TIMx->ccer;
  /* set or reset the cc2np bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC2NP);
  tmpccer |= (uint16_t)(tim_ocnpolarity << 4);
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 3 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocpolarity: specifies the oc3 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_OCPOLARITY_HIGH: output compare active high
  *     @arg tim_ocpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc3polarity)
void tim_oc3polarityconfig(tim_typedef* TIMx, uint16_t tim_ocpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(tim_ocpolarity));
  tmpccer = TIMx->ccer;
  /* set or reset the cc3p bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC3P);
  tmpccer |= (uint16_t)(tim_ocpolarity << 8);
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 3n polarity.
  * @param  TIMx: where x can be 1 or 8 to select the tim peripheral.
  * @param  tim_ocnpolarity: specifies the oc3n polarity
  *   this parameter can be one of the following values:
  *     @arg tim_ocnpolarity_high: output compare active high
  *     @arg tim_ocnpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc3npolarity)
void tim_oc3npolarityconfig(tim_typedef* TIMx, uint16_t tim_ocnpolarity)
{
  uint16_t tmpccer = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(tim_ocnpolarity));

  tmpccer = TIMx->ccer;
  /* set or reset the cc3np bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC3NP);
  tmpccer |= (uint16_t)(tim_ocnpolarity << 8);
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configures the TIMx channel 4 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_ocpolarity: specifies the oc4 polarity
  *   this parameter can be one of the following values:
  *     @arg TIM_OCPOLARITY_HIGH: output compare active high
  *     @arg tim_ocpolarity_low: output compare active low
  * @retval none
  */
AT(.com_periph.oc4polarity)
void tim_oc4polarityconfig(tim_typedef* TIMx, uint16_t tim_ocpolarity)
{
  uint16_t tmpccer = 0;
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(tim_ocpolarity));
  tmpccer = TIMx->ccer;
  /* set or reset the cc4p bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC4P);
  tmpccer |= (uint16_t)(tim_ocpolarity << 12);
  /* write to TIMx ccer register */
  TIMx->ccer = tmpccer;
}

/**
  * @brief  enables or DISABLEs the tim capture compare channel x.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_channel: specifies the tim channel
  *   this parameter can be one of the following values:
  *     @arg TIM_CHANNEL_1: tim channel 1
  *     @arg TIM_CHANNEL_2: tim channel 2
  *     @arg TIM_CHANNEL_3: tim channel 3
  *     @arg TIM_CHANNEL_4: tim channel 4
  * @param  tim_ccx: specifies the tim channel ccxe bit new state.
  *   this parameter can be: tim_ccx_enable or tim_ccx_DISABLE.
  * @retval none
  */
AT(.com_periph.ccxcmd)
void tim_ccxcmd(tim_typedef* TIMx, uint16_t tim_channel, uint16_t tim_ccx)
{
  uint16_t tmp = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CHANNEL(tim_channel));
  assert_param(IS_TIM_CCX(tim_ccx));

  tmp = CCER_CCE_SET << tim_channel;

  /* reset the ccxe bit */
  TIMx->ccer &= (uint16_t)~ tmp;

  /* set or reset the ccxe bit */
  TIMx->ccer |=  (uint16_t)(tim_ccx << tim_channel);
}

/**
  * @brief  enables or DISABLEs the tim capture compare channel xn.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the tim peripheral.
  * @param  tim_channel: specifies the tim channel
  *   this parameter can be one of the following values:
  *     @arg TIM_CHANNEL_1: tim channel 1
  *     @arg TIM_CHANNEL_2: tim channel 2
  *     @arg TIM_CHANNEL_3: tim channel 3
  * @param  tim_ccxn: specifies the tim channel ccxne bit new state.
  *   this parameter can be: tim_ccxn_enable or tim_ccxn_DISABLE.
  * @retval none
  */
AT(.com_periph.ccxncmd)
void tim_ccxncmd(tim_typedef* TIMx, uint16_t tim_channel, uint16_t tim_ccxn)
{
  uint16_t tmp = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_COMPLEMENTARY_CHANNEL(tim_channel));
  assert_param(IS_TIM_CCXN(tim_ccxn));

  tmp = CCER_CCNE_SET << tim_channel;

  /* reset the ccxne bit */
  TIMx->ccer &= (uint16_t) ~tmp;

  /* set or reset the ccxne bit */
  TIMx->ccer |=  (uint16_t)(tim_ccxn << tim_channel);
}

/**
  * @brief  selects the tim output compare mode.
  * @note   this function DISABLEs the selected channel before changing the output
  *         compare mode.
  *         user has to enable this channel using tim_ccxcmd and tim_ccxncmd functions.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_channel: specifies the tim channel
  *   this parameter can be one of the following values:
  *     @arg TIM_CHANNEL_1: tim channel 1
  *     @arg TIM_CHANNEL_2: tim channel 2
  *     @arg TIM_CHANNEL_3: tim channel 3
  *     @arg TIM_CHANNEL_4: tim channel 4
  * @param  tim_ocmode: specifies the tim output compare mode.
  *   this parameter can be one of the following values:
  *     @arg TIM_OCMODE_TIMING
  *     @arg tim_ocmode_active
  *     @arg tim_ocmode_toggle
  *     @arg tim_ocmode_low
  *     @arg tim_ocmode_high
  *     @arg tim_ocmode_pwm1
  *     @arg tim_ocmode_pwm2
  *     @arg tim_forcedaction_active
  *     @arg tim_forcedaction_inactive
  * @retval none
  */
AT(.com_periph.selectocxm)
void tim_selectocxm(tim_typedef* TIMx, uint16_t tim_channel, uint16_t tim_ocmode)
{
  uint32_t tmp = 0;
  uint16_t tmp1 = 0;

  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CHANNEL(tim_channel));
  assert_param(IS_TIM_OCM(tim_ocmode));

  tmp = (uint32_t) TIMx;
  tmp += CCMR_OFFSET;

  tmp1 = CCER_CCE_SET << (uint16_t)tim_channel;

  /* DISABLE the channel: reset the ccxe bit */
  TIMx->ccer &= (uint16_t) ~tmp1;

  if((tim_channel == TIM_CHANNEL_1) ||(tim_channel == TIM_CHANNEL_3))
  {
    tmp += (tim_channel>>1);

    /* reset the ocxm bits in the ccmrx register */
    *(__IO uint32_t *) tmp &= (uint32_t)~((uint32_t)TIM_CCMR1_OC1M);

    /* configure the ocxm bits in the ccmrx register */
    *(__IO uint32_t *) tmp |= tim_ocmode;
  }
  else
  {
    tmp += (uint16_t)(tim_channel - (uint16_t)4)>> (uint16_t)1;

    /* reset the ocxm bits in the ccmrx register */
    *(__IO uint32_t *) tmp &= (uint32_t)~((uint32_t)TIM_CCMR1_OC2M);

    /* configure the ocxm bits in the ccmrx register */
    *(__IO uint32_t *) tmp |= (uint16_t)(tim_ocmode << 8);
  }
}

/**
  * @brief  enables or DISABLEs the TIMx update event.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  newstate: new state of the TIMx udis bit
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_periph.tim.updatedisable)
void tim_updatedisableconfig(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(newstate));
  if (newstate != DISABLE)
  {
    /* set the update DISABLE bit */
    TIMx->cr1 |= TIM_CR1_UDIS;
  }
  else
  {
    /* reset the update DISABLE bit */
    TIMx->cr1 &= (uint16_t)~((uint16_t)TIM_CR1_UDIS);
  }
}

/**
  * @brief  configures the TIMx update request interrupt source.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_updatesource: specifies the update source.
  *   this parameter can be one of the following values:
  *     @arg tim_updatesource_regular: source of update is the counter overflow/underflow
                                       or the setting of ug bit, or an update generation
                                       through the slave mode controller.
  *     @arg tim_updatesource_global: source of update is counter overflow/underflow.
  * @retval none
  */
AT(.com_periph.tim.updaterequest)
void tim_updaterequestconfig(tim_typedef* TIMx, uint16_t tim_updatesource)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_UPDATE_SOURCE(tim_updatesource));
  if (tim_updatesource != TIM_UPDATESOURCE_GLOBAL)
  {
    /* set the urs bit */
    TIMx->cr1 |= TIM_CR1_URS;
  }
  else
  {
    /* reset the urs bit */
    TIMx->cr1 &= (uint16_t)~((uint16_t)TIM_CR1_URS);
  }
}

/**
  * @brief  enables or DISABLEs the TIMx's hall sensor interface.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  newstate: new state of the TIMx hall sensor interface.
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
//void tim_selecthallsensor(tim_typedef* TIMx, FUNCTIONAL_STATE newstate) 预留
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
//  assert_param(IS_FUNCTIONAL_STATE(newstate));
//  if (newstate != DISABLE)
//  {
//    /* set the ti1s bit */
//    TIMx->cr2 |= TIM_CR2_TI1S; // 预留 ■
//  }
//  else
//  {
//    /* reset the ti1s bit */
//   TIMx->cr2 &= (uint16_t)~((uint16_t)TIM_CR2_TI1S); // 预留 ■
//  }
//}

/**
  * @brief  selects the TIMx's one pulse mode.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_opmode: specifies the opm mode to be used.
  *   this parameter can be one of the following values:
  *     @arg tim_opmode_single
  *     @arg tim_opmode_repetitive
  * @retval none
  */
AT(.com_periph.selectonepulsemode)
void tim_selectonepulsemode(tim_typedef* TIMx, uint16_t tim_opmode)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_OPM_MODE(tim_opmode));
  /* reset the opm bit */
  TIMx->cr1 &= (uint16_t)~((uint16_t)TIM_CR1_OPM);
  /* configure the opm mode */
  TIMx->cr1 |= tim_opmode;
}

/**
  * @brief  selects the TIMx trigger output mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 6, 7, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_trgosource: specifies the trigger output source.
  *   this paramter can be one of the following values:
  *
  *  - for all TIMx
  *     @arg tim_trgosource_reset:  the ug bit in the tim_egr register is used as the trigger output (trgo).
  *     @arg tim_trgosource_enable: the counter enable cen is used as the trigger output (trgo).
  *     @arg tim_trgosource_update: the update event is selected as the trigger output (trgo).
  *
  *  - for all TIMx except tim6 and tim7
  *     @arg tim_trgosource_oc1: the trigger output sends a positive pulse when the cc1if flag
  *                              is to be set, as soon as a capture or compare match occurs (trgo).
  *     @arg tim_trgosource_oc1ref: oc1ref signal is used as the trigger output (trgo).
  *     @arg tim_trgosource_oc2ref: oc2ref signal is used as the trigger output (trgo).
  *     @arg tim_trgosource_oc3ref: oc3ref signal is used as the trigger output (trgo).
  *     @arg tim_trgosource_oc4ref: oc4ref signal is used as the trigger output (trgo).
  *
  * @retval none
  */
AT(.com_periph.tim.selectoutputtrigger)
void tim_selectoutputtrigger(tim_typedef* TIMx, uint16_t tim_trgosource)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST7_PERIPH(TIMx));
  assert_param(IS_TIM_TRGO_SOURCE(tim_trgosource));
  /* reset the mms bits */
  TIMx->cr2 &= (uint16_t)~((uint16_t)TIM_CR2_MMS);
  /* select the trgo source */
  TIMx->cr2 |=  tim_trgosource;
}

/**
  * @brief  selects the TIMx slave mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_slavemode: specifies the timer slave mode.
  *   this parameter can be one of the following values:
  *     @arg tim_slavemode_reset: rising edge of the selected trigger signal (trgi) re-initializes
  *                               the counter and triggers an update of the registers.
  *     @arg tim_slavemode_gated:     the counter clock is enabled when the trigger signal (trgi) is high.
  *     @arg tim_slavemode_trigger:   the counter starts at a rising edge of the trigger trgi.
  *     @arg TIM_SLAVEMODE_EXTERNAL1: rising edges of the selected trigger (trgi) clock the counter.
  * @retval none
  */
AT(.com_periph.tim.selectslavemode)
void tim_selectslavemode(tim_typedef* TIMx, uint16_t tim_slavemode)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_SLAVE_MODE(tim_slavemode));
 /* reset the sms bits */
  TIMx->smcr &= (uint16_t)~((uint16_t)TIM_SMCR_SMS);
  /* select the slave mode */
  TIMx->smcr |= tim_slavemode;
}

/**
  * @brief  sets or resets the TIMx master/slave mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_masterslavemode: specifies the timer master slave mode.
  *   this parameter can be one of the following values:
  *     @arg tim_masterslavemode_enable: synchronization between the current timer
  *                                      and its slaves (through trgo).
  *     @arg tim_masterslavemode_DISABLE: no action
  * @retval none
  */
//void tim_selectmasterslavemode(tim_typedef* TIMx, uint16_t tim_masterslavemode) 预留
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
//  assert_param(IS_TIM_MSM_STATE(tim_masterslavemode));
//  /* reset the msm bit */
//  TIMx->smcr &= (uint16_t)~((uint16_t)TIM_SMCR_MSM); // 预留 ■
//
//  /* set or reset the msm bit */
//  TIMx->smcr |= tim_masterslavemode;
//}

/**
  * @brief  sets the TIMx counter register value
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  counter: specifies the counter register new value.
  * @retval none
  */
AT(.com_periph.tim.setcounter)
void tim_setcounter(tim_typedef* TIMx, uint16_t counter)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* set the counter register value */
  TIMx->cnt = counter;
}

/**
  * @brief  sets the TIMx autoreload register value
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  autoreload: specifies the autoreload register new value.
  * @retval none
  */
AT(.com_periph.tim.setautoreload)
void tim_setautoreload(tim_typedef* TIMx, uint16_t autoreload)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* set the autoreload register value */
  TIMx->arr = autoreload;
}

/**
  * @brief  sets the TIMx capture compare1 register value
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  compare1: specifies the capture compare1 register new value.
  * @retval none
  */
AT(.com_periph.setcompare1)
void tim_setcompare1(tim_typedef* TIMx, uint16_t compare1)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  /* set the capture compare1 register value */
  TIMx->ccr1 = compare1;
}

/**
  * @brief  sets the TIMx capture compare2 register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  compare2: specifies the capture compare2 register new value.
  * @retval none
  */
AT(.com_periph.setcompare2)
void tim_setcompare2(tim_typedef* TIMx, uint16_t compare2)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* set the capture compare2 register value */
  TIMx->ccr2 = compare2;
}

/**
  * @brief  sets the TIMx capture compare3 register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  compare3: specifies the capture compare3 register new value.
  * @retval none
  */
AT(.com_periph.setcompare3)
void tim_setcompare3(tim_typedef* TIMx, uint16_t compare3)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* set the capture compare3 register value */
  TIMx->ccr3 = compare3;
}

/**
  * @brief  sets the TIMx capture compare4 register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  compare4: specifies the capture compare4 register new value.
  * @retval none
  */
AT(.com_periph.setcompare4)
void tim_setcompare4(tim_typedef* TIMx, uint16_t compare4)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* set the capture compare4 register value */
  TIMx->ccr4 = compare4;
}

/**
  * @brief  sets the TIMx input capture 1 prescaler.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_icpsc: specifies the input capture1 prescaler new value.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg tim_icpsc_div2: capture is done once every 2 events
  *     @arg tim_icpsc_div4: capture is done once every 4 events
  *     @arg tim_icpsc_div8: capture is done once every 8 events
  * @retval none
  */
AT(.com_periph.setic1prescaler)
void tim_setic1prescaler(tim_typedef* TIMx, uint16_t tim_icpsc)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(tim_icpsc));
  /* reset the ic1psc bits */
  TIMx->ccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_IC1PSC); // 预留 ■
  /* set the ic1psc value */
  TIMx->ccmr1 |= tim_icpsc;
}

/**
  * @brief  sets the TIMx input capture 2 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_icpsc: specifies the input capture2 prescaler new value.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg tim_icpsc_div2: capture is done once every 2 events
  *     @arg tim_icpsc_div4: capture is done once every 4 events
  *     @arg tim_icpsc_div8: capture is done once every 8 events
  * @retval none
  */
AT(.com_periph.setic2prescaler)
void tim_setic2prescaler(tim_typedef* TIMx, uint16_t tim_icpsc)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(tim_icpsc));
  /* reset the ic2psc bits */
  TIMx->ccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_IC2PSC); // 预留 ■
  /* set the ic2psc value */
  TIMx->ccmr1 |= (uint16_t)(tim_icpsc << 8);
}

/**
  * @brief  sets the TIMx input capture 3 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_icpsc: specifies the input capture3 prescaler new value.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg tim_icpsc_div2: capture is done once every 2 events
  *     @arg tim_icpsc_div4: capture is done once every 4 events
  *     @arg tim_icpsc_div8: capture is done once every 8 events
  * @retval none
  */
AT(.com_periph.setic3prescaler)
void tim_setic3prescaler(tim_typedef* TIMx, uint16_t tim_icpsc)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(tim_icpsc));
  /* reset the ic3psc bits */
  TIMx->ccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_IC3PSC); // 预留 ■
  /* set the ic3psc value */
  TIMx->ccmr2 |= tim_icpsc;
}

/**
  * @brief  sets the TIMx input capture 4 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_icpsc: specifies the input capture4 prescaler new value.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg tim_icpsc_div2: capture is done once every 2 events
  *     @arg tim_icpsc_div4: capture is done once every 4 events
  *     @arg tim_icpsc_div8: capture is done once every 8 events
  * @retval none
  */
AT(.com_periph.setic4prescaler)
void tim_setic4prescaler(tim_typedef* TIMx, uint16_t tim_icpsc)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(tim_icpsc));
  /* reset the ic4psc bits */
  TIMx->ccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_IC4PSC); // 预留 ■
  /* set the ic4psc value */
  TIMx->ccmr2 |= (uint16_t)(tim_icpsc << 8);
}

/**
  * @brief  sets the TIMx clock division value.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select
  *   the tim peripheral.
  * @param  tim_ckd: specifies the clock division value.
  *   this parameter can be one of the following value:
  *     @arg TIM_CKD_DIV1: tdts = tck_tim
  *     @arg tim_ckd_div2: tdts = 2*tck_tim
  *     @arg tim_ckd_div4: tdts = 4*tck_tim
  * @retval none
  */
AT(.com_periph.tim.setclockdivision)
void tim_setclockdivision(tim_typedef* TIMx, uint16_t tim_ckd)
{
  /* check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CKD_DIV(tim_ckd));
  /* reset the ckd bits */
  TIMx->cr1 &= (uint16_t)~((uint16_t)TIM_CR1_CKD);
  /* set the ckd value */
  TIMx->cr1 |= tim_ckd;
}

/**
  * @brief  gets the TIMx input capture 1 value.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @retval capture compare 1 register value.
  */
//uint16_t tim_getcapture1(tim_typedef* TIMx)
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
//  /* get the capture 1 register value */
//  return TIMx->ccr1;
//}

/**
  * @brief  gets the TIMx input capture 2 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @retval capture compare 2 register value.
  */
//uint16_t tim_getcapture2(tim_typedef* TIMx)
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
//  /* get the capture 2 register value */
//  return TIMx->ccr2;
//}

/**
  * @brief  gets the TIMx input capture 3 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @retval capture compare 3 register value.
  */
//uint16_t tim_getcapture3(tim_typedef* TIMx)
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  /* get the capture 3 register value */
//  return TIMx->ccr3;
//}

/**
  * @brief  gets the TIMx input capture 4 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @retval capture compare 4 register value.
  */
//uint16_t tim_getcapture4(tim_typedef* TIMx)
//{
//  /* check the parameters */
//  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
//  /* get the capture 4 register value */
//  return TIMx->ccr4;
//}

/**
  * @brief  gets the TIMx counter value.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @retval counter register value.
  */
AT(.com_periph.tim.getcounter)
uint16_t tim_getcounter(tim_typedef* TIMx)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* get the counter register value */
  return TIMx->cnt;
}

/**
  * @brief  gets the TIMx prescaler value.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @retval prescaler register value.
  */
AT(.com_periph.tim.getprescaler)
uint16_t tim_getprescaler(tim_typedef* TIMx)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* get the prescaler register value */
  return TIMx->psc;
}

/**
  * @brief  checks whether the specified tim flag is set or not.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_flag: specifies the flag to check.
  *   this parameter can be one of the following values:
  *     @arg tim_flag_update: tim update flag
  *     @arg tim_flag_cc1: tim capture compare 1 flag
  *     @arg tim_flag_cc2: tim capture compare 2 flag
  *     @arg tim_flag_cc3: tim capture compare 3 flag
  *     @arg tim_flag_cc4: tim capture compare 4 flag
  *     @arg tim_flag_com: tim commutation flag
  *     @arg tim_flag_trigger: tim trigger flag
  *     @arg tim_flag_break: tim break flag
  *     @arg tim_flag_cc1of: tim capture compare 1 overcapture flag
  *     @arg tim_flag_cc2of: tim capture compare 2 overcapture flag
  *     @arg tim_flag_cc3of: tim capture compare 3 overcapture flag
  *     @arg tim_flag_cc4of: tim capture compare 4 overcapture flag
  * @note
  *   - tim6 and tim7 can have only one update flag.
  *   - tim9, TIM12 and TIM15 can have only tim_flag_update, tim_flag_cc1,
  *      tim_flag_cc2 or tim_flag_trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have tim_flag_update or tim_flag_cc1.
  *   - tim_flag_break is used only with TIM1, tim8 and TIM15.
  *   - tim_flag_com is used only with TIM1, tim8, TIM15, TIM16 and TIM17.
  * @retval the new state of tim_flag (set or reset).
  */
AT(.com_periph.getflagstatus)
FLAG_STATE tim_getflagstatus(tim_typedef* TIMx, uint16_t tim_flag)
{
  IT_STATE bitstatus = RESET;
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_GET_FLAG(tim_flag));

  if ((TIMx->sr & tim_flag) != (uint16_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  clears the TIMx's pending flags.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_flag: specifies the flag bit to clear.
  *   this parameter can be any combination of the following values:
  *     @arg tim_flag_update: tim update flag
  *     @arg tim_flag_cc1: tim capture compare 1 flag
  *     @arg tim_flag_cc2: tim capture compare 2 flag
  *     @arg tim_flag_cc3: tim capture compare 3 flag
  *     @arg tim_flag_cc4: tim capture compare 4 flag
  *     @arg tim_flag_com: tim commutation flag
  *     @arg tim_flag_trigger: tim trigger flag
  *     @arg tim_flag_break: tim break flag
  *     @arg tim_flag_cc1of: tim capture compare 1 overcapture flag
  *     @arg tim_flag_cc2of: tim capture compare 2 overcapture flag
  *     @arg tim_flag_cc3of: tim capture compare 3 overcapture flag
  *     @arg tim_flag_cc4of: tim capture compare 4 overcapture flag
  * @note
  *   - tim6 and tim7 can have only one update flag.
  *   - tim9, TIM12 and TIM15 can have only tim_flag_update, tim_flag_cc1,
  *      tim_flag_cc2 or tim_flag_trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have tim_flag_update or tim_flag_cc1.
  *   - tim_flag_break is used only with TIM1, tim8 and TIM15.
  *   - tim_flag_com is used only with TIM1, tim8, TIM15, TIM16 and TIM17.
  * @retval none
  */
AT(.com_periph.clearflag)
void tim_clearflag(tim_typedef* TIMx, uint16_t tim_flag)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_CLEAR_FLAG(tim_flag));

  /* clear the flags */
  TIMx->sr = (uint16_t)tim_flag;
}

/**
  * @brief  checks whether the tim interrupt has occurred or not.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_it: specifies the tim interrupt source to check.
  *   this parameter can be one of the following values:
  *     @arg tim_it_update: tim update interrupt source
  *     @arg tim_it_cc1: tim capture compare 1 interrupt source
  *     @arg tim_it_cc2: tim capture compare 2 interrupt source
  *     @arg tim_it_cc3: tim capture compare 3 interrupt source
  *     @arg tim_it_cc4: tim capture compare 4 interrupt source
  *     @arg tim_it_com: tim commutation interrupt source
  *     @arg tim_it_trigger: tim trigger interrupt source
  *     @arg tim_it_break: tim break interrupt source
  * @note
  *   - tim6 and tim7 can generate only an update interrupt.
  *   - tim9, TIM12 and TIM15 can have only tim_it_update, tim_it_cc1,
  *      tim_it_cc2 or tim_it_trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have tim_it_update or tim_it_cc1.
  *   - tim_it_break is used only with TIM1, tim8 and TIM15.
  *   - tim_it_com is used only with TIM1, tim8, TIM15, TIM16 and TIM17.
  * @retval the new state of the tim_it(set or reset).
  */
AT(.com_periph.getitstatus)
IT_STATE tim_getitstatus(tim_typedef* TIMx, uint16_t tim_it)
{
  IT_STATE bitstatus = RESET;
  uint16_t itstatus = 0x0, itenable = 0x0;
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_GET_IT(tim_it));

  itstatus = TIMx->sr & tim_it;

  itenable = TIMx->dier & tim_it;
  if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  clears the TIMx's interrupt pending bits.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  tim_it: specifies the pending bit to clear.
  *   this parameter can be any combination of the following values:
  *     @arg tim_it_update: TIM1 update interrupt source
  *     @arg tim_it_cc1: tim capture compare 1 interrupt source
  *     @arg tim_it_cc2: tim capture compare 2 interrupt source
  *     @arg tim_it_cc3: tim capture compare 3 interrupt source
  *     @arg tim_it_cc4: tim capture compare 4 interrupt source
  *     @arg tim_it_com: tim commutation interrupt source
  *     @arg tim_it_trigger: tim trigger interrupt source
  *     @arg tim_it_break: tim break interrupt source
  * @note
  *   - tim6 and tim7 can generate only an update interrupt.
  *   - tim9, TIM12 and TIM15 can have only tim_it_update, tim_it_cc1,
  *      tim_it_cc2 or tim_it_trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have tim_it_update or tim_it_cc1.
  *   - tim_it_break is used only with TIM1, tim8 and TIM15.
  *   - tim_it_com is used only with TIM1, tim8, TIM15, TIM16 and TIM17.
  * @retval none
  */
AT(.com_periph.clearitpendingbit)
void tim_clearitpendingbit(tim_typedef* TIMx, uint16_t tim_it)
{
  /* check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_IT(tim_it));
  /* clear the it pending bit */
  TIMx->sr = (uint16_t)tim_it;
}

/**
  * @brief  configure the ti1 as input.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the tim peripheral.
  * @param  tim_icpolarity : the input polarity.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_RISING
  *     @arg TIM_ICPOLARITY_FALLING
  * @param  tim_icselection: specifies the input to be used.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICSELECTION_DIRECTTI: tim input 1 is selected to be connected to ic1.
  *     @arg TIM_ICSELECTION_INDIRECTTI: tim input 1 is selected to be connected to ic2.
  *     @arg tim_icselection_trc: tim input 1 is selected to be connected to trc.
  * @param  tim_icfilter: specifies the input capture filter.
  *   this parameter must be a value between 0x00 and 0x0f.
  * @retval none
  */
static void ti1_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
                       uint16_t tim_icfilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0;
  /* DISABLE the channel 1: reset the cc1e bit */
  TIMx->ccer &= (uint16_t)~((uint16_t)TIM_CCER_CC1E);
  tmpccmr1 = TIMx->ccmr1;
  tmpccer = TIMx->ccer;
  /* select the input and set the filter */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC1S)) & ((uint16_t)~((uint16_t)TIM_CCMR1_IC1F))); // 预留 ■
  tmpccmr1 |= (uint16_t)(tim_icselection | (uint16_t)(tim_icfilter << (uint16_t)4));

  if(TIMx == TIM1)
  {
    /* select the polarity and set the cc1e bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC1P));
    tmpccer |= (uint16_t)(tim_icpolarity | (uint16_t)TIM_CCER_CC1E);
  }
  else
  {
    /* select the polarity and set the cc1e bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC1P | TIM_CCER_CC1NP));
    tmpccer |= (uint16_t)(tim_icpolarity | (uint16_t)TIM_CCER_CC1E);
  }

  /* write to TIMx ccmr1 and ccer registers */
  TIMx->ccmr1 = tmpccmr1;
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configure the ti2 as input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the tim peripheral.
  * @param  tim_icpolarity : the input polarity.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_RISING
  *     @arg TIM_ICPOLARITY_FALLING
  * @param  tim_icselection: specifies the input to be used.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICSELECTION_DIRECTTI: tim input 2 is selected to be connected to ic2.
  *     @arg TIM_ICSELECTION_INDIRECTTI: tim input 2 is selected to be connected to ic1.
  *     @arg tim_icselection_trc: tim input 2 is selected to be connected to trc.
  * @param  tim_icfilter: specifies the input capture filter.
  *   this parameter must be a value between 0x00 and 0x0f.
  * @retval none
  */
static void ti2_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,
                       uint16_t tim_icfilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;
  /* DISABLE the channel 2: reset the cc2e bit */
  TIMx->ccer &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);
  tmpccmr1 = TIMx->ccmr1;
  tmpccer = TIMx->ccer;
  tmp = (uint16_t)(tim_icpolarity << 4);
  /* select the input and set the filter */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC2S)) & ((uint16_t)~((uint16_t)TIM_CCMR1_IC2F))); // 预留 ■
  tmpccmr1 |= (uint16_t)(tim_icfilter << 12);
  tmpccmr1 |= (uint16_t)(tim_icselection << 8);

  if(TIMx == TIM1)
  {
    /* select the polarity and set the cc2e bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC2P));
    tmpccer |=  (uint16_t)(tmp | (uint16_t)TIM_CCER_CC2E);
  }
  else
  {
    /* select the polarity and set the cc2e bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC2P | TIM_CCER_CC2NP));
    tmpccer |= (uint16_t)(tim_icpolarity | (uint16_t)TIM_CCER_CC2E);
  }

  /* write to TIMx ccmr1 and ccer registers */
  TIMx->ccmr1 = tmpccmr1 ;
  TIMx->ccer = tmpccer;
}

/**
  * @brief  configure the ti3 as input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_icpolarity : the input polarity.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_RISING
  *     @arg TIM_ICPOLARITY_FALLING
  * @param  tim_icselection: specifies the input to be used.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICSELECTION_DIRECTTI: tim input 3 is selected to be connected to ic3.
  *     @arg TIM_ICSELECTION_INDIRECTTI: tim input 3 is selected to be connected to ic4.
  *     @arg tim_icselection_trc: tim input 3 is selected to be connected to trc.
  * @param  tim_icfilter: specifies the input capture filter.
  *   this parameter must be a value between 0x00 and 0x0f.
  * @retval none
  */
//static void ti3_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection,  预留
//                       uint16_t tim_icfilter)
//{
//  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;
//  /* DISABLE the channel 3: reset the cc3e bit */
//  TIMx->ccer &= (uint16_t)~((uint16_t)TIM_CCER_CC3E);
//  tmpccmr2 = TIMx->ccmr2;
//  tmpccer = TIMx->ccer;
//  tmp = (uint16_t)(tim_icpolarity << 8);
//  /* select the input and set the filter */
//  tmpccmr2 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR2_CC3S)) & ((uint16_t)~((uint16_t)TIM_CCMR2_IC3F))); // 预留 ■
//  tmpccmr2 |= (uint16_t)(tim_icselection | (uint16_t)(tim_icfilter << (uint16_t)4));
//
//  if(TIMx == TIM1)
//  {
//    /* select the polarity and set the cc3e bit */
//    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P));
//    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC3E);
//  }
//  else
//  {
//    /* select the polarity and set the cc3e bit */
//    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P | TIM_CCER_CC3NP));
//    tmpccer |= (uint16_t)(tim_icpolarity | (uint16_t)TIM_CCER_CC3E);
//  }
//
//  /* write to TIMx ccmr2 and ccer registers */
//  TIMx->ccmr2 = tmpccmr2;
//  TIMx->ccer = tmpccer;
//}

/**
  * @brief  configure the ti4 as input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the tim peripheral.
  * @param  tim_icpolarity : the input polarity.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICPOLARITY_RISING
  *     @arg TIM_ICPOLARITY_FALLING
  * @param  tim_icselection: specifies the input to be used.
  *   this parameter can be one of the following values:
  *     @arg TIM_ICSELECTION_DIRECTTI: tim input 4 is selected to be connected to ic4.
  *     @arg TIM_ICSELECTION_INDIRECTTI: tim input 4 is selected to be connected to ic3.
  *     @arg tim_icselection_trc: tim input 4 is selected to be connected to trc.
  * @param  tim_icfilter: specifies the input capture filter.
  *   this parameter must be a value between 0x00 and 0x0f.
  * @retval none
  */
//static void ti4_config(tim_typedef* TIMx, uint16_t tim_icpolarity, uint16_t tim_icselection, 预留
//                       uint16_t tim_icfilter)
//{
//  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;
//
//   /* DISABLE the channel 4: reset the cc4e bit */
//  TIMx->ccer &= (uint16_t)~((uint16_t)TIM_CCER_CC4E);
//  tmpccmr2 = TIMx->ccmr2;
//  tmpccer = TIMx->ccer;
//  tmp = (uint16_t)(tim_icpolarity << 12);
//  /* select the input and set the filter */
//  tmpccmr2 &= (uint16_t)((uint16_t)(~(uint16_t)TIM_CCMR2_CC4S) & ((uint16_t)~((uint16_t)TIM_CCMR2_IC4F))); // 预留 ■
//  tmpccmr2 |= (uint16_t)(tim_icselection << 8);
//  tmpccmr2 |= (uint16_t)(tim_icfilter << 12);
//
//  if(TIMx == TIM1)
//  {
//    /* select the polarity and set the cc4e bit */
//    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC4P));
//    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC4E);
//  }
//  else
//  {
//    /* select the polarity and set the cc4e bit */
//    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P | TIM_CCER_CC4NP)); // 预留 ■
//    tmpccer |= (uint16_t)(tim_icpolarity | (uint16_t)TIM_CCER_CC4E);
//  }
//  /* write to TIMx ccmr2 and ccer registers */
//  TIMx->ccmr2 = tmpccmr2;
//  TIMx->ccer = tmpccer;
//}

/**
  * @brief  enables or DISABLEs the TIMx saradc trgo enable or DISABLE.
  * @param  TIMx: where x can be 1 to 17 to select the tim peripheral.
  * @param  newstate: new state of the TIMx saradc trgo bit
  *   this parameter can be: enable or DISABLE.
  * @retval none
  */
AT(.com_periph.tim.trgosaradc)
void tim_trgosaradc_config(tim_typedef* TIMx, FUNCTIONAL_STATE newstate)
{

  if(newstate != DISABLE)
  {
    /* set the update DISABLE bit */
    TIMx->sarad |= TIM_SARAD_TRGOEN;
  }
  else
  {
    /* reset the update DISABLE bit */
    TIMx->sarad &= (uint32_t)~((uint32_t)TIM_SARAD_TRGOEN);
  }
}




