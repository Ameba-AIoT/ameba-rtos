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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "rtwpriv.h"
#include "rtw_api.h"
#include "rtw_udpsrv.h"
#include "rtw_nl_cmd.h"
#include "rtw_transport.h"
#include "rtw_hwtx.h"

static int rtwpriv_netlink_dispatch(int argc, char *argv[], int ifname_idx)
{
	rtw_xport_ctx_t xport;
	int err = 0;
	int i;
	char *shifted_argv[64];
	int shifted_argc;

	if (argc <= ifname_idx + 1) {
		rtw_nl_cmd_help();
		return -EINVAL;
	}

	if (strcmp(argv[ifname_idx + 1], "mp_server") == 0) {
		xport = rtw_xport_make(argv[ifname_idx], RTW_XPORT_NETLINK);
		return rtw_udpsrv_start(argv[ifname_idx], &xport);
	}

	if (ifname_idx == 2) {
		shifted_argc = argc - 1;
		shifted_argv[0] = argv[0];
		for (i = 1; i < shifted_argc; i++) {
			shifted_argv[i] = argv[i + 1];
		}

		if (rtw_nl_cmd_try_dispatch(shifted_argc, shifted_argv, &err)) {
			return err;
		}

		return rtw_mp_send_nl_argv(shifted_argc, shifted_argv, 2);
	}

	if (rtw_nl_cmd_try_dispatch(argc, argv, &err)) {
		return err;
	}

	return rtw_mp_send_nl_argv(argc, argv, 2);
}

static int rtwpriv_ioctl_dispatch(int argc, char *argv[])
{
	char *ifname, *incmd;
	char input[BUF_SIZE];
	char tmpcmdstr[BUF_SIZE];
	rtw_xport_ctx_t xport;
	int err = 0;
	int i;

	if (rtw_hwtx_wants_run(argc, argv)) {
		xport = rtw_xport_make(argv[1], RTW_XPORT_IOCTL);
		return rtw_hwtx_run(&xport, argc, argv);
	}

	if (argc < 3) {
		return -EINVAL;
	}

	ifname = argv[1];
	incmd = argv[2];

	if (strcmp(incmd, "ver") == 0 || strcmp(argv[1], "-v") == 0) {
		printf("rtwpriv version:%s\n", RTWPRIVE_VERSION);
		return err;
	}

	if ((strncmp(incmd, "?", 1) == 0) || (strncmp(incmd, "help", 4) == 0)) {
		printf("rtwpriv version:%s\n", RTWPRIVE_VERSION);
		rtw_help();
		return err;
	}

	if (strncmp(incmd, "mp_server", 15) == 0) {
		xport = rtw_xport_make(ifname, RTW_XPORT_IOCTL);
		return rtw_udpsrv_start(ifname, &xport);
	}

	memset(tmpcmdstr, 0, sizeof(tmpcmdstr));
	sprintf(tmpcmdstr, "%s ", incmd);
	for (i = 3; i < argc; i++) {
		strncat(tmpcmdstr, argv[i], strlen(argv[i]));
		strcat(tmpcmdstr, " ");
	}

	DBG("tmpcmdstr:%s strlen=%d\n", tmpcmdstr, strlen(tmpcmdstr));
	strncpy(input, tmpcmdstr, strlen(tmpcmdstr) - 1);
	DBG("input:%s\n", input);
	return rtw_io_ctrl(ifname, input, FALSE);
}

int main(int argc, char *argv[])
{
	int err = 0;

	DBG("input argc = %d\n", argc);

	if (argc < 3) {
		if (argc <= 1) {
			rtw_help();
		} else if (strcmp(argv[1], "-n") == 0) {
			rtw_nl_cmd_help();
		} else if (strcmp(argv[1], "ver") == 0 || strcmp(argv[1], "-v") == 0) {
			printf("rtwpriv version:%s\n", RTWPRIVE_VERSION);
		} else {
			rtw_help();
		}
		return err;
	}

	if (argc >= 3 && strcmp(argv[1], "-n") == 0) {
		return rtwpriv_netlink_dispatch(argc, argv, 2);
	}

#ifdef RTW_DEFAULT_NETLINK
	return rtwpriv_netlink_dispatch(argc, argv, 1);
#endif

	return rtwpriv_ioctl_dispatch(argc, argv);
}
