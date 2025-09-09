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

#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBASMART_USB_AUDIO_HW_CARD_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBASMART_USB_AUDIO_HW_CARD_H

#include <stdio.h>

#include "hardware/audio/audio_hw_card.h"

#ifdef __cplusplus
extern "C" {
#endif

// UsbAudioHwCard
struct UsbAudioHwCard {
    struct AudioHwCard card;

    rtos_mutex_t lock;
    struct UsbAudioHwStreamOut *output;
    struct UsbAudioHwStreamIn *input;
};

#ifdef __cplusplus
}
#endif
#endif //AMEBA_AUDIO_AUDIO_HAL_AMEBASMART_USB_AUDIO_HW_CARD_H