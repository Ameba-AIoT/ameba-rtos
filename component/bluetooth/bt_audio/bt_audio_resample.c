/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <osif.h>
#include <bt_debug.h>
#include "bt_audio_resample.h"

#define SRC_MIN_RATIO_DIFF      (1e-20)

#if defined (RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER) && RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER
static uint32_t *tpdf_generators;
#endif

static inline double fmod_one(double x)
{
	double res;
	res = x - lrint(x);
	if (res < 0.0) {
		return res + 1.0;
	}

	return res;
}

#if defined (RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER) && RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER
static void tpdf_dither_init(int channels)
{
	int generator_bytes = channels * sizeof(uint32_t);
	unsigned char *seed = osif_mem_alloc(RAM_TYPE_DATA_ON, generator_bytes);
	uint32_t random = 0x31415926;

	tpdf_generators = (uint32_t *)seed;

	while (generator_bytes--) {
		*seed++ = random >> 24;
		random = ((random << 4) - random) ^ 1;
		random = ((random << 4) - random) ^ 1;
		random = ((random << 4) - random) ^ 1;
	}
}

static inline double tpdf_dither(int channel, int type)
{
	uint32_t random = tpdf_generators[channel];
	random = ((random << 4) - random) ^ 1;
	random = ((random << 4) - random) ^ 1;
	uint32_t first = type ? tpdf_generators [channel] ^ ((int32_t) type >> 31) : ~random;
	random = ((random << 4) - random) ^ 1;
	random = ((random << 4) - random) ^ 1;
	random = ((random << 4) - random) ^ 1;
	tpdf_generators [channel] = random;
	return (((first >> 1) + (random >> 1)) / 2147483648.0) - 1.0;
}

static void tpdf_dither_free(void)
{
	if (tpdf_generators) {
		osif_mem_free(tpdf_generators);
	}
}
#endif

rtk_bt_audio_resample_t *rtk_bt_audio_resample_alloc(float in_rate, float out_rate, uint8_t in_channels, uint8_t out_channels, uint32_t input_frames)
{
	rtk_bt_audio_resample_t *presample = NULL;

	presample = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_audio_resample_t));
	if (!presample) {
		BT_LOGE("%s allocate bt audio resample struct fail \r\n", __func__);
		return NULL;
	} else {
		memset((void *)presample, 0, sizeof(rtk_bt_audio_resample_t));
	}
	presample->in_rate = in_rate;
	presample->out_rate = out_rate;
	presample->in_channels = in_channels;
	presample->out_channels = out_channels;
	presample->in_frame_size = 16 / 8 * presample->in_channels;
	presample->out_frame_size = 16 / 8 * presample->out_channels;
	presample->gain = RTK_BT_AUDIO_FILTER_DEFAULT_GAIN;
	presample->input_samples = input_frames;
	presample->src_ratio = (double)(out_rate / in_rate);
	presample->last_ratio = presample->src_ratio;
#if defined (RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER) && RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER
	tpdf_dither_init(channels);
#endif

	return presample;
}

void rtk_bt_audio_resample_free(rtk_bt_audio_resample_t *presample)
{
	if (!presample) {
		BT_LOGE("%s presample is NULL \r\n", __func__);
		return;
	}
	osif_mem_free(presample);
#if defined (RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER) && RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER
	tpdf_dither_free();
#endif
}

uint16_t rtk_bt_audio_bq_config(rtk_bt_audio_biquad_t *filter, int type, float gain, unsigned long freq_center, unsigned long sample_rate, double bandwidth)
{
	double A, omega, sn, cs, alpha, beta;
	double a0, a1, a2, b0, b1, b2;

	if (!filter) {
		BT_LOGE("%s input bq filter is NULL \r\n", __func__);
		return 1;
	}
	/* setup variables */
	A = pow(10, gain / 40);
	omega = 2 * M_PI * freq_center / sample_rate;
	sn = sin(omega);
	cs = cos(omega);
	alpha = sn * sinh(M_LN2 / 2 * bandwidth * omega / sn);
	beta = sqrt(A + A);

	switch (type) {

	case RTK_BT_AUDIO_LPF: {
		b0 = (1 - cs) / 2;
		b1 = 1 - cs;
		b2 = (1 - cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
	}
	break;

	case RTK_BT_AUDIO_HPF: {
		b0 = (1 + cs) / 2;
		b1 = -(1 + cs);
		b2 = (1 + cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
	}
	break;

	case RTK_BT_AUDIO_BPF: {
		b0 = alpha;
		b1 = 0;
		b2 = -alpha;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
	}
	break;

	case RTK_BT_AUDIO_NOTCH: {
		b0 = 1;
		b1 = -2 * cs;
		b2 = 1;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
	}
	break;

	case RTK_BT_AUDIO_PEQ: {
		b0 = 1 + (alpha * A);
		b1 = -2 * cs;
		b2 = 1 - (alpha * A);
		a0 = 1 + (alpha / A);
		a1 = -2 * cs;
		a2 = 1 - (alpha / A);
	}
	break;

	case RTK_BT_AUDIO_LSH: {
		b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
		b1 = 2 * A * ((A - 1) - (A + 1) * cs);
		b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
		a0 = (A + 1) + (A - 1) * cs + beta * sn;
		a1 = -2 * ((A - 1) + (A + 1) * cs);
		a2 = (A + 1) + (A - 1) * cs - beta * sn;
	}
	break;

	case RTK_BT_AUDIO_HSH: {
		b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
		b1 = -2 * A * ((A - 1) + (A + 1) * cs);
		b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
		a0 = (A + 1) - (A - 1) * cs + beta * sn;
		a1 = 2 * ((A - 1) - (A + 1) * cs);
		a2 = (A + 1) - (A - 1) * cs - beta * sn;
	}
	break;

	default:
		BT_LOGE("%s unkonw filter type %d \r\n", __func__, type);
		return 1;
	}
	filter->a0 = b0 / a0;
	filter->a1 = b1 / a0;
	filter->a2 = b2 / a0;
	filter->a3 = a1 / a0;
	filter->a4 = a2 / a0;
	filter->x1 = filter->x2 = 0.0F;
	filter->y1 = filter->y2 = 0.0F;

	return 0;
}

void rtk_bt_audio_biquad_apply(rtk_bt_audio_biquad_t *f, float *buffer, int num_samples, int gap)
{
	while (num_samples --) {
		float sum = (*buffer * f->a0) + (f->x1 * f->a1) + (f->x2 * f->a2) - (f->a3 * f->y1) - (f->a4 * f->y2);
		f->y2 = f->y1;
		f->x2 = f->x1;
		f->x1 = *buffer;
		*buffer = f->y1 = sum;
		buffer += gap;
	}
}

#if defined(RTK_BT_AUDIO_RESAMPLE_F32) && RTK_BT_AUDIO_RESAMPLE_F32
static uint32_t resample_process_f32(rtk_bt_audio_resample_t *presample, float *input, float *output, uint32_t in_rate, uint32_t out_rate,
									 uint32_t input_frames, uint32_t channels)
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
	output_frames = (int)((float)out_rate / (float)in_rate + 1) * input_frames;
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
#else
static uint32_t resample_process_s16(rtk_bt_audio_resample_t *presample, int16_t *input, int16_t *output, uint32_t in_rate, uint32_t out_rate,
									 uint32_t input_frames, uint32_t channels)
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
	output_frames = (int)((float)out_rate / (float)in_rate + 1) * input_frames;
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
#endif

#if defined(RTK_BT_AUDIO_RESAMPLE_F32) && RTK_BT_AUDIO_RESAMPLE_F32
static float in_buffer[(48000 / 1000 * 10) * 2] = {0};
static float out_buffer[(48000 / 1000 * 10) * 2] = {0};

static void bt_audio_input_data_short_to_float(rtk_bt_audio_resample_t *presample, short *in, float *out, uint32_t frames)
{
	/* channel re-allocation */
	if (presample->in_channels == 1 && presample->out_channels == 2) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* copy mono data to right channel */
			out[i * 2] = (float)(in[i] / (1.0 * 0x8000));
			out[i * 2 + 1] = out[i * 2];
		}
	} else if (presample->in_channels == 2 && presample->out_channels == 1) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* default use left channel */
			out[i] = (float)(in[i * 2] / (1.0 * 0x8000));
		}
	} else {
		for (uint32_t i = 0; i < frames * presample->out_channels; i ++) {
			out[i] = (float)(in[i] / (1.0 * 0x8000));
		}
	}
}

static void bt_audio_ouput_data_float_to_short(const float *in, short *out, int len)
{
	double scaled_value;

	while (len) {
		len --;
		scaled_value = in[len] * (8.0 * 0x10000000);
		if (scaled_value >= (1.0 * 0x7FFFFFFF)) {
			out[len] = 32767;
			continue;
		};
		if (scaled_value <= (-8.0 * 0x10000000)) {
			out[len] = -32768;
			continue;
		};
		out[len] = (short)(lrint(scaled_value) >> 16);
	};
}
#else
static short in_buffer[(48000 / 1000 * 10) * 2] = {0};

static void bt_audio_input_data_channel_convert(rtk_bt_audio_resample_t *presample, short *in, short *out, uint32_t frames)
{
	/* channel re-allocation */
	if (presample->in_channels == 1 && presample->out_channels == 2) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* copy mono data to right channel */
			out[i * 2] = in[i];
			out[i * 2 + 1] = out[i * 2];
		}
	} else if (presample->in_channels == 2 && presample->out_channels == 1) {
		for (uint32_t i = 0; i < frames; i ++) {
			/* default use left channel */
			out[i] = in[i * 2];
		}
	} else {
		for (uint32_t i = 0; i < frames * presample->out_channels; i ++) {
			out[i] = in[i];
		}
	}
}
#endif

uint32_t rtk_bt_audio_resample_entry(rtk_bt_audio_resample_t *presample, rtk_bt_audio_biquad_t *bq_t, uint8_t *input, uint32_t input_frames_num,
									 uint8_t *output, uint32_t in_rate, uint32_t out_rate)
{
#if !defined (RTK_BT_AUDIO_RESAMPLE_PRE_FILTER) || !RTK_BT_AUDIO_RESAMPLE_PRE_FILTER && \
    !defined (RTK_BT_AUDIO_RESAMPLE_POST_FILTER) || !RTK_BT_AUDIO_RESAMPLE_POST_FILTER
	(void)bq_t;
#endif
	uint32_t out_frames = 0;

#if defined(RTK_BT_AUDIO_RESAMPLE_F32) && RTK_BT_AUDIO_RESAMPLE_F32
	memset((void *)in_buffer, 0, sizeof(in_buffer));
	memset((void *)out_buffer, 0, sizeof(out_buffer));
	/* convert to 32-bit float and do channels re-allocation */
	bt_audio_input_data_short_to_float(presample, (short *)input, in_buffer, input_frames_num);
#if defined (RTK_BT_AUDIO_RESAMPLE_PRE_FILTER) && RTK_BT_AUDIO_RESAMPLE_PRE_FILTER
	/* do pre filter */
	for (uint8_t i = 0; i < presample->out_channels; i ++) {
		rtk_bt_audio_biquad_apply(bq_t, in_buffer + i, input_frames_num, presample->out_channels);
	}
#endif
	out_frames = resample_process_f32(presample, (float *)in_buffer, (float *)out_buffer, in_rate, out_rate, input_frames_num, presample->out_channels);
#if defined (RTK_BT_AUDIO_RESAMPLE_POST_FILTER) && RTK_BT_AUDIO_RESAMPLE_POST_FILTER
	/* do post filter */
	for (uint8_t i = 0; i < presample->out_channels; i ++) {
		rtk_bt_audio_biquad_apply(bq_t, out_buffer + i, out_frames, presample->out_channels);
	}
#endif
	/* convert to S16 */
	bt_audio_ouput_data_float_to_short(out_buffer, (short *)output, out_frames * presample->out_channels);
#else
	/* do channel re-allocation */
	bt_audio_input_data_channel_convert(presample, (short *)input, in_buffer, input_frames_num);
	out_frames = resample_process_s16(presample, (int16_t *)in_buffer, (int16_t *)output, in_rate, out_rate, input_frames_num, presample->out_channels);
#endif

	return out_frames;
}