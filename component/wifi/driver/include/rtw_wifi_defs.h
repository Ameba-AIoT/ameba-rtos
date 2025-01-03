/**
  ******************************************************************************
  * @file    rtw_wifi_defs.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef	__RTW_WIFI_DEFS_H_
#define __RTW_WIFI_DEFS_H_

#define _WEAK           __attribute__ ((weak))

/** @defgroup WIFI_Exported_Constants WIFI Exported Constants
  * @{
  */

/** @defgroup WLAN_Defs
   *@{
   */
#ifndef WLAN0_NAME
#define WLAN0_NAME		"wlan0"    /**< WLAN0 NAME */
#endif

#ifndef WLAN1_NAME
#define WLAN1_NAME		"wlan1"    /**< WLAN1 NAME */
#endif

#ifndef WLAN0_IDX
#define WLAN0_IDX	0  /**< wlan0 index */
#endif

#ifndef WLAN1_IDX
#define WLAN1_IDX	1  /**< wlan1 index */
#endif

#ifndef STA_WLAN_INDEX
#define STA_WLAN_INDEX	0
#endif

#ifndef SOFTAP_WLAN_INDEX
#define SOFTAP_WLAN_INDEX	1
#endif

#ifndef NAN_WLAN_INDEX
#define NAN_WLAN_INDEX	2
#endif

#ifndef STA_WLAN_NAME
#define STA_WLAN_NAME	"wlan0"
#endif

#ifndef SOFTAP_WLAN_NAME
#define SOFTAP_WLAN_NAME	"wlan1"
#endif

#ifndef NAN_WLAN_NAME
#define NAN_WLAN_NAME	"wlan2"
#endif

#define IsSupported24G(band_type) ((band_type) & BAND_CAP_2G? TRUE : FALSE)
#define IsSupported5G(band_type) ((band_type) & BAND_CAP_5G ?  TRUE : FALSE)

#define IsLegacyOnly(NetType)  ((NetType) == ((NetType) & (WLAN_MD_11BG | WLAN_MD_11A)))

#define IsSupportedTxOFDM(NetType) ((NetType) & (WLAN_MD_11G | WLAN_MD_11A) ? TRUE : FALSE)

#define is_supported_ht(NetType) ((NetType) & (WLAN_MD_11N) ? TRUE : FALSE)
#define is_supported_vht(NetType) ((NetType) & (WLAN_MD_11AC) ? TRUE : FALSE)
#define is_supported_he(NetType) ((NetType) & (WLAN_MD_11AX) ? TRUE : FALSE)

#define IS_HT_RATE(_rate)				(((_rate) & 0x80) ? TRUE : FALSE)
#define IS_CCK_RATE(_rate) 				(MGN_1M == _rate || _rate == MGN_2M || _rate == MGN_5_5M || _rate == MGN_11M )
#define IS_OFDM_RATE(_rate)				(MGN_6M <= _rate && _rate <= MGN_54M )

/**
 * @brief  mac address format.
 */
#define MAC_ARG(x)		((u8*)(x))[0],((u8*)(x))[1],\
				((u8*)(x))[2],((u8*)(x))[3],\
				((u8*)(x))[4],((u8*)(x))[5]
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define is_broadcast_mac_addr(Addr) ((((Addr[0]) & 0xff) == 0xff) && (((Addr[1]) & 0xff) == 0xff) && \
(((Addr[2]) & 0xff) == 0xff) && (((Addr[3]) & 0xff) == 0xff) && (((Addr[4]) & 0xff) == 0xff) && \
(((Addr[5]) & 0xff) == 0xff))
#define is_zero_mac_addr(Addr)	((Addr[0] == 0x00) && (Addr[1] == 0x00) && (Addr[2] == 0x00) &&   \
                    (Addr[3] == 0x00) && (Addr[4] == 0x00) && (Addr[5] == 0x00))

/**
  * @}
  */


/** @defgroup Security_Defs
   *@{
   */
#define AUTH_ALG_OPEN_SYSTEM	0x1
#define AUTH_ALG_SHARED_KEY	0x2
#define AUTH_ALG_SAE		0x8
#define AUTH_ALG_LEAP		0x00000004

#define WEP_ENABLED		0x0001		/**< wep enable */
#define TKIP_ENABLED		0x0002		/**< tkip enable */
#define AES_ENABLED		0x0004		/**< aes enable */
#define WSEC_SWFLAG		0x0008		/**< WSEC SWFLAG */
#define AES_CMAC_ENABLED	0x0010		/**< aes cmac enable */
#define ENTERPRISE_ENABLED	0x0020		/**< enterprise enable */
#define OWE_ENABLED		0X0040		/**< owe enable */
#define SHARED_ENABLED		0x00008000	/**< shared enable */
#define WPA_SECURITY		0x00200000	/**< wpa */
#define WPA2_SECURITY		0x00400000	/**< wpa2 */
#define WPA3_SECURITY		0x00800000	/**< wpa3 */
#define WPS_ENABLED		0x10000000	/**< wps  enable*/

/*cipher suite from 802.11-2016 p884*/
#define WIFI_CIPHER_SUITE_WEP_40			0x000FAC01
#define WIFI_CIPHER_SUITE_TKIP				0x000FAC02
#define WIFI_CIPHER_SUITE_CCMP_128		0x000FAC04
#define WIFI_CIPHER_SUITE_WEP_104			0x000FAC05
#define WIFI_CIPHER_SUITE_BIP_CMAC_128	0x000FAC06
#define WIFI_CIPHER_SUITE_GCMP				0x000FAC08
#define WIFI_CIPHER_SUITE_GCMP_256		0x000FAC09
#define WIFI_CIPHER_SUITE_CCMP_256		0x000FAC0A
#define WIFI_CIPHER_SUITE_BIP_GMAC_128	0x000FAC0B
#define WIFI_CIPHER_SUITE_BIP_GMAC_256	0x000FAC0C
#define WIFI_CIPHER_SUITE_BIP_CMAC_256	0x000FAC0D

#define RTW_WPA2_MAX_PSK_LEN		(64)
#define RTW_WPA3_MAX_PSK_LEN		(128)		/**< maxmum psk length */
#define RTW_MAX_PSK_LEN		RTW_WPA3_MAX_PSK_LEN
#define RTW_MIN_PSK_LEN		(8)		/**< minimum psk length */
#define MCSSET_LEN		16		/**<mcsset length */
#define RTW_OWE_KEY_LEN		32 /*32(Temporarily support group 19 with 256 bit public key)*/

/* adaptivity */
#define RTW_ADAPTIVITY_EN_DISABLE			0
#define RTW_ADAPTIVITY_EN_ENABLE			1

#ifndef CONFIG_WIFI_CRITICAL_CODE_SECTION
#define CONFIG_WIFI_CRITICAL_CODE_SECTION
#endif

#define RTW_SEND_AND_WAIT_ACK 				2
#define RTW_SEND_BY_HIGH_RATE				4 // IEEE80211_OFDM_RATE_54MB
#define RTW_NAV_BY_USER						8

/**
  * @}
  */

/** @defgroup Packet_Defs
   *@{
   */
#define	PACKET_NORMAL			0
#define	PACKET_DHCP			1
#define	PACKET_ARP			2
#define	PACKET_EAPOL			3

/**
  * @}
  */


/** @defgroup WPA_MODE_CHECK_Defs
   *@{
   */
#define WPA2_MODE_CHECK(wpa_mode) ((wpa_mode == WPA_AUTO_MODE) || (wpa_mode == WPA2_ONLY_MODE) || (wpa_mode == WPA3_ONLY_MODE) || \
									(wpa_mode == WPA_WPA2_MIXED_MODE) || (wpa_mode == WPA2_WPA3_MIXED_MODE))
#define WPA_MODE_CHECK(wpa_mode) ((wpa_mode == WPA_AUTO_MODE) ||(wpa_mode == WPA_ONLY_MODE) ||(wpa_mode == WPA_WPA2_MIXED_MODE))

/**
  * @}
  */


/**
  * @}
  */

/** @defgroup WIFI_Exported_Types WIFI Exported Types
* @{
*/

/** @addtogroup Enums
   *@{
   */

/**
 * @brief The enumeration lists band type
 */
enum band_type {
	BAND_ON_24G	= 0,   /**< band is on 2.4G                          */
	BAND_ON_5G	= 1,   /**< band is on 5G                          */
	BAND_ON_6G	= 2,   /**< band is on 6G                          */
	BAND_MAX,                  /**< max band                          */
};

/**
 * @brief The enumeration lists wpa mode， size u8
 */
enum {
	WPA_AUTO_MODE,  /**< wpa auto mode                       */
	WPA_ONLY_MODE,   /**< wpa only mode                       */
	WPA2_ONLY_MODE, /**< wpa2 only mode                       */
	WPA3_ONLY_MODE, /**< wpa3 only mode                       */
	WPA_WPA2_MIXED_MODE, /**< wpa and wpa2  mixed mode                       */
	WPA2_WPA3_MIXED_MODE /**< wpa2 and wpa3  mixed mode                       */
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
enum {
	MGN_1M		= 0x02,     /**< 0x02 */
	MGN_2M		= 0x04,     /**< 0x04 */
	MGN_5_5M	= 0x0B,     /**< 0x0B */
	MGN_6M		= 0x0C,     /**< 0x0C */
	MGN_9M		= 0x12,     /**< 0x12 */
	MGN_11M 	= 0x16,     /**< 0x16 */
	MGN_12M = 0x18,      /**< 0x18 */
	MGN_18M = 0x24,      /**< 0x24 */
	MGN_24M = 0x30,      /**< 0x30 */
	MGN_36M = 0x48,      /**< 0x48 */
	MGN_48M = 0x60,      /**< 0x60 */
	MGN_54M = 0x6C,      /**< 0x6C */
	MGN_MCS32	= 0x7F,  /**< 0x7f */
	MGN_MCS0,   /**< 0x80 */
	MGN_MCS1,   /**< 0x81 */
	MGN_MCS2,   /**< 0x82 */
	MGN_MCS3,   /**< 0x83 */
	MGN_MCS4,   /**< 0x84 */
	MGN_MCS5,   /**< 0x85 */
	MGN_MCS6,
	MGN_MCS7,
	MGN_MCS8,
	MGN_MCS9,
	MGN_MCS10,
	MGN_MCS11,
	MGN_MCS12,
	MGN_MCS13,
	MGN_MCS14,
	MGN_MCS15,
	MGN_MCS16,
	MGN_MCS17,
	MGN_MCS18,
	MGN_MCS19,
	MGN_MCS20,
	MGN_MCS21,
	MGN_MCS22,
	MGN_MCS23,
	MGN_MCS24,
	MGN_MCS25,
	MGN_MCS26,
	MGN_MCS27,
	MGN_MCS28,
	MGN_MCS29,
	MGN_MCS30,
	MGN_MCS31,
	MGN_VHT1SS_MCS0,
	MGN_VHT1SS_MCS1,
	MGN_VHT1SS_MCS2,
	MGN_VHT1SS_MCS3,
	MGN_VHT1SS_MCS4,
	MGN_VHT1SS_MCS5,
	MGN_VHT1SS_MCS6,
	MGN_VHT1SS_MCS7,
	MGN_VHT1SS_MCS8,
	MGN_VHT1SS_MCS9,
	MGN_VHT2SS_MCS0,
	MGN_VHT2SS_MCS1,
	MGN_VHT2SS_MCS2,
	MGN_VHT2SS_MCS3,
	MGN_VHT2SS_MCS4,
	MGN_VHT2SS_MCS5,
	MGN_VHT2SS_MCS6,
	MGN_VHT2SS_MCS7,
	MGN_VHT2SS_MCS8,
	MGN_VHT2SS_MCS9,
	MGN_VHT3SS_MCS0,
	MGN_VHT3SS_MCS1,
	MGN_VHT3SS_MCS2,
	MGN_VHT3SS_MCS3,
	MGN_VHT3SS_MCS4,
	MGN_VHT3SS_MCS5,
	MGN_VHT3SS_MCS6,
	MGN_VHT3SS_MCS7,
	MGN_VHT3SS_MCS8,
	MGN_VHT3SS_MCS9,
	MGN_VHT4SS_MCS0,
	MGN_VHT4SS_MCS1,
	MGN_VHT4SS_MCS2,
	MGN_VHT4SS_MCS3,
	MGN_VHT4SS_MCS4,
	MGN_VHT4SS_MCS5,
	MGN_VHT4SS_MCS6,
	MGN_VHT4SS_MCS7,
	MGN_VHT4SS_MCS8,
	MGN_VHT4SS_MCS9,
	MGN_HE1SS_MCS0 = 0xd0,   /**< 0xd0 */
	MGN_HE1SS_MCS1,
	MGN_HE1SS_MCS2,
	MGN_HE1SS_MCS3,
	MGN_HE1SS_MCS4,
	MGN_HE1SS_MCS5,
	MGN_HE1SS_MCS6,
	MGN_HE1SS_MCS7,
	MGN_HE1SS_MCS8,
	MGN_HE1SS_MCS9,
	MGN_HE1SS_MCS10,
	MGN_HE1SS_MCS11,
	MGN_HE2SS_MCS0,
	MGN_HE2SS_MCS1,
	MGN_HE2SS_MCS2,
	MGN_HE2SS_MCS3,
	MGN_HE2SS_MCS4,
	MGN_HE2SS_MCS5,
	MGN_HE2SS_MCS6,
	MGN_HE2SS_MCS7,
	MGN_HE2SS_MCS8,
	MGN_HE2SS_MCS9,
	MGN_HE2SS_MCS10,
	MGN_HE2SS_MCS11,
	MGN_HE3SS_MCS0,
	MGN_HE3SS_MCS1,
	MGN_HE3SS_MCS2,
	MGN_HE3SS_MCS3,
	MGN_HE3SS_MCS4,
	MGN_HE3SS_MCS5,
	MGN_HE3SS_MCS6,
	MGN_HE3SS_MCS7,
	MGN_HE3SS_MCS8,
	MGN_HE3SS_MCS9,
	MGN_HE3SS_MCS10,
	MGN_HE3SS_MCS11,
	MGN_HE4SS_MCS0,
	MGN_HE4SS_MCS1,
	MGN_HE4SS_MCS2,
	MGN_HE4SS_MCS3,
	MGN_HE4SS_MCS4,
	MGN_HE4SS_MCS5,
	MGN_HE4SS_MCS6,
	MGN_HE4SS_MCS7,
	MGN_HE4SS_MCS8,
	MGN_HE4SS_MCS9,
	MGN_HE4SS_MCS10,
	MGN_HE4SS_MCS11 = 0xff,  /**< 0xff */
	MGN_UNKNOWN
};

/**
  * @brief csi trig management frame subtype.
  */
enum {
	CSI_TRIG_ASSOCREQ   = BIT(0),
	CSI_TRIG_ASSOCRSP   = BIT(1),
	CSI_TRIG_REASSOCREQ = BIT(2),
	CSI_TRIG_REASSOCRSP = BIT(3),
	CSI_TRIG_PROBEREQ   = BIT(4),
	CSI_TRIG_PROBERSP   = BIT(5),
	CSI_TRIG_BEACON     = BIT(8),
	CSI_TRIG_ATIM       = BIT(9),
	CSI_TRIG_DISASSOC   = BIT(10),
	CSI_TRIG_AUTH       = BIT(11),
	CSI_TRIG_DEAUTH     = BIT(12),
	CSI_TRIG_ACTION     = BIT(13)
};

/**
  * @brief csi trig control frame subtype.
  */
enum {
	CSI_TRIG_TRIGGER     = BIT(2),
	CSI_TRIG_BA          = BIT(9),
	CSI_TRIG_PSPOLL      = BIT(10),
	CSI_TRIG_RTS         = BIT(11),
	CSI_TRIG_CTS         = BIT(12),
	CSI_TRIG_ACK         = BIT(13),
	CSI_TRIG_CFEND       = BIT(14),
	CSI_TRIG_CFEND_CFACK = BIT(15)
};

/**
  * @brief csi trig data frame subtype.
  */
enum {
	CSI_TRIG_DATA           = BIT(0),
	CSI_TRIG_DATA_CFACK     = BIT(1),
	CSI_TRIG_DATA_CFPOLL    = BIT(2),
	CSI_TRIG_DATA_CFACKPOLL = BIT(3),
	CSI_TRIG_DATA_NULL      = BIT(4),
	CSI_TRIG_CF_ACK         = BIT(5),
	CSI_TRIG_CF_POLL        = BIT(6),
	CSI_TRIG_CF_ACKPOLL     = BIT(7),
	CSI_TRIG_QOS_DATA       = BIT(8),
	CSI_TRIG_QOS_DATA_NULL	= BIT(12)
};

/**
  * @brief csi enable or config
  */
enum {
	CSI_ACT_EN,    /**< enable or disable csi func */
	CSI_ACT_CFG,  /**< config csi parameters */
	CSI_ACT_MAX
};

/**
  * @brief csi group num
  */
enum {
	CSI_GROUP_NUM_1 = 0,  /**< per tone */
	CSI_GROUP_NUM_2,         /**< per 2tone */
	CSI_GROUP_NUM_4,        /**< per 4tone */
	CSI_GROUP_NUM_16,     /**< per 16tone */
	CSI_GROUP_NUM_MAX
};

/**
  * @brief csi mode
  */
enum {
	CSI_MODE_NORMAL = 0,   ///<   normal mode
	CSI_MODE_NDP,                ///<   ndp mode
	CSI_MODE_RX_RESP,         ///<    rx rsp mode
	CSI_MODE_MAX,                 ///<    max mode
};

/**
  * @brief csi accuracy.
  */
enum {
	CSI_ACCU_1BYTE = 0, /**< CSI_ACCU_1BYTE: S(8,4) */
	CSI_ACCU_2BYTES,  /**< CSI_ACCU_2BYTE: S(16,12) */
	CSI_ACCU_MAX
};

/**
  * @brief csi alg_opt.
  */
enum {
	CSI_ALG_LS = 0,  /**< ls algo */
	CSI_ALG_SMOTHING, /**< smothing algo */
	CSI_ALG_MAX            /**< other algo */
};

/**
  * @brief csi ch_opt.
  */
enum {
	CSI_CH_LEGACY = 0, /**< legacy part(L-LTF) channel estmation result */
	CSI_CH_NON_LEGACY,  /**< non-legacy(HT-LTF) part */
	CSI_CH_MAX
};

/**
  * @brief csi play_role for sta mode.
  */
enum {
	CSI_OP_ROLE_TRX = 0,  /**< both trx */
	CSI_OP_ROLE_TX  = 1,  /**< only tx csi triggering frame */
	CSI_OP_ROLE_RX  = 2,  /**< only rx csi triggering frame for fetching csi report */
	CSI_OP_ROLE_MAX
};

/**
  * @brief Power Mgnt
  */
enum power_mgnt_mode {
	PS_MODE_ACTIVE	= 0	, ///< active mode
	PS_MODE_LEGACY		,       ///< legacy mode
	PS_MODE_UAPSD_WMM	, ///< uapsd wmm mode
	PS_MODE_WTNPS	, ///< wtn ps mode
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
* @brief The enumeration lists band types, size u8
*/
enum {
	RTW_802_11_BAND_5GHZ   = 0, /**< Denotes 5GHz radio band */
	RTW_802_11_BAND_2_4GHZ = 1,  /**< Denotes 2.4GHz radio band */
};

/**
  * @brief  The enumeration lists the bss types. size u8
  */
enum {
	RTW_BSS_TYPE_INFRASTRUCTURE 	= 0, /**< Denotes infrastructure network                  */
	RTW_BSS_TYPE_WTN_HELPER          		= 1, /**< Denotes an wtn helper network           */
	RTW_BSS_TYPE_UNKNOWN
};

/**
  * @brief  The enumeration lists the scan options.
  */
enum {
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
enum {
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
enum {
	RTW_MODE_NONE	= 0,		///<none
	RTW_MODE_STA		= 1,	///<sta mode
	RTW_MODE_AP		= 2,	///<ap mode
	RTW_MODE_NAN		= 3,	///<nan mode
	RTW_MODE_MAX
};

/**
  * @brief  The enumeration lists the supported autoreconnect mode by WIFI driver.
  */
enum rtw_autoreconnect_mode {
	RTW_AUTORECONNECT_DISABLE,  ///< disable auto reconnect
	RTW_AUTORECONNECT_FINITE,    ///< finite auto reconnect
	RTW_AUTORECONNECT_INFINITE ///< infinite auto reconnect
};

/* For freertos, core, hal, rf, halbb. */
enum phl_phy_idx {
	HW_PHY_0,
	HW_PHY_1,
	HW_PHY_MAX
};

/**
  * @brief  only internal used events.
  */
enum  {
	/* only internal used */
	WIFI_EVENT_INTERNAL_BASE		 	= 100,
	WIFI_EVENT_RX_MGNT					= 101,
	WIFI_EVENT_RX_MGNT_AP				= 102,
	WIFI_EVENT_EXTERNAL_AUTH_REQ		= 103,

	WIFI_EVENT_WPA_STA_4WAY_START		= 104,
	WIFI_EVENT_WPA_AP_4WAY_START		= 105,
	WIFI_EVENT_WPA_STA_4WAY_RECV		= 106,
	WIFI_EVENT_WPA_AP_4WAY_RECV			= 107,
	WIFI_EVENT_WPA_SET_PSK_INFO			= 108,

	WIFI_EVENT_OWE_START_CALC			= 109,
	WIFI_EVENT_OWE_PEER_KEY_RECV		= 110,

#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K) || defined(CONFIG_IEEE80211R)
	WIFI_EVENT_KVR_CAP_UPDATE			= 111,
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K)
	WIFI_EVENT_NB_RESP_RECV				= 112,
#endif
#ifdef CONFIG_IEEE80211V
	WIFI_EVENT_BTM_REQ_RECV				= 113,
	WIFI_EVENT_BTM_DEBUG_CMD			= 114,
#endif
#ifdef CONFIG_IEEE80211R
	WIFI_EVENT_FT_AUTH_START			= 115,
	WIFI_EVENT_FT_RX_MGNT				= 116,
#endif
#endif

	WIFI_EVENT_DEAUTH_INFO_FLASH		= 117,
	WIFI_EVENT_INTERNAL_MAX,
};
/**
  * The enumeration lists the flash operation status.
  */
enum flash_operation_type {
	FLASH_READ = 0,   /**< read  flash                       */
	FLASH_WRITE,       /**< write  flash                       */
};

/**
  * The enumeration lists the power save status.
  */
enum {
	IPS_WIFI_OFF = 0,
	IPS_WIFI_PG,
	IPS_LEVEL_MAX,
};

/**
  * The enumeration lists the p2p role type.
  */
enum rtw_p2p_role {
	P2P_ROLE_DISABLE = 0,
	P2P_ROLE_DEVICE = 1,
	P2P_ROLE_CLIENT = 2,
	P2P_ROLE_GO = 3
};

enum gen_ie_type {
	P2PWPS_PROBE_REQ_IE = 0,
	P2PWPS_PROBE_RSP_IE,
	P2PWPS_BEACON_IE,
	P2PWPS_ASSOC_REQ_IE,
	P2PWPS_ASSOC_RSP_IE
};

enum UAPSD_MAX_SP {
	NO_LIMIT,
	TWO_MSDU,
	FOUR_MSDU,
	SIX_MSDU
};

/**
  * @}
  */

/**
  * @}
  */

#endif
