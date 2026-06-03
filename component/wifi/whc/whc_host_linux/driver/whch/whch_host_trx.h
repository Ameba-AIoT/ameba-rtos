#ifndef __WHCH_HOST_TRX_H__
#define __WHCH_HOST_TRX_H__

/* TX frame lifecycle */
int  whc_host_xmit_priv_init(void);
void whc_host_xmit_priv_free(void);
struct xmit_frame *whc_host_xmitframe_alloc(u8 iface_type);
int  whc_host_xmitframe_free(struct xmit_frame *pxmitframe);
void whc_host_xmit_open_pktfile(struct sk_buff *pktptr, struct pkt_file *pfile);
u32  whc_host_xmit_read_pktfile(struct pkt_file *pfile, u8 *rmem, u32 rlen);

/* TX frame construction */
int  whc_host_xmit_make_wlanhdr(u8 iface_type, u8 *hdr, struct pkt_attrib *pattrib);
s32  whc_host_xmit_put_snap(u8 *data, u16 h_proto);
void whc_host_xmit_update_attrib_vcs_info(u8 iface_type, struct xmit_frame *pxmitframe, struct sta_mlme_priv *psta_mlmepriv, u8 f_cts2self);
void whc_host_xmit_update_attrib_set_qos(struct pkt_file *ppktfile, struct pkt_attrib *pattrib);
u8   whc_host_xmit_update_attrib_qos_acm(u8 acm_mask, u8 priority);
int  whc_host_xmit_update_attrib_sec_info(struct security_priv *psecuritypriv, struct pkt_attrib *pattrib, struct sta_security_priv *psta_security);
void whc_host_xmit_update_attrib_phy_info(struct pkt_attrib *pattrib, struct sta_mlme_priv *psta_mlmepriv, struct sta_ht_priv *psta_htpriv, u8 bw_40_en);
int  whc_host_xmit_update_attrib(u8 iface_type, struct sk_buff *pkt, struct pkt_attrib *pattrib);

/* TX encryption */
int  whc_host_xmit_tkip_addmic(struct security_priv *psecuritypriv, struct sk_buff *pkt, struct sta_security_priv *psta_security, u16 pkt_len);
int  whc_host_xmit_enc_software(u8 iface_type, struct security_priv *psecuritypriv, struct xmit_frame *pxmitframe);

/* TX entry */
int  whc_host_xmit_entry_prehandle(int idx, struct sk_buff *pskb, u8 *wlan_hw_queue);

/* MLME priv init / deinit */
void whc_host_mlme_priv_init(void);
void whc_host_mlme_priv_deinit(void);
void whc_host_dynamic_timer_hdl(struct timer_list *t);

/* RX priv init / free */
int  whc_host_recv_priv_init(void);
void whc_host_recv_priv_free(void);

/* RX frame alloc / pointer helpers */
union recv_frame *whc_host_recv_alloc_frame(void);
int  whc_host_recv_free_frame(union recv_frame *precvframe);
u8  *whc_host_recv_recvframe_put(union recv_frame *precvframe, signed int sz);
u8  *whc_host_recv_recvframe_pull(union recv_frame *precvframe, signed int sz);
u8  *whc_host_recv_recvframe_pull_tail(union recv_frame *precvframe, signed int sz);

/* RX frame validation & processing */
void whc_host_recv_count_rx_stats(struct whch_rx_stats *pstats, union recv_frame *prframe, struct sta_mlme_priv *psta_mlmepriv);
struct sta_info *whc_host_recv_sta2sta_data_frame(u8 iface_type, union recv_frame *precv_frame);
struct sta_info *whc_host_recv_ap2sta_data_frame(u8 iface_type, union recv_frame *precv_frame);
struct sta_info *whc_host_recv_sta2ap_data_frame(u8 iface_type, union recv_frame *precv_frame);
int  whc_host_recv_validate_data_frame(u8 iface_type, union recv_frame *precv_frame);
int  whc_host_recv_validate_frame(u8 iface_type, union recv_frame **pprecv_frame);
int  whc_host_recv_func_prehandle(u8 iface_type, union recv_frame *rframe);
int  whc_host_recv_func_posthandle(u8 iface_type, union recv_frame *prframe);
int  whc_host_recv_entry(union recv_frame *precvframe);

/* RX defrag */
void whc_host_defrag_ctrl_init(struct recv_defrag_ctrl *pdefrag_ctrl);
void whc_host_defrag_ctrl_deinit(struct recv_defrag_ctrl *pdefrag_ctrl);
void whc_host_defrag_timer_handler(struct timer_list *t);
union recv_frame *whc_host_recv_frame_check_defrag(u8 iface_type, struct security_priv *psecuritypriv, union recv_frame *precv_frame);

/* RX reorder */
int  whc_host_recv_check_indicate_seq(u16 *indicate_seq, u16 wsize, u16 wend, u16 seq_num);
int  whc_host_recv_reorder(void *preorder_per_tid);
int  whc_host_recv_reorder_dequeue(struct recv_reorder_ctrl *preorder_ctrl, int bforced);
void whc_host_recv_reorder_timer_hdl(struct timer_list *t);
struct recv_reorder_ctrl *whc_host_recv_reorder_alloc(u8 ap_compatibilty, u8 iface_type);
void whc_host_recv_reorder_free(struct recv_reorder_ctrl **ppreorder_ctrl);

/* RX security */
signed int whc_host_recv_check_pn(u8 fragnum, union pn48 *rxPN, union pn48 *pCurRxPN);
int  whc_host_recv_frame_check_frag_pn(struct security_priv *psecuritypriv, union recv_frame *precvframe, u8 fragnum);
union recv_frame *whc_host_recv_decryptor(u8 iface_type, struct security_priv *psecuritypriv, union recv_frame *precv_frame);

/* RX indication */
void whc_host_recv_wlanhdr_to_ethhdr(union recv_frame *precvframe);
void whc_host_if_netif_rx(struct sk_buff *skb, void *preorder_ctrl);
int  whc_host_recv_indicatepkt(u8 iface_type, union recv_frame *precv_frame);
int  whc_host_recv_process_indicatepkts(u8 iface_type, union recv_frame *prframe);
void whc_host_netif_rx(struct sk_buff *pskb, u8 wlan_idx);

/* ADDBA / block-ack */
int  whc_host_on_action_block_ack(struct rtw_event_addba_nego *nego_info);

#endif /* __WHCH_HOST_TRX_H__ */
