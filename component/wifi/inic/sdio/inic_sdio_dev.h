#ifndef _INIC_SDIO_DEV_H_
#define _INIC_SDIO_DEV_H_

#define SPDIO_TX_BD_NUM	8	//n*2, 2 bd for one transaction
#define SPDIO_RX_BD_NUM	4

#define DEV_DMA_ALIGN	SPDIO_DMA_ALIGN_4

struct inic_sdio_priv_t {
	struct spdio_t dev;

	rtos_mutex_t tx_lock;
	rtos_sema_t rxbd_release_sema;

};

void inic_dev_init(void);
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
void inic_dev_send(struct spdio_buf_t *pbuf);
u8 inic_dev_tx_path_avail(void);

#endif

