/*
 * Copyright (c) 2021 Realtek, LLC.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup HAL
 * @{
 *
 * @brief Declares the structures and interfaces for the Hardware Abstraction Layer (HAL) module.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hw_types.h
 *
 * @brief Provides definitions of structures and enums for audio types.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_TYPES_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the audio hw format.
 */
enum AudioHwFormat {
	/** audio data bit format not supported */
	AUDIO_HW_FORMAT_INVALID             = 0xFFFFFFFFu,
	/** audio data bit format is pcm */
	AUDIO_HW_FORMAT_PCM                 = 0x00000000u,
	/** mask to check if format if pcm or other categories */
	AUDIO_HW_FORMAT_MAIN_MASK           = 0xFF000000u,

	/** audio data bit format, 8bit per channel per frame */
	AUDIO_HW_FORMAT_PCM_8_BIT           = 0x01u,
	/** audio data bit format, 16bit per channel per frame */
	AUDIO_HW_FORMAT_PCM_16_BIT          = 0x02u,
	/** audio data bit format, 32bit per channel per frame */
	AUDIO_HW_FORMAT_PCM_32_BIT          = 0x04u,
	/** audio data bit format, float per channel per frame */
	AUDIO_HW_FORMAT_PCM_FLOAT           = 0x08u,
	/** audio data bit format, 24bit zero per channel per frame */
	AUDIO_HW_FORMAT_PCM_24_BIT          = 0x10u,
	/** audio data bit format, 24bit+8bit zero per channel per frame */
	AUDIO_HW_FORMAT_PCM_8_24_BIT        = 0x20u,

};

/**
 * @brief Defines the audio hw card type.
 */
enum AudioHwCardType {
	/** audio card type primary*/
	AUDIO_HW_CARD_TYPE_PRIMARY = 0x0u,
};

/**
 * @brief Defines the audio port role.
 */
enum AudioHwPortRole {
	/** audio port's role is output */
	AUDIO_HW_PORT_ROLE_OUT    = 0x1u,
	/** audio port's role is input */
	AUDIO_HW_PORT_ROLE_IN     = 0x2u,
};

/**
 * @brief Defines the audio devices.
 */
enum AudioHwDevice {
	/** audio device none */
	AUDIO_HW_DEVICE_NONE             = 0x0u,
	/** audio device speaker */
	AUDIO_HW_DEVICE_OUT_SPEAKER      = 0x1u,
	/** audio device i2s out */
	AUDIO_HW_DEVICE_OUT_I2S          = 0x2u,
	/** audio device microphone */
	AUDIO_HW_DEVICE_IN_MIC           = 0x8000001u,
	/** audio device digital microphone */
	AUDIO_HW_DEVICE_IN_DMIC_REF_AMIC = 0x8000002u,
	/** audio device i2s in */
	AUDIO_HW_DEVICE_IN_I2S           = 0x8000004u,
};

/**
 * @brief Defines the output flags of an audio stream out.
 */
enum AudioHwOutputFlag {
	/** audio output flag none */
	AUDIO_HW_OUTPUT_FLAG_NONE         = 0x0u,
	/** audio output for no irq of dma */
	AUDIO_HW_OUTPUT_FLAG_NOIRQ        = 0x1u,
};

/**
 * @brief Defines the input flags of an audio stream in.
 */
enum AudioHwInputFlag {
	/** audio input flag none */
	AUDIO_HW_INPUT_FLAG_NONE         = 0x0u,
	/** audio input for no irq of dma */
	AUDIO_HW_INPUT_FLAG_NOIRQ        = 0x1u,
};

/**
 * @brief Defines the audio card information.
 */
struct AudioHwCardDescriptor {
	/** audio card type, for example, primary */
	enum AudioHwCardType type;
	/** supported ports by this card */
	struct AudioHwPort *ports;
	/** supported port count by this card */
	uint32_t port_num;
	/** supported devices by this card */
	enum AudioHwDevice devices;
	/** supported device count by this card */
	uint32_t device_num;
};

/**
 * @brief Defines the audio port information.
 */
struct AudioHwPort {
	/** port index */
	uint32_t port_index;
	/** port role */
	enum AudioHwPortRole role;
	/** port flags */
	union flag {
		enum AudioHwOutputFlag out_flags;
		enum AudioHwInputFlag in_flags;
	} flags;
	/** supported devices by this port */
	enum AudioHwDevice supported_devices;
	/** supported device count by this port */
	uint32_t devices_num;
};

/**
 * @brief Defines the audio patch information.
 */
struct AudioHwPathDescriptor {
	/** port index of the audio path*/
	uint32_t port_index;
	/** flags of the audio path*/
	uint32_t flags;
	/** devices of the audio path*/
	enum AudioHwDevice devices;
};

/**
 * @brief Defines audio stream configs.
 */
struct AudioHwConfig {
	/** rate of the sample */
	uint32_t sample_rate;
	/** channel count of the sample */
	uint32_t channel_count;
	/** format of the sample */
	enum AudioHwFormat format;
	/** dma buffer bytes one time */
	uint32_t buffer_bytes;
};

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_TYPES_H
/** @} */
