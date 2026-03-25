/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_internal.h"
#include "lwip_netconf.h"

static int parser_url(char *url, char *host, u16 *port, char *resource, int len)
{
	if (url) {
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if (http) { // remove http
			url += strlen("http://");
		}
		_memset(host, 0, len);

		pos = strstr(url, ":");	// get port
		if (pos) {
			_memcpy(host, url, (pos - url));
			pos += 1;
			*port = atoi(pos);
		} else {
			pos = strstr(url, "/");
			if (pos) {
				_memcpy(host, url, (pos - url));
				url = pos;
			}
			*port = 80;
		}
		RTK_LOGI(OTA_TAG, "server: %s\n\r", host);
		RTK_LOGI(OTA_TAG, "port: %d\n\r", *port);

		_memset(resource, 0, len);
		pos = strstr(url, "/");
		if (pos) {
			_memcpy(resource, pos + 1, strlen(pos + 1));
		}
		RTK_LOGI(OTA_TAG, "resource: %s\n\r", resource);

		return OTA_OK;
	}
	return OTA_ERR;
}

static int ota_http_send_request(ota_context_t *ctx)
{
	u8 *request = NULL;
	int ret = -1;

	request = (u8 *)rtos_mem_malloc(strlen("GET /") + strlen(ctx->resource) + strlen(" HTTP/1.1\r\nHost: ")
									+ strlen(ctx->host) + strlen("\r\n\r\n") + 1);
	sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", ctx->resource, ctx->host);
	ret = ota_http_write(ctx, request, strlen((char *)request));
	rtos_mem_free(request);
	return ret;
}

/**
  * @brief  parse http response.
  * @param  response: the http response got from server
  * @param  response_len: The length of http response
  * @param  result: The struct that store the useful info from the http response
  * @retval  1:only got status code;3:got status code and content length,but not get the full header;4: got all info;-1:failed
  */
static int ota_http_parse_response(ota_context_t *ctx, u8 *response, u32 response_len, ota_http_response_t *result)
{
	ota_http_redirect_t *redirect = ctx->redirect;
	char *ptr_body_end;

	redirect->port = 0;
	//Get status code
	if (0 == result->parse_status) { //didn't get the http response
		char *crlf_ptr = strstr((char *)response, "\r\n");
		if (crlf_ptr == NULL) { //didn't get the status code
			return OTA_ERR;
		}
		result->status_code = atoi((char const *)response + 9); //get the status code
		if (result->status_code == 200) {
			result->parse_status = 1;
		} else if (result->status_code == 302) {
			char *tmp = NULL;
			const char *location1 = "LOCATION";
			const char *location2 = "Location";
			RTK_LOGI(OTA_TAG, "[HTTP] Response 302:%s \r\n", response);

			if ((tmp = strstr((char *)response, location1)) || (tmp = strstr((char *)response, location2))) {
				redirect->len = strlen(tmp + 10);
				RTK_LOGI(OTA_TAG, "[HTTP] Location len = %d\r\n", redirect->len);
				if (redirect->url == NULL) {
					redirect->url = (char *)rtos_mem_malloc(redirect->len);
					if (redirect->url == NULL) {
						return OTA_ERR;
					}
				}
				_memset(redirect->url, 0, redirect->len);
				_memcpy(redirect->url, tmp + 10, strlen(tmp + 10));
			}
			rtos_mem_free(ctx->host);
			ctx->host = (char *)rtos_mem_zmalloc(redirect->len);
			if (ctx->host == NULL) {
				return OTA_ERR;
			}
			rtos_mem_free(ctx->resource);
			ctx->resource = (char *)rtos_mem_zmalloc(redirect->len);
			if (ctx->resource == NULL) {
				return OTA_ERR;
			}

			parser_url(redirect->url, ctx->host, &redirect->port, ctx->resource, redirect->len);
			rtos_mem_free(redirect->url);
			return OTA_ERR;
		} else {
			RTK_LOGI(OTA_TAG, "[HTTP] Response status code: %lu\n", result->status_code);
			return OTA_ERR;
		}
	}

	//if didn't receive the full http header
	if (3 == result->parse_status) { //didn't get the http response
		if (NULL != (ptr_body_end = strstr((char *)response, "\r\n\r\n"))) {
			result->parse_status = 4;
			result->header_len = (u32)ptr_body_end - (u32)response + 4;
			result->body = response + result->header_len;
		}

		if (3 == result->parse_status) {//Still didn't receive the full header
			_memcpy(result->header_bak, response + response_len - OTA_HEADER_BAK_LEN, OTA_HEADER_BAK_LEN);
		}
	}

	RTK_LOGI(OTA_TAG, "[HTTP] Response: %s\n", response);

	//Get Content-Length
	if (1 == result->parse_status) { //didn't get the content length
		char *tmp_ptr;

		/* parse response_content_len */
		if (NULL != (tmp_ptr = strstr((char *)response, "Content-Length")) ||
			NULL != (tmp_ptr = strstr((char *)response, "CONTENT-LENGTH"))) {
			result->body_len = atoi(tmp_ptr + strlen("Content-Length: "));
			result->parse_status = 2;
		}
		if (NULL != (ptr_body_end = strstr((char *)response, "\r\n\r\n"))) {
			if (result->parse_status == 2) { //get the full header and the content length
				result->parse_status = 4;
				result->header_len = (u32)ptr_body_end - (u32)response + 4;
				result->body = response + result->header_len;
			} else { //there are no content length in header
				RTK_LOGI(OTA_TAG, "[HTTP] No Content-Length in header\n");
				return OTA_ERR;
			}
		}

		if (1 == result->parse_status) {//didn't get the content length and the full header
			_memcpy(result->header_bak, response + response_len - OTA_HEADER_BAK_LEN, OTA_HEADER_BAK_LEN);
		} else if (2 == result->parse_status) { //didn't get the full header but get the content length
			result->parse_status = 3;
			_memcpy(result->header_bak, response + response_len - OTA_HEADER_BAK_LEN, OTA_HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}

static int ota_http_recv_response(ota_context_t *ctx)
{
	int read_bytes = 0;
	int remain_len = -1;
	ota_http_response_t rsp_result = {0};
	u32 idx = 0;
	u8 *buf = NULL;

	buf = (u8 *)rtos_mem_zmalloc(OTA_BUF_SIZE);
	rsp_result.header_bak = rtos_mem_zmalloc(OTA_HEADER_BAK_LEN + 1);

	if (!buf || !rsp_result.header_bak) {
		RTK_LOGE(OTA_TAG, "[HTTP] Malloc failed\n");
		goto exit;
	}

	while (3 >= rsp_result.parse_status) { //still read header
		if (0 == rsp_result.parse_status) { //didn't get the http response
			_memset(buf, 0, OTA_BUF_SIZE);
			read_bytes = ota_http_read(ctx, buf, OTA_BUF_SIZE);
			if (read_bytes <= 0) {
				RTK_LOGE(OTA_TAG, "[HTTP]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes;
			_memset(&rsp_result, 0, sizeof(rsp_result));
			if (ota_http_parse_response(ctx, buf, idx, &rsp_result) == OTA_ERR) {
				goto exit;
			}
		} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
			_memset(buf, 0, OTA_BUF_SIZE);
			_memcpy(buf, rsp_result.header_bak, OTA_HEADER_BAK_LEN);
			read_bytes = ota_http_read(ctx, buf + OTA_HEADER_BAK_LEN, (OTA_BUF_SIZE - OTA_HEADER_BAK_LEN));
			if (read_bytes <= 0) {
				RTK_LOGE(OTA_TAG, "[HTTP]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes + OTA_HEADER_BAK_LEN;
			if (ota_http_parse_response(ctx, buf, read_bytes + OTA_HEADER_BAK_LEN, &rsp_result) == -1) {
				goto exit;
			}
		}
	}

	if (0 == rsp_result.body_len) {
		RTK_LOGI(OTA_TAG, "[HTTP] New firmware size = 0 !\n");
		goto exit;
	} else {
		RTK_LOGI(OTA_TAG, "[HTTP] Download new firmware begin, total size : %lu\n", rsp_result.body_len);
	}

	RTK_LOGI(OTA_TAG, "[HTTP] idx = %lu, rsp_result.header_len = %lu\n", idx, rsp_result.header_len);

	remain_len = idx - rsp_result.header_len;
	/* remove http header_len from buf*/
	if (remain_len > 0) {
		ctx->otaCtrl->NextImgLen = remain_len;
		_memset(ctx->otaCtrl->NextImgBuf, 0, OTA_BUF_SIZE);
		_memcpy((void *)ctx->otaCtrl->NextImgBuf, (void *)(buf + rsp_result.header_len), remain_len);
		ctx->otaCtrl->NextImgFg = 1;
	}

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (rsp_result.header_bak) {
		rtos_mem_free(rsp_result.header_bak);
	}
	return remain_len;
}

/**
  * @brief  connect to the OTA http server.
  * @param  server_socket: the socket used
  * @param  host: host address of the OTA server
  * @param  port: port of the OTA server
  * @retval  OTA_ERR when connect fail, socket value when connect success
  */
static int ota_http_connect_server(ota_context_t *ctx)
{
	struct sockaddr_in server_addr;
	struct hostent *server;
	int fd = -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		RTK_LOGE(OTA_TAG, "[HTTP] Create socket failed\n");
		return OTA_ERR;
	}
	ctx->fd = fd;
	RTK_LOGI(OTA_TAG, "[HTTP] Create socket: %d success!\n", ctx->fd);

	server = gethostbyname(ctx->host);
	if (server == NULL) {
		RTK_LOGE(OTA_TAG, "[HTTP] Get host ip failed\n");
		return OTA_ERR;
	}

	_memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ctx->port);
	_memcpy((void *)&server_addr.sin_addr, (void *)server->h_addr, 4);

	if (connect(ctx->fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		RTK_LOGE(OTA_TAG, "[HTTP] Connect failed\n");
		return OTA_ERR;
	}

	RTK_LOGI(OTA_TAG, "[HTTP] Connect success!\n");

	if (ctx->type == OTA_HTTPS) {
		if (ota_http_tls_init(ctx) != OTA_OK) {
			RTK_LOGE(OTA_TAG, "[HTTP] TLS init failed\n");
			return OTA_ERR;
		}
	}
	return OTA_OK;
}

int ota_http_write(ota_context_t *ctx, u8 *buf, int len)
{
	if (ctx->type == OTA_HTTPS) {
		return ota_http_tls_write(ctx, buf, len);
	}
	return write(ctx->fd, buf, len);
}

int ota_http_read(ota_context_t *ctx, u8 *buf, int len)
{
	if (ctx->type == OTA_HTTPS) {
		return ota_http_tls_read(ctx, buf, len);
	}
	return read(ctx->fd, buf, len);
}

int ota_http_connect(ota_context_t *ctx)
{
	if (ota_http_connect_server(ctx) != OTA_OK) {
		return OTA_ERR;
	}

	if (ota_http_send_request(ctx) < 0) {
		RTK_LOGE(OTA_TAG, "[HTTP] Send request failed\n");
		return OTA_ERR;
	}

	if (ota_http_recv_response(ctx) < 0) {
		RTK_LOGE(OTA_TAG, "[HTTP] Parse response failed\n");
		return OTA_ERR;
	}

	return OTA_OK;
}

void ota_http_close(ota_context_t *ctx)
{
	RTK_LOGI(OTA_TAG, "[HTTP] Closing connection...\n");
	if (ctx->fd >= 0) {
		close(ctx->fd);
		ctx->fd = -1;
	}

	if (ctx->type == OTA_HTTPS) {
		ota_http_tls_free(ctx);
	}
	RTK_LOGI(OTA_TAG, "[HTTP] Connection closed\n");
}