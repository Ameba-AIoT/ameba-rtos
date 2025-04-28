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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "ameba.h"
#include "os_wrapper.h"

#include "amp_dummy.h"

#define TAG "AmpDummy"

typedef struct AmpDummy {
    AudioAmplifier ops;
    AmpPinConfig sd_pin_config;
} AmpDummy;

static int32_t
AmpDummy_SetSDPinmux(AudioAmplifier *amp, AmpPinConfig *pin_config) {
    AmpDummy *dummy = (AmpDummy *)amp;

    if (!pin_config) {
        RTK_LOGE(TAG, "ERROR: pin_config null\n");
        return -EINVAL;
    }

    dummy->sd_pin_config = *pin_config;

    return 0;
}

static int32_t
AmpDummy_SetUpI2c(AudioAmplifier *amp, AmpI2cConfig *i2c_config) {
    (void) amp;
    (void) i2c_config;
    return 0;
}

static int32_t
AmpDummy_SetClassType(AudioAmplifier *amp, AmpClassType type) {
    (void) amp;
    (void) type;
    return 0;
}

static int32_t
AmpDummy_SetEnable(AudioAmplifier *amp, AmpControlMode type, bool enabled) {
    AmpDummy *dummy = (AmpDummy *)amp;

    if (dummy->sd_pin_config.pinmux == -1) {
        RTK_LOGE(TAG, "ERROR: pinmux not initialized\n");
        return -EINVAL;
    }

    //set enabled.
    GPIO_InitTypeDef gpio_initstruct_temp;
    gpio_initstruct_temp.GPIO_Pin = dummy->sd_pin_config.pinmux;
    gpio_initstruct_temp.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(&gpio_initstruct_temp);

    switch (type)
    {
        case AMP_CTRL_GPIO:
            if (enabled) {
                RTK_LOGD(TAG, "Enable amp:%ld\n", dummy->sd_pin_config.pinmux);
                GPIO_WriteBit(dummy->sd_pin_config.pinmux, 1);
                rtos_time_delay_ms(dummy->sd_pin_config.enable_time);
            } else {
                RTK_LOGD(TAG, "Disable amp:%ld\n", dummy->sd_pin_config.pinmux);
                GPIO_WriteBit(dummy->sd_pin_config.pinmux, 0);
                rtos_time_delay_ms(dummy->sd_pin_config.disable_time);
            }
            break;
        default:
            break;
    }

    return 0;
}

static int32_t
AmpDummy_SetMute(AudioAmplifier *amp, bool muted) {
    (void) amp;
    (void) muted;
    return 0;
}

static int32_t
AmpDummy_SetVolume(AudioAmplifier *amp, AmpChannelIndex channel, float vol) {
    (void) amp;
    (void) channel;
    (void) vol;
    return 0;
}

static int32_t
AmpDummy_SetFade(AudioAmplifier *amp, bool enable) {
    (void) amp;
    (void) enable;
    return 0;
}

static int32_t
AmpDummy_SetChannelMask(AudioAmplifier *amp, int8_t channel_mask) {
    (void) amp;
    (void) channel_mask;
    return 0;
}

static void AmpDummy_Destroy(AudioAmplifier *amp) {
    RTK_LOGI(TAG, "--Destroy\n");
    if (amp) {
        rtos_mem_free(amp);
    }
}

AudioAmplifier *AmpDummy_Create(void) {
    AmpDummy *dummy;

    RTK_LOGI(TAG, "++Create\n");

    dummy = (AmpDummy *)rtos_mem_zmalloc(sizeof(AmpDummy));
    if (!dummy) {
        RTK_LOGE(TAG, "ERROR: alloc amp fail\n");
        return NULL;
    }

    dummy->ops.SetSDPinmux = AmpDummy_SetSDPinmux;
    dummy->ops.SetUpI2c = AmpDummy_SetUpI2c;
    dummy->ops.SetClassType = AmpDummy_SetClassType;
    dummy->ops.SetEnable = AmpDummy_SetEnable;
    dummy->ops.SetMute = AmpDummy_SetMute;
    dummy->ops.SetVolume = AmpDummy_SetVolume;
    dummy->ops.SetFade = AmpDummy_SetFade;
    dummy->ops.SetChannelMask = AmpDummy_SetChannelMask;
    dummy->ops.Destroy = AmpDummy_Destroy;

    dummy->sd_pin_config.pinmux = -1;

    return &dummy->ops;
}
