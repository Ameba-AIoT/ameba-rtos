/**
 * @file      rtk_bt_a2dp.h
 * @brief     Bluetooth A2DP type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_A2DP_H__
#define __RTK_BT_A2DP_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_A2DP_MAX_CODEC_PARAM_LENGTH 12      /*!< Maximum length of the codec parameter data, in bytes. */
#define RTK_BT_A2DP_MAX_STREAM_DATA_LENGTH 1024    /*!< Maximum length of the received stream data, in bytes. */

/* SBC codec capability bit masks. */
#define RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_16KHZ                (1<<7)   /*!< SBC 16 kHz sampling frequency, bit 7 of the sampling frequency field. */
#define RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_32KHZ                (1<<6)   /*!< SBC 32 kHz sampling frequency, bit 6 of the sampling frequency field. */
#define RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_44_1KHZ              (1<<5)   /*!< SBC 44.1 kHz sampling frequency, bit 5 of the sampling frequency field. */
#define RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_48KHZ                (1<<4)   /*!< SBC 48 kHz sampling frequency, bit 4 of the sampling frequency field. */
#define RTK_BT_A2DP_SBC_CHANNEL_MODE_MONO                       (1<<3)   /*!< SBC mono channel mode, bit 3 of the channel mode field. */
#define RTK_BT_A2DP_SBC_CHANNEL_MODE_DUAL_CHANNEL               (1<<2)   /*!< SBC dual channel mode, bit 2 of the channel mode field. */
#define RTK_BT_A2DP_SBC_CHANNEL_MODE_STEREO                     (1<<1)   /*!< SBC stereo channel mode, bit 1 of the channel mode field. */
#define RTK_BT_A2DP_SBC_CHANNEL_MODE_JOINT_STEREO               (1<<0)   /*!< SBC joint stereo channel mode, bit 0 of the channel mode field. */
#define RTK_BT_A2DP_SBC_BLOCK_LENGTH_4                          (1<<7)   /*!< SBC block length of 4, bit 7 of the block length field. */
#define RTK_BT_A2DP_SBC_BLOCK_LENGTH_8                          (1<<6)   /*!< SBC block length of 8, bit 6 of the block length field. */
#define RTK_BT_A2DP_SBC_BLOCK_LENGTH_12                         (1<<5)   /*!< SBC block length of 12, bit 5 of the block length field. */
#define RTK_BT_A2DP_SBC_BLOCK_LENGTH_16                         (1<<4)   /*!< SBC block length of 16, bit 4 of the block length field. */
#define RTK_BT_A2DP_SBC_SUBBANDS_4                              (1<<3)   /*!< SBC 4 subbands, bit 3 of the subbands field. */
#define RTK_BT_A2DP_SBC_SUBBANDS_8                              (1<<2)   /*!< SBC 8 subbands, bit 2 of the subbands field. */
#define RTK_BT_A2DP_SBC_ALLOCATION_METHOD_SNR                   (1<<1)   /*!< SBC SNR allocation method, bit 1 of the allocation method field. */
#define RTK_BT_A2DP_SBC_ALLOCATION_METHOD_LOUDNESS              (1<<0)   /*!< SBC loudness allocation method, bit 0 of the allocation method field. */

/* AAC codec capability bit masks. */
#define RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_2_AAC_LC               (1<<7)   /*!< AAC MPEG-2 AAC LC object type, bit 7 of the object type field. */
#define RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_LC               (1<<6)   /*!< AAC MPEG-4 AAC LC object type, bit 6 of the object type field. */
#define RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_LTP              (1<<5)   /*!< AAC MPEG-4 AAC LTP object type, bit 5 of the object type field. */
#define RTK_BT_A2DP_AAC_OBJECT_TYPE_MPEG_4_AAC_SCALABLE         (1<<4)   /*!< AAC MPEG-4 AAC scalable object type, bit 4 of the object type field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_8KHZ                 (1<<15)  /*!< AAC 8 kHz sampling frequency, bit 15 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_11_025KHZ            (1<<14)  /*!< AAC 11.025 kHz sampling frequency, bit 14 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_12KHZ                (1<<13)  /*!< AAC 12 kHz sampling frequency, bit 13 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_16KHZ                (1<<12)  /*!< AAC 16 kHz sampling frequency, bit 12 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_22_05KHZ             (1<<11)  /*!< AAC 22.05 kHz sampling frequency, bit 11 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_24KHZ                (1<<10)  /*!< AAC 24 kHz sampling frequency, bit 10 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_32KHZ                (1<<9)   /*!< AAC 32 kHz sampling frequency, bit 9 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_44_1KHZ              (1<<8)   /*!< AAC 44.1 kHz sampling frequency, bit 8 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_48KHZ                (1<<7)   /*!< AAC 48 kHz sampling frequency, bit 7 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_64KHZ                (1<<6)   /*!< AAC 64 kHz sampling frequency, bit 6 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_88_2KHZ              (1<<5)   /*!< AAC 88.2 kHz sampling frequency, bit 5 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_96KHZ                (1<<4)   /*!< AAC 96 kHz sampling frequency, bit 4 of the sampling frequency field. */
#define RTK_BT_A2DP_AAC_CHANNEL_NUMBER_1                        (1<<3)   /*!< AAC one channel, bit 3 of the channel number field. */
#define RTK_BT_A2DP_AAC_CHANNEL_NUMBER_2                        (1<<2)   /*!< AAC two channels, bit 2 of the channel number field. */

/************************************** Data Types *************************************/

/**
 * @typedef   rtk_bt_a2dp_role_t
 * @brief     Bluetooth A2DP role.
 */
typedef enum {
	RTK_BT_A2DP_ROLE_SRC = 0x00,    /*!< A2DP source (SRC) role. */
	RTK_BT_A2DP_ROLE_SNK = 0x01,    /*!< A2DP sink (SNK) role. */
} rtk_bt_a2dp_role_t;

/**
 * @struct    rtk_bt_a2dp_media_codec_sbc_t
 * @brief     Bluetooth A2DP SBC codec configuration.
 */
typedef struct {
	uint8_t sampling_frequency_mask;    /*!< Supported sampling frequency bit mask.
                                         *   Bit values are defined by the RTK_BT_A2DP_SBC_SAMPLING_FREQUENCY_xxx macros. */
	uint8_t channel_mode_mask;          /*!< Supported channel mode bit mask.
                                         *   Bit values are defined by the RTK_BT_A2DP_SBC_CHANNEL_MODE_xxx macros. */
	uint8_t block_length_mask;          /*!< Supported block length bit mask.
                                         *   Bit values are defined by the RTK_BT_A2DP_SBC_BLOCK_LENGTH_xx macros. */
	uint8_t subbands_mask;              /*!< Supported number of subbands bit mask.
                                         *   Bit values are defined by the RTK_BT_A2DP_SBC_SUBBANDS_x macros. */
	uint8_t allocation_method_mask;     /*!< Supported allocation method bit mask.
                                         *   Bit values are defined by the RTK_BT_A2DP_SBC_ALLOCATION_METHOD_xxx macros. */
	uint8_t min_bitpool;                /*!< Minimum bitpool value supported. */
	uint8_t max_bitpool;                /*!< Maximum bitpool value supported. */
} rtk_bt_a2dp_media_codec_sbc_t;

/**
 * @struct    rtk_bt_a2dp_media_codec_aac_t
 * @brief     Bluetooth A2DP AAC codec configuration.
 */
typedef struct {
	uint8_t  object_type_mask;           /*!< Supported object type bit mask.
                                          *   Bit values are defined by the RTK_BT_A2DP_AAC_OBJECT_TYPE_xxx macros. */
	uint16_t sampling_frequency_mask;    /*!< Supported sampling frequency bit mask.
                                          *   Bit values are defined by the RTK_BT_A2DP_AAC_SAMPLING_FREQUENCY_xxx macros. */
	uint8_t  channel_number_mask;        /*!< Supported number of channels bit mask.
                                          *   Bit values are defined by the RTK_BT_A2DP_AAC_CHANNEL_NUMBER_x macros. */
	bool     vbr_supported;              /*!< Whether variable bit rate (VBR) is supported.
                                          *   - true: VBR is supported.
                                          *   - false: VBR is not supported. */
	uint32_t bit_rate;                   /*!< Maximum bit rate supported, in bits per second.
                                          *   0: Unknown or not specified. */
} rtk_bt_a2dp_media_codec_aac_t;

/**
 * @struct    rtk_bt_a2dp_conn_ind_t
 * @brief     Bluetooth A2DP connection indication.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Address of the remote device. */
} rtk_bt_a2dp_conn_ind_t;

/**
 * @struct    rtk_bt_a2dp_disconn_ind_t
 * @brief     Bluetooth A2DP disconnection indication.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Address of the remote device. */
} rtk_bt_a2dp_disconn_ind_t;

/**
 * @struct    rtk_bt_a2dp_stream_open_t
 * @brief     Bluetooth A2DP stream open indication.
 */
typedef struct {
	uint8_t bd_addr[6];      /*!< Address of the remote device. */
	uint16_t max_pkt_len;    /*!< Maximum length of a media packet, in bytes. */
} rtk_bt_a2dp_stream_open_t;

/**
 * @struct    rtk_bt_a2dp_codec_param_t
 * @brief     Bluetooth A2DP codec parameter data.
 */
typedef struct {
	uint8_t  codec_type;                                  /*!< Codec type. */
	uint32_t length;                                      /*!< Valid length of the codec parameter data, in bytes. */
	uint8_t  data[RTK_BT_A2DP_MAX_CODEC_PARAM_LENGTH];    /*!< Codec parameter data. */
} rtk_bt_a2dp_codec_param_t;

/**
 * @struct    rtk_bt_a2dp_codec_t
 * @brief     Bluetooth A2DP codec configuration.
 */
typedef struct {
	uint8_t codec_type;                        /*!< Codec type. */
	union {
		struct {
			uint8_t     sampling_frequency;    /*!< Selected sampling frequency. */
			uint8_t     channel_mode;          /*!< Selected channel mode. */
			uint8_t     block_length;          /*!< Selected block length. */
			uint8_t     subbands;              /*!< Selected number of subbands. */
			uint8_t     allocation_method;     /*!< Selected allocation method. */
			uint8_t     min_bitpool;           /*!< Minimum bitpool value. */
			uint8_t     max_bitpool;           /*!< Maximum bitpool value. */
		} sbc;
		struct {
			uint8_t     object_type;           /*!< Selected object type. */
			uint16_t    sampling_frequency;    /*!< Selected sampling frequency. */
			uint8_t     channel_number;        /*!< Selected number of channels. */
			bool        vbr_supported;         /*!< Whether variable bit rate (VBR) is supported.
                                                *   - true: VBR is supported.
                                                *   - false: VBR is not supported. */
			uint32_t    bit_rate;              /*!< Bit rate, in bits per second.
                                                *   0: Unknown or not specified. */
		} aac;
		struct {
			uint8_t     info[12];              /*!< Vendor codec information. */
		} vendor;
	};
} rtk_bt_a2dp_codec_t;

/**
 * @struct    rtk_bt_a2dp_stream_start_t
 * @brief     Bluetooth A2DP stream start indication with the negotiated codec information.
 */
typedef struct {
	uint8_t bd_addr[6];                        /*!< Address of the remote device. */
	uint8_t codec_type;                        /*!< Codec type. */
	union {
		struct {
			uint8_t     sampling_frequency;    /*!< Selected sampling frequency. */
			uint8_t     channel_mode;          /*!< Selected channel mode. */
			uint8_t     block_length;          /*!< Selected block length. */
			uint8_t     subbands;              /*!< Selected number of subbands. */
			uint8_t     allocation_method;     /*!< Selected allocation method. */
			uint8_t     min_bitpool;           /*!< Minimum bitpool value. */
			uint8_t     max_bitpool;           /*!< Maximum bitpool value. */
		} sbc;
		struct {
			uint8_t     object_type;           /*!< Selected object type. */
			uint16_t    sampling_frequency;    /*!< Selected sampling frequency. */
			uint8_t     channel_number;        /*!< Selected number of channels. */
			bool        vbr_supported;         /*!< Whether variable bit rate (VBR) is supported.
                                                *   - true: VBR is supported.
                                                *   - false: VBR is not supported. */
			uint32_t    bit_rate;              /*!< Bit rate, in bits per second.
                                                *   0: Unknown or not specified. */
		} aac;
		struct {
			uint8_t     info[12];              /*!< Vendor codec information. */
		} vendor;
	} codec_info;
} rtk_bt_a2dp_stream_start_t;

/**
 * @struct    rtk_bt_a2dp_stream_data_send_t
 * @brief     Bluetooth A2DP stream data sending parameters.
 */
typedef struct {
	uint8_t  bd_addr[6];    /*!< Address of the remote device. */
	uint16_t seq_num;       /*!< The sequence number of the stream data. */
	uint32_t time_stamp;    /*!< The timestamp of the stream data.
                             *   Sampling instant of the first octet of the stream data. */
	uint8_t  frame_num;     /*!< Number of codec frames in the stream data. */
	uint8_t  *frame_buf;    /*!< Buffer holding the codec frames to send. */
	uint16_t len;           /*!< Length of the stream data in the buffer, in bytes. */
	bool     flush;         /*!< Whether to auto flush the current stream data.
                             *   - true: Flush the current stream data automatically.
                             *   - false: Do not flush the current stream data automatically. */
} rtk_bt_a2dp_stream_data_send_t;

/**
 * @struct    rtk_bt_a2dp_stream_data_ind_t
 * @brief     Bluetooth A2DP received stream data indication.
 */
typedef struct {
	uint16_t seq_num;                                     /*!< The sequence number of the received media packet. */
	uint32_t length;                                      /*!< Length of the stream data, in bytes. */
	uint8_t  data[RTK_BT_A2DP_MAX_STREAM_DATA_LENGTH];    /*!< Received stream data. */
} rtk_bt_a2dp_stream_data_ind_t;

/**
 * @struct    rtk_bt_a2dp_stream_data_rsp_t
 * @brief     Bluetooth A2DP stream data response indication.
 */
typedef struct {
	uint8_t  bd_addr[6];    /*!< Address of the remote device. */
} rtk_bt_a2dp_stream_data_rsp_t;

/**
 * @struct    rtk_bt_a2dp_stream_stop_t
 * @brief     Bluetooth A2DP stream stop indication.
 */
typedef struct {
	uint8_t  bd_addr[6];    /*!< Address of the remote device. */
} rtk_bt_a2dp_stream_stop_t;

/**
 * @struct    rtk_bt_a2dp_stream_close_t
 * @brief     Bluetooth A2DP stream close indication.
 */
typedef struct {
	uint8_t  bd_addr[6];    /*!< Address of the remote device. */
	uint16_t cause;         /*!< Cause of the stream close. */
} rtk_bt_a2dp_stream_close_t;

/**
 * @struct    rtk_bt_a2dp_sdp_attr_info_t
 * @brief     Bluetooth A2DP SDP attribute information of the remote device.
 */
typedef struct {
	uint8_t                 bd_addr[6];             /*!< Address of the remote device. */
	br_gap_uuid_type_t      srv_class_uuid_type;    /*!< Service class UUID type. */
	br_gap_uuid_data_t      srv_class_uuid_data;    /*!< Service class UUID data. */
	uint16_t                profile_version;        /*!< Profile version of the remote device. */
	uint16_t                protocol_version;       /*!< Protocol version of the remote device. */
	uint8_t                 server_channel;         /*!< Server channel number. */
	uint16_t                supported_feat;         /*!< Supported features. */
	uint16_t                l2c_psm;                /*!< L2CAP PSM value. */
	uint8_t                 supported_repos;        /*!< Supported repositories. */
	uint32_t                pbap_supported_feat;    /*!< Supported features of phone book access profile. */
} rtk_bt_a2dp_sdp_attr_info_t;


/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_a2dp BT A2DP APIs
 * @brief     BT A2DP related function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Set the A2DP codec configuration used for stream negotiation with the remote device.
 * @param[in] codec_type: Codec to configure.
 * @param[in] codec_t: Codec capability configuration.
 *                     Must not be NULL.
 * @param[in] length: Length of the configuration in bytes.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before BT is enabled; otherwise it fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_a2dp_codec_cfg(uint8_t codec_type, void *codec_t, uint32_t length);

/**
 * @brief     Send an A2DP connection request to the remote device.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_a2dp_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect the A2DP connection with the remote device.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_a2dp_disconnect(uint8_t *bd_addr);

/**
 * @brief     Request to start the A2DP stream with the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_A2DP_EVT_STREAM_START_RSP.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_a2dp_start(uint8_t *bd_addr);

/**
 * @brief     Request to suspend the A2DP stream with the remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_A2DP_EVT_STREAM_STOP.
 * @param[in] bd_addr: Address of the remote device.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_a2dp_suspend(uint8_t *bd_addr);

/**
 * @brief     Send A2DP stream data to the remote device.
 * @param[in] p_data_send_t: Stream data sending parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_a2dp_data_send(rtk_bt_a2dp_stream_data_send_t *p_data_send_t);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_A2DP_H__ */
