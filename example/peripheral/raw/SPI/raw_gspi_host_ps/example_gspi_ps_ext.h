/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GSPI_PS_EXT_H
#define GSPI_PS_EXT_H

#include "platform_autoconf.h"

/*
 * GSPI power-save / wake-up test, Host side. Same wiring as
 * raw_gspi_host_fifo (SDIO_Pin_Grp = 2, no rewiring), plus D2 as an
 * out-of-band wake line:
 *   GSPI_SCLK -> PA18 (Device CLK)
 *   GSPI_MOSI -> PA19 (Device CMD)
 *   GSPI_MISO -> PA28 (Device D0)
 *   GSPI_CS   -> PA21 (Device D3)
 *   GSPI_INT  -> PA20 (Device D1), the protocol interrupt line
 *   GSPI_WAKE -> PA27 (Device D2), GPIO input, Device-driven wake line
 *
 * The Device board must run mbed_gspi_device_ps.
 */

#if defined (CONFIG_AMEBAGREEN2)
#define GSPI_MOSI  _PA_19
#define GSPI_MISO  _PA_28
#define GSPI_SCLK  _PA_18
#define GSPI_CS    _PA_21
#define GSPI_INT   _PA_20
#define GSPI_WAKE  _PA_27
#endif

extern int example_raw_gspi_host_ps(void);
#endif
