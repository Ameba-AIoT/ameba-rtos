/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_app_basic.h
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

#ifndef __WIFI_CONF_BASIC_H
#define __WIFI_CONF_BASIC_H

/** @defgroup WIFI_API
 *  @brief      WIFI_API module
 *  @{
 */
#ifndef CONFIG_FULLMAC
#include "rtw_wifi_constants.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_intf_drv_to_bt.h"
#include "dlist.h"
#include <rtw_skbuff.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup WIFI_Exported_Constants WIFI Exported Constants
  * @{
  */

/** @defgroup SSID_LEN_Defs
   *@{
   */

#define INIC_MAX_SSID_LENGTH (33)

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
 * @brief The enumeration lists the results of the function.
 */
typedef enum _rtw_result_t {
	RTW_SUCCESS                      = 0,    /**< Success */
	RTW_TIMEOUT                      = 2,    /**< Timeout */
	RTW_INVALID_KEY                  = 4,        /**< Invalid key */

	RTW_ERROR                        = -1,   /**< Generic Error */
	RTW_BADARG                       = -2,   /**< Bad Argument */
	RTW_BUSY                         = -16,  /**< Busy */
	RTW_NOMEM                        = -27,  /**< No Memory */
} rtw_result_t;

#if defined(__IAR_SYSTEMS_ICC__) || defined (__GNUC__) || defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* SET pack mode 1-alignment for the following area. */
#pragma pack(1)
#endif

/**
 * @brief The enumeration lists rcr mode under promisc
 */
typedef enum _promisc_rcr_mode {
	RCR_ALL_PKT,  /**< all packets */
	RCR_AP_ALL     /**< only ap related packets */
} promisc_rcr_mode;

/**
 * @brief The enumeration lists promisc callback return value
 */
typedef enum _promisc_result_t {
	NEED_DRIVER_HANDLE,  /**< driver will continue process this pkt */
	BYPASS_DRIVER_HANDLE     /**< driver will bypass this pkt */
} promisc_result_t;


/**
 * @brief The enumeration lists the type of pmksa operations.
 */
enum  {
	PMKSA_SET = 0,
	PMKSA_DEL = 1,
	PMKSA_FLUSH = 2,
};

/**
 * @brief The enumeration typedef export to user. */
typedef enum rtw_promisc_level rtw_rcr_level_t;
typedef enum rtw_country_code rtw_country_code_t;
typedef enum rtw_wpa_mode_type rtw_wpa_mode;
typedef enum rtw_autoreconnect_mode rtw_autoreconnect_mode_t;
typedef enum rtw_event_indicate rtw_event_indicate_t;
typedef enum ips_level_type ips_level;
typedef enum rtw_bss_type rtw_bss_type_t;
typedef enum rtw_security rtw_security_t;
typedef enum rtw_wps_type rtw_wps_type_t;
typedef enum rtw_802_11_band rtw_802_11_band_t;
typedef enum rtw_security rtw_security_t;
typedef enum rtw_join_status_type rtw_join_status_t;
typedef enum mgn_rate_type MGN_RATE;
typedef enum rtw_channel_switch_res rtw_channel_switch_res_t;
typedef enum rtw_mode_type rtw_mode_t;
typedef enum rtw_packet_filter_rule rtw_packet_filter_rule_t;
typedef enum rtw_csi_group_type rtw_csi_group_num;
typedef enum rtw_csi_accuracy_type rtw_csi_accuracy;
typedef enum rtw_csi_alg_opt_type rtw_csi_alg_opt;
typedef enum rtw_csi_ch_opt_type rtw_csi_ch_opt;
typedef enum rtw_csi_mode_type rtw_csi_mode;
typedef enum rtw_csi_action_type rtw_csi_action;

/** @} */


/** @addtogroup Structs
   *@{
   */

/**
  * @brief  The structure is used to describe the SSID.
  */
typedef struct _rtw_ssid_t {
	unsigned char		len;     /**< SSID length */
	unsigned char		val[INIC_MAX_SSID_LENGTH]; /**< SSID name (AP name)  */
} rtw_ssid_t;

/**
  * @brief  The structure is used to describe the unique 6-byte MAC address.
  */
typedef struct _rtw_mac_t {
	unsigned char		octet[6]; /**< Unique 6-byte MAC address */
} rtw_mac_t;

/**
  * @brief  The structure is used to describe the scan result of the AP.
  */
typedef struct rtw_scan_result {
	struct _rtw_ssid_t              SSID;             /**< Service Set Identification (i.e. Name of Access Point)                    */
	struct _rtw_mac_t               BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point)       */
	signed short		                  signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent */
	enum rtw_bss_type          bss_type;         /**< Network type                                                              */
	enum rtw_security          security;         /**< Security type                                                             */
	enum rtw_wps_type          wps_type;         /**< WPS type                                                                  */
	unsigned int                      channel;          /**< Radio channel that the AP beacon was received on                          */
	enum rtw_802_11_band       band;             /**< Radio band                                                                */
} rtw_scan_result_t;

typedef struct _rtw_channel_scan_time_t {
	unsigned short		active_scan_time;      /**< active scan time per channel, units: millisecond, default is 100ms */
	unsigned short		passive_scan_time;     /**< passive scan time per channel, units: millisecond, default is 110ms */
} rtw_channel_scan_time_t;

/* DO NOT define or use any enum _rtw_result_t in linux. Use asm-generic/errno.h instead. */
typedef enum _rtw_result_t (*scan_user_callback_t)(unsigned int ap_num, void *user_data);
typedef enum _rtw_result_t (*scan_report_each_mode_user_callback_t)(struct rtw_scan_result *scanned_ap_info, void *user_data);

/**
  * @brief  The structure is used to describe the scan parameters used for scan,
  * @note  The data length of string pointed by ssid should not exceed 32,
  *        and the data length of string pointed by password should not exceed 64.
  */
typedef struct _rtw_scan_param_t {
	//enum rtw_scan_option 					options;
	u8									options;
	char									*ssid;
	unsigned char							*channel_list;
	unsigned char							channel_list_num;
	struct _rtw_channel_scan_time_t 				chan_scan_time;
	unsigned short						max_ap_record_num;	   /**< config the max number of recorded AP, when set to 0, use default value 64 */
	void									*scan_user_data;
	scan_user_callback_t					scan_user_callback;   /**< used for normal asynchronized mode */
	scan_report_each_mode_user_callback_t	scan_report_each_mode_user_callback; /*used for RTW_SCAN_REPORT_EACH mode */
} rtw_scan_param_t;

#if defined(__IAR_SYSTEMS_ICC__) || defined (__GNUC__) || defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/* REMOVE pack mode 1-alignment for the next definitions, use DEFAULT. */
#pragma pack()
#endif

/**
  * @brief  The structure is used for fullmac to get wpa_supplicant's info for STA connect,
  */
typedef struct _rtw_wpa_supp_connect_t {
	u8 rsnxe_ie[RSNXE_MAX_LEN];
} rtw_wpa_supp_connect_t;

/**
  * @brief  The structure is used to describe the setting about SSID,
  *			security type, password and default channel, used to start AP mode.
  * @note  The data length of string pointed by ssid should not exceed 32,
  *        and the data length of string pointed by password should not exceed 64.
  */
typedef struct _rtw_softap_info_t {
	struct _rtw_ssid_t		ssid;
	unsigned char		hidden_ssid;
	enum rtw_security		security_type;
	unsigned char 		*password;
	unsigned char 		password_len;
	unsigned char		channel;
} rtw_softap_info_t;

typedef struct _raw_data_desc_t {
	unsigned char		wlan_idx;      /**< index of wlan interface which will transmit */
	unsigned char		*buf;          /**< poninter of buf where raw data is stored*/
	unsigned short		buf_len;      /**< the length of raw data*/
	unsigned short		flags;        /**< send options*/
} raw_data_desc_t;

/**
 * @brief  The structure is crypt info.
 */
struct rtw_crypt_info {
	u8 pairwise;
	u8 mac_addr[6];
	u8 wlan_idx;
	u16 key_len;
	u8 key[32];
	u8 key_idx;
	u8 driver_cipher;
	u8 transition_disable_exist;
	u8 transition_disable_bitmap;
	u8 camid: 7;	/**< camid is valid only when force_camid=1*/
	u8 force_camid: 1;
	u8 rpt_mode;
};


/**
 * @brief  The structure is pmksa ops.
 */
struct rtw_pmksa_ops_t {
	u8 ops_id;
	u8 wlan_idx;
	u8 pmkid[16];
	u8 mac_addr[6];
	u8 pmk[32];/*pmksa is maintained in NP when use wpa_lite*/
};

struct wpa_sae_param_t {
	unsigned char 		peer_mac[6];
	unsigned char 		self_mac[6];
	u8					h2e;
};

struct rtw_owe_param_t {
#ifdef CONFIG_OWE_SUPPORT
	u16 group;
	u8 pub_key[RTW_OWE_KEY_LEN];/*32(Temporarily support group 19 with 256 bit public key)*/
	u8 pub_key_len;
	u8 peer_mac[6];
#endif
};

struct rtw_kvr_param_t {
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K) || defined(CONFIG_IEEE80211R)
	u8 nb_active;
	u8 btm_active;
	unsigned char 		peer_mac[6];
	unsigned char 		self_mac[6];
#ifdef CONFIG_IEEE80211R
	u8 ft_active;
	u16 mdid;
	u8 ft_cap;
	u8 privacy;
	u32 grp_privacy;
	u8 ie[5 + MAX_WPA_IE_LEN + MAX_FT_IE_LEN]; /* 1.NP->AP: rsnie; 2.AP->NP: mdie+rsnie+ftie*/
	u32 ielen;
#endif
#endif
};

/**
  * @brief  The structure is used to describe the phy statistics
  */
typedef struct _rtw_phy_statistics_t {
	signed char	rssi;          /*!<average mixed rssi in 1 sec (for STA mode) */
	signed char	data_rssi;          /*!<average data rssi in 1 sec (for STA mode) */
	signed char	beacon_rssi;          /*!<average beacon rssi in 1 sec (for STA mode) */
	signed char	snr;          /*!< average snr in 1 sec (not include cck rate, for STA mode)*/
	unsigned int
	cca_clm; /*<channel loading measurement ratio by cca (the ratio of CCA = 1 in number of samples). driver do clm every 2 seconds, the value is the lastest result>*/
	unsigned int	edcca_clm; /*<channel loading measurement ratio by edcca (the ratio of EDCCA = 1 in number of samples). The value is also the lastest result>*/
	unsigned int	clm_channel; /*<channel corresponding to the latest clm result.>*/
	unsigned int	tx_retry;
	unsigned short	tx_drop;
	unsigned int	rx_drop;
	unsigned int	supported_max_rate;
} rtw_phy_statistics_t;

typedef struct _promisc_para_t {
	enum _promisc_rcr_mode filter_mode;
	enum _promisc_result_t (*callback)(void *);
} promisc_para_t, *ppromisc_para_t;

typedef void (*rtw_joinstatus_callback_t)(enum rtw_join_status_type join_status);

/**
  * @brief	The structure is used to describe the connection setting about SSID,
  * 		security type and password,etc., used when connecting to an AP.
  * @note  The data length of string pointed by ssid should not exceed 32,
  * 	   and the data length of string pointed by password should not exceed 64.
  * @note  If this struct is used for wifi connect, the channel is used to config
  * 	   whether it is a full channel scan(when channel is set to 0), or it will
  * 	   only scan one channel(do active scan on the configured channel).
  * @note  pscan_option set to PSCAN_FAST_SURVEY means do fast survey on the specified channel
  * 	   set to 0 means do normal scan on the specified channel or full channel.
  */
typedef struct _rtw_network_info_t {
	struct _rtw_ssid_t					ssid;
	struct _rtw_mac_t					bssid;
	u32						security_type;	/* because enum rtw_security type would occupy 8 bytes on PC/Raspi, so use u32 instead of enum to keep structure consistent */
	unsigned char				*password;
	int 						password_len;
	int 						key_id;
	unsigned char				channel;		/**< set to 0 means full channel scan, set to other value means only scan on the specified channel */
	unsigned char				pscan_option;	/**< used when the specified channel is set, set to 0 for normal partial scan, set to PSCAN_FAST_SURVEY for fast survey*/
	unsigned char 				is_wps_trigger;	/**< connection triggered by WPS process**/
	rtw_joinstatus_callback_t	joinstatus_user_callback;	/**< user callback for processing joinstatus, please set to NULL if not use it */
	struct _rtw_wpa_supp_connect_t	wpa_supp;
	struct _rtw_mac_t		prev_bssid;
} rtw_network_info_t;
/** @} */

/** @} */


/** @defgroup WIFI_Exported_Constants WIFI Exported Constants
  * @{
  */

/** @defgroup API_INFO_Defs
   *@{
   */
/**
* @brief Create RTW_ENABLE_API_INFO
*/
#define RTW_ENABLE_API_INFO


/**
* @brief Create RTW_API_INFO
*/
#if defined RTW_ENABLE_API_INFO || defined __DOXYGEN__
#define RTW_API_INFO DiagPrintf
#else
#define RTW_API_INFO(args)
#endif
/** @} */


/** @defgroup MAC_Defs
   *@{
   */
/**
* @brief compare mac address
*/
#define CMP_MAC( a, b)		(((a[0])==(b[0]))&& \
				((a[1])==(b[1]))&& \
				((a[2])==(b[2]))&& \
				((a[3])==(b[3]))&& \
				((a[4])==(b[4]))&& \
				((a[5])==(b[5])))

/**
* @brief mac format
*/
#define MAC_FMT			"%02x:%02x:%02x:%02x:%02x:%02x"
/** @} */


/** @defgroup WIFI_MAKEU32_Def
   *@{
   */
/**
* @brief Create u32_t value from bytes
*/
#define WIFI_MAKEU32(a,b,c,d)    (((u32_t)((a) & 0xff) << 24) | \
                               ((u32_t)((b) & 0xff) << 16) | \
                               ((u32_t)((c) & 0xff) << 8)  | \
                                (u32_t)((d) & 0xff))
/** @} */


/** @defgroup SCAN_Defs
   *@{
   */
/**
* @brief scan longest wait time
*/
#define SCAN_LONGEST_WAIT_TIME	(12000)

/**
* @brief enable for partial channel scan
*/
#define PSCAN_ENABLE		0x01

/**
* @brief set to select scan time to FAST_SURVEY_TO, otherwise SURVEY_TO
*/
#define PSCAN_FAST_SURVEY	0x02

/**
* @brief set to select scan time to FAST_SURVEY_TO and resend probe request
*/
#define PSCAN_SIMPLE_CONFIG	0x04
/** @} */


/** @defgroup BIT_Def
   *@{
   */
/**
* @brief define BIT
*/
#ifndef BIT
#define BIT(x)	((__u32)1 << (x))
#endif

/**
 * @brief  parameters of rf_calibration_disable.
 */
#define DIS_DPK BIT(0)

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
 * @brief  The enumeration is transmission type for wifi custom ie.
 */
typedef enum _rtw_custom_ie_type_t {
	PROBE_REQ = BIT(0),  ///<probe request
	PROBE_RSP = BIT(1),  ///<probe response
	BEACON	  = BIT(2),     ///<beacon
	ASSOC_REQ = BIT(3), ///<assocation request
} rtw_custom_ie_type_t;

/**
 * @brief  The enumeration is wl band type.
 */
typedef enum _WL_BAND_TYPE {
	WL_BAND_2_4G = 0,   ///<2.4g band
	WL_BAND_5G,            ///<5g band
	WL_BAND_2_4G_5G_BOTH, ///<2.4g&5g band
	WL_BANDMAX  ///< max band
} WL_BAND_TYPE;
/**
  * @}
  */


/** @addtogroup Structs
   *@{
   */

/** @brief The structure is pkt info.
   *@{
   */
struct rx_pkt_info {
	s8 recv_signal_power;
	enum mgn_rate_type data_rate;
	u8 channel;
	u8 *buf;
	u32 len;
};


typedef int (*wifi_do_fast_connect_ptr)(void);
typedef int (*write_fast_connect_info_ptr)(unsigned int data1, unsigned int data2);
typedef void (*ap_channel_switch_callback_t)(unsigned char channel, enum rtw_channel_switch_res ret);
typedef void (*p_wlan_autoreconnect_hdl_t)(enum rtw_security, char *, int, char *, int, int, char);

typedef void (*wifi_jioninfo_free_ptr)(u8 iface_type);

/**
  * @brief  The structure is used to store the WIFI setting gotten from WIFI driver.
  */
typedef struct _rtw_wifi_setting_t {
	enum rtw_mode_type			mode;   /**< the mode of current wlan interface */
	unsigned char 		ssid[33];   /**< the ssid of connected AP or softAP */
	unsigned char		bssid[6];   /**< the bssid of connected AP or softAP */
	unsigned char		channel;
	enum rtw_security		security_type;   /**< the security type of connected AP or softAP */
	unsigned char 		password[RTW_MAX_PSK_LEN];   /**< the password of connected AP or softAP */
	unsigned char		key_idx;
	unsigned char		iw_mode;	/**< RTK_IW_MODE */
	unsigned char		alg;		/**< encryption algorithm */
	unsigned int		auth_type;
	unsigned char		is_wps_trigger;	/**< connection triggered by WPS process**/
} rtw_wifi_setting_t;

extern struct _rtw_wifi_setting_t wifi_setting[2];

extern  struct wifi_user_conf wifi_user_config;
/**
  * @brief  The structure is power limit regu map.
  */
typedef struct _pwr_lmt_regu_remap {
	unsigned char	domain_code;
	unsigned char	PwrLmtRegu;
} pwr_lmt_regu_remap;

/**
  * @brief  The structure is used to describe the sw statistics
  */
typedef struct _rtw_sw_statistics_t { /* software statistics for tx and rx*/
	unsigned int    max_skbbuf_used_number; /*!< max skb buffer used number       */
	unsigned int    skbbuf_used_number;     /*!< current used skbbuf number       */
	unsigned int    max_skbdata_used_number;/*!< max skb data used number       */
	unsigned int    skbdata_used_number;    /*!< current used skbdata number       */
} rtw_sw_statistics_t;

/**
  * @brief  The structure is used to describe the raw data description
  */
struct raw_frame_desc_t {
	unsigned char wlan_idx;      /**< index of wlan interface which will transmit */
	unsigned char *buf;          /**< poninter of buf where raw data is stored*/
	unsigned short buf_len;      /**< the length of raw data*/
	enum mgn_rate_type tx_rate;
	unsigned char retry_limit;
	unsigned char ac_queue;		/**< 0/3 for BE, 1/2 for BK, 4/5 for VI, 6/7 for VO*/
	unsigned char sgi;		/* 1 for enable data short */
	unsigned char agg_en;
} ;

/**
  * @brief  The structure is used to set WIFI packet filter pattern.
  */
typedef struct _rtw_packet_filter_pattern_t {
	unsigned short	offset;     ///< Offset in bytes to start filtering (referenced to the start of the ethernet packet)
	unsigned short	mask_size;  ///< Size of the mask in bytes
	unsigned char	*mask;       ///< Pattern mask bytes to be ANDed with the pattern eg. "\xff00" (must be in network byte order)
	unsigned char	*pattern;    ///< Pattern bytes used to filter eg. "\x0800"  (must be in network byte order)
} rtw_packet_filter_pattern_t;

/**
  * @brief  The structure is used to describe the 802.11 frame info
  */
typedef struct _ieee80211_frame_info_t {
	unsigned short i_fc;
	unsigned short i_dur;
	unsigned char i_addr1[6];
	unsigned char i_addr2[6];
	unsigned char i_addr3[6];
	unsigned short i_seq;
	unsigned char bssid[6];
	unsigned char encrypt;
	signed char rssi;
} ieee80211_frame_info_t;

/**
  * @brief  The structure is used to describe the packet filter info
  */
typedef struct _rtw_packet_filter_info_t {
	char filter_id;
	struct _rtw_packet_filter_pattern_t patt;
	enum rtw_packet_filter_rule rule;
	unsigned char enable;
} rtw_packet_filter_info_t;

/**
  * @brief  The structure is used to describe the mac filter list
  */
typedef struct _rtw_mac_filter_list_t {
	struct list_head node;
	unsigned char mac_addr[6];
} rtw_mac_filter_list_t;

/**
  * @brief  The structure is used to describe the cfg parameters used for csi report,
  * @note  The mac_addr if not specified, the default value must be 0.
  */
typedef struct _rtw_csi_action_parm_t {
	enum rtw_csi_group_type group_num;
	enum rtw_csi_accuracy_type accuracy;
	enum rtw_csi_alg_opt_type alg_opt;
	enum rtw_csi_ch_opt_type ch_opt;
	enum rtw_csi_op_role csi_role; /* indicate csi operation role */
	enum rtw_csi_mode_type mode;
	enum rtw_csi_action_type act;
	unsigned char enable;
	unsigned char trig_period;
	unsigned char data_rate;
	unsigned char data_bw;
	unsigned char mac_addr[6];
	unsigned char multi_type;     /* 0-uc csi triggering frame; 1-bc csi triggering frame */
	unsigned char trig_flag;      /* indicate source of device for triggering csi[sta<->sta]: 4bits >> 1 ~ 15 */
} rtw_csi_action_parm_t;

/**
  * @brief  The structure is used to describe the cfg parameters used for channel switch announcement,
  */
typedef struct _rtw_csa_parm_t {
	unsigned char new_chl;
	unsigned char chl_switch_cnt;
	unsigned char action_type;	/* 0: unicast csa action, 1: broadcast csa action, other values: disable transmit csa action */
	unsigned char bc_action_cnt; /* indicate the number of broadcast csa actions to send for each beacon interval. only valid when action_type = 1*/
	ap_channel_switch_callback_t callback;
} rtw_csa_parm_t;

//----------------------------
/* ie format
 * +-----------+--------+-----------------------+
 * |element ID | length | content in length byte|
 * +-----------+--------+-----------------------+
 *
 * type: refer to CUSTOM_IE_TYPE
 */
#ifndef _CUS_IE_
#define _CUS_IE_
/**
 * @brief  The structure is used to set WIFI custom ie list,
 * 	and type match CUSTOM_IE_TYPE.
 * 	The ie will be transmitted according to the type.
 */
typedef struct custom_ie {
	__u8 *ie;
	__u8 type;
} rtw_custom_ie_t, *p_rtw_custom_ie_t;
#endif /* _CUS_IE_ */

/**
 * @brief  The structure is used to describe channel plan and country code
 */
struct country_code_table_t {
	char char2[2]; /* country code */
	u8 channel_plan; /* channel plan code */
	u8 pwr_lmt; /* tx power limit index */
};

#ifndef CONFIG_FULLMAC

/**
 * @brief  The structure is join block param.
 */
typedef struct internal_join_block_param {
	void				*join_sema;
	unsigned int		join_timeout;
	unsigned char		block;
} internal_join_block_param_t;

/**
  * @brief  The structure is used to describe the rtw_client_list.
  */
typedef struct _rtw_client_list_t {
	unsigned int    count;         /**< Number of associated clients in the list    */
	struct _rtw_mac_t mac_list[AP_STA_NUM];   /**< max length array of MAC addresses */
	signed char rssi_list[AP_STA_NUM];   /**< max length array of client rssi */
	unsigned char macid_list[AP_STA_NUM];   /**< max length array of client macid */
} rtw_client_list_t;

/**
 * @brief  The structure is used to describe net device
 */
struct net_device {
	void			*priv;		/* pointer to private data */
	unsigned char		dev_addr[6];	/* set during bootup */
};

/**
 * @brief  The structure is used to describe wlan info
 */
typedef struct _Rltk_wlan_t {
	struct net_device	dev;		/* Binding wlan driver netdev */
	void			*skb;		/* pending Rx packet */
	unsigned int		tx_busy;
	unsigned int		rx_busy;
	unsigned char		enable;
	rtos_sema_t			netif_rx_sema;	/* prevent race condition on .skb in rltk_netif_rx() */
} Rltk_wlan_t;

extern struct _Rltk_wlan_t rltk_wlan_info[NET_IF_NUM];

#define netdev_priv(dev)		dev->priv
#define rtw_is_netdev_enable(idx)	(rltk_wlan_info[idx].enable)
#define rtw_get_netdev(idx)		(&(rltk_wlan_info[idx].dev))

#endif

/**
  * @}
  */

/**
* @}
*/

/**
* @}
*/

/** @defgroup WIFI_Exported_Functions WIFI Exported Functions
  * @{
  */



/** @defgroup Basic_Functions
  * @{
  */

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * @brief  Enable Wi-Fi.
 * - Bring the Wireless interface "Up".
 * @param[in]  mode: Decide to enable WiFi in which mode.
 * 	The optional modes are RTW_MODE_STA, RTW_MODE_AP,
 * 	RTW_MODE_STA_AP, RTW_MODE_PROMISC.
 * @return  RTW_SUCCESS: if the WiFi chip initialized successfully.
 * @return  RTW_ERROR: if the WiFi chip initialization failed.
 */
int wifi_on(enum rtw_mode_type mode);

/**
 * @brief  Check if the specified wlan interface  is running.
 * @param[in]  wlan_idx: can be set as STA_WLAN_INDEX or SOFTAP_WLAN_INDEX.
 * @return  If the function succeeds, the return value is 1.
 * 	Otherwise, return 0.
 * @note  For STA mode, use STA_WLAN_INDEX
 * 	For AP mode, use SOFTAP_WLAN_INDEX
 */
int wifi_is_running(unsigned char wlan_idx);

/**
 * @brief  Join a Wi-Fi network.
 * 	Scan for, associate and authenticate with a Wi-Fi network.
 * @param[in]  connect_param: the pointer of a struct which store the connection
 * 	info, including ssid, bssid, password, etc, for details, please refer to struct
 * 	rtw_network_info_t in wifi_conf.h
 * @param[in]  block: if block is set to 1, it means synchronized wifi connect, and this
* 	API will return until connect is finished; if block is set to 0, it means asynchronized
* 	wifi connect, and this API will return immediately.
 * @return  RTW_SUCCESS: when the system is joined for synchronized wifi connect, when connect
* 	cmd is set successfully for asynchronized wifi connect.
 * @return  RTW_ERROR: if an error occurred.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function.
 * 	(@ref wifi_on())
 * @note  if bssid in connect_param is set, then bssid will be used for connect, otherwise ssid
 * 	is used for connect.
 */
int wifi_connect(rtw_network_info_t *connect_param, unsigned char block);

/**
 * @brief  Disassociates from current Wi-Fi network.
 * @param  None
 * @return  RTW_SUCCESS: On successful disassociation from the AP.
 * @return  RTW_ERROR: If an error occurred.
 */
int wifi_disconnect(void);

/**
 * @brief  get join status during wifi connectection
 * @param  None
 * @return join status, refer to macros in wifi_conf.c
 */
enum rtw_join_status_type wifi_get_join_status(void);

/**
 * @brief  Initiate a scan to search for 802.11 networks.
  * Synchronized scan and asynchronized scan can be confgiured by the input param block.
  * For asynchronized scan, there are two different ways about how the scan result will be reported.
  * The first way is that when scan is done ,the total number of scanned APs will be reported through
  * scan_user_callback, and the detailed scanned AP infos can be get by calling wifi_get_scan_records,
  * so in this way, scan_user_callback need to be registered in scan_param.
  * The second way is that every time a AP is scanned, this AP info will be directly reported by
  * scan_report_each_mode_user_callback, and when scan is done, scan_report_each_mode_user_callback will
  * report a NULL pointer for notification. So in this way, scan_report_each_mode_user_callback need to
  * be registered in scan_param, and RTW_SCAN_REPORT_EACH need to be set in scan_param->options.Also in
  * this mode, scan_user_callback is no need to be registered.
 * @param[in]  scan_param: refer to struct rtw_scan_param_t in wifi_conf.h.
 * @param[in]  block: If set to 1, it's synchronized scan and this API will return
 * 	after scan is done. If set to 0, it's asynchronized scan and this API will return
 * 	immediately.
 * @return  RTW_SUCCESS or RTW_ERROR for asynchronized scan, return RTW_ERROR or
 * 	scanned AP number for Synchronized scan.
 * @note  If this API is called, the scanned APs are stored in WiFi driver dynamic
 * 	allocated memory, for synchronized scan or asynchronized scan which not use RTW_SCAN_REPORT_EACH,
 * 	these memory will be freed when wifi_get_scan_records is called.
 */
int wifi_scan_networks(rtw_scan_param_t *scan_param, unsigned char block);

/**
 * @brief  Trigger Wi-Fi driver to start an infrastructure Wi-Fi network.
 * @param[in]  softAP_config:the pointer of a struct which store the softAP
 * 	configuration, please refer to struct rtw_softap_info_t in wifi_conf.h
 * @warning  If a STA interface is active when this function is called,
 * 	the softAP will start on the same channel as the STA.
 * 	It will NOT use the channel provided!
 * @return  RTW_SUCCESS: If successfully creates an AP.
 * @return  RTW_ERROR: If an error occurred.
 * @note  if hidden_ssid in softAP_config is set to 1, then this softAP will start
 * 	with hidden ssid.
 * @note  Please make sure the Wi-Fi is enabled before invoking this function.
 * 	(@ref wifi_on())
 */
int wifi_start_ap(rtw_softap_info_t *softAP_config);

/**
 * @brief  Disable Wi-Fi interface-2.
 * @param  None
 * @return  RTW_SUCCESS: deinit success,
 * 	wifi mode is changed to RTW_MODE_STA.
 * @return  RTW_ERROR: otherwise.
 */
int wifi_stop_ap(void);
/**
 * @brief  Enable Wi-Fi interface-2.
 * @param  None
 * @return  RTW_SUCCESS: success,
 * 	wifi open RTW_MODE_AP .
 * @return  RTW_ERROR: otherwise.
 */
int _wifi_on_ap(void);
/**
 * @brief  Disable Wi-Fi interface-2.
 * @param  None
 * @return  RTW_SUCCESS: close ap mode,
 * @return  RTW_ERROR: otherwise.
 */
int _wifi_off_ap(void);
/**
* @}
*/

/** @} */

/** @} */

/** @defgroup WIFI_USER_CONFIG
*  @brief	WIFI_USER_CONFIG module
*  @{*/

/**
  * @brief  The structure is used to describe the wifi user configuration, can be configured in ameba_wificfg.c
  */
struct wifi_user_conf {
	/*!	This effects EDCCA threshold, wifi won't TX if detected energy exceeds threshold\n
		RTW_EDCCA_NORM: Adjust EDCCA threshold dynamically;\n
		RTW_EDCCA_ADAPT: For ESTI or SRRC;\n
		RTW_EDCCA_CS: For japan;\n
		RTW_EDCCA_DISABLE: Ingore EDCCA */
	unsigned char rtw_edcca_mode;

	/*!	For power limit, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf\n
		0: disable, 1: enable, 2: Depend on efuse(flash) */
	unsigned char rtw_tx_pwr_lmt_enable;

	/*!	For power by rate, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf\n
		0: disable, 1: enable, 2: Depend on efuse(flash) */
	unsigned char rtw_tx_pwr_by_rate;

	/*!	Enabled during TRP TIS certification */
	unsigned char rtw_trp_tis_cert_en;

	/*!	Force wpa mode\n
		WPA_AUTO_MODE: auto mode, follow AP;\n
		WPA_ONLY_MODE: wpa only;\n
		WPA2_ONLY_MODE: wpa2 only;\n
		WPA3_ONLY_MODE: wpa3 only;\n
		WPA_WPA2_MIXED_MODE: wpa and wpa2 mixed;\n
		WPA2_WPA3_MIXED_MODE: wpa2 and wpa3 mixed*/
	unsigned char wifi_wpa_mode_force;

	/*!	TDMA DIG affects the range of RX, when enabled, both near and distant devices can be received\n
		0:tdma dig off, 1:tdma dig on */
	unsigned char tdma_dig_enable;

	/*!	Antdiv mode\n
		RTW_ANTDIV_AUTO: auto antdiv;\n
		RTW_ANTDIV_FIX_MAIN: fix main ant;\n
		RTW_ANTDIV_FIX_AUX: fix aux ant;\n
		RTW_ANTDIV_DISABLE: antdiv disable */
	unsigned char antdiv_mode;

	/*!	The maximum number of STAs connected to the softap should not exceed NUM_STA */
	unsigned char g_user_ap_sta_num;

	/*!	IPS(Inactive power save), If disconnected for more than 2 seconds, WIFI will be powered off*/
	unsigned char ips_enable;

	/*!	Power save status\n
		IPS_WIFI_OFF: The WIFI is powered off during the IPS;\n
		IPS_WIFI_PG: The WIFI enters the PG state during the IPS, and therefore it exits the IPS faster. Only dplus support this status */
	unsigned char ips_level;

	/*!	The driver does not enter the IPS due to 2s disconnection. Instead, API wifi_set_ips_internal controls the IPS\n
		0: control ips enter/leave, 1: wifi_set_ips_internal control ips enable/disable */
	unsigned char ips_ctrl_by_usr;

	/*!	LPS(leisure power save), After connection, with low traffic, part of WIFI can be powered off and woken up upon packet interaction\n
		0: disable power save when wifi connected, 1: enable */
	unsigned char lps_enable;

	/*!	Power management mode\n
		PS_MODE_LEGACY: During TBTT, wake up to receive beacon; otherwise, WIFI remains in power-save mode;\n
		PS_MODE_UAPSD_WMM: not support right now */
	unsigned char lps_mode;

	/*!	In LPS, the sta wakes up every legacy_ps_listen_interval* 102.4ms to receive beacon*/
	unsigned char legacy_ps_listen_interval;

	/*!	0: NO_LIMIT, 1: TWO_MSDU, 2: FOUR_MSDU, 3: SIX_MSDU */
	unsigned char uapsd_max_sp_len;

	/*!	BIT0: AC_VO UAPSD, BIT1: AC_VI UAPSD, BIT2: AC_BK UAPSD, BIT3: AC_BE UAPSD */
	unsigned char uapsd_ac_enable;

	/*!	0: Disable ampdu rx, 1: Enable ampdu rx */
	unsigned char ampdu_rx_enable;

	/*!	0: Disable ampdu tx, 1: Enable ampdu tx */
	unsigned char ampdu_tx_enable;

	/*!	0: If the pkt's destination address doesn't match, it won't be dropped, 1: If the pkt's destination address doesn't match, it will be dropped. */
	unsigned char bCheckDestAddress;

	/*!	The ap_compatibilty_enabled is used to configure the wlan settings, each bit controls one aspect\n
		bit 0: (0: follow 802.11 spec, do not issue deauth, 1(default): issue deauth in 1st REAUTH_TO to be compatible with ap);\n
		bit 1: (0: do not check beacon info to connect with AP with multiple SSID, 1(default): check beacon info);\n
		bit 2: (0(default): do not issue deauth at start of auth, 1: issue deauth at start of auth);\n
		bit 3: (0: do not switch WEP auth algo unless WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG, 1(default): switch WEP auth algo from shared key to open system in 1st REAUTH_TO);\n
		other bits: reserved */
	unsigned char ap_compatibilty_enabled;

	/*!	0: API wifi_set_channel does not trigger RFK;\n
		1: API wifi_set_channel triggers RFK */
	unsigned char set_channel_api_do_rfk;

	/*!	RF calibration is triggered during WIFI initialization and channel switch to calibrate TRX to optimal performance,\n
		but it takes a long time (around 100ms), so some applications can sacrifice performance so that WIFI initializes and switches faster.\n
		Bit0: DIS_DPK;\n
		Other bits: reserved*/
	u16 rf_calibration_disable;

	/*! The maximum number of roaming attempts triggered by BTM*/
	unsigned char max_roaming_times;

	/*!	AP periodically sends null pkt to check whether the STA is offline, not support right now*/
	unsigned char ap_polling_sta;

	/*!	Refer to ameba_wifi_country_code_table_usrcfg.c, e.g. China: country_code[0] = 'C', country_code[1] = 'N'.\n
		Specical value: country_code[0]~[1] = 0x0000: follow efuse; country_code[0]='0', country_code[1]='0' : WORLDWIDE */
	s8 country_code[2];

	/*!	Bandwidth 40MHz, some IC hardware does not support*/
	unsigned char bw_40_enable;

	/*!	Refe to 802.11d spec, obtain the country code information from beacon, and set the pwr limit and channel plan*/
	unsigned char rtw_802_11d_en;

	/*!	When disconnection, STA automatically reconnects, and auto_reconnect_count is the number of attempts*/
	unsigned char auto_reconnect_count;

	/*!	auto_reconnect_interval is Automatic reconnection interval, unit s*/
	unsigned char auto_reconnect_interval;

	/*!	skb_num_np is wifi driver's trx buffer number, each buffer occupies about 1.8K bytes of heap, a little difference between different chips.\n
		These buffer are used for all traffics except tx data in INIC mode, and all traffics in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_np can be increased.\n
		Minimum: 7 (3 for Ameba lite). Default: 10*/
	int skb_num_np;

	/*!	These buffer are used for tx data packtes in INIC mode, not used in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_ap can be increased */
	int skb_num_ap;

	/*!	Every data tx is forced to start with cts2self */
	unsigned char force_cts2self;

	/*!	Multi Channel Concurrent mode, STA and Softap can work on different channels via TDMA, not support right now*/
	unsigned char en_mcc;

	unsigned char tx_shortcut_enable;

	unsigned char rx_shortcut_enable;

	/* for Concurrent Mode */
	/*!	0: STA or SoftAP only at any time, The MAC address of TA or Softap is the MAC address of chip;\n
		1: STA and SoftAP may run at the same time, Softap's mac address depends on softap_addr_offset_idx */
	unsigned char concurrent_enabled;

	/*!	It is valid only when concurrent_enabled =1. The range is 1~5. The lowest bit of mac[0] is 1, which represents the multicast address, so skip mac[0].\n
		e.g. softap_addr_offset_idx = 1, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e1:4c:01:02:03;\n
		e.g. softap_addr_offset_idx = 5, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e1:4c:01:02:04*/
	unsigned char softap_addr_offset_idx;

	/*!	The number of ampdu that Recipient claims to Originator for RX, it can be any value less than 64.\n
		skb_num_np needs to be adjusted simultaneously*/
	unsigned char rx_ampdu_num;

	/*!	Linux Fullmac architecture, ignore in RTOS*/
	unsigned char cfg80211;

	/*!	WPS */
	unsigned char wps_retry_count;

	/*!	in ms */
	unsigned int wps_retry_interval;

	/*!	For wifi speaker configuration\n
		BIT0: main switch, BIT1: enable tsf interrupt, BIT2: enable audio tsf */
	unsigned char wifi_speaker_feature;

	/*STA mode will periodically send null packet to AP to keepalive, unit: second */
	unsigned char keepalive_interval;
};
/** @} */
#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_H
