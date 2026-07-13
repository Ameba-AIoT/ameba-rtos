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

#include "rtwpriv.h"
#include "rtw_nl.h"
#include "rtw_nl_cmd.h"

/* Build WHC_WIFI_TEST payload: [magic(4)] [sub(1)] [data...] */
static int build_whc_payload(uint8_t *buf, uint32_t maxlen, uint8_t sub_cmd,
							 const uint8_t *data, uint32_t datalen)
{
	uint32_t total = 4 + 1 + datalen;
	if (total > maxlen) {
		return -1;
	}
	*(uint32_t *)buf = 0xffa5a5a5; /* WHC_WIFI_TEST */
	buf[4] = sub_cmd;
	if (data && datalen > 0) {
		memcpy(buf + 5, data, datalen);
	}
	return (int)total;
}

static int handle_getip(int argc, char *argv[])
{
	int idx;
	uint8_t payload[16];
	uint8_t idxbuf;
	int len;

	if (argc < 4) {
		fprintf(stderr, "Usage: rtwpriv <ifname> getip <wlan_idx>\n");
		return -1;
	}
	idx = atoi(argv[3]);
	idxbuf = (uint8_t)idx;
	len = build_whc_payload(payload, sizeof(payload), 0x02, &idxbuf, 1);
	if (len < 0) {
		return -1;
	}
	return rtw_nl_send_buf(payload, len, 1);
}

static int handle_getmac(int argc, char *argv[])
{
	int idx;
	uint8_t payload[16];
	uint8_t idxbuf;
	int len;

	if (argc < 4) {
		fprintf(stderr, "Usage: rtwpriv <ifname> getmac <wlan_idx>\n");
		return -1;
	}
	idx = atoi(argv[3]);
	idxbuf = (uint8_t)idx;
	len = build_whc_payload(payload, sizeof(payload), 0x01, &idxbuf, 1);
	if (len < 0) {
		return -1;
	}
	return rtw_nl_send_buf(payload, len, 1);
}

static int handle_logon(int argc, char *argv[])
{
	uint8_t payload[8];
	int len;
	len = build_whc_payload(payload, sizeof(payload), 0x15, NULL, 0);
	if (len < 0) {
		return -1;
	}
	return rtw_nl_send_buf(payload, len, 0);
}

static int handle_logoff(int argc, char *argv[])
{
	uint8_t payload[8];
	int len;
	len = build_whc_payload(payload, sizeof(payload), 0x16, NULL, 0);
	if (len < 0) {
		return -1;
	}
	return rtw_nl_send_buf(payload, len, 0);
}

static int handle_dbg(int argc, char *argv[])
{
	char dbg_buf[256];
	int j, pos;

	if (argc < 4) {
		fprintf(stderr, "Usage: rtwpriv <ifname> dbg <cmd> [args...]\n");
		return -1;
	}
	memset(dbg_buf, 0, sizeof(dbg_buf));
	pos = snprintf(dbg_buf, sizeof(dbg_buf), "%s", argv[3]);
	for (j = 4; j < argc && pos < (int)sizeof(dbg_buf) - 1; j++) {
		pos += snprintf(dbg_buf + pos, sizeof(dbg_buf) - pos, " %s", argv[j]);
	}

	return rtw_nl_send_dbg(dbg_buf);
}

typedef int (*nl_handler_t)(int argc, char *argv[]);

static const struct {
	const char *name;
	nl_handler_t handler;
	const char *usage;
	const char *desc;
} nl_cmd_table[] = {
	{"getip",	handle_getip,	"<wlan_idx>",		"Get the IP address of the given wlan index"},
	{"getmac",	handle_getmac,	"<wlan_idx>",		"Get the MAC address of the given wlan index"},
	{"logon",	handle_logon,	"",			"Enable firmware log output"},
	{"logoff",	handle_logoff,	"",			"Disable firmware log output"},
	{"dbg",		handle_dbg,	"<cmd> [args...]",	"Send a raw debug command string"},
	{NULL, NULL, NULL, NULL}
};

void rtw_nl_cmd_help(void)
{
	int k;

	printf("rtwpriv version:%s\n", RTWPRIVE_VERSION);
	printf("#####[ Netlink (-n) command list ]#####\n");
	printf("Usage: rtwpriv -n <ifname> <command> [args...]\n");
	printf("   e.g. rtwpriv -n wlan0 getmac 0\n\n");
	printf("  %-10s %-26s %s\n", "COMMAND", "ARGS", "DESCRIPTION");
	printf("  %-10s %-26s %s\n", "-------", "----", "-----------");
	for (k = 0; nl_cmd_table[k].name != NULL; k++) {
		printf("  %-10s %-26s %s\n",
			   nl_cmd_table[k].name,
			   (nl_cmd_table[k].usage && nl_cmd_table[k].usage[0]) ?
			   nl_cmd_table[k].usage : "-",
			   nl_cmd_table[k].desc ? nl_cmd_table[k].desc : "");
	}
	printf("\nNote: any command not listed above is forwarded to the driver via CMD_WIFI_MP.\n");
}

int rtw_nl_cmd_try_dispatch(int argc, char *argv[], int *retval)
{
	int k;

	for (k = 0; nl_cmd_table[k].name != NULL; k++) {
		if (strcmp(argv[2], nl_cmd_table[k].name) == 0) {
			DBG("Netlink dispatch: %s\n", argv[2]);
			*retval = nl_cmd_table[k].handler(argc, argv);
			return 1;
		}
	}
	return 0;
}
