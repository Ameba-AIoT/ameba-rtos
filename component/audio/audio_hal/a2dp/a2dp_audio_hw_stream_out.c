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

#include <inttypes.h>

#include "os_wrapper.h"

#include "audio_hw_compat.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_debug.h"
#include "audio_hw_params_handle.h"

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
};

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

/* must be called with hw device and output stream mutexes locked */
static int32_t StartAudioHwStreamOut(struct A2dpAudioHwStreamOut *out)
{
    HAL_AUDIO_VERBOSE("start output stream enter");
    (void) out;
    //todo set a2dp start.
    return HAL_OSAL_OK;
}

static ssize_t A2dpWrite(struct AudioHwStreamOut *stream, const void *buffer, size_t bytes, bool block)
{

    (void) stream;
    (void) buffer;
    (void) bytes;
    (void) block;

    HAL_AUDIO_INFO("a2dp write.");
    rtos_time_delay_ms(10);

    return 0;
}

static int8_t A2dpGetSrcA2dpCredits(void)
{
    return 1;
}

static void A2dpSrcA2dpCreditsDec(void)
{
}

static ssize_t A2dpStreamOutWrite(struct AudioHwStreamOut *stream, const void *buffer,
                                     size_t bytes, bool block)
{
    HAL_AUDIO_PVERBOSE("primaryStreamOutWrite: bytes: %u", bytes);

    int32_t ret = 0;
    struct A2dpAudioHwStreamOut *out = (struct A2dpAudioHwStreamOut *)stream;
    //struct A2dpAudioHwCard *pri_card = out->pri_card;

    size_t frame_size = A2dpAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);

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

    size_t bytes_left = bytes;
    int32_t max_write_cnt = 5;
    while (bytes_left && max_write_cnt)
    {
        max_write_cnt--;

        int32_t max_credit_cnt = 20;
        while(max_credit_cnt) {
            if (!A2dpGetSrcA2dpCredits()) {
                rtos_time_delay_ms(2);
                max_credit_cnt--;
            } else {
                size_t send_size = bytes_left > 1024 ? 1024 : bytes_left;
                ret = A2dpWrite(stream, (int8_t *)buffer + bytes - bytes_left, send_size, block);
                if (ret == 0) {
                    bytes_left -= send_size;
                    A2dpSrcA2dpCreditsDec();
                }
                break;
            }
        }

        if (ret < 0) {
            break;
        }
    }

    //write successfully
    if (ret >= 0) {
        ret = bytes;
        out->written += bytes / frame_size;
        //sync with ameba audio driver's total_counter_boundary max value.
        if (out->written > UINT64_MAX) {
            out->written = 0;
        }
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

    HAL_AUDIO_INFO("a2dp rate:%ld, channels:%ld, format:%d",
                    out->sample_rate, out->channel_count, out->format);

    rtos_mutex_create(&out->lock);

    pri_card->output = out;

    return &out->stream;
}
