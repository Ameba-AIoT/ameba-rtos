#ifndef __WHCH_HOST_ADAPTER_H__
#define __WHCH_HOST_ADAPTER_H__

struct whch_mlme_priv {
	u8					acm_mask;			/* for wmm acm mask */
	u8					user_tx_rate;

	struct timer_list dynamic_timer;

	/* traffic statics */
	u32		NumRxOkInPeriod;
	u32		NumRxUnicastOkInPeriod;
};

struct whch_mlme_info {
	struct addba_request	paddba_req; /* point to adapter->addba_req */
	u8					cur_channel;
	u8					b_preamble_mode;
	u8					total_sta_count_by_port;	/* softap need use */

	u8					b_accept_addba_req;
};

struct whch_ht_priv {
	u8					HT_protection; //dot11 spec
};

struct whch_priv_t {
	/* mlme parameters */
	struct whch_mlme_priv       mlmepriv;
	struct whch_mlme_info		mlmeinfo[WHC_MAX_NET_PORT_NUM];
	struct rtw_chan_def         cur_chandef;	// TODO, current not assign value

	/* ht parameters */
	struct whch_ht_priv			htpriv[WHC_MAX_NET_PORT_NUM];

	/* stainfo parameters */
	struct whch_sta_priv        stapriv[WHC_MAX_NET_PORT_NUM];

	/* tx parameters */
	struct whch_xmit_priv		xmitpriv;
	struct whch_hw_xmit			pending_queue[WHC_MAX_NET_PORT_NUM];

	/* rx parameters */
	struct whch_recv_priv		recvpriv;
	struct semaphore			netif_rx_sema[WHC_MAX_NET_PORT_NUM];

	/* stats parameters */
	struct whch_rx_stats        rx_stats[WHC_MAX_NET_PORT_NUM];

	/* qos parameters */
	struct whch_qos_priv		qospriv[WHC_MAX_NET_PORT_NUM];

	/* security parameters */
	struct whch_security_priv	securitypriv[WHC_MAX_NET_PORT_NUM];
};

#endif