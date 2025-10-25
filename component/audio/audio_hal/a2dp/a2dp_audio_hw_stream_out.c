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

#include "os_wrapper.h"

#include "audio_hw_compat.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_debug.h"
#include "audio_hw_params_handle.h"
#include "ameba_audio_stream_buffer.h"

#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_stream_out.h"

#include "a2dp_audio_ring_buffer.h"

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

    AudioBuffer *rb;
    int32_t rb_size;
    void *rb_sem;
    bool rb_sem_taken;
};

struct A2dpAudioHwStreamOut *k_out = NULL;

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

int32_t a2dp_hal_buffer_read(int8_t *buffer, int32_t bytes)
{
    uint32_t remain_size = 0;
    int32_t size_read = 0;

    if (!k_out) {
        HAL_AUDIO_WARN("a2dp wait for init");
        return HAL_OSAL_ERR_NO_INIT;
    }
    remain_size = ameba_audio_stream_buffer_get_remain_size(k_out->rb);

    if ((uint32_t)bytes > remain_size) {
        HAL_AUDIO_ERROR("byte bigger than remain size:%ld", remain_size);
        return 0;
    }
    size_read = ameba_audio_stream_buffer_read(k_out->rb, buffer, bytes, 0);
    if (k_out->rb_sem_taken) {
        rtos_sema_give(k_out->rb_sem);
    }
    if (bytes != size_read) {
        printf("prefer size %d, size read %d \r\n", (int)bytes, (int)size_read);
    }

    return size_read;
}

/* must be called with hw device and output stream mutexes locked */
static int32_t StartAudioHwStreamOut(struct A2dpAudioHwStreamOut *out)
{
    HAL_AUDIO_VERBOSE("start output stream enter");

    (void) out;
    return HAL_OSAL_OK;
}

static ssize_t A2dpStreamOutWrite(struct AudioHwStreamOut *stream, const void *buffer,
                                    size_t bytes, bool block)
{
    (void) block;
    HAL_AUDIO_VERBOSE("write:%ld", bytes);

    int32_t ret = 0;
    struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;

    size_t frame_size = A2dpAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);
    int32_t max_wait_ms = 500;

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
            out->rb_sem_taken = false;
            if (ret < 0) {
                //wait timeout
                break;
            }
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

    out->rb_size = 1024 * 20;
    out->rb = ameba_audio_stream_buffer_create();
    if (!out->rb) {
        HAL_AUDIO_ERROR("ring buffer create fail");
        return NULL;
    }
    ameba_audio_stream_buffer_alloc(out->rb, out->rb_size);

    rtos_sema_create(&out->rb_sem, 0, RTOS_SEMA_MAX_COUNT);
    out->rb_sem_taken = false;

    k_out = out;

    pri_card->output = out;

    HAL_AUDIO_INFO("a2dp rate:%ld, channels:%ld, format:%d",
                    out->sample_rate, out->channel_count, out->format);

    return &out->stream;
}
