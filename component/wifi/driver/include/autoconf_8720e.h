/**
  ******************************************************************************
  * @file    autoconf_8720e.h
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

#ifndef AUTOCONF_8720E_H
#define AUTOCONF_8720E_H

#define STATIC_RXRING_POOL 1

/* For STA+AP Concurrent MODE */
/****************** configurations for concurrent mode ************************/
#define CONFIG_MCC_MODE
#define SUPPORT_ADAPTER_NUM	2
/**************** configurations for concurrent mode end **********************/

/************************* Default Values of User Configure *****************************/
#define MACID_HW_MAX_NUM		16
/************************* Default Values of User Configure End***************************/

/* Configure for bus */
#define CONFIG_AXI_HCI

/* For efuse or flash config start */
#define CONFIG_EFUSE_SEPARATE
#define CONFIG_EFUSE_RAW
/* For efuse or flash config end */

/* 0: 20 MHz, 1: 40 MHz, 2: 80 MHz, 3: 160MHz, 4: 80+80MHz
* 2.4G use bit 0 ~ 3, 5G use bit 4 ~ 7
* 0x21 means enable 2.4G 40MHz & 5G 80MHz */
#define CONFIG_BW_MODE 0x00

/*Amebalite not support 5G*/
//#define SUPPORT_5G_CHANNEL

/* PHY layer band config end */

//#define SW_WEP_TKIP

//#define CONFIG_BT_COEXIST

#if WIFI_LOGO_CERTIFICATION == 0
#define RX_SHORTCUT /*there's no reoder in rx short right now, wifi logo need ping 10k which needs reorder*/
#define TX_SHORTCUT /*there's no sw encrypt in tx short right now, wifi logo need htc which needs sw encrypt*/
#endif

/* For 11 ax function */
#define CONFIG_80211AX_HE
#ifdef CONFIG_80211AX_HE
//#define CONFIG_MBSSID_AX
//#define CONFIG_TWT
#endif
//#define CONFIG_DEBUG_RTL871X

#if WIFI_LOGO_CERTIFICATION
#ifdef CONFIG_80211AX_HE
#define CONFIG_MBSSID_AX
#endif
#endif

//#define CONFIG_SUPPORT_DYNAMIC_TXPWR

/*enable dynamic mechanism for driver*/
#define CONFIG_WIFI_RA
#define CONFIG_WIFI_DIG
#define CONFIG_WIFI_TDMA_DIG /*for softap*/
#define CONFIG_WIFI_EDCCA
#define CONFIG_WIFI_ANTDIV

/*halbb halrf config*/
#define CONFIG_FW_C2H_PKT
#define PHYDM_VERSION	3/*halbb halrf*/
#define DRV_BB_DFS_DISABLE
#define DRV_BB_RUA_DISABLE
#define DRV_BB_LA_MODE_DISABLE
#define DRV_BB_PWR_CTRL_DISABLE
#define DRV_BB_CFO_TRK_DISABLE
#define PLATFOM_IS_LITTLE_ENDIAN	1/*for halbb use*/

/*Wifi verification*/
#if defined(CONFIG_WIFI_VERIFY_TRUEPHY) || defined(CONFIG_WIFI_VERIFY_PSPHY)
#define WIFI_TEST 1
#define DISABLE_FW
#define DISABLE_BB_RF
#endif
#ifdef CONFIG_WIFI_VERIFY_ASIC
#define WIFI_TEST 1
#endif

//#define CONFIG_DFS_TEST  // add for dfs test
#define CONFIG_DFS // some application may use CSA although AmebaLite does not support 5G

/* enable csi function */
#define CONFIG_CSI

#define PHYSTS_WORK_AROUND
#define RTL8720E_WORK_AROUND

//#define RA_RX_ACK_RSSI

/*************************** Config for Gen TxPower Tool *******************************/
/** VERSION 0: only support extending one EXT PWR_LIMIT table
  * VERSION 1: support more EXT PWR_LIMIT tables, depending on customer configuration
  */
#define GEN_TXPWR_TOOL_VERSION 1
/*************************** Config for Gen TxPower Tool End **************************/

/*************************** Config for MP_MODE *******************************/
#ifdef CONFIG_MP_SHRINK
#undef RX_SHORTCUT
#undef TX_SHORTCUT

#define DRV_BB_DBG_TRACE_DISABLE
#define DRV_BB_PMAC_TX_DISABLE
#define DRV_BB_CMN_RPT_DISABLE
#define DRV_BB_STATISTICS_DISABLE
#define DRV_BB_DGB_SUPPORT_DISABLE
#define DRV_BB_CH_INFO_DISABLE
#define DRV_BB_ENV_MNTR_DISABLE
#define DRV_BB_PHYSTS_PARSING_DISABLE
#define DRV_BB_TIMER_SUPPORT_DISABLE

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
#define DISABLE_FW
#endif

/************************* Config for MP_MODE end *****************************/
/* Config for BB/RF debug */
//#define CONFIG_PHYDM_CMD
//#define CONFIG_HALRF_CMD

/*Config for SKB Size*/
#define SKB_CACHE_SZ	32/*max(AP_Core_Cache, NP_Core_Cache)*/
#define SKB_ALIGNMENT	__attribute__((aligned(SKB_CACHE_SZ)))
#define TXDESC_SIZE	40
#define RXDESC_SIZE	24
#define RXPHYSTS_SIZE 0
#define WLAN_HW_INFO_LEN	40/*max(TXDESC_SIZE, RXDESC_SIZE+RXPHYSTS_SIZE)*/
#define WLAN_MAX_ETHFRM_LEN	1514/*max payload size of wifi frame*/

/* debug log level */
#define RELEASE_WIFI

#define CONFIG_IOT_RS 1
#endif /*#ifndef AUTOCONF_8720E_H */

