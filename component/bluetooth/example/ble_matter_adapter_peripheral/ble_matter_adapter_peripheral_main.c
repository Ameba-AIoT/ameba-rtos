/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

/*============================================================================*
 *                              Header
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <bt_utils.h>

#include "matter_blemgr_common.h"
#include "ble_matter_adapter_peripheral_main.h"
#include "ble_matter_adapter_service.h"

/*============================================================================*
 *                              Constants
 *============================================================================*/
#define RTK_BT_DEV_NAME "BLE_ADAPTER"

#define BLE_MATTER_ADAPTER_CALLBACK_TASK_PRIORITY 4
#define BLE_MATTER_ADAPTER_CALLBACK_TASK_STACK_SIZE 256 * 6
#define BLE_MATTER_ADAPTER_MAX_NUMBER_OF_CALLBACK_MESSAGE 0x20

void *ble_matter_adapter_callback_task_handle = NULL;
void *ble_matter_adapter_callback_queue_handle = NULL;
/*============================================================================*
 *                              Variables
 *============================================================================*/
static uint8_t ble_local_addr[RTK_BD_ADDR_LEN] = {0};           /* BLE local address */
static uint8_t ble_matter_addr[RTK_BD_ADDR_LEN] = {0};          /* Matter address type is Random */

#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
static MULTI_ADV_INFO ble_matter_adv_info = {0};                /* BLE Info */
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static rtk_bt_le_ext_adv_param_t ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED,
	.primary_adv_interval_min = 0x20,
	.primary_adv_interval_max = 0x20,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_1M,
};
#else
#error "Please enable RTK_BLE_5_0_AE_ADV_SUPPORT in bt_api_config.h"
#endif // RTK_BLE_5_0_AE_ADV_SUPPORT

static uint8_t def_adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x0C,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'C', 'U', 'S', 'T', 'O', 'M', 'E', 'R', 'A', 'D', 'V',
};

static uint8_t def_scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

#else
static uint8_t ble_matter_adv_data[31] = {0};
static uint8_t ble_matter_adv_data_length = 0;

static rtk_bt_le_adv_param_t adv_param = {
	.interval_min = 0x20, //units of 0.625ms
	.interval_max = 0x20,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
#endif // RTK_BLE_5_0_AE_ADV_SUPPORT

static rtk_bt_le_security_param_t sec_param = {
	.io_cap = RTK_IO_CAP_NO_IN_NO_OUT,
	.oob_data_flag = 0,
	.bond_flag = 1,
	.mitm_flag = 0,
	.sec_pair_flag = 0,
	.use_fixed_key = 0,
	.fixed_key = 000000,
};

typedef struct {
	uint16_t type;
	uint16_t subtype;
	union {
		uint32_t param;
		void *buf;
	} u;
} T_MATTER_CB_MSG;

extern matter_blemgr_callback matter_blemgr_callback_func;
extern void *matter_blemgr_callback_data;

/*============================================================================*
 *					   Helper Functions
 *============================================================================*/
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
static void ble_matter_set_adv_state(uint8_t adv_handle, bool is_start)
{
	if ((ble_matter_adv_info.matter_adv_create) && (ble_matter_adv_info.matter_adv_handle == adv_handle)) {
		ble_matter_adv_info.matter_adv_start = is_start;
		BT_LOGD("[MATTER] start matter adv: %d\r\n", is_start);
	} else if ((ble_matter_adv_info.customer_adv_create) && (ble_matter_adv_info.customer_adv_handle == adv_handle)) {
		ble_matter_adv_info.customer_adv_start = is_start;
		BT_LOGD("[MATTER] start customer adv: %d\r\n", is_start);
	} else {
		BT_LOGE("[MATTER] unknown adv_handle: %d\r\n", adv_handle);
	}
}

static void ble_matter_remove_adv(uint8_t adv_type)
{
	if (BLE_MATTER_ADV == adv_type) {
		ble_matter_adv_info.matter_adv_create = 0;
		BT_LOGD("[MATTER] removing matter adv\r\n");
		if (ble_matter_adv_info.matter_adv_data) {
			osif_mem_free(ble_matter_adv_info.matter_adv_data);
		}
	} else if (BLE_CUSTOMER_ADV == adv_type) {
		ble_matter_adv_info.customer_adv_create = 0;
		BT_LOGD("[MATTER] removing customer adv\r\n");
		if (ble_matter_adv_info.customer_adv_data) {
			osif_mem_free(ble_matter_adv_info.customer_adv_data);
		}
		if (ble_matter_adv_info.customer_scanrsp_data) {
			osif_mem_free(ble_matter_adv_info.customer_scanrsp_data);
		}
	} else {
		BT_LOGE("[MATTER] unknown adv_type: %d\r\n", adv_type);
	}
}

static int ble_matter_config_adv(uint8_t adv_type, MATTER_ADV_PARAM *adv_param)
{
	uint8_t adv_handle = 0;
	BT_LOGD("[MATTER] adv_type: %d\r\n", adv_type);
	if (BLE_MATTER_ADV == adv_type) {	   /* Matter used Random address */
		ext_adv_param.own_addr.type = RTK_BT_LE_ADDR_TYPE_RANDOM;
		memcpy(ext_adv_param.own_addr.addr_val, ble_matter_addr, sizeof(ble_matter_addr));
	} else {
		ext_adv_param.own_addr.type = RTK_BT_LE_ADDR_TYPE_PUBLIC;
		memcpy(ext_adv_param.own_addr.addr_val, ble_local_addr, sizeof(ble_local_addr));
	}
	ext_adv_param.primary_adv_interval_min = adv_param->adv_int_min;
	ext_adv_param.primary_adv_interval_max = adv_param->adv_int_max;

	if (BLE_MATTER_ADV == adv_type) {
		BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&ext_adv_param, &adv_handle));
		ble_matter_adv_info.matter_adv_create = 1;
		ble_matter_adv_info.matter_adv_handle = adv_handle;
		BT_LOGD("[MATTER] matter_adv_handle: %d\r\n", ble_matter_adv_info.matter_adv_handle);

		if (adv_param->adv_datalen) {
			ble_matter_adv_info.matter_adv_datalen = adv_param->adv_datalen;
			ble_matter_adv_info.matter_adv_data = osif_mem_alloc(0, adv_param->adv_datalen);
			if (ble_matter_adv_info.matter_adv_data) {
				memcpy(ble_matter_adv_info.matter_adv_data, adv_param->adv_data, adv_param->adv_datalen);
			} else {
				BT_LOGE("[MATTER] malloc failed\r\n");
				return -1;
			}
		}
		BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(adv_handle, ble_matter_adv_info.matter_adv_data, ble_matter_adv_info.matter_adv_datalen));
	} else if (BLE_CUSTOMER_ADV == adv_type) {
		BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&ext_adv_param, &adv_handle));
		ble_matter_adv_info.customer_adv_create = 1;
		ble_matter_adv_info.customer_adv_handle = adv_handle;
		BT_LOGD("[MATTER] customer_adv_handle: %d\r\n", ble_matter_adv_info.customer_adv_handle);

		if (adv_param->adv_datalen) {
			ble_matter_adv_info.customer_adv_datalen = adv_param->adv_datalen;
			ble_matter_adv_info.customer_adv_data = osif_mem_alloc(0, adv_param->adv_datalen);
			if (ble_matter_adv_info.customer_adv_data) {
				memcpy(ble_matter_adv_info.customer_adv_data, adv_param->adv_data, adv_param->adv_datalen);
			} else {
				BT_LOGE("[MATTER] malloc failed\r\n");
				return -1;
			}
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(adv_handle, ble_matter_adv_info.customer_adv_data, ble_matter_adv_info.customer_adv_datalen));
		}

		if (adv_param->scanrsp_datalen) {
			ble_matter_adv_info.customer_scanrsp_datalen = adv_param->scanrsp_datalen;
			ble_matter_adv_info.customer_scanrsp_data = osif_mem_alloc(0, adv_param->scanrsp_datalen);
			if (ble_matter_adv_info.customer_scanrsp_data) {
				memcpy(ble_matter_adv_info.customer_scanrsp_data, adv_param->scanrsp_data, adv_param->scanrsp_datalen);
			} else {
				BT_LOGE("[MATTER] malloc failed\r\n");
				return -1;
			}
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_scan_rsp_data(adv_handle, ble_matter_adv_info.customer_scanrsp_data, ble_matter_adv_info.customer_scanrsp_datalen));
		}
	} else {
		BT_LOGE("[MATTER] unknown adv_type: %d\r\n", adv_type);
	}
	return 0;
}

static int ble_matter_connection_chk(uint16_t conn_handle)
{
	bool ret = true;
	if (ble_matter_adv_info.matter_conn_handle == conn_handle) {
		BT_LOGD("[MATTER] matter connection\r\n");
	} else if (ble_matter_adv_info.customer_conn_handle == conn_handle) {
		BT_LOGD("[MATTER] customer connection\r\n");
		ret = false;
	} else {
		BT_LOGE("[MATTER] unknown connection\r\n");
		ret = -1;
	}
	return ret;
}

static void ble_matter_connection_set(uint16_t conn_handle, bool add_conn)
{
	uint8_t adv_handle;
	if (add_conn) {
		rtk_bt_le_gap_get_ext_adv_handle_by_conn_handle(conn_handle, &adv_handle);
		if (ble_matter_adv_info.matter_adv_handle == adv_handle) {
			ble_matter_adv_info.matter_conn_handle = conn_handle;
			BT_LOGD("[MATTER] matter_conn_handle\r\n", conn_handle);
		} else if (ble_matter_adv_info.customer_adv_handle == adv_handle) {
			ble_matter_adv_info.customer_conn_handle = conn_handle;
			BT_LOGD("[MATTER] customer_conn_handle\r\n", conn_handle);
		} else {
			BT_LOGE("[MATTER] unknown connection\r\n");
		}
	} else {
		int conn = ble_matter_connection_chk(conn_handle);
		if (conn) {
			ble_matter_adv_info.matter_conn_handle = 0;
		} else if (0 == conn) {
			ble_matter_adv_info.customer_conn_handle = 0;
		} else {
			BT_LOGE("[MATTER] unknown connection\r\n");
		}
	}
}

static int ble_matter_connected_chk(uint8_t adv_type)
{
	bool ret = false;
	if (BLE_MATTER_ADV == adv_type) {
		if (ble_matter_adv_info.matter_conn_handle) {
			ret = true;
		}
	} else if (BLE_CUSTOMER_ADV == adv_type) {
		if (ble_matter_adv_info.customer_conn_handle) {
			ret = true;
		}
	} else {
		BT_LOGE("[MATTER] unknown adv_type: %d\r\n", adv_type);
	}
	return ret;
}
#endif // CONFIG_BLE_MATTER_MULTI_ADV_ON

/*============================================================================*
 *							  Functions
 *============================================================================*/
void ble_matter_adapter_app_handle_callback_msg(T_MATTER_CB_MSG callback_msg);
bool ble_matter_adapter_send_callback_msg(uint16_t msg_type, uint16_t cb_type, void *arg);
void ble_matter_adapter_callback_main_task(void *p_param)
{
	(void)p_param;
	T_MATTER_CB_MSG callback_msg;
	osif_msg_queue_create(&ble_matter_adapter_callback_queue_handle, BLE_MATTER_ADAPTER_MAX_NUMBER_OF_CALLBACK_MESSAGE, sizeof(T_MATTER_CB_MSG));

	while (true) {
		if (osif_msg_recv(ble_matter_adapter_callback_queue_handle, &callback_msg, 0xFFFFFFFF) == true) {
			if (callback_msg.type == BLE_MATTER_MSG_DEINIT) {
				break;
			}
			ble_matter_adapter_app_handle_callback_msg(callback_msg);
		}
	}
	osif_msg_queue_delete(ble_matter_adapter_callback_queue_handle);
	ble_matter_adapter_callback_queue_handle = NULL;
	ble_matter_adapter_callback_task_handle = NULL;
	osif_task_delete(NULL);
}

void ble_matter_adapter_callback_task_init(void)
{
	osif_task_create(&ble_matter_adapter_callback_task_handle,
					"ble_matter_adapter_callback",
					ble_matter_adapter_callback_main_task,
					0,
					BLE_MATTER_ADAPTER_CALLBACK_TASK_STACK_SIZE,
					BLE_MATTER_ADAPTER_CALLBACK_TASK_PRIORITY);
}

void ble_matter_adapter_callback_task_deinit(void)
{
	if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_DEINIT, NULL, NULL) == false) {
		BT_LOGE("[APP] send callback msg failed\r\n");
	}
	return;
}

bool ble_matter_adapter_send_callback_msg(uint16_t msg_type, uint16_t cb_type, void *arg)
{
	(void) cb_type;
	T_MATTER_CB_MSG callback_msg;
	callback_msg.type = msg_type;

	callback_msg.u.buf = arg;
	if (ble_matter_adapter_callback_queue_handle != NULL) {
		if (osif_msg_send(ble_matter_adapter_callback_queue_handle, &callback_msg, 0) == false) {
			BT_LOGE("[MATTER] send cb msg type 0x%x failed\r\n", callback_msg.type);
			return false;
		}
		return true;
	}
	return false;
}

void ble_matter_adapter_app_handle_callback_msg(T_MATTER_CB_MSG callback_msg)
{
	uint16_t msg_type = callback_msg.type;
	switch (msg_type) {
	case BLE_MATTER_MSG_CONNECTED: {
		BLE_MATTER_APP_CONNECT_CB_ARG *matter_msg_connected = callback_msg.u.buf;
		T_MATTER_BLEMGR_GAP_CONNECT_CB_ARG gap_connect_cb_arg;
		gap_connect_cb_arg.conn_id = matter_msg_connected->conn_handle;
		BT_LOGD("[MATTER] get conn_id %d from case BLE_MATTER_MSG_CONNECTED\r\n", gap_connect_cb_arg.conn_id);
		if (matter_blemgr_callback_func) {
			matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_GAP_CONNECT_CB, &gap_connect_cb_arg);
		}
		osif_mem_free(callback_msg.u.buf);
		callback_msg.u.buf = NULL;
	}
	break;

	case BLE_MATTER_MSG_DISCONNECTED: {
		BLE_MATTER_APP_DISCONNECT_CB_ARG *matter_msg_disconnected = callback_msg.u.buf;
		T_MATTER_BLEMGR_GAP_DISCONNECT_CB_ARG gap_disconnect_cb_arg;
		gap_disconnect_cb_arg.conn_id = matter_msg_disconnected->conn_handle;
		gap_disconnect_cb_arg.disc_cause = matter_msg_disconnected->disc_cause;
		BT_LOGD("[MATTER] get conn_id %d from case BLE_MATTER_MSG_DISCONNECTED\r\n", gap_disconnect_cb_arg.conn_id);
		BT_LOGD("[MATTER] get disconnection cause %d\r\n", gap_disconnect_cb_arg.disc_cause);
		if (matter_blemgr_callback_func) {
			matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_GAP_DISCONNECT_CB, &gap_disconnect_cb_arg);
		}
		osif_mem_free(callback_msg.u.buf);
		callback_msg.u.buf = NULL;
	}
	break;

	case BLE_MATTER_MSG_WRITE_CB: {
		BLE_MATTER_APP_WRITE_CB_ARG *matter_msg_write = callback_msg.u.buf;
		T_MATTER_BLEMGR_RX_CHAR_WRITE_CB_ARG gap_write_cb_arg;
		gap_write_cb_arg.conn_id = matter_msg_write->conn_handle;
		gap_write_cb_arg.p_value = matter_msg_write->p_value;
		gap_write_cb_arg.len = matter_msg_write->len;
		BT_LOGD("[MATTER] get conn_id %d from case BLE_MATTER_MSG_WRITE_CB\r\n", gap_write_cb_arg.conn_id);
		BT_LOGD("[MATTER] ble matter write event, len: %d, data: ", gap_write_cb_arg.len);
		for (int i = 0; i < gap_write_cb_arg.len; i++) {
			if (0 == i % 16) {
				BT_LOGD("\r\n");
			}
			BT_LOGD("%02x ", *(gap_write_cb_arg.p_value + i));
		}
		BT_LOGD("\r\n");
		if (matter_blemgr_callback_func) {
			matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_RX_CHAR_WRITE_CB, &gap_write_cb_arg);
		}
		osif_mem_free(callback_msg.u.buf);
		callback_msg.u.buf = NULL;
	}
	break;

	case BLE_MATTER_MSG_CCCD_EN:
	case BLE_MATTER_MSG_CCCD_DIS: {
		BLE_MATTER_APP_CCCD_CB_ARG *matter_msg_cccd = callback_msg.u.buf;
		T_MATTER_BLEMGR_TX_CHAR_CCCD_WRITE_CB_ARG gap_cccd_cb_arg;
		gap_cccd_cb_arg.conn_id = matter_msg_cccd->conn_handle;
		gap_cccd_cb_arg.indicationsEnabled = matter_msg_cccd->indicationsEnabled;
		gap_cccd_cb_arg.notificationsEnabled = matter_msg_cccd->notificationsEnabled;
		BT_LOGD("[MATTER] get conn_id %d from case BLE_MATTER_MSG_CCCD_EN\r\n", gap_cccd_cb_arg.conn_id);
		BT_LOGD("[MATTER] get indicationsEnabled %d\r\n", gap_cccd_cb_arg.indicationsEnabled);
		BT_LOGD("[MATTER] get notificationsEnabled %d\r\n", gap_cccd_cb_arg.notificationsEnabled);
		if (matter_blemgr_callback_func) {
			matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_TX_CHAR_CCCD_WRITE_CB, &gap_cccd_cb_arg);
		}
		osif_mem_free(callback_msg.u.buf);
		callback_msg.u.buf = NULL;
	}
	break;

	case BLE_MATTER_MSG_SEND_COMPLETE: {
		BLE_MATTER_APP_SEND_COMPLETE_CB_ARG *matter_msg_complete = callback_msg.u.buf;
		T_MATTER_BLEMGR_TX_COMPLETE_CB_ARG gap_complete_cb_arg;
		gap_complete_cb_arg.conn_id = matter_msg_complete->conn_handle;
		BT_LOGD("[MATTER] get conn_id %d from case BLE_MATTER_MSG_SEND_COMPLETE\r\n", gap_complete_cb_arg.conn_id);
		if (matter_blemgr_callback_func) {
			matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_TX_COMPLETE_CB, &gap_complete_cb_arg);
		}
		osif_mem_free(callback_msg.u.buf);
		callback_msg.u.buf = NULL;
	}
	break;

	default:
		BT_LOGE("[MATTER] unknown type(%d) cb msg \r\n", callback_msg.type);
		break;
	}
}

static rtk_bt_evt_cb_ret_t ble_peripheral_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP] adv started: adv_type %d\r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP] adv start failed, err 0x%x\r\n", adv_start_ind->err);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] adv stopped: reason 0x%x\r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] adv stop failed, err 0x%x\r\n", adv_stop_ind->err);
		}
		break;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				BT_LOGA("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				BT_LOGA("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
			}
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && (CONFIG_BLE_MATTER_MULTI_ADV_ON)
			ble_matter_set_adv_state(ext_adv_ind->adv_handle, ext_adv_ind->is_start);
#endif
		} else {
			if (ext_adv_ind->is_start) {
				BT_LOGE("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				BT_LOGE("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			}
		}
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] connected, handle: %d, role: %s, remote device: %s\r\n", conn_ind->conn_handle, role, le_addr);

#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
			ble_matter_connection_set(conn_ind->conn_handle, true);
			if (ble_matter_connection_chk(conn_ind->conn_handle)) {
#endif
				BLE_MATTER_APP_CONNECT_CB_ARG *conn_msg_matter = (BLE_MATTER_APP_CONNECT_CB_ARG *)osif_mem_alloc(0, sizeof(T_MATTER_BLEMGR_GAP_CONNECT_CB_ARG));
				if (conn_msg_matter) {
					memset(conn_msg_matter, 0, sizeof(BLE_MATTER_APP_CONNECT_CB_ARG));
					conn_msg_matter->conn_handle = conn_ind->conn_handle;
					if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_CONNECTED, NULL, conn_msg_matter) == false) {
						BT_LOGE("[APP] send callback msg failed\r\n");
						osif_mem_free(conn_msg_matter);
						conn_msg_matter = NULL;
					}
				} else {
					BT_LOGE("[APP] malloc failed\r\n");
				}
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
			} else {
				/* Non-Matter Connection */
			}
#endif
		} else {
			BT_LOGE("[APP] connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}

		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(disconn_ind->conn_handle)) {
#endif
			BLE_MATTER_APP_DISCONNECT_CB_ARG *disconn_msg_matter = (BLE_MATTER_APP_DISCONNECT_CB_ARG *)osif_mem_alloc(0, sizeof(BLE_MATTER_APP_DISCONNECT_CB_ARG));
			if (disconn_msg_matter) {
				memset(disconn_msg_matter, 0, sizeof(BLE_MATTER_APP_DISCONNECT_CB_ARG));
				disconn_msg_matter->conn_handle = disconn_ind->conn_handle;
				disconn_msg_matter->disc_cause = disconn_ind->reason;
				if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_DISCONNECTED, NULL, disconn_msg_matter) == false) {
					BT_LOGE("[APP] send callback msg failed\r\n");
					osif_mem_free(disconn_msg_matter);
					disconn_msg_matter = NULL;
				}
			} else {
				BT_LOGE("[APP] malloc failed\r\n");
			}
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		} else {
			/* Non-Matter Connection */
			ble_customer_start_adv();
		}
		ble_matter_connection_set(disconn_ind->conn_handle, false);
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind = (rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			BT_LOGE("[APP] update conn param failed, conn_handle: %d, err: 0x%x\r\n",
					conn_update_ind->conn_handle, conn_update_ind->err);
		} else {
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
			if (ble_matter_connection_chk(conn_update_ind->conn_handle)) {
				/* Matter Connection */
			} else {
				/* Non-Matter Connection */
			}
#endif
			BT_LOGA("[APP] conn param is updated, conn_handle: %d, conn_interval: 0x%x, " \
						"conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
						conn_update_ind->conn_handle,
						conn_update_ind->conn_interval,
						conn_update_ind->conn_latency,
						conn_update_ind->supv_timeout);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT sync api shall not be called here
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req = (rtk_bt_le_remote_conn_update_req_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(rmt_update_req->conn_handle)) {
			/* Matter Connection */
		} else {
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] remote device request a change in conn param, conn_handle: %d, "\
					"conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "\
					"timeout: 0x%x. The host stack accept it.\r\n",
					rmt_update_req->conn_handle,
					rmt_update_req->conn_interval_max,
					rmt_update_req->conn_interval_min,
					rmt_update_req->conn_latency,
					rmt_update_req->supv_timeout);
		return RTK_BT_EVT_CB_ACCEPT;
		break;
	}

	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change = (rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] data len is updated, conn_handle: %d, " \
					"max_tx_octets: 0x%x, max_tx_time: 0x%x, "  \
					"max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
					data_len_change->conn_handle,
					data_len_change->max_tx_octets,
					data_len_change->max_tx_time,
					data_len_change->max_rx_octets,
					data_len_change->max_rx_time);
		break;
	}

	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind = (rtk_bt_le_phy_update_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(phy_update_ind->conn_handle)) {
			/* Matter Connection */
		} else {
			/* Non-Matter Connection */
		}
#endif
		if (phy_update_ind->err) {
			BT_LOGE("[APP] update PHY failed, conn_handle: %d, err: 0x%x\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->err);
		} else {
			BT_LOGA("[APP] phy is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind = (rtk_bt_le_auth_pair_cfm_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(pair_cfm_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] just work pairing need user to confirm, conn_handle: %d!\r\n",
					pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		uint16_t ret = 0;
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] just work pairing auto confirm succcess\r\n");
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind = (rtk_bt_le_auth_key_display_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(key_dis_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] auth passkey display: %ld, conn_handle:%d\r\n",
					key_dis_ind->passkey,
					key_dis_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind = (rtk_bt_le_auth_key_input_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(key_input_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] please input the auth passkey get from remote, conn_handle: %d\r\n",
					key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind = (rtk_bt_le_auth_key_cfm_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(key_cfm_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] auth passkey confirm: %ld, conn_handle: %d. "  \
					"Please comfirm if the passkeys are equal!\r\n",
					key_cfm_ind->passkey,
					key_cfm_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind = (rtk_bt_le_auth_oob_input_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(oob_input_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		BT_LOGA("[APP] bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
					oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind = (rtk_bt_le_auth_complete_ind_t *)param;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(auth_cplt_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] pairing failed(err: 0x%x), conn_handle: %d\r\n",
						auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_LOGA("[APP] long term key is 0x");
			for (uint8_t i = 1; i <= auth_cplt_ind->dev_ltk_length; i++) {
				BT_LOGA("%02x", auth_cplt_ind->dev_ltk[auth_cplt_ind->dev_ltk_length - i]); //End size conversion
			}
			BT_LOGA("\r\n");
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind = (rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] bond info modified, op: %d, addr: %s, ident_addr: %s\r\n",
					bond_mdf_ind->op, le_addr, ident_addr);

		break;
	}

	default:
		BT_LOGE("[APP] unkown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gatts_app_id(uint8_t event, void *data)
{
	uint16_t app_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		app_id = p_reg_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		app_id = p_read_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		app_id = p_write_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_CCCD_IND: {
		rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
		app_id = p_cccd_ind->app_id;
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND:
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ntf = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		app_id = p_ind_ntf->app_id;
		break;
	}
	default:
		break;
	}

	return app_id;
}

static rtk_bt_evt_cb_ret_t ble_peripheral_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t app_id = 0xFFFF;

	if (RTK_BT_GATTS_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(p_gatt_mtu_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] gatts mtu exchange successfully, mtu_size: %d, conn_handle: %d\r\n",
						p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] gatts mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES == event) {
		rtk_bt_gatts_client_supported_features_ind_t *p_ind = (rtk_bt_gatts_client_supported_features_ind_t *)data;
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		if (ble_matter_connection_chk(p_ind->conn_handle)) {
			/* Matter Connection */
		} else {			
			/* Non-Matter Connection */
		}
#endif
		if (p_ind->features & RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT) {
			BT_LOGA("[APP] client Supported features is writed: conn_handle %d, features 0x%02X. Remote client supports EATT\r\n",
						p_ind->conn_handle, p_ind->features);
		}
		return RTK_BT_EVT_CB_OK;
	}

	app_id = app_get_gatts_app_id(event, data);
	switch (app_id) {
	case BLE_MATTER_ADAPTER_SRV_ID:
		ble_matter_adapter_ble_service_callback(event, data);
		break;

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

int ble_matter_adapter_peripheral_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char name[30] = {0};

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.slave_init_mtu_req = false;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
		bt_app_conf.user_def_service = false;
		bt_app_conf.cccd_not_check = false;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));

		memcpy(ble_local_addr, bd_addr.addr_val, sizeof(bd_addr));

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_peripheral_gap_app_callback));
		memcpy(name, (const char *)RTK_BT_DEV_NAME, strlen((const char *)RTK_BT_DEV_NAME));
		BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)name));
		BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT));

		BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));

		/* Set random addr */
		uint16_t ret = rtk_bt_le_gap_set_rand_addr(true, RTK_BT_LE_RAND_ADDR_STATIC, ble_matter_addr);
		if (ret) {
			BT_LOGE("[MATTER] gap set random address failed! err: 0x%x\r\n", ret);
		}
		BT_LOGA("[MATTER] ble random addr: %x:%x:%x:%x:%x:%x\r\n",
					ble_matter_addr[0], ble_matter_addr[1], ble_matter_addr[2],
					ble_matter_addr[3], ble_matter_addr[4], ble_matter_addr[5]);

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, ble_peripheral_gatts_app_callback));
		BT_APP_PROCESS(ble_matter_adapter_srv_add());
		ble_matter_adapter_callback_task_init();
		
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
		ble_customer_adv_setup();
#endif
	} else if (0 == enable) {
		ble_matter_adapter_callback_task_deinit();
		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}

int ble_matter_adapter_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *padv_data, uint8_t padv_data_length)
{
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
	MATTER_ADV_PARAM adv_param = {0};

	while (ble_matter_adv_info.matter_adv_start) {
		BT_LOGA("[MATTER] rtk_bt_le_gap_stop_ext_adv, adv_handle: %d\n", ble_matter_adv_info.matter_adv_handle);
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.matter_adv_handle));
		osif_delay(100);
	}

	/* Remove old adv data and handle */
	if (ble_matter_adv_info.matter_adv_create) {
		BT_APP_PROCESS(rtk_bt_le_gap_remove_ext_adv(ble_matter_adv_info.matter_adv_handle));
		ble_matter_remove_adv(BLE_MATTER_ADV);
	}

	adv_param.adv_data = padv_data;
	adv_param.adv_datalen = padv_data_length;
	adv_param.adv_int_min = adv_int_min;
	adv_param.adv_int_max = adv_int_max;
	ble_matter_config_adv(BLE_MATTER_ADV, &adv_param);  /* Create new Matter Adv handle and set adv data */
#else
	rtk_bt_le_gap_dev_state_t dev_state;
	while (1) {
		if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
			if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_ADVERTISING) {
				BT_LOGD("[MATTER] adv is adving, stop adv first\r\n");
				BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
			} else if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				BT_LOGD("[MATTER] adv is idle, continue to process\r\n");
				break;
			}
		} else {
			BT_LOGE("[MATTER] get device status failed\r\n");
		}
		osif_delay(100);
	}

	//sync adv_data
	memset(ble_matter_adv_data, 0, padv_data_length);
	memcpy(ble_matter_adv_data, padv_data, padv_data_length);
	ble_matter_adv_data_length = padv_data_length;

	BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(ble_matter_adv_data, ble_matter_adv_data_length));

	adv_param.interval_min = adv_int_min;
	adv_param.interval_max = adv_int_max;
#endif
	return 0;
}

int ble_matter_adapter_start_adv(void)
{
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
	MATTER_ADV_PARAM adv_param = {0};
	while (ble_matter_adv_info.matter_adv_start) {
		BT_LOGD("[MATTER] rtk_bt_le_gap_stop_ext_adv, adv_handle: %d\n", ble_matter_adv_info.matter_adv_handle);
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.matter_adv_handle));
		osif_delay(100);
	}

	if (ble_matter_connected_chk(BLE_MATTER_ADV)) {
		BT_LOGE("[MATTER] Matter connected, do not start Adv again!\n");
	}
	BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(ble_matter_adv_info.matter_adv_handle, 0, 0));
#else
	rtk_bt_le_gap_dev_state_t dev_state;
	while (1) {
		if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
			if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_ADVERTISING) {
				BT_LOGD("[MATTER] adv is adving, stop adv first\r\n");
				BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
			} else if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				BT_LOGD("[MATTER] adv is idle, continue to process\r\n");
				break;
			}
		} else {
			BT_LOGE("[MATTER] Get device status failed\r\n");
		}
		osif_delay(100);
	}
	adv_param.own_addr_type = RTK_BT_LE_ADDR_TYPE_RANDOM;

	BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
#endif
	return 0;
}

int ble_matter_adapter_stop_adv(void)
{
#if defined(CONFIG_BLE_MATTER_MULTI_ADV_ON) && CONFIG_BLE_MATTER_MULTI_ADV_ON
	while (ble_matter_adv_info.matter_adv_start) {
		BT_LOGD("[MATTER] rtk_bt_le_gap_stop_ext_adv, adv_handle: %d\n", ble_matter_adv_info.matter_adv_handle);
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.matter_adv_handle));
		osif_delay(100);
	}
#else
	rtk_bt_le_gap_dev_state_t dev_state;
	while (1) {
		if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
			if (dev_state.gap_adv_state != RTK_BT_LE_ADV_STATE_IDLE) {
				BT_LOGD("[MATTER] adv is not idle, stop adv\r\n");
				BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
			} else {
				BT_LOGD("[MATTER] adv is idle\r\n");
				break;
			}
		} else {
			BT_LOGE("[MATTER] get device status failed\r\n");
		}
		osif_delay(100);
	}
#endif
	return 0;
}

uint16_t ble_matter_adapter_get_mtu(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint16_t mtu_size;

	if ((ret = rtk_bt_le_gap_get_mtu_size(conn_handle, &mtu_size)) != RTK_BT_OK) {
		BT_LOGE("[MATTER] gap get mtu size failed! err: 0x%x\r\n", ret);
		return 0xFFFF;
	}

	BT_LOGD("[MATTER] gap get mtu size, conn_handle: %d, mtu_size: %d\r\n", conn_handle, mtu_size);

	return mtu_size;
}

int ble_matter_adapter_set_device_name(char *device_name)
{
	int ret = rtk_bt_le_gap_set_device_name((const uint8_t *)device_name);
	if (ret) {
		BT_LOGE("[MATTER] set device name failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGD("[MATTER] set device name success\r\n");

	return 0;
}

int ble_matter_adapter_send_indication(uint16_t conn_handle, uint8_t *data, uint16_t data_length)
{
	uint16_t ret = 0;
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};

	ntf_param.conn_handle = conn_handle;
	ntf_param.app_id = BLE_MATTER_ADAPTER_SRV_ID;
	ntf_param.index = BLE_MATTER_SERVICE_CHAR_TX_INDEX;
	ntf_param.len = data_length;
	ntf_param.data = (void *)osif_mem_alloc(0, ntf_param.len);
	memcpy((uint8_t *)ntf_param.data, data, ntf_param.len);

	ret = rtk_bt_gatts_indicate(&ntf_param);
	if (RTK_BT_OK != ret) {
		osif_mem_free((void *)ntf_param.data);
		BT_LOGE("[MATTER] gatts indicate failed! err: 0x%x\r\n", ret);
		return -1;
	}

	osif_mem_free((void *)ntf_param.data);
	BT_LOGD("[MATTER] gatts indicate sending ...\r\n");

	return 0;
}

int ble_matter_adapter_disconnect(uint16_t conn_handle)
{
	int ret;

	if ((ret = rtk_bt_le_gap_disconnect(conn_handle)) != 0) {
		BT_LOGE("[MATTER] gap disconnect ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	return 0;
}
#if CONFIG_BLE_MATTER_MULTI_ADV_ON
/*============================================================================*
 *					   Customer Adv Functions
 *============================================================================*/
int ble_customer_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *padv_data, uint8_t padv_data_length, uint8_t *pscanrsp_data, uint8_t pcanrsp_datalen)
{
	MATTER_ADV_PARAM adv_param = {0};
	while (ble_matter_adv_info.customer_adv_start) {
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.customer_adv_handle));
		osif_delay(100);
	}

	/* Remove old adv data and handle */
	if (ble_matter_adv_info.customer_adv_create) {
		BT_APP_PROCESS(rtk_bt_le_gap_remove_ext_adv(ble_matter_adv_info.customer_adv_handle));
		ble_matter_remove_adv(BLE_CUSTOMER_ADV);
	}

	adv_param.adv_data = padv_data;
	adv_param.adv_datalen = padv_data_length;
	adv_param.scanrsp_data = pscanrsp_data;
	adv_param.scanrsp_datalen = pcanrsp_datalen;
	adv_param.adv_int_min = adv_int_min;
	adv_param.adv_int_max = adv_int_max;
	ble_matter_config_adv(BLE_CUSTOMER_ADV, &adv_param);		/* Create new Matter Adv handle and set adv data */
	return 0;
}

int ble_customer_start_adv(void)
{
	MATTER_ADV_PARAM adv_param = {0};
	while (ble_matter_adv_info.customer_adv_start) {
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.customer_adv_handle));
		osif_delay(100);
	}

	BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(ble_matter_adv_info.customer_adv_handle, 0, 0));
	return 0;
}

int ble_customer_stop_adv(void)
{
	while (ble_matter_adv_info.customer_adv_start) {
		BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(ble_matter_adv_info.customer_adv_handle));
		osif_delay(100);
	}

	return 0;
}

void ble_customer_adv_setup(void)
{
	/* Setup and config Adv */
	ble_customer_config_adv(60, 60, def_adv_data, sizeof(def_adv_data), def_scan_rsp_data, sizeof(def_scan_rsp_data));

	/* Start Adv */
	ble_customer_start_adv();
}
#endif
