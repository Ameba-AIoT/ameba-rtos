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

#include "hardware/audio/audio_hw_types.h"

#include "ameba_audio_policy_configs.h"

void
GetAudioConfigHwModules(int32_t *hw_modules_num, AudioConfigHwModule **modules) {
    static struct AudioConfigHwModule hw_modules[] = {
        {
            .name = "primary",
            .audio_global_config = {
                    .attached_output_devices = AUDIO_HW_DEVICE_OUT_SPEAKER | AUDIO_HW_DEVICE_OUT_I2S,
                    .default_output_device = AUDIO_HW_DEVICE_OUT_SPEAKER
                },
            .device_ports = {
                {
                    .name = "Speaker",
                    .type = AUDIO_HW_DEVICE_OUT_SPEAKER,
                    .supported_rates = {16000, 44100, 48000, 96000, 192000},
                    .supported_rates_num = 5,
                    .supported_channels = {1, 2, 4, 6, 8},
                    .supported_channels_num = 5,
                    .supported_formats = {AUDIO_HW_FORMAT_PCM_16_BIT, AUDIO_HW_FORMAT_PCM_32_BIT,
                                          AUDIO_HW_FORMAT_PCM_8_24_BIT},
                    .supported_formats_num = 3,
                },
                {
                    .name = "I2SOut",
                    .type = AUDIO_HW_DEVICE_OUT_I2S,
                    .supported_rates = {16000, 44100, 48000, 96000, 192000},
                    .supported_rates_num = 5,
                    .supported_channels = {1, 2, 4, 6, 8},
                    .supported_channels_num = 5,
                    .supported_formats = {AUDIO_HW_FORMAT_PCM_16_BIT, AUDIO_HW_FORMAT_PCM_32_BIT,
                                          AUDIO_HW_FORMAT_PCM_8_24_BIT},
                    .supported_formats_num = 3,
                },
            },
            .device_ports_num = 2,
            .mix_ports = {
                {
                    .name = "primary output",
                    .rate = 44100,
                    .channels = 2,
                    .format = AUDIO_HW_FORMAT_PCM_16_BIT,
                    .flags = AUDIO_HW_OUTPUT_FLAG_NONE,
                }
            },
            .mix_ports_num = 1,
            .audio_routes = {
                {"primary output", "Speaker"},
                {"primary output", "I2SOut"},
            },
            .audio_routes_num = 2,
        },
        {
            .name = "a2dp",
            .device_ports = {
                {
                    .name = "A2dp Out Device",
                    .type = AUDIO_HW_DEVICE_OUT_A2DP,
                    .supported_rates = {44100},
                    .supported_rates_num = 1,
                    .supported_channels = {2},
                    .supported_channels_num = 1,
                    .supported_formats = {AUDIO_HW_FORMAT_PCM_16_BIT},
                    .supported_formats_num = 1,
                }
            },
            .device_ports_num = 1,
            .mix_ports = {
                {
                    .name = "a2dp output",
                    .rate = 44100,
                    .channels = 2,
                    .format = AUDIO_HW_FORMAT_PCM_16_BIT,
                    .flags = AUDIO_HW_OUTPUT_FLAG_NONE,
                }
            },
            .mix_ports_num = 1,
            .audio_routes = {
                {"a2dp output", "A2dp Out Device"},
            },
            .audio_routes_num = 1,
        }
    };

    *hw_modules_num = 2;
    *modules = hw_modules;
}