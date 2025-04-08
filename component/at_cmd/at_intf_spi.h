/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef SPI_EXT_H
#define SPI_EXT_H

#include "platform_autoconf.h"

#define SPI_SLAVE_RD_CMD 0x01
#define SPI_SLAVE_WR_CMD 0x02

#define ATCMD_SPI_DMA_SIZE	2048 * 8//for dma mode, buffer size should be multiple of CACHE_LINE_SIZE 

struct atcmd_spi_req {
	u8 cmd;
	u8 len;
};

extern u8 SPI0_MISO;
extern u8 SPI0_MOSI;
extern u8 SPI0_SCLK;
extern u8 SPI0_CS;
extern u8 AT_SYNC_FROM_MASTER_GPIO;
extern u8 AT_SYNC_TO_MASTER_GPIO;

_LONG_CALL_ void atio_spi_output(char *buf, int len);
_LONG_CALL_ int atio_spi_init(void);

#endif
