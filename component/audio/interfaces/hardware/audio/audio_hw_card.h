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
 * @file audio_hw_card.h
 *
 * @brief Provides APIs for operations related to the audio card.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_CARD_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_CARD_H


#include "hardware/audio/audio_hw_stream_out.h"
#include "hardware/audio/audio_hw_stream_in.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides audio card interfaces.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioHwCard {
	/**
	 * @brief Set parameters to AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param strs is the parameters to set.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetParameters)(struct AudioHwCard *card, const char *strs);

	/**
	 * @brief Get parameters of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param keys is the key value of parameters to get.
	 * @return Returns value of keys.
	 */
	char *(*GetParameters)(const struct AudioHwCard *card,
						   const char *keys);

	/**
	 * @brief Set capture volume to AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param volume is the capture volume set.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetCaptureVolume)(const struct AudioHwCard *card, float volume);

	/**
	 * @brief Get capture volume of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param volume is the capture volume to get.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*GetCaptureVolume)(const struct AudioHwCard *card, float *volume);

	/**
	 * @brief Set playback volume of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param volume is the playback volume to set.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetRenderVolume)(const struct AudioHwCard *card, float volume);

	/**
	 * @brief Get playback volume of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param volume is the volume to get.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*GetRenderVolume)(const struct AudioHwCard *card, float *volume);

	/**
	 * @brief Set mute playback status of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param muted is the muted status to set to hardware.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetRenderMute)(const struct AudioHwCard *card, bool muted);

	/**
	 * @brief Get current playback mute status of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param muted is the current muted status of the hardware.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*GetRenderMute)(const struct AudioHwCard *card, bool *muted);

	/**
	 * @brief Set capture mute status to AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param muted is the muted status to set to hardware.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetCaptureMute)(const struct AudioHwCard *card, bool muted);

	/**
	 * @brief Get current capture mute status of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param muted is the current muted status of the hardware.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*GetCaptureMute)(const struct AudioHwCard *card, bool *muted);

	/**
	 * @brief Get input buffer size of AudioHwcard.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param config is streaming attributes, such as rate, channels, and so on.
	 * @return Returns input buffer size in bytes;
	 */
	size_t (*GetInputBufferSize)(const struct AudioHwCard *card,
								 const struct AudioHwConfig *config);

	/**
	 * @brief Creates AudioHwStreamOut.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param desc is the descriptor of the streaming path(port and devices).
	 * @param config is the pointer of the audio streaming configs.
	 * @return Returns pointer of AudioHwStreamOut.
	 */
	struct AudioHwStreamOut *(*CreateStreamOut)(struct AudioHwCard *card,
			const struct AudioHwPathDescriptor *desc,
			const struct AudioHwConfig *config);

	/**
	 * @brief Destroys AudioHwStreamOut.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param stream_out is the pointer of the AudioHwStreamOut.
	 */
	void (*DestroyStreamOut)(struct AudioHwCard *card,
							 struct AudioHwStreamOut *stream_out);

	/**
	 * @brief Creates AudioHwStreamIn.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param desc is the descriptor of the streaming path(port and devices).
	 * @param config is the pointer of the audio streaming attributes.
	 * @return Returns pointer of AudioHwStreamIn.
	 */
	struct AudioHwStreamIn *(*CreateStreamIn)(struct AudioHwCard *card,
			const struct AudioHwPathDescriptor *desc,
			const struct AudioHwConfig *config);

	/**
	 * @brief Destroys AudioHwStreamIn.
	 *
	 * @param card is the pointer of the struct AudioHwcard.
	 * @param stream_in is the pointer of the AudioHwStreamIn.
	 */
	void (*DestroyStreamIn)(struct AudioHwCard *card,
							struct AudioHwStreamIn *stream_in);
};

/**
 * @brief Creates AudioHwCard.
 *
 * @return Returns pointer of the AudioHwCard created.
 */
struct AudioHwCard *CreateAudioHwCard(void);

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_CARD_H
/** @} */
