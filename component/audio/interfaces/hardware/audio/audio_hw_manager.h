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
 * @file audio_hw_manager.h
 *
 * @brief Provides APIs for audio card management.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_MANAGER_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_MANAGER_H

#include "hardware/audio/audio_hw_types.h"
#include "hardware/audio/audio_hw_card.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The management of audio cards.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioHwManager {

	/**
	 * @brief Get all the audio cards count supported.
	 *
	 * @param manager is the pointer of the audio manager.
	 * @return Returns all cards count.
	 */
	int32_t (*GetCardsCount)(struct AudioHwManager *manager);

	/**
	 * @brief Get all the audio cards supported.
	 *
	 * @param manager is the pointer of the audio manager.
	 * @return Returns all cards.
	 */
	struct AudioHwCardDescriptor *(*GetCards)(struct AudioHwManager *manager);

	/**
	 * @brief Create an audio card.
	 *
	 * @param manager is the pointer of the audio manager.
	 * @param desc is the pointer of the descriptor of the audio card.
	 * @return Returns pointer of the audio card.
	 */
	struct AudioHwCard *(*OpenCard)(struct AudioHwManager *manager,
									const struct AudioHwCardDescriptor *desc);

	/**
	 * @brief Destroy an audio card.
	 *
	 * @param manager is the pointer of the audio manager.
	 * @param card is the pointer of the audio card to be destroyed.
	 * @param desc is the pointer of the descriptor of the audio card.
	 */
	void (*CloseCard)(struct AudioHwManager *manager, struct AudioHwCard *card,
					  const struct AudioHwCardDescriptor *desc);
};

/**
 * @brief Create AudioHwManager.
 *
 * @return Returns the pointer of the AudioHwManager if successfully created,
 * otherwise returns NULL.
 */
struct AudioHwManager *CreateAudioHwManager(void);

/**
 * @brief Destroy AudioHwManager.
 */
void DestoryAudioHwManager(struct AudioHwManager *manager);

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_MANAGER_H
/** @} */
