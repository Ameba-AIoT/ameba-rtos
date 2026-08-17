/******************************************************************************
 *
 * whch host-MAC NAN shim (Block A).
 *
 * The whch host-MAC data path was ported from the NP full-MAC driver and its
 * NAN branches still reference NP-only symbols (peer table, is_nan_frame,
 * datalink helpers, wlanhdr/txdesc builders). This header declares host-side
 * counterparts that keep the NP names so future re-sync stays mergeable.
 *
 * Block B fills in the real host NDP peer table: the device (NP) runs NAN NDP
 * negotiation + PASN/pairing and pushes each established/terminated NDP to the
 * host via WHC_API_NAN_NDP_STATUS, mirroring the STA join/disconnect path. This
 * header now describes that live peer table and its lookup helpers.
 *
 ******************************************************************************/
#ifndef _WHCH_HOST_NAN_H_
#define _WHCH_HOST_NAN_H_

#define NAN_PKT_TYPE_NONE	0x0
#define NAN_PKT_TYPE_SDF	0x2
#define NAN_PKT_TYPE_DATA	0x3
#define NAN_PKT_TYPE_NAF	0x4

static inline int IS_IPV6_MCAST(const u8 *da)
{
	return (da[0] == 0x33 && da[1] == 0x33);
}

struct nan_peer_info_t {
	struct list_head	list;
	u8			peer_ndi[ETH_ALEN];		/* peer NAN data interface addr */
	u8			peer_ndl_mac_id;		/* mac_id assigned to the NDL */
	u8			ndp_count;			/* established NDPs to this NDI */
	u8			peer_info_ndl_establish : 1;	/* NDL established */
	u8			peer_info_ndl_renego : 1;	/* NDL re-negotiation in progress */
};

struct whch_host_nan_priv {
	struct {
		u8	mac[ETH_ALEN];		/* NAN management interface (NMI) addr */
		u8	bssid[ETH_ALEN];	/* NAN cluster BSSID (used as addr3) */
	} nmi;

	struct list_head	peer_list;	/* struct nan_peer_info_t */
	spinlock_t		peer_lock;
	u8			enabled;	/* NMI/cluster learned, NAN data path live */
	u8			datalink_cnt;	/* peers with an established NDL */
};

extern struct whch_host_nan_priv g_nanpriv;

u8 whch_host_nan_is_nan_frame(union recv_frame *precvframe);
u8 whch_host_nan_check_datalink_exist(void);
struct nan_peer_info_t *whch_host_nan_peer_info_get_by_ndi(u8 *ndi);
struct nan_peer_info_t *whch_host_nan_peer_info_first_entry_get(u8 only_established);
void whch_host_nan_update_txdesc(struct xmit_frame *pxmitframe, u8 *pbuf);

/* NDP status event handling (device->host, WHC_API_NAN_NDP_STATUS). */
void whch_host_nan_ndp_established(struct rtw_event_nan_ndp_status_info *info);
void whch_host_nan_ndp_terminated(struct rtw_event_nan_ndp_status_info *info);
void whch_host_nan_free_all(void);

#endif /* _WHCH_HOST_NAN_H_ */
