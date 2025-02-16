// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_WIPHY_H__
#define __RTW_WIPHY_H__

#define RTW_SSID_SCAN_AMOUNT		1 /* for WEXT_CSCAN_AMOUNT 9 */
#define RTW_CHANNEL_SCAN_AMOUNT		(14+37)
#define RTW_SCAN_IE_LEN_MAX      	2304
#define RTW_MAX_NUM_PMKIDS		4
#define	MAX_CHANNEL_NUM_2G		14
#define	MAX_CHANNEL_NUM_5G		28
#define MAX_CHANNEL_NUM 		(MAX_CHANNEL_NUM_2G + MAX_CHANNEL_NUM_5G)
#define RTW_G_RATES_NUM			12
#define RTW_A_RATES_NUM			8
#define RTW_MAX_REMAIN_ON_CHANNEL_DURATION	5000

#define WIFI_AKM_SUITE_PSK		0x000FAC02
#define WIFI_AKM_SUITE_SAE		0x000FAC08
#define WIFI_AKM_SUITE_OWE		0x000FAC12

#define ASSOC_IE_MAX_LEN		500
#define WLAN_HDR_A3_LEN			24
#define RSNXE_MAX_LEN (18)
#define WLAN_EID_RSNX 244
#define WLAN_EID_RSN			48
#define WLAN_EID_EXT_OWE_DH_PARAM	32

#define rtw_a_rates			(rtw_rates + 4)
#define rtw_g_rates			(rtw_rates + 0)

#define RATETAB_ENT(_rate, _rateid, _flags) \
	{								\
		.bitrate		= (_rate),			\
		.hw_value		= (_rateid),			\
		.flags			= (_flags),			\
	}

#define CHAN2G(_channel, _freq, _flags) {				\
		.band			= NL80211_BAND_2GHZ,		\
		.center_freq		= (_freq),			\
		.hw_value		= (_channel),			\
		.flags			= (_flags),			\
		.max_antenna_gain	= 0,				\
		.max_power		= 30,				\
	}

#define CHAN5G(_channel, _flags) {					\
		.band			= NL80211_BAND_5GHZ,		\
		.center_freq		= 5000 + (5 * (_channel)),	\
		.hw_value		= (_channel),			\
		.flags			= (_flags),			\
		.max_antenna_gain	= 0,				\
		.max_power		= 30,				\
	}

#endif //__RTW_WIPHY_H__
