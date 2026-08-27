/* Do NOT modify this file */
#include <platform_autoconf.h>
#include <basic_types.h>
#include <sdn_mem_conf.h>
#ifdef CONFIG_BT_SDN
#include <host_bt_feature.h>

uint8_t g_ble_ll_max_link_num = BLE_LL_MAX_CONN_NUM;
uint8_t g_ble_ll_max_conn_tx_ctrl_num = (BLE_LL_MAX_CONN_NUM *BLE_LL_TX_CTRL_NUM_PER_LINK);
uint8_t g_ble_ll_max_conn_tx_acl_num = (BLE_LL_MAX_CONN_NUM *BLE_LL_TX_ACL_NUM_PER_LINK);
uint8_t g_ble_ll_max_conn_rx_num = (BLE_LL_MAX_CONN_NUM *BLE_LL_RX_DATA_NUM_PER_LINK);

#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
uint8_t g_ble_ll_max_rl_num = BLE_LL_RESOLVE_LIST_SIZE;
#else
uint8_t g_ble_ll_max_rl_num = 0;
#endif

#ifdef CONFIG_BLE_LL_EXT_ADV_ENABLE
uint8_t g_ble_ll_max_adv_set = BLE_LL_MAX_ADV_SET;
uint8_t g_ble_ll_max_tx_adv_chain_num = BLE_LL_MAX_ADV_TX_CHAIN_NUM;
uint8_t g_ble_ll_max_rx_adv_chain_num = BLE_LL_MAX_ADV_RX_CHAIN_NUM;
#else
uint8_t g_ble_ll_max_adv_set = 1;
uint8_t g_ble_ll_max_tx_adv_chain_num = 0;
uint8_t g_ble_ll_max_rx_adv_chain_num = 0;
#endif

#ifdef CONFIG_BLE_LL_PA_SYNC_ENABLE
uint8_t g_ble_ll_max_pa_sync_set_num = BLE_LL_MAX_PA_SYNC_SET_NUM;
uint8_t g_ble_ll_pa_sync_chain_num = BLE_LL_MAX_PA_SYNC_CHAIN_NUM;
#else
uint8_t g_ble_ll_max_pa_sync_set_num = 0;
uint8_t g_ble_ll_pa_sync_chain_num = 0;
#endif

#endif