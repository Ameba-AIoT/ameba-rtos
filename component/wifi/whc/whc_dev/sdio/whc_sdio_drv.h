#ifndef _WHC_SDIO_DRV_H_
#define _WHC_SDIO_DRV_H_

#define SPDIO_HOST_RX_BD_NUM	8	//n*2, 2 bd for one transaction
#define SPDIO_HOST_TX_BD_NUM	4

#define DEV_DMA_ALIGN	SPDIO_DMA_ALIGN_4

#define whc_dev_trigger_rx_handle()   whc_sdio_dev_trigger_rx_handle()
#define whc_dev_tx_path_avail       whc_sdio_dev_tx_path_avail
#define whc_dev_send                whc_sdio_dev_send
#define whc_dev_flowctrl(a, b)

#define whc_sdio_dev_event_int_hdl  whc_sdio_dev_pkt_rx

struct whc_sdio_priv_t {
	struct spdio_t dev;

	rtos_mutex_t tx_lock;
	rtos_sema_t rxbd_release_sema;

};

#ifdef WHC_SDIO_USE_GPIO_INT
#define RX_REQ_PIN		_PA_26
#define DEV_RX_REQ		1
#define DEV_RX_IDLE		0

static inline void whc_sdio_dev_set_rxreq_pin(u8 status)
{
#ifdef CONFIG_AMEBAGREEN2
	GPIO_WriteBit(RX_REQ_PIN, status);
#else
	GPIO_WriteBit_Critical(RX_REQ_PIN, status);
#endif
}
#endif

void whc_sdio_dev_send(struct spdio_buf_t *pbuf);
u8 whc_sdio_dev_tx_path_avail(void);
void whc_sdio_dev_device_init(void);
void whc_sdio_dev_send_data(u8 *data, u32 len);
void whc_sdio_dev_trigger_rx_handle(void);

void whc_sdio_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb, u16 size);

#endif

