#include "include.h"
#include "utils_ring_buffer.h"


void utils_ring_buf_init(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size)
{
    memset(ring_buf, 0, sizeof(ring_buf_t));

    ring_buf->buf = buffer;
    ring_buf->total_len = size;
}

void utils_ring_buf_clear(ring_buf_t *ring_buf)
{
    if(ring_buf){
        ring_buf->data_len = 0;
        ring_buf->buf_read = 0;
        ring_buf->buf_write = 0;
    }
}

AT(.com_text.ring_buf.write)
uint32_t utils_ring_buf_write(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size)
{
    uint32_t len            = 0;

    uint32_t total_len    = ring_buf->total_len;
    uint8_t *ring_buf_source = ring_buf->buf;

    if((ring_buf->data_len + size) < total_len){
        uint32_t ring_buf_bw     = ring_buf->buf_write;
        if((ring_buf_bw + size) <= total_len) {
            memcpy(ring_buf_source + ring_buf_bw, buffer, size);
        } else {
            len = total_len - ring_buf_bw;
            memcpy(ring_buf_source + ring_buf_bw, buffer, len);
            memcpy(ring_buf_source, buffer + len, size - len);
        }
        {
            GLOBAL_INT_DISABLE();
            ring_buf->buf_write = (ring_buf->buf_write + size) % total_len;
            ring_buf->data_len += size;
            GLOBAL_INT_RESTORE();
        }
        return size;
    }else {
        return 0;
    }
}

AT(.com_text.ring_buf.read)
uint32_t utils_ring_buf_read(ring_buf_t *ring_buf, uint8_t *buffer, uint32_t size)
{
	uint32_t len            = 0;

	uint32_t total_len    = ring_buf->total_len;
    uint8_t *ring_buf_source = ring_buf->buf;

    if(ring_buf->data_len >= size){

        GLOBAL_INT_DISABLE();
        uint32_t ring_buf_br     = ring_buf->buf_read;
        ring_buf->buf_read = (ring_buf->buf_read + size) % total_len;
        ring_buf->data_len -= size;
        GLOBAL_INT_RESTORE();

        if((ring_buf_br + size) <= total_len) {
            memcpy(buffer, ring_buf_source + ring_buf_br, size);
        } else {
            len = total_len - ring_buf_br;
            memcpy(buffer, ring_buf_source + ring_buf_br, len);
            memcpy(buffer + len, ring_buf_source, size - len);
        }
        return size;
    }

    return 0;
}

AT(.com_text.ring_buf.get)
uint8_t* utils_ring_buf_get(ring_buf_t *ring_buf, uint32_t size)
{
	uint32_t total_len    = ring_buf->total_len;
    uint8_t *ring_buf_source = ring_buf->buf;

    if(ring_buf->data_len >= size){

        uint32_t ring_buf_br     = ring_buf->buf_read;
        if((ring_buf_br + size) <= total_len) {
            GLOBAL_INT_DISABLE();
            ring_buf->buf_read = (ring_buf->buf_read + size) % total_len;
            ring_buf->data_len -= size;
            GLOBAL_INT_RESTORE();

            return ring_buf_source + ring_buf_br;
        }
    }

    return NULL;
}

AT(.com_text.ring_buf.dl_get)
uint32_t utils_ring_buf_data_len_get(ring_buf_t *ring_buf)
{
	return ring_buf->data_len;
}

AT(.com_text.ring_buf.tl_get)
uint32_t utils_ring_buf_total_len_get(ring_buf_t *ring_buf)
{
	return ring_buf->total_len;
}

