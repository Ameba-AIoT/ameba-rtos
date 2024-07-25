#ifndef __INIC_SPI_H__
#define __INIC_SPI_H__

#define DEV_READY_PIN			22
#define DEV_READY_IRQ			gpio_to_irq(DEV_READY_PIN)
#define RX_REQ_PIN				27
#define RX_REQ_IRQ				gpio_to_irq(RX_REQ_PIN)

#define SIZE_RX_DESC	0
#define SIZE_TX_DESC	0

struct inic_spi {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	struct spi_device *spi_dev;

	s32 bSurpriseRemoved;

	struct semaphore dev_rdy_sema;

	u8 dev_state;

	bool rx_pending;

	int (*rx_process_func)(struct sk_buff *pskb);

};

#endif /* __INIC_SPI_H__ */
