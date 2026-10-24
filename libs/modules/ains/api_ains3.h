#ifndef _API_AINS3_H
#define _API_AINS3_H

typedef struct {
    u32 nt;
    u8  prior_opt_idx;
    s16 gain_floor;
} ains3_cb_t;

void ains3_mono_init(ains3_cb_t *ains3_cb);
void ains3_mono_process(s32 *data);

#endif
