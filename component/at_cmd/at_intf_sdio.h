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
 * packet), so the SDIO IP hands the host a bare payload.  The host can use
 * the RX0_REQ_LEN register for the total transfer length, but carrying the
 * payload length inline in this header keeps the stream self-delimiting and
 * allows a host to walk several packets out of one RX FIFO read.
 *
 * magic makes a device/host firmware mismatch fail with a log line instead of
 * garbage on the host UART.
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