/**
 * @file      rtk_bt_bap.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Basic Audio Profile (BAP) type and function definition.
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_BAP_H__
#define __RTK_BT_BAP_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#define RTK_BT_LE_AUDIO_LINK_DISC_PACS_DONE 0x0001             /*!< PACS discovery is done. */
#define RTK_BT_LE_AUDIO_LINK_DISC_ASCS_DONE 0x0002             /*!< ASCS discovery is done. */
#define RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE 0x0004             /*!< CSIS discovery is done. */
#define RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE 0x0008             /*!< BASS discovery is done. */
#define RTK_BT_LE_AUDIO_LINK_DISC_CAS_DONE 0x0010              /*!< CAS discovery is done. */
/* Define Codec ID and Codec ID Length. */
#define RTK_BT_LE_LC3_CODEC_ID 0x06                            /*!< LC3 codec ID. */
#define RTK_BT_LE_AUDIO_CODEC_ID_LEN (5)                       /*!< Length of the codec ID in octets. */
/* Define Supported Audio Channel Counts. */
#define RTK_BT_LE_AUDIO_CHANNEL_COUNTS_1 0x01                  /*!< 1 audio channel is supported. */
#define RTK_BT_LE_AUDIO_CHANNEL_COUNTS_2 0x02                  /*!< 2 audio channels are supported. */
/* Define Supported Sampling Frequencies. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_8K 0x0001                 /*!< 8 kHz sampling frequency is supported. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_16K 0x0004                /*!< 16 kHz sampling frequency is supported. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_24K 0x0010                /*!< 24 kHz sampling frequency is supported. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_32K 0x0020                /*!< 32 kHz sampling frequency is supported. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_44_1K 0x0040              /*!< 44.1 kHz sampling frequency is supported. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_48K 0x0080                /*!< 48 kHz sampling frequency is supported. */
/* Define Supported Frame Durations. */
#define RTK_BT_LE_FRAME_DURATION_7_5_MS_BIT 0x01               /*!< 7.5 ms frame duration is supported. */
#define RTK_BT_LE_FRAME_DURATION_10_MS_BIT 0x02                /*!< 10 ms frame duration is supported. */
#define RTK_BT_LE_FRAME_DURATION_PREFER_7_5_MS_BIT 0x10        /*!< 7.5 ms frame duration is preferred. */
#define RTK_BT_LE_FRAME_DURATION_PREFER_10_MS_BIT 0x20         /*!< 10 ms frame duration is preferred. */
/* Define Sampling Frequency Configurations. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K 0x01               /*!< 8 kHz sampling frequency configuration value. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K 0x03              /*!< 16 kHz sampling frequency configuration value. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K 0x05              /*!< 24 kHz sampling frequency configuration value. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K 0x06              /*!< 32 kHz sampling frequency configuration value. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K 0x07            /*!< 44.1 kHz sampling frequency configuration value. */
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K 0x08              /*!< 48 kHz sampling frequency configuration value. */
/* Define Frame Duration Configurations. */
#define RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS 0x00               /*!< 7.5 ms frame duration configuration value. */
#define RTK_BT_LE_FRAME_DURATION_CFG_10_MS 0x01                /*!< 10 ms frame duration configuration value. */
/* Define Codec Configuration Types. */
#define RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY 0x01       /*!< Sampling frequency configuration type. */
#define RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION 0x02           /*!< Frame duration configuration type. */
#define RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION 0x03 /*!< Audio channel allocation configuration type. */
#define RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME 0x04    /*!< Octets per codec frame configuration type. */
#define RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU 0x05           /*!< Codec frame blocks per SDU configuration type. */
/* Define Metadata Types.
 * The Streaming_Audio_Contexts LTV structure is typically included in a Metadata parameter value when initiating the Enable or Update Metadata ASE Control operations
 * for unicast audio streams, or in the Metadata parameter value included in a BASE structure for broadcast audio streams. */
#define RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS 0x02        /*!< Streaming audio contexts metadata type. */
/* Title and/or summary of audio stream content in UTF-8 format. */
#define RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO 0x03                    /*!< Program information metadata type. */
/* 3-byte lower case language code (ISO 639-3). */
#define RTK_BT_LE_METADATA_TYPE_LANGUAGE 0x04                        /*!< Language metadata type. */
/* Array of CCID values. */
#define RTK_BT_LE_METADATA_TYPE_CCID_LIST 0x05                       /*!< CCID list metadata type. */
/* Parental rating. */
#define RTK_BT_LE_METADATA_TYPE_PARENTAL_RATING 0x06                 /*!< Parental rating metadata type. */
/* UTF-8 formatted URL used to present more information about Program_Info. */
#define RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO_URI 0x07                /*!< Program information URI metadata type. */
/* Extended metadata. */
#define RTK_BT_LE_METADATA_TYPE_EXTENDED 0xFE                        /*!< Extended metadata type. */
/* Vendor specific metadata. */
#define RTK_BT_LE_METADATA_TYPE_VENDOR_SPECIFIC 0xFF                 /*!< Vendor specific metadata type. */
/* Define Audio Context Type. */
#define RTK_BT_LE_AUDIO_CONTEXT_PROHIBITED 0x0000                    /*!< No audio context is allowed. */
#define RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED 0x0001                   /*!< Unspecified audio context. */
#define RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL 0x0002                /*!< Conversational audio. Conversation between humans. */
#define RTK_BT_LE_AUDIO_CONTEXT_MEDIA 0x0004                         /*!< Media audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_GAME 0x0008                          /*!< Game audio. Audio associated with video gaming. */
#define RTK_BT_LE_AUDIO_CONTEXT_INSTRUCTIONAL 0x0010                 /*!< Instructional audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_VOICE_ASSISTANTS 0x0020              /*!< Voice assistants audio. Man-machine communication. */
#define RTK_BT_LE_AUDIO_CONTEXT_LIVE 0x0040                          /*!< Live audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_SOUND_EFFECTS 0x0080                 /*!< Sound effects audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_NOTIFICATIONS 0x0100                 /*!< Notifications audio. Notification and reminder sounds; attention-seeking audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_RINGTONE 0x0200                      /*!< Ringtone audio. Alerts the user to an incoming call. */
#define RTK_BT_LE_AUDIO_CONTEXT_ALERTS 0x0400                        /*!< Alerts audio. Alarms and timers; immediate alerts. */
#define RTK_BT_LE_AUDIO_CONTEXT_EMERGENCY_ALERT 0x0800               /*!< Emergency alarm audio. Emergency sounds. */
#define RTK_BT_LE_AUDIO_CONTEXT_MASK 0x0FFF                          /*!< Mask of all valid audio context bits. */
/* Define Advertising Announcements Type. */
#define RTK_BT_LE_ADV_TARGETED_ANNOUNCEMENT 0x01                     /*!< Targeted announcement. */
#define RTK_BT_LE_ADV_GENERAL_ANNOUNCEMENT 0x00                      /*!< General announcement. */
/* The Preferred_Audio_Contexts LTV structure is typically included in the Metadata field of PAC records exposed by Unicast Servers and Broadcast Sinks. */
#define RTK_BT_LE_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS 0x01        /*!< Preferred audio contexts metadata type. */
/* Define Codec Specific Capability Types. */
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES 0x01 /*!< Supported sampling frequencies capability type. */
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS 0x02      /*!< Supported frame durations capability type. */
#define RTK_BT_LE_CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS 0x03           /*!< Supported audio channel counts capability type. */
/* 4 octets.
 * Octet 0-1: Minimum number of octets supported per codec frame.
 * Octet 2-3: Maximum number of octets supported per codec frame. */
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME 0x04    /*!< Supported octets per codec frame capability type. */
#define RTK_BT_LE_CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU     0x05    /*!< Maximum supported codec frames per SDU capability type. */
/* Define Service UUID. */
#define RTK_BT_LE_GATT_UUID_AICS 0x1843                                   /*!< Audio Input Control Service. */
#define RTK_BT_LE_GATT_UUID_VCS 0x1844                                    /*!< Volume Control Service. */
#define RTK_BT_LE_GATT_UUID_VOCS 0x1845                                   /*!< Volume Offset Control Service. */
#define RTK_BT_LE_GATT_UUID_CSIS 0x1846                                   /*!< Coordinated Set Identification Service. */
#define RTK_BT_LE_GATT_UUID_MICS 0x184D                                   /*!< Microphone Input Control Service. */
#define RTK_BT_LE_GATT_UUID_ASCS 0x184E                                   /*!< Audio Stream Control Service. */
#define RTK_BT_LE_GATT_UUID_BASS 0x184F                                   /*!< Broadcast Audio Scan Service. */
#define RTK_BT_LE_BASIC_AUDIO_ANNOUNCEMENT_SRV_UUID 0x1851                /*!< Basic Audio Announcement Service. */
#define RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID 0x1852            /*!< Broadcast Audio Announcement Service. */
#define RTK_BT_LE_GATT_UUID_CAS 0x1853                                    /*!< Common Audio Service. */
#define RTK_BT_LE_GATT_UUID_TMAS 0x1855                                   /*!< Telephony and Media Audio Service. */
#define RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID 0x1856            /*!< Public Broadcast Announcement Service. */
#define RTK_BT_LE_GATT_UUID_GMAS 0x1858                                   /*!< Gaming Audio Service. */
/* Define Public Broadcast Announcement Features. */
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_ENCRYPTED 0x01                /*!< The broadcast stream is encrypted. */
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_STANDARD_QUALITY_PRESENT 0x02 /*!< Standard quality configuration is present. */
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_HIGH_QUALITY_PRESENT 0x04     /*!< High quality configuration is present. */
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_MASK 0x07                     /*!< Mask of all public broadcast feature bits. */
/* Define Codec Configuration Exist Flags. */
#define RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST 0x0001               /*!< Sampling frequency configuration exists. */
#define RTK_BT_LE_CODEC_CFG_FRAME_DURATION_EXIST 0x0002                   /*!< Frame duration configuration exists. */
#define RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST 0x0004         /*!< Audio channel allocation configuration exists. */
#define RTK_BT_LE_CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST 0x0008            /*!< Octets per codec frame configuration exists. */
#define RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST 0x0010              /*!< Codec frame blocks per SDU configuration exists. */
/* Define Broadcast ID and Code Length. */
#define RTK_BT_LE_AUDIO_BROADCAST_ID_LEN (3)                              /*!< Length of the broadcast ID in octets. */
#define RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN (16)                           /*!< Length of the broadcast code in octets. */
/* Define ISO mode. */
#define RTK_BT_LE_AUDIO_CIS_MODE 0x01                                     /*!< Connected isochronous stream (CIS) mode. */
#define RTK_BT_LE_AUDIO_BIS_MODE 0x02                                     /*!< Broadcast isochronous stream (BIS) mode. */
#define RTK_BT_LE_ASCS_AES_CHAR_NUM 7                                     /*!< Number of ASEs carried in an ASCS control point indication. */
/**
 * @typedef   rtk_bt_le_audio_codec_cfg_item_t
 * @brief     Bluetooth LE Audio codec configuration items.
 */
typedef enum {
	RTK_BT_LE_CODEC_CFG_ITEM_8_1 = 0,      /*!< 8 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_8_2 = 1,      /*!< 8 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_16_1 = 2,     /*!< 16 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_16_2 = 3,     /*!< 16 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_24_1 = 4,     /*!< 24 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_24_2 = 5,     /*!< 24 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_32_1 = 6,     /*!< 32 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_32_2 = 7,     /*!< 32 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_441_1 = 8,    /*!< 44.1 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_441_2 = 9,    /*!< 44.1 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_1 = 10,    /*!< 48 kHz sampling frequency, 7.5 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_2 = 11,    /*!< 48 kHz sampling frequency, 10 ms frame duration. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_3 = 12,    /*!< 48 kHz sampling frequency, 7.5 ms frame duration, larger frame size than item 48_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_4 = 13,    /*!< 48 kHz sampling frequency, 10 ms frame duration, larger frame size than item 48_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_5 = 14,    /*!< 48 kHz sampling frequency, 7.5 ms frame duration, larger frame size than item 48_3. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_6 = 15,    /*!< 48 kHz sampling frequency, 10 ms frame duration, larger frame size than item 48_4. */
	RTK_BT_LE_CODEC_CFG_ITEM_LC3_MAX,      /*!< Number of LC3 codec configuration items. Not a valid item. */
} rtk_bt_le_audio_codec_cfg_item_t;

/**
 * @typedef   rtk_bt_le_audio_qos_cfg_type_t
 * @brief     Bluetooth LE Audio QoS configuration types.
 */
typedef enum {
	RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY = 0,        /*!< Unicast (CIS) low latency QoS configuration. */
	RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY = 1,    /*!< Unicast (CIS) high reliability QoS configuration. */
	RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY = 2,        /*!< Broadcast (BIS) low latency QoS configuration. */
	RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY = 3,    /*!< Broadcast (BIS) high reliability QoS configuration. */
	RTK_BT_LE_QOS_CFG_MAX,                        /*!< Number of QoS configuration types. Not a valid type. */
} rtk_bt_le_audio_qos_cfg_type_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_cfg_type_t
 * @brief     Bluetooth LE Audio unicast audio configuration types.
 */
typedef enum {
	RTK_BT_LE_UNICAST_AUDIO_CFG_UNKNOWN,    /*!< Unknown audio configuration. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_1,          /*!< Audio configuration 1. Data flows from client to server. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_2,          /*!< Audio configuration 2. Data flows from server to client. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_3,          /*!< Audio configuration 3. Data flows in both directions. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_4,          /*!< Audio configuration 4. Data flows from client to server with multiple audio channels. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_5,          /*!< Audio configuration 5. Data flows in both directions with multiple audio channels. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_I,        /*!< Audio configuration 6, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_II,       /*!< Audio configuration 6, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_I,        /*!< Audio configuration 7, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_II,       /*!< Audio configuration 7, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_I,        /*!< Audio configuration 8, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_II,       /*!< Audio configuration 8, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_I,        /*!< Audio configuration 9, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_II,       /*!< Audio configuration 9, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_10,         /*!< Audio configuration 10. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_I,       /*!< Audio configuration 11, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_II,      /*!< Audio configuration 11, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_MAX         /*!< Number of audio configuration types. Not a valid type. */
} rtk_bt_le_audio_unicast_cfg_type_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_cfg_type_bit_mask_t
 * @brief     Bluetooth LE Audio unicast audio configuration type bit masks.
 */
typedef enum {
	RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_1),           /*!< Bit mask of audio configuration 1. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_2_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_2),           /*!< Bit mask of audio configuration 2. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_3),           /*!< Bit mask of audio configuration 3. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_4),           /*!< Bit mask of audio configuration 4. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_5_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_5),           /*!< Bit mask of audio configuration 5. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_6_I),        /*!< Bit mask of audio configuration 6, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_6_II),      /*!< Bit mask of audio configuration 6, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_7_I),       /*!< Bit mask of audio configuration 7, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_7_II),      /*!< Bit mask of audio configuration 7, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_8_I),       /*!< Bit mask of audio configuration 8, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_8_II),      /*!< Bit mask of audio configuration 8, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_9_I),       /*!< Bit mask of audio configuration 9, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_9_II),      /*!< Bit mask of audio configuration 9, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_10_BIT   = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_10),        /*!< Bit mask of audio configuration 10. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_I_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_11_I),      /*!< Bit mask of audio configuration 11, variant I. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_11_II),    /*!< Bit mask of audio configuration 11, variant II. */
	RTK_BT_LE_UNICAST_AUDIO_CFG_MASK  =   0x1FFFE                                        /*!< Mask of all audio configuration bits. */
} rtk_bt_le_audio_unicast_cfg_type_bit_mask_t;

/**
 * @typedef   rtk_bt_le_audio_codec_cfg_item_bit_mask_t
 * @brief     Bluetooth LE Audio codec configuration item bit masks.
 */
typedef enum {
	RTK_BT_LE_CODEC_CFG_ITEM_8_1_BIT  = (1 << RTK_BT_LE_CODEC_CFG_ITEM_8_1),       /*!< Bit mask of codec configuration item 8_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_8_2_BIT  = (1 << RTK_BT_LE_CODEC_CFG_ITEM_8_2),       /*!< Bit mask of codec configuration item 8_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_16_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_16_1),      /*!< Bit mask of codec configuration item 16_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_16_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_16_2),      /*!< Bit mask of codec configuration item 16_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_24_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_24_1),      /*!< Bit mask of codec configuration item 24_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_24_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_24_2),      /*!< Bit mask of codec configuration item 24_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_32_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_32_1),      /*!< Bit mask of codec configuration item 32_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_32_2),      /*!< Bit mask of codec configuration item 32_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_441_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_441_1),    /*!< Bit mask of codec configuration item 441_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_441_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_441_2),    /*!< Bit mask of codec configuration item 441_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_1),      /*!< Bit mask of codec configuration item 48_1. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_2),      /*!< Bit mask of codec configuration item 48_2. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_3_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_3),      /*!< Bit mask of codec configuration item 48_3. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_4_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_4),      /*!< Bit mask of codec configuration item 48_4. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_5_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_5),      /*!< Bit mask of codec configuration item 48_5. */
	RTK_BT_LE_CODEC_CFG_ITEM_48_6_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_6),      /*!< Bit mask of codec configuration item 48_6. */
} rtk_bt_le_audio_codec_cfg_item_bit_mask_t;

/**
 * @typedef   rtk_bt_le_audio_bap_role_t
 * @brief     Bluetooth LE Audio BAP roles.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN = 0,         /*!< Unknown role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR = 0x01,     /*!< BAP Broadcast Source role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK = 0x02,     /*!< BAP Broadcast Sink role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI = 0x04,     /*!< BAP Broadcast Assistant role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE = 0x08,    /*!< BAP Scan Delegate role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER = 0x10,      /*!< BAP Unicast Server role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI = 0x20,      /*!< BAP Unicast Client role. */
} rtk_bt_le_audio_bap_role_t;

/**
 * @typedef   rtk_bt_le_audio_initiator_status_t
 * @brief     Bluetooth LE Audio initiator status.
 */
typedef enum {
	RTK_BT_LE_AUDIO_INITIATOR_DISABLE = 0,        /*!< Initiator is disabled. */
	RTK_BT_LE_AUDIO_INITIATOR_ENABLE,             /*!< Initiator is enabled. */
	RTK_BT_LE_AUDIO_INITIATOR_CONN,               /*!< Connected to a remote device. */
	RTK_BT_LE_AUDIO_INITIATOR_UNICAST_START,      /*!< Unicast streaming is started. */
	RTK_BT_LE_AUDIO_INITIATOR_UNICAST_STOP,       /*!< Unicast streaming is stopped. */
	RTK_BT_LE_AUDIO_INITIATOR_DISC,               /*!< Disconnected from the remote device. */
	RTK_BT_LE_AUDIO_INITIATOR_BROADCAST_START,    /*!< Broadcast streaming is started. */
	RTK_BT_LE_AUDIO_INITIATOR_BROADCAST_STOP,     /*!< Broadcast streaming is stopped. */
	RTK_BT_LE_AUDIO_INITIATOR_NOT_SUPPORT,        /*!< Initiator is not supported. */
} rtk_bt_le_audio_initiator_status_t;

/**
 * @typedef   rtk_bt_le_audio_acceptor_status_t
 * @brief     Bluetooth LE Audio acceptor status.
 */
typedef enum {
	RTK_BT_LE_AUDIO_ACCEPTOR_DISABLE = 0,           /*!< Acceptor is disabled. */
	RTK_BT_LE_AUDIO_ACCEPTOR_ENABLE,                /*!< Acceptor is enabled. */
	RTK_BT_LE_AUDIO_ACCEPTOR_CONN,                  /*!< Connected to a remote device. */
	RTK_BT_LE_AUDIO_ACCEPTOR_TX_READY,              /*!< Ready to transmit audio data. */
	RTK_BT_LE_AUDIO_ACCEPTOR_DISC,                  /*!< Disconnected from the remote device. */
	RTK_BT_LE_AUDIO_ACCEPTOR_START,                 /*!< Audio streaming is started. */
	RTK_BT_LE_AUDIO_ACCEPTOR_STOP,                  /*!< Audio streaming is stopped. */
	RTK_BT_LE_AUDIO_ACCEPTOR_BIG_SYNC,              /*!< Synchronized to a BIG. */
	RTK_BT_LE_AUDIO_ACCEPTOR_BIG_SYNC_TERMINATE,    /*!< Synchronization to the BIG is terminated. */
	RTK_BT_LE_AUDIO_ACCEPTOR_NOT_SUPPORT,           /*!< Acceptor is not supported. */
} rtk_bt_le_audio_acceptor_status_t;

/**
 * @typedef   rtk_bt_le_audio_commander_status_t
 * @brief     Bluetooth LE Audio commander status.
 */
typedef enum {
	RTK_BT_LE_AUDIO_COMMANDER_DISABLE = 0,    /*!< Commander is disabled. */
	RTK_BT_LE_AUDIO_COMMANDER_ENABLE,         /*!< Commander is enabled. */
	RTK_BT_LE_AUDIO_COMMANDER_CONN,           /*!< Connected to a remote device. */
	RTK_BT_LE_AUDIO_COMMANDER_DISC,           /*!< Disconnected from the remote device. */
	RTK_BT_LE_AUDIO_COMMANDER_NOT_SUPPORT,    /*!< Commander is not supported. */
} rtk_bt_le_audio_commander_status_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_source_state_t
 * @brief     Bluetooth LE Audio broadcast source state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE                = 0x00,    /*!< Idle. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED_STARTING = 0x01,    /*!< Transitioning to the configured state. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED          = 0x02,    /*!< Configured and not streaming. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED_STOPPING = 0x03,    /*!< Leaving the configured state. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STARTING  = 0x04,    /*!< Streaming is starting. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING           = 0x05,    /*!< Streaming. */
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STOPPING  = 0x06     /*!< Streaming is stopping. */
} rtk_bt_le_audio_broadcast_source_state_t;

/**
 * @typedef   rtk_bt_le_audio_iso_data_path_direction_t
 * @brief     Bluetooth LE Audio ISO data path directions. Specifies which directions have been configured, or which directions are to have the data path removed.
 */
typedef enum {
	RTK_BLE_AUDIO_ISO_DATA_PATH_TX  = 0x01,    /*!< Input (Host to Controller). Source or TX. */
	RTK_BLE_AUDIO_ISO_DATA_PATH_RX,            /*!< Output (Controller to Host). Sink or RX. */
} rtk_bt_le_audio_iso_data_path_direction_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_ase_state_t
 * @brief     Bluetooth LE Audio ASE state.
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_STATE_IDLE             = 0x00,    /*!< Idle. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_CODEC_CONFIGURED = 0x01,    /*!< Codec configured. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_QOS_CONFIGURED   = 0x02,    /*!< QoS configured. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_ENABLING         = 0x03,    /*!< Enabling. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_STREAMING        = 0x04,    /*!< Streaming. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_DISABLING        = 0x05,    /*!< Disabling. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_RELEASING        = 0x06,    /*!< Releasing. */
	RTK_BLE_AUDIO_ASCS_ASE_STATE_UNKNOWN          = 0xff,    /*!< Unknown state. */
} rtk_bt_le_audio_ascs_ase_state_t;

/**
 * @typedef   rtk_bt_le_audio_framed_t
 * @brief     Bluetooth LE Audio ISO PDU framing.
 */
typedef enum {
	RTK_BLE_AUDIO_UNFRAMED = 0x00,    /*!< Unframed ISO data PDU. */
	RTK_BLE_AUDIO_FRAMED   = 0x01,    /*!< Framed ISO data PDU. */
} rtk_bt_le_audio_framed_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_ase_target_latency_t
 * @brief     Bluetooth LE Audio ASE target latency.
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY      = 0x01,    /*!< Target low latency. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED           = 0x02,    /*!< Target balanced latency and reliability. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY = 0x03,    /*!< Target high reliability. */
} rtk_bt_le_audio_ascs_ase_target_latency_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_ase_target_phy_t
 * @brief     Bluetooth LE Audio ASE target PHY.
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_1M    = 0x01,    /*!< LE 1M PHY. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M    = 0x02,    /*!< LE 2M PHY. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_CODED = 0x03,    /*!< LE Coded PHY. */
} rtk_bt_le_audio_ascs_ase_target_phy_t;

/**
 * @typedef   rtk_bt_le_audio_group_dev_msg_type_t
 * @brief     Bluetooth LE Audio group device message types.
 */
typedef enum {
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN                = 0x20,    /*!< A device is connected to the group. */
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN             = 0x21,    /*!< A device in the group is disconnected. */
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE         = 0x22,    /*!< Bond information of a device in the group is removed. */
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR          = 0x23,    /*!< Bond information of all devices is cleared. */
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY               = 0x24,    /*!< The group has no device left. */
} rtk_bt_le_audio_group_dev_msg_type_t;

/**
 * @typedef   rtk_bt_le_audio_isoch_data_pkt_status_t
 * @brief     Bluetooth LE Audio Packet status flag of a received ISO data packet reported by the Controller.
 */
typedef enum {
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_VALID_DATA = 0,             /*!< Valid data. The complete SDU was received correctly. */
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_POSSIBLE_ERROR_DATA = 1,    /*!< Possibly invalid data.
                                                                     *   The contents of the SDU may contain errors or part of the SDU may be missing. */
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_LOST_DATA = 2,              /*!< Part(s) of the SDU were not received correctly. */
} rtk_bt_le_audio_isoch_data_pkt_status_t;

/**
 * @typedef   rtk_bt_le_audio_stream_state_t
 * @brief     Bluetooth LE Audio stream state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_STREAM_STATE_IDLE               = 0x00,    /*!< Idle. */
	RTK_BT_LE_AUDIO_STREAM_STATE_IDLE_CONFIGURED    = 0x01,    /*!< Idle with an audio configuration present. */
	RTK_BT_LE_AUDIO_STREAM_STATE_CONFIGURED         = 0x02,    /*!< Configured. */
	RTK_BT_LE_AUDIO_STREAM_STATE_STARTING           = 0x03,    /*!< Stream is starting. */
	RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING          = 0x04,    /*!< Streaming. */
	RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING  = 0x05,    /*!< Only some of the streams are streaming. */
	RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING           = 0x06,    /*!< Stream is stopping. */
	RTK_BT_LE_AUDIO_STREAM_STATE_RELEASING          = 0x07,    /*!< Stream is releasing. */
} rtk_bt_le_audio_stream_state_t;

/**
 * @typedef   rtk_bt_le_audio_location_t
 * @brief     Bluetooth LE Audio locations.
 */
typedef enum {
	RTK_BT_LE_AUDIO_LOCATION_MONO                = 0x00000000,    /*!< Mono. */
	RTK_BT_LE_AUDIO_LOCATION_FL                  = 0x00000001,    /*!< Front Left. */
	RTK_BT_LE_AUDIO_LOCATION_FR                  = 0x00000002,    /*!< Front Right. */
	RTK_BT_LE_AUDIO_LOCATION_FC                  = 0x00000004,    /*!< Front Center. */
	RTK_BT_LE_AUDIO_LOCATION_LFE1                = 0x00000008,    /*!< Low Frequency Effects 1. */
	RTK_BT_LE_AUDIO_LOCATION_BL                  = 0x00000010,    /*!< Back Left. */
	RTK_BT_LE_AUDIO_LOCATION_BR                  = 0x00000020,    /*!< Back Right. */
	RTK_BT_LE_AUDIO_LOCATION_FLC                 = 0x00000040,    /*!< Front Left of Center. */
	RTK_BT_LE_AUDIO_LOCATION_FRC                 = 0x00000080,    /*!< Front Right of Center. */
	RTK_BT_LE_AUDIO_LOCATION_BC                  = 0x00000100,    /*!< Back Center. */
	RTK_BT_LE_AUDIO_LOCATION_LFE2                = 0x00000200,    /*!< Low Frequency Effects 2. */
	RTK_BT_LE_AUDIO_LOCATION_SIL                 = 0x00000400,    /*!< Side Left. */
	RTK_BT_LE_AUDIO_LOCATION_SIR                 = 0x00000800,    /*!< Side Right. */
	RTK_BT_LE_AUDIO_LOCATION_TPFL                = 0x00001000,    /*!< Top Front Left. */
	RTK_BT_LE_AUDIO_LOCATION_TPFR                = 0x00002000,    /*!< Top Front Right. */
	RTK_BT_LE_AUDIO_LOCATION_TPFC                = 0x00004000,    /*!< Top Front Center. */
	RTK_BT_LE_AUDIO_LOCATION_TPC                 = 0x00008000,    /*!< Top Center. */
	RTK_BT_LE_AUDIO_LOCATION_TPBL                = 0x00010000,    /*!< Top Back Left. */
	RTK_BT_LE_AUDIO_LOCATION_TPBR                = 0x00020000,    /*!< Top Back Right. */
	RTK_BT_LE_AUDIO_LOCATION_TPSIL               = 0x00040000,    /*!< Top Side Left. */
	RTK_BT_LE_AUDIO_LOCATION_TPSIR               = 0x00080000,    /*!< Top Side Right. */
	RTK_BT_LE_AUDIO_LOCATION_TPBC                = 0x00100000,    /*!< Top Back Center. */
	RTK_BT_LE_AUDIO_LOCATION_BTFC                = 0x00200000,    /*!< Bottom Front Center. */
	RTK_BT_LE_AUDIO_LOCATION_BTFL                = 0x00400000,    /*!< Bottom Front Left. */
	RTK_BT_LE_AUDIO_LOCATION_BTFR                = 0x00800000,    /*!< Bottom Front Right. */
	RTK_BT_LE_AUDIO_LOCATION_FLW                 = 0x01000000,    /*!< Front Left Wide. */
	RTK_BT_LE_AUDIO_LOCATION_FRW                 = 0x02000000,    /*!< Front Right Wide. */
	RTK_BT_LE_AUDIO_LOCATION_LS                  = 0x04000000,    /*!< Left Surround. */
	RTK_BT_LE_AUDIO_LOCATION_RS                  = 0x08000000,    /*!< Right Surround. */
	RTK_BT_LE_AUDIO_LOCATION_RFU                 = 0xF0000000,    /*!< Reserved for future use. */
	RTK_BT_LE_AUDIO_LOCATION_MASK                = 0x0FFFFFFF     /*!< Mask of all defined audio location bits. */
} rtk_bt_le_audio_location_t;

/**
 * @typedef   rtk_bt_le_audio_big_sync_state_t
 * @brief     Bluetooth LE Audio BIG synchronization state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED = 0x00,       /*!< Terminated. */
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZING = 0x01,    /*!< Start synchronizing to a BIG. A temporary state, the result has not been received yet. */
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED = 0x02,     /*!< Synchronized to a BIG. */
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATING = 0x03,      /*!< Terminate synchronization to the BIG. A temporary state, the result has not been received yet. */
} rtk_bt_le_audio_big_sync_state_t;

/**
 * @typedef   rtk_bt_le_audio_pa_sync_state_t
 * @brief     Bluetooth LE Audio PA synchronization state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED = 0x00,                     /*!< PA synchronization is terminated. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_SCAN_IDLE = 0x01,        /*!< Start synchronizing while extended scanning is disabled.
                                                                          *   A temporary state, the result has not been received yet. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING = 0x02,    /*!< No attempt to synchronize will take place while extended scanning is disabled. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING = 0x03,                  /*!< Start synchronizing while extended scanning is enabled. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED = 0x04,                   /*!< PA synchronization is established. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATING = 0x05,                    /*!< Terminate PA synchronization. A temporary state, the result has not been received yet. */
} rtk_bt_le_audio_pa_sync_state_t;

/**
 * @typedef   rtk_bt_le_audio_bass_past_type_t
 * @brief     Bluetooth LE Audio BASS PAST type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC     = 0x00,    /*!< Initiate PAST for the local PA. */
	RTK_BT_LE_AUDIO_BASS_PAST_BY_REMOTE_SRC    = 0x01     /*!< Initiate PAST for the remote PA. */
} rtk_bt_le_audio_bass_past_type_t;

/**
 * @typedef   rtk_bt_le_audio_bass_brs_big_encrption_t
 * @brief     Bluetooth LE Audio broadcast receive state BIG encryption state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_NOT_ENCRYPTED           = 0x00,    /*!< The BIG is not encrypted. */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_BROADCAST_CODE_REQUIRED = 0x01,    /*!< The Broadcast Code is required to decrypt the BIG. */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_DECRYPTING              = 0x02,    /*!< The BIG is being decrypted. */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_BAD_CODE                = 0x03     /*!< Decryption failed because the Broadcast Code is incorrect. */
} rtk_bt_le_audio_bass_brs_big_encrption_t;

/**************************************** Event ****************************************/

/**
 * @typedef   rtk_bt_le_audio_group_handle_t
 * @brief     Bluetooth LE Audio group handle.
 */
typedef void *rtk_bt_le_audio_group_handle_t;

/**
 * @typedef   rtk_bt_le_audio_stream_session_handle_t
 * @brief     Bluetooth LE Audio stream session handle.
 */
typedef void *rtk_bt_le_audio_stream_session_handle_t;

/**
 * @typedef   rtk_bt_le_audio_device_handle_t
 * @brief     Bluetooth LE Audio device handle.
 */
typedef void *rtk_bt_le_audio_device_handle_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_source_handle_t
 * @brief     Bluetooth LE Audio broadcast source handle.
 */
typedef void *rtk_bt_le_audio_broadcast_source_handle_t;

/**
 * @typedef   rtk_bt_le_audio_sync_handle_t
 * @brief     Bluetooth LE Audio sync handle.
 */
typedef void *rtk_bt_le_audio_sync_handle_t;

/**
 * @typedef   rtk_bt_le_audio_base_data_mapping_t
 * @brief     Bluetooth LE Audio BASE data mapping handle.
 */
typedef void *rtk_bt_le_audio_base_data_mapping_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_source_config_t
 * @brief     Bluetooth LE Audio broadcast source configuration parameters.
 */
typedef struct {
	uint32_t primary_adv_interval_min;           /*!< Minimum advertising interval for undirected and low duty cycle directed advertising.
                                                  *   Range: 0x000020 to 0xFFFFFF.
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 20 ms to 10,485.759375 s. */
	uint32_t primary_adv_interval_max;           /*!< Maximum advertising interval for undirected and low duty cycle directed advertising.
                                                  *   Range: 0x000020 to 0xFFFFFF.
                                                  *   Time = N * 0.625 ms.
                                                  *   Time Range: 20 ms to 10,485.759375 s. */
	uint8_t primary_adv_channel_map;             /*!< Advertising primary channel map, bitmask of @ref rtk_bt_le_adv_ch_map_t. */
	rtk_bt_le_addr_t own_addr;                   /*!< Own address type and address. */
	rtk_bt_le_adv_filter_t filter_policy;        /*!< Advertising filter policy. */
	uint8_t tx_power;                            /*!< Advertising TX power: indicates max TX power.
                                                  *   Range: -127 to +20 dBm.
                                                  *   0x7F: Host has no preference. */
	rtk_bt_le_phy_prim_adv_t primary_adv_phy;    /*!< Primary advertising PHY. */
	uint8_t secondary_adv_max_skip;              /*!< Maximum number of primary advertising events that can be skipped before the secondary advertising packet is sent. */
	rtk_bt_le_phy_type_t secondary_adv_phy;      /*!< Secondary advertising PHY. */
	uint16_t periodic_adv_interval_min;          /*!< Minimum advertising interval for periodic advertising.
                                                  *   Should be different from periodic_adv_interval_max.
                                                  *   Range: 0x0006 to 0xFFFF.
                                                  *   Time = N * 1.25 ms.
                                                  *   Time Range: 7.5 ms to 81.91875 s. */
	uint16_t periodic_adv_interval_max;          /*!< Maximum advertising interval for periodic advertising.
                                                  *   Should be different from periodic_adv_interval_min.
                                                  *   Range: 0x0006 to 0xFFFF.
                                                  *   Time = N * 1.25 ms.
                                                  *   Time Range: 7.5 ms to 81.91875 s. */
	uint16_t periodic_adv_prop;                  /*!< Periodic advertising properties, @ref rtk_bt_le_pa_prop_t. */
} rtk_bt_le_audio_broadcast_source_config_t;

/**
 * @typedef   rtk_bt_le_audio_bap_discovery_done_ind_t
 * @brief     Bluetooth LE Audio information reported when BAP service discovery is done.
 */
typedef struct {
	uint16_t conn_handle;      /*!< Connection handle of the ACL connection.
                                *   Range: 0x0000 to 0x0EFF. */
	bool    pacs_is_found;     /*!< Whether the Published Audio Capabilities Service (PACS) is found on the remote device.
                                *   - true: PACS is found.
                                *   - false: PACS is not found. */
	uint8_t sink_pac_num;      /*!< Number of Sink PAC records found. */
	uint8_t source_pac_num;    /*!< Number of Source PAC records found. */
	bool    ascs_is_found;     /*!< Whether the Audio Stream Control Service (ASCS) is found on the remote device.
                                *   - true: ASCS is found.
                                *   - false: ASCS is not found. */
	uint8_t sink_ase_num;      /*!< Number of Sink ASEs found. */
	uint8_t source_ase_num;    /*!< Number of Source ASEs found. */
	bool    bass_is_found;     /*!< Whether the Broadcast Audio Scan Service (BASS) is found on the remote device.
                                *   - true: BASS is found.
                                *   - false: BASS is not found. */
	uint8_t brs_char_num;      /*!< Number of Broadcast Receive State characteristics found. */
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	uint8_t group_idx;         /*!< Index of the audio group that the remote device belongs to. */
#endif
} rtk_bt_le_audio_bap_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_qos_cfg_preferred_t
 * @brief     Bluetooth LE Audio preferred QoS configuration.
 */
typedef struct {
	uint32_t sdu_interval;             /*!< SDU interval, in microseconds. */
	uint8_t  framing;                  /*!< Framing of the ISO data PDUs, @ref rtk_bt_le_audio_framed_t. */
	uint16_t max_sdu;                  /*!< Maximum SDU size, in octets. */
	uint8_t  retransmission_number;    /*!< Number of retransmissions of each ISO data PDU. */
	uint16_t max_transport_latency;    /*!< Maximum transport latency, in milliseconds. */
	uint32_t presentation_delay;       /*!< Presentation delay, in microseconds. */
} rtk_bt_le_audio_qos_cfg_preferred_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_ase_qos_t
 * @brief     Bluetooth LE Audio unicast ASE QoS parameters.
 */
typedef struct {
	uint8_t  phy;                      /*!< PHY used by the ASE. */
	uint8_t  retransmission_number;    /*!< Number of retransmissions of each ISO data PDU. */
	uint16_t max_sdu;                  /*!< Maximum SDU size, in octets. */
} rtk_bt_le_audio_unicast_ase_qos_t;

/**
 * @typedef   rtk_bt_le_audio_bis_conn_handle_info_t
 * @brief     Bluetooth LE Audio BIS connection handle information.
 */
typedef struct {
	uint8_t bis_idx;             /*!< BIS index. */
	uint16_t bis_conn_handle;    /*!< Connection handle of a BIS.
                                  *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_audio_bis_conn_handle_info_t;

/**
 * @typedef   rtk_bt_le_audio_bis_conn_info_t
 * @brief     Bluetooth LE Audio BIS connection information used when setting up the data path of a synchronized BIG.
 */
typedef struct {
	void *sync_handle;           /*!< Synchronization handle of the broadcast sync. */
	uint16_t bis_conn_handle;    /*!< Connection handle of the BIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t bis_idx;             /*!< BIS index. */
	uint8_t path_direction;      /*!< ISO data path direction, @ref rtk_bt_le_audio_iso_data_path_direction_t. */
} rtk_bt_le_audio_bis_conn_info_t;

/**
 * @typedef   rtk_bt_le_audio_bass_cp_bis_info_t
 * @brief     Bluetooth LE Audio BASS broadcast receive state characteristic subgroup information.
 */
typedef struct {
	uint32_t bis_sync_state;    /*!< BIS synchronization state bitfield of the subgroup. */
	uint8_t metadata_len;       /*!< Length of the subgroup metadata, in octets. */
	uint8_t *p_metadata;        /*!< Subgroup metadata. */
} rtk_bt_le_audio_bass_cp_bis_info_t;

/**
 * @typedef   rtk_bt_le_audio_pa_sync_establish_t
 * @brief     Bluetooth LE Audio parameters for establishing PA synchronization.
 */
typedef struct {
	uint8_t options;          /*!< Bitmask of @ref rtk_bt_le_pa_sync_create_opt_t. */
	uint8_t sync_cte_type;    /*!< Bitmask of @ref rtk_bt_le_pa_sync_cte_t specifying which CTE types to exclude. */
	uint16_t skip;            /*!< The maximum number of periodic advertising events that can be skipped after a successful receive.
                               *   Range: 0x0000 to 0x01F3. */
	uint16_t sync_timeout;    /*!< Synchronization timeout for the periodic advertising train.
                               *   Range: 0x000A to 0x4000.
                               *   Time = N * 10 ms.
                               *   Time Range: 100 ms to 163.84 s. */
} rtk_bt_le_audio_pa_sync_establish_t;

/**
 * @typedef   rtk_bt_le_audio_bass_brs_data_t
 * @brief     Bluetooth LE Audio BASS broadcast receive state characteristic data.
 */
typedef struct {
	bool brs_is_used;                                          /*!< Whether this broadcast receive state is in use.
                                                                *   - true: The broadcast receive state is in use.
                                                                *   - false: The broadcast receive state is not in use. */
	uint8_t source_id;                                         /*!< Source ID identifying the broadcast receive state. */
	rtk_bt_le_addr_t source_address;                           /*!< Address type and address of the broadcast source advertiser. */
	uint8_t source_adv_sid;                                    /*!< Advertising SID of the broadcast source. */
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];    /*!< Broadcast ID of the broadcast source. */
	uint8_t pa_sync_state;                                     /*!< PA synchronization state. */
	uint8_t big_encryption;                                    /*!< BIG encryption state, @ref rtk_bt_le_audio_bass_brs_big_encrption_t. */
	uint8_t bad_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];      /*!< Incorrect Broadcast Code that failed to decrypt the BIG. */
	uint8_t num_subgroups;                                     /*!< Number of subgroups. */
	uint16_t bis_info_size;                                    /*!< Size of the subgroup BIS information. */
	rtk_bt_le_audio_bass_cp_bis_info_t *p_cp_bis_info;         /*!< Subgroup BIS information. */
} rtk_bt_le_audio_bass_brs_data_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_cis_conn_info_t
 * @brief     Bluetooth LE Audio CIS connection information.
 */
typedef struct {
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t cis_conn_handle;    /*!< Connection handle of the CIS.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t  cig_id;             /*!< CIG ID. */
	uint8_t  cis_id;             /*!< CIS ID. */
} rtk_bt_le_audio_ascs_cis_conn_info_t;

/**
 * @typedef   rtk_bt_le_audio_cfg_codec_t
 * @brief     Bluetooth LE Audio codec configuration.
 */
typedef struct {
	uint16_t type_exist;                   /*!< Bitmask indicating which codec configuration fields are present. */
	uint8_t frame_duration;                /*!< Frame duration.
                                            *   0x00: 7.5 ms.
                                            *   0x01: 10 ms. */
	uint8_t sample_frequency;              /*!< Sampling frequency configuration.
                                            *   0x01: 8 kHz.
                                            *   0x03: 16 kHz.
                                            *   0x05: 24 kHz.
                                            *   0x06: 32 kHz.
                                            *   0x07: 44.1 kHz.
                                            *   0x08: 48 kHz. */
	uint8_t codec_frame_blocks_per_sdu;    /*!< Number of codec frame blocks per SDU. */
	uint16_t octets_per_codec_frame;       /*!< Number of octets per codec frame. */
	uint32_t audio_channel_allocation;     /*!< Audio channel allocation bitmask, @ref rtk_bt_le_audio_location_t. */
} rtk_bt_le_audio_cfg_codec_t;

/**
 * @typedef   rtk_bt_le_audio_iso_chann_t
 * @brief     Bluetooth LE Audio ISO channel information.
 */
typedef struct {
	void *p_iso_chann;                                           /*!< ISO channel handle. */
	uint16_t iso_conn_handle;                                    /*!< ISO connection handle.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t iso_interval;                                       /*!< ISO interval.
                                                                  *   Range: 0x0004 to 0x0C80.
                                                                  *   Time = N * 1.25 ms.
                                                                  *   Time Range: 5 ms to 4 s. */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /*!< Audio path direction. */
} rtk_bt_le_audio_iso_chann_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_setup_data_path_ind_t
 * @brief     Bluetooth LE Audio ASCS setup data path indication.
 */
typedef struct {
	uint16_t conn_handle;                                        /*!< Connection handle of the ACL connection.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t ase_id;                                              /*!< ASE ID. */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /*!< Audio path direction. */
	uint16_t cis_conn_handle;                                    /*!< Connection handle of the CIS.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_cfg_codec_t codec_cfg;                       /*!< Codec configuration of the ASE. */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                     /*!< ISO channel information. */
	uint16_t source_ase_num;                                     /*!< Number of Source ASEs, used by the TX path to wait until all devices are ready. */
	uint32_t presentation_delay;                                 /*!< Presentation delay, in microseconds. */
	uint32_t transport_latency_m_to_s;                           /*!< The actual transport latency from central to peripheral, in microseconds. */
	uint32_t transport_latency_s_to_m;                           /*!< The actual transport latency from peripheral to central, in microseconds. */
} rtk_bt_le_audio_ascs_setup_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_remove_data_path_ind_t
 * @brief     Bluetooth LE Audio ASCS remove data path indication.
 */
typedef struct {
	uint16_t conn_handle;                                        /*!< Connection handle of the ACL connection.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t  ase_id;                                             /*!< ASE ID. */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /*!< Audio path direction. */
	uint16_t cis_conn_handle;                                    /*!< Connection handle of the CIS.
                                                                  *   Range: 0x0000 to 0x0EFF. */
} rtk_bt_le_audio_ascs_remove_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_ase_state_ind_t
 * @brief     Bluetooth LE Audio ASCS ASE state indication.
 */
typedef struct {
	uint16_t    conn_handle;                           /*!< Connection handle of the ACL connection.
                                                        *   Range: 0x0000 to 0x0EFF. */
	uint8_t     ase_id;                                /*!< ASE ID. */
	uint8_t     direction;                             /*!< Direction of the ASE (sink or source). */
	rtk_bt_le_audio_ascs_ase_state_t     ase_state;    /*!< ASE state. */
} rtk_bt_le_audio_ascs_ase_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t
 * @brief     Bluetooth LE Audio ASCS codec configuration control point indication.
 */
typedef struct {
	uint16_t    conn_handle;                  /*!< Connection handle of the ACL connection.
                                               *   Range: 0x0000 to 0x0EFF. */
	uint8_t     ase_id;                       /*!< ASE ID. */
	rtk_bt_le_audio_cfg_codec_t codec_cfg;    /*!< Codec configuration requested by the remote device. */
} rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t
 * @brief     Bluetooth LE Audio ASCS QoS configuration control point indication.
 */
typedef struct {
	uint16_t    conn_handle;              /*!< Connection handle of the ACL connection.
                                           *   Range: 0x0000 to 0x0EFF. */
	uint8_t     ase_id;                   /*!< ASE ID. */
	uint8_t     cig_id;                   /*!< CIG ID. */
	uint8_t     cis_id;                   /*!< CIS ID. */
	uint8_t     framing;                  /*!< Framing of the ISO data PDUs, @ref rtk_bt_le_audio_framed_t. */
	uint8_t     phy;                      /*!< PHY used by the ASE. */
	uint8_t     retransmission_number;    /*!< Number of retransmissions of each ISO data PDU. */
	uint16_t    max_sdu;                  /*!< Maximum SDU size, in octets. */
	uint16_t    max_transport_latency;    /*!< Maximum transport latency, in milliseconds. */
	uint32_t    sdu_interval;             /*!< SDU interval, in microseconds. */
	uint32_t    presentation_delay;       /*!< Presentation delay, in microseconds. */
} rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_cp_enable_ind_t
 * @brief     Bluetooth LE Audio ASCS enable control point indication.
 */
typedef struct {
	uint16_t    conn_handle;                            /*!< Connection handle of the ACL connection.
                                                         *   Range: 0x0000 to 0x0EFF. */
	uint8_t     ase_num;                                /*!< Number of ASEs to be enabled. */
	uint8_t     ase_id[RTK_BT_LE_ASCS_AES_CHAR_NUM];    /*!< IDs of the ASEs to be enabled. Only the first ase_num entries are valid. */
} rtk_bt_le_audio_ascs_cp_enable_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_cp_disable_ind_t
 * @brief     Bluetooth LE Audio ASCS disable control point indication.
 */
typedef rtk_bt_le_audio_ascs_cp_enable_ind_t rtk_bt_le_audio_ascs_cp_disable_ind_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_prefer_qos_data_t
 * @brief     Bluetooth LE Audio ASCS preferred QoS data supplied by the application.
 */
typedef struct {
	uint8_t  supported_framing;                   /*!< Supported framing, @ref rtk_bt_le_audio_framed_t. */
	uint8_t  preferred_phy;                       /*!< Preferred PHY.
                                                   *   0: Use the target PHY requested by the remote device. */
	uint8_t  preferred_retrans_number;            /*!< Preferred number of retransmissions.
                                                   *   0: Use the default value derived from the codec configuration. */
	uint16_t max_transport_latency;               /*!< Maximum transport latency, in milliseconds.
                                                   *   0: Use the default value derived from the codec configuration. */
	uint32_t presentation_delay_min;              /*!< Minimum presentation delay, in microseconds. */
	uint32_t presentation_delay_max;              /*!< Maximum presentation delay, in microseconds.
                                                   *   0: Use the default value derived from the codec configuration. */
	uint32_t preferred_presentation_delay_min;    /*!< Preferred minimum presentation delay, in microseconds. */
	uint32_t preferred_presentation_delay_max;    /*!< Preferred maximum presentation delay, in microseconds.
                                                   *   0: Use the default value derived from the codec configuration. */
} rtk_bt_le_audio_ascs_prefer_qos_data_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_get_prefer_qos_ind_t
 * @brief     Bluetooth LE Audio ASCS get preferred QoS indication.
 */
typedef struct {
	uint16_t conn_handle;                                         /*!< Connection handle of the ACL connection.
                                                                   *   Range: 0x0000 to 0x0EFF. */
	uint8_t ase_id;                                               /*!< ASE ID. */
	uint8_t direction;                                            /*!< Direction of the ASE (sink or source). */
	rtk_bt_le_audio_cfg_codec_t codec_cfg;                        /*!< Codec configuration of the ASE. */
	rtk_bt_le_audio_ascs_ase_target_latency_t target_latency;     /*!< Target latency of the ASE. */
	rtk_bt_le_audio_ascs_ase_target_phy_t target_phy;             /*!< Target PHY of the ASE. */
	rtk_bt_le_audio_ascs_prefer_qos_data_t *p_prefer_qos_data;    /*!< Preferred QoS data to be filled by the application. */
} rtk_bt_le_audio_ascs_get_prefer_qos_ind_t;

/**
 * @typedef   rtk_bt_le_audio_bass_pa_sync_param_t
 * @brief     Bluetooth LE Audio BASS PA synchronization parameters.
 */
typedef struct {
	uint8_t pa_sync_options;     /*!< Bitmask of @ref rtk_bt_le_pa_sync_create_opt_t. */
	uint16_t pa_sync_skip;       /*!< The maximum number of periodic advertising events that can be skipped after a successful receive.
                                  *   Range: 0x0000 to 0x01F3. */
	uint16_t pa_sync_timeout;    /*!< Synchronization timeout for the periodic advertising train.
                                  *   Range: 0x000A to 0x4000.
                                  *   Time = N * 10 ms.
                                  *   Time Range: 100 ms to 163.84 s. */
	uint16_t past_timeout;       /*!< Synchronization timeout for PAST.
                                  *   Range: 0x000A to 0x4000.
                                  *   Time = N * 10 ms.
                                  *   Time Range: 100 ms to 163.84 s. */
} rtk_bt_le_audio_bass_pa_sync_param_t;

/**
 * @typedef   rtk_bt_le_audio_bass_get_pa_sync_param_ind_t
 * @brief     Bluetooth LE Audio BASS get PA synchronization parameter indication.
 */
typedef struct {
	rtk_bt_le_audio_sync_handle_t sync_handle;                /*!< Synchronization handle. */
	uint8_t source_id;                                        /*!< Source ID of the broadcast receive state. */
	bool    is_past;                                          /*!< Whether PA synchronization is established through PAST.
                                                               *   - true: PA synchronization is established through PAST.
                                                               *   - false: PA synchronization is not established through PAST. */
	uint16_t pa_interval;                                     /*!< Periodic advertising interval of the broadcast source. */
	rtk_bt_le_audio_bass_pa_sync_param_t *p_pa_sync_param;    /*!< PA synchronization parameters to be filled by the application. */
} rtk_bt_le_audio_bass_get_pa_sync_param_ind_t;

/**
 * @typedef   rtk_bt_le_audio_bass_big_sync_param_t
 * @brief     Bluetooth LE Audio BASS BIG synchronization parameters.
 */
typedef struct {
	uint8_t big_mse;              /*!< The maximum number of subevents that a Controller should use to receive data payloads in each interval for a BIS.
                                   *   0x00: The Controller can schedule reception of any number of subevents up to NSE.
                                   *   0x01 to 0x1F: Maximum number of subevents that should be used to receive data payloads in each BIS event. */
	uint16_t big_sync_timeout;    /*!< Maximum permitted time between successful receptions of BIS PDUs.
                                   *   Range: 0x000A to 0x4000.
                                   *   Time = N * 10 ms.
                                   *   Time Range: 100 ms to 163.84 s. */
} rtk_bt_le_audio_bass_big_sync_param_t;

/**
 * @typedef   rtk_bt_le_audio_bass_get_big_sync_param_ind_t
 * @brief     Bluetooth LE Audio BASS get BIG synchronization parameter indication.
 */
typedef struct {
	rtk_bt_le_audio_sync_handle_t sync_handle;                  /*!< Synchronization handle. */
	uint8_t source_id;                                          /*!< Source ID of the broadcast receive state. */
	rtk_bt_le_audio_bass_big_sync_param_t *p_big_sync_param;    /*!< BIG synchronization parameters to be filled by the application. */
} rtk_bt_le_audio_bass_get_big_sync_param_ind_t;

/**
 * @typedef   rtk_bt_le_audio_pa_sync_state_ind_t
 * @brief     Bluetooth LE Audio PA synchronization state indication.
 */
typedef struct {
	uint8_t cause;                                /*!< Cause code of the PA synchronization state change, truncated to 8 bits from the stack cause. */
	rtk_bt_le_audio_sync_handle_t sync_handle;    /*!< Synchronization handle. */
	uint8_t sync_state;                           /*!< PA synchronization state, @ref rtk_bt_le_audio_pa_sync_state_t. */
	uint16_t action;                              /*!< Action that caused the PA synchronization state change.
                                                   *   0x00: Idle.
                                                   *   0x01: Synchronize.
                                                   *   0x02: Terminate.
                                                   *   0x03: Synchronization lost. */
} rtk_bt_le_audio_pa_sync_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_big_sync_state_ind_t
 * @brief     Bluetooth LE Audio BIG synchronization state indication.
 */
typedef struct {
	uint8_t sync_state;                                        /*!< BIG synchronization state, @ref rtk_bt_le_audio_big_sync_state_t. */
	uint8_t encryption;                                        /*!< Whether the BIG data is encrypted.
                                                                *   0x00: Unencrypted.
                                                                *   0x01: Encrypted. */
	uint8_t action;                                            /*!< BIG synchronization action.
                                                                *   0x00: Idle.
                                                                *   0x01: Synchronize.
                                                                *   0x02: Terminate.
                                                                *   0x03: Synchronization lost. */
	uint8_t action_role;                                       /*!< Initiator of the BIG synchronization action.
                                                                *   0x00: Idle.
                                                                *   0x01: Local API.
                                                                *   0x02: BASS. */
	uint8_t cause;                                             /*!< Cause code of the BIG synchronization state change, truncated to 8 bits from the stack cause. */
	uint8_t adv_type;                                          /*!< Advertiser address type. */
	uint8_t adv_addr_val[RTK_BD_ADDR_LEN];                     /*!< Advertiser address. */
	uint8_t adv_sid;                                           /*!< Advertising SID of the broadcast source. */
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];    /*!< Broadcast ID of the broadcast source. */
} rtk_bt_le_audio_big_sync_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_big_setup_data_path_ind_t
 * @brief     Bluetooth LE Audio BIG setup data path indication.
 */
typedef struct {
	uint8_t bis_idx;                              /*!< BIS index. */
	uint16_t bis_conn_handle;                     /*!< Connection handle of the BIS.
                                                   *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                               /*!< Result of the data path setup. */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;      /*!< ISO channel information. */
	rtk_bt_le_audio_cfg_codec_t codec_t;          /*!< Codec configuration. */
	rtk_bt_le_audio_sync_handle_t sync_handle;    /*!< Synchronization handle. */
	uint32_t presentation_delay;                  /*!< Presentation delay, in microseconds. */
	uint32_t transport_latency_big;               /*!< BIG transport latency, in microseconds. */
} rtk_bt_le_audio_big_setup_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_big_info_adv_report_t
 * @brief     Bluetooth LE Audio BIG Info advertising report.
 */
typedef struct {
	uint8_t sync_id;                                 /*!< Identifies the periodic advertising train. */
	uint16_t sync_handle;                            /*!< Sync handle identifying the periodic advertising train.
                                                      *   Range: 0x0000 to 0x0EFF. */
	uint8_t num_bis;                                 /*!< The number of BISes in the BIG. */
	uint8_t nse;                                     /*!< The number of subevents per BIS in each BIG event. */
	uint16_t iso_interval;                           /*!< Time between two adjacent BIG anchor points.
                                                      *   Range: 0x0004 to 0x0C80.
                                                      *   Time = N * 1.25 ms.
                                                      *   Time Range: 5 ms to 4 s. */
	uint8_t bn;                                      /*!< Burst number: the number of new payloads in each BIS event. */
	uint8_t pto;                                     /*!< Pre-transmission offset: the offset, in units of ISO_Interval, used for pre-transmission of future data. */
	uint8_t irc;                                     /*!< Immediate repetition count: the number of times a scheduled data packet is sent,
                                                      *   i.e. the number of groups that carry the data of the current BIS event. */
	uint16_t max_pdu;                                /*!< The maximum number of data octets (excluding the MIC, if any) that can be carried in each BIS Data PDU
                                                      *   in the BIG. */
	uint32_t sdu_interval;                           /*!< The time interval of the periodic SDUs, in microseconds. */
	uint16_t max_sdu;                                /*!< The maximum size of an SDU, in octets. */
	uint8_t phy;                                     /*!< PHY used by the BIG. */
	uint8_t framing;                                 /*!< Whether the BIG carries framed or unframed data.
                                                      *   0x00: Unframed.
                                                      *   0x01: Framed. */
	uint8_t encryption;                              /*!< Whether the BIG is encrypted.
                                                      *   0x00: The BIG carries unencrypted data.
                                                      *   0x01: The BIG carries encrypted data. */
	rtk_bt_le_audio_sync_handle_t cb_sync_handle;    /*!< Synchronization handle. */
} rtk_bt_le_audio_big_info_adv_report_t;

/**
 * @typedef   rtk_bt_le_audio_big_remove_data_path_ind_t
 * @brief     Bluetooth LE Audio BIG remove data path indication.
 */
typedef struct {
	uint8_t bis_idx;                                             /*!< BIS index. */
	uint16_t bis_conn_handle;                                    /*!< Connection handle of the BIS.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                                              /*!< Result of the data path removal. */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /*!< Audio path direction. */
	rtk_bt_le_audio_sync_handle_t sync_handle;                   /*!< Synchronization handle. */
} rtk_bt_le_audio_big_remove_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_source_state_ind_t
 * @brief     Bluetooth LE Audio broadcast source state indication.
 */
typedef struct {
	uint8_t cause;                                                        /*!< Result code of the broadcast source state change. */
	rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle;    /*!< Broadcast source handle. */
	uint8_t broadcast_source_state;                                       /*!< Broadcast source state, @ref rtk_bt_le_audio_broadcast_source_state_t. */
} rtk_bt_le_audio_broadcast_source_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_bap_big_setup_data_path_ind_t
 * @brief     Bluetooth LE Audio BAP BIG setup data path indication.
 */
typedef struct {
	uint8_t bis_idx;                            /*!< BIS index. */
	uint16_t bis_conn_handle;                   /*!< Connection handle of the BIS.
                                                 *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                             /*!< Result of the data path setup. */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;    /*!< ISO channel information. */
	rtk_bt_le_audio_cfg_codec_t codec_t;        /*!< Codec configuration. */
	uint32_t presentation_delay;                /*!< Presentation delay, in microseconds. */
	uint32_t transport_latency_big;             /*!< BIG transport latency, in microseconds. */
} rtk_bt_le_audio_bap_big_setup_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_bap_big_remove_data_path_ind_t
 * @brief     Bluetooth LE Audio BAP BIG remove data path indication.
 */
typedef struct {
	uint8_t bis_idx;                                             /*!< BIS index. */
	uint16_t bis_conn_handle;                                    /*!< Connection handle of the BIS.
                                                                  *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                                              /*!< Result of the data path removal. */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /*!< Audio path direction. */
} rtk_bt_le_audio_bap_big_remove_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_group_dev_msg_ind_t
 * @brief     Bluetooth LE Audio group device message indication.
 */
typedef struct {
	uint16_t cause;                                   /*!< Result code of the group device message. */
	rtk_bt_le_audio_group_dev_msg_type_t msg_type;    /*!< Group device message type. */
	rtk_bt_le_audio_group_handle_t group_handle;      /*!< Group handle. */
	rtk_bt_le_audio_device_handle_t device_handle;    /*!< Device handle. */
} rtk_bt_le_audio_group_dev_msg_ind_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_session_qos_t
 * @brief     Bluetooth LE Audio unicast session QoS.
 */
typedef struct {
	uint8_t  sca;                          /*!< Worst-case sleep clock accuracy of all the peripherals that will participate in the CIG.
                                            *   0x00: 251 ppm to 500 ppm.
                                            *   0x01: 151 ppm to 250 ppm.
                                            *   0x02: 101 ppm to 150 ppm.
                                            *   0x03: 76 ppm to 100 ppm.
                                            *   0x04: 51 ppm to 75 ppm.
                                            *   0x05: 31 ppm to 50 ppm.
                                            *   0x06: 21 ppm to 30 ppm.
                                            *   0x07: 0 ppm to 20 ppm. */
	uint8_t  packing;                      /*!< Packing scheme of the ISO data PDUs.
                                            *   0x00: Sequential.
                                            *   0x01: Interleaved. */
	uint8_t  framing;                      /*!< Framing of the ISO data PDUs, @ref rtk_bt_le_audio_framed_t. */
	uint32_t sdu_interval_m_s;             /*!< SDU interval from central to peripheral, in microseconds. */
	uint32_t sdu_interval_s_m;             /*!< SDU interval from peripheral to central, in microseconds. */
	uint16_t latency_m_s;                  /*!< Maximum transport latency from central to peripheral, in milliseconds. */
	uint16_t latency_s_m;                  /*!< Maximum transport latency from peripheral to central, in milliseconds. */
	uint32_t sink_presentation_delay;      /*!< Presentation delay of the audio sink, in microseconds. */
	uint32_t source_presentation_delay;    /*!< Presentation delay of the audio source, in microseconds. */
} rtk_bt_le_audio_unicast_session_qos_t;

/**
 * @typedef   rtk_bt_le_audio_bap_setup_data_path_ind_t
 * @brief     Bluetooth LE Audio BAP setup data path indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                      /*!< Group handle. */
	rtk_bt_le_audio_stream_session_handle_t stream_session_handle;    /*!< Stream session handle. */
	rtk_bt_le_audio_device_handle_t device_handle;                    /*!< Device handle. */
	uint8_t ase_id;                                                   /*!< ASE ID. */
	uint8_t path_direction;                                           /*!< Audio path direction, @ref rtk_bt_le_audio_iso_data_path_direction_t. */
	uint16_t cis_conn_handle;                                         /*!< Connection handle of the CIS.
                                                                       *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_cfg_codec_t codec_parsed_data;                    /*!< Codec configuration of the ASE. */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                          /*!< ISO channel information. */
	uint8_t dev_num;                                                  /*!< Number of devices in the group, used by the TX path to wait until all devices are ready. */
	uint32_t presentation_delay;                                      /*!< Presentation delay, in microseconds. */
	uint32_t transport_latency_m_to_s;                                /*!< The actual transport latency from central to peripheral, in microseconds. */
	uint32_t transport_latency_s_to_m;                                /*!< The actual transport latency from peripheral to central, in microseconds. */
} rtk_bt_le_audio_bap_setup_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_bap_remove_data_path_ind_t
 * @brief     Bluetooth LE Audio BAP remove data path indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                      /*!< Group handle. */
	rtk_bt_le_audio_stream_session_handle_t stream_session_handle;    /*!< Stream session handle. */
	rtk_bt_le_audio_device_handle_t device_handle;                    /*!< Device handle. */
	uint8_t ase_id;                                                   /*!< ASE ID. */
	uint8_t path_direction;                                           /*!< Audio path direction, @ref rtk_bt_le_audio_iso_data_path_direction_t. */
	uint16_t cis_conn_handle;                                         /*!< Connection handle of the CIS.
                                                                       *   Range: 0x0000 to 0x0EFF. */
	uint16_t cause;                                                   /*!< Result of the data path removal. */
} rtk_bt_le_audio_bap_remove_data_path_ind_t;

/**
 * @typedef   rtk_bt_le_audio_direct_iso_data_ind_t
 * @brief     Bluetooth LE Audio Indication of a received ISO data packet.
 */
typedef struct {
	uint16_t iso_conn_handle;                                   /*!< Connection handle of the ISO.
                                                                 *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_isoch_data_pkt_status_t pkt_status_flag;    /*!< Packet status of the received SDU. */
	uint16_t  offset;                                           /*!< Offset from the start of p_buf to the start of the SDU.
                                                                 *   e.g. p_buf + offset indicates the start of the SDU. */
	uint16_t  iso_sdu_len;                                      /*!< Length of the SDU, in octets. */
	uint16_t  pkt_seq_num;                                      /*!< Sequence number of the SDU. */
	bool      ts_flag;                                          /*!< Whether time_stamp is valid.
                                                                 *   - true: time_stamp is valid.
                                                                 *   - false: time_stamp is not valid. */
	uint32_t  time_stamp;                                       /*!< Time stamp of the SDU, in microseconds.
                                                                 *   Only valid when ts_flag is true. */
	uint16_t  buf_len;                                          /*!< Total length of p_buf, in octets. */
	uint8_t   *p_buf;                                           /*!< Pointer to the buffer that holds the received ISO data. */
} rtk_bt_le_audio_direct_iso_data_ind_t;

/* act */
/**
 * @typedef   rtk_bt_le_audio_ext_scan_config_t
 * @brief     Bluetooth LE Audio extended scan configuration.
 */
typedef struct {
	rtk_bt_le_ext_scan_param_t *p_param;    /*!< Extended scan parameters. */
	bool enable;                            /*!< Whether to start or stop the extended scan.
                                             *   - true: Start the extended scan.
                                             *   - false: Stop the extended scan. */
} rtk_bt_le_audio_ext_scan_config_t;

/**
 * @typedef   rtk_bt_le_audio_iso_data_send_info_t
 * @brief     Bluetooth LE Audio ISO data send information.
 */
typedef struct {
	uint16_t iso_conn_handle;    /*!< Connection handle of the ISO.
                                  *   Range: 0x0000 to 0x0EFF. */
	bool ts_flag;                /*!< Whether time_stamp is used.
                                  *   - true: time_stamp is used.
                                  *   - false: time_stamp is not used. */
	uint32_t time_stamp;         /*!< Time stamp of the SDU, in microseconds.
                                  *   Only used when ts_flag is true. */
	uint16_t pkt_seq_num;        /*!< Sequence number of the SDU. */
	uint16_t data_len;           /*!< Length of the SDU to be sent, in octets. */
	uint8_t *p_data;             /*!< Pointer to the SDU data to be sent. */
} rtk_bt_le_audio_iso_data_send_info_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_client_start_t
 * @brief     Bluetooth LE Audio unicast client start parameters.
 */
typedef struct {
	uint8_t group_idx;          /*!< Group index. */
	uint16_t sink_context;      /*!< Audio contexts requested for the sink direction.
                                 *   0: Sink context availability is not checked. */
	uint16_t source_context;    /*!< Audio contexts requested for the source direction.
                                 *   0: Source context availability is not checked. */
} rtk_bt_le_audio_unicast_client_start_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_client_stop_t
 * @brief     Bluetooth LE Audio unicast client stop parameters.
 */
typedef struct {
	uint8_t group_idx;          /*!< Group index. */
	uint32_t cis_timeout_ms;    /*!< Timeout for stopping the CIS, in milliseconds. */
} rtk_bt_le_audio_unicast_client_stop_t;

/**
 * @typedef   rtk_bt_le_audio_bap_param_config_type_t
 * @brief     Bluetooth LE Audio BAP parameter configuration type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BAP_BROADCAST_SOURCE_CONFIG = 0x01,    /*!< Broadcast source parameters, carried in cfg.brs_config. */
	RTK_BT_LE_AUDIO_BAP_BROADCAST_CODE_CONFIG = 0x02,      /*!< Broadcast code, carried in cfg.broadcast_code. */
	RTK_BT_LE_AUDIO_BAP_UNICAST_QOS_CONFIG = 0x03          /*!< Unicast QoS parameters, carried in cfg.unicast_config. */
} rtk_bt_le_audio_bap_param_config_type_t;

/**
 * @typedef   rtk_bt_le_audio_unicast_config_t
 * @brief     Bluetooth LE Audio unicast configuration parameters.
 */
typedef struct {
	rtk_bt_le_audio_unicast_ase_qos_t bap_ase_qos;        /*!< ASE QoS parameters. */
	rtk_bt_le_audio_unicast_session_qos_t session_qos;    /*!< Session QoS parameters. */
} rtk_bt_le_audio_unicast_config_t;

/**
 * @typedef   rtk_bt_le_audio_bap_param_config_t
 * @brief     Bluetooth LE Audio BAP parameter configuration.
 */
typedef struct {
	rtk_bt_le_audio_bap_param_config_type_t cfg_type;                  /*!< Configuration type that selects the valid member of cfg. */
	union {
		rtk_bt_le_audio_broadcast_source_config_t brs_config;          /*!< Broadcast source parameters. */
		uint8_t broadcast_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];    /*!< Broadcast code of the encrypted BIG, 16 octets. */
		rtk_bt_le_audio_unicast_config_t unicast_config;               /*!< Unicast QoS parameters. */
	} cfg;
} rtk_bt_le_audio_bap_param_config_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_source_create_param_t
 * @brief     Bluetooth LE Audio broadcast source create parameters.
 */
typedef struct {
	rtk_bt_le_audio_codec_cfg_item_t cfg_codec_index;         /*!< Codec configuration item of the broadcast stream. */
	rtk_bt_le_audio_qos_cfg_type_t cfg_qos_type;              /*!< QoS configuration type of the broadcast stream. */
	bool manual_qos_flag;                                     /*!< Whether to use the manual QoS parameters.
                                                               *   - true: p_manual_qos_cfg is used as the QoS parameters.
                                                               *   - false: The QoS parameters are derived from cfg_codec_index and cfg_qos_type. */
	rtk_bt_le_audio_qos_cfg_preferred_t *p_manual_qos_cfg;    /*!< Manual QoS parameters.
                                                               *   Only used when manual_qos_flag is true, and must not be NULL in that case. */
	rtk_bt_le_addr_type_t local_addr_type;                    /*!< Local address type used by the broadcast source. */
	bool encryption;                                          /*!< Whether the BIG is encrypted.
                                                               *   - true: The BIG carries encrypted data.
                                                               *   - false: The BIG carries unencrypted data. */
	uint16_t stream_audio_contexts;                           /*!< Audio contexts of the broadcast stream. */
} rtk_bt_le_audio_broadcast_source_create_param_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_sync_create_param_t
 * @brief     Bluetooth LE Audio broadcast sink PA sync create parameters.
 */
typedef struct {
	rtk_bt_le_addr_t adv_addr;                                 /*!< Address of the broadcast source. */
	uint8_t adv_sid;                                           /*!< Advertising SID of the broadcast source.
                                                                *   Range: 0x00 to 0x0F. */
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];    /*!< Broadcast ID of the broadcast source, 3 octets. */
} rtk_bt_le_audio_broadcast_sync_create_param_t;


/**
 * @typedef   rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t
 * @brief     Bluetooth LE Audio broadcast assistant remote scan parameters.
 */
typedef struct {
	bool enable;          /*!< Whether to start or stop the remote scan.
                           *   - true: Start the remote scan.
                           *   - false: Stop the remote scan. */
	uint8_t group_idx;    /*!< Group index of the Scan Delegators. */
} rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t;

/**
 * @typedef   rtk_bt_le_audio_broadcast_assistant_reception_act_param_t
 * @brief     Bluetooth LE Audio broadcast assistant reception action parameters.
 */
typedef struct {
	rtk_bt_le_addr_t addr;    /*!< Address of the broadcast source. */
	uint8_t group_idx;        /*!< Group index of the Scan Delegators. */
} rtk_bt_le_audio_broadcast_assistant_reception_act_param_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_bap BT BAP APIs
 * @brief     BT BAP related function APIs.
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     Allocate the LE Audio link resources for a newly established LE ACL connection. Call it when event @ref RTK_BT_LE_GAP_EVT_CONNECT_IND
 *            is received and the connection succeeded.
 * @param[in] conn_ind: Connection indication of the established LE connection.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_link_alloc(rtk_bt_le_conn_ind_t *conn_ind);

/**
 * @brief     Free the LE Audio link resources of an LE ACL connection that has been disconnected. Call it when event @ref RTK_BT_LE_GAP_EVT_DISCONN_IND is received.
 * @param[in] disconn_ind: Disconnection indication of the closed LE connection.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_link_free(rtk_bt_le_disconn_ind_t *disconn_ind);

/**
 * @brief     Send one ISO SDU over an established ISO data path.
 * @param[in] info: ISO data to send.
 *                  Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API fails with @ref RTK_BT_ERR_PARAM_INVALID if the ISO connection handle is 0, the data length is 0, or the data pointer is NULL.
 */
uint16_t rtk_bt_le_audio_iso_data_send(rtk_bt_le_audio_iso_data_send_info_t *info);

/**
 * @brief     Start the unicast audio stream of a group as a unicast client. A session is created, codec and QoS are configured, and the stream is started
 *            with the connected devices of the group.
 * @param[in] group_idx: Index of the group to start.
 * @param[in] sink_context: Audio contexts requested for the sink direction.
 *                          0: Sink context availability is not checked.
 * @param[in] source_context: Audio contexts requested for the source direction.
 *                            0: Source context availability is not checked.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The group must exist and have connected devices that have finished discovery.
 *            - This API fails if the stream of the group is already starting, streaming, stopping, or releasing.
 */
uint16_t rtk_bt_bap_unicast_client_start(uint8_t group_idx, uint16_t sink_context, uint16_t source_context);

/**
 * @brief     Stop the unicast audio stream of a group as a unicast client.
 * @param[in] group_idx: Index of the group to stop.
 * @param[in] cis_timeout_ms: Timeout for stopping the CIS, in milliseconds.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API fails if the stream of the group is not active.
 */
uint16_t rtk_bt_bap_unicast_client_stop(uint8_t group_idx, uint32_t cis_timeout_ms);

/**
 * @brief     Release the unicast audio session of a group as a unicast client.
 * @param[in] group_idx: Index of the group whose session is released.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API fails if no session exists for the group.
 */
uint16_t rtk_bt_bap_unicast_client_release(uint8_t group_idx);

/**
 * @brief     Configure BAP parameters, such as the broadcast source parameters, the broadcast code, or the unicast QoS.
 * @param[in] p_param: BAP parameter configuration.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The BAP must be initialized before calling this API, otherwise it fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_bap_param_config(rtk_bt_le_audio_bap_param_config_t *p_param);

/**
 * @brief     Create the broadcast source with the given codec, QoS and encryption settings.
 * @param[in] p_param: Broadcast source creation parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Only one broadcast source can exist at a time, otherwise this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_le_audio_broadcast_source_create(rtk_bt_le_audio_broadcast_source_create_param_t *p_param);

/**
 * @brief     Start the broadcast source and begin streaming. The result is reported asynchronously via event @ref RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The broadcast source must be created by @ref rtk_bt_le_audio_broadcast_source_create first, otherwise this API fails with @ref RTK_BT_FAIL.
 */

uint16_t rtk_bt_le_audio_broadcast_source_start(void);

/**
 * @brief     Stop the broadcast source and end streaming. The result is reported asynchronously via event @ref RTK_BT_LE_AUDIO_EVT_BROADCAST_SOURCE_STATE_IND.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The broadcast source must be created by @ref rtk_bt_le_audio_broadcast_source_create first, otherwise this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_le_audio_broadcast_source_stop(void);

/**
 * @brief     Start or stop the extended scan used to find broadcast sources.
 * @param[in] enable: Whether to start or stop the extended scan.
 *                    - true: Start the extended scan.
 *                    - false: Stop the extended scan.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_ext_scan_act(bool enable);

/**
 * @brief     Create PA sync with a broadcast source. The extended scan is started if it is not already running. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND.
 * @param[in] addr: Address of the target broadcast source.
 * @param[in] adv_sid: Advertising SID of the target broadcast source.
 *                     Range: 0x00 to 0x0F.
 * @param[in] broadcast_id: Broadcast ID of the target broadcast source, 3 octets.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API fails if PA sync with the broadcast source is already established or in progress.
 */
uint16_t rtk_bt_le_audio_broadcast_pa_sync_create(rtk_bt_le_addr_t addr, uint8_t adv_sid, uint8_t *broadcast_id);

/**
 * @brief     Terminate PA sync with a broadcast source. The result is reported asynchronously via event @ref RTK_BT_LE_AUDIO_EVT_PA_SYNC_STATE_IND.
 * @param[in] addr: Address of the target broadcast source.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must have been created by @ref rtk_bt_le_audio_broadcast_pa_sync_create, otherwise this API fails.
 */
uint16_t rtk_bt_le_audio_broadcast_pa_sync_terminate(rtk_bt_le_addr_t addr);

/**
 * @brief     Create BIG sync with a broadcast source identified by address. The result is reported asynchronously via event @ref RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND.
 * @param[in] addr: Address of the target broadcast source.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must be created by @ref rtk_bt_le_audio_broadcast_pa_sync_create first, otherwise this API fails.
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_create(rtk_bt_le_addr_t addr);

/**
 * @brief     Create BIG sync with a broadcast source identified by sync handle. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND.
 * @param[in] sync_handle: Synchronization handle of the target broadcast source.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must be created by @ref rtk_bt_le_audio_broadcast_pa_sync_create first, otherwise this API fails.
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_create_by_handle(rtk_bt_le_audio_sync_handle_t sync_handle);

/**
 * @brief     Terminate BIG sync with a broadcast source identified by address. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND.
 * @param[in] addr: Address of the target broadcast source.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must have been created, otherwise this API fails.
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_terminate(rtk_bt_le_addr_t addr);

/**
 * @brief     Terminate BIG sync with a broadcast source identified by sync handle. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_BIG_SYNC_STATE_IND.
 * @param[in] sync_handle: Synchronization handle of the target broadcast source.
 *                         Range: 0x0000 to 0x0EFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must have been created, otherwise this API fails.
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_terminate_by_handle(rtk_bt_le_audio_sync_handle_t sync_handle);

/**
 * @brief     Start or stop the remote scan of the Scan Delegators in a group, on behalf of the broadcast assistant. The local extended scan is started or stopped too.
 * @param[in] enable: Whether to start or stop the remote scan.
 *                    - true: Start the remote scan.
 *                    - false: Stop the remote scan.
 * @param[in] group_idx: Index of the group of the Scan Delegators.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The group must exist and have connected devices, otherwise this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_remote_ext_scan_act(bool enable, uint8_t group_idx);

/**
 * @brief     Add a broadcast source to the Scan Delegators in a group, and transfer the synchronization information of the source to them.
 * @param[in] addr: Address of the broadcast source.
 * @param[in] group_idx: Index of the group of the Scan Delegators.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must be created by @ref rtk_bt_le_audio_broadcast_pa_sync_create first, otherwise this API fails
 *              with @ref RTK_BT_ERR_PARAM_INVALID.
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_start(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Modify the broadcast source in the Broadcast Receive State characteristic of the Scan Delegators in a group, so that they stop synchronizing to it.
 * @param[in] addr: Address of the broadcast source.
 * @param[in] group_idx: Index of the group of the Scan Delegators.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must have been created, otherwise this API fails with @ref RTK_BT_ERR_PARAM_INVALID.
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_stop(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Remove the broadcast source from the Broadcast Receive State characteristic of the Scan Delegators in a group.
 * @param[in] addr: Address of the broadcast source.
 * @param[in] group_idx: Index of the group of the Scan Delegators.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - PA sync with the broadcast source must have been created, otherwise this API fails with @ref RTK_BT_ERR_PARAM_INVALID.
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_remove(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Allocate a group.
 * @param[out] p_group_handle: Output for the allocated group handle.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_group_allocate(rtk_bt_le_audio_group_handle_t *p_group_handle);

/**
 * @}
 */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_BAP_H__ */
