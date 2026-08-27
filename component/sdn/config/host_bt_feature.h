#ifndef __HOST_BT_FEATURE_H__
#define __HOST_BT_FEATURE_H__

/* DO NOT modify this file */

#include <platform_autoconf.h>
#ifdef CONFIG_BT_INIC
#include <sdn_inic_conf.h>
#else
#include <bt_api_config.h>
#endif

#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
#define CONFIG_BLE_LL_DATA_LEN_EXT_ENABLE
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
#define CONFIG_BLE_LL_PRIVACY_ENABLE
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
#define CONFIG_BLE_LL_SET_PHY_ENABLE
#endif

#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) || \
    (defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT)
#define CONFIG_BLE_LL_EXT_ADV_ENABLE
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
#define CONFIG_BLE_LL_PA_ADV_ENABLE
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
#define CONFIG_BLE_LL_PA_SYNC_ENABLE
#endif

#ifdef CONFIG_MP_INCLUDED
#define CONFIG_BLE_LL_DTM_ENABLE
#endif

#define BLE_LL_MAX_CONN_NUM                               RTK_BLE_GAP_MAX_LINKS

#endif