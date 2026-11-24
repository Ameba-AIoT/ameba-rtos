/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/

#ifndef __RTW_HWTX_H__
#define __RTW_HWTX_H__

#include "rtw_transport.h"

/* True for "wlan0 stop" or positional/key=value HW TX (argc >= 7). */
int rtw_hwtx_wants_run(int argc, char *argv[]);

/* Run HW TX / stop sequence (ioctl transport only). */
int rtw_hwtx_run(rtw_xport_ctx_t *ctx, int argc, char *argv[]);

#endif /* __RTW_HWTX_H__ */
