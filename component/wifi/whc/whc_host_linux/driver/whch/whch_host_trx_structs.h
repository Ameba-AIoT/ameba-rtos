#ifndef __WHCH_HOST_TRX_STRUCTS_H__
#define __WHCH_HOST_TRX_STRUCTS_H__

#ifndef RECVFRAME_HDR_ALIGN
#define RECVFRAME_HDR_ALIGN 128
#endif

/** @defgroup Packet_Defs
   *@{
   */
#define	PACKET_NORMAL			0
#define	PACKET_DHCP				1
#define	PACKET_ARP				2
#define	PACKET_EAPOL			3

#define	nr_xmitframe 2

#define N_BYTE_ALIGMENT(__Value, __Aligment) ((__Aligment == 1) ? (__Value) : (((__Value + __Aligment - 1) / __Aligment) * __Aligment))

#define SN_LESS(a, b)	(((a-b)&0x800)!=0)
#define SN_EQUAL(a, b)	(a == b)

#define NULL_FRAMETAG		(0x0)
#define DATA_FRAMETAG		0x01
#define MGNT_FRAMETAG		0x03

#define DEFRAG_TO	(2000)
#define REORDER_WAIT_TIME	(30) // (ms)

#define RXFRAME_ALIGN				8
#define RXFRAME_ALIGN_SZ			(1<<RXFRAME_ALIGN)

#define WEP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = dot11txpn._byte_.TSC2;\
	pattrib_iv[3] = ((keyidx & 0x3)<<6);\
	dot11txpn.val = (dot11txpn.val == 0xffffff) ? 0: (dot11txpn.val+1);\
}while(0)


#define TKIP_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC1;\
	pattrib_iv[1] = (dot11txpn._byte_.TSC1 | 0x20) & 0x7f;\
	pattrib_iv[2] = dot11txpn._byte_.TSC0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define AES_IV(pattrib_iv, dot11txpn, keyidx)\
do{\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = 0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0: (dot11txpn.val+1);\
}while(0)

#define GCMP_IV(pattrib_iv, dot11txpn, keyidx)\
do {\
	pattrib_iv[0] = dot11txpn._byte_.TSC0;\
	pattrib_iv[1] = dot11txpn._byte_.TSC1;\
	pattrib_iv[2] = 0;\
	pattrib_iv[3] = BIT(5) | ((keyidx & 0x3)<<6);\
	pattrib_iv[4] = dot11txpn._byte_.TSC2;\
	pattrib_iv[5] = dot11txpn._byte_.TSC3;\
	pattrib_iv[6] = dot11txpn._byte_.TSC4;\
	pattrib_iv[7] = dot11txpn._byte_.TSC5;\
	dot11txpn.val = dot11txpn.val == 0xffffffffffffULL ? 0 : (dot11txpn.val+1);\
} while (0)


enum VCS_TYPE {
	NONE_VCS,
	RTS_CTS_BY_HW,
	CTS_TO_SELF_FORCE,
	RTS_CTS_FORCE
};

enum channel_width {
	CHANNEL_WIDTH_20		= 0,
	CHANNEL_WIDTH_40		= 1,
	CHANNEL_WIDTH_80		= 2,
	CHANNEL_WIDTH_160		= 3,
	CHANNEL_WIDTH_80_80	= 4,
	CHANNEL_WIDTH_5		= 5,
	CHANNEL_WIDTH_10	= 6,
	CHANNEL_WIDTH_MAX	= 7,
};

enum chan_offset {
	CHAN_OFFSET_NO_EXT = 0,	/*SCN - no secondary channel*/
	CHAN_OFFSET_UPPER = 1,	/*SCA - secondary channel above*/
	CHAN_OFFSET_NO_DEF = 2,	/*Reserved*/
	CHAN_OFFSET_LOWER = 3,	/*SCB - secondary channel below*/
	CHAN_OFFSET_40M_UPPER = 4,
	CHAN_OFFSET_40M_LOWER = 5,
	CHAN_OFFSET_80M_UPPER = 6,
	CHAN_OFFSET_80M_LOWER = 7,
};

union Keytype {
	u8   skey[16];
	u32  lkey[4];
};

union Keytype_32 {
	u8   skey[32];
	u32  lkey[8];
};

union pn48 {
	u64	val;
	struct {
		u8 TSC0;
		u8 TSC1;
		u8 TSC2;
		u8 TSC3;
		u8 TSC4;
		u8 TSC5;
		u8 TSC6;
		u8 TSC7;
	} _byte_;
};

/**
 *format of ipv6 header
 * |<---4 --->|<---8----->|<------20------->|
 * |--version--|-traffic class-|-----flow label-----|
 * |<--16--->|<----8----->|<--- ----8------>|
 * |--length --|--next header-|------hop limit----|
 * |<----------------128-------------------->|
 * |-----------------source addr---------------|
 * |<-----------------128------------------->|
 * |-----------------dest addr-----------------|
 */
struct ipv6_hdr {
	u32 	v_tc_fl;
	u16	payload_len;
	u8	next_header;
	u8	hop_limit;
	u32	saddr[4];
	u32	daddr[4];
};


struct pkt_file {
	struct sk_buff *pkt;
	size_t pkt_len;	 //the remainder length of the open_file
	u8 *cur_buffer;
	u8 *buf_start;
	u8 *cur_addr;
	size_t buf_len;
};

struct pkt_attrib {
	u8	b_swenc : 1;
	u8	b_qos_en : 1;
	u8	b_tx_order : 1;
	u8	b_fix_rate : 1;
	u8	b_ht_en : 1;
	u8	b_ampdu_en : 1; //tx ampdu enable
	u8	b_mdata : 1; //more data bit
	u8	b_tx_eosp : 1;

	u8	b_tx_raw : 1;
	u8	b_tx_navusehdr : 1;	//NAVUSEHDR
	u8	vcs_mode : 3;	//virtual carrier sense method
	u8	bwmode : 3;

	u8	encrypt : 4;	//when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith
	u8	b_tx_sgi : 4; //short GI

	u8 	agg_num : 5;
	u8 	ampdu_density : 3;

	u32	pktlen;		//the original 802.3 pkt raw_data len (not include ether_hdr data)
	u16	ether_type;
	u16	seqnum;
	u16	hdrlen;		//the WLAN Header Len
	u16	last_txcmdsz;

	u8	pkt_hdrlen;	//the original 802.3 pkt header len
	u8	type_subtype;
	u8	iv_len;
	u8	icv_len;
	u8	iv[18];
	u8	priority;
	u8	ack_policy;
	u8	mac_id;
	u8 	dst[ETH_ALEN];
	u8	src[ETH_ALEN];
	u8	ta[ETH_ALEN];
	u8 	ra[ETH_ALEN];
	u8	key_idx;
//	u8	raid;//rate adpative id
	u8	ch_offset;//PRIME_CHNL_OFFSET
	u8	qsel;
	u8	rate;
	u8 	retry_limit_by_user; //for user control retry limit
	u8 	tx_done_id;
	u8  pkt_type;        // NORMAL/DHCP/EAPOL/ARP
	u8  issue_addba;

	u8 nan_pkt_type;
};

struct xmit_frame {
	struct list_head	list;
	struct pkt_attrib	attrib;
	struct sk_buff		*pkt;
	int			        frame_tag;
	u8 					iface_type;
	u8 					b_need_polling : 1;
};

struct whch_xmit_priv {
	spinlock_t			mutex;

	struct task_struct 	*hal_tx_thread;
	struct semaphore 	hal_tx_sema;

	struct list_head	free_xmit_queue; /* free_xmit_queue queue */
	spinlock_t			free_xmit_lock; /* free_xmit_queue lock */

	u8					*pallocated_frame_buf;
	u8					*pxmit_frame_buf;

	atomic_t			free_xmitframe_cnt;
};

struct hw_xmit {
	struct list_head	sta_queue; /* sta_queue queue */
	spinlock_t			sta_queue_lock; /* sta_queue lock */

	atomic_t			accnt;
};

struct whch_hw_xmit {
	struct hw_xmit		hwxmits[4];	/* 0(VO), 1(VI), 2(BE), 3(BK) */
};

struct whch_qos_priv	{
	u32					qos_option;			/* bit mask option: u-apsd, s-apsd, ts, block ack... */
};

struct rx_pkt_attrib {
	u8	b_physt_valid : 1;
	u8 	b_amsdu : 1;
	u8	b_qos : 1;
	u8	b_mdata : 1;
	u8	b_mfrag : 1;
	u8	b_privacy : 1; //in frame_ctrl field
	u8	b_decrypted : 1;
	u8	b_bc : 1;
	u8	b_order : 1 ;
	u8	b_crc_err  : 1;
	u8	b_icv_err  : 1;
	u8	b_ap_bcmc_frame  : 1;
	u8	shift_sz : 2;
	u8	encrypt : 4; //when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith
	u8 	key_index : 4;
	u8 	sgi : 4;
	u8 	pkt_rpt_type : 4;
	u8 	A1_matched : 4;
	u8	b_wtn_bcn : 1;
	u8	b_wtn_partial_bcn : 1;

	u16	seq_num;
	u16	pkt_len;
	u16	eth_type;
	u16	data_rate;

	u8	drvinfo_sz;
	u8	hdrlen; //the WLAN Header Len
	u8 	to_fr_ds;
	u8	priority;
	u8	frag_num;
	u8	iv_len;
	u8	icv_len;
	u8 	dst[ETH_ALEN];
	u8 	src[ETH_ALEN];
	u8 	ta[ETH_ALEN];
	u8 	ra[ETH_ALEN];
	u8 	bssid[ETH_ALEN];
	u8 	channel;
};

struct recv_frame_hdr {
	struct list_head	list;

	struct sk_buff	 *pkt;

	struct rx_pkt_attrib attrib;

	u32  len;
	u8 *rx_head;
	u8 *rx_data;
	u8 *rx_tail;
	u8 *rx_end;

	//struct sta_mlme_priv *psta_mlmepriv;
	struct sta_info *psta;

	u8 iface_type;
};

union recv_frame {
	union {
		struct list_head list;
		struct recv_frame_hdr hdr;
		uint mem[RECVFRAME_HDR_ALIGN >> 3];
	} u;
};

enum _RX_PACKET_TYPE {
	NORMAL_RX = 0,//Normal rx packet
	CH_INFO_NDPA = 1,//Channel info trigger by NDPA or normal pkt
	CH_INFO_RSP = 2,//Channel info trigger by response pkt
	RADAR_RPT = 3,//radar rpt
	PPDU_STATUS = 4,// phystatus
	C2H_PACKET = 5
};

struct whch_recv_priv {
	struct	list_head	free_recv_queue;
	spinlock_t			free_recv_lock;
	u32					free_recvframe_cnt;

	struct amsdu_priv_t	*amsdu_priv;

	u8					*pallocated_frame_buf;
};

struct whch_rx_stats {
	u32	rx_bytes_in2s;				/*!< total bytes received         */
	u32 rx_byte_uni_in2s;			/*halbb just print for debug*/
	u32 rx_packets;					/*!< total packets received       */

	u16 cur_rx_data_rate; // current rx data rate
	u8 b_is_any_non_be_pkts: 1;
};
#endif /* __WHCH_HOST_TRX_STRUCTS_H__ */
