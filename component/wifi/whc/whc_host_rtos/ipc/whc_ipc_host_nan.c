/**
  ******************************************************************************
  * @file    whc_ipc_host_nan.c
  * @author
  * @version
  * @date
  * @brief   AP-core (whc_host_rtos) NAN cfgvendor command/event glue over IPC.
  *          Mirrors the linux-host path (whc_host_linux/driver/ipc/
  *          whc_ipc_host_event_tx.c / _rx.c) for an on-chip RTOS host: the NAN
  *          driver core runs on the NP core and is reached via the existing
  *          WHC IPC transport. AP and NP share memory, so only DCache
  *          maintenance is needed (no phys<->virt translation).
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "rtw_inic_common.h"
#include "whc_ipc_host_nan.h"

#ifdef CONFIG_WIFI_NAN_HOST_APP

/* ---------------------------- Global Variables ---------------------------- */
/* NAN event / cmd-reply callbacks registered by the AP-core control layer. */
static void (*nan_evt_cb)(uint16_t event_id, void *buf, uint32_t len) = NULL;
static void (*nan_cmd_reply_cb)(void *buf, uint32_t len) = NULL;

/* ---------------------------- Public Functions ---------------------------- */
void whc_host_nan_register_evt_cb(void (*cb)(uint16_t event_id, void *buf, uint32_t len))
{
	nan_evt_cb = cb;
}

void whc_host_nan_register_cmd_reply_cb(void (*cb)(void *buf, uint32_t len))
{
	nan_cmd_reply_cb = cb;
}

int whc_host_nan_cfgvendor_cmd(uint16_t vendor_cmd, const void *data, uint32_t len)
{
	u32 param_buf[3];
	void *buf = NULL;
	int ret;

	if (data && len) {
		buf = rtos_mem_zmalloc(len);
		if (buf == NULL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "%s: malloc fail\n", __func__);
			return -1;
		}
		memcpy(buf, data, len);
		/* NP DCache_Invalidate's this buffer before reading it */
		DCache_Clean((u32)buf, len);
	}

	param_buf[0] = (u32)vendor_cmd;
	param_buf[1] = (u32)buf;
	param_buf[2] = (u32)len;

	/* blocking: returns after NP has run rtw_cfgvendor_cmd_process() */
	ret = whc_ipc_host_api_message_send(WHC_API_NAN_CFGVENFOR, param_buf, 3);

	if (buf) {
		rtos_mem_free(buf);
	}
	return ret;
}

int whc_host_nan_init(void)
{
	return whc_ipc_host_api_message_send(WHC_API_NAN_INIT, NULL, 0);
}

int whc_host_nan_deinit(void)
{
	return whc_ipc_host_api_message_send(WHC_API_NAN_DEINIT, NULL, 0);
}

int whc_host_nan_start(uint8_t master_pref, uint8_t band_support)
{
	u32 param_buf[2];

	param_buf[0] = (u32)master_pref;
	param_buf[1] = (u32)band_support;
	return whc_ipc_host_api_message_send(WHC_API_NAN_START, param_buf, 2);
}

int whc_host_nan_stop(void)
{
	return whc_ipc_host_api_message_send(WHC_API_NAN_STOP, NULL, 0);
}

/* ---- C2H dispatch-table handlers (called from whc_ipc_host_unblk_api_task_h) ----
 * The payload buffer was copied from shared memory into a host-owned buffer in
 * whc_ipc_host_api_unblk_api_msg_to_node(); the dispatch table's free_bitmap
 * frees it after the handler returns. */
void whc_ipc_host_api_nan_cfgvendor_event_handler(struct whc_ipc_host_unblk_api_node *p_node)
{
	u16 event_id = (u16)p_node->param_buf[0];
	void *buf = (void *)p_node->param_buf[1];
	u32 len = p_node->param_buf[2];

	if (nan_evt_cb) {
		nan_evt_cb(event_id, buf, len);
	}
}

void whc_ipc_host_api_nan_cfgvendor_cmd_reply_handler(struct whc_ipc_host_unblk_api_node *p_node)
{
	void *buf = (void *)p_node->param_buf[0];
	u32 len = p_node->param_buf[1];

	if (nan_cmd_reply_cb) {
		nan_cmd_reply_cb(buf, len);
	}
}

#endif /* CONFIG_WIFI_NAN_HOST_APP */
