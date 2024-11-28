/*
 * Copyright (c) 2024 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from PanKore
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
 * @file audio_manager.h
 *
 * @brief Provides APIs of the audio manager.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_MANAGER_H
#define AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_MANAGER_H

#include <stdint.h>

#include "audio/audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create RTAudioManager.
 */
struct RTAudioManager *RTAudioManager_GetInstance(void);

/**
 * @brief Release RTAudioManager.
 */
void RTAudioManager_Destroy(void);

/**
 * @brief create audio patch between devices or ports.
 * @only support amebadplus I2S IN->I2S OUT
 *
 * @param manager is the pointer of struct RTAudioManager.
 * @param num_sources is total number of sources.
 * @param sources are the sources of the patch.
 * @param num_sinks is total number of sinks.
 * @param sinks are the sinks of the patch.
 * @return Returns index of patch that's created.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioManager_CreateAudioPatch(struct RTAudioManager *manager,
                                        uint32_t num_sources, struct RTAudioPatchConfig *sources,
                                        uint32_t num_sinks, struct RTAudioPatchConfig *sinks);

/**
 * @brief release audio patch between devices or ports.
 *
 * @param manager is the pointer of struct RTAudioManager.
 * @param patch_index is the index of patch to be released.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_NO_INIT | the init is not done.
 * AUDIO_ERR_INVALID_OPERATION | the config value is not proper.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioManager_ReleaseAudioPatch(struct RTAudioManager *manager, int32_t patch_index);

#ifdef __cplusplus
}
#endif

#endif