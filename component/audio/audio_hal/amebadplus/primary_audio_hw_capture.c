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
#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_params_handle.h"
#include "ameba_audio_types.h"
#include "ameba_audio_stream_capture.h"
#include "ameba_audio_stream_control.h"
#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_capture.h"
#include "os_wrapper.h"
#include "primary_audio_hw_adapter.h"

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
	.sport_index = 0,
	.is_multi_io = 0,
	.need_sync_start = false,
};

StreamConfig stream_input_config_extra = {
	.channels = 2,
	.rate = 44100,
	.period_size = CAPTURE_PERIOD_SIZE,
	.period_count = CAPTURE_PERIOD_COUNT,
	.format = AUDIO_HW_FORMAT_PCM_16_BIT,
	.frame_size = 4,
	.sport_index = 1,
	.is_multi_io = 0,
	.need_sync_start = false,
};

struct PrimaryAudioHwCapture {
	struct AudioHwCapture stream;

	struct PrimaryAudioHwAdapter *adap;
	struct AudioHwDeviceDescriptor desc;
	rtos_mutex_t lock;
	StreamConfig config;
	StreamConfig config_extra;
	Stream *in_pcm;
	Stream *in_pcm_extra;
	void *stream_buf;
	uint32_t cap_stream_buf_bytes;
	void *stream_buf_extra;
	uint32_t cap_stream_buf_bytes_extra;
	int standby;
	uint64_t rframe;
	uint32_t requested_channels;
	CAPTURE_MODE mode;
	uint32_t channel_for_ref;
	uint64_t mic_category;
	uint32_t device;
	rtos_mutex_t time_lock;

#if (NO_AFE_PURE_DATA_DUMP || NO_AFE_ALL_DATA_DUMP)
	char *in_buf;  //2s data
	char *out_buf; //2s data
#endif

};

static inline size_t PrimaryAudioHwCaptureFrameSize(const struct AudioHwCapture *s)
{
	size_t chan_samp_sz;
	AudioHwFormat format = s->common.GetFormat(&s->common);

	if (AudioIsLinearPCM(format)) {
		chan_samp_sz = GetAudioBytesPerSample(format);
		return s->common.GetChannels(&s->common) * chan_samp_sz;
	}

	return sizeof(int8_t);
}

/** audio_stream_in implementation **/
static uint32_t PrimaryGetCaptureSampleRate(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;

	return cap->config.rate;
}

static int PrimarySetCaptureSampleRate(struct AudioHwStream *stream, uint32_t rate)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;

	if (rate < 4000 || rate > 192000) {
		return - EINVAL;
	}

	cap->config.rate = rate;
	return 0;
}

static size_t PrimaryGetCaptureBufferSize(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;

	return cap->config.period_size * PrimaryAudioHwCaptureFrameSize((const struct AudioHwCapture *)stream);
}

static uint32_t PrimaryGetCaptureChannels(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;

	return (uint32_t)cap->requested_channels;
}

static int PrimarySetCaptureChannels(const struct AudioHwStream *stream, uint32_t channel)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;

	cap->config.channels = channel;
	return 0;
}

static AudioHwFormat PrimaryGetCaptureFormat(const struct AudioHwStream *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	return cap->config.format;
}

static int PrimarySetCaptureFormat(struct AudioHwStream *stream, AudioHwFormat format)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	cap->config.format = format;
	return 0;
}

static int DoInputStandby(struct PrimaryAudioHwCapture *cap)
{
	if (!cap->standby) {
		ameba_audio_stream_rx_stop(cap->in_pcm);
		ameba_audio_stream_rx_close(cap->in_pcm);

		if (cap->requested_channels == 10) {
			ameba_audio_stream_rx_stop(cap->in_pcm_extra);
			ameba_audio_stream_rx_sync_stop(cap->in_pcm, cap->config.sport_index, cap->config_extra.sport_index);
			ameba_audio_stream_rx_close(cap->in_pcm_extra);
		}

		cap->in_pcm = NULL;
		cap->in_pcm_extra = NULL;
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

static int PrimaryStandbyCapture(struct AudioHwStream *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	int status;

	//rtos_mutex_take(cap->adap->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);
	status = DoInputStandby(cap);
	rtos_mutex_give(cap->lock);
	//rtos_mutex_give(cap->adap->lock);

	return status;
}

static int PrimaryDumpCapture(const struct AudioHwStream *stream, int fd)
{
	(void) stream;
	(void) fd;
	return 0;
}

static int PrimarySetCaptureParameters(struct AudioHwStream *stream, const char *str_pairs)
{
	HAL_AUDIO_VERBOSE("%s, keys = %s", __FUNCTION__, str_pairs);
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
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

static char *PrimaryGetCaptureParameters(const struct AudioHwStream *stream,
		const char *keys)
{
	(void) stream;
	(void) keys;
	return (char *)strdup("");
}

static uint32_t PrimaryGetCaptureLatency(const struct AudioHwCapture *stream)
{
	(void) stream;
	return 15;
}

static int PrimaryGetCapturePosition(const struct AudioHwCapture *stream, uint64_t *frames, struct timespec *timestamp)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
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

static int64_t PrimaryGetTriggerTime(const struct AudioHwCapture *stream)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	int64_t ret = -1;
	if (cap->in_pcm) {
		ret = ameba_audio_stream_rx_get_trigger_time(cap->in_pcm);
	}
	return ret;
}

static int PrimaryGetPresentTime(const struct AudioHwCapture *stream, int64_t *now_ns, int64_t *audio_ns)
{

	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	int ret = -1;

	//Better not add mutex, because if only do record, will always lock in read api.So this api will not work.
	//rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);

	rtos_mutex_take(cap->time_lock, MUTEX_WAIT_TIMEOUT);
	if (cap->in_pcm) {
		ret = ameba_audio_stream_rx_get_time(cap->in_pcm, now_ns, audio_ns);
	} else {
		HAL_AUDIO_ERROR("%s no in_pcm", __func__);
	}
	rtos_mutex_give(cap->time_lock);

	//rtos_mutex_give(cap->lock);

	return ret;
}

static int ConfigureNoAfePureData(struct PrimaryAudioHwCapture *cap)
{
	unsigned int driver_bytes = PrimaryGetCaptureBufferSize((struct AudioHwStream *)&cap->stream) * cap->config.channels /
								cap->requested_channels * cap->config.period_count;   // *4chan/3chan

	if (cap->requested_channels == 3) {
		cap->config.channels = 4;   //no 3 channels tdm in driver
		HAL_AUDIO_INFO("malloc stream_buf:%d cap->config.channels:%" PRId32 ", cap->requested_channels:%" PRId32 "", driver_bytes, cap->config.channels,
					   cap->requested_channels);
		cap->stream_buf = (char *) rtos_mem_zmalloc(driver_bytes);
		if (cap->stream_buf == NULL) {
			HAL_AUDIO_ERROR("stream_buf malloc fail");
			return HAL_OSAL_ERR_NO_MEMORY;
		}
		cap->cap_stream_buf_bytes = driver_bytes;
	} else if (cap->requested_channels == 10) {
		cap->config.channels = 8;
		cap->config_extra.channels = 2;

		cap->stream_buf = (char *) rtos_mem_zmalloc(driver_bytes);
		if (cap->stream_buf == NULL) {
			HAL_AUDIO_ERROR("stream_buf malloc fail");
			return HAL_OSAL_ERR_NO_MEMORY;
		}
		cap->cap_stream_buf_bytes = driver_bytes;

		unsigned int driver_bytes_extra = PrimaryGetCaptureBufferSize((struct AudioHwStream *)&cap->stream) * 2 /
										  cap->requested_channels * cap->config.period_count;   // *4chan/3chan

		cap->stream_buf_extra = (char *) rtos_mem_zmalloc(driver_bytes_extra);
		if (cap->stream_buf_extra == NULL) {
			HAL_AUDIO_ERROR("stream_buf_extra malloc fail");
			return HAL_OSAL_ERR_NO_MEMORY;
		}
		cap->cap_stream_buf_bytes_extra = driver_bytes_extra;
		HAL_AUDIO_INFO("alloc stream_buf:%d, stream_buf_extra:%d", driver_bytes, driver_bytes_extra);

	} else {
		cap->config.channels = cap->requested_channels;
	}

#if NO_AFE_PURE_DATA_DUMP
	cap->out_buf = (char *)rtos_mem_zmalloc(DUMP_FRAME * cap->requested_channels * GetAudioBytesPerSample(cap->config.format));
	HAL_AUDIO_INFO("afe cap, cap->out_buf:%p", cap->out_buf);
#endif

	return HAL_OSAL_OK;
}

static int StartAudioHwCapture(struct PrimaryAudioHwCapture *cap)
{
	int ret = HAL_OSAL_OK;
	cap->config.channels = cap->requested_channels;

	//HAL_AUDIO_INFO("%s", __FUNCTION__);

	switch (cap->mode) {
	case CAPTURE_NO_AFE_PURE_DATA:
		ret = ConfigureNoAfePureData(cap);
		break;
	default:
		HAL_AUDIO_ERROR("mode(%d) not supported!", cap->mode);
		break;
	}

	if (ret != HAL_OSAL_OK) {
		return ret;
	}

	cap->config.frame_size = PrimaryAudioHwCaptureFrameSize(&cap->stream) * cap->config.channels / cap->requested_channels;
	HAL_AUDIO_VERBOSE("rate = %" PRId32 " , channels = %" PRId32 ", format = %" PRId32 ", period_size = %" PRIu32 ", period_count = %" PRIu32 ", frame_size = %"
					  PRId32
					  "",
					  cap->config.rate, cap->config.channels, cap->config.format, cap->config.period_size, cap->config.period_count, cap->config.frame_size);

	if (cap->in_pcm == NULL) {
		cap->in_pcm = ameba_audio_stream_rx_init(cap->device, cap->config);
	}

	if (cap->requested_channels == 10) {
		cap->config_extra.frame_size = PrimaryAudioHwCaptureFrameSize(&cap->stream) * cap->config_extra.channels / cap->requested_channels;
		if (cap->in_pcm_extra == NULL) {
			cap->in_pcm_extra = ameba_audio_stream_rx_init(cap->device, cap->config_extra);
		}
	}

	ameba_audio_stream_rx_start(cap->in_pcm);
	if (cap->requested_channels == 10) {
		ameba_audio_stream_rx_start(cap->in_pcm_extra);
		ameba_audio_stream_rx_sync_start(cap->in_pcm, cap->config.sport_index, cap->config_extra.sport_index);
	}

	return 0;
}

static ssize_t NoAfePureDataRead(struct AudioHwCapture *stream, void *buffer, size_t bytes)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	uint32_t i = 0;
	size_t app_frame_size = PrimaryAudioHwCaptureFrameSize((const struct AudioHwCapture *)stream);
	size_t driver_frame_size = app_frame_size * cap->config.channels / cap->requested_channels;
	size_t driver_frame_size_extra;
	uint32_t frames = bytes / app_frame_size;

	if (cap->requested_channels == 10) {
		driver_frame_size_extra = app_frame_size * 2 / cap->requested_channels;
	}

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
	} else if (cap->requested_channels == 10) {
		uint32_t driver_bytes = bytes * cap->config.channels / cap->requested_channels;
		uint32_t driver_bytes_extra = bytes * cap->config_extra.channels / cap->requested_channels;

		if (driver_bytes > cap->cap_stream_buf_bytes)  {
			HAL_AUDIO_ERROR("read bytes once is too large.");
			return HAL_OSAL_ERR_INVALID_PARAM;
		}

		if (driver_bytes_extra > cap->cap_stream_buf_bytes_extra)  {
			HAL_AUDIO_ERROR("read extra bytes once is too large.");
			return HAL_OSAL_ERR_INVALID_PARAM;
		}

		//HAL_AUDIO_INFO("%s: frames:%ld, bytes %u, app_frame_size:%d, driver_frame_size:%d, driver_frame_size_extra:%d, driver_bytes:%lu, driver_bytes_extra:%lu",
		//			__FUNCTION__, frames, bytes, app_frame_size, driver_frame_size, driver_frame_size_extra, driver_bytes, driver_bytes_extra);

		ameba_audio_stream_rx_read(cap->in_pcm, cap->stream_buf, driver_bytes);
		ameba_audio_stream_rx_read(cap->in_pcm_extra, cap->stream_buf_extra, driver_bytes_extra);

		for (i = 0; i < frames; i++) {
			memcpy((uint8_t *)buffer + i * app_frame_size, (uint8_t *)cap->stream_buf + i * driver_frame_size, driver_frame_size);
		}

		for (i = 0; i < frames; i++) {
			memcpy((uint8_t *)buffer + i * app_frame_size + driver_frame_size, (uint8_t *)cap->stream_buf_extra + i * driver_frame_size_extra, driver_frame_size_extra);
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

static ssize_t PrimaryCaptureRead(struct AudioHwCapture *stream, void *buffer, size_t bytes)
{
	int ret = 0;

	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)stream;
	//struct PrimaryAudioHwAdapter *adap = cap->adap;

	//rtos_mutex_take(adap->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(cap->lock, MUTEX_WAIT_TIMEOUT);
	if (cap->standby) {
		// in this StartAudioHwCapture, it needs to check capture mode, so this apis should be called
		// after setparameters. Because setparameters need to be set after RTAudioRecord_start(), so this api
		// can only be called in the first time call RTAudioRecord_read().Don't move it to other place.
		ret = StartAudioHwCapture(cap);
		if (ret == 0) {
			cap->standby = 0;
		} else {
			HAL_AUDIO_ERROR("start audio capture fail");
			goto exit;
		}
	}

	switch (cap->mode) {
	case CAPTURE_NO_AFE_PURE_DATA:
		ret = NoAfePureDataRead(stream, buffer, bytes);
		break;

	default:
		HAL_AUDIO_ERROR("mode(%d) not supported!", cap->mode);
		break;
	}

exit:
	//DelayUs((int64_t)bytes * 1000000 / PrimaryAudioHwCaptureFrameSize(stream) / PrimaryGetCaptureSampleRate(&stream->common));
	//memset(buffer, 0, bytes);
	//rtos_mutex_give(adap->lock);
	rtos_mutex_give(cap->lock);

	return ret;
}

static int CheckInputParameters(uint32_t sample_rate, AudioHwFormat format, uint32_t channel_count)
{
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_16_BIT:
	case AUDIO_HW_FORMAT_PCM_32_BIT:
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		break;
	default:
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	if ((channel_count < 1) || (channel_count > 10)) {
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

size_t GetHwInputBufferSize(uint32_t sample_rate, AudioHwFormat format, uint32_t channel_count)
{
	size_t size;

	if (CheckInputParameters(sample_rate, format, channel_count) != 0) {
		return 0;
	}

	size = (stream_input_config.period_size * sample_rate) / stream_input_config.rate;

	size = ((size + 15) / 16) * 16;

	return size * channel_count * sizeof(short);
}

void CloseAudioHwCapture(struct AudioHwCapture *capture)
{
	struct PrimaryAudioHwCapture *cap = (struct PrimaryAudioHwCapture *)capture;

	PrimaryStandbyCapture(&capture->common);

	if (cap->stream_buf) {
		rtos_mem_free(cap->stream_buf);
		cap->stream_buf = NULL;
	}

	if (cap->stream_buf_extra) {
		rtos_mem_free(cap->stream_buf_extra);
		cap->stream_buf_extra = NULL;
	}
	rtos_mutex_delete(cap->lock);
	rtos_mutex_delete(cap->time_lock);

	rtos_mem_free(capture);
	//capture = NULL;
}

//for passthrough, this api is called in RTAudioRecord_start(). Please pay attention to it when do logic change.
struct AudioHwCapture *GetAudioHwCaptureFuncs(struct AudioHwAdapter *adapter, const struct AudioHwDeviceDescriptor *desc,
		const struct AudioHwSampleAttributes *attrs, uint32_t flags)
{
	struct PrimaryAudioHwAdapter *ladap = (struct PrimaryAudioHwAdapter *)adapter;
	struct PrimaryAudioHwCapture *in;

	HAL_AUDIO_VERBOSE("primaryCreateCapture() with format:%d, sample_rate:%" PRId32 " channel_count:0x%lx", attrs->format, attrs->sample_rate,
					  attrs->channel_count);
	if (CheckInputParameters(attrs->sample_rate, attrs->format, attrs->channel_count) != 0) {
		HAL_AUDIO_ERROR("primaryCreateCapture: invalid config");
		return NULL;
	}

	in = (struct PrimaryAudioHwCapture *)rtos_mem_zmalloc(sizeof(struct PrimaryAudioHwCapture));
	if (!in) {
		return NULL;
	}

	in->adap = ladap;
	in->desc = *desc;

	in->stream.common.GetSampleRate = PrimaryGetCaptureSampleRate;
	in->stream.common.SetSampleRate = PrimarySetCaptureSampleRate;
	in->stream.common.GetBufferSize = PrimaryGetCaptureBufferSize;
	in->stream.common.GetChannels = PrimaryGetCaptureChannels;
	in->stream.common.SetChannels = PrimarySetCaptureChannels;
	in->stream.common.GetFormat = PrimaryGetCaptureFormat;
	in->stream.common.SetFormat = PrimarySetCaptureFormat;
	in->stream.common.Standby = PrimaryStandbyCapture;
	in->stream.common.Dump = PrimaryDumpCapture;
	in->stream.common.SetParameters = PrimarySetCaptureParameters;
	in->stream.common.GetParameters = PrimaryGetCaptureParameters;
	in->stream.GetLatency = PrimaryGetCaptureLatency;
	in->stream.GetCapturePosition = PrimaryGetCapturePosition;
	in->stream.GetPresentTime = PrimaryGetPresentTime;
	in->stream.GetTriggerTime = PrimaryGetTriggerTime;
	in->stream.Read = PrimaryCaptureRead;

	in->config = stream_input_config;
	in->config_extra = stream_input_config_extra;
	in->in_pcm = NULL;
	in->in_pcm_extra = NULL;
	in->stream_buf = NULL;
	in->cap_stream_buf_bytes = 0;
	in->stream_buf_extra = NULL;
	in->cap_stream_buf_bytes_extra = 0;
	in->standby = 1;
	in->rframe = 0;
	rtos_mutex_create(&in->lock);
	rtos_mutex_create(&in->time_lock);
	ladap->input = in;
	in->mode = CAPTURE_NO_AFE_PURE_DATA;
	in->device = AMEBA_AUDIO_IN_I2S;
	//in->device = AMEBA_AUDIO_IN_MIC;

	in->config.rate = attrs->sample_rate;
	in->config.format = attrs->format;

	if (attrs->channel_count == 10) {
		in->config.channels = 8;
		in->config_extra.channels = 2;
		in->config.need_sync_start = true;
		in->config_extra.need_sync_start = true;
	} else {
		in->config.channels = attrs->channel_count;
	}

	in->config_extra.rate = attrs->sample_rate;
	in->config_extra.format = attrs->format;
	in->config_extra.channels = attrs->channel_count;

	in->requested_channels = attrs->channel_count;
	in->channel_for_ref = 2;

	if (flags & AUDIO_HW_INPUT_FLAG_NOIRQ) {
		HAL_AUDIO_INFO("GetAudioHwCaptureFuncs in NO_IRQ mode, buffer_bytes: %" PRIu32 "", attrs->buffer_bytes);
		in->config.mode = AUDIO_HW_DMA_NOIRQ_MODE;
		in->config_extra.mode = AUDIO_HW_DMA_NOIRQ_MODE;
		if (attrs->buffer_bytes) {
			in->config.period_size = attrs->buffer_bytes / PrimaryAudioHwCaptureFrameSize(&in->stream);
			in->config_extra.period_size = attrs->buffer_bytes / PrimaryAudioHwCaptureFrameSize(&in->stream);
		} else {
			in->config.period_size = NOIRQ_CAPTURE_PERIOD_SIZE;
			in->config_extra.period_size = NOIRQ_CAPTURE_PERIOD_SIZE;
		}
	} else {
		in->config.mode = AUDIO_HW_DMA_IRQ_MODE;
		in->config_extra.mode = AUDIO_HW_DMA_IRQ_MODE;
		if (attrs->buffer_bytes) {
			in->config.period_size = attrs->buffer_bytes / PrimaryAudioHwCaptureFrameSize(&in->stream);
			in->config_extra.period_size = attrs->buffer_bytes / PrimaryAudioHwCaptureFrameSize(&in->stream);
		} else {
			in->config.period_size = CAPTURE_PERIOD_SIZE;
			in->config_extra.period_size = CAPTURE_PERIOD_SIZE;
		}
	}

#if (NO_AFE_PURE_DATA_DUMP || NO_AFE_ALL_DATA_DUMP)
	in->in_buf = NULL;
	in->out_buf = NULL;
#endif

	//both dmic_ref_amic and in_mic are in ameba's soc codec control, I2S does not use ameba's soc codec control.
	if (desc->pins == AUDIO_HW_PIN_IN_DMIC_REF_AMIC) {
		HAL_AUDIO_INFO("ref dmic & amic case");
		in->device = AMEBA_AUDIO_IN_MIC;
		ameba_audio_ctl_set_mic_usage(ameba_audio_get_ctl(), AMEBA_AUDIO_CAPTURE_USAGE_DMIC_REF_AMIC);
	} else if (desc->pins == AUDIO_HW_PIN_IN_MIC) {
		HAL_AUDIO_INFO("ref dmic case");
		in->device = AMEBA_AUDIO_IN_MIC;
		ameba_audio_ctl_set_mic_usage(ameba_audio_get_ctl(), AMEBA_AUDIO_CAPTURE_USAGE_DMIC);
	} else if (desc->pins == AUDIO_HW_PIN_IN_I2S) {
		HAL_AUDIO_INFO("i2s case");
		in->device = AMEBA_AUDIO_IN_I2S;
		in->config.sport_index = AUDIO_I2S_IN_SPORT_INDEX;
		in->config_extra.sport_index = AUDIO_I2S_IN_EXTRA_SPORT_INDEX;
		in->config.is_multi_io = AUDIO_I2S_IN_MULTIIO_EN;
		in->config_extra.is_multi_io = AUDIO_I2S_IN_MULTIIO_EN;
	} else {
		HAL_AUDIO_ERROR("pins:%d for capture not supported, now using default amic instead", desc->pins);
	}

	HAL_AUDIO_VERBOSE("%s done", __FUNCTION__);
	return &in->stream;
}
