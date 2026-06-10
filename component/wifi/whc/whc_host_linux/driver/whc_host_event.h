#ifndef __WHC_HOST_EVENT_H__
#define __WHC_HOST_EVENT_H__

#define RTW_JOIN_TIMEOUT		20000
#define RTW_SCAN_TIMEOUT		12000
#define RTW_SCAN_ABORT_TIMEOUT	20
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

#ifdef CONFIG_P2P
#define rtw_p2p_frame_is_registered(p2p_role, frame_type) \
	global_idev.p2p_global.mgmt_register[p2p_role - 1] & BIT(frame_type >> 4)
#endif

/**
 * @brief  The enumeration is join block param.
 */
struct internal_block_param {
	struct completion	sema;
};

struct whc_event_entry {
	u32 api_id;
	u8(*handler)(u32 *param_buf);
};

u8 whc_host_internal_event_handle(u32 api_id, u32 *param_buf);

void whc_host_join_status_hdl(u32 event, char *evt_info);
u8 whc_host_scan_user_callback_hdl(u32 *param_buf);
void whc_host_acs_info_hdl(struct rtw_acs_mntr_rpt *acs_rpt);
void whc_host_set_netif_info_hdl(int idx, unsigned char *dev_addr);
u8 whc_host_get_network_info_hdl(uint32_t type, int idx, void *out_buf, uint32_t *rsp_len);

u8 whc_host_event_scan_report_indicate(u32 *param_buf);
u8 whc_host_event_update_regd_indicate(u32 *param_buf);
u8 whc_host_event_set_acs_info(u32 *param_buf);
u8 whc_host_event_join_status_indicate(u32 *param_buf);
u8 whc_host_event_set_netif_info(u32 *param_buf);
u8 whc_host_event_get_network_info(u32 *param_buf);
u8 whc_host_event_promisc_pkt_hdl(u32 *param_buf);
#ifdef CONFIG_NAN
u8 whc_host_event_nan_match_indicate(u32 *param_buf);
u8 whc_host_event_nan_cfgvendor_event_indicate(u32 *param_buf);
u8 whc_host_event_nan_cfgvendor_cmd_reply(u32 *param_buf);
#endif

#endif //__WHC_HOST_EVENT_H__
