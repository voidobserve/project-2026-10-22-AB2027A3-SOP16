#ifndef _API_INTERRUPT_H
#define _API_INTERRUPT_H

#define GLOBAL_INT_DISABLE()    uint32_t cpu_ie = PICCON&BIT(0); PICCONCLR = BIT(0);asm volatile("":::"memory")
#define GLOBAL_INT_RESTORE()    asm volatile("":::"memory");PICCON |= cpu_ie

/**
 * @brief XBOX Interrupt Number Definition.
 */
typedef enum {
    IRQn_SOFT = 2,
    IRQn_TMR0,
    IRQn_TMR1,
    IRQn_TMR2,
    IRQn_IR_QDEC_LEDC,
    IRQn_USB_CONTROL,
    IRQn_SD,
    IRQn_KEY_SCAN,
    IRQn_SDADC_DMA,
    IRQn_SDADC_SAMPLE               = 13,
    IRQn_UART0_UDET,
    IRQn_HSUART,
    IRQn_TMR3,
    IRQn_DAC_DMAIN                  = 19,
    IRQn_SPIx,
    IRQn_URAT0_KEY_UDET_KEY,
    IRQn_FREQUENCY_DET_TOUCH_KEY    = 23,
    IRQn_PORT                       = 26,
    IRQn_SARADC                     = 28,
    IRQn_RTC_SECOND_ALARM_LVD_WDT,
    IRQn_IIC,
    IRQn_TICK0,
} IRQ_TYPEDEF;

typedef void (*isr_t)(void);

/**
 * @brief system interrupt init
 * @param interrupt number,define in IRQ_TYPEDEF
 * @param interrupt priority, 1 is high priority, 0 is low priority
 * @param interrupt callback function
 */
bool sys_irq_init(int vector, int pr, isr_t isr);

#endif
