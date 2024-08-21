/**
  ******************************************************************************
  * @file    autoconf_8721da.h
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

#ifndef AUTOCONF_8721DA_H
#define AUTOCONF_8721DA_H

#define PHYSTATUS_WITH_PKT 1
/* For STA+AP Concurrent MODE */
/****************** configurations for concurrent mode ************************/
//#define CONFIG_WIFI_TUNNEL
//#define WIFI_TUNNEL_DEMO
//#define CONFIG_NAN
#define CONFIG_MCC_MODE
//#define CONFIG_MCC_NAN_TEST
#ifdef CONFIG_NAN
#define NET_IF_NUM	3
#define SUPPORT_ADAPTER_NUM	3
#else
#define NET_IF_NUM 2
#define SUPPORT_ADAPTER_NUM	2
#endif
/**************** configurations for concurrent mode end **********************/

/************************* Default Values of User Configure *****************************/
/* Upper limit of STAs connected with SoftAP, more STAs connected will cost more heap*/
#define AP_STA_NUM	5
#define MACID_HW_MAX_NUM		16
/************************* Default Values of User Configure End***************************/

/* Configure for bus */
#define CONFIG_AXI_HCI
/* For efuse or flash config start */
#define CONFIG_EFUSE_SEPARATE
#define CONFIG_EFUSE_RAW
/* For efuse or flash config end */
/* PHY layer band config */
#define CONFIG_DFS

/* 0: 20 MHz, 1: 40 MHz, 2: 80 MHz, 3: 160MHz, 4: 80+80MHz
* 2.4G use bit 0 ~ 3, 5G use bit 4 ~ 7
* 0x21 means enable 2.4G 40MHz & 5G 80MHz */
#define  CONFIG_BW_MODE	0x11
#define SUPPORT_5G_CHANNEL
/* PHY layer band config end */

/* enable 1X code in lib_wlan as default (increase 380 bytes) */
#define CONFIG_EAP
/* For phydm configurations */
#define CONFIG_FW_C2H_PKT

#undef RFDBG
#define RFDBG 0 /* for rf dbg */
#define CONFIG_BT_COEXIST
#define RX_SHORTCUT
#define TX_SHORTCUT
//#define SW_WEP_TKIP

#define RTL8721DA_SPECIFIC
//#define CONFIG_SUPPORT_DYNAMIC_TXPWR
#define CONFIG_ADDRESS_CAM
/* enable csi function */
#define CONFIG_CSI
#define RTL8721DA_WORK_AROUND

//#define CONFIG_TWT
#ifdef CONFIG_NAN
#define CONFIG_NAN_PAIRING
#define CONFIG_TWT
#undef CONFIG_ADDRESS_CAM
#endif

#ifdef CONFIG_MCC_MODE
#define CONFIG_TWT
#endif

//#define CONFIG_P2P

/*enable dynamic mechanism for driver*/
#define CONFIG_WIFI_RA
#define CONFIG_WIFI_DIG
#ifdef CONFIG_WIFI_DIG
#define CONFIG_WIFI_DIG_DAMPING_CHK
#endif
#define CONFIG_WIFI_EDCCA
#define CONFIG_WIFI_ANTDIV

/*dplus not support 11ax, use phydm*/
#define PHYDM_VERSION	2 /*phydm trunk*/

/*Wifi verification*/
#if defined(CONFIG_WIFI_VERIFY_TRUEPHY) || defined (CONFIG_WIFI_VERIFY_ASIC)
// #define DISABLE_BB_RF
#elif defined (CONFIG_WIFI_VERIFY_PSPHY)
#define DISABLE_BB_RF
#endif

#ifdef CONFIG_WIFI_VERIFY
#define WIFI_TEST 1
#endif

/* debug log level */
#undef RELEASE_WIFI

#define RA_RX_ACK_RSSI

//#define CONFIG_SPEAKER_ENCRYPT

//#define CONFIG_AUDIO_TSF
/*************************** Config for MP_MODE *******************************/
#ifdef CONFIG_MP_INCLUDED
#undef RX_SHORTCUT
#undef TX_SHORTCUT

#define CONFIG_PHYDM_CMD  /*disable it in normal driver,lite can save 172KB code size, smart need check*/
//#define CONFIG_PHYDM_DEBUG_FUNCTION
#endif

#ifdef CONFIG_MP_SHRINK
#undef CONFIG_TWT
#undef CONFIG_MCC_MODE
#undef CONFIG_DFS
#undef CONFIG_FW_C2H_PKT
#undef CONFIG_IEEE80211W
#undef CONFIG_WIFI_RA
#undef CONFIG_WIFI_DIG
#undef CONFIG_WIFI_TDMA_DIG /*for softap*/
#undef CONFIG_WIFI_EDCCA
#undef CONFIG_WIFI_ANTDIV
#undef RA_RX_ACK_RSSI
#undef CONFIG_FW_C2H_PKT
#undef CONFIG_CSI
#define DISABLE_FW
#endif

#define DBG 0 /* for phydm debug */
/************************* Config for MP_MODE end *****************************/

/*Config for SKB Size*/
#define SKB_CACHE_SZ	32/*max(AP_Core_Cache, NP_Core_Cache)*/
#define SKB_ALIGNMENT	__attribute__((aligned(SKB_CACHE_SZ)))
#define TXDESC_SIZE	40
#define RXDESC_SIZE	24
#define RXPHYSTS_SIZE 32
#define WLAN_HW_INFO_LEN	56/*max(TXDESC_SIZE, RXDESC_SIZE+RXPHYSTS_SIZE)*/
#ifdef CONFIG_SPEAKER_TEST
#define WLAN_MAX_ETHFRM_LEN	2014/*max payload size of speaker frame*/
#else
#define WLAN_MAX_ETHFRM_LEN	1514/*max payload size of wifi frame*/
#endif
#define CONFIG_IOT_RS 1
#endif /*#ifndef AUTOCONF_8721DA_H */

