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

#include "ameba_soc.h"
#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"

#include "hardware/audio/audio_hw_manager.h"

// ----------------------------------------------------------------------------
// AudioHwManager
extern void DestroyAudioHwCard(struct AudioHwCard *card);

static struct AudioHwPort gPrimaryAudioHwPort[2] = {
	//this could be also separated to different ports according to none or direct flags;
	//take out port for example: NONE port0, NOIRQ port1.
	{ 0, AUDIO_HW_PORT_ROLE_OUT, { AUDIO_HW_OUTPUT_FLAG_NONE | AUDIO_HW_OUTPUT_FLAG_NOIRQ }, AUDIO_HW_DEVICE_OUT_SPEAKER | AUDIO_HW_DEVICE_OUT_I2S, 2},
	{ 1, AUDIO_HW_PORT_ROLE_IN, { AUDIO_HW_INPUT_FLAG_NONE | AUDIO_HW_INPUT_FLAG_NOIRQ }, AUDIO_HW_DEVICE_IN_MIC | AUDIO_HW_DEVICE_IN_DMIC_REF_AMIC | AUDIO_HW_DEVICE_IN_I2S, 3}
};

static const enum AudioHwDevice gPrimaryAudioHwDevices =
	AUDIO_HW_DEVICE_OUT_SPEAKER | AUDIO_HW_DEVICE_OUT_I2S
	| AUDIO_HW_DEVICE_IN_MIC | AUDIO_HW_DEVICE_IN_DMIC_REF_AMIC
	| AUDIO_HW_DEVICE_IN_I2S;

static struct AudioHwCardDescriptor gAudioHwCardDescs[] = {
	{ AUDIO_HW_CARD_TYPE_PRIMARY, gPrimaryAudioHwPort, 2, gPrimaryAudioHwDevices, 5}
};

static const int32_t CARD_DESCRIPTER_COUNT =
	(sizeof(gAudioHwCardDescs) / sizeof(gAudioHwCardDescs[0]));

static int ClosePrimaryAudioHwCard(struct AudioHwCard *card)
{
	DestroyAudioHwCard(card);

	return 0;
}

static struct AudioHwCard *CreatePrimaryAudioHwCard(void)
{
	return CreateAudioHwCard();
}

static int32_t AmebaGetCardsCount(struct AudioHwManager *manager)
{
	(void) manager;
	return CARD_DESCRIPTER_COUNT;
}

static struct AudioHwCardDescriptor *AmebaGetCards(struct AudioHwManager *manager)
{
	(void) manager;
	return gAudioHwCardDescs;
}

static struct AudioHwCard *AmebaOpenCard(struct AudioHwManager *manager, const struct AudioHwCardDescriptor *desc)
{
	(void) manager;
	if (!desc) {
		HAL_AUDIO_ERROR("OpenCard: invalid card description!");
		return NULL;
	}

	if (desc->type == AUDIO_HW_CARD_TYPE_PRIMARY) {
		HAL_AUDIO_VERBOSE("OpenCard(type:%d) %s", desc->type, ret ? "failed" : "success");
		return CreatePrimaryAudioHwCard();
	} else {
		HAL_AUDIO_WARN("OpenCard: unsupported card(type:%d)", desc->type);
		return NULL;
	}
}

static void AmebaCloseCard(struct AudioHwManager *manager, struct AudioHwCard *card, const struct AudioHwCardDescriptor *desc)
{
	(void) manager;
	if (!desc) {
		HAL_AUDIO_ERROR("CloseCard: invalid card description!");
		return;
	}

	if (desc->type == AUDIO_HW_CARD_TYPE_PRIMARY) {
		ClosePrimaryAudioHwCard(card);
		HAL_AUDIO_VERBOSE("CloseCard(type:%d)", desc->type);
	}
}

struct AudioHwManager *CreateAudioHwManager(void)
{
	struct AudioHwManager *audio_manager;

	audio_manager = (struct AudioHwManager *)rtos_mem_zmalloc(sizeof(struct AudioHwManager));
	if (!audio_manager) {
		return NULL;
	}

	audio_manager->GetCardsCount = AmebaGetCardsCount;
	audio_manager->GetCards = AmebaGetCards;
	audio_manager->OpenCard = AmebaOpenCard;
	audio_manager->CloseCard = AmebaCloseCard;

	return audio_manager;
}

void DestoryAudioHwManager(struct AudioHwManager *manager)
{
	if (manager != NULL) {
		HAL_AUDIO_VERBOSE("free manager");
		rtos_mem_free(manager);
		//manager = NULL;
	}
}
