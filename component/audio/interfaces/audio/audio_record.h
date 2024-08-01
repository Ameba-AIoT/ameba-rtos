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
 * @file audio_record.h
 *
 * @brief Provides APIs of the audio capture streaming.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_FWK_MEDIA_AUDIO_AUDIOLITE_INTERFACES_AUDIO_AUDIO_RECORD_H
#define AMEBA_FWK_MEDIA_AUDIO_AUDIOLITE_INTERFACES_AUDIO_AUDIO_RECORD_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "audio/audio_time.h"
#include "audio/audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RTAudioRecord;

/**
 * @brief Defines information about audio capture parameters, including the sampling
 * rate (Hz), number of audio channels, audio codec format, bit rate, and portPin.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
	/** sample rate of record
	 * 8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000*/
	uint32_t sample_rate;
	/** channel count of record
	 * 1, 2, 4, 6, 8*/
	uint32_t channel_count;
	/** format of record
	 * supports RTAUDIO_FORMAT_PCM_16_BIT, RTAUDIO_FORMAT_PCM_32_BIT,
	 * RTAUDIO_FORMAT_PCM_24_BIT, RTAUDIO_FORMAT_PCM_8_24_BIT*/
	uint32_t format;
	/** device of record, supports RTDEVICE_IN_MIC and RTDEVICE_IN_DMIC_REF_AMIC, RTDEVICE_IN_I2S*/
	uint32_t device;
	/** buffer bytes per one period of record*/
	uint32_t buffer_bytes;
} RTAudioRecordConfig;

/**
* @brief Create RTAudioRecord.
*/
struct RTAudioRecord *RTAudioRecord_Create(void);

/**
 * @param record is the pointer of struct RTAudioRecord.
 * @brief Release RTAudioRecord.
 */
void RTAudioRecord_Destroy(struct RTAudioRecord *record);

/**
 * @brief Init audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param config a {@link RTAudioRecordConfig} instance used to configure record information.
 * @param flags is the input flags of struct RTAudioRecord.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_NO_INIT | the init is not done.
 * AUDIO_ERR_INVALID_OPERATION | the config value is not proper.
 * AUDIO_ERR_NO_MEMORY | the memory alloc for record is fail.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_Init(struct RTAudioRecord *record, const RTAudioRecordConfig *config, uint32_t flags);

/**
 * @brief Start audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | should not start now.
 * AUDIO_ERR_DEAD_OBJECT | the ipc is dead.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_Start(struct RTAudioRecord *record);

/**
 * @brief Stop audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @since 1.0
 * @version 1.0
 */
void RTAudioRecord_Stop(struct RTAudioRecord *record);

/**
 * @brief Release audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @since 1.0
 * @version 1.0
 */
void RTAudioRecord_Destroy(struct RTAudioRecord *record);

/**
 * @brief Read audio data.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param buffer is the dst buffer of application.
 * @param size is the dst buffer data bytes.
 * @param blocking choice whether to block when read stuck, suggest:true.
 * @return size of data read
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_Read(struct RTAudioRecord *record, void *buffer, size_t size, bool blocking);

/**
 * @brief Read audio data.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param buffer is the dst buffer of application.
 * @param size is the dst buffer data bytes.
 * @param time_out_ms set time_out_ms to audio framework, if read blocks for more than time_out_ms, it will return.
 * @return size of data read, if timeout, return -ETIMEDOUT.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_ReadTimeout(struct RTAudioRecord *record, void *buffer, size_t size, uint32_t time_out_ms);

/**
 * @brief Get buffer bytes for record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @return buffer bytes suggested to read for one period
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_GetBufferSize(struct RTAudioRecord *record);

/**
 * @brief Set samplerate of record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param sample_rate the sample_rate of record.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_SetSampleRate(struct RTAudioRecord *record, uint32_t sample_rate);

/**
 * @brief Get samplerate of audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @return samplerate
 * @since 1.0
 * @version 1.0
 */
uint32_t RTAudioRecord_GetSampleRate(struct RTAudioRecord *record);

/**
 * @brief Set channel count of record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param channel_count the channel count of record.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_SetChannelCount(struct RTAudioRecord *record, uint32_t channel_count);

/**
 * @brief Get channel count of audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @return channel count of record.
 * @since 1.0
 * @version 1.0
 */
uint32_t RTAudioRecord_GetChannelCount(struct RTAudioRecord *record);

/**
 * @brief Get format of audio record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @return format
 * @since 1.0
 * @version 1.0
 */
uint32_t RTAudioRecord_GetFormat(struct RTAudioRecord *record);

/**
 * @brief Set format of record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param format supports RTAUDIO_FORMAT_PCM_16_BIT,
 * RTAUDIO_FORMAT_PCM_8_24_BIT, RTAUDIO_FORMAT_PCM_32_BIT.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_SetFormat(struct RTAudioRecord *record, uint32_t format);

/**
 * @brief Set params of record.
 *
 * @param record is the pointer of struct RTAudioRecord.
 * @param strs supports audio capture mode settings like "cap_mode=no_afe_pure_data".
 * The meaning of the string is to choose "no_afe_pure_data" mode for audio capture.
 * [amebalite] & [amebaSmart]: There are 4 modes, all take the default above amic setting for example,
 *              the data stucture is as follows:
 *              mode(no_afe_pure_data): take RTAudioRecordConfig's channel_count 3 for example:mic1+mic2+mic3;
 *                                     for lite demo borad, mic3 is ref data as the hardware design.
 *              mode(do_afe_only_out): take RTAudioRecordConfig's channel_count 1 for example:out;
 *                                     out means the mic data after denoise.
 *              mode(no_afe_all_data): take RTAudioRecordConfig's channel_count 4 for example:mic1+mic2+mic3+out;
 *                                     for lite demo borad, mic3 is ref data as the hardware design.
 *                                     out is NULL, and for the app to set.
 *              mode(do_afe_all_data): take RTAudioRecordConfig's channel_count 4 for example:mic1+mic2+mic3+out;
 *                                     for lite demo borad, mic3 is ref data as the hardware design.
 *                                     out means the mic data after denoise.
 * If default setting meets your requirements, no need to set again.
 * strs alsa supports to set the channel of reference like:"ref_channel=2"
 * The meaning of the string: it means channel2 is the reference channel.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_SetParameters(struct RTAudioRecord *audio_record, const char *strs);

/**
 * @brief Get timestamp of audio_record.
 *
 * @param audio_record is the pointer of struct RTAudioRecord.
 * @param tstamp the timestamp get from audio framework.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t    RTAudioRecord_GetTimestamp(struct RTAudioRecord *audio_record, RTAudioTimestamp *tstamp);

/**
 * @brief Get present recording PTS of record.
 *
 * @param audio_record is the pointer of struct RTAudioRecord.
 * @param now_ns the system time, or tsf time.
 * @param audio_ns the audio recording time.
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_GetPresentTime(struct RTAudioRecord *audio_record, int64_t *now_ns, int64_t *audio_ns);

/**
 * @brief Get present recording PTS of record.
 *
 * @param audio_record is the pointer of struct RTAudioRecord.
 * @param trigger_ns the time record is triggered(start or stop).
 * @return Returns a value listed below: \n
 * int32_t | Description
 * ----------------------| -----------------------
 * AUDIO_OK | the operation is successful.
 * AUDIO_ERR_INVALID_OPERATION | param not supported.
 * @since 1.0
 * @version 1.0
 */
int32_t RTAudioRecord_GetTriggerTimestamp(struct RTAudioRecord *audio_record, int64_t *trigger_ns);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_AUDIO_RECORD
