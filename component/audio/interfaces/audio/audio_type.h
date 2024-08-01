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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Declares APIs for audio framework.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_type.h
 *
 * @brief Provides definition of the audio stream types and formats.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_FWK_MEDIA_AUDIO_INTERFACES_AUDIO_AUDIO_TYPE_H
#define AMEBA_FWK_MEDIA_AUDIO_INTERFACES_AUDIO_AUDIO_TYPE_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines all the audio playback usages.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** category type of track, for media */
	RTAUDIO_CATEGORY_MEDIA         = 0,
	/** category type of track, for calls*/
	RTAUDIO_CATEGORY_COMMUNICATION = 1,
	/** category type of track, for voice recognition */
	RTAUDIO_CATEGORY_SPEECH        = 2,
	/** category type of track, for beep sound, some as key tone */
	RTAUDIO_CATEGORY_BEEP          = 3,
	/** total category type number of track */
	RTAUDIO_CATEGORY_MAX_NUM       = 4,
};

/**
 * @brief Defines all the audio formats.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** invalid audio track and audio record data bit format*/
	RTAUDIO_FORMAT_INVALID           = 0xFFFFFFFFu,
	/** audio track and audio record data bit format, 8bit per channel per frame*/
	RTAUDIO_FORMAT_PCM_8_BIT         = 0x01u,
	/** audio track and audio record data bit format, 16bit per channel per frame*/
	RTAUDIO_FORMAT_PCM_16_BIT        = 0x02u,
	/** audio track and audio record data bit format, 32bit per channel per frame*/
	RTAUDIO_FORMAT_PCM_32_BIT        = 0x04u,
	/** audio track and audio record data bit format, float per channel per frame*/
	RTAUDIO_FORMAT_PCM_FLOAT         = 0x08u,
	/** audio track and audio record data bit format, 24bit packed per channel per frame*/
	RTAUDIO_FORMAT_PCM_24_BIT        = 0x10u,
	/** audio record and audio record data bit format, 24+8bit per channel per frame*/
	RTAUDIO_FORMAT_PCM_8_24_BIT      = 0x20u,
};

enum {
	/** invalid device */
	RTDEVICE_NONE             = 0x0u,
	/** input device analog microphone */
	RTDEVICE_IN_MIC           = 0x8000001u,
	/** input device digital microphone */
	RTDEVICE_IN_DMIC_REF_AMIC = 0x8000002u,
	/** input device i2s */
	RTDEVICE_IN_I2S           = 0x8000004u,
};

/**
 * @brief Defines all the audio output flags.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** output flag none */
	RTAUDIO_OUTPUT_FLAG_NONE         = 0x0u,
	/** output flag no dma irq */
	RTAUDIO_OUTPUT_FLAG_NOIRQ        = 0x1u,
};

/**
 * @brief Defines all the audio input flags.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** output flag none */
	RTAUDIO_INPUT_FLAG_NONE         = 0x0u,
	/** output flag no dma irq */
	RTAUDIO_INPUT_FLAG_NOIRQ        = 0x1u,
};

/**
 * @brief Defines all the audio effect types.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** invalid audio effect */
	RTAUDIO_EFFECT_INVALID           = 0xFFFFFFFFu,
	/** audio equalizer of audio effect */
	RTAUDIO_EFFECT_EQUALIZER         = 0x1u,
};

/**
 * @brief Defines all the audio effect param types.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** audio effect total bands */
	RTAUDIO_EFFECT_PARAM_NUM_BANDS       = 0x0u,
	/** audio effect level range */
	RTAUDIO_EFFECT_PARAM_LEVEL_RANGE     = 0x1u,
	/** audio effect band level */
	RTAUDIO_EFFECT_PARAM_BAND_LEVEL      = 0x2u,
	/** audio effect center frequency */
	RTAUDIO_EFFECT_PARAM_CENTER_FREQ     = 0x3u,
	/** audio effect band frequency range */
	RTAUDIO_EFFECT_PARAM_BAND_FREQ_RANGE = 0x4u,
	/** audio effect get band */
	RTAUDIO_EFFECT_PARAM_GET_BAND        = 0x5u,
	/** audio effect qfactor */
	RTAUDIO_EFFECT_PARAM_QFACTOR         = 0x6u,
};

/**
 * @brief Defines all the audio min frames stages, only for mixer to use.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** allows more data for service to write to HAL one time, default uing it */
	RTAUDIO_OUT_MIN_FRAMES_STAGE1  = 0,
	/** allows less data for service to write to HAL one time */
	RTAUDIO_OUT_MIN_FRAMES_STAGE2  = 1,
};

static inline size_t RTAudio_GetAudioBytesPerSample(int32_t format)
{
	size_t size = 0;

	switch (format) {
	case RTAUDIO_FORMAT_PCM_8_BIT:
		size = sizeof(uint8_t);
		break;
	case RTAUDIO_FORMAT_PCM_16_BIT:
		size = sizeof(int16_t);
		break;
	case RTAUDIO_FORMAT_PCM_24_BIT:
		size = sizeof(uint8_t) * 3;
		break;
	case RTAUDIO_FORMAT_PCM_8_24_BIT:
	case RTAUDIO_FORMAT_PCM_32_BIT:
		size = sizeof(int32_t);
		break;
	default:
		break;
	}
	return size;
}

#ifdef __cplusplus
}
#endif


#endif // AMEBA_FWK_MEDIA_AUDIO_INTERFACES_AUDIO_AUDIO_TYPE_H
/** @} */
