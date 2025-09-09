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
#include "usbh_uac1.h"
#include "usbh.h"

#include "audio_hw_compat.h"
#include "audio_hw_osal_errnos.h"
#include "audio_hw_debug.h"
#include "audio_hw_params_handle.h"

#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_utils.h"
#include "hardware/audio/audio_hw_stream_out.h"

#include "usb_audio_hw_card.h"

#define DEFAULT_OUT_SAMPLING_RATE 16000
#define NOIRQ_SHORT_PERIOD_SIZE   384
#define SHORT_PERIOD_SIZE         1024
#define SHORT_PERIOD_COUNT        4
#define DUMP_FRAME                192000
#define DUMP_ENABLE               0

#define DELAY_START              "delay_start"

struct UsbAudioHwStreamOut {
    struct AudioHwStreamOut stream;

    rtos_mutex_t lock;
    size_t buffer_frames;
    int32_t standby;
    enum AudioHwFormat format;
    uint32_t channel_count;
    uint32_t sample_rate;
    struct UsbAudioHwCard *pri_card;
    struct AudioHwPathDescriptor desc;
    uint32_t amp_pin;

    //max value should sync with ameba audio driver's total_counter_boundary.
    uint16_t sequence;
    uint64_t written;
    bool delay_start;
};

static inline size_t UsbAudioHwStreamOutFrameSize(const struct AudioHwStreamOut *s)
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

static uint32_t UsbGetStreamOutSampleRate(const struct AudioHwStream *stream)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    return out->sample_rate;
}

static int32_t UsbSetStreamOutSampleRate(struct AudioHwStream *stream, uint32_t rate)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    out->sample_rate = rate;
    return HAL_OSAL_OK;
}

static size_t UsbGetStreamOutBufferSize(const struct AudioHwStream *stream)
{
    (void) stream;
    //todo
    return 1024;
}

static uint32_t UsbGetStreamOutChannels(const struct AudioHwStream *stream)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;

    return out->channel_count;
}

static int32_t UsbSetStreamOutChannels(const struct AudioHwStream *stream, uint32_t channel)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;

    out->channel_count = channel;
    return HAL_OSAL_OK;
}

static enum AudioHwFormat UsbGetStreamOutFormat(const struct AudioHwStream *stream)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;

    return out->format;
}

static int32_t UsbSetStreamOutFormat(struct AudioHwStream *stream, enum AudioHwFormat format)
{
    (void) stream;
    (void) format;
    return HAL_OSAL_OK;
}

/* must be called with hw device and output stream mutexes locked */
static int32_t DoStandbyOutput(struct UsbAudioHwStreamOut *out)
{
    if (!out->standby) {
        out->standby = 1;
    }
    return HAL_OSAL_OK;
}

static int32_t UsbStandbyStreamOut(struct AudioHwStream *stream)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    int32_t status;

    //rtos_mutex_take(out->pri_card->lock, MUTEX_WAIT_TIMEOUT);
    rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
    status = DoStandbyOutput(out);
    rtos_mutex_give(out->lock);
    //rtos_mutex_give(out->pri_card->lock);

    return status;
}

static int32_t UsbDumpStreamOut(const struct AudioHwStream *stream, int32_t fd)
{
    (void) stream;
    (void) fd;
    return HAL_OSAL_OK;
}

static int32_t UsbGetStreamOutBufferStatus(struct AudioHwStream *stream)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    if (!out) {
        HAL_AUDIO_ERROR("UsbGetStreamOutBufferStatus stream_out is not initialized");
        return HAL_OSAL_ERR_NO_INIT;
    }

    //todo
    return 0;
}

static int32_t UsbSetStreamOutParameters(struct AudioHwStream *stream, const char *str_pairs)
{
    HAL_AUDIO_INFO("%s, keys = %s", __FUNCTION__, str_pairs);
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
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

static char *UsbGetStreamOutParameters(const struct AudioHwStream *stream, const char *keys)
{
    (void) stream;
    (void) keys;
    return (char *)strdup("");
}

static uint32_t UsbGetStreamOutLatency(const struct AudioHwStreamOut *stream)
{
    (void) stream;
    //todo
    return 0;
}

static int32_t UsbGetPresentationPosition(const struct AudioHwStreamOut *stream, uint64_t *frames, struct timespec *timestamp)
{
    (void) frames;
    (void) timestamp;

    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    int32_t ret = -1;

    rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
    //todo
    rtos_mutex_give(out->lock);

    return ret;
}

static int32_t UsbGetPresentTime(const struct AudioHwStreamOut *stream, int64_t *now_ns, int64_t *audio_ns)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    int32_t ret = -1;
    (void) now_ns;
    (void) audio_ns;

    rtos_mutex_take(out->lock, MUTEX_WAIT_TIMEOUT);
    //todo get more accurate time.
    rtos_mutex_give(out->lock);

    return ret;
}

static int64_t UsbGetTriggerTime(const struct AudioHwStreamOut *stream)
{
    (void) stream;
    //todo get start time.
    return 0;
}

static int32_t UsbSetStreamOutVolume(struct AudioHwStreamOut *stream, float left,
                                     float right)
{
    HAL_AUDIO_VERBOSE("UsbSetStreamOutVolume enter, left: %f, right: %f", left, right);
    (void) stream;
    (void) left;
    (void) right;
    //todo set usb volume.
    return HAL_OSAL_OK;
}

static ssize_t UsbStreamOutWrite(struct AudioHwStreamOut *stream, const void *buffer,
                                     size_t bytes, bool block)
{
    HAL_AUDIO_VERBOSE("write: %u %d", bytes, block);
    (void) block;
    int32_t ret = 0;
    int32_t timeout_ms = 1000 * 10;
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream;
    size_t frame_size = UsbAudioHwStreamOutFrameSize((const struct AudioHwStreamOut *)stream);

    ret = usbh_uac_write((uint8_t *)(buffer), bytes, timeout_ms);
    if (ret != (int32_t)bytes) {
        HAL_AUDIO_INFO("bytes: %d, written: %ld", bytes, ret);
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

    rtos_mutex_give(out->lock);

    return ret;
}

void DestroyUsbAudioHwStreamOut(struct AudioHwStreamOut *stream_out)
{
    struct UsbAudioHwStreamOut *out = (struct UsbAudioHwStreamOut *)stream_out;

    UsbStandbyStreamOut(&stream_out->common);

    rtos_mutex_delete(out->lock);
    rtos_mem_free(out);

    HAL_AUDIO_INFO("DestroyUsbAudioHwStreamOut");
}

struct AudioHwStreamOut *CreateUsbAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
        const struct AudioHwConfig *config)
{
    struct UsbAudioHwCard *pri_card = (struct UsbAudioHwCard *)card;
    struct UsbAudioHwStreamOut *out;

    out = (struct UsbAudioHwStreamOut *)rtos_mem_zmalloc(sizeof(struct UsbAudioHwStreamOut));
    if (!out) {
        return NULL;
    }

    out->pri_card = pri_card;
    out->desc = *desc;

    out->stream.common.GetSampleRate = UsbGetStreamOutSampleRate;
    out->stream.common.SetSampleRate = UsbSetStreamOutSampleRate;
    out->stream.common.GetBufferSize = UsbGetStreamOutBufferSize;
    out->stream.common.GetChannels = UsbGetStreamOutChannels;
    out->stream.common.SetChannels = UsbSetStreamOutChannels;
    out->stream.common.GetFormat = UsbGetStreamOutFormat;
    out->stream.common.SetFormat = UsbSetStreamOutFormat;
    out->stream.common.Standby = UsbStandbyStreamOut;
    out->stream.common.Dump = UsbDumpStreamOut;
    out->stream.common.SetParameters = UsbSetStreamOutParameters;
    out->stream.common.GetParameters = UsbGetStreamOutParameters;
    out->stream.common.GetBufferStatus = UsbGetStreamOutBufferStatus;
    out->stream.GetPresentationPosition = UsbGetPresentationPosition;
    out->stream.GetPresentTime = UsbGetPresentTime;
    out->stream.GetTriggerTime = UsbGetTriggerTime;
    out->stream.GetLatency = UsbGetStreamOutLatency;
    out->stream.SetVolume = UsbSetStreamOutVolume;
    out->stream.Write = UsbStreamOutWrite;

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

    HAL_AUDIO_INFO("usb rate:%ld, channels:%ld, format:%d",
                    out->sample_rate, out->channel_count, out->format);

    rtos_mutex_create(&out->lock);

    pri_card->output = out;

    return &out->stream;
}