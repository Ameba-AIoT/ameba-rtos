/**
 * @file      bt_audio_track_api.h
 * @brief     Bluetooth audio track API definition.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_TRACK_API_H__
#define __BT_AUDIO_TRACK_API_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @typedef   bt_audio_fmt_t
 * @brief     Audio track and audio record PCM data bit format.
 */
typedef enum {
	BT_AUDIO_FORMAT_PCM_8_BIT         = 0x1u,    /*!< 8 bits per channel per frame. */
	BT_AUDIO_FORMAT_PCM_16_BIT        = 0x2u,    /*!< 16 bits per channel per frame. */
	BT_AUDIO_FORMAT_PCM_32_BIT        = 0x4u,    /*!< 32 bits per channel per frame. */
} bt_audio_fmt_t;

/**
 * @defgroup  bt_audio_track BT Audio Track APIs
 * @brief     BT Audio track related function APIs
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     Create an audio track.
 * @param[in] channels: Number of audio channels.
 * @param[in] rate: Sample rate in Hz.
 * @param[in] format: Audio data bit format.
 *                    Range: @ref BT_AUDIO_FORMAT_PCM_8_BIT, @ref BT_AUDIO_FORMAT_PCM_16_BIT or @ref BT_AUDIO_FORMAT_PCM_32_BIT.
 *                    Other values are handled as 16-bit PCM.
 * @param[in] period_size: Period size. Unused.
 * @param[in] period_count: Period count. Unused.
 * @param[in] duration: Duration between two continuous audio packets, in microseconds.
 *                      0: Common BT audio demo. The buffer size is derived from the minimum buffer size of the audio track.
 *                      Others: Shall be equal to the ISO duration in the LE audio demo.
 * @return
 *            - NULL: Fail
 *            - track_hdl: Success
 */
void *rtk_bt_audio_track_init(uint32_t channels,
							  uint32_t rate,
							  uint32_t format,
							  uint32_t period_size,
							  uint32_t period_count,
							  uint32_t duration);

/**
 * @brief     Pause, stop and destroy the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_deinit(void *track_hdl);

/**
 * @brief     Stop the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_stop(void *track_hdl);

/**
 * @brief     Pause the audio track and flush the buffered data.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_pause(void *track_hdl);

/**
 * @brief     Pause the audio track without flushing the buffered data.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_pause_without_flush(void *track_hdl);

/**
 * @brief     Resume the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_resume(void *track_hdl);

/**
 * @brief     Write audio data to the audio track for playback.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] buffer: Audio data to play.
 * @param[in] size: Size of the audio data in bytes.
 * @return
 *            - Number of bytes written
 */
int32_t rtk_bt_audio_track_play(void *track_hdl, void *buffer, uint32_t size);

/**
 * @brief     Set the hardware volume.
 * @param[in] left_volume: Left channel volume.
 * @param[in] right_volume: Right channel volume.
 * @return
 *            - 0: Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_audio_track_set_hardware_volume(float left_volume, float right_volume);

/**
 * @brief     Get the hardware volume.
 * @param[out] left_volume: Left channel volume.
 *                          Must not be NULL.
 * @param[out] right_volume: Right channel volume.
 *                           Must not be NULL.
 * @return
 *            - 0: Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_audio_track_get_volume(float *left_volume, float *right_volume);

/**
 * @brief     Set the software volume of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] left_volume: Left channel volume.
 * @param[in] right_volume: Right channel volume.
 * @return
 *            - 0: Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_audio_track_set_software_volume(void *track_hdl, float left_volume, float right_volume);

/**
 * @brief     Start the audio track and set its software volume.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] left_volume: Left channel software volume.
 * @param[in] right_volume: Right channel software volume.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_start(void *track_hdl, float left_volume, float right_volume);

/**
 * @brief     Enable or disable delayed start of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] ctrl: Whether to delay the start of the audio track.
 *                  - true: Enable delayed start.
 *                  - false: Disable delayed start.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_delay_start(void *track_hdl, bool ctrl);

/**
 * @brief     Start the audio hardware transmission.
 * @return
 *            - 0: Succeed
 * @note
 *            - This API shall be called when delayed start is enabled by @ref rtk_bt_audio_track_delay_start.
 */
uint16_t rtk_bt_audio_track_hw_start(void);

/**
 * @brief     Mute or unmute the audio track playback.
 * @param[in] muted: Whether to mute the playback.
 *                   - true: Mute the playback.
 *                   - false: Unmute the playback.
 */
void rtk_bt_audio_track_set_mute(bool muted);

/**
 * @brief     Get the mute state of the audio track playback.
 * @return
 *            - true: Playback is muted
 *            - false: Playback is not muted
 */
bool rtk_bt_audio_track_get_muted(void);

/**
 * @brief     Set the sample rate of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] sample_rate: Sample rate in Hz.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_set_sample_rate(void *track_hdl, uint32_t sample_rate);

/**
 * @brief     Get the sample rate of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Fail
 *            - Others: Sample rate in Hz
 */
uint32_t rtk_bt_audio_track_get_sample_rate(void *track_hdl);

/**
 * @brief     Set the channel count of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @param[in] channel_count: Number of audio channels.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_track_set_channel_count(void *track_hdl, uint32_t channel_count);

/**
 * @brief     Get the channel count of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Fail
 *            - Others: Number of audio channels
 */
uint32_t rtk_bt_audio_track_get_channel_count(void *track_hdl);

/**
 * @brief     Get the start threshold of the audio track.
 * @param[in] track_hdl: Audio track handle returned by @ref rtk_bt_audio_track_init.
 * @return
 *            - 0: Fail
 *            - Others: Start threshold in bytes
 */
int32_t rtk_bt_audio_track_get_start_thresholdbytes(void *track_hdl);

/**
 * @brief     Get the buffer size of the audio track.
 * @return
 *            - Buffer size
 * @note
 *            - Not ready.
 */
long rtk_bt_audio_track_get_buffer_size(void);

/**
 * @}
 */

#endif /* __BT_AUDIO_TRACK_API_H__ */
