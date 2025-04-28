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

#include "os_wrapper.h"

#include "i2c_api.h"

#include "ht513.h"

#define TAG "HT513"

/* fixed addr for ht513 in spec */
#define I2C_HT513_ADDR      0x6E

#define MAX_DAC_VOLUME      0xCF  // 0db
#define MIN_DAC_VOLUME      0x07  // -100db

typedef struct AmpHT513 {
    AudioAmplifier ops;
    AmpPinConfig sd_pin_config;
    i2c_t i2c;
    bool enable_fade;
} AmpHT513;

static inline void ht513_reg_write(i2c_t *i2c, uint8_t reg, uint8_t val) {
    char buf[2] = {reg, val};
    i2c_write(i2c, I2C_HT513_ADDR, buf, 2, 0);
}

static int32_t
HT513_SetSDPinmux(AudioAmplifier *amp, AmpPinConfig *pinmux_info) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    if (!pinmux_info) {
        RTK_LOGE(TAG, "ERROR: pinmux_info null");
        return -EINVAL;
    }

    ht513->sd_pin_config = *pinmux_info;

    return 0;
}

static int32_t
HT513_SetUpI2c(AudioAmplifier *amp, AmpI2cConfig *i2c_config) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    if (!i2c_config) {
        RTK_LOGE(TAG, "ERROR: i2c_config null");
        return -EINVAL;
    }

    memset(&ht513->i2c, 0x00, sizeof(i2c_t));
    ht513->i2c.i2c_idx = i2c_config->port;

    i2c_init(&ht513->i2c, i2c_config->sda_pinmux, i2c_config->scl_pinmux);
    i2c_frequency(&ht513->i2c, i2c_config->bus_clk);

    // Fade 0.5db every LRCLK, default right channel.
    ht513_reg_write(&ht513->i2c, 0x17, 0x73);

    if (ht513->enable_fade) {
        ht513_reg_write(&ht513->i2c, 0x14, 0x90);
    } else {
        ht513_reg_write(&ht513->i2c, 0x14, 0x80);
    }

    return 0;
}

static int32_t
HT513_SetClassType(AudioAmplifier *amp, AmpClassType type) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    RTK_LOGD(TAG, "SetClassType %d", type);

    switch (type)
    {
        case AMP_CLASS_TYPE_D:
            ht513_reg_write(&ht513->i2c, 0x19, 0X70);
            break;
        case AMP_CLASS_TYPE_AB:
            ht513_reg_write(&ht513->i2c, 0x19, 0X20);
            break;
        default:
            return -EINVAL;
    }

    rtos_time_delay_ms(1);

    return 0;
}

static int32_t
HT513_SetEnabled(AudioAmplifier *amp, AmpControlMode type, bool enabled) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    if (ht513->sd_pin_config.pinmux == -1) {
        RTK_LOGE(TAG, "ERROR: pinmux not initialized\n");
        return -EINVAL;
    }

    //set enabled.
    GPIO_InitTypeDef gpio_initstruct_temp;
    gpio_initstruct_temp.GPIO_Pin = ht513->sd_pin_config.pinmux;
    gpio_initstruct_temp.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(&gpio_initstruct_temp);

    switch (type)
    {
        case AMP_CTRL_GPIO:
            if (enabled) {
                RTK_LOGD(TAG, "Enable amp:%ld\n", ht513->sd_pin_config.pinmux);
                GPIO_WriteBit(ht513->sd_pin_config.pinmux, 1);
                rtos_time_delay_ms(ht513->sd_pin_config.enable_time);
            } else {
                RTK_LOGD(TAG, "Disable amp:%ld\n", ht513->sd_pin_config.pinmux);
                GPIO_WriteBit(ht513->sd_pin_config.pinmux, 0);
                rtos_time_delay_ms(ht513->sd_pin_config.disable_time);
            }
            break;
        case AMP_CTRL_I2C:
            if (enabled) {
                RTK_LOGW(TAG, "Not support i2c enable\n");
            } else {
                RTK_LOGD(TAG, "Disable amp %ld", ht513->sd_pin_config.pinmux);
                HT513_SetClassType(amp, AMP_CLASS_TYPE_AB);

                GPIO_WriteBit(ht513->sd_pin_config.pinmux, 0);
                rtos_time_delay_ms(ht513->sd_pin_config.disable_time);

                HT513_SetClassType(amp, AMP_CLASS_TYPE_D);
            }
            break;
        default:
            break;
    }

    return 0;
}

static int32_t
HT513_SetMute(AudioAmplifier *amp, bool muted) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    RTK_LOGD(TAG, "SetMute %d\n", muted);

    if (ht513->enable_fade) {
        if (muted) {
            ht513_reg_write(&ht513->i2c, 0x14, 0x94);
        } else {
            ht513_reg_write(&ht513->i2c, 0x14, 0x90);
        }
    } else {
        if (muted) {
            ht513_reg_write(&ht513->i2c, 0x14, 0x84);
        } else {
            ht513_reg_write(&ht513->i2c, 0x14, 0x80);
        }
    }

    return 0;
}

/*
 * step:0.5db
 * <00000111  means mute
 * 00000111  -100db
 * 11001111   0db
 * 11111111  24db
 */
static int32_t
HT513_SetVolume(AudioAmplifier *amp, enum AmpChannelIndex channel, float vol) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;
    uint32_t vol_index = vol * (MAX_DAC_VOLUME - MIN_DAC_VOLUME) + 0x07;

    RTK_LOGD(TAG, "SetVolume %lx\n", vol_index);

    switch (channel)
    {
        case AMP_CHANNEL_INDEX_0:
            ht513_reg_write(&ht513->i2c, 0x15, vol_index);
            break;
        case AMP_CHANNEL_INDEX_1:
            ht513_reg_write(&ht513->i2c, 0x16, vol_index);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

static int32_t
HT513_SetFade(AudioAmplifier *amp, bool enable) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;
    ht513->enable_fade = enable;
    return 0;
}

static int32_t
HT513_SetChannelMask(AudioAmplifier *amp, int8_t channel_mask) {
    AmpHT513 *ht513 = (AmpHT513 *)amp;

    RTK_LOGD(TAG, "SetChannelMask %d\n", channel_mask);

    // ht513 only has two channels input and one channel output.
    // channel index can only be 0x02 or 0x01.
    switch (channel_mask)
    {
        case AMP_CHANNEL_INDEX_0:
            ht513_reg_write(&ht513->i2c, 0x17, 0xF3);
            break;
        case AMP_CHANNEL_INDEX_1:
            ht513_reg_write(&ht513->i2c, 0x17, 0x73);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

static void HT513_Destroy(AudioAmplifier *amp) {
    RTK_LOGD(TAG, "--Destroy\n");
    if (amp) {
        AmpHT513 *ht513 = (AmpHT513 *)amp;
        i2c_reset(&ht513->i2c);
        rtos_mem_free(ht513);
    }
}

AudioAmplifier *HT513_Create(void) {
    AmpHT513 *ht513;
    
    RTK_LOGD(TAG, "++Create\n");

    ht513 = (AmpHT513  *)rtos_mem_zmalloc(sizeof(AmpHT513));
    if (!ht513) {
        RTK_LOGE(TAG, "ERROR: alloc amp fail\n");
        return NULL;
    }

    ht513->ops.SetSDPinmux = HT513_SetSDPinmux;
    ht513->ops.SetUpI2c = HT513_SetUpI2c;
    ht513->ops.SetClassType = HT513_SetClassType;
    ht513->ops.SetEnable = HT513_SetEnabled;
    ht513->ops.SetMute = HT513_SetMute;
    ht513->ops.SetVolume = HT513_SetVolume;
    ht513->ops.SetFade = HT513_SetFade;
    ht513->ops.SetChannelMask = HT513_SetChannelMask;
    ht513->ops.Destroy = HT513_Destroy;

    //if enable fade, then usrcfg's amp disable should be set as 45ms.
    //if disable fade, then usecfg's amp disable should be set as 1ms as spec(real test should be 9ms instead of 1ms).
    ht513->enable_fade = false;

    ht513->sd_pin_config.pinmux = -1;

    return &ht513->ops;
}
