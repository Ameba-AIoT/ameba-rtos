/**
  ******************************************************************************
  * @file    rtw_autoconf.h
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

#ifndef WLANCONFIG_H
#define WLANCONFIG_H

/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#include "platform_autoconf.h"

#define CONFIG_LITTLE_ENDIAN


//#define CONFIG_HIGH_TP

#define WIFI_LOGO_CERTIFICATION 0
#define RX_AMSDU

/* no IOT chip supports 80M now, so close it in common */
#define CONFIG_AUTO_RECONNECT 1

/* For WPA3 */
#define CONFIG_IEEE80211W
#define CONFIG_OWE_SUPPORT
#define CONFIG_SAE_SUPPORT
#ifdef CONFIG_SAE_SUPPORT
#define CONFIG_SAE_DH_SUPPORT 1
#endif

#define CONFIG_PS_EN

/* For promiscuous mode */
#define CONFIG_PROMISC

/* For WPS and P2P */
#define CONFIG_WPS

/************************** config to support chip ****************************/
#define RTL8723B_SUPPORT 0
#define RTL8192E_SUPPORT 0
#define RTL8188E_SUPPORT 0
#define RTL8188F_SUPPORT 0
#define RTL8720E_SUPPORT 0
#define RTL8720F_SUPPORT 0
#define RTL8721D_SUPPORT 0
#define RTL8723D_SUPPORT 0
#define RTL8195B_SUPPORT 0
#define RTL8710C_SUPPORT 0
#define RTL8730A_SUPPORT 0
#define RTL8730E_SUPPORT 0
#define RTL8721DA_SUPPORT 0
/************************ config to support chip end **************************/

/******************* Ameba Series Common Configurations ***********************/
/*PHYDM version*/
#define OUTSRC	1
#define PHYDM	2
#define HALBBRF	3

#if defined(CONFIG_AMEBADPLUS)
/******************************* AmebaDPLUS (8721DA) *******************************/
#include "autoconf_8721da.h"
#elif defined(CONFIG_AMEBASMART)
/******************************* AmebaD2 (8730E) ******************************/
#include "autoconf_8730e.h"
#elif defined(CONFIG_AMEBALITE)
/***************************** AmebaLite (8720E) *****************************/
#include "autoconf_8720e.h"
#elif defined(CONFIG_AMEBAGREEN2)
/******************************* Amebalite2 (8720f) ******************************/
#include "autoconf_8720f.h"

#endif
/****************** Configurations for each platform end **********************/


/************************ For EAP auth configurations *************************/
#include "autoconf_eap.h"
/************************ For EAP auth configurations *************************/
/* KVR macro is default opened, but actually not working. To use it, need turn on the switch in menuconfig */
#define CONFIG_RTW_WNM
#define CONFIG_IEEE80211R
#define CONFIG_IEEE80211K
#if WIFI_LOGO_CERTIFICATION
#undef WLAN_MAX_ETHFRM_LEN
#define WLAN_MAX_ETHFRM_LEN	4000
/* 80211 - K MBO */
#define CONFIG_RTW_MBO
#endif

#define CONFIG_BEACON_PERIOD 100

#define CONFIG_ACM_METHOD 0	// 0:By SW 1:By HW.

//#define CONFIG_FRAME_DEFRAG // support frame defragmentaion

#ifdef CONFIG_MP_INCLUDED
#undef CONFIG_PS_EN
#undef CONFIG_AUTO_RECONNECT

#undef CONFIG_IEEE80211W
#undef CONFIG_OWE_SUPPORT
#undef CONFIG_SAE_SUPPORT
#undef CONFIG_SAE_DH_SUPPORT
#undef CONFIG_RTW_WNM
#undef CONFIG_IEEE80211R
#undef CONFIG_IEEE80211K
#undef CONFIG_RTW_MBO

#define CONFIG_AUTO_RECONNECT 0
#endif


#endif //WLANCONFIG_H
