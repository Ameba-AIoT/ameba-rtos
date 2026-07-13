/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/

#ifndef __RTW_TRANSPORT_H__
#define __RTW_TRANSPORT_H__

#include <stddef.h>

typedef enum {
	RTW_XPORT_IOCTL = 0,
	RTW_XPORT_NETLINK = 1,
} rtw_xport_type_t;

typedef struct {
	const char	*ifname;
	rtw_xport_type_t type;
} rtw_xport_ctx_t;

/* Build transport context from CLI argv (handles -n and RTW_DEFAULT_NETLINK). */
rtw_xport_ctx_t rtw_xport_from_args(int argc, char *argv[]);

/* Build transport context with explicit type. */
rtw_xport_ctx_t rtw_xport_make(const char *ifname, rtw_xport_type_t type);

/*
 * Send MP string command through ioctl or netlink.
 * When expect_reply is non-zero, driver reply is written into cmd[].
 */
int rtw_mp_send(rtw_xport_ctx_t *ctx, char *cmd, size_t cmdlen, int expect_reply);

/*
 * Netlink MP fallback: join argv[cmd_start..] and send via rtw_nl_send_cmd().
 */
int rtw_mp_send_nl_argv(int argc, char *argv[], int cmd_start);

/*
 * Send binary MP ioctl payload (HW TX mp_hxtx). Ioctl transport only.
 */
int rtw_mp_send_binary(rtw_xport_ctx_t *ctx, void *buf, size_t len);

#endif /* __RTW_TRANSPORT_H__ */
