#include "include.h"
#include "driver_gpio.h"
#include "tbox_voice_sdadc.h"


#if (FUNC_TBOX_EN && PROD_TEST_MIC_EN)
#include "utils_ring_buffer.h"
#include "driver_sdadc.h"

#define SDADC_OUTPUT_SIZE   (512+2)       //DMA样点数 HALF DONE + ALL DONE
#define ENC_INPUT_SIZE      (256+1)       //压缩前样点数
#define ENC_OUTPUT_SIZE     (4+128)       //压缩后byte数，2byte presample + 1byte index + 1byte reserve + 128byte 偏差值
#define SDADC_SPR           SPR_8000

#define SDADC_BUF_CNT       4

#define SDADC_SKIP_FRAME     25         //丢弃mic刚启动时的不稳定数据

static bool sdadc_start_flag = 0;

extern void tbox_enc_buf_process(uint8_t *buffer, uint32_t size);

static uint16_t sdadc_output_buf[SDADC_OUTPUT_SIZE] AT(.tbox.voice_pcm_obuf);
static uint8_t enc_output_buf[ENC_OUTPUT_SIZE] AT(.tbox.voice_enc_obuf);

static ring_buf_t sdadc_pcm_ring_buf AT(.tbox.voice_ring_buf);
static uint8_t sdadc_pcm_buf[ENC_INPUT_SIZE * 2 * SDADC_BUF_CNT] AT(.tbox.voice_ring_buf);

static uint8_t sdadc_skip_frame = SDADC_SKIP_FRAME;

AT(.com_text.sdadc.proc)
void tbox_voice_sdadc_process(SDADC_DONE_TYPEDEF type)
{
    adpcm_encode_block_big((uint8_t *)enc_output_buf, (uint8_t *)utils_ring_buf_get(&sdadc_pcm_ring_buf, ENC_INPUT_SIZE*2), ENC_INPUT_SIZE);
    tbox_enc_buf_process((uint8_t *)enc_output_buf, ENC_OUTPUT_SIZE);
}

AT(.com_text.sdadc.isr)
void tbox_voice_sdadc_isr(void)
{
    if (sdadc_skip_frame) {
        sdadc_skip_frame--;
    }
    if (sdadc_get_flag(SDADC_FLAG_DMA_ALL_DONE) != RESET) {
        if (sdadc_skip_frame == 0) {
            utils_ring_buf_write(&sdadc_pcm_ring_buf, (u8*)&sdadc_output_buf[SDADC_OUTPUT_SIZE / 2], ENC_INPUT_SIZE*2);
            sdadc_done_proc_kick(SDADC_ALL_DONE);
        }
        sdadc_clear_flag(SDADC_FLAG_DMA_ALL_DONE);
    } else if (sdadc_get_flag(SDADC_FLAG_DMA_HALF_DONE) != RESET) {
        if (sdadc_skip_frame == 0) {
            utils_ring_buf_write(&sdadc_pcm_ring_buf, (u8*)sdadc_output_buf, ENC_INPUT_SIZE*2);
            sdadc_done_proc_kick(SDADC_HALF_DONE);
        }
        sdadc_clear_flag(SDADC_FLAG_DMA_HALF_DONE);
    }
}

AT(.text.prod_test.voice_sdadc_init)
void tbox_voice_sdadc_init(void)
{
    sdadc_init_typedef sdadc_cfg_init;

    utils_ring_buf_init(&sdadc_pcm_ring_buf, sdadc_pcm_buf, sizeof(sdadc_pcm_buf));

    sdadc_done_callback_set(tbox_voice_sdadc_process);
    clk_gate0_cmd(CLK_GATE0_SDADCL, CLK_EN);
    clk_sdadda_clk_set(CLK_SDADDA_CLK_XOSC24M_CLK_2PLL, 0);
    sdadc_cfg_init.sample_rate = SDADC_SPR;
    sdadc_cfg_init.mic_analog_cfg = MIC_ANALOG_CFG_NORMAL;
    sdadc_cfg_init.subtract_soft_dc_en = ENABLE;
    sdadc_cfg_init.get_dc_en = ENABLE;
    sdadc_cfg_init.remove_dc_en = ENABLE;
    sdadc_cfg_init.dig_gain_en = ENABLE;
    sdadc_init(&sdadc_cfg_init);
    sdadc_pic_config(tbox_voice_sdadc_isr, 0, SDADC_FLAG_DMA_HALF_DONE | SDADC_FLAG_DMA_ALL_DONE, ENABLE);
    sdadc_digital_gain_set(DIG_GAIN_3200); //输入增益
    mic_analog_gain_set(MIC_ANA_GAIN);
}

AT(.text.prod_test.voice_sdadc_start)
void tbox_voice_sdadc_start(void)
{
    sdadc_start_flag = 1;
    tbox_voice_sdadc_init();

    sdadc_skip_frame = SDADC_SKIP_FRAME;  //丢弃mic刚启动时的不稳定数据
#if MIC_BIAS_EN
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = GPIO_PIN_1;  //only
    gpio_init_structure.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(GPIOB_REG, &gpio_init_structure);
    mic_bias_open(MIC_BIAS_LEVEL, MIC_BIAS_RES);
#endif  //MIC_BIAS_EN
    sdadc_dma_cmd((u32)sdadc_output_buf, SDADC_OUTPUT_SIZE - 1, ENABLE);
    sdadc_cmd(ENABLE);
}

AT(.text.prod_test.voice_sdadc_stop)
void tbox_voice_sdadc_stop(void)
{
    if (sdadc_start_flag == 0) {
        return;
    } else {
        sdadc_start_flag  = 0;
    }
#if MIC_BIAS_EN
    gpio_init_typedef gpio_init_structure;
    gpio_init_structure.gpio_pin = GPIO_PIN_1;
    gpio_init_structure.gpio_dir = GPIO_DIR_INPUT;
    gpio_init_structure.gpio_fen = GPIO_FEN_GPIO;
    gpio_init_structure.gpio_fdir = GPIO_FDIR_SELF;
    gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_structure.gpio_pupd = GPIO_PUPD_NULL;
    gpio_init(GPIOB_REG, &gpio_init_structure);
    mic_bias_close();
#endif  //MIC_BIAS_EN
    sdadc_dma_cmd(0, 0, DISABLE);
    sdadc_cmd(DISABLE);

    sdadc_deinit();
}

#endif  //(FUNC_TBOX_EN && PROD_TEST_MIC_EN)
