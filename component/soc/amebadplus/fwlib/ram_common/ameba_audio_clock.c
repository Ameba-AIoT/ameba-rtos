/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_audio_clock.h"

static const char *const TAG = "AUDIO";

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AUDIO
  * @brief AUDIO driver modules
  * @{
  */

/** @defgroup AUDIO_CLOCK
* @brief AUDIO_CLOCK driver modules
* @{
*/

/* Exported constants ------------------------------------------------------------*/
/** @defgroup AUDIO_CLOCK_Exported_Constants AUDIO_CLOCK Exported Constants
  * @{
  */

const u32 i2s_clock_source[3] = {PLL_CLOCK_98P304M, PLL_CLOCK_45P1584M, I2S_CLOCK_XTAL40M};
/**
* @}
*/

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_CLOCK_Exported_Functions AUDIO_CLOCK Exported Functions
  * @{
  */

/**
  * @brief  Determine whether the clock can be divided normally.
  * @param	clock: audio clock.
  * @param  sr: sport sample rate.
  * @param  chn_len: sport channel length.
  * @param  chn_cnt: sport channel number.
  * @retval ni_mi_found:0/1
  */
bool is_sport_ni_mi_supported(u32 clock, u32 sr, u32 chn_len, u32 chn_cnt)
{
	int ni = 1;
	int mi = 1;
	int max_ni = 32767;
	int max_mi = 65535;
	bool ni_mi_found = 0;

	for (; ni <= max_ni ; ni++) {
		if (clock * ni % (chn_cnt * chn_len * sr) == 0) {
			mi = clock * ni / (chn_cnt * chn_len * sr);
			//printf("check founded: ni : %d, mi: %d \n", ni, mi);
			if (mi < 2 * ni) {
				//printf("mi <= ni, check fail, try another pll divider or sport mclk divider \n");
				break;
			}
			if (mi <= max_mi) {
				//printf("check ni : %d, mi: %d success\n", ni, mi);
				ni_mi_found = 1;
				break;
			}
		}
	}

	return ni_mi_found;
}

/**
  * @brief  Choose the appropriate clock according to the corresponding audio parameters.
  * @param  clock_sel: select PLL clock or XTAL clock.
  * @param  initparams: i2s init parameters: sr,channel length, channel number, codec_multiplier_with_rate and sport_mclk_fixed_max.
  * @param  params: audio clock parameters: clock and MCLK divider.
  * @retval None
  */
void Audio_Clock_Choose(u32 clock_sel, AUDIO_InitParams *initparams, AUDIO_ClockParams *params)
{
	u32 clock_index = 0;
	bool choose_done = 0;
	u32 NI = 1;
	u32 MI = 1;
	u32 clock = 0;

	if (clock_sel == PLL_CLK) {
		if (initparams->codec_multiplier_with_rate && initparams->sport_mclk_fixed_max) {
			RTK_LOGI(TAG, "not supported multi\n");
		}

		// external codec needs mclk to be codec_multiplier_with_rate * fs.
		if (initparams->codec_multiplier_with_rate) {
			for (; clock_index < PLL_CLOCK_MAX_NUM; clock_index++) {
				if (i2s_clock_source[clock_index] % (initparams->codec_multiplier_with_rate * initparams->sr) != 0) {
					continue;
				} else {

					MI = i2s_clock_source[clock_index] / initparams->codec_multiplier_with_rate / initparams->sr;
					if (is_sport_ni_mi_supported(i2s_clock_source[clock_index], initparams->sr, initparams->chn_len, initparams->chn_cnt)) {

						if (MI < 2) {
							choose_done = 0;

						} else {
							choose_done = 1;
						}
					}
					break;
				}

			}
		}

		// external codec needs mclk to be less than sport_mclk_fixed_max.
		if (initparams->sport_mclk_fixed_max) {

			if (!(initparams->sr % 4000)) {
				clock = PLL_CLOCK_98P304M;
			} else if (!(initparams->sr % 11025)) {
				clock = PLL_CLOCK_45P1584M;
				clock_index = 1;
			}

			for (; MI < 65536; MI = MI + 1) {
				if ((clock / MI  <= initparams->sport_mclk_fixed_max) && ((MI & (MI - 1)) == 0)) {
					if (is_sport_ni_mi_supported(clock, initparams->sr, initparams->chn_len, initparams->chn_cnt)) {
						choose_done = 1;
					}
					break;

				} else {
					continue;
				}
			}
		}

		if (!choose_done) {
			RTK_LOGE(TAG, "can't find proper clock for the current rate:%lu \n", initparams->sr);
			return;
		}

	} else {
		clock_index = 2;

		switch (initparams->codec_multiplier_with_rate) {
		case 512:
			if (!(initparams->sr % 4000)) {  //48k series
				if (initparams->sr <= SP_24K) {
					MI = 1250;
					NI = 2 * 8 * initparams->sr / 1000;
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 24k,please check sample rate!\n");
					return;
				}
			} else {									   //44.1k series
				if (initparams->sr <= SP_22P05K) {
					MI = 50000;
					NI = 2 * (441 * 8 * (initparams->sr / 11025));
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 22050,please check sample rate!\n");
					return;
				}
			}
			break;

		case 256:
			if (!(initparams->sr % 4000)) {
				if (initparams->sr <= SP_48K) {
					MI = 1250;
					NI = 8 * initparams->sr / 1000;
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 48k,please check sample rate!\n");
					return;
				}
			} else {
				if (initparams->sr <= SP_44P1K) {
					MI = 50000;
					NI = 441 * 8 * (initparams->sr / 11025);
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 44100,please check sample rate!\n");
					return;
				}
			}
			break;

		case 128:
			if (!(initparams->sr % 4000)) {
				if (initparams->sr <= SP_96K) {
					MI = 1250;
					NI = 8 * initparams->sr / 1000 / 2;
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 96k,please check sample rate!\n");
					return;
				}
			} else {
				if (initparams->sr <= SP_88P2K) {
					MI = 50000;
					NI = (441 * 8 * (initparams->sr / 11025)) / 2;
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 88200,please check sample rate!\n");
					return;
				}
			}
			break;

		case 64:
			if (!(initparams->sr % 4000)) {
				if (initparams->sr <= SP_192K) {
					MI = 1250;
					NI = 8 * initparams->sr / 1000 / 4;
				} else {
					RTK_LOGE(TAG, "error,sample rate should be less or equal than 192k,please check sample rate!\n");
					return;
				}
			} else {
				MI = 50000;
				NI = (441 * 8 * (initparams->sr / 11025)) / 4;
			}
			break;

		case 32:
			if (initparams->sr == SP_384K) {
				MI = 1250;
				NI = initparams->sr / 1000;
			} else {
				return;
			}
			break;

		default:
			RTK_LOGE(TAG, "please check multiple rate!\n");
			break;

		}
	}

	params->Clock = i2s_clock_source[clock_index];
	params->MCLK_NI = NI ;
	params->MCLK_MI = MI ;

}
/**
* @}
*/

/** @} */

/** @} */

/** @} */


