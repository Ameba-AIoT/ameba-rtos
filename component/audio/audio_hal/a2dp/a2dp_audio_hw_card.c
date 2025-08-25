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

#include "a2dp_audio_hw_card.h"

// ----------------------------------------------------------------------------
// AudioHwCard
extern struct AudioHwStreamOut *CreateA2dpAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
        const struct AudioHwConfig *config);
extern void DestroyA2dpAudioHwStreamOut(struct AudioHwStreamOut *stream_out);
extern struct AudioHwStreamIn *CreateAudioHwStreamIn(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
        const struct AudioHwConfig *config);
extern void DestroyAudioHwStreamIn(struct AudioHwStreamIn *stream_in);

static int32_t A2dpSetCardParameters(struct AudioHwCard *card, const char *strs)
{
    (void) card;
    (void) strs;
    return HAL_OSAL_ERR_INVALID_OPERATION;
}

static char *A2dpGetCardParameters(const struct AudioHwCard *card,
                                      const char *keys)
{
    (void) card;
    (void) keys;
    return (char *)strdup("");
}

static int32_t A2dpSetCaptureVolume(const struct AudioHwCard *card, float volume)
{
    (void) card;
    (void) volume;
    return HAL_OSAL_OK;
}

static int32_t A2dpGetCaptureVolume(const struct AudioHwCard *card, float *volume)
{
    (void) card;
    (void) volume;
    return 0;
}

static int32_t A2dpSetRenderVolume(const struct AudioHwCard *card, float volume)
{
    (void) card;
    (void) volume;
    return HAL_OSAL_OK;
}

static int32_t A2dpGetRenderVolume(const struct AudioHwCard *card, float *volume)
{
    (void) card;
    (void) volume;
    return 0;
}

static int32_t A2dpSetRenderMute(const struct AudioHwCard *card, bool muted)
{
    (void) card;
    (void) muted;
    return HAL_OSAL_OK;
}

static int32_t A2dpGetRenderMute(const struct AudioHwCard *card, bool *muted)
{
    (void) card;
    (void) muted;
    return 0;
}

static int32_t A2dpSetCaptureMute(const struct AudioHwCard *card, bool muted)
{
    (void) card;
    (void) muted;
    return HAL_OSAL_OK;
}

static int32_t A2dpGetCaptureMute(const struct AudioHwCard *card, bool *muted)
{
    (void) card;
    (void) muted;
    return 0;
}

static size_t A2dpGetCardInputBufferSize(const struct AudioHwCard *card, const struct AudioHwConfig *config)
{
    (void) card;
    if (config == NULL) {
        HAL_AUDIO_ERROR("primaryGetCardInputBufferSize AudioHwConfig is null");
        return HAL_OSAL_ERR_INVALID_PARAM;
    }

    return GetHwInputBufferSize(config->sample_rate, config->format, config->channel_count);
}

static struct AudioHwStreamOut *A2dpCreateStreamOut(
    struct AudioHwCard *card,
    const struct AudioHwPathDescriptor *desc,
    const struct AudioHwConfig *config)
{
    return CreateA2dpAudioHwStreamOut(card, desc, config);
}

static void A2dpDestroyStreamOut(struct AudioHwCard *card, struct AudioHwStreamOut *stream_out)
{
    (void) card;
    DestroyA2dpAudioHwStreamOut(stream_out);

}


static struct AudioHwStreamIn *A2dpCreateStreamIn(
    struct AudioHwCard *card,
    const struct AudioHwPathDescriptor *desc,
    const struct AudioHwConfig *config)
{
    return CreateAudioHwStreamIn(card, desc, config);
}

static void A2dpDestroyStreamIn(struct AudioHwCard *card, struct AudioHwStreamIn *stream_in)
{
    (void) card;
    DestroyAudioHwStreamIn(stream_in);

    return;
}

struct AudioHwCard *CreateA2dpAudioHwCard()
{
    struct A2dpAudioHwCard *pri_card;
    pri_card = (struct A2dpAudioHwCard *)rtos_mem_zmalloc(sizeof(struct A2dpAudioHwCard));
    if (!pri_card) {
        return NULL;
    }

    pri_card->card.SetParameters = A2dpSetCardParameters;
    pri_card->card.GetParameters = A2dpGetCardParameters;
    pri_card->card.SetCaptureVolume = A2dpSetCaptureVolume;
    pri_card->card.GetCaptureVolume = A2dpGetCaptureVolume;
    pri_card->card.SetRenderVolume = A2dpSetRenderVolume;
    pri_card->card.GetRenderVolume = A2dpGetRenderVolume;
    pri_card->card.SetRenderMute = A2dpSetRenderMute;
    pri_card->card.GetRenderMute = A2dpGetRenderMute;
    pri_card->card.SetCaptureMute = A2dpSetCaptureMute;
    pri_card->card.GetCaptureMute = A2dpGetCaptureMute;
    pri_card->card.GetInputBufferSize = A2dpGetCardInputBufferSize;
    pri_card->card.CreateStreamOut = A2dpCreateStreamOut;
    pri_card->card.DestroyStreamOut = A2dpDestroyStreamOut;
    pri_card->card.CreateStreamIn = A2dpCreateStreamIn;
    pri_card->card.DestroyStreamIn = A2dpDestroyStreamIn;

    rtos_mutex_create(&pri_card->lock);

    return  &pri_card->card;

}

void DestroyA2dpAudioHwCard(struct AudioHwCard *card)
{
    struct A2dpAudioHwCard *pri_card = (struct A2dpAudioHwCard *)(card);
    rtos_mutex_delete(pri_card->lock);

    if (card != NULL) {
        rtos_mem_free(card);
        //card = NULL;
    }
}
