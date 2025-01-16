/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <bt_utils.h>

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include "rtk_bt_le_iso.h"
#include "app_le_iso_common.h"

bool le_iso_demo_init_flag = false;
static uint8_t app_demo_le_iso_role = RTK_BLE_ISO_ROLE_UNKNOWN;
void app_le_iso_demo_continuous_data_send_control(uint8_t le_iso_role, uint8_t cig_id, bool enable);

app_bt_le_iso_initiator_info_t g_le_iso_cis_initiator_status = {
	.mutex = NULL,
	.cig_num = 0,
	.iso_send_task_sem = NULL,
	.iso_send_msg_q = NULL,
	.iso_send_task_hdl = NULL,
	.initiator_info = {}
};
app_bt_le_iso_acceptor_info_t g_le_iso_cis_acceptor_status = {
	.mutex = NULL,
	.cig_num = 0,
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	.adv_handle = 0,
#endif
	.iso_send_task_sem = NULL,
	.iso_send_msg_q = NULL,
	.iso_send_task_hdl = NULL,
	.acceptor_info = {}
};
app_bt_le_iso_broadcaster_info_t g_le_iso_bis_broadcaster_status = {
	.mutex = NULL,
	.status = RTK_BLE_EXAMPLE_BIS_BROADCASTER_DISABLE,
#if defined(RTK_LE_ISO_DEMO_USE_HW_GTIMER) && RTK_LE_ISO_DEMO_USE_HW_GTIMER
	.iso_send_timer = 0,
#else
	.iso_send_timer = NULL,
#endif
	.iso_send_task_sem = NULL,
	.iso_send_msg_q = NULL,
	.iso_send_task_hdl = NULL,
	.sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S,
	.adv_handle = 0xFF,
	.big_handle = 0xFF,
	.bis_num = 0,
	.bis_conn_handle = {0}
};
static uint8_t g_deflault_big_num = 1;
static uint8_t g_deflault_bis_num = 1;
app_bt_le_iso_receiver_info_t g_le_iso_bis_receiver_status = {
	.mutex = NULL,
	.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_DISABLE,
	.big_handle = 0xFF,
	.sync_id = 0xFF,
	.sync_handle = 0xFF,
	.bis_num = 0,
	.bis_conn_handle = {0}
};

/* for accpetor */
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint8_t def_cis_acceptor_ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x11,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'C', 'I', 'S', '_', 'A', 'C', 'C', 'E', 'P', 'T', 'O', 'R',
	// Manufacturer Specific Data
	0xc3,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0xc0, 0x00,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
	0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
	0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
};

static rtk_bt_le_ext_adv_param_t def_cis_acceptor_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = RTK_BLE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = RTK_BLE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MAX,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};
#else
static uint8_t def_cis_acceptor_adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x11,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'C', 'I', 'S', '_', 'A', 'C', 'C', 'E', 'P', 'T', 'O', 'R',
};

static uint8_t def_cis_acceptor_scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

static rtk_bt_le_adv_param_t def_cis_acceptor_adv_param = {
	.interval_min = 0x30,
	.interval_max = 0x60,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
#endif
/* done for accpetor */

/* for big broadcaster */
static rtk_bt_le_iso_big_broadcaster_param_t def_bis_broadcaster_param = {
	.num_bis = 0,                                                   /**< Total number of BISes in the BIG. */
	.sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US,    /**< The interval, in microseconds, of periodic SDUs.
                                                                        Range: 0x0000FF to 0x0FFFFF */
	.max_sdu = RTK_BLE_ISO_MAX_SDU_M_S,                         /**< Maximum size of an SDU, in octets. */
	.max_transport_latency = RTK_BLE_ISO_DEFAULT_LATENCY_M_S,   /**< Maximum transport latency, in milliseconds.Range: 0x0005 to 0x0FA0 */
	.rtn = RTK_BLE_ISO_DEFAULT_RTN_M_S,                         /**< The number of times that every BIS Data PDU should be retransmitted.Range: 0x00 to 0x1E */
	.phy = RTK_BLE_ISO_DEFAULT_PHY_2M,                          /**< A bit field that indicates the PHY used for transmission of PDUs of BISes in the BIG.
                                                                        bit 0: The transmitter PHY of packets is LE 1M.
                                                                        bit 1: The transmitter PHY of packets is LE 2M.
                                                                        bit 2: The transmitter PHY of packets is LE Coded. */
	.packing = 1,                                                   /**< Indicate the preferred method of arranging subevents of multiple BISes.
                                                                        0x00: Sequential
                                                                        0x01: Interleaved */
	.framing = 0,                                                   /**< Indicate the format for sending BIS Data PDUs.
                                                                        0x00: Unframed
                                                                        0x01: Framed */
	.encryption = 0,                                                /**< Identify the encryption mode of the BISes.
                                                                        0x00: Unencrypted
                                                                        0x01: Encrypted */
	.broadcast_code = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E}
	/* Default value of TSPX_broadcast_code in PTS 8.0.0 is 8ED03323D1205E2D58191BF6285C3182 */
};

static uint8_t def_bis_broadcaster_ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x14,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'I', 'S', '_', 'B', 'O', 'A', 'R', 'D', 'C', 'A', 'S', 'T', 'E', 'R',
};

static rtk_bt_le_ext_adv_param_t def_bis_broadcaster_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
	.primary_adv_interval_min = RTK_BLE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = RTK_BLE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MAX,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};

static uint8_t def_bis_broadcaster_pa_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x14,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'I', 'S', '_', 'B', 'O', 'A', 'R', 'D', 'C', 'A', 'S', 'T', 'E', 'R',
};
/* done for broadcaster */

/* for receiver */
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static rtk_bt_le_ext_scan_param_t def_bis_receiver_ext_scan_param = {
	.own_addr_type      = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.phys               = {true, true},
	.type               = {RTK_BT_LE_SCAN_TYPE_ACTIVE, RTK_BT_LE_SCAN_TYPE_ACTIVE},
	.interval           = {108, 108},
	.window             = {54, 54},
	.duration           = 0,
	.period             = 0,
	.filter_policy      = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt      = RTK_BT_LE_SCAN_DUPLICATE_ENABLE,
};
#endif

rtk_bt_le_iso_big_receiver_create_sync_param_t def_bis_receiver_create_sync_param = {
	.encryption         = 0,
	.broadcast_code     = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E},
	/* Default value of TSPX_broadcast_code in PTS 8.0.0 is 8ED03323D1205E2D58191BF6285C3182 */
	.mse                = 0,
	.big_sync_timeout   = 0x2ee0,   /*timeout = N*10 ms = 120s*/
	.num_bis            = RTK_BLE_BIS_RECEIVER_MAX_NUM,
	.bis                = {}
};
/* done for receiver */

static rtk_bt_evt_cb_ret_t app_bt_le_iso_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	uint16_t  ret = 0;
	char le_addr[30] = {0};
	char *role = NULL;

	switch (evt_code) {

	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP]ADV started: adv_type %d \r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP]ADV start failed, err 0x%x \r\n", adv_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP]ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP]ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,stop,%d,0x%x\r\n", (adv_stop_ind->err == 0) ? 0 : -1, adv_stop_ind->stop_reason);
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
		} else {
			if (ext_adv_ind->is_start) {
				BT_LOGE("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				BT_LOGE("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			}
		}
		BT_AT_PRINT("+BLEGAP:eadv,%s,%d,%d\r\n",
					ext_adv_ind->is_start ? "start" : "stop",
					(ext_adv_ind->err == 0) ? 0 : -1,
					ext_adv_ind->adv_handle);
		break;
	}
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_IND: {
		rtk_bt_le_pa_ind_t *pa_ind = (rtk_bt_le_pa_ind_t *)param;
		if (pa_ind->state == RTK_BT_LE_PA_STATE_IDLE) {
			if (pa_ind->cause) {
				BT_LOGE("[APP] Periodic ADV(%d) stopped failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
			} else {
				BT_LOGA("[APP] Periodic ADV(%d) stopped\r\n", pa_ind->adv_handle);
			}
		} else if (pa_ind->state == RTK_BT_LE_PA_STATE_ADVERTISING) {
			if (pa_ind->cause) {
				BT_LOGE("[APP] Periodic ADV(%d) started failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
			} else {
				BT_LOGA("[APP] Periodic ADV(%d) started\r\n", pa_ind->adv_handle);
				//create big
				def_bis_broadcaster_param.num_bis = g_deflault_bis_num;
			}
		} else if (pa_ind->state == RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING) {
			BT_LOGE("[APP] Periodic ADV(%d) waiting for ext adv start, cause 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
		}
		BT_AT_PRINT("+BLEGAP:pa_adv,%s,%d,%d\r\n",
					((pa_ind->state == RTK_BT_LE_PA_STATE_IDLE) ? "stop" : ((pa_ind->state == RTK_BT_LE_PA_STATE_ADVERTISING) ? "start" : "wait_eadv")),
					((pa_ind->state == RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING) ? 0 : ((pa_ind->cause == 0) ? 0 : -1)),
					pa_ind->adv_handle);
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		uint16_t cause = *(uint16_t *)param;
		if (cause != 0) {
			BT_LOGE("[APP] Start scan failed\r\n");
			break;
		}
		BT_LOGA("[APP] Start scan success\r\n");
		BT_AT_PRINT("+BLEGAP:scan,start,%d\r\n", (cause == 0) ? 0 : -1);
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%d,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
		break;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND: {
		rtk_bt_le_ext_scan_res_ind_t *scan_res_ind = (rtk_bt_le_ext_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->addr), le_addr, sizeof(le_addr));
#if 0
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %d, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
#endif
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%d,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		break;
	}

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_pa_sync_ind_t *pa_sync_ind = (rtk_bt_le_pa_sync_ind_t *)param;
		BT_LOGA("[APP] PA sync state change: sync_id: %d, state = %d, cause: 0x%x\r\n",
				pa_sync_ind->sync_id, pa_sync_ind->state, pa_sync_ind->cause);
		if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZING_WAIT_SCANNING) {
			ret = rtk_bt_le_gap_set_ext_scan_param(&def_bis_receiver_ext_scan_param);
			if (RTK_BT_OK != ret) {
				BT_LOGE("[APP] BIS Receiver set default ext scan paramters failed! err: 0x%x\r\n", ret);
			} else {
				ret = rtk_bt_le_gap_start_ext_scan();
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Receiver start ext scan failed! err: 0x%x\r\n", ret);
				}
			}
		} else if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
			rtk_bt_le_pa_synced_info_t *p_info = &pa_sync_ind->info;
			rtk_bt_le_addr_to_str(&p_info->addr, le_addr, sizeof(le_addr));
			BT_LOGA("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid: %d, past_received: %d\r\n",
					le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			BT_AT_PRINT("+BLEGAP:pa_sync,%s,0x%x,%d,%d\r\n",
						le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);

			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_PA_CSYNC;
			g_le_iso_bis_receiver_status.sync_id = pa_sync_ind->sync_id;
			g_le_iso_bis_receiver_status.sync_handle = p_info->sync_handle;
		} else if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] PA SYNCHRONIZE TERMINATED\r\n");
			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_PA_TSYNC;
		}

		break;
	}

	case RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_pa_adv_report_ind_t *pa_report = (rtk_bt_le_pa_adv_report_ind_t *)param;
		BT_LOGA("[APP] PA sync ADV report: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
				pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
				pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		BT_AT_PRINT("+BLEGAP:pa_report,%d,0x%x,%d,%d,%d,0x%x,%d\r\n",
					pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
					pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		if (conn_ind->err) {
			BT_LOGE("[APP] Connection establish failed, remote device: %s\r\n",
					le_addr);
			break;
		}
		role = conn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Connected, conn_handle: %d, role: %s, remote device: %s\r\n",
				conn_ind->conn_handle, role, le_addr);
		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
				disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		if (RTK_BLE_ISO_ROLE_CIS_INITIATOR == app_demo_le_iso_role) {
			uint8_t found_cig_num = 0;
			uint8_t cig_index[RTK_BLE_CIG_MAX_NUM] = {0};
			/* check whether this conn_handle is exited */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				if (g_le_iso_cis_initiator_status.initiator_info[i].conn_handle == disconn_ind->conn_handle) {
					cig_index[found_cig_num++] = i;
				}
			}
			if (found_cig_num == 0) {
				BT_LOGA("[APP] This conn_handle 0x%x is not fill within CIG \r\n", disconn_ind->conn_handle);
				break;
			}
			for (uint8_t i = 0; i < found_cig_num; i ++) {
				osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
				memset((void *)&g_le_iso_cis_initiator_status.initiator_info[cig_index[i]], 0, sizeof(rtk_ble_cis_initiator_info_t));
				g_le_iso_cis_initiator_status.initiator_info[cig_index[i]].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_DISC;
				g_le_iso_cis_initiator_status.cig_num --;
				osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
			}
		} else if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
			uint8_t found_cig_num = 0;
			uint8_t cig_index[RTK_BLE_CIG_MAX_NUM] = {0};
			/* check whether this conn_handle is exited */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				if (g_le_iso_cis_acceptor_status.acceptor_info[i].conn_handle == disconn_ind->conn_handle) {
					cig_index[found_cig_num++] = i;
				}
			}
			if (found_cig_num == 0) {
				BT_LOGA("[APP] This conn_handle 0x%x is not fill within CIG \r\n", disconn_ind->conn_handle);
				break;
			}
			for (uint8_t i = 0; i < found_cig_num; i ++) {
				osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
				memset((void *)&g_le_iso_cis_acceptor_status.acceptor_info[cig_index[i]], 0, sizeof(rtk_ble_cis_acceptor_info_t));
				g_le_iso_cis_acceptor_status.acceptor_info[cig_index[i]].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_DISC;
				g_le_iso_cis_acceptor_status.cig_num --;
				osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
			}
			/* start ext adv */
			rtk_bt_le_gap_dev_state_t dev_state;
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
				BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(g_le_iso_cis_acceptor_status.adv_handle, 0, 0));
#else
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&def_cis_acceptor_adv_param));
#endif
			}
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *update_conn_ind = (rtk_bt_le_conn_update_ind_t *)param;
		if (update_conn_ind->err) {
			BT_LOGE("[APP] Update conn param failed, conn_handle: %d\r\n",
					update_conn_ind->conn_handle);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,-1\r\n", conn_update_ind->conn_handle);
			break;
		}
		BT_LOGA("[APP] The conn param is updated, conn_handle: %d, conn_interval: %d, "
				"conn_latency: %d, supervision_timeout: %d\r\n",
				update_conn_ind->conn_handle,
				update_conn_ind->conn_interval,
				update_conn_ind->conn_latency,
				update_conn_ind->supv_timeout);
		BT_AT_PRINT("+BLEGAP:conn_update,%d,0,0x%x,0x%x,0x%x\r\n",
					conn_update_ind->conn_handle,
					conn_update_ind->conn_interval,
					conn_update_ind->conn_latency,
					conn_update_ind->supv_timeout);
		break;
	}

	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: {
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_ind = (rtk_bt_le_remote_conn_update_req_ind_t *)param;
		BT_LOGA("[APP] Remote device request a change in conn param, conn_handle: %d, "
				"conn_interval_max: %d, conn_interval_min: %d, conn_latency: %d, "
				"timeout: %d.\r\n",
				rmt_ind->conn_handle,
				rmt_ind->conn_interval_max,
				rmt_ind->conn_interval_min,
				rmt_ind->conn_latency,
				rmt_ind->supv_timeout);
		if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
			return RTK_BT_EVT_CB_ACCEPT;
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *update_data_len_ind = (rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] The date len is updated, conn_handle: %d, "
				"max_tx_octets: %d, max_tx_time: %d, "
				"max_rx_octets: %d, max_rx_time: %d\r\n",
				update_data_len_ind->conn_handle,
				update_data_len_ind->max_tx_octets,
				update_data_len_ind->max_tx_time,
				update_data_len_ind->max_rx_octets,
				update_data_len_ind->max_rx_time);
		BT_AT_PRINT("+BLEGAP:conn_datalen,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					data_len_change->conn_handle,
					data_len_change->max_tx_octets,
					data_len_change->max_tx_time,
					data_len_change->max_rx_octets,
					data_len_change->max_rx_time);
		break;
	}

	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *update_phy_ind = (rtk_bt_le_phy_update_ind_t *)param;
		if (update_phy_ind->err) {
			BT_LOGE("[APP] Update PHY failed, conn_handle: %d\r\n", update_phy_ind->conn_handle);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
			break;
		}
		BT_LOGA("[APP] The PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
				update_phy_ind->conn_handle,
				update_phy_ind->tx_phy,
				update_phy_ind->rx_phy);
		BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->tx_phy,
					phy_update_ind->rx_phy);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind = (rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm for conn_handle: %d!\r\n", pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
			rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
			pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
			pair_cfm_param.confirm = 1;
			rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_disp_ind = (rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n", (int)key_disp_ind->passkey, key_disp_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *auth_passkey_input_ind = (rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey, conn_handle=0x%x\r\n", auth_passkey_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", auth_passkey_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind = (rtk_bt_le_auth_key_cfm_ind_t *)param;
		BT_LOGA("[APP] Auth passkey confirm: %d, conn_handle: %d. "  \
				"Please comfirm if the passkeys are equal!\r\n",
				(int)key_cfm_ind->passkey,
				key_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_cfm,%d,%d\r\n",
					(int)key_cfm_ind->conn_handle,
					(int)key_cfm_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *auth_oob_key_input_ind = (rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key! Please input the oob tk, conn_handle=0x%x\r\n", auth_oob_key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
			(rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT("+BLEGAP:sec,%d,%d\r\n",
					auth_cplt_ind->conn_handle,
					(auth_cplt_ind->err == 0) ? 0 : -1);
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
					auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_DUMPHEXA("[APP] long term key is 0x", auth_cplt_ind->dev_ltk, auth_cplt_ind->dev_ltk_length, true);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
			(rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d", bond_mdf_ind->op);
		BT_AT_PRINT("+BLEGAP:bond_modify,%d,%s,%s\r\n", bond_mdf_ind->op, le_addr, ident_addr);
		break;
	}

	default:
		BT_LOGE("%s, GAP RTK_BLE_EVT: %d unhandled\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

/* initiator cb */
static rtk_bt_evt_cb_ret_t app_bt_le_iso_cb(uint8_t evt_code, void *data, uint32_t data_len)
{
	(void) data_len;
	uint16_t ret = 0;
	switch (evt_code) {
	/* for cig event */
	case RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO: {
		rtk_bt_le_iso_cig_disconnect_info_t *param = (rtk_bt_le_iso_cig_disconnect_info_t *)data;
		BT_LOGA("[APP] rtk_bt_le_iso_cig_disconnect is indicated(conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x)\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id, param->cause);
		//If role is @ref RTK_BLE_ISO_ISOCH_ROLE_INITIATOR for specified CIS,
		//the handle and the associated data paths of that CIS shall remain valid when When CIS connection is disconnected
		//So we remove it here.
		if (RTK_BLE_ISO_ROLE_CIS_INITIATOR == app_demo_le_iso_role) {
			bool found = false;
			uint8_t cig_index = 0;
			/* check whether this conn_handle is exited */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				if ((g_le_iso_cis_initiator_status.initiator_info[i].conn_handle == param->conn_handle) && (param->cig_id == (i + 1))) {
					cig_index = i;
					found = true;
				}
			}
			if (!found) {
				BT_LOGA("[APP] This conn_handle 0x%x is not fill within CIG %s\r\n", param->conn_handle);
				break;
			}
			if (RTK_BLE_EXAMPLE_CIS_INITIATOR_PATH_SETUP == g_le_iso_cis_initiator_status.initiator_info[cig_index].status) {
				ret = rtk_bt_le_iso_cig_remove_path(param->cis_conn_handle, RTK_BLE_ISO_DATA_PATH_INPUT_FLAG);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] rtk_bt_le_iso_cig_remove_path for input fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				}
				ret = rtk_bt_le_iso_cig_remove_path(param->cis_conn_handle, RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] rtk_bt_le_iso_cig_remove_path for output fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				}
			}
			osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
			g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_info[param->cis_id - 1].active = false;
			g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_info[param->cis_id - 1].cis_id = 0;
			g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_info[param->cis_id - 1].cis_conn_handle = 0;
			g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_num --;
			if (g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_num == 0) {
				g_le_iso_cis_initiator_status.initiator_info[cig_index].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_CIG_DIS;
				/* stop send timer */
				if (g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer) {
					osif_timer_stop(&g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer);
				}
			}
			osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
		} else if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
			//If role is @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR for specified CIS,
			//the handle and the associated data path of that CIS will be auto deleted,
			//we don't need to repeat remove path by API rtk_bt_le_iso_cig_remove_path()
			bool found = false;
			uint8_t cig_index = 0;
			/* check whether this conn_handle is exited */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				if ((g_le_iso_cis_acceptor_status.acceptor_info[i].conn_handle == param->conn_handle) && (param->cig_id == (i + 1))) {
					cig_index = i;
					found = true;
				}
			}
			if (!found) {
				BT_LOGA("[APP] This conn_handle 0x%x is not fill within CIG %s\r\n", param->conn_handle);
				break;
			}
			osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
			g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_info[param->cis_id - 1].active = false;
			g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_info[param->cis_id - 1].cis_id = 0;
			g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_info[param->cis_id - 1].cis_conn_handle = 0;
			g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_num --;
			if (g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_num == 0) {
				g_le_iso_cis_acceptor_status.acceptor_info[cig_index].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_CIG_DIS;
				/* stop send timer */
				if (g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer) {
					osif_timer_stop(&g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer);
				}
			}
			osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO: {
		bool used_cig = false;
		rtk_bt_le_iso_cig_cis_established_info_t *param = (rtk_bt_le_iso_cig_cis_established_info_t *)data;
		if (param->cause != 0) {
			BT_LOGE("[APP] rtk_bt_le_iso_cig_cis_established fail!(cause 0x%x, conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x)\r\n", param->cause,
					param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id);
		} else {
			BT_LOGA("[APP] rtk_bt_le_iso_cig_cis_established indicate(cause 0x%x, conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cig_sync_delay 0x%x, cis_sync_delay 0x%x\r\n",
					param->cause, param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id,
					(unsigned int)param->cig_sync_delay, (unsigned int)param->cis_sync_delay);
			BT_LOGA("[APP] rtk_bt_le_iso_cig_cis_established: transport_latency_m_s 0x%x, transport_latency_s_m 0x%x, phy_m_s 0x%x, phy_s_m 0x%x, nse 0x%x\r\n",
					(unsigned int)param->transport_latency_m_s, (unsigned int)param->transport_latency_s_m, param->phy_m_s, param->phy_s_m, param->nse);
			BT_LOGA("[APP] rtk_bt_le_iso_cig_cis_established: bn_m_s 0x%x, bn_s_m 0x%x, ft_m_s 0x%x, ft_s_m 0x%x, max_pdu_m_s 0x%x, max_pdu_s_m 0x%x, iso_interval 0x%x\r\n",
					param->bn_m_s, param->bn_s_m, param->ft_m_s, param->ft_s_m, param->max_pdu_m_s, param->max_pdu_s_m, param->iso_interval);
			if (RTK_BLE_ISO_ROLE_CIS_INITIATOR == app_demo_le_iso_role) {
				/* check whether this cig has been assigned */
				if (g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].status != RTK_BLE_EXAMPLE_CIS_INITIATOR_ENABLE) {
					used_cig = true;
					/* check whether this cis has already been assigned */
					if (g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].cis_info[param->cis_id - 1].active) {
						BT_LOGA("[APP] cis id is already exited \r\n");
						break;
					}
				}
				osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
				g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].cis_info[param->cis_id - 1].active = true;
				g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].cis_info[param->cis_id - 1].cis_id = param->cis_id;
				g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].cis_info[param->cis_id - 1].cis_conn_handle = param->cis_conn_handle;
				g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].cis_num ++;
				if (!used_cig) {
					g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_CIG_EST;
					g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S;
					g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].conn_handle = param->conn_handle;
					g_le_iso_cis_initiator_status.cig_num ++;
				}
				osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
			} else if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
				/* check whether this cig has been assigned */
				if (g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].status != RTK_BLE_EXAMPLE_CIS_ACCEPTOR_ENABLE) {
					used_cig = true;
					/* check whether this cis has already been assigned */
					if (g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].cis_info[param->cis_id - 1].active) {
						BT_LOGA("[APP] cis id is already exited \r\n");
						break;
					}
				}
				osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
				g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].cis_info[param->cis_id - 1].active = true;
				g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].cis_info[param->cis_id - 1].cis_id = param->cis_id;
				g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].cis_info[param->cis_id - 1].cis_conn_handle = param->cis_conn_handle;
				g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].cis_num ++;
				if (!used_cig) {
					g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_CIG_EST;
					g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M;
					g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].conn_handle = param->conn_handle;
					g_le_iso_cis_acceptor_status.cig_num ++;
				}
				osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
			}
			app_le_iso_setup_path_param.iso_conn_handle = param->cis_conn_handle;
			app_le_iso_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_INPUT;
			ret = rtk_bt_le_iso_cig_setup_path(&app_le_iso_setup_path_param);
			if (RTK_BT_OK != ret) {
				BT_LOGE("[APP] rtk_bt_le_iso_cig_setup_path for input fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
			} else {
				BT_LOGA("[APP] rtk_bt_le_iso_cig_setup_path for input ok(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				app_le_iso_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT;
				ret = rtk_bt_le_iso_cig_setup_path(&app_le_iso_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] rtk_bt_le_iso_cig_setup_path for output fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				} else {
					BT_LOGA("[APP] rtk_bt_le_iso_cig_setup_path for output ok(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
					if (RTK_BLE_ISO_ROLE_CIS_INITIATOR == app_demo_le_iso_role) {
						g_le_iso_cis_initiator_status.initiator_info[param->cig_id - 1].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_PATH_SETUP;
					} else if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == app_demo_le_iso_role) {
						g_le_iso_cis_acceptor_status.acceptor_info[param->cig_id - 1].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_PATH_SETUP;
					}
				}
			}
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_ACCEPTOR_REQUEST_CIS_IND: {
		rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *param = (rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *)data;
		BT_LOGA("[APP] rtk_bt_le_iso_cig_cis_request indicate(conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cis_req_action 0x%x)\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id, param->cis_req_action);
		if (param->cis_req_action == RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_PENDING) {
			rtk_bt_le_iso_cig_acceptor_accept_cis(param->cis_conn_handle);//accept cis request
			//rtk_bt_le_iso_cig_acceptor_reject_cis(param->cis_conn_handle,HCI_SUCCESS);//if you want to reject cis request
		}
		break;
	}

	/* for big event */
	case RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_CREATE_CMPL_INFO: {
		rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t *param = (rtk_bt_le_iso_big_broadcaster_create_cmpl_info_t *)data;
		if (param->cause != 0) {
			BT_LOGE("[APP] rtk_bt_le_iso_big_broadcaster_create fail!(cause 0x%x,adv_handle 0x%x)\r\n", param->cause, param->adv_handle);
		} else {
			BT_LOGA("[APP] rtk_bt_le_iso_big_broadcaster_create is done(adv_handle 0x%x, big_handle 0x%x, big_sync_delay 0x%x, transport_latency_big 0x%x, phy 0x%x, nse 0x%x, bn 0x%x,pto 0x%x,irc 0x%x,max_pdu 0x%x,iso_interval 0x%x,num_bis 0x%x)\r\n",
					param->adv_handle, param->big_handle, (unsigned int)param->big_sync_delay, (unsigned int)param->transport_latency_big, param->phy,
					param->nse, param->bn, param->pto, param->irc, param->max_pdu, param->iso_interval, param->num_bis);
			g_le_iso_bis_broadcaster_status.bis_num = param->num_bis;
			for (uint8_t i = 0; i < param->num_bis; i++) {
				BT_LOGA("[APP] bis index %d, bis_conn_handle 0x%x\r\n", param->bis_conn_handle_info[i].bis_idx, param->bis_conn_handle_info[i].bis_conn_handle);
				g_le_iso_bis_broadcaster_status.bis_conn_handle[i] = param->bis_conn_handle_info[i].bis_conn_handle;
			}
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_SYNC_STATE_IND: {
		rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *param = (rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *)data;
		BT_LOGA("[APP] bis broadcaster sync state change: big_handle: 0x%x, adv_handle 0x%x, sync_state 0x%x, cause: 0x%x\r\n",
				param->big_handle, param->adv_handle, param->sync_state, param->cause);
		if (param->sync_state == RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_BROADCASTING) {
			BT_LOGA("[APP] bis broadcaster is isochronous broadcasting\r\n");
			g_le_iso_bis_broadcaster_status.status = RTK_BLE_EXAMPLE_BIS_BROADCASTER_CBIG;
			app_le_iso_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_INPUT;
			for (uint8_t i = 0; i < g_le_iso_bis_broadcaster_status.bis_num; i++) {
				app_le_iso_setup_path_param.iso_conn_handle = g_le_iso_bis_broadcaster_status.bis_conn_handle[i];
				ret = rtk_bt_le_iso_big_setup_path(&app_le_iso_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Boardcaster setup data path for bis_conn_handle 0x%x failed! err: 0x%x", app_le_iso_setup_path_param.iso_conn_handle, ret);
				}
			}
		} else if (param->sync_state == RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_IDLE) {
			BT_LOGA("[APP] bis broadcaster is idle\r\n");
			g_le_iso_bis_broadcaster_status.status = RTK_BLE_EXAMPLE_BIS_BROADCASTER_TBIG;
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_ESTABLISHED_INFO: {
		rtk_bt_le_iso_big_receiver_sync_established_info_t *param = (rtk_bt_le_iso_big_receiver_sync_established_info_t *)data;
		if (param->cause != 0) {
			BT_LOGE("[APP] rtk_bt_le_iso_big_receiver_create_sync fail!(cause 0x%x,sync_handle 0x%x)\r\n", param->cause, param->sync_handle);
		} else {
			BT_LOGA("[APP] rtk_bt_le_iso_big_receiver_create_sync is done(big_handle 0x%x, sync_id 0x%x, sync_handle 0x%x, transport_latency_big 0x%x, nse 0x%x, bn 0x%x,pto 0x%x,irc 0x%x,max_pdu 0x%x,iso_interval 0x%x,num_bis 0x%x)\r\n",
					param->big_handle, param->sync_id, param->sync_handle, (unsigned int)param->transport_latency_big, param->nse,
					param->bn, param->pto, param->irc, param->max_pdu, param->iso_interval, param->num_bis);
			g_le_iso_bis_receiver_status.bis_num = param->num_bis;
			for (uint8_t i = 0; i < param->num_bis; i++) {
				BT_LOGA("[APP] bis index %d, bis_conn_handle 0x%x\r\n", param->bis_conn_handle_info[i].bis_idx, param->bis_conn_handle_info[i].bis_conn_handle);
				g_le_iso_bis_receiver_status.bis_conn_handle[i] = param->bis_conn_handle_info[i].bis_conn_handle;
			}
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_STATE_IND: {
		rtk_bt_le_iso_big_receiver_sync_state_ind_t *param = (rtk_bt_le_iso_big_receiver_sync_state_ind_t *)data;
		BT_LOGA("[APP] bis receiver sync state change: big_handle: 0x%x, sync_state 0x%x,param->sync_id 0x%x, sync_handle 0x%x, cause: 0x%x\r\n",
				param->big_handle, param->sync_state, param->sync_id, param->sync_handle,  param->cause);
		if (param->sync_state == RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] bis receiver sychronized to a big\r\n");
			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_CSYNC;
			g_le_iso_bis_receiver_status.big_handle = param->big_handle;
			app_le_iso_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT;
			for (uint8_t i = 0; i < g_le_iso_bis_receiver_status.bis_num; i++) {
				app_le_iso_setup_path_param.iso_conn_handle = g_le_iso_bis_receiver_status.bis_conn_handle[i];
				ret = rtk_bt_le_iso_big_setup_path(&app_le_iso_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Receiver setup data path for bis_conn_handle 0x%x failed! err: 0x%x\r\n", app_le_iso_setup_path_param.iso_conn_handle, ret);
				}
			}
		} else if (param->sync_state == RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] bis receiver sync termiated\r\n");
			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_TSYNC;
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND: { //BT api shall not be called here
		rtk_bt_le_iso_direct_iso_data_ind_t *p_bt_direct_iso = (rtk_bt_le_iso_direct_iso_data_ind_t *)data;
		if (p_bt_direct_iso->iso_sdu_len) {
			BT_LOGA("[APP] RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND, iso_conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp 0x%x,iso_sdu_len 0x%x, p_buf %08x, buf_len %d, offset %d\r\n",
					p_bt_direct_iso->iso_conn_handle, p_bt_direct_iso->pkt_status_flag,
					p_bt_direct_iso->pkt_seq_num, p_bt_direct_iso->ts_flag,
					(unsigned int)p_bt_direct_iso->time_stamp, p_bt_direct_iso->iso_sdu_len, p_bt_direct_iso->p_buf, p_bt_direct_iso->buf_len, p_bt_direct_iso->offset);
		}
		break;
	}

	default: {
		BT_LOGE("[APP] default evt_code 0x%x\r\n", evt_code);
		break;
	}
	}
	return RTK_BT_EVT_CB_OK;
}

typedef struct {
	uint8_t event;
	rtk_bt_le_iso_data_send_info_t send_info_t;
} app_le_iso_demo_data_send_info_t;

static uint8_t iso_data[128] = {0};
static app_le_iso_demo_data_send_info_t app_iso_initiator_send_info = {0};
static app_le_iso_demo_data_send_info_t app_iso_acceptor_send_info = {0};
static app_le_iso_demo_data_send_info_t app_iso_broadcaster_send_info = {0};

static void app_le_iso_initiator_send_timer_handler(void *arg)
{
	bool found = false;
	rtk_ble_cis_initiator_info_t *p_initiator_info = NULL;

	/* foreach timer */
	osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
	for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
		if (g_le_iso_cis_initiator_status.initiator_info[i].iso_send_timer == arg) {
			found = true;
			p_initiator_info = &g_le_iso_cis_initiator_status.initiator_info[i];
		}
	}
	if (found) {
		app_iso_initiator_send_info.event = 0;
		app_iso_initiator_send_info.send_info_t.ts_flag = false;
		app_iso_initiator_send_info.send_info_t.time_stamp = 0;
		app_iso_initiator_send_info.send_info_t.pkt_seq_num++;
		app_iso_initiator_send_info.send_info_t.data_len = 128;
		app_iso_initiator_send_info.send_info_t.p_data = iso_data;
		/* foreach cis */
		for (uint8_t i = 0; i < RTK_BLE_CIS_MAX_NUM; i ++) {
			if (p_initiator_info->cis_info[i].active) {
				app_iso_initiator_send_info.send_info_t.iso_conn_handle = p_initiator_info->cis_info[i].cis_conn_handle;
				if (!osif_msg_send(g_le_iso_cis_initiator_status.iso_send_msg_q, &app_iso_initiator_send_info, 0)) {
					BT_LOGE("%s: osif_msg_send failed \r\n", __func__);
				}
			}
		}
	}
	osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
}

static void app_le_iso_initiator_send_task(void *ctx)
{
	(void)ctx;
	uint16_t ret = RTK_BT_FAIL;
	app_le_iso_demo_data_send_info_t app_send_info = {0};

	osif_sem_give(g_le_iso_cis_initiator_status.iso_send_task_sem);
	while (true) {
		if (true == osif_msg_recv(g_le_iso_cis_initiator_status.iso_send_msg_q, &app_send_info, BT_TIMEOUT_FOREVER)) {
			if (app_send_info.event == 0xFF) {
				goto exit;
			}
			ret = rtk_bt_le_iso_data_send(&app_send_info.send_info_t);
			if (ret) {
				BT_LOGE("%s: rtk_bt_le_iso_data_send failed ret:0x%x \r\n", __func__, ret);
			}
		}
	}

exit:
	BT_LOGD("[le iso initiator send task] exit\r\n");
	osif_sem_give(g_le_iso_cis_initiator_status.iso_send_task_sem);
	osif_task_delete(NULL);
}

static void app_le_iso_acceptor_send_timer_handler(void *arg)
{
	bool found = false;
	rtk_ble_cis_acceptor_info_t *p_acceptor_info = NULL;

	/* foreach timer */
	osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
	for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
		if (g_le_iso_cis_acceptor_status.acceptor_info[i].iso_send_timer == arg) {
			found = true;
			p_acceptor_info = &g_le_iso_cis_acceptor_status.acceptor_info[i];
		}
	}
	if (found) {
		app_iso_acceptor_send_info.event = 0;
		app_iso_acceptor_send_info.send_info_t.ts_flag = false;
		app_iso_acceptor_send_info.send_info_t.time_stamp = 0;
		app_iso_acceptor_send_info.send_info_t.pkt_seq_num++;
		app_iso_acceptor_send_info.send_info_t.data_len = 128;
		app_iso_acceptor_send_info.send_info_t.p_data = iso_data;
		/* for each cis */
		for (uint8_t i = 0; i < RTK_BLE_CIS_MAX_NUM; i ++) {
			if (p_acceptor_info->cis_info[i].active) {
				app_iso_acceptor_send_info.send_info_t.iso_conn_handle = p_acceptor_info->cis_info[i].cis_conn_handle;
				if (!osif_msg_send(g_le_iso_cis_acceptor_status.iso_send_msg_q, &app_iso_acceptor_send_info, 0)) {
					BT_LOGE("%s: osif_msg_send failed \r\n", __func__);
				}
			}
		}
	}
	osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
}

static void app_le_iso_acceptor_send_task(void *ctx)
{
	(void)ctx;
	uint16_t ret = RTK_BT_FAIL;
	app_le_iso_demo_data_send_info_t app_send_info = {0};

	osif_sem_give(g_le_iso_cis_acceptor_status.iso_send_task_sem);
	while (true) {
		if (true == osif_msg_recv(g_le_iso_cis_acceptor_status.iso_send_msg_q, &app_send_info, BT_TIMEOUT_FOREVER)) {
			if (app_send_info.event == 0xFF) {
				goto exit;
			}
			ret = rtk_bt_le_iso_data_send(&app_send_info.send_info_t);
			if (ret) {
				BT_LOGE("%s: rtk_bt_le_iso_data_send failed ret:0x%x \r\n", __func__, ret);
			}
		}
	}

exit:
	BT_LOGD("[le iso acceptor send task] exit\r\n");
	osif_sem_give(g_le_iso_cis_acceptor_status.iso_send_task_sem);
	osif_task_delete(NULL);
}

static void app_le_iso_broadcaster_send_timer_handler(void *arg)
{
	(void)arg;
	/* foreach timer */
	osif_mutex_take(g_le_iso_bis_broadcaster_status.mutex, BT_TIMEOUT_FOREVER);
	app_iso_broadcaster_send_info.event = 0;
	app_iso_broadcaster_send_info.send_info_t.ts_flag = false;
	app_iso_broadcaster_send_info.send_info_t.time_stamp = 0;
	app_iso_broadcaster_send_info.send_info_t.pkt_seq_num++;
	app_iso_broadcaster_send_info.send_info_t.data_len = 128;
	app_iso_broadcaster_send_info.send_info_t.p_data = iso_data;
	/* for each bis */
	for (uint8_t i = 0; i < g_le_iso_bis_broadcaster_status.bis_num; i ++) {
		app_iso_broadcaster_send_info.send_info_t.iso_conn_handle = g_le_iso_bis_broadcaster_status.bis_conn_handle[i];
		if (!osif_msg_send(g_le_iso_bis_broadcaster_status.iso_send_msg_q, &app_iso_broadcaster_send_info, 0)) {
			BT_LOGE("%s: osif_msg_send failed \r\n", __func__);
		}
	}
	osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
}

static void app_le_iso_broadcaster_send_task(void *ctx)
{
	(void)ctx;
	uint16_t ret = RTK_BT_FAIL;
	app_le_iso_demo_data_send_info_t app_send_info = {0};

	osif_sem_give(g_le_iso_bis_broadcaster_status.iso_send_task_sem);
	while (true) {
		if (true == osif_msg_recv(g_le_iso_bis_broadcaster_status.iso_send_msg_q, &app_send_info, BT_TIMEOUT_FOREVER)) {
			if (app_send_info.event == 0xFF) {
				goto exit;
			}
			ret = rtk_bt_le_iso_data_send(&app_send_info.send_info_t);
			if (ret) {
				BT_LOGE("%s: rtk_bt_le_iso_data_send failed ret:0x%x \r\n", __func__, ret);
			}
		}
	}

exit:
	BT_LOGD("[le iso broadcaster send task] exit\r\n");
	osif_sem_give(g_le_iso_bis_broadcaster_status.iso_send_task_sem);
	osif_task_delete(NULL);
}

void app_le_iso_demo_continuous_data_send_control(uint8_t le_iso_role, uint8_t cig_id, bool enable)
{
	uint8_t cig_index = cig_id - 1;

	BT_LOGA("[APP] %s cig id %d control %d\r\n", __func__, cig_id, enable);
	/* check timer state of the cig_id */
	if (!cig_id) {
		BT_LOGE("%s cig id 0x%x should not be 0! \r\n", __func__, cig_id);
		return;
	}
	switch (le_iso_role) {
	case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
		osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
		if (enable) {
			if (g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer) {
				BT_LOGA("%s cig id 0x%x iso send timer is already initialized \r\n", __func__, cig_id);
				goto initiator_exit;
			}
			if (false == osif_sem_create(&g_le_iso_cis_initiator_status.iso_send_task_sem, 0, 1)) {
				BT_LOGA("%s iso_send_task_sem create fail \r\n", __func__);
				goto initiator_exit;
			}
			if (false == osif_msg_queue_create(&g_le_iso_cis_initiator_status.iso_send_msg_q, LE_ISO_DEMO_SEND_DATA_MSG_QUEUE_SIZE,
											   sizeof(app_le_iso_demo_data_send_info_t))) {
				BT_LOGA("%s iso_send_msg_q create fail \r\n", __func__);
				goto initiator_exit;
			}
			if (false == osif_timer_create(&g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer,
										   "le_iso_send_timer",
										   cig_id,
										   g_le_iso_cis_initiator_status.initiator_info[cig_index].sdu_interval,
										   true,
										   app_le_iso_initiator_send_timer_handler)) {
				BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
				goto initiator_exit;
			}
			if (false == osif_task_create(&g_le_iso_cis_initiator_status.iso_send_task_hdl, "le_iso_send_task", app_le_iso_initiator_send_task, NULL,
										  LE_ISO_DEMO_SEND_DATA_TASK_STACK_SIZE, LE_ISO_DEMO_SEND_DATA_TASK_PRIORITY)) {
				BT_LOGA("%s iso_send_task create fail \r\n", __func__);
				goto initiator_exit;
			}
			/* Wait until task is running */
			if (false == osif_sem_take(g_le_iso_cis_initiator_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
				BT_LOGA("%s iso send task sema take fail \r\n", __func__);
				goto initiator_exit;
			}
			osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
			return;
		}
initiator_exit:
		if (g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer) {
			osif_timer_stop(&g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer);
			osif_timer_delete(&g_le_iso_cis_initiator_status.initiator_info[cig_index].iso_send_timer);
		}
		if (enable) {
			if (g_le_iso_cis_initiator_status.iso_send_task_hdl) {
				osif_task_delete(g_le_iso_cis_initiator_status.iso_send_task_hdl);
				g_le_iso_cis_initiator_status.iso_send_task_hdl = NULL;
			}
		} else {
			if (g_le_iso_cis_initiator_status.iso_send_task_sem) {
				app_le_iso_demo_data_send_info_t app_send_info = {0};
				app_send_info.event = 0xFF;
				if (!osif_msg_send(g_le_iso_cis_initiator_status.iso_send_msg_q, &app_send_info, 0)) {
					BT_LOGE("%s osif_msg_send fail \r\n", __func__);
					osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
					break;
				}
				if (false == osif_sem_take(g_le_iso_cis_initiator_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
					BT_LOGE("%s osif_sem_take fail \r\n", __func__);
					osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
					break;
				}
				g_le_iso_cis_initiator_status.iso_send_task_hdl = NULL;
			}
		}
		if (g_le_iso_cis_initiator_status.iso_send_task_sem) {
			osif_sem_delete(g_le_iso_cis_initiator_status.iso_send_task_sem);
			g_le_iso_cis_initiator_status.iso_send_task_sem = NULL;
		}
		if (g_le_iso_cis_initiator_status.iso_send_msg_q) {
			osif_msg_queue_delete(g_le_iso_cis_initiator_status.iso_send_msg_q);
			g_le_iso_cis_initiator_status.iso_send_msg_q = NULL;
		}
		osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
		break;
	}
	case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
		osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
		if (enable) {
			if (g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer) {
				BT_LOGA("%s cig id 0x%x iso send timer is already initialized \r\n", __func__, cig_id);
				goto acceptor_exit;
			}
			if (false == osif_sem_create(&g_le_iso_cis_acceptor_status.iso_send_task_sem, 0, 1)) {
				BT_LOGA("%s iso_send_task_sem create fail \r\n", __func__);
				goto acceptor_exit;
			}
			if (false == osif_msg_queue_create(&g_le_iso_cis_acceptor_status.iso_send_msg_q, LE_ISO_DEMO_SEND_DATA_MSG_QUEUE_SIZE,
											   sizeof(app_le_iso_demo_data_send_info_t))) {
				BT_LOGA("%s iso_send_msg_q create fail \r\n", __func__);
				goto acceptor_exit;
			}
			if (false == osif_timer_create(&g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer,
										   "le_iso_send_timer",
										   cig_id,
										   g_le_iso_cis_acceptor_status.acceptor_info[cig_index].sdu_interval,
										   true,
										   app_le_iso_acceptor_send_timer_handler)) {
				BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
				goto acceptor_exit;
			}
			if (false == osif_task_create(&g_le_iso_cis_acceptor_status.iso_send_task_hdl, "le_iso_send_task", app_le_iso_acceptor_send_task, NULL,
										  LE_ISO_DEMO_SEND_DATA_TASK_STACK_SIZE, LE_ISO_DEMO_SEND_DATA_TASK_PRIORITY)) {
				BT_LOGA("%s iso_send_task create fail \r\n", __func__);
				goto acceptor_exit;
			}
			/* Wait until task is running */
			if (false == osif_sem_take(g_le_iso_cis_acceptor_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
				BT_LOGA("%s iso send task sema take fail \r\n", __func__);
				goto acceptor_exit;
			}
			osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
			return;
		}
acceptor_exit:
		if (g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer) {
			osif_timer_stop(&g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer);
			osif_timer_delete(&g_le_iso_cis_acceptor_status.acceptor_info[cig_index].iso_send_timer);
		}
		if (enable) {
			if (g_le_iso_cis_acceptor_status.iso_send_task_hdl) {
				osif_task_delete(g_le_iso_cis_acceptor_status.iso_send_task_hdl);
				g_le_iso_cis_acceptor_status.iso_send_task_hdl = NULL;
			}
		} else {
			if (g_le_iso_cis_acceptor_status.iso_send_task_sem) {
				app_le_iso_demo_data_send_info_t app_send_info = {0};
				app_send_info.event = 0xFF;
				if (!osif_msg_send(g_le_iso_cis_acceptor_status.iso_send_msg_q, &app_send_info, 0)) {
					BT_LOGE("%s osif_msg_send fail \r\n", __func__);
					osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
					break;
				}
				if (false == osif_sem_take(g_le_iso_cis_acceptor_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
					BT_LOGE("%s osif_sem_take fail \r\n", __func__);
					osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
					break;
				}
				g_le_iso_cis_acceptor_status.iso_send_task_hdl = NULL;
			}
		}
		if (g_le_iso_cis_acceptor_status.iso_send_task_sem) {
			osif_sem_delete(g_le_iso_cis_acceptor_status.iso_send_task_sem);
			g_le_iso_cis_acceptor_status.iso_send_task_sem = NULL;
		}
		if (g_le_iso_cis_acceptor_status.iso_send_msg_q) {
			osif_msg_queue_delete(g_le_iso_cis_acceptor_status.iso_send_msg_q);
			g_le_iso_cis_acceptor_status.iso_send_msg_q = NULL;
		}
		osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
		break;
	}
	case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
		osif_mutex_take(g_le_iso_bis_broadcaster_status.mutex, BT_TIMEOUT_FOREVER);
		if (enable) {
			if (g_le_iso_bis_broadcaster_status.iso_send_timer) {
				BT_LOGA("%s cig id 0x%x iso send timer is already initialized \r\n", __func__, cig_id);
				goto broadcaster_exit;
			}
			if (false == osif_sem_create(&g_le_iso_bis_broadcaster_status.iso_send_task_sem, 0, 1)) {
				BT_LOGA("%s iso_send_task_sem create fail \r\n", __func__);
				goto broadcaster_exit;
			}
			if (false == osif_msg_queue_create(&g_le_iso_bis_broadcaster_status.iso_send_msg_q, LE_ISO_DEMO_SEND_DATA_MSG_QUEUE_SIZE,
											   sizeof(app_le_iso_demo_data_send_info_t))) {
				BT_LOGA("%s iso_send_msg_q create fail \r\n", __func__);
				goto broadcaster_exit;
			}
			if (false == osif_timer_create(&g_le_iso_bis_broadcaster_status.iso_send_timer,
										   "le_iso_send_timer",
										   cig_id,
										   g_le_iso_bis_broadcaster_status.sdu_interval,
										   true,
										   app_le_iso_broadcaster_send_timer_handler)) {
				BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
				goto broadcaster_exit;
			}
			if (false == osif_task_create(&g_le_iso_bis_broadcaster_status.iso_send_task_hdl, "le_iso_send_task", app_le_iso_broadcaster_send_task, NULL,
										  LE_ISO_DEMO_SEND_DATA_TASK_STACK_SIZE, LE_ISO_DEMO_SEND_DATA_TASK_PRIORITY)) {
				BT_LOGA("%s iso_send_task create fail \r\n", __func__);
				goto broadcaster_exit;
			}
			/* Wait until task is running */
			if (false == osif_sem_take(g_le_iso_bis_broadcaster_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
				BT_LOGA("%s iso send task sema take fail \r\n", __func__);
				goto broadcaster_exit;
			}
			osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
			return;
		}
broadcaster_exit:
		if (g_le_iso_bis_broadcaster_status.iso_send_timer) {
			osif_timer_stop(&g_le_iso_bis_broadcaster_status.iso_send_timer);
			osif_timer_delete(&g_le_iso_bis_broadcaster_status.iso_send_timer);
		}
		if (enable) {
			if (g_le_iso_bis_broadcaster_status.iso_send_task_hdl) {
				osif_task_delete(g_le_iso_bis_broadcaster_status.iso_send_task_hdl);
				g_le_iso_bis_broadcaster_status.iso_send_task_hdl = NULL;
			}
		} else {
			if (g_le_iso_bis_broadcaster_status.iso_send_task_sem) {
				app_le_iso_demo_data_send_info_t app_send_info = {0};
				app_send_info.event = 0xFF;
				if (!osif_msg_send(g_le_iso_bis_broadcaster_status.iso_send_msg_q, &app_send_info, 0)) {
					BT_LOGE("%s osif_msg_send fail \r\n", __func__);
					osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
					break;
				}
				if (false == osif_sem_take(g_le_iso_bis_broadcaster_status.iso_send_task_sem, BT_TIMEOUT_FOREVER)) {
					BT_LOGE("%s osif_sem_take fail \r\n", __func__);
					osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
					break;
				}
				g_le_iso_bis_broadcaster_status.iso_send_task_hdl = NULL;
			}
		}
		if (g_le_iso_bis_broadcaster_status.iso_send_task_sem) {
			osif_sem_delete(g_le_iso_bis_broadcaster_status.iso_send_task_sem);
			g_le_iso_bis_broadcaster_status.iso_send_task_sem = NULL;
		}
		if (g_le_iso_bis_broadcaster_status.iso_send_msg_q) {
			osif_msg_queue_delete(g_le_iso_bis_broadcaster_status.iso_send_msg_q);
			g_le_iso_bis_broadcaster_status.iso_send_msg_q = NULL;
		}
		osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
		break;
	}
	default:
		BT_LOGE("%s invalid le iso role 0x%x \r\n", __func__, le_iso_role);
		break;
	}
}

uint16_t app_le_iso_demo_data_send_start(uint8_t op)
{
	bool active = false;

	if (!le_iso_demo_init_flag) {
		BT_LOGE("%s LE ISO demo has not been initialized ! \r\n", __func__);
		return 1;
	}
	switch (app_demo_le_iso_role) {
	case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
		osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
		for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
			if (g_le_iso_cis_initiator_status.initiator_info[i].iso_send_timer) {
				active = true;
				if (op) {
					osif_timer_start(&g_le_iso_cis_initiator_status.initiator_info[i].iso_send_timer);
				} else {
					osif_timer_stop(&g_le_iso_cis_initiator_status.initiator_info[i].iso_send_timer);
				}
			}
		}
		osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
		break;
	}
	case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
		osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
		for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
			if (g_le_iso_cis_acceptor_status.acceptor_info[i].iso_send_timer) {
				active = true;
				if (op) {
					osif_timer_start(&g_le_iso_cis_acceptor_status.acceptor_info[i].iso_send_timer);
				} else {
					osif_timer_stop(&g_le_iso_cis_acceptor_status.acceptor_info[i].iso_send_timer);
				}
			}
		}
		osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
		break;
	}
	case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
		osif_mutex_take(g_le_iso_bis_broadcaster_status.mutex, BT_TIMEOUT_FOREVER);
		if (g_le_iso_bis_broadcaster_status.iso_send_timer) {
			active = true;
			if (op) {
				osif_timer_start(&g_le_iso_bis_broadcaster_status.iso_send_timer);
			} else {
				osif_timer_stop(&g_le_iso_bis_broadcaster_status.iso_send_timer);
			}
		}
		osif_mutex_give(g_le_iso_bis_broadcaster_status.mutex);
		break;
	}
	default:
		BT_LOGE("%s invalid le iso role 0x%x \r\n", __func__, app_demo_le_iso_role);
		break;
	}
	if (!active) {
		BT_LOGE("%s no timer is initialized \r\n", __func__);
		return 1;
	}

	return 0;
}

static uint16_t rtk_ble_cig_initiator_init(uint8_t cig_max_num, uint8_t cis_max_num)
{
	rtk_bt_le_iso_cig_initiator_set_cig_param_t set_cig_param = {0};
	rtk_bt_le_iso_cig_initiator_set_cis_param_t set_cis_param = {0};

	/* register iso event callback */
	BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
	/* cig param */
	set_cig_param.cig_id = 1;
	set_cig_param.sdu_interval_m_s = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US;//10000;
	set_cig_param.sdu_interval_s_m = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M_US;//10000;
	set_cig_param.sca = 0;
	set_cig_param.packing = 0;
	set_cig_param.framing = 0;
	set_cig_param.latency_m_s = RTK_BLE_ISO_DEFAULT_LATENCY_M_S;//10,
	set_cig_param.latency_s_m = RTK_BLE_ISO_DEFAULT_LATENCY_S_M; //10
	/* cis param */
	set_cis_param.cis_id = 1;
	set_cis_param.max_sdu_m_s = RTK_BLE_ISO_MAX_SDU_M_S;//128;
	set_cis_param.max_sdu_s_m = RTK_BLE_ISO_MAX_SDU_S_M;//128;
	set_cis_param.phy_m_s = RTK_BLE_ISO_DEFAULT_PHY_2M;
	set_cis_param.phy_s_m = RTK_BLE_ISO_DEFAULT_PHY_2M;
	set_cis_param.rtn_m_s = RTK_BLE_ISO_DEFAULT_RTN_M_S;//2;
	set_cis_param.rtn_s_m = RTK_BLE_ISO_DEFAULT_RTN_S_M;//2;

	for (uint8_t i = 0; i < cig_max_num; i ++) {
		set_cig_param.cig_id = i + 1;
		/* register cig initiator callback for cig */
		BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_register_callback(set_cig_param.cig_id));
		BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_set_cig_param(&set_cig_param));//use the default value if not set
		for (uint8_t j = 0; j < cis_max_num / cig_max_num; j ++) {
			BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_add_cis(set_cig_param.cig_id, (j + 1) + (i * 2)));
			set_cis_param.cis_id = (j + 1) + (i * 2);
			BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_set_cis_param(&set_cis_param));
		}
		BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_start_setting(set_cig_param.cig_id));
		g_le_iso_cis_initiator_status.initiator_info[i].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_ENABLE;
		g_le_iso_cis_initiator_status.initiator_info[i].sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S;
	}

	return RTK_BT_OK;
}

static uint16_t rtk_ble_cig_initiator_deinit(uint8_t cig_max_num, uint8_t cis_max_num)
{
	uint8_t i = 0;
	uint16_t cis_conn_handle = 0;
	rtk_bt_le_iso_cis_channel_info_t isoch_info = {0};

	for (i = 0; i < cis_max_num; i++) {
		uint8_t cis_id = i + 1;
		//get cis_conn_handle
		rtk_bt_le_iso_cig_initiator_get_cis_conn_handle(cis_id, &cis_conn_handle);
		//get isoch info
		BT_APP_PROCESS(rtk_bt_le_iso_cig_get_isoch_info(cis_conn_handle, &isoch_info));
		//disconnect cis first when state is not idle
		if (isoch_info.state != RTK_BLE_ISO_ISOCH_STATE_IDLE) {
			BT_LOGA("[APP] %s isoch_info.state=0x%x\r\n", __func__, isoch_info.state);
			BT_APP_PROCESS(rtk_bt_le_iso_cig_disconnect(cis_conn_handle));
		}
	}
	/* deinitilize cig part */
	for (uint8_t i = 0; i < cig_max_num; i ++) {
		BT_APP_PROCESS(rtk_bt_le_iso_cig_initiator_remove_cig(i + 1));
	}

	return RTK_BT_OK;
}

static uint16_t rtk_ble_cig_acceptor_init(uint8_t cig_max_num)
{
	/* register iso event callback */
	BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
	/* register cig acceptor callback */
	BT_APP_PROCESS(rtk_bt_le_iso_cig_acceptor_register_callback());
	BT_APP_PROCESS(rtk_bt_le_iso_cig_acceptor_config_cis_req_action(RTK_BLE_ISO_ACCEPTOR_CIS_REQ_ACTION_ACCEPT));
	for (uint8_t i = 0; i < cig_max_num; i ++) {
		g_le_iso_cis_acceptor_status.acceptor_info[i].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_ENABLE;
		g_le_iso_cis_acceptor_status.acceptor_info[i].sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M;
	}

	return RTK_BT_OK;
}

int bt_le_iso_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (le_iso_demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		switch (role) {
		/* cis initiator */
		case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			/* intialize cis part */
			BT_APP_PROCESS(rtk_ble_cig_initiator_init(RTK_BLE_CIG_MAX_NUM, RTK_BLE_CIS_MAX_NUM));
			osif_mutex_create(&g_le_iso_cis_initiator_status.mutex);
			memset((void *)&app_iso_initiator_send_info, 0, sizeof(rtk_bt_le_iso_data_send_info_t));
			/* init iso data send timer */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				app_le_iso_demo_continuous_data_send_control(RTK_BLE_ISO_ROLE_CIS_INITIATOR, i + 1, true);
			}
			le_iso_demo_init_flag = true;
			break;
		}

		/* cis acceptor */
		case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			char name[30] = {0};
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			memcpy(name, (const char *)RTK_BT_DEV_NAME, strlen((const char *)RTK_BT_DEV_NAME));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT));

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&def_cis_acceptor_ext_adv_param, &g_le_iso_cis_acceptor_status.adv_handle));
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(g_le_iso_cis_acceptor_status.adv_handle, (uint8_t *)def_cis_acceptor_ext_adv_data,
														  sizeof(def_cis_acceptor_ext_adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(g_le_iso_cis_acceptor_status.adv_handle, 0, 0));
#else
			BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(def_cis_acceptor_adv_data, sizeof(def_cis_acceptor_adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(def_cis_acceptor_scan_rsp_data, sizeof(def_cis_acceptor_scan_rsp_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&def_cis_acceptor_adv_param));
#endif
			/* intialize cis part */
			BT_APP_PROCESS(rtk_ble_cig_acceptor_init(RTK_BLE_CIG_MAX_NUM));
			osif_mutex_create(&g_le_iso_cis_acceptor_status.mutex);
			memset((void *)&app_iso_acceptor_send_info, 0, sizeof(rtk_bt_le_iso_data_send_info_t));
			/* init iso data send timer */
			for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				app_le_iso_demo_continuous_data_send_control(RTK_BLE_ISO_ROLE_CIS_ACCEPTOR, i + 1, true);
			}
			le_iso_demo_init_flag = true;
			break;
		}

		/* bis broadcaster */
		case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			rtk_bt_le_pa_param_t pa_param = {0xFF, RTK_BLE_ISO_BIS_BROADCASTER_PA_INTERVAL_MIN, RTK_BLE_ISO_BIS_BROADCASTER_PA_INTERVAL_MAX,
											 RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER, (uint8_t *)def_bis_broadcaster_pa_data, sizeof(def_bis_broadcaster_pa_data)
											};
			def_bis_broadcaster_param.num_bis = g_deflault_bis_num;
			//1. register iso event callback
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			//2. intialize big
			BT_APP_PROCESS(rtk_bt_le_iso_big_broadcaster_init(g_deflault_big_num, g_deflault_bis_num));
			//3. create ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&def_bis_broadcaster_ext_adv_param, &g_le_iso_bis_broadcaster_status.adv_handle));
			//4. set ext adv data
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(g_le_iso_bis_broadcaster_status.adv_handle, (uint8_t *)def_bis_broadcaster_ext_adv_data,
														  sizeof(def_bis_broadcaster_ext_adv_data)));
			//5. set ext scan rsp data
			//BT_APP_PROCESS(rtk_bt_le_gap_set_ext_scan_rsp_data(g_le_iso_bis_broadcaster_status.adv_handle,(uint8_t *)def_bis_broadcaster_ext_scan_data,sizeof(def_bis_broadcaster_ext_scan_data)));
			//6. start ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(g_le_iso_bis_broadcaster_status.adv_handle, 0, 0));
			//7. start pa
			pa_param.adv_handle = g_le_iso_bis_broadcaster_status.adv_handle;
			BT_APP_PROCESS(rtk_bt_le_gap_start_pa(&pa_param));
			osif_mutex_create(&g_le_iso_bis_broadcaster_status.mutex);
			g_le_iso_bis_broadcaster_status.status = RTK_BLE_EXAMPLE_BIS_BROADCASTER_ENABLE;
			g_le_iso_bis_broadcaster_status.sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S;
			memset((void *)&app_iso_broadcaster_send_info, 0, sizeof(rtk_bt_le_iso_data_send_info_t));
			/* init iso data send timer */
			app_le_iso_demo_continuous_data_send_control(RTK_BLE_ISO_ROLE_BIS_BROADCASTER, 1, true);
			le_iso_demo_init_flag = true;
			//9. create big
			BT_APP_PROCESS(rtk_bt_le_iso_big_broadcaster_create(g_le_iso_bis_broadcaster_status.adv_handle, &def_bis_broadcaster_param,
																&g_le_iso_bis_broadcaster_status.big_handle));
			break;
		}

		/* bis receiver */
		case RTK_BLE_ISO_ROLE_BIS_RECEIVER: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			//set GAP configuration
			bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
			bt_app_conf.mtu_size = 180;
			bt_app_conf.master_init_mtu_req = true;
			bt_app_conf.prefer_all_phy = 0;
			bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
			bt_app_conf.max_tx_octets = 0x40;
			bt_app_conf.max_tx_time = 0x200;
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* 1. Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			/* 2. register iso event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			/* 3. intialize big_receiver*/
			BT_APP_PROCESS(rtk_bt_le_iso_big_receiver_init(g_deflault_big_num, g_deflault_bis_num));
			osif_mutex_create(&g_le_iso_bis_receiver_status.mutex);
			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_ENABLE;
			le_iso_demo_init_flag = true;
			break;
		}

		default:
			BT_LOGD("[APP] unknown role 0x%x\r\n", role);
			break;
		}
		app_demo_le_iso_role = role;
	} else if (0 == enable) {
		if (!le_iso_demo_init_flag) {
			BT_LOGE("%s No need to deinit! \r\n", __func__);
			return -1;
		}
		switch (role) {
		/* cis initiator */
		case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
			le_iso_demo_init_flag = false;
			/* deintialize cis part */
			BT_APP_PROCESS(rtk_ble_cig_initiator_deinit(RTK_BLE_CIG_MAX_NUM, RTK_BLE_CIS_MAX_NUM));
			for (uint8_t i; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				/* deinit iso send timer */
				app_le_iso_demo_continuous_data_send_control(app_demo_le_iso_role, i + 1, false);
				if (g_le_iso_cis_initiator_status.initiator_info[i].status != RTK_BLE_EXAMPLE_CIS_INITIATOR_ENABLE &&
					g_le_iso_cis_initiator_status.initiator_info[i].status != RTK_BLE_EXAMPLE_CIS_INITIATOR_DISC) {
					BT_APP_PROCESS(rtk_bt_le_gap_disconnect(g_le_iso_cis_initiator_status.initiator_info[i].conn_handle));
					osif_delay(5);
				}
				g_le_iso_cis_initiator_status.initiator_info[i].status = RTK_BLE_EXAMPLE_CIS_INITIATOR_DISABLE;
			}
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			osif_mutex_delete(g_le_iso_cis_initiator_status.mutex);
			break;
		}

		/* cis acceptor */
		case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
			le_iso_demo_init_flag = false;
			/* deintialize cis part */
			for (uint8_t i; i < RTK_BLE_CIG_MAX_NUM; i ++) {
				/* deinit iso send timer */
				app_le_iso_demo_continuous_data_send_control(app_demo_le_iso_role, i + 1, false);
				if (g_le_iso_cis_acceptor_status.acceptor_info[i].status != RTK_BLE_EXAMPLE_CIS_ACCEPTOR_ENABLE &&
					g_le_iso_cis_acceptor_status.acceptor_info[i].status != RTK_BLE_EXAMPLE_CIS_ACCEPTOR_DISC) {
					BT_APP_PROCESS(rtk_bt_le_gap_disconnect(g_le_iso_cis_acceptor_status.acceptor_info[i].conn_handle));
					osif_delay(5);
				}
				g_le_iso_cis_acceptor_status.acceptor_info[i].status = RTK_BLE_EXAMPLE_CIS_ACCEPTOR_DISABLE;
			}
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
			//1. stop ea
			BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(g_le_iso_cis_acceptor_status.adv_handle));
#else
			//1. stop adv
			BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
#endif
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			osif_mutex_delete(g_le_iso_cis_acceptor_status.mutex);
			break;
		}

		case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
			le_iso_demo_init_flag = false;
			/* deinit iso send timer */
			app_le_iso_demo_continuous_data_send_control(app_demo_le_iso_role, 1, false);
			if (g_le_iso_bis_broadcaster_status.status == RTK_BLE_EXAMPLE_BIS_BROADCASTER_CBIG) {
				//1. remove big data path when big create
				for (uint8_t i = 0; i < g_le_iso_bis_broadcaster_status.bis_num; i++) {
					BT_APP_PROCESS(rtk_bt_le_iso_big_remove_path(g_le_iso_bis_broadcaster_status.bis_conn_handle[i], RTK_BLE_ISO_DATA_PATH_INPUT_FLAG));
				}
				//2. terminate big when big create
				BT_APP_PROCESS(rtk_bt_le_iso_big_broadcaster_terminate(g_le_iso_bis_broadcaster_status.big_handle));
			}
			//3. stop pa
			BT_APP_PROCESS(rtk_bt_le_gap_stop_pa(g_le_iso_bis_broadcaster_status.adv_handle));
			//4. stop ea
			BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(g_le_iso_bis_broadcaster_status.adv_handle));
			g_le_iso_bis_broadcaster_status.status = RTK_BLE_EXAMPLE_BIS_BROADCASTER_DISABLE;
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			osif_mutex_delete(g_le_iso_bis_broadcaster_status.mutex);
			break;
		}

		case RTK_BLE_ISO_ROLE_BIS_RECEIVER: {
			le_iso_demo_init_flag = false;
			if (g_le_iso_bis_receiver_status.status == RTK_BLE_EXAMPLE_BIS_RECEIVER_CSYNC) {
				//1. remove big data path when receiver sync
				for (uint8_t i = 0; i < g_le_iso_bis_receiver_status.bis_num; i++) {
					BT_APP_PROCESS(rtk_bt_le_iso_big_remove_path(g_le_iso_bis_receiver_status.bis_conn_handle[i], RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG));
				}
				//2. terminate sync when receiver sync
				BT_APP_PROCESS(rtk_bt_le_iso_big_receiver_terminate_sync(g_le_iso_bis_receiver_status.big_handle));
			}
			if (g_le_iso_bis_receiver_status.status == RTK_BLE_EXAMPLE_BIS_RECEIVER_PA_CSYNC ||
				g_le_iso_bis_receiver_status.status == RTK_BLE_EXAMPLE_BIS_RECEIVER_CSYNC) {
				//3. terminate pa sync
				BT_APP_PROCESS(rtk_bt_le_gap_pa_sync_terminate(g_le_iso_bis_receiver_status.sync_id));
				//4. stop ext scan
				BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_scan());
			}
			g_le_iso_bis_receiver_status.status = RTK_BLE_EXAMPLE_BIS_RECEIVER_DISABLE;
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			osif_mutex_delete(g_le_iso_bis_receiver_status.mutex);
			break;
		}

		default:
			BT_LOGD("[APP] unknown role 0x%x\r\n", role);
			break;
		}
		app_demo_le_iso_role = RTK_BLE_ISO_ROLE_UNKNOWN;
	}
	return 0;
}
#endif