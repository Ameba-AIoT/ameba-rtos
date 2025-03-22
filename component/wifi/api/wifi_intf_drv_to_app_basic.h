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

#ifndef CONFIG_FULLMAC
#include "rtw_wifi_common.h"
#if !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))
#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_intf_drv_to_bt.h"
#include "dlist.h"
#include <rtw_skbuff.h>
#endif
#endif

#include <wifi_ind.h>
#include "wifi_intf_drv_to_app_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @addtogroup WIFI_Exported_Types Wi-Fi Exported Types
* @{
*/
/**********************************************************************************************
 *                                  common structures
 *********************************************************************************************/
/** @addtogroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */

#pragma pack(1)/*_rtw_ssid_t and _rtw_mac_t are 1 byte alignment for some issues long long ago*/
/**
  * @brief  The structure is used to describe the SSID (Service Set Identification), i.e., the name of Access Point.
  */
struct _rtw_ssid_t {
	unsigned char
	len;     /**< SSID length, i.e., equal to the length of `val`. The length of ssid should not >= @ref WHC_MAX_SSID_LENGTH.  */
	unsigned char		val[WHC_MAX_SSID_LENGTH]; /**< SSID name (AP name).*/
};

/**
  * @brief  The structure is used to describe the unique 6-byte MAC address.
  */
struct _rtw_mac_t {
	unsigned char		octet[6]; /**< Unique 6-byte MAC address. */
};
#pragma pack()

/**********************************************************************************************
 *                                   scan structures
 *********************************************************************************************/

#pragma pack(1)/*scan related structs are 1 byte alignment for some issues long long ago*/
/**
  * @brief  The structure is used to describe the busyness of a channel for ACS(Automatic Channel Selection).
  */
struct acs_mntr_rpt {
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
	struct _rtw_ssid_t       SSID;             /**< Service Set Identification (i.e. Name of Access Point). */
	struct _rtw_mac_t        BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point). */
	signed short             signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent. */
	u8
	bss_type;         /**< The bss type. The noraml type is infrastructure BSS. Val: RTW_BSS_TYPE_INFRASTRUCTURE, RTW_BSS_TYPE_WTN_HELPER.*/
	u32                      security;         /**< The security type of this AP. Val: RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK...*/
	u8
	wps_type;         /**< The WPS(Wi-Fi Protected Setup) types supported by this AP. Val: RTW_WPS_TYPE_DEFAULT, RTW_WPS_TYPE_USER_SPECIFIED...*/
	unsigned int             channel;          /**< Radio channel that the AP beacon was received on. */
	u8                       band;             /**< The frequency ranges used by this AP. Val: RTW_802_11_BAND_5GHZ, RTW_802_11_BAND_2_4GHZ. */

	/** The wireless spectrum management regulations of which region followed by the AP. `country_code` is coded
	 * according to ISO 3166 standard. Specific values can refer to ameba_wifi_country_code_table_usrcfg.c.\n
	 * e.g. China: country_code[0] = 'C', country_code[1] = 'N'. */
	char                     country_code[2];
	char                     wireless_mode;    /**< The wireless mode of this AP. Val: WLAN_MD_11B, WLAN_MD_11A...*/
	u8                       rom_rsvd[3];
};

/**
  * @brief  The structure is used to describe the scan time per channel.
  */
struct _rtw_channel_scan_time_t {
	unsigned short		active_scan_time;      /**< Active scan time per channel, units: millisecond, default is 110ms. */
	unsigned short		passive_scan_time;     /**< Passive scan time per channel, units: millisecond, default is 110ms. */
};

/**
  * @brief  The structure is used to describe the scan parameters used for scan.
  */
struct _rtw_scan_param_t {
	u8                                 options; /**< The scan option, such as active scan. Val: RTW_SCAN_ACTIVE, RTW_SCAN_PASSIVE...*/
	char                              *ssid;    /**< The data length of string pointed by ssid should not exceed @ref WHC_MAX_SSID_LENGTH. */
	unsigned char                     *channel_list;      /**< The list of specified channels to be scanned.*/
	unsigned char                      channel_list_num;  /**< The total number in `channel_list`.*/
	struct _rtw_channel_scan_time_t    chan_scan_time;    /**< The scan time per channel.*/

	/** Config the max number of recorded AP. When set to 0, use default value 64.
	 * When the number of scanned APs exceed `max_ap_record_num`, the AP(s) with smallest rssi will be discarded. */
	unsigned short                     max_ap_record_num;
	void                              *scan_user_data;
	int (*scan_user_callback)(unsigned int ap_num, void *user_data);/**< Used for normal asynchronized mode. */
	int (*scan_report_each_mode_user_callback)(struct rtw_scan_result *scanned_ap_info, void *user_data); /**< Used for RTW_SCAN_REPORT_EACH mode. */
	int (*scan_report_acs_user_callback)(struct acs_mntr_rpt *acs_mntr_rpt); /**< Used for report acs info.*/
};
#pragma pack()

/**********************************************************************************************
 *                                     connect structures
 *********************************************************************************************/
/**
  * @brief  The structure is used for Linux host to get wpa_supplicant's info for STA connect,
  *         which RTOS not need.
  */
struct _rtw_wpa_supp_connect_t {
	u8 rsnxe_ie[RSNXE_MAX_LEN];  /**< The RSNXE IE in beacon of AP which STA is trying to connect.*/
};

/**
  * @brief	The structure is used to describe the connection setting about SSID,
  * 		security type and password,etc., used when trying to connect an AP.
  * @note
  *        1. If this struct is used for wifi connect, the channel is used to config
  * 	      whether it is a full channel scan(when channel is set to 0), or it will
  * 	      only scan one channel(do active scan on the configured channel).
  *        2. `pscan_option` set to @ref PSCAN_FAST_SURVEY means do fast survey on the specified channel
  * 	      set to 0 means do normal scan on the specified channel or full channel.
  */
struct _rtw_network_info_t {
	struct _rtw_ssid_t					ssid;  /**< The AP's name and the length of name (should not exceed @ref WHC_MAX_SSID_LENGTH). */
	struct _rtw_mac_t					bssid; /**< The unique 6-byte MAC address of AP. */
	u32							security_type; /**< The security type of AP which sta is trying to connect. Val: RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK...*/
	unsigned char				*password;	   /**< The password of AP which sta is trying to connect. */
	int 						password_len;  /**< The data length of string pointed by password should not exceed RTW_MAX_PSK_LEN. Equal to length of `password`. */
	int 						key_id;		   /**< Should be set when `security_type` is WEP_ENABLED (i.e. RTW_SECURITY_WEP_PSK or RTW_SECURITY_WEP_SHARED), other security types will ignore. Val: 0~3.*/
	unsigned char				channel;		/**< Set to 0 means full channel scan, set to other value means only scan on the specified channel. */
	unsigned char
	pscan_option;	/**< Used when the specified channel is set. Set to @ref PSCAN_FAST_SURVEY for fast survey, set to other value is invalid and will be ignored. */
	unsigned char 				is_wps_trigger;	/**< Connection triggered by WPS process.*/
	struct _rtw_wpa_supp_connect_t	wpa_supp;   /**< Only used by Linux host to specific some details required for STA connect, which RTOS do not use. */
	struct _rtw_mac_t		prev_bssid;
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
struct _rtw_wifi_setting_t {
	u8					mode;   /**< The mode of current wlan interface, val: RTW_MODE_STA, RTW_MODE_AP, RTW_MODE_NAN. */
	unsigned char 		ssid[33];   /**< The ssid of connected AP or softAP. */
	unsigned char		bssid[6];   /**< The bssid of connected AP or softAP. */
	unsigned char		channel;
	u32					security_type; /**< The security type of connected AP or softAP, val: RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK...*/
	unsigned char 		password[RTW_MAX_PSK_LEN];   /**< The password of connected AP or softAP. */
	unsigned char		key_idx;
	unsigned char		alg;		/**< Encryption algorithm. */
	unsigned int		auth_type;
	unsigned char		is_wps_trigger;	/**< Connection triggered by WPS process.*/
	unsigned int		rom_rsvd;
};

/**
  * @brief  The structure is used to describe the software statistics for tx and rx.
  */
struct _rtw_sw_statistics_t { /* software statistics for tx and rx*/
	unsigned int    max_skbbuf_used_number; /**< Max skb buffer used number. */
	unsigned int    skbbuf_used_number;     /**< Current used skbbuf number. */
};

/**
  * @brief  The structure is used to describe the phy statistics.
  */
struct _rtw_phy_statistics_t {
	signed char	rssi;          /**< Average mixed rssi in 1 sec (for STA mode). */
	signed char	data_rssi;          /**< Average data rssi in 1 sec (for STA mode). */
	signed char	beacon_rssi;          /**< Average beacon rssi in 1 sec (for STA mode). */
	signed char	snr;          /**< Average snr in 1 sec (not include cck rate, for STA mode).*/
	unsigned char	cur_rx_data_rate; /**< Current rx data rate (for STA mode), val: MGN_1M, MGN_2M...*/
	unsigned char	cur_tx_data_rate; /**< Current tx data rate (for STA mode), val: MGN_1M, MGN_2M... */
	unsigned int
	cca_clm; /**< Channel loading measurement ratio by cca (the ratio of CCA = 1 in number of samples). driver do clm every 2 seconds, the value is the lastest result. */
	unsigned int	edcca_clm; /**< Channel loading measurement ratio by edcca (the ratio of EDCCA = 1 in number of samples). The value is also the lastest result. */
	unsigned int	clm_channel; /**< Channel corresponding to the latest clm result.*/
	unsigned int	tx_retry;
	unsigned short	tx_drop;
	unsigned int	rx_drop;
};

/**********************************************************************************************
 *                                     softap structures
 *********************************************************************************************/
/**
  * @brief  The structure is used to describe the setting about SSID,
  *			security type, password and default channel, used to start AP mode.
  * @note  The data length of string pointed by ssid should not exceed @ref WHC_MAX_SSID_LENGTH,
  *        and the data length of string pointed by password should not exceed RTW_MAX_PSK_LEN.
  */
struct _rtw_softap_info_t {
	struct _rtw_ssid_t		ssid;
	unsigned char		hidden_ssid;
	u32					security_type; /**< Val: RTW_SECURITY_OPEN, RTW_SECURITY_WEP_PSK...*/
	unsigned char 		*password;
	unsigned char 		password_len;
	unsigned char		channel;
};

#ifndef CONFIG_FULLMAC
/**
  * @brief  The structure is used to describe the associated clients of SoftAP.
  */
struct _rtw_client_list_t {
	unsigned int    count;         /**< Number of associated clients in the list.    */
	struct _rtw_mac_t mac_list[MACID_HW_MAX_NUM - 2]; /**< Max length array of MAC addresses. */
	signed char rssi_list[MACID_HW_MAX_NUM - 2]; /**< Max length array of client rssi. */
	unsigned char macid_list[MACID_HW_MAX_NUM - 2]; /**< Max length array of client macid. */
};
#endif

/**
  * @brief  The structure is used to describe the cfg parameters used for channel switch announcement.
  */
struct _rtw_csa_parm_t {
	unsigned char new_chl; /**< The new channel will be switched to. */
	unsigned char chl_switch_cnt; /**< The channel switch cnt, after chl_switch_cnt*102ms, ap will switch to new channel. */
	unsigned char action_type;	/**< 0: unicast csa action, 1: broadcast csa action, other values: disable transmit csa action. */
	unsigned char bc_action_cnt; /**< Indicate the number of broadcast csa actions to send for each beacon interval. only valid when action_type = 1.*/
	/** @brief User handle when softap switch channel by csa function. This callback will be called after channel switch is
	 *         done, and will return the new channel number and channel switch result.
	  * @param[in] channel:  New channel.
	  * @param[in] ret: Val: @ref RTW_ERROR, @ref RTW_SUCCESS.
	  */
	void (*callback)(unsigned char channel, s8 ret);
};

/**********************************************************************************************
 *                                     promisc structures
 *********************************************************************************************/
/**
*@brief The structure is used to describe the rx packet's detail information.
*/
struct rx_pkt_info {
	s8 recv_signal_power;
	u8 data_rate; /**< Val: MGN_1M, MGN_2M...*/
	u8 channel;
	u8 *buf;
	u32 len;
};

/**
*@brief The structure is used to describe the selected promisc mode and callback function.
*/
struct _promisc_para_t {
	/** @brief Receive all packets in the air or set some filtering conditions.
		- @ref RCR_ALL_PKT : Receive all packets in the air.
		- @ref RCR_AP_ALL : Receive all packtets send by connected AP.
		*/
	u8 filter_mode;
	/** @brief User handle a packet.
	  * @param[in] pkt_info:  The pointer of a struct rx_pkt_info which store the packet's detail information.
	  * @return Should the driver continue processing this packet after user has processed.
	  * 	- @ref NEED_DRIVER_HANDLE : Driver continue processing this packet, This setting is usually required when the STA remains connected.
	  *     - @ref BYPASS_DRIVER_HANDLE : Driver drop this packet, This setting is usually used when STA does not need connect.*/
	u8(*callback)(struct rx_pkt_info *pkt_info);
};

/**********************************************************************************************
 *                                     wpa_lite structures
 *********************************************************************************************/
/**
 * @brief  The structure is pmksa ops.
 */
struct rtw_pmksa_ops_t {
	u8 ops_id;
	u8 wlan_idx;
	u8 pmkid[16];
	u8 mac_addr[6];
	u8 pmk[32]; /**< Pmksa is maintained in NP when use wpa_lite.*/
};

/**
 * @brief  The structure is crypt info.
 */
struct rtw_crypt_info {
	u8 pairwise;            /**<  Indicate pairwise(1) key or group key(0). */
	u8 mac_addr[6];
	u8 wlan_idx;
	u16 key_len;
	u8 key[32];
	u8 key_idx;
	u8 driver_cipher;
	u8 transition_disable_exist;
	u8 transition_disable_bitmap;
	u8 device_id : 5;       /**<  tx_raw: Flag for no linked peer, and will be converted to camid when force_cam_entry = 1. */
	u8 force_cam_entry : 1; /**<  tx_raw must set force_cam_entry = 1. Upper layer direct set specified cam entry, not dependent on 4 way or stainfo. */
	u8 rpt_mode : 1;
};

/**
 * @brief  The structure is status of wpa_4way.
 */
struct rtw_wpa_4way_status {
	u8 *mac_addr;             /**< Mac addr of 4-way interactive peer device. */
	u8 wlan_idx;              /**< Index of wlan interface. */
	u8 is_start : 1;          /**< Start(1) or stop(0) of 4way/2way exchange. */
	u8 is_grpkey_update : 1;  /**< Indicate first key change(0) or update grp_key change(1). */
	u8 is_success : 1;        /**< Result of 4way/2way exchange: 0-fail; 1-success. */
};

/**
 * @brief  The structure is used to store configuration of SAE protocol
 * which is used in secure auth process.
 */
struct wpa_sae_param_t {
	unsigned char 		peer_mac[6];
	unsigned char 		self_mac[6];
	u8					h2e;  /**< A flag indicating the use of Hash-to-Element (H2E) optimization in SAE. */
	u8					sae_reauth_limit;
};

/**
 * @brief  The structure is used to describe the DH(Diffie-Hellman) params of
 *         OWE(Opportunistic Wireless Encryption).
 * @note  Temporarily support group 19 with 256 bit public key, i.e., group = 19, pub_key_len = 32.
 */
struct rtw_owe_param_t {
	u16 group;                   /**< Specify the DH group number used for public key generation. */
	u8 pub_key[RTW_OWE_KEY_LEN]; /**< The public key for OWE. */
	u8 pub_key_len;              /**< The length of the public key.*/
	u8 peer_mac[6];              /**< The MAC address of the peer device.*/
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
	u8 ie[5 + MAX_WPA_IE_LEN + MAX_FT_IE_LEN]; /**<  1.NP->AP: rsnie; 2.AP->NP: mdie+rsnie+ftie*/
	u32 ielen;
#endif
#endif
};

/**********************************************************************************************
 *                                     speaker structures
 *********************************************************************************************/
union speaker_set {
	struct {
		u8 mode;              /**< 0 for slave, 1 for master. */
		u8 nav_thresh;        /**< Unit 128us. */
		u8 relay_en;          /**< Relay control. */
	} init; /**< For wifi speaker setting case SPEAKER_SET_INIT.*/
	struct {
		u8 port;           /**< 0 for select port 0's TSFT to trigger audio latch count, 1 for port 1. */
		u8 latch_period;      /**< 0 for trigger audio latch period is 4.096ms, 1 for 8.192ms. */
	} latch_i2s_count; /**< For wifi speaker setting case  SPEAKER_SET_LATCH_I2S_COUNT.*/
	struct {
		u8 enable;			/**< 1 for enable, 0 for disable. */
		u64 tsft;           /**< Unit us. */
		u8 port;           /**< 0 for select port 0's TSFT to trigger audio latch count, 1 for port 1. */
	} tsf_timer; /**< For wifi speaker setting case SPEAKER_SET_TSF_TIMER.*/
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
struct _rtw_csi_action_parm_t {
	unsigned char group_num;   /**< val: CSI_GROUP_NUM_1, CSI_GROUP_NUM_2... */
	unsigned char accuracy;    /**< val: CSI_ACCU_1BYTE, CSI_ACCU_2BYTE */
	unsigned char alg_opt;     /**< val: CSI_ALG_LS, CSI_ALG_SMOTHING */
	unsigned char ch_opt;      /**< val: CSI_CH_LEGACY, CSI_CH_NON_LEGACY */
	unsigned char csi_role;    /**< indicate csi operation role, val: CSI_OP_ROLE_TRX, CSI_OP_ROLE_TX, CSI_OP_ROLE_RX */
	unsigned char mode;        /**< val: CSI_MODE_NORMAL, CSI_MODE_NDP, CSI_MODE_RX_RESP*/
	unsigned char act;         /**< val: CSI_ACT_EN, CSI_ACT_CFG */
	unsigned short trig_frame_mgnt;	/**< indicate management frame subtype of rx csi triggering frame for fetching csi, val: CSI_TRIG_ASSOCREQ... */
	unsigned short trig_frame_ctrl;	/**< indicate control frame subtype of rx csi triggering frame for fetching csi, val: CSI_TRIG_TRIGGER... */
	unsigned short trig_frame_data;	/**< indicate data frame subtype of rx csi triggering frame for fetching csi, val: CSI_TRIG_DATA... */
	unsigned char enable;
	unsigned char trig_period;
	unsigned char data_rate;
	unsigned char data_bw;
	unsigned char mac_addr[6];
	unsigned char multi_type;  /**< 0-uc csi triggering frame; 1-bc csi triggering frame */

	/** indicate role for transmitting CSI triggering frame in METHOD4 and
	 * role for transmitting response ACK for CSI triggering frame in METHOD1_Variant,
	 * others are reserved. \n
	 * Value=1 ~ 15 (0 is reserved)*/
	unsigned char trig_flag;
};


/**********************************************************************************************
 *                                     other structures
 *********************************************************************************************/
#ifndef CONFIG_FULLMAC
/**
 * @brief  The structure is join block param.
 */
struct internal_join_block_param {
	void				*join_sema;
	unsigned int		join_timeout;
	unsigned char		block;
};

/**
 * @brief  The structure is used to describe net device.
 */
struct net_device {
	void			*priv;		/**<  Pointer to private data. */
	unsigned char		dev_addr[6];	/**<  Set during bootup. */
	u8 iface_type;
};

/**
 * @brief  The structure is used to describe wlan info.
 */
struct _Rltk_wlan_t {
	struct net_device	dev;		/**<  Binding wlan driver netdev. */
	void			*skb;		/**<  Pending Rx packet. */
	unsigned int		tx_busy;
	unsigned int		rx_busy;
	unsigned char		enable;
	rtos_sema_t			netif_rx_sema;	/**<  Prevent race condition on .skb in rltk_netif_rx(). */
};
#endif

/**
  * @brief  The structure is power limit regu map.
  */
struct _pwr_lmt_regu_remap {
	unsigned char	domain_code;
	unsigned char	PwrLmtRegu_2g;	/**< Not distinguish 2.4G and 5G; just set PwrLmtRegu_2g. */
	unsigned char	PwrLmtRegu_5g;
};

/**
  * @brief  The structure is used to describe the raw frame.
  */
struct raw_frame_desc_t {
	unsigned char wlan_idx;      /**< Index of wlan interface which will transmit. */
	unsigned char device_id;     /**< Index of peer device which as a rx role for receiving this pkt, and will be update when linked peer. */
	unsigned char *buf;          /**< Poninter of buf where raw data is stored.*/
	unsigned short buf_len;      /**< The length of raw data.*/
	unsigned char tx_rate;	     /**< Val: MGN_1M, MGN_2M...*/
	unsigned char retry_limit;
	unsigned char ac_queue;      /**< 0/3 for BE, 1/2 for BK, 4/5 for VI, 6/7 for VO. */
	unsigned char sgi : 1;       /**< 1 for enable data short. */
	unsigned char agg_en : 1;    /**< Aggregation of tx_raw frames. 1:enable; 0-disable. */
};

/**
  * @brief  Old version raw data description, only used for driver internal send mgnt frames.
  */
struct _raw_data_desc_t {
	unsigned char		wlan_idx;      /**< Index of wlan interface which will transmit. */
	unsigned char		*buf;          /**< Poninter of buf where raw data is stored.*/
	unsigned short		buf_len;      /**< The length of raw data.*/
	unsigned short		flags;        /**< Send options.*/
};

/**
 * @brief  The structure is used to set WIFI custom ie list,
 *
 */
struct custom_ie {
	/** Format:
	 * <table>
	 * <tr><th>1byte</th><th>1byte</th><th>length bytes</th></tr>
	 * <tr><td>element ID</td><td>length</td><td>content</td></tr>
	 * </table>
	 */
	u8 *ie;
	u8 type;    /**< Val: @ref PROBE_REQ, @ref PROBE_RSP...*/
};

/**
 * @brief  The structure is used to describe channel plan and country code.
 */
struct country_code_table_t {
	char char2[2];   /**< Country code. */
	u8 channel_plan; /**< Channel plan code. */
	u8 pwr_lmt;      /**< Tx power limit index. */
};

struct rtw_tx_power_ctl_info_t {
	s8	tx_pwr_force; /**< Currently user can specify tx power for all rate, unit 0.25dbm.*/
	u8	b_tx_pwr_force_enbale : 1; /**< 1 for enable, 0 for disable. */
};

/**********************************************************************************************
 *                                    user_configure struct
 *********************************************************************************************/
/**
  * @brief  The structure is used to describe the wifi user configuration, can be configured in ameba_wificfg.c.
  */
struct wifi_user_conf {
	/*!	This effects EDCCA threshold, wifi won't TX if detected energy exceeds threshold.
		- \b RTW_EDCCA_NORM: Adjust EDCCA threshold dynamically;
		- \b RTW_EDCCA_ADAPT: For ESTI or SRRC;
		- \b RTW_EDCCA_CS: For japan;
		- \b RTW_EDCCA_DISABLE: Ingore EDCCA. */
	unsigned char rtw_edcca_mode;

	/*!	For power limit, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	unsigned char rtw_tx_pwr_lmt_enable;

	/*!	For power by rate, see Ameba Wi-Fi TX power and Country code Setup Guideline.pdf.\n
		0: Disable, 1: Enable, 2: Depend on efuse(flash). */
	unsigned char rtw_tx_pwr_by_rate;

	/*!	Enabled during TRP TIS certification. */
	unsigned char rtw_trp_tis_cert_en;

	/*!	Force wpa mode:
		- \b WPA_AUTO_MODE: Auto mode, follow AP;
		- \b WPA_ONLY_MODE: Wpa only;
		- \b WPA2_ONLY_MODE: Wpa2 only;
		- \b WPA3_ONLY_MODE: Wpa3 only;
		- \b WPA_WPA2_MIXED_MODE: Wpa and wpa2 mixed;
		- \b WPA2_WPA3_MIXED_MODE: Wpa2 and wpa3 mixed.*/
	unsigned char wifi_wpa_mode_force;

	/*!	TDMA DIG affects the range of RX, when enabled, both near and distant devices can be received.\n
		0: TDMA DIG off, 1: TDMA DIG on. */
	unsigned char tdma_dig_enable;

	/*!	Antdiv mode:
		- \b RTW_ANTDIV_AUTO: Auto antdiv;
		- \b RTW_ANTDIV_FIX_MAIN: Fix main ant;
		- \b RTW_ANTDIV_FIX_AUX: Fix aux ant;
		- \b RTW_ANTDIV_DISABLE: Antdiv disable. */
	unsigned char antdiv_mode;

	/*!	The maximum number of STAs connected to the softap should not exceed the num specified in notes of func wifi_set_user_config(). */
	unsigned char ap_sta_num;

	/*!	IPS(Inactive power save), If disconnected for more than 2 seconds, WIFI will be powered off. */
	unsigned char ips_enable;

	/*!	Power save status:
		- \b IPS_WIFI_OFF: The WIFI is powered off during the IPS;
		- \b IPS_WIFI_PG: The WIFI enters the PG state during the IPS, and therefore it exits the IPS faster. Only dplus support this status. */
	unsigned char ips_level;

	/*!	The driver does not enter the IPS due to 2s disconnection. Instead, API wifi_set_ips_internal() controls the IPS.\n
		0: API wifi_set_ips_internal() control ips enable/disable, 1: Control ips enter/leave. */
	unsigned char ips_ctrl_by_usr;

	/*!	LPS(leisure power save), After connection, with low traffic, part of WIFI can be powered off and woken up upon packet interaction.\n
		0: Disable power save when wifi connected, 1: Enable. */
	unsigned char lps_enable;

	/*!	Power management mode:
		- \b PS_MODE_LEGACY: During TBTT, wake up to receive beacon; otherwise, WIFI remains in power-save mode;\n
		- \b PS_MODE_UAPSD_WMM: Not support right now. */
	unsigned char lps_mode;

	/*!	In LPS, the sta wakes up every legacy_ps_listen_interval* 102.4ms to receive beacon.*/
	unsigned char legacy_ps_listen_interval;

	/*!	0: NO_LIMIT, 1: TWO_MSDU, 2: FOUR_MSDU, 3: SIX_MSDU. */
	unsigned char uapsd_max_sp_len;

	/*!	BIT0: AC_VO UAPSD, BIT1: AC_VI UAPSD, BIT2: AC_BK UAPSD, BIT3: AC_BE UAPSD. */
	unsigned char uapsd_ac_enable;

	/*!	0: Disable ampdu rx, 1: Enable ampdu rx. */
	unsigned char ampdu_rx_enable;

	/*!	0: Disable ampdu tx, 1: Enable ampdu tx. */
	unsigned char ampdu_tx_enable;

	/*!	0: If the pkt's destination address doesn't match, it won't be dropped, 1: If the pkt's destination address doesn't match, it will be dropped. */
	unsigned char bCheckDestAddress;

	/*!	The ap_compatibilty_enabled is used to configure the wlan settings, each bit controls one aspect.
		- <b>bit 0:</b> 0: follow 802.11 spec, do not issue deauth, 1(default): issue deauth in 1st REAUTH_TO to be compatible with ap;
		- <b>bit 1:</b> 0: do not check beacon info to connect with AP with multiple SSID, 1(default): check beacon info;
		- <b>bit 2:</b> 0(default): do not issue deauth at start of auth, 1: issue deauth at start of auth;
		- <b>bit 3:</b> 0: do not switch WEP auth algo unless WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG, 1(default): switch WEP auth algo from shared key to open system in 1st REAUTH_TO;
		- <b>other bits:</b> reserved */
	unsigned char ap_compatibilty_enabled;

	/*!	0: API wifi_set_channel() does not trigger RFK;
		1: API wifi_set_channel() triggers RFK. */
	unsigned char set_channel_api_do_rfk;

	/*!	0: Do not limit the peak current of DPD;
		1: Limit the peak current of DPD. */
	unsigned char dpk_peak_limit;

	/*!	RF calibration is triggered during WIFI initialization and channel switch to calibrate TRX to optimal performance,\n
		but it takes a long time (around 100ms), so some applications can sacrifice performance so that WIFI initializes and switches faster.\n
		- \b Bit0: DIS_DPK;
		- <b>Other bits:</b> reserved.*/
	u16 rf_calibration_disable;

	/*! The maximum number of roaming attempts triggered by BTM.*/
	unsigned char max_roaming_times;

	/*!	AP periodically sends null pkt to check whether the STA is offline, not support right now.*/
	unsigned char ap_polling_sta;

	/*!	Refer to ameba_wifi_country_code_table_usrcfg.c, e.g. China: country_code[0] = 'C', country_code[1] = 'N'.\n
		Specical value: country_code[0]~[1] = 0x0000: follow efuse; country_code[0]='0', country_code[1]='0' : WORLDWIDE. */
	s8 country_code[2];

	/*!	Bandwidth 40MHz, some IC hardware does not support.*/
	unsigned char bw_40_enable;

	/*!	Refe to 802.11d spec, obtain the country code information from beacon, and set the pwr limit and channel plan.*/
	unsigned char rtw_802_11d_en;

	/*!	When booting the STA, it automatically reconnects to previously connected AP*/
	unsigned char fast_reconnect_en;

	/*!	When disconnection, STA automatically reconnects.*/
	unsigned char auto_reconnect_en;

	/*!	When disconnection, STA automatically reconnects, and auto_reconnect_count is the number of attempts.*/
	unsigned char auto_reconnect_count;

	/*!	auto_reconnect_interval is Automatic reconnection interval, unit s.*/
	unsigned char auto_reconnect_interval;

	/*!	no_beacon_disconnect_time is the disconnect time when no beacon occurs, unit 2s.*/
	unsigned char no_beacon_disconnect_time;

	/*!	skb_num_np is wifi driver's trx buffer number, each buffer occupies about 1.8K bytes of heap, a little difference between different chips.\n
		These buffer are used for all traffics except tx data in INIC mode, and all traffics in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_np can be increased.\n
		Minimum: 7 (3 for Ameba lite). Default: 10.*/
	int skb_num_np;

	/*!	These buffer are used for tx data packtes in INIC mode, not used in single core mode.\n
		For higher throughput or a large number of STAs connected to softap, skb_num_ap can be increased. */
	int skb_num_ap;

	/*!	Specify the trx buffer size of each skb.\n
		Cache size(32 for amebadplus&amebalite and 64 for amebasmart)alignment will be applied to the input size.\n
		Considering the length field of L-SIG is 12 bits, the max PSDU size is 4095 bytes, so skb_buf_size is suggested not exceed 4k.\n
		0: use default size. */
	unsigned int skb_buf_size;

	/*!	Every data tx is forced to start with cts2self. */
	unsigned char force_cts2self;

	/*!	Multi Channel Concurrent mode, STA and Softap can work on different channels via TDMA.
		@note Mcc performance has limitations, please contact Realtek before use to clarify your requirements. */
	unsigned char en_mcc;

	unsigned char tx_shortcut_enable;

	unsigned char rx_shortcut_enable;

	/*! For concurrent mode:
	    - \b 0: STA or SoftAP only at any time, The MAC address of TA or Softap is the MAC address of chip;
		- \b 1: STA and SoftAP may run at the same time, Softap's mac address depends on softap_addr_offset_idx. */
	unsigned char concurrent_enabled;

	/*!	It is valid only when concurrent_enabled =1. The range is 0~5.The lowest bit of mac[0] is 1, which represents the multicast address,
		therefore, the mac[0] of softap is incremented by 2, others is incremented by 1.
		- e.g. softap_addr_offset_idx = 0, chip's mac = 00:e0:4c:01:02:03, softap's mac = 02:e0:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 1, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e1:4c:01:02:03;
		- e.g. softap_addr_offset_idx = 5, chip's mac = 00:e0:4c:01:02:03, softap's mac = 00:e0:4c:01:02:04.*/
	unsigned char softap_addr_offset_idx;

	/*!	The number of ampdu that Recipient claims to Originator for RX, it can be any value less than 64.\n
		skb_num_np needs to be adjusted simultaneously.*/
	unsigned char rx_ampdu_num;

	/*!	Linux Fullmac architecture, ignore in RTOS.*/
	unsigned char cfg80211;

	/*!	WPS. */
	unsigned char wps_retry_count;

	/*!	Unit: ms. */
	unsigned int wps_retry_interval;

	/*!	For wifi speaker configuration.\n
		BIT0: Main switch, BIT1: Enable tsf interrupt, BIT2: Enable audio tsf. */
	unsigned char wifi_speaker_feature;

	/*! STA mode will periodically send null packet to AP to keepalive, unit: second. */
	unsigned char keepalive_interval;

	/*! Automatic channel selection.*/
	unsigned char acs_en;

	/*! R-mesh AP strong RSSI thresh, when AP rssi larger than this thresh, will try to switch to AP.*/
	signed char wtn_strong_rssi_thresh;

	/*! R-mesh father refresh timeout, when not receive beacon from father for this timeout, will switch to other node, unit: millisecond.*/
	u16 wtn_father_refresh_timeout;

	/*! R-mesh child refresh timeout, when not receive beacon from child for this timeout, will delete this child, unit: millisecond.*/
	u16 wtn_child_refresh_timeout;

	/*! 0: Disable R-mesh NAT function, 1: Enable R-mesh NAT function.*/
	unsigned char wtn_rnat_en;

	/*! Max node number in R-mesh network, this is used for decide each node's beacon window.*/
	u16 wtn_max_node_num;
};

/** @} End of WIFI_Exported_Structure_Types group*/
/** @} End of WIFI_Exported_Types group*/
/** @} End of WIFI_API group */

/* not included in any api groups*/
#ifndef CONFIG_FULLMAC
extern struct _Rltk_wlan_t rltk_wlan_info[NET_IF_NUM];
#endif
extern  struct wifi_user_conf wifi_user_config;
extern struct _rtw_wifi_setting_t wifi_setting[2];

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @addtogroup WIFI_Exported_Constants Wi-Fi Exported Constants
 * @{
 */
#ifndef CONFIG_FULLMAC
#define netdev_priv(dev)		dev->priv
#define rtw_is_netdev_enable(idx)	(rltk_wlan_info[idx].enable)
#define rtw_get_netdev(idx)		(&(rltk_wlan_info[idx].dev))
#endif
/** @} End of WIFI_Exported_Constants group*/

/**********************************************************************************************
 *                                     Function Declarations
 *********************************************************************************************/
/** @defgroup WIFI_Exported_Functions Wi-Fi Exported Functions
 * @{
 */
/** @addtogroup WIFI_Exported_Basic_Functions Basic Functions
  * @{
  */

/**
 * @brief  Enable Wi-Fi, i.e., bring the Wireless interface "Up".
 * @param[in]  mode: Should always set to RTW_MODE_STA.
 * @return
 *    - @ref RTW_SUCCESS : If the WiFi chip initialized successfully.
 *    - @ref RTW_ERROR : If the WiFi chip initialization failed.
 * @note  Call wifi_start_ap() afther this API if want to use AP mode.
 */
int wifi_on(u8 mode);

/**
 * @brief  Check if the specified wlan interface  is running.
 * @param[in]  wlan_idx: Can be set as STA_WLAN_INDEX or SOFTAP_WLAN_INDEX.
 * @return  If the function succeeds, the return value is 1.
 * 	Otherwise, return 0.
 * @note  For STA mode, use STA_WLAN_INDEX; for AP mode, use SOFTAP_WLAN_INDEX.
 */
int wifi_is_running(unsigned char wlan_idx);

/**
 * @brief  Join a Wi-Fi network.
 * 	Scan for, associate and authenticate with a Wi-Fi network.
 * @param[in]  connect_param: The pointer of a struct which store the connection
 * 	info, including ssid, bssid, password, etc, for details, please refer to struct
 * 	_rtw_network_info_t in wifi_conf.h.
 * @param[in]  block:
 *                  - If block is set to 1, it means synchronized wifi connect, and this
 * 	                  API will return until connect is finished;
 *                  - If block is set to 0, it means asynchronized wifi connect, and this
 *                    API will return immediately.
 * @return
 *    - @ref RTW_SUCCESS : Join successfully for synchronized wifi connect,
 *  						or connect cmd is set successfully for asynchronized wifi connect.
 *    - @ref RTW_ERROR : An error occurred.
 *    - @ref RTW_BUSY : Wifi connect or scan is ongoing.
 *    - @ref RTW_NOMEM : Malloc fail during wifi connect.
 *    - @ref RTW_TIMEOUT : More than RTW_JOIN_TIMEOUT(~70s) without successful connection.
 *    - @ref RTW_CONNECT_INVALID_KEY : Password format wrong.
 *    - @ref RTW_CONNECT_SCAN_FAIL : Scan fail.
 *    - @ref RTW_CONNECT_AUTH_FAIL : Auth fail.
 *    - @ref RTW_CONNECT_AUTH_PASSWORD_WRONG : Password error causing auth failure, not entirely accurate.
 *    - @ref RTW_CONNECT_ASSOC_FAIL : Assoc fail.
 *    - @ref RTW_CONNECT_4WAY_HANDSHAKE_FAIL : 4 way handshake fail.
 *    - @ref RTW_CONNECT_4WAY_PASSWORD_WRONG : Password error causing 4 way handshake failure,not entirely accurate.
 * @note
 *      - Please make sure the Wi-Fi is enabled (wifi_on()) before invoking this function.
 *      - If bssid in connect_param is set, then bssid will be used for connect, otherwise ssid is used for connect.
 */
int wifi_connect(struct _rtw_network_info_t *connect_param, unsigned char block);

/**
 * @brief  Disassociates from current Wi-Fi network.
 * @return
 *    - @ref RTW_SUCCESS : On successful disassociation from the AP.
 *    - @ref RTW_ERROR : If an error occurred.
 */
int wifi_disconnect(void);

/**
 * @brief  Get join status during wifi connectection.
 * @return
 *    - @ref RTW_JOINSTATUS_UNKNOWN : Unknown.
 *    - @ref RTW_JOINSTATUS_STARTING :	Starting phase.
 *    - @ref RTW_JOINSTATUS_SCANNING : Scanning phase.
 *    - @ref RTW_JOINSTATUS_AUTHENTICATING : Authenticating phase.
 *    - @ref RTW_JOINSTATUS_AUTHENTICATED : Authenticated phase.
 *    - @ref RTW_JOINSTATUS_ASSOCIATING : Associating phase.
 *    - @ref RTW_JOINSTATUS_ASSOCIATED : Associated phase.
 *    - @ref RTW_JOINSTATUS_4WAY_HANDSHAKING : 4 way handshaking phase.
 *    - @ref RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE : 4 way handshake done phase.
 *    - @ref RTW_JOINSTATUS_SUCCESS : Join success.
 *    - @ref RTW_JOINSTATUS_FAIL :	Join fail.
 *    - @ref RTW_JOINSTATUS_DISCONNECT : Disconnect.
 */
u8 wifi_get_join_status(void);

/**
 * @brief  Initiate a scan to search for 802.11 networks. There are two different scan type can be confgiured
 *         by the input param block:
 *         - Synchronized scan.
 *         - Asynchronized scan. There are two different ways about how the scan result will be reported:
 *           - The first way is that when scan is done ,the total number of scanned APs will be reported through
 *             scan_user_callback, and the detailed scanned AP infos can be get by calling wifi_get_scan_records(),
 *             so in this way, scan_user_callback need to be registered in scan_param.
 *           - The second way is that every time a AP is scanned, this AP info will be directly reported by
 *             scan_report_each_mode_user_callback, and when scan is done, scan_report_each_mode_user_callback will
 *             report a NULL pointer for notification. So in this way, scan_report_each_mode_user_callback need to
 *             be registered in scan_param, and RTW_SCAN_REPORT_EACH need to be set in scan_param->options.Also in
 *             this mode, scan_user_callback is no need to be registered.
 * @param[in]  scan_param: Refer to struct struct _rtw_scan_param_t in wifi_conf.h.
 * @param[in]  block:
 * 					- If set to 1, it's synchronized scan and this API will return after scan is done.
 * 					- If set to 0, it's asynchronized scan and this API will return immediately.
 * @return  @ref RTW_SUCCESS or @ref RTW_ERROR for asynchronized scan, return @ref RTW_ERROR or
 * 	scanned AP number for Synchronized scan.
 * @note  If this API is called, the scanned APs are stored in WiFi driver dynamic
 * 	allocated memory, for synchronized scan or asynchronized scan which not use RTW_SCAN_REPORT_EACH,
 * 	these memory will be freed when wifi_get_scan_records() is called.
 */
int wifi_scan_networks(struct _rtw_scan_param_t *scan_param, unsigned char block);

/**
 * @brief  Trigger Wi-Fi driver to start an infrastructure Wi-Fi network.
 * @param[in]  softAP_config: The pointer of a struct which store the softAP
 * 	configuration, please refer to struct _rtw_softap_info_t in wifi_conf.h.
 * @warning  If a STA interface is active when this function is called,
 * 	the softAP will start on the same channel as the STA.
 * 	It will NOT use the channel provided!
 * @return
 *    - @ref RTW_SUCCESS : If successfully creates an AP.
 *    - @ref RTW_ERROR : If an error occurred.
 * @note
 *     - If hidden_ssid in softAP_config is set to 1, then this softAP will start
 * 	     with hidden ssid.
 *     - Please make sure the Wi-Fi is enabled (wifi_on()) before invoking this function.
 */
int wifi_start_ap(struct _rtw_softap_info_t *softAP_config);

/**
 * @brief  Disable Wi-Fi interface-2.
 * @return
 *    - @ref RTW_SUCCESS : Deinit success, wifi mode is changed to RTW_MODE_STA.
 *    - @ref RTW_ERROR : Otherwise.
 */
int wifi_stop_ap(void);

/** @} End of Basic_Functions group */
/** @} End of WIFI_Exported_Functions group */
/** @} End of WIFI_API group */
#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_H
