/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <os_wrapper.h>
#include "ameba.h"
#include "atcmd_service.h"
#include "atcmd_coex.h"

#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_host_api.h"

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
static void fCOMMONCOEX(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int pid = 0, vid = 0;
	static int wl_slot = 0;
	int i = 0, j = 0;

	struct rtk_coex_vendor_info vendor_info = {.product_id = 0, .vendor_id = 0};

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
		vendor_info.product_id = pid;
		vendor_info.vendor_id = vid;
		rtk_coex_com_vendor_info_set((void *)&vendor_info, sizeof(struct rtk_coex_vendor_info));

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

	} else if (0 == strcasecmp(argv[1], "help")) {
		at_printf("\r\n");
		at_printf("AT%s=<type=vendor>,vid,<vid>,pid,<pid>\r\n", CMD_NAME_COEX);
		at_printf("\t<vid>: 0-255 (dec)\r\n");
		at_printf("\t<pid>: 0-255 (dec)\r\n");
		at_printf("AT%s=<type=wl_slot>[,<value>]\r\n", CMD_NAME_COEX);
		at_printf("\t<value>: 0: disable, [1-100]: percent (dec)\r\n");
		at_printf("AT%s=<type=state>\r\n", CMD_NAME_COEX);
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

}

static int pad_from_string_to_value(char *str)
{
	int value = 0;

	if (str == NULL) {
		return _PNC;
	}

	value = atoi(str + 1);
	if (str[0] == 'A' || str[0] == 'a') {
		return _PA_0 + value;
	} else if (str[0] == 'B' || str[0] == 'b') {
		return _PB_0 + value;
	}
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2)
	else if (str[0] == 'C' || str[0] == 'c') {
		return _PC_0 + value;
	}
#endif

	return _PNC;
}

static void at_coexext_help(void)
{
#if defined(AT_CMD_HELP) && AT_CMD_HELP
	at_printf("\r\n");
	at_printf("AT%s=<type=init>,<prot>,<pta_index>,<pad_req>,<pad_pri>,<pad_gnt>,\
				<gnt_out_use>,<trx_det_time>,<pri_det_time>[,<pri_mode>,<req_polar>,<gnt_polar>]\r\n", CMD_NAME_EXTC);
	at_printf("\t<prot>: 1: wpan; 2: bt; 3: wpan&bt\r\n");
	at_printf("\t<pta_index>: 0: pta1; 1: pta2\r\n");
	at_printf("\t<pad_req>: AXX, such as A12,A3,a4...\r\n");
	at_printf("\t<pad_pri>: BXX, such as B12,B3,b4...\r\n");
	at_printf("\t<pad_gnt>: AXX, such as A12,A3,a4...\r\n");
	at_printf("\t<gnt_out_use>: 0:default; 1:gnt_bt\r\n");
	at_printf("\t<trx_det_time>: 0-255(us)\r\n");
	at_printf("\t<pri_det_time>: 0-255(us)\r\n");
	at_printf("\t<pri_mode>: 0: directional priority; 1: static priority\r\n");
	at_printf("\t<req_polar>: 0: active low; 1: active high\r\n");
	at_printf("\t<gnt_polar>: 0: active low; 1: active high\r\n");
#endif
}

// @init:
// AT+COEXEXT=<type=init>,<prot>,<pta_index>,<pad_req>,<pad_pri>,<pad_gnt>,<gnt_out_use>,<trx_det_time>,<pri_det_time>[,<pri_mode>,<req_polar>,<gnt_polar>]
// <prot>: 1: wpan; 2: bt; 3: wpan&bt
// <pta_index>: 0: pta1; 1: pta2
// <pad_req>: AXX, such as A12,A3,a4...
// <pad_pri>: BXX, such as B12,B3,b4...
// <pad_gnt>: AXX, such as A12,A3,a4...
// <gnt_out_use>: 0:default; 1:gnt_bt
// <trx_det_time>: 0-255(us)
// <pri_det_time>: 0-255(us)
// <pri_mode>: 0: directional priority; 1: static priority
// <req_polar>: 0: active low; 1: active high
// <gnt_polar>: 0: active low; 1: active high
//
// @state:
// AT+COEXEXT=<type=state>,<state_value>
//
// @channel:
// AT+COEXEXT=<type=chan>,<chan_num>
static void fEXTCOEX(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int prot = 0;
	int pta_index = 0;
	int pad_req = 0, pad_pri = 0, pad_gnt = 0;
	int gnt_out_use = 1;
	int trx_det_time = 8;
	int pri_det_time = 2;
	int pri_mode = 0, req_polar = 1, gnt_polar = 0;
	int state_value = 0;
	int chan_num = 0;

	struct extchip_para_t extchip_para = {0};

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
	} else if (0 == strcasecmp(argv[1], "init")) {
		if (argc < 10) {
			RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: Wrong input args number!\r\n", CMD_NAME_EXTC);
			error_no = 11;
			goto exit;
		}

		prot = atoi(argv[2]);
		pta_index = atoi(argv[3]);
		pad_req = pad_from_string_to_value(argv[4]);
		pad_pri = pad_from_string_to_value(argv[5]);
		pad_gnt = pad_from_string_to_value(argv[6]);
		gnt_out_use = atoi(argv[7]);
		trx_det_time = atoi(argv[8]);
		pri_det_time = atoi(argv[9]);
		if (argc > 10) {
			pri_mode = atoi(argv[10]);
		}
		if (argc > 11) {
			req_polar = atoi(argv[11]);
		}
		if (argc > 12) {
			gnt_polar = atoi(argv[12]);
		}

		at_printf("prot = %d\r\n", prot);
		at_printf("pta_index = %d\r\n", pta_index);
		at_printf("pad_req = 0x%x\r\n", pad_req);
		at_printf("pad_pri = 0x%x\r\n", pad_pri);
		at_printf("pad_gnt = 0x%x\r\n", pad_gnt);
		at_printf("gnt_out_use = %d\r\n", gnt_out_use);
		at_printf("trx_det_time = %dus\r\n", trx_det_time);
		at_printf("pri_det_time = %dus\r\n", pri_det_time);
		at_printf("pri_mode = %d\r\n", pri_mode);
		at_printf("req_polar = %d\r\n", req_polar);
		at_printf("gnt_polar = %d\r\n", gnt_polar);

		extchip_para.pri_mode = pri_mode;
		extchip_para.pri_det_time = pri_det_time;
		extchip_para.trx_det_time = trx_det_time;
		extchip_para.req_polar = req_polar;
		extchip_para.gnt_polar = gnt_polar;
		if (prot == 1) {
			extchip_para.active_protocol = EXT_PTA_PROTOCOL_WPAN;
		} else if (prot == 2) {
			extchip_para.active_protocol = EXT_PTA_PROTOCOL_BT;
		} else if (prot == 3) {
			extchip_para.active_protocol = EXT_PTA_PROTOCOL_BT_WPAN;
		} else {
			extchip_para.active_protocol = EXT_PTA_PROTOCOL_UNDEF;
		}
		extchip_para.pta_pad_req = pad_req;
		extchip_para.pta_pad_pri = pad_pri;
		extchip_para.pta_pad_gnt = pad_gnt;
		extchip_para.pta_index = (pta_index == 0) ? EXT_PTA1 : EXT_PTA2;

		rtk_coex_extc_ntfy_init(&extchip_para);

	} else if (0 == strcasecmp(argv[1], "state")) {
		if (argc != 3) {
			RTK_LOGE(AT_COEX_TAG, "[AT%s] Error: Wrong input args number!\r\n", CMD_NAME_EXTC);
			error_no = 21;
			goto exit;
		}
		state_value = atoi(argv[2]);
		at_printf("state_value = %d\r\n", state_value);
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

static log_item_t at_coex_items[] = {
	{CMD_NAME_COEX,          fCOMMONCOEX,              {NULL, NULL}},
	{CMD_NAME_BTC,          fBTCOEX,              {NULL, NULL}},
	{CMD_NAME_EXTC,          fEXTCOEX,              {NULL, NULL}},
	{CMD_NAME_WPC,          fWPCOEX,              {NULL, NULL}},
};

/* coex atcmd as a part of AT command "AT+LIST". */
void print_coex_at(void)
{
#if ((defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED) && (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK))
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
#if ((defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED) && (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK))
	(void)at_coex_items;
#else
	atcmd_service_add_table(at_coex_items, sizeof(at_coex_items) / sizeof(at_coex_items[0]));
#endif
}

#endif