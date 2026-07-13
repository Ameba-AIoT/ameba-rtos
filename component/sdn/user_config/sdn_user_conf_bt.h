#ifndef __SDN_USER_CONF_BT_H__
#define __SDN_USER_CONF_BT_H__

#ifdef CONFIG_BT_INIC
#define BLE_FEATURE_DEFAULT         1
#else
#include <bt_api_config.h>
#define BLE_FEATURE_DEFAULT         0
#endif

#ifndef RTK_BLE_PRIVACY_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT                             BLE_FEATURE_DEFAULT
#endif

#ifndef RTK_BLE_5_0_SET_PHYS_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT                        BLE_FEATURE_DEFAULT
#endif

#ifndef RTK_BLE_5_0_AE_ADV_SUPPORT
#define RTK_BLE_5_0_AE_ADV_SUPPORT                          BLE_FEATURE_DEFAULT
#endif

#ifndef RTK_BLE_5_0_AE_SCAN_SUPPORT
#define RTK_BLE_5_0_AE_SCAN_SUPPORT                         BLE_FEATURE_DEFAULT
#endif

#ifndef RTK_BLE_5_0_PA_ADV_SUPPORT
#define RTK_BLE_5_0_PA_ADV_SUPPORT                          0
#endif

#ifndef RTK_BLE_5_0_PA_SYNC_SUPPORT
#define RTK_BLE_5_0_PA_SYNC_SUPPORT                         0
#endif

#ifndef RTK_BLE_GAP_MAX_LINKS
#define RTK_BLE_GAP_MAX_LINKS                               8
#endif


#if (RTK_BLE_PRIVACY_SUPPORT == 1)
#define CONFIG_BLE_LL_PRIVACY_ENABLE
#define CONFIG_BLE_LL_RESOLVE_LIST_SIZE                     8
#endif

#if (RTK_BLE_5_0_SET_PHYS_SUPPORT == 1)
#define CONFIG_BLE_LL_SET_PHY_ENABLE
#endif

#if (RTK_BLE_5_0_PA_ADV_SUPPORT == 1)
#define CONFIG_BLE_LL_PA_ADV_ENABLE
#endif

#if (RTK_BLE_5_0_PA_SYNC_SUPPORT == 1)
#define CONFIG_BLE_LL_PA_SYNC_ENABLE
#endif

#if (RTK_BLE_5_0_AE_ADV_SUPPORT == 1) || (RTK_BLE_5_0_AE_SCAN_SUPPORT == 1)
#define CONFIG_BLE_LL_EXT_ADV_ENABLE
#define BLE_LL_MAX_ADV_SET                                  3
#define BLE_LL_MAX_ADV_TX_CHAIN_NUM                         4
#define BLE_LL_MAX_ADV_RX_CHAIN_NUM                         4
#else
#define BLE_LL_MAX_ADV_SET                                  1
#define BLE_LL_MAX_ADV_TX_CHAIN_NUM                         0
#define BLE_LL_MAX_ADV_RX_CHAIN_NUM                         0
#endif

#if defined(CONFIG_MP_INCLUDED)
#define CONFIG_BLE_LL_DTM_ENABLE
#endif

#if defined(CONFIG_BLE_LL_PA_SYNC_ENABLE)
#define BLE_LL_MAX_PA_SYNC_SET_NUM                          1
#define BLE_LL_MAX_PA_SYNC_CHAIN_NUM                        7
#else
#define BLE_LL_MAX_PA_SYNC_SET_NUM                          0
#define BLE_LL_MAX_PA_SYNC_CHAIN_NUM                        0
#endif

#define BT_LL_LE_MAX_CONN_NUM                               RTK_BLE_GAP_MAX_LINKS

#define BLE_LL_RX_DATA_NUM_PER_LINK                         5
#define BLE_LL_TX_ACL_NUM_PER_LINK                          5
#define BLE_LL_TX_CTRL_NUM_PER_LINK                         3

#endif