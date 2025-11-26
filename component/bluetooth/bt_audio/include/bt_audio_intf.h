/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
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

#define RTK_BT_AUDIO_TRACK_PRES_MAX_COUNT 100 /* 10ms duration for 1s */
#define RTK_BT_AUDIO_TRACK_PRES_DELTA_THRESHOLD_NS 1000000

/**
 * @typedef   rtk_bt_audio_role_t
 * @brief     audio role
 */
typedef enum {
	RTK_BT_AUDIO_ROLE_TRACK = 0x00,                     /*!< Audio Track */
	RTK_BT_AUDIO_ROLE_RECORD = 0x01,                    /*!< Audio Record */
} rtk_bt_audio_role_t;

/**
 * @typedef   rtk_bt_audio_pres_comp_t
 * @brief     presentation compensation status
 */
typedef enum {
	RTK_BT_AUDIO_TRACK_PRES_INIT = 0x00,                /*!< Audio hal start or overflow handling(wait two count buffers before invoking rtk_bt_audio_track_hw_start()) */
	RTK_BT_AUDIO_TRACK_PRES_LOCKED = 0x01,              /*!< Compensation locked */
} rtk_bt_audio_pres_comp_t;

/**
 * @typedef   rtk_bt_audio_pcm_cb_t
 * @brief     bt audio pcm data call back handle
 * @param[in] p_pcm_buf: pointer of pcm data
 * @param[in] p_len: data length
 */
typedef uint16_t (*pcm_data_cb)(void *p_pcm_buf, uint16_t p_len, void *pentity, void *track);

/**
 * @typedef rtk_bt_audio_codec_conf_t
 * @brief   structure of default bt audio configuration
 */
typedef struct {
	uint32_t                codec_index;                                        /*!< audio codec profile support @ref rtk_bt_audio_codec_t */
	void                    *param;                                             /*!< audio parameter */
	uint32_t                param_len;                                          /*!< param length */
} rtk_bt_audio_codec_conf_t;

/**
 * @typedef rtk_bt_audio_track_t
 * @brief   structure of audio track configuration
 */
typedef struct {
	struct list_head        list;                                                              /*!< list head */
	uint16_t (*pcm_data_cb)(void *p_pcm_buf, uint16_t p_len, void *pentity, void *track);      /*!< callback pcm data to APP */
	void                       *audio_track_hdl;                                               /*!< RTAudioTrack pointer */
	uint32_t                   channels;                                                       /*!< indicate channels */
	uint32_t                   channel_allocation;                                             /*!< indicate mono or dual mode
                                                                                                    (1 left channel, 2 right channel, 3 dual mode) */
	uint32_t                   rate;                                                           /*!< sample rate */
	uint32_t                   bits;                                                           /*!< bit per sample */
	uint32_t                   format;                                                         /*!< audio format */
	int32_t                    track_num;                                                      /*!< indicate enqueued audio data numbers */
	bool                       audio_sync_flag;                                                /*!< indicate whether audio sync is need */
	uint8_t                    pres_comp_event;                                                /*!< indicate presentation delay compensation events */
	uint32_t                   iso_interval;                                                   /*!< iso_interval */
	uint32_t                   sdu_interval;                                                   /*!< record sdu interval(micro second) */
	bool
	frc_cal_flag;                                                   /*!< indicate whether free run clock calculation is done previously */
	bool                       ts_oveflow_flag;                                                /*!< indicate whether timestamp is overflow  */
	int64_t                    expt_sdu_frc;                                                   /*!< calculate the expected sdu time stamp */
	int64_t
	frc_drift;                                                      /*!< save free run clock drift between app core and lower stack core */
	uint64_t                   controller_free_run_clock;                                      /*!< save free controller_free_run_clock */
	uint32_t                   pres_delay_us;                                                  /*!< indicate presentation delay value */
	uint32_t                   prev_ts_us;                                                     /*!< recording previous timestamp to check sdu loss */
	uint32_t
	prev_sys_us;                                                    /*!< recording previous timestamp DTimestamp_Get() to check overflow */
	bool                       ref_ap_oveflow_flag;
	uint64_t                   trans_bytes;                                                    /*!< to record successfully transmitted audio bytes */
	uint32_t                   delta_index;                                                    /*!< to record delta number buffered in pres_delta_sum */
	int64_t                    pres_delta_sum;                                                 /*!< record sum of render data time delta */
	uint32_t                   pre_drop_cnt_left;                                              /*!< indicate left drop packets number */
	uint8_t                    audio_hal_buff_count;                                           /*!< count audio hal buffer numbers */
	void                       *audio_delay_start_timer;                                       /*!< delay start timer */
	void                       *audio_sync_mutex;                                              /*!< audio sync mutex */
	bt_audio_ring_buffer_t     audio_delay_buff;                                               /*!< rtk_bt_audio_delay_start_t*/
} rtk_bt_audio_track_t;

/**
 * @typedef rtk_bt_audio_record_t
 * @brief   structure of audio record configuration
 */
typedef struct {
	struct list_head        list;                   /*!< list head */
	void                    *audio_record_hdl;      /*!< RTAudioRecord pointer */
	uint32_t                channels;               /*!< indicate channels */
	uint32_t                rate;                   /*!< sample rate */
	uint32_t                buffer_bytes;           /*!< buffer bytes per one period of record */
	int32_t                 record_num;             /*!< indicate aptured audio data numbers */
} rtk_bt_audio_record_t;

/**  @brief audio stream message definition */
typedef struct {
	uint32_t                type;                   /*!< indicate application type */
	rtk_bt_audio_track_t    *track;                 /*!< indicate audio track */
	// rtk_bt_audio_record_t    *record;                /*!< indicate audio record */
	void                    *entity;                /*!< indicate audio codec entity */
	uint8_t                 *data;                  /*!< indicate audio stream buffer */
	uint16_t                size;                   /*!< indicate audio stream length */
	uint32_t                ts_us;                  /*!< indicate audio stream timestamp microseconds */
} T_AUDIO_STREAM_MSG;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_group BT Audio APIs
 * @brief     List all BT Audio related APIs
 * @ingroup   BT_APIs
 */

/**
 * @defgroup  bt_audio_intf BT Audio Interface APIs
 * @brief     BT Audio related interface APIs
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     encode pcm data
 * @param[in] type: audio data codec type
 * @param[in] entity: pointer of codec entity
 * @param[in] pdata: pointer of audio data
 * @param[in] len: data length(In Bytes)
 * @return    encoded struct buffer
 */
struct enc_codec_buffer *rtk_bt_audio_data_encode(uint32_t type, void *entity, int16_t *pdata, uint32_t len);

/**
 * @brief     free encode buffer (encode buffer allocate is within rtk_bt_audio_data_encode())
 * @param[in] type: audio data codec type
 * @param[in] entity: pointer of codec entity
 * @param[in] pbuffer: encoded struct buffer
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_free_encode_buffer(uint32_t type, void *entity, struct enc_codec_buffer *pbuffer);

/**
 * @brief     decode data
 *            This api will be invoked after rtk_bt_audio_recvd_data_in() within bt audio component’s thread automatically, so it shouldn’t be invoked by application.
 *            The decode struct buffer will also be invoked within bt audio component’s thread, so which hasn’t be open to upper layer
 * @param[in] entity: pointer of codec entity
 * @param[in] pparam: pointer of param
 * @param[in] data: data
 * @param[in] length: data length
 * @param[in] ppcm_data_size: decoded return pacm data length
 * @param[in] paudio_param: audio parameter
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_decode_data(void *entity, void *pparam, uint8_t *data, uint32_t length, uint32_t *ppcm_data_size, void *paudio_param);

/**
 * @brief     read record data after rtk_bt_audio_record_add(RTK_BT_AUDIO_ROLE_RECORD)
 * @param[in] type: audio data codec type
 * @param[in] record: audio record handle
 * @param[in] entity: pointer of codec entity
 * @param[in] buffer is the dst buffer of application.
 * @param[in] size is the dst buffer data bytes.
 * @param[in] blocking choice whether to block when read stuck, suggest:true.
 * @return    none
 */
int rtk_bt_audio_record_data_get(uint32_t type, rtk_bt_audio_record_t *record, void *entity, void *buffer, int size, bool blocking);

/**
 * @brief     add one audio track(Audio mix support multiple stream/ Passthrough only one)
 * @param[in] type: audio data codec type
 * @param[in] left_volume: left init volume
 * @param[in] right_volume: right init volume
 * @param[in] channels: channels
 * @param[in] rate: sample rate
 * @param[in] format: audio format(ref bt_audio_fmt_t)
 * @param[in] duration: define duration time between two continuous audio packets
 *                      this number should be 0 for common bt audio demo
 *                      this number should be equal to ISO duration value(micro seconds) in le audio demo
 * @param[in] cb: data directly calling back function (no need to play)
 * @param[in] play_flag: indicate whether this track need to be played
 * @return
 *            - NULL  : Fail
 *            - others: Track Handle
 */
rtk_bt_audio_track_t *rtk_bt_audio_track_add(uint32_t type, float left_volume, float right_volume, uint32_t channels, uint32_t rate, uint32_t format,
											 uint32_t duration, pcm_data_cb cb, bool play_flag);

/**
* @brief     enable audio play sync mode for specific track(This api should be invoked immediately after rtk_bt_audio_track_add, if used)
* @param[in] ptrack: audio data codec type
* @param[in] pd: left init volume
* @return
*            - NULL  : Fail
*            - others: Track Handle
*/
uint16_t rtk_bt_audio_track_enable_sync_mode(rtk_bt_audio_track_t *ptrack, uint32_t pd);

/**
 * @brief     config bt audio record parameter
 * @param[in] p_table: configuration table
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_record_config(rtk_bt_audio_record_config_table_t *p_table);

/**
 * @brief     add one audio record(Audio mix support multiple stream/ Passthrough only one)
 * @param[in] type: audio data codec type
 * @param[in] channels: channels
 * @param[in] rate: sample rate
 * @param[in] buffer_bytes: buffer bytes per one period of record
 * @param[in] volume: set the record get volume max 0xaf
 * @return
 *            - NULL  : Fail
 *            - others: Record Handle
 */
rtk_bt_audio_record_t *rtk_bt_audio_record_add(uint32_t type, uint32_t channels, uint32_t rate, uint32_t buffer_bytes, uint32_t volume);

/**
 * @brief     delete one audio track
 * @param[in] type: audio data codec type
 * @param[in] ptrack: track handle
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_track_del(uint32_t type, rtk_bt_audio_track_t *ptrack);

/**
 * @brief     delete all audio track
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_track_del_all(void);

/**
 * @brief     delete one audio record
 * @param[in] type: audio data codec type
 * @param[in] precord: record handle
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_record_del(uint32_t type, rtk_bt_audio_record_t *precord);

/**
 * @brief     delete all audio record
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_record_del_all(void);

/**
 * @brief     add one audio codec
 * @param[in] paudio_codec_conf: audio codec configuration
 * @return
 *            - NULL  : Fail
 *            - others: Codec entity
 */
void *rtk_bt_audio_codec_add(rtk_bt_audio_codec_conf_t *paudio_codec_conf);

/**
 * @brief     update audio codec
 * @param[in] paudio_codec_conf: audio codec configuration
 * @param[in] pentity: audio codec entity
 * @return
 *            - NULL  : Fail
 *            - others: Codec entity
 */
uint16_t rtk_bt_audio_codec_update(rtk_bt_audio_codec_conf_t *paudio_codec_conf, void *pentity);

/**
 * @brief     remove one audio codec
 * @param[in] type: audio data codec type
 * @param[in] pentity: audio codec entity
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_codec_remove(uint32_t type, void *pentity);

/**
 * @brief     remove all audio codec
 * @return
 *            - 1  : Fail
 *            - 0: Success
 */
uint16_t rtk_bt_audio_codec_remove_all(void);

/**
 * @brief     enqueue received audio data
 * @param[in] type: audio data codec type (refer to rtk_bt_common.h rtk_bt_audio_codec_t)
 * @param[in] track: track handle
 * @param[in] entity: entity handle
 * @param[in] pdata: pointer of audio data
 * @param[in] len: data length
 * @param[in] ts_us:time stamp for audio sync
 * @return
 *            - 0  : Succeed
 *            - 0xEF: BT Audio Framework buffer is not enough(AUDIO_STREAM_MSG_QUEUE_SIZE)
 *            - others: Error code
 */
uint16_t rtk_bt_audio_recvd_data_in(uint32_t type, rtk_bt_audio_track_t *track, void *entity, uint8_t *pdata, uint32_t len, uint32_t ts_us);

/**
 * @brief     Initializes bt audio component internal resources
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_init(void);

/**
 * @brief     Deinit bt audio component
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_INTF_H__ */
