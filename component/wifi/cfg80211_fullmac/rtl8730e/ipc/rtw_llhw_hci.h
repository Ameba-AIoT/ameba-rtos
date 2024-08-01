// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __INIC_LINUX_BASE_TYPE__
#define __INIC_LINUX_BASE_TYPE__

#ifdef CONFIG_NAN
#define INIC_MAX_NET_PORT_NUM		(3)
#else
#define INIC_MAX_NET_PORT_NUM		(2)
#endif
#define INIC_STA_PORT			(0)
#define INIC_AP_PORT			(1)

#define IPC_MSG_QUEUE_DEPTH		(50)
#define IPC_MSG_QUEUE_WARNING_DEPTH	(4)

#define RTW_IP_ADDR_LEN 4
#define RTW_IPv6_ADDR_LEN 16

/* Layer 2 structs. */

/* Layer 1 structs. */
struct event_priv_t {
	struct tasklet_struct		api_tasklet; /* event_priv task to haddle event_priv msg */
	ipc_msg_struct_t		api_ipc_msg; /* to store ipc msg for event_priv */
	struct mutex			iiha_send_mutex; /* mutex to protect send host event_priv message */
	struct inic_ipc_host_req_msg	*preq_msg;/* host event_priv message to send to device */
	dma_addr_t			req_msg_phy_addr;/* host event_priv message to send to device */
	uint32_t			*dev_req_network_info;
	dma_addr_t			dev_req_network_info_phy;
	ipc_msg_struct_t		recv_ipc_msg;
};

struct ipc_msg_q_priv {
	struct list_head		queue_head; /* msg queue */
	spinlock_t			lock; /* queue lock */
	struct work_struct		msg_work; /* message task in linux */
	struct mutex			msg_work_lock; /* tx lock lock */
	void	(*task_hdl)(u8 event_num, u32 msg_addr, u8 wlan_idx);    /* the haddle function of task */
	bool				b_queue_working; /* flag to notice the queue is working */
	struct ipc_msg_node		ipc_msg_pool[IPC_MSG_QUEUE_DEPTH]; /* static pool for queue node */
	int				queue_free; /* the free size of queue */
	struct inic_ipc_ex_msg		*p_inic_ipc_msg;/* host inic ipc message to send to device */
	dma_addr_t			ipc_msg_phy_addr;/* host inic message to send to device */
	spinlock_t			ipc_send_msg_lock;
};

struct xmit_priv_t {
	struct dev_sk_buff		*host_skb_buff;
	dma_addr_t			host_skb_buff_phy;
	atomic_t				skb_free_num;
	u32				skb_idx;
	spinlock_t			skb_lock;
};

/* Scan and Join related parameters. */
struct mlme_priv_t {
	/* scan parameters. */
	struct cfg80211_scan_request	*pscan_req_global;
	bool b_in_scan;

	/* join parameters. */
	struct internal_join_block_param	*join_block_param;
	unsigned int		rtw_join_status;
	u8				assoc_req_ie[ASSOC_IE_MAX_LEN];
	u8				assoc_rsp_ie[ASSOC_IE_MAX_LEN];
	size_t				assoc_req_ie_len;
	size_t				assoc_rsp_ie_len;
	struct cfg80211_external_auth_params auth_ext_para;

	/* disconnect parameters */
	bool b_in_disconnect;
	struct completion	disconnect_done_sema;
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

struct inic_device {
	/* device register to upper layer. */
	struct device			*fullmac_dev;
	struct wiphy			*pwiphy_global;
	struct net_device		*pndev[INIC_MAX_NET_PORT_NUM];
	struct wireless_dev		*pwdev_global[INIC_MAX_NET_PORT_NUM];

	/* ops for upper layer. */
	struct cfg80211_ops		rtw_cfg80211_ops;
	struct ethtool_ops		rtw_ethtool_ops;

	/* IPC related parameters. */
	struct device			*ipc_dev;
	aipc_ch_t			*data_ch;
	aipc_ch_t			*event_ch;
	struct event_priv_t		event_priv;
	struct ipc_msg_q_priv		msg_priv;
	struct xmit_priv_t		xmit_priv;

	/* fullmac status management. */
	struct mlme_priv_t		mlme_priv;
	struct net_device_stats		stats[INIC_MAX_NET_PORT_NUM];
	u8				ip_addr[RTW_IP_ADDR_LEN];
	u8				ipv6_addr[RTW_IPv6_ADDR_LEN];
	u8				wowlan_state; /* 0: resume, 1: suspend */
	u8				mp_fw; /* 0: normal, 1: mp */

	/* wifi user config */
	struct  wifi_user_conf	wifi_user_config;
#ifdef CONFIG_P2P
	struct p2p_priv_t		p2p_global;
#endif
};

extern struct inic_device global_idev;

#define MAX_NUM_WLAN_PORT		(2)
#define IPC_USER_POINT			0

#endif /* __INIC_LINUX_BASE_TYPE__ */
