/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <math.h>
#include <string.h>
#include <osif.h>
#include <bt_debug.h>
#include "bt_audio_resample.h"

#define SRC_MIN_RATIO_DIFF      (1e-20)
#define RESAMPLE_BUFFER_SIZE    (8 * 16 * 2 * 15) * (48000 / 16000) * 2

rtk_bt_audio_resample_t *rtk_bt_audio_resample_alloc(uint32_t in_rate, uint32_t out_rate, uint32_t in_channels, uint32_t out_channels, uint32_t input_frames)
{
	rtk_bt_audio_resample_t *p_bt_resample = NULL;

	p_bt_resample = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_audio_resample_t));
	if (!p_bt_resample) {
		BT_LOGE("%s allocate bt audio resample struct fail \r\n", __func__);
		return NULL;
	} else {
		memset((void *)p_bt_resample, 0, sizeof(rtk_bt_audio_resample_t));
	}
	p_bt_resample->init_input_frames = input_frames;
	p_bt_resample->init_output_frames = (uint32_t)((float)out_rate / (float)in_rate + 1) * input_frames;
	p_bt_resample->out_buffer_size = p_bt_resample->init_output_frames * out_channels * (16 / 8);
	p_bt_resample->out_buffer = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, p_bt_resample->out_buffer_size);
	if (!p_bt_resample->out_buffer) {
		BT_LOGE("%s allocate out buffer fail \r\n", __func__);
		goto fail;
	}
	p_bt_resample->p_buffer = RingBuffer_Create(NULL, RESAMPLE_BUFFER_SIZE, LOCAL_RINGBUFF, 1);
	if (!p_bt_resample->p_buffer) {
		BT_LOGE("%s allocate bt audio resample p_buffer fail \r\n", __func__);
		goto fail;
	}

	p_bt_resample->in_channels = in_channels;
	p_bt_resample->out_channels = out_channels;
	p_bt_resample->in_rate = in_rate;
	p_bt_resample->out_rate = out_rate;
	p_bt_resample->in_frame_size = 16 / 8 * in_channels;
	p_bt_resample->out_frame_size = 16 / 8 * out_channels;
	p_bt_resample->src_ratio = (double)out_rate / (double)in_rate;
	p_bt_resample->last_ratio = p_bt_resample->src_ratio;
	return p_bt_resample;

fail:
	if (p_bt_resample->p_buffer) {
		RingBuffer_Destroy(p_bt_resample->p_buffer);
		p_bt_resample->p_buffer = NULL;
	}
	if (p_bt_resample->out_buffer) {
		osif_mem_free((void *)p_bt_resample->out_buffer);
	}
	if (p_bt_resample) {
		osif_mem_free((void *)p_bt_resample);
	}

	return NULL;
}

void rtk_bt_audio_resample_free(rtk_bt_audio_resample_t *p_bt_resample)
{
	if (!p_bt_resample) {
		BT_LOGE("%s p_bt_resample is NULL \r\n", __func__);
		return;
	}
	if (p_bt_resample->p_buffer) {
		RingBuffer_Destroy(p_bt_resample->p_buffer);
		p_bt_resample->p_buffer = NULL;
	}
	if (p_bt_resample->out_buffer) {
		osif_mem_free((void *)p_bt_resample->out_buffer);
	}
	osif_mem_free((void *)p_bt_resample);
}

static short in_buffer[(48000 / 1000 * 10) * 2] = {0};

static void bt_audio_input_data_channel_convert(rtk_bt_audio_resample_t *p_resample, short *in, short *out, uint32_t frames)
{
	/* channel re-allocation */
	if (p_resample->in_channels == 1 && p_resample->out_channels == 2) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* copy mono data to right channel */
			out[i * 2] = in[i];
			out[i * 2 + 1] = out[i * 2];
		}
	} else if (p_resample->in_channels == 2 && p_resample->out_channels == 1) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* default use left channel */
			out[i] = in[i * 2];
		}
	} else {
		for (uint32_t i = 0; i < frames * p_resample->out_channels; i ++) {
			out[i] = in[i];
		}
	}
}

static inline double fmod_one(double x)
{
	double res;
	res = x - lrint(x);
	if (res < 0.0) {
		return res + 1.0;
	}

	return res;
}

static uint32_t resample_process_s16(rtk_bt_audio_resample_t *presample, int16_t *input, int16_t *output, uint32_t input_frames, uint32_t channels)
{
	long in_count, in_used;
	long out_count, out_gen;
	double temp_src_ratio, input_index, rem;
	uint32_t ch = channels;
	int output_frames = 0;

	if (!presample || !input || !output) {
		BT_LOGE("%s input or output or presample is NULL \r\n", __func__);
		return 0;
	}
	if (presample->in_rate == presample->out_rate) {
		memcpy((void *)output, (void *)input, input_frames * channels * 16 / 8);
		return input_frames;
	}
	output_frames = (int)((float)presample->out_rate / (float)presample->in_rate + 1) * input_frames;
	in_count = input_frames * channels;
	out_count = output_frames * channels;
	in_used = out_gen = 0;
	temp_src_ratio = presample->last_ratio;
	input_index = presample->last_position;

	// BT_LOGA("%s in_count: %d, out_count: %d, in_used: %d, src_ratio: %f, input_index: %f \r\n ", __func__,
	//  in_count, out_count, in_used, temp_src_ratio, input_index);

	while (input_index < 1.0 && out_gen < out_count) {
		if (in_used + channels * (1.0 + input_index) >= in_count) {
			break;
		}
		if (out_count > 0 && fabs(presample->last_ratio - presample->src_ratio) > SRC_MIN_RATIO_DIFF) {
			temp_src_ratio = presample->last_ratio + out_gen * (presample->src_ratio - presample->last_ratio) / out_count;
		}
		for (ch = 0 ; ch < channels ; ch++) {
			output[out_gen] = (float)(presample->last_value[ch] + input_index * (input[ch] - presample->last_value[ch]));
			out_gen ++;
		};
		/* Figure out the next index. */
		input_index += 1.0 / temp_src_ratio;
	};
	rem = fmod_one(input_index);
	in_used += channels * lrint(input_index - rem);
	input_index = rem;
	// BT_LOGA("%s Main in_count: %d, out_count: %d, in_used: %d, src_ratio: %f, input_index: %f \r\n ", __func__,
	//  in_count, out_count, in_used, temp_src_ratio, input_index);
	/* Main processing loop. */
	while (out_gen < out_count && in_used + channels * input_index < in_count) {
		if (out_count > 0 && fabs(presample->last_ratio - presample->src_ratio) > SRC_MIN_RATIO_DIFF) {
			temp_src_ratio = presample->last_ratio + out_gen * (presample->src_ratio - presample->last_ratio) / out_count;
		}

		if (in_used < (long)channels && input_index < 1.0) {
			BT_LOGE("%s Something unexpected happened in_used : %d     channels : %d     input_index : %f \r\n", __func__, in_used, (int)channels, input_index);
			return 0;
		};

		for (ch = 0 ; ch < channels ; ch++) {
			output[out_gen] = (float)(input[in_used - channels + ch] + input_index * (input[in_used + ch] - input[in_used - channels + ch]));
			out_gen ++;
		};
		/* Figure out the next index. */
		input_index += 1.0 / temp_src_ratio;
		rem = fmod_one(input_index);
		in_used += channels * lrint(input_index - rem);
		input_index = rem;
	};
	// BT_LOGA("%s Out in_count: %d, out_count: %d, in_used: %d, src_ratio: %f, input_index: %f \r\n ", __func__,
	//  in_count, out_count, in_used, temp_src_ratio, input_index);
	if (in_used > in_count) {
		input_index += (in_used - in_count) / channels;
		in_used = in_count;
	};
	presample->last_position = input_index;
	if (in_used > 0) {
		for (ch = 0 ; ch < channels ; ch++) {
			presample->last_value[ch] = input[in_used - channels + ch];
		}
	}
	/* Save current ratio rather then target ratio. */
	presample->last_ratio = temp_src_ratio;
	presample->output_generated = out_gen / channels;

	return presample->output_generated;
}

uint32_t rtk_bt_audio_resample_available_data(rtk_bt_audio_resample_t *p_bt_resample)
{
	if (!p_bt_resample) {
		BT_LOGE("%s input param error \r\n", __func__);
		return 0;
	}

	return RingBuffer_Available(p_bt_resample->p_buffer);
}

uint32_t rtk_bt_audio_resample_feed(rtk_bt_audio_resample_t *p_bt_resample, uint8_t *input, uint32_t input_frames_num)
{
	uint32_t input_frames = input_frames_num;
	uint32_t output_frames = 0;
	uint32_t output_bytes = 0;

	if (!p_bt_resample || !input || !input_frames_num) {
		BT_LOGE("%s input param error \r\n", __func__);
		return 0;
	}
	/* judge whether the output frames is larger than the pre allocated output buff size */
	output_frames = (uint32_t)(((float)p_bt_resample->out_rate / (float)p_bt_resample->in_rate) * (float)input_frames_num) + 1;
	if (output_frames > p_bt_resample->init_output_frames) {
		BT_LOGE("%s input frames(%d) is overflow \r\n", __func__, input_frames_num);
		return 0;
	}
	/* do channel re-allocation */
	bt_audio_input_data_channel_convert(p_bt_resample, (short *)input, in_buffer, input_frames_num);
	if (p_bt_resample->out_rate != p_bt_resample->in_rate) {
		output_frames = resample_process_s16(p_bt_resample, (int16_t *)in_buffer, (int16_t *)p_bt_resample->out_buffer, input_frames_num, p_bt_resample->out_channels);
	} else {
		memcpy((void *)p_bt_resample->out_buffer, (void *)in_buffer, input_frames_num * p_bt_resample->out_channels * (16 / 8));
		output_frames = input_frames_num;
	}
	output_bytes = output_frames * p_bt_resample->out_channels * (16 / 8);
	if (RingBuffer_Space(p_bt_resample->p_buffer) >= output_bytes) {
		RingBuffer_Write(p_bt_resample->p_buffer, (uint8_t *)p_bt_resample->out_buffer, output_bytes);
	} else {
		BT_LOGE("%s p_bt_resample ringbuffer is not enough \r\n", __func__);
		return 0;
	}

	return input_frames;
}

int16_t rtk_bt_audio_resample_read(rtk_bt_audio_resample_t *p_bt_resample, void *buffer, uint32_t size)
{
	if (!p_bt_resample || !buffer) {
		BT_LOGE("%s input param error \r\n", __func__);
		return 1;
	}
	if (RingBuffer_Available(p_bt_resample->p_buffer) < size) {
		BT_LOGE("%s not enough resampled data \r\n", __func__);
		return -1;
	}
	RingBuffer_Read(p_bt_resample->p_buffer, buffer, size);

	return 0;
}