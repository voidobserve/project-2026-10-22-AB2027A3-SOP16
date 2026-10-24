#ifndef _API_CODEC_H
#define _API_CODEC_H

//wav
uint16_t wav_res_analize(u8 *head_buf, u8 *nch, u32 *spr, u16 *block_size);

//sbc
/* sampling frequency */
enum {
    SBC_FREQ_16000 = 0x00,
    SBC_FREQ_32000 = 0x01,
    SBC_FREQ_44100 = 0x02,
    SBC_FREQ_48000 = 0x03,
};

/* blocks */
enum {
    SBC_BLK_4  = 0x00,
    SBC_BLK_8  = 0x01,
    SBC_BLK_12 = 0x02,
    SBC_BLK_16 = 0x03,
};

/* subbands */
enum {
    SBC_SB_4 = 0x00,
    SBC_SB_8 = 0x01,
};

/* channel mode */
enum {
    SBC_MODE_MONO         = 0x00,
    SBC_MODE_DUAL_CHANNEL = 0x01,
    SBC_MODE_STEREO       = 0x02,
    SBC_MODE_JOINT_STEREO = 0x03,
};

/* allocation method */
enum {
    SBC_AM_LOUDNESS = 0x00,
    SBC_AM_SNR      = 0x01,
};
/**
  * @brief  Take the mono 16K sampling rate as an example
  * @brief  mSBC:
  * @brief  mSBC parameter cannot be changed, ilen = 120 * 2, olen = 57;
  *
  * @brief  SBC:
  * @brief  subbands_l = subbands ? 8 : 4;
  * @brief  blocks_l = 4 + (blocks * 4);
  * @brief  channels_l = mode ? 2 : 1;
  * @brief  SBC ilen = subbands_l * blocks_l * channels_l * 2;
  * @brief  SBC olen = 4 + (4 * subbands_l * channels_l) / 8  +  ((blocks_l * channels_l * bitpool) + 7) / 8;  //Ignore the decimal point
  *
  * @brief  mic sample points = ilen / 2; (max: 128)
  */
void sbc_encode_init(uint8_t freq, uint8_t blocks, uint8_t subbands, uint8_t mode, uint8_t allocation, uint8_t bitpool);
uint16_t sbc_encode_frame(uint8_t *ibuf, uint16_t ilen, uint8_t *obuf, uint16_t olen);
void msbc_encode_init(void);
uint16_t msbc_encode_frame(uint8_t *ibuf, uint16_t ilen, uint8_t *obuf, uint16_t olen);

/**
 * @brief  SBC decode
 * 
 * @brief  is_msbc:  1 -- msbc, 0 -- sbc.
 * @brief  ibuf: SBC encoded buf
 * @brief  ilen: SBC encoded buf len
 * @brief  obuf: SBC decoded output buf
 * 
 * @brief  return: decode output buf sample points. (max: 128)
 */
void sbc_decode_init(bool is_msbc);
uint16_t sbc_decode_frame(uint8_t *ibuf, uint16_t ilen, uint8_t *obuf);

//adpcm
/**
  * @brief  ima adpcm encode compression ratio 4:1
  * @brief  encode output buf format:2byte prediction sample, 1byte index, 1byte reserve, nbyte data
  * @brief  decode input buf format:2byte prediction sample, 1byte index, 1byte reserve, nbyte data
  * 
  * @brief  ima adpcm need one more sample point. reference routine bsp_sdadc_voice.c
  * 
  * @param  p_dst: output buf
  * @param  p_src: input buf
  * @param  sample_len:sample points
  */
void adpcm_decode_block(uint8_t *p_dst,uint8_t *p_src,uint32_t sample_len);
void adpcm_encode_block(uint8_t *p_dst, uint8_t *p_src, uint32_t sample_len);
void adpcm_decode_block_big(uint8_t *p_dst, uint8_t *p_src, uint32_t sample_len);
void adpcm_encode_block_big(uint8_t *p_dst, uint8_t *p_src, uint32_t sample_len);
void adpcm_sample_idx_set(int16_t idx);
void adpcm_sample_idx_reset(void);
void adpcm_presample_set(int32_t pcm_16);
uint16_t adpcm_sample_idx_get(void);
int16_t adpcm_presample_get(void);

//opus
/**
  * @brief  opus encode initialization, 16bit, 20ms one frame
  * @brief  opus encode requires high computing power, so the system needs to raise the main frequency to more than 120M
  * @param  samplerate: sampling rate, only support 8000 or 16000
  * @param  nch: number of channels,only support 1 channel now
  * @param  bit_rate:16000 or 32000, 16000-->1:16  32000-->1:8
  * @return true or false
  */
bool opus_enc_init(u32 samplerate, u32 nch, u32 bit_rate);

/**
  * @brief  opus encode exit
  * @return true or false
  */
bool opus_enc_exit(void);

/**
  * @brief  opus encode process
  * @param  pcm: Data to be encoded
            At a sample rate of 16K, 16bit, 20ms one frame, pcm_size = (16 * 20 * 2)byte
            Ensure that there is a sufficient length of data
  * @param  out: opus encoded data, out_len = pcm_size / 16
            Ensure that there is a sufficient length of data
  */
int32_t opus_enc_process(int16_t *pcm, u8 *out);


/**
  * @brief  mp3 decode
  */
bool mp3_dec_frame(void);
void mp3_music_pcm_kick(void);
u8 mp3_music_dec_sta_get(void);
void mp3_music_dec_sta_set(u8 sta);
bool mp3_res_play_kick(u32 addr, u32 len);
void mp3_res_play_exit(void);

#endif // _API_CODEC_H
