#include <basic_types.h>
#include <sdn_user_conf_bt.h>
#include <sdn_user_conf_intf.h>

uint8_t g_ble_ll_max_adv_set = 0;
uint8_t g_ble_ll_max_rx_adv_chain_num = 0;
uint8_t g_ble_ll_max_tx_adv_chain_num = 0;
uint8_t g_ble_ll_max_link_num = 0;
uint8_t g_ble_ll_max_conn_tx_ctrl_num = 0;
uint8_t g_ble_ll_max_conn_tx_acl_num = 0;
uint8_t g_ble_ll_max_conn_rx_num = 0;
uint8_t g_ble_ll_max_rl_num = 0;


void ble_ll_load_config(void)
{
	g_ble_ll_max_adv_set = BLE_LL_MAX_ADV_SET;
	g_ble_ll_max_tx_adv_chain_num = BLE_LL_MAX_ADV_TX_CHAIN_NUM;
	g_ble_ll_max_rx_adv_chain_num = BLE_LL_MAX_ADV_RX_CHAIN_NUM;

	g_ble_ll_max_link_num = BT_LL_LE_MAX_CONN_NUM;


	g_ble_ll_max_conn_tx_ctrl_num = (BT_LL_LE_MAX_CONN_NUM * BLE_LL_TX_CTRL_NUM_PER_LINK);
	g_ble_ll_max_conn_tx_acl_num = (BT_LL_LE_MAX_CONN_NUM * BLE_LL_TX_ACL_NUM_PER_LINK);
	g_ble_ll_max_conn_rx_num = (BT_LL_LE_MAX_CONN_NUM * BLE_LL_RX_DATA_NUM_PER_LINK);

#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
	g_ble_ll_max_rl_num = CONFIG_BLE_LL_RESOLVE_LIST_SIZE;
#endif
}