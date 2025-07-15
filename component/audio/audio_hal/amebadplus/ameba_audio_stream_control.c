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

#include <inttypes.h>
#include <stdlib.h>

#include "basic_types.h"

#include "ameba.h"

#include "amp/audio_amplifier.h"

#include "ameba_audio_types.h"
#include "ameba_audio_hw_usrcfg.h"
#include "ameba_audio_stream_utils.h"

#include "audio_hw_compat.h"
#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"

#include "ameba_audio_stream_control.h"

#define DEVICE_SPEAKER 1
#define IS_AUDIO_DMIC(NUM) (((NUM) == AMEBA_AUDIO_DMIC1) || ((NUM) == AMEBA_AUDIO_DMIC2)

static StreamControl *g_control_instance = NULL;
volatile uint32_t g_control_create = 0;

int32_t ameba_audio_ctl_set_mic_category_for_adc(StreamControl *control, uint32_t adc_num, uint32_t mic_category);

StreamControl *ameba_audio_get_ctl(void)
{
	uint32_t expected_value = 0;
	uint32_t new_value = 1;

	if (AudioHALAtomicCompareAddSwap(&g_control_create, &expected_value, &new_value)) {
		if (g_control_instance == NULL) {
			g_control_instance = (StreamControl *)rtos_mem_zmalloc(sizeof(StreamControl));
			if (!g_control_instance) {
				HAL_AUDIO_ERROR("alloc control fail");
				goto err_finish;
			}

			g_control_instance->amplifier = CreateAudioAmplifier(AUDIO_HW_AMPLIFIER_TYPE);
			if (!g_control_instance->amplifier) {
				goto err_create_amplifier;
			}

			g_control_instance->board_amp_pin = AUDIO_HW_AMPLIFIER_PIN;
			g_control_instance->amp_state = true;
			g_control_instance->tx_state = false;
			g_control_instance->playback_device = AMEBA_AUDIO_DEVICE_SPEAKER;
			g_control_instance->capture_usage = AMEBA_AUDIO_CAPTURE_USAGE_DMIC;
			g_control_instance->adc_use_status = 0;
			g_control_instance->volume_for_dacl = 0.6;
			g_control_instance->volume_for_dacr = 0.6;
			g_control_instance->volume_for_adc[0] = 0x2f;
			g_control_instance->volume_for_adc[1] = 0x2f;
			g_control_instance->mic_category_for_adc[0] = AMEBA_AUDIO_DMIC1;
			g_control_instance->mic_category_for_adc[1] = AMEBA_AUDIO_DMIC2;
			g_control_instance->mute_for_adc[0] = false;
			g_control_instance->mute_for_adc[1] = false;

			AmpPinConfig amp_info;
			amp_info.pinmux = AUDIO_HW_AMPLIFIER_PIN;
			amp_info.enable_time = AUDIO_HW_AMPLIFIER_ENABLE_TIME;
			amp_info.disable_time = AUDIO_HW_AMPLIFIER_DISABLE_TIME;
			g_control_instance->amplifier->SetSDPinmux(g_control_instance->amplifier, &amp_info);

			AmpI2cConfig i2c_config;
			i2c_config.sda_pinmux = AUDIO_HW_AMPLIFIER_I2C_SDA_PIN;
			i2c_config.scl_pinmux = AUDIO_HW_AMPLIFIER_I2C_SCL_PIN;
			i2c_config.bus_clk = 400000;
			i2c_config.port = 0;
			g_control_instance->amplifier->SetUpI2c(g_control_instance->amplifier, &i2c_config);
		}
	}

	return g_control_instance;

err_create_amplifier:
	rtos_mem_free(g_control_instance);
	g_control_instance = NULL;

err_finish:
	return NULL;
}

void ameba_audio_destroy_ctl()
{
	uint32_t expected_value = 1;
	uint32_t new_value = 0;

	if (AudioHALAtomicCompareAddSwap(&g_control_create, &expected_value, &new_value)) {
		if (g_control_instance != NULL) {
			DestoryAudioAmplifier(g_control_instance->amplifier);
			rtos_mem_free(g_control_instance);
			g_control_instance = NULL;
		}
	}
}

int32_t ameba_audio_ctl_set_tx_volume(StreamControl *control, float left, float right)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	control->volume_for_dacl = left;
	control->amplifier->SetVolume(control->amplifier, AMP_CHANNEL_INDEX_0, control->volume_for_dacl);

	control->volume_for_dacr = right;
	control->amplifier->SetVolume(control->amplifier, AMP_CHANNEL_INDEX_1, control->volume_for_dacr);

	return HAL_OSAL_ERR_NOT_SUPPORT;
}

int32_t ameba_audio_ctl_get_tx_volume(StreamControl *control, float *left, float *right)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	*left = control->volume_for_dacl;
	*right = control->volume_for_dacr;

	return HAL_OSAL_ERR_NOT_SUPPORT;
}

int32_t ameba_audio_ctl_set_tx_mute(StreamControl *control, bool muted, bool should_zdet, bool user_set)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	(void) should_zdet;

	if (muted) {
		control->amplifier->SetMute(control->amplifier, true);
	} else {
		control->amplifier->SetMute(control->amplifier, false);
	}

	if (user_set) {
		control->tx_state = muted;
	}

	return HAL_OSAL_OK;
}

bool ameba_audio_ctl_get_tx_mute(StreamControl *control)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return false;
	}

	return control->tx_state;
}

int32_t ameba_audio_ctl_set_amp_pin(StreamControl *control, uint32_t pin)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	if (control->board_amp_pin != (int32_t)pin) {
		HAL_AUDIO_INFO("set amp pin from %" PRId32 " to %" PRIu32 "", control->board_amp_pin, pin);
		control->board_amp_pin = pin;
		AmpPinConfig amp_info;
		amp_info.pinmux = pin;
		amp_info.enable_time = AUDIO_HW_AMPLIFIER_ENABLE_TIME;
		amp_info.disable_time = AUDIO_HW_AMPLIFIER_DISABLE_TIME;
		control->amplifier->SetSDPinmux(control->amplifier, &amp_info);
	}

	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_get_amp_pin(StreamControl *control)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}
	return control->board_amp_pin;
}

int32_t ameba_audio_ctl_set_amp_state(StreamControl *control, bool state, bool user_set)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	if (control->board_amp_pin < 0) {
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	Pinmux_Swdoff();

	if (state) {
		control->amplifier->SetEnable(control->amplifier, AMP_CTRL_GPIO, true);
	} else {
#if AUDIO_HW_AMPLIFIER_TYPE == 1
		control->amplifier->SetEnable(control->amplifier, AMP_CTRL_I2C, false);
#else
		control->amplifier->SetEnable(control->amplifier, AMP_CTRL_GPIO, false);
#endif
	}

	if (user_set) {
		control->amp_state = state;
	}

	return HAL_OSAL_OK;
}

bool ameba_audio_ctl_get_amp_state(StreamControl *control)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	return control->amp_state;
}

int32_t ameba_audio_ctl_set_device_category(StreamControl *control, uint32_t device_category)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}
	control->playback_device = device_category;
	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_get_device_category(StreamControl *control)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}
	return control->playback_device;
}

int32_t ameba_audio_ctl_set_mic_usage(StreamControl *control, uint32_t mic_usage)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	control->capture_usage = mic_usage;
	switch (mic_usage) {
	case AMEBA_AUDIO_CAPTURE_USAGE_DMIC:
		control->mic_category_for_adc[0] = AMEBA_AUDIO_DMIC1;
		control->mic_category_for_adc[1] = AMEBA_AUDIO_DMIC2;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported mic usage");
		break;
	}

	control->adc_use_status = 0;

	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_get_mic_usage(StreamControl *control)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}
	return control->capture_usage;
}

int32_t ameba_audio_ctl_set_mic_category_for_adc(StreamControl *control, uint32_t adc_num, uint32_t mic_category)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	(void)adc_num;
	(void)mic_category;

	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_get_mic_category_for_adc(StreamControl *control, uint32_t adc_num)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	return control->mic_category_for_adc[adc_num];
}

int32_t ameba_audio_ctl_set_adc_volume(StreamControl *control, uint32_t adc_num, uint32_t volume)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	if (adc_num >= MAX_AD_NUM) {
		HAL_AUDIO_ERROR("ops, adc_num(%lu) out of range", adc_num);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	control->volume_for_adc[adc_num] = volume;

	if (ameba_audio_is_audio_ip_in_use(CODEC)) {
		AUDIO_CODEC_SetADCVolume(adc_num, volume);
	}

	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_get_adc_volume(StreamControl *control, uint32_t adc_num)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	return control->volume_for_adc[adc_num];
}

int32_t ameba_audio_ctl_set_tdm_adc_volume(StreamControl *control, uint32_t channels, uint32_t volume)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	if (channels > MAX_AD_NUM) {
		HAL_AUDIO_ERROR("ops, channels(%lu) out of range", channels);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	uint32_t i = 0;
	for (; i < channels; i++) {
		control->volume_for_adc[i] = volume;
		if (ameba_audio_is_audio_ip_in_use(CODEC)) {
			AUDIO_CODEC_SetADCVolume(i, volume);
		}
	}

	return HAL_OSAL_OK;
}

int32_t ameba_audio_ctl_set_mic_bst_gain(StreamControl *control, uint32_t mic_category, uint32_t gain)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	(void)mic_category;
	(void)gain;

	return HAL_OSAL_ERR_NOT_SUPPORT;

}

int32_t ameba_audio_ctl_get_mic_bst_gain(StreamControl *control, uint32_t mic_category)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	(void)mic_category;

	return HAL_OSAL_ERR_NOT_SUPPORT;
}

float ameba_audio_ctl_pll_clock_tune(StreamControl *control, uint32_t rate, float ppm, uint32_t action)
{
	float real_ppm = 0;

	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return 0;
	}

	if (rate % 8000 == 0) {
		//HAL_AUDIO_INFO("98M tune (%f, %lu).", ppm, action);
		real_ppm = PLL_I2S_98P304M_ClkTune(NULL, ppm, action);
	} else {
		real_ppm = PLL_I2S_45P158M_ClkTune(NULL, ppm, action);
	}

	return real_ppm;
}

int32_t ameba_audio_ctl_set_adc_mute(StreamControl *control, uint32_t adc_num, bool muted)
{
	uint32_t adc_state = UNMUTE;

	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return HAL_OSAL_ERR_INVALID_OPERATION;
	}

	control->mute_for_adc[adc_num] = muted;

	if (ameba_audio_is_audio_ip_in_use(CODEC)) {
		adc_state = muted ? MUTE : UNMUTE;
		AUDIO_CODEC_SetADCMute(adc_num, adc_state);
	}

	return HAL_OSAL_OK;
}

bool ameba_audio_ctl_get_adc_mute(StreamControl *control, uint32_t adc_num)
{
	if (control == NULL) {
		HAL_AUDIO_ERROR("ops, %s fail, control null", __func__);
		return false;
	}

	return control->mute_for_adc[adc_num];
}
