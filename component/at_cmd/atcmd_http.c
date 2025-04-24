/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_HTTP) && (CONFIG_ATCMD_HTTP == 1)

#include "ameba.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_http.h"
#include "httpc/httpc.h"
#include "lwip/opt.h"

static const char *const AT_HTTP_TAG = "AT_HTTP";

static int http_timeout = 10;	//Set by AT+HTTPCONF; Default value is 10s
static int http_server_port = 0;	//Set by AT+HTTPCONF; Default value is 80 for HTTP and 443 for HTTPS

static int g_http_req_header_cnt = 0;	//The maximum value is HTTP_GLOBAL_REQ_HEADER_NUM
static char *g_http_req_header[HTTP_GLOBAL_REQ_HEADER_NUM] = {0};	//Set by AT+HTTPHEADER


//AT+HTTPCONF=<timeout>[,<port>]
void at_httpconf(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int http_to = 0, http_srv_port = 0;

	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > MAX_ARGC)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) == 0)  {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}
	http_to = atoi(argv[1]);
	if (http_to < 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] HTTP timeout setting value is incorrect\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[2]))  {
		http_srv_port = atoi(argv[2]);
		if ((http_srv_port < 0) || (http_srv_port > 65535)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] HTTP server port setting value is incorrect\r\n");
			error_no = 1;
			goto end;
		}
	}
	http_timeout = http_to;
	http_server_port = http_srv_port;

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


//AT+HTTPHEADER=<req_header_len>
void at_httpheader(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int req_header_len = 0;

	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpheader] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpheader] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	req_header_len = atoi(argv[1]);
	if ((req_header_len < 0) || (req_header_len >= MAX_TT_BUF_LEN)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpheader] HTTP request header length is incorrect\r\n");
		error_no = 1;
		goto end;
	}

	if (req_header_len > 0) {
		if (g_http_req_header_cnt >= HTTP_GLOBAL_REQ_HEADER_NUM) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpheader] The maximum of HTTP request header reached\r\n");
			error_no = 1;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)req_header_len) < 0)  {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpheader] Enter TT mode failed\r\n");
			error_no = 3;
			goto end;
		}
		g_http_req_header[g_http_req_header_cnt] = rtos_mem_zmalloc(req_header_len + 1);
		if (g_http_req_header[g_http_req_header_cnt] == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpheader] g_http_req_header[%d] malloc fail\r\n", g_http_req_header_cnt);
			error_no = 2;
			goto end;
		}
		if (atcmd_tt_mode_get((u8 *)g_http_req_header[g_http_req_header_cnt], (u32)req_header_len) != req_header_len)  {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpheader] Get data failed in TT mode\r\n");
			error_no = 3;
			goto end;
		}
		atcmd_tt_mode_end();
		g_http_req_header_cnt++;
	} else if (req_header_len == 0) {
		while (g_http_req_header_cnt) {
			g_http_req_header_cnt--;
			rtos_mem_free(g_http_req_header[g_http_req_header_cnt]);
			g_http_req_header[g_http_req_header_cnt] = NULL;
		}
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if ((g_http_req_header_cnt < HTTP_GLOBAL_REQ_HEADER_NUM) && (g_http_req_header[g_http_req_header_cnt] != NULL)) {
			rtos_mem_free(g_http_req_header[g_http_req_header_cnt]);
			g_http_req_header[g_http_req_header_cnt] = NULL;
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


void print_global_http_config(void)
{
	int i = 0;
	at_printf("\r\n");
	at_printf("Global HTTP configuration:\r\n");
	at_printf("http_timeout: %d\r\n", http_timeout);
	at_printf("http_server_port: %d\r\n", http_server_port);
	at_printf("http_req_header_cnt: %d\r\n", g_http_req_header_cnt);
	if (g_http_req_header_cnt) {
		at_printf("http_req_header_list:\r\n");
	}
	for (i = 0; i < g_http_req_header_cnt; i++) {
		at_printf("%s\r\n", g_http_req_header[i]);
	}
}


//AT+HTTPQUERY
void at_httpquery(void *arg)
{
	int error_no = 0;

	if (arg != NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpquery] No need input parameter\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		print_global_http_config();
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


//AT+HTTPGET=<host>,<path>,<conn_type>[,<certificate_index>][,<req_header_cnt>,<req_header>...<req_header>]
void at_httpget(void *arg)
{
	int argc = 0, error_no = 0, ret = -1;
	char *argv[MAX_ARGC] = {0};
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	int cert_size = 0, pk_size = 0;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	uint8_t *response_data = NULL;
	int read_size = 0;
	uint32_t total_resp_body_size = 0;


	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc < 4) || (argc > MAX_ARGC)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) == 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Input <host> is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) > DNS_MAX_NAME_LENGTH) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Input <host> length should not exceed DNS_MAX_NAME_LENGTH\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[2]) == 0) {
		argv[2] = "/";
	}

	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Connect type is incorrect\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL)  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Certificate index is NULL\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Certificate index is incorrect\r\n");
			error_no = 1;
			goto end;
		}
	}

	if (argc > 5) {
		header_cnt = atoi(argv[5]);
		if ((header_cnt == 0) || (header_cnt != argc - 6)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpget] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = 1;
			goto end;
		}
		for (i = 1; i <= header_cnt; i++) {
			find_result = strstr(argv[5 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpget] The input request header is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index)) <= 0)  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] Failed to get CLIENT_CA_%d size!\r\n", cert_index);
			error_no = 3;
			goto end;
		}
		http_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_ca_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] http_client_ca_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_ca_pem, CLIENT_CA, cert_index);
		if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else if (ret < 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index)) <= 0))  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size!\r\n", cert_index, cert_index);
			error_no = 3;
			goto end;
		}
		http_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_cert_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] http_client_cert_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_cert_pem, CLIENT_CERT, cert_index);
		if (ret > 0) {
			http_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (http_client_pkey_pem == NULL) {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpget] http_client_pkey_pem malloc fail\r\n");
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(http_client_pkey_pem, CLIENT_KEY, cert_index);
			if (ret <= 0)  {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpget] CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 3;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpget] CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}

	switch (conn_type) {
	case HTTP_OVER_TCP:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_NO_VERIFY:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_SERVER:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, http_client_ca_pem);
		break;
	default:
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Connect type is incorrect\r\n");
		error_no = 1;
		break;
	}

	if (!conn_ptr) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpget] conn_ptr is NULL\r\n");
		error_no = 2;
		goto end;
	}
	httpc_enable_ignore_content_len(conn_ptr);

	if (http_server_port == 0)  {
		if (conn_type == HTTP_OVER_TCP) {
			ret = httpc_conn_connect(conn_ptr, argv[1], 80, http_timeout);
		} else {
			ret = httpc_conn_connect(conn_ptr, argv[1], 443, http_timeout);
		}
	} else  {
		ret = httpc_conn_connect(conn_ptr, argv[1], (uint16_t)http_server_port, http_timeout);
	}

	if (ret != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_conn_connect failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, "GET", argv[2], NULL, 0) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_request_write_header_start failed\r\n");
		error_no = 4;
		goto end;
	}

	for (i = 1; i <= header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, argv[5 + i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	for (i = 0; i < g_http_req_header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, g_http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_request_write_header_finish failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_response_read_header failed\r\n");
		error_no = 5;
		goto end;
	}

	at_printf("+HTTPGET: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPGET: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPGET: HEADER DUMP END\r\n");
	at_printf_unlock();

	response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "[at_httpget] response_data malloc fail\r\n");
		error_no = 2;
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPGET: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
			if (total_resp_body_size >= conn_ptr->response.content_len) {
				break;
			}
		}
		at_printf("+HTTPGET: BODY DUMP END\r\n");
		at_printf("+HTTPGET: BODY LEN = %d\r\n", conn_ptr->response.content_len);
	} else {    //Some websites’ response headers do not include the content length, ex: Transfer-Encoding: chunked
		at_printf("+HTTPGET: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpget] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
		}
		at_printf("+HTTPGET: BODY DUMP END\r\n");
		at_printf("+HTTPGET: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(http_client_ca_pem);
		http_client_ca_pem = NULL;
		rtos_mem_free(http_client_cert_pem);
		http_client_cert_pem = NULL;
		rtos_mem_free(http_client_pkey_pem);
		http_client_pkey_pem = NULL;
	}
	if (conn_ptr) {
		httpc_conn_close(conn_ptr);
		httpc_conn_free(conn_ptr);
	}

	rtos_mem_free(response_data);
	response_data = NULL;

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


//AT+HTTPPOST=<host>,<path>,<conn_type>[,<certificate_index>],<body_size>[,<req_header_cnt>,<req_header>...<req_header>]
void at_httppost(void *arg)
{
	int argc = 0, error_no = 0, ret = -1;
	char *argv[MAX_ARGC] = {0};
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	int cert_size = 0, pk_size = 0;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	int total_post_body_size = 0, single_post_size = 0;
	u8 *post_data_buffer = NULL;
	uint8_t *response_data = NULL;
	int read_size = 0;
	uint32_t total_resp_body_size = 0;


	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc < 5) || (argc > MAX_ARGC)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) == 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Input <host> is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) > DNS_MAX_NAME_LENGTH) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Input <host> length should not exceed DNS_MAX_NAME_LENGTH\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[2]) == 0) {
		argv[2] = "/";
	}

	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Connect type is incorrect\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL)  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Certificate index is NULL\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Certificate index is incorrect\r\n");
			error_no = 1;
			goto end;
		}
	}

	total_post_body_size = atoi(argv[5]);
	if (total_post_body_size < 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Total length of POST is incorrect\r\n");
		error_no = 1;
		goto end;
	}

	if (argc > 6) {
		header_cnt = atoi(argv[6]);
		if ((header_cnt == 0) || (header_cnt != argc - 7)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httppost] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = 1;
			goto end;
		}
		for (i = 1; i <= header_cnt; i++) {
			find_result = strstr(argv[6 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httppost] The input request header is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index)) <= 0)  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] Failed to get CLIENT_CA_%d size!\r\n", cert_index);
			error_no = 3;
			goto end;
		}
		http_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_ca_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] http_client_ca_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_ca_pem, CLIENT_CA, cert_index);
		if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else if (ret < 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index)) <= 0))  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size!\r\n", cert_index, cert_index);
			error_no = 3;
			goto end;
		}
		http_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_cert_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] http_client_cert_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_cert_pem, CLIENT_CERT, cert_index);
		if (ret > 0) {
			http_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (http_client_pkey_pem == NULL) {
				RTK_LOGW(AT_HTTP_TAG, "[at_httppost] http_client_pkey_pem malloc fail\r\n");
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(http_client_pkey_pem, CLIENT_KEY, cert_index);
			if (ret <= 0)  {
				RTK_LOGW(AT_HTTP_TAG, "[at_httppost] CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 3;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}

	switch (conn_type) {
	case HTTP_OVER_TCP:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_NO_VERIFY:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_SERVER:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, http_client_ca_pem);
		break;
	default:
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Connect type is incorrect\r\n");
		error_no = 1;
		break;
	}

	if (!conn_ptr) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] conn_ptr is NULL\r\n");
		error_no = 2;
		goto end;
	}
	httpc_enable_ignore_content_len(conn_ptr);

	if (http_server_port == 0)  {
		if (conn_type == HTTP_OVER_TCP) {
			ret = httpc_conn_connect(conn_ptr, argv[1], 80, http_timeout);
		} else {
			ret = httpc_conn_connect(conn_ptr, argv[1], 443, http_timeout);
		}
	} else  {
		ret = httpc_conn_connect(conn_ptr, argv[1], (uint16_t)http_server_port, http_timeout);
	}

	if (ret != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_conn_connect failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, "POST", argv[2], NULL, (size_t)total_post_body_size) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_header_start failed\r\n");
		error_no = 4;
		goto end;
	}

	for (i = 1; i <= header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, argv[6 + i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	for (i = 0; i < g_http_req_header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, g_http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_header_finish failed\r\n");
		error_no = 4;
		goto end;
	}

	if (total_post_body_size > 0)  {
		post_data_buffer = rtos_mem_zmalloc((total_post_body_size <= MAX_TT_BUF_LEN ? total_post_body_size : MAX_TT_BUF_LEN) + 1);
		if (post_data_buffer == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httppost] post_data_buffer malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)total_post_body_size) < 0)  {
			RTK_LOGI(AT_HTTP_TAG, "[at_httppost] Enter TT mode failed\r\n");
			error_no = 5;
			goto end;
		}
		if (total_post_body_size <= MAX_TT_BUF_LEN)  {
			if (atcmd_tt_mode_get(post_data_buffer, (u32)total_post_body_size) != total_post_body_size)  {
				RTK_LOGI(AT_HTTP_TAG, "[at_httppost] Get data failed in TT mode\r\n");
				error_no = 5;
				goto end;
			}
			if (httpc_request_write_data(conn_ptr, (uint8_t *)post_data_buffer, (size_t)total_post_body_size) != total_post_body_size)  {
				RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_data failed\r\n");
				error_no = 6;
				goto end;
			}
		} else  {
			while (total_post_body_size > 0)  {
				single_post_size = (total_post_body_size <= MAX_TT_BUF_LEN) ? total_post_body_size : MAX_TT_BUF_LEN;
				if (atcmd_tt_mode_get(post_data_buffer, (u32)single_post_size) != single_post_size)  {
					RTK_LOGI(AT_HTTP_TAG, "[at_httppost] Get data failed in TT mode\r\n");
					error_no = 5;
					goto end;
				}
				if (httpc_request_write_data(conn_ptr, (uint8_t *)post_data_buffer, (size_t)single_post_size) != single_post_size)  {
					RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_request_write_data failed\r\n");
					error_no = 6;
					goto end;
				}
				total_post_body_size -= single_post_size;
			}
		}
		atcmd_tt_mode_end();

		rtos_mem_free(post_data_buffer);
		post_data_buffer = NULL;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_response_read_header failed\r\n");
		error_no = 7;
		goto end;
	}

	at_printf("+HTTPPOST: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPPOST: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPPOST: HEADER DUMP END\r\n");
	at_printf_unlock();

	response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "[at_httppost] response_data malloc fail\r\n");
		error_no = 2;
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPPOST: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
			if (total_resp_body_size >= conn_ptr->response.content_len) {
				break;
			}
		}
		at_printf("+HTTPPOST: BODY DUMP END\r\n");
		at_printf("+HTTPPOST: BODY LEN = %d\r\n", conn_ptr->response.content_len);
	} else {    //Some websites’ response headers do not include the content length, ex: Transfer-Encoding: chunked
		at_printf("+HTTPPOST: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
		}
		at_printf("+HTTPPOST: BODY DUMP END\r\n");
		at_printf("+HTTPPOST: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(http_client_ca_pem);
		http_client_ca_pem = NULL;
		rtos_mem_free(http_client_cert_pem);
		http_client_cert_pem = NULL;
		rtos_mem_free(http_client_pkey_pem);
		http_client_pkey_pem = NULL;
	}
	if (conn_ptr) {
		httpc_conn_close(conn_ptr);
		httpc_conn_free(conn_ptr);
	}

	rtos_mem_free(response_data);
	response_data = NULL;
	rtos_mem_free(post_data_buffer);
	post_data_buffer = NULL;

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


//AT+HTTPPUT=<host>,<path>,<conn_type>[,<certificate_index>],<body_size>[,<req_header_cnt>,<req_header>...<req_header>]
void at_httpput(void *arg)
{
	int argc = 0, error_no = 0, ret = -1;
	char *argv[MAX_ARGC] = {0};
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	int cert_size = 0, pk_size = 0;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	int total_post_body_size = 0, single_post_size = 0;
	u8 *post_data_buffer = NULL;
	uint8_t *response_data = NULL;
	int read_size = 0;
	uint32_t total_resp_body_size = 0;


	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc < 5) || (argc > MAX_ARGC)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) == 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Input <host> is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) > DNS_MAX_NAME_LENGTH) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Input <host> length should not exceed DNS_MAX_NAME_LENGTH\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[2]) == 0) {
		argv[2] = "/";
	}

	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Connect type is incorrect\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL)  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Certificate index is NULL\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Certificate index is incorrect\r\n");
			error_no = 1;
			goto end;
		}
	}

	total_post_body_size = atoi(argv[5]);
	if (total_post_body_size < 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Total length of POST is incorrect\r\n");
		error_no = 1;
		goto end;
	}

	if (argc > 6) {
		header_cnt = atoi(argv[6]);
		if ((header_cnt == 0) || (header_cnt != argc - 7)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpput] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = 1;
			goto end;
		}
		for (i = 1; i <= header_cnt; i++) {
			find_result = strstr(argv[6 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpput] The input request header is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index)) <= 0)  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] Failed to get CLIENT_CA_%d size!\r\n", cert_index);
			error_no = 3;
			goto end;
		}
		http_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_ca_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] http_client_ca_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_ca_pem, CLIENT_CA, cert_index);
		if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else if (ret < 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index)) <= 0))  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size!\r\n", cert_index, cert_index);
			error_no = 3;
			goto end;
		}
		http_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_cert_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] http_client_cert_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_cert_pem, CLIENT_CERT, cert_index);
		if (ret > 0) {
			http_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (http_client_pkey_pem == NULL) {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpput] http_client_pkey_pem malloc fail\r\n");
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(http_client_pkey_pem, CLIENT_KEY, cert_index);
			if (ret <= 0)  {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpput] CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 3;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}

	switch (conn_type) {
	case HTTP_OVER_TCP:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_NO_VERIFY:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_SERVER:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, http_client_ca_pem);
		break;
	default:
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Connect type is incorrect\r\n");
		error_no = 1;
		break;
	}

	if (!conn_ptr) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] conn_ptr is NULL\r\n");
		error_no = 2;
		goto end;
	}
	httpc_enable_ignore_content_len(conn_ptr);

	if (http_server_port == 0)  {
		if (conn_type == HTTP_OVER_TCP) {
			ret = httpc_conn_connect(conn_ptr, argv[1], 80, http_timeout);
		} else {
			ret = httpc_conn_connect(conn_ptr, argv[1], 443, http_timeout);
		}
	} else  {
		ret = httpc_conn_connect(conn_ptr, argv[1], (uint16_t)http_server_port, http_timeout);
	}

	if (ret != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_conn_connect failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, "PUT", argv[2], NULL, (size_t)total_post_body_size) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_header_start failed\r\n");
		error_no = 4;
		goto end;
	}

	for (i = 1; i <= header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, argv[6 + i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	for (i = 0; i < g_http_req_header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, g_http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_header_finish failed\r\n");
		error_no = 4;
		goto end;
	}

	if (total_post_body_size > 0)  {
		post_data_buffer = rtos_mem_zmalloc((total_post_body_size <= MAX_TT_BUF_LEN ? total_post_body_size : MAX_TT_BUF_LEN) + 1);
		if (post_data_buffer == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpput] post_data_buffer malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)total_post_body_size) < 0)  {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpput] Enter TT mode failed\r\n");
			error_no = 5;
			goto end;
		}
		if (total_post_body_size <= MAX_TT_BUF_LEN)  {
			if (atcmd_tt_mode_get(post_data_buffer, (u32)total_post_body_size) != total_post_body_size)  {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpput] Get data failed in TT mode\r\n");
				error_no = 5;
				goto end;
			}
			if (httpc_request_write_data(conn_ptr, (uint8_t *)post_data_buffer, (size_t)total_post_body_size) != total_post_body_size)  {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_data failed\r\n");
				error_no = 6;
				goto end;
			}
		} else  {
			while (total_post_body_size > 0)  {
				single_post_size = (total_post_body_size <= MAX_TT_BUF_LEN) ? total_post_body_size : MAX_TT_BUF_LEN;
				if (atcmd_tt_mode_get(post_data_buffer, (u32)single_post_size) != single_post_size)  {
					RTK_LOGI(AT_HTTP_TAG, "[at_httpput] Get data failed in TT mode\r\n");
					error_no = 5;
					goto end;
				}
				if (httpc_request_write_data(conn_ptr, (uint8_t *)post_data_buffer, (size_t)single_post_size) != single_post_size)  {
					RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_request_write_data failed\r\n");
					error_no = 6;
					goto end;
				}
				total_post_body_size -= single_post_size;
			}
		}
		atcmd_tt_mode_end();

		rtos_mem_free(post_data_buffer);
		post_data_buffer = NULL;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_response_read_header failed\r\n");
		error_no = 7;
		goto end;
	}

	at_printf("+HTTPPUT: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPPUT: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPPUT: HEADER DUMP END\r\n");
	at_printf_unlock();

	response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "[at_httpput] response_data malloc fail\r\n");
		error_no = 2;
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPPUT: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
			if (total_resp_body_size >= conn_ptr->response.content_len) {
				break;
			}
		}
		at_printf("+HTTPPUT: BODY DUMP END\r\n");
		at_printf("+HTTPPUT: BODY LEN = %d\r\n", conn_ptr->response.content_len);
	} else {    //Some websites’ response headers do not include the content length, ex: Transfer-Encoding: chunked
		at_printf("+HTTPPUT: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
		}
		at_printf("+HTTPPUT: BODY DUMP END\r\n");
		at_printf("+HTTPPUT: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(http_client_ca_pem);
		http_client_ca_pem = NULL;
		rtos_mem_free(http_client_cert_pem);
		http_client_cert_pem = NULL;
		rtos_mem_free(http_client_pkey_pem);
		http_client_pkey_pem = NULL;
	}
	if (conn_ptr) {
		httpc_conn_close(conn_ptr);
		httpc_conn_free(conn_ptr);
	}

	rtos_mem_free(response_data);
	response_data = NULL;
	rtos_mem_free(post_data_buffer);
	post_data_buffer = NULL;

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}



//AT+HTTPDEL=<host>,<path>,<conn_type>[,<certificate_index>][,<req_header_cnt>,<req_header>...<req_header>]
void at_httpdel(void *arg)
{
	int argc = 0, error_no = 0, ret = -1;
	char *argv[MAX_ARGC] = {0};
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	int cert_size = 0, pk_size = 0;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	uint8_t *response_data = NULL;
	int read_size = 0;
	uint32_t total_resp_body_size = 0;


	if (arg == NULL) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc < 4) || (argc > MAX_ARGC)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) == 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Input <host> is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[1]) > DNS_MAX_NAME_LENGTH) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Input <host> length should not exceed DNS_MAX_NAME_LENGTH\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[2]) == 0) {
		argv[2] = "/";
	}

	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Connect type is incorrect\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL)  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Certificate index is NULL\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Certificate index is incorrect\r\n");
			error_no = 1;
			goto end;
		}
	}

	if (argc > 5) {
		header_cnt = atoi(argv[5]);
		if ((header_cnt == 0) || (header_cnt != argc - 6)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = 1;
			goto end;
		}
		for (i = 1; i <= header_cnt; i++) {
			find_result = strstr(argv[5 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] The input request header is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index)) <= 0)  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] Failed to get CLIENT_CA_%d size!\r\n", cert_index);
			error_no = 3;
			goto end;
		}
		http_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_ca_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] http_client_ca_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_ca_pem, CLIENT_CA, cert_index);
		if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else if (ret < 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index)) <= 0))  {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size!\r\n", cert_index, cert_index);
			error_no = 3;
			goto end;
		}
		http_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (http_client_cert_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] http_client_cert_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_cert_pem, CLIENT_CERT, cert_index);
		if (ret > 0) {
			http_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (http_client_pkey_pem == NULL) {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] http_client_pkey_pem malloc fail\r\n");
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(http_client_pkey_pem, CLIENT_KEY, cert_index);
			if (ret <= 0)  {
				RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 3;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else {
			RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}

	switch (conn_type) {
	case HTTP_OVER_TCP:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_NO_VERIFY:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_SERVER:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, http_client_ca_pem);
		break;
	default:
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Connect type is incorrect\r\n");
		error_no = 1;
		break;
	}

	if (!conn_ptr) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] conn_ptr is NULL\r\n");
		error_no = 2;
		goto end;
	}
	httpc_enable_ignore_content_len(conn_ptr);

	if (http_server_port == 0)  {
		if (conn_type == HTTP_OVER_TCP) {
			ret = httpc_conn_connect(conn_ptr, argv[1], 80, http_timeout);
		} else {
			ret = httpc_conn_connect(conn_ptr, argv[1], 443, http_timeout);
		}
	} else  {
		ret = httpc_conn_connect(conn_ptr, argv[1], (uint16_t)http_server_port, http_timeout);
	}

	if (ret != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_conn_connect failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, "DELETE", argv[2], NULL, 0) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_request_write_header_start failed\r\n");
		error_no = 4;
		goto end;
	}

	for (i = 1; i <= header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, argv[5 + i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	for (i = 0; i < g_http_req_header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, g_http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_request_write_header_raw failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_request_write_header_finish failed\r\n");
		error_no = 4;
		goto end;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_response_read_header failed\r\n");
		error_no = 5;
		goto end;
	}

	at_printf("+HTTPDEL: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPDEL: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPDEL: HEADER DUMP END\r\n");
	at_printf_unlock();

	response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "[at_httpdel] response_data malloc fail\r\n");
		error_no = 2;
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPDEL: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
			if (total_resp_body_size >= conn_ptr->response.content_len) {
				break;
			}
		}
		at_printf("+HTTPDEL: BODY DUMP END\r\n");
		at_printf("+HTTPDEL: BODY LEN = %d\r\n", conn_ptr->response.content_len);
	} else {    //Some websites’ response headers do not include the content length, ex: Transfer-Encoding: chunked
		at_printf("+HTTPDEL: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				RTK_LOGI(AT_HTTP_TAG, "[at_httpdel] httpc_response_read_data() read_size=%d\r\n", read_size);
				total_resp_body_size += read_size;
				at_printf_data((char *)response_data, read_size);
			} else {
				break;
			}
		}
		at_printf("+HTTPDEL: BODY DUMP END\r\n");
		at_printf("+HTTPDEL: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(http_client_ca_pem);
		http_client_ca_pem = NULL;
		rtos_mem_free(http_client_cert_pem);
		http_client_cert_pem = NULL;
		rtos_mem_free(http_client_pkey_pem);
		http_client_pkey_pem = NULL;
	}
	if (conn_ptr) {
		httpc_conn_close(conn_ptr);
		httpc_conn_free(conn_ptr);
	}

	rtos_mem_free(response_data);
	response_data = NULL;

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}



log_item_t at_http_items[] = {
	{"+HTTPHEADER", at_httpheader, {NULL, NULL}},
	{"+HTTPCONF", at_httpconf, {NULL, NULL}},
	{"+HTTPQUERY", at_httpquery, {NULL, NULL}},
	{"+HTTPGET", at_httpget, {NULL, NULL}},
	{"+HTTPPOST", at_httppost, {NULL, NULL}},
	{"+HTTPPUT", at_httpput, {NULL, NULL}},
	{"+HTTPDEL", at_httpdel, {NULL, NULL}},
};

void print_http_at(void)
{
	int i = 0;
	int itemNum = sizeof(at_http_items) / sizeof(at_http_items[0]);

	for (i = 0; i < itemNum; i++)  {
		at_printf("AT%s\r\n", at_http_items[i].log_cmd);
	}
}

void at_http_init(void)
{
	atcmd_service_add_table(at_http_items, sizeof(at_http_items) / sizeof(at_http_items[0]));
}

#endif /* CONFIG_ATCMD_FTTP */
#endif /* CONFIG_LWIP_LAYER */