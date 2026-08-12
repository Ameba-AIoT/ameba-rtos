#ifndef __SDN_CONF_H__
#define __SDN_CONF_H__

//#include "platform_autoconf.h"

//SDN Configuration
#define SDN_HAL_SUSPEND_ENABLE                                     1

//IC configuration
#define BT_LL_VERSION                                               BT_HCI_VERSION_5_4
#define BT_LL_HCI_REVISION                                          0x000A
#define BT_LL_LMP_SUB_VERSION                                       0x7098
#define BT_LL_COMPANY_ID                                            0x005D

//BT Feature
#define BT_LL_FEATURE_CONN_PARAM_REQ                                0

//BT 4.2
#define BT_LL_FEATURE_BT42_LE_DATA_LENGTH_EXTENSION                 1

#define BT_LL_FEATURE_BT42_SECURE_CONNECTION                        1

#define BT_LL_FEATURE_BT42_EXTENDED_SCAN_FILTER_POLICY              1

#define BT_LL_FEATURE_BT42_LE_PRIVACY                               1

//BT 5.0
#define BT_LL_FEATURE_BT50_LE_2M_PHY                                1

#define BT_LL_FEATURE_BT50_LE_CODED_PHY                             1

#define BT_LL_FEATURE_BT50_LE_EXT_ADV                               1

#define BT_LL_FEATURE_BT50_LE_CHNL_SEL_ALGO_2                       1

#define BT_LL_FEATURE_BT50_LE_PA_ADV                                0

#define BT_LL_FEATURE_BT50_LE_PA_SYNC                               0

//BLE Role
#define BT_LL_LE_CENTRAL                                            1
#define BT_LL_SCAN_AND_INITIATE_IN_PARALLEL                         1
#define BT_LL_LE_PSD_SCAN                                           0

#if BT_LL_FEATURE_BT50_LE_EXT_ADV
#define BT_LL_LE_SCAN_SET_NUM                                       2
#else
#define BT_LL_LE_SCAN_SET_NUM                                       1
#endif

//BT parameter configuration
#define BT_LL_LE_MAX_ACCEPT_LIST_SIZE                               8

#define BT_LL_LE_SCAN_MAX_DUP_FILTER_NUM                            8

#define BT_LL_LE_RESOLVING_LIST_SIZE                                8

#define BT_LL_LE_MAX_PA_SYNC_LIST_SIZE                              8

#define BT_LL_LE_PA_SYNC_LIST_SIZE                                  8

#endif