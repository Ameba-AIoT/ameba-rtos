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
#include "wifi_api.h"
#include "atcmd_ota.h"

static const char *const TAG = "AT-OTA";

static int at_ota_status = 0;
extern void sys_reset(void);

static int ota_set_ssl_certificate(char **dest, CERT_TYPE cert_type, int index)
{
	char *cert_buffer = NULL;
	int cert_size = 0;
	int ret = 0;

	cert_size = atcmd_get_ssl_certificate_size(cert_type, index);
	if (cert_size <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%d get size %d\r\n", cert_type, cert_size);
		return -1;
	}

	cert_buffer = (char *)rtos_mem_zmalloc(cert_size);
	if (cert_buffer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "cert_buffer malloc failed\r\n");
		return -1;
	}
	ret = atcmd_get_ssl_certificate(cert_buffer, cert_type, index);
	if (ret <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%d get failed\r\n", cert_type);
		rtos_mem_free(cert_buffer);
		return -1;
	}

	*dest = cert_buffer;

	return 0;
}

static void at_otahttp_progress(int percent)
{
	static int percent_bak = -1;
	if (percent_bak != percent && percent % 10 == 0) {
		percent_bak = percent;
		at_printf_indicate("[OTA][HTTP]: UPDATE %d%%\r\n", percent);
	}
}

static void at_ota_update_task(void *param)
{
	ota_context *ctx = (ota_context *)param;
	int ret = -1;

	ret = ota_update_start(ctx);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+OTA] update failed\r\n");
		at_printf_indicate("[OTA][HTTP]: UPDATE FAILED\r\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+OTA] update success\r\n");
		at_printf_indicate("[OTA][HTTP]: UPDATE SUCCESS\r\n");
		if (at_ota_status & OTA_STATUS_SYSRST) {
			rtos_time_delay_ms(20);
			sys_reset();
		}
	}

	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
		ctx = NULL;
	}
	at_ota_status = 0;

	rtos_task_delete(NULL);
}

static void at_otahttp_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+OTAHTTP=<sysrst>,<host>,<path>[,<port>],<conn_type>[,<cert_index>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<sysrst>:\t1: the system will reset after ota finished. 0: the system will not reset after ota finished. Default:0\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<host>:\thostname\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<resource>:\tfirmware path, e.g. ota_all.bin\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<port>:\t[0,65535]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<conn_type>:\t[1,5]. Connection type.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1:\tHTTP.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t2:\tHTTPS (no certificate verify).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t3:\tHTTPS (verify server certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t4:\tHTTPS (provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t5:\tHTTPS (verify server certificate and provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<cert_index>:\tcertificate index.\r\n");
}

/****************************************************************
AT command process:
	AT+OTAHTTP=<sysrst>,<host>,<path>[,<port>],<conn_type>[,<cert_index>]
	Update firmware from host.
****************************************************************/
void at_otahttp(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	ota_context *ctx = NULL;
	char *host = NULL;
	char *resource = NULL;
	char *ca_cert = NULL;
	char *client_cert = NULL;
	char *client_key = NULL;
	int argc = 0, ret = -1, port = 0, err_no = 0;
	int sysrst = 0, conn_type = 0, cert_index = 0;
	int ota_type = OTA_HTTP;

	if (!arg) {
		at_otahttp_help();
		err_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 6) {
		err_no = 1;
		goto end;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");

	if (at_ota_status & OTA_STATUS_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ota is already running\r\n");
		err_no = 4;
		goto end;
	}

	sysrst = atoi(argv[1]);
	if (sysrst != 0 && sysrst != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid sysrst\r\n");
		err_no = 1;
		goto end;
	}

	if (strlen(argv[2]) == 0 || strlen(argv[2]) > OTA_MAX_HOSTNAME_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid host\r\n");
		err_no = 1;
		goto end;
	}


	if (strlen(argv[3]) == 0 || strlen(argv[3]) > OTA_MAX_PATH_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid path\r\n");
		err_no = 1;
		goto end;
	}


	if (strlen(argv[4]) != 0) {
		port = atoi(argv[4]);
		if (port < 0 || port > 65535) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid port\r\n");
			err_no = 1;
			goto end;
		}
	}

	conn_type = atoi(argv[5]);
	if (conn_type < OTA_HTTP_OVER_TCP || conn_type > OTA_HTTPS_VERIFY_BOTH) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid type\r\n");
		err_no = 1;
		goto end;
	}

	if (argc == 7) {
		cert_index = atoi(argv[6]);

		if (conn_type == OTA_HTTPS_VERIFY_SERVER || conn_type == OTA_HTTPS_VERIFY_BOTH) {
			ret = ota_set_ssl_certificate(&ca_cert, CLIENT_CA, cert_index);
			if (ret != 0) {
				err_no = 6;
				goto end;
			}
			ota_type = OTA_HTTPS;
		}

		if (conn_type == OTA_HTTPS_VERIFY_CLIENT || conn_type == OTA_HTTPS_VERIFY_BOTH) {
			ret = ota_set_ssl_certificate(&client_cert, CLIENT_CERT, cert_index);
			if (ret != 0) {
				err_no = 6;
				goto end;
			}
			ret = ota_set_ssl_certificate(&client_key, CLIENT_KEY, cert_index);
			if (ret != 0) {
				err_no = 6;
				goto end;
			}
			ota_type = OTA_HTTPS;
		}
	}

	host = argv[2];
	resource = argv[3];
	port = (port == 0) ? (ota_type == OTA_HTTPS ? 443 : 80) : port;
	if (sysrst) {
		at_ota_status |= OTA_STATUS_SYSRST;
	}

	ctx = (ota_context *)rtos_mem_zmalloc(sizeof(ota_context));
	if (!ctx) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ctx malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "host: %s, port: %d, resource: %s\r\n", host, port, resource);

	ret = ota_update_init(ctx, host, port, resource, ota_type);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ota_update_init failed\r\n");
		err_no = 3;
		goto end;
	}

	if (ca_cert) {
		ctx->ca_cert = ca_cert;
	}
	if (client_cert) {
		ctx->client_cert = client_cert;
	}
	if (client_key) {
		ctx->private_key = client_key;
	}
	ota_update_register_progress_cb(ctx, at_otahttp_progress);

	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"at_ota_update_task"), at_ota_update_task, (void *)ctx, 1024 * 10, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create update task failed");
		err_no = 5;
		goto end;
	}
	at_ota_status |= OTA_STATUS_RUNNING;

end:
	if (err_no != 0) {
		ota_update_deinit(ctx);
		if (ctx) {
			rtos_mem_free(ctx);
			ctx = NULL;
		}
		at_ota_status = 0;
		at_printf(ATCMD_ERROR_END_STR, err_no);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_otauser_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+OTAUSER=<sysrst>,<length>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<sysrst>:\t1: the system will reset after ota finished. 0: the system will not reset after ota finished. Default:0\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<length>:\timage ota_all.bin length\r\n");
}

/****************************************************************
AT command process:
	AT+OTAUSER=<sysrst>,<length>
	Update firmware from mcu host.
****************************************************************/
void at_otauser(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	ota_context *ctx = NULL;
	u8 *buffer = NULL;
	int argc = 0, ret = -1, err_no = 0;
	int frag_len = 0;
	int length = 0, sysrst = 0;

	if (!arg) {
		at_otauser_help();
		err_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 3) {
		err_no = 1;
		goto end;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");

	if (at_ota_status & OTA_STATUS_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ota is already running\r\n");
		err_no = 4;
		goto end;
	}

	sysrst = atoi(argv[1]);
	if (sysrst != 0 && sysrst != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid sysrst\r\n");
		err_no = 1;
		goto end;
	}

	/* ota image length. */
	length = atoi(argv[2]);
	if (length <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid length\r\n");
		err_no = 1;
		goto end;
	}

	buffer = (u8 *)rtos_mem_zmalloc(BUF_SIZE);
	if (!buffer) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (!ctx) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ctx malloc failed\r\n");
		err_no = 2;
		goto end;
	}

	memset(ctx, 0, sizeof(ota_context));

	ret = ota_update_init(ctx, NULL, 0, NULL, OTA_USER);
	if (ret != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ota_update_init failed\r\n");
		err_no = 3;
		goto end;
	}
	at_ota_status |= OTA_STATUS_RUNNING;

	/* tt mode */
	ret = atcmd_tt_mode_start(length);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "atcmd_tt_mode_start failed\r\n");
		err_no = 5;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "ota user download start, length: %d, sysrst: %d\r\n", length, sysrst);

	while (length > 0) {
		frag_len = (length < BUF_SIZE) ? length : BUF_SIZE;
		ret = atcmd_tt_mode_get(buffer, frag_len);
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "host stops tt mode\r\n");
			err_no = 5;
			break;
		}
		ret = ota_update_fw_program(ctx, buffer, frag_len);
		if (ret == OTA_RET_FINISH) {
			at_printf(ATCMD_OK_END_STR);
			if (sysrst) {
				rtos_time_delay_ms(20);
				sys_reset();
			}
		}
		if (ret == OTA_RET_ERR) {
			err_no = 6;
			break;
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
	{"+OTAHTTP", at_otahttp, {NULL, NULL}},
	{"+OTAUSER", at_otauser, {NULL, NULL}},
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