/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifndef CONFIG_MP_INCLUDED
#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_HTTP) && (CONFIG_ATCMD_HTTP == 1)

#include "ameba.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_http.h"
#include "httpc/httpc.h"


uint32_t http_timeout = 0;  //Set by AT+HTTPCONF


//AT+HTTPGET=<host>,<path>,<conn_type>[,<certificate_index>][,<req_header_cnt>,<req_header>...<req_header>]
void at_httpget(void *arg)
{
	int argc = 0, error_no = 0, ret = -1;
	char *argv[MAX_ARGC] = {0};
	int conn_type = 0, cert_index = 0, header_cnt = 0, i = 0;
	char *find_result = NULL;
	struct httpc_conn *conn_ptr = NULL;
	char *http_client_ca_pem[HTTP_CERT_NUM] = {NULL};   // read client CA
	char *http_client_cert_pem[HTTP_CERT_NUM] = {NULL};   // read client cert
	char *http_client_pkey_pem[HTTP_CERT_NUM] = {NULL};   // read client private key
	char header_name[HTTP_REQ_HEADER_NAME_LEN] = {0}, header_value[HTTP_REQ_HEADER_VALUE_LEN] = {0};
	uint8_t *response_data = NULL;
	int read_size = 0;
	uint32_t total_size = 0;


	if (arg == NULL) {
		RTK_LOGE(NOTAG, "[at_httpget] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 4) {
		RTK_LOGE(NOTAG, "[at_httpget] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0)) {
		RTK_LOGE(NOTAG, "[at_httpget] Input <host> or <path> is NULL\r\n");
		error_no = 2;
		goto end;
	}

	conn_type = atoi(argv[3]);
	if ((conn_type < HTTP_OVER_TCP) || (conn_type > HTTP_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGE(NOTAG, "[at_httpget] Connect type is incorrect\r\n");
		error_no = 3;
		goto end;
	}
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		cert_index = atoi(argv[4]);
		if (cert_index < 0) {
			RTK_LOGE(NOTAG, "[at_httpget] Certificate index is incorrect\r\n");
			error_no = 4;
			goto end;
		}
	}

	if (argc > 5) {
		header_cnt = atoi(argv[5]);
		if ((header_cnt == 0) || (header_cnt != argc - 6)) {
			RTK_LOGE(NOTAG, "[at_httpget] <req_header_cnt> does not match the number of actual <req_header>\r\n");
			error_no = 5;
			goto end;
		}
		for (i = 1; i <= header_cnt; i++) {
			find_result = strstr(argv[5 + i], ": ");
			if (!find_result) {
				RTK_LOGE(NOTAG, "[at_httpget] The input request header is incorrect\r\n");
				error_no = 6;
				goto end;
			}
		}
	}

	if ((conn_type == HTTP_OVER_TLS_VERIFY_SERVER) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		http_client_ca_pem[0] = rtos_mem_zmalloc(HTTP_CERT_SIZE);
		if (http_client_ca_pem[0] == NULL) {
			RTK_LOGW(NOTAG, "[at_httpget] http_client_ca_pem[0] malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_ca_pem[0], CLIENT_CA, cert_index);
		if (ret > 0) {
			if (ret > HTTP_CERT_SIZE) {
				RTK_LOGW(NOTAG, "[at_httpget] CLIENT_CA_%d length larger than HTTP_CERT_SIZE\r\n", cert_index);
				error_no = 8;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(NOTAG, "[at_httpget] NO CLIENT_CA_%d!\r\n", cert_index);
			error_no = 9;
			goto end;
		} else {
			RTK_LOGW(NOTAG, "[at_httpget] CLIENT_CA_%d read failed\r\n", cert_index);
			error_no = 10;
			goto end;
		}
	}
	if ((conn_type == HTTP_OVER_TLS_VERIFY_CLIENT) || (conn_type == HTTP_OVER_TLS_VERIFY_BOTH)) {
		http_client_cert_pem[0] = rtos_mem_zmalloc(HTTP_CERT_SIZE);
		if (http_client_cert_pem[0] == NULL) {
			RTK_LOGW(NOTAG, "[at_httpget] http_client_cert_pem[0] malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(http_client_cert_pem[0], CLIENT_CERT, cert_index);
		if (ret > 0) {
			if (ret > HTTP_CERT_SIZE) {
				RTK_LOGW(NOTAG, "[at_httpget] CLIENT_CERT_%d length larger than HTTP_CERT_SIZE\r\n", cert_index);
				error_no = 8;
				goto end;
			}
			http_client_pkey_pem[0] = rtos_mem_zmalloc(HTTP_CERT_SIZE);
			if (http_client_pkey_pem[0] == NULL) {
				RTK_LOGW(NOTAG, "[at_httpget] http_client_pkey_pem[0] malloc fail\r\n");
				error_no = 7;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(http_client_pkey_pem[0], CLIENT_KEY, cert_index);
			if (ret > 0) {
				if (ret > HTTP_CERT_SIZE) {
					RTK_LOGW(NOTAG, "[at_httpget] CLIENT_KEY_%d length larger than HTTP_CERT_SIZE\r\n", cert_index);
					error_no = 8;
					goto end;
				}
			} else {
				RTK_LOGW(NOTAG, "[at_httpget] CLIENT_KEY_%d read failed\r\n", cert_index);
				error_no = 10;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(NOTAG, "[at_httpget] NO CLIENT_CERT_%d!\r\n", cert_index);
			error_no = 9;
			goto end;
		} else {
			RTK_LOGW(NOTAG, "[at_httpget] CLIENT_CERT_%d read failed\r\n", cert_index);
			error_no = 10;
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
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, http_client_ca_pem[0]);
		break;
	case HTTP_OVER_TLS_VERIFY_CLIENT:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem[0], http_client_pkey_pem[0], NULL);
		break;
	case HTTP_OVER_TLS_VERIFY_BOTH:
		conn_ptr = httpc_conn_new(HTTPC_SECURE_TLS, http_client_cert_pem[0], http_client_pkey_pem[0], http_client_ca_pem[0]);
		break;
	default:
		RTK_LOGE(NOTAG, "[at_httpget] Connect type is incorrect\r\n");
		error_no = 2;
		break;
	}

	if (!conn_ptr) {
		RTK_LOGI(NOTAG, "[at_httpget] conn_ptr is NULL\r\n");
		error_no = 11;
		goto end;
	}
	httpc_enable_ignore_content_len(conn_ptr);

	if (conn_type == HTTP_OVER_TCP) {
		ret = httpc_conn_connect(conn_ptr, argv[1], 80, http_timeout);
	} else {
		ret = httpc_conn_connect(conn_ptr, argv[1], 443, http_timeout);
	}
	if (ret != 0) {
		RTK_LOGI(NOTAG, "[at_httpget] httpc_conn_connect failed\r\n");
		error_no = 12;
		goto end;
	}

	if (httpc_request_write_header_start(conn_ptr, "GET", argv[2], NULL, 0) != 0) {
		RTK_LOGI(NOTAG, "[at_httpget] httpc_request_write_header_start failed\r\n");
		error_no = 13;
		goto end;
	}

	for (i = 1; i <= header_cnt; i++) {
		find_result = strstr(argv[5 + i], ": ");
		if (((find_result - argv[5 + i]) >= HTTP_REQ_HEADER_NAME_LEN) || ((strlen(argv[5 + i]) - (find_result - argv[5 + i] + 2)) >= HTTP_REQ_HEADER_VALUE_LEN)) {
			RTK_LOGI(NOTAG, "[at_httpget] HTTP request header name/value length larger than HTTP_REQ_HEADER_NAME(VALUE)_LEN\r\n");
			error_no = 14;
			goto end;
		}
		memcpy(header_name, argv[5 + i], (find_result - argv[5 + i]));
		memcpy(header_value, find_result + 2, (strlen(argv[5 + i]) - (find_result - argv[5 + i] + 2)));
		if (httpc_request_write_header(conn_ptr, header_name, header_value) != 0) {
			RTK_LOGI(NOTAG, "[at_httpget] httpc_request_write_header failed\r\n");
			error_no = 13;
			goto end;
		}
	}

	if (httpc_request_write_header_finish(conn_ptr) <= 0) {
		RTK_LOGI(NOTAG, "[at_httpget] httpc_request_write_header_finish failed\r\n");
		error_no = 13;
		goto end;
	}

	if (httpc_response_read_header(conn_ptr) != 0) {
		RTK_LOGI(NOTAG, "[at_httpget] httpc_response_read_header failed\r\n");
		error_no = 15;
		goto end;
	}

	at_printf("+HTTPGET: HEADER LEN = %d\r\n", conn_ptr->response.header_len);
	at_printf("+HTTPGET: HEADER DUMP START\r\n");
	at_printf("%s", conn_ptr->response.header);
	at_printf("+HTTPGET: HEADER DUMP END\r\n");

	response_data = rtos_mem_zmalloc(HTTP_READ_RESPONSE_DATA);
	if (response_data == NULL) {
		RTK_LOGW(NOTAG, "[at_httpget] response_data malloc fail\r\n");
		error_no = 16;
		goto end;
	}

	if (conn_ptr->response.content_len > 0) {
		at_printf("+HTTPGET: BODY LEN = %d\r\n", conn_ptr->response.content_len);
		at_printf("+HTTPGET: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				total_size += read_size;
				at_printf("%s", response_data);
			} else {
				break;
			}
			if (total_size >= conn_ptr->response.content_len) {
				break;
			}
		}
		at_printf("+HTTPGET: BODY DUMP END\r\n");
	} else {    //Some websites’ response headers do not include the content length, ex: Transfer-Encoding: chunked
		at_printf("+HTTPGET: BODY DUMP START\r\n");
		while (1) {
			memset(response_data, 0, HTTP_READ_RESPONSE_DATA);
			read_size = httpc_response_read_data(conn_ptr, response_data, HTTP_READ_RESPONSE_DATA - 1);
			if (read_size > 0) {
				total_size += read_size;
				at_printf("%s", response_data);
			} else {
				break;
			}
		}
		at_printf("+HTTPGET: BODY DUMP END\r\n");
		at_printf("+HTTPGET: BODY LEN = %d\r\n", total_size);
	}

end:
	if ((conn_type >= HTTP_OVER_TLS_VERIFY_SERVER) && (conn_type <= HTTP_OVER_TLS_VERIFY_BOTH)) {
		rtos_mem_free(http_client_ca_pem[0]);
		http_client_ca_pem[0] = NULL;
		rtos_mem_free(http_client_cert_pem[0]);
		http_client_cert_pem[0] = NULL;
		rtos_mem_free(http_client_pkey_pem[0]);
		http_client_pkey_pem[0] = NULL;
	}
	httpc_conn_close(conn_ptr);
	httpc_conn_free(conn_ptr);

	rtos_mem_free(response_data);
	response_data = NULL;

	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}


log_item_t at_http_items[] = {
	{"+HTTPGET", at_httpget, {NULL, NULL}},
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
#endif /* CONFIG_MP_INCLUDED */