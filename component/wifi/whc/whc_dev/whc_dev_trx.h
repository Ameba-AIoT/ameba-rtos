/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __WHC_DEV_TRX_H__
#define __WHC_DEV_TRX_H__

/* -------------------------------- Defines --------------------------------- */
#define whc_dev_tx_done(a)

/* ------------------------------- Data Types ------------------------------- */
/* device tx structure */
struct xmit_priv_t {
	struct __queue xmit_queue; /* xmit queue */
};

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */
extern struct xmit_priv_t dev_xmit_priv;

/* -------------------------- Function declaration -------------------------- */
void whc_dev_xmit_init(void);
void whc_dev_netif_rx(int idx);
void whc_dev_trigger_rx(void);
void whc_dev_send_flowctrl_cmd(u8 fc_state);
struct whc_txbuf_info_t *whc_dev_alloc_buf_info(u8 *buf, u16 len, void *alloc_buf, u8 is_skb);
void whc_dev_free_txbuf(struct whc_txbuf_info_t *buf_info);
void whc_dev_dispatch_event_copy(const u8 *src, u32 size);
void whc_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
#endif /* __WHC_DEV_TRX_H__ */
