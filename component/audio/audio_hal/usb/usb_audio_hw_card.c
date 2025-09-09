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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "os_wrapper.h"

#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"

#include "usb_audio_hw_card.h"

// ----------------------------------------------------------------------------
// AudioHwCard
extern struct AudioHwStreamOut *CreateUsbAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
        const struct AudioHwConfig *config);
extern void DestroyUsbAudioHwStreamOut(struct AudioHwStreamOut *stream_out);
extern struct AudioHwStreamIn *CreateAudioHwStreamIn(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
        const struct AudioHwConfig *config);
extern void DestroyAudioHwStreamIn(struct AudioHwStreamIn *stream_in);

static int32_t UsbSetCardParameters(struct AudioHwCard *card, const char *strs)
{
    (void) card;
    (void) strs;
    return HAL_OSAL_ERR_INVALID_OPERATION;
}

static char *UsbGetCardParameters(const struct AudioHwCard *card,
                                      const char *keys)
{
    (void) card;
    (void) keys;
    return (char *)strdup("");
}

static int32_t UsbSetCaptureVolume(const struct AudioHwCard *card, float volume)
{
    (void) card;
    (void) volume;
    return HAL_OSAL_OK;
}

static int32_t UsbGetCaptureVolume(const struct AudioHwCard *card, float *volume)
{
    (void) card;
    (void) volume;
    return 0;
}

static int32_t UsbSetRenderVolume(const struct AudioHwCard *card, float volume)
{
    (void) card;
    (void) volume;
    return HAL_OSAL_OK;
}

static int32_t UsbGetRenderVolume(const struct AudioHwCard *card, float *volume)
{
    (void) card;
    (void) volume;
    return 0;
}

static int32_t UsbSetRenderMute(const struct AudioHwCard *card, bool muted)
{
    (void) card;
    (void) muted;
    return HAL_OSAL_OK;
}

static int32_t UsbGetRenderMute(const struct AudioHwCard *card, bool *muted)
{
    (void) card;
    (void) muted;
    return 0;
}

static int32_t UsbSetCaptureMute(const struct AudioHwCard *card, bool muted)
{
    (void) card;
    (void) muted;
    return HAL_OSAL_OK;
}

static int32_t UsbGetCaptureMute(const struct AudioHwCard *card, bool *muted)
{
    (void) card;
    (void) muted;
    return 0;
}

static size_t UsbGetCardInputBufferSize(const struct AudioHwCard *card, const struct AudioHwConfig *config)
{
    (void) card;
    if (config == NULL) {
        HAL_AUDIO_ERROR("primaryGetCardInputBufferSize AudioHwConfig is null");
        return HAL_OSAL_ERR_INVALID_PARAM;
    }

    return GetHwInputBufferSize(config->sample_rate, config->format, config->channel_count);
}

static struct AudioHwStreamOut *UsbCreateStreamOut(
    struct AudioHwCard *card,
    const struct AudioHwPathDescriptor *desc,
    const struct AudioHwConfig *config)
{
    return CreateUsbAudioHwStreamOut(card, desc, config);
}

static void UsbDestroyStreamOut(struct AudioHwCard *card, struct AudioHwStreamOut *stream_out)
{
    (void) card;
    DestroyUsbAudioHwStreamOut(stream_out);

}


static struct AudioHwStreamIn *UsbCreateStreamIn(
    struct AudioHwCard *card,
    const struct AudioHwPathDescriptor *desc,
    const struct AudioHwConfig *config)
{
    return CreateAudioHwStreamIn(card, desc, config);
}

static void UsbDestroyStreamIn(struct AudioHwCard *card, struct AudioHwStreamIn *stream_in)
{
    (void) card;
    DestroyAudioHwStreamIn(stream_in);

    return;
}

struct AudioHwCard *CreateUsbAudioHwCard()
{
    struct UsbAudioHwCard *pri_card;
    pri_card = (struct UsbAudioHwCard *)rtos_mem_zmalloc(sizeof(struct UsbAudioHwCard));
    if (!pri_card) {
        return NULL;
    }

    pri_card->card.SetParameters = UsbSetCardParameters;
    pri_card->card.GetParameters = UsbGetCardParameters;
    pri_card->card.SetCaptureVolume = UsbSetCaptureVolume;
    pri_card->card.GetCaptureVolume = UsbGetCaptureVolume;
    pri_card->card.SetRenderVolume = UsbSetRenderVolume;
    pri_card->card.GetRenderVolume = UsbGetRenderVolume;
    pri_card->card.SetRenderMute = UsbSetRenderMute;
    pri_card->card.GetRenderMute = UsbGetRenderMute;
    pri_card->card.SetCaptureMute = UsbSetCaptureMute;
    pri_card->card.GetCaptureMute = UsbGetCaptureMute;
    pri_card->card.GetInputBufferSize = UsbGetCardInputBufferSize;
    pri_card->card.CreateStreamOut = UsbCreateStreamOut;
    pri_card->card.DestroyStreamOut = UsbDestroyStreamOut;
    pri_card->card.CreateStreamIn = UsbCreateStreamIn;
    pri_card->card.DestroyStreamIn = UsbDestroyStreamIn;

    rtos_mutex_create(&pri_card->lock);

    return  &pri_card->card;

}

void DestroyUsbAudioHwCard(struct AudioHwCard *card)
{
    struct UsbAudioHwCard *pri_card = (struct UsbAudioHwCard *)(card);
    rtos_mutex_delete(pri_card->lock);

    if (card != NULL) {
        rtos_mem_free(card);
        //card = NULL;
    }
}
