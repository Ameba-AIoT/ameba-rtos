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

#include "rtk_bt_le_iso.h"
#include <rtk_bt_le_iso_def.h>

#define APP_LE_ISO_CIS_INITIATOR_DEVICE_NAME "Ameba ISO Initiator"
#define APP_LE_ISO_CIS_ACCEPTOR_DEVICE_NAME "Ameba ISO Acceptor"
#define APP_LE_ISO_BIS_BROADCASTER_DEVICE_NAME "Ameba ISO Broadcaster"
#define APP_LE_ISO_BIS_RECEIVER_DEVICE_NAME "Ameba ISO Receiver"
#define APP_LE_ISO_BIS_BROADCASTER_PA_INTERVAL_MIN (80)
#define APP_LE_ISO_BIS_BROADCASTER_PA_INTERVAL_MAX (80)
#define APP_LE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MIN (48)
#define APP_LE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MAX (48)
#define APP_LE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MIN (48)
#define APP_LE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MAX (48)

bool demo_init_flag = false;
static uint8_t iso_demo_role = RTK_BLE_ISO_ROLE_UNKNOWN;
static uint8_t bt_le_iso_demo_bis_broadcaster_ext_adv_handle = 0xFF;
/* init le iso app config */
static rtk_bt_le_iso_app_conf_t bt_le_iso_demo_app_conf = {
	.device_name = {0},
	.device_name_len = 0,
	.iso_role = RTK_BLE_ISO_ROLE_UNKNOWN,
	.cig_num = RTK_BLE_CIG_MAX_NUM,
	.cis_num = RTK_BLE_CIS_MAX_NUM,
	.big_num = RTK_BLE_BIG_MAX_NUM,
	.bis_num = RTK_BLE_BIS_MAX_NUM,
};

static uint8_t bt_le_iso_demo_broadcaster_pa_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x14,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'I', 'S', '_', 'B', 'O', 'A', 'R', 'D', 'C', 'A', 'S', 'T', 'E', 'R',
};

static rtk_bt_le_pa_param_t bt_le_iso_demo_pa_param = {
	.adv_handle = 0xFF,
	.pa_interval_min = APP_LE_ISO_BIS_BROADCASTER_PA_INTERVAL_MIN,
	.pa_interval_max = APP_LE_ISO_BIS_BROADCASTER_PA_INTERVAL_MAX,
	.pa_prop = RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER,
	.pa_data = (uint8_t *)bt_le_iso_demo_broadcaster_pa_data,
	.pa_len = sizeof(bt_le_iso_demo_broadcaster_pa_data)
};

static rtk_bt_le_ext_adv_param_t bt_le_iso_demo_bis_broadcaster_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
	.primary_adv_interval_min = APP_LE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = APP_LE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MAX,
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

static uint8_t bt_le_iso_demo_bis_broadcaster_ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x14,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'I', 'S', '_', 'B', 'O', 'A', 'R', 'D', 'C', 'A', 'S', 'T', 'E', 'R',
};

/* for big broadcaster */
static rtk_bt_le_iso_big_broadcaster_param_t bt_le_iso_demo_bis_broadcaster_param = {
	.num_bis = RTK_BLE_BIS_MAX_NUM, /**< Total number of BISes in the BIG. */
	.sdu_interval = RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US, /**< The interval, in microseconds, of periodic SDUs. Range: 0x0000FF to 0x0FFFFF */
	.max_sdu = RTK_BLE_ISO_MAX_SDU_M_S, /**< Maximum size of an SDU, in octets. */
	.max_transport_latency = RTK_BLE_ISO_DEFAULT_LATENCY_M_S, /**< Maximum transport latency, in milliseconds.Range: 0x0005 to 0x0FA0 */
	.rtn = RTK_BLE_ISO_DEFAULT_RTN_M_S, /**< The number of times that every BIS Data PDU should be retransmitted.Range: 0x00 to 0x1E */
	.phy = RTK_BLE_ISO_DEFAULT_PHY_2M, /**< A bit field that indicates the PHY used for transmission of PDUs of BISes in the BIG.
                                            bit 0: The transmitter PHY of packets is LE 1M.
                                            bit 1: The transmitter PHY of packets is LE 2M.
                                            bit 2: The transmitter PHY of packets is LE Coded. */
	.packing = 1, /**< Indicate the preferred method of arranging subevents of multiple BISes.
                       0x00: Sequential
                       0x01: Interleaved */
	.framing = 0, /**< Indicate the format for sending BIS Data PDUs.
                       0x00: Unframed
                       0x01: Framed */
	.encryption = 0, /**< Identify the encryption mode of the BISes.
                          0x00: Unencrypted
                          0x01: Encrypted */
	.broadcast_code = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E}
	/* Default value of TSPX_broadcast_code in PTS 8.0.0 is 8ED03323D1205E2D58191BF6285C3182 */
};

static uint8_t bt_le_iso_demo_big_handle = 0xFF;

/* for accpetor */
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint8_t bt_le_iso_demo_cis_acceptor_ext_adv_data[] = {
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

static rtk_bt_le_ext_adv_param_t bt_le_iso_demo_cis_acceptor_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = APP_LE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MIN,
	.primary_adv_interval_max = APP_LE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MAX,
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

uint8_t bt_le_iso_demo_cis_acceptor_ext_adv_handle = 0xFF;
#else
static uint8_t bt_le_iso_demo_cis_acceptor_adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x11,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'C', 'I', 'S', '_', 'A', 'C', 'C', 'E', 'P', 'T', 'O', 'R',
};

static uint8_t bt_le_iso_demo_cis_acceptor_scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

static rtk_bt_le_adv_param_t bt_le_iso_demo_cis_acceptor_adv_param = {
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

/* for receiver */
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static rtk_bt_le_ext_scan_param_t bt_le_iso_demo_bis_receiver_ext_scan_param = {
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
#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
static uint8_t bt_le_iso_demo_bis_receiver_sync_id = 0xFF;
static uint8_t bt_le_iso_demo_bis_receiver_scan_flag = 0;
#endif
/* done for receiver */

rtk_bt_le_iso_setup_path_param_t le_iso_demo_setup_path_param = {
	0,                                      /*cis_conn_handle */
	0,                                      /*data_path_direction */
	0,                                      /*data_path_id 0x00 is for hci, others for vendor */
	{0x03, 0, 0, 0, 0},                     /*codec_id: Octet 0 for Coding format 0x03 for Transparent Mode
                                                        Octets 1 to 2 company ID
                                                        Octets 3 to 4 Vendor-defined codec ID */
	0,                                      /*controller_delay */
	0,                                      /*codec_config_len 0 for Transparent Mode */
	0,                                      /*codec_data */
};

/* iso demo send data */
#define APP_LE_ISO_USE_HW_GTIMER 1
#define APP_LE_ISO_DATA_SEND_PATH_NUM 3 // 1 cis initiator + 1 cis acceptor + 1 broadcaster source
#define APP_LE_ISO_DATA_SEND_TASK_PRIO 5
#define APP_LE_ISO_DATA_SEND_TASK_STACK_SIZE (1024*5)
#define APP_LE_ISO_DEFAULT_SDU_INTERVAL_M_S_US RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US //units: us
#if defined(APP_LE_ISO_USE_HW_GTIMER) && APP_LE_ISO_USE_HW_GTIMER
#include "timer_api.h"
#define LE_ISO_SEND_TIMER_ID TIMER13
static gtimer_t bt_le_iso_demo_send_timer = {0};
#else
static void *bt_le_iso_demo_send_timer = NULL;
#endif

typedef struct {
	bool used;
	rtk_bt_le_iso_data_send_info_t iso_data_t;
} app_bt_le_iso_data_path_t;

struct le_iso_demo_task_t {
	void *hdl;
	void *sem;
	uint8_t run;
};

static bool bt_le_iso_demo_send_enable = false;
static void *bt_le_iso_demo_send_sem = NULL;
static struct le_iso_demo_task_t bt_le_iso_demo_data_send_task = {
	.hdl = NULL,
	.sem = NULL,
	.run = 0,
};
static app_bt_le_iso_data_path_t app_le_iso_data_path[APP_LE_ISO_DATA_SEND_PATH_NUM] = {0};
static uint32_t bt_le_iso_demo_send_timer_interval_us = APP_LE_ISO_DEFAULT_SDU_INTERVAL_M_S_US;

static uint16_t app_bt_le_iso_add_data_path(uint16_t iso_conn_handle)
{
	for (uint16_t i = 0; i < APP_LE_ISO_DATA_SEND_PATH_NUM; i ++) {
		if (!app_le_iso_data_path[i].used) {
			app_le_iso_data_path[i].used = true;
			memset((void *)&app_le_iso_data_path[i].iso_data_t, 0, sizeof(rtk_bt_le_iso_data_send_info_t));
			app_le_iso_data_path[i].iso_data_t.iso_conn_handle = iso_conn_handle;
			return 0;
		}
	}
	BT_LOGE("[APP] %s: app_bt_le_iso_add_data_path fail, APP_LE_ISO_DATA_SEND_PATH_NUM not enough! \r\n", __func__);
	return 1;
}

static uint16_t app_bt_le_iso_remove_data_path(uint16_t iso_conn_handle)
{
	for (uint16_t i = 0; i < APP_LE_ISO_DATA_SEND_PATH_NUM; i ++) {
		if (app_le_iso_data_path[i].used &&
			(app_le_iso_data_path[i].iso_data_t.iso_conn_handle == iso_conn_handle)) {
			memset((void *)&app_le_iso_data_path[i], 0, sizeof(app_bt_le_iso_data_path_t));
			return 0;
		}
	}
	BT_LOGE("[APP] %s: app_bt_le_iso_remove_data_path fail, not found! \r\n", __func__);
	return 1;
}

static void app_bt_le_iso_remove_data_path_all(void)
{
	for (uint16_t i = 0; i < APP_LE_ISO_DATA_SEND_PATH_NUM; i ++) {
		if (app_le_iso_data_path[i].used) {
			memset((void *)&app_le_iso_data_path[i], 0, sizeof(app_bt_le_iso_data_path_t));
		}
	}
}

static uint8_t iso_demo_data[128] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

static void bt_le_iso_demo_data_send_task_entry(void *ctx)
{
	(void)ctx;
	uint16_t ret = RTK_BT_FAIL;
	bt_le_iso_demo_data_send_task.run = 1;

	while (bt_le_iso_demo_data_send_task.run) {
		if (bt_le_iso_demo_send_sem) {
			osif_sem_take(bt_le_iso_demo_send_sem, BT_TIMEOUT_FOREVER);
		}
		BT_LOGD("[APP] %s sys_time %d\r\n", __func__, (int)osif_sys_time_get());
		/* foreach iso tx app data path */
		for (uint16_t i = 0; i < APP_LE_ISO_DATA_SEND_PATH_NUM; i ++) {
			if (app_le_iso_data_path[i].used) {
				app_le_iso_data_path[i].iso_data_t.ts_flag = false;
				app_le_iso_data_path[i].iso_data_t.time_stamp = 0;
				app_le_iso_data_path[i].iso_data_t.pkt_seq_num++;
				app_le_iso_data_path[i].iso_data_t.data_len = 128;
				app_le_iso_data_path[i].iso_data_t.p_data = iso_demo_data;
				ret = rtk_bt_le_iso_data_send(&app_le_iso_data_path[i].iso_data_t);
				// BT_LOGA("DATA SEND ISO handle 0x%x pkt 0x%x systime (%d) \r\n", app_le_iso_data_path[i].iso_data_t.iso_conn_handle,
				//      app_le_iso_data_path[i].iso_data_t.pkt_seq_num,
				//      osif_sys_time_get());
				if (ret) {
					BT_LOGE("%s: rtk_bt_le_iso_data_send failed ret:0x%x \r\n", __func__, ret);
				}
			}
		}
	}
	BT_LOGD("[APP] %s task_delete\r\n", __func__);
	osif_sem_give(bt_le_iso_demo_data_send_task.sem);
	bt_le_iso_demo_data_send_task.run = 0;
	bt_le_iso_demo_data_send_task.hdl = NULL;
	osif_task_delete(NULL);
}

static void app_bt_le_iso_send_timer_handler(void *arg)
{
	(void)arg;

	if (bt_le_iso_demo_data_send_task.run) {
		if (bt_le_iso_demo_send_sem) {
			osif_sem_give(bt_le_iso_demo_send_sem);
		}
	}
}

static void app_bt_le_iso_send_timer_init(void)
{
#if defined(APP_LE_ISO_USE_HW_GTIMER) && APP_LE_ISO_USE_HW_GTIMER
	BT_LOGD("[APP] %s hw timer id %d,time_interval_us = %d\r\n", __func__, LE_ISO_SEND_TIMER_ID, (int)bt_le_iso_demo_send_timer_interval_us);
	if (bt_le_iso_demo_send_timer.handler == NULL) {
		gtimer_init(&bt_le_iso_demo_send_timer, LE_ISO_SEND_TIMER_ID);
		gtimer_start_periodical(&bt_le_iso_demo_send_timer, bt_le_iso_demo_send_timer_interval_us, (void *)app_bt_le_iso_send_timer_handler, NULL);
	}
#else
	BT_LOGD("[APP] %s sw time_interval_us = %d\r\n", __func__, (int)bt_le_iso_demo_send_timer_interval_us);
	if (bt_le_iso_demo_send_timer_interval_us % 1000 != 0) {
		BT_LOGE("[APP] %s error: interval %d(us) cannot use sw timer, please use hw timer instead\r\n", __func__, (int)bt_le_iso_demo_send_timer_interval_us);
		return;
	}
	if (!bt_le_iso_demo_send_timer) {
		if (false == osif_timer_create(&bt_le_iso_demo_send_timer, "le_iso_send_timer", 1, bt_le_iso_demo_send_timer_interval_us / 1000, true,
									   app_bt_le_iso_send_timer_handler)) {
			BT_LOGE("[APP] %s osif_timer_create fail\r\n", __func__);
			return;
		}
	}
	if (false == osif_timer_start(&bt_le_iso_demo_send_timer)) {
		BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		return;
	}
#endif
}

#if 0
static void app_bt_le_iso_send_timer_update(uint32_t time_interval_us)
{
	BT_LOGA("[APP] %s: time_interval_us=%d\r\n", __func__, (int)time_interval_us);
	bt_le_iso_demo_send_timer_interval_us = time_interval_us;
#if defined(APP_LE_ISO_USE_HW_GTIMER) && APP_LE_ISO_USE_HW_GTIMER
	if (bt_le_iso_demo_send_timer.handler) {
		gtimer_reload(&bt_le_iso_demo_send_timer, time_interval_us);
	} else {
		BT_LOGE("[APP] %s: timer not init\r\n", __func__);
	}
#else
	if (bt_le_iso_demo_send_timer) {
		if (false == osif_timer_stop(&bt_le_iso_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_restart(&bt_le_iso_demo_send_timer, time_interval_us / 1000)) {
			BT_LOGE("[APP] %s osif_timer_start fail\r\n", __func__);
		}
	}
#endif
}
#endif

static void app_bt_le_iso_send_timer_deinit(void)
{
#if defined(APP_LE_ISO_USE_HW_GTIMER) && APP_LE_ISO_USE_HW_GTIMER
	if (bt_le_iso_demo_send_timer.handler) {
		gtimer_stop(&bt_le_iso_demo_send_timer);
		gtimer_deinit(&bt_le_iso_demo_send_timer);
		bt_le_iso_demo_send_timer.handler = NULL;
	}
#else
	if (bt_le_iso_demo_send_timer) {
		if (false == osif_timer_stop(&bt_le_iso_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_stop fail \r\n", __func__);
		}
		if (false == osif_timer_delete(&bt_le_iso_demo_send_timer)) {
			BT_LOGE("[APP] %s osif_timer_delete fail \r\n", __func__);
		}
		bt_le_iso_demo_send_timer = NULL;
	}
#endif
}

static void app_bt_le_iso_send_data_control(bool enable)
{
	BT_LOGA("[APP] %s %d\r\n", __func__, enable);
	if (enable) {
		if (bt_le_iso_demo_send_enable == true) {
			BT_LOGE("[APP] %s: iso send demo is alreay enabled\r\n", __func__);
			return;
		}
		bt_le_iso_demo_send_enable = true;
		if (bt_le_iso_demo_send_sem == NULL) {
			osif_sem_create(&bt_le_iso_demo_send_sem, 0, 0xFFFFFFFF);
		}
		if (bt_le_iso_demo_data_send_task.sem == NULL) {
			osif_sem_create(&bt_le_iso_demo_data_send_task.sem, 0, 1);
		}
		if (bt_le_iso_demo_data_send_task.hdl == NULL) {
			if (osif_task_create(&bt_le_iso_demo_data_send_task.hdl, ((const char *)"iso_data_send_task"), bt_le_iso_demo_data_send_task_entry,
								 NULL, APP_LE_ISO_DATA_SEND_TASK_STACK_SIZE, APP_LE_ISO_DATA_SEND_TASK_PRIO) != true) {
				BT_LOGE("[APP] %s xTaskCreate(le_iso_data_send_task) failed\r\n", __func__);
			}
		}
		app_bt_le_iso_send_timer_init();
	} else {
		if (bt_le_iso_demo_send_enable == false) {
			BT_LOGE("[APP] %s: iso demo send is alreay disabled\r\n", __func__);
			return;
		}
		bt_le_iso_demo_send_enable = false;
		app_bt_le_iso_send_timer_deinit();
		if (bt_le_iso_demo_data_send_task.hdl) {
			bt_le_iso_demo_data_send_task.run = 0;
			osif_sem_give(bt_le_iso_demo_send_sem);
			osif_sem_take(bt_le_iso_demo_data_send_task.sem, BT_TIMEOUT_FOREVER);
			osif_sem_delete(bt_le_iso_demo_data_send_task.sem);
			bt_le_iso_demo_data_send_task.sem = NULL;
			bt_le_iso_demo_data_send_task.hdl = NULL;
		}
		if (bt_le_iso_demo_send_sem) {
			osif_sem_delete(bt_le_iso_demo_send_sem);
			bt_le_iso_demo_send_sem = NULL;
		}
	}
}

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
			ret = rtk_bt_le_gap_set_ext_scan_param(&bt_le_iso_demo_bis_receiver_ext_scan_param);
			if (RTK_BT_OK != ret) {
				BT_LOGE("[APP] BIS Receiver set default ext scan paramters failed! err: 0x%x\r\n", ret);
			} else {
				ret = rtk_bt_le_gap_start_ext_scan();
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Receiver start ext scan failed! err: 0x%x\r\n", ret);
				}
				bt_le_iso_demo_bis_receiver_scan_flag = 1;
			}
		} else if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
			rtk_bt_le_iso_sync_info_t param = {0};
			rtk_bt_le_pa_synced_info_t *p_info = &pa_sync_ind->info;
			rtk_bt_le_addr_to_str(&p_info->addr, le_addr, sizeof(le_addr));
			BT_LOGA("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid: %d, past_received: %d\r\n",
					le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			BT_AT_PRINT("+BLEGAP:pa_sync,%s,0x%x,%d,%d\r\n",
						le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			bt_le_iso_demo_bis_receiver_sync_id = pa_sync_ind->sync_id;
			param.type = RTK_BT_LE_ISO_PA_SYNC_STATE_SYNCHRONIZED;
			memcpy((void *)&param.info.pa_sync, (void *)pa_sync_ind, sizeof(rtk_bt_le_pa_sync_ind_t));
			rtk_bt_le_iso_sync_info(&param);
		} else if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_TERMINATED) {
			rtk_bt_le_iso_sync_info_t param = {0};
			BT_LOGA("[APP] PA SYNCHRONIZE TERMINATED\r\n");
			param.type = RTK_BT_LE_ISO_PA_SYNC_STATE_TERMINATED;
			memcpy((void *)&param.info.pa_sync, (void *)pa_sync_ind, sizeof(rtk_bt_le_pa_sync_ind_t));
			rtk_bt_le_iso_sync_info(&param);
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
		{
			rtk_bt_le_iso_sync_info_t param = {0};
			param.type = RTK_BT_LE_ISO_GAP_EVT_DISCONN_IND;
			memcpy((void *)&param.info.disc_ind, (void *)disconn_ind, sizeof(rtk_bt_le_disconn_ind_t));
			rtk_bt_le_iso_sync_info(&param);
			app_bt_le_iso_remove_data_path_all();
			app_bt_le_iso_send_data_control(false);
		}
		if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == iso_demo_role) {
			/* start ext adv */
			rtk_bt_le_gap_dev_state_t dev_state;
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
				BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(bt_le_iso_demo_cis_acceptor_ext_adv_handle, 0, 0));
#else
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&bt_le_iso_demo_cis_acceptor_adv_param));
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
		if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == iso_demo_role) {
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
		if (RTK_BLE_ISO_ROLE_CIS_ACCEPTOR == iso_demo_role) {
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
	uint16_t ret = RTK_BT_FAIL;
	switch (evt_code) {
	/* for cig event */
	case RTK_BT_LE_ISO_EVT_CIG_DISCONNECT_INFO: {
		rtk_bt_le_iso_cig_disconnect_info_t *param = (rtk_bt_le_iso_cig_disconnect_info_t *)data;
		BT_LOGA("[APP] rtk_bt_le_iso_cig_disconnect is indicated(conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, cause 0x%x)\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id, param->cause);
		if (RTK_BLE_ISO_ROLE_CIS_INITIATOR == iso_demo_role) {
			if (param->input_path_remove_ind) {
				rtk_bt_le_iso_cig_remove_path(param->cis_conn_handle, RTK_BLE_ISO_DATA_PATH_INPUT_FLAG);
			}
			if (param->output_path_remove_ind) {
				rtk_bt_le_iso_cig_remove_path(param->cis_conn_handle, RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG);
			}
		} else {
			//If role is @ref RTK_BLE_ISO_ISOCH_ROLE_ACCEPTOR for specified CIS,
			//the handle and the associated data path of that CIS will be auto deleted,
			//we don't need to repeat remove path by API rtk_bt_le_iso_cig_remove_path()
			app_bt_le_iso_remove_data_path(param->cis_conn_handle);
			app_bt_le_iso_send_data_control(false);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_CIS_ESTABLISHED_INFO: {
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
			/* setup data path */
			le_iso_demo_setup_path_param.iso_conn_handle = param->cis_conn_handle;
			le_iso_demo_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_INPUT;
			ret = rtk_bt_le_iso_cig_setup_path(&le_iso_demo_setup_path_param);
			if (RTK_BT_OK != ret) {
				BT_LOGE("[APP] rtk_bt_le_iso_cig_setup_path for input fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
			} else {
				BT_LOGA("[APP] rtk_bt_le_iso_cig_setup_path for input ok(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				le_iso_demo_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT;
				ret = rtk_bt_le_iso_cig_setup_path(&le_iso_demo_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] rtk_bt_le_iso_cig_setup_path for output fail(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				} else {
					BT_LOGA("[APP] rtk_bt_le_iso_cig_setup_path for output ok(cis_conn_handle = 0x%x)\r\n", param->cis_conn_handle);
				}
			}
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_SETUP_DATA_PATH: {
		rtk_bt_le_iso_cig_setup_data_path_info_t *param = (rtk_bt_le_iso_cig_setup_data_path_info_t *)data;
		BT_LOGA("[APP] cig setup data path indicate(cause 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, direction 0x%x)\r\n",
				param->cause, param->cis_conn_handle, param->cig_id, param->cis_id, param->data_path_direction);
		if (!param->cause && (RTK_BLE_ISO_DATA_PATH_ADD_INPUT == param->data_path_direction)) {
			/* add app le iso data send path */
			app_bt_le_iso_add_data_path(param->cis_conn_handle);
			app_bt_le_iso_send_data_control(true);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_REMOVE_DATA_PATH: {
		rtk_bt_le_iso_cig_remove_data_path_info_t *param = (rtk_bt_le_iso_cig_remove_data_path_info_t *)data;
		BT_LOGA("[APP] cig remove data path indicate(cause 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, direction 0x%x)\r\n",
				param->cause, param->cis_conn_handle, param->cig_id, param->cis_id, param->data_path_direction);
		if (!param->cause && (RTK_BLE_ISO_DATA_PATH_ADD_INPUT == param->data_path_direction)) {
			/* remove app le iso data send path */
			app_bt_le_iso_remove_data_path(param->cis_conn_handle);
			app_bt_le_iso_send_data_control(false);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_CIG_ACCEPTOR_REQUEST_CIS_IND: {
		rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *param = (rtk_bt_le_iso_cig_acceptor_request_cis_ind_t *)data;
		BT_LOGA("[APP] rtk_bt_le_iso_cig_cis_request indicate(conn_handle 0x%x, cis_conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x)\r\n",
				param->conn_handle, param->cis_conn_handle, param->cig_id, param->cis_id);
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
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_BROADCASTER_SYNC_STATE_IND: {
		rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *param = (rtk_bt_le_iso_big_broadcaster_sync_state_ind_t *)data;
		BT_LOGA("[APP] bis broadcaster sync state change: big_handle: 0x%x, adv_handle 0x%x, sync_state 0x%x, cause: 0x%x\r\n",
				param->big_handle, param->adv_handle, param->sync_state, param->cause);
		if (param->sync_state == RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_BROADCASTING) {
			BT_LOGA("[APP] bis broadcaster is isochronous broadcasting\r\n");
			le_iso_demo_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_INPUT;
			for (uint8_t i = 0; i < param->bis_num; i++) {
				le_iso_demo_setup_path_param.iso_conn_handle = param->bis_conn_handle[i];
				ret = rtk_bt_le_iso_big_setup_path(&le_iso_demo_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Boardcaster setup data path for bis_conn_handle 0x%x failed! err: 0x%x", le_iso_demo_setup_path_param.iso_conn_handle, ret);
				}
			}
		} else if (param->sync_state == RTK_BLE_ISO_BIG_BOARDCASTER_SYNC_STATE_IDLE) {
			BT_LOGA("[APP] bis broadcaster is idle\r\n");
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
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_RECEIVER_SYNC_STATE_IND: {
		rtk_bt_le_iso_big_receiver_sync_state_ind_t *param = (rtk_bt_le_iso_big_receiver_sync_state_ind_t *)data;
		BT_LOGA("[APP] bis receiver sync state change: big_handle: 0x%x, sync_state 0x%x,param->sync_id 0x%x, sync_handle 0x%x, cause: 0x%x\r\n",
				param->big_handle, param->sync_state, param->sync_id, param->sync_handle,  param->cause);
		if (param->sync_state == RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_SYNCHRONIZED) {
			BT_LOGA("[APP] bis receiver sychronized to a big\r\n");
			bt_le_iso_demo_big_handle = param->big_handle;
			le_iso_demo_setup_path_param.data_path_direction = RTK_BLE_ISO_DATA_PATH_ADD_OUTPUT;
			for (uint8_t i = 0; i < param->bis_num; i++) {
				le_iso_demo_setup_path_param.iso_conn_handle = param->bis_conn_handle[i];
				ret = rtk_bt_le_iso_big_setup_path(&le_iso_demo_setup_path_param);
				if (RTK_BT_OK != ret) {
					BT_LOGE("[APP] BIS Receiver setup data path for bis_conn_handle 0x%x failed! err: 0x%x\r\n", le_iso_demo_setup_path_param.iso_conn_handle, ret);
				}
			}
		} else if (param->sync_state == RTK_BLE_ISO_BIG_RECEIVER_SYNC_STATE_TERMINATED) {
			BT_LOGA("[APP] bis receiver sync termiated\r\n");
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_SETUP_DATA_PATH: {
		rtk_bt_le_iso_big_setup_data_path_info_t *param = (rtk_bt_le_iso_big_setup_data_path_info_t *)data;
		BT_LOGA("[APP] big setup data path indicate(cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, direction 0x%x)\r\n",
				param->cause, param->bis_conn_handle, param->big_handle, param->data_path_direction);
		if (!param->cause && (RTK_BLE_ISO_DATA_PATH_ADD_INPUT == param->data_path_direction)) {
			/* add app le iso data send path */
			app_bt_le_iso_add_data_path(param->bis_conn_handle);
			app_bt_le_iso_send_data_control(true);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_BIG_REMOVE_DATA_PATH: {
		rtk_bt_le_iso_big_remove_data_path_info_t *param = (rtk_bt_le_iso_big_remove_data_path_info_t *)data;
		BT_LOGA("[APP] big remove data path indicate(cause 0x%x, bis_conn_handle 0x%x, big_handle 0x%x, direction 0x%x)\r\n",
				param->cause, param->bis_conn_handle, param->big_handle, param->data_path_direction);
		if (!param->cause && (RTK_BLE_ISO_DATA_PATH_ADD_INPUT == param->data_path_direction)) {
			/* add app le iso data send path */
			app_bt_le_iso_remove_data_path(param->bis_conn_handle);
			app_bt_le_iso_send_data_control(false);
		}
		break;
	}

	case RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND: { //BT api shall not be called here
		rtk_bt_le_iso_direct_iso_data_ind_t *p_bt_direct_iso = (rtk_bt_le_iso_direct_iso_data_ind_t *)data;
		if (p_bt_direct_iso->iso_sdu_len) {
			// BT_LOGA("[APP] RTK_BT_LE_ISO_EVT_DATA_RECEIVE_IND, iso_conn_handle 0x%x, pkt_status_flag 0x%x, pkt_seq_num 0x%x, ts_flag 0x%x, time_stamp (0x%x),iso_sdu_len 0x%x, p_buf %08x, buf_len %d, offset %d, Systime (%d)\r\n",
			//      p_bt_direct_iso->iso_conn_handle, p_bt_direct_iso->pkt_status_flag,
			//      p_bt_direct_iso->pkt_seq_num, p_bt_direct_iso->ts_flag,
			//      (unsigned int)p_bt_direct_iso->time_stamp, p_bt_direct_iso->iso_sdu_len, p_bt_direct_iso->p_buf, p_bt_direct_iso->buf_len, p_bt_direct_iso->offset, osif_sys_time_get());
#if 0
			{
				static uint32_t last_pkt_num = 0;

				if (p_bt_direct_iso->pkt_seq_num != (last_pkt_num + 1)) {
					BT_LOGE("[APP] data loss: pkt_seq_num: %d, last_pkt_num: %d \r\n", p_bt_direct_iso->pkt_seq_num, last_pkt_num);
				}
				last_pkt_num = p_bt_direct_iso->pkt_seq_num;
			}
#endif
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

int bt_le_iso_main(uint8_t role, uint8_t enable)
{
	if (1 == enable) {
		if (demo_init_flag) {
			BT_LOGE("%s Already init! \r\n", __func__);
			return -1;
		}

		switch (role) {
		/* cis initiator */
		case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_iso_app_conf_t *p_le_iso_conf = &bt_le_iso_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			{
				p_le_iso_conf->iso_role = RTK_BLE_ISO_ROLE_CIS_INITIATOR;
				memset((void *)p_le_iso_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_le_iso_conf->device_name, (uint8_t *)APP_LE_ISO_CIS_INITIATOR_DEVICE_NAME, strlen((const char *)APP_LE_ISO_CIS_INITIATOR_DEVICE_NAME));
				p_le_iso_conf->device_name_len = strlen((const char *)APP_LE_ISO_CIS_INITIATOR_DEVICE_NAME);
			}
			{
				//set GAP configuration
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC | RTK_BT_PROFILE_LE_ISO;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_iso_app_conf, (void *)p_le_iso_conf, sizeof(rtk_bt_le_iso_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			/* register iso event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			{
				rtk_bt_le_iso_set_cig_cis_param_t cig_start_setting = {0};
				rtk_bt_le_iso_cig_initiator_set_cig_param_t set_cig_param = {0};
				rtk_bt_le_iso_cig_initiator_set_cis_param_t set_cis_param = {0};

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
				for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
					/* set cis number */
					cig_start_setting.cis_num = RTK_BLE_CIS_MAX_NUM / RTK_BLE_CIG_MAX_NUM;
					set_cig_param.cig_id = i + 1;
					memcpy((void *)&cig_start_setting.set_cig_param, (void *)&set_cig_param, sizeof(rtk_bt_le_iso_cig_initiator_set_cig_param_t));
					for (uint8_t j = 0; j < cig_start_setting.cis_num; j ++) {
						set_cis_param.cis_id = (j + 1) + (i * 2);
						memcpy((void *)&cig_start_setting.set_cis_param[j], (void *)&set_cis_param, sizeof(rtk_bt_le_iso_cig_initiator_set_cis_param_t));
					}
					BT_APP_PROCESS(rtk_bt_le_iso_cig_start_setting(&cig_start_setting));
				}
			}
			iso_demo_role = role;
			demo_init_flag = true;
			break;
		}

		/* cis acceptor */
		case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_iso_app_conf_t *p_le_iso_conf = &bt_le_iso_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			{
				p_le_iso_conf->iso_role = RTK_BLE_ISO_ROLE_CIS_ACCEPTOR;
				memset((void *)p_le_iso_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_le_iso_conf->device_name, (uint8_t *)APP_LE_ISO_CIS_ACCEPTOR_DEVICE_NAME, strlen((const char *)APP_LE_ISO_CIS_ACCEPTOR_DEVICE_NAME));
				p_le_iso_conf->device_name_len = strlen((const char *)APP_LE_ISO_CIS_ACCEPTOR_DEVICE_NAME);
			}
			{
				//set GAP configuration
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LE_ISO;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_iso_app_conf, (void *)p_le_iso_conf, sizeof(rtk_bt_le_iso_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			/* register iso event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(p_le_iso_conf->device_name));
			BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT));

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&bt_le_iso_demo_cis_acceptor_ext_adv_param, &bt_le_iso_demo_cis_acceptor_ext_adv_handle));
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(bt_le_iso_demo_cis_acceptor_ext_adv_handle, (uint8_t *)bt_le_iso_demo_cis_acceptor_ext_adv_data,
														  sizeof(bt_le_iso_demo_cis_acceptor_ext_adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(bt_le_iso_demo_cis_acceptor_ext_adv_handle, 0, 0));
#else
			BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(bt_le_iso_demo_cis_acceptor_adv_data, sizeof(bt_le_iso_demo_cis_acceptor_adv_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(bt_le_iso_demo_cis_acceptor_scan_rsp_data, sizeof(bt_le_iso_demo_cis_acceptor_scan_rsp_data)));
			BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&bt_le_iso_demo_cis_acceptor_adv_param));
#endif
			iso_demo_role = role;
			demo_init_flag = true;
			break;
		}

		/* bis broadcaster */
		case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_iso_app_conf_t *p_le_iso_conf = &bt_le_iso_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			{
				p_le_iso_conf->iso_role = RTK_BLE_ISO_ROLE_BIS_BROADCASTER;
				memset((void *)p_le_iso_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_le_iso_conf->device_name, (uint8_t *)APP_LE_ISO_BIS_BROADCASTER_DEVICE_NAME, strlen((const char *)APP_LE_ISO_BIS_BROADCASTER_DEVICE_NAME));
				p_le_iso_conf->device_name_len = strlen((const char *)APP_LE_ISO_BIS_BROADCASTER_DEVICE_NAME);
			}
			{
				//set GAP configuration
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LE_ISO;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_iso_app_conf, (void *)p_le_iso_conf, sizeof(rtk_bt_le_iso_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			//1. register iso event callback
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			//3. create ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&bt_le_iso_demo_bis_broadcaster_ext_adv_param, &bt_le_iso_demo_bis_broadcaster_ext_adv_handle));
			//4. set ext adv data
			BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(bt_le_iso_demo_bis_broadcaster_ext_adv_handle, (uint8_t *)bt_le_iso_demo_bis_broadcaster_ext_adv_data,
														  sizeof(bt_le_iso_demo_bis_broadcaster_ext_adv_data)));
			//5. start ext adv
			BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(bt_le_iso_demo_bis_broadcaster_ext_adv_handle, 0, 0));
			//6. start pa
			bt_le_iso_demo_pa_param.adv_handle = bt_le_iso_demo_bis_broadcaster_ext_adv_handle;
			BT_APP_PROCESS(rtk_bt_le_gap_start_pa(&bt_le_iso_demo_pa_param));
			//7. create big
			BT_APP_PROCESS(rtk_bt_le_iso_big_broadcaster_create(bt_le_iso_demo_bis_broadcaster_ext_adv_handle, &bt_le_iso_demo_bis_broadcaster_param,
																&bt_le_iso_demo_big_handle));
			iso_demo_role = role;
			demo_init_flag = true;
			break;
		}

		/* bis receiver */
		case RTK_BLE_ISO_ROLE_BIS_RECEIVER: {
			rtk_bt_app_conf_t bt_app_conf = {0};
			rtk_bt_le_iso_app_conf_t *p_le_iso_conf = &bt_le_iso_demo_app_conf;
			rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
			char addr_str[30] = {0};
			{
				p_le_iso_conf->iso_role = RTK_BLE_ISO_ROLE_BIS_RECEIVER;
				memset((void *)p_le_iso_conf->device_name, 0, RTK_BT_GAP_DEVICE_NAME_LEN);
				memcpy((void *)p_le_iso_conf->device_name, (uint8_t *)APP_LE_ISO_BIS_RECEIVER_DEVICE_NAME, strlen((const char *)APP_LE_ISO_BIS_RECEIVER_DEVICE_NAME));
				p_le_iso_conf->device_name_len = strlen((const char *)APP_LE_ISO_BIS_RECEIVER_DEVICE_NAME);
			}
			{
				//set GAP configuration
				bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_LE_ISO;
				bt_app_conf.mtu_size = 180;
				bt_app_conf.master_init_mtu_req = true;
				bt_app_conf.prefer_all_phy = 0;
				bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
				bt_app_conf.max_tx_octets = 0x40;
				bt_app_conf.max_tx_time = 0x200;
				memcpy((void *)&bt_app_conf.le_iso_app_conf, (void *)p_le_iso_conf, sizeof(rtk_bt_le_iso_app_conf_t));
			}
			/* Enable BT */
			BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
			BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
			rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
			BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);
			/* 1. Initilize GAP part */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, app_bt_le_iso_gap_app_callback));
			/* 2. register iso event callback */
			BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_ISO, app_bt_le_iso_cb));
			iso_demo_role = role;
			demo_init_flag = true;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;
		}
	} else if (0 == enable) {
		if (!demo_init_flag) {
			BT_LOGE("%s No need to deinit! \r\n", __func__);
			return -1;
		}
		switch (role) {
		/* cis initiator */
		case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
			rtk_bt_le_iso_conn_handle_info_t info = {0};
			rtk_bt_le_iso_cis_channel_info_t isoch_info = {0};
			app_bt_le_iso_remove_data_path_all();
			app_bt_le_iso_send_data_control(false);
			{
				if (!rtk_bt_le_iso_get_iso_conn_handle(RTK_BLE_ISO_ROLE_CIS_INITIATOR, &info)) {
					for (uint8_t i = 0; i < info.iso_handle_num; i ++) {
						if (!rtk_bt_le_iso_cig_get_isoch_info(info.iso_handle[i], &isoch_info)) {
							if (isoch_info.state != RTK_BLE_ISO_ISOCH_STATE_IDLE) {
								BT_LOGA("[APP] %s isoch_info.state=0x%x\r\n", __func__, isoch_info.state);
								rtk_bt_le_iso_cig_disconnect(info.iso_handle[i]);
							}
						}
					}
				}
				for (uint8_t i = 0; i < RTK_BLE_CIG_MAX_NUM; i ++) {
					rtk_bt_le_iso_cig_initiator_remove_cig(i + 1);
				}
			}
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			iso_demo_role = RTK_BLE_ISO_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}

		/* cis acceptor */
		case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
			app_bt_le_iso_remove_data_path_all();
			app_bt_le_iso_send_data_control(false);
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
			//1. stop ea
			BT_APP_PROCESS(rtk_bt_le_gap_stop_ext_adv(bt_le_iso_demo_cis_acceptor_ext_adv_handle));
#else
			//1. stop adv
			BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
#endif
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			iso_demo_role = RTK_BLE_ISO_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}

		case RTK_BLE_ISO_ROLE_BIS_BROADCASTER: {
			rtk_bt_le_iso_conn_handle_info_t info = {0};
			app_bt_le_iso_remove_data_path_all();
			app_bt_le_iso_send_data_control(false);
			/* remove path */
			if (!rtk_bt_le_iso_get_iso_conn_handle(RTK_BLE_ISO_ROLE_BIS_BROADCASTER, &info)) {
				for (uint8_t i = 0; i < info.iso_handle_num; i ++) {
					rtk_bt_le_iso_big_remove_path(info.iso_handle[i], RTK_BLE_ISO_DATA_PATH_INPUT_FLAG);
				}
			}
			/* terminate big */
			if (bt_le_iso_demo_big_handle) {
				rtk_bt_le_iso_big_broadcaster_terminate(bt_le_iso_demo_big_handle);
				bt_le_iso_demo_big_handle = 0;
			}
			/* stop pa */
			rtk_bt_le_gap_stop_pa(bt_le_iso_demo_bis_broadcaster_ext_adv_handle);
			/* stop ea */
			rtk_bt_le_gap_stop_ext_adv(bt_le_iso_demo_bis_broadcaster_ext_adv_handle);
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			iso_demo_role = RTK_BLE_ISO_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}

		case RTK_BLE_ISO_ROLE_BIS_RECEIVER: {
			rtk_bt_le_iso_conn_handle_info_t info = {0};
			app_bt_le_iso_remove_data_path_all();
			app_bt_le_iso_send_data_control(false);
			/* remove path */
			if (!rtk_bt_le_iso_get_iso_conn_handle(RTK_BLE_ISO_ROLE_BIS_RECEIVER, &info)) {
				for (uint8_t i = 0; i < info.iso_handle_num; i ++) {
					rtk_bt_le_iso_big_remove_path(info.iso_handle[i], RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG);
				}
			}
			/* terminate sync */
			if (0xFF != bt_le_iso_demo_big_handle) {
				rtk_bt_le_iso_big_receiver_terminate_sync(bt_le_iso_demo_big_handle);
				bt_le_iso_demo_big_handle = 0xFF;
			}
			if (0xFF != bt_le_iso_demo_bis_receiver_sync_id) {
				rtk_bt_le_gap_pa_sync_terminate(bt_le_iso_demo_bis_receiver_sync_id);
				bt_le_iso_demo_bis_receiver_sync_id = 0xFF;
			}
			/* stop ext scan */
			if (bt_le_iso_demo_bis_receiver_scan_flag) {
				rtk_bt_le_gap_stop_ext_scan();
				bt_le_iso_demo_bis_receiver_scan_flag = 0;
			}
			/* Disable BT */
			BT_APP_PROCESS(rtk_bt_disable());
			iso_demo_role = RTK_BLE_ISO_ROLE_UNKNOWN;
			demo_init_flag = false;
			break;
		}

		default:
			BT_LOGE("[APP] unknown role 0x%x\r\n", role);
			break;
		}
	}
	return 0;
}