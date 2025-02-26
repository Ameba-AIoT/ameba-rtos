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
	rtos_sema_t xmit_sema; /* tx sema */
	u32 tx_bytes; /* xmit bytes */
	u32 tx_pkts; /* xmit number of packets */

	struct __queue xmit_queue; /* xmit queue */
};

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Private Variables ---------------------------- */
extern struct xmit_priv_t dev_xmit_priv;

/* -------------------------- Function declaration -------------------------- */
void whc_dev_init_priv(void);
void whc_dev_recv(int idx);
void whc_dev_tx_done(int idx);

#endif /* __INIC_DEV_TX_H__ */
