/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#ifndef __BT_APP_CFG_H__
#define __BT_APP_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "platform_autoconf.h"

#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <bt_le_audio_app_config.h>
#endif
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <bt_le_iso_app_config.h>
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <rtk_bt_mesh_def.h>
#endif

/**
 * @def       RTK_BT_LE_GAP_IRK_LEN
 * @brief     Length of the Identity Resolving Key (IRK).
 */
#define RTK_BT_LE_GAP_IRK_LEN       16

/**
 * @typedef   rtk_bt_app_conf_t
 * @brief     Default application configuration.
 */
typedef struct {
	uint8_t  a2dp_role;                              /*!< Default A2DP profile role. Only used in BR/EDR. @ref rtk_bt_a2dp_role_t. */
	uint8_t  spp_role;                               /*!< Default SPP profile role. Only used in BR/EDR. @ref rtk_bt_spp_role_t. */
	uint8_t  hfp_role;                               /*!< Default HFP profile role. Only used in BR/EDR. @ref rtk_bt_hfp_role_t. */
	uint8_t  hid_role;                               /*!< Default HID profile role. Only used in BR/EDR. @ref rtk_bt_hid_role_t. */
	uint32_t app_profile_support;                    /*!< Bit mask of the profiles supported by the application, @ref rtk_bt_profile_t values. */
	uint8_t  server_chann;                           /*!< RFCOMM server channel number. */
	uint16_t mtu_size;                               /*!< Default ATT MTU size.
                                                      *   Max value: 512.
                                                      *   Default: 180 when NULL is passed to @ref rtk_bt_enable. */
	bool     master_init_mtu_req;                    /*!< Whether the local device, as central, automatically initiates the exchange MTU request once connected.
                                                      *   - true: send the exchange MTU request automatically.
                                                      *   - false: do not send the exchange MTU request automatically. */
	bool     slave_init_mtu_req;                     /*!< Whether the local device, as peripheral, automatically initiates the exchange MTU request once connected.
                                                      *   - true: send the exchange MTU request automatically.
                                                      *   - false: do not send the exchange MTU request automatically. */
	uint8_t  prefer_all_phy;                         /*!< Default preferred PHY setting: whether the Host has preference for the TX and RX PHY.
                                                      *   @ref rtk_bt_le_all_phy_prefer_t. */
	uint8_t  prefer_tx_phy;                          /*!< Default preferred TX PHY setting. @ref rtk_bt_le_trx_phy_prefer_t. */
	uint8_t  prefer_rx_phy;                          /*!< Default preferred RX PHY setting. @ref rtk_bt_le_trx_phy_prefer_t. */
	uint16_t max_tx_octets;                          /*!< Default data length setting.
                                                      *   Range: 0x001B to 0x00FB.
                                                      *   All other values reserved for future use. */
	uint16_t max_tx_time;                            /*!< Default data length setting.
                                                      *   Range: 0x0148 to 0x4290.
                                                      *   Units: microseconds.
                                                      *   All other values reserved for future use. */
#if defined(RTK_BLE_SET_TX_QUEUE_NUM) && RTK_BLE_SET_TX_QUEUE_NUM
	uint16_t max_stack_tx_pending_num;               /*!< Maximum number of packets pending in the stack TX queue.
                                                      *   Range: 2 to 15.
                                                      *   0: @ref RTK_BT_GATT_DEFAULT_CREDITS is used. */
#endif
	bool     irk_auto_gen;                           /*!< Whether the stack automatically generates the local IRK.
                                                      *   - true: generate a local IRK and save it to FTL on first use, then load it from FTL afterwards.
                                                      *   - false: use the value set in @ref irk. */
	uint8_t  irk[RTK_BT_LE_GAP_IRK_LEN];             /*!< Local IRK data.
                                                      *   Only used when @ref irk_auto_gen is false. */
	bool     user_def_service;                       /*!< Whether the GAP and GATT services are registered by the application.
                                                      *   - true: the application registers the GAP and GATT services.
                                                      *   - false: the built-in services of the stack are used. */
	bool     cccd_not_check;                         /*!< Whether to skip checking the CCCD before sending an indication or notification.
                                                      *   - true: the CCCD is not checked.
                                                      *   - false: the CCCD is checked, and sending fails if the corresponding indication or notification
                                                      *   bit is not enabled. */
	bool     cccd_not_save;                          /*!< Whether to skip saving the local server's CCCD information to flash when bonded.
                                                      *   - true: the CCCD is not saved.
                                                      *   - false: the CCCD is saved, and @ref RTK_BT_GATTS_EVT_CCCD_IND is indicated immediately
                                                      *   when reconnecting to the bonded device.
                                                      *   By default, the CCCD is saved. This value is ignored if @ref RTK_BLE_AUDIO_SUPPORT is enabled. */
	bool     key_convert_le_to_bredr;                /*!< Whether to convert the BLE LTK (secure connections pairing only) to a BR/EDR link key.
                                                      *   - true: convert the key.
                                                      *   - false: do not convert the key.
                                                      *   Only used in BLE-BR/EDR dual mode. */
	uint8_t  min_enc_key_size;                       /*!< Minimum encryption key size permitted during BLE pairing.
                                                      *   Range: 7 to 16.
                                                      *   0: the setting is not applied. */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
	rtk_bt_le_iso_app_conf_t le_iso_app_conf;        /*!< Default configuration of LE ISO. */
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	rtk_bt_le_audio_app_conf_t le_audio_app_conf;    /*!< Default configuration of LE Audio. */
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	rtk_bt_mesh_app_conf_t bt_mesh_app_conf;         /*!< Default configuration of BT Mesh. */
#endif
} rtk_bt_app_conf_t;

#ifdef __cplusplus
}
#endif

#endif  /* __BT_APP_CFG_H__ */