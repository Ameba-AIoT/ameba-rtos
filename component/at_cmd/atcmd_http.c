/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_HTTP) && (CONFIG_ATCMD_HTTP == 1)
#include "lwip_netconf.h"
#include "atcmd_service.h"
#include "atcmd_http.h"
#include "httpc/httpc.h"
#include "httpc/httpc_util.h"
#include "mbedtls/ssl.h"

static const char *const AT_HTTP_TAG = "AT_HTTP";

/* Delay between retries when a body write makes no progress, aligned with the websocket client. */
#define AT_HTTP_WRITE_RETRY_MS 25

static int http_timeout = 10;	//Set by AT+HTTPCONF; Default value is 10s
static int http_server_port = 0;	//Set by AT+HTTPCONF; Default value is 80 for HTTP and 443 for HTTPS

static int g_http_req_header_cnt = 0;	//The maximum value is HTTP_GLOBAL_REQ_HEADER_NUM
static char *g_http_req_header[HTTP_GLOBAL_REQ_HEADER_NUM] = {0};	//Set by AT+HTTPHEADER


//AT+HTTPCONF=<timeout>[,<port>]
void at_httpconf(u16 argc, char **argv)
{
	int error_no = 0;
	int http_to = 0, http_srv_port = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpconf] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

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
	if ((argc > 2) && strlen(argv[2]))  {
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
void at_httpheader(u16 argc, char **argv)
{
	int error_no = 0;
	int req_header_len = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpheader] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

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
			goto tt_end;
		}
		if (atcmd_tt_mode_get((u8 *)g_http_req_header[g_http_req_header_cnt], (u32)req_header_len) != req_header_len)  {
			RTK_LOGI(AT_HTTP_TAG, "[at_httpheader] Get data failed in TT mode\r\n");
			error_no = 3;
			goto tt_end;
		}
		g_http_req_header_cnt++;
tt_end:
		atcmd_tt_mode_end();
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
void at_httpquery(u16 argc, char **argv)
{
	int error_no = 0;

	UNUSED(argc);
	UNUSED(argv);

	if (argc > 1) {
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


int at_http_load_ssl_cert_from_flash(int conn_type, int cert_index, char **http_client_ca_pem, char **http_client_cert_pem, char **http_client_pkey_pem)
{
	int cert_size = 0, pk_size = 0;
	int error_no = 0;
	int ret = -1;

	if (http_client_ca_pem == NULL || http_client_cert_pem == NULL || http_client_pkey_pem == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "http_client_ca_pem or http_client_cert_pem or http_client_pkey_pem is NULL\r\n");
		error_no = 1;
		goto end;
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index)) <= 0)  {
			RTK_LOGW(AT_HTTP_TAG, "Failed to get CLIENT_CA_%d size!\r\n", cert_index);
			error_no = 3;
			goto end;
		}
		*http_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (*http_client_ca_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "*http_client_ca_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(*http_client_ca_pem, CLIENT_CA, cert_index);
		if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else if (ret < 0) {
			RTK_LOGW(AT_HTTP_TAG, "CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index)) <= 0))  {
			RTK_LOGW(AT_HTTP_TAG, "Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size!\r\n", cert_index, cert_index);
			error_no = 3;
			goto end;
		}
		*http_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (*http_client_cert_pem == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "*http_client_cert_pem malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(*http_client_cert_pem, CLIENT_CERT, cert_index);
		if (ret > 0) {
			*http_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (*http_client_pkey_pem == NULL) {
				RTK_LOGW(AT_HTTP_TAG, "*http_client_pkey_pem malloc fail\r\n");
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(*http_client_pkey_pem, CLIENT_KEY, cert_index);
			if (ret <= 0)  {
				RTK_LOGW(AT_HTTP_TAG, "CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 3;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_HTTP_TAG, "NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 3;
			goto end;
		} else {
			RTK_LOGW(AT_HTTP_TAG, "CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 3;
			goto end;
		}
	}

end:
	return error_no;
}


static int at_http_create_connection_ex(int conn_type, struct httpc_conn **conn_ptr, char *http_client_ca_pem, char *http_client_cert_pem,
										char *http_client_pkey_pem, char *host, int timeout, int port);

int at_http_create_connection(int conn_type, struct httpc_conn **conn_ptr, char *http_client_ca_pem, char *http_client_cert_pem, char *http_client_pkey_pem,
							  char *host)
{
	return at_http_create_connection_ex(conn_type, conn_ptr, http_client_ca_pem, http_client_cert_pem, http_client_pkey_pem, host,
										http_timeout, http_server_port);
}


static int at_http_create_connection_ex(int conn_type, struct httpc_conn **conn_ptr, char *http_client_ca_pem, char *http_client_cert_pem,
										char *http_client_pkey_pem, char *host, int timeout, int port)
{
	int error_no = 0;
	int ret = -1;

	if (conn_ptr == NULL || host == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "conn_ptr or host is NULL\r\n");
		error_no = 1;
		goto end;
	}

	switch (conn_type) {
	case HTTP_OVER_TCP:
		*conn_ptr = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_NO_VERIFY:
		*conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_SERVER:
		*conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		*conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		*conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem, http_client_pkey_pem, http_client_ca_pem);
		break;
	default:
		RTK_LOGE(AT_HTTP_TAG, "The range of <conn_type> is [1, 5]\r\n");
		error_no = 1;
		break;
	}

	if (*conn_ptr == NULL) {
		RTK_LOGI(AT_HTTP_TAG, "*conn_ptr is NULL\r\n");
		error_no = 2;
		goto end;
	}
	httpc_enable_ignore_content_len(*conn_ptr);

	if (port == 0)  {
		if (conn_type == HTTP_OVER_TCP) {
			ret = httpc_conn_connect(*conn_ptr, host, 80, (uint32_t)timeout);
		} else {
			ret = httpc_conn_connect(*conn_ptr, host, 443, (uint32_t)timeout);
		}
	} else  {
		ret = httpc_conn_connect(*conn_ptr, host, (uint16_t)port, (uint32_t)timeout);
	}

	if (ret != 0) {
		RTK_LOGI(AT_HTTP_TAG, "httpc_conn_connect() failed\r\n");
		error_no = 4;
		goto end;
	}

end:
	return error_no;
}


int at_http_send_req_header(struct httpc_conn *conn_ptr, char *method, char *resource, char *content_type, size_t content_len, int header_cnt,
							char **http_req_header)
{
	int error_no = 0;
	int i = 0;

	if (conn_ptr == NULL || method == NULL || resource == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "conn_ptr or method or resource is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if (header_cnt > 0 && http_req_header == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "http_req_header is NULL but header_cnt=%d\r\n", header_cnt);
		error_no = 1;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, method, resource, content_type, content_len) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_header_start() failed\r\n");
		error_no = 4;
		goto end;
	}

	for (i = 0; i < header_cnt; i++) {
		if (http_req_header[i] == NULL) {
			continue;
		}
		if (httpc_request_write_header_raw(conn_ptr, http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_header_raw() failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	for (i = 0; i < g_http_req_header_cnt; i++) {
		if (httpc_request_write_header_raw(conn_ptr, g_http_req_header[i]) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_header_raw() failed\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_header_finish() failed\r\n");
		error_no = 4;
		goto end;
	}

end:
	return error_no;
}


int at_http_read_resp_header(struct httpc_conn *conn_ptr)
{
	int error_no = 0;

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "httpc_response_read_header() failed\r\n");
		error_no = 5;
	}
	return error_no;
}


int at_http_malloc_response_body(uint8_t **response_data)
{
	int error_no = 0;

	if (response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "response_data is NULL\r\n");
		error_no = 1;
		goto end;
	}

	*response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (*response_data == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "*response_data malloc fail\r\n");
		error_no = 2;
		goto end;
	}

end:
	return error_no;
}


int at_http_read_resp_body(uint8_t *response_data, struct httpc_conn *conn_ptr, uint32_t *total_resp_body_size)
{
	int read_size = 0;

	memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
	read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
	if (read_size > 0) {
		RTK_LOGI(AT_HTTP_TAG, "httpc_response_read_data() read_size = %d\r\n", read_size);
		*total_resp_body_size += read_size;
		at_printf_data((char *)response_data, read_size);
	} else {
		goto end;
	}

end:
	return read_size;
}


void at_http_free_resource(int conn_type, char **http_client_ca_pem, char **http_client_cert_pem, char **http_client_pkey_pem, struct httpc_conn **conn_ptr,
						   uint8_t **response_data)
{
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(*http_client_ca_pem);
		*http_client_ca_pem = NULL;
		rtos_mem_free(*http_client_cert_pem);
		*http_client_cert_pem = NULL;
		rtos_mem_free(*http_client_pkey_pem);
		*http_client_pkey_pem = NULL;
	}
	if (*conn_ptr) {
		httpc_conn_close(*conn_ptr);
		httpc_conn_free(*conn_ptr);
	}

	rtos_mem_free(*response_data);
	*response_data = NULL;
}


//AT+HTTPGET=<host>,<path>,<conn_type>[,<certificate_index>][,<req_header_cnt>,<req_header>...<req_header>]
void at_httpget(u16 argc, char **argv)
{
	int error_no = 0;
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	uint8_t *response_data = NULL;
	uint32_t total_resp_body_size = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
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
		RTK_LOGE(AT_HTTP_TAG, "[at_httpget] The range of <conn_type> is [1, 5]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL || (strlen(argv[4]) == 0))  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpget] Missing <cert_index> for HTTP_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpget] The <cert_index> should be greater than or equal to 1\r\n");
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
		for (i = 0; i < header_cnt; i++) {
			find_result = strstr(argv[6 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpget] The format of <req_header> is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((error_no = at_http_load_ssl_cert_from_flash(conn_type, cert_index, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem)) != 0) {
		goto end;
	}

	if ((error_no = at_http_create_connection(conn_type, &conn_ptr, http_client_ca_pem, http_client_cert_pem, http_client_pkey_pem, argv[1])) != 0) {
		goto end;
	}

	{
		char **req_headers = (header_cnt > 0) ? &argv[6] : NULL;
		if ((error_no = at_http_send_req_header(conn_ptr, "GET", argv[2], NULL, 0, header_cnt, req_headers)) != 0) {
			goto end;
		}
	}

	if ((error_no = at_http_read_resp_header(conn_ptr)) != 0) {
		goto end;
	}

	at_printf("+HTTPGET: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPGET: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPGET: HEADER DUMP END\r\n");
	at_printf_unlock();

	if ((error_no = at_http_malloc_response_body(&response_data)) != 0) {
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPGET: BODY DUMP START\r\n");
		while (1) {
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
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
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
				break;
			}
		}
		at_printf("+HTTPGET: BODY DUMP END\r\n");
		at_printf("+HTTPGET: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	at_http_free_resource(conn_type, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem, &conn_ptr, &response_data);

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


/* Send the whole body chunk. httpc_request_write_data() sends at most one TLS record and
 * returns 0 when it made no progress, so keep writing the remainder until the peer accepts
 * it all or the link breaks. Returns 0 on success, -1 on a send error.
 * stall_ms bounds how long the write may go without sending a single byte, 0 means no bound. A write that makes
 * progress, however slowly, restarts the clock. The bound has to be measured with the system time: a write that
 * returns 0 has usually blocked for the whole SO_SNDTIMEO, which sleeping time alone would not account for.
 * Callers that run in the AT task need a bound, otherwise a peer that stops reading freezes every AT command. */
static int at_http_write_body_all(struct httpc_conn *conn_ptr, u8 *data, int data_len, u32 stall_ms)
{
	int written = 0;
	u32 stall_start = rtos_time_get_current_system_time_ms();

	while (written < data_len) {
		int ret = httpc_request_write_data(conn_ptr, data + written, (size_t)(data_len - written));
		if (ret < 0) {
			return -1;
		}
		if (ret == 0) {
			if ((stall_ms != 0) && (rtos_time_get_current_system_time_ms() - stall_start >= stall_ms)) {
				RTK_LOGI(AT_HTTP_TAG, "write made no progress for %u ms, give up\r\n", (unsigned int)stall_ms);
				return -1;
			}
			rtos_time_delay_ms(AT_HTTP_WRITE_RETRY_MS);
			continue;
		}
		written += ret;
		stall_start = rtos_time_get_current_system_time_ms();
	}

	return 0;
}


int at_http_send_req_body(int total_post_body_size, struct httpc_conn *conn_ptr)
{
	u8 *post_data_buffer = NULL;
	int single_post_size = 0;
	int error_no = 0;
	int recv_tt_len = 0;

	if (total_post_body_size > 0)  {
		post_data_buffer = rtos_mem_zmalloc((total_post_body_size <= MAX_TT_BUF_LEN ? total_post_body_size : MAX_TT_BUF_LEN) + 1);
		if (post_data_buffer == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "post_data_buffer malloc fail\r\n");
			error_no = 2;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)total_post_body_size) < 0)  {
			RTK_LOGI(AT_HTTP_TAG, "Enter TT mode failed\r\n");
			error_no = 5;
			goto end;
		}
		if (total_post_body_size <= MAX_TT_BUF_LEN)  {
			recv_tt_len = atcmd_tt_mode_get(post_data_buffer, (u32)total_post_body_size);
			if (recv_tt_len == 0)  {
				RTK_LOGI(AT_HTTP_TAG, "Get data failed in TT mode\r\n");
				error_no = 5;
				goto tt_end;
			}
			if (at_http_write_body_all(conn_ptr, post_data_buffer, recv_tt_len, 0) != 0)  {
				RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_data() failed\r\n");
				error_no = 6;
				goto tt_end;
			}
		} else  {
			while (total_post_body_size > 0)  {
				single_post_size = (total_post_body_size <= MAX_TT_BUF_LEN) ? total_post_body_size : MAX_TT_BUF_LEN;
				recv_tt_len = atcmd_tt_mode_get(post_data_buffer, (u32)single_post_size);
				if (recv_tt_len == 0)  {
					RTK_LOGI(AT_HTTP_TAG, "Get data failed in TT mode\r\n");
					error_no = 5;
					goto tt_end;
				}
				if (at_http_write_body_all(conn_ptr, post_data_buffer, recv_tt_len, 0) != 0)  {
					RTK_LOGI(AT_HTTP_TAG, "httpc_request_write_data() failed\r\n");
					error_no = 6;
					goto tt_end;
				}
				total_post_body_size -= recv_tt_len;
			}
		}

tt_end:
		atcmd_tt_mode_end();

		rtos_mem_free(post_data_buffer);
		post_data_buffer = NULL;
	}

end:
	return error_no;
}


//AT+HTTPPOST=<host>,<path>,<conn_type>[,<certificate_index>],<body_size>[,<req_header_cnt>,<req_header>...<req_header>]
void at_httppost(u16 argc, char **argv)
{
	int error_no = 0;
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	int total_post_body_size = 0;
	uint8_t *response_data = NULL;
	uint32_t total_resp_body_size = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if ((argc < 6) || (argc > MAX_ARGC)) {
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
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] The range of <conn_type> is [1, 5]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL || (strlen(argv[4]) == 0))  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Missing <cert_index> for HTTP_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httppost] The <cert_index> should be greater than or equal to 1\r\n");
			error_no = 1;
			goto end;
		}
	}

	if (strlen(argv[5]) == 0)  {
		RTK_LOGE(AT_HTTP_TAG, "[at_httppost] Missing <body_size>\r\n");
		error_no = 1;
		goto end;
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
		for (i = 0; i < header_cnt; i++) {
			find_result = strstr(argv[7 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httppost] The format of <req_header> is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((error_no = at_http_load_ssl_cert_from_flash(conn_type, cert_index, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem)) != 0) {
		goto end;
	}

	if ((error_no = at_http_create_connection(conn_type, &conn_ptr, http_client_ca_pem, http_client_cert_pem, http_client_pkey_pem, argv[1])) != 0) {
		goto end;
	}

	{
		char **req_headers = (header_cnt > 0) ? &argv[7] : NULL;
		if ((error_no = at_http_send_req_header(conn_ptr, "POST", argv[2], NULL, (size_t)total_post_body_size, header_cnt, req_headers)) != 0) {
			goto end;
		}
	}

	if ((error_no = at_http_send_req_body(total_post_body_size, conn_ptr)) != 0) {
		goto end;
	}


	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httppost] httpc_response_read_header() failed\r\n");
		error_no = 7;
		goto end;
	}

	at_printf("+HTTPPOST: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPPOST: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPPOST: HEADER DUMP END\r\n");
	at_printf_unlock();

	if ((error_no = at_http_malloc_response_body(&response_data)) != 0) {
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPPOST: BODY DUMP START\r\n");
		while (1) {
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
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
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
				break;
			}
		}
		at_printf("+HTTPPOST: BODY DUMP END\r\n");
		at_printf("+HTTPPOST: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	at_http_free_resource(conn_type, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem, &conn_ptr, &response_data);

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


//AT+HTTPPUT=<host>,<path>,<conn_type>[,<certificate_index>],<body_size>[,<req_header_cnt>,<req_header>...<req_header>]
void at_httpput(u16 argc, char **argv)
{
	int error_no = 0;
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	int total_put_body_size = 0;
	uint8_t *response_data = NULL;
	uint32_t total_resp_body_size = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	if ((argc < 6) || (argc > MAX_ARGC)) {
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
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] The range of <conn_type> is [1, 5]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL || (strlen(argv[4]) == 0))  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Missing <cert_index> for HTTP_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpput] The <cert_index> should be greater than or equal to 1\r\n");
			error_no = 1;
			goto end;
		}
	}

	if (strlen(argv[5]) == 0)  {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Missing <body_size>\r\n");
		error_no = 1;
		goto end;
	}
	total_put_body_size = atoi(argv[5]);
	if (total_put_body_size < 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpput] Total length of PUT is incorrect\r\n");
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
		for (i = 0; i < header_cnt; i++) {
			find_result = strstr(argv[7 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpput] The format of <req_header> is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((error_no = at_http_load_ssl_cert_from_flash(conn_type, cert_index, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem)) != 0) {
		goto end;
	}

	if ((error_no = at_http_create_connection(conn_type, &conn_ptr, http_client_ca_pem, http_client_cert_pem, http_client_pkey_pem, argv[1])) != 0) {
		goto end;
	}

	{
		char **req_headers = (header_cnt > 0) ? &argv[7] : NULL;
		if ((error_no = at_http_send_req_header(conn_ptr, "PUT", argv[2], NULL, (size_t)total_put_body_size, header_cnt, req_headers)) != 0) {
			goto end;
		}
	}

	if ((error_no = at_http_send_req_body(total_put_body_size, conn_ptr)) != 0) {
		goto end;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpput] httpc_response_read_header() failed\r\n");
		error_no = 7;
		goto end;
	}

	at_printf("+HTTPPUT: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPPUT: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPPUT: HEADER DUMP END\r\n");
	at_printf_unlock();

	if ((error_no = at_http_malloc_response_body(&response_data)) != 0) {
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPPUT: BODY DUMP START\r\n");
		while (1) {
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
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
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
				break;
			}
		}
		at_printf("+HTTPPUT: BODY DUMP END\r\n");
		at_printf("+HTTPPUT: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	at_http_free_resource(conn_type, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem, &conn_ptr, &response_data);

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}



//AT+HTTPDEL=<host>,<path>,<conn_type>[,<certificate_index>][,<req_header_cnt>,<req_header>...<req_header>]
void at_httpdel(u16 argc, char **argv)
{
	int error_no = 0;
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	char *http_client_ca_pem = NULL;   // read client CA
	char *http_client_cert_pem = NULL;   // read client cert
	char *http_client_pkey_pem = NULL;   // read client private key
	uint8_t *response_data = NULL;
	uint32_t total_resp_body_size = 0;

	if (argc == 1) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
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
		RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] The range of <conn_type> is [1, 5]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		if (argv[4] == NULL || (strlen(argv[4]) == 0))  {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] Missing <cert_index> for HTTP_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[4]);
		if (cert_index <= 0) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] The <cert_index> should be greater than or equal to 1\r\n");
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
		for (i = 0; i < header_cnt; i++) {
			find_result = strstr(argv[6 + i], ": ");
			if (!find_result) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpdel] The format of <req_header> is incorrect\r\n");
				error_no = 1;
				goto end;
			}
		}
	}

	if ((error_no = at_http_load_ssl_cert_from_flash(conn_type, cert_index, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem)) != 0) {
		goto end;
	}

	if ((error_no = at_http_create_connection(conn_type, &conn_ptr, http_client_ca_pem, http_client_cert_pem, http_client_pkey_pem, argv[1])) != 0) {
		goto end;
	}

	{
		char **req_headers = (header_cnt > 0) ? &argv[6] : NULL;
		if ((error_no = at_http_send_req_header(conn_ptr, "DELETE", argv[2], NULL, 0, header_cnt, req_headers)) != 0) {
			goto end;
		}
	}

	if ((error_no = at_http_read_resp_header(conn_ptr)) != 0) {
		goto end;
	}

	at_printf("+HTTPDEL: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf_lock();
	at_printf("+HTTPDEL: HEADER DUMP START\r\n");
	at_printf_data(conn_ptr->response.header, conn_ptr->response.header_len);
	at_printf("+HTTPDEL: HEADER DUMP END\r\n");
	at_printf_unlock();

	if ((error_no = at_http_malloc_response_body(&response_data)) != 0) {
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPDEL: BODY DUMP START\r\n");
		while (1) {
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
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
			if (at_http_read_resp_body(response_data, conn_ptr, &total_resp_body_size) <= 0) {
				break;
			}
		}
		at_printf("+HTTPDEL: BODY DUMP END\r\n");
		at_printf("+HTTPDEL: BODY LEN = %d\r\n", total_resp_body_size);
	}

end:
	at_http_free_resource(conn_type, &http_client_ca_pem, &http_client_cert_pem, &http_client_pkey_pem, &conn_ptr, &response_data);

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}

/* ==================== Stepwise request links ==================== */

enum {
	AT_HTTP_LINK_IDLE = 0,
	AT_HTTP_LINK_CONNECTED,
	AT_HTTP_LINK_HEADER_SENT,
	AT_HTTP_LINK_RESP_READING,
	AT_HTTP_LINK_CLOSING,
};

enum {
	AT_HTTP_CLOSE_NONE = 0,
	AT_HTTP_CLOSE_HOST,
	AT_HTTP_CLOSE_PEER,
	AT_HTTP_CLOSE_ERROR,
	AT_HTTP_CLOSE_ABORT,
};

#define AT_HTTP_EVT_START_READ  (1U << 0)
#define AT_HTTP_EVT_CLOSE       (1U << 1)
#define AT_HTTP_EVT_ERROR       (1U << 2)

/*
 * Ownership: in CONNECTED/HEADER_SENT the AT thread uses conn; in RESP_READING/CLOSING it belongs to
 * the worker, which is the only place conn is freed. lock guards state, evt, close_reason, conn.
 * Lock order is lock -> at_printf lock; the AT thread never prints while holding lock.
 */
struct at_http_link {
	rtos_mutex_t       lock;
	rtos_sema_t        evt_sema;
	u32                evt;
	u8                 state;
	u8                 close_reason;
	volatile u8        abort;
	struct httpc_conn *conn;
	u32                total_body;
	u32                sent_body;
	u32                timeout;      /* seconds, copied from AT+HTTPOPEN */
	char               method[AT_HTTP_METHOD_MAX_LEN + 1];
};

static struct at_http_link g_http_link[AT_HTTP_MAX_LINK];

static const char *at_http_link_state_name(u8 state)
{
	switch (state) {
	case AT_HTTP_LINK_IDLE:
		return "IDLE";
	case AT_HTTP_LINK_CONNECTED:
		return "CONNECTED";
	case AT_HTTP_LINK_HEADER_SENT:
		return "HEADER_SENT";
	case AT_HTTP_LINK_RESP_READING:
		return "RESP_READING";
	case AT_HTTP_LINK_CLOSING:
		return "CLOSING";
	default:
		return "UNKNOWN";
	}
}

static const char *at_http_close_reason_name(u8 reason)
{
	switch (reason) {
	case AT_HTTP_CLOSE_HOST:
		return "host";
	case AT_HTTP_CLOSE_ERROR:
		return "error";
	case AT_HTTP_CLOSE_ABORT:
		return "abort";
	default:
		return "peer";
	}
}

/* Returns 1 if s has a control character (< 0x20 or 0x7f), or a space when allow_space is 0. These strings are
 * copied straight into the request line / header block, so a CR or LF would start a new header line and any other
 * control character would corrupt the request. The AT line parser already drops CR/LF, this keeps the check local. */
static int at_http_has_bad_char(const char *s, int allow_space)
{
	for (; *s; s++) {
		if (((unsigned char)*s < 0x20) || ((unsigned char)*s == 0x7f) || (!allow_space && (*s == ' '))) {
			return 1;
		}
	}
	return 0;
}

/* atoi() turns garbage into 0, so where 0 is a legal value (link, body size) require a leading digit. */
static int at_http_is_number(const char *s)
{
	return (s != NULL) && (s[0] >= '0') && (s[0] <= '9');
}

static int at_http_parse_link_id(const char *s, int *link_id)
{
	if (!at_http_is_number(s) || ((*link_id = atoi(s)) < 0) || (*link_id >= AT_HTTP_MAX_LINK)) {
		RTK_LOGE(AT_HTTP_TAG, "<link> range is [0, %d]\r\n", AT_HTTP_MAX_LINK - 1);
		return -1;
	}
	return 0;
}

static int at_http_has_token_nocase(const char *value, const char *token)
{
	size_t n = strlen(token);
	size_t i;

	for (; *value; value++) {
		for (i = 0; i < n; i++) {
			char a = value[i], b = token[i];
			if (a >= 'A' && a <= 'Z') {
				a = (char)(a + ('a' - 'A'));
			}
			if (a != b) {
				break;
			}
		}
		if (i == n) {
			return 1;
		}
	}
	return 0;
}

static void at_http_link_set_reason(struct at_http_link *lk, u8 reason)
{
	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	if (lk->close_reason == AT_HTTP_CLOSE_NONE) {
		lk->close_reason = reason;
	}
	rtos_mutex_give(lk->lock);
}

/* Hand the link over to the worker: switch state, record the close reason (first one wins) and wake it. */
static void at_http_link_post(struct at_http_link *lk, u32 evt, u8 new_state, u8 reason)
{
	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	if (lk->state != AT_HTTP_LINK_CLOSING) {
		lk->state = new_state;
	}
	lk->evt |= evt;
	if (reason != AT_HTTP_CLOSE_NONE && lk->close_reason == AT_HTTP_CLOSE_NONE) {
		lk->close_reason = reason;
	}
	rtos_mutex_give(lk->lock);
	rtos_sema_give(lk->evt_sema);
}

static u8 at_http_link_get_state(struct at_http_link *lk)
{
	u8 state;

	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	state = lk->state;
	rtos_mutex_give(lk->lock);
	return state;
}

#define AT_HTTP_IDLE_DRAIN_MAX 4

/*
 * An idle keep-alive connection should have nothing to read, so a readable socket is only a hint.
 * Read it (non-blocking) and treat the link as closed only when what arrived is a peer-close signal:
 *   FIN (read returns 0), RST or any other connection error, TLS close_notify, a fatal TLS alert.
 * Application data (stray bytes) is discarded, a TLS 1.3 NewSessionTicket is consumed, and an
 * incomplete TLS record is left in mbedtls; none of these close the link.
 */
static int at_http_link_peer_closed(struct httpc_conn *conn)
{
	fd_set rfds, efds;
	struct timeval tv;
	uint8_t scratch[64];
	int nonblock, closed = 0, i, n;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_ZERO(&efds);
	FD_SET(conn->sock, &rfds);
	FD_SET(conn->sock, &efds);

	if (select(conn->sock + 1, &rfds, NULL, &efds, &tv) <= 0) {
		return 0;
	}
	if (FD_ISSET(conn->sock, &efds)) {
		RTK_LOGI(AT_HTTP_TAG, "idle link: socket error pending\r\n");
		return 1;
	}

	/* A blocking read could wait for the rest of a partial TLS record, so make it non-blocking. */
	nonblock = 1;
	lwip_ioctl(conn->sock, FIONBIO, &nonblock);

	for (i = 0; i < AT_HTTP_IDLE_DRAIN_MAX; i++) {
		errno = 0;
		n = httpc_read(conn, scratch, sizeof(scratch));
		if (n == 0) {
			RTK_LOGI(AT_HTTP_TAG, "idle link: EOF from peer\r\n");
			closed = 1;
			break;
		}
		if (n > 0) {
			RTK_LOGW(AT_HTTP_TAG, "idle link: discarded %d unexpected byte(s)\r\n", n);
			continue;
		}
		if (conn->tls) {
			if (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE) {
				break;
			}
#ifdef MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET
			if (n == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
				continue;
			}
#endif
			RTK_LOGI(AT_HTTP_TAG, "idle link: TLS read returned -0x%x, peer closed or connection broken\r\n", -n);
			closed = 1;
			break;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			break;
		}
		RTK_LOGI(AT_HTTP_TAG, "idle link: read failed, errno=%d\r\n", errno);
		closed = 1;
		break;
	}

	nonblock = 0;
	lwip_ioctl(conn->sock, FIONBIO, &nonblock);
	return closed;
}

/* Idle connections are never read by the worker, so keepalive is the only way to notice a peer that vanished
 * without FIN/RST. The values are the AT_HTTP_KEEPALIVE_* macros in atcmd_http.h.
 * It is only an aid: the caller keeps a working connection even if this fails, dead peers are still caught by the
 * peer-closed check in AT+HTTPREQ and by the read/write timeout. */
static int at_http_link_set_keepalive(struct httpc_conn *conn)
{
#if LWIP_TCP_KEEPALIVE
	int on = 1, idle = AT_HTTP_KEEPALIVE_IDLE, intvl = AT_HTTP_KEEPALIVE_INTERVAL, cnt = AT_HTTP_KEEPALIVE_COUNT;

	errno = 0;
	if ((setsockopt(conn->sock, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) < 0) ||
		(setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) < 0) ||
		(setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl)) < 0) ||
		(setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt)) < 0)) {
		RTK_LOGW(AT_HTTP_TAG, "set keepalive option failed(errno=%d), continue without keepalive\r\n", errno);
		return -1;
	}
#else
	UNUSED(conn);
#endif
	return 0;
}

static void at_http_link_report_data(int link_id, const char *type, u8 *data, int len)
{
	at_printf_lock();
	at_printf_indicate("[HTTP][%s][%d][%d]:", type, link_id, len);
	at_printf_data((char *)data, (u32)len);
	at_printf("\r\n");
	at_printf_unlock();
}

#define AT_HTTP_MAX_INTERIM_RESP 4

/* Runs in the worker. Reports HEADER/BODY/DONE and returns 1 if the connection can be reused, in which case the link
 * is already back in CONNECTED when DONE is reported. */
static int at_http_link_read_response(struct at_http_link *lk, int link_id, int force_close)
{
	struct httpc_conn *conn = lk->conn;
	char *field = NULL;
	u8 *buf = NULL;
	int status, no_body, chunked, has_len = 0, until_close = 0, keep = 0, n, interim = 0;
	u32 content_len = 0, body_len = 0;

	while (1) {
		if (httpc_response_read_header(conn) != 0) {
			RTK_LOGI(AT_HTTP_TAG, "link %d read response header failed\r\n", link_id);
			at_http_link_set_reason(lk, AT_HTTP_CLOSE_ERROR);
			return 0;
		}
		at_http_link_report_data(link_id, "HEADER", (u8 *)conn->response.header, (int)conn->response.header_len);
		status = atoi(conn->response.status);
		/* An interim 1xx (e.g. 100 Continue) is followed by the final response on the same connection. */
		if ((status < 100) || (status >= 200) || (status == 101)) {
			break;
		}
		if (++interim > AT_HTTP_MAX_INTERIM_RESP || lk->abort) {
			at_http_link_set_reason(lk, AT_HTTP_CLOSE_ERROR);
			return 0;
		}
	}

	/* 101 Switching Protocols hands the connection over to another protocol, it cannot be reused. */
	if (status == 101) {
		force_close = 1;
	}
	no_body = (strcmp(lk->method, "HEAD") == 0) || (status == 101) || (status == 204) || (status == 304);
	/* Must match the chunked detection in httpc_response_read_data(). */
	chunked = (conn->response.trans_enc != NULL) && (memcmp(conn->response.trans_enc, "chunked", strlen("chunked")) == 0);
	/* httpc only recognises two spellings of Content-Length, so parse the value ourselves. */
	if (httpc_response_get_header_field(conn, "Content-Length", &field) == 0) {
		has_len = 1;
		content_len = (u32)strtoul(field, NULL, 10);
		httpc_free(field);
		field = NULL;
	}
	until_close = !no_body && !chunked && !has_len;

	if (!no_body && !(has_len && content_len == 0)) {
		buf = rtos_mem_malloc(AT_HTTP_REPORT_CHUNK);
		if (buf == NULL) {
			RTK_LOGW(AT_HTTP_TAG, "link %d response buffer malloc fail\r\n", link_id);
			at_http_link_set_reason(lk, AT_HTTP_CLOSE_ERROR);
			return 0;
		}
		while (1) {
			u32 want = AT_HTTP_REPORT_CHUNK;

			if (lk->abort) {
				goto exit;
			}
			if (has_len && !chunked) {
				if (body_len >= content_len) {
					break;
				}
				if (content_len - body_len < want) {
					want = content_len - body_len;
				}
			}
			n = httpc_response_read_data(conn, buf, want);
			if (n > 0) {
				at_http_link_report_data(link_id, "BODY", buf, n);
				body_len += (u32)n;
				continue;
			}
			/* Clean end: last chunk of a chunked body, or FIN / TLS close_notify for a close-delimited body. */
			if ((chunked && n == 0) || (until_close && (n == 0 || n == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY))) {
				break;
			}
			RTK_LOGI(AT_HTTP_TAG, "link %d read response body failed(%d)\r\n", link_id, n);
			at_http_link_set_reason(lk, AT_HTTP_CLOSE_ERROR);
			goto exit;
		}
	}

	keep = !until_close && !force_close;
	if (keep && httpc_response_get_header_field(conn, "Connection", &field) == 0) {
		if (at_http_has_token_nocase(field, "close")) {
			keep = 0;
		} else if (conn->response.version[7] == '0' && !at_http_has_token_nocase(field, "keep-alive")) {
			keep = 0;
		}
		httpc_free(field);
	} else if (keep && conn->response.version[7] == '0') {
		keep = 0;
	}

	/* Hand the link back before DONE goes out. The host may answer DONE with the next AT+HTTPREQ at once, and the
	 * output task has a higher priority than this worker, so switching the state after the report would let
	 * at_httpreq see a stale RESP_READING. The lock is released before printing, HTTPCLOSE prints under it too.
	 * Nothing else touches conn until the next START_READ, so the worker reading the next response is not affected.
	 * If HTTPCLOSE already moved the link to CLOSING the connection is not reused: report keep=0 and let the
	 * worker close it. */
	if (keep) {
		rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
		if (lk->state == AT_HTTP_LINK_RESP_READING) {
			lk->state = AT_HTTP_LINK_CONNECTED;
		} else {
			keep = 0;
		}
		rtos_mutex_give(lk->lock);
	}

	at_printf_indicate("[HTTP][DONE][%d][%d][%u][%d]\r\n", link_id, status, body_len, keep);

exit:
	rtos_mem_free(buf);
	return keep;
}

static void at_http_link_close(struct at_http_link *lk, int link_id)
{
	struct httpc_conn *conn;
	u8 reason;

	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	lk->state = AT_HTTP_LINK_CLOSING;
	conn = lk->conn;
	lk->conn = NULL;
	rtos_mutex_give(lk->lock);

	if (conn) {
		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	/* Report under lock so a new OPEN on this link can never be answered before CLOSED is out. */
	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	reason = (lk->close_reason == AT_HTTP_CLOSE_NONE) ? AT_HTTP_CLOSE_PEER : lk->close_reason;
	at_printf_indicate("[HTTP][CLOSED][%d][%s]\r\n", link_id, at_http_close_reason_name(reason));
	lk->state = AT_HTTP_LINK_IDLE;
	lk->evt = 0;
	lk->abort = 0;
	lk->close_reason = AT_HTTP_CLOSE_NONE;
	lk->total_body = 0;
	lk->sent_body = 0;
	rtos_mutex_give(lk->lock);
}

static void at_http_link_worker(void *param)
{
	int link_id = (int)(uintptr_t)param;
	struct at_http_link *lk = &g_http_link[link_id];
	u32 evt;
	int keep;

	while (1) {
		rtos_sema_take(lk->evt_sema, RTOS_MAX_DELAY);

		rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
		evt = lk->evt;
		lk->evt = 0;
		rtos_mutex_give(lk->lock);

		if (evt == 0) {
			continue;
		}

		keep = 0;
		if ((evt & AT_HTTP_EVT_START_READ) && !(evt & AT_HTTP_EVT_CLOSE)) {
			keep = at_http_link_read_response(lk, link_id, (evt & AT_HTTP_EVT_ERROR) != 0);
			/* Body write failed but the server's answer (e.g. 413) was read: the peer cut the connection.
			 * A failed read has already recorded ERROR, and a host HTTPCLOSE has recorded HOST; both win. */
			if (evt & AT_HTTP_EVT_ERROR) {
				at_http_link_set_reason(lk, AT_HTTP_CLOSE_PEER);
			}
		}
		/* keep means the link is already CONNECTED again (set before DONE was reported), wait for the next request.
		 * It is never set for a CLOSE/ERROR event or when HTTPCLOSE arrived while the response was being read. */
		if (keep && !(evt & (AT_HTTP_EVT_CLOSE | AT_HTTP_EVT_ERROR))) {
			continue;
		}
		break;
	}

	at_http_link_close(lk, link_id);
	rtos_task_delete(NULL);
}


//AT+HTTPOPEN=<link>,<host>,<conn_type>[,<cert_index>][,<timeout>][,<port>]
void at_httpopen(u16 argc, char **argv)
{
	int error_no = 0;
	int link_id = 0, conn_type = 0, cert_index = 0, timeout = http_timeout, port = http_server_port;
	struct at_http_link *lk = NULL;
	struct httpc_conn *conn = NULL;
	char *ca_pem = NULL, *cert_pem = NULL, *pkey_pem = NULL;
	char task_name[16];

	if ((argc < 4) || (argc > 7)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] Invalid number of parameters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (at_http_parse_link_id(argv[1], &link_id) != 0) {
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if ((strlen(argv[2]) == 0) || (strlen(argv[2]) > DNS_MAX_NAME_LENGTH) || at_http_has_bad_char(argv[2], 0)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] <host> is empty, too long, or has spaces or control characters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] The range of <conn_type> is [1, 5]\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) {
		if ((argc < 5) || ((cert_index = atoi(argv[4])) <= 0)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] <cert_index> (>= 1) is required for conn_type %d\r\n", conn_type);
			error_no = AT_HTTP_ERR_PARAM;
			goto end;
		}
	}
	if ((argc > 5) && strlen(argv[5])) {
		timeout = atoi(argv[5]);
	}
	/* timeout 0 would leave SO_RCVTIMEO unset, so a blocked worker could never see HTTPCLOSE. */
	if ((timeout < 1) || (timeout > AT_HTTP_MAX_TIMEOUT)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] <timeout> range is [1, %d]\r\n", AT_HTTP_MAX_TIMEOUT);
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if ((argc > 6) && strlen(argv[6]) && (((port = atoi(argv[6])) < 1) || (port > 65535))) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] <port> range is [1, 65535]\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}

	lk = &g_http_link[link_id];
	if (at_http_link_get_state(lk) != AT_HTTP_LINK_IDLE) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] link %d is %s\r\n", link_id, at_http_link_state_name(at_http_link_get_state(lk)));
		error_no = AT_HTTP_ERR_STATE;
		goto end;
	}

	if ((error_no = at_http_load_ssl_cert_from_flash(conn_type, cert_index, &ca_pem, &cert_pem, &pkey_pem)) != 0) {
		goto end;
	}
	if ((error_no = at_http_create_connection_ex(conn_type, &conn, ca_pem, cert_pem, pkey_pem, argv[2], timeout, port)) != 0) {
		goto end;
	}
	/* Keepalive is only auxiliary dead-peer detection. A connection that works must not be torn down because the
	 * option was refused, the failure is already logged by the setter. */
	(void)at_http_link_set_keepalive(conn);

	/* Drop wake-ups left over from the previous connection on this link. */
	while (rtos_sema_take(lk->evt_sema, 0) == RTK_SUCCESS) {
	}
	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	lk->conn = conn;
	lk->state = AT_HTTP_LINK_CONNECTED;
	lk->evt = 0;
	lk->abort = 0;
	lk->close_reason = AT_HTTP_CLOSE_NONE;
	lk->total_body = 0;
	lk->sent_body = 0;
	lk->timeout = (u32)timeout;
	lk->method[0] = '\0';
	rtos_mutex_give(lk->lock);

	DiagSnPrintf(task_name, sizeof(task_name), "at_http_%d", link_id);
	if (rtos_task_create(NULL, task_name, at_http_link_worker, (void *)(uintptr_t)link_id, AT_HTTP_WORKER_STACK_SIZE,
						 AT_HTTP_WORKER_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpopen] create worker for link %d failed\r\n", link_id);
		rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
		lk->conn = NULL;
		lk->state = AT_HTTP_LINK_IDLE;
		rtos_mutex_give(lk->lock);
		error_no = AT_HTTP_ERR_NOMEM;
		goto end;
	}
	conn = NULL;

end:
	/* The certificates are parsed into the TLS context by httpc_conn_new(), the PEM strings are no longer needed. */
	rtos_mem_free(ca_pem);
	rtos_mem_free(cert_pem);
	rtos_mem_free(pkey_pem);
	if (conn) {
		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


//AT+HTTPREQ=<link>,<method>,<path>,<total_body_size>[,<req_header_cnt>,<req_header>...<req_header>]
void at_httpreq(u16 argc, char **argv)
{
	int error_no = 0;
	int link_id = 0, total = 0, header_cnt = 0, i = 0;
	struct at_http_link *lk = NULL;
	char *path = NULL;
	u32 post_evt = 0;
	u8 post_state = AT_HTTP_LINK_CLOSING, post_reason = AT_HTTP_CLOSE_NONE;

	if (argc < 5) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] Invalid number of parameters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (at_http_parse_link_id(argv[1], &link_id) != 0) {
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if ((strlen(argv[2]) == 0) || (strlen(argv[2]) > AT_HTTP_METHOD_MAX_LEN)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] <method> length range is [1, %d]\r\n", AT_HTTP_METHOD_MAX_LEN);
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	for (i = 0; argv[2][i]; i++) {
		if (argv[2][i] < 'A' || argv[2][i] > 'Z') {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] <method> must be upper-case letters\r\n");
			error_no = AT_HTTP_ERR_PARAM;
			goto end;
		}
	}
	path = (strlen(argv[3]) == 0) ? "/" : argv[3];
	if (at_http_has_bad_char(path, 0)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] <path> must not contain spaces or control characters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (!at_http_is_number(argv[4]) || ((total = atoi(argv[4])) < 0)) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] <total_body_size> must be >= 0\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (argc > 5) {
		if (((header_cnt = atoi(argv[5])) <= 0) || (header_cnt != argc - 6)) {
			RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = AT_HTTP_ERR_PARAM;
			goto end;
		}
		for (i = 0; i < header_cnt; i++) {
			if ((strstr(argv[6 + i], ": ") == NULL) || at_http_has_bad_char(argv[6 + i], 1)) {
				RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] The format of <req_header> is incorrect\r\n");
				error_no = AT_HTTP_ERR_PARAM;
				goto end;
			}
		}
	}

	lk = &g_http_link[link_id];
	if (at_http_link_get_state(lk) != AT_HTTP_LINK_CONNECTED) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpreq] link %d is %s\r\n", link_id, at_http_link_state_name(at_http_link_get_state(lk)));
		error_no = AT_HTTP_ERR_STATE;
		goto end;
	}

	if (at_http_link_peer_closed(lk->conn)) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpreq] link %d was closed by peer while idle\r\n", link_id);
		error_no = AT_HTTP_ERR_PEER_CLOSED;
		post_evt = AT_HTTP_EVT_ERROR;
		post_reason = AT_HTTP_CLOSE_PEER;
		goto end;
	}

	if (at_http_send_req_header(lk->conn, argv[2], path, NULL, (size_t)total, header_cnt, (header_cnt > 0) ? &argv[6] : NULL) != 0) {
		error_no = AT_HTTP_ERR_CONN;
		post_evt = AT_HTTP_EVT_ERROR;
		post_reason = AT_HTTP_CLOSE_ERROR;
		goto end;
	}

	strcpy(lk->method, argv[2]);
	lk->total_body = (u32)total;
	lk->sent_body = 0;
	if (total == 0) {
		post_evt = AT_HTTP_EVT_START_READ;
		post_state = AT_HTTP_LINK_RESP_READING;
	} else {
		rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
		if (lk->state == AT_HTTP_LINK_CONNECTED) {
			lk->state = AT_HTTP_LINK_HEADER_SENT;
		}
		rtos_mutex_give(lk->lock);
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
	if (post_evt) {
		at_http_link_post(lk, post_evt, post_state, post_reason);
	}
}


//AT+HTTPBODY=<link>,<size>
void at_httpbody(u16 argc, char **argv)
{
	int error_no = 0;
	int link_id = 0, size = 0;
	u32 remaining = 0, chunk = 0, got = 0;
	struct at_http_link *lk = NULL;
	u8 *buf = NULL;
	int write_failed = 0, aborted = 0;
	u32 post_evt = 0;
	u8 post_state = AT_HTTP_LINK_CLOSING, post_reason = AT_HTTP_CLOSE_NONE;

	if (argc != 3) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpbody] Invalid number of parameters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (at_http_parse_link_id(argv[1], &link_id) != 0) {
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if ((size = atoi(argv[2])) <= 0) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpbody] <size> must be > 0\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}

	lk = &g_http_link[link_id];
	if (at_http_link_get_state(lk) != AT_HTTP_LINK_HEADER_SENT) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpbody] link %d is %s\r\n", link_id, at_http_link_state_name(at_http_link_get_state(lk)));
		error_no = AT_HTTP_ERR_STATE;
		goto end;
	}
	if ((u32)size > lk->total_body - lk->sent_body) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpbody] <size> exceeds the remaining %u bytes\r\n", lk->total_body - lk->sent_body);
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}

	buf = rtos_mem_malloc(((u32)size <= MAX_TT_BUF_LEN ? (u32)size : MAX_TT_BUF_LEN) + 1);
	if (buf == NULL) {
		RTK_LOGW(AT_HTTP_TAG, "[at_httpbody] buffer malloc fail\r\n");
		error_no = AT_HTTP_ERR_NOMEM;
		goto end;
	}
	if (atcmd_tt_mode_start((u32)size) < 0) {
		RTK_LOGI(AT_HTTP_TAG, "[at_httpbody] Enter TT mode failed\r\n");
		error_no = AT_HTTP_ERR_TT;
		goto end;
	}

	remaining = (u32)size;
	while (remaining > 0) {
		chunk = (remaining <= MAX_TT_BUF_LEN) ? remaining : MAX_TT_BUF_LEN;
		got = (u32)atcmd_tt_mode_get(buf, chunk);
		if (got < chunk) {
			aborted = 1;
			break;
		}
		/* After a write failure keep draining TT so the rest of the data is not parsed as AT commands. */
		if (!write_failed && at_http_write_body_all(lk->conn, buf, (int)got, lk->timeout * 1000U) != 0) {
			write_failed = 1;
		}
		remaining -= got;
	}
	atcmd_tt_mode_end();

	if (aborted) {
		/* The announced Content-Length can no longer be honoured. */
		RTK_LOGI(AT_HTTP_TAG, "[at_httpbody] link %d body aborted\r\n", link_id);
		error_no = AT_HTTP_ERR_ABORT;
		post_evt = AT_HTTP_EVT_ERROR;
		post_reason = AT_HTTP_CLOSE_ABORT;
	} else if (write_failed) {
		/* Servers often answer 401/413 and close before the body is complete, so try to read that response. */
		RTK_LOGI(AT_HTTP_TAG, "[at_httpbody] link %d body write failed\r\n", link_id);
		error_no = AT_HTTP_ERR_WRITE;
		post_evt = AT_HTTP_EVT_START_READ | AT_HTTP_EVT_ERROR;
		post_state = AT_HTTP_LINK_RESP_READING;
	} else {
		lk->sent_body += (u32)size;
		if (lk->sent_body == lk->total_body) {
			post_evt = AT_HTTP_EVT_START_READ;
			post_state = AT_HTTP_LINK_RESP_READING;
		}
	}

end:
	rtos_mem_free(buf);
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
	if (post_evt) {
		at_http_link_post(lk, post_evt, post_state, post_reason);
	}
}


//AT+HTTPCLOSE=<link>
void at_httpclose(u16 argc, char **argv)
{
	int error_no = 0;
	int link_id = 0, wake = 0;
	struct at_http_link *lk = NULL;

	if (argc != 2) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpclose] Invalid number of parameters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if (at_http_parse_link_id(argv[1], &link_id) != 0) {
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}

	lk = &g_http_link[link_id];
	rtos_mutex_take(lk->lock, RTOS_MAX_DELAY);
	if ((lk->state != AT_HTTP_LINK_IDLE) && (lk->state != AT_HTTP_LINK_CLOSING)) {
		lk->state = AT_HTTP_LINK_CLOSING;
		lk->abort = 1;
		lk->evt |= AT_HTTP_EVT_CLOSE;
		if (lk->close_reason == AT_HTTP_CLOSE_NONE) {
			lk->close_reason = AT_HTTP_CLOSE_HOST;
		}
		wake = 1;
	}
	rtos_mutex_give(lk->lock);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
	if (wake) {
		rtos_sema_give(lk->evt_sema);
	}
}


//AT+HTTPSTATE[=<link>]
void at_httpstate(u16 argc, char **argv)
{
	int error_no = 0;
	int link_id = 0, first = 0, last = AT_HTTP_MAX_LINK - 1, i;
	u8 state;
	u32 sent, total;

	if (argc > 2) {
		RTK_LOGE(AT_HTTP_TAG, "[at_httpstate] Invalid number of parameters\r\n");
		error_no = AT_HTTP_ERR_PARAM;
		goto end;
	}
	if ((argc == 2) && strlen(argv[1])) {
		if (at_http_parse_link_id(argv[1], &link_id) != 0) {
			error_no = AT_HTTP_ERR_PARAM;
			goto end;
		}
		first = last = link_id;
	}

	for (i = first; i <= last; i++) {
		rtos_mutex_take(g_http_link[i].lock, RTOS_MAX_DELAY);
		state = g_http_link[i].state;
		sent = g_http_link[i].sent_body;
		total = g_http_link[i].total_body;
		rtos_mutex_give(g_http_link[i].lock);
		at_printf("+HTTPSTATE: %d,%s,%u,%u\r\n", i, at_http_link_state_name(state), sent, total);
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


void at_http_init(void)
{
	int i;

	for (i = 0; i < AT_HTTP_MAX_LINK; i++) {
		memset(&g_http_link[i], 0, sizeof(g_http_link[i]));
		rtos_mutex_create(&g_http_link[i].lock);
		rtos_sema_create(&g_http_link[i].evt_sema, 0, 0xFFFF);
		g_http_link[i].state = AT_HTTP_LINK_IDLE;
	}
}


ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_http_items[] = {
	{"+HTTPHEADER", at_httpheader},
	{"+HTTPCONF", at_httpconf},
	{"+HTTPQUERY", at_httpquery},
	{"+HTTPGET", at_httpget},
	{"+HTTPPOST", at_httppost},
	{"+HTTPPUT", at_httpput},
	{"+HTTPDEL", at_httpdel},
	{"+HTTPOPEN", at_httpopen},
	{"+HTTPREQ", at_httpreq},
	{"+HTTPBODY", at_httpbody},
	{"+HTTPCLOSE", at_httpclose},
	{"+HTTPSTATE", at_httpstate},
};

void print_http_at(void)
{
	int i = 0;
	int itemNum = sizeof(at_http_items) / sizeof(at_http_items[0]);

	for (i = 0; i < itemNum; i++)  {
		at_printf("AT%s\r\n", at_http_items[i].log_cmd);
	}
}

#endif /* CONFIG_ATCMD_FTTP */
#endif /* CONFIG_LWIP_LAYER */
