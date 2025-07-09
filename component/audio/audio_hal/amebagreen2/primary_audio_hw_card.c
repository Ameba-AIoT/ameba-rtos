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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "os_wrapper.h"

#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"

#include "primary_audio_hw_card.h"

// ----------------------------------------------------------------------------
// AudioHwCard
extern struct AudioHwStreamOut *CreateAudioHwStreamOut(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
		const struct AudioHwConfig *config);
extern void DestroyAudioHwStreamOut(struct AudioHwStreamOut *stream_out);
extern struct AudioHwStreamIn *CreateAudioHwStreamIn(struct AudioHwCard *card, const struct AudioHwPathDescriptor *desc,
		const struct AudioHwConfig *config);
extern void DestroyAudioHwStreamIn(struct AudioHwStreamIn *stream_in);

static int32_t PrimarySetCardParameters(struct AudioHwCard *card, const char *strs)
{
	(void) card;
	(void) strs;
	return HAL_OSAL_ERR_INVALID_OPERATION;
}

static char *PrimaryGetCardParameters(const struct AudioHwCard *card,
									  const char *keys)
{
	(void) card;
	(void) keys;
	return (char *)strdup("");
}

static int32_t PrimarySetCaptureVolume(const struct AudioHwCard *card, float volume)
{
	(void) card;
	(void) volume;
	return HAL_OSAL_OK;
}

static int32_t PrimaryGetCaptureVolume(const struct AudioHwCard *card, float *volume)
{
	(void) card;
	(void) volume;
	return 0;
}

static int32_t PrimarySetRenderVolume(const struct AudioHwCard *card, float volume)
{
	(void) card;
	(void) volume;
	return HAL_OSAL_OK;
}

static int32_t PrimaryGetRenderVolume(const struct AudioHwCard *card, float *volume)
{
	(void) card;
	(void) volume;
	return 0;
}

static int32_t PrimarySetRenderMute(const struct AudioHwCard *card, bool muted)
{
	(void) card;
	(void) muted;
	return HAL_OSAL_OK;
}

static int32_t PrimaryGetRenderMute(const struct AudioHwCard *card, bool *muted)
{
	(void) card;
	(void) muted;
	return 0;
}

static int32_t PrimarySetCaptureMute(const struct AudioHwCard *card, bool muted)
{
	(void) card;
	(void) muted;
	return HAL_OSAL_OK;
}

static int32_t PrimaryGetCaptureMute(const struct AudioHwCard *card, bool *muted)
{
	(void) card;
	(void) muted;
	return 0;
}

static size_t PrimaryGetCardInputBufferSize(const struct AudioHwCard *card, const struct AudioHwConfig *config)
{
	(void) card;
	if (config == NULL) {
		HAL_AUDIO_ERROR("primaryGetCardInputBufferSize AudioHwConfig is null");
		return HAL_OSAL_ERR_INVALID_PARAM;
	}

	return GetHwInputBufferSize(config->sample_rate, config->format, config->channel_count);
}

static int32_t PrimaryCreateAudioPatch(struct AudioHwCard *card,
											int32_t num_sources, struct AudioHwPatchConfig *sources,
											int32_t num_sinks, struct AudioHwPatchConfig *sinks)
{
	(void) card;
	(void) num_sources;
	(void) sources;
	(void) num_sinks;
	(void) sinks;
	return 0;
}

static int32_t PrimaryReleaseAudioPatch(struct AudioHwCard *card, int32_t patch_index)
{
	(void) card;
	(void) patch_index;
	return 0;
}

static struct AudioHwStreamOut *PrimaryCreateStreamOut(
	struct AudioHwCard *card,
	const struct AudioHwPathDescriptor *desc,
	const struct AudioHwConfig *config)
{
	return CreateAudioHwStreamOut(card, desc, config);
}

static void PrimaryDestroyStreamOut(struct AudioHwCard *card, struct AudioHwStreamOut *stream_out)
{
	(void) card;
	DestroyAudioHwStreamOut(stream_out);

}


static struct AudioHwStreamIn *PrimaryCreateStreamIn(
	struct AudioHwCard *card,
	const struct AudioHwPathDescriptor *desc,
	const struct AudioHwConfig *config)
{
	return CreateAudioHwStreamIn(card, desc, config);
}

static void PrimaryDestroyStreamIn(struct AudioHwCard *card, struct AudioHwStreamIn *stream_in)
{
	(void) card;
	DestroyAudioHwStreamIn(stream_in);

	return;
}

struct AudioHwCard *CreateAudioHwCard()
{
	struct PrimaryAudioHwCard *pri_card;
	pri_card = (struct PrimaryAudioHwCard *)rtos_mem_zmalloc(sizeof(struct PrimaryAudioHwCard));
	if (!pri_card) {
		return NULL;
	}

	pri_card->card.SetParameters = PrimarySetCardParameters;
	pri_card->card.GetParameters = PrimaryGetCardParameters;
	pri_card->card.SetCaptureVolume = PrimarySetCaptureVolume;
	pri_card->card.GetCaptureVolume = PrimaryGetCaptureVolume;
	pri_card->card.SetRenderVolume = PrimarySetRenderVolume;
	pri_card->card.GetRenderVolume = PrimaryGetRenderVolume;
	pri_card->card.SetRenderMute = PrimarySetRenderMute;
	pri_card->card.GetRenderMute = PrimaryGetRenderMute;
	pri_card->card.SetCaptureMute = PrimarySetCaptureMute;
	pri_card->card.GetCaptureMute = PrimaryGetCaptureMute;
	pri_card->card.GetInputBufferSize = PrimaryGetCardInputBufferSize;
	pri_card->card.CreateAudioPatch = PrimaryCreateAudioPatch;
	pri_card->card.ReleaseAudioPatch = PrimaryReleaseAudioPatch;
	pri_card->card.CreateStreamOut = PrimaryCreateStreamOut;
	pri_card->card.DestroyStreamOut = PrimaryDestroyStreamOut;
	pri_card->card.CreateStreamIn = PrimaryCreateStreamIn;
	pri_card->card.DestroyStreamIn = PrimaryDestroyStreamIn;

	rtos_mutex_create(&pri_card->lock);

	return  &pri_card->card;

}

void DestroyAudioHwCard(struct AudioHwCard *card)
{
	struct PrimaryAudioHwCard *pri_card = (struct PrimaryAudioHwCard *)(card);
	rtos_mutex_delete(pri_card->lock);

	if (card != NULL) {
		rtos_mem_free(card);
		//card = NULL;
	}
}
