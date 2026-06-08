// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

extern struct rtw_event_hdl_func_t event_external_hdl[];
extern u16 array_len_of_event_external_hdl;

/**
 * @brief external event handle
 */
static void whc_host_external_event_handle(struct whc_ipc_dev_req_msg *p_recv_msg)
{
	u32 i, event;
	char *evt_info;

	if (p_recv_msg->enevt_id == WHC_API_WIFI_EVENT) {
		event = (u32)p_recv_msg->param_buf[0];
		evt_info = llhw_ipc_fw_phy_to_virt(p_recv_msg->param_buf[1]);

		for (i = 0; i < array_len_of_event_external_hdl; i++) {
			if (event_external_hdl[i].evt_id == event) {
				if (event_external_hdl[i].handler == NULL) {
					continue;
				}
				event_external_hdl[i].handler(evt_info);
			}
		}
	}
}

u8 whc_host_event_scan_report_indicate(u32 *param_buf)
{
	struct device *pdev = NULL;
	u32 channel = param_buf[0];
	u32 frame_is_bcn = param_buf[1];
	s32 rssi = (s32)param_buf[2];
	unsigned char *mac_addr = llhw_ipc_fw_phy_to_virt(param_buf[3]);
	unsigned char *IEs = llhw_ipc_fw_phy_to_virt(param_buf[4]);
	u32 ie_len = param_buf[5];

	if (!global_idev.event_ch) {
		dev_err(global_idev.pwhc_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	whc_host_inform_bss(channel, frame_is_bcn, rssi, mac_addr, IEs, ie_len);

func_exit:
	return 0;
}

u8 whc_host_event_update_regd_indicate(u32 *param_buf)
{
	struct rtw_country_code_table *ptab = (struct rtw_country_code_table *)llhw_ipc_fw_phy_to_virt(param_buf[0]);

	whc_host_regd_update(ptab);
	return 0;
}

u8 whc_host_event_set_acs_info(u32 *param_buf)
{
	struct rtw_acs_mntr_rpt *acs_rpt = llhw_ipc_fw_phy_to_virt(param_buf[0]);

	if (!global_idev.event_ch) {
		dev_err(global_idev.pwhc_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		return 0;
	}

	if (!global_idev.ipc_dev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		return 0;
	}

	whc_host_acs_info_hdl(acs_rpt);
	return 0;
}

u8 whc_host_event_join_status_indicate(u32 *param_buf)
{
	u32 event = (u32)param_buf[0];
	char *evt_info = llhw_ipc_fw_phy_to_virt(param_buf[1]);

	whc_host_join_status_hdl(event, evt_info);
	return 0;
}

u8 whc_host_event_set_netif_info(u32 *param_buf)
{
	int idx = (u32)param_buf[0];
	unsigned char *dev_addr = llhw_ipc_fw_phy_to_virt(param_buf[1]);

	if (!global_idev.event_ch) {
		dev_err(global_idev.pwhc_dev, "%s,%s: event_priv_t is NULL in!\n", "event", __func__);
		return 0;
	}

	if (!global_idev.ipc_dev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		return 0;
	}

	whc_host_set_netif_info_hdl(idx, dev_addr);
	return 0;
}

u8 whc_host_event_get_network_info(u32 *param_buf)
{
	struct whc_ipc_dev_req_msg *p_ipc_msg = llhw_ipc_fw_phy_to_virt(global_idev.event_priv.recv_ipc_msg.msg);
	uint32_t type = (uint32_t)param_buf[0];
	int idx = param_buf[2];
	uint32_t rsp_len = 0;

	if (!global_idev.event_ch) {
		dev_err(global_idev.pwhc_dev, "%s: event_priv_t is NULL!\n", "event");
		return 0;
	}

	if (!global_idev.ipc_dev) {
		dev_err(global_idev.pwhc_dev, "%s: device is NULL!\n", "event");
		return 0;
	}

	if (whc_host_get_network_info_hdl(type, idx, global_idev.event_priv.dev_req_network_info, &rsp_len) == 0) {
		return 0;
	}

	p_ipc_msg->ret = (u32)global_idev.event_priv.dev_req_network_info_phy;
	return 0;
}

u8 whc_host_event_promisc_pkt_hdl(u32 *param_buf)
{
	struct device *pdev = NULL;
	struct rtw_rx_pkt_info *ppktinfo = NULL;
	uint8_t *buf = NULL;

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL!\n", "event", __func__);
		goto func_exit;
	}

	ppktinfo = llhw_ipc_fw_phy_to_virt((phys_addr_t)param_buf[0]);
	buf = (uint8_t *)llhw_ipc_fw_phy_to_virt((phys_addr_t)ppktinfo->buf);

	ppktinfo->buf = buf;
	rtw_promisc_rx(ppktinfo);
func_exit:
	return 0;
}

#ifdef CONFIG_NAN
u8 whc_host_event_nan_match_indicate(u32 *param_buf)
{
	struct device *pdev = NULL;
	u8 type = param_buf[0];
	u8 inst_id = param_buf[1];
	u8 peer_inst_id = param_buf[2];
	unsigned char *mac_addr = llhw_ipc_fw_phy_to_virt(param_buf[3]);
	unsigned char *IEs = llhw_ipc_fw_phy_to_virt(param_buf[4]);
	u32 info_len = param_buf[5];
	u64 cookie = ((u64)param_buf[7] << 32) | param_buf[6];

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	whc_host_nan_handle_sdf(type, inst_id, peer_inst_id, mac_addr, info_len, IEs, cookie);

func_exit:
	return 0;
}

u8 whc_host_event_nan_cfgvendor_event_indicate(u32 *param_buf)
{
	struct device *pdev = NULL;
	u8 event_id = param_buf[0];
	unsigned char *event_addr = llhw_ipc_fw_phy_to_virt(param_buf[1]);
	u32 size = param_buf[2];

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	whc_host_cfgvendor_nan_event_indication(event_id, event_addr, size);

func_exit:
	return 0;
}

u8 whc_host_event_nan_cfgvendor_cmd_reply(u32 *param_buf)
{
	struct device *pdev = NULL;
	unsigned char *data_addr = llhw_ipc_fw_phy_to_virt(param_buf[0]);
	u32 size = param_buf[1];

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s,%s: device is NULL in scan!\n", "event", __func__);
		goto func_exit;
	}

	whc_host_cfgvendor_send_cmd_reply(data_addr, size);

func_exit:
	return 0;
}
#endif

void whc_host_event_task(struct work_struct *data)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct device *pdev = NULL;
	struct whc_ipc_dev_req_msg *p_recv_msg = NULL;
	int msg_len = 0;

	if (!global_idev.event_ch) {
		dev_err(global_idev.pwhc_dev, "%s: event_priv_t is NULL!\n", "event");
		goto func_exit;
	}

	pdev = global_idev.ipc_dev;
	if (!pdev) {
		dev_err(global_idev.pwhc_dev, "%s: device is NULL!\n", "event");
		goto func_exit;
	}

	msg_len = event_priv->recv_ipc_msg.msg_len;
	if (!event_priv->recv_ipc_msg.msg || !msg_len) {
		dev_err(global_idev.pwhc_dev, "%s: Invalid device message!\n", "event");
		goto func_exit;
	}
	p_recv_msg = llhw_ipc_fw_phy_to_virt(event_priv->recv_ipc_msg.msg);

	whc_host_external_event_handle(p_recv_msg);

	whc_host_internal_event_handle(p_recv_msg->enevt_id, p_recv_msg->param_buf);

	/*set enevt_id to 0 to notify NP that event is finished*/
	p_recv_msg->enevt_id = WHC_API_PROCESS_DONE;

func_exit:
	return;
}

static u32 whc_host_event_interrupt(aipc_ch_t *ch, ipc_msg_struct_t *pmsg)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	u32 ret = 0;

	if (!event_priv) {
		dev_err(global_idev.pwhc_dev, "%s: event_priv_t is NULL in interrupt!\n", "event");
		goto func_exit;
	}

	/* copy ipc_msg from temp memory in ipc interrupt. */
	memcpy((u8 *) & (event_priv->recv_ipc_msg), (u8 *)pmsg, sizeof(ipc_msg_struct_t));
	queue_work(event_priv->api_workqueue, &(event_priv->api_work));

func_exit:
	return ret;
}

int whc_host_event_init(struct whc_device *idev)
{
	struct event_priv_t	*event_priv = &global_idev.event_priv;
	aipc_ch_t	*event_ch = global_idev.event_ch;

	/* initialize the mutex to send event_priv message. */
	mutex_init(&(event_priv->iiha_send_mutex));

	event_priv->preq_msg = dma_alloc_coherent(event_ch->pdev, sizeof(struct whc_ipc_host_req_msg), &event_priv->req_msg_phy_addr, GFP_KERNEL);
	if (!event_priv->preq_msg) {
		dev_err(global_idev.pwhc_dev, "%s: allloc req_msg error.\n", "event");
		return -ENOMEM;
	}

	/* coherent alloc some non-cache memory for transmit network_info to NP */
	event_priv->dev_req_network_info = dma_alloc_coherent(event_ch->pdev, DEV_REQ_NETWORK_INFO_MAX_LEN, &event_priv->dev_req_network_info_phy, GFP_KERNEL);
	if (!event_priv->dev_req_network_info) {
		dev_err(global_idev.pwhc_dev, "%s: allloc dev_req_network_info error.\n", "event");
		dma_free_coherent(event_ch->pdev, sizeof(struct whc_ipc_host_req_msg), event_priv->preq_msg, event_priv->req_msg_phy_addr);
		return -ENOMEM;
	}

	/* initialize event work */
	event_priv->api_workqueue = alloc_ordered_workqueue("api_ordered_wq", 0);
	if (!event_priv->api_workqueue) {
		dev_err(global_idev.pwhc_dev, "%s: Failed to create workqueue\n", "event");
		dma_free_coherent(event_ch->pdev, DEV_REQ_NETWORK_INFO_MAX_LEN,
						  event_priv->dev_req_network_info, event_priv->dev_req_network_info_phy);
		dma_free_coherent(event_ch->pdev, sizeof(struct whc_ipc_host_req_msg), event_priv->preq_msg, event_priv->req_msg_phy_addr);
		return -ENOMEM;
	}

	INIT_WORK(&(event_priv->api_work), whc_host_event_task);

	return 0;
}

void whc_host_event_deinit(void)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	aipc_ch_t	*event_ch = global_idev.event_ch;

	/* free sema to wakeup the message queue task */
	if (event_priv->api_workqueue) {
		destroy_workqueue(event_priv->api_workqueue);
	}

	dma_free_coherent(event_ch->pdev, DEV_REQ_NETWORK_INFO_MAX_LEN,
					  event_priv->dev_req_network_info, event_priv->dev_req_network_info_phy);
	dma_free_coherent(event_ch->pdev, sizeof(struct whc_ipc_host_req_msg), event_priv->preq_msg, event_priv->req_msg_phy_addr);

	/* deinitialize the mutex to send event_priv message. */
	mutex_destroy(&(event_priv->iiha_send_mutex));

	return;
}

struct aipc_ch_ops whc_ipc_host_event_ops = {
	.channel_recv = whc_host_event_interrupt,
};
