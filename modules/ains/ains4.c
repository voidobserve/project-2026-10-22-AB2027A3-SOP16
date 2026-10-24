#include "include.h"
#include "api_ains4.h"

#if (AINS4_EN)

#define PART_LEN			                    (AINS4_PART_LEN << 1)
#define PART_LEN2			                    (PART_LEN << 1)
#define PART_HALF_LEN                           (PART_LEN / 2)
#define HANN_SHIFT                              (16)

typedef struct {
    u8  sysclk;
    bool init;
} ains4_cb_t;

static ains4_cb_t ains4_cb;

typedef struct {
    // s16 input[PART_LEN];
    // s16 fft_output[PART_LEN];
    // s16 output[PART_LEN];
    s16 input[128];
    s16 fft_output[128];
    s16 output[128];
    u8 iptr;
    u8 optr;
} ains4_rdft_t;

static ains4_rdft_t ains4_rdft AT(.ains4_buf.rdft);

static struct nr_buf_t {
    // int fft[PART_LEN2];
	// s16 xbuf[PART_LEN2];
	// int outBuf[PART_LEN];
	int fft[256];    
	s16 xbuf[256];
	int outBuf[128];
} nr_buf AT(.fft_buf.buf);

#if (PART_LEN == 120)
//120
AT(.fft_rodata)
static const unsigned short sqrt_Hanning[121] = {
	854, 1708, 2562, 3416, 4268, 5121, 5972, 6822, 7671, 8519, 9365, 10210, 11053, 11894, 12733,
	13570, 14405, 15237, 16066, 16893, 17717, 18538, 19356, 20170, 20982, 21789, 22593, 23393,
	24189, 24981, 25769, 26552, 27331, 28105, 28874, 29638, 30398, 31152, 31901, 32645, 33383,
	34115, 34841, 35562, 36277, 36985, 37687, 38383, 39072, 39754, 40430, 41099, 41761, 42416,
	43064, 43704, 44337, 44962, 45580, 46190, 46792, 47386, 47972, 48550, 49120, 49681, 50234,
	50778, 51314, 51841, 52359, 52869, 53369, 53860, 54342, 54815, 55279, 55733, 56178, 56613,
	57038, 57454, 57860, 58257, 58643, 59019, 59386, 59742, 60088, 60424, 60750, 61065, 61370,
	61664, 61948, 62222, 62485, 62737, 62979, 63210, 63430, 63639, 63838, 64026, 64203, 64369,
	64524, 64668, 64801, 64923, 65034, 65134, 65223, 65301, 65368, 65423, 65468, 65501, 65523,
	65535, 65535,
};
#else
//128
AT(.fft_rodata)
static const unsigned short sqrt_Hanning[129] = {
	0x0000,0x0321,0x0642,0x0963,0x0C83,0x0FA3,0x12C2,0x15E1,
	0x18FF,0x1C1C,0x1F37,0x2252,0x256B,0x2883,0x2B99,0x2EAE,
	0x31C0,0x34D1,0x37E0,0x3AED,0x3DF7,0x40FF,0x4405,0x4708,
	0x4A08,0x4D06,0x5000,0x52F8,0x55EC,0x58DD,0x5BCA,0x5EB5,
	0x619B,0x647E,0x675D,0x6A38,0x6D0E,0x6FE1,0x72AF,0x7579,
	0x783F,0x7B00,0x7DBC,0x8074,0x8326,0x85D4,0x887C,0x8B1F,
	0x8DBD,0x9055,0x92E8,0x9575,0x97FD,0x9A7F,0x9CFA,0x9F70,
	0xA1E0,0xA44A,0xA6AD,0xA90A,0xAB60,0xADB0,0xAFF9,0xB23B,
	0xB477,0xB6AC,0xB8DA,0xBB00,0xBD20,0xBF38,0xC149,0xC353,
	0xC555,0xC74F,0xC942,0xCB2E,0xCD11,0xCEED,0xD0C0,0xD28C,
	0xD450,0xD60B,0xD7BF,0xD96A,0xDB0D,0xDCA7,0xDE39,0xDFC2,
	0xE143,0xE2BC,0xE42B,0xE592,0xE6F0,0xE845,0xE992,0xEAD5,
	0xEC10,0xED41,0xEE6A,0xEF89,0xF09F,0xF1AC,0xF2AF,0xF3AA,
	0xF49B,0xF582,0xF661,0xF736,0xF801,0xF8C3,0xF97B,0xFA2A,
	0xFACF,0xFB6B,0xFBFD,0xFC86,0xFD04,0xFD7A,0xFDE5,0xFE47,
	0xFE9F,0xFEED,0xFF31,0xFF6C,0xFF9D,0xFFC4,0xFFE1,0xFFF5,
	0xFFFF
};
#endif
void fft_256(s32 *buf);
void ifft_256(s32* buf);
void lock_code_alg(u32 start_addr, u32 end_addr);
void unlock_code_alg(void);

extern u32 __code_start_alg, __code_end_alg;

typedef signed long long __int64;
__int64 lo;

#define MZR()           do {(lo) = 0;} while(0)
#define ML0(x, y)       ((lo)  =  (__int64)(x) * (__int64)(y))
#define MS0(x, y)       ((lo)  = -(__int64)(x) * (__int64)(y))
#define MLA(x, y)       ((lo) +=  (__int64)(x) * (__int64)(y))
#define MSB(x, y)       ((lo) -=  (__int64)(x) * (__int64)(y))
#define MLZ(n)          (int)(((lo) + (1<<(n-1)))>>(n))
#define round(x,n)      ((int) ((x+(1<<(n-1)))>>n))

#define MAC_LO()        ((int *)(void *)&lo)[0]
#define MAC_HI()        ((int *)(void *)&lo)[1]

#define MAC_SET_LO(x)   do {((int *)(void *)&lo)[0] = x;} while(0)
#define MAC_SET_HI(x)   do {((int *)(void *)&lo)[1] = x;} while(0)
#define AEC_SPL_SAT(A, B, C)    (B > A ? A : B < C ? C : B)
#define AEC_SPL_WORD16_MAX      (32767)
#define AEC_SPL_WORD16_MIN      (-32768)

ALWAYS_INLINE int aec_spl_sat(int x0)
{
	int y0;
	__asm__ volatile("p.clip %0, %1, %2" : "=r"(y0) : "r"(x0), "i"(15));
	return y0;
}

AT(.ains4_text.alg)
static void asr_kws_process(void)
{
    int i;
    struct nr_buf_t *p = &nr_buf;
    for(i = 0; i < PART_LEN; i++) {
        p->xbuf[i + PART_LEN] = ains4_rdft.input[i];
    }
    for(i = 0; i < PART_LEN; i++) {
        ML0(p->xbuf[i], sqrt_Hanning[i]);
        p->fft[i] = MLZ(HANN_SHIFT);
        ML0(p->xbuf[PART_LEN + i], sqrt_Hanning[PART_LEN - i]);
        p->fft[PART_LEN + i] = MLZ(HANN_SHIFT);
    }
    fft_256((s32*)p->fft);
    ains4_mono_process((s32*)p->fft);
    ifft_256((s32*)p->fft);

	for(i = 0; i < PART_LEN; i++) {
		// Do we need saturation ?
		p->fft[i] = AEC_SPL_SAT(AEC_SPL_WORD16_MAX, p->fft[i], AEC_SPL_WORD16_MIN);
        ML0(p->fft[i], sqrt_Hanning[i]);
		p->fft[i] = MLZ(HANN_SHIFT)+p->outBuf[i];
        ains4_rdft.fft_output[i] = (short)AEC_SPL_SAT(AEC_SPL_WORD16_MAX, p->fft[i], AEC_SPL_WORD16_MIN);
		ML0(p->fft[PART_LEN + i], sqrt_Hanning[PART_LEN - i]);
		p->outBuf[i] = MLZ(HANN_SHIFT);
	}
	memcpy(p->xbuf, p->xbuf + PART_LEN, 2 * PART_LEN);
}

AT(.ains4_text.ains4)
void ains4_process(s16 *buf)
{
    for (int i = 0; i < PART_HALF_LEN; i++) {
        ains4_rdft.input[ains4_rdft.iptr * PART_HALF_LEN + i]  = buf[i];
    }
    ains4_rdft.iptr++;
    if (ains4_rdft.iptr >= 2) {
        ains4_rdft.iptr = 0;
        memcpy(ains4_rdft.output, ains4_rdft.fft_output, PART_LEN * 2);
        asr_kws_process();
        ains4_rdft.optr = 0;
    }

    memcpy(buf, &ains4_rdft.output[ains4_rdft.optr * PART_HALF_LEN], PART_HALF_LEN * 2);
    ains4_rdft.optr++;
    if (ains4_rdft.optr >= 2) {
        ains4_rdft.optr = 0;
    }
}

AT(.text.ains4_init)
void ains4_init(void)
{
    if (ains4_cb.init == 0) {
        memset(&ains4_rdft, 0, sizeof(ains4_rdft_t));
        memset(&nr_buf, 0, sizeof(struct nr_buf_t));

        ains4_cb.sysclk = sys_clk_get_cur();
        printf("ains4_cb.sysclk:%d\n",ains4_cb.sysclk);

        if (ains4_cb.sysclk < SYS_160M) {
            sys_clk_set(SYS_160M);
        }

        ains4_rdft.optr = 1;

        // lock_code_alg((u32)&__code_start_alg, (u32)&__code_end_alg);

        ains4_mono_init(AINS4_NR_SUPPRESS, AINS4_PRIOR_OPT_IDX);

        ains4_cb.init = 1;
    }
}

AT(.text.ains4_exit)
void ains4_exit(void)
{
    if (ains4_cb.init) {
        ains4_cb.init = 0;
        // unlock_code_alg();
        sys_clk_set(ains4_cb.sysclk);
    }
}

#endif  //(AINS4_EN)