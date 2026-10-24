#ifndef _API_TIMER_H
#define _API_TIMER_H


/**
 * @brief The memory overhead required by a single soft timer unit (unchangeable).
 */
#define SOFT_TIMER_MEM_SPACE                        28

/**
 * @brief Soft timer Error code.
 */
#define SOFT_TIMER_ERR_NO_ERR                       0x00
#define SOFT_TIMER_ERR_UNINITIALIZE                 0x01
#define SOFT_TIMER_ERR_NOT_FREE_MEM                 0x02
#define SOFT_TIMER_ERR_NOT_EXIST_ID                 0x03
#define SOFT_TIMER_ERR_ALREADY_ACTIVE               0x04
#define SOFT_TIMER_ERR_ALREADY_STOP                 0x05
#define SOFT_TIMER_ERR_INTV_OUT_RANGE               0x06
#define SOFT_TIMER_ERR_DISALLOW                     0x07


typedef uint32_t soft_timer_handle_t;

typedef void (*soft_timer_callback_t)(soft_timer_handle_t hdl);

typedef struct {
    uint8_t             mem[SOFT_TIMER_MEM_SPACE];
} soft_timer_mem_t;

typedef struct {
    soft_timer_handle_t handle;
    soft_timer_mem_t    mem;
} soft_timer_t;


/**
 * @brief  Initialize the timer memery pool.
 * @note   This function and a series of related functions are invoked after ble/wireless init.
 * @param  pool: memery pool head address.
 * @param  size: byte size of memery pool, one alarm timer need `SOFT_TIMER_MEM_SPACE` bytes memery.
 * @retval None.
 */
void soft_timer_init(void *pool, uint32_t size);

/**
 * @brief  Extend the timer memery pool, it can be used before and after initialization.
 * @param  pool: memery pool head address.
 * @param  size: byte size of memery pool, one alarm timer need `SOFT_TIMER_MEM_SPACE` bytes memery.
 * @retval result code.
 */
uint8_t soft_timer_mem_extend(void *pool, uint32_t size);

/**
 * @brief  Free the timer memery pool, and all timers in the space are released.
 * @param  pool: memery pool head address.
 * @param  size: byte size of memery pool, one alarm timer need `SOFT_TIMER_MEM_SPACE` bytes memery.
 * @retval result code.
 */
uint8_t soft_timer_mem_free(void *pool, uint32_t size);

/**
 * @brief  Start specified handle alarm timer.
 * @param  handle: alarm timer handle.
 * @retval result code.
 */
uint8_t soft_timer_start(soft_timer_handle_t handle);

/**
 * @brief  Stop specified handle alarm timer.
 * @param  handle: alarm timer handle.
 * @retval result code.
 */
uint8_t soft_timer_stop(soft_timer_handle_t handle);

/**
 * @brief  Acquire a alarm timer.
 * @param  handle: alarm timer handle.
 * @param  interval: alarm timer trigger interval, unit [1, 0xffffff) ms.
 * @param  is_repeat: trigger mode.
 * @param  callback: callback function, it will be invoke by thread_stack when timer timeout.
 * @retval result code.
 */
uint8_t soft_timer_acquire(soft_timer_handle_t *handle, uint32_t interval, uint8_t is_repeat, soft_timer_callback_t callback);

/**
 * @brief  Release specified id alarm timer.
 * @param  handle: alarm timer handle.
 * @retval result code.
 */
uint8_t soft_timer_release(soft_timer_handle_t handle);

/**
 * @brief  Query specified handle alarm timer remain time.
 * @param  handle: alarm timer handle.
 * @param  remain: the remain time for specified handle alarm timer.
 * @retval result code.
 */
uint8_t soft_timer_query_time(soft_timer_handle_t handle, uint32_t *remain);

/**
 * @brief  Modify specified handle alarm timer interval, if the timer is start, it
 *         will be re-start after update interval param.
 * @param  handle: alarm timer handle.
 * @param  interval: new interval, unit [1, 0xffffff) ms.
 * @param  is_repeat: new is_repeat mode state.
 * @retval result code.
 */
uint8_t soft_timer_modify_interval(soft_timer_handle_t handle, uint32_t interval, uint8_t is_repeat);

/**
 * @brief  Create one timer.
 *         [IMPORTANT] If you use temporary variables, please be sure to destroy the timer after you finish using it.
 *         [IMPORTANT] If you use temporary variables, please be sure to destroy the timer after you finish using it.
 *         [IMPORTANT] If you use temporary variables, please be sure to destroy the timer after you finish using it.
 * @param  timer: timer instance pointer, see `soft_timer_t`.
 * @param  interval: alarm timer trigger interval, unit [1, 0xffffff) ms.
 * @param  is_repeat: trigger mode.
 * @param  func: callback function.
 * @retval result code.
 */
ALWAYS_INLINE uint8_t soft_timer_create(soft_timer_t *timer, uint32_t interval, uint8_t is_repeat, soft_timer_callback_t func)
{
    uint8_t status = soft_timer_mem_extend(&timer->mem, SOFT_TIMER_MEM_SPACE);

    if (status == 0) {
        status = soft_timer_acquire(&timer->handle, interval, is_repeat, func);
    }

    return status;
}

/**
 * @brief  Destroy specified timer.
 * @param  timer: timer instance pointer, see `soft_timer_t`.
 * @retval result code.
 */
ALWAYS_INLINE uint8_t soft_timer_destroy(soft_timer_t *timer)
{
    return soft_timer_mem_free(&timer->mem, SOFT_TIMER_MEM_SPACE);
}

#endif // _API_TIMER_H
