#ifndef __INIC_SPI_H__
#define __INIC_SPI_H__

#define DEV_READY_PIN			22
#define DEV_READY_IRQ			gpio_to_irq(DEV_READY_PIN)
#define DEV_READY				1
#define DEV_BUSY				0

#define RX_REQ_PIN				23
#define RX_REQ_IRQ				gpio_to_irq(RX_REQ_PIN)
#define DEV_RX_REQ				1
#define DEV_RX_IDLE				0

#undef SPI_DEBUG
#define DEBUG_PIN				13//26

#define SIZE_RX_DESC			0
#define SIZE_TX_DESC			0

struct inic_spi {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	struct spi_device *spi_dev;

	s32 bSurpriseRemoved;

	struct semaphore dev_rdy_sema;

	u8 dev_state;

	int (*rx_process_func)(struct sk_buff *pskb);

};

extern struct inic_spi inic_spi_priv;
extern struct hci_ops_t spi_intf_ops;

#endif /* __INIC_SPI_H__ */
