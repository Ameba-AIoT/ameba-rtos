/**
  ******************************************************************************
  * @file    rtw_radar_event.c
  * @author
  * @version
  * @date
  * @brief   Radar proc report event dispatch and host side IPC report flow control.
  *          The radar algorithm itself lives in component/sensing_algo.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "rtw_inic_common.h"
#if defined(CONFIG_RADAR) && defined(CONFIG_WIFI_RADAR_ENABLE) && !defined(CONFIG_MP_SHRINK) && \
	(defined(CONFIG_WHC_NONE) || defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD))

static void (*g_radar_report_cb)(u8 *evt_info) = NULL;
static u8 g_radar_drop_grp = 0;  /* current proc rpt group is dropped on host */
static u8 g_radar_pending_rpt = 0;

void wifi_radar_register_report_cb(void (*cb)(u8 *evt_info))
{
	g_radar_report_cb = cb;
	g_radar_drop_grp = 0;
}

void wifi_radar_report_evt_hdl(u8 *evt_info)
{
	if (g_radar_report_cb) {
		g_radar_report_cb(evt_info);
	}
}

/* Host side, before queuing an NP event. Returns TRUE to drop it: a proc rpt group is dropped whole if
 * the previous group is still queued at its head (host evt task busy, e.g. PSK calc), as the app buffer
 * would reject it anyway. Only decided at the group head, a partial group breaks the app reassembly. */
u8 wifi_radar_host_rpt_drop(u32 event_id, u32 *param_buf)
{
	struct rtw_event_radar_proc_rpt_info *rpt;

	if ((event_id == WHC_API_WIFI_EVENT) && (param_buf[0] == RTW_EVENT_RADAR_PROC_RPT) && (param_buf[1])) {
		rpt = (struct rtw_event_radar_proc_rpt_info *)param_buf[1];
	} else {
		return FALSE;
	}

	DCache_Invalidate((u32)rpt, sizeof(struct rtw_event_radar_proc_rpt_info));
	if (rpt->rpt_type == RTW_RADAR_TYPE_STATIC_REMOVE_FAR && rpt->rpt_seg_start) {
		/* Latched only at the group head and held until the next head, so a group is kept or dropped as a whole */
		g_radar_drop_grp = g_radar_pending_rpt ? 1 : 0;
		if (g_radar_drop_grp) {
			RTK_LOGA(NOTAG, "[radar] host busy(%d), drop grp\n", g_radar_pending_rpt);
		}
	}
	return g_radar_drop_grp;
}

/* Called from both host blk task (enqueue) and evt task (dequeue), so the caller must hold the
 * unblk queue mutex to keep this read-modify-write atomic. */
void wifi_radar_host_pending_cnt(u32 event_id, u32 event_cmd, u8 is_enqueue)
{
	if ((event_id == WHC_API_WIFI_EVENT) && (event_cmd == RTW_EVENT_RADAR_PROC_RPT)) {
		if (is_enqueue) {
			g_radar_pending_rpt++;
		} else {
			g_radar_pending_rpt--;
		}
	}
}

#endif
