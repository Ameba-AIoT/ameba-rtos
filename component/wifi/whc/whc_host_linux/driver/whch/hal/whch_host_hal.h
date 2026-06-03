#ifndef __WHC_HOST_HAL_H__
#define __WHC_HOST_HAL_H__

#include "whch_host_hal_structs.h"

/************************** SOME DEFINES ABOUT KEYCAM ****************************/
#define KEY_MAX_NUM			32

#define STA_PTK_TRX_KEYCAM_ID		4
#define AP_PTK_TRX_KEYCAM_ID_OFFSET	(STA_PTK_TRX_KEYCAM_ID - 1)
#define AP_GTK_TX_KEYCAM_ID		(KEY_MAX_NUM - 1)
#define NAN_GTK_TX_KEYCAM_ID		(KEY_MAX_NUM - 2)

// CCK Rates, TxHT = 0
#define DESC_RATE1M			0x00
#define DESC_RATE2M			0x01
#define DESC_RATE5_5M		0x02
#define DESC_RATE11M		0x03

// OFDM Rates, TxHT = 0
#define DESC_RATE6M			0x04
#define DESC_RATE9M			0x05
#define DESC_RATE12M		0x06
#define DESC_RATE18M		0x07
#define DESC_RATE24M		0x08
#define DESC_RATE36M		0x09
#define DESC_RATE48M		0x0a
#define DESC_RATE54M		0x0b


enum	_REG_PREAMBLE_MODE {
	PREAMBLE_LONG	= 1,
	PREAMBLE_AUTO	= 2,
	PREAMBLE_SHORT	= 3,
};

/* TX pending queue */
void whc_host_hal_pending_q_init(u8 iface_type);
void whc_host_hal_pending_q_free(int iface_type, struct sta_xmit_priv *psta_xmitpriv);
int  whc_host_hal_pending_q_check(struct hw_xmit *ppending_q);
int  whc_host_hal_pending_q_status(u8 iface_type);
void whc_host_hal_pending_q_enqueue(struct hw_xmit *pending_top, struct sta_xmit_priv *psta_xmitpriv, struct sk_buff *pkt, u32 hw_queue);
void whc_host_hal_pending_q_dequeue(int iface_type);
void whc_host_hal_pending_q_resume(void);

/* TX frame operations */
int  whc_host_hal_xmitframe_coalesce(u8 iface_type, struct sk_buff *pkt, struct xmit_frame *pxmitframe, u8 force_cts2self);
int  whc_host_hal_xmitframe_dump(u8 iface_type, struct xmit_frame *pxmitframe, u8 *wlan_hw_queue);
void whc_host_hal_xmit_check_eapol4(u8 iface_type, struct xmit_frame *pxmitframe);
int  whc_host_hal_xmit(u8 iface_type, struct xmit_frame *pxmitframe, u8 *wlan_hw_queue);
int  whc_host_hal_xmit_thread(void *data);

/* TX descriptor helpers */
u32  whc_host_hal_hwqueue_get(u8 qsel);
int  whc_host_hal_txbd_enough_check(void);
u8   whc_host_hal_txdesc_sc_mapping(u8 bwmode, enum channel_width CurrentChannelBW, u8 pri_ch, u8 central_ch);
u8   whc_host_hal_txdesc_bw_mapping(u8 bwmode, enum channel_width CurrentChannelBW);
u8   whc_host_hal_txdesc_fill_sectype(u8 encrypt, u8 bswenc);
void whc_host_hal_txdesc_fill_vcs(struct txdesc_priv *ptxdesc, u8 vcs_mode, u8 cur_channel, u8 preamble_mode);
void whc_host_hal_txdesc_fill(struct xmit_frame *pxmitframe, u8 *pbuf);
void whc_host_hal_rxdesc_query(struct rx_pkt_attrib *pattrib, u8 *pdesc);

/* RX frame operations */
void whc_host_hal_normal_rx_process(union recv_frame *precvframe, u8 *pphy_info);
int  whc_host_hal_rx_mpdu(struct sk_buff *pskb);
int  whc_host_hal_rx_mpdu_deseg(u8 *rxbd, struct sk_buff *skb, u8 *pdata);

/* Rate conversion */
u16  whc_host_hal_mrate_to_hwrate(u8 rate);

#endif /* __WHC_HOST_HAL_H__ */
