#ifndef _API_LIST_H_
#define _API_LIST_H_

typedef struct list_hdr {
    struct list_hdr *next;
} list_hdr_t;

typedef struct {
    struct list_hdr *first;
    struct list_hdr *last;
} list_t;


/**
 * @brief  Initialize a list of free element in target pool.
 * @param  list: Pointer to the list.
 * @param  pool: Pointer to the free pool.
 * @param  elt_size: Size of each element of the free pool.
 * @param  elt_cnt: Count of elements in free pool.
 * @retval None.
 */
void list_pool_init(list_t *list, void *pool, size_t elt_size, uint32_t elt_cnt);

/**
 * @brief  Push an element into target list rear.
 * @param  list: Pointer to the list.
 * @param  list_hdr: Pointer to the target element.
 * @retval None.
 */
void list_push_back(list_t *list, list_hdr_t *list_hdr);

/**
 * @brief  Pop first element from target list.
 * @param  list: Pointer to the list.
 * @retval List element head pointer.
 */
list_hdr_t *list_pop_front(list_t *list);

#endif // _API_LIST_H_

