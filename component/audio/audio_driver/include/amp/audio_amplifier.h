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

#ifndef AMEBA_AUDIO_DRIVER_INCLUDE_AUDIO_AMPLIFIER_H
#define AMEBA_AUDIO_DRIVER_INCLUDE_AUDIO_AMPLIFIER_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AmpType {
    AMP_DUMMY,
    /** ht513 amplifer */
    AMP_HT513,
    /** add more here, exp: ALC5616 */
} AmpType;

typedef enum AmpControlMode {
    /** ~sd high to enable amplifier, ~sd low to disable amplifier */
    AMP_CTRL_GPIO,
    /** using i2c flow to enable or disable amplifer */
    AMP_CTRL_I2C,
} AmpControlMode;

typedef enum AmpClassType {
    /** class D */
    AMP_CLASS_TYPE_D,
    /** class AB */
    AMP_CLASS_TYPE_AB,
} AmpClassType;

typedef enum AmpChannelIndex {
    AMP_CHANNEL_INDEX_0 = 0x01,
    AMP_CHANNEL_INDEX_1 = 0x02,
    AMP_CHANNEL_INDEX_2 = 0x04,
    AMP_CHANNEL_INDEX_3 = 0x08,
    AMP_CHANNEL_INDEX_4 = 0x10,
    AMP_CHANNEL_INDEX_5 = 0x20,
    AMP_CHANNEL_INDEX_6 = 0x40,
    AMP_CHANNEL_INDEX_7 = 0x80,
} AmpChannelIndex;

typedef struct AmpPinConfig {
    /** pinmux, for exp: PB_31 */
    int32_t pinmux;
    /** stable enable time of this amplifer */
    int32_t enable_time;
    /** stable disable time of this amplifier */
    int32_t disable_time;
} AmpPinConfig;

typedef struct AmpI2cConfig {
    /** pinmux for i2c sda */
    int32_t sda_pinmux;
    /** pinmux for i2c scl */
    int32_t scl_pinmux;
    /** i2c bus clk frequency */
    int32_t bus_clk;
    /** i2c port */
    int32_t port;
} AmpI2cConfig;

typedef struct AudioAmplifier AudioAmplifier;
struct AudioAmplifier {
    int32_t (*SetSDPinmux)(AudioAmplifier *amp, AmpPinConfig *pin_config);
    int32_t (*SetUpI2c)(AudioAmplifier *amp, AmpI2cConfig *i2c_config);
    int32_t (*SetClassType)(AudioAmplifier *amp, AmpClassType type);
    int32_t (*SetEnable)(AudioAmplifier *amp, AmpControlMode type, bool enable);
    int32_t (*SetMute)(AudioAmplifier *amp, bool mute);
    int32_t (*SetVolume)(AudioAmplifier *amp, AmpChannelIndex channel, float vol);
    int32_t (*SetFade)(AudioAmplifier *amp, bool enable);
    int32_t (*SetChannelMask)(AudioAmplifier *amp, int8_t channel_mask);
    void    (*Destroy)(AudioAmplifier *amp);
};

AudioAmplifier *CreateAudioAmplifier(AmpType type);
void DestoryAudioAmplifier(AudioAmplifier *amp);

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_AUDIO_DRIVER_INCLUDE_AUDIO_AMPLIFIER_H
