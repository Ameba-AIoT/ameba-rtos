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

#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_params_handle.h"
#include "ameba_audio_types.h"
#include "ameba_audio_stream_capture.h"
#include "ameba_audio_stream_control.h"
#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_stream_in.h"
#include "os_wrapper.h"
#include "primary_audio_hw_card.h"

#define NOIRQ_CAPTURE_PERIOD_SIZE     128
#define CAPTURE_PERIOD_SIZE           1024
#define CAPTURE_PERIOD_COUNT          4
#define AUDIO_CAPTURE_MODE            "cap_mode"
#define MIC_CATEGORY                  "mic_category"
// channels of pure data
#define NO_AFE_PURE_DATA              "no_afe_pure_data"
// for debug (mic,mic,..ref,out), only out buffer not filled by audio fwk
#define NO_AFE_ALL_DATA               "no_afe_all_data"
#define REF_CHANNEL                   "ref_channel"
#define NO_AFE_PURE_DATA_DUMP         0
#define NO_AFE_ALL_DATA_DUMP          0
#define DUMP_FRAME                    48000

typedef enum CAPTURE_MODE {
	CAPTURE_NO_AFE_PURE_DATA = 0,
	CAPTURE_NO_AFE_PURE_DATA_ADD_OUT,
} CAPTURE_MODE;

StreamConfig stream_input_config = {
	.channels = 2,
	.rate = 44100,
	.period_size = CAPTURE_PERIOD_SIZE,
	.period_count = CAPTURE_PERIOD_COUNT,
	.format = AUDIO_HW_FORMAT_PCM_16_BIT,
	.frame_size = 4,
};

struct PrimaryAudioHwStreamIn {
	struct AudioHwStreamIn stream;

	struct PrimaryAudioHwCard *pri_card;
	struct AudioHwPathDescriptor desc;
	rtos_mutex_t lock;
	StreamConfig config;
	Stream *in_pcm;
	void *stream_buf;
	uint32_t cap_stream_buf_bytes;
	int standby;
	uint64_t rframe;
	uint32_t requested_channels;
	CAPTURE_MODE mode;
	uint32_t channel_for_ref;
	uint64_t mic_category;
	uint32_t device;

#if (NO_AFE_PURE_DATA_DUMP || NO_AFE_ALL_DATA_DUMP)
	char *in_buf;  //2s data
	char *out_buf; //2s data
#endif

};

static inline size_t PrimaryAudioHwStreamInFrameSize(const struct AudioHwStreamIn *s)
{
	size_t chan_samp_sz;
	enum AudioHwFormat format = s->common.GetFormat(&s->common);

	if (AudioIsLinearPCM(format)) {
		chan_samp_sz = GetAudioBytesPerSample(format);
		return s->common.GetChannels(&s->common) * chan_samp_sz;
	}

	return sizeof(int8_t);
}

/** audio_stream_in implementation **/
static uint32_t PrimaryGetStreamInSampleRate(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	return cap->config.rate;
}

static int PrimarySetStreamInSampleRate(struct AudioHwStream *stream, uint32_t rate)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	if (rate < 4000 || rate > 192000) {
		return - EINVAL;
	}

	cap->config.rate = rate;
	return 0;
}

static size_t PrimaryGetStreamInBufferSize(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	return cap->config.period_size * PrimaryAudioHwStreamInFrameSize((const struct AudioHwStreamIn *)stream);
}

static uint32_t PrimaryGetStreamInChannels(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	return (uint32_t)cap->requested_channels;
}

static int PrimarySetStreamInChannels(const struct AudioHwStream *stream, uint32_t channel)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	cap->config.channels = channel;
	return 0;
}

static enum AudioHwFormat PrimaryGetStreamInFormat(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	return cap->config.format;
}

static int PrimarySetStreamInFormat(struct AudioHwStream *stream, enum AudioHwFormat format)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	cap->config.format = format;
	return 0;
}

static int DoInputStandby(struct PrimaryAudioHwStreamIn *cap)
{
	if (!cap->standby) {
		ameba_audio_stream_rx_stop(cap->in_pcm);
		ameba_audio_stream_rx_close(cap->in_pcm);
		cap->in_pcm = NULL;
		cap->standby = 1;
	}

#if (NO_AFE_PURE_DATA_DUMP || NO_AFE_ALL_DATA_DUMP)
	if (cap->in_buf) {
		rtos_mem_free(cap->in_buf);
		cap->in_buf = NULL;
	}

	if (cap->out_buf) {
		rtos_mem_free(cap->out_buf);
		cap->out_buf = NULL;
	}
#endif

	return 0;
}

static int PrimaryStandbyStreamIn(struct AudioHwStream *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	int status;

	//rtos_mutex_take(cap->pri_card->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);
	status = DoInputStandby(cap);
	rtos_mutex_give(cap->lock);
	//rtos_mutex_give(cap->pri_card->lock);

	return status;
}

static int PrimaryDumpStreamIn(const struct AudioHwStream *stream, int fd)
{
	(void) stream;
	(void) fd;
	return 0;
}

static int PrimarySetStreamInParameters(struct AudioHwStream *stream, const char *str_pairs)
{
	HAL_AUDIO_VERBOSE("%s, keys = %s", __FUNCTION__, str_pairs);
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	struct string_cell *cells;
	cells = string_cells_create_from_str(str_pairs);
	int value;
	char string_value[15];

	if (string_cells_has_key(cells, REF_CHANNEL)) {
		string_cells_get_int(cells, REF_CHANNEL, &value);
		cap->channel_for_ref = value;
	}

	if (string_cells_has_key(cells, MIC_CATEGORY)) {
		string_cells_get_int(cells, MIC_CATEGORY, &value);
		cap->mic_category = value;
	}

	HAL_AUDIO_VERBOSE("check key AUDIO_CAPTURE_MODE");
	if (string_cells_has_key(cells, AUDIO_CAPTURE_MODE)) {
		string_cells_get_str(cells, AUDIO_CAPTURE_MODE, string_value, 15);
		if (!strncmp(string_value, NO_AFE_PURE_DATA, 14)) {
			HAL_AUDIO_VERBOSE("mode:NO AFE PURE DATA");
			cap->mode = CAPTURE_NO_AFE_PURE_DATA;
		} else if (!strncmp(string_value, NO_AFE_ALL_DATA, 14)) {
			HAL_AUDIO_VERBOSE("mode:NO AFE ALL DATA");
			cap->mode = CAPTURE_NO_AFE_PURE_DATA_ADD_OUT;
		}
	}

	string_cells_destroy(cells);
	return 0;
}

static char *PrimaryGetStreamInParameters(const struct AudioHwStream *stream,
		const char *keys)
{
	(void) stream;
	(void) keys;
	return (char *)strdup("");
}

static uint32_t PrimaryGetStreamInLatency(const struct AudioHwStreamIn *stream)
{
	(void) stream;
	return 15;
}

static int PrimaryGetStreamInPosition(const struct AudioHwStreamIn *stream, uint64_t *frames, struct timespec *timestamp)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	int ret = -1;

	//Better not add mutex, because if only do record, will always lock in read api.So this api will not work.
	//rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);

	if (cap->in_pcm) {
		uint64_t captured_frames;
		if (ameba_audio_stream_rx_get_position(cap->in_pcm, &captured_frames, timestamp) == 0) {
			*frames = captured_frames;
			HAL_AUDIO_VERBOSE("frames:%llu", *frames);
			//rtos_mutex_give(cap->lock);
			return 0;
		} else {
			HAL_AUDIO_ERROR("get ts fail");
		}
	} else {
		HAL_AUDIO_ERROR("%s no in_pcm", __func__);
	}

	//rtos_mutex_give(cap->lock);

	return ret;
}

static int PrimaryGetPresentTime(const struct AudioHwStreamIn *stream, int64_t *now_ns, int64_t *audio_ns)
{

	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	int ret = -1;

	//Better not add mutex, because if only do record, will always lock in read api.So this api will not work.
	//rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);

	if (cap->in_pcm) {
		ret = ameba_audio_stream_rx_get_time(cap->in_pcm, now_ns, audio_ns);
	} else {
		HAL_AUDIO_ERROR("%s no in_pcm", __func__);
	}

	//rtos_mutex_give(cap->lock);

	return ret;
}

static int64_t PrimaryGetTriggerTime(const struct AudioHwStreamIn *stream)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	int64_t ret = -1;
	if (cap->in_pcm) {
		ret = ameba_audio_stream_rx_get_trigger_time(cap->in_pcm);
	}
	return ret;
}

static int ConfigureNoAfePureData(struct PrimaryAudioHwStreamIn *cap)
{
	if (cap->requested_channels == 3) {
		cap->config.channels = 4;   //no 3 channels tdm in driver
		unsigned int driver_bytes = PrimaryGetStreamInBufferSize((struct AudioHwStream *)&cap->stream) * cap->config.channels /
									cap->requested_channels * cap->config.period_count;   // *4chan/3chan
		HAL_AUDIO_INFO("malloc stream_buf:%d cap->config.channels:%" PRId32 ", cap->requested_channels:%" PRId32 "", driver_bytes, cap->config.channels,
					   cap->requested_channels);
		cap->stream_buf = (char *) rtos_mem_zmalloc(driver_bytes);
		if (cap->stream_buf == NULL) {
			HAL_AUDIO_ERROR("stream_buf malloc fail");
			return HAL_OSAL_ERR_NO_MEMORY;
		}
		cap->cap_stream_buf_bytes = driver_bytes;
	} else {
		cap->config.channels = cap->requested_channels;
	}

#if NO_AFE_PURE_DATA_DUMP
	cap->out_buf = (char *)rtos_mem_zmalloc(DUMP_FRAME * cap->requested_channels * GetAudioBytesPerSample(cap->config.format));
	HAL_AUDIO_INFO("afe cap, cap->out_buf:%p", cap->out_buf);
#endif

	return HAL_OSAL_OK;
}

/*    amebalite has at most 1~3 amics, 1~4 dmics.
 *    requested_channels      mic_channels    ref_channels     channels_for_denoise
 *    3                       1               1                 1
 *    4                       2               1                 1
 *    5                       3               1                 1
 */
static int ConfigureNoAfePureDataAddOut(struct PrimaryAudioHwStreamIn *cap)
{
	switch (cap->requested_channels) {
	case 3:
	case 5:
		cap->config.channels = cap->requested_channels - 1;
		break;

	case 4:
		cap->config.channels = 4;   //no 3 channels tdm in driver
		break;

	default:
		HAL_AUDIO_ERROR("%s not supported channel count", __FUNCTION__);
		break;
	}

	unsigned int driver_bytes = PrimaryGetStreamInBufferSize((struct AudioHwStream *)&cap->stream) * cap->config.channels /
								cap->requested_channels * cap->config.period_count;   // *4chan/3chan
	HAL_AUDIO_INFO("malloc stream_buf:%d cap->config.channels:%" PRId32 ", cap->requested_channels:%" PRId32 "", driver_bytes, cap->config.channels,
				   cap->requested_channels);
	cap->stream_buf = (char *) rtos_mem_zmalloc(driver_bytes);
	if (cap->stream_buf == NULL) {
		HAL_AUDIO_ERROR("stream_buf malloc fail");
		return HAL_OSAL_ERR_NO_MEMORY;
	}
	cap->cap_stream_buf_bytes = driver_bytes;
	HAL_AUDIO_INFO("cap stream buf bytes:%" PRId32 "", cap->cap_stream_buf_bytes);

#if NO_AFE_ALL_DATA_DUMP
	//cap->in_buf = (char *)rtos_mem_zmalloc(DUMP_FRAME * cap->config.channels * 2);
	cap->out_buf = (char *)rtos_mem_zmalloc(DUMP_FRAME * cap->requested_channels * GetAudioBytesPerSample(cap->config.format));
	HAL_AUDIO_INFO("afe cap, cap->in_buf:%p, cap->out_buf:%p", cap->in_buf, cap->out_buf);
#endif

	return HAL_OSAL_OK;
}

static int StartAudioHwStreamIn(struct PrimaryAudioHwStreamIn *cap)
{
	int ret = HAL_OSAL_OK;
	cap->config.channels = cap->requested_channels;

	//HAL_AUDIO_INFO("%s", __FUNCTION__);

	switch (cap->mode) {
	case CAPTURE_NO_AFE_PURE_DATA:
		ret = ConfigureNoAfePureData(cap);
		break;
	case CAPTURE_NO_AFE_PURE_DATA_ADD_OUT:
		ret = ConfigureNoAfePureDataAddOut(cap);
		break;
	default:
		HAL_AUDIO_ERROR("mode(%d) not supported!", cap->mode);
		break;
	}

	if (ret != HAL_OSAL_OK) {
		return ret;
	}

	cap->config.frame_size = PrimaryAudioHwStreamInFrameSize(&cap->stream) * cap->config.channels / cap->requested_channels;
	HAL_AUDIO_VERBOSE("rate = %" PRId32 " , channels = %" PRId32 ", format = %" PRId32 ", period_size = %" PRIu32 ", period_count = %" PRIu32 ", frame_size = %"
					  PRId32
					  "",
					  cap->config.rate, cap->config.channels, cap->config.format, cap->config.period_size, cap->config.period_count, cap->config.frame_size);

	if (cap->in_pcm == NULL) {
		cap->in_pcm = ameba_audio_stream_rx_init(cap->device, cap->config);

	}

	ameba_audio_stream_rx_start(cap->in_pcm);
	return 0;
}

static ssize_t NoAfePureDataRead(struct AudioHwStreamIn *stream, void *buffer, size_t bytes)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	uint32_t i = 0;
	size_t app_frame_size = PrimaryAudioHwStreamInFrameSize((const struct AudioHwStreamIn *)stream);
	size_t driver_frame_size = app_frame_size * cap->config.channels / cap->requested_channels;
	HAL_AUDIO_VERBOSE("%s: bytes %u, app_frame_size:%d, driver_frame_size:%d", __FUNCTION__, bytes, app_frame_size, driver_frame_size);

	if (cap->requested_channels == 3) {
		uint32_t driver_bytes = bytes * cap->config.channels / cap->requested_channels;   // *4chan/3chan
		if (driver_bytes > cap->cap_stream_buf_bytes)  {
			HAL_AUDIO_ERROR("read bytes once is too large.");
			return HAL_OSAL_ERR_INVALID_PARAM;
		}
		HAL_AUDIO_VERBOSE("read bytes:%u, driver_bytes:%lu", bytes, driver_bytes);
		ameba_audio_stream_rx_read(cap->in_pcm, cap->stream_buf, driver_bytes);
		for (i = 0; i < driver_bytes / driver_frame_size; i++) {
			if (cap->config.format == AUDIO_HW_FORMAT_PCM_16_BIT) {
				*((uint16_t *)buffer + cap->requested_channels * i) = *((uint16_t *)cap->stream_buf + cap->config.channels * i);
				*((uint16_t *)buffer + cap->requested_channels * i + 1) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint16_t *)buffer + cap->requested_channels * i + 2) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 2);
			} else if (cap->config.format == AUDIO_HW_FORMAT_PCM_32_BIT || cap->config.format == AUDIO_HW_FORMAT_PCM_8_24_BIT) {
				*((uint32_t *)buffer + cap->requested_channels * i) = *((uint32_t *)cap->stream_buf + cap->config.channels * i);
				*((uint32_t *)buffer + cap->requested_channels * i + 1) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint32_t *)buffer + cap->requested_channels * i + 2) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 2);
			}
		}
	} else {
		ameba_audio_stream_rx_read(cap->in_pcm, buffer, bytes);
	}

#if NO_AFE_PURE_DATA_DUMP
	if (cap->rframe + bytes / app_frame_size <= DUMP_FRAME) {
		memcpy((char *)cap->out_buf + (cap->rframe * app_frame_size), (char *)buffer, bytes);
	}
#endif

	cap->rframe += bytes / app_frame_size;
	return bytes;
}

static ssize_t NoAfePureDataAddOutRead(struct AudioHwStreamIn *stream, void *buffer, size_t bytes)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	uint32_t i = 0;

	//bytes read from driver at one read.
	uint32_t driver_bytes = 0;

	//frame size requested by app
	uint32_t app_frame_size = PrimaryAudioHwStreamInFrameSize((const struct AudioHwStreamIn *)stream);

	//frames by app one read
	uint32_t frames = bytes / app_frame_size;

	//frame size to read from driver.
	size_t driver_frame_size = PrimaryAudioHwStreamInFrameSize((const struct AudioHwStreamIn *)stream) * cap->config.channels / cap->requested_channels;
	HAL_AUDIO_CVERBOSE("bytes %u, app_frame_size:%d, driver_frame_size:%d, frames:%d", bytes, app_frame_size, driver_frame_size, frames);

	driver_bytes = frames * driver_frame_size;
	if (driver_bytes > cap->cap_stream_buf_bytes)  {
		HAL_AUDIO_ERROR("read bytes once is too large.");
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	HAL_AUDIO_CVERBOSE("read bytes:%d, driver_bytes:%d", bytes, driver_bytes);
	ameba_audio_stream_rx_read(cap->in_pcm, cap->stream_buf, driver_bytes);

	switch (cap->requested_channels) {
	case 3:
		for (i = 0; i < frames; i++) {
			if (cap->config.format == AUDIO_HW_FORMAT_PCM_16_BIT) {
				*((uint16_t *)buffer + cap->requested_channels * i) = *((uint16_t *)cap->stream_buf + cap->config.channels * i);
				*((uint16_t *)buffer + cap->requested_channels * i + 1) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint16_t *)buffer + cap->requested_channels * i + 2) = 0;
			} else if (cap->config.format == AUDIO_HW_FORMAT_PCM_32_BIT || cap->config.format == AUDIO_HW_FORMAT_PCM_8_24_BIT) {
				*((uint32_t *)buffer + cap->requested_channels * i) = *((uint32_t *)cap->stream_buf + cap->config.channels * i);
				*((uint32_t *)buffer + cap->requested_channels * i + 1) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint32_t *)buffer + cap->requested_channels * i + 2) = 0;
			}
		}
		break;
	case 5:
		for (i = 0; i < frames; i++) {
			if (cap->config.format == AUDIO_HW_FORMAT_PCM_16_BIT) {
				*((uint16_t *)buffer + cap->requested_channels * i) = *((uint16_t *)cap->stream_buf + cap->config.channels * i);
				*((uint16_t *)buffer + cap->requested_channels * i + 1) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint16_t *)buffer + cap->requested_channels * i + 2) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 2);
				*((uint16_t *)buffer + cap->requested_channels * i + 3) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 3);
				*((uint16_t *)buffer + cap->requested_channels * i + 4) = 0;
			} else if (cap->config.format == AUDIO_HW_FORMAT_PCM_32_BIT || cap->config.format == AUDIO_HW_FORMAT_PCM_8_24_BIT) {
				*((uint32_t *)buffer + cap->requested_channels * i) = *((uint32_t *)cap->stream_buf + cap->config.channels * i);
				*((uint32_t *)buffer + cap->requested_channels * i + 1) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint32_t *)buffer + cap->requested_channels * i + 2) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 2);
				*((uint32_t *)buffer + cap->requested_channels * i + 3) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 3);
				*((uint32_t *)buffer + cap->requested_channels * i + 4) = 0;
			}
		}
		break;
	case 4:
		for (i = 0; i < driver_bytes / driver_frame_size; i++) {
			if (cap->config.format == AUDIO_HW_FORMAT_PCM_16_BIT) {
				*((uint16_t *)buffer + cap->requested_channels * i) = *((uint16_t *)cap->stream_buf + cap->config.channels * i);
				*((uint16_t *)buffer + cap->requested_channels * i + 1) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint16_t *)buffer + cap->requested_channels * i + 2) = *((uint16_t *)cap->stream_buf + cap->config.channels * i + 2);
				*((uint16_t *)buffer + cap->requested_channels * i + 3) = 0;
			} else if (cap->config.format == AUDIO_HW_FORMAT_PCM_32_BIT || cap->config.format == AUDIO_HW_FORMAT_PCM_8_24_BIT) {
				*((uint32_t *)buffer + cap->requested_channels * i) = *((uint32_t *)cap->stream_buf + cap->config.channels * i);
				*((uint32_t *)buffer + cap->requested_channels * i + 1) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 1);
				*((uint32_t *)buffer + cap->requested_channels * i + 2) = *((uint32_t *)cap->stream_buf + cap->config.channels * i + 2);
				*((uint32_t *)buffer + cap->requested_channels * i + 3) = 0;
			}
		}
		break;

	default:
		HAL_AUDIO_ERROR("channel(%" PRId32 ")not supported", cap->requested_channels);
		break;
	}

#if NO_AFE_ALL_DATA_DUMP
	if (cap->rframe + frames <= DUMP_FRAME) {
		memcpy((char *)cap->out_buf + (cap->rframe * app_frame_size), (char *)buffer, bytes);
	}
#endif

	cap->rframe += frames;
	return bytes;
}

static ssize_t PrimaryStreamInRead(struct AudioHwStreamIn *stream, void *buffer, size_t bytes)
{
	int ret = 0;

	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;
	//struct PrimaryAudioHwCard *pri_card = cap->pri_card;

	//rtos_mutex_take(pri_card->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);
	if (cap->standby) {
		// in this StartAudioHwStreamIn, it needs to check stream_in mode, so this apis should be called
		// after setparameters. Because setparameters need to be set after RTAudioRecord_start(), so this api
		// can only be called in the first time call RTAudioRecord_read().Don't move it to other place.
		ret = StartAudioHwStreamIn(cap);
		if (ret == 0) {
			cap->standby = 0;
		} else {
			HAL_AUDIO_ERROR("start audio stream_in fail");
			goto exit;
		}
	}

	switch (cap->mode) {
	case CAPTURE_NO_AFE_PURE_DATA:
		ret = NoAfePureDataRead(stream, buffer, bytes);
		break;

	case CAPTURE_NO_AFE_PURE_DATA_ADD_OUT:
		ret = NoAfePureDataAddOutRead(stream, buffer, bytes);
		break;

	default:
		HAL_AUDIO_ERROR("mode(%d) not supported!", cap->mode);
		break;
	}

exit:
	//DelayUs((int64_t)bytes * 1000000 / PrimaryAudioHwStreamInFrameSize(stream) / PrimaryGetStreamInSampleRate(&stream->common));
	//memset(buffer, 0, bytes);
	//rtos_mutex_give(pri_card->lock);
	rtos_mutex_give(cap->lock);

	return ret;
}

static ssize_t PrimaryStreamInReadTimeout(struct AudioHwStreamIn *stream, void *buffer, size_t bytes, uint32_t time_out_ms)
{
	int ret = 0;
	(void) time_out_ms;

	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream;

	rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);
	if (cap->standby) {
		// in this StartAudioHwStreamIn, it needs to check stream_in mode, so this apis should be called
		// after setparameters. Because setparameters need to be set after RTAudioRecord_start(), so this api
		// can only be called in the first time call RTAudioRecord_read().Don't move it to other place.
		ret = StartAudioHwStreamIn(cap);
		if (ret == 0) {
			cap->standby = 0;
		} else {
			HAL_AUDIO_ERROR("start audio stream_in fail");
			goto exit;
		}
	}

	switch (cap->mode) {
	case CAPTURE_NO_AFE_PURE_DATA:
		ret = NoAfePureDataRead(stream, buffer, bytes);
		break;

	case CAPTURE_NO_AFE_PURE_DATA_ADD_OUT:
		ret = NoAfePureDataAddOutRead(stream, buffer, bytes);
		break;

	default:
		HAL_AUDIO_ERROR("mode(%d) not supported!", cap->mode);
		break;
	}

exit:
	rtos_mutex_give(cap->lock);

	return ret;
}

static int CheckInputParameters(uint32_t sample_rate, enum AudioHwFormat format, uint32_t channel_count)
{
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_16_BIT:
	case AUDIO_HW_FORMAT_PCM_32_BIT:
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		break;
	default:
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	if ((channel_count < 1) || (channel_count > 8)) {
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	switch (sample_rate) {
	case 8000:
	case 11025:
	case 16000:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
	case 176400:
	case 192000:
		break;
	default:
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	return 0;
}

size_t GetHwInputBufferSize(uint32_t sample_rate, enum AudioHwFormat format, uint32_t channel_count)
{
	size_t size;

	if (CheckInputParameters(sample_rate, format, channel_count) != 0) {
		return 0;
	}

	size = (stream_input_config.period_size * sample_rate) / stream_input_config.rate;

	size = ((size + 15) / 16) * 16;

	return size * channel_count * sizeof(short);
}

void DestroyAudioHwStreamIn(struct AudioHwStreamIn *stream_in)
{
	struct PrimaryAudioHwStreamIn *cap = (struct PrimaryAudioHwStreamIn *)stream_in;

	PrimaryStandbyStreamIn(&stream_in->common);

	if (cap->stream_buf) {
		rtos_mem_free(cap->stream_buf);
		cap->stream_buf = NULL;
	}

	rtos_mutex_delete(cap->lock);

	rtos_mem_free(stream_in);
	//stream_in = NULL;
}

//for passthrough, this api is called in RTAudioRecord_start(). Please pay attention to it when do logic change.
struct AudioHwStreamIn *CreateAudioHwStreamIn(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
		const struct AudioHwConfig *config)
{
	struct PrimaryAudioHwCard *lpri_card = (struct PrimaryAudioHwCard *)card;
	struct PrimaryAudioHwStreamIn *in;

	HAL_AUDIO_VERBOSE("primaryCreateStreamIn() with format:%d, sample_rate:%" PRId32 " channel_count:0x%lx", config->format, config->sample_rate,
					  config->channel_count);
	if (CheckInputParameters(config->sample_rate, config->format, config->channel_count) != 0) {
		HAL_AUDIO_ERROR("primaryCreateStreamIn: invalid config");
		return NULL;
	}

	in = (struct PrimaryAudioHwStreamIn *)rtos_mem_zmalloc(sizeof(struct PrimaryAudioHwStreamIn));
	if (!in) {
		return NULL;
	}

	in->pri_card = lpri_card;
	in->desc = *desc;

	in->stream.common.GetSampleRate = PrimaryGetStreamInSampleRate;
	in->stream.common.SetSampleRate = PrimarySetStreamInSampleRate;
	in->stream.common.GetBufferSize = PrimaryGetStreamInBufferSize;
	in->stream.common.GetChannels = PrimaryGetStreamInChannels;
	in->stream.common.SetChannels = PrimarySetStreamInChannels;
	in->stream.common.GetFormat = PrimaryGetStreamInFormat;
	in->stream.common.SetFormat = PrimarySetStreamInFormat;
	in->stream.common.Standby = PrimaryStandbyStreamIn;
	in->stream.common.Dump = PrimaryDumpStreamIn;
	in->stream.common.SetParameters = PrimarySetStreamInParameters;
	in->stream.common.GetParameters = PrimaryGetStreamInParameters;
	in->stream.GetLatency = PrimaryGetStreamInLatency;
	in->stream.GetCapturePosition = PrimaryGetStreamInPosition;
	in->stream.GetPresentTime = PrimaryGetPresentTime;
	in->stream.GetTriggerTime = PrimaryGetTriggerTime;
	in->stream.Read = PrimaryStreamInRead;
	in->stream.ReadTimeout = PrimaryStreamInReadTimeout;

	in->config = stream_input_config;
	in->in_pcm = NULL;
	in->stream_buf = NULL;
	in->cap_stream_buf_bytes = 0;
	in->standby = 1;
	in->rframe = 0;
	rtos_mutex_create(&in->lock);
	lpri_card->input = in;
	in->mode = CAPTURE_NO_AFE_PURE_DATA;
	in->device = AMEBA_AUDIO_IN_MIC;

	in->config.rate = config->sample_rate;
	in->config.format = config->format;
	in->config.channels = config->channel_count;
	in->requested_channels = config->channel_count;
	in->channel_for_ref = 2;

	if (desc->flags & AUDIO_HW_INPUT_FLAG_NOIRQ) {
		HAL_AUDIO_INFO("CreateAudioHwStreamIn in NO_IRQ mode, buffer_bytes: %" PRIu32 "", config->buffer_bytes);
		in->config.mode = AMEBA_AUDIO_DMA_NOIRQ_MODE;
		if (config->buffer_bytes) {
			in->config.period_size = config->buffer_bytes / PrimaryAudioHwStreamInFrameSize(&in->stream);
		} else {
			in->config.period_size = NOIRQ_CAPTURE_PERIOD_SIZE;
		}
	} else {
		in->config.mode = AMEBA_AUDIO_DMA_IRQ_MODE;
		if (config->buffer_bytes) {
			in->config.period_size = config->buffer_bytes / PrimaryAudioHwStreamInFrameSize(&in->stream);
		} else {
			in->config.period_size = CAPTURE_PERIOD_SIZE;
		}
	}

#if (NO_AFE_PURE_DATA_DUMP || NO_AFE_ALL_DATA_DUMP)
	in->in_buf = NULL;
	in->out_buf = NULL;
#endif

	//both dmic_ref_amic and in_mic are in ameba's soc codec control, I2S does not use ameba's soc codec control.
	if (desc->devices == AUDIO_HW_DEVICE_IN_DMIC_REF_AMIC) {
		HAL_AUDIO_VERBOSE("ref dmic & amic case");
		ameba_audio_ctl_set_mic_usage(ameba_audio_get_ctl(), AMEBA_AUDIO_CAPTURE_USAGE_DMIC_REF_AMIC);
	} else if (desc->devices == AUDIO_HW_DEVICE_IN_MIC) {
		HAL_AUDIO_VERBOSE("ref amic case");
		ameba_audio_ctl_set_mic_usage(ameba_audio_get_ctl(), AMEBA_AUDIO_CAPTURE_USAGE_AMIC);
	} else if (desc->devices == AUDIO_HW_DEVICE_IN_I2S) {
		HAL_AUDIO_VERBOSE("i2s case");
		in->device = AMEBA_AUDIO_IN_I2S;
	} else {
		HAL_AUDIO_ERROR("devices:%d for stream_in not supported, now using default amic instead", desc->devices);
	}

	HAL_AUDIO_VERBOSE("%s done", __FUNCTION__);
	return &in->stream;
}
