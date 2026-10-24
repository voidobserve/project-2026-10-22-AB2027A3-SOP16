#include "driver_sddac.h"
#include "driver_clk.h"

/**
 * @brief  Kick Start SDDAC DMA function.
 * @param  buf: DMA start address.
 * @param  size: DMA transmit len.
 * @retval None
 */
AT(.com_periph.sddac.kick)
void sddac_dma_kick_start(uint8_t *buf, uint16_t size)
{
    SDDAC->dma_con &= ~SDDAC_DMA_EN;
    SDDAC->dma_addr = (s32)buf;
    SDDAC->dma_size = size - 1;
    SDDAC->dma_con |= SDDAC_DMA_EN;
}

/**
 * @brief  SDDAC DMA stop function.
 * @retval None
 */
AT(.com_periph.sddac.dma_stop)
void sddac_dma_stop(void)
{
    SDDAC->dma_con &= ~SDDAC_DMA_EN;
}

/**
 * @brief  SDDAC sample rate get function.
 * @retval sample rate
 */
AT(.com_periph.sddac.spr_get)
uint8_t sddac_spr_get(void)
{
    uint8_t cur_spr = (SDDAC->con & SDDAC_FSIN_SEL) >> 1;
    uint8_t cur_spr_mode = (SDDAC->con & SDDAC_OSR_MODE) >> 4;
    if (cur_spr_mode) {
        return cur_spr + SDDAC_NORMAL_SPR_8k + 1;
    } else {
        return cur_spr;
    }
}

/**
 * @brief  SDDAC sample rate set function.
 * @retval bool  1: same sampling rate;  0: different sampling rate
 */
AT(.com_periph.sddac.spr_set)
bool sddac_spr_set(uint8_t spr)
{
    if (spr == sddac_spr_get()) {
        return 1;
    }

    SDDAC->dma_con &= ~SDDAC_DMA_EN;
    SDDAC->con &= ~SDDAC_DAC_EN;

    if(spr > SDDAC_NORMAL_SPR_8k){
        SDDAC->con |= SDDAC_OSR_MODE;
        spr -= (SDDAC_NORMAL_SPR_8k + 1);
    }else{
        SDDAC->con &= ~SDDAC_OSR_MODE;
    }

    SDDAC->con &= ~SDDAC_FSIN_SEL;
    SDDAC->con |= ((spr * SDDAC_FSIN_SEL_0) & SDDAC_FSIN_SEL);
    SDDAC->con |= SDDAC_DAC_EN;
    return 0;
}

/**
 * @brief  Initializes the sddac peripheral according to the specified
 *         parameters in the config
 * @param  config: pointer to a sddac_init_typedef structure that
 *         contains the configuration information for the specified SDDAC peripheral.
 * @retval None
 */
void sddac_init(sddac_init_typedef *config)
{
    if(config){
        if(config->sample_rate > SDDAC_NORMAL_SPR_8k){
            SDDAC->con |= SDDAC_OSR_MODE;
            config->sample_rate -= (SDDAC_NORMAL_SPR_8k + 1);
        }else{
            SDDAC->con &= ~SDDAC_OSR_MODE;
        }
        SDDAC->con &= ~SDDAC_FSIN_SEL;
        SDDAC->con |= ((config->sample_rate * SDDAC_FSIN_SEL_0) & SDDAC_FSIN_SEL);
        SDDAC->dma_addr = (s32)config->dma_buf;
        SDDAC->dma_size = config->dma_size - 1;
        SDDAC->dma_con |= SDDAC_DMA_EN;
    }
}

/**
 * @brief  De-initialize the specified SDDAC peripheral.
 * @retval None
 */
void sddac_deinit(void)
{
    SDDAC->dma_con &= ~SDDAC_DMA_EN;                    //dma disable
    SDDAC->con &= ~SDDAC_DAC_EN;                        //dac disable

    clk_gate0_cmd(CLK_GATE0_DAC, CLK_DIS);              //dac clk gate disable
}

/**
 * @brief  Enable or disable the specified SDDAC interrupt.
 * @param  isr: Function to be executed for service interruption.
 * @param  pr: Priority of service interruption.
 * @param  interrupt_type: specifies the SDDAC interrupt sources to be enable or disable.
 *         this parameter can be one of the following values:
 *             @arg SDDAC_IT_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDDAC_IT_HALF_DONE: DMA half-transfer interrupt.
 * @param  state: the state of the SDDAC peripheral's interrupt.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void sddac_pic_config(isr_t isr, int pr, SDDAC_IT_TYPEDEF interrupt_type, FUNCTIONAL_STATE state)
{
    if(state == ENABLE){
        if(interrupt_type & SDDAC_IT_ALL_DONE){
            SDDAC->dma_con |= SDDAC_DMA_APEND_IE;
        }
        if(interrupt_type & SDDAC_IT_HALF_DONE){
            SDDAC->dma_con |= SDDAC_DMA_HPEND_IE;
        }
        sys_irq_init(IRQn_DAC_DMAIN, pr, isr);
    }else{
        SDDAC->dma_con &= ~(SDDAC_DMA_APEND_IE | SDDAC_DMA_HPEND_IE);
    }
}

/**
 * @brief  Enable or disable the specified SDDAC peripheral.
 * @param  state: the state of the SDDAC peripheral.
 *         this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void sddac_cmd(FUNCTIONAL_STATE state)
{
    if(state == ENABLE){
        SDDAC->con |= SDDAC_DAC_EN;
    }else{
        SDDAC->con &= ~SDDAC_DAC_EN;
    }
}

/**
 * @brief  Check the specified SDDAC flag is set or not.
 * @param  interrupt_type: specifies the flag to check.
 *         the parameter can be one of the following values:
 *             @arg SDDAC_IT_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDDAC_IT_HALF_DONE: DMA half-transfer interrupt.
 * @retval the state of interrupt_type (SET or RESET).
 */
AT(.com_periph.sddac.get)
FLAG_STATE sddac_get_flag(SDDAC_IT_TYPEDEF interrupt_type)
{
    if((interrupt_type == SDDAC_IT_ALL_DONE) && (SDDAC->dma_pend & SDDAC_DMA_ALLPEND)){
        return SET;
    }else if((interrupt_type == SDDAC_IT_HALF_DONE) && (SDDAC->dma_pend & SDDAC_DMA_HALFPEND)){
        return SET;
    }

    return RESET;
}

/**
 * @brief  Clear the SDDAC pending.
 * @param  interrupt_type: specifies flag to clear.
 *         the parameter can be one of the following values:
 *             @arg SDDAC_IT_ALL_DONE: DMA full-transfer interrupt.
 *             @arg SDDAC_IT_HALF_DONE: DMA half-transfer interrupt.
 * @retval None
 */
AT(.com_periph.sddac.clear)
void sddac_clear_flag(SDDAC_IT_TYPEDEF interrupt_type)
{
    if(interrupt_type == SDDAC_IT_ALL_DONE){
        SDDAC->dma_cpnd = SDDAC_DMA_ALLPEND_CLR;
    }else if (interrupt_type == SDDAC_IT_HALF_DONE){
        SDDAC->dma_cpnd = SDDAC_DMA_HALFPEND_CLR;
    }
}

