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

/**
 * @addtogroup HAL
 * @{
 *
 * @brief Declares the structures and interfaces for the Hardware Abstraction Layer (HAL) module.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hw_control.h
 *
 * @brief Provides APIs for audio control.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_AUDIO_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_CONTROL_H
#define AMEBA_AUDIO_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_CONTROL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "hardware/audio/audio_hw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	/* [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
	 * [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
	 */
	AUDIO_HW_AMIC1         = 0,
	AUDIO_HW_AMIC2         = 1,
	AUDIO_HW_AMIC3         = 2,
	AUDIO_HW_AMIC4         = 3,
	AUDIO_HW_AMIC5         = 4,
	AUDIO_HW_DMIC1         = 5,
	AUDIO_HW_DMIC2         = 6,
	AUDIO_HW_DMIC3         = 7,
	AUDIO_HW_DMIC4         = 8,
	AUDIO_HW_DMIC5         = 9,
	AUDIO_HW_DMIC6         = 10,
	AUDIO_HW_DMIC7         = 11,
	AUDIO_HW_DMIC8         = 12,
	AUDIO_HW_MIC_MAX_NUM   = 13,
};

/**
 * @brief Defines the audio capture main usages.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	AUDIO_HW_CAPTURE_USAGE_AMIC         = 0,
	AUDIO_HW_CAPTURE_USAGE_DMIC         = 1,
	AUDIO_HW_CAPTURE_USAGE_DMIC_REF_AMIC = 2,
	AUDIO_HW_CAPTURE_USAGE_LINE_IN      = 3,
	AUDIO_HW_CAPTURE_USAGE_MAX_NUM      = 4,
};
/**
 * @brief Defines all the audio mic bst gain values.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	AUDIO_HW_MICBST_GAIN_0DB          = 0,
	AUDIO_HW_MICBST_GAIN_5DB          = 1,
	AUDIO_HW_MICBST_GAIN_10DB         = 2,
	AUDIO_HW_MICBST_GAIN_15DB         = 3,
	AUDIO_HW_MICBST_GAIN_20DB         = 4,
	AUDIO_HW_MICBST_GAIN_25DB         = 5,
	AUDIO_HW_MICBST_GAIN_30DB         = 6,
	AUDIO_HW_MICBST_GAIN_35DB         = 7,
	AUDIO_HW_MICBST_GAIN_40DB         = 8,
	AUDIO_HW_MICBST_GAIN_MAX_NUM      = 9,
};

/**
 * @brief Defines all the audio playback devices.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/** play through speaker */
	AUDIO_HW_DEVICE_SPEAKER         = 0,
	/** play through headphone*/
	AUDIO_HW_DEVICE_HEADPHONE       = 1,
	AUDIO_HW_DEVICE_MAX_NUM         = 2,
};

struct AudioHwControl {
	/**
	 * @brief set audio dac volume.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param left_volume is the left channel dac volume, from 0.0-1.0.
	 * @param right_volume is the right channel dac volume, from 0.0-1.0.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetHardwareVolume)(struct AudioHwControl *control, float left_volume, float right_volume);

	/**
	 * @brief set audio dac volume.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param left_volume is the left channel dac volume, from 0.0-1.0.
	 * @param right_volume is the right channel dac volume, from 0.0-1.0.
	 * @return Returns <b>0</b> if get successfully; returns a negative value otherwise.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetHardwareVolume)(struct AudioHwControl *control, float *left_volume, float *right_volume);

	/**
	 * @brief set audio amplifier pin.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param amp_pin is the left channel dac volume, from 0.0-1.0.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetAmplifierEnPin)(struct AudioHwControl *control, uint32_t amp_pin);

	/**
	 * @brief get audio amplifier pin.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @return Returns amplifier pin value.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetAmplifierEnPin)(struct AudioHwControl *control);

	/**
	 * @brief Set Amplifier Mute.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mute true means mute amplifier, false means unmute amplifier.
	 * @return  Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetAmplifierMute)(struct AudioHwControl *control, bool mute);

	/**
	 * @brief Get Amplifier Mute State.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @return  Returns the state of amplifier. true means amplifier is muted, false means amplifier is unmuted.
	 * @since 1.0
	 * @version 1.0
	 */
	bool (*GetAmplifierMute)(struct AudioHwControl *control);

	/**
	 * @brief Set Audio Codec Mute.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mute true means mute dac, false means unmute dac.
	 * @return  Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetPlaybackMute)(struct AudioHwControl *control, bool mute);

	/**
	 * @brief Get Audio Codec Mute State.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @return  Returns the state of dac. true means dac is muted, false means dac is unmuted.
	 * @since 1.0
	 * @version 1.0
	 */
	bool (*GetPlaybackMute)(struct AudioHwControl *control);

	/**
	 * @brief set audio playback device type.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param device_category is device type of playback.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetPlaybackDevice)(struct AudioHwControl *control, uint32_t device_category);

	/**
	 * @brief get audio playback device type.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @return Returns playback device type, maybe AUDIO_HW_DEVICE_SPEAKER or AUDIO_HW_DEVICE_HEADPHONE.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetPlaybackDevice)(struct AudioHwControl *control);

	/**
	 * @brief set mic category of audio channel.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num is the channel to set mic category.
	 * @param mic_category is mic category of audio channel.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetChannelMicCategory)(struct AudioHwControl *control, uint32_t channel_num, uint32_t mic_category);

	/**
	 * @brief get mic category of audio channel.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num is the channel to get mic category.
	 * @return Returns the mic category of audio channel.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetChannelMicCategory)(struct AudioHwControl *control, uint32_t channel_num);

	/**
	 * @brief set volume of record channel.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num is the channel to set volume.
	 * @param volume is volume to set to channel_num, it can be 0x00-0xaf.
	 *        This parameter can be -17.625dB~48dB in 0.375dB step
	 *        0x00 -17.625dB
	 *        0xaf 48dB
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureChannelVolume)(struct AudioHwControl *control, uint32_t channel_num, uint32_t volume);

	/**
	 * @brief Get volume of record channel.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num is the channel to get volume.
	 * @return volume is volume to set to channel_num, it can be 0x00-0xaf.
	 *        This parameter can be -17.625dB~48dB in 0.375dB step
	 *        0x00 -17.625dB
	 *        0xaf 48dB
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetCaptureChannelVolume)(struct AudioHwControl *control, uint32_t channel_num);

	/**
	 * @brief Set Capture volume for channel.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channels the total channels number to set volume, also the channels to capture.
	 * @param volume the value of volume, can be 0x00-0xaf.
	  *          This parameter can be -17.625dB~48dB in 0.375dB step
	  *          0x00 -17.625dB
	  *          0xaf 48dB
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureVolume)(struct AudioHwControl *control, uint32_t channels, uint32_t volume);

	/**
	 * @brief Set Micbst Gain.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mic_category the mic type, can be AUDIO_HW_AMIC1...AUDIO_HW_DMIC8.
	 *       [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
	 *       [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
	 * @param gain can be AUDIO_HW_MICBST_GAIN_0DB-AUDIO_HW_MICBST_GAIN_40DB
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetMicBstGain)(struct AudioHwControl *control, uint32_t mic_category, uint32_t gain);

	/**
	 * @brief Get Micbst Gain.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mic_category the mic type, can be AUDIO_HW_AMIC1...AUDIO_HW_DMIC8.
	 *       [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
	 *       [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
	 * @return gain can be AUDIO_HW_MICBST_GAIN_0DB-AUDIO_HW_MICBST_GAIN_40DB
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetMicBstGain)(struct AudioHwControl *control, uint32_t mic_category);

	/**
	 * @brief Set adc's hfp fc.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num the channel to adjust hfp's fc, ranges: 0~8.
	 * @param fc : select high pass filter fc
	 *	           This parameter can be one of the following values:
	 *	           @arg 0：5e10^-3 Fs
	 *	           @arg 1：2.5e10^-3 Fs
	 *	           @arg 2：1.125e10^-3 Fs
	 *	           @arg 3: 6.25e10^-4 Fs
	 *	           @arg 4: 3.125e10^-4 Fs
	 *	           @arg 5: 1.5625e10^-4 Fs
	 *	           @arg 6: 7.8125e10^-5 Fs
	 *	           @arg 7: 3.90625e10^-5 Fs
	 * @return Returns a value listed below: \n
	 * int32_t | Description
	 * ----------------------| -----------------------
	 * AUDIO_OK | the operation is successful.
	 * AUDIO_ERR_INVALID_OPERATION | param not supported.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureHpfFc)(struct AudioHwControl *control, uint32_t channel_num, uint32_t fc);

	/**
	 * @brief Get adc's hfp fc.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel_num the channel to adjust hfp's fc, ranges: 0~8.
	 * @return Returns a value listed below: \n
	 * 0：5e10^-3 Fs
	 * 1：2.5e10^-3 Fs
	 * 2：1.125e10^-3 Fs
	 * 3: 6.25e10^-4 Fs
	 * 4: 3.125e10^-4 Fs
	 * 5: 1.5625e10^-4 Fs
	 * 6: 7.8125e10^-5 Fs
	 * 7: 3.90625e10^-5 Fs
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetCaptureHpfFc)(struct AudioHwControl *control, uint32_t channel_num);

	/**
	 * @brief Set capture eq enable.
	 *
	 * @param channel_num channel to enable eq, ranges 0 ~ (total_channel-1).
	 * @param state true means enable eq, false means disable eq for this channel.
	 * @return Returns a value listed below: \n
	 * int32_t | Description
	 * ----------------------| -----------------------
	 * AUDIO_OK | the operation is successful.
	 * AUDIO_ERR_INVALID_OPERATION | param not supported.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureEqEnable)(struct AudioHwControl *control, uint32_t channel_num, bool state);

	/**
	 * @brief Set capture eq coef.
	 *
	 * @param channel_num channel to set eq filter, ranges 0 ~ (total_channel-1).
	 * @param band_sel band to set eq filter, ranges 0 ~ 4.
	 * @param eq_filter_coef eq filter coefficient.
	 * @return Returns a value listed below: \n
	 * int32_t | Description
	 * ----------------------| -----------------------
	 * AUDIO_OK | the operation is successful.
	 * AUDIO_ERR_INVALID_OPERATION | param not supported.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureEqFilter)(struct AudioHwControl *control, uint32_t channel_num, uint32_t band_sel, struct AudioHwEqFilterCoef *eq_filter_coef);

	/**
	 * @brief Set capture eq band state.
	 *
	 * @param channel_num channel to set eq filter, ranges 0 ~ (total_channel-1).
	 * @param band_sel band to set eq filter, ranges 0 ~ 4.
	 * @param state true to enable band, false to disable band.
	 * @return Returns a value listed below: \n
	 * int32_t | Description
	 * ----------------------| -----------------------
	 * AUDIO_OK | the operation is successful.
	 * AUDIO_ERR_INVALID_OPERATION | param not supported.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetCaptureEqBand)(struct AudioHwControl *control, uint32_t ad_chn, uint32_t band_sel, bool state);

	/**
	 * @brief Set mic usage.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mic_usage AUDIO_HW_CAPTURE_USAGE_AMIC, or AUDIO_HW_CAPTURE_USAGE_DMIC, if user's
	 *        microphone data is recorded from amic, then set AUDIO_HW_CAPTURE_USAGE_AMIC, if
	 *        user's microphone data is recorded from dmic, then set AUDIO_HW_CAPTURE_USAGE_DMIC.
	 *        The reference data is always recorded from amic, even if the usage is
	 *        AUDIO_HW_CAPTURE_USAGE_DMIC, the default setting is AUDIO_HW_CAPTURE_USAGE_AMIC.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetMicUsage)(struct AudioHwControl *control, uint32_t mic_usage);

	/**
	 * @brief Get mic usage.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @return Returns AUDIO_HW_CAPTURE_USAGE_AMIC, or AUDIO_HW_CAPTURE_USAGE_DMIC, if user's
	 *        microphone data is recorded from amic, then set AUDIO_HW_CAPTURE_USAGE_AMIC, if
	 *        user's microphone data is recorded from dmic, then set AUDIO_HW_CAPTURE_USAGE_DMIC.
	 *        If mic data is from dmic, the reference data is recorded from amic, then set
	 *        AUDIO_HW_CAPTURE_USAGE_DMIC_REF_AMIC, the default setting is AUDIO_HW_CAPTURE_USAGE_AMIC.
	 *        if the return value<0, then get fail.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*GetMicUsage)(struct AudioHwControl *control);

	/**
	 * @brief Adjust PLL clk .
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param rate sample rate of current stream
	 * @param ppm ~1.55ppm per FOF step
	 * @param action can be AUDIO_HW_PLL_AUTO AUDIO_HW_PLL_FASTER AUDIO_HW_PLL_SLOWER
	 * @return Returns pll actually set.
	 * @since 1.0
	 * @version 1.0
	 */
	float (*AdjustPLLClock)(struct AudioHwControl *control, uint32_t rate, float ppm, uint32_t action);

	/**
	 * @brief Set Audio ADC Mute.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param channel is the channel number to mute or unmute.
	 * @param mute true means mute adc, false means unmute adc.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 * @since 1.0
	 * @version 1.0
	 */
	int32_t (*SetRecordMute)(struct AudioHwControl *control, uint32_t channel, bool mute);

	/**
	 * @brief Set Audio ADC Mute.
	 *
	 * @param control is the pointer to the audio control to operate.
	 * @param mute is the record channel mute or unmute.
	 * @return Returns the mute state of record channel.
	 * @since 1.0
	 * @version 1.0
	 */
	bool (*GetRecordMute)(struct AudioHwControl *control, uint32_t channel);
};

struct AudioHwControl *GetAudioHwControl(void);
void DestroyAudioHwControl(struct AudioHwControl *control);

#ifdef __cplusplus
}
#endif

#endif