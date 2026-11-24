/**
 * @file      bt_vendor_config.h
 * @author    ryan_wang@realsil.com.cn
 * @brief     Bluetooth platform vendor cmd config macros definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_VENDOR_CONFIG_H__
#define __BT_VENDOR_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "platform_autoconf.h"

/*
 * AmebaSmart
 */
#if defined(CONFIG_AMEBASMART) && CONFIG_AMEBASMART
#define VENDOR_CMD_READ_BT_REGISTER_SUPPORT               1
#if defined(VENDOR_CMD_READ_BT_REGISTER_SUPPORT) && VENDOR_CMD_READ_BT_REGISTER_SUPPORT
#define VENDOR_CMD_READ_BT_REGISTER_OPCODE                0xFC61
#define VENDOR_CMD_READ_PI_BT_RF_REGISTER_OPCODE          0xFD49
#define VENDOR_CMD_READ_BT_RF_REGISTER_OPCODE             0xFD4B
#endif

#define VENDOR_CMD_ONE_SHOT_ADV_SUPPORT                   1
#if defined(VENDOR_CMD_ONE_SHOT_ADV_SUPPORT) && VENDOR_CMD_ONE_SHOT_ADV_SUPPORT
#define VENDOR_CMD_ONE_SHOT_ADV_OPCODE                    0xFC87
#endif

/**
 * @def       VENDOR_CMD_SET_TX_POWER_SUPPORT
 * @brief     Enable the vendor command to set the advertising TX power or the connection TX power.
 * @note
 *            - 1: Enabled. 0: Disabled.
 *            - Enabled by default on AmebaSmart, AmebaLite, AmebaDplus and AmebaGreen2. Not available on the other SoCs.
 *            - If disabled, @ref rtk_bt_set_tx_power fails.
 */
#define VENDOR_CMD_SET_TX_POWER_SUPPORT                   1

/**
 * @def       VENDOR_CMD_READ_ACC_HIT_COUNTER_SUPPORT
 * @brief     Enable the vendor command to read the ACC hit counter, which is used to get the connection clock information
 *            of an ACL link where the local device is the peripheral.
 * @note
 *            - 1: Enabled. 0: Disabled.
 *            - Disabled by default on AmebaSmart. Not available on the other SoCs.
 *            - If disabled, @ref rtk_bt_le_get_slave_conn_clock fails.
 */
#define VENDOR_CMD_READ_ACC_HIT_COUNTER_SUPPORT           0
#if ((defined(VENDOR_CMD_SET_TX_POWER_SUPPORT) && VENDOR_CMD_SET_TX_POWER_SUPPORT) || \
    (defined(VENDOR_CMD_READ_ACC_HIT_COUNTER_SUPPORT) && VENDOR_CMD_READ_ACC_HIT_COUNTER_SUPPORT))
#define VENDOR_CMD_LE_EXTENSION_FEATURE_OPCODE            0xFD80
#endif

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                  1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                   0xFCA6
#endif

/**
 * @def       VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT
 * @brief     Enable the vendor command to latch the free run clock of the BT controller.
 * @note
 *            - 1: Enabled. 0: Disabled.
 *            - Enabled by default on AmebaSmart and AmebaLite. Not available on the other SoCs.
 *            - If disabled, @ref rtk_bt_get_hc_clock_offset fails.
 */
#define VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT           1
#if defined(VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT) && VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT
#define VENDOR_CMD_FREE_RUN_CLOCK_LATCH_OPCODE            0xFDD2
#endif

/**
 * @def       VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT
 * @brief     Enable the vendor command to get the CIG or BIG reference anchor point information of an ISO channel.
 * @note
 *            - 1: Enabled. 0: Disabled.
 *            - Disabled by default on AmebaSmart. Not available on the other SoCs.
 *            - If disabled, @ref rtk_bt_get_le_iso_sync_ref_ap_info fails.
 */
#define VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT    0
#if defined(VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT) && VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT
#define VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_OPCODE     0xFDD3
#endif

/*
 * AmebaLite
 */
#elif defined(CONFIG_AMEBALITE) && CONFIG_AMEBALITE
#define VENDOR_CMD_READ_BT_REGISTER_SUPPORT               1
#if defined(VENDOR_CMD_READ_BT_REGISTER_SUPPORT) && VENDOR_CMD_READ_BT_REGISTER_SUPPORT
#define VENDOR_CMD_READ_BT_REGISTER_OPCODE                0xFC61
#define VENDOR_CMD_READ_PI_BT_RF_REGISTER_OPCODE          0xFD49
#define VENDOR_CMD_READ_BT_RF_REGISTER_OPCODE             0xFD4B
#endif

#define VENDOR_CMD_ONE_SHOT_ADV_SUPPORT                   1
#if defined(VENDOR_CMD_ONE_SHOT_ADV_SUPPORT) && VENDOR_CMD_ONE_SHOT_ADV_SUPPORT
#define VENDOR_CMD_ONE_SHOT_ADV_OPCODE                    0xFC87
#endif

#define VENDOR_CMD_SET_TX_POWER_SUPPORT                   1
#if defined(VENDOR_CMD_SET_TX_POWER_SUPPORT) && VENDOR_CMD_SET_TX_POWER_SUPPORT
#define VENDOR_CMD_LE_EXTENSION_FEATURE_OPCODE            0xFD80
#endif

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                  1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                   0xFCA6
#endif

#define VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT           1
#if defined(VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT) && VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT
#define VENDOR_CMD_FREE_RUN_CLOCK_LATCH_OPCODE            0xFDD2
#endif

/*
 * AmebaDplus
 */
#elif defined(CONFIG_AMEBADPLUS) && CONFIG_AMEBADPLUS
#define VENDOR_CMD_READ_BT_REGISTER_SUPPORT              1
#if defined(VENDOR_CMD_READ_BT_REGISTER_SUPPORT) && VENDOR_CMD_READ_BT_REGISTER_SUPPORT
#define VENDOR_CMD_READ_BT_REGISTER_OPCODE               0xFC61
#define VENDOR_CMD_READ_PI_BT_RF_REGISTER_OPCODE         0xFD49
#define VENDOR_CMD_READ_BT_RF_REGISTER_OPCODE            0xFD4B
#endif

#define VENDOR_CMD_ONE_SHOT_ADV_SUPPORT                  1
#if defined(VENDOR_CMD_ONE_SHOT_ADV_SUPPORT) && VENDOR_CMD_ONE_SHOT_ADV_SUPPORT
#define VENDOR_CMD_ONE_SHOT_ADV_OPCODE                   0xFC87
#endif

#define VENDOR_CMD_SET_TX_POWER_SUPPORT                  1
#define VENDOR_CMD_LE_EXTENSION_FEATURE_SUPPORT          0
#if (defined(VENDOR_CMD_SET_TX_POWER_SUPPORT) && VENDOR_CMD_SET_TX_POWER_SUPPORT) || (defined(VENDOR_CMD_LE_EXTENSION_FEATURE_SUPPORT) && VENDOR_CMD_LE_EXTENSION_FEATURE_SUPPORT)
#define VENDOR_CMD_LE_EXTENSION_FEATURE_OPCODE           0xFD80
#endif

#define VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_SUPPORT    0
#if defined(VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_SUPPORT) && VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_SUPPORT
#define VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_OPCODE     0xFD82
#endif

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                 1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                  0xFCA6
#endif

/*
 * AmebaGreen2
 */
#elif defined(CONFIG_AMEBAGREEN2) && CONFIG_AMEBAGREEN2
#define VENDOR_CMD_READ_BT_REGISTER_SUPPORT         1
#if defined(VENDOR_CMD_READ_BT_REGISTER_SUPPORT) && VENDOR_CMD_READ_BT_REGISTER_SUPPORT
#define VENDOR_CMD_READ_BT_REGISTER_OPCODE          0xFC61
#define VENDOR_CMD_READ_PI_BT_RF_REGISTER_OPCODE    0xFD49
#define VENDOR_CMD_READ_BT_RF_REGISTER_OPCODE       0xFD4B
#endif

#define VENDOR_CMD_ONE_SHOT_ADV_SUPPORT                  1
#if defined(VENDOR_CMD_ONE_SHOT_ADV_SUPPORT) && VENDOR_CMD_ONE_SHOT_ADV_SUPPORT
#define VENDOR_CMD_ONE_SHOT_ADV_OPCODE                   0xFC87
#endif

#define VENDOR_CMD_SET_TX_POWER_SUPPORT             1
#if defined(VENDOR_CMD_SET_TX_POWER_SUPPORT) && VENDOR_CMD_SET_TX_POWER_SUPPORT
#define VENDOR_CMD_LE_EXTENSION_FEATURE_OPCODE      0xFD80
#endif

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                  1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                   0xFCA6
#endif

/*
 * Ameba 8720F
 */
#elif defined(CONFIG_RTL8720F) && CONFIG_RTL8720F

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                 1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                  0xFCA6
#endif

/*
 * RLE1509
 */
#elif defined(CONFIG_RLE1509) && CONFIG_RLE1509

#define VENDOR_CMD_SET_MESH_INFO_SUPPORT                 1
#if defined(VENDOR_CMD_SET_MESH_INFO_SUPPORT) && VENDOR_CMD_SET_MESH_INFO_SUPPORT
#define VENDOR_CMD_SET_MESH_INFO_OPCODE                  0xFCA6
#endif

/*
 * AmebaPro3
 */
#elif defined(CONFIG_AMEBAPRO3) && CONFIG_AMEBAPRO3
#define VENDOR_CMD_READ_BT_REGISTER_SUPPORT         1
#if defined(VENDOR_CMD_READ_BT_REGISTER_SUPPORT) && VENDOR_CMD_READ_BT_REGISTER_SUPPORT
#define VENDOR_CMD_READ_BT_REGISTER_OPCODE          0xFC61
#define VENDOR_CMD_READ_PI_BT_RF_REGISTER_OPCODE    0xFD49
#define VENDOR_CMD_READ_BT_RF_REGISTER_OPCODE       0xFD4B
#endif

/*
 * Error Platform
 */
#else
#error "Please choose a correct platform"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BT_VENDOR_CONFIG_H__ */
