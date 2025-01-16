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

#define SPI_IS_AS_MASTER	1

#if defined (CONFIG_AMEBASMART)
#define SPI_GPIO_CS0 _PA_0
#define SPI_GPIO_CS1 _PA_1
#if SPI_IS_AS_MASTER
// SPI1
#define SPI1_MOSI  _PA_5
#define SPI1_MISO  _PA_4
#define SPI1_SCLK  _PA_3
#define SPI1_CS    _PA_2

#else
// SPI0
//both EV30EL0 and EV30EA0 support
#define SPI0_MOSI  _PA_13
#define SPI0_MISO  _PA_14
#define SPI0_SCLK  _PA_15
#define SPI0_CS    _PA_16

#endif

#elif defined (CONFIG_AMEBALITE)
#define SPI_GPIO_CS0 _PA_0
#define SPI_GPIO_CS1 _PA_1
#if SPI_IS_AS_MASTER
// SPI1
#define SPI1_MOSI	_PB_3
#define SPI1_MISO	_PB_4
#define SPI1_SCLK	_PB_2
#define SPI1_CS		_PB_5

#else
// SPI0
#define SPI0_MOSI	_PA_29
#define SPI0_MISO	_PA_30
#define SPI0_SCLK	_PA_28
#define SPI0_CS		_PA_31
#endif

#elif defined (CONFIG_AMEBADPLUS)
#define SPI_GPIO_CS0 _PB_30
#define SPI_GPIO_CS1 _PB_31
#if SPI_IS_AS_MASTER
// SPI1 FID=8 & Fully PG
#define SPI1_MOSI	_PB_19
#define SPI1_MISO	_PB_20
#define SPI1_SCLK	_PB_18
#define SPI1_CS		_PB_21

#else
// SPI0 FID=8
#define SPI0_MOSI	_PB_24
#define SPI0_MISO	_PB_25
#define SPI0_SCLK	_PB_23
#define SPI0_CS		_PB_26
#endif


#elif defined (CONFIG_AMEBAD)
#define SPI_GPIO_CS0 _PA_12
#define SPI_GPIO_CS1 _PA_13

#if SPI_IS_AS_MASTER
// SPI1 //Master
#define SPI1_MOSI  _PB_4
#define SPI1_MISO  _PB_5
#define SPI1_SCLK  _PB_6
#define SPI1_CS    _PB_7

#else
// SPI0 //Slave
#define SPI0_MOSI  _PB_18
#define SPI0_MISO  _PB_19
#define SPI0_SCLK  _PB_20
#define SPI0_CS    _PB_21
#endif

#endif

extern int example_raw_spi_multislave(void);
#endif
