// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 */
#ifndef _WHC_GSPI_HOST_H_
#define _WHC_GSPI_HOST_H_

struct whc_gspi;

#include <linux/spi/spi.h>
#include <linux/version.h>
#include <linux/gpio.h>

#include "ameba_inic.h"
#include "whc_gspi_host_reg.h"
#include "whc_gspi_host_drvio.h"

/*
 * DEV_INT pin: device pulls it low to signal either "RX packet ready"
 * (HISR RX_REQUEST) or "TX BD available" (HISR TXBD_AVAL). The handler reads
 * HISR to tell them apart. Numbers below track the Raspberry Pi header for the
 * different kernel GPIO numbering schemes, matching the WHC SPI driver.
 */
#if (KERNEL_VERSION(6, 12, 0) <= LINUX_VERSION_CODE)
#ifdef CONFIG_AMEBALITE
#define DEV_INT_PIN			584
#else
#define DEV_INT_PIN			591
#endif
#elif (KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE)
#ifdef CONFIG_AMEBALITE
#define DEV_INT_PIN			586
#else
#define DEV_INT_PIN			593
#endif
#else
#ifdef CONFIG_AMEBALITE
#define DEV_INT_PIN			15
#else
#define DEV_INT_PIN			22
#endif
#endif

#define DEV_INT_IRQ			gpio_to_irq(DEV_INT_PIN)

/* GSPI carries an INIC TX descriptor ahead of the payload, like SDIO. There is
 * no separate RX descriptor on this path. */
#define SIZE_RX_DESC			0
#define SIZE_TX_DESC			(sizeof(INIC_TX_DESC))

#define BUF_ALIGN_SZ			4

#define SPI_DMA_ALIGN(x)		((((x - 1) >> 5) + 1) << 5)	/* round up to 32 */
#define SPI_BUFSZ			(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))

/* Largest single TX transfer accepted (payload + descriptor). */
#define GSPI_TX_MAX_SZ			SPI_BUFSZ

/* HW rx aggregation fires when either threshold is hit, whichever comes first */
#define GSPI_RX_AGG_TO			0	/* timeout = (0+1)*32us? */
#define GSPI_RX_AGG_BD_CNT_TH	4	/* aggregate up to 4 RXBDs per transfer */

/* Wake the TX waiter from the TXBD-available interrupt instead of polling. */
#define CONFIG_GSPI_TX_ENABLE_AVAL_INT

/* for txbd should <= 4, 1.6K every pkt， about 6.4K， roundup to 8K*/
#define CONFIG_MAX_TXAGG_SZ		8192
#define PWR_STATE_ACTIVE		0
#define PWR_STATE_SLEEP			1

enum RPWM2_EVENT {
	RPWM2_PWR_SUSPEND	= 0,	/* clock gated */
	RPWM2_PWR_RESUME	= 1,	/* wakeup event */
};

struct whc_gspi {
	struct spi_device	*spi_dev;
	struct mutex	lock;		/* serialize the TX send path */

	wait_queue_head_t	txbd_wq;

	u32			gspi_himr;	/* HIMR shadow: the register is write-only */
	u32			gspi_hisr;
	u32			GspiTxMaxSZ;
	u16			txbd_size;
	u8			GspiTxBDFreeNum;

	u8			swap;		/* 1 = 4-byte swap needed (device little-endian) */
	u8			irq_alloc;
	u8			dev_state;
	u8			tx_avail_int_triggered;
	u8			txbd_wptr;

	void	(*rx_recv_notify)(void);
#ifdef WHC_TX_AGG
	u8				*agg_buf;
#endif
};

extern struct whc_gspi whc_gspi_priv;
extern struct hci_ops_t whc_gspi_host_intf_ops;

/* whc_gspi_host_init.c */
u32  rtw_gspi_init(struct whc_gspi *priv);
void rtw_gspi_deinit(struct whc_gspi *priv);
u32  rtw_gspi_get_rx_len(struct whc_gspi *priv);
u32  rtw_gspi_get_free_txbd(struct whc_gspi *priv);

/* whc_gspi_host_ops.c */
void whc_gspi_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);

#endif  /* _WHC_GSPI_HOST_H_ */
