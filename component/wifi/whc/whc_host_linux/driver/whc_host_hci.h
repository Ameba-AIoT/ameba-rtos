// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_HOST_HCI_H__
#define __WHC_HOST_HCI_H__

#define WHC_STA_PORT			(0)
#define WHC_AP_PORT			(1)
#define WHC_NAN_PORT			(2)

#define MAX_NUM_WLAN_PORT		(2)
#define IPC_USER_POINT			0
#define IPC_MSG_QUEUE_DEPTH		(50)
#define IPC_MSG_QUEUE_WARNING_DEPTH	(4)

#define RTW_IP_ADDR_LEN 4
#define RTW_IPv6_ADDR_LEN 16

#ifdef CONFIG_WHC_HCI_IPC
/* for ipc intf */
struct event_priv_t {
	struct work_struct		api_work; /* event_priv work to haddle event_priv msg */
	struct workqueue_struct *api_workqueue; /* event_priv work queue to haddle event_priv msg */
	ipc_msg_struct_t		api_ipc_msg; /* to store ipc msg for event_priv */
	struct mutex			iiha_send_mutex; /* mutex to protect send host event_priv message */
	struct whc_ipc_host_req_msg	*preq_msg;/* host event_priv message to send to device */
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
	struct whc_ipc_ex_msg		*p_inic_ipc_msg;/* host inic ipc message to send to device */
	dma_addr_t			ipc_msg_phy_addr;/* host inic message to send to device */
	spinlock_t			ipc_send_msg_lock;
};

struct xmit_priv_t {
	struct dev_sk_buff		*host_skb_buff;
	dma_addr_t			host_skb_buff_phy;
	dma_addr_t			host_skb_data_phy;
	u32			skb_buf_max_size;
	atomic_t				skb_free_num;
	u32				skb_idx;
	spinlock_t			skb_lock;
};

#else
/* for non-ipc intf */
struct event_priv_t {
	struct work_struct			api_work; /* event_priv task to haddle event_priv msg */
	struct mutex				send_mutex; /* mutex to protect send host event_priv message */

	struct sk_buff				*rx_api_msg;
	struct sk_buff				*rx_api_ret_msg;

	struct completion			api_ret_sema; /* sema to wait for API calling done */

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
	u8				flowctrl_en: 1;
};

struct hci_ops_t {
	void (*send_data)(u8 *buf, u32 len, struct sk_buff *pskb);
	void (*recv_data)(void *intf_priv);
};
#endif

/* Scan and Join related parameters. */
struct mlme_priv_t {
	/* scan parameters. */
	struct cfg80211_scan_request	*pscan_req_global;

	/* join parameters. */
	struct internal_block_param	*join_block_param;
	struct internal_block_param	*scan_block_param;
	struct internal_block_param	*scan_abort_block_param;
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
	u8					b_in_linking: 1;	// 1: represent sta_linking
};

#ifdef CONFIG_SUPPLICANT_SME
struct sme_priv_t {
	enum nl80211_auth_type	auth_type;
	u8						wep_key[13];
	u8 						wep_key_len, wep_key_idx;

	struct cfg80211_bss 	*cfg80211_assoc_bss;

	u8						*deauth_ies;
	u32						deauth_ie_len;
	bool					deauth_from_wpas;
};
#endif

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

struct netdev_work {
	struct work_struct work;
	int op;  // 0: NETDEV_REGISTER, 1: NETDEV_UNREGISTER
};
#endif

struct whc_device {
	/* device register to upper layer. */
	struct device			*pwhc_dev;
	struct wiphy			*pwiphy_global;
	struct net_device		*pndev[WHC_MAX_NET_PORT_NUM];
	struct wireless_dev		*pwdev_global[WHC_MAX_NET_PORT_NUM];

	/* ops for upper layer. */
	struct cfg80211_ops		rtw_cfg80211_ops;
	struct ethtool_ops		rtw_ethtool_ops;

	/* Interface related parameters. */
#ifdef CONFIG_WHC_HCI_IPC
	/* for ipc intf */
	struct device			*ipc_dev;
	aipc_ch_t			*data_ch;
	aipc_ch_t			*event_ch;
	struct ipc_msg_q_priv		msg_priv;
#else
	/* for non-ipc intf */
	void					*intf_priv;
	struct hci_ops_t		*intf_ops;
	struct recv_priv_t			recv_priv;
#endif

	struct event_priv_t		event_priv;
	struct xmit_priv_t		xmit_priv;

	/* whc status management. */
	struct mlme_priv_t			mlme_priv;
	struct net_device_stats	stats[WHC_MAX_NET_PORT_NUM];
	u8						ip_addr[RTW_IP_ADDR_LEN];
	u8						ipv6_addr[RTW_IPv6_ADDR_LEN];
	u8						wowlan_state; /* 0: resume, 1: suspend */
	u8						is_need_4way[WHC_MAX_NET_PORT_NUM]; /* 0: no need, 1: need */
	u8						is_4way_ongoing[WHC_MAX_NET_PORT_NUM]; /* 0: 4-way is not ongoing, 1: 4-way is going */
	u8 						bssid[ETH_ALEN];
	bool b_in_roaming;

	/* wifi user config */
	struct  wifi_user_conf	wifi_user_config;

#ifdef CONFIG_P2P
	struct p2p_priv_t		p2p_global;
#endif

#ifdef CONFIG_SUPPLICANT_SME
	struct sme_priv_t		sme_priv;
#endif

#ifdef CONFIG_WHCH
	struct whch_priv_t		whchpriv;
#endif

	u8 host_init_done: 1;
	u8 ipv6_addr_updated: 1;
};

extern struct whc_device global_idev;

#endif /* __WHC_HOST_HCI_H__ */
