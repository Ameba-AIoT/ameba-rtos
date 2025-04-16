/**
  ******************************************************************************
  * @file    wifi_api_types.h
  * @author
  * @version
  * @date
  * @brief   This file provides user interface for Wi-Fi station and AP mode configuration
  *             base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_API_TYPES_H
#define __WIFI_API_TYPES_H

#ifndef CONFIG_FULLMAC
#include "ameba.h"
#include "rtw_wifi_common.h"
#include "rtw_byteorder.h"
#include "dlist.h"
#include "platform_stdlib.h"
#if !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))
#include "os_wrapper.h"
#include "rtw_misc.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @addtogroup WIFI_Exported_Constants Wi-Fi Exported Constants
 * @{
 */

#define STA_WLAN_INDEX	    0
#define SOFTAP_WLAN_INDEX	1
#define NAN_WLAN_INDEX	    2
#define NONE_WLAN_INDEX	    0xFF

/** Set to this means do fast survey on the specified channel with scan time set to 25ms,
 * otherwise do normal scan on the specified channel with scan time set to 110ms. */
#define RTW_PSCAN_FAST_SURVEY	0x02

/** Max channel num in each channel plan */
#if !defined(SUPPORT_5G_CHANNEL)
#define	RTW_MAX_CHANNEL_NUM			14
#else
#define	RTW_MAX_CHANNEL_NUM			42
#endif


/** Set to this means disable DPK(Digital Pre-Distortion Calibration) of rf calibration. */
#define RTW_RFK_DIS_DPK     BIT(0)

/* Maximum size of the ESSID and NICKN strings */
#define RTW_ESSID_MAX_SIZE	32  /**< Refer to 802.11 spec, the max length of ssid is 32 bytes. */
#define RTW_WPA2_MAX_PSK_LEN		(64)
#define RTW_WPA3_MAX_PSK_LEN		(128)		/**< maxmum psk length */
#define RTW_MAX_PSK_LEN		RTW_WPA3_MAX_PSK_LEN /* TODO: rom should also check RTW_PASSPHRASE_MAX_SIZE. */
#define RTW_MIN_PSK_LEN		(8)		/**< minimum psk length */

/**
 * @brief  mac address format.
 */
#define MAC_ARG(x)		((u8*)(x))[0],((u8*)(x))[1],\
				((u8*)(x))[2],((u8*)(x))[3],\
				((u8*)(x))[4],((u8*)(x))[5]
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

/** @} End of WIFI_Exported_Constants group*/


/** @addtogroup WIFI_Exported_Types Wi-Fi Exported Types
* @{
*/
/** @addtogroup WIFI_Exported_Enumeration_Types Enumeration Type
 * @{
 */
enum rtw_security_flag {
	WEP_ENABLED          = BIT(0),     /**< wep enable */
	TKIP_ENABLED         = BIT(1),     /**< tkip enable */
	AES_ENABLED          = BIT(2),     /**< aes enable */
	AES_CMAC_ENABLED     = BIT(4),     /**< aes cmac enable */
	ENTERPRISE_ENABLED   = BIT(5),     /**< enterprise enable */
	OWE_ENABLED          = BIT(6),     /**< owe enable */
	SHARED_ENABLED       = BIT(15),    /**< shared enable */
	WPA_SECURITY         = BIT(21),    /**< wpa */
	WPA2_SECURITY        = BIT(22),    /**< wpa2 */
	WPA3_SECURITY        = BIT(23),    /**< wpa3 */
};

/**
  * @brief  The enumeration lists the disconnet reasons in rtw_event_info_joinstatus_disconn when @ref RTW_JOINSTATUS_DISCONNECT happenned.
  */
enum rtw_disconn_reason {
#ifndef CONFIG_FULLMAC
	/*Reason code in 802.11 spec, Receive AP's deauth or disassoc after wifi connected*/
	RTW_DISCONN_RSN_80211_UNSPECIFIED                         = 1,
	RTW_DISCONN_RSN_80211_PREV_AUTH_NOT_VALID 			      = 2,
	RTW_DISCONN_RSN_80211_DEAUTH_LEAVING                      = 3,
	RTW_DISCONN_RSN_80211_DISASSOC_DUE_TO_INACTIVITY          = 4,
	RTW_DISCONN_RSN_80211_DISASSOC_AP_BUSY                    = 5,
	RTW_DISCONN_RSN_80211_CLASS2_FRAME_FROM_NONAUTH_STA       = 6,
	RTW_DISCONN_RSN_80211_CLASS3_FRAME_FROM_NONASSOC_STA      = 7,
	RTW_DISCONN_RSN_80211_DISASSOC_STA_HAS_LEFT               = 8,
	RTW_DISCONN_RSN_80211_STA_REQ_ASSOC_WITHOUT_AUTH          = 9,
	RTW_DISCONN_RSN_80211_PWR_CAPABILITY_NOT_VALID            = 10,
	RTW_DISCONN_RSN_80211_SUPPORTED_CHANNEL_NOT_VALID         = 11,
	RTW_DISCONN_RSN_80211_INVALID_IE                          = 13,
	RTW_DISCONN_RSN_80211_MICHAEL_MIC_FAILURE                 = 14,
	RTW_DISCONN_RSN_80211_4WAY_HANDSHAKE_TIMEOUT              = 15,
	RTW_DISCONN_RSN_80211_GROUP_KEY_UPDATE_TIMEOUT            = 16,
	RTW_DISCONN_RSN_80211_IE_IN_4WAY_DIFFERS                  = 17,
	RTW_DISCONN_RSN_80211_GROUP_CIPHER_NOT_VALID              = 18,
	RTW_DISCONN_RSN_80211_PAIRWISE_CIPHER_NOT_VALID           = 19,
	RTW_DISCONN_RSN_80211_AKMP_NOT_VALID                      = 20,
	RTW_DISCONN_RSN_80211_UNSUPPORTED_RSN_IE_VERSION          = 21,
	RTW_DISCONN_RSN_80211_INVALID_RSN_IE_CAPAB                = 22,
	RTW_DISCONN_RSN_80211_IEEE_802_1X_AUTH_FAILED             = 23,
	RTW_DISCONN_RSN_80211_CIPHER_SUITE_REJECTED               = 24,
#endif
	/*RTK defined, Driver disconenct from AP after wifi connected and detect something wrong*/
	RTW_DISCONN_RSN_DRV_BASE                            = 60000,
	RTW_DISCONN_RSN_DRV_AP_LOSS                         = 60001,
	RTW_DISCONN_RSN_DRV_AP_CHANGE                       = 60002,
	RTW_DISCONN_RSN_DRV_BASE_END                        = 60099,

	/*RTK defined, Application layer call some API to cause wifi disconnect*/
	RTW_DISCONN_RSN_APP_BASE                            = 60100,
	RTW_DISCONN_RSN_APP_DISCONN                         = 60101,
	RTW_DISCONN_RSN_APP_CONN_WITHOUT_DISCONN            = 60102,
	RTW_DISCONN_RSN_APP_BASE_END                        = 60199,

	RTW_DISCONN_RSN_MAX                                 = 65535,/*0xffff*/
};

/**
 * @brief  The enumeration lists supported band type.
 */
enum rtw_support_band {
	RTW_SUPPORT_BAND_2_4G = 0,     /**< 2.4g band. */
	RTW_SUPPORT_BAND_5G,           /**< 5g band. */
	RTW_SUPPORT_BAND_2_4G_5G_BOTH, /**< 2.4g&5g band. */
	RTW_SUPPORT_BAND_MAX            /**< Max band. */
};

/**
 * @brief The enumeration lists the type of speaker related settings.
 */
enum rtw_speaker_set_type {
	RTW_SPEAKER_SET_INIT = 0,
	RTW_SPEAKER_SET_LATCH_I2S_COUNT = 1,
	RTW_SPEAKER_SET_TSF_TIMER = 2,
};

/**
 * @brief  The enumeration lists transmission frame type for wifi custom ie.
 */
enum rtw_cus_ie_frame_type {
	RTW_CUS_IE_PROBEREQ = BIT(0),  /**< Probe request. */
	RTW_CUS_IE_PROBERSP = BIT(1),  /**< Probe response. */
	RTW_CUS_IE_BEACON	  = BIT(2),     /**< Beacon. */
	RTW_CUS_IE_ASSOCREQ = BIT(3), /**< Assocation request. */
};

/**
 * @brief The enumeration lists rcr mode under promisc.
 */
enum rtw_promisc_filter_mode {
	RTW_PROMISC_FILTER_ALL_PKT,  /**< Receive all packets. */
	RTW_PROMISC_FILTER_AP_ALL     /**< Receive all packtets send by connected ap. */
};

/**
 * @brief The enumeration lists promisc callback return value.
 */
enum rtw_promisc_drv_hdl {
	RTW_PROMISC_NEED_DRV_HDL,  /**< Driver will continue process this pkt. */
	RTW_PROMISC_BYPASS_DRV_HDL     /**< Driver will bypass this pkt. */
};

/**
 * @brief The enumeration lists band type where the sta/ap is located.
 */
enum rtw_band_type {
	RTW_BAND_ON_24G	= 0,   /**< band is on 2.4G                          */
	RTW_BAND_ON_5G	= 1,   /**< band is on 5G                          */
	RTW_BAND_ON_6G	= 2,   /**< band is on 6G                          */
	RTW_BAND_MAX,                  /**< max band                          */
};

/**
 * @brief The enumeration lists wpa mode， size u8
 */
enum rtw_wpa_mode {
	RTW_WPA_AUTO_MODE,       /**< wpa auto mode                       */
	RTW_WPA_ONLY_MODE,       /**< wpa only mode                       */
	RTW_WPA2_ONLY_MODE,      /**< wpa2 only mode                       */
	RTW_WPA3_ONLY_MODE,      /**< wpa3 only mode                       */
	RTW_WPA_WPA2_MIXED_MODE, /**< wpa and wpa2  mixed mode                       */
	RTW_WPA2_WPA3_MIXED_MODE /**< wpa2 and wpa3  mixed mode                       */
};


/**
 * @brief The enumeration lists the possible security types to set when connection.\n
 *			Station mode supports OPEN, WEP, and WPA2.\n
 *			AP mode support OPEN and WPA2.
 */
enum rtw_security {
	RTW_SECURITY_OPEN               = 0,                                                            /**< Open security                           */
	RTW_SECURITY_WEP_PSK            = (WEP_ENABLED),                                                /**< WEP Security with open authentication   */
	RTW_SECURITY_WEP_SHARED         = (WEP_ENABLED | SHARED_ENABLED),                               /**< WEP Security with shared authentication */

	RTW_SECURITY_WPA_TKIP_PSK       = (WPA_SECURITY | TKIP_ENABLED),                                /**< WPA Security with TKIP                  */
	RTW_SECURITY_WPA_AES_PSK        = (WPA_SECURITY | AES_ENABLED),                                 /**< WPA Security with AES                   */
	RTW_SECURITY_WPA_MIXED_PSK      = (WPA_SECURITY | AES_ENABLED | TKIP_ENABLED),                  /**< WPA Security with AES & TKIP            */
	RTW_SECURITY_WPA2_TKIP_PSK		= (WPA2_SECURITY | TKIP_ENABLED),								/**< WPA2 Security with TKIP				 */
	RTW_SECURITY_WPA2_AES_PSK       = (WPA2_SECURITY | AES_ENABLED),                                /**< WPA2 Security with AES                  */
	RTW_SECURITY_WPA2_MIXED_PSK     = (WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED),                 /**< WPA2 Security with AES & TKIP           */
	RTW_SECURITY_WPA_WPA2_TKIP_PSK  = (WPA_SECURITY | WPA2_SECURITY | TKIP_ENABLED),                /**< WPA/WPA2 Security with TKIP             */
	RTW_SECURITY_WPA_WPA2_AES_PSK   = (WPA_SECURITY | WPA2_SECURITY | AES_ENABLED),                 /**< WPA/WPA2 Security with AES              */
	RTW_SECURITY_WPA_WPA2_MIXED_PSK = (WPA_SECURITY  | WPA2_SECURITY | TKIP_ENABLED | AES_ENABLED), /**< WPA/WPA2 Security with AES & TKIP       */
	RTW_SECURITY_WPA3_AES_PSK	 = (WPA3_SECURITY | AES_ENABLED),				/**< WPA3-SAE with AES security			   */
	RTW_SECURITY_WPA3_OWE	 = (WPA3_SECURITY | OWE_ENABLED | AES_ENABLED),				/**< WPA3-OWE with AES security			   */
	RTW_SECURITY_WPA2_WPA3_MIXED = (WPA2_SECURITY | WPA3_SECURITY | AES_ENABLED), /**< WPA3-SAE/WPA2 with AES security		   */
	RTW_SECURITY_WPA2_AES_CMAC      = (WPA2_SECURITY | AES_CMAC_ENABLED),                           /**< WPA2 Security with AES and Management Frame Protection */
};

/**
  * @brief The enumeration lists the BIT 7 HT Rate.
  */
enum rtw_rate {
	RTW_RATE_1M		= 0x02,     /**< 0x02 */
	RTW_RATE_2M		= 0x04,     /**< 0x04 */
	RTW_RATE_5_5M	= 0x0B,     /**< 0x0B */
	RTW_RATE_6M		= 0x0C,     /**< 0x0C */
	RTW_RATE_9M		= 0x12,     /**< 0x12 */
	RTW_RATE_11M 	= 0x16,     /**< 0x16 */
	RTW_RATE_12M = 0x18,      /**< 0x18 */
	RTW_RATE_18M = 0x24,      /**< 0x24 */
	RTW_RATE_24M = 0x30,      /**< 0x30 */
	RTW_RATE_36M = 0x48,      /**< 0x48 */
	RTW_RATE_48M = 0x60,      /**< 0x60 */
	RTW_RATE_54M = 0x6C,      /**< 0x6C */
	RTW_RATE_MCS32	= 0x7F,  /**< 0x7f */
	RTW_RATE_MCS0,   /**< 0x80 */
	RTW_RATE_MCS1,   /**< 0x81 */
	RTW_RATE_MCS2,   /**< 0x82 */
	RTW_RATE_MCS3,   /**< 0x83 */
	RTW_RATE_MCS4,   /**< 0x84 */
	RTW_RATE_MCS5,   /**< 0x85 */
	RTW_RATE_MCS6,
	RTW_RATE_MCS7,

	RTW_RATE_VHT1SS_MCS0 = 0xa0,
	RTW_RATE_VHT1SS_MCS1,
	RTW_RATE_VHT1SS_MCS2,
	RTW_RATE_VHT1SS_MCS3,
	RTW_RATE_VHT1SS_MCS4,
	RTW_RATE_VHT1SS_MCS5,
	RTW_RATE_VHT1SS_MCS6,
	RTW_RATE_VHT1SS_MCS7,
	RTW_RATE_VHT1SS_MCS8,
	RTW_RATE_VHT1SS_MCS9,

	RTW_RATE_HE1SS_MCS0 = 0xd0,   /**< 0xd0 */
	RTW_RATE_HE1SS_MCS1,
	RTW_RATE_HE1SS_MCS2,
	RTW_RATE_HE1SS_MCS3,
	RTW_RATE_HE1SS_MCS4,
	RTW_RATE_HE1SS_MCS5,
	RTW_RATE_HE1SS_MCS6,
	RTW_RATE_HE1SS_MCS7,
	RTW_RATE_HE1SS_MCS8,
	RTW_RATE_HE1SS_MCS9,
	RTW_RATE_HE1SS_MCS10,
	RTW_RATE_HE1SS_MCS11,

	RTW_RATE_UNKNOWN = 0xff  /**< 0xff */
};

/**
  * @brief csi trig management frame subtype.
  */
enum rtw_csi_trig_frame_mgnt {
	RTW_CSI_TRIG_ASSOCREQ   = BIT(0),
	RTW_CSI_TRIG_ASSOCRSP   = BIT(1),
	RTW_CSI_TRIG_REASSOCREQ = BIT(2),
	RTW_CSI_TRIG_REASSOCRSP = BIT(3),
	RTW_CSI_TRIG_PROBEREQ   = BIT(4),
	RTW_CSI_TRIG_PROBERSP   = BIT(5),
	RTW_CSI_TRIG_BEACON     = BIT(8),
	RTW_CSI_TRIG_ATIM       = BIT(9),
	RTW_CSI_TRIG_DISASSOC   = BIT(10),
	RTW_CSI_TRIG_AUTH       = BIT(11),
	RTW_CSI_TRIG_DEAUTH     = BIT(12),
	RTW_CSI_TRIG_ACTION     = BIT(13)
};

/**
  * @brief csi trig control frame subtype.
  */
enum rtw_csi_trig_frame_ctrl {
	RTW_CSI_TRIG_TRIGGER     = BIT(2),
	RTW_CSI_TRIG_BA          = BIT(9),
	RTW_CSI_TRIG_PSPOLL      = BIT(10),
	RTW_CSI_TRIG_RTS         = BIT(11),
	RTW_CSI_TRIG_CTS         = BIT(12),
	RTW_CSI_TRIG_ACK         = BIT(13),
	RTW_CSI_TRIG_CFEND       = BIT(14),
	RTW_CSI_TRIG_CFEND_CFACK = BIT(15)
};

/**
  * @brief csi trig data frame subtype.
  */
enum rtw_csi_trig_frame_data {
	RTW_CSI_TRIG_DATA           = BIT(0),
	RTW_CSI_TRIG_DATA_CFACK     = BIT(1),
	RTW_CSI_TRIG_DATA_CFPOLL    = BIT(2),
	RTW_CSI_TRIG_DATA_CFACKPOLL = BIT(3),
	RTW_CSI_TRIG_DATA_NULL      = BIT(4),
	RTW_CSI_TRIG_CF_ACK         = BIT(5),
	RTW_CSI_TRIG_CF_POLL        = BIT(6),
	RTW_CSI_TRIG_CF_ACKPOLL     = BIT(7),
	RTW_CSI_TRIG_QOS_DATA       = BIT(8),
	RTW_CSI_TRIG_QOS_DATA_NULL	= BIT(12)
};

/**
  * @brief csi enable or config.
  */
enum rtw_csi_action_type {
	RTW_CSI_ACT_EN,           /**< enable or disable csi func */
	RTW_CSI_ACT_CFG,          /**< config csi parameters */
	RTW_CSI_ACT_MAX
};

/**
  * @brief csi group num.
  */
enum rtw_csi_group_num {
	RTW_CSI_GROUP_NUM_1 = 0,  /**< per tone */
	RTW_CSI_GROUP_NUM_2,      /**< per 2tone */
	RTW_CSI_GROUP_NUM_4,      /**< per 4tone */
	RTW_CSI_GROUP_NUM_8_16,   /**< per 8tone for dplus; per 16tone for others */
	RTW_CSI_GROUP_NUM_MAX
};

/**
  * @brief csi mode.
  */
enum rtw_csi_mode {
	RTW_CSI_MODE_NORMAL = 0,  /**< rx normal mode */
	RTW_CSI_MODE_NDP,         /**< rx ndp mode: not support */
	RTW_CSI_MODE_RX_RESP,     /**< rx response mode */
	RTW_CSI_MODE_MAX,
};

/**
  * @brief csi accuracy.
  */
enum rtw_csi_accuracy {
	RTW_CSI_ACCU_1BYTE = 0,   /**< S(8,3) for dplus and S(8,4) for others */
	RTW_CSI_ACCU_2BYTES,      /**< S(16,11) for dplus and S(16,12) for others */
	RTW_CSI_ACCU_MAX
};

/**
  * @brief csi alg_opt.
  */
enum rtw_csi_alg {
	RTW_CSI_ALG_LS = 0,       /**< ls algo */
	RTW_CSI_ALG_SMOTHING,     /**< smothing algo */
	RTW_CSI_ALG_MAX           /**< other algo */
};

/**
  * @brief csi ch_opt.
  */
enum rtw_csi_ch {
	RTW_CSI_CH_LEGACY = 0,    /**< legacy part(L-LTF) channel estmation result */
	RTW_CSI_CH_NON_LEGACY,    /**< non-legacy(HT-LTF) part */
	RTW_CSI_CH_MAX
};

/**
  * @brief csi csi_role.
  */
enum rtw_csi_role {
	RTW_CSI_OP_ROLE_TRX = 0,  /**< both trx */
	RTW_CSI_OP_ROLE_TX  = 1,  /**< only tx csi triggering frame */
	RTW_CSI_OP_ROLE_RX  = 2,  /**< only rx csi triggering frame for fetching csi report */
	RTW_CSI_OP_ROLE_MAX
};

/**
  * @brief Power Mgnt
  */
enum rtw_power_mgnt_mode {
	RTW_PS_MODE_ACTIVE	= 0	, ///< active mode
	RTW_PS_MODE_LEGACY		,       ///< legacy mode
	RTW_PS_MODE_UAPSD_WMM	, ///< uapsd wmm mode
	RTW_PS_MODE_WTNPS	, ///< wtn ps mode
};

/**
* @brief The enumeration lists the trp tis types.
*/
enum rtw_trp_tis_mode {
	RTW_TRP_TIS_DISABLE = 0,
	RTW_TRP_TIS_NORMAL = 1,
	RTW_TRP_TIS_DYNAMIC = 3,					///< enable dynamic mechanism
	RTW_TRP_TIS_FIX_ACK_RATE = 5,			///< fix ack rate to 6M
	RTW_TRP_TIS_FIX_PHY_ACK_HIGH_RATE = 9	///< fix phy ack rate to RATE_54M | RATE_48M | RATE_36M | RATE_24M | RATE_18M | RATE_12M | RATE_9M | RATE_6M
};

/**
* @brief The enumeration lists the edcca mode types.
*/
enum rtw_edcca_mode {
	RTW_EDCCA_NORM	= 0, /* normal */
	RTW_EDCCA_ADAPT	= 1, /* adaptivity */
	RTW_EDCCA_CS	= 2, /* carrier sense */
	RTW_EDCCA_DISABLE	= 9, /* disable */
};

/**
* @brief The enumeration lists the antdiv mode types.
*/
enum rtw_antdiv_mode {
	RTW_ANTDIV_AUTO     = 0,    /* auto antdiv */
	RTW_ANTDIV_FIX_MAIN = 1,    /* fix main ant */
	RTW_ANTDIV_FIX_AUX  = 2,    /* fix aux ant */
	RTW_ANTDIV_DISABLE  = 0xF,  /* disable antdiv*/
};

/**
  * @brief  The enumeration lists the bss types. size u8
  */
enum rtw_bss_type {
	RTW_BSS_TYPE_INFRASTRUCTURE 	= 0, /**< Denotes infrastructure network                  */
	RTW_BSS_TYPE_WTN_HELPER          		= 1, /**< Denotes an wtn helper network           */
	RTW_BSS_TYPE_UNKNOWN
};

/**
  * @brief  The enumeration lists the scan options.
  */
enum rtw_scan_type {
	RTW_SCAN_NOUSE			= 0x00,  /**< default value */
	RTW_SCAN_ACTIVE              	= 0x01,     /**< active scan */
	RTW_SCAN_PASSIVE             	= 0x02,    /**< passive scan*/
	RTW_SCAN_NO_HIDDEN_SSID	= 0x04, /**< Filter hidden ssid APs*/
	RTW_SCAN_REPORT_EACH	= 0x08,    /**< report each */
	RTW_SCAN_WITH_P2P		= 0x10    /**< for P2P usage */
};

/**
  * @brief  The enumeration lists the WPS types.
  */
enum rtw_wps_type {
	RTW_WPS_TYPE_DEFAULT 		    		= 0,	/**< default type */
	RTW_WPS_TYPE_USER_SPECIFIED 		= 1,	/**< user specified type */
	RTW_WPS_TYPE_MACHINE_SPECIFIED   	= 2,	/**< machine specified type */
	RTW_WPS_TYPE_REKEY 			        	= 3,	/**< retry key type */
	RTW_WPS_TYPE_PUSHBUTTON 		    	= 4,	/**< push button type */
	RTW_WPS_TYPE_REGISTRAR_SPECIFIED 	= 5,	/**< register specified type */
	RTW_WPS_TYPE_NONE                   		= 6, 	/**< none */
	RTW_WPS_TYPE_WSC                    		= 7,	/**< wsc type */
};

/**
  * @brief The enumeration lists the supported operation mode by WIFI driver,
  *			including station and AP mode.
  */
enum rtw_drv_op_mode {
	RTW_MODE_NONE	= 0,		///<none
	RTW_MODE_STA		= 1,	///<sta mode
	RTW_MODE_AP		= 2,	///<ap mode
	RTW_MODE_NAN		= 3,	///<nan mode
	RTW_MODE_MAX
};

/**
  * The enumeration lists the power save status.
  */
enum rtw_ips_level {
	RTW_IPS_WIFI_OFF = 0,
	RTW_IPS_WIFI_PG,
	RTW_IPS_LEVEL_MAX,
};

enum rtw_uapsd_max_sp {
	RTW_UAPSD_NO_LIMIT,
	RTW_UAPSD_TWO_MSDU,
	RTW_UAPSD_FOUR_MSDU,
	RTW_UAPSD_SIX_MSDU
};

/**
  * @brief  The enumeration lists all the network mode.
  */
/*TODO: rom should check because moved from rom_rtw_defs.h*/
enum rtw_wireless_mode {
	RTW_80211_INVALID = 0,
	RTW_80211_B       = BIT(0),
	RTW_80211_A       = BIT(1),
	RTW_80211_G       = BIT(2),
	RTW_80211_N       = BIT(3),
	RTW_80211_AC      = BIT(4),
	RTW_80211_AX      = BIT(5),

	/* Type for current wireless mode */
	RTW_80211_BG      = (RTW_80211_B | RTW_80211_G),
	RTW_80211_GN      = (RTW_80211_G | RTW_80211_N),
	RTW_80211_AN      = (RTW_80211_A | RTW_80211_N),
	RTW_80211_BN      = (RTW_80211_B | RTW_80211_N),
	RTW_80211_BGN     = (RTW_80211_B | RTW_80211_G | RTW_80211_N),
	RTW_80211_BGAX    = (RTW_80211_B | RTW_80211_G | RTW_80211_AX),
	RTW_80211_GAX     = (RTW_80211_G | RTW_80211_AX),
	RTW_80211_A_AC    = (RTW_80211_A | RTW_80211_AC),
	RTW_80211_A_AX    = (RTW_80211_A | RTW_80211_AX),

	/* Capability -Type for registry default wireless mode */
	RTW_80211_AGN     = (RTW_80211_A | RTW_80211_G | RTW_80211_N),
	RTW_80211_ABGN    = (RTW_80211_A | RTW_80211_B | RTW_80211_G | RTW_80211_N),
	RTW_80211_ANAC    = (RTW_80211_A | RTW_80211_N | RTW_80211_AC),
	RTW_80211_24G_MIX = (RTW_80211_B | RTW_80211_G | RTW_80211_N  | RTW_80211_AX),
	RTW_80211_5G_MIX  = (RTW_80211_A | RTW_80211_N | RTW_80211_AC | RTW_80211_AX),
	RTW_80211_MAX     = (RTW_80211_24G_MIX | RTW_80211_5G_MIX),
};

// Tx Power Limit Table Size
/*TODO: rom should check because moved from rom_rtw_defs.h*/
enum rtw_txpwr_lmt {
	TXPWR_LMT_FCC = 0,
	TXPWR_LMT_MKK = 1,      /* Japan */
	TXPWR_LMT_ETSI = 2,     /* CE */
	TXPWR_LMT_IC = 3,       /* Canada */
	TXPWR_LMT_KCC = 4,      /* South Korea */
	TXPWR_LMT_ACMA = 5, 	/* Australia */
	TXPWR_LMT_CHILE = 6,    /* Chile */
	TXPWR_LMT_MEXICO = 7,   /* Mexico */
	TXPWR_LMT_WW = 8,       /* Worldwide, The mininum of all */
	TXPWR_LMT_GL = 9,		/* Global */
	TXPWR_LMT_UKRAINE = 10, /* Ukraine */
	TXPWR_LMT_CN = 11,       /* China */
	TXPWR_LMT_QATAR = 12,   /* Qatar */
	TXPWR_LMT_UK = 13,      /* Great Britain (United Kingdom; England) */
	TXPWR_LMT_NCC = 14,     /* Taiwan */
	TXPWR_LMT_EXT = 15,     /* Customer Customization */

	/* ===== Add new power limit above this line. ===== */
	TXPWR_LMT_CONST_MAX     /* unchanging part define max */
};

enum rtw_frame_type {
	RTW_MGT_TYPE  =	(0),
	RTW_CTRL_TYPE =	(BIT(2)),
	RTW_DATA_TYPE =	(BIT(3)),
	RTW_QOS_DATA_TYPE	= (BIT(7) | BIT(3)),	//!< QoS Data
};

enum rtw_frame_type_subtype {
	// below is for mgt frame
	RTW_ASSOCREQ       = (0 | RTW_MGT_TYPE),
	RTW_ASSOCRSP       = (BIT(4) | RTW_MGT_TYPE),
	RTW_REASSOCREQ     = (BIT(5) | RTW_MGT_TYPE),
	RTW_REASSOCRSP     = (BIT(5) | BIT(4) | RTW_MGT_TYPE),
	RTW_PROBEREQ       = (BIT(6) | RTW_MGT_TYPE),
	RTW_PROBERSP       = (BIT(6) | BIT(4) | RTW_MGT_TYPE),
	RTW_BEACON         = (BIT(7) | RTW_MGT_TYPE),
	RTW_ATIM           = (BIT(7) | BIT(4) | RTW_MGT_TYPE),
	RTW_DISASSOC       = (BIT(7) | BIT(5) | RTW_MGT_TYPE),
	RTW_AUTH           = (BIT(7) | BIT(5) | BIT(4) | RTW_MGT_TYPE),
	RTW_DEAUTH         = (BIT(7) | BIT(6) | RTW_MGT_TYPE),
	RTW_ACTION         = (BIT(7) | BIT(6) | BIT(4) | RTW_MGT_TYPE),

	// below is for control frame
	RTW_PSPOLL         = (BIT(7) | BIT(5) | RTW_CTRL_TYPE),
	RTW_RTS            = (BIT(7) | BIT(5) | BIT(4) | RTW_CTRL_TYPE),
	RTW_CTS            = (BIT(7) | BIT(6) | RTW_CTRL_TYPE),
	RTW_ACK            = (BIT(7) | BIT(6) | BIT(4) | RTW_CTRL_TYPE),
	RTW_BA            	= (BIT(7) | BIT(4) | RTW_CTRL_TYPE),
	RTW_CFEND          = (BIT(7) | BIT(6) | BIT(5) | RTW_CTRL_TYPE),
	RTW_CFEND_CFACK    = (BIT(7) | BIT(6) | BIT(5) | BIT(4) | RTW_CTRL_TYPE),
	RTW_TRIGGER        = (BIT(5) | RTW_CTRL_TYPE),

	// below is for data frame
	RTW_DATA           = (0 | RTW_DATA_TYPE),
	RTW_DATA_CFACK     = (BIT(4) | RTW_DATA_TYPE),
	RTW_DATA_CFPOLL    = (BIT(5) | RTW_DATA_TYPE),
	RTW_DATA_CFACKPOLL = (BIT(5) | BIT(4) | RTW_DATA_TYPE),
	RTW_DATA_NULL      = (BIT(6) | RTW_DATA_TYPE),
	RTW_CF_ACK         = (BIT(6) | BIT(4) | RTW_DATA_TYPE),
	RTW_CF_POLL        = (BIT(6) | BIT(5) | RTW_DATA_TYPE),
	RTW_CF_ACKPOLL     = (BIT(6) | BIT(5) | BIT(4) | RTW_DATA_TYPE),
	RTW_QOS_DATA_NULL	= (BIT(6) | RTW_QOS_DATA_TYPE),
};

/** @} End of WIFI_Exported_Enumeration_Types group*/

/** @addtogroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */
/**********************************************************************************************
 *                                  common structures
 *********************************************************************************************/
#pragma pack(1)/*rtw_ssid and rtw_mac are 1 byte alignment for some issues long long ago*/
/**
  * @brief  The structure is used to describe the SSID (Service Set Identification), i.e., the name of Access Point.
  */
struct rtw_ssid {
	u8		len;     /**< SSID length, i.e., equal to the length of `val`. The length of ssid should not > @ref RTW_ESSID_MAX_SIZE.  */
	u8		val[RTW_ESSID_MAX_SIZE + 1]; /**< SSID name (AP name).*/
};

/**
  * @brief  The structure is used to describe the unique 6-byte MAC address.
  */
struct rtw_mac {
	u8		octet[6]; /**< Unique 6-byte MAC address. */
};
#pragma pack()

/**********************************************************************************************
 *                                   scan structures
 *********************************************************************************************/

#pragma pack(1)/*scan related structs are 1 byte alignment for some issues long long ago*/
/**
  * @brief  The structure is used to describe the busyness of a channel for ACS(Automatic Channel Selection).
  */
struct rtw_acs_mntr_rpt {
	u16 meas_time; /**< Measurements time on this channel, unit:ms.*/
	u16 busy_time; /**< Time that the primary channel was sensed busy, unit:ms.*/
	u16 tx_time;   /**< Time spent transmitting frame on this channel, unit:ms.*/
	s8 noise;      /**< Noise power measured on this channel, unit: dbm.*/
	u8 channel;    /**< The scanned channel number. */
};

/**
  * @brief  The structure is used to describe the details of a scanned AP.
  */
struct rtw_scan_result {
	struct rtw_ssid    ssid;             /**< Service Set Identification (i.e. Name of Access Point). */
	struct rtw_mac     bssid;            /**< Basic Service Set Identification (i.e. MAC address of Access Point). */
	s16                signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent. */
	u8				   bss_type;         /**< The bss type. The noraml type is infrastructure BSS. Val: @ref RTW_BSS_TYPE_INFRASTRUCTURE, @ref RTW_BSS_TYPE_WTN_HELPER.*/
	u32                security;         /**< The security type of this AP. Val: @ref RTW_SECURITY_OPEN, @ref RTW_SECURITY_WEP_PSK...*/
	u8
	wps_type;         /**< The WPS(Wi-Fi Protected Setup) types supported by this AP. Val: @ref RTW_WPS_TYPE_DEFAULT, @ref RTW_WPS_TYPE_USER_SPECIFIED...*/
	u32                channel;          /**< Radio channel that the AP beacon was received on. */
	u8                 band;             /**< The frequency ranges used by this AP. Val: @ref RTW_BAND_ON_5G, @ref RTW_BAND_ON_24G. */

	/** The wireless spectrum management regulations of which region followed by the AP. `country_code` is coded
	 * according to ISO 3166 standard. Specific values can refer to ameba_wifi_country_code_table_usrcfg.c.\n
	 * e.g. China: country_code[0] = 'C', country_code[1] = 'N'. */
	u8                 country_code[2];
	s8                 wireless_mode;    /**< The wireless mode of this AP. Val: @ref RTW_80211_B, @ref RTW_80211_A...*/
	u8                 rom_rsvd[3];
};

/**
  * @brief  The structure is used to describe the scan time per channel.
  */
struct rtw_channel_scan_time {
	u16		active_scan_time;      /**< Active scan time per channel, units: millisecond, default is 110ms. */
	u16		passive_scan_time;     /**< Passive scan time per channel, units: millisecond, default is 110ms. */
};

/**
  * @brief  The structure is used to describe the scan parameters used for scan.
  */
struct rtw_scan_param {
	u8                               options; /**< The scan option, such as active scan. Val: @ref RTW_SCAN_ACTIVE, @ref RTW_SCAN_PASSIVE...*/
	u8                              *ssid;    /**< The data length of string pointed by ssid should not exceed @ref RTW_ESSID_MAX_SIZE. */
	u8                              *channel_list;      /**< The list of specified channels to be scanned.*/
	u8                               channel_list_num;  /**< The total number in `channel_list`.*/
	struct rtw_channel_scan_time     chan_scan_time;    /**< The scan time per channel.*/

	/** Config the max number of recorded AP. When set to 0, use default value 64.
	 * When the number of scanned APs exceed `max_ap_record_num`, the AP(s) with smallest rssi will be discarded. */
	u16                              max_ap_record_num;
	void                            *scan_user_data;
	s32(*scan_user_callback)(u32 ap_num, void *user_data); /**< Used for normal asynchronized mode. */
	s32(*scan_report_each_mode_user_callback)(struct rtw_scan_result *scanned_ap_info, void *user_data);  /**< Used for @ref RTW_SCAN_REPORT_EACH mode. */
	s32(*scan_report_acs_user_callback)(struct rtw_acs_mntr_rpt *acs_mntr_rpt);  /**< Used for report acs info.*/
};
#pragma pack()

/**********************************************************************************************
 *                                     connect structures
 *********************************************************************************************/
/**
  * @brief  The structure is used for Linux host to get wpa_supplicant's info for STA connect,
  *         which RTOS not need.
  */
struct rtw_wpa_supp_connect {
	u8 rsnxe_ie[RSNXE_MAX_LEN];  /**< The RSNXE IE in beacon of AP which STA is trying to connect.*/
};

/**
  * @brief	The structure is used to describe the connection setting about SSID,
  * 		security type and password,etc., used when trying to connect an AP.
  * @note
  *        1. If this struct is used for wifi connect, the channel is used to config
  * 	      whether it is a full channel scan(when channel is set to 0), or it will
  * 	      only scan one channel(do active scan on the configured channel).
  *        2. `pscan_option` set to @ref RTW_PSCAN_FAST_SURVEY means do fast survey on the specified channel
  * 	      set to 0 means do normal scan on the specified channel or full channel.
  */
struct rtw_network_info {
	struct rtw_ssid				ssid;  /**< The AP's name and the length of name (should not exceed @ref RTW_ESSID_MAX_SIZE). */
	struct rtw_mac				bssid; /**< The unique 6-byte MAC address of AP. */
	u32							security_type; /**< Only need to be set when use WEP (@ref RTW_SECURITY_WEP_PSK @ref RTW_SECURITY_WEP_SHARED), Other case will automatically adjust according to the AP.*/
	u8				           *password;	   /**< The password of AP which sta is trying to connect. */
	s32 						password_len;  /**< The data length of string pointed by password should not exceed @ref RTW_MAX_PSK_LEN. Equal to length of `password`. */
	s32 						key_id;		   /**< Only need to be set when use WEP. Val: 0~3.*/
	u8				            channel;		/**< Set to 0 means full channel scan, set to other value means only scan on the specified channel. */
	u8
	pscan_option;	/**< Can set to @ref RTW_PSCAN_FAST_SURVEY for fast survey, which means quick scan, involves using an active scan on a specified channel, scanning for 25ms each time, and attempting up to 7 times until the target AP is found.. */
	u8 				            is_wps_trigger;	/**< Connection triggered by WPS process.*/
	struct rtw_wpa_supp_connect	wpa_supp;   /**< Only used by Linux host to specific some details required for STA connect, which RTOS do not use. */
	struct rtw_mac				prev_bssid; /**< The BSSID of the AP before roaming. */
	u8							by_reconn; /**< Connection triggered by RTK auto reconnect process. */
	u8							rom_rsvd[4];
};

/**
 * @brief  The structure is retry_limit for auth/assoc/key exchange.
 * @note   All re_limits are limited to within 10.
 */
struct rtw_conn_step_retries {
	u8 reauth_limit : 4;             /**< Indicate retry limit of auth-open/shared key. */
	u8 sae_reauth_limit : 4;         /**< Indicate retry limit of sae auth. */
	u8 reassoc_limit : 4;            /**< Indicate retry limit of assoc. */
	u8 eapol_key_rsend_limit : 4;    /**< Indicate retry limit of 4way handshake. */
};

/**********************************************************************************************
 *                                     wifi_status structures
 *********************************************************************************************/
/**
  * @brief  The structure is used to store the WIFI setting gotten from WIFI driver.
  * @note	Size can't be changed.
  */
struct rtw_wifi_setting {
	u8		mode;   /**< The mode of current wlan interface, val: @ref RTW_MODE_STA, @ref RTW_MODE_AP, @ref RTW_MODE_NAN. */
	u8 		ssid[33];   /**< The ssid of connected AP or softAP. */
	u8		bssid[6];   /**< The bssid of connected AP or softAP. */
	u8		channel;
	u32		security_type; /**< The security type of connected AP or softAP, val: @ref RTW_SECURITY_OPEN, @ref RTW_SECURITY_WEP_PSK...*/
	u8 		password[RTW_MAX_PSK_LEN];   /**< The password of connected AP or softAP. */
	u8		key_idx;
	u8		alg;		/**< Encryption algorithm. */
	u32		auth_type;
	u8		is_wps_trigger;	/**< Connection triggered by WPS process.*/
	u32		rom_rsvd;
};

/**
  * @brief  The structure is used to describe the traffic statistics.
  */
union rtw_traffic_stats {
	struct rtw_sta_traffic_stats {
		u32	rx_packets;			/**< total packets received on the interface(exclude custom pkts).*/
		u32	tx_packets;			/**< total packets transmitted on the interface.*/
		u8	cur_rx_data_rate;	/**< Current rx data rate, val: @ref RTW_RATE_1M, @ref RTW_RATE_2M...*/
		u8	cur_tx_data_rate;	/**< Current tx data rate, val: @ref RTW_RATE_1M, @ref RTW_RATE_2M... */
	} sta; /**< For STA mode statistic.*/
	struct rtw_ap_traffic_stats {
		u32	rx_packets;			/**< total packets received on the interface(exclude custom pkts).*/
		u32	tx_packets;			/**< total packets transmitted on the interface.*/
	} ap; /**< For SOFTAP mode statistic.*/
};

/**
  * @brief  The structure is used to describe the phy statistics.
  */
union rtw_phy_stats {
	struct rtw_sta_phy_stats {
		s8	rssi;          /**< Average mixed rssi in 1 sec. */
		s8	data_rssi;          /**< Average data rssi in 1 sec. */
		s8	beacon_rssi;          /**< Average beacon rssi in 1 sec. */
		s8	snr;          /**< Average snr in 1 sec (not include cck rate).*/
	} sta; /**< For STA mode statistic.*/
	struct rtw_ap_phy_stats {
		s8	data_rssi;          /**< Average data rssi in 1 sec. */
	} ap; /**< For SOFTAP mode statistic.*/
	struct rtw_cmn_phy_stats {
		u8  cca_clm; /**< Channel loading measurement ratio by cca (the ratio of CCA = 1 in number of samples). driver do clm every 2 seconds, the value is the lastest result. */
		u8	edcca_clm; /**< Channel loading measurement ratio by edcca (the ratio of EDCCA = 1 in number of samples). The value is also the lastest result. */
		u8	clm_channel; /**< Channel corresponding to the latest clm result.*/
	} cmn; /**< For common statistic.*/
};

/**********************************************************************************************
 *                                     softap structures
 *********************************************************************************************/
/**
  * @brief  The structure is used to describe the setting about SSID,
  *			security type, password and default channel, used to start AP mode.
  * @note  The data length of string pointed by ssid should not exceed @ref RTW_ESSID_MAX_SIZE,
  *        and the data length of string pointed by password should not exceed @ref RTW_MAX_PSK_LEN.
  */
struct rtw_softap_info {
	struct rtw_ssid		ssid;
	u8		            hidden_ssid;
	u32					security_type; /**< Val: @ref RTW_SECURITY_OPEN, @ref RTW_SECURITY_WEP_PSK...*/
	u8 		           *password;
	u8 		            password_len;
	u8		            channel;
};

#ifndef CONFIG_FULLMAC
/**
  * @brief  The structure is used to describe the associated clients of SoftAP.
  */
struct rtw_client_list {
	u32    count;         /**< Number of associated clients in the list.    */
	struct rtw_mac mac_list[MACID_HW_MAX_NUM - 2]; /**< Max length array of MAC addresses. */
};
#endif

/**
  * @brief  The structure is used to describe the cfg parameters used for channel switch announcement.
  */
struct rtw_csa_parm {
	u8 new_chl; /**< The new channel will be switched to. */
	u8 chl_switch_cnt; /**< The channel switch cnt, after chl_switch_cnt*102ms, ap will switch to new channel. */
	u8 action_type;	/**< 0: unicast csa action, 1: broadcast csa action, other values: disable transmit csa action. */
	u8 bc_action_cnt; /**< Indicate the number of broadcast csa actions to send for each beacon interval. only valid when action_type = 1.*/
	/** @brief User handle when softap switch channel by csa function. This callback will be called after channel switch is
	 *         done, and will return the new channel number and channel switch result.
	  * @param[in] channel:  New channel.
	  * @param[in] ret: Val: @ref RTK_FAIL, @ref RTK_SUCCESS.
	  */
	void (*callback)(u8 channel, s8 ret);
};

/**********************************************************************************************
 *                                     promisc structures
 *********************************************************************************************/
/**
*@brief The structure is used to describe the rx packet's detail information.
*/
struct rtw_rx_pkt_info {
	s8 recv_signal_power;
	u8 data_rate; /**< Val: @ref RTW_RATE_1M, @ref RTW_RATE_2M...*/
	u8 channel;
	u8 *buf;
	u32 len;
};

/**
*@brief The structure is used to describe the selected promisc mode and callback function.
*/
struct rtw_promisc_para {
	/** @brief Receive all packets in the air or set some filtering conditions.
		- @ref RTW_PROMISC_FILTER_ALL_PKT : Receive all packets in the air.
		- @ref RTW_PROMISC_FILTER_AP_ALL : Receive all packtets send by connected AP.
		*/
	u8 filter_mode;
	/** @brief User handle a packet.
	  * @param[in] pkt_info:  The pointer of a struct rtw_rx_pkt_info which store the packet's detail information.
	  * @return Should the driver continue processing this packet after user has processed.
	  * 	- @ref RTW_PROMISC_NEED_DRV_HDL : Driver continue processing this packet, This setting is usually required when the STA remains connected.
	  *     - @ref RTW_PROMISC_BYPASS_DRV_HDL : Driver drop this packet, This setting is usually used when STA does not need connect.*/
	u8(*callback)(struct rtw_rx_pkt_info *pkt_info);
};

/**********************************************************************************************
 *                                     speaker structures
 *********************************************************************************************/
union rtw_speaker_set {
	struct rtw_speaker_init {
		u8 mode;              /**< 0 for slave, 1 for master. */
		u8 nav_thresh;        /**< Unit 128us. */
		u8 relay_en;          /**< Relay control. */
	} init; /**< For wifi speaker setting case @ref RTW_SPEAKER_SET_INIT.*/
	struct rtw_speaker_i2s {
		u8 port;           /**< 0 for select port 0's TSFT to trigger audio latch count, 1 for port 1. */
		u8 latch_period;      /**< 0 for trigger audio latch period is 4.096ms, 1 for 8.192ms. */
	} latch_i2s_count; /**< For wifi speaker setting case @ref RTW_SPEAKER_SET_LATCH_I2S_COUNT.*/
	struct rtw_speaker_tsf_timer {
		u8 enable;			/**< 1 for enable, 0 for disable. */
		u64 tsft;           /**< Unit us. */
		u8 port;           /**< 0 for select port 0's TSFT to trigger twt timer interrupt, 1 for port 1. */
	} tsf_timer; /**< For wifi speaker setting case @ref RTW_SPEAKER_SET_TSF_TIMER.*/
};

/**********************************************************************************************
 *                                     csi structures
 *********************************************************************************************/
#pragma pack(1) /* csi report header should be 1 byte alignment */
/**
 * @brief  The structure for layout of csi report header.
 */

struct rtw_csi_header {
	u16 csi_signature;          /**< pattern that be used to detect a new CSI packet */
	u8 hdr_len;
	/**< client MAC address, specifies transmitter address (MAC address) for CSI triggering frame in Active CSI
	 * and receiver address for CSI triggering frame in Passive CSI.*/
	u8 mac_addr[6];
	/**< client MAC address, specifies destination address (MAC address) for CSI triggering frame in Active CSI
	 * and source address for CSI triggering frame in Passive CSI (purpose to fetch CSI information from response packet)*/
	u8 trig_addr[6];
	u32 hw_assigned_timestamp;  /**< csi timestamp,unit:us */
	u32 csi_sequence;           /**< csi data sequence number */
	u32 csi_data_length;        /**< csi raw data length, unit: byte */
	u8 csi_valid;               /**< indicates the current csi raw data whether valid */
	u8 channel;                 /**< operation channel */
	u8 bandwidth;               /**< operation bandwidth */
	u8 rx_rate;                 /**< the rate of rx packet which used to obtain csi info */
	u8 protocol_mode;           /**< protocol mode of the response packet, ofdm(0)/ht(1)/vht(2)/he(3) */
	u16 num_sub_carrier;        /**< number of subcarriers contain in csi raw data */
	u8 num_bit_per_tone;        /**< csi data wordlength(sum of I and Q) */
	s8 rssi[2];                 /**< rssi[0]: dbm, rssi[1] is reserved */
	s8 evm[2];                  /**< reserved, db */
	u8 rxsc;                    /**< indicate which sub 20M channel is used to transmit packet */
	u8 n_rx;                    /**< reserved */
	u8 n_sts;                   /**< reserved */
	u8 trig_flag;               /**< indicate source of role for triggering csi in sta-sta csi mode, trig_addr is invalid if zero */
	u8 rsvd[5];
};
#pragma pack()

/**
  * @brief  The structure is used to describe the cfg parameters used for csi report.
  * @note  The mac_addr if not specified, the default value must be 0.
  */
struct rtw_csi_action_parm {
	u8 group_num;   /**< val: @ref RTW_CSI_GROUP_NUM_1, @ref RTW_CSI_GROUP_NUM_2... */
	u8 accuracy;    /**< val: @ref RTW_CSI_ACCU_1BYTE, @ref RTW_CSI_ACCU_2BYTES */
	u8 alg_opt;     /**< val: @ref RTW_CSI_ALG_LS, @ref RTW_CSI_ALG_SMOTHING */
	u8 ch_opt;      /**< val: @ref RTW_CSI_CH_LEGACY, @ref RTW_CSI_CH_NON_LEGACY */
	u8 csi_role;    /**< indicate csi operation role, val: @ref RTW_CSI_OP_ROLE_TRX, @ref RTW_CSI_OP_ROLE_TX, @ref RTW_CSI_OP_ROLE_RX */
	u8 mode;        /**< val: @ref RTW_CSI_MODE_NORMAL, @ref RTW_CSI_MODE_NDP, @ref RTW_CSI_MODE_RX_RESP*/
	u8 act;         /**< val: @ref RTW_CSI_ACT_EN, @ref RTW_CSI_ACT_CFG */
	u16 trig_frame_mgnt;	/**< indicate management frame subtype of rx csi triggering frame for fetching csi, val: @ref RTW_CSI_TRIG_ASSOCREQ... */
	u16 trig_frame_ctrl;	/**< indicate control frame subtype of rx csi triggering frame for fetching csi, val: @ref RTW_CSI_TRIG_TRIGGER... */
	u16 trig_frame_data;	/**< indicate data frame subtype of rx csi triggering frame for fetching csi, val: @ref RTW_CSI_TRIG_DATA... */
	u8 enable;
	u8 trig_period;
	u8 data_rate;
	u8 data_bw;
	u8 mac_addr[6];
	u8 multi_type;  /**< 0-uc csi triggering frame; 1-bc csi triggering frame */

	/** indicate role for transmitting CSI triggering frame in METHOD4 and
	 * role for transmitting response ACK for CSI triggering frame in METHOD1_Variant,
	 * others are reserved. \n
	 * Value=1 ~ 15 (0 is reserved)*/
	u8 trig_flag;
};


/**********************************************************************************************
 *                                     other structures
 *********************************************************************************************/
/**
  * @brief  The structure is power limit regu map.
  */
struct _pwr_lmt_regu_remap {
	u8	domain_code;
	u8	PwrLmtRegu_2g;	/**< Not distinguish 2.4G and 5G; just set PwrLmtRegu_2g. */
	u8	PwrLmtRegu_5g;
};

/**
  * @brief  The structure is used to describe the raw frame.
  */
struct rtw_raw_frame_desc {
	u8 wlan_idx;      /**< Index of wlan interface which will transmit. */
	u8 device_id;     /**< Index of peer device which as a rx role for receiving this pkt, and will be update when linked peer. */
	u8 *buf;          /**< Poninter of buf where raw data is stored.*/
	u16 buf_len;      /**< The length of raw data.*/
	u8 tx_rate;	     /**< Val: @ref RTW_RATE_1M, @ref RTW_RATE_2M...*/
	u8 retry_limit;
	u8 ac_queue;      /**< 0/3 for BE, 1/2 for BK, 4/5 for VI, 6/7 for VO. */
	u8 sgi : 1;       /**< 1 for enable data short. */
	u8 agg_en : 1;    /**< Aggregation of tx_raw frames. 1:enable; 0-disable. */
};

/**
 * @brief  The structure is used to set WIFI custom ie list.
 *
 */
struct rtw_custom_ie {
	/** Format:
	 * <table>
	 * <tr><th>1byte</th><th>1byte</th><th>length bytes</th></tr>
	 * <tr><td>element ID</td><td>length</td><td>content</td></tr>
	 * </table>
	 */
	u8 *ie;
	u8 type;    /**< Val: @ref RTW_CUS_IE_PROBEREQ, @ref RTW_CUS_IE_PROBERSP...*/
};

/**
 * @brief  The structure is used to describe channel info.
 */

struct rtw_channel_info {
	u8 channel;      /**< Channel id */
	u8 scan_type;    /**< 1 for passive, 0 for active. */
};

/**
 * @brief  The structure is used to describe channel list.
 */
struct rtw_channel_list {
	u8 ch_num;         /**< Number of available channel in the list.    */
	struct rtw_channel_info ch_info[RTW_MAX_CHANNEL_NUM];
};


/**
 * @brief  The structure is used to describe channel plan and country code.
 */
struct rtw_country_code_table {
	u8 char2[2];   /**< Country code. */
	u8 channel_plan; /**< Channel plan code. */
	u8 pwr_lmt;      /**< Tx power limit index. */
};

struct rtw_tx_power_ctl_info {
	s8	tx_pwr_force; /**< Currently user can specify tx power for all rate, unit 0.25dbm.*/
	u8	b_tx_pwr_force_enbale : 1; /**< 1 for enable, 0 for disable. */
};

/** @} End of WIFI_Exported_Structure_Types group*/
/** @} End of WIFI_Exported_Types group*/
/** @} End of WIFI_API group */

/* not included in any api groups*/
extern  struct wifi_user_conf wifi_user_config;
extern struct rtw_wifi_setting wifi_setting[2];

#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_TYPES_H
