/**
 * @file      rtk_bt_bap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
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
#define RTK_BT_LE_AUDIO_LINK_DISC_PACS_DONE 0x0001
#define RTK_BT_LE_AUDIO_LINK_DISC_ASCS_DONE 0x0002
#define RTK_BT_LE_AUDIO_LINK_DISC_CSIS_DONE 0x0004
#define RTK_BT_LE_AUDIO_LINK_DISC_BASS_DONE 0x0008
#define RTK_BT_LE_AUDIO_LINK_DISC_CAS_DONE 0x0010
/* Define Codec ID and Codec ID Length.*/
#define RTK_BT_LE_LC3_CODEC_ID 0x06
#define RTK_BT_LE_AUDIO_CODEC_ID_LEN (5)
/* Define Supported Audio Channel Counts.*/
#define RTK_BT_LE_AUDIO_CHANNEL_COUNTS_1 0x01
#define RTK_BT_LE_AUDIO_CHANNEL_COUNTS_2 0x02
/* Define Supported Sampling Frequencies.*/
#define RTK_BT_LE_SAMPLING_FREQUENCY_8K 0x0001
#define RTK_BT_LE_SAMPLING_FREQUENCY_16K 0x0004
#define RTK_BT_LE_SAMPLING_FREQUENCY_24K 0x0010
#define RTK_BT_LE_SAMPLING_FREQUENCY_32K 0x0020
#define RTK_BT_LE_SAMPLING_FREQUENCY_44_1K 0x0040
#define RTK_BT_LE_SAMPLING_FREQUENCY_48K 0x0080
/* Define Supported Frame Durations.*/
#define RTK_BT_LE_FRAME_DURATION_7_5_MS_BIT 0x01
#define RTK_BT_LE_FRAME_DURATION_10_MS_BIT 0x02
#define RTK_BT_LE_FRAME_DURATION_PREFER_7_5_MS_BIT 0x10
#define RTK_BT_LE_FRAME_DURATION_PREFER_10_MS_BIT 0x20
/* Define Sampling Frequency Configurations.*/
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K 0x01
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K 0x03
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_24K 0x05
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K 0x06
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_44_1K 0x07
#define RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K 0x08
/* Define Frame Duration Configurations.*/
#define RTK_BT_LE_FRAME_DURATION_CFG_7_5_MS 0x00
#define RTK_BT_LE_FRAME_DURATION_CFG_10_MS 0x01
//Config Codec operation
#define RTK_BT_LE_CODEC_CFG_TYPE_SAMPLING_FREQUENCY 0x01
#define RTK_BT_LE_CODEC_CFG_TYPE_FRAME_DURATION 0x02
#define RTK_BT_LE_CODEC_CFG_TYPE_AUDIO_CHANNEL_ALLOCATION 0x03
#define RTK_BT_LE_CODEC_CFG_TYPE_OCTET_PER_CODEC_FRAME 0x04
#define RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU 0x05
/*The Streaming_Audio_Contexts LTV structure is typically included in a Metadata parameter value
when initiating the Enable or Update Metadata ASE Control operations for unicast Audio Streams,
or in the Metadata parameter value included in a BASE structure for broadcast Audio Streams.*/
#define RTK_BT_LE_METADATA_TYPE_STREAMING_AUDIO_CONTEXTS 0x02
/*Title and/or summary of Audio Stream content: UTF-8 format. */
#define RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO 0x03
/*3-byte, lower case language code as defined in ISO 639-3. */
#define RTK_BT_LE_METADATA_TYPE_LANGUAGE 0x04
/*Array of CCID values. */
#define RTK_BT_LE_METADATA_TYPE_CCID_LIST 0x05
/*Parental_Rating. */
#define RTK_BT_LE_METADATA_TYPE_PARENTAL_RATING 0x06
/*A UTF-8 formatted URL link used to present more information about Program_Info. */
#define RTK_BT_LE_METADATA_TYPE_PROGRAM_INFO_URI 0x07
/*Extended Metadata. */
#define RTK_BT_LE_METADATA_TYPE_EXTENDED 0xFE
/*Vendor_Specific. */
#define RTK_BT_LE_METADATA_TYPE_VENDOR_SPECIFIC 0xFF
/* Define Audio Context Type.*/
#define RTK_BT_LE_AUDIO_CONTEXT_PROHIBITED 0x0000
#define RTK_BT_LE_AUDIO_CONTEXT_UNSPECIFIED 0x0001        /**< Unspecified. */
#define RTK_BT_LE_AUDIO_CONTEXT_CONVERSATIONAL 0x0002     /**< Conversational. Conversation between humans. */
#define RTK_BT_LE_AUDIO_CONTEXT_MEDIA 0x0004              /**< Media. */
#define RTK_BT_LE_AUDIO_CONTEXT_GAME 0x0008               /**< Game. Audio associated with video gaming. */
#define RTK_BT_LE_AUDIO_CONTEXT_INSTRUCTIONAL 0x0010      /**< Instructional. Instructional audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_VOICE_ASSISTANTS 0x0020   /**< Voice assistants. Man-machine communication. */
#define RTK_BT_LE_AUDIO_CONTEXT_LIVE 0x0040               /**< Live. Live audio */
#define RTK_BT_LE_AUDIO_CONTEXT_SOUND_EFFECTS 0x0080      /**< Sound effects. Sound effects including keyboard and touch feedback; menu and user interface sounds; and other system sounds. */
#define RTK_BT_LE_AUDIO_CONTEXT_NOTIFICATIONS 0x0100      /**< Notifications. Notification and reminder sounds; attention-seeking audio. */
#define RTK_BT_LE_AUDIO_CONTEXT_RINGTONE 0x0200           /**< Ringtone. Alerts the user to an incoming call. */
#define RTK_BT_LE_AUDIO_CONTEXT_ALERTS 0x0400             /**< Alerts. Alarms and timers; immediate alerts. */
#define RTK_BT_LE_AUDIO_CONTEXT_EMERGENCY_ALERT 0x0800    /**< Emergency alarm. Emergency alarm Emergency sounds. */
#define RTK_BT_LE_AUDIO_CONTEXT_MASK 0x0FFF
/* Define Advertising Announcements Type.*/
#define RTK_BT_LE_ADV_TARGETED_ANNOUNCEMENT 0x01
#define RTK_BT_LE_ADV_GENERAL_ANNOUNCEMENT 0x00
/*The Preferred_Audio_Contexts LTV structure is typically included
in the Metadata field of PAC records exposed by Unicast Servers and Broadcast Sinks. */
#define RTK_BT_LE_METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS 0x01
/* Codec_Specific_Capabilities parameters*/
//Codec Specific Capability Types
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_SAMPLING_FREQUENCIES 0x01
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_FRAME_DURATIONS 0x02
#define RTK_BT_LE_CODEC_CAP_TYPE_AUDIO_CHANNEL_COUNTS 0x03
/*4 octets.
Octet 0-1: Minimum number of octets supported per codec frame
Octet 2-3: Maximum number of octets supported per codec frame*/
#define RTK_BT_LE_CODEC_CAP_TYPE_SUPPORTED_OCTETS_PER_CODEC_FRAME 0x04
#define RTK_BT_LE_CODEC_CAP_TYPE_MAX_SUPPORTED_FRAMES_PER_SDU     0x05
/* Define Service UUID */
#define RTK_BT_LE_GATT_UUID_AICS 0x1843                          /**< Audio Input Control Service */
#define RTK_BT_LE_GATT_UUID_VCS 0x1844                           /**< Volume Control Service */
#define RTK_BT_LE_GATT_UUID_VOCS 0x1845                          /**< Volume Offset Control Service */
#define RTK_BT_LE_GATT_UUID_CSIS 0x1846                          /**< Coordinated Set Identification Service */
#define RTK_BT_LE_GATT_UUID_MICS 0x184D                          /**< Microphone Input Control Service */
#define RTK_BT_LE_GATT_UUID_ASCS 0x184E                          /**< Audio Stream Control Service */
#define RTK_BT_LE_GATT_UUID_BASS 0x184F                          /**< Broadcast Audio Scan Service */
#define RTK_BT_LE_BASIC_AUDIO_ANNOUNCEMENT_SRV_UUID 0x1851       /**< Basic Audio Announcement Service */
#define RTK_BT_LE_BROADCAST_AUDIO_ANNOUNCEMENT_SRV_UUID 0x1852   /**< Broadcast Audio Announcement Service */
#define RTK_BT_LE_GATT_UUID_CAS 0x1853                           /**< Common Audio Service */
#define RTK_BT_LE_GATT_UUID_TMAS 0x1855                          /**< Telephony and Media Audio Service */
#define RTK_BT_LE_PUBIC_BROADCAST_ANNOUNCEMENT_SRV_UUID 0x1856   /**< Public Broadcast Announcement Service */
#define RTK_BT_LE_GATT_UUID_GMAS 0x1858                          /**< Gaming Audio Service */
//Public Broadcast Announcement features
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_ENCRYPTED 0x01
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_STANDARD_QUALITY_PRESENT 0x02
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_HIGH_QUALITY_PRESENT 0x04
#define RTK_BT_LE_AUDIO_PUBIC_BROADCAST_BIT_MASK 0x07
/* Define Codec Configuration Exist Flags.*/
#define RTK_BT_LE_CODEC_CFG_SAMPLING_FREQUENCY_EXIST 0x0001
#define RTK_BT_LE_CODEC_CFG_FRAME_DURATION_EXIST 0x0002
#define RTK_BT_LE_CODEC_CFG_AUDIO_CHANNEL_ALLOCATION_EXIST 0x0004
#define RTK_BT_LE_CODEC_CFG_OCTET_PER_CODEC_FRAME_EXIST 0x0008
#define RTK_BT_LE_CODEC_CFG_TYPE_BLOCKS_PER_SDU_EXIST 0x0010
/* Define Broadcast ID and Code Length.*/
#define RTK_BT_LE_AUDIO_BROADCAST_ID_LEN (3)
#define RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN (16)
/* Define ISO mode.*/
#define RTK_BT_LE_AUDIO_CIS_MODE 0x01
#define RTK_BT_LE_AUDIO_BIS_MODE 0x02
#define RTK_BT_LE_ASCS_AES_CHAR_NUM 7
/**
 * @typedef    rtk_bt_le_audio_codec_cfg_item_t
 * @brief     Bluetooth LE audio codec cfg item. see in BAP P86 and P111.
 */
typedef enum {
	RTK_BT_LE_CODEC_CFG_ITEM_8_1 = 0,
	RTK_BT_LE_CODEC_CFG_ITEM_8_2 = 1,
	RTK_BT_LE_CODEC_CFG_ITEM_16_1 = 2,
	RTK_BT_LE_CODEC_CFG_ITEM_16_2 = 3,
	RTK_BT_LE_CODEC_CFG_ITEM_24_1 = 4,
	RTK_BT_LE_CODEC_CFG_ITEM_24_2 = 5,
	RTK_BT_LE_CODEC_CFG_ITEM_32_1 = 6,
	RTK_BT_LE_CODEC_CFG_ITEM_32_2 = 7,
	RTK_BT_LE_CODEC_CFG_ITEM_441_1 = 8,
	RTK_BT_LE_CODEC_CFG_ITEM_441_2 = 9,
	RTK_BT_LE_CODEC_CFG_ITEM_48_1 = 10,
	RTK_BT_LE_CODEC_CFG_ITEM_48_2 = 11,
	RTK_BT_LE_CODEC_CFG_ITEM_48_3 = 12,
	RTK_BT_LE_CODEC_CFG_ITEM_48_4 = 13,
	RTK_BT_LE_CODEC_CFG_ITEM_48_5 = 14,
	RTK_BT_LE_CODEC_CFG_ITEM_48_6 = 15,
	RTK_BT_LE_CODEC_CFG_ITEM_LC3_MAX,
} rtk_bt_le_audio_codec_cfg_item_t;

/**
 * @typedef    rtk_bt_le_audio_qos_cfg_type_t
 * @brief     Bluetooth LE audio qos cfg type item. see in BAP P86 and P111.
 */
typedef enum {
	RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY = 0,
	RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY = 1,
	RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY = 2,
	RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY = 3,
	RTK_BT_LE_QOS_CFG_MAX,
} rtk_bt_le_audio_qos_cfg_type_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_cfg_type_t
 * @brief     Bluetooth LE audio unicast cfg type. see in BAP P50.
 */
typedef enum {
	RTK_BT_LE_UNICAST_AUDIO_CFG_UNKNOWN,
	RTK_BT_LE_UNICAST_AUDIO_CFG_1,                      /* ---->  */
	RTK_BT_LE_UNICAST_AUDIO_CFG_2,                      /* <----  */
	RTK_BT_LE_UNICAST_AUDIO_CFG_3,                      /* <---->  */
	RTK_BT_LE_UNICAST_AUDIO_CFG_4,                      /* ---->>  */
	RTK_BT_LE_UNICAST_AUDIO_CFG_5,                      /* <---->>  */
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_I,
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_II,
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_I,
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_II,
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_I,
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_II,
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_I,
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_II,
	RTK_BT_LE_UNICAST_AUDIO_CFG_10,
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_I,
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_II,
	RTK_BT_LE_UNICAST_AUDIO_CFG_MAX
} rtk_bt_le_audio_unicast_cfg_type_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_cfg_type_bit_mask_t
 * @brief     Bluetooth LE audio codec cfg type bit mask.
 */
typedef enum {
	RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_1),
	RTK_BT_LE_UNICAST_AUDIO_CFG_2_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_2),
	RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_3),
	RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_4),
	RTK_BT_LE_UNICAST_AUDIO_CFG_5_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_5),
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_6_I),
	RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_6_II),
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_7_I),
	RTK_BT_LE_UNICAST_AUDIO_CFG_7_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_7_II),
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_8_I),
	RTK_BT_LE_UNICAST_AUDIO_CFG_8_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_8_II),
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_I_BIT  = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_9_I),
	RTK_BT_LE_UNICAST_AUDIO_CFG_9_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_9_II),
	RTK_BT_LE_UNICAST_AUDIO_CFG_10_BIT   = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_10),
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_I_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_11_I),
	RTK_BT_LE_UNICAST_AUDIO_CFG_11_II_BIT = (1 << RTK_BT_LE_UNICAST_AUDIO_CFG_11_II),
	RTK_BT_LE_UNICAST_AUDIO_CFG_MASK  =   0x1FFFE
} rtk_bt_le_audio_unicast_cfg_type_bit_mask_t;

/**
 * @typedef    rtk_bt_le_audio_codec_cfg_item_bit_mask_t
 * @brief     Bluetooth LE audio codec cfg item bit mask.
 */
typedef enum {
	RTK_BT_LE_CODEC_CFG_ITEM_8_1_BIT  = (1 << RTK_BT_LE_CODEC_CFG_ITEM_8_1),
	RTK_BT_LE_CODEC_CFG_ITEM_8_2_BIT  = (1 << RTK_BT_LE_CODEC_CFG_ITEM_8_2),
	RTK_BT_LE_CODEC_CFG_ITEM_16_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_16_1),
	RTK_BT_LE_CODEC_CFG_ITEM_16_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_16_2),
	RTK_BT_LE_CODEC_CFG_ITEM_24_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_24_1),
	RTK_BT_LE_CODEC_CFG_ITEM_24_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_24_2),
	RTK_BT_LE_CODEC_CFG_ITEM_32_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_32_1),
	RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_32_2),
	RTK_BT_LE_CODEC_CFG_ITEM_441_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_441_1),
	RTK_BT_LE_CODEC_CFG_ITEM_441_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_441_2),
	RTK_BT_LE_CODEC_CFG_ITEM_48_1_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_1),
	RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_2),
	RTK_BT_LE_CODEC_CFG_ITEM_48_3_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_3),
	RTK_BT_LE_CODEC_CFG_ITEM_48_4_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_4),
	RTK_BT_LE_CODEC_CFG_ITEM_48_5_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_5),
	RTK_BT_LE_CODEC_CFG_ITEM_48_6_BIT = (1 << RTK_BT_LE_CODEC_CFG_ITEM_48_6),
} rtk_bt_le_audio_codec_cfg_item_bit_mask_t;

/**
 * @typedef    rtk_bt_le_audio_bap_role_t
 * @brief     Bluetooth LE AUDIO BAP Role.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BAP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR = 0x01,                   /**< BAP Broadcast Source role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK = 0x02,                   /**< BAP Broadcast Sink role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI = 0x04,                   /**< BAP Broadcast Assistant role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE = 0x08,                  /**< BAP Scan Delegate role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER = 0x10,                    /**< BAP Unicast Server role. */
	RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI = 0x20,                    /**< BAP Unicast Client role. */
} rtk_bt_le_audio_bap_role_t;

typedef enum {
	RTK_BT_LE_AUDIO_INITIATOR_DISABLE = 0,
	RTK_BT_LE_AUDIO_INITIATOR_ENABLE,
	RTK_BT_LE_AUDIO_INITIATOR_CONN,
	RTK_BT_LE_AUDIO_INITIATOR_UNICAST_START,
	RTK_BT_LE_AUDIO_INITIATOR_UNICAST_STOP,
	RTK_BT_LE_AUDIO_INITIATOR_DISC,
	RTK_BT_LE_AUDIO_INITIATOR_BROADCAST_START,
	RTK_BT_LE_AUDIO_INITIATOR_BROADCAST_STOP,
	RTK_BT_LE_AUDIO_INITIATOR_NOT_SUPPORT,
} rtk_bt_le_audio_initiator_status_t;

typedef enum {
	RTK_BT_LE_AUDIO_ACCEPTOR_DISABLE = 0,
	RTK_BT_LE_AUDIO_ACCEPTOR_ENABLE,
	RTK_BT_LE_AUDIO_ACCEPTOR_CONN,
	RTK_BT_LE_AUDIO_ACCEPTOR_TX_READY,
	RTK_BT_LE_AUDIO_ACCEPTOR_DISC,
	RTK_BT_LE_AUDIO_ACCEPTOR_START,
	RTK_BT_LE_AUDIO_ACCEPTOR_STOP,
	RTK_BT_LE_AUDIO_ACCEPTOR_BIG_SYNC,
	RTK_BT_LE_AUDIO_ACCEPTOR_BIG_SYNC_TERMINATE,
	RTK_BT_LE_AUDIO_ACCEPTOR_NOT_SUPPORT,
} rtk_bt_le_audio_acceptor_status_t;

typedef enum {
	RTK_BT_LE_AUDIO_COMMANDER_DISABLE = 0,
	RTK_BT_LE_AUDIO_COMMANDER_ENABLE,
	RTK_BT_LE_AUDIO_COMMANDER_CONN,
	RTK_BT_LE_AUDIO_COMMANDER_DISC,
	RTK_BT_LE_AUDIO_COMMANDER_NOT_SUPPORT,
} rtk_bt_le_audio_commander_status_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_source_state_t
 * @brief     Bluetooth LE audio state for broadcast source.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_IDLE                = 0x00,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED_STARTING = 0x01,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED          = 0x02,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_CONFIGURED_STOPPING = 0x03,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STARTING  = 0x04,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING           = 0x05,
	RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STATE_STREAMING_STOPPING  = 0x06
} rtk_bt_le_audio_broadcast_source_state_t;

/**
 * @typedef   rtk_bt_le_audio_iso_data_path_direction_t
 * @brief     Specify which directions have been configured.Or use the combination of macro definitions to specify which directions are to have the data path removed
*/
typedef enum {
	RTK_BLE_AUDIO_ISO_DATA_PATH_TX  = 0x01,    /**< Input (Host to Controller). Source or Tx*/
	RTK_BLE_AUDIO_ISO_DATA_PATH_RX,           /**< Output (Controller to Host). Sink or Rx*/
} rtk_bt_le_audio_iso_data_path_direction_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_ase_state_t
 * @brief     enum of LE audio ascs ase state
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_STATE_IDLE             = 0x00,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_CODEC_CONFIGURED = 0x01,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_QOS_CONFIGURED   = 0x02,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_ENABLING         = 0x03,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_STREAMING        = 0x04,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_DISABLING        = 0x05,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_RELEASING        = 0x06,
	RTK_BLE_AUDIO_ASCS_ASE_STATE_UNKNOWN          = 0xff,
} rtk_bt_le_audio_ascs_ase_state_t;

/**
 * @typedef    rtk_bt_le_audio_framed_t
 * @brief     enum of LE audio support unframed or framed iso pdu
 */
typedef enum {
	RTK_BLE_AUDIO_UNFRAMED = 0x00,
	RTK_BLE_AUDIO_FRAMED   = 0x01,
} rtk_bt_le_audio_framed_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_ase_target_latency_t
 * @brief     enum of LE audio ascs ase target latency
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY      = 0x01,    /**< Target low latency. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED           = 0x02,    /**< Target balanced latency and reliability. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY = 0x03,    /**< Target high reliability. */
} rtk_bt_le_audio_ascs_ase_target_latency_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_ase_target_phy_t
 * @brief     enum of LE audio ascs ase target phy
 */
typedef enum {
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_1M    = 0x01,      /**< LE 1M PHY. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_2M    = 0x02,      /**< LE 2M PHY. */
	RTK_BLE_AUDIO_ASCS_ASE_TARGET_PHY_CODED = 0x03,      /**< LE Coded PHY. */
} rtk_bt_le_audio_ascs_ase_target_phy_t;

/**
 * @typedef    rtk_bt_le_audio_group_dev_msg_type_t
 * @brief     Bluetooth LE audio group device message type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_CONN                = 0x20,
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_DISCONN             = 0x21,
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_REMOVE         = 0x22,
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_BOND_CLEAR          = 0x23,
	RTK_BT_LE_AUDIO_GROUP_MSG_DEV_EMPTY               = 0x24,
} rtk_bt_le_audio_group_dev_msg_type_t;

/**
 * @typedef   rtk_bt_le_iso_isoch_data_pkt_status_t
 * @brief     Packet Status Flag in Bluetooth LE HCI ISO Datat packet sent by the Controller
*/
typedef enum {
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_VALID_DATA = 0,             /**< Valid data. The complete SDU was received correctly. */
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_POSSIBLE_ERROR_DATA = 1,    /**< Possibly invalid data. The contents of the SDU may
                                                                        contain errors or part of the SDU may be missing.
                                                                        This is reported as "data with possible errors". */
	RTK_BT_LE_ISO_ISOCH_DATA_PKT_STATUS_LOST_DATA = 2,              /**< Part(s) of the SDU were not received correctly.
                                                                        This is reported as "lost data". */
} rtk_bt_le_audio_isoch_data_pkt_status_t;

/**
 * @typedef    rtk_bt_le_audio_stream_state_t
 * @brief     Bluetooth LE audio stream state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_STREAM_STATE_IDLE               = 0x00,
	RTK_BT_LE_AUDIO_STREAM_STATE_IDLE_CONFIGURED    = 0x01,
	RTK_BT_LE_AUDIO_STREAM_STATE_CONFIGURED         = 0x02,
	RTK_BT_LE_AUDIO_STREAM_STATE_STARTING           = 0x03,
	RTK_BT_LE_AUDIO_STREAM_STATE_STREAMING          = 0x04,
	RTK_BT_LE_AUDIO_STREAM_STATE_PARTIAL_STREAMING  = 0x05,
	RTK_BT_LE_AUDIO_STREAM_STATE_STOPPING           = 0x06,
	RTK_BT_LE_AUDIO_STREAM_STATE_RELEASING          = 0x07,
} rtk_bt_le_audio_stream_state_t;

/**
 * @typedef    rtk_bt_le_audio_pacs_char_value_exist_t
 * @brief     Define Audio Locations.
 */
typedef enum {
	RTK_BT_LE_AUDIO_LOCATION_MONO                = 0x00000000,
	RTK_BT_LE_AUDIO_LOCATION_FL                  = 0x00000001,
	RTK_BT_LE_AUDIO_LOCATION_FR                  = 0x00000002,
	RTK_BT_LE_AUDIO_LOCATION_FC                  = 0x00000004,
	RTK_BT_LE_AUDIO_LOCATION_LFE1                = 0x00000008,
	RTK_BT_LE_AUDIO_LOCATION_BL                  = 0x00000010,
	RTK_BT_LE_AUDIO_LOCATION_BR                  = 0x00000020,
	RTK_BT_LE_AUDIO_LOCATION_FLC                 = 0x00000040,
	RTK_BT_LE_AUDIO_LOCATION_FRC                 = 0x00000080,
	RTK_BT_LE_AUDIO_LOCATION_BC                  = 0x00000100,
	RTK_BT_LE_AUDIO_LOCATION_LFE2                = 0x00000200,
	RTK_BT_LE_AUDIO_LOCATION_SIL                 = 0x00000400,
	RTK_BT_LE_AUDIO_LOCATION_SIR                 = 0x00000800,
	RTK_BT_LE_AUDIO_LOCATION_TPFL                = 0x00001000,
	RTK_BT_LE_AUDIO_LOCATION_TPFR                = 0x00002000,
	RTK_BT_LE_AUDIO_LOCATION_TPFC                = 0x00004000,
	RTK_BT_LE_AUDIO_LOCATION_TPC                 = 0x00008000,
	RTK_BT_LE_AUDIO_LOCATION_TPBL                = 0x00010000,
	RTK_BT_LE_AUDIO_LOCATION_TPBR                = 0x00020000,
	RTK_BT_LE_AUDIO_LOCATION_TPSIL               = 0x00040000,
	RTK_BT_LE_AUDIO_LOCATION_TPSIR               = 0x00080000,
	RTK_BT_LE_AUDIO_LOCATION_TPBC                = 0x00100000,
	RTK_BT_LE_AUDIO_LOCATION_BTFC                = 0x00200000,
	RTK_BT_LE_AUDIO_LOCATION_BTFL                = 0x00400000,
	RTK_BT_LE_AUDIO_LOCATION_BTFR                = 0x00800000,
	RTK_BT_LE_AUDIO_LOCATION_FLW                 = 0x01000000,
	RTK_BT_LE_AUDIO_LOCATION_FRW                 = 0x02000000,
	RTK_BT_LE_AUDIO_LOCATION_LS                  = 0x04000000,
	RTK_BT_LE_AUDIO_LOCATION_RS                  = 0x08000000,
	RTK_BT_LE_AUDIO_LOCATION_RFU                 = 0xF0000000,
	RTK_BT_LE_AUDIO_LOCATION_MASK                = 0x0FFFFFFF
} rtk_bt_le_audio_location_t;

/**
 * @typedef    rtk_bt_le_audio_big_sync_state_t
 * @brief     Bluetooth LE audio paramsters for big sync.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATED = 0x00,     /**< Terminated. */
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZING = 0x01,  /**< Start synchronizing to a BIG. A temporary state, haven't received the result.*/
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_SYNCHRONIZED = 0x02,   /**< Synchronized to a BIG. */
	RTK_BT_LE_AUDIO_BIG_SYNC_STATE_TERMINATING = 0x03,    /**< Terminate synchronization to the BIG. A temporary state, haven't received the result.*/
} rtk_bt_le_audio_big_sync_state_t;

/**
 * @typedef    rtk_bt_le_audio_pa_sync_state_t
 * @brief     Bluetooth LE audio pa sync state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATED = 0x00,                   /**< Terminated. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_SCAN_IDLE = 0x01,      /**< Start synchronizing when extended scanning is disabled. A temporary state, haven't received the result. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING = 0x02,  /**< No attempt to synchronize will take place while extended scanning is disabled. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZING = 0x03,                /**< Start synchronizing when extended scanning is enabled. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_SYNCHRONIZED = 0x04,                 /**< Synchronized. */
	RTK_BT_LE_AUDIO_PA_SYNC_STATE_TERMINATING = 0x05,                  /**< Terminate synchronization. A temporary state, haven't received the result. */
} rtk_bt_le_audio_pa_sync_state_t;

/**
 * @typedef    rtk_bt_le_audio_bass_past_type_t
 * @brief     Bluetooth LE audio BASS PAST TYPE.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BASS_PAST_BY_LOCAL_SRC     = 0x00,     /**< Initiating PAST for local PA */
	RTK_BT_LE_AUDIO_BASS_PAST_BY_REMOTE_SRC    = 0x01      /**< Initiating PAST for remote PA */
} rtk_bt_le_audio_bass_past_type_t;

/**
 * @typedef    rtk_bt_le_audio_bass_brs_big_encrption_t
 * @brief     Bluetooth LE audio broadcast receive state BIG_Encryption field.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_NOT_ENCRYPTED           = 0x00,      /**< Not encrypted */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_BROADCAST_CODE_REQUIRED = 0x01,      /**< Broadcast_Code required */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_DECRYPTING              = 0x02,      /**< Decrypting */
	RTK_BT_LE_AUDIO_BASS_BRS_BIG_BAD_CODE                = 0x03       /**< Bad_Code (incorrect encryption key) */
} rtk_bt_le_audio_bass_brs_big_encrption_t;

/*----------------------------------------------------------------------------------------------------------*/
/* event */

/**
 * @typedef    rtk_bt_le_audio_group_handle_t
 * @brief     Bluetooth LE AUDIO group handle.
 */
typedef void *rtk_bt_le_audio_group_handle_t;

/**
 * @typedef    rtk_bt_le_audio_stream_session_handle_t
 * @brief     Bluetooth LE AUDIO stream session handle.
 */
typedef void *rtk_bt_le_audio_stream_session_handle_t;

/**
 * @typedef    rtk_bt_le_audio_device_handle_t
 * @brief     Bluetooth LE AUDIO device handle.
 */
typedef void *rtk_bt_le_audio_device_handle_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_source_handle_t
 * @brief     Bluetooth LE audio broadcast source handle.
 */
typedef void *rtk_bt_le_audio_broadcast_source_handle_t;

/**
 * @typedef    rtk_bt_le_audio_sync_handle_t
 * @brief     Bluetooth LE audio sync handle.
 */
typedef void *rtk_bt_le_audio_sync_handle_t;

/**
 * @typedef    rtk_bt_le_audio_base_data_mapping_t
 * @brief     Bluetooth LE audio base data mapping.
 */
typedef void *rtk_bt_le_audio_base_data_mapping_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_source_config_t
 * @brief     Bluetooth LE audio param for config broadcast source.
 */
typedef struct {
	uint32_t primary_adv_interval_min;                  /** Minimum advertising interval for undirected and low duty cycle\n
                                                        * directed advertising.\n
                                                        * Range: 0x000020 to 0xFFFFFF\n
                                                        * Time = N * 0.625 ms\n
                                                        * Time Range: 20 ms to 10,485.759375 s */
	uint32_t primary_adv_interval_max;                  /** Maximum advertising interval for undirected and low duty cycle\n
                                                        * directed advertising.\n
                                                        * Range: 0x000020 to 0xFFFFFF\n
                                                        * Time = N * 0.625 ms\n
                                                        * Time Range: 20 ms to 10,485.759375 s */
	uint8_t primary_adv_channel_map;                    /** Advertising_Primary_Channel_Map */
	rtk_bt_le_addr_t own_addr;                          /** Own_Address_Type_and_Address */
	rtk_bt_le_adv_filter_t filter_policy;               /** Advertising_Filter_Policy */
	uint8_t tx_power;                                   /** Advertising_TX_Power: indicates max Tx power.\n
                                                        * Range: -127 to +20\n
                                                        * Units: dBm\n
                                                        * 0x7F: Host has no preference. */
	rtk_bt_le_phy_prim_adv_t primary_adv_phy;           /** Primary_Advertising_PHY */
	uint8_t secondary_adv_max_skip;                     /** Secondary_Advertising_Max_Skip */
	rtk_bt_le_phy_type_t secondary_adv_phy;             /** Secondary_Advertising_PHY */
	uint16_t periodic_adv_interval_min;                 /** Minimum advertising interval for periodic advertising.\n
                                                        * Must be different from periodic_adv_interval_max.\n
                                                        * Range: 0x0006 to 0xFFFF\n
                                                        * Time = N * 1.25 ms\n
                                                        * Time Range: 7.5 ms to 81.91875 s\n */
	uint16_t periodic_adv_interval_max;                 /** Maximum advertising interval for periodic advertising.\n
                                                        * Must be different from periodic_adv_interval_min.\n
                                                        * Range: 0x0006 to 0xFFFF\n
                                                        * Time = N * 1.25 ms\n
                                                        * Time Range: 7.5 ms to 81.91875 s\n */
	uint16_t periodic_adv_prop;                          /** Periodic_Advertising_Properties, @ref rtk_bt_le_pa_prop_t */
} rtk_bt_le_audio_broadcast_source_config_t;

/**
 * @typedef    rtk_bt_le_audio_bap_discovery_done_ind_t
 * @brief     Bluetooth LE audio info for bap discovery all done indication.
 */
typedef struct {
	uint16_t conn_handle;
	bool    pacs_is_found;
	uint8_t sink_pac_num;
	uint8_t source_pac_num;
	bool    ascs_is_found;
	uint8_t sink_ase_num;
	uint8_t source_ase_num;
	bool    bass_is_found;
	uint8_t brs_char_num;
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	uint8_t group_idx;
#endif
} rtk_bt_le_audio_bap_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_qos_cfg_preferred_t
 * @brief     Bluetooth LE audio qos cfg preferred item. see in BAP P86 and P111.
 */
typedef struct {
	uint32_t sdu_interval;
	uint8_t  framing;
	uint16_t max_sdu;
	uint8_t  retransmission_number;
	uint16_t max_transport_latency;
	uint32_t presentation_delay;
} rtk_bt_le_audio_qos_cfg_preferred_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_ase_qos_t
 * @brief     Bluetooth LE audio unicast ase qos.
 */
typedef struct {
	uint8_t  phy;
	uint8_t  retransmission_number;
	uint16_t max_sdu;
} rtk_bt_le_audio_unicast_ase_qos_t;

/**
 * @typedef    rtk_bt_le_audio_bis_conn_handle_info_t
 * @brief     Bluetooth LE audio bis connetion handle information.
 */
typedef struct {
	uint8_t bis_idx;                  /**< bis index */
	uint16_t bis_conn_handle;         /**< Connection handle of a BIS. */
} rtk_bt_le_audio_bis_conn_handle_info_t;

typedef struct {
	void *sync_handle;
	uint16_t bis_conn_handle;
	uint8_t bis_idx;
	uint8_t path_direction;
} rtk_bt_le_audio_bis_conn_info_t;

/**
 * @typedef    rtk_bt_le_audio_bass_cp_bis_info_t
 * @brief     Bluetooth LE audio bass broadcast receive state characteristic subgroup info.
 */
typedef struct {
	uint32_t bis_sync_state;                    /**< BIS_Sync_State for the subgroup */
	uint8_t metadata_len;
	uint8_t *p_metadata;
} rtk_bt_le_audio_bass_cp_bis_info_t;

/**
 * @typedef    rtk_bt_le_audio_pa_sync_establish_t
 * @brief     Bluetooth LE audio paramsters for pa sync establish.
 */
typedef struct {
	uint8_t options;                            /**< @ref rtk_bt_le_pa_sync_create_opt_t */
	uint8_t sync_cte_type;                      /**< @ref rtk_bt_le_pa_sync_cte_t */
	uint16_t skip;                              /**< The maximum number of periodic advertising events that can be skipped after a successful receive.
                                                     Range: 0x0000 to 0x01F3 */
	uint16_t sync_timeout;                      /**< Synchronization timeout for the periodic advertising train.
                                                     Range: 0x000A to 0x4000
                                                     Time = N*10 ms
                                                     Time Range: 100 ms to 163.84 s */
} rtk_bt_le_audio_pa_sync_establish_t;

/**
 * @typedef    rtk_bt_le_audio_bass_brs_data_t
 * @brief     Bluetooth LE audio bass broadcast receive state characteristic data.
 */
typedef struct {
	bool brs_is_used;
	uint8_t source_id;                                          /**< Broadcast Receive State Field */
	rtk_bt_le_addr_t source_address;
	uint8_t source_adv_sid;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
	uint8_t pa_sync_state;                                      /**< @ref rtk_bt_le_audio_pa_sync_state_t */
	uint8_t big_encryption;                                     /**< @ref rtk_bt_le_audio_bass_brs_big_encrption_t */
	uint8_t bad_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];
	uint8_t num_subgroups;
	uint16_t bis_info_size;
	rtk_bt_le_audio_bass_cp_bis_info_t *p_cp_bis_info; /**< @ref rtk_bt_le_audio_bass_cp_bis_info_t */
} rtk_bt_le_audio_bass_brs_data_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_cis_conn_info_t
 * @brief     structure of LE audio cis connection information
 */
typedef struct {
	uint16_t conn_handle;
	uint16_t cis_conn_handle;
	uint8_t  cig_id;
	uint8_t  cis_id;
} rtk_bt_le_audio_ascs_cis_conn_info_t;

/**
 * @typedef rtk_bt_le_audio_codec_config_t
 * @brief   structure of LE audio codec config
 */
typedef struct {
	uint16_t type_exist;
	uint8_t frame_duration;
	uint8_t sample_frequency;
	uint8_t codec_frame_blocks_per_sdu;
	uint16_t octets_per_codec_frame;
	uint32_t audio_channel_allocation;
} rtk_bt_le_audio_cfg_codec_t;

/**
 * @typedef    rtk_bt_le_audio_iso_chann_t
 * @brief     structure of LE audio iso channel struct
 */
typedef struct {
	void *p_iso_chann;                                           /**< iso channel handle */
	uint16_t iso_conn_handle;
	uint16_t iso_interval;                                       /**< Value of the ISO_Interval in units of 1.25 ms.
                                                                     The value shall be between 4 and 3200 (i.e. 5 ms to 4 s).*/
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;    /**< audio path direction */
} rtk_bt_le_audio_iso_chann_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_setup_data_path_ind_t
 * @brief     structure of LE audio ascs setup data path indication
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t ase_id;
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;/**< @ref rtk_bt_le_audio_iso_data_path_direction_t.*/
	uint16_t cis_conn_handle;                                 /**< connection handle of the CIS*/
	rtk_bt_le_audio_cfg_codec_t codec_cfg;                    /**< @ref rtk_bt_le_audio_cfg_codec_t*/
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                  /**< iso channel struct */
	uint8_t dev_num;                                          /**< for tx path waiting all devices ready */
	uint32_t presentation_delay;                              /**< presentation delay */
	uint32_t transport_latency_m_to_s;                        /**< The actual transport latency, in microseconds, from Central to Peripheral. */
	uint32_t transport_latency_s_to_m;                        /**< The actual transport latency, in microseconds, from Peripheral to Central. */
} rtk_bt_le_audio_ascs_setup_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_remove_data_path_ind_t
 * @brief     structure of LE audio ascs remove data path indication
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t  ase_id;
	rtk_bt_le_audio_iso_data_path_direction_t path_direction; /**< @ref rtk_bt_le_audio_iso_data_path_direction_t.*/
	uint16_t cis_conn_handle;                                 /**< connection handle of the CIS*/
} rtk_bt_le_audio_ascs_remove_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_asc_state_ind_t
 * @brief     structure of LE audio ascs state
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     ase_id;
	uint8_t     direction;
	rtk_bt_le_audio_ascs_ase_state_t     ase_state;        /**< @ref rtk_bt_le_audio_ascs_ase_state_t*/
} rtk_bt_le_audio_ascs_ase_state_ind_t;

/**
 * @typedef rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t
 * @brief   structure of LE audio ascs codec config control point indication
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     ase_id;
	rtk_bt_le_audio_cfg_codec_t codec_cfg;          /**< @ref rtk_bt_le_audio_cfg_codec_t*/
} rtk_bt_le_audio_ascs_cp_codec_cfg_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_cp_qos_config_t
 * @brief     structure of LE audio ascs qos config control point indication
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     ase_id;
	uint8_t     cig_id;
	uint8_t     cis_id;
	uint8_t     framing;
	uint8_t     phy;
	uint8_t     retransmission_number;
	uint16_t    max_sdu;
	uint16_t    max_transport_latency;
	uint32_t    sdu_interval;
	uint32_t    presentation_delay;
} rtk_bt_le_audio_ascs_cp_qos_cfg_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_cp_enable_ind_t
 * @brief     structure of LE audio ascs enable control point indication
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     ase_num;
	uint8_t     ase_id[RTK_BT_LE_ASCS_AES_CHAR_NUM];
} rtk_bt_le_audio_ascs_cp_enable_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_cp_disable_ind_t
 * @brief     structure of LE audio ascs disable control point indication
 */
typedef rtk_bt_le_audio_ascs_cp_enable_ind_t rtk_bt_le_audio_ascs_cp_disable_ind_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_prefer_qos_data_t
 * @brief     structure of LE audio ascs prefer qos data
 */
typedef struct {
	uint8_t  supported_framing;
	uint8_t  preferred_phy;
	uint8_t  preferred_retrans_number;
	uint16_t max_transport_latency;
	uint32_t presentation_delay_min;
	uint32_t presentation_delay_max;
	uint32_t preferred_presentation_delay_min;
	uint32_t preferred_presentation_delay_max;
} rtk_bt_le_audio_ascs_prefer_qos_data_t;

/**
 * @typedef    rtk_bt_le_audio_ascs_get_prefer_qos_ind_t
 * @brief     structure of LE audio ascs get prefer qos indication
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t ase_id;
	uint8_t direction;
	rtk_bt_le_audio_cfg_codec_t codec_cfg;
	rtk_bt_le_audio_ascs_ase_target_latency_t target_latency;
	rtk_bt_le_audio_ascs_ase_target_phy_t target_phy;
	rtk_bt_le_audio_ascs_prefer_qos_data_t *p_prefer_qos_data;
} rtk_bt_le_audio_ascs_get_prefer_qos_ind_t;

typedef struct {
	uint8_t pa_sync_options;                   /**< @ref rtk_bt_le_pa_sync_create_opt_t */
	uint16_t pa_sync_skip;                     /**< The maximum number of periodic advertising events that can be skipped after a successful receive.
                                                     Range: 0x0000 to 0x01F3 */
	uint16_t pa_sync_timeout;                  /**< Synchronization timeout for the periodic advertising train.
                                                     Range: 0x000A to 0x4000
                                                     Time = N*10 ms
                                                     Time Range: 100 ms to 163.84 s */
	uint16_t past_timeout;                     /**< Synchronization timeout for PAST.
                                                     Range: 0x000A to 0x4000
                                                     Time = N*10 ms
                                                     Time Range: 100 ms to 163.84 s */
} rtk_bt_le_audio_bass_pa_sync_param_t;

/**
 * @typedef    rtk_bt_le_audio_bass_get_pa_sync_param_ind_t
 * @brief     Bluetooth LE audio bass set pa sync parameter.
 */
typedef struct {
	rtk_bt_le_audio_sync_handle_t sync_handle;              /**< Synchronization handle*/
	uint8_t source_id;
	bool    is_past;
	uint16_t pa_interval;
	rtk_bt_le_audio_bass_pa_sync_param_t *p_pa_sync_param;  /**< @ref rtk_bt_le_audio_bass_pa_sync_param_t*/
} rtk_bt_le_audio_bass_get_pa_sync_param_ind_t;

/**
 * @typedef    rtk_bt_le_audio_bass_big_sync_param_t
 * @brief     Bluetooth LE audio bass big sync param.
 */
typedef struct {
	uint8_t big_mse;                       /**< The maximum number of subevents that a Controller should use to receive data
                                                payloads in each interval for a BIS.
                                                0x00: The Controller can schedule reception of any number of subevents up
                                                      to NSE.
                                                0x01 to 0x1F: Maximum number of subevents that should be used to receive data
                                                              payloads in each BIS event      */
	uint16_t big_sync_timeout;             /**< Specify the maximum permitted time between successful receptions of BIS PDUs.
                                                Range: 0x000A to 0x4000
                                                Time = N*10 ms
                                                Time Range: 100 ms to 163.84 s */
} rtk_bt_le_audio_bass_big_sync_param_t;

/**
 * @typedef    rtk_bt_le_audio_bass_get_big_sync_param_ind_t
 * @brief     Bluetooth LE audio bass set big sync parameter.
 */
typedef struct {
	rtk_bt_le_audio_sync_handle_t sync_handle;                  /**< Synchronization handle*/
	uint8_t source_id;
	rtk_bt_le_audio_bass_big_sync_param_t *p_big_sync_param;    /**< @ref rtk_bt_le_audio_bass_big_sync_param_t*/
} rtk_bt_le_audio_bass_get_big_sync_param_ind_t;

/**
 * @typedef    rtk_bt_le_audio_pa_sync_state_ind_t
 * @brief     Bluetooth LE audio pa sync indication infomation.
 */
typedef struct {
	uint8_t cause;                                                  /**< process result */
	rtk_bt_le_audio_sync_handle_t sync_handle;                      /**< Synchronization handle*/
	uint8_t sync_state;                                             /**< PA Synchronization state. @ref rtk_bt_le_audio_pa_sync_state_t*/
	uint16_t action;                                                /**< PA Synchronization action. @ref rtk_bt_le_audio_pa_action_t. */
} rtk_bt_le_audio_pa_sync_state_ind_t;

/**
 * @typedef    rtk_bt_le_audio_big_setup_data_path_ind_t
 * @brief     Bluetooth LE audio setup data path indication infomation.
 */
typedef struct {
	uint8_t bis_idx;                                          /**< bis index */
	uint16_t bis_conn_handle;                                 /**< bis connection handle */
	uint16_t cause;                                           /**< result */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                  /**< iso channel struct */
	rtk_bt_le_audio_cfg_codec_t codec_t;                      /**< codec config */
	rtk_bt_le_audio_sync_handle_t sync_handle;                /**< Synchronization handle*/
	uint32_t presentation_delay;                              /**< presentation delay */
	uint32_t transport_latency_big;                           /**< BIG transport latency, in microseconds */
} rtk_bt_le_audio_big_setup_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_big_info_adv_report_t
 * @brief     Bluetooth LE audio big info adv report.
 */
typedef struct {
	uint8_t sync_id;         /**< Identify the periodic advertising train. */
	uint16_t sync_handle;   /**< Sync_Handle identifying the periodic advertising train. */
	uint8_t num_bis;         /**< The number of BISes in the BIG. */
	uint8_t nse;             /**< The number of subevents per BIS in each BIG event. */
	uint16_t iso_interval;   /**< Value of the ISO_Interval subfield of the BIGInfo field.
                                Time between two adjacent BIG anchor points, in units of 1.25 ms.
                                The value shall be between 4 and 3200 (i.e. 5 ms to 4 s).*/
	uint8_t bn;              /**< Value of the BN subfield of the BIGInfo field.
                                Burst Number contains the number of new payloads for each BIS in a BIS event. */
	uint8_t pto;             /**< Value of the PTO subfield of the BIGInfo field.
                                Pre_Transmission_Offset contains the offset in number of ISO_Intervals for
                                pre transmissions of data packets. */
	uint8_t irc;             /**< Value of the IRC subfield of the BIGInfo field.
                                Immediate Repetition Count contains the number of times the scheduled
                                data packet is transmitted. */
	uint16_t max_pdu;        /**< The maximum number of data octets (excluding the MIC, if any) that can be
                                carried in each BIS Data PDU in the BIG. */
	uint32_t sdu_interval;   /**< Value of the SDU_Interval subfield of the BIGInfo field.
                                The time interval, in microseconds, of the periodic SDUs. */
	uint16_t max_sdu;        /**< The maximum size, in octets, of an SDU. */
	uint8_t phy;             /**< Indicate the PHY used by the BIG. @ref T_GAP_PHYS_TYPE. */
	uint8_t framing;         /**< Indicate whether the BIG carries framed or unframed data.
                                0x00: Unframed
                                0x01: Framed */
	uint8_t encryption;      /**< Indicate whether the BIG is encrypted or not.
                                0x00: BIG carries unencrypted data
                                0x01: BIG carries encrypted data */
	rtk_bt_le_audio_sync_handle_t cb_sync_handle;                /**< Synchronization handle*/
} rtk_bt_le_audio_big_info_adv_report_t;

/**
 * @typedef    rtk_bt_le_audio_big_remove_data_path_ind_t
 * @brief     Bluetooth LE audio remove data path indication infomation.
 */
typedef struct {
	uint8_t bis_idx;                                          /**< bis index */
	uint16_t bis_conn_handle;                                 /**< bis connection handle */
	uint16_t cause;                                           /**< result */
	rtk_bt_le_audio_iso_data_path_direction_t path_direction; /**< audio path direction */
	rtk_bt_le_audio_sync_handle_t sync_handle;                /**< Synchronization handle*/
} rtk_bt_le_audio_big_remove_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_source_state_ind_t
 * @brief     Bluetooth LE audio broadcast source state indication.
 */
typedef struct {
	uint8_t cause;                                                      /**< process result */
	rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle;  /**< Broadcast source handle*/
	uint8_t broadcast_source_state;                                     /**< Broadcast source state. @ref rtk_bt_le_audio_broadcast_source_state_t*/
} rtk_bt_le_audio_broadcast_source_state_ind_t;

/**
 * @typedef    rtk_bt_le_audio_bap_big_setup_data_path_ind_t
 * @brief     Bluetooth LE audio bap big setup data path indication.
 */
typedef struct {
	uint8_t bis_idx;                                                /**< bis index */
	uint16_t bis_conn_handle;                                       /**< Connection handle of a BIS. */
	uint16_t cause;
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                        /**< iso channel struct */
	rtk_bt_le_audio_cfg_codec_t codec_t;                            /**< codec config */
	uint32_t presentation_delay;                                    /**< presentation delay */
	uint32_t transport_latency_big;                                 /**< BIG transport latency, in microseconds */
} rtk_bt_le_audio_bap_big_setup_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_bap_big_remove_data_path_ind_t
 * @brief     Bluetooth LE audio bap big remove data path indication.
 */
typedef struct {
	uint8_t bis_idx;                                                /**< bis index */
	uint16_t bis_conn_handle;                                       /**< Connection handle of a BIS. */
	uint16_t cause;
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;       /**< audio path direction */
} rtk_bt_le_audio_bap_big_remove_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_group_dev_msg_ind_t
 * @brief     Bluetooth LE audio group msg device indication.
 */
typedef struct {
	uint16_t cause;
	rtk_bt_le_audio_group_dev_msg_type_t
	msg_type;                  /**< msg type. @ref rtk_bt_le_audio_group_dev_msg_type_t  */                                          /**< process result */
	rtk_bt_le_audio_group_handle_t group_handle;                    /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	rtk_bt_le_audio_device_handle_t device_handle;                  /**< device handle. @ref rtk_bt_le_audio_device_handle_t */
} rtk_bt_le_audio_group_dev_msg_ind_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_session_qos_t
 * @brief     Bluetooth LE audio unicast session qos.
 */
typedef struct {
	uint8_t  sca;
	uint8_t  packing;
	uint8_t  framing;
	uint32_t sdu_interval_m_s;/*3bytes*/
	uint32_t sdu_interval_s_m;/*3bytes*/
	uint16_t latency_m_s;
	uint16_t latency_s_m;
	uint32_t sink_presentation_delay;
	uint32_t source_presentation_delay;
} rtk_bt_le_audio_unicast_session_qos_t;

/**
 * @typedef    rtk_bt_le_audio_bap_setup_data_path_ind_t
 * @brief     Bluetooth LE audio bap setup data path indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                     /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	rtk_bt_le_audio_stream_session_handle_t stream_session_handle;   /**< stream session handle. @ref rtk_bt_le_audio_stream_session_handle_t */
	rtk_bt_le_audio_device_handle_t device_handle;                   /**< device handle. @ref rtk_bt_le_audio_device_handle_t */
	uint8_t ase_id;
	uint8_t path_direction;                                          /**< path direction. @ref rtk_bt_le_audio_iso_data_path_direction_t */
	uint16_t cis_conn_handle;                                        /**< connection handle of the CIS*/
	rtk_bt_le_audio_cfg_codec_t codec_parsed_data;                   /**< @ref rtk_bt_le_audio_cfg_codec_t */
	rtk_bt_le_audio_iso_chann_t iso_chann_t;                         /**< iso channel struct */
	uint8_t dev_num;                                                 /**< for tx path waiting all devices */
	uint32_t presentation_delay;                                     /**< presentation delay */
	uint32_t transport_latency_m_to_s;                               /**< The actual transport latency, in microseconds, from Central to Peripheral. */
	uint32_t transport_latency_s_to_m;                               /**< The actual transport latency, in microseconds, from Peripheral to Central. */
} rtk_bt_le_audio_bap_setup_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_audio_bap_remove_data_path_ind_t
 * @brief     Bluetooth LE audio bap remove data path indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                     /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	rtk_bt_le_audio_stream_session_handle_t stream_session_handle;   /**< stream session handle. @ref rtk_bt_le_audio_stream_session_handle_t */
	rtk_bt_le_audio_device_handle_t device_handle;                   /**< device handle. @ref rtk_bt_le_audio_device_handle_t */
	uint8_t ase_id;
	uint8_t path_direction;                                          /**< path direction. @ref rtk_bt_le_audio_iso_data_path_direction_t */
	uint16_t cis_conn_handle;                                        /**< connection handle of the CIS*/
	uint16_t cause;
} rtk_bt_le_audio_bap_remove_data_path_ind_t;

/**
 * @typedef    rtk_bt_le_iso_direct_iso_data_ind_t
 * @brief     Indication of ISO Datat packet with cb_type @ref RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND .
 */
typedef struct {
	uint16_t iso_conn_handle;                                   /**< Connection handle of the ISO. */
	rtk_bt_le_audio_isoch_data_pkt_status_t pkt_status_flag;    /**< @ref rtk_bt_le_audio_isoch_data_pkt_status_t. */
	uint16_t  offset;                                           /**< Offset from start of the SDU to @ref p_buf.
                                                                    e.g. p_buf + offset indicates the start of the SDU. */
	uint16_t  iso_sdu_len;                                      /**< Length of the SDU. */
	uint16_t  pkt_seq_num;                                      /**< The sequence number of the SDU. */
	bool      ts_flag;                                          /**< Indicates whether contains time_stamp.
                                                                    True: contain time_stamp.
                                                                    False: not contain time_stamp. */
	uint32_t  time_stamp;                                       /**< A time in microseconds. time_stamp is valid when @ref ts_flag is True. */
	uint16_t  buf_len;                                          /**< indicate buffer length. */
	uint8_t   *p_buf;                                           /**< Point the buffer that needs to release. */
} rtk_bt_le_audio_direct_iso_data_ind_t;

/* act */
/**
 * @typedef    rtk_bt_le_audio_ext_scan_config_t
 * @brief     Extend scan config struct.
 */
typedef struct {
	rtk_bt_le_ext_scan_param_t *p_param;    /*!< extended scan parameter */
	bool enable;                            /*!< start or stop ext scan */
} rtk_bt_le_audio_ext_scan_config_t;

/**
 * @typedef    rtk_bt_le_audio_iso_data_send_info_t
 * @brief     Bluetooth LE audio mgr data send info definition .
 */
typedef struct {
	uint16_t iso_conn_handle;       /*!< connection handle of the ISO*/
	bool ts_flag;                   /*!< indicates whether contains time_stamp, true: contain time_stamp; false: not contain time_stamp */
	uint32_t time_stamp;            /*!< a time in microseconds. time_stamp is used when @ref ts_flag is True */
	uint16_t pkt_seq_num;           /*!< sequence number of the SDU */
	uint16_t data_len;              /*!< length of the SDU to be sent */
	uint8_t *p_data;                /*!< point to data to be sent */
} rtk_bt_le_audio_iso_data_send_info_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_client_start_t
 * @brief     Bluetooth LE audio unicast client start struct.
 */
typedef struct {
	uint8_t group_idx;
	uint16_t sink_context;
	uint16_t source_context;
} rtk_bt_le_audio_unicast_client_start_t;

/**
 * @typedef    rtk_bt_le_audio_unicast_client_stop_t
 * @brief     Bluetooth LE audio unicast client stop struct.
 */
typedef struct {
	uint8_t group_idx;
	uint32_t cis_timeout_ms;
} rtk_bt_le_audio_unicast_client_stop_t;

/**
 * @typedef    rtk_bt_le_audio_bap_param_config_type_t
 * @brief     Bluetooth LE audio bap config type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_BAP_BROADCAST_SOURCE_CONFIG = 0x01, /**< rtk_bt_le_audio_broadcast_source_config_t. */
	RTK_BT_LE_AUDIO_BAP_BROADCAST_CODE_CONFIG = 0x02,   /**< broadcast_code.*/
	RTK_BT_LE_AUDIO_BAP_ASE_QOS_CONFIG = 0x03,          /**< rtk_bt_le_audio_unicast_ase_qos_t. */
} rtk_bt_le_audio_bap_param_config_type_t;

/**
 * @typedef    rtk_bt_le_audio_bap_param_config_t
 * @brief     Bluetooth LE audio bap parameter config struct .
 */
typedef union {
	rtk_bt_le_audio_bap_param_config_type_t cfg_type;
	union {
		rtk_bt_le_audio_broadcast_source_config_t brs_config;
		uint8_t broadcast_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];
		rtk_bt_le_audio_unicast_ase_qos_t bap_ase_qos;
	} cfg;
} rtk_bt_le_audio_bap_param_config_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_source_create_param_t
 * @brief     Bluetooth LE audio broadcast source create parameter.
 */
typedef struct {
	rtk_bt_le_audio_codec_cfg_item_t cfg_codec_index;
	rtk_bt_le_audio_qos_cfg_type_t cfg_qos_type;
	bool manual_qos_flag;                                  /**< this flag is used for manual qos */
	rtk_bt_le_audio_qos_cfg_preferred_t *p_manual_qos_cfg; /**< manual qos parameter structure pointer */
	rtk_bt_le_addr_type_t local_addr_type;
	bool encryption;
	uint16_t stream_audio_contexts;
} rtk_bt_le_audio_broadcast_source_create_param_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_sync_create_param_t
 * @brief     Bluetooth LE audio broadcast sink pa sync create parameter.
 */
typedef struct {
	rtk_bt_le_addr_t adv_addr;
	uint8_t adv_sid;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
} rtk_bt_le_audio_broadcast_sync_create_param_t;


/**
 * @typedef    rtk_bt_le_audio_broadcast_assisatant_remote_scan_param_t
 * @brief     Bluetooth LE audio broadcast assistant remote scan parameter.
 */
typedef struct {
	bool enable;
	uint8_t group_idx;
} rtk_bt_le_audio_broadcast_assistant_remote_scan_param_t;

/**
 * @typedef    rtk_bt_le_audio_broadcast_assistant_reception_act_param_t
 * @brief     Bluetooth LE audio broadcast assistant reception action parameter.
 */
typedef struct {
	rtk_bt_le_addr_t addr;
	uint8_t group_idx;
} rtk_bt_le_audio_broadcast_assistant_reception_act_param_t;

/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_bap BT BAP APIs
 * @brief     BT BAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     allocate le audio acl link for stack flow.
 * @param[in] conn_ind: pointer for le connection indication
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_le_audio_link_alloc(rtk_bt_le_conn_ind_t *conn_ind);

/**
 * @brief     free le audio acl link for stack flow.
 * @param[in] disconn_ind: pointer for le disconnection indication
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_le_audio_link_free(rtk_bt_le_disconn_ind_t *disconn_ind);

/**
 * @brief     Send ISO data .
 * @param[in] info: The info of iso send data
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_iso_data_send(rtk_bt_le_audio_iso_data_send_info_t *info);

/**
 * @brief     Start unicast client.
 * @param[in] group_idx: group index
 * @param[in] sink_context: sink context
 * @param[in] source_context: source context
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_bap_unicast_client_start(uint8_t group_idx, uint16_t sink_context, uint16_t source_context);

/**
 * @brief     Stop unicast client.
 * @param[in] group_idx: group index
 * @param[in] cis_timeout_ms: timeout
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_bap_unicast_client_stop(uint8_t group_idx, uint32_t cis_timeout_ms);

/**
 * @brief     Release session.
 * @param[in] group_idx: group index
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_bap_unicast_client_release(uint8_t group_idx);

/**
 * @brief     Config bap parameters.
 * @param[in] p_param: config parameter struct ref rtk_bt_le_audio_bap_param_config_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_bap_param_config(rtk_bt_le_audio_bap_param_config_t *p_param);

/**
 * @brief     Create broadcast source.
 * @param[in] p_param: parameter struct ref rtk_bt_le_audio_broadcast_source_create_param_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_source_create(rtk_bt_le_audio_broadcast_source_create_param_t *p_param);

/**
 * @brief     Start broadcast source.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */

uint16_t rtk_bt_le_audio_broadcast_source_start(void);

/**
 * @brief     Stop broadcast source.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_source_stop(void);

/**
 * @brief     Start or stop extended scan
 * @param[in] enable: when enable is true, scan start. Otherwise, scan stopped.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_ext_scan_act(bool enable);

/**
 * @brief     Create PA sync
 * @param[in] addr: target broadcast source BT address
 * @param[in] adv_sid: target broadcast source advertise set ID
 * @param[in] broadcast_id: target broadcast source broadcast ID
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_pa_sync_create(rtk_bt_le_addr_t addr, uint8_t adv_sid, uint8_t *broadcast_id);

/**
 * @brief     Terminate PA sync
 * @param[in] addr: target broadcast source BT address
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_pa_sync_terminate(rtk_bt_le_addr_t addr);

/**
 * @brief     Create BIG sync
 * @param[in] addr: target broadcast source BT address
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_create(rtk_bt_le_addr_t addr);

/**
 * @brief     Create BIG sync by sync handle
 * @param[in] sync_handle: target broadcast source sync handle
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_create_by_handle(rtk_bt_le_audio_sync_handle_t sync_handle);

/**
 * @brief     Terminate BIG sync
 * @param[in] addr: target broadcast source BT address
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_terminate(rtk_bt_le_addr_t addr);

/**
 * @brief     Terminate BIG sync by handle
 * @param[in] sync_handle: target broadcast source sync handle
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_big_sync_terminate_by_handle(rtk_bt_le_audio_sync_handle_t sync_handle);

/**
 * @brief     Broadcast assistant executes remote scan action for Scan Delegator.
 * @param[in] enable: true: start remote scan, false: stop remote scan
 * @param[in] group_idx: group index
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_remote_ext_scan_act(bool enable, uint8_t group_idx);

/**
 * @brief     Broadcast assistant add broadcast source and transfer syncInfo to Scan Delegator.
 * @param[in] addr: broadcast source address
 * @param[in] group_idx: group index
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_start(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Broadcast assistant modify broadcast source in broadcast receive state charateristic of Scan Delegator.
 * @param[in] addr: broadcast source address
 * @param[in] group_idx: group index
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_stop(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Broadcast assistant remove broadcast source in broadcast receive state charateristic of Scan Delegator.
 * @param[in] addr: broadcast source address
 * @param[in] group_idx: group index
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_broadcast_assistant_reception_remove(rtk_bt_le_addr_t addr, uint8_t group_idx);

/**
 * @brief     Allocate group .
 * @param[in] p_group_handle: allocated group handle
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
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
