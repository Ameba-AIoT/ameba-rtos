#ifndef __INIC_LINUX_BASE_TYPE__
#define __INIC_LINUX_BASE_TYPE__

#ifdef CONFIG_NAN
#define INIC_MAX_NET_PORT_NUM		(3)
#else
#define INIC_MAX_NET_PORT_NUM		(2)
#endif
#define INIC_STA_PORT			(0)
#define INIC_AP_PORT			(1)

#define INIC_MSG_QUEUE_DEPTH		(50)
#define INIC_MSG_QUEUE_WARNING_DEPTH	(4)

#define RTW_IP_ADDR_LEN 4
#define RTW_IPv6_ADDR_LEN 16

/* Layer 2 structs. */

/* Layer 1 structs. */
struct event_priv_t {
	struct work_struct			api_work; /* event_priv task to haddle event_priv msg */
	struct mutex				send_mutex; /* mutex to protect send host event_priv message */

	struct sk_buff				*rx_api_msg;
	struct sk_buff				*rx_api_ret_msg;

	struct completion			api_ret_sema; /* sema to wait for API calling done */
#ifdef CONFIG_SDIO_BRIDGE
	struct completion			bridge_scan_done_sema; /* sema to wait for scan done */
#endif

	u8					b_waiting_for_ret: 1;
};

struct recv_priv_t {
	struct task_struct			*rx_thread;
	struct semaphore			rx_sema;
	u8					initialized: 1;
};

struct xmit_priv_t {
	struct list_head		queue_head; /* msg queue */
	spinlock_t				lock; /* queue lock */

	atomic_t				msg_num;

	struct task_struct 		*tx_thread;
	struct semaphore 		tx_sema;

	u8 				initialized: 1;
};

/* Scan and Join related parameters. */
struct mlme_priv_t {
	/* scan parameters. */
	struct cfg80211_scan_request	*pscan_req_global;

	/* join parameters. */
	struct internal_join_block_param	*join_block_param;
	unsigned int		rtw_join_status;
	u8				assoc_req_ie[ASSOC_IE_MAX_LEN];
	u8				assoc_rsp_ie[ASSOC_IE_MAX_LEN];
	size_t				assoc_req_ie_len;
	size_t				assoc_rsp_ie_len;
	struct cfg80211_external_auth_params auth_ext_para;

	/* disconnect parameters */
	struct completion	disconnect_done_sema;

	u8					b_in_scan: 1;
	u8					b_in_disconnect: 1;
};

#ifdef CONFIG_P2P
struct p2p_priv_t {
	struct wireless_dev 	*pd_pwdev; /*wdev of P2P Device intf*/
	u8						pd_wlan_idx;
	enum rtw_p2p_role			p2p_role;
	u16 					mgmt_register[3]; /* 0 for P2P_DEV, 1 for P2P_GO, 2 for P2P_GC */
	u8						beacon_p2p_ie_idx;
	u8						beacon_wps_ie_idx;
	u8						roch_onging;
	unsigned int			roch_duration;
	u64						roch_cookie;
	struct ieee80211_channel roch;
};
#endif

struct hci_ops_t {
	void (*send_data)(u8 *buf, u32 len, struct sk_buff *pskb);
	void (*recv_data_process)(void *intf_priv);
};

struct inic_device {
	/* device register to upper layer. */
	struct device				*fullmac_dev;
	struct wiphy				*pwiphy_global;
	struct net_device			*pndev[INIC_MAX_NET_PORT_NUM];
	struct wireless_dev		*pwdev_global[INIC_MAX_NET_PORT_NUM];

	/* ops for upper layer. */
	struct cfg80211_ops		rtw_cfg80211_ops;
	struct ethtool_ops			rtw_ethtool_ops;

	/* Interface related parameters. */
	void					*intf_priv;
	struct hci_ops_t		*intf_ops;

	struct event_priv_t			event_priv;
	struct recv_priv_t			recv_priv;
	struct xmit_priv_t			xmit_priv;

	/* fullmac status management. */
	struct mlme_priv_t			mlme_priv;
	struct net_device_stats	stats[INIC_MAX_NET_PORT_NUM];
	u8						ip_addr[RTW_IP_ADDR_LEN];
	u8						ipv6_addr[RTW_IPv6_ADDR_LEN];
	u8						wowlan_state; /* 0: resume, 1: suspend */
	u8				mp_fw; /* 0: normal, 1: mp */

	/* wifi user config */
	struct  wifi_user_conf	wifi_user_config;

#ifdef CONFIG_P2P
	struct p2p_priv_t		p2p_global;
#endif

};

extern struct inic_device global_idev;

#define MAX_NUM_WLAN_PORT		(2)

#endif /* __INIC_LINUX_BASE_TYPE__ */
