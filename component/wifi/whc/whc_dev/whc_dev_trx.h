/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __WHC_DEV_TX_H__
#define __WHC_DEV_TX_H__

/* -------------------------------- Defines --------------------------------- */
#define WHC_MSG_QUEUE_WARNING_DEPTH (4)

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
void whc_dev_tx_done(int idx);
void whc_dev_trigger_rx(void);
void whc_dev_send_flowctrl_cmd(u8 fc_state);
struct whc_txbuf_info_t *whc_dev_alloc_buf_info(u8 *buf, u16 len, void *alloc_buf, u8 is_skb);
#endif /* __INIC_DEV_TX_H__ */
