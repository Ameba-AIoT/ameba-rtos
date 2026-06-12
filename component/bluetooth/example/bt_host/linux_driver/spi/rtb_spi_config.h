#ifndef __RTB_SPI_CONFIG_H__
#define __RTB_SPI_CONFIG_H__

#if (KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE)

/* For kernel version higher than 6.6, gpio pins of raspberry pi are numbered differently.
check the correct number of the required GPIO pin in /sys/kernel/debug/gpio*/

#define DEV_READY_PIN           534
#define RX_REQ_PIN              535
#define DEBUG_PIN               525
#else
#define DEV_READY_PIN           22
#define RX_REQ_PIN              23
#define DEBUG_PIN               13
#endif

#define DEV_READY_IRQ           gpio_to_irq(DEV_READY_PIN)
#define DEV_READY               1
#define DEV_BUSY                0

#define RX_REQ_IRQ              gpio_to_irq(RX_REQ_PIN)
#define DEV_RX_REQ              1
#define DEV_RX_IDLE             0

#define SPI_BUFSZ       (256 + 32)

#endif /* __RTB_SPI_CONFIG_H__ */
