/*
 * Copyright (c) 2025 Realtek, LLC.
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

#include "ameba.h"
#include "os_wrapper.h"

#include "amp_dummy.h"
#include "ht513.h"

#include "amp/audio_amplifier.h"

#define TAG "AudioAmplifier"

AudioAmplifier *CreateAudioAmplifier(AmpType type) {
    AudioAmplifier *amp;

    switch (type)
    {
        case AMP_HT513:
            amp = HT513_Create();
            break;
        case AMP_DUMMY:
            amp = AmpDummy_Create();
            break;
        default:
            RTK_LOGE(TAG, "ERROR: Unkonw amp type %d\n", type);
            amp = NULL;
            break;
    }

    return amp;
}

void DestoryAudioAmplifier(AudioAmplifier *amp) {
    if (amp) {
        amp->Destroy(amp);
    }
}
