/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef __BT_AUDIO_TRACK_API_H__
#define __BT_AUDIO_TRACK_API_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @typedef   bt_audio_fmt_t
 * @brief     audio role
 */
typedef enum {
	/** audio track and audio record data bit format, 8bit per channel per frame*/
	BT_AUDIO_FORMAT_PCM_8_BIT         = 0x1u,
	/** audio track and audio record data bit format, 16bit per channel per frame*/
	BT_AUDIO_FORMAT_PCM_16_BIT        = 0x2u,
	/** audio track and audio record data bit format, 32bit per channel per frame*/
	BT_AUDIO_FORMAT_PCM_32_BIT        = 0x4u,
	// /** audio track and audio record data bit format, float per channel per frame*/
	// BT_AUDIO_FORMAT_PCM_FLOAT         = 0x5u,
	// /** audio track and audio record data bit format, 24bit packed per channel per frame*/
	// BT_AUDIO_FORMAT_PCM_24_BIT = 0x6u,
	// /** audio record and audio record data bit format, 24+8bit per channel per frame*/
	// BT_AUDIO_FORMAT_PCM_8_24_BIT      = 0x7u,
} bt_audio_fmt_t;

/**
 * @defgroup  bt_audio_track BT Audio Track APIs
 * @brief     BT Audio track related function APIs
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief                       bt audio init
 * @param[in]                   channels: audio channel
 * @param[in]                   rate: audio rate
 * @param[in]                   format: audio format(ref bt_audio_fmt_t)
 * @param[in]                   period_size: period size
 * @param[in]                   period_count: period count
 * @param[in]                   duration: define duration time between two continuous audio packets
 *                                        this number should be 0 for common bt audio demo
 *                                        this number should be equal to ISO duration value(micro seconds) in le audio demo
 * @return
 *                              - NULL  : Fail
 *                              - track_hdl: Success
 */
void *rtk_bt_audio_track_init(uint32_t channels,
							  uint32_t rate,
							  uint32_t format,
							  uint32_t period_size,
							  uint32_t period_count,
							  uint32_t duration);

/**
 * @brief     bt audio track deinit
.* @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_deinit(void *track_hdl);

/**
 * @brief     bt audio track stop
 * @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_stop(void *track_hdl);

/**
 * @brief     bt audio track pause
 * @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_pause(void *track_hdl);

/**
 * @brief     bt audio track resume
 * @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_resume(void *track_hdl);

/**
 * @brief     bt audio track playback
 * @param[in] track_hdl: audio track handle
 * @param[in] buffer: pointer of audio data
 * @param[in] size: size of audio data
 * @return successfully written bytes
 */
int32_t rtk_bt_audio_track_play(void *track_hdl, void *buffer, uint16_t size);

/**
 * @brief     set hardware volume
 * @param[in] left_volume: left audio volume
 * @param[in] right_volume: right audio volume
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_track_set_hardware_volume(float left_volume, float right_volume);

/**
 * @brief     get audio track volume
 * @param[in] left_volume: left audio volume which cannot be a null pointer
 * @param[in] right_volume: right audio volume which cannot be a null pointer
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_track_get_volume(float *left_volume, float *right_volume);

/**
 * @brief     set sotfware volume of audio track
 * @param[in] track_hdl: audio track handle
 * @param[in] left_volume: left audio volume
 * @param[in] right_volume: right audio volume
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_track_set_software_volume(void *track_hdl, float left_volume, float right_volume);

/**
 * @brief     audio track start
 * @param[in] track_hdl: audio track handle
 * @param[in] left_volume: left volume value
 * @param[in] right_volume: right volume value
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_start(void *track_hdl, float left_volume, float right_volume);

/**
 * @brief     audio track delay start
 * @param[in] track_hdl: audio track handle
 * @param[in] ctrl: true, enable delay start
 *                  false, disable delay start
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_delay_start(void *track_hdl, bool ctrl);

/**
 * @brief     audio track hw start
 * @param[in] none
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_hw_start(void);

/**
 * @brief     audio track configure mute(Not ready)
 * @param[in] muted: mute flag, true means mute, false means unmute.
 * @return    none
 */
void rtk_bt_audio_track_set_mute(bool muted);

/**
 * @brief     audio track get mute, true means mute, false means unmute.
 * @return    mute status
 */
bool rtk_bt_audio_track_get_muted(void);

/**
 * @brief     audio track set sample rate
 * @param[in] track_hdl: audio track handle
 * @param[in] sample_rate: sample rate
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_set_sample_rate(void *track_hdl, uint32_t sample_rate);

/**
 * @brief     audio track get sample rate
 * @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Fail
 *                              - Others: Samplerate
 */
uint32_t rtk_bt_audio_track_get_sample_rate(void *track_hdl);

/**
 * @brief     audio track set channel count
 * @param[in] track_hdl: audio track handle
 * @param[in] channel_count: channel count
 * @return
 *                              - 0  : Succeed
 *                              - 1: Fail
 */
uint16_t rtk_bt_audio_track_set_channel_count(void *track_hdl, uint32_t channel_count);

/**
 * @brief     audio track get channel count
 * @param[in] track_hdl: audio track handle
 * @return
 *                              - 0  : Fail
 *                              - Others: Channel count
 */
uint32_t rtk_bt_audio_track_get_channel_count(void *track_hdl);

/**
 * @brief     audio track get buffer size(Not ready)
 * @return    buffer size
 */
long rtk_bt_audio_track_get_buffer_size(void);

/**
 * @}
 */

#endif // __BT_AUDIO_TRACK_API_H__
