// SPDX-License-Identifier: GPL-2.0-only
#include <whc_host_linux.h>

u8 whc_host_event_scan_report_indicate(u32 *param_buf)
{
	u32 channel = param_buf[0];
	u32 frame_is_bcn = param_buf[1];
	s32 rssi = (s32)param_buf[2];
	u32 ie_len = param_buf[3];
	unsigned char *mac_addr = (u8 *) &param_buf[4];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	whc_host_inform_bss(channel, frame_is_bcn, rssi, mac_addr, IEs, ie_len);
	return 0;
}

u8 whc_host_event_update_regd_indicate(u32 *param_buf)
{
	struct rtw_country_code_table *ptab = (struct rtw_country_code_table *)&param_buf[0];

	whc_host_regd_update(ptab);
	return 0;
}

u8 whc_host_event_set_acs_info(u32 *param_buf)
{
	whc_host_acs_info_hdl((struct rtw_acs_mntr_rpt *)&param_buf[0]);
	return 0;
}

u8 whc_host_event_join_status_indicate(u32 *param_buf)
{
	u32 event = (u32)param_buf[0];
	char *evt_info = (char *)&param_buf[2];

	whc_host_join_status_hdl(event, evt_info);
	return 0;
}

u8 whc_host_event_set_netif_info(u32 *param_buf)
{
	whc_host_set_netif_info_hdl((int)param_buf[0], (u8 *)&param_buf[1]);
	return 0;
}

u8 whc_host_event_get_network_info(u32 *param_buf)
{
	/* NOT support for whc non-ipc mode.
	Host would update network info through WHC_WIFI_TEST_NETWORK_INFO_UPDATE. */
	return 0;
}

#ifdef CONFIG_NAN
u8 whc_host_event_nan_match_indicate(u32 *param_buf)
{
	u8 type = param_buf[0];
	u8 inst_id = param_buf[1];
	u8 peer_inst_id = param_buf[2];
	u32 info_len = param_buf[3];
	u64 cookie = ((u64)param_buf[5] << 32) | param_buf[4];
	unsigned char *mac_addr = (u8 *)&param_buf[6];
	unsigned char *IEs = mac_addr + ETH_ALEN;

	whc_host_nan_handle_sdf(type, inst_id, peer_inst_id, mac_addr, info_len, IEs, cookie);
	return 0;
}

u8 whc_host_event_nan_cfgvendor_event_indicate(u32 *param_buf)
{
	u8 event_id = param_buf[0];
	u32 size = param_buf[1];
	unsigned char *event_addr = (u8 *)&param_buf[2];

	whc_host_cfgvendor_nan_event_indication(event_id, event_addr, size);
	return 0;
}

u8 whc_host_event_nan_cfgvendor_cmd_reply(u32 *param_buf)
{
	u32 size = param_buf[0];
	unsigned char *data_addr = (u8 *)&param_buf[1];

	whc_host_cfgvendor_send_cmd_reply(data_addr, size);
	return 0;
}

#ifdef CONFIG_WHCH
u8 whc_host_event_nan_ndp_status_indicate(u32 *param_buf)
{
	struct rtw_event_nan_ndp_status_info *info =
		(struct rtw_event_nan_ndp_status_info *)&param_buf[2];

	if (info->status) {
		whch_host_nan_ndp_established(info);
	} else {
		whch_host_nan_ndp_terminated(info);
	}
	return 0;
}
#endif /* CONFIG_WHCH */
#endif

void whc_host_event_task(struct work_struct *data)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct whc_api_info *p_recv_msg = (struct whc_api_info *)(event_priv->rx_api_msg->data + SIZE_RX_DESC);
	u32 *param_buf = (u32 *)(p_recv_msg + 1);
	struct whc_api_info *ret_msg;
	u8 *buf;
	u32 buf_len;
	u32 api_id = p_recv_msg->api_id;

	dev_dbg(global_idev.pwhc_dev, "-----DEVICE CALLING API %x START\n", api_id);

	whc_host_internal_event_handle(api_id, param_buf);

	kfree_skb(event_priv->rx_api_msg);

	dev_dbg(global_idev.pwhc_dev, "-----DEVICE CALLING API %x END\n", api_id);

	/* signal dispatch: this API is fully processed; the next WHC_WIFI_EVT_API_CALL
	 * can now overwrite rx_api_msg without overwriting a still-pending message */
	event_priv->rx_api_msg = NULL;

	return;
}

int whc_host_event_init(struct whc_device *idev)
{
	struct event_priv_t	*event_priv = &global_idev.event_priv;

	/* initialize the mutex to send event_priv message. */
	mutex_init(&(event_priv->send_mutex));
	init_completion(&event_priv->api_ret_sema);
	event_priv->b_waiting_for_ret = 0;

	/* initialize event tasklet */
	INIT_WORK(&(event_priv->api_work), whc_host_event_task);

	return 0;
}

void whc_host_event_deinit(void)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;

	/* deinitialize the mutex to send event_priv message. */
	mutex_destroy(&(event_priv->send_mutex));

	complete_release(&event_priv->api_ret_sema);

	return;
}
