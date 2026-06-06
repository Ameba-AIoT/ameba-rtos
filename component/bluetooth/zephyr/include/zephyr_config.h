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

#define ZEPHYR_FIX_CODE 1  /* the code that fix the bug or add additional function for zephyr stack */

#define CONFIG_LITTLE_ENDIAN

//osif configure
#define CONFIG_RISCV

//HCI
// #define CONFIG_BT_RECV_BLOCKING
#define CONFIG_BT_RECV_WORKQ_BT
#define CONFIG_BT_RX_PRIO 5
#define CONFIG_BT_RX_STACK_SIZE 4096

#define CONFIG_BT_HCI_TX_PRIO 6
#define CONFIG_BT_HCI_TX_STACK_SIZE 2048

#define CONFIG_BT_LONG_WQ_PRIO 4
#define CONFIG_BT_LONG_WQ_STACK_SIZE 4096

//host Stack
#define CONFIG_BT_DEVICE_APPEARANCE 0
#define CONFIG_BT_DEVICE_NAME "Ameba"
#define CONFIG_BT_ID_MAX 1

#define CONFIG_BT_HCI_RESERVE 1
#define CONFIG_BT_BUF_CMD_TX_COUNT 5
#define CONFIG_BT_BUF_CMD_TX_SIZE 255

#define CONFIG_BT_BUF_ACL_RX_COUNT 8  /* Increase it to fix acl flow pending, ref RSWLAND-1587 Q12 */
#define CONFIG_BT_BUF_ACL_RX_SIZE 255

#define CONFIG_BT_BUF_EVT_RX_COUNT 8  /* Increase it to fix deadlock problem, ref RSWLAND-1587 Q4 */
#define CONFIG_BT_BUF_EVT_RX_SIZE 255

#define CONFIG_BT_BUF_EVT_DISCARDABLE_COUNT 8
#define CONFIG_BT_BUF_EVT_DISCARDABLE_SIZE 255

#define CONFIG_BT_L2CAP_TX_BUF_COUNT 8
#define CONFIG_BT_ATT_TX_COUNT 5
#define CONFIG_BT_L2CAP_TX_MTU 252

#define CONFIG_BT_SCAN_AND_INITIATE_IN_PARALLEL

//bredr related
#if 0//defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#define CONFIG_BT_CLASSIC 1

#define CONFIG_BT_MAX_SCO_CONN 1
#define CONFIG_BT_L2CAP_DYNAMIC_CHANNEL 1
#define CONFIG_BT_L2CAP_RESCHED_MS  5
#define CONFIG_BT_PAGE_TIMEOUT  0x4000
#define CONFIG_BT_COD (0x18 | (0x04 << 8) | (0x24 << 16))

#define CONFIG_BT_AVDTP 1
#define CONFIG_BT_AVCTP 1
#define CONFIG_BT_A2DP 1
#define CONFIG_BT_A2DP_SINK 1
#define CONFIG_BT_A2DP_SOURCE 1
#define CONFIG_BT_AVRCP 1
#define CONFIG_BT_RFCOMM 1
#define CONFIG_BT_SPP 1

#define CONFIG_BT_RFCOMM_L2CAP_MTU  252
#define CONFIG_BT_AVDTP_TX_MTU  512
#define CONFIG_BT_SCO_TX_BUF_COUNT  5
#define CONFIG_BT_SCO_TX_MTU  200
#define CONFIG_BT_SCO_RX_BUF_COUNT  5
#define CONFIG_BT_SCO_RX_MTU  200
#define CONFIG_BT_HFP_AG_TX_BUF_COUNT 5
#define CONFIG_BT_HFP_AG_PHONE_NUMBER_MAX_LEN 20
#define CONFIG_BT_HFP_AG_INCOMING_TIMEOUT 30
#define CONFIG_BT_HFP_AG_OUTGOING_TIMEOUT 30
#define CONFIG_BT_HFP_AG_ALERTING_TIMEOUT 30
#define CONFIG_BT_HFP_AG_RING_NOTIFY_INTERVAL 3
#define CONFIG_BT_HFP_AG_THREAD_PRIO  4

#define CONFIG_BT_SCO_USE_TX_THREAD 0
#define CONFIG_BT_INQUIRY_RESULT_COUNT  10
#define CONFIG_BT_HCI_ACL_FLOW_CONTROL 1  /* Open flow control for A2DP media data recv, ref RSWLAND-1587 Q7 */
#endif /* RTK_BREDR_SUPPORT */

//host configuration
#define CONFIG_BT_BROADCASTER 1
#define CONFIG_BT_OBSERVER 1
#define CONFIG_BT_PERIPHERAL 1
#define CONFIG_BT_CENTRAL 1

#define CONFIG_BT_SMP 1
#define CONFIG_BT_BONDABLE 1
#define CONFIG_BT_FIXED_PASSKEY 1
/* to support secure connection */
#define CONFIG_BT_ECC 1
#define CONFIG_BT_TINYCRYPT_ECC 1
#define CONFIG_BT_DEVICE_NAME_DYNAMIC 1
#define CONFIG_BT_DEVICE_APPEARANCE_DYNAMIC 1

#define CONFIG_BT_FILTER_ACCEPT_LIST 1
#define CONFIG_BT_CONN 1
#define CONFIG_BT_CONN_TX 1

#define CONFIG_BT_DATA_LEN_UPDATE 1
#define CONFIG_BT_USER_DATA_LEN_UPDATE 1

#define CONFIG_BT_PHY_UPDATE 1
#define CONFIG_BT_USER_PHY_UPDATE 1

/* to enable directed advertiser reports */
#if !defined(CONFIG_BT_PRIVACY)
#define CONFIG_BT_SCAN_WITH_IDENTITY 1
#endif

//profile
#define CONFIG_BT_GATT_CLIENT 1
#define CONFIG_BT_GATT_AUTO_UPDATE_MTU 1

#define CONFIG_BT_MAX_CONN  8

#define CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT 5000
#define CONFIG_BT_CREATE_CONN_TIMEOUT 60

//host parameter
#define CONFIG_BT_CONN_TX_USER_DATA_SIZE 16
#define CONFIG_BT_CONN_FRAG_COUNT 4
#define CONFIG_BT_CONN_TX_MAX 4

#define CONFIG_BT_BACKGROUND_SCAN_INTERVAL 2048
#define CONFIG_BT_BACKGROUND_SCAN_WINDOW 18

#define CONFIG_BT_LIM_ADV_TIMEOUT 60

#define CONFIG_BT_DEVICE_NAME_MAX 248

#define CONFIG_BT_SMP_MIN_ENC_KEY_SIZE 7

//GATT
#define CONFIG_BT_MAX_PAIRED CONFIG_BT_MAX_CONN

#define CONFIG_BT_PER_ADV_SYNC_BUF_SIZE 1650

#define CONFIG_BT_CTLR_ADV_DATA_LEN_MAX 1650
#define CONFIG_BT_CTLR_SCAN_DATA_LEN_MAX 1650

#define CONFIG_NET_BUF_WARN_ALLOC_INTERVAL 0

#define CONFIG_BT_L2CAP_TX_FRAG_COUNT 0

#define CONFIG_BT_ATT_PREPARE_COUNT 3

//assert
#define CONFIG_BT_ASSERT_VERBOSE
#define CONFIG_ASSERT_NO_COND_INFO
#define CONFIG_BT_ASSERT
#ifdef CONFIG_BT_ASSERT
#define __ASSERT_ON 1
#else
#define __ASSERT_ON 0
#endif

#define CONFIG_LOG 1

#define CONFIG_BT_GATT_SERVICE_CHANGED 1
#define CONFIG_BT_GATT_DYNAMIC_DB 1

//ftl
#define CONFIG_BT_SETTINGS 1
#define CONFIG_BT_SETTINGS_USE_PRINTK 1
#define CONFIG_SETTINGS_NVS_NAME_CACHE 0

// #define CONFIG_BT_REMOTE_VERSION 1
#define CONFIG_HEAP_MEM_POOL_SIZE 5

//BT features configuration
//privacy
#define CONFIG_BT_PRIVACY 1
#define CONFIG_BT_RPA_TIMEOUT 600

//extend adv
#define CONFIG_BT_EXT_ADV 1
#define CONFIG_BT_EXT_ADV_MAX_ADV_SET 3
#define CONFIG_BT_EXT_SCAN_BUF_SIZE 1650
/* Permit to choose legacy adv hci cmd if the ext adv feature bit is not set,
even if CONFIG_BT_EXT_ADV is enabled */
#define CONFIG_BT_EXT_ADV_LEGACY_SUPPORT 1

#if 0
//periodic adv
#define CONFIG_BT_PER_ADV 1

//periodic adv sync
#define CONFIG_BT_PER_ADV_SYNC 1
#define CONFIG_BT_PER_ADV_SYNC_MAX 3

//past sender
#define CONFIG_BT_PER_ADV_SYNC_TRANSFER_SENDER 1

//past recipient
#define CONFIG_BT_PER_ADV_SYNC_TRANSFER_RECEIVER 1

//cte
#define CONFIG_BT_DF_CONNECTION_CTE_REQ    1
#define CONFIG_BT_DF_CONNECTION_CTE_RSP    1
#define CONFIG_BT_DF_CONNECTION_CTE_TX     1
#define CONFIG_BT_DF_CONNECTION_CTE_RX     1
#define CONFIG_BT_DF_CONNECTIONLESS_CTE_RX 1

//padv rsp
#define CONFIG_BT_PER_ADV_RSP   1

//padv sync rsp
#define CONFIG_BT_PER_ADV_SYNC_RSP   1
#endif

#ifdef __cplusplus
}
#endif

#endif
