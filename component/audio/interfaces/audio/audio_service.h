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
 * @file audio_service.h
 *
 * @brief Provides APIs of the audio service.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_SERVICE_H
#define AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_SERVICE_H

#include <stdint.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Init RTAudioService.
*/
void RTAudioService_Init(void);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_SERVICE_H