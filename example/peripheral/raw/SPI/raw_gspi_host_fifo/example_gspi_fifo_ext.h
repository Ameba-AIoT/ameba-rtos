/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GSPI_FIFO_EXT_H
#define GSPI_FIFO_EXT_H

#include "platform_autoconf.h"

/*
 * Stage 4: GSPI TX/RX FIFO domain bring-up. Builds directly on
 * raw_gspi_host_min's proven mode-lock + register read/write code.
 *
 * Same wiring as raw_gspi_host_min (SDIO_Pin_Grp=2, no rewiring):
 *   GSPI_SCLK -> PA18 (Device CLK)
 *   GSPI_MOSI -> PA19 (Device CMD)
 *   GSPI_MISO -> PA28 (Device D0)
 *   GSPI_CS   -> PA21 (Device D3)
 *   GSPI_INT  -> PA20 (Device D1), GPIO input only, not read by this test
 *   PA27 (D2) is not used by this minimal test.
 *
 * Device board must run mbed_sdio_device_tp unmodified (its Phase 0
 * handshake -- "Hello Device" / "Hello Host! I am Here" -- is what this
 * example exercises over the TX/RX FIFO domain instead of SDIO).
 */

#if defined (CONFIG_AMEBAGREEN2)
#define GSPI_MOSI  _PA_19
#define GSPI_MISO  _PA_28
#define GSPI_SCLK  _PA_18
#define GSPI_CS    _PA_21
#define GSPI_INT   _PA_20
#endif

extern int example_raw_gspi_host_fifo(void);
#endif
