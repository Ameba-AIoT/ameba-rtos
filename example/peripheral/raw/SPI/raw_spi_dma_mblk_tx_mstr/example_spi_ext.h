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

#if defined (CONFIG_AMEBASMART)
//both EV30EL0 and EV30EA0 support
#define SPI_MOSI  _PA_13
#define SPI_MISO  _PA_14
#define SPI_SCLK  _PA_15
#define SPI_CS    _PA_16

#define CACHE_LINE_SIZE CACHE_L1_DCACHE_LINE_SIZE

#elif defined (CONFIG_AMEBALITE)
#define SPI_MOSI	_PA_29
#define SPI_MISO	_PA_30
#define SPI_SCLK	_PA_28
#define SPI_CS		_PA_31

#elif defined (CONFIG_AMEBADPLUS)
#define SPI_MOSI	_PB_24
#define SPI_MISO	_PB_25
#define SPI_SCLK	_PB_23
#define SPI_CS		_PB_26

#elif defined (CONFIG_AMEBAD)
#define SPI_MOSI  _PB_18
#define SPI_MISO  _PB_19
#define SPI_SCLK  _PB_20
#define SPI_CS    _PB_21

#elif defined (CONFIG_AMEBAGREEN2)
#define SPI_MOSI  _PA_30
#define SPI_MISO  _PA_31
#define SPI_SCLK  _PA_29
#define SPI_CS    _PB_0

#elif defined (CONFIG_RTL8720F)
#define SPI_MOSI  _PA_8
#define SPI_MISO  _PA_9
#define SPI_SCLK  _PA_7
#define SPI_CS    _PA_10

#endif

extern int example_raw_spi_multi_dma_tx_master(void);
#endif
