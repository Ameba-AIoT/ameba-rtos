/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlist.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <zephyr_stack_config.h>
#include <bt_api_config.h>

#include <zephyr/bluetooth/hci.h>
#include <zephyr_stack_api.h>
#include <zephyr_stack_internal.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/net/buf.h>
#include <stack/host/hci_core.h>
#include <stack/host/conn_internal.h>
#include <stack/host/smp.h>
#include <stack/host/adv.h>
#include <zephyr/sys/byteorder.h>
#include <stack/host/keys.h>
#include <stack/host/settings.h>
#include <stack/host/id.h>
#include <stack/common/rpa.h>
#include <stack/common/bt_str.h>

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
#include "direction_internal.h"
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <rtk_bt_device.h>
bool rtk_ble_mesh_scan_enable_flag = false;
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
static bool privacy_whitelist = false;
static bool privacy_irk_app_cfg = false;
static uint8_t privacy_irk[RTK_BT_LE_GAP_IRK_LEN];
#endif

enum {
	ZEPHYR_SCAN_STATE_IDLE,
	ZEPHYR_SCAN_STATE_LEGACY,
	ZEPHYR_SCAN_STATE_EXT,
};

static uint16_t bt_stack_le_conn_handle[CONFIG_BT_MAX_CONN] = {0};
static bt_zephyr_gap_phy_setting phy_setting;
static bt_zephyr_gap_data_len_setting data_len_setting;
static uint8_t legacy_ad_buf[BT_ZEPHYR_LEGACY_ADV_MAX_LEN];
static uint8_t legacy_sd_buf[BT_ZEPHYR_LEGACY_ADV_MAX_LEN];
static bt_zephyr_adv_data legacy_ad;
static bt_zephyr_adv_data legacy_sd;
static struct bt_le_scan_param scan_param;
static uint8_t zephyr_scan_state = ZEPHYR_SCAN_STATE_IDLE;
static bt_security_t security_level = BT_SECURITY_L2;

extern struct bt_dev bt_dev;
uint16_t bt_stack_le_gap_get_conn_id(uint16_t conn_handle, uint8_t *p_conn_id);

static void _indicate_adv_stop(uint8_t stop_reason)
{
	rtk_bt_le_adv_stop_ind_t *p_le_adv_stop_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_ADV_STOP_IND,
								sizeof(rtk_bt_le_adv_stop_ind_t));
	if (!p_evt) {
		return;
	}

	p_le_adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)p_evt->data;
	p_le_adv_stop_ind->err = 0;
	p_le_adv_stop_ind->stop_reason = stop_reason;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void bt_zephyr_connected_callback(struct bt_conn *conn, uint8_t err)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_conn_ind_t *p_conn_ind;
	bt_addr_le_t *peer_addr;

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_CONNECT_IND,
								sizeof(rtk_bt_le_conn_ind_t));
	if (!p_evt) {
		return;
	}

	p_conn_ind = (rtk_bt_le_conn_ind_t *)p_evt->data;

	/* For central, if le connection cannot be established within timeout, zephyr stack will
	auto cancel it, then the hci_le_conn_complete_evt or hci_le_enhanced_conn_complete_evt
	will report error code RTK_BT_HCI_ERR_UNKNOWN_CONN_ID(0x2) */
	if (RTK_BT_HCI_ERR_UNKNOWN_CONN_ID == err && BT_HCI_ROLE_CENTRAL == conn->role) {
		p_conn_ind->err = RTK_BT_ERR_CREATE_CONN_TIMEOUT;
	} else {
		p_conn_ind->err = RTK_BT_HCI_ERR(err);
	}

	bt_stack_le_conn_handle[bt_conn_index(conn)] = conn->handle;
	p_conn_ind->role = conn->role;
	p_conn_ind->conn_handle = conn->handle;
	p_conn_ind->conn_interval = conn->le.interval;
	p_conn_ind->conn_latency = conn->le.latency;
	p_conn_ind->supv_timeout = conn->le.timeout;

	peer_addr = (BT_HCI_ROLE_PERIPHERAL == conn->role) ? &conn->le.init_addr : &conn->le.resp_addr;
	if (!conn->le.dst_flag) {
		p_conn_ind->peer_addr.type = conn->le.dst.type;
		if (!err && memcmp(peer_addr->a.val, conn->le.dst.a.val, BT_ADDR_SIZE)) {
			/* If the peer_addr not equal to dst addr(identity address), that means peer_addr is RPA,
			 translate the public(0)/random(1) addr type to public-ident(2)/random-ident(3) addr type */
			p_conn_ind->peer_addr.type |= 0x2;
		}
		memcpy(p_conn_ind->peer_addr.addr_val, conn->le.dst.a.val, BT_ADDR_SIZE);
	} else {
		/* dst addr is resolved by host, this situation is for stack internal use. So, not report dst addr in
		conn/disconn callback to APP in this situation. */
		p_conn_ind->peer_addr.type = peer_addr->type;
		memcpy(p_conn_ind->peer_addr.addr_val, peer_addr->a.val, BT_ADDR_SIZE);
	}

	rtk_bt_evt_indicate(p_evt, NULL);

	if (conn->role == BT_HCI_ROLE_CENTRAL) {
		// bt_le_set_phy(conn, phy_setting.all_phys, phy_setting.tx_phy, phy_setting.rx_phy, 0);
		// bt_le_set_data_len(conn, data_len_setting.tx_octets, data_len_setting.tx_time);
	}

#if !defined(RTK_BLE_5_0_USE_EXTENDED_ADV) || !RTK_BLE_5_0_USE_EXTENDED_ADV
	if (conn->role == BT_HCI_ROLE_PERIPHERAL) {
		_indicate_adv_stop(RTK_BT_LE_ADV_STOP_BY_CONN);
	}
#endif
}

static void bt_zephyr_disconnected_callback(struct bt_conn *conn, uint8_t reason)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_disconn_ind_t *p_disconn_ind;
	bt_addr_le_t *peer_addr;

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}

	if (BT_HCI_ROLE_CENTRAL == conn->role) {
		bt_stack_gattc_disconnect_clear(conn->handle);
	} else if (BT_HCI_ROLE_PERIPHERAL == conn->role) {
		bt_stack_gatts_disconnect_clear(conn);
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_DISCONN_IND,
								sizeof(rtk_bt_le_disconn_ind_t));
	if (!p_evt) {
		return;
	}

	p_disconn_ind = (rtk_bt_le_disconn_ind_t *)p_evt->data;

	p_disconn_ind->reason = reason ? RTK_BT_HCI_ERR(reason) : 0;
	p_disconn_ind->role = conn->role;
	p_disconn_ind->conn_handle = conn->handle;

	peer_addr = (BT_HCI_ROLE_PERIPHERAL == conn->role) ? &conn->le.init_addr : &conn->le.resp_addr;
	if (!conn->le.dst_flag) {
		p_disconn_ind->peer_addr.type = conn->le.dst.type;
		if (memcmp(peer_addr->a.val, conn->le.dst.a.val, BT_ADDR_SIZE)) {
			p_disconn_ind->peer_addr.type |= 0x2;
		}
		memcpy(p_disconn_ind->peer_addr.addr_val, conn->le.dst.a.val, BT_ADDR_SIZE);
	} else {
		p_disconn_ind->peer_addr.type = peer_addr->type;
		memcpy(p_disconn_ind->peer_addr.addr_val, peer_addr->a.val, BT_ADDR_SIZE);
	}

	rtk_bt_evt_indicate(p_evt, NULL);
}

void bt_zephyr_le_param_update_callback(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
	(void)interval;

	rtk_bt_evt_t *p_evt = NULL;
	uint8_t cb_ret = 0;
	rtk_bt_le_conn_update_ind_t *update_conn = NULL;

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND,
								sizeof(rtk_bt_le_conn_update_ind_t));

	update_conn = (rtk_bt_le_conn_update_ind_t *)p_evt->data;

	update_conn->err = 0;
	update_conn->conn_handle = conn->handle;
	update_conn->conn_interval = interval;
	update_conn->conn_latency = latency;
	update_conn->supv_timeout = timeout;

	rtk_bt_evt_indicate(p_evt, &cb_ret);
}

extern bool is_smp_key_distributing(struct bt_conn *conn);
extern void pair_result_indicate(struct bt_conn *conn, enum bt_security_err reason);
void bt_zephyr_le_security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	(void)level;

	/* The encryption changed event may occurred in pairing process, the pairing hasn't finished now,
	so, leave pairing result report on pairing_complete */
	if (is_smp_key_distributing(conn)) {
		return;
	}

	pair_result_indicate(conn, err);
}

void bt_zephyr_data_len_updated_callback(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_data_len_change_ind_t *data_len_change = NULL;

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND,
								sizeof(rtk_bt_le_data_len_change_ind_t));

	if (!p_evt) {
		return;
	}

	data_len_change = (rtk_bt_le_data_len_change_ind_t *)p_evt->data;

	data_len_change->conn_handle = conn->handle;
	data_len_change->max_rx_octets = info->rx_max_len;
	data_len_change->max_rx_time = info->rx_max_time;
	data_len_change->max_tx_octets = info->tx_max_len;
	data_len_change->max_tx_time = info->tx_max_time;

	rtk_bt_evt_indicate(p_evt, NULL);
}

void bt_zephyr_phy_updated_callback(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_phy_update_ind_t *phy_update = NULL;

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND,
								sizeof(rtk_bt_le_phy_update_ind_t));

	if (!p_evt) {
		return;
	}

	phy_update = (rtk_bt_le_phy_update_ind_t *)p_evt->data;

	phy_update->conn_handle = conn->handle;
	phy_update->err = 0;
	phy_update->tx_phy = param->tx_phy;
	phy_update->rx_phy = param->rx_phy;

	rtk_bt_evt_indicate(p_evt, NULL);
}

#if defined(CONFIG_BT_DF_CONNECTION_CTE_RX) && defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
static void bt_zephyr_conn_cte_report(struct bt_conn *conn, const struct bt_df_conn_iq_samples_report *iq_report)
{
	rtk_bt_evt_t *p_evt = NULL;

	if (!conn || !iq_report) {
		BT_LOGE("%s: invalid data\r\n", __func__);
		return;
	}

	if (conn->type != BT_CONN_TYPE_LE) {
		return;
	}
	if (iq_report->err) {
		rtk_bt_le_gap_cte_req_fail_ind_t *p_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
									RTK_BT_LE_GAP_EVT_CONN_CTE_REQ_FAIL_IND,
									sizeof(rtk_bt_le_gap_cte_req_fail_ind_t));
		if (!p_evt) {
			return;
		}

		p_ind = (rtk_bt_le_gap_cte_req_fail_ind_t *)p_evt->data;
		p_ind->conn_handle = conn->handle;

		if (iq_report->err == BT_DF_IQ_REPORT_ERR_NO_CTE) {
			p_ind->cause = 0x00;
		} else if (iq_report->err == BT_DF_IQ_REPORT_ERR_PEER_REJECTED) {
			p_ind->cause = 0X01;
		} else {
			p_ind->cause = iq_report->err;
		}

		rtk_bt_evt_indicate(p_evt, NULL);

	} else {
		rtk_bt_le_gap_conn_iq_report_ind_t *p_ind = NULL;

		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
									RTK_BT_LE_GAP_EVT_CONN_IQ_REPORT_IND,
									sizeof(rtk_bt_le_gap_conn_iq_report_ind_t));
		if (!p_evt) {
			return;
		}

		p_ind = (rtk_bt_le_gap_conn_iq_report_ind_t *)p_evt->data;
		p_ind->conn_handle              = conn->handle;
		p_ind->rx_phy                   = iq_report->rx_phy;
		p_ind->data_chan_index          = iq_report->chan_idx;
		p_ind->rssi                     = iq_report->rssi;
		p_ind->rssi_antenna_id          = iq_report->rssi_ant_id;
		if (iq_report->cte_type == BT_DF_CTE_TYPE_AOA) {
			p_ind->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOA;
		} else if (iq_report->cte_type == BT_DF_CTE_TYPE_AOD_1US) {
			p_ind->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_1US;
		} else if (iq_report->cte_type == BT_DF_CTE_TYPE_AOD_2US) {
			p_ind->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_2US;
		}
		p_ind->slot_durations           = (rtk_bt_le_gap_slot_durations_e)iq_report->slot_durations;
		p_ind->packet_status            = (rtk_bt_le_gap_cte_packet_status_type_e)iq_report->packet_status;
		p_ind->connection_event_counter = iq_report->conn_evt_counter;
		if (iq_report->sample_type == BT_DF_IQ_SAMPLE_8_BITS_INT) {
			p_ind->sample_count         = iq_report->sample_count;
			for (uint8_t i = 0; i < p_ind->sample_count; ++i) {
				p_ind->iq_sample[i * 2] = iq_report->sample[i].i;
				p_ind->iq_sample[i * 2 + 1] = iq_report->sample[i].q;
			}
		}

		rtk_bt_evt_indicate(p_evt, NULL);
	}
}
#endif

static struct bt_conn_cb bt_zephyr_conn_cb = {
	.connected = bt_zephyr_connected_callback,
	.disconnected = bt_zephyr_disconnected_callback,
	.le_param_updated = bt_zephyr_le_param_update_callback,
#if defined(CONFIG_BT_SMP)
	.security_changed = bt_zephyr_le_security_changed,
#endif
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
	.le_data_len_updated = bt_zephyr_data_len_updated_callback,
#endif
#if defined(CONFIG_BT_USER_PHY_UPDATE)
	.le_phy_updated = bt_zephyr_phy_updated_callback,
#endif
#if defined(CONFIG_BT_DF_CONNECTION_CTE_RX) && defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	.cte_report_cb = bt_zephyr_conn_cte_report,
#endif
};

static void _indicate_scan_start(void)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_scan_start_ind_t *p_scan_start_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_SCAN_START_IND,
								sizeof(rtk_bt_le_scan_start_ind_t));
	if (!p_evt) {
		return;
	}

	p_scan_start_ind = (rtk_bt_le_scan_start_ind_t *)p_evt->data;
	p_scan_start_ind->scan_type = scan_param.type;
	p_scan_start_ind->err = 0;

	rtk_bt_evt_indicate(p_evt, NULL);
}

static void _indicate_scan_stop(uint8_t reason)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_scan_stop_ind_t *p_scan_stop_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_SCAN_STOP_IND,
								sizeof(rtk_bt_le_scan_stop_ind_t));
	if (!p_evt) {
		return;
	}

	p_scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)p_evt->data;
	p_scan_stop_ind->err = 0;
	p_scan_stop_ind->stop_reason = reason;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void scan_cb_recv(const struct bt_le_scan_recv_info *info, struct net_buf_simple *buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_scan_res_ind_t *scan_res = NULL;
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	rtk_bt_le_ext_scan_res_ind_t *ext_scan_res = NULL;
#endif

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (!rtk_bt_mesh_is_enable() || rtk_ble_mesh_scan_enable_flag)
#endif
	{
		if (zephyr_scan_state == ZEPHYR_SCAN_STATE_LEGACY) {
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
										RTK_BT_LE_GAP_EVT_SCAN_RES_IND,
										sizeof(rtk_bt_le_scan_res_ind_t));
			if (!p_evt) {
				return;
			}

			scan_res = (rtk_bt_le_scan_res_ind_t *)p_evt->data;
			scan_res->num_report = 1;
			scan_res->adv_report.addr.type = (rtk_bt_le_addr_type_t)info->addr->type;
			memcpy(scan_res->adv_report.addr.addr_val, info->addr->a.val, BT_ADDR_SIZE);

			scan_res->adv_report.rssi = info->rssi;
			scan_res->adv_report.evt_type = (rtk_bt_le_adv_report_type_t)info->adv_type;

			scan_res->adv_report.len = buf->len;
			memcpy(scan_res->adv_report.data, buf->data, buf->len);

		}
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
		else if (zephyr_scan_state == ZEPHYR_SCAN_STATE_EXT) {
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
										RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND,
										sizeof(rtk_bt_le_ext_scan_res_ind_t) + buf->len);
			if (!p_evt) {
				return;
			}

			ext_scan_res = (rtk_bt_le_ext_scan_res_ind_t *)p_evt->data;
			ext_scan_res->evt_type = (rtk_bt_le_adv_report_type_t)info->adv_type;
			ext_scan_res->rssi = info->rssi;
			ext_scan_res->adv_sid = info->sid;
			ext_scan_res->tx_power = info->tx_power;
			ext_scan_res->primary_phy = info->primary_phy;
			ext_scan_res->secondary_phy = info->secondary_phy;
			ext_scan_res->peri_adv_interval = info->interval;
			ext_scan_res->addr.type = (rtk_bt_le_addr_type_t)info->addr->type;
			memcpy(ext_scan_res->addr.addr_val, info->addr->a.val, BT_ADDR_SIZE);
			ext_scan_res->data = (uint8_t *)ext_scan_res + sizeof(rtk_bt_le_ext_scan_res_ind_t);
			ext_scan_res->len = buf->len;
			memcpy(ext_scan_res->data, buf->data, buf->len);
		}
#endif
		else {
			return;
		}

		rtk_bt_evt_indicate(p_evt, NULL);
	}
}

static void scan_cb_timeout(void)
{
	_indicate_scan_stop(RTK_BT_LE_SCAN_STOP_BY_DURATION);
}

static struct bt_le_scan_cb bt_zephyr_scan_cb = {
	.recv = scan_cb_recv,
	.timeout = scan_cb_timeout,
};

#if defined(CONFIG_BT_SMP)
void zephyr_auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_auth_key_display_ind_t *key_disp_ind;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND,
								sizeof(rtk_bt_le_auth_key_display_ind_t));
	if (!p_evt) {
		return;
	}

	key_disp_ind = (rtk_bt_le_auth_key_display_ind_t *)p_evt->data;
	key_disp_ind->conn_handle = conn->handle;
	key_disp_ind->passkey = passkey;

	BT_LOGD("[auth_passkey_display]: need app to display passkey, conn_handle: %d, passkey: %u\r\n", conn->handle, passkey);
	rtk_bt_evt_indicate(p_evt, NULL);
}

void zephyr_auth_passkey_entry(struct bt_conn *conn)
{
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_auth_key_input_ind_t *key_input_ind;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND,
								sizeof(rtk_bt_le_auth_key_input_ind_t));
	if (!p_evt) {
		return;
	}

	key_input_ind = (rtk_bt_le_auth_key_input_ind_t *)p_evt->data;
	key_input_ind->conn_handle = conn->handle;
	BT_LOGD("[auth_passkey_entry]: need user to input auth passkey, conn_handle: %d\r\n", conn->handle);

	rtk_bt_evt_indicate(p_evt, NULL);
}

void zephyr_auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND,
								sizeof(rtk_bt_le_auth_key_cfm_ind_t));
	if (!p_evt) {
		return;
	}

	key_cfm_ind = (rtk_bt_le_auth_key_cfm_ind_t *)p_evt->data;
	key_cfm_ind->conn_handle = conn->handle;
	key_cfm_ind->passkey = passkey;
	BT_LOGD("[auth_passkey_confirmation]: need user to compare passkey, conn_handle: %d, passkey: %u\r\n", conn->handle, passkey);

	rtk_bt_evt_indicate(p_evt, NULL);
}

void zephyr_auth_oob_data_request(struct bt_conn *conn, struct bt_conn_oob_info *info)
{
	(void)info;
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_auth_oob_input_ind_t *oob_input_ind;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND,
								sizeof(rtk_bt_le_auth_oob_input_ind_t));
	if (!p_evt) {
		return;
	}

	oob_input_ind = (rtk_bt_le_auth_oob_input_ind_t *)p_evt->data;
	oob_input_ind->conn_handle = conn->handle;
	// if (info->type == BT_CONN_OOB_LE_LEGACY)
	//  oob_input_ind->type = RTK_BT_LE_SM_OOB_LEGACY;
	// else if (info->lesc.oob_config == BT_CONN_OOB_LOCAL_ONLY) {
	//  oob_input_ind->type = RTK_BT_LE_SM_OOB_SC_LOCAL;
	// }
	// else if (info->lesc.oob_config == BT_CONN_OOB_REMOTE_ONLY) {
	//  oob_input_ind->type = RTK_BT_LE_SM_OOB_SC_REMOTE;
	// }
	// else if (info->lesc.oob_config == BT_CONN_OOB_BOTH_PEERS) {
	//  oob_input_ind->type = RTK_BT_LE_SM_OOB_SC_BOTH;
	// }

	BT_LOGD("[auth_oob_input]: need user to input oob data, conn_handle: %d\r\n", conn->handle);

	rtk_bt_evt_indicate(p_evt, NULL);
}

void pair_result_indicate(struct bt_conn *conn, enum bt_security_err reason)
{
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_auth_complete_ind_t *auth_ind;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND,
								sizeof(rtk_bt_le_auth_complete_ind_t));
	if (!p_evt) {
		return;
	}

	auth_ind = (rtk_bt_le_auth_complete_ind_t *)p_evt->data;
	auth_ind->conn_handle = conn->handle;

	if (BT_SECURITY_ERR_SUCCESS == reason) {
		auth_ind->err = 0;
		BT_LOGD("[authen_state_evt]: Auth state change succeed\r\n");
		auth_ind->dev_ltk_length = conn->le.keys->enc_size;
		if (BT_CONN_TYPE_LE == conn->type) {
			if (!(conn->le.keys->flags & BT_KEYS_SC) && BT_HCI_ROLE_PERIPHERAL == conn->role) {
				memcpy(auth_ind->dev_ltk, conn->le.keys->periph_ltk.val, auth_ind->dev_ltk_length);
			} else {
				memcpy(auth_ind->dev_ltk, conn->le.keys->ltk.val, auth_ind->dev_ltk_length);
			}
			auth_ind->sec_level = (rtk_bt_le_sec_level_t)conn->sec_level;
		}
	} else {
		auth_ind->err = reason;
		BT_LOGD("[authen_state_change]: Auth state change failed(reason: 0x%x)\r\n", reason);
	}

	rtk_bt_evt_indicate(p_evt, NULL);
}

void zephyr_auth_cancel(struct bt_conn *conn)
{
	(void)conn;
	// pair_result_indicate(conn, BT_SECURITY_ERR_AUTH_FAIL);
}

void zephyr_auth_pairing_confirm(struct bt_conn *conn)
{
	rtk_bt_evt_t *p_evt;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND,
								sizeof(rtk_bt_le_auth_pair_cfm_ind_t));
	if (!p_evt) {
		return;
	}

	rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind = (rtk_bt_le_auth_pair_cfm_ind_t *)p_evt->data;
	pair_cfm_ind->conn_handle = conn->handle;
	BT_LOGD("[auth_just_work]: device is not pair initiator, need user to confirm whether go on pairing, conn_handle: %d\r\n", conn->handle);

	rtk_bt_evt_indicate(p_evt, NULL);
}

static struct bt_conn_auth_cb bt_zephyr_auth_cb;

/**
 *  @param id_addr identity address of remote device.
 *  @param remote remote address.
 */
void bond_info_indicate(rtk_bt_le_bond_op_t op, const bt_addr_le_t *id_addr, const bt_addr_le_t *remote)
{
	rtk_bt_evt_t *p_evt;
	rtk_bt_le_bond_modify_ind_t *bond_modify;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND,
								sizeof(rtk_bt_le_bond_modify_ind_t));
	if (!p_evt) {
		return;
	}

	bond_modify = (rtk_bt_le_bond_modify_ind_t *)p_evt->data;
	bond_modify->op = op;
	if (RTK_BT_LE_BOND_ADD == op || RTK_BT_LE_BOND_DELETE == op) {
		bond_modify->ident_addr.type = id_addr->type;
		memcpy(bond_modify->ident_addr.addr_val, id_addr->a.val, BT_ADDR_SIZE);
	}
	if (RTK_BT_LE_BOND_ADD == op) {
		bond_modify->remote_addr.type = remote->type;
		memcpy(bond_modify->remote_addr.addr_val, remote->a.val, BT_ADDR_SIZE);
	}
	rtk_bt_evt_indicate(p_evt, NULL);

}

void bt_zephyr_auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	const bt_addr_le_t *addr = NULL;

	pair_result_indicate(conn, BT_SECURITY_ERR_SUCCESS);

	if (bonded) {
		if (BT_HCI_ROLE_CENTRAL == conn->role) {
			addr = &conn->le.resp_addr;
		} else if (BT_HCI_ROLE_PERIPHERAL == conn->role) {
			addr = &conn->le.init_addr;
		}
		bond_info_indicate(RTK_BT_LE_BOND_ADD, &conn->le.dst, addr);
	}
}

void bt_zephyr_auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	(void)conn;
	(void)reason;
	// pair_result_indicate(conn, reason);
}

void bt_zephyr_auth_bond_deleted(uint8_t id, const bt_addr_le_t *peer)
{
	(void)id;

	bond_info_indicate(RTK_BT_LE_BOND_DELETE, peer, NULL);
}

struct bt_conn_auth_info_cb bt_zephyr_auth_info_cb = {
	.pairing_complete = bt_zephyr_auth_pairing_complete,
	.pairing_failed = bt_zephyr_auth_pairing_failed,
	.bond_deleted = bt_zephyr_auth_bond_deleted,
};
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
static void bt_zephyr_le_per_adv_synced(struct bt_le_per_adv_sync *sync, struct bt_le_per_adv_sync_synced_info *info)
{
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND,
								sizeof(rtk_bt_le_pa_sync_ind_t));
	if (!p_evt) {
		return;
	}

	rtk_bt_le_pa_sync_ind_t *p_ind = (rtk_bt_le_pa_sync_ind_t *)p_evt->data;
	p_ind->sync_id = bt_le_per_adv_sync_get_index(sync);
	p_ind->state = RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED;
	p_ind->cause = 0;

	p_ind->info.past_received = info->recv_enabled;
	p_ind->info.sync_handle = sync->handle;
	p_ind->info.adv_sid = info->sid;
	p_ind->info.addr.type = (rtk_bt_le_addr_type_t)info->addr->type;
	memcpy(p_ind->info.addr.addr_val, info->addr->a.val, BT_ADDR_SIZE);
	p_ind->info.adv_phy = (rtk_bt_le_phy_type_t)info->phy;
	p_ind->info.adv_clock_accuracy = sync->clock_accuracy;
	p_ind->info.periodic_adv_interval = info->interval;
#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	p_ind->info.num_subevents = info->num_subevents;
	p_ind->info.subevent_interval = info->subevent_interval;
	p_ind->info.response_slot_delay = info->response_slot_delay;
	p_ind->info.response_slot_spacing = info->response_slot_spacing;
#endif

	p_ind->info.past_received = info->conn ? true : false;
	if (p_ind->info.past_received) {
		p_ind->info.conn_handle = info->conn->handle;
		p_ind->info.service_data = info->service_data;
	}

	rtk_bt_evt_indicate(p_evt, NULL);
}

static void bt_zephyr_le_per_adv_term(struct bt_le_per_adv_sync *sync, const struct bt_le_per_adv_sync_term_info *info)
{
	rtk_bt_le_pa_sync_ind_t *p_ind;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND,
								sizeof(rtk_bt_le_pa_sync_ind_t));
	if (!p_evt) {
		return;
	}

	p_ind = (rtk_bt_le_pa_sync_ind_t *)p_evt->data;
	p_ind->sync_id = bt_le_per_adv_sync_get_index(sync);
	p_ind->state = RTK_BT_LE_PA_SYNC_STATE_TERMINATED;
	p_ind->cause = info->reason;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void bt_zephyr_le_per_adv_recv_info(struct bt_le_per_adv_sync *sync, const struct bt_le_per_adv_sync_recv_info *info, struct net_buf_simple *buf)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_pa_adv_report_ind_t *pa_report = NULL;
	uint16_t len = 0;

	/* when received fail, buf is NULL */
	if (buf) {
		len = buf->len;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND,
								sizeof(rtk_bt_le_pa_adv_report_ind_t) + len);
	if (!p_evt) {
		return;
	}

	pa_report = (rtk_bt_le_pa_adv_report_ind_t *)p_evt->data;

	pa_report->sync_id = bt_le_per_adv_sync_get_index(sync);
	pa_report->sync_handle = sync->handle;
	pa_report->tx_power = info->tx_power;
	pa_report->rssi = info->rssi;
	pa_report->cte_type = (rtk_bt_le_gap_cte_type_t)info->cte_type;
#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	pa_report->periodic_event_counter = info->periodic_event_counter;
	pa_report->subevent = info->subevent;
#endif
	if (buf) {
		pa_report->data_status = RTK_BT_LE_PA_REPORT_DATA_STATUS_COMPLETE;
		pa_report->data_len = buf->len;
		pa_report->p_data = (uint8_t *)pa_report + sizeof(rtk_bt_le_pa_adv_report_ind_t);
		memcpy(pa_report->p_data, buf->data, buf->len);
	} else {
		pa_report->data_status = RTK_BT_LE_PA_REPORT_DATA_STATUS_RX_FAILED;
	}
	rtk_bt_evt_indicate(p_evt, NULL);
}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
static void bt_zephyr_le_per_adv_cte_report(struct bt_le_per_adv_sync *sync, struct bt_df_per_adv_sync_iq_samples_report const *info)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_gap_connless_iq_report_ind_t *iq_report = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP,
								RTK_BT_LE_GAP_EVT_CONNLESS_IQ_REPORT_IND,
								sizeof(rtk_bt_le_gap_connless_iq_report_ind_t));
	if (!p_evt) {
		return;
	}

	iq_report = (rtk_bt_le_gap_connless_iq_report_ind_t *)p_evt->data;

	if (info->cte_type == BT_DF_CTE_TYPE_AOA) {
		iq_report->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOA;
	} else if (info->cte_type == BT_DF_CTE_TYPE_AOD_1US) {
		iq_report->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_1US;
	} else if (info->cte_type == BT_DF_CTE_TYPE_AOD_2US) {
		iq_report->cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_2US;
	} else {
		iq_report->cte_type = RTK_BT_LE_GAP_CTE_TYPE_NONE;
	}

	iq_report->sync_id                  = bt_le_per_adv_sync_get_index(sync);
	iq_report->sync_handle              = sync->handle;
	iq_report->channel_index            = info->chan_idx;
	iq_report->rssi                     = info->rssi;
	iq_report->rssi_antenna_id          = info->rssi_ant_id;
	iq_report->slot_durations           = (rtk_bt_le_gap_slot_durations_e)info->slot_durations;
	iq_report->packet_status            = (rtk_bt_le_gap_cte_packet_status_type_e)info->packet_status;
	iq_report->periodic_event_counter   = info->per_evt_counter;

	if (info->sample_type == BT_DF_IQ_SAMPLE_8_BITS_INT) {
		iq_report->sample_count = info->sample_count;
		for (uint8_t i = 0; i < iq_report->sample_count; ++i) {
			iq_report->iq_sample[i * 2] = info->sample[i].i;
			iq_report->iq_sample[i * 2 + 1] = info->sample[i].q;
		}
	}

	rtk_bt_evt_indicate(p_evt, NULL);
}
#endif

static struct bt_le_per_adv_sync_cb bt_zephyr_per_adv_sync_cb = {
	.synced = bt_zephyr_le_per_adv_synced,
	.term = bt_zephyr_le_per_adv_term,
	.recv = bt_zephyr_le_per_adv_recv_info,
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	.cte_report_cb = bt_zephyr_le_per_adv_cte_report,
#endif
};
#endif

int bt_stack_le_gap_init(void *app_config)
{
	rtk_bt_app_conf_t *papp_conf = (rtk_bt_app_conf_t *)app_config;

	if (papp_conf != NULL) {
		phy_setting.all_phys = papp_conf->prefer_all_phy;
		phy_setting.tx_phy = papp_conf->prefer_tx_phy;
		phy_setting.rx_phy = papp_conf->prefer_rx_phy;

		data_len_setting.tx_octets = papp_conf->max_tx_octets;
		data_len_setting.tx_time = papp_conf->max_tx_time;
	} else {
		phy_setting.all_phys = 0;
		phy_setting.tx_phy = BIT0 | BIT1 | BIT2;
		phy_setting.rx_phy = BIT0 | BIT1 | BIT2;

		data_len_setting.tx_octets = 0x40;
		data_len_setting.tx_time = 0x200;
	}

	if (is_stack_never_enabled()) {
		bt_conn_cb_register(&bt_zephyr_conn_cb);
		bt_le_scan_cb_register(&bt_zephyr_scan_cb);

		if (bt_conn_auth_info_cb_register(&bt_zephyr_auth_info_cb)) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
		bt_le_per_adv_sync_cb_register(&bt_zephyr_per_adv_sync_cb);
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		if (!((rtk_bt_app_conf_t *)app_config)->irk_auto_gen) {
			memcpy(privacy_irk, ((rtk_bt_app_conf_t *)app_config)->irk, RTK_BT_LE_GAP_IRK_LEN);
			privacy_irk_app_cfg = true;
		}
#endif
	}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	le_df_init();
#endif

	return 0;
}

uint16_t bt_stack_le_gap_deinit(void)
{
	memset((void *)bt_stack_le_conn_handle, 0, sizeof(uint16_t) * CONFIG_BT_MAX_CONN);
	return 0;
}

static uint16_t bt_stack_le_gap_get_version(void *param)
{
	rtk_bt_le_version_info_t *version = (rtk_bt_le_version_info_t *)param;
	const char *version_str_full = bt_stack_version_str();
	char version_str[9] = {0};
	size_t version_num = 0;

	version->hci_revision = bt_dev.hci_version;
	version->lmp_subversion = bt_dev.lmp_subversion;
	memcpy(version_str, version_str_full, 8);
	version_num = strtoul(version_str, NULL, 16);
	version->btgap_revision = version_num >> 16;
	version->btgap_buildnum = version_num & 0xFFFF;

	return 0;
}

static uint16_t bt_stack_le_gap_get_device_addr(void *param)
{
	rtk_bt_le_addr_t *paddr = (rtk_bt_le_addr_t *)param;

	paddr->type = RTK_BT_LE_ADDR_TYPE_PUBLIC;
	memcpy(paddr->addr_val, bt_dev.public_addr.val, BT_ADDR_SIZE);
	return 0;
}

static uint16_t bt_stack_le_gap_set_device_name(void *param)
{
	int err;
	char *name = (char *)param;

	err = bt_set_name(name);
	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	} else {
		return 0;
	}
}

static uint16_t bt_stack_le_gap_set_apperance(void *param)
{
	int err = 0;
	uint16_t *paprc = (uint16_t *)param;

	err = bt_set_appearance(*paprc);

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	} else {
		return 0;
	}
}

static uint16_t _bt_stack_adv_data_parse(bt_zephyr_adv_data *p_data)
{
	uint8_t num = 0;
	uint16_t i = 0;

	while (i < p_data->buf_len) {
		p_data->data[num].data_len = p_data->buf[i] - 1;
		p_data->data[num].type = p_data->buf[i + 1];
		p_data->data[num].data = p_data->buf + i + 2;
		i += (p_data->buf[i] + 1);
		num++;
		if (num == BT_ZEPHYR_ADV_STRUCTRUES_MAX_NUM) {
			return RTK_BT_ERR_PARAM_INVALID;
		}
	}

	p_data->num = num;
	p_data->is_valid = 1;

	return 0;
}

static uint16_t bt_stack_le_gap_set_adv_data(void *param, uint32_t param_len)
{
	memcpy(legacy_ad_buf, param, param_len);
	legacy_ad.buf = legacy_ad_buf;
	legacy_ad.buf_len = param_len;

	return _bt_stack_adv_data_parse(&legacy_ad);
}

static uint16_t bt_stack_le_gap_set_scan_rsp_data(void *param, uint32_t param_len)
{
	memcpy(legacy_sd_buf, param, param_len);
	legacy_sd.buf = legacy_sd_buf;
	legacy_sd.buf_len = param_len;

	return _bt_stack_adv_data_parse(&legacy_sd);
}

extern struct bt_le_ext_adv *adv_get_legacy(void);
static uint16_t bt_stack_le_gap_start_adv(void *param)
{
	rtk_bt_le_adv_param_t *padv_param = (rtk_bt_le_adv_param_t *)param;
	struct bt_le_adv_param le_adv;
	struct bt_data *ad = NULL;
	struct bt_data *sd = NULL;
	size_t ad_len = 0, sd_len = 0;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_adv_start_ind_t *p_le_adv_start_ind = NULL;
	bt_addr_le_t peer_addr;
	int err;

	if (legacy_ad.is_valid) {
		ad = legacy_ad.data;
		ad_len = legacy_ad.num;
	}

	if (legacy_sd.is_valid) {
		sd = legacy_sd.data;
		sd_len = legacy_sd.num;
	}

	memset(&le_adv, 0, sizeof(struct bt_le_adv_param));

	le_adv.interval_min = padv_param->interval_min;
	le_adv.interval_max = padv_param->interval_max;

	/* channel map */
	if (!(padv_param->channel_map & RTK_BT_LE_ADV_CHNL_37)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_37;
	}
	if (!(padv_param->channel_map & RTK_BT_LE_ADV_CHNL_38)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_38;
	}
	if (!(padv_param->channel_map & RTK_BT_LE_ADV_CHNL_39)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_39;
	}

	/* filter policy */
	switch (padv_param->filter_policy) {
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		break;
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_CONN;
		break;
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		le_adv.options |= BT_LE_ADV_OPT_FILTER_CONN;
		break;
	default:
		break;
	}

	switch (padv_param->type) {
	case RTK_BT_LE_ADV_TYPE_IND:
		le_adv.options |= BT_LE_ADV_OPT_CONNECTABLE;
		le_adv.options |= BT_LE_ADV_OPT_ONE_TIME;
		le_adv.options |= BT_LE_ADV_OPT_SCANNABLE;
		break;
	case RTK_BT_LE_ADV_TYPE_DIRECT_IND_HIGH:
		le_adv.options |= BT_LE_ADV_OPT_CONNECTABLE;
		le_adv.options |= BT_LE_ADV_OPT_ONE_TIME;
		break;
	case RTK_BT_LE_ADV_TYPE_SCAN_IND:
		le_adv.options |= BT_LE_ADV_OPT_SCANNABLE;
		break;
	case RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW:
		le_adv.options |= BT_LE_ADV_OPT_CONNECTABLE;
		le_adv.options |= BT_LE_ADV_OPT_ONE_TIME;
		le_adv.options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
		break;
	default:
		break;
	}

	/* own address */
	if (padv_param->own_addr_type == RTK_BT_LE_ADDR_TYPE_PUBLIC) {
		le_adv.options |= BT_LE_ADV_OPT_USE_IDENTITY;
	}
	le_adv.own_addr_type = padv_param->own_addr_type;

	/* directed */
	if (padv_param->type == RTK_BT_LE_ADV_TYPE_DIRECT_IND_HIGH ||
		padv_param->type == RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW) {
		switch (padv_param->peer_addr.type) {
		case RTK_BT_LE_ADDR_TYPE_PUBLIC:
			peer_addr.type = BT_ADDR_LE_PUBLIC;
			break;
		case RTK_BT_LE_ADDR_TYPE_RANDOM:
			peer_addr.type = BT_ADDR_LE_RANDOM;
			break;
		case RTK_BT_LE_ADDR_TYPE_RPA_PUBLIC:
			le_adv.options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
			peer_addr.type = BT_ADDR_LE_PUBLIC_ID;
			break;
		case RTK_BT_LE_ADDR_TYPE_RPA_RANDOM:
			le_adv.options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
			peer_addr.type = BT_ADDR_LE_RANDOM_ID;
			break;
		default:
			break;
		}
		memcpy(peer_addr.a.val, padv_param->peer_addr.addr_val, BT_ADDR_SIZE);
		le_adv.peer = &peer_addr;
	}

	if (!(le_adv.options & BT_LE_ADV_OPT_SCANNABLE)) {
		sd = NULL;
		sd_len = 0;
	}

	if (IS_ENABLED(CONFIG_BT_EXT_ADV) && BT_DEV_FEAT_LE_EXT_ADV(bt_dev.le.features) &&
		padv_param->own_addr_type != RTK_BT_LE_ADDR_TYPE_PUBLIC &&
		!bt_addr_le_eq(&bt_dev.random_addr, BT_ADDR_LE_ANY)) {
		struct bt_le_ext_adv *adv = adv_get_legacy();
		err = bt_id_set_adv_random_addr(adv, &bt_dev.random_addr.a);
		if (err) {
			return zephyr_err_to_rtk(err);
		}
	}

	err = bt_le_adv_start(&le_adv, ad, ad_len, sd, sd_len);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_ADV_START_IND,
								sizeof(rtk_bt_le_adv_start_ind_t));
	if (!p_evt) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	p_le_adv_start_ind = (rtk_bt_le_adv_start_ind_t *)p_evt->data;
	p_le_adv_start_ind->adv_type = padv_param->type;
	p_le_adv_start_ind->err = 0;
	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

static uint16_t bt_stack_le_gap_stop_adv(void)
{
	int err;

	err = bt_le_adv_stop();
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	_indicate_adv_stop(RTK_BT_LE_ADV_STOP_BY_HOST);

	return 0;
}

static void adv_idle_check_func(struct bt_le_ext_adv *adv, void *data)
{
	bool *p_idle = data;

	if (atomic_test_bit(adv->flags, BT_ADV_ENABLED)) {
		*p_idle = false;
	}
}

static bool bt_stack_le_gap_adv_is_idle(void)
{
	bool idle = true;

	bt_le_ext_adv_foreach(adv_idle_check_func, &idle);
	return idle;
}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
typedef struct {
	struct bt_le_ext_adv *adv;
	bt_zephyr_adv_data ad;
	bt_zephyr_adv_data sd;
} _ext_adv_info_t;

static _ext_adv_info_t _ext_adv_tbl[CONFIG_BT_EXT_ADV_MAX_ADV_SET] = {0};

static void _indicate_ext_adv(uint8_t handle, bool is_start, uint8_t stop_reason)
{
	rtk_bt_le_ext_adv_ind_t *p_ext_adv_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_EXT_ADV_IND,
								sizeof(rtk_bt_le_ext_adv_ind_t));
	if (!p_evt) {
		return;
	}

	p_ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)p_evt->data;
	p_ext_adv_ind->adv_handle = handle;
	p_ext_adv_ind->is_start = is_start;
	p_ext_adv_ind->err = 0;
	p_ext_adv_ind->stop_reason = stop_reason;

	rtk_bt_evt_indicate(p_evt, NULL);
}

static void ext_adv_sent(struct bt_le_ext_adv *adv, struct bt_le_ext_adv_sent_info *info)
{
	(void)info;
	uint8_t id = bt_le_ext_adv_get_index(adv);

	_indicate_ext_adv(id, false, RTK_BT_LE_ADV_STOP_BY_DURATION);
}

void ext_adv_connected(struct bt_le_ext_adv *adv, struct bt_le_ext_adv_connected_info *info)
{
	(void)info;
	uint8_t id = bt_le_ext_adv_get_index(adv);

	_indicate_ext_adv(id, false, RTK_BT_LE_ADV_STOP_BY_CONN);
}

#if defined(CONFIG_BT_PRIVACY)
bool ext_adv_rpa_expired(struct bt_le_ext_adv *adv)
{
	(void)adv;
	return true;
}
#endif

#if defined(CONFIG_BT_PER_ADV_RSP) && defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
void ext_adv_pawr_data_request(struct bt_le_ext_adv *adv, const struct bt_le_per_adv_data_request *request)
{
	rtk_bt_le_gap_pawr_data_req_ind_t *p_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PAWR_DATA_REQ_IND,
								sizeof(rtk_bt_le_gap_pawr_data_req_ind_t));
	if (!p_evt) {
		return;
	}

	p_ind = (rtk_bt_le_gap_pawr_data_req_ind_t *)p_evt->data;
	p_ind->adv_handle = adv->handle;
	p_ind->start = request->start;
	p_ind->count = request->count;

	rtk_bt_evt_indicate(p_evt, NULL);
}

void ext_adv_pawr_response(struct bt_le_ext_adv *adv, struct bt_le_per_adv_response_info *info, struct net_buf_simple *buf)
{
	rtk_bt_le_gap_pawr_rsp_ind_t *p_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	uint16_t len = 0;

	if (buf) {
		len = buf->len;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PAWR_RSP_IND,
								sizeof(rtk_bt_le_gap_pawr_rsp_ind_t) + len);
	if (!p_evt) {
		return;
	}

	p_ind = (rtk_bt_le_gap_pawr_rsp_ind_t *)p_evt->data;
	p_ind->adv_handle = adv->handle;
	p_ind->subevent = info->subevent;
	p_ind->tx_status = info->tx_status;
	p_ind->tx_power = info->tx_power;
	p_ind->rssi = info->rssi;
	p_ind->cte_type = info->cte_type;
	p_ind->len = len;
	if (len) {
		p_ind->data = (uint8_t *)p_evt->data + sizeof(rtk_bt_le_gap_pawr_rsp_ind_t);
		memcpy(p_ind->data, buf->data, len);
	}

	rtk_bt_evt_indicate(p_evt, NULL);
}
#endif

static struct bt_le_ext_adv_cb bt_zephyr_ext_adv_cb = {
	.sent = ext_adv_sent,
	.connected = ext_adv_connected,
#if defined(CONFIG_BT_PRIVACY)
	.rpa_expired = ext_adv_rpa_expired,
#endif
#if defined(CONFIG_BT_PER_ADV_RSP) && defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	.pawr_data_request = ext_adv_pawr_data_request,
	.pawr_response = ext_adv_pawr_response,
#endif
};

static bool bt_stack_le_gap_ext_adv_handle_valid(uint8_t handle)
{
	if (handle < CONFIG_BT_EXT_ADV_MAX_ADV_SET && _ext_adv_tbl[handle].adv) {
		return true;
	}

	return false;
}

static uint16_t bt_stack_le_gap_set_ext_adv_data(void *param)
{
	rtk_bt_le_ext_adv_data_t *padv_data = (rtk_bt_le_ext_adv_data_t *)param;

	if (!bt_stack_le_gap_ext_adv_handle_valid(padv_data->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	BT_LOGD("bt_stack_le_gap_set_ext_adv_data: handle = %d, len = %d\r\n", padv_data->adv_handle, padv_data->len);

	_ext_adv_tbl[padv_data->adv_handle].ad.buf = padv_data->pdata;
	_ext_adv_tbl[padv_data->adv_handle].ad.buf_len = padv_data->len;

	return _bt_stack_adv_data_parse(&_ext_adv_tbl[padv_data->adv_handle].ad);
}

static uint16_t bt_stack_le_gap_set_ext_scan_rsp_data(void *param)
{
	rtk_bt_le_ext_adv_data_t *padv_data = (rtk_bt_le_ext_adv_data_t *)param;

	if (!bt_stack_le_gap_ext_adv_handle_valid(padv_data->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	BT_LOGD("bt_stack_le_gap_set_ext_scan_rsp_data: handle = %d, len = %d\r\n", padv_data->adv_handle, padv_data->len);

	_ext_adv_tbl[padv_data->adv_handle].sd.buf = padv_data->pdata;
	_ext_adv_tbl[padv_data->adv_handle].sd.buf_len = padv_data->len;

	return _bt_stack_adv_data_parse(&_ext_adv_tbl[padv_data->adv_handle].sd);
}

static uint16_t bt_stack_le_gap_create_ext_adv(void *param)
{
	rtk_bt_le_ext_adv_create_t *p_create = (rtk_bt_le_ext_adv_create_t *)param;
	rtk_bt_le_ext_adv_param_t *padv_param = p_create->p_adv_param;
	uint8_t id = 0;
	int err = 0;
	struct bt_le_adv_param le_adv;
	bt_addr_le_t peer_addr;
	bt_addr_t own_addr;
	struct bt_le_ext_adv *adv = NULL;

	memset(&le_adv, 0, sizeof(struct bt_le_adv_param));

	le_adv.id = id;
	le_adv.sid = padv_param->adv_sid;
	le_adv.secondary_max_skip = padv_param->secondary_adv_max_skip;
	le_adv.interval_min = padv_param->primary_adv_interval_min;
	le_adv.interval_max = padv_param->primary_adv_interval_max;

	/* adv event properties */
	if (padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_CONN) {
		le_adv.options |= BT_LE_ADV_OPT_CONNECTABLE;
		le_adv.options |= BT_LE_ADV_OPT_ONE_TIME;
	}
	if (padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_SCAN) {
		le_adv.options |= BT_LE_ADV_OPT_SCANNABLE;
	}
	if ((padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_DIRECT) &&
		!(padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_HI_DC_CONN)) {
		le_adv.options |= BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY;
	}
	if (!(padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_LEGACY)) {
		le_adv.options |= BT_LE_ADV_OPT_EXT_ADV;
	}

	/* PHY */
	if (padv_param->primary_adv_phy == RTK_BT_LE_PHYS_PRIM_ADV_CODED &&
		padv_param->secondary_adv_phy == RTK_BT_LE_PHYS_CODED) {
		le_adv.options |= BT_LE_ADV_OPT_CODED;
	} else if (padv_param->primary_adv_phy == RTK_BT_LE_PHYS_PRIM_ADV_1M &&
			   padv_param->secondary_adv_phy == RTK_BT_LE_PHYS_1M) {
		le_adv.options |= BT_LE_ADV_OPT_NO_2M;
	} else if (!(padv_param->primary_adv_phy == RTK_BT_LE_PHYS_PRIM_ADV_1M &&
				 padv_param->secondary_adv_phy == RTK_BT_LE_PHYS_2M)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	/* channel map */
	if (!(padv_param->primary_adv_channel_map & RTK_BT_LE_ADV_CHNL_37)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_37;
	}
	if (!(padv_param->primary_adv_channel_map & RTK_BT_LE_ADV_CHNL_38)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_38;
	}
	if (!(padv_param->primary_adv_channel_map & RTK_BT_LE_ADV_CHNL_39)) {
		le_adv.options |= BT_LE_ADV_OPT_DISABLE_CHAN_39;
	}

	/* filter policy */
	switch (padv_param->filter_policy) {
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		break;
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_CONN;
		break;
	case RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST:
		le_adv.options |= BT_LE_ADV_OPT_FILTER_SCAN_REQ;
		le_adv.options |= BT_LE_ADV_OPT_FILTER_CONN;
		break;
	default:
		break;
	}

	// /* TX power */
	// if (padv_param->tx_power != 0x7F) {
	//  le_adv.options |= BT_LE_ADV_OPT_USE_TX_POWER;
	// }

	/* own address */
	if (padv_param->own_addr.type == RTK_BT_LE_ADDR_TYPE_PUBLIC) {
		le_adv.options |= BT_LE_ADV_OPT_USE_IDENTITY;
	}
	le_adv.own_addr_type = padv_param->own_addr.type;

	/* directed */
	if (padv_param->adv_event_prop & BT_HCI_LE_ADV_PROP_DIRECT) {
		switch (padv_param->peer_addr.type) {
		case RTK_BT_LE_ADDR_TYPE_PUBLIC:
			peer_addr.type = BT_ADDR_LE_PUBLIC;
			break;
		case RTK_BT_LE_ADDR_TYPE_RANDOM:
			peer_addr.type = BT_ADDR_LE_RANDOM;
			break;
		case RTK_BT_LE_ADDR_TYPE_RPA_PUBLIC:
			le_adv.options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
			peer_addr.type = BT_ADDR_LE_PUBLIC_ID;
			break;
		case RTK_BT_LE_ADDR_TYPE_RPA_RANDOM:
			le_adv.options |= BT_LE_ADV_OPT_DIR_ADDR_RPA;
			peer_addr.type = BT_ADDR_LE_RANDOM_ID;
			break;
		default:
			break;
		}
		memcpy(peer_addr.a.val, padv_param->peer_addr.addr_val, BT_ADDR_SIZE);
		le_adv.peer = &peer_addr;
	}

	err = bt_le_ext_adv_create(&le_adv, &bt_zephyr_ext_adv_cb, &adv);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	if (padv_param->own_addr.type == RTK_BT_LE_ADDR_TYPE_RANDOM ||
		padv_param->own_addr.type == RTK_BT_LE_ADDR_TYPE_RPA_RANDOM) {
		memcpy(own_addr.val, padv_param->own_addr.addr_val, BT_ADDR_SIZE);
		err = bt_id_set_adv_random_addr(adv, &own_addr);
		if (err) {
			return zephyr_err_to_rtk(err);
		}
	}

	id = bt_le_ext_adv_get_index(adv);
	_ext_adv_tbl[id].adv = adv;
	*p_create->p_adv_handle = id;
	return 0;
}

static uint16_t bt_stack_le_gap_start_ext_adv(void *param)
{
	rtk_bt_le_ext_adv_start_t *p_start = (rtk_bt_le_ext_adv_start_t *)param;
	struct bt_le_ext_adv_start_param p_eadv_start;
	_ext_adv_info_t *p_adv;
	struct bt_data *ad = NULL;
	struct bt_data *sd = NULL;
	size_t ad_len = 0, sd_len = 0;
	int err;

	if (!bt_stack_le_gap_ext_adv_handle_valid(p_start->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_adv = &_ext_adv_tbl[p_start->adv_handle];

	if (p_adv->ad.is_valid) {
		ad = p_adv->ad.data;
		ad_len = p_adv->ad.num;
	}

	if (p_adv->sd.is_valid) {
		sd = p_adv->sd.data;
		sd_len = p_adv->sd.num;
	}

	err = bt_le_ext_adv_set_data(p_adv->adv, ad, ad_len, sd, sd_len);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	p_eadv_start.timeout = p_start->duration;
	p_eadv_start.num_events = p_start->num_events;

	err = bt_le_ext_adv_start(p_adv->adv, &p_eadv_start);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	_indicate_ext_adv(p_start->adv_handle, true, 0);

	return 0;
}

static uint16_t bt_stack_le_gap_stop_ext_adv(void *param)
{
	uint8_t id = *((uint8_t *)param);
	int err;

	if (!bt_stack_le_gap_ext_adv_handle_valid(id)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	err = bt_le_ext_adv_stop(_ext_adv_tbl[id].adv);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	_indicate_ext_adv(id, false, RTK_BT_LE_ADV_STOP_BY_HOST);

	return 0;
}

static uint16_t bt_stack_le_gap_remove_ext_adv(void *param)
{
	uint8_t id = *((uint8_t *)param);
	int err = 0;

	if (!bt_stack_le_gap_ext_adv_handle_valid(id)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	err = bt_le_ext_adv_delete(_ext_adv_tbl[id].adv);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	_ext_adv_tbl[id].adv = NULL;
	_ext_adv_tbl[id].ad.is_valid = 0;
	_ext_adv_tbl[id].sd.is_valid = 0;

	return 0;
}
#endif

static uint16_t bt_stack_le_gap_set_scan_param(void *param)
{
	rtk_bt_le_scan_param_t *p_param = (rtk_bt_le_scan_param_t *)param;

	memset(&scan_param, 0, sizeof(struct bt_le_scan_param));
	bt_dev.scan_own_addr_type = p_param->own_addr_type;
	scan_param.interval = p_param->interval;
	scan_param.window = p_param->window;
	scan_param.type = p_param->type;

	if ((p_param->duplicate_opt == RTK_BT_LE_SCAN_DUPLICATE_ENABLE) || (p_param->duplicate_opt == RTK_BT_LE_SCAN_DUPLICATE_ENABLED_RESET_FOR_EACH_PERIOD)) {
		scan_param.options |= BT_LE_SCAN_OPT_FILTER_DUPLICATE;
	}

	if (p_param->filter_policy == RTK_BT_LE_SCAN_FILTER_ALLOW_ONLY_WLST || p_param->filter_policy == RTK_BT_LE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR) {
		scan_param.options |= BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_start_scan(bool is_ext)
{
	int err;

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	extern uint8_t rtk_bt_mesh_stack_set_scan_switch(bool scan_switch);
	if (rtk_bt_mesh_is_enable()) {
		rtk_bt_mesh_stack_set_scan_switch(true);
	} else
#endif
	{
		err = bt_le_scan_start(&scan_param, NULL);
		if (err) {
			return zephyr_err_to_rtk(err);
		}

		_indicate_scan_start();
	}

	if (is_ext) {
		zephyr_scan_state = ZEPHYR_SCAN_STATE_EXT;
	} else {
		zephyr_scan_state = ZEPHYR_SCAN_STATE_LEGACY;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_stop_scan(void)
{
	int err;

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	extern uint8_t rtk_bt_mesh_stack_set_scan_switch(bool scan_switch);
	if (rtk_bt_mesh_is_enable()) {
		rtk_bt_mesh_stack_set_scan_switch(false);
	} else
#endif
	{
		err = bt_le_scan_stop();
		if (err) {
			return zephyr_err_to_rtk(err);
		}

		_indicate_scan_stop(RTK_BT_LE_SCAN_STOP_BY_HOST);
	}

	return 0;
}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint16_t bt_stack_le_gap_ext_scan_set_param(void *param)
{
	rtk_bt_le_ext_scan_param_t *p_param = (rtk_bt_le_ext_scan_param_t *)param;

	if (p_param->phys[0] == false && p_param->phys[1] == false) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(&scan_param, 0, sizeof(struct bt_le_scan_param));
	bt_dev.scan_own_addr_type = p_param->own_addr_type;
	scan_param.timeout = p_param->duration;

	if ((p_param->duplicate_opt == RTK_BT_LE_SCAN_DUPLICATE_ENABLE) || (p_param->duplicate_opt == RTK_BT_LE_SCAN_DUPLICATE_ENABLED_RESET_FOR_EACH_PERIOD)) {
		scan_param.options |= BT_LE_SCAN_OPT_FILTER_DUPLICATE;
	}

	if (p_param->filter_policy == RTK_BT_LE_SCAN_FILTER_ALLOW_ONLY_WLST || p_param->filter_policy == RTK_BT_LE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR) {
		scan_param.options |= BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST;
	}

	scan_param.type = p_param->phys[0] ? p_param->type[0] : p_param->type[1];

	if (!p_param->phys[0]) {
		scan_param.options |= BT_LE_SCAN_OPT_NO_1M;
	}
	scan_param.interval = p_param->interval[0];
	scan_param.window = p_param->window[0];

	if (p_param->phys[1]) {
		scan_param.options |= BT_LE_SCAN_OPT_CODED;
		scan_param.interval_coded = p_param->interval[1];
		scan_param.window_coded = p_param->window[1];
	}

	return 0;
}
#endif

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint16_t bt_stack_le_gap_ext_connect(void *param)
{
	rtk_bt_le_ext_create_conn_param_t *p_ext_conn_param = (rtk_bt_le_ext_create_conn_param_t *)param;
	bt_addr_le_t peer_addr = {};
	struct bt_conn_le_create_param le_create_param = {0};
	struct bt_le_conn_param le_conn_param = {0};
	uint32_t i = 0;
	int err = 0;
	struct bt_conn *conn = NULL;

	peer_addr.type = p_ext_conn_param->peer_addr.type;
	memcpy(peer_addr.a.val, p_ext_conn_param->peer_addr.addr_val, BT_ADDR_SIZE);

	memset(&le_create_param, 0, sizeof(struct bt_conn_le_create_param));
	memset(&le_conn_param, 0, sizeof(struct bt_le_conn_param));

	if (!p_ext_conn_param->init_phys[0]) {
		le_create_param.options |= BT_CONN_LE_OPT_NO_1M;
	}

	for (i = 0; i < 3; i++) {
		if (p_ext_conn_param->init_phys[i]) {
			if (i == 2) {
				le_create_param.interval_coded = p_ext_conn_param->scan_interval[i];
				le_create_param.window_coded = p_ext_conn_param->scan_window[i];
				le_create_param.options |= BT_CONN_LE_OPT_CODED;
			} else {
				le_create_param.interval = p_ext_conn_param->scan_interval[i];
				le_create_param.window = p_ext_conn_param->scan_window[i];
			}

			le_conn_param.interval_max = p_ext_conn_param->conn_interval_max[i];
			le_conn_param.interval_min = p_ext_conn_param->conn_interval_min[i];
			le_conn_param.latency = p_ext_conn_param->conn_latency[i];
			le_conn_param.timeout = p_ext_conn_param->supv_timeout[i];

		}
	}

	le_create_param.own_addr_type = p_ext_conn_param->own_addr_type;
	le_create_param.timeout = p_ext_conn_param->scan_timeout;

	if (p_ext_conn_param->filter_policy == RTK_BT_LE_CONN_FILTER_WITH_WHITELIST) {
		err = bt_conn_le_create_auto(&le_create_param, &le_conn_param);
	} else {
		err = bt_conn_le_create(&peer_addr, &le_create_param, &le_conn_param, &conn);
	}

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	if (conn) {
		bt_conn_unref(conn);
	}

	return 0;
}
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
static uint16_t bt_stack_le_gap_start_pa(void *p_param)
{
	rtk_bt_le_pa_param_t *param = (rtk_bt_le_pa_param_t *)p_param;
	struct bt_data ad_data[50];
	uint32_t ad_num = 0;
	uint32_t i = 0;
	struct bt_le_ext_adv *p_ext_adv = NULL;
	struct bt_le_per_adv_param per_adv_param = {0};
	int err = 0;
	rtk_bt_le_pa_ind_t *p_pa_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	if (!bt_stack_le_gap_ext_adv_handle_valid(param->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_ext_adv = _ext_adv_tbl[param->adv_handle].adv;
	while (i < param->pa_len) {
		if (i + param->pa_data[i] + 1 > param->pa_len) {
			break;
		}

		ad_data[ad_num].data_len = (param->pa_data[i] - 1);
		ad_data[ad_num].type = param->pa_data[i + 1];
		ad_data[ad_num].data = param->pa_data + i + 2;
		i += (ad_data[ad_num].data_len + 2);
		ad_num ++;
	}


	per_adv_param.interval_min = param->pa_interval_min;
	per_adv_param.interval_max = param->pa_interval_max;

	if (param->pa_prop & 0x40) {
		per_adv_param.options |= BT_LE_PER_ADV_OPT_USE_TX_POWER;
	}

	per_adv_param.options |= BT_LE_PER_ADV_OPT_INCLUDE_ADI;

#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
	per_adv_param.num_subevents = param->num_subevents;
	per_adv_param.subevent_interval = param->subevent_interval;
	per_adv_param.response_slot_delay = param->response_slot_delay;
	per_adv_param.response_slot_spacing = param->response_slot_spacing;
	per_adv_param.num_response_slots = param->num_response_slots;
#endif

	err = bt_le_per_adv_set_param(p_ext_adv, &per_adv_param);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	err = bt_le_per_adv_set_data(p_ext_adv, ad_data, ad_num);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	err = bt_le_per_adv_start(p_ext_adv);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PA_IND,
								sizeof(rtk_bt_le_pa_ind_t));
	if (!p_evt) {
		return 0;
	}

	p_pa_ind = (rtk_bt_le_pa_ind_t *)p_evt->data;
	p_pa_ind->adv_handle = param->adv_handle;
	p_pa_ind->cause = 0;
	p_pa_ind->state = RTK_BT_LE_PA_STATE_ADVERTISING;

	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

static uint16_t bt_stack_le_gap_stop_pa(void *param)
{
	uint8_t id = *((uint8_t *)param);
	rtk_bt_le_pa_ind_t *p_pa_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	int err = 0;

	if (!bt_stack_le_gap_ext_adv_handle_valid(id)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	err = bt_le_per_adv_stop(_ext_adv_tbl[id].adv);
	if (err) {
		return zephyr_err_to_rtk(err);
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_PA_IND, sizeof(rtk_bt_le_pa_ind_t));
	if (!p_evt) {
		return 0;
	}
	p_pa_ind = (rtk_bt_le_pa_ind_t *)p_evt->data;
	p_pa_ind->adv_handle = id;
	p_pa_ind->cause = 0;
	p_pa_ind->state = RTK_BT_LE_PA_STATE_IDLE;
	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

static uint16_t bt_stack_le_gap_update_pa(void *param)
{
	rtk_bt_le_pa_update_t *p_param = (rtk_bt_le_pa_update_t *)param;
	struct bt_le_ext_adv *p_ext_adv = NULL;
	struct bt_data ad_data[1];
	uint8_t *data = p_param->pa_data;
	uint8_t data_len = p_param->pa_len;
	int err = 0;

	if (!bt_stack_le_gap_ext_adv_handle_valid(p_param->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	p_ext_adv = _ext_adv_tbl[p_param->adv_handle].adv;

	ad_data[0].data_len = data_len;
	ad_data[0].type = RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC;
	ad_data[0].data = data;


	err = bt_le_per_adv_set_data(p_ext_adv, ad_data, 1);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
static uint16_t bt_stack_le_gap_pa_sync_create(void *param)
{
	rtk_bt_le_pa_sync_create_t *ptr = (rtk_bt_le_pa_sync_create_t *)param;
	struct bt_le_per_adv_sync_param create = {0};
	int err = 0;
	struct bt_le_per_adv_sync *out_sync;

	create.addr.type = ptr->adv_addr.type;
	memcpy(create.addr.a.val, ptr->adv_addr.addr_val, BT_ADDR_SIZE);
	create.sid = ptr->adv_sid;
	create.skip = ptr->skip;
	create.timeout = ptr->sync_timeout;

	if (ptr->options & RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_USE_PERIODIC_ADV_LIST) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_USE_PER_ADV_LIST;
	}
	if (ptr->options & RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_REPORT_INITIALLY_DISABLED) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_REPORTING_INITIALLY_DISABLED;
	}
	if (ptr->options & RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_DUPLICATE_FILTER_INITIALLY_ENABLED) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_FILTER_DUPLICATE;
	}

	if (ptr->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOA_CTE) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOA;
	}
	if (ptr->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_1US) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOD_1US;
	}
	if (ptr->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_2US) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_DONT_SYNC_AOD_2US;
	}
	if (ptr->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITHOUT_CTE) {
		create.options |= BT_LE_PER_ADV_SYNC_OPT_SYNC_ONLY_CONST_TONE_EXT;
	}

	err = bt_le_per_adv_sync_create(&create, &out_sync);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	*ptr->p_sync_id = bt_le_per_adv_sync_get_index(out_sync);
	return 0;
}

static uint16_t bt_stack_le_gap_pa_sync_terminate(void *param)
{
	uint8_t sync_id = *(uint8_t *)param;
	struct bt_le_per_adv_sync *sync;
	int err = 0;

	sync = bt_le_per_adv_sync_lookup_index(sync_id);
	if (!sync) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	err = bt_le_per_adv_sync_delete(sync);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_pa_sync_modify_adv_list(void *param)
{
	rtk_bt_le_pa_sync_modify_advlist_t *ptr = (rtk_bt_le_pa_sync_modify_advlist_t *)param;
	int err = 0;
	bt_addr_le_t addr = {0};

	switch (ptr->operation) {
	case 0x00:
		err = bt_le_per_adv_list_clear();
		break;
	case 0x01:
		addr.type = ptr->adv_addr.type;
		memcpy(addr.a.val, ptr->adv_addr.addr_val, BT_ADDR_SIZE);
		err = bt_le_per_adv_list_add(&addr, ptr->adv_sid);
		break;
	case 0x02:
		addr.type = ptr->adv_addr.type;
		memcpy(addr.a.val, ptr->adv_addr.addr_val, BT_ADDR_SIZE);
		err = bt_le_per_adv_list_remove(&addr, ptr->adv_sid);
		break;
	default:
		break;
	}

	return zephyr_err_to_rtk(err);
}
#endif

#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
static uint16_t bt_stack_le_gap_past_send(void *param)
{
	rtk_bt_le_past_send_t *p_param = (rtk_bt_le_past_send_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(p_param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (p_param->use_sync_id) {
		uint8_t sync_id = p_param->idx;
		struct bt_le_per_adv_sync *sync = bt_le_per_adv_sync_lookup_index(sync_id);

		if (!sync) {
			return RTK_BT_ERR_NO_ENTRY;
		}

		BT_LOGA("%s %d %d\r\n", __func__, sync_id, p_param->conn_handle);
		err = bt_le_per_adv_sync_transfer(sync, conn, p_param->service_data);
	} else {
		struct bt_le_ext_adv *adv = NULL;
		uint8_t adv_handle = p_param->idx;
		if (bt_stack_le_gap_ext_adv_handle_valid(adv_handle)) {
			adv = _ext_adv_tbl[adv_handle].adv;
		} else {
			return RTK_BT_ERR_NO_ENTRY;
		}

		err = bt_le_per_adv_set_info_transfer(adv, conn, p_param->service_data);
	}

	bt_conn_unref(conn);

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
static uint16_t bt_stack_le_gap_past_recv_set(void *param)
{
	rtk_bt_le_past_recv_param_t *p_param = (rtk_bt_le_past_recv_param_t *)param;
	struct bt_conn *conn = NULL;
	struct bt_le_per_adv_sync_transfer_param past_param = {0};
	int err = 0;

	conn = bt_conn_lookup_handle(p_param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_NO_ATTEMPT_TO_SYNCHRONIZE) {
		err = bt_le_per_adv_sync_transfer_unsubscribe(conn);
	} else {
		past_param.skip = p_param->skip;
		past_param.timeout = p_param->sync_timeout;

		if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_DISABLED) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_REPORTING_INITIALLY_DISABLED;
		}
		if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_FILTER_DUPLICATES;
		}

		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOA_CTE) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOA;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_1US) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_1US;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_2US) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_2US;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITHOUT_CTE) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_ONLY_CTE;
		}
		err = bt_le_per_adv_sync_transfer_subscribe(conn, &past_param);
	}

	bt_conn_unref(conn);

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_default_past_recv_set(void *param)
{
	rtk_bt_le_past_recv_param_t *p_param = (rtk_bt_le_past_recv_param_t *)param;
	struct bt_le_per_adv_sync_transfer_param past_param = {0};
	int err = 0;

	if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_NO_ATTEMPT_TO_SYNCHRONIZE) {
		err = bt_le_per_adv_sync_transfer_unsubscribe(NULL);
	} else {
		past_param.skip = p_param->skip;
		past_param.timeout = p_param->sync_timeout;

		if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_DISABLED) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_REPORTING_INITIALLY_DISABLED;
		}
		if (p_param->mode == RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_FILTER_DUPLICATES;
		}

		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOA_CTE) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOA;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_1US) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_1US;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITH_AOD_CTE_2US) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_NO_AOD_2US;
		}
		if (p_param->sync_cte_type & RTK_BT_LE_PA_SYNC_CTE_TYPE_NOT_SYNC_WITHOUT_CTE) {
			past_param.options |= BT_LE_PER_ADV_SYNC_TRANSFER_OPT_SYNC_ONLY_CTE;
		}


		err = bt_le_per_adv_sync_transfer_subscribe(NULL, &past_param);
	}

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	} else {
		return 0;
	}
}


#endif
static uint16_t bt_stack_le_gap_connect(void *param)
{
	rtk_bt_le_create_conn_param_t *p_conn_param = (rtk_bt_le_create_conn_param_t *)param;
	bt_addr_le_t peer_addr = {};
	struct bt_conn_le_create_param le_create_param = {};
	struct bt_le_conn_param le_conn_param = {};
	struct bt_conn *conn = NULL;
	int err = 0;

	peer_addr.type = p_conn_param->peer_addr.type;
	memcpy(peer_addr.a.val, p_conn_param->peer_addr.addr_val, BT_ADDR_SIZE);

	le_create_param.own_addr_type = p_conn_param->own_addr_type;
	le_create_param.interval = p_conn_param->scan_interval;
	le_create_param.interval_coded = p_conn_param->scan_interval;
	le_create_param.window = p_conn_param->scan_window;
	le_create_param.window_coded = p_conn_param->scan_window;
	le_create_param.timeout = p_conn_param->scan_timeout;

	le_conn_param.interval_max = p_conn_param->conn_interval_max;
	le_conn_param.interval_min = p_conn_param->conn_interval_min;
	le_conn_param.latency = p_conn_param->conn_latency;
	le_conn_param.timeout = p_conn_param->supv_timeout;

	if (p_conn_param->filter_policy == RTK_BT_LE_CONN_FILTER_WITH_WHITELIST) {
		err = bt_conn_le_create_auto(&le_create_param, &le_conn_param);
	} else {
		err = bt_conn_le_create(&peer_addr, &le_create_param, &le_conn_param, &conn);
	}

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	if (conn) {
		bt_conn_unref(conn);
	}
	return 0;
}

static uint16_t bt_stack_le_gap_disconnect(void *param)
{
	uint16_t conn_handle = *(uint16_t *)param;
	struct bt_conn *conn = NULL;
	int err;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_disconnect_with_reason(void *param)
{
	rtk_bt_le_disconn_with_reason_param_t *disconn_with_reason = (rtk_bt_le_disconn_with_reason_param_t *)param;
	struct bt_conn *conn = NULL;
	int err;

	conn = bt_conn_lookup_handle(disconn_with_reason->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_conn_disconnect(conn, disconn_with_reason->reason);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_update_conn_param(void *param)
{
	rtk_bt_le_update_conn_param_t *p_update_conn_param = (rtk_bt_le_update_conn_param_t *)param;
	struct bt_le_conn_param conn_le_update_param = {};
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(p_update_conn_param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	conn_le_update_param.interval_max = p_update_conn_param->conn_interval_max;
	conn_le_update_param.interval_min = p_update_conn_param->conn_interval_min;
	conn_le_update_param.latency = p_update_conn_param->conn_latency;
	conn_le_update_param.timeout = p_update_conn_param->supv_timeout;

	err = bt_conn_le_param_update(conn, &conn_le_update_param);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_modify_whitelist(void *param)
{
	int err = 0;
	rtk_bt_le_modify_wl_param_t *p_wl_op_param = (rtk_bt_le_modify_wl_param_t *)param;
	bt_addr_le_t addr = {};

	switch (p_wl_op_param->op) {
	case RTK_BT_LE_WHITELIST_ADD:
		addr.type = p_wl_op_param->addr.type;
		memcpy(addr.a.val, p_wl_op_param->addr.addr_val, BT_ADDR_SIZE);
		err = bt_le_filter_accept_list_add(&addr);
		break;
	case RTK_BT_LE_WHITELIST_REMOVE:
		addr.type = p_wl_op_param->addr.type;
		memcpy(addr.a.val, p_wl_op_param->addr.addr_val, BT_ADDR_SIZE);
		err = bt_le_filter_accept_list_remove(&addr);
		break;
	case RTK_BT_LE_WHITELIST_CLEAR:
		err = bt_le_filter_accept_list_clear();
		break;
	default:
		break;
	}

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_get_conn_info(void *param)
{
	rtk_bt_le_get_conn_info_param_t *get_info = (rtk_bt_le_get_conn_info_param_t *)param;
	rtk_bt_le_conn_info_t *info = get_info->p_conn_info;
	struct bt_conn *conn = NULL;
	struct bt_conn_info conn_info = {};
	int err = 0;

	conn = bt_conn_lookup_handle(get_info->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_conn_get_info(conn, &conn_info);
	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	memset(info, 0, sizeof(rtk_bt_le_conn_info_t));

	if (BT_CONN_ROLE_CENTRAL == conn_info.role) {
		info->role = RTK_BT_LE_ROLE_MASTER;
	} else if (BT_CONN_ROLE_PERIPHERAL == conn_info.role) {
		info->role = RTK_BT_LE_ROLE_SLAVE;
	} else {
		info->role = RTK_BT_LE_ROLE_UNKNOWN;
	}

	info->remote.type = conn_info.le.remote->type;
	memcpy(info->remote.addr_val, conn_info.le.remote->a.val, BT_ADDR_SIZE);

	info->interval = conn_info.le.interval;
	info->latency = conn_info.le.latency;
	info->supv_timeout = conn_info.le.timeout;

#if defined(CONFIG_BT_USER_PHY_UPDATE)
	info->tx_phy = conn_info.le.phy->tx_phy;
	info->rx_phy = conn_info.le.phy->rx_phy;
#endif

	return 0;
}

static uint16_t bt_stack_le_gap_get_dev_state(void *param)
{
	struct bt_conn *conn;
	rtk_bt_le_gap_dev_state_t *gap_state = (rtk_bt_le_gap_dev_state_t *)param;

	gap_state->gap_init_state = atomic_test_bit(bt_dev.flags, BT_DEV_READY) ? RTK_BT_LE_GAP_INIT_STATE_STACK_READY : RTK_BT_LE_GAP_INIT_STATE_INIT;

	gap_state->gap_adv_state = bt_stack_le_gap_adv_is_idle() ? RTK_BT_LE_ADV_STATE_IDLE : RTK_BT_LE_ADV_STATE_ADVERTISING;

	gap_state->gap_scan_state = atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING) ? RTK_BT_LE_SCAN_STATE_SCANNING : RTK_BT_LE_SCAN_STATE_IDLE;

	gap_state->gap_conn_state = RTK_BT_LE_CONN_STATE_IDLE;
	conn = bt_conn_lookup_state_le(BT_ID_DEFAULT, NULL, BT_CONN_INITIATING);
	if (conn) {
		bt_conn_unref(conn);
		gap_state->gap_conn_state = RTK_BT_LE_CONN_STATE_INITIATING;
	}

	return 0;
}

static int le_create_rand_addr(rtk_bt_le_rand_addr_type_t rand_addr_type, bt_addr_le_t *rand_addr)
{
	if (RTK_BT_LE_RAND_ADDR_STATIC == rand_addr_type) {
		return bt_addr_le_create_static(rand_addr);
	} else if (RTK_BT_LE_RAND_ADDR_NON_RESOLVABLE == rand_addr_type) {
		return bt_addr_le_create_nrpa(rand_addr);
	} else if (RTK_BT_LE_RAND_ADDR_RESOLVABLE == rand_addr_type) {
		rand_addr->type = BT_ADDR_LE_RANDOM;
		return bt_rpa_create(bt_dev.irk[BT_ID_DEFAULT], &rand_addr->a);
	}

	return -ENOENT;
}

static rtk_bt_le_rand_addr_type_t bt_stack_rand_addr_type(uint8_t *addr_val)
{
	uint8_t flag_bits = ((addr_val[5] >> 6) & 0x3);

	if (3 == flag_bits) {
		return RTK_BT_LE_RAND_ADDR_STATIC;
	} else if (0 == flag_bits) {
		return RTK_BT_LE_RAND_ADDR_NON_RESOLVABLE;
	} else if (1 == flag_bits) {
		return RTK_BT_LE_RAND_ADDR_RESOLVABLE;
	} else {
		return (rtk_bt_le_rand_addr_type_t)0xFF;
	}
}

static void le_cfg_local_ident_addr(uint8_t *addr_val, rtk_bt_le_ident_addr_type_t ident_addr_type)
{
	bt_dev.id_addr[BT_ID_DEFAULT].type = (ident_addr_type == RTK_BT_LE_IDENT_ADDR_PUBLIC ? 0 : 1);
	memcpy(bt_dev.id_addr[BT_ID_DEFAULT].a.val, addr_val, BT_ADDR_SIZE);
}

static uint16_t bt_stack_le_gap_set_rand_addr(void *param)
{
	rtk_bt_le_set_rand_addr_t *p_param = (rtk_bt_le_set_rand_addr_t *)param;
	struct net_buf *buf;
	bt_addr_le_t addr = {};
	int err;

	if (p_param->auto_generate) {
		err = le_create_rand_addr(p_param->type, &addr);
		if (err) {
			return zephyr_err_to_rtk(err);
		}
		memcpy(p_param->addr_val, addr.a.val, BT_ADDR_SIZE);
	} else {
		addr.type = p_param->type;
		memcpy(addr.a.val, p_param->addr_val, BT_ADDR_SIZE);
	}

	/* Do nothing if we already have the right address */
	if (!bt_addr_cmp(&addr.a, &bt_dev.random_addr.a)) {
		return 0;
	}

	buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_RANDOM_ADDRESS, sizeof(addr.a.val));
	if (!buf) {
		return RTK_BT_ERR_NO_BUFS;
	}

	net_buf_add_mem(buf, &addr.a.val, sizeof(addr.a.val));

	err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_RANDOM_ADDRESS, buf, NULL);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	bt_addr_copy(&bt_dev.random_addr.a, &addr.a);
	bt_dev.random_addr.type = BT_ADDR_LE_RANDOM;

	/*  If set static random addr as local addr, it should also be set as identity addr,
	    otherwise, public addr will be distributed when pairing, may cause problem */
	if (RTK_BT_LE_RAND_ADDR_STATIC == bt_stack_rand_addr_type(p_param->addr_val)) {
		le_cfg_local_ident_addr(addr.a.val, RTK_BT_LE_IDENT_ADDR_RAND);
	}
	return 0;
}

static uint16_t bt_stack_le_gap_gen_rand_addr(void *param)
{
	rtk_bt_le_gen_rand_addr_t *gen_rand = (rtk_bt_le_gen_rand_addr_t *)param;
	bt_addr_le_t addr = {};
	int err;

	err = le_create_rand_addr(gen_rand->type, &addr);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	memcpy(gen_rand->p_addr, addr.a.val, BT_ADDR_SIZE);

	return 0;
}

static uint16_t bt_stack_le_gap_cfg_local_ident_addr(void *param)
{
	rtk_bt_le_ident_addr_t *ident_addr = (rtk_bt_le_ident_addr_t *)param;

	le_cfg_local_ident_addr(ident_addr->addr_val, ident_addr->type);

	return 0;
}

static uint16_t bt_stack_le_gap_read_rssi(void *param)
{
	rtk_bt_le_read_rssi_param_t *read_rssi = (rtk_bt_le_read_rssi_param_t *)param;
	struct net_buf *buf, *rsp = NULL;
	struct bt_hci_cp_read_rssi *cp;
	struct bt_hci_rp_read_rssi *rp;

	int err;

	buf = bt_hci_cmd_create(BT_HCI_OP_READ_RSSI, sizeof(*cp));
	if (!buf) {
		return RTK_BT_ERR_NO_BUFS;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = sys_cpu_to_le16(read_rssi->conn_handle);

	err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_RSSI, buf, &rsp);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	rp = (void *)rsp->data;
	*read_rssi->p_rssi = rp->rssi;

	net_buf_unref(rsp);

	return 0;
}

static uint16_t bt_stack_le_gap_get_active_conn(void *param)
{
	uint16_t active_num = 0;
	uint16_t i = 0;
	struct bt_conn *conn = NULL;
	rtk_bt_le_get_active_conn_t *p_active_conn = (rtk_bt_le_get_active_conn_t *)param;
	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		conn = bt_conn_lookup_index(i);
		if (conn) {
			p_active_conn->conn_handle[active_num] = conn->handle;
			active_num ++;
			bt_conn_unref(conn);
		}
	}

	p_active_conn->conn_num = active_num ++;

	return 0;
}

static uint16_t bt_stack_le_gap_get_conn_handle_by_addr(void *param)
{
	rtk_bt_le_get_conn_handle_by_addr_param_t *p_conn_addr = (rtk_bt_le_get_conn_handle_by_addr_param_t *)param;
	rtk_bt_le_addr_t addr = p_conn_addr->addr;
	uint16_t i = 0;
	struct bt_conn *conn = NULL;

	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		conn = bt_conn_lookup_index(i);
		if (conn) {
			if (conn->le.dst.type == addr.type) {
				if (!memcmp(conn->le.dst.a.val, addr.addr_val, BT_ADDR_SIZE)) {
					*p_conn_addr->p_conn_handle = conn->handle;
					bt_conn_unref(conn);
					return 0;
				}
			}
			bt_conn_unref(conn);
		}
	}

	return RTK_BT_ERR_NO_CONNECTION;
}

#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
static uint16_t bt_stack_le_gap_set_data_len(void *param)
{
	rtk_bt_le_set_datalen_param_t *set_datalen = (rtk_bt_le_set_datalen_param_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(set_datalen->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_le_set_data_len(conn, set_datalen->max_tx_octets, set_datalen->max_tx_time);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
static uint16_t bt_stack_le_gap_set_phy(void *param)
{
	rtk_bt_le_set_phy_param_t *set_phy = (rtk_bt_le_set_phy_param_t *)param;
	int err = 0;
	struct bt_conn *conn = NULL;

	conn = bt_conn_lookup_handle(set_phy->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_le_set_phy(conn, set_phy->all_phys, set_phy->tx_phys, set_phy->rx_phys, set_phy->phy_options);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}
#endif

static uint16_t bt_stack_le_gap_set_channels(void *param)
{
	uint8_t *chan_map = (uint8_t *)param;
	int err = 0;

	err = bt_le_set_chan_map(chan_map);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
void bt_zephyr_rslv_op_cb(struct bt_keys *keys, bool is_add, uint8_t privacy_mode)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_modify_resolv_list_ind_t *p_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_RESOLV_LIST_MODIFY_IND,
								sizeof(rtk_bt_le_modify_resolv_list_ind_t));
	if (p_evt) {
		p_ind = (rtk_bt_le_modify_resolv_list_ind_t *)p_evt->data;
		memset(p_ind, 0, sizeof(rtk_bt_le_modify_resolv_list_ind_t));
		p_ind->op = is_add ? RTK_BT_LE_RESOLV_LIST_OP_ADD : RTK_BT_LE_RESOLV_LIST_OP_REMOVE;
		p_ind->entry.device_mode = privacy_mode;
		/* When adding all-zero resolving list, keys is NULL */
		if (keys) {
			p_ind->entry.addr_type = keys->addr.type;
			memcpy(p_ind->entry.addr, keys->addr.a.val, RTK_BD_ADDR_LEN);
		}
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	/* When enable privacy whitelist, each device added/deleted in resolving list should be
	also added/deleted in whitelist except the all-zero resolving list */
	if (privacy_whitelist && keys) {
		if (is_add) {
			bt_le_filter_accept_list_add(&keys->addr);
		} else {
			bt_le_filter_accept_list_remove(&keys->addr);
		}
	}
}

void bt_zephyr_rslv_pend_cb(enum bt_rslv_pend_reason pend_reason)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_le_resolv_list_pending_ind_t *p_ind = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_RESOLV_LIST_PENDING_IND,
								sizeof(rtk_bt_le_resolv_list_pending_ind_t));
	if (p_evt) {
		p_ind = (rtk_bt_le_resolv_list_pending_ind_t *)p_evt->data;
		p_ind->reason = (rtk_bt_le_resolv_list_pending_reason_t)pend_reason;
		rtk_bt_evt_indicate(p_evt, NULL);
	}
}

static struct bt_id_cb zephyr_id_cb = {
	.rslv_op_complete = bt_zephyr_rslv_op_cb,
	.rslv_op_pend = bt_zephyr_rslv_pend_cb,
};

static void add_privacy_dev_to_rl(struct bt_keys *keys, void *data)
{
	(void)data;
	bt_id_add(keys);
}

extern int hci_id_add(uint8_t id, const bt_addr_le_t *addr, uint8_t peer_irk[16]);
static int add_dummy_dev_to_rl(void)
{
	int err;
	bt_addr_le_t peer_ident_addr = {0};
	uint8_t peer_irk[16] = {0};

	err = hci_id_add(0, &peer_ident_addr, peer_irk);
	if (err) {
		return err;
	}

	bt_dev.le.rl_entries++;
	bt_zephyr_rslv_op_cb(NULL, true, BT_HCI_LE_PRIVACY_MODE_NETWORK);

	return 0;
}

static uint16_t bt_stack_le_privacy_init(void *param)
{
	int err;

	privacy_whitelist = *((bool *)param);
	bt_id_register_cb(&zephyr_id_cb);

	/* Zephyr has no API to update IRK of default id. */
	if (privacy_irk_app_cfg && memcmp(bt_dev.irk[BT_ID_DEFAULT], privacy_irk, RTK_BT_LE_GAP_IRK_LEN)) {
		memcpy(bt_dev.irk[BT_ID_DEFAULT], privacy_irk, RTK_BT_LE_GAP_IRK_LEN);
		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			bt_settings_store_irk();
		}
	}

	/* Add a dummy resolving list whose peer addr and irk value is 0, use this way
	to start adv with RPA when local device has not bond with others. */
	err = add_dummy_dev_to_rl();
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	bt_keys_foreach_type(BT_KEYS_IRK, add_privacy_dev_to_rl, NULL);
	bt_dev.privacy_enabled = true;

	return 0;
}

extern int le_set_privacy_mode(const bt_addr_le_t *addr, uint8_t mode);
static uint16_t bt_stack_le_gap_set_privacy_mode(void *param)
{
	rtk_bt_le_set_privacy_mode_param_t *set_privacy_param = (rtk_bt_le_set_privacy_mode_param_t *)param;
	int err;
	bt_addr_le_t le_addr = {};

	le_addr.type = set_privacy_param->peer_ident_addr_type;
	memcpy(le_addr.a.val, set_privacy_param->peer_addr, BT_ADDR_SIZE);
	err = le_set_privacy_mode(&le_addr, set_privacy_param->privacy_mode);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_gap_read_local_resolv_addr(void *param)
{
	rtk_bt_le_read_local_rpa_param_t *rpa_param = (rtk_bt_le_read_local_rpa_param_t *)param;
	struct bt_hci_cp_le_read_local_rpa cp;
	struct bt_hci_rp_le_read_local_rpa *rp;
	struct net_buf *buf, *rsp;
	int err;

	cp.peer_id_addr.type = rpa_param->peer_ident_addr_type;
	memcpy(cp.peer_id_addr.a.val, rpa_param->peer_addr, BT_ADDR_SIZE);
	buf = bt_hci_cmd_create(BT_HCI_OP_LE_READ_LOCAL_RPA, sizeof(cp));
	if (!buf) {
		return RTK_BT_ERR_NO_BUFS;
	}

	net_buf_add_mem(buf, &cp, sizeof(cp));
	err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_LOCAL_RPA, buf, &rsp);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	rp = (void *)rsp->data;
	memcpy(rpa_param->local_rpa, rp->local_rpa.val, BT_ADDR_SIZE);
	net_buf_unref(buf);

	return 0;
}

static uint16_t bt_stack_le_gap_read_peer_resolv_addr(void *param)
{
	rtk_bt_le_read_peer_rpa_param_t *rpa_param = (rtk_bt_le_read_peer_rpa_param_t *)param;
	struct bt_hci_cp_le_read_peer_rpa cp;
	struct bt_hci_rp_le_read_peer_rpa *rp;
	struct net_buf *buf, *rsp;
	int err;

	cp.peer_id_addr.type = rpa_param->peer_ident_addr_type;
	memcpy(cp.peer_id_addr.a.val, rpa_param->peer_addr, BT_ADDR_SIZE);
	buf = bt_hci_cmd_create(BT_HCI_OP_LE_READ_PEER_RPA, sizeof(cp));
	if (!buf) {
		return RTK_BT_ERR_NO_BUFS;
	}

	net_buf_add_mem(buf, &cp, sizeof(cp));
	err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_LOCAL_RPA, buf, &rsp);
	if (err) {
		return zephyr_err_to_rtk(err);
	}

	rp = (void *)rsp->data;
	memcpy(rpa_param->peer_rpa, rp->peer_rpa.val, BT_ADDR_SIZE);
	net_buf_unref(buf);

	return 0;
}
#endif

void bt_set_sc_support(bool enable);
static uint16_t bt_stack_le_sm_set_security_param(void *p_param)
{
	rtk_bt_le_security_param_t *param = (rtk_bt_le_security_param_t *)p_param;

#if defined(CONFIG_BT_FIXED_PASSKEY)
	if (param->use_fixed_key) {
		if (param->io_cap == RTK_IO_CAP_KEYBOARD_ONLY || param->io_cap == RTK_IO_CAP_NO_IN_NO_OUT) {
			return RTK_BT_ERR_PARAM_INVALID;
		}
		bt_passkey_set(param->fixed_key);
	} else {
		bt_passkey_set(BT_PASSKEY_INVALID);
	}
#else
	if (param->use_fixed_key) {
		return RTK_BT_ERR_UNSUPPORTED;
	}
#endif

	security_level = BT_SECURITY_L2;

	bt_set_bondable(param->bond_flag);

	memset(&bt_zephyr_auth_cb, 0, sizeof(struct bt_conn_auth_cb));
	bt_zephyr_auth_cb.cancel = zephyr_auth_cancel;
	bt_zephyr_auth_cb.pairing_confirm = zephyr_auth_pairing_confirm;

	if (param->io_cap == RTK_IO_CAP_DISPALY_ONLY) {
		bt_zephyr_auth_cb.passkey_display = zephyr_auth_passkey_display;
	} else if (param->io_cap == RTK_IO_CAP_DISPLAY_YES_NO) {
		bt_zephyr_auth_cb.passkey_display = zephyr_auth_passkey_display;
		bt_zephyr_auth_cb.passkey_confirm = zephyr_auth_passkey_confirm;
	} else if (param->io_cap == RTK_IO_CAP_KEYBOARD_ONLY) {
		bt_zephyr_auth_cb.passkey_entry = zephyr_auth_passkey_entry;
	} else if (param->io_cap == RTK_IO_CAP_KEYBOARD_DISPALY) {
		bt_zephyr_auth_cb.passkey_display = zephyr_auth_passkey_display;
		bt_zephyr_auth_cb.passkey_confirm = zephyr_auth_passkey_confirm;
		bt_zephyr_auth_cb.passkey_entry = zephyr_auth_passkey_entry;
	}

	if (param->mitm_flag) { /* may support numerical comparison or passkey entry */
		security_level = BT_SECURITY_L3;
	}

	if (param->oob_data_flag) {
		bt_zephyr_auth_cb.oob_data_request = zephyr_auth_oob_data_request;
	}
	// sc oob is not supported temporarily
	bt_le_oob_set_sc_flag(false);
	bt_le_oob_set_legacy_flag(param->oob_data_flag);

	/* bt_set_sc_support() is added by ping_yan in smp.c, remove it when sc oob is supported */
	if (param->sec_pair_flag && (!param->oob_data_flag)) {
		security_level = BT_SECURITY_L4;
		bt_set_sc_support(true);
	} else {
		bt_set_sc_support(false); /* disable sc when oob is requested, remove this restiction when sc oob is supported */
	}

	if (param->io_cap == BT_SMP_IO_NO_INPUT_OUTPUT) {
		security_level = BT_SECURITY_L2;
	}

	bt_conn_auth_cb_register(NULL);
	if (bt_conn_auth_cb_register(&bt_zephyr_auth_cb)) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_sm_start_security(void *param)
{
	uint16_t conn_handle = *((uint16_t *)param);
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_conn_set_security(conn, security_level);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_sm_pairing_confirm(void *param)
{
	rtk_bt_le_pair_cfm_t *pair_cfm = (rtk_bt_le_pair_cfm_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(pair_cfm->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (pair_cfm->confirm) {
		err = bt_conn_auth_pairing_confirm(conn);
	} else {
		err = bt_conn_auth_cancel(conn);
	}

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_sm_passkey_entry(void *param)
{
	rtk_bt_le_auth_key_input_t *key_input = (rtk_bt_le_auth_key_input_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(key_input->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_conn_auth_passkey_entry(conn, key_input->passkey);

	bt_conn_unref(conn);

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static uint16_t bt_stack_le_sm_passkey_confirm(void *param)
{
	rtk_bt_le_auth_key_confirm_t *key_cfm = (rtk_bt_le_auth_key_confirm_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(key_cfm->conn_handle, BT_CONN_TYPE_LE);

	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (key_cfm->confirm) {
		err = bt_conn_auth_passkey_confirm(conn);
	} else {
		err = bt_conn_auth_cancel(conn);
	}

	bt_conn_unref(conn);

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_sm_set_oob_data(void *param)
{
	rtk_bt_le_set_oob_key_t *oob = (rtk_bt_le_set_oob_key_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(oob->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_le_oob_set_legacy_tk(conn, oob->oob_key);
	bt_conn_unref(conn);

	// when sc oob is used, generate local oob data (including rand, confirm) by following APIs when init or ext adv started.
	// int bt_le_oob_get_local();
	// int bt_le_ext_adv_oob_get_local();
	// Tell remote device local oob data before pairing

	// bt_le_oob_set_sc_data() is used to set local & remote oob data to complete pairing. pointer oobd_local & oobd_remote should be always valid.

	if (err) {
		return zephyr_err_to_rtk(err);
	}

	return 0;
}

static void count_bonded_device(const struct bt_bond_info *info, void *param)
{
	(void)info;

	*((uint8_t *)param) += 1;
}

static uint16_t bt_stack_le_sm_get_bond_num(void *param)
{
	uint8_t id;

	*((uint8_t *)param) = 0;

	if (IS_ENABLED(CONFIG_BT_SMP)) {
		for (id = 0; id < CONFIG_BT_ID_MAX; id++) {
			bt_foreach_bond(id, count_bonded_device, param);
		}
	}

	return 0;
}

static void get_bonded_info(const struct bt_bond_info *info, void *p_param)
{
	rtk_bt_le_get_bond_info_param_t *param = (rtk_bt_le_get_bond_info_param_t *)p_param;

	if (param->size < * (param->p_size)) {
		param->p_bond_info[param->size].remote_addr.type = info->addr.type;
		memcpy(param->p_bond_info[param->size].remote_addr.addr_val, info->addr.a.val, BT_ADDR_SIZE);

		param->p_bond_info[param->size].ident_addr.type = info->addr.type;
		memcpy(param->p_bond_info[param->size].ident_addr.addr_val, info->addr.a.val, BT_ADDR_SIZE);

		if (!bt_addr_eq(&info->rpa, BT_ADDR_ANY)) {
			param->p_bond_info[param->size].remote_addr.type = RTK_BT_LE_ADDR_TYPE_RANDOM;
			memcpy(param->p_bond_info[param->size].remote_addr.addr_val, info->rpa.val, BT_ADDR_SIZE);
		}

		param->size++;
	}
}

static uint16_t bt_stack_le_sm_get_bond_info(void *p_param)
{
	rtk_bt_le_get_bond_info_param_t *param = (rtk_bt_le_get_bond_info_param_t *)p_param;
	uint8_t id;

	if (param->size == 0) {
		return 0;
	}

	if (IS_ENABLED(CONFIG_BT_SMP)) {
		param->size = 0;
		for (id = 0; id < CONFIG_BT_ID_MAX; id++) {
			bt_foreach_bond(id, get_bonded_info, (void *)param);
		}
	}

	*(param->p_size) = param->size;

	return 0;
}

static uint16_t bt_stack_le_sm_delete_bond_device(void *param)
{
	rtk_bt_le_addr_t *addr = (rtk_bt_le_addr_t *)param;
	bt_addr_le_t dst;
	uint8_t id = 0;

	dst.type = addr->type;
	memcpy(dst.a.val, addr->addr_val, BT_ADDR_SIZE);

	for (id = 0; id < CONFIG_BT_ID_MAX; id++) {
		if (bt_unpair(id, &dst)) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}
	return 0;
}

static uint16_t bt_stack_le_sm_clear_bond_list(void)
{
	uint8_t id = 0;

	for (id = 0; id < CONFIG_BT_ID_MAX; id++) {
		if (bt_unpair(id, NULL)) {
			return RTK_BT_ERR_LOWER_STACK_API;
		}
	}

	bond_info_indicate(RTK_BT_LE_BOND_CLEAR, NULL, NULL);

	return 0;
}

static uint16_t bt_stack_le_gap_get_mtu_size(void *param)
{
	uint16_t mtu_size = 0;
	struct bt_conn *conn = NULL;

	rtk_bt_le_get_mtu_size_param_t *get_mtu = (rtk_bt_le_get_mtu_size_param_t *)param;

	conn = bt_conn_lookup_handle(get_mtu->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	mtu_size = bt_gatt_get_mtu(conn);

	bt_conn_unref(conn);

	*get_mtu->p_mtu_size = mtu_size;

	return 0;
}

uint16_t bt_stack_le_gap_get_conn_id(uint16_t conn_handle, uint8_t *p_conn_id)
{
	struct bt_conn *conn = NULL;
	int i;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (conn) {
		*p_conn_id = bt_conn_index(conn);
		bt_conn_unref(conn);
		return 0;
	}

	/* disconnected device */
	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		if (bt_stack_le_conn_handle[i] == conn_handle) {
			*p_conn_id = i;
			return 0;
		}
	}

	return RTK_BT_ERR_PARAM_INVALID;
}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

#define CTE_SAMPLING_ANTENNA_NUMBER_MIN    2
static uint16_t bt_stack_le_gap_connless_cte_rx_start(void *param)
{
	rtk_bt_le_gap_connless_cte_rx_start_t *p_start = (rtk_bt_le_gap_connless_cte_rx_start_t *)param;
	struct bt_df_per_adv_sync_cte_rx_param cte_rx_param = {0};
	struct bt_le_per_adv_sync *sync;
	int err = 0;

	if (!p_start || !p_start->param) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	sync = bt_le_per_adv_sync_lookup_index(p_start->sync_id);

	if (!sync) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (p_start->param->num_ant_ids >= CTE_SAMPLING_ANTENNA_NUMBER_MIN && p_start->param->ant_ids) {
		cte_rx_param.cte_types = BT_DF_CTE_TYPE_ALL;
	} else {
		cte_rx_param.cte_types = (BT_DF_CTE_TYPE_AOD_1US | BT_DF_CTE_TYPE_AOD_2US);
	}
	cte_rx_param.slot_durations = p_start->param->slot_durations;
	cte_rx_param.max_cte_count = p_start->param->max_sampled_ctes;
	cte_rx_param.num_ant_ids = p_start->param->num_ant_ids;
	cte_rx_param.ant_ids = p_start->param->ant_ids;

	err = bt_df_per_adv_sync_cte_rx_enable(sync, &cte_rx_param);
	if (err) {
		BT_LOGE("per adv sync cte rx enable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_connless_cte_rx_stop(void *param)
{
	uint8_t sync_id;
	int err = 0;
	struct bt_le_per_adv_sync *sync;

	if (!param) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	sync_id = *((uint8_t *)param);
	sync = bt_le_per_adv_sync_lookup_index(sync_id);

	if (!sync) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	err = bt_df_per_adv_sync_cte_rx_disable(sync);
	if (err) {
		BT_LOGE("per adv sync cte rx disable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_conn_cte_rx_start(void *param)
{
	rtk_bt_le_gap_conn_cte_rx_t *p_start = (rtk_bt_le_gap_conn_cte_rx_t *)param;
	uint16_t conn_handle = p_start->conn_handle;
	struct bt_conn *conn = NULL;
	struct bt_df_conn_cte_req_params req_param = {0};
	struct bt_df_conn_cte_rx_param rx_param = {0};
	int err;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (p_start->p_rx_param->req_cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOA) {
		rx_param.cte_types = BT_DF_CTE_TYPE_AOA;
		req_param.cte_type = BT_DF_CTE_TYPE_AOA;
	} else if (p_start->p_rx_param->req_cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOD_1US) {
		rx_param.cte_types = BT_DF_CTE_TYPE_AOD_1US;
		req_param.cte_type = BT_DF_CTE_TYPE_AOD_1US;
	} else if (p_start->p_rx_param->req_cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOD_2US) {
		rx_param.cte_types = BT_DF_CTE_TYPE_AOD_2US;
		req_param.cte_type = BT_DF_CTE_TYPE_AOD_2US;
	}

	rx_param.slot_durations = p_start->p_rx_param->slot_durations;
	rx_param.num_ant_ids = p_start->p_rx_param->num_ant_ids;
	rx_param.ant_ids = p_start->p_rx_param->ant_ids;
	BT_LOGD("conn_cte_rx_enable: cte_types=%u, slot_durations=%u, num_ant_ids=%u\r\n",
			rx_param.cte_types, rx_param.slot_durations, rx_param.num_ant_ids);

	err = bt_df_conn_cte_rx_enable(conn, &rx_param);
	if (err) {
		BT_LOGE("conn cte rx enable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

#if defined(CONFIG_BT_DF_CONNECTION_CTE_REQ)

	req_param.interval = p_start->p_rx_param->cte_req_interval;
	req_param.cte_length = p_start->p_rx_param->req_cte_len;
	BT_LOGD("conn_cte_req_enable: cte_type=%u, interval=%u, cte_length=%u\r\n",
			req_param.cte_type, req_param.interval, req_param.cte_length);

	err = bt_df_conn_cte_req_enable(conn, &req_param);
	if (err) {
		BT_LOGE("conn cte req enable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#else
	return RTK_BT_ERR_LOWER_STACK_API;
#endif
	return 0;
}

static uint16_t bt_stack_le_gap_conn_cte_rx_stop(void *param)
{
	int err;
	struct bt_conn *conn = NULL;
	rtk_bt_le_gap_conn_cte_rx_t *p_stop = (rtk_bt_le_gap_conn_cte_rx_t *)param;

	if (!p_stop) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	conn = bt_conn_lookup_handle(p_stop->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_df_conn_cte_rx_disable(conn);
	if (err) {
		BT_LOGE("conn cte rx disable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

#if defined(CONFIG_BT_DF_CONNECTION_CTE_REQ)
	err = bt_df_conn_cte_req_disable(conn);
	if (err) {
		BT_LOGE("conn cte req disable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#else
	return RTK_BT_ERR_LOWER_STACK_API;
#endif
	return 0;
}

static uint16_t bt_stack_le_gap_conn_cte_tx_start(void *param)
{
	int err;
	struct bt_conn *conn = NULL;
	rtk_bt_le_gap_conn_cte_tx_start_t *p_start = (rtk_bt_le_gap_conn_cte_tx_start_t *)param;
	struct bt_df_conn_cte_tx_param rx_param = {0};

	if (!p_start || !p_start->param) {
		BT_LOGE("conn CTE tx start invalid parameter\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	conn = bt_conn_lookup_handle(p_start->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

#if defined(CONFIG_BT_DF_CONNECTION_CTE_TX)
	rx_param.cte_types = p_start->param->cte_types;
	rx_param.num_ant_ids = p_start->param->num_ant_ids;
	rx_param.ant_ids = p_start->param->ant_ids;

	err = bt_df_set_conn_cte_tx_param(conn, &rx_param);
	if (err) {
		BT_LOGE("set conn cte tx param fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#else
	return RTK_BT_ERR_LOWER_STACK_API;
#endif

#if defined(CONFIG_BT_DF_CONNECTION_CTE_RSP)
	err = bt_df_conn_cte_rsp_enable(conn);
	if (err) {
		BT_LOGE("conn cte rsp enable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#else
	return RTK_BT_ERR_LOWER_STACK_API;
#endif
	return 0;
}

static uint16_t bt_stack_le_gap_conn_cte_tx_stop(void *param)
{
	int err;
	uint16_t conn_handle;
	struct bt_conn *conn = NULL;

	if (!param) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	conn_handle = *((uint16_t *)param);
	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

#if defined(CONFIG_BT_DF_CONNECTION_CTE_RSP)
	err = bt_df_conn_cte_rsp_disable(conn);
	if (err) {
		BT_LOGE("conn cte rsp disable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
#else
	return RTK_BT_ERR_LOWER_STACK_API;
#endif
	return 0;
}

static uint16_t bt_stack_le_gap_connless_cte_tx_start(void *param)
{
	rtk_bt_le_gap_connless_cte_tx_start_t *p_start = (rtk_bt_le_gap_connless_cte_tx_start_t *)param;
	_ext_adv_info_t *p_adv = NULL;
	struct bt_df_adv_cte_tx_param tx_param = {0};
	int err;

	if (!p_start || !p_start->param) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (!bt_stack_le_gap_ext_adv_handle_valid(p_start->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_adv = &_ext_adv_tbl[p_start->adv_handle];

	tx_param.cte_len = p_start->param->cte_len;
	if (p_start->param->cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOA) {
		tx_param.cte_type = BT_DF_CTE_TYPE_AOA;
	} else if (p_start->param->cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOD_1US) {
		tx_param.cte_type = BT_DF_CTE_TYPE_AOD_1US;
	} else if (p_start->param->cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOD_2US) {
		tx_param.cte_type = BT_DF_CTE_TYPE_AOD_2US;
	}
	tx_param.cte_count = p_start->param->cte_count;
	tx_param.num_ant_ids = p_start->param->num_ant_ids;
	tx_param.ant_ids = p_start->param->ant_ids;

	err = bt_df_set_adv_cte_tx_param(p_adv->adv, &tx_param);
	if (err) {
		BT_LOGE("set adv cte tx param fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	err = bt_df_adv_cte_tx_enable(p_adv->adv);
	if (err) {
		BT_LOGE("adv cte tx enable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_connless_cte_tx_stop(void *param)
{
	int err;
	uint8_t adv_handle;
	_ext_adv_info_t *p_adv = NULL;

	if (!param) {
		BT_LOGE("%s: invalid param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	adv_handle = *((uint8_t *)param);

	if (!bt_stack_le_gap_ext_adv_handle_valid(adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	p_adv = &_ext_adv_tbl[adv_handle];

	err = bt_df_adv_cte_tx_disable(p_adv->adv);
	if (err) {
		BT_LOGE("adv cte tx disable fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_le_gap_get_antenna_info(void *param)
{
	int err;
	rtk_bt_le_gap_antenna_info_t *ant = (rtk_bt_le_gap_antenna_info_t *)param;

	if (!ant) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	err = hci_df_read_ant_info(&ant->supported_switching_sampling_rates,
							   &ant->num_antennae,
							   &ant->max_switching_pattern_length,
							   &ant->max_cte_length);
	if (err) {
		BT_LOGE("get antenna information fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return 0;
}
#endif /* RTK_BLE_5_1_CTE_SUPPORT */

#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
static uint16_t bt_stack_le_pawr_set_subevent_data(void *param)
{
	int err;
	struct bt_le_per_adv_subevent_data_params sub;
	struct bt_le_ext_adv *adv;
	rtk_bt_le_gap_pawr_subevent_data_t *ptr = (rtk_bt_le_gap_pawr_subevent_data_t *)param;

	if (!bt_stack_le_gap_ext_adv_handle_valid(ptr->adv_handle)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	adv = _ext_adv_tbl[ptr->adv_handle].adv;

	sub.subevent = ptr->subevent;
	sub.response_slot_start = ptr->response_slot_start;
	sub.response_slot_count = ptr->response_slot_count;

	err = bt_le_per_adv_set_subevent_data(adv, 1, &sub);
	if (err) {
		BT_LOGE("set subevent data fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return 0;
}
#endif

#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
static uint16_t bt_stack_le_pawr_set_response_data(void *param)
{
	rtk_bt_le_gap_pawr_set_response_t *rsp = (rtk_bt_le_gap_pawr_set_response_t *)param;
	struct bt_le_per_adv_sync *sync;
	struct bt_le_per_adv_response_params rsp_param;
	NET_BUF_SIMPLE_DEFINE(data, rsp->len);
	int err;

	sync = bt_le_per_adv_sync_lookup_index(rsp->sync_id);

	rsp_param.request_event = rsp->request_event;
	rsp_param.request_subevent = rsp->request_subevent;
	rsp_param.response_subevent = rsp->response_subevent;
	rsp_param.response_slot = rsp->response_slot;

	net_buf_simple_add_mem(&data, rsp->rsp, rsp->len);
	err = bt_le_per_adv_set_response_data(sync, &rsp_param, &data);
	if (err) {
		BT_LOGE("set subevent response fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return 0;
}

static uint16_t bt_stack_le_pawr_sync_subevent(void *param)
{
	rtk_bt_le_gap_pawr_sync_subevent_t *sub = (rtk_bt_le_gap_pawr_sync_subevent_t *)param;
	struct bt_le_per_adv_sync *sync;
	struct bt_le_per_adv_sync_subevent_params sub_param;
	int err;

	sub_param.properties = sub->properties;
	sub_param.num_subevents = sub->num_subevents;
	sub_param.subevents = sub->subevents;

	sync = bt_le_per_adv_sync_lookup_index(sub->sync_id);
	err = bt_le_per_adv_sync_subevent(sync, &sub_param);
	if (err) {
		BT_LOGE("sync subevent fail, err=%d\r\n", err);
		return RTK_BT_ERR_LOWER_STACK_API;
	}
	return 0;
}
#endif

uint16_t bt_stack_le_gap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_le_gap_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {
	case RTK_BT_LE_GAP_ACT_GET_VERSION:
		BT_LOGD("RTK_BT_LE_GAP_ACT_GET_VERSION \r\n");
		ret = bt_stack_le_gap_get_version(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_GET_DEVICE_ADDR:
		BT_LOGD("RTK_BT_LE_GAP_ACT_GET_DEVICE_ADDR \r\n");
		ret = bt_stack_le_gap_get_device_addr(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_DEVICE_NAME:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_DEVICE_NAME \r\n");
		ret = bt_stack_le_gap_set_device_name(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_APPEARANCE:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_APPEARANCE \r\n");
		ret = bt_stack_le_gap_set_apperance(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_PREFERRED_CONN_PARAM:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_PREFERRED_CONN_PARAM \r\n");
		//ret = bt_stack_le_gap_set_preferred_conn_param(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_RAND_ADDR:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_RAND_ADDR \r\n");
		ret = bt_stack_le_gap_set_rand_addr(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_GEN_RAND_ADDR:
		BT_LOGD("RTK_BT_LE_GAP_ACT_GEN_RAND_ADDR \r\n");
		ret = bt_stack_le_gap_gen_rand_addr(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_CFG_LOCAL_IDENT_ADDR:
		BT_LOGD("RTK_BT_LE_GAP_ACT_CFG_LOCAL_IDENT_ADDR");
		ret = bt_stack_le_gap_cfg_local_ident_addr(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_ADV_DATA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_ADV_DATA \r\n");
		ret = bt_stack_le_gap_set_adv_data(p_cmd->param, p_cmd->param_len);
		break;
	case RTK_BT_LE_GAP_ACT_SET_SCAN_RSP_DATA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_SCAN_RSP_DATA \r\n");
		ret = bt_stack_le_gap_set_scan_rsp_data(p_cmd->param, p_cmd->param_len);
		break;
	case RTK_BT_LE_GAP_ACT_START_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_START_ADV \r\n");
		ret = bt_stack_le_gap_start_adv(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_STOP_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_STOP_ADV \r\n");
		ret = bt_stack_le_gap_stop_adv();
		break;
	case RTK_BT_LE_GAP_ACT_ADV_STATE:
		BT_LOGD("RTK_BT_LE_GAP_ACT_ADV_STATE \r\n");
		ret = (uint16_t)bt_stack_le_gap_adv_is_idle();
		break;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_ACT_SET_EXT_ADV_DATA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_EXT_ADV_DATA \r\n");
		ret = bt_stack_le_gap_set_ext_adv_data(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_EXT_SCAN_RSP_DATA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_EXT_SCAN_RSP_DATA \r\n");
		ret = bt_stack_le_gap_set_ext_scan_rsp_data(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_CREATE_EXT_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_CREATE_EXT_ADV \r\n");
		ret = bt_stack_le_gap_create_ext_adv(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_START_EXT_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_START_EXT_ADV \r\n");
		ret = bt_stack_le_gap_start_ext_adv(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_STOP_EXT_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_STOP_EXT_ADV \r\n");
		ret = bt_stack_le_gap_stop_ext_adv(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_REMOVE_EXT_ADV:
		BT_LOGD("RTK_BT_LE_GAP_ACT_REMOVE_EXT_ADV \r\n");
		ret = bt_stack_le_gap_remove_ext_adv(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_EXT_CONN:
		BT_LOGD("RTK_BT_LE_GAP_ACT_EXT_CONN \r\n");
		ret = bt_stack_le_gap_ext_connect(p_cmd->param);
		break;
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
	case RTK_BT_LE_GAP_ACT_START_PA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_START_PA \r\n");
		ret = bt_stack_le_gap_start_pa(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_STOP_PA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_STOP_PA \r\n");
		ret = bt_stack_le_gap_stop_pa(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_UPDATE_PA:
		BT_LOGD("RTK_BT_LE_GAP_ACT_UPDATE_PA \r\n");
		//p_cmd->user_data = (RTK_BT_LE_GAP_ACT_UPDATE_PA << 8);
		//bt_stack_pending_cmd_insert(p_cmd);
		ret = bt_stack_le_gap_update_pa(p_cmd->param);
		//goto async_handle;
		break;
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	case RTK_BT_LE_GAP_ACT_PA_SYNC_MODIFY_ADV_LIST:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PA_SYNC_MODIFY_ADV_LIST \r\n");
		ret = bt_stack_le_gap_pa_sync_modify_adv_list(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_PA_SYNC_CREATE:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PA_SYNC_CREATE \r\n");
		ret = bt_stack_le_gap_pa_sync_create(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_PA_SYNC_TERMINATE:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PA_SYNC_TERMINATE \r\n");
		ret = bt_stack_le_gap_pa_sync_terminate(p_cmd->param);
		break;
#endif

#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
	case RTK_BT_LE_GAP_ACT_PAST_TRANSFER:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAST_TRANSFER \r\n");
		ret = bt_stack_le_gap_past_send(p_cmd->param);
		break;
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
	case RTK_BT_LE_GAP_ACT_PAST_RECV_SET:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAST_RECV_SET \r\n");
		ret = bt_stack_le_gap_past_recv_set(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_DEFAULT_PAST_RECV_SET:
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAST_RECV_SET \r\n");
		ret = bt_stack_le_gap_default_past_recv_set(p_cmd->param);
		break;
#endif

	case RTK_BT_LE_GAP_ACT_SCAN_INFO_FILTER:
		//ret = bt_stack_le_gap_scan_info_filter(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_SCAN_PARAM:
		BT_LOGD("RTK_BT_LE_GAP_ACT_SET_SCAN_PARAM \r\n");
		ret = bt_stack_le_gap_set_scan_param(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_START_SCAN:
		BT_LOGD("RTK_BT_LE_GAP_ACT_START_SCAN \r\n");
		ret = bt_stack_le_gap_start_scan(false);
		break;
	case RTK_BT_LE_GAP_ACT_STOP_SCAN:
		BT_LOGD("RTK_BT_LE_GAP_ACT_STOP_SCAN \r\n");
		ret = bt_stack_le_gap_stop_scan();
		break;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_ACT_EXT_SCAN_SET_PARAM:
		BT_LOGD("RTK_BT_LE_GAP_ACT_EXT_SCAN_SET_PARAM \r\n");
		ret = bt_stack_le_gap_ext_scan_set_param(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_EXT_SCAN_START:
		BT_LOGD("RTK_BT_LE_GAP_ACT_EXT_SCAN_START \r\n");
		ret = bt_stack_le_gap_start_scan(true);
		break;
	case RTK_BT_LE_GAP_ACT_EXT_SCAN_STOP:
		BT_LOGD("RTK_BT_LE_GAP_ACT_EXT_SCAN_STOP \r\n");
		ret = bt_stack_le_gap_stop_scan();
		break;
#endif

	case RTK_BT_LE_GAP_ACT_CONN:
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONN \r\n");
		ret = bt_stack_le_gap_connect(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_DISCONN:
		BT_LOGD("RTK_BT_LE_GAP_ACT_DISCONN \r\n");
		ret = bt_stack_le_gap_disconnect(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_DISCONN_WITH_REASON:
		BT_LOGD("RTK_BT_LE_GAP_ACT_DISCONN_WITH_REASON");
		ret = bt_stack_le_gap_disconnect_with_reason(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_UPDATE_CONN_PARAM:
		BT_LOGD("RTK_BT_LE_GAP_ACT_UPDATE_CONN_PARAM\r\n");
		ret = bt_stack_le_gap_update_conn_param(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_READ_RSSI:
		ret = bt_stack_le_gap_read_rssi(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_MODIFY_WHITELIST:
		ret = bt_stack_le_gap_modify_whitelist(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_CONN_INFO:
		ret = bt_stack_le_gap_get_conn_info(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_DEV_STATE:
		ret = bt_stack_le_gap_get_dev_state(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_ACTIVE_CONN:
		ret = bt_stack_le_gap_get_active_conn(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_CONN_HANDLE_BY_ADDR:
		ret = bt_stack_le_gap_get_conn_handle_by_addr(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_MTU_SIZE:
		ret = bt_stack_le_gap_get_mtu_size(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_SET_CHANNELS:
		ret = bt_stack_le_gap_set_channels(p_cmd->param);
		break;
#if defined(CONFIG_BT_USER_DATA_LEN_UPDATE)
	case RTK_BT_LE_GAP_ACT_SET_DATA_LEN:
		ret = bt_stack_le_gap_set_data_len(p_cmd->param);
		break;
#endif

#if defined(CONFIG_BT_USER_PHY_UPDATE)
	case RTK_BT_LE_GAP_ACT_SET_PHY:
		ret = bt_stack_le_gap_set_phy(p_cmd->param);
		break;
#endif

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	case RTK_BT_LE_GAP_ACT_PRIVACY_INIT:
		ret = bt_stack_le_privacy_init(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_SET_PRIVACY_MODE:
		ret = bt_stack_le_gap_set_privacy_mode(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_READ_LOCAL_RESOLV_ADDR:
		ret = bt_stack_le_gap_read_local_resolv_addr(p_cmd->param);
		break;
	case RTK_BT_LE_GAP_ACT_READ_PEER_RESOLV_ADDR:
		ret = bt_stack_le_gap_read_peer_resolv_addr(p_cmd->param);
		break;
#endif

	case RTK_BT_LE_GAP_ACT_SET_SEC_PARAM:
		ret = bt_stack_le_sm_set_security_param(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_START_SECURITY:
		ret = bt_stack_le_sm_start_security(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_PAIRING_CONFIRM:
		ret = bt_stack_le_sm_pairing_confirm(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_PASSKEY_ENTRY:
		ret = bt_stack_le_sm_passkey_entry(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_PASSKEY_CONFIRM:
		ret = bt_stack_le_sm_passkey_confirm(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_OOB_KEY_INPUT:
		ret = bt_stack_le_sm_set_oob_data(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_BOND_NUM:
		ret = bt_stack_le_sm_get_bond_num(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_GET_BOND_INFO:
		ret = bt_stack_le_sm_get_bond_info(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_DELETE_BOND_DEVICE:
		ret = bt_stack_le_sm_delete_bond_device(p_cmd->param);
		break;

	case RTK_BT_LE_GAP_ACT_CLEAR_BOND_LIST:
		ret = bt_stack_le_sm_clear_bond_list();
		break;

	case RTK_BT_LE_GAP_ACT_GET_TX_PENDING_NUM:
		//ret = bt_stack_le_gap_get_tx_pending_num(p_cmd->param);
		break;

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
	case RTK_BT_LE_GAP_ACT_READ_LOCAL_TX_POWER:
		//p_cmd->user_data = GAP_LE_RF_ENHANCED_READ_TRANSMIT_POWER_LEVEL | (GAP_MSG_LE_RF_INFO << 8);
		//bt_stack_pending_cmd_insert(p_cmd);
		//ret = bt_stack_le_gap_read_local_tx_power(p_cmd->param);
		//goto async_handle;
		break;

	case RTK_BT_LE_GAP_ACT_READ_REMOTE_TX_POWER:
		//p_cmd->user_data = GAP_LE_RF_READ_REMOTE_TRANSMIT_POWER_LEVEL | (GAP_MSG_LE_RF_INFO << 8);
		//bt_stack_pending_cmd_insert(p_cmd);
		//ret = bt_stack_le_gap_read_remote_tx_power(p_cmd->param);
		//goto async_handle;
		break;

	case RTK_BT_LE_GAP_ACT_TX_POWER_REPORT_SET:
		//p_cmd->user_data = GAP_LE_RF_SET_TRANSMIT_POWER_REPORTING_ENABLE | (GAP_MSG_LE_RF_INFO << 8);
		//bt_stack_pending_cmd_insert(p_cmd);
		//ret = bt_stack_le_gap_tx_power_report_set(p_cmd->param);
		//goto async_handle;
		break;
#endif

	case RTK_BT_LE_GAP_ACT_GET_CONN_ID: {
		rtk_bt_le_get_conn_id_t *p_param = (rtk_bt_le_get_conn_id_t *)p_cmd->param;
		ret = bt_stack_le_gap_get_conn_id(p_param->conn_handle, p_param->p_conn_id);
		break;
	}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	case RTK_BT_LE_GAP_ACT_GET_ANTENNA_INFO: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_GET_ANTENNA_INFO\r\n");
		ret = bt_stack_le_gap_get_antenna_info(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_START: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_START\r\n");
		ret = bt_stack_le_gap_connless_cte_rx_start(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_STOP: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONNLESS_CTE_RX_STOP\r\n");
		ret = bt_stack_le_gap_connless_cte_rx_stop(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONN_CTE_RX_START: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONN_CTE_RX_START\r\n");
		ret = bt_stack_le_gap_conn_cte_rx_start(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONN_CTE_RX_STOP: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONN_CTE_RX_STOP\r\n");
		ret = bt_stack_le_gap_conn_cte_rx_stop(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONN_CTE_TX_START: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONN_CTE_TX_START\r\n");
		ret = bt_stack_le_gap_conn_cte_tx_start(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONN_CTE_TX_STOP: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONN_CTE_TX_STOP\r\n");
		ret = bt_stack_le_gap_conn_cte_tx_stop(p_cmd->param);
		break;
	}

#if (defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV) && \
(defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT)
	case RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_START: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_START\r\n");
		ret = bt_stack_le_gap_connless_cte_tx_start(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_STOP: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_CONNLESS_CTE_TX_STOP\r\n");
		ret = bt_stack_le_gap_connless_cte_tx_stop(p_cmd->param);
		break;
	}
#endif
#endif

#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
	case RTK_BT_LE_GAP_ACT_PAWR_SET_SUBEVENT_DATA: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAWR_SET_SUBEVENT_DATA\r\n");
		ret = bt_stack_le_pawr_set_subevent_data(p_cmd->param);
		break;
	}
#endif

#if defined(RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT) && RTK_BLE_5_4_PA_SYNC_RSP_SUPPORT
	case RTK_BT_LE_GAP_ACT_PAWR_SET_RESPONSE_DATA: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAWR_SET_RESPONSE_DATA\r\n");
		ret = bt_stack_le_pawr_set_response_data(p_cmd->param);
		break;
	}

	case RTK_BT_LE_GAP_ACT_PAWR_SYNC_SUBEVENT: {
		BT_LOGD("RTK_BT_LE_GAP_ACT_PAWR_SYNC_SUBEVENT\r\n");
		ret = bt_stack_le_pawr_sync_subevent(p_cmd->param);
		break;
	}
#endif

	default:
		BT_LOGE("bt_stack_le_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = RTK_BT_ERR_NO_CASE_ELEMENT;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}