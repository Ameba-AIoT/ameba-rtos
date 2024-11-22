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

#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBALITE_AUDIO_HW_MIX_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBALITE_AUDIO_HW_MIX_H

#include <stddef.h>
#include <stdint.h>

void ProcessMix(const void *buffer, int16_t *raw_out, size_t frame_count);

#endif