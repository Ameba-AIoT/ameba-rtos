#ifndef __RTW_LLHW_EVENT_H__
#define __RTW_LLHW_EVENT_H__

#define RTW_JOIN_TIMEOUT		20000
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

/**
 * @brief  The enumeration is join block param.
 */
struct internal_join_block_param {
	struct completion	join_sema;
	unsigned int		join_timeout;
	unsigned char		block;
};

#ifdef CONFIG_NAN
struct rtw_nan_func_info_t {
	u8 type;
	u8 service_id[NL80211_NAN_FUNC_SERVICE_ID_LEN];
	u8 publish_type;
	bool close_range;
	bool publish_bcast;
	bool subscribe_active;
	u8 followup_id;
	u8 followup_reqid;
	struct mac_address followup_dest;
	u32 ttl;
	const u8 *serv_spec_info;
	u8 serv_spec_info_len;
	bool srf_include;
	const u8 *srf_bf;
	u8 srf_bf_len;
	u8 srf_bf_idx;
	struct mac_address *srf_macs;
	int srf_num_macs;
	struct cfg80211_nan_func_filter *rx_filters;
	struct cfg80211_nan_func_filter *tx_filters;
	u8 num_tx_filters;
	u8 num_rx_filters;
	u8 instance_id;
	u64 cookie;
};
#endif

#endif //__RTW_LLHW_EVENT_H__
