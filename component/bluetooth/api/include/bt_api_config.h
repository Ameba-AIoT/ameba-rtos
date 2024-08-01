/**
 * @file      bt_api_config.h
 * @author    frank_shen@realsil.com.cn
 * @brief     Bluetooth platform config macros definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_API_CONFIG_H__
#define __BT_API_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "platform_autoconf.h"

/*
 * PLATFORM_AMEBASMART (AmebaSmart)
 */
#if defined(CONFIG_AMEBASMART) && CONFIG_AMEBASMART
#define RTK_BLE_GAP_MAX_LINKS               4
#define RTK_BLE_SUPPORT                     1
#if defined(CONFIG_BT_BLE_ONLY) && CONFIG_BT_BLE_ONLY   // Defined in menuconfig
#define RTK_BREDR_SUPPORT                   0
#endif
#if defined(CONFIG_BT_DUAL_MODE) && CONFIG_BT_DUAL_MODE // Defined in menuconfig
#define RTK_BREDR_SUPPORT                   1
#endif
#define RTK_BT_POWER_CONTROL_SUPPORT        1
#define RTK_BLE_SET_TX_QUEUE_NUM            0
#define RTK_BLE_TX_SOF_EOF_INDICATION       0

#define RTK_BLE_GATTS                       1
#define RTK_BLE_GATTC                       1
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 1
#define RTK_BLE_5_2_SUPPORT                 1
#define RTK_BLE_SMP_OOB_SUPPORT             1
#define RTK_BLE_COC_SUPPORT                 0
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_4_2_SUPPORT) && RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if defined(RTK_BLE_5_0_SUPPORT) && RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          0
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         0
#define RTK_BLE_5_0_PA_ADV_SUPPORT         (0 && RTK_BLE_5_0_AE_ADV_SUPPORT)
#define RTK_BLE_5_0_PA_SYNC_SUPPORT        (0 && RTK_BLE_5_0_AE_SCAN_SUPPORT)
#endif /* RTK_BLE_5_0_SUPPORT */

#if defined(RTK_BLE_5_1_SUPPORT) && RTK_BLE_5_1_SUPPORT
#define RTK_BLE_5_1_PAST_SENDER_SUPPORT     0
#define RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT  0
#define RTK_BLE_5_1_CTE_SUPPORT             0
#endif /* RTK_BLE_5_1_SUPPORT */

#if defined(RTK_BLE_5_2_SUPPORT) && RTK_BLE_5_2_SUPPORT
#define RTK_BLE_5_2_POWER_CONTROL_SUPPORT   0
#define RTK_BT_5_2_EATT_SUPPORT             0
#define RTK_BT_5_2_L2C_ECFC_SUPPORT         (RTK_BT_5_2_EATT_SUPPORT)
#endif /* RTK_BLE_5_2_SUPPORT */

/*
 * PLATFORM_AMEBALITE (AmebaLite)
 */
#elif defined(CONFIG_AMEBALITE) && CONFIG_AMEBALITE
#define RTK_BLE_GAP_MAX_LINKS               4
#define RTK_BLE_SUPPORT                     1
#if defined(CONFIG_BT_BLE_ONLY) && CONFIG_BT_BLE_ONLY   // Defined in menuconfig
#define RTK_BREDR_SUPPORT                   0
#endif
#if defined(CONFIG_BT_DUAL_MODE) && CONFIG_BT_DUAL_MODE // Defined in menuconfig
#define RTK_BREDR_SUPPORT                   1
#endif
#define RTK_BT_POWER_CONTROL_SUPPORT        1

#define RTK_BLE_GATTS                       1
#define RTK_BLE_GATTC                       1
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 1
#define RTK_BLE_5_2_SUPPORT                 1
#define RTK_BLE_SMP_OOB_SUPPORT             1
#define RTK_BLE_COC_SUPPORT                 0
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_4_2_SUPPORT) && RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if defined(RTK_BLE_5_0_SUPPORT) && RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          0
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         0
#define RTK_BLE_5_0_PA_ADV_SUPPORT         (0 && RTK_BLE_5_0_AE_ADV_SUPPORT)
#define RTK_BLE_5_0_PA_SYNC_SUPPORT        (0 && RTK_BLE_5_0_AE_SCAN_SUPPORT)
#endif /* RTK_BLE_5_0_SUPPORT */

#if defined(RTK_BLE_5_1_SUPPORT) && RTK_BLE_5_1_SUPPORT
#define RTK_BLE_5_1_PAST_SENDER_SUPPORT     0
#define RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT  0
#define RTK_BLE_5_1_CTE_SUPPORT             0
#endif /* RTK_BLE_5_1_SUPPORT */

#if defined(RTK_BLE_5_2_SUPPORT) && RTK_BLE_5_2_SUPPORT
#define RTK_BLE_5_2_POWER_CONTROL_SUPPORT   0
#define RTK_BT_5_2_EATT_SUPPORT             0
#define RTK_BT_5_2_L2C_ECFC_SUPPORT         (RTK_BT_5_2_EATT_SUPPORT)
#endif /* RTK_BLE_5_2_SUPPORT */

/*
 * PLATFORM_8735B (AmebaPro2)
 */
#elif defined(CONFIG_8735B) && CONFIG_8735B
#define RTK_BLE_GAP_MAX_LINKS               4
#define RTK_BLE_SUPPORT                     1

#define RTK_BLE_GATTS                       1
#define RTK_BLE_GATTC                       1
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 1
#define RTK_BLE_5_2_SUPPORT                 0
#define RTK_BLE_SMP_OOB_SUPPORT             1
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_4_2_SUPPORT) && RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if defined(RTK_BLE_5_0_SUPPORT) && RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          0
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         0
#endif /* RTK_BLE_5_0_SUPPORT */

/*
 * PLATFORM_AMEBADPLUS (AmebaDplus)
 */
#elif defined(CONFIG_AMEBADPLUS) && CONFIG_AMEBADPLUS
#define RTK_BLE_GAP_MAX_LINKS               4
#define RTK_BLE_SUPPORT                     1
#define RTK_BT_POWER_CONTROL_SUPPORT        1

#define RTK_BLE_GATTS                       1
#define RTK_BLE_GATTC                       1
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 1
#define RTK_BLE_5_2_SUPPORT                 0
#define RTK_BLE_SMP_OOB_SUPPORT             1
#define RTK_BLE_COC_SUPPORT                 0
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_4_2_SUPPORT) && RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if defined(RTK_BLE_5_0_SUPPORT) && RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          0
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         0
#endif /* RTK_BLE_5_0_SUPPORT */

/*
 * PLATFORM_AMEBAGREEN2 (AmebaGreen2)
 */
#elif defined(CONFIG_AMEBAGREEN2) && CONFIG_AMEBAGREEN2
#define RTK_BLE_GAP_MAX_LINKS               4
#define RTK_BLE_SUPPORT                     1
#define RTK_BT_POWER_CONTROL_SUPPORT        1

#define RTK_BLE_GATTS                       1
#define RTK_BLE_GATTC                       1
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define RTK_BLE_4_0_SUPPORT                 1
#define RTK_BLE_4_2_SUPPORT                 1
#define RTK_BLE_5_0_SUPPORT                 1
#define RTK_BLE_5_1_SUPPORT                 0
#define RTK_BLE_5_2_SUPPORT                 0
#define RTK_BLE_SMP_OOB_SUPPORT             1
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_4_2_SUPPORT) && RTK_BLE_4_2_SUPPORT
#define RTK_BLE_PRIVACY_SUPPORT             1
#define RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT    1
#endif /* RTK_BLE_4_2_SUPPORT */

#if defined(RTK_BLE_5_0_SUPPORT) && RTK_BLE_5_0_SUPPORT
#define RTK_BLE_5_0_SET_PHYS_SUPPORT        1
#define RTK_BLE_5_0_AE_ADV_SUPPORT          0
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         0
#endif /* RTK_BLE_5_0_SUPPORT */

/*
 * Error Platform
 */
#else
#error "Please choose a corret platform"
#endif

#if (defined(RTK_BLE_5_2_SUPPORT) && RTK_BLE_5_2_SUPPORT)
#if (defined(CONFIG_BT_ISO_TEST) && CONFIG_BT_ISO_TEST)
#define RTK_BLE_ISO_SUPPORT                 1
#endif
#if defined(CONFIG_BT_LE_AUDIO) && CONFIG_BT_LE_AUDIO
#define RTK_BLE_AUDIO_SUPPORT               1
#endif
#endif /* RTK_BLE_5_2_SUPPORT */

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#undef RTK_BLE_5_0_AE_ADV_SUPPORT
#undef RTK_BLE_5_0_AE_SCAN_SUPPORT
#undef RTK_BLE_5_0_PA_ADV_SUPPORT
#undef RTK_BLE_5_0_PA_SYNC_SUPPORT
#define RTK_BLE_5_0_AE_ADV_SUPPORT          1
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         1
#define RTK_BLE_5_0_PA_ADV_SUPPORT          1
#define RTK_BLE_5_0_PA_SYNC_SUPPORT         1
#endif

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#undef RTK_BLE_5_0_AE_ADV_SUPPORT
#undef RTK_BLE_5_0_AE_SCAN_SUPPORT
#undef RTK_BLE_MGR_LIB_EADV
#undef RTK_BLE_5_0_PA_ADV_SUPPORT
#undef RTK_BLE_5_0_PA_SYNC_SUPPORT
#define RTK_BLE_5_0_AE_ADV_SUPPORT          1
#define RTK_BLE_5_0_AE_SCAN_SUPPORT         1
#define RTK_BLE_MGR_LIB_EADV                1
#define RTK_BLE_5_0_PA_ADV_SUPPORT          0
#define RTK_BLE_5_0_PA_SYNC_SUPPORT         0
#endif

#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
#define RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT    0    //can set 1 when CAP Commander role
#define RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT      0    //can set 1 when CAP Acceptor role
#define RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT      0    //can set 1 when CAP Commander role
#define RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT          0    //can set 1 when CAP Acceptor role
#define RTK_BLE_AUDIO_AICS_SUPPORT                     0    //can set 1 when CAP Acceptor or Commander role
#define RTK_BLE_AUDIO_VOCS_SUPPORT                     0    //can set 1 when CAP Acceptor or Commander role
#define RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT  0    //not support now
#define RTK_BLE_AUDIO_CCP_CALL_CONTROL_CLIENT_SUPPORT  0    //not support now
#define RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT 0    //can set 1 when CAP Initiator role
#define RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT 0    //can set 1 when CAP Acceptor or Commander role
#define RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT     0    //can set 1 when CAP Initiator or Commander role
#define RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT          0    //can set 1 when CAP Acceptor role 
#endif

#if ((!defined(CONFIG_BT_ZEPHYR) || !CONFIG_BT_ZEPHYR) && \
    ((defined(RTK_BT_5_2_EATT_SUPPORT) && RTK_BT_5_2_EATT_SUPPORT) || (defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT)))
#define RTK_BLE_MGR_LIB             1
#define GATTC_TBL_STORAGE_SUPPORT   0
#else
#define RTK_BLE_MGR_LIB             0
#endif

#if (defined(CONFIG_BT_ISO_TEST) && CONFIG_BT_ISO_TEST) && (defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT) && \
    ((!defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT) || (!defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) || !RTK_BLE_5_0_AE_SCAN_SUPPORT) || \
     (!defined(RTK_BLE_5_0_PA_ADV_SUPPORT) || !RTK_BLE_5_0_PA_ADV_SUPPORT) || (!defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) || !RTK_BLE_5_0_PA_SYNC_SUPPORT))
#error "Please enable AE, AE Scan, PA, PA Sync for corret platform when enable ISO DEMO"
#endif

#if (defined(CONFIG_BT_LE_AUDIO) && CONFIG_BT_LE_AUDIO) && (defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT) && \
    ((!defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT) || (!defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) || !RTK_BLE_5_0_AE_SCAN_SUPPORT))
#error "Please enable AE, AE Scan for corret platform when enable LE AUDIO"
#endif

#if (defined(CONFIG_AMEBASMART) && CONFIG_AMEBASMART) && defined(__ICCARM__) && (defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT)
#undef RTK_BREDR_SUPPORT
#define RTK_BREDR_SUPPORT  0
#endif

#if defined(CONFIG_BT_MESH_PROVISIONER_SUPPORT) && CONFIG_BT_MESH_PROVISIONER_SUPPORT
#define RTK_BLE_MESH_SUPPORT                1
#define RTK_BLE_MESH_PROVISIONER_SUPPORT    1
#define RTK_BLE_MESH_FN_SUPPORT             1
#else
#define RTK_BLE_MESH_PROVISIONER_SUPPORT    0
#endif
#if defined(CONFIG_BT_MESH_DEVICE_SUPPORT) && CONFIG_BT_MESH_DEVICE_SUPPORT
#define RTK_BLE_MESH_SUPPORT                1
#define RTK_BLE_MESH_DEVICE_SUPPORT         1
#define RTK_BLE_MESH_FN_SUPPORT             1
#define RTK_BLE_MESH_LPN_SUPPORT            1
#else
#define RTK_BLE_MESH_DEVICE_SUPPORT         0
#endif
#if (defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT) && (defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT)
#error "Can not enable RTK_BLE_MESH_PROVISIONER_SUPPORT and RTK_BLE_MESH_DEVICE_SUPPORT at same time"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BT_API_CONFIG_H__ */