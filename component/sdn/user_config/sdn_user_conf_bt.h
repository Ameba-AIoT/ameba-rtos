#ifndef __SDN_USER_CONF_BT_H__
#define __SDN_USER_CONF_BT_H__

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
#define CONFIG_BLE_LL_RESOLVE_LIST_SIZE                     8
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
#define CONFIG_BLE_LL_SET_PHY_ENABLE
#endif

#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) || \
    (defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT)
#define CONFIG_BLE_LL_EXT_ADV_ENABLE
#define BLE_LL_MAX_ADV_SET                                  3
#define BLE_LL_MAX_ADV_TX_CHAIN_NUM                         4
#define BLE_LL_MAX_ADV_RX_CHAIN_NUM                         4
#else
#define BLE_LL_MAX_ADV_SET                                  1
#define BLE_LL_MAX_ADV_TX_CHAIN_NUM                         0
#define BLE_LL_MAX_ADV_RX_CHAIN_NUM                         0
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
#define CONFIG_BLE_LL_PA_ADV_ENABLE
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
#define CONFIG_BLE_LL_PA_SYNC_ENABLE
#define BLE_LL_MAX_PA_SYNC_SET_NUM                          2
#define BLE_LL_MAX_PA_SYNC_CHAIN_NUM                        7
#endif

#ifdef CONFIG_MP_INCLUDED
#define CONFIG_BLE_LL_DTM_ENABLE
#endif

#define BT_LL_LE_MAX_CONN_NUM                               RTK_BLE_GAP_MAX_LINKS

#define BLE_LL_RX_DATA_NUM_PER_LINK                         5
#define BLE_LL_TX_ACL_NUM_PER_LINK                          5
#define BLE_LL_TX_CTRL_NUM_PER_LINK                         3

#endif