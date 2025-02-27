/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_OTA) && (CONFIG_ATCMD_OTA == 1)
#include "wifi_conf.h"
#include "atcmd_ota.h"

static const char *const TAG = "AT-OTA";

static int at_ota_status = 0;
extern void sys_reset(void);

static void at_ota_update_task(void *param)
{
	ota_context *ctx = (ota_context *)param;
	int ret = -1;

	ret = ota_update_start(ctx);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] update failed\r\n");
		at_printf_indicate("+OTA: UPDATE FAILED\r\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+OTA] update success\r\n");
		at_printf_indicate("+OTA: UPDATE SUCCESS\r\n");
	}

	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	at_ota_status = 0;

	rtos_task_delete(NULL);
}

static void at_ota_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+OTA=<type>,<host>,<port>,<resource>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<type>: \t%d~%d. OTA type.\r\n", OTA_TYPE_MIN, OTA_TYPE_MAX);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1: \tupdate from HTTP server\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t2: \tupdate from HTTPS server\r\n");
#ifdef CONFIG_AMEBADPLUS
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t3: \tupdate from VFS\r\n");
#elif defined(CONFIG_AMEBASMART)
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t3: \tupdate from SDCard\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t4: \tupdate from VFS\r\n");
#endif
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<host>:\thostname\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<port>:\t[1,65535]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<resource>:\tfirmware path, e.g. ota_all.bin\r\n");
}

/****************************************************************
AT command process:
	AT+OTA=<type>,<host>,<port>,<resource>
	Update firmware from host.
****************************************************************/
void at_ota(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	ota_context *ctx = NULL;
	char *host = NULL;
	char *resource = NULL;
	int argc = 0, ret = -1, type = 0, port = 0, err_no = 0;

	if (!arg) {
		at_ota_help();
		err_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 5) {
		err_no = 1;
		goto end;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");

	if (at_ota_status) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+OTA] ota is already running\r\n");
		err_no = 4;
		goto end;
	}

	type = atoi(argv[1]);
	if (type < OTA_TYPE_MIN || type > OTA_TYPE_MAX) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] Invalid type\r\n");
		err_no = 1;
		goto end;
	}

	if (strlen(argv[2]) != 0) {
		host = (char *)rtos_mem_malloc(strlen(argv[2]) + 1);
		if (!host) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] host malloc failed\r\n");
			err_no = 2;
			goto end;
		}
		strcpy(host, argv[2]);
		host[strlen(argv[2])] = '\0';
	}

	port = atoi(argv[3]);
	if (port < 0 || port > 65535) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] Invalid port\r\n");
		err_no = 1;
		goto end;
	}

	if (port == 0 && type <= OTA_HTTPS) {
		port = (type == OTA_HTTPS ? 443 : 8082);
	}

	if (strlen(argv[4]) == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] Invalid resource\r\n");
		err_no = 1;
		goto end;
	}

	resource = (char *)rtos_mem_malloc(strlen(argv[4]) + 1);
	if (!resource) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] resource malloc failed\r\n");
		err_no = 2;
		goto end;
	}
	strcpy(resource, argv[4]);
	resource[strlen(argv[4])] = '\0';

	/* Check whether the network is linked. */
	if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] The network is not ready\r\n");
		err_no = 3;
		goto end;
	}

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (!ctx) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] ctx malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	memset(ctx, 0, sizeof(ota_context));

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+OTA] host: %s, port: %d, resource: %s\r\n", host ? host : "null", port, resource);

	ret = ota_update_init(ctx, host, port, resource, type);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] ota_update_init failed\r\n");
		err_no = 5;
		goto end;
	}

	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"at_ota_update_task"), at_ota_update_task, (void *)ctx, 1024 * 8, 1) != SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] Create update task failed");
		err_no = 6;
		goto end;
	}
	at_ota_status = 1;

end:
	if (err_no != 0) {
		if (ctx) {
			rtos_mem_free(ctx);
		}
		if (host) {
			rtos_mem_free(host);
		}
		if (resource) {
			rtos_mem_free(resource);
		}
		at_printf(ATCMD_ERROR_END_STR, err_no);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_userota_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+USEROTA=<length>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<length>:\timage ota_all.bin length\r\n");
}

/****************************************************************
AT command process:
	AT+USEROTA=<length>
	Update firmware from mcu host.
****************************************************************/
void at_userota(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	ota_context *ctx = NULL;
	u8 *buffer = NULL;
	int argc = 0, ret = -1, err_no = 0;
	int frag_len = 0;
	int length = 0;

	if (!arg) {
		at_userota_help();
		err_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 2) {
		err_no = 1;
		goto end;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");

	if (at_ota_status) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+USEROTA] ota is already running\r\n");
		err_no = 4;
		goto end;
	}

	/* ota image length. */
	length = atoi(argv[1]);
	if (length <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+USEROTA] Invalid length\r\n");
		err_no = 1;
		goto end;
	}

	buffer = (u8 *)rtos_mem_zmalloc(BUF_SIZE);
	if (!buffer) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+USEROTA] Malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (!ctx) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+USEROTA] ctx malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	memset(ctx, 0, sizeof(ota_context));

	ret = ota_update_init(ctx, NULL, 0, NULL, OTA_USER);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+USEROTA] ota_update_init failed\r\n");
		err_no = 5;
		goto end;
	}
	at_ota_status = 1;

	/* tt mode */
	ret = atcmd_tt_mode_start(length);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+USEROTA] atcmd_tt_mode_start failed\r\n");
		err_no = 2;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+USEROTA] ota user download start, length: %d\r\n", length);

	while (length > 0) {
		frag_len = (length < BUF_SIZE) ? length : BUF_SIZE;
		atcmd_tt_mode_get(buffer, frag_len);
		ret = ota_update_fw_program(ctx, buffer, frag_len);
		if (ret == OTA_RET_FINISH) {
			at_printf(ATCMD_OK_END_STR);
			rtos_time_delay_ms(20);
			sys_reset();
		}
		if (ret == OTA_RET_ERR) {
			atcmd_tt_mode_end();
			err_no = 3;
			goto end;
		}
		length -= frag_len;
	}
	atcmd_tt_mode_end();
end:
	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	if (buffer) {
		rtos_mem_free(buffer);
	}
	at_ota_status = 0;
	if (err_no != 0) {
		at_printf(ATCMD_ERROR_END_STR, err_no);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}


log_item_t at_ota_items[] = {
	{"+OTA", at_ota, {NULL, NULL}},
	{"+USEROTA", at_userota, {NULL, NULL}},
};

void print_ota_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_ota_items) / sizeof(at_ota_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_ota_items[index].log_cmd);
	}
}

void at_ota_init(void)
{
	atcmd_service_add_table(at_ota_items, sizeof(at_ota_items) / sizeof(at_ota_items[0]));
}

#endif /* CONFIG_ATCMD_OTA */
#endif /* CONFIG_LWIP_LAYER */