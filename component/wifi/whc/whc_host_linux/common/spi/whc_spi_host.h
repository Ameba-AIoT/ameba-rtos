#ifndef __WHC_SPI_HOST_H__
#define __WHC_SPI_HOST_H__

#if (KERNEL_VERSION(6, 12, 0) <= LINUX_VERSION_CODE)
#ifdef CONFIG_AMEBALITE
// amebalite use different pin due to _PA_1(num 15, GPIO22 got by command "pinout") will be pull down by raspberry pi
#define DEV_READY_PIN	        584
#define RX_REQ_PIN				583
#else
#define DEV_READY_PIN			591
#define RX_REQ_PIN				592
#endif

#define DEBUG_PIN				582

#elif (KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE)

/* For kernel version higher than 6.6, gpio pins of raspberry pi 5 are numbered differently.
check the correct number of the required GPIO pin to use command " cat /sys/kernel/debug/gpio" */
#ifdef CONFIG_AMEBALITE
#define DEV_READY_PIN	        586
#define RX_REQ_PIN				585
#else
#define DEV_READY_PIN			593
#define RX_REQ_PIN				594
#endif

#define DEBUG_PIN				584

#else
#ifdef CONFIG_AMEBALITE
#define DEV_READY_PIN			15
#define RX_REQ_PIN				14
#else
#define DEV_READY_PIN			22
#define RX_REQ_PIN				23
#endif

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

	atomic_t dev_state;

	int (*rx_process_func)(struct sk_buff *pskb);

};

extern struct whc_spi whc_spi_priv;
extern struct hci_ops_t whc_spi_host_intf_ops;

#endif /* __INIC_SPI_H__ */
