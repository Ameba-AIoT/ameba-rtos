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
 * @addtogroup Audio
 * @{
 *
 * @brief Declares APIs for audio framework.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_control.h
 *
 * @brief Provides APIs of the audio control, such as volume, mute, input devices.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_CONTROL_H
#define AMEBA_AUDIO_INTERFACES_AUDIO_AUDIO_CONTROL_H

#include <stdint.h>
#include "audio/audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines all the audio playback devices.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	/* [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
	 * [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
	 */
	RTAUDIO_AMIC1         = 0,
	RTAUDIO_AMIC2         = 1,
	RTAUDIO_AMIC3         = 2,
	RTAUDIO_AMIC4         = 3,
	RTAUDIO_AMIC5         = 4,
	RTAUDIO_DMIC1         = 5,
	RTAUDIO_DMIC2         = 6,
	RTAUDIO_DMIC3         = 7,
	RTAUDIO_DMIC4         = 8,
	RTAUDIO_DMIC5         = 9,
	RTAUDIO_DMIC6         = 10,
	RTAUDIO_DMIC7         = 11,
	RTAUDIO_DMIC8         = 12,
	RTAUDIO_MIC_MAX_NUM   = 13,
};

/**
 * @brief Defines the audio capture main usages.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	RTAUDIO_CAPTURE_USAGE_AMIC         = 0,
	RTAUDIO_CAPTURE_USAGE_DMIC         = 1,
	RTAUDIO_CAPTURE_USAGE_DMIC_REF_AMIC = 2,
	RTAUDIO_CAPTURE_USAGE_LINE_IN      = 3,
	RTAUDIO_CAPTURE_USAGE_MAX_NUM      = 4,
};

/**
 * @brief Defines all the audio mic bst gain values.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	RTAUDIO_MICBST_GAIN_0DB          = 0,
	RTAUDIO_MICBST_GAIN_5DB          = 1,
	RTAUDIO_MICBST_GAIN_10DB         = 2,
	RTAUDIO_MICBST_GAIN_15DB         = 3,
	RTAUDIO_MICBST_GAIN_20DB         = 4,
	RTAUDIO_MICBST_GAIN_25DB         = 5,
	RTAUDIO_MICBST_GAIN_30DB         = 6,
	RTAUDIO_MICBST_GAIN_35DB         = 7,
	RTAUDIO_MICBST_GAIN_40DB         = 8,
	RTAUDIO_MICBST_GAIN_MAX_NUM      = 9,
};

/**
 * @brief Defines all the audio PLL clock tune options.
 *
 * @since 1.0
 * @version 1.0
 */
enum {
	RTAUDIO_PLL_AUTO    = 0,
	RTAUDIO_PLL_FASTER  = 1,
	RTAUDIO_PLL_SLOWER  = 2,
};

typedef struct {
	uint32_t H0_Q;
	uint32_t B1_Q;
	uint32_t B2_Q;
	uint32_t A1_Q;
	uint32_t A2_Q;
} EqFilterCoef;

/**
 * @brief Set Hardware Volume of audio dac.
 *
 * @param left_volume volume of left channel, 0.0-1.0.
 * @param right_volume volume of right channel, 0.0-1.0.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetHardwareVolume(float left_volume, float right_volume);

/**
 * @brief Get Hardware Volume of audio dac.
 *
 * @param left_volume pointer of volume of left channel to get, 0.0-1.0.
 * @param right_volume pointer of volume of right channel to get, 0.0-1.0.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | HAL control instance not created.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetHardwareVolume(float *left_volume, float *right_volume);

/**
 * @brief Set Amplifier En Pin.
 *
 * @param amp_pin the pin of the amplifier en, you can get your pin value from:
 * fwlib/include/ameba_pinmux.h, for example, if your pin is _PB_7, it's value is "#define _PB_7 (0x27)"
 * which is 39 of uint32_t.So the amp_pin here should set 39.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetAmplifierEnPin(uint32_t amp_pin);

/**
 * @brief Set Amplifier En Pin.
 *
 * @return Returns amp_pin the pin of the amplifier en, value is from:
 *         fwlib/include/ameba_pinmux.h, for example, if your pin is _PB_7, it's value is "#define _PB_7 (0x27)"
 *         which is 39 of uint32_t.
 *         If the value < 0, means get fail.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetAmplifierEnPin(void);

/**
 * @brief Set Amplifier Mute.
 * Note: this interface may take some time, see component/audio/audio_hal/ameba_audio_stream_control:
 *       ameba_audio_ctl_set_amp_state.
 *
 * @param mute true means mute amplifier, false means unmute amplifier.
 * @return  Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | HAL control instance not created.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetAmplifierMute(bool mute);

/**
 * @brief Get Amplifier Mute State.
 *
 * @return  Returns the state of amplifier. true means amplifier is muted, false means amplifier is unmuted.
 * @since 1.0
 * @version 1.0
 */
bool RTAudioControl_GetAmplifierMute(void);

/**
 * @brief Set Audio Codec Mute.
 *
 * @param mute true means mute dac, false means unmute dac.
 * @return  Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | HAL control instance not created.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetPlaybackMute(bool mute);

/**
 * @brief Get Audio Codec Mute State.
 *
 * @return  Returns the state of dac. true means dac is muted, false means dac is unmuted.
 * @since 1.0
 * @version 1.0
 */
bool RTAudioControl_GetPlaybackMute(void);

/**
 * @brief Set Playback Device. Please set it before create RTAudioTrack.
 *
 * @param device_category the device of playback, maybe RTDEVICE_OUT_SPEAKER or RTDEVICE_OUT_HEADPHONE.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetPlaybackDevice(uint32_t device_category);

/**
 * @brief Get Playback Device.
 *
 * @return Returns the device of playback, maybe RTDEVICE_OUT_SPEAKER or RTDEVICE_OUT_HEADPHONE.
 *         If the value < 0, means get fail.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetPlaybackDevice(void);

/**
 * @brief Set Capture Mic type for channel.
 *
 * @param channel_num the channel number to set mic type.
 * @param mic_category the mic type, can be RTAUDIO_AMIC1...RTAUDIO_DMIC8.
 *       [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
 *       [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetChannelMicCategory(uint32_t channel_num, uint32_t mic_category);

/**
 * @brief Get Mic Category.
 * @param channel_num the channel number to get mic type.
 * @return Returns the mic type, can be RTAUDIO_AMIC1...RTAUDIO_DMIC8.
 *         [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
 *         [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
 *         If the value < 0, means get fail.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetChannelMicCategory(uint32_t channel_num);

/**
 * @brief Set Capture volume for channel.
 *
 * @param channel_num the channel number to set volume.
 * @param volume the value of volume, can be 0x00-0xaf.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *          0x00 -17.625dB
  *          0xaf 48dB
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetCaptureChannelVolume(uint32_t channel_num, uint32_t volume);

/**
 * @brief Get Capture volume for channel.
 *
 * @param channel_num the channel number to get volume.
 * @return the value of volume, can be 0x00-0xaf.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *          0x00 -17.625dB
  *          0xaf 48dB
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetCaptureChannelVolume(uint32_t channel_num);

/**
 * @brief Set Capture volume for channel.
 *
 * @param channels the total channels number to set volume, also the channels to capture.
 * @param volume the value of volume, can be 0x00-0xaf.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *          0x00 -17.625dB
  *          0xaf 48dB
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetCaptureVolume(uint32_t channels, uint32_t volume);

/**
 * @brief Set Micbst Gain.
 *
 * @param mic_category the mic type, can be RTAUDIO_AMIC1...RTAUDIO_DMIC8.
 *       [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
 *       [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
 * @param gain can be RTAUDIO_MICBST_GAIN_0DB-RTAUDIO_MICBST_GAIN_40DB
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetMicBstGain(uint32_t mic_category, uint32_t gain);

/**
 * @brief Get MicBst gain of microphone.
 *
 * @param mic_category the mic type, can be RTAUDIO_AMIC1...RTAUDIO_DMIC8.
 *       [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
 *       [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
 * @return gain of mic_category, can be RTAUDIO_MICBST_GAIN_0DB-RTAUDIO_MICBST_GAIN_40DB
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetMicBstGain(uint32_t mic_category);

/**
 * @brief Set adc's hfp fc.
 *
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
int32_t RTAudioControl_SetCaptureHpfFc(uint32_t channel_num, uint32_t fc);

/**
 * @brief Get adc's hfp fc.
 *
 * @param channel_num the channel to adjust hfp's fc, ranges 0 ~ (total_channel-1).
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
int32_t RTAudioControl_GetCaptureHpfFc(uint32_t channel_num);

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
int32_t RTAudioControl_SetCaptureEqEnable(uint32_t channel_num, bool state);

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
int32_t RTAudioControl_SetCaptureEqFilter(uint32_t channel_num, uint32_t band_sel, EqFilterCoef *eq_filter_coef);

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
int32_t RTAudioControl_SetCaptureEqBand(uint32_t channel_num, uint32_t band_sel, bool state);

/**
 * @brief Set mic usage.
 *
 * @param mic_usage RTAUDIO_CAPTURE_USAGE_AMIC, or RTAUDIO_CAPTURE_USAGE_DMIC, if user's
 *        microphone data is recorded from amic, then set RTAUDIO_CAPTURE_USAGE_AMIC, if
 *        user's microphone data is recorded from dmic, then set RTAUDIO_CAPTURE_USAGE_DMIC.
 *        The reference data is always recorded from amic, even if the usage is
 *        RTAUDIO_CAPTURE_USAGE_DMIC, the default setting is RTAUDIO_CAPTURE_USAGE_AMIC.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetMicUsage(uint32_t mic_usage);

/**
 * @brief Get mic usage.
 *
 * @param Returns RTAUDIO_CAPTURE_USAGE_AMIC, or RTAUDIO_CAPTURE_USAGE_DMIC, if user's
 *        microphone data is recorded from amic, then set RTAUDIO_CAPTURE_USAGE_AMIC, if
 *        user's microphone data is recorded from dmic, then set RTAUDIO_CAPTURE_USAGE_DMIC.
 *        If mic data is from dmic, the reference data is recorded from amic, then set
 *        RTAUDIO_CAPTURE_USAGE_DMIC_REF_AMIC, the default setting is RTAUDIO_CAPTURE_USAGE_AMIC.
 *        if the return value<0, then get fail.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_GetMicUsage(void);

/**
 * @brief Adjust PLL clk .
 *
 * @param rate sample rate of current stream
 * @param ppm ~1.55ppm per FOF step
 * @param action can be RTAUDIO_PLL_AUTO RTAUDIO_PLL_FASTER RTAUDIO_PLL_SLOWER
 * @return Returns the ppm adjusted.
 * @since 1.0
 * @version 1.0
 */
float RTAudioControl_AdjustPLLClock(uint32_t rate, float ppm, uint32_t action);

/**
 * @brief Set Audio Record Mute.
 *
 * @param channel the channel to mute or unmute record.
 * @param mute true means mute record, false means unmute record.
 * @return  Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | HAL control instance not created.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioControl_SetRecordMute(uint32_t channel, bool mute);

/**
 * @brief Get Audio Record Mute State.
 *
 * @param channel the channel to mute or unmute state.
 * @return  Returns the state of record. true means record is muted, false means record is unmuted.
 * @since 1.0
 * @version 1.0
 */
bool RTAudioControl_GetRecordMute(uint32_t channel);

#ifdef __cplusplus
}
#endif


#endif
