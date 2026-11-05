/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef GSPI_EXT_H
#define GSPI_EXT_H

#include "platform_autoconf.h"

/*
 * Minimal GSPI (Realtek SDIO Device / SPI Slave combo IP) host bring-up test.
 * Uses SPI0 as a plain SPI Master (Mode 0). CS is driven manually via GPIO
 * (not the SPI peripheral's own hardware CS) because GSPI requires CS to stay
 * low for an entire Command+Status+Data exchange without interruption.
 *
 * Reuses the exact same physical wiring as the Stage-1 SDIO test (no rewiring),
 * per gspi/GSPI_Usage_Guide_v1_fixpin 1.md ch.2.2 SDIO_Pin_Grp=2:
 *   GSPI_SCLK -> PA18 (Device CLK)
 *   GSPI_MOSI -> PA19 (Device CMD)
 *   GSPI_MISO -> PA28 (Device D0)
 *   GSPI_CS   -> PA21 (Device D3)
 *   GSPI_INT  -> PA20 (Device D1), GPIO input only, not read by this test
 *   PA27 (D2) is not used by this minimal test.
 *
 * Pinmux uses the per-signal extended func IDs the usage guide's ch.2.3
 * wiring table specifies (SPI0_CLK=75/SPI0_MISO=76/SPI0_MOSI=77, CS/INT=GPIO),
 * applied via explicit Pinmux_Config() calls in the .c file -- id9/10 (the
 * chip's one fixed SPI0/1 pin group, PA29/30/31/PB0) must never be written
 * to these pins; spi_init() is bootstrapped on that unused fixed group only.
 */

#if defined (CONFIG_AMEBAGREEN2)
#define GSPI_MOSI  _PA_19
#define GSPI_MISO  _PA_28
#define GSPI_SCLK  _PA_18
#define GSPI_CS    _PA_21
#define GSPI_INT   _PA_20
#endif

extern int example_raw_gspi_host_min(void);
#endif
