/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#if defined(CONFIG_BT_COEXIST)
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
// @enable
// AT+COEX=<type=enable>[,<value>]
// <value>: 0: disable, 1: enable
//
// @gnt
// AT+COEXBT=<type=gnt>,<value>
// <value>: wifi: gnt to wifi, bt: gnt to bt, auto: gnt by auto
//
static void fCOMMONCOEX(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int pid = 0, vid = 0;
	static int wl_slot = 0;
	bool coex_is_en = false;
	bool coex_ext_is_en = false;
	int i = 0, j = 0;

	if (!arg) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: No input args number!\r\n", CMD_NAME_COEX);
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: Wrong input args number!\r\n", CMD_NAME_COEX);
		error_no = 1;
		goto exit;
	}

	if ((strlen(argv[1]) == 0)) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Input <type>  is NULL\r\n", CMD_NAME_COEX);
		error_no = 1;
		goto exit;
	} else if (0 == strcasecmp(argv[1], "vendor")) {
		/* The parameters appear by pairs, so i += 2. */
		for (i = 2; argc > i; i += 2) {
			j = i + 1;  /* next i. */
			/* vid. */
			if (0 == strcasecmp("vid", argv[i])) {
				if ((argc <= j) || (strlen(argv[j]) == 0) || (atoi(argv[j]) > 0xff)) {
					RTK_LOGW(AT_COEX_TAG, "[AT%s] Invalid VID\r\n", CMD_NAME_COEX);
					error_no = 11;
					goto exit;
				}
				vid = atoi(argv[j]);
			}
			/* pid. */
			else if (0 == strcasecmp("pid", argv[i])) {
				if ((argc <= j) || (strlen(argv[j]) == 0) || (atoi(argv[j]) > 0xff)) {
					RTK_LOGW(AT_COEX_TAG, "[AT%s] Invalid PID\r\n", CMD_NAME_COEX);
					error_no = 12;
					goto exit;
				}
				pid = atoi(argv[j]);
			}
			/* Invalid input. */
			else {
				RTK_LOGW(AT_COEX_TAG, "[AT%s] Invalid parameter type\r\n", CMD_NAME_COEX);
				error_no = 13;
				goto exit;
			}
		}
		at_printf("pid=0x%x, vid=0x%x\r\n", pid, vid);
		rtk_coex_com_vendor_info_set(pid, vid);

	} else if (0 == strcasecmp(argv[1], "wl_slot")) {
		if (argc == 2) {
			at_printf("\r\nwl_slot=%d%%\r\n", wl_slot);
		} else if (atoi(argv[2]) > 100) {
			RTK_LOGW(AT_COEX_TAG, "[AT%s] Invalid wl_slot value\r\n", CMD_NAME_COEX);
			error_no = 21;
			goto exit;
		} else {
			wl_slot = atoi(argv[2]);
			at_printf("\r\nwl_slot=%d%%\r\n", wl_slot);
			rtk_coex_com_wl_slot_set(wl_slot);
		}

	} else if (0 == strcasecmp(argv[1], "state")) {
		rtk_coex_com_state_get();
	} else if (0 == strcasecmp(argv[1], "enable")) {
		if (argc == 2) {
			coex_is_en = rtk_coex_com_coex_is_enabled();
			coex_ext_is_en = rtk_coex_extc_is_ready();
			at_printf("\r\n[coex,ext]=%d,%d\r\n", coex_is_en, coex_ext_is_en);
		} else if (argc == 3) {
			rtk_coex_com_coex_set_enable((atoi(argv[2]) ? 1 : 0));
		} else {
			RTK_LOGW(AT_COEX_TAG, "[AT%s] Wrong Params Number\r\n", CMD_NAME_COEX);
			error_no = 22;
			goto exit;
		}
	} else if (0 == strcasecmp(argv[1], "gnt")) {
		if (argc == 3 && 0 == strcasecmp(argv[2], "wifi")) {
			rtk_coex_btc_set_pta(PTA_WIFI, 0, 0);
		} else if (argc == 3 && 0 == strcasecmp(argv[2], "bt")) {
			rtk_coex_btc_set_pta(PTA_BT, 0, 0);
		} else if (argc == 3 && 0 == strcasecmp(argv[2], "auto")) {
			rtk_coex_btc_set_pta(PTA_AUTO, 0, 0);
		} else {
			RTK_LOGW(AT_COEX_TAG, "[AT%s] Invalid Params\r\n", CMD_NAME_COEX);
			error_no = 23;
			goto exit;
		}
	} else if (0 == strcasecmp(argv[1], "help")) {
		at_printf("\r\n");
		at_printf("AT%s=<type=vendor>,vid,<vid>,pid,<pid>\r\n", CMD_NAME_COEX);
		at_printf("\t<vid>: 0-255 (dec)\r\n");
		at_printf("\t<pid>: 0-255 (dec)\r\n");
		at_printf("AT%s=<type=wl_slot>[,<value>]\r\n", CMD_NAME_COEX);
		at_printf("\t<value>: 0: disable, [1-100]: percent (dec)\r\n");
		at_printf("AT%s=<type=state>\r\n", CMD_NAME_COEX);
		at_printf("AT%s=<type=enable>[,<value>]\r\n", CMD_NAME_COEX);
		at_printf("\t<value>: 0: disable, 1: enable\r\n");
		at_printf("AT%s=<type=gnt>[,<value>]\r\n", CMD_NAME_COEX);
		at_printf("\t<value>: wifi: gnt to wifi, bt: gnt to bt, auto: gnt by auto\r\n");
	} else {
		error_no = 21;
	}

exit:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}

	return;
}

static void fBTCOEX(void *arg)
{
	(void)arg;

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
static void fEXTCOEX(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int chan_num = 0;

	if (!arg) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: No input args number!\r\n", CMD_NAME_EXTC);
		error_no = 1;
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: Wrong input args number!\r\n", CMD_NAME_EXTC);
		error_no = 1;
		goto exit;
	}

	if ((strlen(argv[1]) == 0)) {
		RTK_LOGE(AT_COEX_TAG, "[AT%s] Input <type>  is NULL\r\n", CMD_NAME_EXTC);
		error_no = 1;
		goto exit;
	} else if (0 == strcasecmp(argv[1], "chan")) {
		if (argc != 3) {
			RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: Wrong input args number!\r\n", CMD_NAME_EXTC);
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

static void fWPCOEX(void *arg)
{
	(void)arg;

	at_printf("%s is not supported\n", CMD_NAME_WPC);

}

ATCMD_TABLE_DATA_SECTION
static const log_item_t at_coex_items[] = {
	{CMD_NAME_COEX,          fCOMMONCOEX,              {NULL, NULL}},
	{CMD_NAME_BTC,          fBTCOEX,              {NULL, NULL}},
	{CMD_NAME_EXTC,          fEXTCOEX,              {NULL, NULL}},
	{CMD_NAME_WPC,          fWPCOEX,              {NULL, NULL}},
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

void at_coex_init(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)
	(void)at_coex_items;
#else
	atcmd_service_add_table((log_item_t *)at_coex_items, sizeof(at_coex_items) / sizeof(at_coex_items[0]));
#endif
}

#endif