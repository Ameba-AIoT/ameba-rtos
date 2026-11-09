/**
 * @file      bt_audio_record_api.h
 * @brief     Bluetooth audio record API definition.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_RECORD_API_H__
#define __BT_AUDIO_RECORD_API_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define RTK_BT_AUDIO_MICBST_GAIN_0DB        ((u32)0x00000000)    /*!< Microphone boost gain of 0 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_5DB        ((u32)0x00000001)    /*!< Microphone boost gain of 5 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_10DB       ((u32)0x00000002)    /*!< Microphone boost gain of 10 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_15DB       ((u32)0x00000003)    /*!< Microphone boost gain of 15 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_20DB       ((u32)0x00000004)    /*!< Microphone boost gain of 20 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_25DB       ((u32)0x00000005)    /*!< Microphone boost gain of 25 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_30DB       ((u32)0x00000006)    /*!< Microphone boost gain of 30 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_35DB       ((u32)0x00000007)    /*!< Microphone boost gain of 35 dB. */
#define RTK_BT_AUDIO_MICBST_GAIN_40DB       ((u32)0x00000008)    /*!< Microphone boost gain of 40 dB. */

/**
 * @typedef   rtk_bt_audio_mic_t
 * @brief     Microphone category.
 */
typedef enum {
	/* [amebalite] support AMIC1-AMIC3, DMIC1-DMIC4
	 * [amebasmart] support AMIC1-AMIC5, DMIC1-DMIC8
	 */
	RTK_BT_AUDIO_AMIC1         = 0,     /*!< Analog microphone 1. */
	RTK_BT_AUDIO_AMIC2         = 1,     /*!< Analog microphone 2. */
	RTK_BT_AUDIO_AMIC3         = 2,     /*!< Analog microphone 3. */
	RTK_BT_AUDIO_AMIC4         = 3,     /*!< Analog microphone 4. */
	RTK_BT_AUDIO_AMIC5         = 4,     /*!< Analog microphone 5. */
	RTK_BT_AUDIO_DMIC1         = 5,     /*!< Digital microphone 1. */
	RTK_BT_AUDIO_DMIC2         = 6,     /*!< Digital microphone 2. */
	RTK_BT_AUDIO_DMIC3         = 7,     /*!< Digital microphone 3. */
	RTK_BT_AUDIO_DMIC4         = 8,     /*!< Digital microphone 4. */
	RTK_BT_AUDIO_DMIC5         = 9,     /*!< Digital microphone 5. */
	RTK_BT_AUDIO_DMIC6         = 10,    /*!< Digital microphone 6. */
	RTK_BT_AUDIO_DMIC7         = 11,    /*!< Digital microphone 7. */
	RTK_BT_AUDIO_DMIC8         = 12,    /*!< Digital microphone 8. */
	RTK_BT_AUDIO_MIC_MAX_NUM   = 13,    /*!< Number of microphone categories. */
} rtk_bt_audio_mic_t;

/**
 * @typedef   rtk_bt_audio_mic_type_t
 * @brief     Microphone type.
 */
typedef enum {
	RTK_BT_AUDIO_AMIC,    /*!< Analog microphone. */
	RTK_BT_AUDIO_DMIC,    /*!< Digital microphone. */
} rtk_bt_audio_mic_type_t;

/**
 * @struct    rtk_bt_audio_record_config_t
 * @brief     Record configuration of one microphone channel.
 */
typedef struct {
	uint8_t mic_channel_index;       /*!< Index of the record channel the microphone is assigned to. */
	rtk_bt_audio_mic_t mic_category; /*!< Microphone assigned to the record channel. */
	uint32_t gain;                   /*!< Microphone boost gain.
	                                  *   Range: @ref RTK_BT_AUDIO_MICBST_GAIN_0DB to @ref RTK_BT_AUDIO_MICBST_GAIN_40DB. */
} rtk_bt_audio_record_config_t;

/**
 * @struct    rtk_bt_audio_record_config_table_t
 * @brief     Record configuration table of all microphone channels.
 */
typedef struct {
	const char *strs;                                                  /*!< Record parameter string. */
	rtk_bt_audio_mic_type_t mic_type;                                  /*!< Microphone type used for recording. */
	uint8_t record_nums;                                               /*!< Number of valid entries in config. */
	rtk_bt_audio_record_config_t config[RTK_BT_AUDIO_MIC_MAX_NUM + 1]; /*!< Record configuration of each channel. */
} rtk_bt_audio_record_config_table_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_record BT Audio Record APIs
 * @brief     BT Audio record related function APIs
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     Create an audio record with 16-bit PCM format and start recording.
 * @param[in] channels: Number of record channels.
 * @param[in] rate: Sample rate in Hz.
 * @param[in] buffer_bytes: Buffer size in bytes per record period.
 * @param[in] mic_type: Microphone type.
 * @return
 *            - NULL: Fail
 *            - record_hdl: Success
 */
void *rtk_bt_audio_record_init(uint32_t channels, uint32_t rate, uint32_t buffer_bytes, rtk_bt_audio_mic_type_t mic_type);

/**
 * @brief     Stop and destroy the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_deinit(void *record_hdl);

/**
 * @brief     Stop the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_stop(void *record_hdl);

/**
 * @brief     Start the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_start(void *record_hdl);

/**
 * @brief     Set the sample rate of the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @param[in] sample_rate: Sample rate in Hz.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_set_sample_rate(void *record_hdl, uint32_t sample_rate);

/**
 * @brief     Get the sample rate of the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @return
 *            - 0: Fail
 *            - Others: Sample rate in Hz
 */
uint32_t rtk_bt_audio_record_get_sample_rate(void *record_hdl);

/**
 * @brief     Set the channel count of the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @param[in] channel_count: Number of record channels.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_set_channel_count(void *record_hdl, uint32_t channel_count);

/**
 * @brief     Get the channel count of the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @return
 *            - 0: Fail
 *            - Others: Number of record channels
 */
uint32_t rtk_bt_audio_record_get_channel_count(void *record_hdl);

/**
 * @brief     Set the parameters of the audio record.
 *            Audio capture mode and reference channel are selected through the parameter string.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @param[in] strs: Parameter string.
 *                  Capture mode, for example "cap_mode=no_afe_pure_data" selects the "no_afe_pure_data" mode.
 *                  [amebalite] and [amebasmart] support 4 modes. The examples below use channel_count of AudioRecordConfig with the default AMIC setting.
 *                  - no_afe_pure_data: channel_count 3 gives mic1 + mic2 + mic3. On the lite demo board, mic3 is the reference data by hardware design.
 *                  - do_afe_only_out: channel_count 1 gives out. The out channel is the mic data after denoise.
 *                  - no_afe_all_data: channel_count 4 gives mic1 + mic2 + mic3 + out. On the lite demo board, mic3 is the reference data by hardware design.
 *                    The out channel is NULL and is left for the application to fill.
 *                  - do_afe_all_data: channel_count 4 gives mic1 + mic2 + mic3 + out. On the lite demo board, mic3 is the reference data by hardware design.
 *                    The out channel is the mic data after denoise.
 *                  If the default setting meets the requirement, there is no need to set it again.
 *                  Reference channel, for example "ref_channel=2" selects channel 2 as the reference channel.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 */
uint16_t rtk_bt_audio_record_set_parameters(void *record_hdl, const char *strs);

/**
 * @brief     Set the capture volume of the record channels.
 * @param[in] channels: Total number of channels to set the volume for, also the channels to capture.
 * @param[in] volume: Capture volume.
 *                    Range: 0x00 to 0xAF.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 * @note
 *            - This API shall be called after @ref rtk_bt_audio_record_init.
 */
uint16_t rtk_bt_audio_record_set_capture_volume(uint32_t channels, uint32_t volume);

/**
 * @brief     Set the microphone category of a record channel.
 * @param[in] channel_num: Record channel number to set the microphone category for.
 * @param[in] mic_category: Microphone category.
 *                          Range: @ref RTK_BT_AUDIO_AMIC1 to @ref RTK_BT_AUDIO_DMIC8.
 *                          [amebalite] supports AMIC1-AMIC3, DMIC1-DMIC4.
 *                          [amebasmart] supports AMIC1-AMIC5, DMIC1-DMIC8.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 * @note
 *            - This API shall be called after @ref rtk_bt_audio_record_init.
 */
uint16_t rtk_bt_audio_record_set_channel_mic_category(uint32_t channel_num, uint32_t mic_category);

/**
 * @brief     Set the microphone boost gain.
 * @param[in] mic_category: Microphone category.
 *                          Range: @ref RTK_BT_AUDIO_AMIC1 to @ref RTK_BT_AUDIO_DMIC8.
 *                          [amebalite] supports AMIC1-AMIC3, DMIC1-DMIC4.
 *                          [amebasmart] supports AMIC1-AMIC5, DMIC1-DMIC8.
 * @param[in] gain: Microphone boost gain.
 *                  Range: @ref RTK_BT_AUDIO_MICBST_GAIN_0DB to @ref RTK_BT_AUDIO_MICBST_GAIN_40DB.
 * @return
 *            - 0: Succeed
 *            - 1: Fail
 * @note
 *            - This API shall be called after @ref rtk_bt_audio_record_init.
 */
uint16_t rtk_bt_audio_record_set_mic_bst_gain(uint32_t mic_category, uint32_t gain);

/**
 * @brief     Get the microphone gain.
 * @param[in] mic_category: Microphone category.
 *                          Range: @ref RTK_BT_AUDIO_AMIC1 to @ref RTK_BT_AUDIO_DMIC8.
 *                          [amebalite] supports AMIC1-AMIC3, DMIC1-DMIC4.
 *                          [amebasmart] supports AMIC1-AMIC5, DMIC1-DMIC8.
 * @return
 *            - Microphone gain, in the range @ref RTK_BT_AUDIO_MICBST_GAIN_0DB to @ref RTK_BT_AUDIO_MICBST_GAIN_40DB.
 * @note
 *            - This API shall be called after @ref rtk_bt_audio_record_init.
 */
int32_t rtk_bt_audio_record_get_mic_bst_gain(uint32_t mic_category);

/**
 * @brief     Read record data from the audio record.
 * @param[in] record_hdl: Audio record handle returned by @ref rtk_bt_audio_record_init.
 * @param[in] buffer: Destination buffer of the application.
 * @param[in] size: Size of the destination buffer in bytes.
 * @param[in] blocking: Whether to block when no data is available. Suggested value is true.
 *                      - true: Block until data is read.
 *                      - false: Do not block.
 * @return
 *            - Number of bytes read
 * @note
 *            - This API shall be called after @ref rtk_bt_audio_record_init.
 */
int rtk_bt_audio_record_read(void *record_hdl, void *buffer, int size, bool blocking);

/**
 * @brief     Mute or unmute the record of a channel.
 * @param[in] channel: Record channel to mute or unmute.
 * @param[in] muted: Whether to mute the record.
 *                   - true: Mute the record.
 *                   - false: Unmute the record.
 */
void rtk_bt_audio_record_set_mute(uint32_t channel, bool muted);

/**
 * @brief     Get the mute state of the record of a channel.
 * @param[in] channel: Record channel to query.
 * @return
 *            - true: Record is muted
 *            - false: Record is not muted
 */
bool rtk_bt_audio_record_get_muted(uint32_t channel);

/**
 * @}
 */

#endif /* __BT_AUDIO_RECORD_API_H__ */
