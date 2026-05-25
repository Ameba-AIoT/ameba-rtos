#ifndef _WHC_SDIO_DEV_H_
#define _WHC_SDIO_DEV_H_

#define SPDIO_HOST_RX_BD_NUM	8	//n*2, 2 bd for one transaction
#define SPDIO_HOST_TX_BD_NUM	4

#define DEV_DMA_ALIGN	SPDIO_DMA_ALIGN_4

#define whc_dev_intf_init                whc_sdio_dev_init
#define whc_dev_bus_is_idle              whc_sdio_dev_bus_is_idle
#define whc_dev_send                     whc_sdio_dev_send

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

void whc_sdio_dev_init(void);
u8 whc_sdio_dev_bus_is_idle(void);
void whc_sdio_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);

#endif

