/**
 * @file      bt_audio_codec_wrapper.h
 * @brief     Bluetooth audio codec entity type and codec wrapper function declarations.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_CODEC_WRAPPER_H__
#define __BT_AUDIO_CODEC_WRAPPER_H__

#include <dlist.h>

/************************************** Data Types *************************************/

#define BT_AUDIO_CODEC_REG_MAX_PARAM_LEN 128              /*!< Maximum length in octets of the codec parameter data in @ref bt_audio_reg_codec_t. */
#define BT_AUDIO_MAX_DATA_SIZE 1024                       /*!< Maximum length in octets of the media data buffers in the offload data structures. */

/**
 * @struct    enc_codec_buffer
 * @brief     Bluetooth audio encode buffer.
 */
struct enc_codec_buffer {
	struct list_head            list;               /*!< List node linking the buffer. */
	uint8_t                     *pbuffer;           /*!< Pointer to the encoded data. */
	uint8_t                     frame_num;          /*!< Number of encoded frames in the buffer. */
	uint32_t                    frame_size;         /*!< Size of the encoded data in octets. */
	void                        *offload_buffer;    /*!< Reserved for future use. */
};

/**
 * @struct    audio_param
 * @brief     Bluetooth audio stream parameters.
 */
struct audio_param {
	uint32_t                    channels;              /*!< Number of audio channels. */
	uint32_t                    channel_allocation;    /*!< Audio channel allocation bit mask. */
	uint32_t                    rate;                  /*!< Sampling rate in Hz. */
	uint32_t                    bits;                  /*!< Audio format indication; e.g. BT_AUDIO_FORMAT_PCM_16_BIT. */
};

/**
 * @struct    dec_codec_buffer
 * @brief     Bluetooth audio decode buffer holding the decoded PCM data.
 */
struct dec_codec_buffer {
	struct list_head            list;                 /*!< List node linking the buffer. */
	uint32_t                    type;                 /*!< Codec type of the decoded data, a value of @ref rtk_bt_audio_codec_t. */
	int16_t                     *pbuffer;             /*!< Pointer to the decoded PCM data. */
	uint32_t                    total_size;           /*!< Total size of the PCM buffer. */
	uint32_t                    actual_write_size;    /*!< Size of the PCM data written by the decoder in octets. */
	struct audio_param          audio_param_t;        /*!< Audio parameters of the decoded PCM data. */
	void                        *offload_buffer;      /*!< Reserved for future use. */
};

/**
 * @struct    audio_codec_entity_priv
 * @brief     Bluetooth audio codec entity. Holds the codec state, the codec parameters and the codec operation callbacks.
 */
struct audio_codec_entity_priv {
	struct list_head            list;                                              /*!< List node linking the entity. */
	uint8_t                     active_flag;                                       /*!< Entity state.
                                                                                    *   1: The codec is registered and the entity is active.
                                                                                    *   0: The entity is not active. */
	void                        *mutex;                                            /*!< Mutex protecting the codec operations of the entity. */
	int32_t                     track_num;                                         /*!< Number of tracks currently using the entity. */
	int32_t                     record_num;                                        /*!< Number of records currently using the entity. */
	int32_t                     encode_num;                                        /*!< Number of encode operations in progress. */
	int32_t                     decode_num;                                        /*!< Number of decode operations in progress. */
	bool                        encode_lock;                                       /*!< Whether an encode buffer is held.
                                                                                    *   - true: An encode buffer has been obtained and not yet freed.
                                                                                    *   - false: No encode buffer is held. */
	bool                        decode_lock;                                       /*!< Whether a decode buffer is held.
                                                                                    *   - true: A decode buffer has been obtained and not yet freed.
                                                                                    *   - false: No decode buffer is held. */
	uint32_t                    type;                                              /*!< Codec type, a value of @ref rtk_bt_audio_codec_t. */
	int                         stream_in_num;                                     /*!< Number of data items or commands enqueued by the application. */
	union {
		struct {
			uint8_t     sampling_frequency;                                        /*!< Sampling frequency. */
			uint8_t     channel_mode;                                              /*!< Channel mode. */
			uint8_t     block_length;                                              /*!< Block length. */
			uint8_t     subbands;                                                  /*!< Number of subbands. */
			uint8_t     allocation_method;                                         /*!< Allocation method. */
			uint8_t     min_bitpool;                                               /*!< Minimum bitpool. */
			uint8_t     max_bitpool;                                               /*!< Maximum bitpool. */
		} sbc;
		struct {
			uint8_t     object_type;                                               /*!< Object type. */
			uint16_t    sampling_frequency;                                        /*!< Sampling frequency. */
			uint8_t     channel_number;                                            /*!< Number of channels. */
			bool        vbr_supported;                                             /*!< Whether variable bit rate is supported.
                                                                                    *   - true: Variable bit rate is supported.
                                                                                    *   - false: Variable bit rate is not supported. */
			uint32_t    bit_rate;                                                  /*!< Bit rate. */
		} aac;
		struct {
			uint8_t     channel_mode;                                              /*!< Channel mode. */
			int         sample_rate;                                               /*!< Sampling rate. */
			uint8_t     bitpool;                                                   /*!< Bitpool. */
			uint8_t     allocation_method;                                         /*!< Allocation method. */
			uint8_t     subbands;                                                  /*!< Number of subbands. */
			uint8_t     block_length;                                              /*!< Block length. */
		} msbc;
		struct {
			uint8_t     channel_num;                                               /*!< Number of channels. */
			int         sample_rate;                                               /*!< Sampling rate. */
			uint8_t     frame_duration;                                            /*!< Frame duration. */
		} cvsd;
		struct {
			uint16_t    type_exist;                                                /*!< Flags indicating which codec configuration types are present.
                                                                                    *   e.g. RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST. */
			uint16_t    frame_duration;                                            /*!< Frame duration in us.
                                                                                    *   Value: 7500 or 10000. */
			int         sample_rate;                                               /*!< Sampling rate in Hz. */
			uint8_t     bits_per_audio_sample_enc;                                 /*!< Number of bits per audio sample for encoding. */
			uint8_t     bits_per_audio_sample_dec;                                 /*!< Number of bits per audio sample for decoding. */
			uint8_t     codec_frame_blocks_per_sdu;                                /*!< Number of codec frame blocks per SDU. */
			uint16_t    octets_per_codec_frame;                                    /*!< Number of octets per codec frame. */
			uint32_t    audio_channel_allocation;                                  /*!< Audio channel allocation bit mask. */
			uint32_t    channels;                                                  /*!< Number of audio channels, derived from the audio channel allocation. */
			int         compress_bytes;                                            /*!< Size of one compressed frame of a single channel in octets. */
			uint32_t    presentation_delay;                                        /*!< Presentation delay. */
			void        *p_lc3_t;                                                  /*!< Pointer to the LC3 encoder and decoder context. */
		} lc3;
		struct {
			uint8_t     info[12];                                                  /*!< Vendor codec data. */
		} vendor;
	};
	uint16_t (*init)(void *pentity, void *param);                                  /*!< Initialize the codec with the given parameters. */
	uint16_t (*update)(void *pentity, void *param);                                /*!< Update the codec parameters. */
	uint16_t (*reset)(void *pentity);                                              /*!< Reset the codec state. */
	uint16_t (*deinit)(void *pentity);                                             /*!< Deinitialize the codec and release its resources. */
	/*!< Parse a received media packet to get the frame size, frame number, codec header flag and audio parameters. */
	uint16_t (*bt_audio_handle_media_data_packet)(void *pentity, uint8_t *packet, uint16_t size, uint32_t *pframe_size, uint8_t *pframe_num,
												  uint8_t *pcodec_header_flag, struct audio_param *paudio_param);
	/*!< Encode PCM data to codec data. */
	uint16_t (*encoding_func)(void *pentity, int16_t *data, uint32_t size, struct enc_codec_buffer *pencoder_buffer, uint8_t *p_frame_num, uint32_t *p_actual_len);
	/*!< Decode codec data to PCM data. */
	uint16_t (*decoding_func)(void *pentity, uint8_t *data, uint32_t size, struct dec_codec_buffer *pdecoder_buffer, uint32_t *ppcm_size,
							  struct audio_param *paudio_param);
	struct dec_codec_buffer     *(*get_decode_buffer)(void *pentity);              /*!< Allocate a decode buffer. */
	void (*free_decode_buffer)(void *pentity, struct dec_codec_buffer *buffer);    /*!< Free a decode buffer. */
	struct enc_codec_buffer     *(*get_encode_buffer)(void *pentity);              /*!< Allocate an encode buffer. */
	void (*free_encode_buffer)(void *pentity, struct enc_codec_buffer *buffer);    /*!< Free an encode buffer. */
	int (*decoder_num_samples_per_frame)(void *pentity);                           /*!< Get the number of PCM samples per decoded frame. */
	int (*decoder_num_channels)(void *pentity);                                    /*!< Get the number of decoded audio channels. */
	int (*decoder_sample_rate)(void *pentity);                                     /*!< Get the sampling rate of the decoder. */
	uint16_t (*get_pcm_buffer_from_decode)(void *p_pcm_buf, uint16_t p_len);       /*!< Get the PCM buffer from the decoder. */
	void *user_param;                                                              /*!< User private data pointer. */
};

/**
 * @typedef   AUDIO_CODEC_ENTITY
 * @brief     Bluetooth audio codec entity type, with @ref PAUDIO_CODEC_ENTITY as its pointer type.
 */
typedef struct audio_codec_entity_priv AUDIO_CODEC_ENTITY, *PAUDIO_CODEC_ENTITY;

/********************************** Offload Data Types *********************************/

/**
 * @typedef   bt_audio_reg_codec_t
 * @brief     Bluetooth audio offload codec registration data.
 */
typedef struct {
	uint32_t            type;                                       /*!< Codec type, a value of @ref rtk_bt_audio_codec_t. */
	uint8_t             param[BT_AUDIO_CODEC_REG_MAX_PARAM_LEN];    /*!< Codec parameter data. */
	uint32_t            param_len;                                  /*!< Length of the codec parameter data in octets.
                                                                     *   Range: 0 to @ref BT_AUDIO_CODEC_REG_MAX_PARAM_LEN. */
} bt_audio_reg_codec_t;

/**
 * @typedef   bt_audio_unreg_codec_t
 * @brief     Bluetooth audio offload codec unregistration data.
 */
typedef struct {
	uint32_t            type;        /*!< Codec type, a value of @ref rtk_bt_audio_codec_t. */
	void                *pentity;    /*!< Codec entity pointer. */
} bt_audio_unreg_codec_t;

/**
 * @typedef   bt_audio_parse_media_t
 * @brief     Bluetooth audio media packet struct for parsing.
 */
typedef struct {
	void                *pentity;                        /*!< Codec entity pointer. */
	uint8_t             data[BT_AUDIO_MAX_DATA_SIZE];    /*!< Media packet data. */
	uint16_t            size;                            /*!< Length of the media packet data in octets.
                                                          *   Range: 0 to @ref BT_AUDIO_MAX_DATA_SIZE. */
} bt_audio_parse_media_t;

/**
 * @typedef   bt_audio_packet_info_t
 * @brief     Bluetooth audio media packet information.
 */
typedef struct {
	uint32_t            frame_size;           /*!< Size of one frame in octets. */
	uint8_t             frame_num;            /*!< Number of frames in the media packet. */
	uint8_t             codec_header_flag;    /*!< Whether the media packet carries a codec header.
                                               *   1: The media packet carries a codec header.
                                               *   0: The media packet carries no codec header. */
	struct audio_param  param;                /*!< Audio parameters parsed from the media packet. */
} bt_audio_packet_info_t;

/**
 * @typedef   bt_audio_decode_process_data_t
 * @brief     Bluetooth audio data for decoding.
 */
typedef struct {
	void                *pentity;                        /*!< Codec entity pointer. */
	void                *decode_buffer;                  /*!< Decode buffer pointer. */
	uint8_t             data[BT_AUDIO_MAX_DATA_SIZE];    /*!< Codec data to decode. */
	uint32_t            size;                            /*!< Length of the codec data in octets.
                                                          *   Range: 0 to @ref BT_AUDIO_MAX_DATA_SIZE. */
} bt_audio_decode_process_data_t;

/**
 * @typedef   bt_audio_decode_data_info_t
 * @brief     Bluetooth audio decoded data information.
 */
typedef struct {
	uint8_t                 data[BT_AUDIO_MAX_DATA_SIZE];    /*!< Decoded PCM data. */
	struct dec_codec_buffer buffer;                          /*!< Decode buffer describing the decoded PCM data. */
	uint32_t                pcm_size;                        /*!< Length of the decoded PCM data in octets. */
	struct audio_param      param;                           /*!< Audio parameters of the decoded PCM data. */
} bt_audio_decode_data_info_t;

/**
 * @typedef   bt_audio_free_decode_buff_t
 * @brief     Bluetooth audio offload decode buffer to free.
 */
typedef struct {
	void                *pentity;    /*!< Codec entity pointer. */
	void                *buffer;     /*!< Decode buffer pointer to free. */
} bt_audio_free_decode_buff_t;

/**
 * @typedef   bt_audio_encode_process_data_t
 * @brief     Bluetooth audio data for encoding.
 */
typedef struct {
	void                *pentity;                        /*!< Codec entity pointer. */
	void                *encode_buffer;                  /*!< Encode buffer pointer. */
	uint8_t             data[BT_AUDIO_MAX_DATA_SIZE];    /*!< PCM data to encode. */
	uint32_t            size;                            /*!< Length of the PCM data in octets.
                                                          *   Range: 0 to @ref BT_AUDIO_MAX_DATA_SIZE. */
} bt_audio_encode_process_data_t;

/**
 * @typedef   bt_audio_encode_data_info_t
 * @brief     Bluetooth audio encoded data information.
 */
typedef struct {
	uint8_t                 data[BT_AUDIO_MAX_DATA_SIZE];    /*!< Encoded codec data. */
	struct enc_codec_buffer buffer;                          /*!< Encode buffer describing the encoded data. */
	uint8_t                 frame_num;                       /*!< Number of encoded frames. */
	uint32_t                actual_len;                      /*!< Length of the encoded data in octets. */
} bt_audio_encode_data_info_t;

/**
 * @typedef   bt_audio_free_encode_buff_t
 * @brief     Bluetooth audio offload encode buffer to free.
 */
typedef struct {
	void                *pentity;    /*!< Codec entity pointer. */
	void                *buffer;     /*!< Encode buffer pointer to free. */
} bt_audio_free_encode_buff_t;

/********************************* Functions Declaration *******************************/

/**
 * @brief     Parse a received media packet to get the frame size, frame number, codec header flag and audio parameters.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @param[in] packet: Received media packet data.
 * @param[in] size: Length of the media packet data in octets.
 * @param[out] pframe_size: Size of one frame in octets.
 *                          Must not be NULL.
 * @param[out] pframe_num: Number of frames in the media packet.
 *                         Must not be NULL.
 * @param[out] pcodec_header_flag: Whether the media packet carries a codec header.
 *                                 1: The media packet carries a codec header.
 *                                 0: The media packet carries no codec header.
 *                                 Must not be NULL.
 * @param[out] paudio_param: Audio parameters parsed from the media packet.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - This API shall be called before @ref bt_audio_decode_process_data.
 *            - This API fails with @ref RTK_BT_AUDIO_FAIL if the codec entity is not active.
 */
uint16_t bt_audio_handle_media_data_packet(PAUDIO_CODEC_ENTITY pentity,
										   uint8_t *packet,
										   uint16_t size,
										   uint32_t *pframe_size,
										   uint8_t *pframe_num,
										   uint8_t *pcodec_header_flag, struct audio_param *paudio_param);

/**
 * @brief     Decode codec data to PCM data. The decoded PCM data is stored in the decode buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @param[in,out] pdecoder_buffer: Decode buffer that receives the decoded PCM data.
 *                                 Must not be NULL.
 * @param[in] data: Codec data to decode.
 * @param[in] size: Length of the codec data in octets.
 * @param[out] ppcm_size: Length of the decoded PCM data in octets.
 *                        Must not be NULL.
 * @param[in] paudio_param: Audio parameters of the stream.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - This API fails with @ref RTK_BT_AUDIO_FAIL if the codec entity is not active.
 */
uint16_t bt_audio_decode_process_data(PAUDIO_CODEC_ENTITY pentity, struct dec_codec_buffer *pdecoder_buffer, uint8_t *data, uint32_t size, uint32_t *ppcm_size,
									  struct audio_param *paudio_param);

/**
 * @brief     Allocate a decode buffer from the codec entity. The decode buffer shall be released with @ref bt_audio_free_decode_buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - Pointer to the decode buffer.
 *            - NULL: The codec entity is NULL or not active, or the allocation failed.
 * @note
 *            - While the decode buffer is held, @ref bt_audio_unregister_codec waits for it to be freed.
 */
struct dec_codec_buffer *bt_audio_get_decode_buffer(PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Free a decode buffer obtained from @ref bt_audio_get_decode_buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @param[in] buffer: Decode buffer to free.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t bt_audio_free_decode_buffer(PAUDIO_CODEC_ENTITY pentity, struct dec_codec_buffer *buffer);

/**
 * @brief     Encode PCM data to codec data. The encoded data is stored in the encode buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @param[in,out] pencoder_buffer: Encode buffer that receives the encoded data.
 *                                 Must not be NULL.
 * @param[in] data: PCM data to encode.
 *                  Must not be NULL.
 * @param[in] size: Length of the PCM data in octets.
 * @param[out] p_frame_num: Number of encoded frames.
 *                          Must not be NULL.
 * @param[out] p_actual_len: Length of the encoded data in octets.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - This API fails with @ref RTK_BT_AUDIO_FAIL if the codec entity is not active.
 */
uint16_t bt_audio_encode_process_data(PAUDIO_CODEC_ENTITY pentity, struct enc_codec_buffer *pencoder_buffer, int16_t *data, uint32_t size, uint8_t *p_frame_num,
									  uint32_t *p_actual_len);

/**
 * @brief     Allocate an encode buffer from the codec entity. The encode buffer shall be released with @ref bt_audio_free_encode_buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - Pointer to the encode buffer.
 *            - NULL: The codec entity is NULL or not active, or the allocation failed.
 * @note
 *            - While the encode buffer is held, @ref bt_audio_unregister_codec waits for it to be freed.
 */
struct enc_codec_buffer *bt_audio_get_encode_buffer(PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Free an encode buffer obtained from @ref bt_audio_get_encode_buffer.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @param[in] buffer: Encode buffer to free.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t bt_audio_free_encode_buffer(PAUDIO_CODEC_ENTITY pentity, struct enc_codec_buffer *buffer);

/**
 * @brief     Get the number of PCM samples per decoded frame.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - Number of PCM samples per frame.
 *            - @ref RTK_BT_AUDIO_GET_VALUE_FAIL: The codec entity is NULL.
 *            - @ref RTK_BT_AUDIO_FAIL: The codec entity is not active.
 */
uint16_t bt_codec_decoder_num_samples_per_frame(PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Get the number of decoded audio channels.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - Number of audio channels.
 *            - @ref RTK_BT_AUDIO_GET_VALUE_FAIL: The codec entity is NULL.
 *            - @ref RTK_BT_AUDIO_FAIL: The codec entity is not active.
 */
uint16_t bt_codec_decoder_num_channels(PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Get the sampling rate of the decoder.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - Sampling rate in Hz.
 *            - @ref RTK_BT_AUDIO_GET_VALUE_FAIL: The codec entity is NULL.
 *            - @ref RTK_BT_AUDIO_FAIL: The codec entity is not active.
 */
int bt_codec_decoder_sample_rate(PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Register a codec to the codec entity. The codec is initialized, the entity mutex is created and the entity becomes active.
 * @param[in] type: Codec type, a value of @ref rtk_bt_audio_codec_t.
 * @param[in] param: Codec-specific initialization parameters.
 * @param[in] param_len: Length of the initialization parameters in octets.
 * @param[in,out] pentity: Codec entity to be initialized.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - The codec type shall be enabled by its CONFIG_BT_AUDIO_CODEC_xxx_SUPPORT option, otherwise this API fails with @ref RTK_BT_AUDIO_FAIL.
 *            - Registering @ref RTK_BT_AUDIO_CODEC_AAC always fails with @ref RTK_BT_AUDIO_FAIL.
 */
uint16_t bt_audio_register_codec(uint32_t type, void *param, uint32_t param_len, PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Update the codec parameters of an active codec entity.
 * @param[in] type: Codec type, a value of @ref rtk_bt_audio_codec_t.
 * @param[in] param: Codec-specific parameters.
 * @param[in] param_len: Length of the parameters in octets.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - Only @ref RTK_BT_AUDIO_CODEC_SBC and @ref RTK_BT_AUDIO_CODEC_mSBC can be updated, other codec types fail with @ref RTK_BT_AUDIO_FAIL.
 */
uint16_t bt_audio_update_codec(uint32_t type, void *param, uint32_t param_len, PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Reset the codec state of an active codec entity.
 * @param[in] type: Codec type, a value of @ref rtk_bt_audio_codec_t.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 * @note
 *            - Only @ref RTK_BT_AUDIO_CODEC_SBC and @ref RTK_BT_AUDIO_CODEC_mSBC can be reset, other codec types fail with @ref RTK_BT_AUDIO_FAIL.
 */
uint16_t bt_audio_reset_codec(uint32_t type, PAUDIO_CODEC_ENTITY pentity);

/**
 * @brief     Unregister the codec of the codec entity. The entity becomes inactive, waits until the decode and encode buffers are freed,
 *            then the codec is deinitialized and the entity mutex is deleted.
 * @param[in] type: Codec type.
 * @param[in] pentity: Codec entity.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_audio_err_common
 */
uint16_t bt_audio_unregister_codec(uint32_t type, PAUDIO_CODEC_ENTITY pentity);

#endif /* __BT_AUDIO_CODEC_WRAPPER_H__ */
