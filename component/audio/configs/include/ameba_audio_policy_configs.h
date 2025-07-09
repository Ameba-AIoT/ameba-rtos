/*
 * Copyright (c) 2023 Realtek, LLC.
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

#ifndef AMEBA_COMPONENT_AUDIO_POLICY_CONFIGS_AUDIO_POLICY_CONFIGS_H
#define AMEBA_COMPONENT_AUDIO_POLICY_CONFIGS_AUDIO_POLICY_CONFIGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RATES_NUM        10
#define MAX_CHANNEL_NUM      10
#define MAX_FORMATS_NUM      10
#define MAX_DEVICE_PORTS_NUM 10
#define MAX_MIX_PORTS_NUM    10
#define MAX_ROUTES_NUM       10
#define MAX_HW_MODULES_NUM   10

typedef struct AudioConfigGlobalDevices {
    int32_t attached_output_devices;
    int32_t default_output_device;
} AudioConfigGlobalDevices;

typedef struct AudioConfigDevicePort {
    char *name;
    int32_t type;
    int32_t supported_rates[MAX_RATES_NUM];
    int32_t supported_rates_num;
    int32_t supported_channels[MAX_CHANNEL_NUM];
    int32_t supported_channels_num;
    int32_t supported_formats[MAX_FORMATS_NUM];
    int32_t supported_formats_num;
} AudioConfigDevicePort;

typedef struct AudioConfigMixPort {
    char *name;
    int32_t rate;
    int32_t channels;
    int32_t format;
    int32_t flags;
} AudioConfigMixPort;

typedef struct AudioConfigRoute {
    char *source_name;
    char *sink_name;
} AudioConfigRoute;

typedef struct AudioConfigHwModule {
    char *name;
    AudioConfigGlobalDevices audio_global_config;
    AudioConfigDevicePort device_ports[MAX_DEVICE_PORTS_NUM];
    int32_t device_ports_num;
    AudioConfigMixPort mix_ports[MAX_MIX_PORTS_NUM];
    int32_t mix_ports_num;
    AudioConfigRoute audio_routes[MAX_ROUTES_NUM];
    int32_t audio_routes_num;
} AudioConfigHwModule;

void GetAudioConfigHwModules(int32_t *hw_modules_num, AudioConfigHwModule **modules);

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_COMPONENT_AUDIO_POLICY_CONFIGS_AUDIO_POLICY_CONFIGS_H