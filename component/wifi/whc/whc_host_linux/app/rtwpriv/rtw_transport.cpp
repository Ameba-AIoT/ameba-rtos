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
#include <strings.h>
#include <stdlib.h>

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

static int is_mp_mode_flag(const char *arg, rtw_mp_mode_t *mode)
{
	if (!arg || !mode) {
		return 0;
	}

	if (strcmp(arg, "-l") == 0 || strcmp(arg, "--mp-legacy") == 0) {
		*mode = RTW_MP_MODE_LEGACY;
		return 1;
	}

	if (strcmp(arg, "--mp-sendbuf") == 0) {
		*mode = RTW_MP_MODE_SENDBUF;
		return 1;
	}

	return 0;
}

rtw_mp_mode_t rtw_mp_mode_default(void)
{
#if defined(RTW_WHC_MP_DEFAULT_LEGACY)
	return RTW_MP_MODE_LEGACY;
#else
	/* Default: sendbuf (matches v1.2.1 USB MP driver without CMD_WIFI_MP). */
	return RTW_MP_MODE_SENDBUF;
#endif
}

rtw_mp_mode_t rtw_mp_mode_resolve(int argc, char *argv[])
{
	rtw_mp_mode_t mode = rtw_mp_mode_default();
	int cli_mode_set = 0;
	const char *env;
	int i;

	for (i = 1; i < argc; i++) {
		if (is_mp_mode_flag(argv[i], &mode)) {
			cli_mode_set = 1;
		}
	}

	if (cli_mode_set) {
		return mode;
	}

	env = getenv("RTW_WHC_MP_MODE");
	if (env) {
		if (strcasecmp(env, "legacy") == 0) {
			return RTW_MP_MODE_LEGACY;
		}
		if (strcasecmp(env, "sendbuf") == 0) {
			return RTW_MP_MODE_SENDBUF;
		}
	}

	return rtw_mp_mode_default();
}

int rtw_argv_strip_mp_flags(int *argc, char **argv)
{
	int rd = 1;
	int wr = 1;
	rtw_mp_mode_t dummy;

	if (!argc || !argv || *argc <= 0) {
		return 0;
	}

	while (rd < *argc) {
		if (is_mp_mode_flag(argv[rd], &dummy)) {
			rd++;
			continue;
		}
		if (wr != rd) {
			argv[wr] = argv[rd];
		}
		wr++;
		rd++;
	}

	*argc = wr;
	return 0;
}

rtw_xport_ctx_t rtw_xport_make(const char *ifname, rtw_xport_type_t type)
{
	rtw_xport_ctx_t ctx;

	ctx.ifname = ifname;
	ctx.type = type;
	ctx.mp_mode = rtw_mp_mode_default();
	return ctx;
}

rtw_xport_ctx_t rtw_xport_from_args(int argc, char *argv[])
{
	rtw_xport_ctx_t ctx;

	ctx.ifname = NULL;
	ctx.type = RTW_XPORT_IOCTL;
	ctx.mp_mode = rtw_mp_mode_resolve(argc, argv);

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
			return rtw_nl_send_cmd_buf_mode(cmd, cmd, cmdlen, ctx->mp_mode);
		}
		return rtw_nl_send_cmd_mode(cmd, ctx->mp_mode);
	}

	if (expect_reply) {
		return rtw_io_ctrl((char *)ctx->ifname, cmd, FALSE);
	}

	return do_io_ctrl((char *)ctx->ifname, cmd);
}

int rtw_mp_send_nl_argv(rtw_xport_ctx_t *ctx, int argc, char *argv[], int cmd_start)
{
	char tmp[BUF_SIZE];
	int i;

	if (!ctx || !argv || argc <= cmd_start) {
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	snprintf(tmp, sizeof(tmp), "%s", argv[cmd_start]);
	for (i = cmd_start + 1; i < argc; i++) {
		strncat(tmp, " ", sizeof(tmp) - strlen(tmp) - 1);
		strncat(tmp, argv[i], sizeof(tmp) - strlen(tmp) - 1);
	}

	DBG("Netlink cmd (MP mode=%d): %s\n", (int)ctx->mp_mode, tmp);
	return rtw_nl_send_cmd_mode(tmp, ctx->mp_mode);
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
