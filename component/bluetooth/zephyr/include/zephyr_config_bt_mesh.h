/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __ZEPHYR_CONFIG_BT_MESH_H__
#define __ZEPHYR_CONFIG_BT_MESH_H__

#include <bt_api_config.h>
#include <zephyr_config.h>
#include <rtk_bt_mesh_def.h>

// Macro for add code to fix bug in zephyr mesh stack code
#define CONFIG_BT_MESH_ZEPHYR_FIX    1

#if defined(CONFIG_BT_MESH_PROVISIONER_SUPPORT) && CONFIG_BT_MESH_PROVISIONER_SUPPORT
#endif

#if defined(CONFIG_BT_MESH_DEVICE_SUPPORT) && CONFIG_BT_MESH_DEVICE_SUPPORT
#define CONFIG_BT_MESH_GATT_SERVER    1
#endif

// #define CONFIG_BT_MESH_PROXY    0  // maybe used in future
// #define CONFIG_BT_MESH_GATT    0  // maybe used in future
#define CONFIG_BT_MESH_PROXY_MSG_LEN    66
#define CONFIG_BT_MESH_GATT_CLIENT    1
#define CONFIG_BT_MESH_PB_GATT_COMMON    1

/* ------------------------ Advertiser menu ---------------------------------*/
#define CONFIG_BT_MESH_ADV_LEGACY    1  // if enabled, use legacy adv for mesh message
#if defined(CONFIG_BT_EXT_ADV) && CONFIG_BT_EXT_ADV
#define CONFIG_BT_MESH_ADV_EXT    0  // if enabled, use extended adv for mesh message
#endif

#if (defined(CONFIG_BT_MESH_ADV_LEGACY) && CONFIG_BT_MESH_ADV_LEGACY) && (defined(CONFIG_BT_MESH_ADV_EXT) && CONFIG_BT_MESH_ADV_EXT)
#error "Can not enable legacy ADV and extended ADV for mesh at same time"
#endif
#if !((defined(CONFIG_BT_MESH_ADV_LEGACY) && CONFIG_BT_MESH_ADV_LEGACY) || (defined(CONFIG_BT_MESH_ADV_EXT) && CONFIG_BT_MESH_ADV_EXT))
#error "Should enable only one of CONFIG_BT_MESH_ADV_LEGACY and CONFIG_BT_MESH_ADV_EXT"
#endif

// Legacy ADV
#if CONFIG_BT_MESH_ADV_LEGACY
#define CONFIG_BT_MESH_ADV_STACK_SIZE    2048  // default is 768 or 776 if CONFIG_BT_MESH_PRIV_BEACONS or 1024 if CONFIG_BT_HOST_CRYPTO
#define CONFIG_BT_MESH_ADV_PRIO    4  // default is 7
#endif  // CONFIG_BT_MESH_ADV_LEGACY

// Extended ADV
#if defined(CONFIG_BT_MESH_ADV_EXT) && CONFIG_BT_MESH_ADV_EXT
#define CONFIG_BT_MESH_RELAY_ADV_SETS    0  // range:0 to CONFIG_BT_EXT_ADV_MAX_ADV_SET
#if CONFIG_BT_MESH_RELAY_ADV_SETS > 0
// Notice:can not define this macro, will cause send UDB fail in bt_mesh_adv_buf_get_by_tag(); the newest zephyr repo code have fixed this problem
#undef CONFIG_BT_MESH_ADV_EXT_RELAY_USING_MAIN_ADV_SET
#endif

#if defined(CONFIG_BT_MESH_GATT_SERVER) && CONFIG_BT_MESH_GATT_SERVER
#define CONFIG_BT_MESH_ADV_EXT_GATT_SEPARATE
#endif

#define CONFIG_BT_MESH_ADV_EXT_FRIEND_SEPARATE  0
#endif  // CONFIG_BT_MESH_ADV_EXT

#define CONFIG_BT_MESH_ADV_BUF_COUNT    32  // range:1~256, the default value 6 is not big when execute mesh OTA
#define CONFIG_BT_MESH_DEBUG_USE_ID_ADDR    1
/* ------------------------ end Advertiser menu ---------------------------------*/

/*------------------------ Provisioning menu --------------------------*/
#define CONFIG_BT_MESH_PB_ADV    1
#define CONFIG_BT_MESH_UNPROV_BEACON_INT    5  // range 1~10 in seconds
#if CONFIG_BT_MESH_PB_ADV
#define CONFIG_BT_MESH_PROV  1
#if defined(CONFIG_BT_MESH_RELAY_ADV_SETS) && CONFIG_BT_MESH_RELAY_ADV_SETS > 0
#define CONFIG_BT_MESH_PB_ADV_USE_RELAY_SETS  0
#endif
#define CONFIG_BT_MESH_PB_ADV_TRANS_PDU_RETRANSMIT_COUNT  0  // range:0~7, 7 if BT_MESH_PB_ADV_USE_RELAY_SETS
#define CONFIG_BT_MESH_PB_ADV_TRANS_ACK_RETRANSMIT_COUNT    2  // range:0~7
#define CONFIG_BT_MESH_PB_ADV_LINK_CLOSE_RETRANSMIT_COUNT   2  // range 0 7, 7 if BT_MESH_PB_ADV_USE_RELAY_SETS
#define CONFIG_BT_MESH_PB_ADV_RETRANS_TIMEOUT    200  // range 100 800 Timeout value of retransmit provisioning PDUs
#endif  // CONFIG_BT_MESH_PB_ADV

#if defined(CONFIG_BT_CONN) && CONFIG_BT_CONN
#define CONFIG_BT_MESH_PB_GATT    1
#if CONFIG_BT_MESH_PB_GATT
#define CONFIG_BT_MESH_PB_GATT_USE_DEVICE_NAME    1
#endif
#if defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL
#define CONFIG_BT_MESH_PB_GATT_CLIENT    1
#endif
#endif  // CONFIG_BT_CONN

#define CONFIG_BT_MESH_PROV_DEVICE  1
#if CONFIG_BT_MESH_PROV_DEVICE
#if CONFIG_BT_MESH_PB_ADV && CONFIG_BT_MESH_PB_GATT
#define CONFIG_BT_MESH_PROVISIONEE  1
#endif
#if defined(CONFIGBT_MESH_PROVISIONEE) && CONFIGBT_MESH_PROVISIONEE
#define CONFIG_BT_MESH_PROV_OOB_PUBLIC_KEY  1
#endif
#if defined(CONFIG_BT_MESH_PROV) && CONFIG_BT_MESH_PROV
#define CONFIG_BT_MESH_ECDH_P256_CMAC_AES128_AES_CCM    1  // Algorithm values in capability packet when provisioning;Represent FIPS P-256 Elliptic Curve for mesh spec 1.0 or BTM_ECDH_P256_CMAC_AES128_AES_CCM for mesh spec 1.1
#define CONFIG_BT_MESH_ECDH_P256_HMAC_SHA256_AES_CCM  0  // Maybe used in future
#if CONFIG_BT_MESH_ECDH_P256_HMAC_SHA256_AES_CCM
#define CONFIG_BT_MESH_OOB_AUTH_REQUIRED  0
#endif
#endif  // CONFIG_BT_MESH_PROV
#endif  // CONFIG_BT_MESH_PROV_DEVICE

#define CONFIG_BT_MESH_PROVISIONER    1
#define CONFIG_BT_MESH_CDB    1
#if CONFIG_BT_MESH_CDB
#define CONFIG_BT_MESH_CDB_NODE_COUNT    20  // default 8
#define CONFIG_BT_MESH_CDB_SUBNET_COUNT    1
#define CONFIG_BT_MESH_CDB_APP_KEY_COUNT    1
#endif
/* ------------------------ end Provisioning menu ---------------------------------*/

/*------------------------ Network layer menu --------------------------*/
#define CONFIG_BT_MESH_LOOPBACK_BUFS    3
#define CONFIG_BT_MESH_NETWORK_TRANSMIT_COUNT    BT_MESH_CONFIG_NET_TRANS_COUNTS  // should set to 4 when execute mesh OTA
#define CONFIG_BT_MESH_NETWORK_TRANSMIT_INTERVAL    ((BT_MESH_CONFIG_NET_TRANS_STEPS + 1) * 10)  // should >= 20ms in bt_data_send()
#define CONFIG_BT_MESH_MSG_CACHE_SIZE    128  // default 32
// Relay feature
#if defined(BT_MESH_CONFIG_RELAY_IS_ENABL) && BT_MESH_CONFIG_RELAY_IS_ENABL
#define CONFIG_BT_MESH_RELAY    1
#endif
#if defined(CONFIG_BT_MESH_RELAY) && CONFIG_BT_MESH_RELAY
#define CONFIG_BT_MESH_RELAY_ENABLED    1
#define CONFIG_BT_MESH_RELAY_RETRANSMIT_COUNT    BT_MESH_CONFIG_RELAY_RETRANS_COUNTS
#define CONFIG_BT_MESH_RELAY_RETRANSMIT_INTERVAL    ((BT_MESH_CONFIG_RELAY_RETRANS_STEPS + 1) * 10)  // should >= 20ms in bt_data_send()
#define CONFIG_BT_MESH_RELAY_BUF_COUNT    32
#endif  // CONFIG_BT_MESH_RELAY
/* ------------------------ end Network layer menu ---------------------------------*/

/*------------------------ Transport layer menu --------------------------*/
/*------------------------ Transport SAR configuration menu --------------------------*/
#define CONFIG_BT_MESH_TX_SEG_MSG_COUNT    3 // default 1, set 3 for zephyr remote provision
#define CONFIG_BT_MESH_RX_SEG_MSG_COUNT    3
#define CONFIG_BT_MESH_SEG_BUFS    64
#define CONFIG_BT_MESH_RX_SEG_MAX    3  // Maximum number of segments in incoming messages
#define CONFIG_BT_MESH_TX_SEG_MAX    3  // Maximum number of segments in outgoing messages
#define CONFIG_BT_MESH_SAR_TX_SEG_INT_STEP    5
#define CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_COUNT    BT_MESH_CONFIG_TRANS_RETRANS_COUNTS  // Should set to 6 when execute mesh OTA
#define CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_WITHOUT_PROG_COUNT    2
#define CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_STEP    7
#define CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_INC    1
#define CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_COUNT    BT_MESH_CONFIG_TRANS_RETRANS_COUNTS
#define CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_INT    9
#define CONFIG_BT_MESH_SAR_RX_SEG_THRESHOLD    3
#define CONFIG_BT_MESH_SAR_RX_ACK_DELAY_INC    1
#define CONFIG_BT_MESH_SAR_RX_SEG_INT_STEP    5
#define CONFIG_BT_MESH_SAR_RX_DISCARD_TIMEOUT    1
#define CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT    0
/* ------------------------ end Transport SAR configuration menu ---------------------------------*/
#define CONFIG_BT_MESH_DEFAULT_TTL    BT_MESH_CONFIG_MSG_TTL
/*------------------------ Replay Protection List menu --------------------------*/
#define CONFIG_BT_MESH_CRPL    10
#define CONFIG_BT_MESH_RPL_STORAGE_MODE_SETTINGS    1
/* ------------------------ end Replay Protection List menu ---------------------------------*/
/* ------------------------ end Transport layer menu ---------------------------------*/

/*------------------------ Access layer menu --------------------------*/
// #define CONFIG_BT_MESH_ACCESS_LAYER_MSG    1  // maybe used in future
// #define CONFIG_BT_MESH_MODEL_VND_MSG_CID_FORCE    1  // maybe used in future
#define CONFIG_BT_MESH_MODEL_EXTENSIONS  1
#define CONFIG_BT_MESH_COMP_PST_BUF_SIZE    100
// #define CONFIG_BT_MESH_LABEL_NO_RECOVER    1  // maybe used in future
#define CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG  1
#if defined(CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG) && CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG
#define CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG_CTX_ENABLED  1
#define CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG_COUNT  4
#define CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG_CHUNK_SIZE  10
#define CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG_CHUNK_COUNT  40
#endif
#define CONFIG_BT_MESH_DELAYABLE_PUBLICATION  1
/* ------------------------ end Access layer menu ---------------------------------*/

/*------------------------ Models menu --------------------------*/
#define CONFIG_BT_MESH_CFG_CLI    1
#if CONFIG_BT_MESH_CFG_CLI
#define CONFIG_BT_MESH_CFG_CLI_TIMEOUT    5000
#endif

#define CONFIG_BT_MESH_HEALTH_CLI    1
#if CONFIG_BT_MESH_HEALTH_CLI
#define CONFIG_BT_MESH_HEALTH_CLI_TIMEOUT  2000
#endif

#define CONFIG_BT_MESH_BLOB_SRV  1
#if CONFIG_BT_MESH_BLOB_SRV
#define CONFIG_BT_MESH_BLOB_SRV_PULL_REQ_COUNT  4
#define CONFIG_BT_MESH_BLOB_SIZE_MAX  1500000  // default:524288
#define CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN  4096
#define CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MAX  4096  // range BT_MESH_BLOB_BLOCK_SIZE_MIN ~ 1048576
#define CONFIG_BT_MESH_BLOB_REPORT_TIMEOUT  10
#endif

#define CONFIG_BT_MESH_BLOB_CLI  1
#if CONFIG_BT_MESH_BLOB_CLI
#define CONFIG_BT_MESH_BLOB_CLI_BLOCK_RETRIES  5
#endif

/*------------------------ BLOB models common configuration menu --------------------------*/
#if CONFIG_BT_MESH_BLOB_SRV || CONFIG_BT_MESH_BLOB_CLI
#define CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX  256
#endif
/* ------------------------ end BLOB models common configuration menu ---------------------------------*/

// #if (CONFIG_BT_MESH_BLOB_SRV || CONFIG_BT_MESH_BLOB_CLI) && CONFIG_FLASH_MAP
#if (CONFIG_BT_MESH_BLOB_SRV || CONFIG_BT_MESH_BLOB_CLI) && 0
#define CONFIG_BT_MESH_BLOB_IO_FLASH  1
#endif

#if CONFIG_BT_MESH_MODEL_EXTENSIONS && CONFIG_BT_MESH_BLOB_SRV
#define CONFIG_BT_MESH_DFU_SRV  1
#endif

#if CONFIG_BT_MESH_MODEL_EXTENSIONS && CONFIG_BT_MESH_BLOB_CLI
#define CONFIG_BT_MESH_DFU_CLI  1
#endif

/*------------------------ Firmware Update model configuration menu --------------------------*/
#define CONFIG_BT_MESH_DFU_FWID_MAXLEN  16
#define CONFIG_BT_MESH_DFU_METADATA_MAXLEN  32
#define CONFIG_BT_MESH_DFU_METADATA  1
#define CONFIG_BT_MESH_DFU_URI_MAXLEN  32
/* ------------------------ end Firmware Update model configuration menu ---------------------------------*/

#define CONFIG_BT_MESH_DFU_SLOTS  1
#if CONFIG_BT_MESH_DFU_SLOTS
#define CONFIG_BT_MESH_DFU_SLOT_CNT  1
#endif

#if CONFIG_BT_MESH_BLOB_SRV && CONFIG_BT_MESH_DFU_CLI
#define CONFIG_BT_MESH_DFD_SRV  1
#if CONFIG_BT_MESH_DFD_SRV
#define CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE  CONFIG_BT_MESH_BLOB_SIZE_MAX
#define CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE  CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE
#define CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX  8
#define CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD  1
#endif
#endif
/* ------------------------ end Models menu ---------------------------------*/

/*------------- Remote Provisioning configuration menu ----------------*/
//RPR Feature
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
#define CONFIG_BT_MESH_RPR_CLI    1
#else
#define CONFIG_BT_MESH_RPR_SRV    1 // if dont need rpr srv,this line should be masked
#endif

#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV || \
    defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
#define CONFIG_BT_MESH_RPR_AD_TYPES_MAX 1
#define CONFIG_BT_MESH_RPR_SRV_SCANNED_ITEMS_MAX 4
#define CONFIG_BT_MESH_RPR_SRV_AD_DATA_MAX 31 //default 31
#endif
/* ------------------------ end Remote Provisioning configuration menu ---------------------------------*/

//PRB Feature
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON) && BT_MESH_ENABLE_PRIVATE_BEACON
#define CONFIG_BT_MESH_PRIV_BEACONS 1

#if defined(CONFIG_BT_MESH_PRIV_BEACONS) && CONFIG_BT_MESH_PRIV_BEACONS
#if defined(BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL) && BT_MESH_ENABLE_PRIVATE_BEACON_CLIENT_MODEL
#define CONFIG_BT_MESH_PRIV_BEACON_CLI    1
#else
#define CONFIG_BT_MESH_PRIV_BEACON_SRV    1 // if dont need prb srv,this line should be masked
#endif
#endif
#endif

/*------------------------ Proxy menu --------------------------*/
// Gatt proxy server relate macros
#define CONFIG_BT_MESH_GATT_PROXY    1  // GATT proxy server support
#if defined(CONFIG_BT_MESH_GATT_PROXY) && CONFIG_BT_MESH_GATT_PROXY
#define CONFIG_BT_MESH_GATT_PROXY_ENABLED    1
#define CONFIG_BT_MESH_NODE_ID_TIMEOUT    60
#define CONFIG_BT_MESH_PROXY_USE_DEVICE_NAME    1
#define CONFIG_BT_MESH_PROXY_FILTER_SIZE    16
#endif
#define CONFIG_BT_MESH_PROXY_CLIENT    1
/* ------------------------ end Proxy menu ---------------------------------*/

#define CONFIG_BT_MESH_USES_TINYCRYPT    1
// #define CONFIG_BT_MESH_USES_MBEDTLS_PSA    1  // maybe used in future
// #define CONFIG_BT_MESH_USES_TFM_PSA    1  // maybe used in future
// #if CONFIG_BT_MESH_USES_MBEDTLS_PSA || CONFIG_BT_MESH_USES_TFM_PSA
// #define CONFIG_BT_MESH_PSA_KEY_ID_USER_MIN_OFFSET    0  // maybe used in future
// #endif
#define CONFIG_BT_MESH_BEACON_ENABLED    1

/*------------------------ IV Index & Sequence number menu --------------------------*/
#define CONFIG_BT_MESH_IV_UPDATE_TEST    1  // maybe used in future
#define CONFIG_BT_MESH_IV_UPDATE_SEQ_LIMIT    0x800000
#define CONFIG_BT_MESH_IVU_DIVIDER    4
/* ------------------------ end IV Index & Sequence number menu ---------------------------------*/

// Friendship Low Power Node
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
#define CONFIG_BT_MESH_LOW_POWER    1
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#define CONFIG_BT_MESH_LPN_ESTABLISHMENT    1
// #define CONFIG_BT_MESH_LPN_AUTO    1  // maybe used in future
// #define CONFIG_BT_MESH_LPN_AUTO_TIMEOUT    15  // maybe used in future
#define CONFIG_BT_MESH_LPN_RETRY_TIMEOUT    8
#define CONFIG_BT_MESH_LPN_RSSI_FACTOR    0
#define CONFIG_BT_MESH_LPN_RECV_WIN_FACTOR    0
#define CONFIG_BT_MESH_LPN_MIN_QUEUE_SIZE    1
#define CONFIG_BT_MESH_LPN_RECV_DELAY    100
#define CONFIG_BT_MESH_LPN_POLL_TIMEOUT    100  // default 300
#define CONFIG_BT_MESH_LPN_INIT_POLL_TIMEOUT    CONFIG_BT_MESH_LPN_POLL_TIMEOUT
#define CONFIG_BT_MESH_LPN_SCAN_LATENCY    15
#define CONFIG_BT_MESH_LPN_GROUPS    8
// #define CONFIG_BT_MESH_LPN_SUB_ALL_NODES_ADDR    0  // maybe used in future
#endif  // CONFIG_BT_MESH_LOW_POWER
#endif  // RTK_BLE_MESH_LPN_SUPPORT

// Friendship Friend Node
#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
#define CONFIG_BT_MESH_FRIEND    1
#if defined(CONFIG_BT_MESH_FRIEND) && CONFIG_BT_MESH_FRIEND
#define CONFIG_BT_MESH_FRIEND_ENABLED    1
#define CONFIG_BT_MESH_FRIEND_RECV_WIN    255
#define CONFIG_BT_MESH_FRIEND_QUEUE_SIZE    16
#define CONFIG_BT_MESH_FRIEND_SUB_LIST_SIZE    3
#define CONFIG_BT_MESH_FRIEND_LPN_COUNT    2
#define CONFIG_BT_MESH_FRIEND_SEG_RX    1
#define CONFIG_BT_MESH_FRIEND_ADV_LATENCY    0
#endif  // CONFIG_BT_MESH_FRIEND
#endif  // RTK_BLE_MESH_FN_SUPPORT

/*------------------------ Capabilities menu --------------------------*/
#define CONFIG_BT_MESH_SUBNET_COUNT    1
#define CONFIG_BT_MESH_APP_KEY_COUNT    1
#define CONFIG_BT_MESH_MODEL_KEY_COUNT    1
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT    1
#define CONFIG_BT_MESH_LABEL_COUNT    1
/* ------------------------ end Capabilities menu ---------------------------------*/

/*------------------------ Persistent storage menu --------------------------*/
#define CONFIG_BT_MESH_STORE_TIMEOUT    2
#define CONFIG_BT_MESH_SEQ_STORE_RATE    0  // Set to 0 for keep update sequence number in flash for every one increment, in case can not get correct sequence number after next init
#if CONFIG_BT_MESH_RPL_STORAGE_MODE_SETTINGS && CONFIG_BT_SETTINGS
#define CONFIG_BT_MESH_RPL_STORE_TIMEOUT    5
#endif
#define CONFIG_BT_MESH_SETTINGS_WORKQ    1
#if CONFIG_BT_MESH_SETTINGS_WORKQ
#define CONFIG_BT_MESH_SETTINGS_WORKQ_PRIO    4  // default:1
#define CONFIG_BT_MESH_SETTINGS_WORKQ_STACK_SIZE    2048  // default:880
#endif
/* ------------------------ end Persistent storage menu ---------------------------------*/

/* ---------------------------- end of ALL menus above -------------------------*/

#if CONFIG_BT_CONN
#define CONFIG_BT_MESH_MAX_CONN    CONFIG_BT_MAX_CONN
#endif

// #define CONFIG_BT_MESH_SELF_TEST    0  // maybe used in future
// #define CONFIG_BT_MESH_STATISTIC    0  // maybe used in future

#if !(defined(CONFIG_BT_MESH_RELAY) && CONFIG_BT_MESH_RELAY)
#undef CONFIG_BT_MESH_RELAY_ADV_SETS
#endif

#if !(defined(CONFIG_BT_MESH_FRIEND) && CONFIG_BT_MESH_FRIEND)
#undef CONFIG_BT_MESH_ADV_EXT_FRIEND_SEPARATE
#endif

#if (defined(CONFIG_BT_MESH_BLOB_SRV) && CONFIG_BT_MESH_BLOB_SRV) || (defined(CONFIG_BT_MESH_BLOB_CLI) && CONFIG_BT_MESH_BLOB_CLI) \
     || (defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV) || (defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI)
#undef CONFIG_BT_MESH_RX_SEG_MAX
#undef CONFIG_BT_MESH_TX_SEG_MAX
#define CONFIG_BT_MESH_RX_SEG_MAX    25  // Maximum number of segments in incoming messages, default is 6
#define CONFIG_BT_MESH_TX_SEG_MAX    25  // Maximum number of segments in outgoing messages, default is 6
#endif

#endif  // __ZEPHYR_CONFIG_BT_MESH_H__
