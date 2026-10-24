#include "include.h"
#include "thr_track_flash.h"

#ifndef THR_TRACK_FLASH_PART_LEN
#define THR_TRACK_FLASH_PART_LEN	  128    //128个样点一帧
#endif

#ifndef THR_TRACK_FLASH_BUF_LEN
#define THR_TRACK_FLASH_BUF_LEN	      6      //6帧输出一次鼓点判断值（(1/8000) * 128 * 6 = 96ms）
#endif

#ifndef THR_TRACK_FLASH_DUMP_EN
#define THR_TRACK_FLASH_DUMP_EN       0      //获取基础阈值（enr_nr_thr_base）
#endif

static thr_track_flash_cb_t thr_track_flash_cb AT(.thr_track_flash.small);
static s32 data_max_mean_s[THR_TRACK_FLASH_BUF_LEN] AT(.thr_track_flash.small);
static u8 buf_s[THR_TRACK_FLASH_BUF_LEN] AT(.thr_track_flash.small);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//鼓点产生条件如下：
// if(enr_dnr_sm_log > enr_thr_final_out){
// 	 if((enr_cy > st->enr_nr_thr_base2)&&(enr_dnr_mc_sm_log > st->enr_nr_thr_mc)){
// 	 	enr_flag = 1;  //输出鼓点标志
// 	 } else {
// 		enr_flag = 0;
//   }
// }
//
//变量解释：
// 1.enr_dnr_sm_log > enr_thr_final_out -->  enr_dnr_sm_log（当前平滑平均能量），enr_thr_final_out = 算法值 - (系数*sensitity)。
//                                           用户可以调整sensitity变量影响鼓点的产生，开启THR_TRACK_FLASH_DUMP_EN可以查看enr_dnr_diff（实际是enr_dnr_sm_log-enr_thr_final_out）。
// 2.enr_cy > st->enr_nr_thr_base2  -->  enr_cy（前后能量的差值），用户可以调整enr_nr_thr_base2变量影响鼓点的产生。
//
// 3.enr_dnr_mc_sm_log > st->enr_nr_thr_mc  -->  enr_dnr_mc_sm_log（当前的负噪声值），用户可以调整enr_nr_thr_mc变量影响鼓点产生，开启THR_TRACK_FLASH_DUMP_EN可以查看enr_nr_thr_mc。
//
//enr_nr_thr_mc通常在产品研发阶段配好，sensitity和enr_nr_thr_base2根据灵敏度要求配置。
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//用户需要修改参数1~5
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//使能THR_TRACK_FLASH_DUMP_EN会自动打印实时的enr_nr_thr_base、enr_nr_thr_mc、enr_dnr_diff
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AT(.text.thr_track_flash_small_init)
void thr_track_flash_init(void)
{
	thr_track_flash_cb.dump_en                  = THR_TRACK_FLASH_DUMP_EN;

	thr_track_flash_cb.part_len2                = (THR_TRACK_FLASH_PART_LEN << 1);
	thr_track_flash_cb.rand_len                 = THR_TRACK_FLASH_PART_LEN;
	thr_track_flash_cb.buf_len                  = THR_TRACK_FLASH_BUF_LEN;
	thr_track_flash_cb.data_max_mean_s          = data_max_mean_s;
	thr_track_flash_cb.buf_s                    = buf_s;

	thr_track_flash_cb.enr_mean_max_en			= 1;//1:mean   0:max
	thr_track_flash_cb.enr_mean_max_en2		    = 0;//1:mean   0:max
	thr_track_flash_cb.pre_en					= 0;

	//参数1
	thr_track_flash_cb.sensitity				= 50;     //灵敏度，范围 s16

	//参数2
	thr_track_flash_cb.enr_nr_thr				= -31;    //dB 最大正噪声值，通常比安静噪声高9dB

	//参数3
	thr_track_flash_cb.enr_nr_thr_mc		    = -20-2;    //dB 最小负噪声值
	//thr_track_flash_cb.enr_nr_thr2				= -40;//dB

	//参数4
	thr_track_flash_cb.enr_nr_thr_base			= -38-2;  //dB 安静环境的底噪值(样机自身底噪)，通常取出现频率最多的值然后再减2dB

	//参数5
	thr_track_flash_cb.enr_nr_thr_base2		    = 32768/2;//dB Q15 鼓点阈值， 范围0~s32，32768表示1dB，32768/2表示0.5dB

	thr_track_flash_cb.enr_nr_ry				= 32768;//dB Q15->1dB

	thr_track_flash_cb.smooth_v				    = 26216; // 0.8f
	thr_track_flash_cb.smooth_max_v			    = 26216; // 0.8f
	thr_track_flash_cb.cal_cnt					= 25;//1 frame 8ms 50��-��25*8=0.2s
	thr_track_flash_cb.min_range				= 76;
	thr_track_flash_cb.enr_nr_ry_mcra		    = 32768;//dB Q15->3dB
	thr_track_flash_nr_init(&thr_track_flash_cb);
}
