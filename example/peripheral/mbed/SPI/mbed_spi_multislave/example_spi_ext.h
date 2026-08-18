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

#ifndef SPI_IS_AS_MASTER
#define SPI_IS_AS_MASTER	1
#endif

#if defined (CONFIG_AMEBASMART)
#define SPI_GPIO_CS0 _PA_9
#define SPI_GPIO_CS1 _PA_10
#if SPI_IS_AS_MASTER
#define SPI_MOSI  _PA_5
#define SPI_MISO  _PA_4
#define SPI_SCLK  _PA_3
#define SPI_CS    _PA_2

#else
//both EV30EL0 and EV30EA0 support
#define SPI_MOSI  _PA_13
#define SPI_MISO  _PA_14
#define SPI_SCLK  _PA_15
#define SPI_CS    _PA_16

#endif

#elif defined (CONFIG_AMEBALITE)
#define SPI_GPIO_CS0 _PA_0
#define SPI_GPIO_CS1 _PA_1
#if SPI_IS_AS_MASTER
#define SPI_MOSI	_PB_3
#define SPI_MISO	_PB_4
#define SPI_SCLK	_PB_2
#define SPI_CS		_PB_5

#else
#define SPI_MOSI	_PA_29
#define SPI_MISO	_PA_30
#define SPI_SCLK	_PA_28
#define SPI_CS		_PA_31
#endif

#elif defined (CONFIG_AMEBADPLUS)
#define SPI_GPIO_CS0 _PB_30
#define SPI_GPIO_CS1 _PB_31
#if SPI_IS_AS_MASTER
#define SPI_MOSI	_PB_19
#define SPI_MISO	_PB_20
#define SPI_SCLK	_PB_18
#define SPI_CS		_PB_21

#else
#define SPI_MOSI	_PB_24
#define SPI_MISO	_PB_25
#define SPI_SCLK	_PB_23
#define SPI_CS		_PB_26
#endif

#elif defined (CONFIG_AMEBAD)
#define SPI_GPIO_CS0 _PA_12
#define SPI_GPIO_CS1 _PA_13

#if SPI_IS_AS_MASTER
#define SPI_MOSI  _PB_4
#define SPI_MISO  _PB_5
#define SPI_SCLK  _PB_6
#define SPI_CS    _PB_7

#else
#define SPI_MOSI  _PB_18
#define SPI_MISO  _PB_19
#define SPI_SCLK  _PB_20
#define SPI_CS    _PB_21
#endif

#elif defined (CONFIG_AMEBAGREEN2)
#define SPI_GPIO_CS0 _PA_26
#define SPI_GPIO_CS1 _PA_25

#if SPI_IS_AS_MASTER
#define SPI_MOSI  _PB_8
#define SPI_MISO  _PB_9
#define SPI_SCLK  _PB_7
#define SPI_CS    _PB_10

#else
#define SPI_MOSI  _PB_8
#define SPI_MISO  _PB_9
#define SPI_SCLK  _PB_7
#define SPI_CS    _PA_26
#endif


#elif defined (CONFIG_RTL8720F)
#define SPI_GPIO_CS0 _PA_10
#define SPI_GPIO_CS1 _PA_3

#if SPI_IS_AS_MASTER
#define SPI_MOSI  _PA_8
#define SPI_MISO  _PA_9
#define SPI_SCLK  _PA_7
#define SPI_CS    _PA_5

#else
#define SPI_MOSI  _PA_8
#define SPI_MISO  _PA_9
#define SPI_SCLK  _PA_7
#define SPI_CS    _PA_10
#endif
#endif

extern int example_mbed_spi_multislave(void);
#endif
