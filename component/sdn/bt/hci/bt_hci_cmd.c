#include <basic_types.h>
#include <sdn_user_conf_bt.h>
#include <sdn_conf.h>
#include <bt_hci.h>

struct bt_hci_cmd_pkt {
	uint16_t opcode;
	uint8_t param_len;
	uint8_t param[0];
};

#define BT_HCI_EVT_RSP_MAX_LEN              70

static const struct bt_hci_cmd_func_hdl g_bt_hci_cmd_func_tbl[] = {
	//BT_OGF_LINK_CTRL
	{BT_HCI_OP_DISCONNECT, bt_hci_cmd_ogf_linkctrl_ocf_disconnect},
	{BT_HCI_OP_READ_REMOTE_VERSION_INFO, bt_hci_cmd_ogf_linkctrl_ocf_read_version},

	//BT_OGF_BASEBAND
	{BT_HCI_OP_SET_EVENT_MASK, bt_hci_cmd_ogf_baseband_ocf_evt_mask},
	{BT_HCI_OP_RESET, bt_hci_cmd_ofg_baseband_ocf_reset},

	//BT_OGF_INFO
	{BT_HCI_OP_READ_LOCAL_VERSION_INFO, bt_hci_cmd_ogf_info_ocf_read_local_version_info},
	{BT_HCI_OP_READ_SUPPORTED_COMMANDS, bt_hci_cmd_ogf_info_ocf_read_supported_commands},
	{BT_HCI_OP_READ_LOCAL_FEATURES, bt_hci_cmd_ogf_info_ocf_read_local_features},
	{BT_HCI_OP_READ_BD_ADDR, bt_hci_cmd_ogf_info_ocf_read_bd_address},

	//BT_OGF_STATUS
	{BT_HCI_OP_READ_RSSI, bt_hci_cmd_ogf_status_ocf_read_rssi},

	//BT_OGF_LE
	{BT_HCI_OP_LE_SET_EVENT_MASK, bt_hci_ogf_le_ocf_set_evt_mask},
	{BT_HCI_OP_LE_READ_BUFFER_SIZE, bt_hci_ogf_le_ocf_read_buffer_size},
	{BT_HCI_OP_LE_READ_LOCAL_FEATURES, bt_hci_ogf_le_ocf_read_local_feature},
	{BT_HCI_OP_LE_SET_RANDOM_ADDRESS, bt_hci_ogf_le_ocf_set_random_addr},
	{BT_HCI_OP_LE_SET_ADV_PARAM, bt_hci_cmd_ogf_le_ocf_adv_set_param},
	{BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER, bt_hci_cmd_ogf_le_ocf_read_adv_chanl_tx_power},
	{BT_HCI_OP_LE_SET_ADV_DATA, bt_hci_cmd_ogf_le_ocf_set_adv_data},
	{BT_HCI_OP_LE_SET_SCAN_RSP_DATA, bt_hci_cmd_ogf_le_ocf_set_scan_rsp_data},
	{BT_HCI_OP_LE_SET_ADV_ENABLE, bt_hci_cmd_ogf_le_ocf_set_adv_enable},
	{BT_HCI_OP_LE_SET_SCAN_PARAM, bt_hci_cmd_ogf_le_ocf_set_scan_param},
	{BT_HCI_OP_LE_SET_SCAN_ENABLE, bt_hci_cmd_ogf_le_ocf_set_scan_enable},
	{BT_HCI_OP_LE_CREATE_CONN, bt_hci_cmd_ogf_le_ocf_create_connection},
	{BT_HCI_OP_LE_ADD_DEV_TO_FAL, bt_hci_cmd_ogf_le_ocf_add_dev_to_fal},
	{BT_HCI_OP_LE_REM_DEV_FROM_FAL, bt_hci_cmd_ogf_le_ocf_remove_dev_from_fal},
	{BT_HCI_OP_LE_CLEAR_ALL_FAL, bt_hci_cmd_ogf_le_ocf_clear_all_fal},
	{BT_HCI_OP_LE_CONN_UPDATE, bt_hci_cmd_ogf_le_ocf_conn_udpate},
	{BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF, bt_hci_cmd_ogf_le_ocf_set_host_chan_classif},
	{BT_HCI_OP_LE_READ_CHAN_MAP, bt_hci_cmd_ogf_le_ocf_read_channel_map},
	{BT_HCI_OP_LE_READ_REMOTE_FEATURES, bt_hci_cmd_ogf_le_ocf_read_remote_feature},
	{BT_HCI_OP_LE_ENCRYPT, bt_hci_cmd_ofg_le_ocf_le_encrypt},
	{BT_HCI_OP_LE_RAND, bt_hci_cmd_ogf_le_ocf_rand},
	{BT_HCI_OP_LE_ENABLE_ENCRYPTION, bt_hci_cmd_ogf_le_ocf_enable_encryption},
	{BT_HCI_OP_LE_LTK_REQ_REPLY, bt_hci_cmd_ogf_le_ocf_ltk_req_reply},
	{BT_HCI_OP_LE_LTK_REQ_NEG_REPLY, bt_hci_cmd_ogf_le_ocf_ltk_req_neg_reply},
	{BT_HCI_OP_LE_READ_SUPP_STATES, bt_hci_cmd_ogf_le_ocf_read_supp_states},
#ifdef CONFIG_BLE_LL_DTM_ENABLE
	{BT_HCI_OP_LE_RECEIVER_TEST_V1, bt_hci_cmd_ogf_le_ocf_recv_test_v1},
	{BT_HCI_OP_LE_TRANSMITTER_TEST_V1, bt_hci_cmd_ogf_le_ocf_transmit_test_v1},
	{BT_HCI_OP_LE_TEST_END, bt_hci_cmd_ogf_le_ocf_test_end},
	{BT_HCI_OP_LE_RECEIVER_TEST_V2, bt_hci_ogf_le_ocf_recv_test_v2},
	{BT_HCI_OP_LE_TRANSMITTER_TEST_V2, bt_hci_ogf_le_ocf_transmit_test_v2},
#endif
	{BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY, bt_hci_cmd_ogf_le_ocf_conn_param_req_reply},
	{BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY, bt_hci_cmd_ogf_le_ocf_conn_param_req_neg_reply},
	{BT_HCI_OP_LE_SET_DATA_LENGTH, bt_hci_cmd_ogf_le_ocf_set_data_length},
	{BT_HCI_OP_LE_READ_SUGGESTED_DATALEN, bt_hci_cmd_ogf_le_ocf_read_suggested_datalen},
	{BT_HCI_OP_LE_WRITE_SUGGESTED_DATALEN, bt_hci_cmd_ogf_le_ocf_write_suggested_datalen},
#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
	{BT_HCI_OP_LE_READ_PEER_RPA, bt_hci_cmd_ogf_le_ocf_read_peer_rpa},
	{BT_HCI_OP_LE_READ_LOCAL_RPA, bt_hci_cmd_ogf_le_ocf_read_local_rpa},
	{BT_HCI_OP_LE_ADD_DEV_TO_RL, bt_hci_cmd_ogf_le_ocf_add_dev_to_rl},
	{BT_HCI_OP_LE_REM_DEV_FROM_RL, bt_hci_cmd_ogf_le_ocf_remove_dev_from_rl},
	{BT_HCI_OP_LE_CLEAR_RL, bt_hci_cmd_ogf_le_ocf_clear_rl},
	{BT_HCI_OP_LE_READ_RL_SIZE, bt_hci_cmd_ogf_le_ocf_read_rl_size},
	{BT_HCI_OP_LE_SET_RPA_TIMEOUT, bt_hci_cmd_ogf_le_ocf_set_rpa_timeout},
	{BT_HCI_OP_LE_SET_ADDR_RES_ENABLE, bt_hci_cmd_ogf_le_ocf_set_addr_res_enable},
	{BT_HCI_OP_LE_SET_PRIVACY_MODE, bt_hci_cmd_ogf_le_ocf_set_privacy_mode},
#endif
	{BT_HCI_OP_LE_READ_MAX_DATALEN, bt_hci_cmd_ogf_le_ocf_read_max_datalen},
#ifdef CONFIG_BLE_LL_SET_PHY_ENABLE
	{BT_HCI_OP_LE_READ_PHY, bt_hci_cmd_ogf_le_ocf_read_phy},
	{BT_HCI_OP_LE_SET_DEFAULT_PHY, bt_hci_cmd_ogf_le_ocf_set_default_phy},
	{BT_HCI_OP_LE_SET_PHY, bt_hci_cmd_ogf_le_ocf_set_phy},
#endif

#ifdef CONFIG_BLE_LL_EXT_ADV_ENABLE
	{BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN, bt_hci_ogf_le_ocf_read_max_adv_data_len},
	{BT_HCI_OP_LE_READ_NUM_ADV_SETS, bt_hci_ogf_le_ocf_read_max_adv_set},
	{BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR, bt_hci_ogf_le_ocf_set_ext_adv_random_addr},
	{BT_HCI_OP_LE_SET_EXT_ADV_PARAM_V1, bt_hci_cmd_ogf_le_ocf_set_ext_adv_param_v1},
	{BT_HCI_OP_LE_SET_EXT_ADV_DATA, bt_hci_cmd_ogf_le_ocf_set_ext_adv_data},
	{BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA, bt_hci_cmd_ogf_le_ocf_set_ext_scan_rsp_data},
	{BT_HCI_OP_LE_SET_EXT_ADV_ENABLE, bt_hci_cmd_ogf_le_ocf_ext_set_enable},
	{BT_HCI_OP_LE_REMOVE_ADV_SET, bt_hci_cmd_ogf_le_ocf_ext_set_remove},
	{BT_HCI_OP_CLEAR_ADV_SETS, bt_hci_cmd_ogf_le_ocf_ext_set_clear},
	{BT_HCI_OP_LE_SET_EXT_SCAN_PARAM, bt_hci_ogf_le_ocf_set_ext_scan_param},
	{BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE, bt_hci_ogf_le_ocf_set_ext_scan_enable},
	{BT_HCI_OP_LE_EXT_CREATE_CONN, bt_hci_cmd_ogf_le_ocf_ext_create_connection},
	{BT_HCI_OP_LE_EXT_CREATE_CONN_V2, bt_hci_cmd_ogf_le_ocf_ext_create_connection_v2},
	{BT_HCI_OP_LE_CREATE_CONN_CANCEL, bt_hci_cmd_ogf_le_ocf_ext_create_connection_cancel},
#else
	{BT_HCI_OP_LE_CREATE_CONN_CANCEL, bt_hci_cmd_ogf_le_ocf_legacy_create_connection_cancel}, /* for coding size */
#endif

#ifdef CONFIG_BLE_LL_PA_ADV_ENABLE
	{BT_HCI_OP_LE_SET_PERIODIC_ADV_PARAM_V1, bt_hci_cmd_ogf_le_ocf_set_periodic_adv_param_v1},
	{BT_HCI_OP_LE_SET_PERIODIC_ADV_DATA, bt_hci_cmd_ogf_le_ocf_set_periodic_adv_data},
	{BT_HCI_OP_LE_SET_PERIODIC_ADV_ENABLE, bt_hci_cmd_ogf_le_ocf_set_periodic_adv_enable},
#endif

#ifdef CONFIG_BLE_LL_PA_SYNC_ENABLE
	{BT_HCI_OP_LE_PERIODIC_ADV_CREATE_SYNC, bt_hci_cmd_ogf_le_ocf_pa_create_sync},
	{BT_HCI_OP_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL, bt_hci_cmd_ogf_le_ocf_pa_create_sync_cancel},
	{BT_HCI_OP_LE_PERIODIC_ADV_TERMINATE_SYNC, bt_hci_cmd_ogf_le_ocf_pa_terminate_sync},
	{BT_HCI_OP_LE_ADD_PERIODIC_ADV_LIST, bt_hci_cmd_ogf_le_ocf_add_pa_list},
	{BT_HCI_OP_LE_REMOVE_PERIODIC_ADV_LIST, bt_hci_cmd_ogf_le_ocf_remove_pa_list},
	{BT_HCI_OP_LE_CLEAR_PERIODIC_ADV_LIST, bt_hci_cmd_ogf_le_ocf_clear_pa_list},
	{BT_HCI_OP_LE_READ_PERIODIC_ADV_LIST_SIZE, bt_hci_cmd_ogf_le_ocf_read_pa_list_size},
#endif

	//BT_OGF_VENDOR
	{BT_HCI_OP_VENDOR_READ_RTK_CHIP_ID,  bt_hci_cmd_ogf_vendor_ocf_read_rtk_chip_id},
	{BT_HCI_OP_VENDOR_READ_VENDOR_REG, bt_hci_cmd_ogf_vendor_ocf_read_vendor_reg},
	{BT_HCI_OP_VENDOR_WRITE_VENDOR_REG, bt_hci_cmd_ogf_vendor_ocf_write_vendor_reg},
	{BT_HCI_OP_VENDOR_GET_ECO_VERSION, bt_hci_cmd_ogf_vendor_ocf_get_eco_version},
#ifdef CONFIG_MP_INCLUDED
	{BT_HCI_OP_VENDOR_MP, bt_hci_cmd_ogf_vendor_ocf_mp},
#endif
	{BT_HCI_OP_VENDOR_MP_PHY_CMD, bt_hci_cmd_ogf_vendor_ocf_phy},
	{BT_HCI_OP_VENDOR_READ_RADIO_REG_PI, bt_hci_cmd_ogf_vendor_ocf_read_radio_reg_pi},
	{BT_HCI_OP_VENDOR_WRITE_RADIO_REG_PI, bt_hci_cmd_ogf_vendor_ocf_write_radio_reg_pi},
};

void bt_hci_get_supported_hci_command(uint8_t *pcommands)
{
	pcommands[0] |= COMMAND_00_HCI_DISCONNECT;
	pcommands[2] |= COMMAND_02_HCI_READ_REMOTE_VERSION_INFORMATION;
	pcommands[5] |= (COMMAND_05_HCI_SET_EVENT_MASK | COMMAND_05_HCI_RESET);
	pcommands[10] |= (COMMAND_10_HCI_READ_TRANSMIT_POWER_LEVEL |
					  COMMAND_10_HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL |
					  COMMAND_10_HCI_HOST_BUFFER_SIZE |
					  COMMAND_10_HCI_HOST_NUMBER_OF_COMPLETED_PACKETS);
	pcommands[14] |= (COMMAND_14_HCI_READ_LOCAL_VERSION_INFORMATION |
					  COMMAND_14_HCI_READ_LOCAL_SUPPORTED_FEATURES);
	pcommands[15] |= COMMAND_15_HCI_READ_BD_ADDR;
	pcommands[25] |= (COMMAND_25_HCI_LE_SET_EVENT_MASK |
					  COMMAND_25_HCI_LE_READ_BUFFER_SIZE_V1 |
					  COMMAND_25_HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0 |
					  COMMAND_25_HCI_LE_SET_RANDOM_ADDRESS);
	pcommands[25] |= (COMMAND_25_HCI_LE_SET_ADVERTISING_PARAMETERS |
					  COMMAND_25_HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER |
					  COMMAND_25_HCI_LE_SET_ADVERTISING_DATA);
	pcommands[26] |= (COMMAND_26_HCI_LE_SET_RESPONSE_DATA |
					  COMMAND_26_HCI_LE_SET_ADVERTISING_ENABLE);

	pcommands[26] |= (COMMAND_26_HCI_LE_SET_SCAN_PARAMETERS | COMMAND_26_HCI_LE_SET_SCAN_ENABLE |
					  COMMAND_26_HCI_LE_CREATE_CONNECTION   | COMMAND_26_HCI_LE_CREATE_CONNECTION_CANCEL);

	pcommands[26] |= (COMMAND_26_HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE | COMMAND_26_HCI_LE_CLEAR_FILTER_ACCEPT_LIST);

	pcommands[27] |= (COMMAND_27_HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST |
					  COMMAND_27_HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST);
	pcommands[27] |= (COMMAND_27_HCI_LE_ENCRYPT | COMMAND_27_HCI_LE_RAND);
// #ifdef BT_LL_LE_CENTRAL
	pcommands[27] |= COMMAND_27_HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION;
// #endif
	pcommands[27] |= (COMMAND_27_HCI_LE_CONNECTION_UPDATE |
					  COMMAND_27_HCI_LE_READ_CHANNEL_MAP |
					  COMMAND_27_HCI_LE_READ_REMOTE_FEATURES_PAGE_0);
// #ifdef BT_LL_LE_CENTRAL
	pcommands[28] |= COMMAND_28_HCI_LE_ENABLE_ENCRYPTION;
// #endif
	pcommands[28] |= (COMMAND_28_HCI_LE_LONG_TERM_KEY_REQUEST_REPLY |
					  COMMAND_28_HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY);
	pcommands[28] |= COMMAND_28_HCI_LE_READ_SUPPORTED_STATES;
#ifdef CONFIG_BLE_LL_DTM_ENABLE
	pcommands[28] |= (COMMAND_28_HCI_LE_RECEIVER_TEST_V1 | COMMAND_28_HCI_LE_TRANSMITTER_TEST_V1 |
					  COMMAND_28_HCI_LE_TEST_END);

	pcommands[35] |= COMMAND_35_HCI_LE_RECEIVER_TEST_V2;
	pcommands[36] |= COMMAND_36_HCI_LE_TRANSMITTER_TEST_V2;
#endif

	pcommands[33] |= (COMMAND_33_HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY |
					  COMMAND_33_HCI_LE_REMOTE_CONNECTION_PATRAMETER_REQUEST_NEGATIVE_REPLY |
					  COMMAND_33_HCI_LE_SET_DATA_LENGTH |
					  COMMAND_33_HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH);


#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
	pcommands[34] |= (COMMAND_34_HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST |
					  COMMAND_34_HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST |
					  COMMAND_34_HCI_LE_CLEAR_RESOLVING_LIST |
					  COMMAND_34_HCI_LE_READ_RESOLVING_LIST_SIZE |
					  COMMAND_34_HCI_LE_READ_PEER_RESOLVABLE_ADDRESS);
	pcommands[35] |= (COMMAND_35_HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS |
					  COMMAND_35_HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE |
					  COMMAND_35_HCI_LE_SET_RESOLUABLE_PRIVATE_ADDRESS_TIMEOUT);
	pcommands[39] |= COMMAND_39_HCI_LE_SET_PRIVACY_MODE;
#endif

	pcommands[35] |= COMMAND_35_HCI_LE_READ_MAXIMUM_DATA_LENGTH;

#ifdef CONFIG_BLE_LL_SET_PHY_ENABLE
	pcommands[35] |= (COMMAND_35_HCI_LE_READ_PH | COMMAND_35_HCI_LE_SET_DEFAULT_PHY |
					  COMMAND_35_HCI_LE_SET_PHY);
#endif

#ifdef CONFIG_BLE_LL_EXT_ADV_ENABLE
	pcommands[36] |= (COMMAND_36_HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS |
					  COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETER_V1 |
					  COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_DATA |
					  COMMAND_36_HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA |
					  COMMAND_36_HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE |
					  COMMAND_36_HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH |
					  COMMAND_36_HCI_LE_READ_NUMBER_OF_SUPPORT_ADVERTISING_SET);

	pcommands[37] |= (COMMAND_37_HCI_LE_REMOVE_ADVERTISING_SET | COMMAND_37_HCI_LE_CLEAR_ADVERTISING_SET |
					  COMMAND_37_HCI_LE_SET_EXTENDED_SCAN_PARAMETERS | COMMAND_37_HCI_LE_SET_EXTENDED_SCAN_ENABLE |
					  COMMAND_37_HCI_LE_EXTENDED_CREATE_CONNECTION_V1);
#endif

#ifdef CONFIG_BLE_LL_PA_ADV_ENABLE
	pcommands[37] |= (COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETER_V1 |
					  COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_DATA |
					  COMMAND_37_HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE);
#endif
}

void ble_ll_init_feature(uint64_t *pfeature)
{
	*pfeature |= BT_LL_LE_FEATURE_ENCRYPTION;
#if (BT_LL_FEATURE_CONN_PARAM_REQ == 1)
	*pfeature |= BT_LL_LE_FEATURE_CONN_PARA_REQ_PROCEDURE;
#endif
	*pfeature |= BT_LL_LE_FEATURE_EXT_REJECT_IND;
	*pfeature |= BT_LL_LE_FEATURE_PERIPHERAL_INIT_FEATURE_EXCHANGE;
	*pfeature |= BT_LL_LE_FEATURE_LE_PING;
	*pfeature |= BT_LL_LE_FEATURE_LE_DATA_LENGTH_EXTENSION;

#ifdef CONFIG_BLE_LL_PRIVACY_ENABLE
	*pfeature |= BT_LL_LE_FEATURE_LL_PRIVACY;
#endif
	*pfeature |= BT_LL_LE_FEATURE_EXT_SCAN_FILTER_POLICY;

#ifdef CONFIG_BLE_LL_SET_PHY_ENABLE
	*pfeature |= BT_LL_LE_FEATURE_LE_2M_PHY;
#endif

	*pfeature |= BT_LL_LE_FEATURE_STABLE_MODULATION_INDEX_TX;
	*pfeature |= BT_LL_LE_FEATURE_STABLE_MODULATION_INDEX_RX;

#ifdef CONFIG_BLE_LL_SET_PHY_ENABLE
	*pfeature |= BT_LL_LE_FEATURE_LE_CODED_PHY;
#endif

#ifdef CONFIG_BLE_LL_EXT_ADV_ENABLE
	*pfeature |= BT_LL_LE_FEATURE_LE_EXTENDED_ADV;
#endif

#if (defined(CONFIG_BLE_LL_PA_ADV_ENABLE) || defined(CONFIG_BLE_LL_PA_SYNC_ENABLE))
	*pfeature |= BT_LL_LE_FEATURE_LE_PERIODIC_ADV;
#endif

	*pfeature |= BT_LL_LE_FEATURE_LE_CHANNEL_SEL_2;
}

void bt_hci_cmd_handler(uint8_t *pbuf)
{
	struct bt_hci_cmd_pkt *phci_cmd_pkt = (struct bt_hci_cmd_pkt *)pbuf;
	uint8_t len = sizeof(struct bt_hci_evt_cc_status);;
	uint8_t rsp[BT_HCI_EVT_RSP_MAX_LEN] = {BT_HCI_ERROR_UNKNOWN_COMMAND};
	uint32_t i = 0;

	for (i = 0; i < (sizeof(g_bt_hci_cmd_func_tbl) / sizeof(struct bt_hci_cmd_func_hdl)); i++) {
		if (phci_cmd_pkt->opcode == g_bt_hci_cmd_func_tbl[i].opcode) {
			rsp[0] = BT_HCI_ERROR_CMD_DISALLOWED;
			len = g_bt_hci_cmd_func_tbl[i].hdl(phci_cmd_pkt->param, rsp);
			break;
		}
	}

	if (len) {
		bt_hci_evt_command_complete(phci_cmd_pkt->opcode, len, rsp);
	}
}
