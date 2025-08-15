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

#include <cstddef>

#include "include/ameba_media_audio_output_usrcfg.h"

// ----------------------------------------------------------------------
//MediaAudioOutputConfig
#if defined MEDIA_PLAYER
extern void *CreateBTOutput();
extern void *CreateUACOutput();

MediaAudioOutputConfig kMediaAudioOutputConfigs[] = {
#if defined(MEDIA_SINK_BT)
    { "a2dp", CreateBTOutput },
#endif
#if defined(MEDIA_SINK_UAC)
    { "uac", CreateUACOutput },
#endif
};

size_t kNumMediaAudioOutputConfigs =
    sizeof(kMediaAudioOutputConfigs) / sizeof(kMediaAudioOutputConfigs[0]);
#endif