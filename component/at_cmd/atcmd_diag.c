/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "ameba_diagnose.h"

#include <stdlib.h>

static const char *TAG = "ATCMD_DIAG";

static void at_diag_help(void)
{
	RTK_LOGI(TAG, "\n");
	// RTK_LOGI(TAG, "AT+LOG=<get_set>,<module>[,<log_level>]\n");
	// RTK_LOGI(TAG, "\t<get_set>:\t0-get, 1-set, 2-print all, 3-clear all\n");
	// RTK_LOGI(TAG, "\t<module>:\t*-each module, others-specific module\n");
	// RTK_LOGI(TAG, "\t<log_level>:\t[0,5], only applicable for set mode\n");
}

void at_diag(void *arg)
{
	int argc = 0, ret = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGA(NOTAG, "[DIAG] ERROR arg: \n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc <= 1) || (argv[1] == NULL)) {
		RTK_LOGA(NOTAG, "[DIAG] ERROR arg: \n");
		error_no = 1;
		goto end;
	}
	const char *op = (const char *)argv[1];
	if (strcmp(op, "TM") == 0) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_timestamp();
	} else if (strcmp(op, "BUF_COM") == 0) {
		if (argc != 3) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_set_buf_com_capacity(atoi(argv[2]));
	} else if (strcmp(op, "BUF_EVT") == 0) {
		if (argc != 3) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_set_buf_evt_capacity(atoi(argv[2]));
		// } else if (strcmp(op, "BUF_EVT_POOL") == 0) {
		// 	if (argc != 3) {
		// 		RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
		// 		error_no = 1;
		// 		goto end;
		// 	}
		// 	ret = rtk_diag_req_set_buf_evt_pool_capacity(atoi(argv[2]));
	} else if (strcmp(op, "VER") == 0) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_version();
	} else if (strcmp(op, "GET_EVT") == 0) {
		if (argc != 4) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_get_event(tm, atoi(argv[3]));
	} else if (strcmp(op, "DEL_EVT_BF") == 0) {
		if (argc != 3) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_del_event_before(tm);
	} else if (strcmp(op, "DEL_EVT_AF") == 0) {
		if (argc != 3) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_del_event_after(tm);
	} else if (strcmp(op, "GET_DEL") == 0) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_get_del_events();
	} else if (strcmp(op, "CLR_DEL") == 0) {
		if (argc != 2) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_clr_del_events();
	} else if (strcmp(op, "DBG_ADD_EVT") == 0) {
		if (argc != 4) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		rtk_diag_req_add_event_demo1(atoi(argv[2]), argv[3]);
	} else if (strcmp(op, "LOG") == 0) {
		if (argc != 3) {
			RTK_LOGA(NOTAG, "[DIAG] Invalid parameter number.\n");
			error_no = 1;
			goto end;
		}
		ret = rtk_diag_req_log_enable(atoi(argv[2]));
	} else {
		RTK_LOGA(NOTAG, "[DIAG] ERROR arg unsupport: %s\n", op);
		error_no = 1;
		goto end;
	}

	if (ret != RTK_SUCCESS) {
		RTK_LOGA(NOTAG, "[DIAG] Failed %d when set.\n", ret);
		error_no = 2;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_diag_help();
	}
}

log_item_t at_diag_items[] = {
	{"+DIAG", at_diag, {NULL, NULL}},
};

void at_diag_init(void)
{
	atcmd_service_add_table(at_diag_items, sizeof(at_diag_items) / sizeof(at_diag_items[0]));
}
