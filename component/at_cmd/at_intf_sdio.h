/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef AT_INTF_SDIO_H
#define AT_INTF_SDIO_H

#define ATCMD_SDIO_MAX_SIZE (2048 - 16)

/* Device->Host framing.
 *
 * The device TX path no longer prepends an INIC_RX_DESC BD (saves one BD per
 * packet), so the SDIO IP hands the host a bare payload and the only length
 * hint left is the RX0_REQ_LEN register.  Carry the length inline instead, the
 * same way the WHC SDIO host driver takes it from the payload itself rather
 * than from that register (whc_host_rtos/sdio/whc_sdio_host.c).
 *
 * magic makes a device/host firmware mismatch fail with a log line instead of
 * garbage on the host UART, and makes the stream self-delimiting so a host can
 * walk several packets out of one RX FIFO read.
 *
 * Host->Device keeps using INIC_TX_DESC and is unaffected.
 */
#define ATCMD_SDIO_HDR_MAGIC 0x5AA5

struct atcmd_sdio_hdr {
	u16 magic;   /* ATCMD_SDIO_HDR_MAGIC */
	u16 len;     /* payload bytes following this header */
};

#define ATCMD_SDIO_HDR_SIZE ((u16)sizeof(struct atcmd_sdio_hdr))

int atio_sdio_init(void);

#endif /* AT_INTF_SPI_H */