/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __WHC_DEV_TX_H__
#define __WHC_DEV_TX_H__

/* -------------------------------- Defines --------------------------------- */
#define WHC_MSG_QUEUE_WARNING_DEPTH (4)

#define WHC_FLOWCTRL_LOW_THRESHOLD		(3 + 1)  // 3 skb reserved for wifi rx in driver
#define WHC_FLOWCTRL_HIGH_THRESHOLD		(3 + 2)

#define whc_dev_tx_done(a)

/* ------------------------------- Data Types ------------------------------- */
/* device tx structure */
struct xmit_priv_t {
	struct __queue xmit_queue; /* xmit queue */

	u8 flowctrl_en: 1;
};

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */
extern struct xmit_priv_t dev_xmit_priv;

/* -------------------------- Function declaration -------------------------- */
void whc_dev_xmit_init(void);
void whc_dev_netif_rx(int idx);
void whc_dev_flowctrl(u8 *status, u8 send_cmd);
struct whc_txbuf_info_t *whc_dev_alloc_buf_info(u8 *buf, u16 len, void *alloc_buf, u8 is_skb);
void whc_dev_free_txbuf(struct whc_txbuf_info_t *buf_info);
void whc_dev_dispatch_event_copy(const u8 *src, u32 size);
void whc_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
#endif /* __INIC_DEV_TX_H__ */
