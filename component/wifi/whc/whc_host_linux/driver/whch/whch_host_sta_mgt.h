#ifndef __WHC_HOST_STAINFO_H__
#define __WHC_HOST_STAINFO_H__

#define MAXTID	16

struct asoc_cap_t {
	u8 htc_rx: 1;
};

struct sta_mlme_priv {
	struct asoc_cap_t		asoc_cap;

	u8      stainfo_macid;
	u8      stainfo_mac_addr[ETH_ALEN];
	u8		sta_state;

	u32		stainfo_rx_byte_uni_in2s;
	u16		stainfo_rx_data_pkts_in2s;

	u8      tx_agg_num : 5;
	u8      tx_ampdu_density : 3;

	u8		b_sta_qos_option : 1;
	u8		b_erp_protection : 1;
};

struct sta_security_priv {
	union Keytype	dot11tkiptxmickey;
	union Keytype	dot11tkiprxmickey;
	union Keytype_32		dot118021x_UncstKey;
	union pn48		dot11txpn;			/* PN48 used for Unicast xmit. */

	u32				dot11_security_privacy;		/* aes, tkip... */

	u8				b_ieee8021x_blocked : 1;		/* 0: allowed, 1:blocked */
	u8				b_pairwise_key_installed : 1;
	u8				gtk_bmp;
	union Keytype_32		gtk;
};

struct pending_sta_t {
	struct list_head	list;

	struct list_head	frame_queue; /* queue head */
	spinlock_t			frame_queue_lock; /* queue lock */

	atomic_t			qcnt;
};

struct sta_xmit_priv {
	struct pending_sta_t	sta_pending_be_q;	/* priority == 0,3 */
	struct pending_sta_t	sta_pending_bk_q;	/* priority == 1,2 */
	struct pending_sta_t	sta_pending_vi_q;	/* priority == 4,5 */
	struct pending_sta_t	sta_pending_vo_q;	/* priority == 6,7 */
};

struct sta_ht_priv {
	u8					ht_option;
	u8					he_option;

	u8					bwmode;
	u8					ch_offset;
	u8					sgi_20m;			/* short GI */
	u8					sgi_40m;
	u8					ampdu_enable;

	u8					addba_success_bitmap;
};

struct reorder_node {
	struct list_head	list;
	struct sk_buff	*skb;
	u16	seq_num;
};

//for Rx reordering buffer control
struct recv_reorder_ctrl {
	struct	list_head	pending_recvframe_queue;
	spinlock_t			pending_recvframe_lock;

	struct timer_list reordering_ctrl_timer;

	struct reorder_node *node_pool;		/* array of pool_size elements */
	struct list_head	node_free_list;	/* free nodes ready for reuse */
	u16				pool_size;		/* = wsize_b when pool is allocated */
	struct reorder_node *new_node;

	u16 indicate_seq;//=wstart_b, init_value=0xffff
	u16 wend_b;
	u16 wsize_b;

	u8 iface_type;
	u8 enable;
	u8 bReorderWaiting;
	unsigned char ap_compatibilty_enabled;// BIT0-BIT3 rx counter, BIT4 compatibilty
};

struct recv_defrag_ctrl {
	struct timer_list		defrag_timer; 	/* for free defrag memory */
	u8 *pfragbuf;
	u32 len;
	u8 latest_frag_num;
	u64 latest_pn;
};

struct sta_recv_priv {
	struct recv_reorder_ctrl	recvreorder_ctrl[MAXTID];	/* for A-MPDU Rx reordering buffer control */
	struct recv_defrag_ctrl		defrag_ctrl;

	u16 tid_rxseq[16];
	u8 iv[16][8];
};


struct sta_info {
	struct list_head        	list;				/* sta_list */

	struct sta_security_priv	sta_security;
	struct sta_mlme_priv    	sta_mlmepriv;
	struct sta_xmit_priv		sta_xmitpriv;
	struct sta_ht_priv			sta_htpriv;
	struct sta_recv_priv		sta_recvpriv;
};

struct whch_sta_priv {
	struct list_head  	sta_list;
	spinlock_t		    sta_list_mutex;
};

struct rtw_chan_def {
	enum rtw_band_type band; /* protocol -2.4G,5G,6G*/
	enum channel_width bw;
	enum chan_offset offset;/*used as offset of 40MHz, g6 driver may also used as offset of primary channel*/
	u8 chan; /*primary channel*/
	u8 center_ch;
};


/* STA lifecycle */
void whc_host_sta_init(u8 iface_type);
void _whc_host_sta_init_stainfo(struct sta_info *psta);
struct sta_info *whc_host_sta_alloc_stainfo(u8 iface_type, u8 *hwaddr);
void whc_host_sta_update_stainfo(u8 iface_type, u8 *hwaddr, struct rtw_event_sta_info *pstainfo, struct rtw_event_security_priv *psecinfo);
int  whc_host_sta_free_stainfo(u8 iface_type, u8 *hwaddr);
void whc_host_sta_free_resource(u8 wlan_idx);

/* STA query */
struct sta_info *whc_host_sta_get_stainfo(u8 iface_type, u8 *hwaddr);
struct pending_sta_t *whc_host_sta_get_sta_pending(struct sta_xmit_priv *psta_xmitpriv, int hw_queue);

/* STA state / MLME */
int  whc_host_state_check_ap_client_assoc_success(struct sta_mlme_priv *psta_mlmepriv);
int  whc_host_check_sta_associated_to_ap(void);
int  whc_host_init_default_value(u8 iface_type);

#endif /* __WHC_HOST_STAINFO_H__ */
