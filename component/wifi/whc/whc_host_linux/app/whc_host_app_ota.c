#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <poll.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/syscall.h>

#include <whc_host_app_api.h>
#include <whc_host_netlink.h>
#include <whc_host_app_ota.h>

#define PORT	8082
static const char *ota_host = "192.168.50.55";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
static const char *ota_resource = "ota_all.bin";     //"051103061600.bin"
#define OTA_TYPE OTA_HTTP

//#define WHC_OTA_AUTO_RMMOD	1
#ifdef WHC_OTA_AUTO_RMMOD
/* For example only. Replace with the real module name as needed. */
static const char *module_name = "whc_sdio";
#endif

struct whc_ota_context *whc_ota_ctx;

#ifdef todo
#endif
#define OTA_MAX_BUFSZ	1200
#define HTTP_HEADER_BAK_LEN 32
int whc_ota_ongoing;

extern uint8_t rmesh_self_mac[2][6];
extern uint8_t rmesh_ap_gw[4];

void whc_host_ota_init(struct whc_host_ota_info *ota_info)
{
	uint32_t buf_len, offset;
	uint8_t *buf;
	uint8_t *ptr;
	struct whc_host_ota_hdr *pota_hdr;
	struct whc_host_ota_info *pota_info;

	if (ota_info == NULL) {
		printf("ota_info shouldn't be empty\r\n");
		return;
	}

	whc_ota_ongoing = 1;

	if ((ota_info->host_len <= 0) || (ota_info->resource_len <= 0)) {
		printf("wrong ota info\r\n");
		return;
	}

	/* sizeof(struct whc_host_ota_info) can -8 in total len, didn't do here for margin for align */
	buf_len = 4 + sizeof(struct whc_host_ota_hdr) + sizeof(struct whc_host_ota_info) + ota_info->host_len + ota_info->resource_len;

	buf = malloc(buf_len);
	memset(buf, 0, buf_len);

	if (buf == NULL) {
		printf("%s mem error\r\n", __func__);
		return;
	}

	ptr = buf;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	pota_hdr = (struct whc_host_ota_hdr *)ptr;
	pota_hdr->type = WHC_WIFI_TEST_OTA;
	pota_hdr->subtype = WHC_OTA_START;

	ptr += sizeof(struct whc_host_ota_hdr);

	pota_info = (struct whc_host_ota_info *)ptr;

	offset = offsetof(struct whc_host_ota_info, host);
	memcpy(pota_info, ota_info, offset);
	ptr += offset;

	memcpy(ptr, ota_info->host, ota_info->host_len);
	ptr += ota_info->host_len;

	memcpy(ptr, ota_info->resource, ota_info->resource_len);
	ptr += ota_info->resource_len;

	/* tell dev prepare */
	whc_host_api_send_nl_data(buf, buf_len);

	if (buf) {
		free(buf);
	}

	if (whc_ota_ctx) {
		printf("error!, whc_ota_ctx should be empty \r\n");
	}
	/* set param */
	whc_ota_ctx = malloc(sizeof(struct whc_ota_context));
	memset(whc_ota_ctx, 0, sizeof(struct whc_ota_context));
#ifdef todo
#endif
	//fix http now
	whc_ota_ctx->type = OTA_TYPE;
	whc_ota_ctx->host = (char *)malloc(strlen(ota_info->host) + 1);
	strcpy(whc_ota_ctx->host, ota_info->host);
	whc_ota_ctx->resource = (char *)malloc(strlen(ota_info->resource) + 1);
	strcpy(whc_ota_ctx->resource, ota_info->resource);
	whc_ota_ctx->port = ota_info->port;

	whc_ota_ctx->fd = -1;

}

void whc_host_ota_deinit(void)
{
	if (whc_ota_ctx) {
		if (whc_ota_ctx->host) {
			free(whc_ota_ctx->host);
		}
		if (whc_ota_ctx->resource) {
			free(whc_ota_ctx->resource);
		}

		free(whc_ota_ctx);
		whc_ota_ctx = NULL;
	}

	whc_ota_ongoing = 0;

}

int whc_host_ota_conn_read(struct whc_ota_context *ctx, uint8_t *data, int data_len)
{
	int bytes_rcvd = -1;

	if (ctx->fd == -1) {
		printf("[%s], socket is invalid\n", __FUNCTION__);
		return bytes_rcvd;
	}

	if (ctx->type == OTA_HTTPS) {
#ifdef todo
#endif
	} else {
		bytes_rcvd = read(ctx->fd, data, data_len);
	}

	if (bytes_rcvd < 0) {
		printf("[%s], Close HTTP Socket[%d].\n", __FUNCTION__, ctx->fd);
		return -2;
	}
	return bytes_rcvd;
}

int whc_host_ota_conn_write(struct whc_ota_context *ctx, uint8_t *data, int data_len)
{
	if (ctx->type == OTA_HTTPS) {
#ifdef todo
#endif
	}
	return write(ctx->fd, data, data_len);
}

int whc_host_ota_http_connect_server(struct whc_ota_context *ctx)
{
	struct sockaddr_in server_addr;
	struct hostent *server;
	int fd = -1;
	int ret = -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		printf("[%s] Create socket failed", __FUNCTION__);
		return -1;
	}

	ctx->fd = fd;
	printf("[%s] Create socket: %d success!\n", __FUNCTION__, ctx->fd);

#ifdef todo
#endif
	server = gethostbyname(ctx->host);
	if (server == NULL) {
		printf("[ERROR] Get host ip failed\n");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ctx->port);
	memcpy((void *)&server_addr.sin_addr, (void *)server->h_addr, 4);

	if (connect(ctx->fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("[%s] Socket connect failed", __FUNCTION__);
		return -1;
	}

	printf("[%s] Connect success!\n", __FUNCTION__);

	if (ctx->type == OTA_HTTPS) {
#ifdef todo
#endif
	}
	return 0;
}

int whc_host_ota_http_send_request(struct whc_ota_context *ctx)
{
	uint8_t *request = NULL;
	int ret = -1;

	request = (uint8_t *)malloc(strlen("GET /") + strlen(ctx->resource) + strlen(" HTTP/1.1\r\nHost: ")
								+ strlen(ctx->host) + strlen("\r\n\r\n") + 1);
	sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", ctx->resource, ctx->host);
	ret = whc_host_ota_conn_write(ctx, request, strlen((char *)request));
	free(request);
	return ret;
}

int whc_host_ota_http_parse_response(struct whc_ota_context *ctx, uint8_t *response, uint32_t response_len, struct whc_ota_http_response *result)
{
	char *ptr_body_end;

	//Get status code
	if (0 == result->parse_status) { //didn't get the http response
		char *crlf_ptr = strstr((char *)response, "\r\n");
		if (crlf_ptr == NULL) { //didn't get the status code
			return -1;
		}
		result->status_code = atoi((char const *)response + 9); //get the status code
		if (result->status_code == 200) {
			result->parse_status = 1;
		} else if (result->status_code == 302) {
			// 302 response means redirect, which is not supported now, can be added in the future if needed
			printf("[HTTP] Response 302:%s \r\n", response);
			return -1;
		} else {
			printf("[HTTP] Response status code: %lu\n", result->status_code);
			return -1;
		}
	}

	//if didn't receive the full http header
	if (3 == result->parse_status) { //didn't get the http response
		if (NULL != (ptr_body_end = strstr((char *)response, "\r\n\r\n"))) {
			result->parse_status = 4;
			result->header_len = (uint32_t)ptr_body_end - (uint32_t)response + 4;
			result->body = response + result->header_len;
		}

		if (3 == result->parse_status) {//Still didn't receive the full header
			memcpy(result->header_bak, response + response_len - HTTP_HEADER_BAK_LEN, HTTP_HEADER_BAK_LEN);
		}
	}

	printf("[HTTP] Response: %s\n", response);

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
				result->header_len = (uint32_t)ptr_body_end - (uint32_t)response + 4;
				result->body = response + result->header_len;
			} else { //there are no content length in header
				printf("[HTTP] No Content-Length in header\n");
				return -1;
			}
		}

		if (1 == result->parse_status) {//didn't get the content length and the full header
			memcpy(result->header_bak, response + response_len - HTTP_HEADER_BAK_LEN, HTTP_HEADER_BAK_LEN);
		} else if (2 == result->parse_status) { //didn't get the full header but get the content length
			result->parse_status = 3;
			memcpy(result->header_bak, response + response_len - HTTP_HEADER_BAK_LEN, HTTP_HEADER_BAK_LEN);
		}
	}

	return result->parse_status;
}

static int whc_host_ota_http_recv_response(struct whc_ota_context *ctx)
{
	int read_bytes = 0;
	int remain_len = -1;
	struct whc_ota_http_response rsp_result = {0};
	uint32_t idx = 0;
	uint8_t *buf = NULL;

	buf = (uint8_t *)malloc(OTA_MAX_BUFSZ);
	rsp_result.header_bak = malloc(HTTP_HEADER_BAK_LEN + 1);

	if (!buf || !rsp_result.header_bak) {
		printf("[HTTP] Malloc failed\n");
		goto exit;
	}
	memset(buf, 0, OTA_MAX_BUFSZ);
	memset(rsp_result.header_bak, 0, HTTP_HEADER_BAK_LEN + 1);

	while (3 >= rsp_result.parse_status) { //still read header
		if (0 == rsp_result.parse_status) { //didn't get the http response
			memset(buf, 0, OTA_MAX_BUFSZ);
			read_bytes = whc_host_ota_conn_read(ctx, buf, OTA_MAX_BUFSZ);
			if (read_bytes <= 0) {
				printf("[HTTP]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes;
			memset(&rsp_result, 0, sizeof(rsp_result));
			if (whc_host_ota_http_parse_response(ctx, buf, idx, &rsp_result) == -1) {
				goto exit;
			}
		} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
			memset(buf, 0, OTA_MAX_BUFSZ);
			memcpy(buf, rsp_result.header_bak, HTTP_HEADER_BAK_LEN);
			read_bytes = whc_host_ota_conn_read(ctx, buf + HTTP_HEADER_BAK_LEN, (OTA_MAX_BUFSZ - HTTP_HEADER_BAK_LEN));
			if (read_bytes <= 0) {
				printf("[HTTP]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes + HTTP_HEADER_BAK_LEN;
			if (whc_host_ota_http_parse_response(ctx, buf, read_bytes + HTTP_HEADER_BAK_LEN, &rsp_result) == -1) {
				goto exit;
			}
		}
	}

	if (0 == rsp_result.body_len) {
		printf("[HTTP] New firmware size = 0 !\n");
		goto exit;
	} else {
		printf("[HTTP] Download new firmware begin, total size : %lu\n", rsp_result.body_len);
	}

	printf("[HTTP] idx = %lu, rsp_result.header_len = %lu\n", idx, rsp_result.header_len);

	remain_len = idx - rsp_result.header_len;
	/* remove http header_len from buf*/
	if (remain_len > 0) {
		ctx->NextImgBuf = malloc(remain_len);
		if (!ctx->NextImgBuf) {
			printf("[HTTP] Malloc failed\n");
			remain_len = -1;
			goto exit;
		}
		memset(ctx->NextImgBuf, 0, remain_len);
		memcpy((void *)ctx->NextImgBuf, (void *)(buf + rsp_result.header_len), remain_len);
		ctx->NextImgLen = remain_len;
	}

exit:
	if (buf) {
		free(buf);
	}
	if (rsp_result.header_bak) {
		free(rsp_result.header_bak);
	}
	return remain_len;
}

int whc_host_ota_http_prepare(struct whc_ota_context *whc_ota_ctx)
{
	int ret = -1;

	ret = whc_host_ota_http_connect_server(whc_ota_ctx);
	if (ret != 0) {
		return -1;
	}

	ret = whc_host_ota_http_send_request(whc_ota_ctx);
	if (!ret) {
		printf("[%s] Send HTTP request failed\n", __FUNCTION__);
		return -1;
	}

	if (whc_host_ota_http_recv_response(whc_ota_ctx) < 0) {
		printf("[HTTP] Parse response failed\n");
		return -1;
	}

	return 0;
}


int whc_host_ota_s1_prepare(struct whc_ota_context *whc_ota_ctx)
{
	whc_host_ota_http_prepare(whc_ota_ctx);
}

void whc_host_ota_start(struct whc_ota_context *whc_ota_ctx)
{
	int ret = -1;

	ret = whc_host_ota_s1_prepare(whc_ota_ctx);

}

int whc_host_ota(void)
{
	struct whc_host_ota_info pota_info;

	memset(&pota_info, 0, sizeof(struct whc_host_ota_info));
	pota_info.port = PORT;
	pota_info.ota_type = OTA_FOR_NORMAL;

	pota_info.host_len = strlen(ota_host) + 1;
	pota_info.resource_len = strlen(ota_resource) + 1;

	pota_info.host = malloc(pota_info.host_len);
	memset(pota_info.host, 0, pota_info.host_len);

	pota_info.resource = malloc(pota_info.resource_len);
	memset(pota_info.resource, 0, pota_info.resource_len);

	memcpy(pota_info.host, ota_host, strlen(ota_host));
	memcpy(pota_info.resource, ota_resource, strlen(ota_resource));

	/* sync with dev, set param */
	whc_host_ota_init(&pota_info);

	whc_host_ota_start(whc_ota_ctx);

	if (pota_info.host) {
		free(pota_info.host);
	}

	if (pota_info.resource) {
		free(pota_info.resource);
	}
	return 0;
}

void whc_host_ota_data_to_dev(int16_t len)
{
	uint8_t *buf = malloc(OTA_MAX_BUFSZ);
	uint32_t read_bytes;
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
#ifdef todo
#endif
	struct whc_host_ota_hdr *ota_hdr;

	if (len + sizeof(struct whc_host_ota_hdr) + 4 > OTA_MAX_BUFSZ) {
		printf("[%s]:len over size %d %d \r\n", __func__, len, OTA_MAX_BUFSZ);
	}

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	ota_hdr = (struct whc_host_ota_hdr *)ptr;
	ota_hdr->type = WHC_WIFI_TEST_OTA;
	ota_hdr->subtype = WHC_OTA_WRITE;

	ptr += sizeof(struct whc_host_ota_hdr);

	if (whc_ota_ctx->NextImgLen > 0) {
		read_bytes = whc_ota_ctx->NextImgLen > len ? len : whc_ota_ctx->NextImgLen;
		memcpy(ptr, whc_ota_ctx->NextImgBuf, read_bytes);
		if (read_bytes < whc_ota_ctx->NextImgLen) {
			memcpy(whc_ota_ctx->NextImgBuf, whc_ota_ctx->NextImgBuf + read_bytes, whc_ota_ctx->NextImgLen - read_bytes);
		}
		whc_ota_ctx->NextImgLen -= read_bytes;
		if (whc_ota_ctx->NextImgLen == 0) {
			free(whc_ota_ctx->NextImgBuf);
			whc_ota_ctx->NextImgBuf = NULL;
		}
	} else {
		read_bytes = whc_host_ota_conn_read(whc_ota_ctx, ptr, len);
		if (read_bytes < 0) {
			printf("[%s] Read socket failed\n", __FUNCTION__);
		}
	}

	ota_hdr->len = (int16_t)read_bytes;

	buf_len = 4 + sizeof(struct whc_host_ota_hdr) + ota_hdr->len;
	whc_host_api_send_nl_data(buf, buf_len);

	if (read_bytes <= 0) {
		printf("[%s] close sock \n", __FUNCTION__);
		whc_host_ota_deinit();
	}
	free(buf);
}

void whc_host_ota_end(void)
{
	whc_host_ota_deinit();
#ifdef WHC_OTA_AUTO_RMMOD
	unsigned int flags = 0; // O_NONBLOCK
	int ret = syscall(SYS_delete_module, module_name, flags);
	if (ret != 0) {
		printf("[%s] fail to rmmod target module \n", __FUNCTION__);
	}
#endif
}

void whc_host_ota_from_dev(uint8_t *buf)
{
	struct whc_host_ota_hdr *ota_hdr = (struct whc_host_ota_hdr *)buf;
	uint16_t len = ota_hdr->len;
	switch (ota_hdr->subtype) {
	case WHC_OTA_START:
		break;
	case WHC_OTA_READ:
		whc_host_ota_data_to_dev(len);
		break;
	case WHC_OTA_END:
		whc_host_ota_end();
		break;
	default:
		break;
	}

}

#ifdef CONFIG_RMESH
int whc_host_remsh_ota_request(uint8_t *buf, int recv_len)
{
	uint8_t ota_request_seq = 0;
	uint8_t httpiplen = 0;
	uint8_t http_resourcelen = 0;
	uint8_t *self_mac_p0 = rmesh_self_mac[0];

	uint8_t target_mac[6] = {0};
	struct whc_host_ota_info ota_info;

	if (whc_ota_ongoing == 1) {
		printf("last ota are ongoing !\r\n");
		return 0;
	}

	if (buf[12] != ota_request_seq) {
		ota_request_seq = buf[12];

		/*get target mac when rnat*/
		memcpy(target_mac, buf + 15, ETH_ALEN);
		/*get host and resource*/
		memset(&ota_info, 0, sizeof(struct whc_host_ota_info));

		/* host */
		httpiplen = buf[22];
		ota_info.host_len = httpiplen;
		ota_info.host = malloc(httpiplen);
		memset(ota_info.host, 0, httpiplen);
		memcpy(ota_info.host, buf + 23, httpiplen);

		/* port */
		memcpy(&ota_info.port, buf + 25 + httpiplen, 2);
		ota_info.port = ntohs(ota_info.port);

		/* resource */
		http_resourcelen = buf[28 + httpiplen];
		ota_info.resource_len = http_resourcelen;
		ota_info.resource =  malloc(http_resourcelen);
		memset(ota_info.resource, 0, http_resourcelen);
		memcpy(ota_info.resource, buf + 29 + httpiplen, http_resourcelen);

		ota_info.ota_type = OTA_FOR_RMESH;
		ota_info.rmesh_ota_type = buf[31 + httpiplen + http_resourcelen];

		if (!memcmp(target_mac, self_mac_p0, ETH_ALEN)) { /*ota cmd to my self, peform OTA*/
			whc_host_ota_init(&ota_info);
			whc_host_ota_start(whc_ota_ctx);
		}

		if (ota_info.host) {
			free(ota_info.host);
		}

		if (ota_info.resource) {
			free(ota_info.resource);
		}
	}
	return 0;
}

void whc_host_rmesh_ota_sock_hdl(struct msgtemplate *msg, uint32_t recv_len)
{
	uint8_t ota_packet_pattern[6] = {0x52, 0x2d, 0x4D, 0x45, 0x53, 0x48};
	uint8_t *buf = msg->buf;
	uint8_t packet_type;
	int ret;

	if (memcmp(buf, ota_packet_pattern, 6) == 0) {
		packet_type = buf[7];
		switch (packet_type) {
		case 3:
			ret = whc_host_remsh_ota_request(buf, recv_len);
			break;
		default:
			break;
		}
	}
}
#endif
