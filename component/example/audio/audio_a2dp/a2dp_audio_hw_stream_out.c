/*
 * Copyright (c) 2025 Realtek, LLC.
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

#define LOG_TAG "A2DP Hal"

#include <inttypes.h>
#include <rtk_bt_def.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <bt_api_config.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <rtk_bt_avrcp.h>
#include <rtk_bt_a2dp.h>
#include <bt_audio_intf.h>
#include <bt_audio_track_api.h>
#include <sbc_codec_entity.h>

#include "os_wrapper.h"

#include "audio_hw_compat.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_debug.h"
#include "audio_hw_params_handle.h"
#include "ameba_audio_stream_buffer.h"

#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_stream_out.h"

#include "a2dp_audio_hw_card.h"

#define DEFAULT_OUT_SAMPLING_RATE 16000
#define NOIRQ_SHORT_PERIOD_SIZE   384
#define SHORT_PERIOD_SIZE         1024
#define SHORT_PERIOD_COUNT        4
#define DUMP_FRAME                192000
#define DUMP_ENABLE               0

#define DELAY_START              "delay_start"

struct A2dpAudioHwStreamOut {
	struct AudioHwStreamOut stream;

	rtos_mutex_t lock;
	size_t buffer_frames;
	int32_t standby;
	enum AudioHwFormat format;
	uint32_t channel_count;
	uint32_t sample_rate;
	struct A2dpAudioHwCard *pri_card;
	struct AudioHwPathDescriptor desc;
	uint32_t amp_pin;

	//max value should sync with ameba audio driver's total_counter_boundary.
	uint16_t sequence;
	uint64_t written;
	bool delay_start;

	rtos_task_t rb_task;
	bool rb_task_running;
	AudioBuffer *rb;
	int32_t rb_size;
	void *rb_sem;
	bool rb_sem_taken;
	int8_t *send_buffer;
};

extern void *a2dp_demo_codec_entity;
extern uint8_t remote_bd_addr[6];
extern rtk_bt_sbc_codec_t sbc_codec_t;
extern uint8_t src_a2dp_credits;
//notice: this size is frames * channels, not real bytes.
extern uint16_t src_a2dp_send_data_size;
static void *src_send_timer = NULL;
static void *src_send_sem = NULL;
static bool src_send_sem_taken = false;
static uint32_t send_interval_ms = 0;
static int32_t send_bytes = 0;

static inline size_t A2dpAudioHwStreamOutFrameSize(const struct AudioHwStreamOut *s)
{
	int32_t chan_samp_sz;
	enum AudioHwFormat format = s->common.GetFormat(&s->common);

	if (AudioIsLinearPCM(format)) {
		chan_samp_sz = GetAudioBytesPerSample(format);
		size_t frame_size = s->common.GetChannels(&s->common) * chan_samp_sz;
		HAL_AUDIO_VERBOSE("Stream out frame size:%d", frame_size);
		return frame_size;
	}

	return sizeof(int8_t);
}

static uint32_t A2dpGetStreamOutSampleRate(const struct AudioHwStream *stream)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	return out->sample_rate;
}

static int32_t A2dpSetStreamOutSampleRate(struct AudioHwStream *stream, uint32_t rate)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	out->sample_rate = rate;
	return HAL_OSAL_OK;
}

static size_t A2dpGetStreamOutBufferSize(const struct AudioHwStream *stream)
{
	(void) stream;
	//todo
	return 1024;
}

static uint32_t A2dpGetStreamOutChannels(const struct AudioHwStream *stream)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

	return out->channel_count;
}

static int32_t A2dpSetStreamOutChannels(const struct AudioHwStream *stream, uint32_t channel)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

	out->channel_count = channel;
	return HAL_OSAL_OK;
}

static enum AudioHwFormat A2dpGetStreamOutFormat(const struct AudioHwStream *stream)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

	return out->format;
}

static int32_t A2dpSetStreamOutFormat(struct AudioHwStream *stream, enum AudioHwFormat format)
{
	(void) stream;
	(void) format;
	return HAL_OSAL_OK;
}

/* must be called with hw device and output stream mutexes locked */
static int32_t DoStandbyOutput(struct A2dpAudioHwStreamOut *out)
{
	if (!out->standby) {
		out->standby = 1;
	}

	while (out->rb_task_running) {
		rtos_time_delay_ms(2);
	}

	if (src_send_timer) {
		rtos_timer_stop(src_send_timer, 0);
		rtos_timer_delete(src_send_timer, 0);
		src_send_timer = NULL;
	}
	if (src_send_sem) {
		rtos_sema_delete(src_send_sem);
		src_send_sem = NULL;
	}

	HAL_AUDIO_INFO("a2dp standby");
	return HAL_OSAL_OK;
}

static int32_t A2dpStandbyStreamOut(struct AudioHwStream *stream)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	int32_t status;

	//rtos_mutex_take(out->pri_card->lock, MUTEX_WAIT_TIMEOUT);
	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	status = DoStandbyOutput(out);
	rtos_mutex_give(out->lock);
	//rtos_mutex_give(out->pri_card->lock);

	return status;
}

static int32_t A2dpDumpStreamOut(const struct AudioHwStream *stream, int32_t fd)
{
	(void) stream;
	(void) fd;
	return HAL_OSAL_OK;
}

static int32_t A2dpGetStreamOutBufferStatus(struct AudioHwStream *stream)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	if (!out) {
		HAL_AUDIO_ERROR("A2dpGetStreamOutBufferStatus stream_out is not initialized");
		return HAL_OSAL_ERR_NO_INIT;
	}

	//todo
	return 0;
}

static int32_t A2dpSetStreamOutParameters(struct AudioHwStream *stream, const char *str_pairs)
{
	HAL_AUDIO_INFO("%s, keys = %s", __FUNCTION__, str_pairs);
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	struct string_cell *cells;
	cells = string_cells_create_from_str(str_pairs);
	int32_t value;

	if (string_cells_has_key(cells, DELAY_START)) {
		string_cells_get_int(cells, DELAY_START, &value);
		out->delay_start = value == 1 ? true : false;
	}

	string_cells_destroy(cells);
	return HAL_OSAL_OK;
}

static char *A2dpGetStreamOutParameters(const struct AudioHwStream *stream, const char *keys)
{
	(void) stream;
	(void) keys;
	return (char *)strdup("");
}

static uint32_t A2dpGetStreamOutLatency(const struct AudioHwStreamOut *stream)
{
	(void) stream;
	//todo
	return 0;
}

static int32_t A2dpGetPresentationPosition(const struct AudioHwStreamOut *stream, uint64_t *frames, struct timespec *timestamp)
{
	(void) frames;
	(void) timestamp;

	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	int32_t ret = -1;

	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	//todo
	rtos_mutex_give(out->lock);

	return ret;
}

static int32_t A2dpGetPresentTime(const struct AudioHwStreamOut *stream, int64_t *now_ns, int64_t *audio_ns)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
	int32_t ret = -1;
	(void) now_ns;
	(void) audio_ns;

	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	//todo get more accurate time.
	rtos_mutex_give(out->lock);

	return ret;
}

static int64_t A2dpGetTriggerTime(const struct AudioHwStreamOut *stream)
{
	(void) stream;
	//todo get start time.
	return 0;
}

static int32_t A2dpSetStreamOutVolume(struct AudioHwStreamOut *stream, float left,
									  float right)
{
	HAL_AUDIO_VERBOSE("A2dpSetStreamOutVolume enter, left: %f, right: %f", left, right);
	(void) stream;
	(void) left;
	(void) right;
	//todo set a2dp volume.
	return HAL_OSAL_OK;
}

static int8_t A2dpGetSrcA2dpCredits(void)
{
	return src_a2dp_credits;
}

static void A2dpSrcA2dpCreditsDec(void)
{
	src_a2dp_credits--;
}

static void src_send_timer_handler(void *arg)
{
	(void) arg;

	if (src_send_sem && src_send_sem_taken) {
		HAL_AUDIO_VERBOSE("timer enter");
		rtos_sema_give(src_send_sem);
	}
}

static ssize_t A2dpWrite(struct AudioHwStreamOut *stream, const void *buffer, size_t bytes, bool block)
{
	(void) block;
	rtk_bt_a2dp_stream_data_send_t data_send_t = {0};
	struct enc_codec_buffer *enc_buffer = NULL;
	int res = bytes;

	if (!a2dp_demo_codec_entity) {
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

	enc_buffer = rtk_bt_audio_data_encode(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, (short *)buffer, bytes);
	if (enc_buffer) {
		memset((void *)&data_send_t, 0, sizeof(rtk_bt_a2dp_stream_data_send_t));
		memcpy((void *)data_send_t.bd_addr, (void *)remote_bd_addr, 6);
		data_send_t.seq_num = out->sequence;
		data_send_t.frame_buf = (uint8_t *)enc_buffer->pbuffer;
		data_send_t.frame_num = (uint8_t)enc_buffer->frame_num;
		data_send_t.time_stamp += data_send_t.frame_num * sbc_codec_t.encoder_t.subbands * sbc_codec_t.encoder_t.blocks;
		data_send_t.len = (uint16_t)(enc_buffer->frame_num * enc_buffer->frame_size);
		data_send_t.flush = false;
		res = rtk_bt_a2dp_data_send(&data_send_t);
		if (res) {
			HAL_AUDIO_ERROR("[A2DP] data send fail %d \r\n", bytes);
		} else {
			HAL_AUDIO_VERBOSE("A2dp send:%d, ret:%d, seq:%d, fb:%p, fn:%d, ts:%ld, len:%d",
							  bytes, res, data_send_t.seq_num, data_send_t.frame_buf, data_send_t.frame_num, data_send_t.time_stamp, data_send_t.len);
		}
		rtk_bt_audio_free_encode_buffer(RTK_BT_AUDIO_CODEC_SBC, a2dp_demo_codec_entity, enc_buffer);
		out->sequence++;
	} else {
		HAL_AUDIO_ERROR("[A2DP SRC Demo]: Encode fail \r\n");
	}

	return res;
}

static void read_ring_buffer_task(void *param)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)param;
	int32_t max_wait_ms = 2 * send_bytes * 1000 /
						  A2dpAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)out) /
						  out->sample_rate;

	HAL_AUDIO_INFO("enter ring buffer task.");

	while (!out->standby) {
		src_send_sem_taken = true;
		rtos_sema_take(src_send_sem, max_wait_ms);
		src_send_sem_taken = false;

		int32_t size_read = ameba_audio_stream_buffer_read(out->rb, out->send_buffer, send_bytes, 0);
		if (out->rb_sem_taken) {
			rtos_sema_give(out->rb_sem);
		}

		if (size_read < send_bytes) {
			HAL_AUDIO_ERROR("underrun.");
		} else {
			int32_t max_credit_cnt = 20;
			while (max_credit_cnt) {
				if (!A2dpGetSrcA2dpCredits()) {
					rtos_time_delay_ms(2);
					max_credit_cnt--;
				} else {
					int32_t ret = A2dpWrite((struct AudioHwStreamOut *)out, out->send_buffer, send_bytes, true);
					if (ret == 0) {
						A2dpSrcA2dpCreditsDec();
					} else {
						HAL_AUDIO_VERBOSE("Error send data.");
					}
					break;
				}
			}
		}
	}

	HAL_AUDIO_INFO("rb task exit.");
	out->rb_task_running = false;
	rtos_task_delete(NULL);
}

/* must be called with hw device and output stream mutexes locked */
static int32_t StartAudioHwStreamOut(struct A2dpAudioHwStreamOut *out)
{
	HAL_AUDIO_VERBOSE("start output stream enter");
	send_interval_ms = send_bytes * 1000 / A2dpAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)out) / out->sample_rate;

	if (src_send_sem == NULL) {
		rtos_sema_create(&src_send_sem, 0, RTOS_SEMA_MAX_COUNT);
	}

	if (src_send_timer == NULL) {
		HAL_AUDIO_INFO("create timer %ld", send_interval_ms);
		if (RTK_SUCCESS != rtos_timer_create(&src_send_timer, "src_send_timer", 1, send_interval_ms, true,
											 src_send_timer_handler)) {
			HAL_AUDIO_ERROR("create timer fail");
			goto err_create;
		}

		if (RTK_SUCCESS != rtos_timer_start(src_send_timer, 0)) {
			HAL_AUDIO_ERROR("start timer fail");
			goto err_start;
		}
	}

	if (rtos_task_create(&out->rb_task, ((const char *)"ahal_rb_task"), read_ring_buffer_task, out, 8192, 1) != RTK_SUCCESS) {
		HAL_AUDIO_ERROR("create ahal_rb_task fail");
		goto err_start;
	}

	out->rb_task_running = true;

	return HAL_OSAL_OK;

err_start:
	HAL_AUDIO_ERROR("err start");
	rtos_timer_delete(src_send_timer, 0);

err_create:
	return HAL_OSAL_ERR_UNKNOWN_ERROR;
}

static ssize_t A2dpStreamOutWrite(struct AudioHwStreamOut *stream, const void *buffer,
								  size_t bytes, bool block)
{
	(void) block;
	HAL_AUDIO_VERBOSE("write:%ld", bytes);

	int32_t ret = 0;
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

	size_t frame_size = A2dpAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);
	int32_t max_wait_ms = 2 * send_bytes * 1000 / frame_size / out->sample_rate;

	rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
	if (out->standby) {
		out->standby = 0;
		ret = StartAudioHwStreamOut(out);
		if (ret != 0) {
			HAL_AUDIO_ERROR("start stream_out fail");
			goto exit;
		}
	}

	if (bytes > (size_t)out->rb_size) {
		HAL_AUDIO_WARN("make sure write less than rb size once, please check A2dpGetStreamOutBufferSize's return value.");
	}

	int32_t bytes_left = bytes;
	while (bytes_left) {
		if (ameba_audio_stream_buffer_get_available_size(out->rb) < (size_t)bytes_left) {
			out->rb_sem_taken = true;
			ret = rtos_sema_take(out->rb_sem, max_wait_ms);
			if (ret < 0) {
				//wait timeout
				break;
			}
			out->rb_sem_taken = false;
		}

		int32_t written = ameba_audio_stream_buffer_write(out->rb, (int8_t *)buffer + bytes - bytes_left, bytes_left);
		bytes_left -= written;
	}

	//write successfully
	ret = bytes - bytes_left;
	out->written += bytes / frame_size;
	//sync with ameba audio driver's total_counter_boundary max value.
	if (out->written > UINT64_MAX) {
		out->written = 0;
	}

exit:
	rtos_mutex_give(out->lock);

	return ret;
}

void DestroyA2dpAudioHwStreamOut(struct AudioHwStreamOut *stream_out)
{
	struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream_out;

	A2dpStandbyStreamOut(&stream_out->common);

	rtos_mutex_delete(out->lock);
	ameba_audio_stream_buffer_release(out->rb);
	rtos_sema_delete(out->rb_sem);
	rtos_mem_free(out->send_buffer);
	rtos_mem_free(out);

	HAL_AUDIO_INFO("DestroyA2dpAudioHwStreamOut");
}

struct AudioHwStreamOut *CreateA2dpAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
		const struct AudioHwConfig *config)
{
	struct A2dpAudioHwCard *pri_card = (struct A2dpAudioHwCard *)card;
	struct A2dpAudioHwStreamOut *out;

	out = (struct A2dpAudioHwStreamOut *)rtos_mem_zmalloc(sizeof(struct A2dpAudioHwStreamOut));
	if (!out) {
		return NULL;
	}

	out->pri_card = pri_card;
	out->desc = *desc;

	out->stream.common.GetSampleRate = A2dpGetStreamOutSampleRate;
	out->stream.common.SetSampleRate = A2dpSetStreamOutSampleRate;
	out->stream.common.GetBufferSize = A2dpGetStreamOutBufferSize;
	out->stream.common.GetChannels = A2dpGetStreamOutChannels;
	out->stream.common.SetChannels = A2dpSetStreamOutChannels;
	out->stream.common.GetFormat = A2dpGetStreamOutFormat;
	out->stream.common.SetFormat = A2dpSetStreamOutFormat;
	out->stream.common.Standby = A2dpStandbyStreamOut;
	out->stream.common.Dump = A2dpDumpStreamOut;
	out->stream.common.SetParameters = A2dpSetStreamOutParameters;
	out->stream.common.GetParameters = A2dpGetStreamOutParameters;
	out->stream.common.GetBufferStatus = A2dpGetStreamOutBufferStatus;
	out->stream.GetPresentationPosition = A2dpGetPresentationPosition;
	out->stream.GetPresentTime = A2dpGetPresentTime;
	out->stream.GetTriggerTime = A2dpGetTriggerTime;
	out->stream.GetLatency = A2dpGetStreamOutLatency;
	out->stream.SetVolume = A2dpSetStreamOutVolume;
	out->stream.Write = A2dpStreamOutWrite;

	out->format = AUDIO_HW_FORMAT_PCM_16_BIT;
	out->channel_count = 2;
	out->sample_rate = DEFAULT_OUT_SAMPLING_RATE;
	out->pri_card = pri_card;
	out->standby = 1;
	out->sequence = 0;
	out->written = 0;
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
	//src_a2dp_send_data_size is the number of uint16_t.
	send_bytes = src_a2dp_send_data_size * 2;
	out->send_buffer = (int8_t *)rtos_mem_zmalloc(send_bytes);
	out->rb_size = send_bytes * 4;
	out->rb = ameba_audio_stream_buffer_create();
	if (!out->rb) {
		HAL_AUDIO_ERROR("ring buffer create fail");
		return NULL;
	}
	ameba_audio_stream_buffer_alloc(out->rb, out->rb_size);

	out->rb_task_running = false;
	rtos_sema_create(&out->rb_sem, 0, RTOS_SEMA_MAX_COUNT);
	out->rb_sem_taken = false;

	pri_card->output = out;

	HAL_AUDIO_INFO("a2dp rate:%ld, channels:%ld, format:%d rp:%p sp:%p sd:%ld",
				   out->sample_rate, out->channel_count, out->format, out->rb, out->send_buffer, send_bytes);

	return &out->stream;
}
