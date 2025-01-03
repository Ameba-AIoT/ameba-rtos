/*
 * Copyright (c) 2022 Realtek, LLC.
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

#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBADPLUS_AMEBA_AUDIO_STREAM_AUDIO_PATCH_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBADPLUS_AMEBA_AUDIO_STREAM_AUDIO_PATCH_H

#include "basic_types.h"

#include "ameba_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t ameba_audio_stream_create_audio_patch(int32_t num_sources, struct AmebaAudioPatchConfig *sources,
                                              int32_t num_sinks, struct AmebaAudioPatchConfig *sinks);
int32_t ameba_audio_stream_release_audio_patch(int32_t patch_index);

#ifdef __cplusplus
}
#endif

#endif
