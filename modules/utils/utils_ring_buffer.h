#ifndef _UTILS_RING_BUFFER_H
#define _UTILS_RING_BUFFER_H


typedef struct{
    uint8_t *buf;
    uint32_t buf_read;           //read位置
    uint32_t buf_write;          //write位置
    uint32_t data_len;           //可使用的数据长度
    uint32_t total_len;          //ringbuf数据长度
} ring_buf_t;


void utils_ring_buf_init(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size);
void utils_ring_buf_clear(ring_buf_t *ring_buf);
uint32_t utils_ring_buf_write(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size);
uint32_t utils_ring_buf_read(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size);
uint32_t utils_ring_buf_data_len_get(ring_buf_t *ring_buf);
uint8_t* utils_ring_buf_get(ring_buf_t *ring_buf, uint32_t size);

#endif // _UTILS_RING_BUFFER_H
