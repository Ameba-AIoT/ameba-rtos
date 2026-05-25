/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#if defined(CONFIG_BT_COEXIST)
#ifndef CONFIG_MP_SHRINK
#include "rtw_coex_host_api.h"
#include "platform_stdlib.h"
#include "atcmd_service.h"
#include "atcmd_coex.h"

#define AT_CMD_HELP	1

#define CMD_NAME_COEX	"+COEX"
#define CMD_NAME_BTC	"+COEXBT"
#define CMD_NAME_EXTC	"+COEXEXT"
#define CMD_NAME_WPC	"+COEXWP"

static const char *const AT_COEX_TAG = "AT_COEX";

// @vendor
// AT+COEX=<type=vendor>,vid,<vid>,pid,<pid>
// <vid>: 0-255 (dec)
// <pid>: 0-255 (dec)
//
// @wl_slot
// AT+COEX=<type=wl_slot>[,<value>]
// <value>: 0: disable, [1-100]: valid value (dec, percent)
//
// @state
// AT+COEX=<type=state>
//
// @dbg
// AT+COEX=<type=dbg>[,<dbgcmd>]
// <dbgcmd>: enable[,<value>](NULL: get coex disable or enable, 0: disable coex, 1: enable coex)
// <dbgcmd>: log[,<value>](0xFF: enable all coex log, 0: disable all coex log)
// <dbgcmd>: gnt[,<value>](wifi: gnt to wifi, bt: gnt to bt, auto: gnt by auto)
//

#define RTK_COEX_CMD_MAX_LEN  64
static void fCOMMONCOEX(u16 argc, char **argv)
{

	int error_no = 0;
	static int wl_slot = 0;

	if (argc < 2) {
		error_no = 1;
		goto exit;
	}

	/* -------------------------------------------------------
	 * Construct command string from argv[1..argc-1]
	 * e.g. argv = ["COEX", "vendor", "vid", "10", "pid", "20"]
	 *      cmd  = "vid 10 pid 20"
	 * ----------------------------------------------------- */
	size_t cmd_size = 1;
	for (u8 k = 1; k < argc; k++) {
		cmd_size += strlen(argv[k]);
		if (k > 1) {
			cmd_size += 1;
		}
	}
	cmd_size = (cmd_size < RTK_COEX_CMD_MAX_LEN) ? cmd_size : RTK_COEX_CMD_MAX_LEN;

	char cmd[RTK_COEX_CMD_MAX_LEN] = {0};
	int  pos = 0;

	for (u8 k = 1; k < argc; k++) {
		int remaining = (int)cmd_size - 1 - pos;  /* reserve space for '\0' */
		if (remaining <= 0) {
			break;
		}
		/* insert space before each argument except the first */
		if (k > 1) {
			cmd[pos++] = ' ';
			remaining--;
			if (remaining <= 0) {
				break;
			}
		}

		int copy_len = (int)strlen(argv[k]);
		if (copy_len > remaining) {
			copy_len = remaining;
		}
		memcpy(cmd + pos, argv[k], (size_t)copy_len);
		pos += copy_len;
	}

	cmd[pos] = '\0'; 	/* null-terminate */

	/* -------------------------------------------------------
	 * get subcmd
	 * ----------------------------------------------------- */
	char subcmd[32] = {0};
	if (_sscanf_ss(cmd, "%31s", subcmd, (unsigned)sizeof(subcmd)) != 1) {
		error_no = 1;
		goto exit;
	}

	/* -------------------------------------------------------
	 * dispatch subcmd
	 * ----------------------------------------------------- */
	if (0 == strcasecmp(subcmd, "vendor")) {
		int pid = -1, vid = -1;
		int matched = 0;

		matched = _sscanf_ss(cmd, "vid %d pid %d", &vid, &pid);
		if (matched != 2) {
			matched = _sscanf_ss(cmd, "pid %d vid %d", &pid, &vid);
		}
		if (matched != 2) {
			error_no = 10;
			goto exit;
		}
		if (vid < 0 || vid > 0xFF || pid < 0 || pid > 0xFF) {
			error_no = 11;
			goto exit;
		}

		RTK_LOGW(AT_COEX_TAG, "pid=0x%x, vid=0x%x\r\n", pid, vid);
		rtk_coex_com_vendor_info_set(pid, vid);

	} else if (0 == strcasecmp(subcmd, "wl_slot")) {
		const char *param = cmd + strlen(subcmd);
		while (*param == ' ') {
			param++;
		}
		if (*param == '\0') {
			RTK_LOGW(AT_COEX_TAG, "get wl_slot=%d\r\n", wl_slot);
		} else {
			int val = -1;
			if (_sscanf_ss(cmd, "%d", &val) != 1 || val < 0 || val > 100) {
				error_no = 21;
				goto exit;
			}
			wl_slot = val;
			RTK_LOGW(AT_COEX_TAG, "set wl_slot=%d\r\n", wl_slot);
			rtk_coex_com_wl_slot_set(wl_slot);
		}

	} else if (0 == strcasecmp(subcmd, "state")) {
		rtk_coex_com_state_get();

	} else if (0 == strcasecmp(subcmd, "dbg")) {
		rtk_coex_com_dbg(cmd + 4, cmd_size - 4);

	} else {
		RTK_LOGW(AT_COEX_TAG, "Unknown cmd\r\n");
		error_no = 31;
	}

exit:
	if (error_no) {
		RTK_LOGW(AT_COEX_TAG, "ERR:%d\r\n", error_no);
	}
}

static void fBTCOEX(u16 argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	at_printf("%s is not supported\n", CMD_NAME_BTC);

	return;
}

static void at_coexext_help(void)
{
#if defined(AT_CMD_HELP) && AT_CMD_HELP
	at_printf("\r\n");
	at_printf("AT%s=<type=chan>,<chan_num>[11-26]\r\n", CMD_NAME_EXTC);

#endif
}

// @channel:
// AT+COEXEXT=<type=chan>,<chan_num>
static void fEXTCOEX(u16 argc, char **argv)
{
	int error_no = 0;
	int chan_num = 0;

	if (argc < 2) {
		error_no = 1;
		goto exit;
	}

	if ((strlen(argv[1]) == 0)) {
		error_no = 1;
		goto exit;
	} else if (0 == strcasecmp(argv[1], "chan")) {
		if (argc != 3) {
			error_no = 31;
			goto exit;
		}
		chan_num = atoi(argv[2]);
		if (chan_num >= 11 && chan_num <= 26) {
			at_printf("chan_num = %d\r\n", chan_num);
		} else {
			error_no = 32;
			goto exit;
		}
	} else if (0 == strcasecmp(argv[1], "help")) {
		at_coexext_help();
	} else {
		error_no = 41;
	}

exit:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
		if (error_no > 10 && error_no < 20) {
			at_coexext_help();
		}
	}

	return;
}

static void fWPCOEX(u16 argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	at_printf("%s is not supported\n", CMD_NAME_WPC);

}

ATCMD_APONLY_TABLE_DATA_SECTION
static const log_item_t at_coex_items[] = {
	{CMD_NAME_COEX,          fCOMMONCOEX},
	{CMD_NAME_BTC,          fBTCOEX},
	{CMD_NAME_EXTC,          fEXTCOEX},
	{CMD_NAME_WPC,          fWPCOEX},
};

/* coex atcmd as a part of AT command "AT+LIST". */
void print_coex_at(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)
	//Print nothing
#else

	int index;
	int num = 0;

	num = sizeof(at_coex_items) / sizeof(at_coex_items[0]);
	for (index = 0; index < num; index++) {
		at_printf("AT%s\r\n", at_coex_items[index].log_cmd);
	}
#endif
}
#endif /* CONFIG_MP_SHRINK */
#endif /* CONFIG_BT_COEXIST */