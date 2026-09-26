/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_CONFIG_H
#define ZEPHYR_CONFIG_H

#include <bt_api_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The code that fix bugs or add additional function for zephyr stack. Do not change it! */
#define ZEPHYR_RTK_PATCH 1

/* Note: If you want to enable a macro that is used in zephyr stack, please use
 * "#define SAMPLE_MACRO 1". And when disable a marco, please remove it directly,
 * it's not suggested to use "#define SAMPLE_MACRO 0" to disable macro.
 */

#define CONFIG_LITTLE_ENDIAN 1

/*------------------------------ BT host buffer -----------------------------*/
#define CONFIG_BT_BUF_CMD_TX_COUNT 5
#define CONFIG_BT_BUF_CMD_TX_SIZE 255

#define CONFIG_BT_BUF_ACL_RX_COUNT 8  /* Increase it to fix acl flow pending, ref RSWLAND-1587 Q12 */
#define CONFIG_BT_BUF_ACL_RX_SIZE 266

#define CONFIG_BT_BUF_EVT_RX_COUNT 8  /* Increase it to fix deadlock problem, ref RSWLAND-1587 Q4 */
#define CONFIG_BT_BUF_EVT_RX_SIZE 255

#define CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT 8
#define CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE 255

/*------------------------------- BT trx task -------------------------------*/
#define CONFIG_BT_RECV_WORKQ_BT 1

#define CONFIG_BT_RX_PRIO 4
#define CONFIG_BT_RX_STACK_SIZE 2048

#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY 4
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE 2048

/*--------------------------------- BT role ---------------------------------*/
#define CONFIG_BT_BROADCASTER 1
#define CONFIG_BT_OBSERVER 1
#define CONFIG_BT_PERIPHERAL 1
#define CONFIG_BT_CENTRAL 1

/*-------------------------- BT host configuration --------------------------*/
#define CONFIG_BT_DEVICE_NAME "Ameba"
#define CONFIG_BT_DEVICE_NAME_MAX 248
#define CONFIG_BT_DEVICE_NAME_DYNAMIC 1

#define CONFIG_BT_DEVICE_APPEARANCE 0
#define CONFIG_BT_DEVICE_APPEARANCE_DYNAMIC 1

#define CONFIG_BT_SETTINGS 1
#define CONFIG_BT_SETTINGS_USE_PRINTK 1

#define CONFIG_BT_ID_MAX 1
/* bt_rand() from local HMAC-DRBG instead of a synchronous HCI LE_RAND,
 * which deadlocked the RX-drain thread once a burst had drained evt_pool */
#define CONFIG_BT_HOST_CRYPTO_PRNG 1

/*----------------------------- BT adv and scan -----------------------------*/
#define CONFIG_BT_FILTER_ACCEPT_LIST 1
#define CONFIG_BT_LIM_ADV_TIMEOUT 60
#define CONFIG_BT_SCAN_WITH_IDENTITY 1 /* to enable directed advertiser reports */
#define CONFIG_BT_SCAN_AND_INITIATE_IN_PARALLEL 1
#define CONFIG_BT_BACKGROUND_SCAN_INTERVAL 2048
#define CONFIG_BT_BACKGROUND_SCAN_WINDOW 18

/*--------------------------------- BT conn ---------------------------------*/
#define CONFIG_BT_CONN 1
#define CONFIG_BT_MAX_CONN    RTK_BLE_GAP_MAX_LINKS
#define CONFIG_BT_MAX_PAIRED    CONFIG_BT_MAX_CONN
#define CONFIG_BT_CONN_TX 1
#define CONFIG_BT_CONN_TX_MAX 4
#define CONFIG_BT_CONN_TX_USER_DATA_SIZE 16
/* Open flow control can help prevent too much ACL rx data, it's better not close it.
 * ref RSWLAND-1587 Q7 and RSWLANDIOT-16535 */
#define CONFIG_BT_HCI_ACL_FLOW_CONTROL 1
#define CONFIG_BT_CONN_FRAG_COUNT 4
#define CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT 5000  /* in millisecond */
#define CONFIG_BT_CREATE_CONN_TIMEOUT 10 /* in sencond */
#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
#define CONFIG_BT_DATA_LEN_UPDATE 1
#define CONFIG_BT_USER_DATA_LEN_UPDATE 1
#endif
#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
#define CONFIG_BT_PHY_UPDATE 1
#define CONFIG_BT_USER_PHY_UPDATE 1
#endif
// #define CONFIG_BT_REMOTE_VERSION 1

/*--------------------------------- BT l2cap --------------------------------*/
#define CONFIG_BT_L2CAP_TX_MTU 262
#define CONFIG_BT_L2CAP_TX_BUF_COUNT 8
#define CONFIG_BT_L2CAP_TX_FRAG_COUNT 0

/*--------------------------------- BT gatt ---------------------------------*/
#define CONFIG_BT_ATT_TX_COUNT 5
#define CONFIG_BT_ATT_PREPARE_COUNT 4
#define CONFIG_BT_GATT_AUTO_UPDATE_MTU 1
#define CONFIG_BT_GATT_SERVICE_CHANGED 1
#define CONFIG_BT_GATT_DYNAMIC_DB 1
#define CONFIG_BT_DEVICE_NAME_GATT_WRITABLE 1
#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
#define CONFIG_BT_GATT_CLIENT 1
#endif

/*---------------------------- BT smp and privacy ---------------------------*/
#define CONFIG_BT_SMP 1
#define CONFIG_BT_SMP_MIN_ENC_KEY_SIZE 7
#define CONFIG_BT_BONDABLE 1
#define CONFIG_BT_FIXED_PASSKEY 1
#define CONFIG_BT_ECC 1 /* to support secure connection pairing */
#define CONFIG_BT_MBEDTLS_ECC 1
#if defined(CONFIG_BT_SMP) && defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
#define CONFIG_BT_PRIVACY 1
#define CONFIG_BT_RPA_TIMEOUT 900
#endif

/*------------------------------ system related -----------------------------*/
#define CONFIG_ASSERT_NO_COND_INFO 1
#define CONFIG_BT_ASSERT 1
#define CONFIG_BT_ASSERT_VERBOSE 1
#define CONFIG_LOG 1
#define CONFIG_BT_LOG_SNIFFER_INFO 1
#define CONFIG_HEAP_MEM_POOL_SIZE 5

/*----------------------------- BT extended adv -----------------------------*/
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
#define CONFIG_BT_EXT_ADV 1
#define CONFIG_BT_EXT_ADV_MAX_ADV_SET 3
#define CONFIG_BT_EXT_SCAN_BUF_SIZE 1650
/* Permit to choose legacy adv hci cmd if the ext adv feature bit is not set, even if
 * CONFIG_BT_EXT_ADV is enabled. */
#define CONFIG_BT_EXT_ADV_LEGACY_SUPPORT 1
#endif

/*----------------------------- BT periodic adv -----------------------------*/
#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
#define CONFIG_BT_PER_ADV 1
#endif

/*--------------------------- BT periodic adv sync --------------------------*/
#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
#define CONFIG_BT_PER_ADV_SYNC 1
#define CONFIG_BT_PER_ADV_SYNC_MAX 3
#define CONFIG_BT_PER_ADV_SYNC_BUF_SIZE 1650
#endif

/*------------------------------ BT past sender -----------------------------*/
#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
#define CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER 1
#endif

/*---------------------------- BT past recipient ----------------------------*/
#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
#define CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER 1
#endif

/*---------------------------------- BT cte ---------------------------------*/
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
#define CONFIG_BT_DF_CONNECTION_CTE_REQ    1
#define CONFIG_BT_DF_CONNECTION_CTE_RSP    1
#define CONFIG_BT_DF_CONNECTION_CTE_TX     1
#define CONFIG_BT_DF_CONNECTION_CTE_RX     1
#define CONFIG_BT_DF_CONNECTIONLESS_CTE_RX 1
#endif

/*------------------------------- BT padv rsp -------------------------------*/
#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
#define CONFIG_BT_PER_ADV_RSP   1
#endif

/*----------------------------- BT padv sync rsp ----------------------------*/
#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
#define CONFIG_BT_PER_ADV_SYNC_RSP   1
#endif

#ifdef __cplusplus
}
#endif

#endif
