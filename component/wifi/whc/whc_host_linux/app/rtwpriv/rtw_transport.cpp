/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "rtwpriv.h"
#include "rtw_api.h"
#include "rtw_nl.h"
#include "rtw_transport.h"

int do_io_ctrl(char *ifname, char *pBufdata);

static void trim_mp_cmd(char *cmd)
{
	size_t len;

	if (!cmd) {
		return;
	}

	len = strlen(cmd);
	while (len > 0) {
		char ch = cmd[len - 1];

		if (ch != '\r' && ch != '\n' && ch != ' ') {
			break;
		}
		cmd[--len] = '\0';
	}
}

rtw_xport_ctx_t rtw_xport_make(const char *ifname, rtw_xport_type_t type)
{
	rtw_xport_ctx_t ctx;

	ctx.ifname = ifname;
	ctx.type = type;
	return ctx;
}

rtw_xport_ctx_t rtw_xport_from_args(int argc, char *argv[])
{
	rtw_xport_ctx_t ctx;

	ctx.ifname = NULL;
	ctx.type = RTW_XPORT_IOCTL;

	if (argc < 2) {
		return ctx;
	}

	if (strcmp(argv[1], "-n") == 0) {
		ctx.type = RTW_XPORT_NETLINK;
		if (argc >= 3) {
			ctx.ifname = argv[2];
		}
		return ctx;
	}

#ifdef RTW_DEFAULT_NETLINK
	if (argc >= 3) {
		ctx.type = RTW_XPORT_NETLINK;
		ctx.ifname = argv[1];
		return ctx;
	}
#endif

	ctx.type = RTW_XPORT_IOCTL;
	ctx.ifname = argv[1];
	return ctx;
}

int rtw_mp_send(rtw_xport_ctx_t *ctx, char *cmd, size_t cmdlen, int expect_reply)
{
	if (!ctx || !ctx->ifname || !cmd) {
		return -1;
	}

	trim_mp_cmd(cmd);

	if (ctx->type == RTW_XPORT_NETLINK) {
		if (expect_reply) {
			return rtw_nl_send_cmd_buf(cmd, cmd, cmdlen);
		}
		return rtw_nl_send_cmd(cmd);
	}

	if (expect_reply) {
		return rtw_io_ctrl((char *)ctx->ifname, cmd, FALSE);
	}

	return do_io_ctrl((char *)ctx->ifname, cmd);
}

int rtw_mp_send_nl_argv(int argc, char *argv[], int cmd_start)
{
	char tmp[BUF_SIZE];
	int i;

	if (!argv || argc <= cmd_start) {
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%s", argv[cmd_start]);
	for (i = cmd_start + 1; i < argc; i++) {
		strncat(tmp, " ", sizeof(tmp) - strlen(tmp) - 1);
		strncat(tmp, argv[i], sizeof(tmp) - strlen(tmp) - 1);
	}

	DBG("Netlink cmd (fallback MP mode): %s\n", tmp);
	return rtw_nl_send_cmd(tmp);
}

int rtw_mp_send_binary(rtw_xport_ctx_t *ctx, void *buf, size_t len)
{
	int skfd, err;

	if (!ctx || !ctx->ifname || !buf || len == 0) {
		return -1;
	}

	if (ctx->type != RTW_XPORT_IOCTL) {
		fprintf(stderr, "binary MP ioctl requires ioctl transport\n");
		return -1;
	}

	skfd = iw_sockets_open();
	if (skfd < 0) {
		return -1;
	}

	err = wlan_ioctl_mp(skfd, (char *)ctx->ifname, buf, len);
	iw_sockets_close(skfd);
	return err;
}
