#ifndef __WHC_SPI_HOST_H__
#define __WHC_SPI_HOST_H__

#if (KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE)

/* For kernel version higher than 6.6, gpio pins of raspberry pi 5 are numbered differently.
check the correct number of the required GPIO pin to use command "gpioinfo" */

#define DEV_READY_PIN			593
#define RX_REQ_PIN				594
#define DEBUG_PIN				584
#else
#define DEV_READY_PIN			22
#define RX_REQ_PIN				23
#define DEBUG_PIN				13
#endif

#define DEV_READY_IRQ			gpio_to_irq(DEV_READY_PIN)
#define DEV_READY				1
#define DEV_BUSY				0

#define RX_REQ_IRQ				gpio_to_irq(RX_REQ_PIN)
#define DEV_RX_REQ				1
#define DEV_RX_IDLE				0

#undef SPI_DEBUG

#define SIZE_RX_DESC			0
#define SIZE_TX_DESC			0

#define BUF_ALIGN_SZ			4

struct whc_spi {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	struct spi_device *spi_dev;

	s32 bSurpriseRemoved;

	struct semaphore dev_rdy_sema;

	u8 dev_state;

	int (*rx_process_func)(struct sk_buff *pskb);

};

extern struct whc_spi whc_spi_priv;
extern struct hci_ops_t whc_spi_host_intf_ops;

#endif /* __INIC_SPI_H__ */
