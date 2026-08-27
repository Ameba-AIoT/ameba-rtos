#ifndef __SDN_INIC_CONF_H__
#define __SDN_INIC_CONF_H__

#include <platform_autoconf.h>

#ifdef CONFIG_RTL8720F
#define RTK_BLE_GAP_MAX_LINKS               6
#define RTK_BLE_SUPPORT                     1
#define RTK_BT_POWER_CONTROL_SUPPORT        0

#define RTK_BLE_GATTS_SUPPORT               1
#define RTK_BLE_GATTC_SUPPORT               1
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 0
#define RTK_BLE_5_2_SUPPORT                 0
#define RTK_BLE_SMP_OOB_SUPPORT             1
#define RTK_BLE_COC_SUPPORT                 0

#if RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          1
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         1
#define RTK_BLE_5_0_PA_ADV_SUPPORT         (0 && RTK_BLE_5_0_AE_ADV_SUPPORT)
#define RTK_BLE_5_0_PA_SYNC_SUPPORT        (0 && RTK_BLE_5_0_AE_SCAN_SUPPORT)
#endif /* RTK_BLE_5_0_SUPPORT */

#endif

#endif /* #ifndef __SDN_INIC_CONF_H__ */