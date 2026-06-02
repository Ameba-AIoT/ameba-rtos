#include <basic_types.h>
#include <sdn_user_conf_bt.h>
#include <sdn_user_conf_intf.h>

uint8_t g_ble_ll_max_adv_set = BLE_LL_MAX_ADV_SET;
uint8_t g_ble_ll_max_rx_adv_chain_num = BLE_LL_MAX_ADV_RX_CHAIN_NUM;
uint8_t g_ble_ll_max_tx_adv_chain_num = BLE_LL_MAX_ADV_TX_CHAIN_NUM;
uint8_t g_ble_ll_max_link_num = BT_LL_LE_MAX_CONN_NUM;
uint8_t g_ble_ll_max_conn_tx_ctrl_num = (BT_LL_LE_MAX_CONN_NUM *BLE_LL_TX_CTRL_NUM_PER_LINK);
uint8_t g_ble_ll_max_conn_tx_acl_num = (BT_LL_LE_MAX_CONN_NUM *BLE_LL_TX_ACL_NUM_PER_LINK);
uint8_t g_ble_ll_max_conn_rx_num = (BT_LL_LE_MAX_CONN_NUM *BLE_LL_RX_DATA_NUM_PER_LINK);
#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
uint8_t g_ble_ll_max_rl_num = CONFIG_BLE_LL_RESOLVE_LIST_SIZE;
#else
uint8_t g_ble_ll_max_rl_num = 0;
#endif