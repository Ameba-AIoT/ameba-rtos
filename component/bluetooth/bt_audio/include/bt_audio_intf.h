/**
 * @file      bt_audio_intf.h
 * @brief     Bluetooth audio interface type and function definition.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_INTF_H__
#define __BT_AUDIO_INTF_H__

#include <dlist.h>
#include <basic_types.h>
#include <bt_audio_track_api.h>
#include <bt_audio_record_api.h>
#include <bt_audio_ring_buffer.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_AUDIO_TRACK_PRES_MAX_COUNT 100 /*!< Number of presentation deltas averaged per compensation decision. At 10 ms per packet this covers 1 s. */
#define RTK_BT_AUDIO_TRACK_PRES_DELTA_THRESHOLD_NS 1000000 /*!< Presentation delta threshold. Units: nanoseconds. */
#define BT_AUDIO_CLOCK_EPOCH_THRESHOLD  0x80000000  /*!< Half cycle of the 32-bit clock, used to detect the controller free run clock reset. */

/************************************** Data Types *************************************/
/**
 * @typedef   rtk_bt_audio_role_t
 * @brief     Bluetooth audio role.
 */
typedef enum {
	RTK_BT_AUDIO_ROLE_TRACK = 0x00,                     /*!< Audio track (playback). */
	RTK_BT_AUDIO_ROLE_RECORD = 0x01,                    /*!< Audio record (capture). */
} rtk_bt_audio_role_t;

/**
 * @typedef   rtk_bt_audio_pres_comp_t
 * @brief     Bluetooth audio presentation delay compensation state.
 */
typedef enum {
	RTK_BT_AUDIO_TRACK_PRES_INIT = 0x00,                /*!< Compensation is being initialized after audio start or clock overflow handling. */
	RTK_BT_AUDIO_TRACK_PRES_LOCKED = 0x01,              /*!< Compensation locked. */
} rtk_bt_audio_pres_comp_t;

/**
 * @typedef   rtk_bt_audio_track_play_state_t
 * @brief     Bluetooth audio track play state.
 */
typedef enum {
	RTK_BT_AUDIO_TRACK_PAUSED = 0x00,                   /*!< Track is paused. */
	RTK_BT_AUDIO_TRACK_PLAYING = 0x01,                  /*!< Track is playing. */
} rtk_bt_audio_track_play_state_t;

/**
 * @typedef   pcm_data_cb
 * @brief     Callback invoked with decoded PCM data before it is written to the audio track.
 * @param[in] p_pcm_buf: Decoded PCM data.
 * @param[in] p_len: Length of the PCM data in bytes.
 * @param[in] pentity: Codec entity that produced the data.
 * @param[in] track: Audio track that the data belongs to.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
typedef uint16_t (*pcm_data_cb)(void *p_pcm_buf, uint16_t p_len, void *pentity, void *track);

/**
 * @typedef   rtk_bt_audio_codec_conf_t
 * @brief     Bluetooth audio codec configuration.
 */
typedef struct {
	uint32_t                codec_index;                                        /*!< Audio codec type, as defined in @ref rtk_bt_audio_codec_t. */
	void                    *param;                                             /*!< Codec specific parameter. */
	uint32_t                param_len;                                          /*!< Length of the codec specific parameter in bytes. */
} rtk_bt_audio_codec_conf_t;

/**
 * @typedef   rtk_bt_audio_track_t
 * @brief     Bluetooth audio track context.
 */
typedef struct {
	struct list_head        list;                                                              /*!< List node linking the track into the track list of its codec type. */
	uint16_t (*pcm_data_cb)(void *p_pcm_buf, uint16_t p_len, void *pentity, void *track);      /*!< Callback that receives the decoded PCM data.
                                                                                                *   NULL: No callback is registered. */
	void                       *audio_track_hdl;                                               /*!< Handle of the underlying audio track. */
	uint32_t                   channels;                                                       /*!< Number of channels. */
	uint32_t                   channel_allocation;                                             /*!< Channel allocation of a mono or dual channel track.
                                                                                                *   1: Left channel.
                                                                                                *   2: Right channel.
                                                                                                *   3: Dual channel. */
	uint32_t                   rate;                                                           /*!< Sample rate in Hz. */
	uint32_t                   bits;                                                           /*!< Number of bits per sample. */
	uint32_t                   format;                                                         /*!< Audio sample format, as defined in @ref bt_audio_fmt_t. */
	uint16_t                   play_state;                                                     /*!< Play state, as defined in @ref rtk_bt_audio_track_play_state_t. */
	int32_t                    track_num;                                                      /*!< Number of enqueued audio data messages not yet handled. */
	bool                       audio_sync_flag;                                                /*!< Whether audio sync is enabled for the track.
                                                                                                *   - true: Audio sync is enabled.
                                                                                                *   - false: Audio sync is disabled. */
	uint8_t                    pres_comp_event;                                                /*!< Compensation state, as defined in @ref rtk_bt_audio_pres_comp_t. */
	uint32_t                   iso_interval;                                                   /*!< Iso interval for le iso. */
	uint32_t                   sdu_interval;                                                   /*!< SDU interval for LE Audio. */
	bool                       frc_cal_flag;                                                   /*!< Whether the free run clock offset has been calculated */
	int64_t                    expt_sdu_frc;                                                   /*!< Expected free run clock timestamp of the next SDU.
                                                                                                *   Units: microseconds. */
	int64_t                    frc_drift;                                                      /*!< Free run clock offset between application and lower stack core. */
	uint64_t                   controller_free_run_clock;                                      /*!< Controller free run clock. */
	uint32_t                   pres_delay_us;                                                  /*!< Presentation delay.
                                                                                                *   Units: microseconds. */
	uint32_t                   prev_ts_us;                                                     /*!< Timestamp of the previous packet, used to detect SDU loss.
                                                                                                *   Units: microseconds. */
	bool                       prev_ts_valid;                                                  /*!< Whether prev_ts_us holds a valid value. */
	uint32_t                   mixer_start_threshold;                                          /*!< Audio mixer start threshold.
                                                                                                *   Units: bytes. */
	uint32_t                   prev_sys_us;                                                    /*!< Previous DTimestamp_Get() value, used to detect overflow. */
	bool                       ref_ap_oveflow_flag;                                            /*!< Whether the CIG or BIG reference anchor point has overflowed.
                                                                                                *   - true: The reference anchor point has overflowed.
                                                                                                *   - false: The reference anchor point has not overflowed. */
	bool                       audio_mixer_conf;                                               /*!< Audio output mode of the track.
                                                                                                *   - true: Audio mixer.
                                                                                                *   - false: Passthrough. */
	uint64_t                   trans_bytes;                                                    /*!< Total audio bytes successfully written to the track. */
	uint32_t                   delta_index;                                                    /*!< Recording delta number buffered in pres_delta_sum. */
	int64_t                    pres_delta_sum;                                                 /*!< Sum of the accumulated render time deltas. */
	uint32_t                   pre_drop_cnt_left;                                              /*!< Remaining bytes to drop to speed up track rendering. */
	uint8_t                    mixer_pre_start_flag;                                           /*!< Non-zero while mixer_pre_start_timer is running. */
	void                       *mixer_pre_start_timer;                                         /*!< Timer used to pre-start the audio mixer. */
	uint32_t                   latency;                                                        /*!< Audio mixer latency.
                                                                                                *   Units: milliseconds. */
	uint8_t                    period;                                                         /*!< Period of mixer_pre_start_timer.
                                                                                                *   Units: milliseconds. */
	void                       *audio_sync_mutex;                                              /*!< Mutex protecting the audio sync state of the track. */
	bt_audio_ring_buffer_t     audio_delay_buff;                                               /*!< Ring buffer that holds PCM data for delayed playback. */
} rtk_bt_audio_track_t;

/**
 * @typedef   rtk_bt_audio_record_t
 * @brief     Bluetooth audio record context.
 */
typedef struct {
	struct list_head        list;                   /*!< List node linking the record into the record list of its codec type. */
	void                    *audio_record_hdl;      /*!< Handle of the underlying audio record. */
	uint32_t                channels;               /*!< Number of channels. */
	uint32_t                rate;                   /*!< Sample rate in Hz. */
	uint32_t                buffer_bytes;           /*!< Buffer size of one record period.
                                                     *   Units: bytes. */
	int32_t                 record_num;             /*!< Number of record data read operations currently in progress. */
} rtk_bt_audio_record_t;

/**
 * @typedef   T_AUDIO_STREAM_MSG
 * @brief     Bluetooth audio stream message handled by the audio stream task.
 */
typedef struct {
	uint32_t                type;                   /*!< Audio codec type of the stream data, as defined in @ref rtk_bt_audio_codec_t. */
	rtk_bt_audio_track_t    *track;                 /*!< Audio track that the stream data belongs to. */
	void                    *entity;                /*!< Audio codec entity used to decode the stream data. */
	uint8_t                 *data;                  /*!< Audio stream data buffer. */
	uint16_t                size;                   /*!< Length of the audio stream data.
                                                     *   Units: bytes. */
	uint32_t                ts_us;                  /*!< Timestamp of the audio stream data.
                                                     *   Units: microseconds. */
} T_AUDIO_STREAM_MSG;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_intf BT Audio Interface APIs
 * @brief     BT Audio related interface APIs.
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     Encode PCM data with a codec entity.
 *            The returned buffer shall be released with @ref rtk_bt_audio_free_encode_buffer.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] entity: Codec entity returned by @ref rtk_bt_audio_codec_add.
 *                    Must not be NULL.
 * @param[in] pdata: PCM data to encode.
 * @param[in] len: Length of the PCM data in bytes.
 * @return
 *            - Pointer to the encoded buffer.
 *            - NULL: Encoding failed.
 */
struct enc_codec_buffer *rtk_bt_audio_data_encode(uint32_t type, void *entity, int16_t *pdata, uint32_t len);

/**
 * @brief     Free an encode buffer allocated by @ref rtk_bt_audio_data_encode.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] entity: Codec entity returned by @ref rtk_bt_audio_codec_add.
 *                    Must not be NULL.
 * @param[in] pbuffer: Encode buffer to free.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_free_encode_buffer(uint32_t type, void *entity, struct enc_codec_buffer *pbuffer);

/**
 * @brief     Decode one audio frame into PCM data.
 *            This API is called automatically within the BT audio stream task after @ref rtk_bt_audio_recvd_data_in, so it shall not be called by the application.
 *            The decode buffer is also handled within the BT audio stream task and is not exposed to the upper layer.
 * @param[in]  entity: Codec entity used for decoding.
 *                     Must not be NULL.
 * @param[in]  pparam: Decode buffer that receives the decoded PCM data.
 *                     Must not be NULL.
 * @param[in]  data: Encoded audio frame.
 * @param[in]  length: Length of the encoded audio frame in bytes.
 * @param[out] ppcm_data_size: Length of the decoded PCM data in bytes.
 * @param[out] paudio_param: Audio parameters reported by the decoder.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_decode_data(void *entity, void *pparam, uint8_t *data, uint32_t length, uint32_t *ppcm_data_size, void *paudio_param);

/**
 * @brief     Read recorded PCM data from an audio record created by @ref rtk_bt_audio_record_add.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] record: Audio record handle.
 *                    Must not be NULL.
 * @param[in] entity: Codec entity associated with the record.
 *                    Must not be NULL.
 * @param[in] buffer: Destination buffer of the application.
 *                    Must not be NULL.
 * @param[in] size: Size of the destination buffer in bytes.
 * @param[in] blocking: Whether to block when no record data is available.
 *                      - true: Block until data is read.
 *                      - false: Return without waiting for data.
 * @return
 *            - Number of bytes read.
 *            - 0: Invalid parameter or the record is not available.
 */
int rtk_bt_audio_record_data_get(uint32_t type, rtk_bt_audio_record_t *record, void *entity, void *buffer, int size, bool blocking);

/**
 * @brief     Add one audio track.
 *            With the audio mixer multiple tracks are supported; with passthrough only one track is allowed.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] left_volume: Initial volume of the left channel.
 * @param[in] right_volume: Initial volume of the right channel.
 * @param[in] channels: Number of channels.
 * @param[in] rate: Sample rate in Hz.
 * @param[in] format: Audio sample format, as defined in @ref bt_audio_fmt_t.
 * @param[in] duration: Duration between two continuous audio packets.
 *                      Units: microseconds.
 *                      0: Common BT audio demo.
 *                      ISO duration: LE Audio demo.
 * @param[in] cb: Callback that receives the decoded PCM data.
 *                NULL: No callback is registered.
 * @param[in] play_flag: Whether the track plays the audio data.
 *                       - true: Create the audio track and play the audio data.
 *                       - false: Do not create the audio track; data is only delivered through the callback.
 * @return
 *            - Track handle.
 *            - NULL: Failed to add the track.
 * @note
 *            - BT audio shall be initialized by @ref rtk_bt_audio_init first; otherwise this API returns NULL.
 */
rtk_bt_audio_track_t *rtk_bt_audio_track_add(uint32_t type, float left_volume, float right_volume, uint32_t channels, uint32_t rate, uint32_t format,
											 uint32_t duration, pcm_data_cb cb, bool play_flag);

/**
 * @brief     Set the play state of an audio track.
 * @param[in] ptrack: Audio track handle.
 *                    Must not be NULL.
 * @param[in] play_state: Play state to set, as defined in @ref rtk_bt_audio_track_play_state_t.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_track_set_play_state(rtk_bt_audio_track_t *ptrack, uint16_t play_state);

/**
 * @brief     Get the play state of an audio track.
 * @param[in] ptrack: Audio track handle.
 *                    Must not be NULL.
 * @return
 *            - @ref RTK_BT_AUDIO_TRACK_PAUSED
 *            - @ref RTK_BT_AUDIO_TRACK_PLAYING
 *            - @ref RTK_BT_AUDIO_GET_VALUE_FAIL
 */
uint16_t rtk_bt_audio_track_get_play_state(rtk_bt_audio_track_t *ptrack);

/**
 * @brief     Enable audio play sync mode for an audio track.
 * @param[in] ptrack: Audio track handle.
 *                    Must not be NULL.
 * @param[in] pd: Presentation delay.
 *                Units: microseconds.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - This API shall be called immediately after @ref rtk_bt_audio_track_add, if audio sync is used.
 */
uint16_t rtk_bt_audio_track_enable_sync_mode(rtk_bt_audio_track_t *ptrack, uint32_t pd);

/**
 * @brief     Restart the audio sync of an audio track.
 *            The presentation delay compensation returns to its initial state and the free run clock offset is recalculated on the next packet.
 * @param[in] track: Audio track handle.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_track_sync_restart(rtk_bt_audio_track_t *track);

/**
 * @brief     Get the CIG or BIG reference anchor point of an ISO channel for audio sync.
 * @param[in]  track: Audio track handle.
 *                    Must not be NULL.
 * @param[in]  iso_conn_handle: Connection handle of the ISO.
 * @param[in]  dir: Direction of the ISO path.
 * @param[in]  iso_interval: ISO interval.
 *                           Units: microseconds.
 * @param[out] sync_ref_ap: Reference anchor point of the CIG or BIG.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - If RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT is not enabled, this API fails with @ref RTK_BT_AUDIO_FAIL.
 */
uint16_t rtk_bt_audio_get_iso_ref_ap(rtk_bt_audio_track_t *track, uint16_t iso_conn_handle, uint8_t dir, uint16_t iso_interval,
									 uint32_t *sync_ref_ap);

/**
 * @brief     Configure the parameters used when audio records are added.
 * @param[in] p_table: Audio record configuration table.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - This API shall be called before @ref rtk_bt_audio_record_add.
 */
uint16_t rtk_bt_audio_record_config(rtk_bt_audio_record_config_table_t *p_table);

/**
 * @brief     Add one audio record.
 *            With the audio mixer multiple records are supported; with passthrough only one record is allowed.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] channels: Number of channels.
 * @param[in] rate: Sample rate in Hz.
 * @param[in] buffer_bytes: Buffer size of one record period in bytes.
 * @param[in] volume: Capture volume of the record.
 *                    Maximum: 0xAF.
 * @return
 *            - Record handle.
 *            - NULL: Failed to add the record.
 * @note
 *            - BT audio shall be initialized by @ref rtk_bt_audio_init first; otherwise this API returns NULL.
 */
rtk_bt_audio_record_t *rtk_bt_audio_record_add(uint32_t type, uint32_t channels, uint32_t rate, uint32_t buffer_bytes, uint32_t volume);

/**
 * @brief     Delete one audio track.
 *            This API waits until all audio data of the track that is already enqueued has been handled.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] ptrack: Audio track handle.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_track_del(uint32_t type, rtk_bt_audio_track_t *ptrack);

/**
 * @brief     Delete all audio tracks.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_track_del_all(void);

/**
 * @brief     Delete one audio record.
 *            This API waits until all pending read operations of the record have finished.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] precord: Audio record handle.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_record_del(uint32_t type, rtk_bt_audio_record_t *precord);

/**
 * @brief     Delete all audio records.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_record_del_all(void);

/**
 * @brief     Add one audio codec entity.
 *            The number of entities of each codec type is limited.
 * @param[in] paudio_codec_conf: Audio codec configuration.
 *                               Must not be NULL.
 * @return
 *            - Codec entity.
 *            - NULL: Failed to add the codec entity.
 * @note
 *            - BT audio shall be initialized by @ref rtk_bt_audio_init first; otherwise this API returns NULL.
 */
void *rtk_bt_audio_codec_add(rtk_bt_audio_codec_conf_t *paudio_codec_conf);

/**
 * @brief     Update the configuration of an audio codec entity.
 * @param[in] paudio_codec_conf: Audio codec configuration.
 *                               Must not be NULL.
 * @param[in] pentity: Codec entity to update.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_codec_update(rtk_bt_audio_codec_conf_t *paudio_codec_conf, void *pentity);

/**
 * @brief     Reset an audio codec entity.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] pentity: Codec entity to reset.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_codec_reset(uint32_t type, void *pentity);

/**
 * @brief     Remove one audio codec entity.
 *            This API waits until the entity is no longer used by any encode, decode, track or record operation.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] pentity: Codec entity to remove.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_codec_remove(uint32_t type, void *pentity);

/**
 * @brief     Remove all audio codec entities.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t rtk_bt_audio_codec_remove_all(void);

/**
 * @brief     Enqueue received audio data for decoding and playback.
 *            The data is copied and then handled within the BT audio stream task.
 * @param[in] type: Audio codec type, as defined in @ref rtk_bt_audio_codec_t.
 * @param[in] track: Audio track handle.
 *                   Must not be NULL.
 * @param[in] entity: Codec entity used to decode the data.
 *                    Must not be NULL.
 * @param[in] pdata: Received audio data.
 * @param[in] len: Length of the audio data in bytes.
 * @param[in] ts_us: Timestamp of the audio data for audio sync.
 *                   Units: microseconds.
 *                   Ignored when audio sync is not enabled for the track.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - If the audio stream queue is full, this API blocks until space is available.
 */
uint16_t rtk_bt_audio_recvd_data_in(uint32_t type, rtk_bt_audio_track_t *track, void *entity, uint8_t *pdata, uint32_t len, uint32_t ts_us);

/**
 * @brief     Initialize the BT audio component.
 *            The audio stream task, the audio sync and the internal lists are created.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - If BT audio is already initialized, this API returns @ref RTK_BT_AUDIO_OK.
 */
uint16_t rtk_bt_audio_init(void);

/**
 * @brief     Deinitialize the BT audio component.
 *            All audio tracks, audio records and codec entities are deleted, then the audio stream task and the audio sync are stopped.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - If BT audio is not initialized, this API fails with @ref RTK_BT_AUDIO_FAIL.
 */
uint16_t rtk_bt_audio_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_INTF_H__ */
