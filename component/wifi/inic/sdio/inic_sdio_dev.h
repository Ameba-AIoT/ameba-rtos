#ifndef _INIC_SDIO_DEV_H_
#define _INIC_SDIO_DEV_H_

#define SPDIO_TX_BD_NUM	8	//n*2, 2 bd for one transaction
#define SPDIO_RX_BD_NUM	4

#define SPDIO_RX_BUFSZ	(SPDIO_RX_BUFSZ_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct inic_msg_info) + sizeof(INIC_TX_DESC))) //n*64, must be rounded to 64
#define SPDIO_SKB_RSVD_LEN		N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct inic_msg_info) - sizeof(INIC_TX_DESC), SPDIO_DMA_ALIGN_4)

struct sdio_tx_info_t {
	struct spdio_buf_t txbuf_info;
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb;	/* the original buffer is skb or not */
};

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

