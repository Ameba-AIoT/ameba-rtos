/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RTW_GSPI_DRVIO_H_
#define _RTW_GSPI_DRVIO_H_

/* GSPI_DRV_VER (1 or 2) is defined by the CMake of the user, together with the matching rtw_gspi_drvio_v<N>.c. */
#if !defined(GSPI_DRV_VER)
#error "GSPI protocol version not set: define GSPI_DRV_VER as 1 or 2 (see rtw_gspi_drvio.h)"
#elif GSPI_DRV_VER == 2
#include "rtw_gspi_drvio_v2.h"
#elif GSPI_DRV_VER == 1
#include "rtw_gspi_drvio_v1.h"
#else
#error "unsupported GSPI_DRV_VER, expected 1 or 2"
#endif

#endif
