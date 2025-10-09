/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef __SBC_CODEC_ENTITY_H__
#define __SBC_CODEC_ENTITY_H__

#include <dlist.h>
#include <oi_status.h>
#include <oi_codec_sbc.h>
#include <sbc_encoder.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_audio_config.h>

#define RTK_A2DP_SINK 0x01

typedef enum {
	SBC_MODE_STANDARD = 0,
	SBC_MODE_mSBC
} sbc_mode_t;

typedef enum {
	SBC_ALLOCATION_METHOD_LOUDNESS = 0,
	SBC_ALLOCATION_METHOD_SNR
} sbc_allocation_method_t;

typedef enum {
	SBC_CHANNEL_MODE_MONO = 0,
	SBC_CHANNEL_MODE_DUAL_CHANNEL,
	SBC_CHANNEL_MODE_STEREO,
	SBC_CHANNEL_MODE_JOINT_STEREO
} sbc_channel_mode_t;

/**
 * @typedef rtk_bt_sbc_encode_t
 * @brief   structure of sbc encoder init
 */
typedef struct {
	sbc_channel_mode_t      sbc_enc_mode;           /*!< sbc encode mode */
	uint8_t                 sbc_pkt_num;            /*!< sbc encode packet number */
	int                     blocks;                 /*!< sbc blocks */
	int                     subbands;               /*!< sbc subbands */
	sbc_allocation_method_t alloc_method;           /*!< sbc alloc method */
	int                     sample_rate;            /*!< sbc sample rate */
	int                     bitpool;                /*!< sbc bitpool */
	sbc_channel_mode_t      channel_mode;           /*!< sbc channel mode */
	uint16_t                frame_size;             /*!< sbc frame size */
} rtk_bt_sbc_encode_t;

/**
 * @typedef rtk_bt_sbc_decode_t
 * @brief   structure of sbc decoder init
 */
typedef struct {
	sbc_channel_mode_t      sbc_dec_mode;           /*!< sbc decode mode */
	uint32_t                sampling_frequency;     /*!< indicate sample frequence */
	sbc_channel_mode_t      channel_mode;           /*!< indicate channel mode */
	uint8_t                 block_length;           /*!< indicate block length */
	uint8_t                 subbands;               /*!< indicate subbands */
	sbc_allocation_method_t allocation_method;      /*!< indicate allocation method */
	uint8_t                 min_bitpool;            /*!< indicate min bitpool */
	uint8_t                 max_bitpool;            /*!< indicate max bitpool */
} rtk_bt_sbc_decode_t;

/**
 * @typedef rtk_bt_sbc_codec_t
 * @brief   structure of sbc codec init
 */
typedef struct {
	rtk_bt_sbc_encode_t encoder_t;                                  /*!< encoder structure */
	rtk_bt_sbc_decode_t decoder_t;                                  /*!< decoder structure */
} rtk_bt_sbc_codec_t;

typedef struct {
	uint8_t                         fragmentation;
	uint8_t                         starting_packet; // of fragmented SBC frame
	uint8_t                         last_packet;     // of fragmented SBC frame
	uint8_t                         num_frames;
} avdtp_sbc_codec_header_t;

typedef struct {
	uint8_t                         syncword;
	uint8_t                         sampling_frequency: 2;
	uint8_t                         blocks: 2;
	uint8_t                         channel_mode: 2;
	uint8_t                         allocation_method: 1;
	uint8_t                         subbands: 1;
	uint8_t                         bitpool;
	uint8_t                         crc_check;
} sbc_frame_header_t;

uint16_t rtk_bt_audio_sbc_register(uint32_t type, PAUDIO_CODEC_ENTITY p_entity);

#endif /* __SBC_CODEC_ENTITY_H__ */