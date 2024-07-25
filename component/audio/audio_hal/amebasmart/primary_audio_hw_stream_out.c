/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "audio_hw_compat.h"
#include <inttypes.h>

#include "ameba_audio_hw_usrcfg.h"
#include "ameba_audio_types.h"
#include "ameba_audio_stream_control.h"
#include "ameba_audio_stream_render.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_debug.h"
#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_stream_out.h"
#include "os_wrapper.h"
#include "audio_hw_params_handle.h"
#include "primary_audio_hw_card.h"

#define DEFAULT_OUT_SAMPLING_RATE 16000
#define NOIRQ_SHORT_PERIOD_SIZE   384
#define SHORT_PERIOD_SIZE         1024
#define SHORT_PERIOD_COUNT        4
#define AMPLIFIER_EN_PIN          "amp_pin"
#define DELAY_START               "delay_start"

#define DUMP_FRAME            192000
#define DUMP_ENABLE           0

StreamConfig stream_output_config = {
	.channels = 2,
	.rate = DEFAULT_OUT_SAMPLING_RATE,
	.format = AUDIO_HW_FORMAT_PCM_16_BIT,
	.period_size = SHORT_PERIOD_SIZE,
	.period_count = SHORT_PERIOD_COUNT,
};

struct PrimaryAudioHwStreamOut {
	struct AudioHwStreamOut stream;

	rtos_mutex_t lock;
	StreamConfig config;
	Stream *out_pcm;
	char *buffer;
	size_t buffer_frames;
	int standby;
	enum AudioHwFormat format;
	uint32_t channel_count;
	uint32_t sample_rate;
	uint32_t period_size;
	struct PrimaryAudioHwCard *pri_card;
	struct AudioHwPathDescriptor desc;
	uint32_t amp_pin;

	//max value should sync with ameba audio driver's total_counter_boundary.
	uint64_t written;
	bool delay_start;
};

static inline size_t PrimaryAudioHwStreamOutFrameSize(const struct AudioHwStreamOut *s)
{
	int chan_samp_sz;
	enum AudioHwFormat format = s->common.GetFormat(&s->common);

	if (AudioIsLinearPCM(format)) {
		chan_samp_sz = GetAudioBytesPerSample(format);
		return s->common.GetChannels(&s->common) * chan_samp_sz;
	}

	return sizeof(int8_t);
}

static uint32_t PrimaryGetStreamOutSampleRate(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	return out->sample_rate;
}

static int PrimarySetStreamOutSampleRate(struct AudioHwStream *stream, uint32_t rate)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	out->sample_rate = rate;
	return 0;
}

static size_t PrimaryGetStreamOutBufferSize(const struct AudioHwStream *stream)
{
	size_t ret = 0;
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	size_t size = (out->period_size * out->sample_rate) / out->sample_rate;
	size = ((size + 15) / 16) * 16;
	ret = size * PrimaryAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);
	return ret;
}

static uint32_t PrimaryGetStreamOutChannels(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;

	return out->channel_count;
}

static int PrimarySetStreamOutChannels(const struct AudioHwStream *stream, uint32_t channel)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;

	out->channel_count = channel;
	return 0;
}

static enum AudioHwFormat PrimaryGetStreamOutFormat(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;

	return out->format;
}

static int PrimarySetStreamOutFormat(struct AudioHwStream *stream, enum AudioHwFormat format)
{
	(void) stream;
	(void) format;
	return 0;
}

/* must be called with hw device and output stream mutexes locked */
static int DoStandbyOutput(struct PrimaryAudioHwStreamOut *out)
{
	if (!out->standby) {
		out->standby = 1;
		if (AUDIO_HW_AMPLIFIER_MUTE_ENABLE) {
			ameba_audio_stream_tx_set_amp_state(false);
		}

		ameba_audio_stream_tx_standby(out->out_pcm);
		ameba_audio_stream_buffer_flush(out->out_pcm->rbuffer);
	}
	return 0;
}

static int PrimaryStandbyStreamOut(struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	int status;

	//rtos_mutex_take(out->pri_card->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	status = DoStandbyOutput(out);
	rtos_mutex_give(out->lock);
	//rtos_mutex_give(out->pri_card->lock);

	return status;
}

static int PrimaryDumpStreamOut(const struct AudioHwStream *stream, int fd)
{
	(void) stream;
	(void) fd;
	return 0;
}

static uint32_t PrimaryGetStreamOutBufferStatus(struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	if (!out) {
		HAL_AUDIO_ERROR("PrimaryGetStreamOutBufferStatus stream_out is not initialized");
		return 0;
	}


	return ameba_audio_stream_tx_get_buffer_status(out->out_pcm);
}

static int PrimarySetStreamOutParameters(struct AudioHwStream *stream, const char *str_pairs)
{
	HAL_AUDIO_INFO("%s, keys = %s", __FUNCTION__, str_pairs);
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	struct string_cell *cells;
	cells = string_cells_create_from_str(str_pairs);
	int value;

	if (string_cells_has_key(cells, AMPLIFIER_EN_PIN)) {
		string_cells_get_int(cells, AMPLIFIER_EN_PIN, &value);
		out->amp_pin = value;
		ameba_audio_ctl_set_amp_pin(ameba_audio_get_ctl(), out->amp_pin);
	}

	if (string_cells_has_key(cells, DELAY_START)) {
		string_cells_get_int(cells, DELAY_START, &value);
		out->delay_start = value == 1 ? true : false;
		ameba_audio_stream_tx_set_delay_start(out->out_pcm, out->delay_start);
	}

	string_cells_destroy(cells);
	return 0;
}

static char *PrimaryGetStreamOutParameters(const struct AudioHwStream *stream, const char *keys)
{
	(void) stream;
	(void) keys;
	return (char *)strdup("");
}

static uint32_t PrimaryGetStreamOutLatency(const struct AudioHwStreamOut *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	uint64_t sport_out_frames = ameba_audio_stream_tx_sport_rendered_frames(out->out_pcm);
	uint32_t latency_ms = (ameba_audio_stream_tx_get_frames_written(out->out_pcm) - sport_out_frames) * 1000 / out->config.rate;
	HAL_AUDIO_VERBOSE("hal written_to_driver:%llu, sport_out_frames:%llu, latency_ms:%lu", ameba_audio_stream_tx_get_frames_written(out->out_pcm), sport_out_frames,
					  latency_ms);

	uint32_t dma_buf_add_codec_latency_ms = (out->config.period_size * out->config.period_count + 36) * 1000 / out->config.rate;
	if (latency_ms > dma_buf_add_codec_latency_ms * 2) {
		return dma_buf_add_codec_latency_ms;
	} else {
		return latency_ms;
	}
}

static int PrimaryGetPresentationPosition(const struct AudioHwStreamOut *stream, uint64_t *frames, struct timespec *timestamp)
{
	HAL_AUDIO_VERBOSE("primaryGetPresentationPosition latency:%lu", PrimaryGetStreamOutLatency(stream));

	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	int ret = -1;

	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);

	if (out->out_pcm) {
		uint64_t rendered_frames;
		if (ameba_audio_stream_tx_get_position(out->out_pcm, &rendered_frames, timestamp) == 0) {
			int64_t signed_frames = out->written - (ameba_audio_stream_tx_get_frames_written(out->out_pcm) - rendered_frames);
			HAL_AUDIO_VERBOSE("out->written:%llu, rendered_frames:%llu, written to driver:%llu, signed_frames:%lld, sec:%lld, nsec:%ld", out->written, rendered_frames,
							  ameba_audio_stream_tx_get_frames_written(out->out_pcm),
							  signed_frames, timestamp->tv_sec, timestamp->tv_nsec);
			if (signed_frames >= 0) {
				*frames = signed_frames;
				HAL_AUDIO_VERBOSE("frames:%llu", *frames);
				rtos_mutex_give(out->lock);
				return 0;
			}
		} else {
			HAL_AUDIO_ERROR("get ts fail");
		}
	} else {
		HAL_AUDIO_ERROR("%s no out_pcm", __func__);
	}

	rtos_mutex_give(out->lock);

	return ret;
}

static int PrimaryGetPresentTime(const struct AudioHwStreamOut *stream, int64_t *now_ns, int64_t *audio_ns)
{
	HAL_AUDIO_VERBOSE("primaryGetPresentationPosition latency:%lu", PrimaryGetStreamOutLatency(stream));

	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	int ret = -1;
	int64_t tmp_now_ns = 0;
	int64_t tmp_audio_ns = 0;

	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);

	if (out->out_pcm) {

		rtos_critical_enter();
		int64_t written_to_driver_frames = ameba_audio_stream_tx_get_frames_written(out->out_pcm);
		ret = ameba_audio_stream_tx_get_time(out->out_pcm, &tmp_now_ns, &tmp_audio_ns);
		rtos_critical_exit();

		int64_t out_written_to_hal_ns = (int64_t)(((double)out->written / (double)PrimaryGetStreamOutSampleRate((const struct AudioHwStream *)stream) *
										(double)1000000000));
		int64_t written_to_driver_ns = (int64_t)(((double)written_to_driver_frames / (double)PrimaryGetStreamOutSampleRate((const struct AudioHwStream *)stream) *
									   (double)1000000000));
		*audio_ns = out_written_to_hal_ns - (written_to_driver_ns - tmp_audio_ns);
		*now_ns = tmp_now_ns;
	} else {
		HAL_AUDIO_ERROR("%s no out_pcm", __func__);
	}

	rtos_mutex_give(out->lock);

	return ret;
}

static int64_t PrimaryGetTriggerTime(const struct AudioHwStreamOut *stream)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	int64_t ret = -1;
	if (out->out_pcm) {
		ret = ameba_audio_stream_tx_get_trigger_time(out->out_pcm);
	}
	return ret;
}

static int PrimarySetStreamOutVolume(struct AudioHwStreamOut *stream, float left,
									 float right)
{
	HAL_AUDIO_VERBOSE("PrimarySetStreamOutVolume enter, left: %f, right: %f", left, right);
	(void) stream;
	ameba_audio_ctl_set_tx_volume(ameba_audio_get_ctl(), left, right);
	return 0;
}

/* must be called with hw device and output stream mutexes locked */
static int StartAudioHwStreamOut(struct PrimaryAudioHwStreamOut *out)
{
	HAL_AUDIO_VERBOSE("start output stream enter");
	(void) out;
	//ameba_audio_ctl_set_tx_mute(ameba_audio_get_ctl(), false);
	if (AUDIO_HW_AMPLIFIER_MUTE_ENABLE) {
		ameba_audio_stream_tx_set_amp_state(true);
	}
	return HAL_OSAL_OK;
}

static ssize_t PrimaryStreamOutWrite(struct AudioHwStreamOut *stream, const void *buffer,
									 size_t bytes, bool block)
{
	HAL_AUDIO_PVERBOSE("primaryStreamOutWrite: bytes: %u", bytes);

	int ret = 0;
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream;
	//struct PrimaryAudioHwCard *pri_card = out->pri_card;

	size_t frame_size = PrimaryAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);

	//rtos_mutex_take(pri_card->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	if (out->standby) {
		ret = StartAudioHwStreamOut(out);
		if (ret != 0) {
			HAL_AUDIO_ERROR("start stream_out fail");
			//rtos_mutex_give(pri_card->lock);
			goto exit;
		}
		out->standby = 0;
	}
	//rtos_mutex_give(pri_card->lock);

	/* Write to all active PCMs */
	if (out->out_pcm) {
		ret = ameba_audio_stream_tx_write(out->out_pcm, (void *)buffer, bytes, block);
	} else {
		HAL_AUDIO_ERROR("out pcm is NULL!!!");
	}

	if (DUMP_ENABLE) {
		if (out->written * frame_size + bytes <= DUMP_FRAME * frame_size) {
			memcpy(out->buffer + out->written * frame_size, buffer, bytes);
			DCache_Invalidate((u32)(out->buffer + out->written * frame_size), bytes);
		}
	}

	//write successfully
	if (ret >= 0) {
		out->written += ret / frame_size;
		//sync with ameba audio driver's total_counter_boundary max value.
		if (out->written > UINT64_MAX) {
			out->written = 0;
		}
	}

exit:
	rtos_mutex_give(out->lock);

	return ret;
}

void DestroyAudioHwStreamOut(struct AudioHwStreamOut *stream_out)
{
	struct PrimaryAudioHwStreamOut *out = (struct PrimaryAudioHwStreamOut *)stream_out;

	PrimaryStandbyStreamOut(&stream_out->common);

	if (out->out_pcm) {
		ameba_audio_stream_tx_close(out->out_pcm);
		out->out_pcm = NULL;
	}

	if (DUMP_ENABLE) {
		rtos_mem_free(out->buffer);
	}

	rtos_mutex_delete(out->lock);
	rtos_mem_free(stream_out);
	//stream_out = NULL;
}

struct AudioHwStreamOut *CreateAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
		const struct AudioHwConfig *config)
{
	struct PrimaryAudioHwCard *pri_card = (struct PrimaryAudioHwCard *)card;
	struct PrimaryAudioHwStreamOut *out;

	out = (struct PrimaryAudioHwStreamOut *)rtos_mem_zmalloc(sizeof(struct PrimaryAudioHwStreamOut));
	if (!out) {
		return NULL;
	}

	out->pri_card = pri_card;
	out->desc = *desc;

	out->stream.common.GetSampleRate = PrimaryGetStreamOutSampleRate;
	out->stream.common.SetSampleRate = PrimarySetStreamOutSampleRate;
	out->stream.common.GetBufferSize = PrimaryGetStreamOutBufferSize;
	out->stream.common.GetChannels = PrimaryGetStreamOutChannels;
	out->stream.common.SetChannels = PrimarySetStreamOutChannels;
	out->stream.common.GetFormat = PrimaryGetStreamOutFormat;
	out->stream.common.SetFormat = PrimarySetStreamOutFormat;
	out->stream.common.Standby = PrimaryStandbyStreamOut;
	out->stream.common.Dump = PrimaryDumpStreamOut;
	out->stream.common.SetParameters = PrimarySetStreamOutParameters;
	out->stream.common.GetParameters = PrimaryGetStreamOutParameters;
	out->stream.common.GetBufferStatus = PrimaryGetStreamOutBufferStatus;
	out->stream.GetPresentationPosition = PrimaryGetPresentationPosition;
	out->stream.GetPresentTime = PrimaryGetPresentTime;
	out->stream.GetTriggerTime = PrimaryGetTriggerTime;
	out->stream.GetLatency = PrimaryGetStreamOutLatency;
	out->stream.SetVolume = PrimarySetStreamOutVolume;
	out->stream.Write = PrimaryStreamOutWrite;

	out->format = AUDIO_HW_FORMAT_PCM_16_BIT;
	out->channel_count = 2;
	out->sample_rate = DEFAULT_OUT_SAMPLING_RATE;
	out->period_size = SHORT_PERIOD_SIZE;
	out->pri_card = pri_card;
	out->standby = 1;
	out->written = 0;
	out->amp_pin = -1;
	out->delay_start = false;

	enum AudioHwFormat format = out->stream.common.GetFormat(&out->stream.common);
	uint32_t channel_count =  out->stream.common.GetChannels(&out->stream.common);
	uint32_t sample = out->stream.common.GetSampleRate(&out->stream.common);

	if (format != config->format) {
		out->format =  config->format;
	}
	if (channel_count != config->channel_count) {
		out->channel_count = config->channel_count;
	}
	if (sample != config->sample_rate) {
		out->sample_rate = config->sample_rate;
	}

	rtos_mutex_create(&out->lock);

	pri_card->output = out;

	out->config = stream_output_config;

	out->config.rate = out->sample_rate; // update sample_rate according to top level player
	out->config.format = out->format;
	out->config.channels = out->channel_count;
	out->config.frame_size = PrimaryAudioHwStreamOutFrameSize(&out->stream);

	if (desc->flags & AUDIO_HW_OUTPUT_FLAG_NOIRQ) {
		HAL_AUDIO_INFO("startAudioHwStreamOut in noirq mode");
		out->config.mode = AMEBA_AUDIO_DMA_NOIRQ_MODE;
		if (config->buffer_bytes) {
			out->period_size = config->buffer_bytes / out->config.frame_size;
		} else {
			out->period_size = NOIRQ_SHORT_PERIOD_SIZE;
		}
	} else {
		out->config.mode = AMEBA_AUDIO_DMA_IRQ_MODE;
		if (config->buffer_bytes) {
			out->period_size = config->buffer_bytes / out->config.frame_size;
		} else {
			out->period_size = SHORT_PERIOD_SIZE;
		}
	}

	out->config.period_size = out->period_size;

	/*stream_tx_init can only be set here, because if it's in first write, then set parameters will stuck.Because setparameters should be called before write*/
	if (out->out_pcm == NULL) {
		HAL_AUDIO_INFO("startAudioHwStreamOut samplerate:%" PRIu32 ", format:%" PRIu32 ", channel:%" PRIu32 ", framesize:%" PRIu32 ", period_size:%" PRIu32 "",
					   out->config.rate, out->config.format,
					   out->config.channels,
					   out->config.frame_size, out->config.period_size);
		out->out_pcm = ameba_audio_stream_tx_init(AMEBA_AUDIO_DEVICE_SPEAKER, out->config);
	} else {
		HAL_AUDIO_DEBUG("out pcm has been opened");
	}

	if (DUMP_ENABLE) {
		out->buffer = (char *)rtos_mem_zmalloc(DUMP_FRAME * out->config.frame_size);
		HAL_AUDIO_INFO("dump buffer:%p", out->buffer);
	}

	return &out->stream;
}
