/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "ameba_diagnose.h"

#include <stdlib.h>

static const char *TAG = "ATCMD_DIAG";
static RtkDiagDataFrame_t *g_private_frame_buffer = NULL;
static u16 g_sender_buffer_size = RTK_DIAG_SEND_BUFFER_SIZE;

static void at_diag_help(void)
{
	RTK_LOGI(TAG, "\n");
	RTK_LOGI(TAG, "AT+DIAG=VER: get event format hash value\n");
	RTK_LOGI(TAG, "AT+DIAG=TM: get soc timestamp\n");
	RTK_LOGI(TAG, "AT+DIAG=BUF_COM: set event transform buffer size\n");
	RTK_LOGI(TAG, "AT+DIAG=BUF_EVT: set event store buffer capacity\n");
	RTK_LOGI(TAG, "AT+DIAG=GET_EVT,<timestamp>,<offset>: get event\n");
	RTK_LOGI(TAG, "AT+DIAG=DEL_EVT_BF,<timestamp>: delete event before <timestamp>\n");
	RTK_LOGI(TAG, "AT+DIAG=DEL_EVT_AF,<timestamp>: delete event after <timestamp>\n");
	RTK_LOGI(TAG, "AT+DIAG=GET_DEL: get recent deleted event\n");
	RTK_LOGI(TAG, "AT+DIAG=LOG,<state>: enable or disable log\n");
#ifndef CONFIG_AMEBA_RLS
	RTK_LOGI(TAG, "AT+DIAG=DBG_ADD_EVT,<evt_level>,<data_str>: add a debug event\n");
	RTK_LOGI(TAG, "AT+DIAG=DBG_LOG,<state>: enable or disable diag debug log\n");
#endif
}

void at_diag(void *arg)
{
	/* These code cannot be invoked in at_diag_init because it rely on diagnose init */
	if (unlikely(g_private_frame_buffer == NULL)) {
		g_private_frame_buffer = (RtkDiagDataFrame_t *)rtos_mem_malloc(g_sender_buffer_size);
		if (NULL == g_private_frame_buffer) {
			RTK_LOGA(NOTAG, "Malloc failed\n");
		}
	}
	if (unlikely(!rtk_diag_transform_has_configed())) {
		rtk_diag_config_transform(NULL, (u8 *)g_private_frame_buffer, g_sender_buffer_size);
	}

	int argc = 0, ret = RTK_ERR_BADARG;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc <= 1) || (argv[1] == NULL)) {
		goto end;
	}
	const char *op = (const char *)argv[1];
	if (strcmp(op, "TM") == 0) {
		if (argc != 2) {
			goto end;
		}
		ret = rtk_diag_req_timestamp();
	} else if (strcmp(op, "BUF_COM") == 0) {
		if (argc != 3) {
			goto end;
		}
		u16 new_size = atoi(argv[2]);
		RtkDiagDataFrame_t *tmp = NULL;
		if (new_size >= RTK_DIAG_SEND_BUFFER_SIZE_MIN) {
			tmp = (RtkDiagDataFrame_t *)rtos_mem_malloc(new_size);
		}
		ret = rtk_diag_req_set_buf_com_capacity((u8 *)tmp, new_size);
		if (ret == RTK_SUCCESS) {
			rtos_mem_free(g_private_frame_buffer);
			g_private_frame_buffer = tmp;
			g_sender_buffer_size = new_size;
		}
	} else if (strcmp(op, "BUF_EVT") == 0) {
		if (argc != 3) {
			goto end;
		}
		ret = rtk_diag_req_set_buf_evt_capacity(atoi(argv[2]));
	} else if (strcmp(op, "VER") == 0) {
		if (argc != 2) {
			goto end;
		}
		ret = rtk_diag_req_version();
	} else if (strcmp(op, "GET_EVT") == 0) {
		if (argc != 4) {
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			goto end;
		}
		ret = rtk_diag_req_get_event(tm, atoi(argv[3]));
	} else if (strcmp(op, "DEL_EVT_BF") == 0) {
		if (argc != 3) {
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			goto end;
		}
		ret = rtk_diag_req_del_event_before(tm);
	} else if (strcmp(op, "DEL_EVT_AF") == 0) {
		if (argc != 3) {
			goto end;
		}
		char *endptr;
		u32 tm = _strtoul(argv[2], &endptr, 10);
		if (endptr == argv[2] || *endptr != '\0') {
			goto end;
		}
		ret = rtk_diag_req_del_event_after(tm);
	} else if (strcmp(op, "GET_DEL") == 0) {
		if (argc != 2) {
			goto end;
		}
		ret = rtk_diag_req_get_del_events();
	} else if (strcmp(op, "CLR_DEL") == 0) {
		if (argc != 2) {
			goto end;
		}
		ret = rtk_diag_req_clr_del_events();
#ifdef DIAG_DEBUG_TEST
	} else if (strcmp(op, "DBG_ADD_EVT") == 0) {
		if (argc != 4) {
			goto end;
		}
		ret = rtk_diag_req_add_event_demo1(atoi(argv[2]), argv[3]);
	} else if (strcmp(op, "DBG_LOG") == 0) {
		if (argc != 3) {
			goto end;
		}
		ret = rtk_diag_req_dbg_log_enable(atoi(argv[2]));
#endif
	} else if (strcmp(op, "LOG") == 0) {
		if (argc != 3) {
			goto end;
		}
		ret = rtk_diag_req_log_enable(atoi(argv[2]));
	} else {
		goto end;
	}

end:
	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (ret == RTK_ERR_BADARG) {
			RTK_LOGA("DIAG", "Invalid param.\n");
		}
		at_printf(ATCMD_ERROR_END_STR, ret);
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
