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

#include <whc_host_app_api.h>
#include <whc_host_netlink.h>
#include <whc_host_app_ota.h>

#define PORT	8082
static const char *ota_host = "192.168.50.55";  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
static const char *ota_resource = "ota_all.bin";     //"051103061600.bin"
#define OTA_TYPE OTA_HTTP
struct whc_ota_context *whc_ota_ctx;

#ifdef todo
#endif
#define OTA_MAX_BUFSZ	1200
int whc_ota_ongoing;

extern uint8_t rmesh_self_mac[2][6];
extern uint8_t rmesh_ap_gw[4];

void whc_host_ota_updata_init(struct whc_host_ota_info *ota_info)
{
	if (ota_info == NULL) {
		printf("ota_info shouldn't be empty\r\n");
	}

	whc_ota_ongoing = 1;
	uint32_t buf_len = 4 + sizeof(struct whc_host_ota_hdr) + sizeof(struct whc_host_ota_info);
	uint8_t *buf = malloc(buf_len);
	uint8_t *ptr = buf;

	struct whc_host_ota_hdr *pota_hdr;
	struct whc_host_ota_info *pota_info;

	memset(buf, 0, buf_len);

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	pota_hdr = (struct whc_host_ota_hdr *)ptr;
	pota_hdr->type = WHC_WIFI_TEST_OTA;
	pota_hdr->subtype = WHC_OTA_START;

	ptr += sizeof(struct whc_host_ota_hdr);

	memcpy(ptr, ota_info, sizeof(struct whc_host_ota_info));
	/* tell dev prepare */
	whc_host_api_send_nl_data(buf, buf_len);
	free(buf);

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

int whc_host_ota_update_http_connect_server(struct whc_ota_context *ctx)
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

int whc_host_ota_update_conn_write(struct whc_ota_context *ctx, uint8_t *data, int data_len)
{
	if (ctx->type == OTA_HTTPS) {
#ifdef todo
#endif
	}
	return write(ctx->fd, data, data_len);
}

int whc_host_ota_update_http_send_request(struct whc_ota_context *ctx)
{
	uint8_t *request = NULL;
	int ret = -1;

	request = (uint8_t *)malloc(strlen("GET /") + strlen(ctx->resource) + strlen(" HTTP/1.1\r\nHost: ")
								+ strlen(ctx->host) + strlen("\r\n\r\n") + 1);
	sprintf((char *)request, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", ctx->resource, ctx->host);
	ret = whc_host_ota_update_conn_write(ctx, request, strlen((char *)request));
	free(request);
	return ret;
}

int whc_host_ota_update_http_prepare(struct whc_ota_context *whc_ota_ctx)
{
	int ret = -1;

	ret = whc_host_ota_update_http_connect_server(whc_ota_ctx);
	if (ret != 0) {
		return -1;
	}

	ret = whc_host_ota_update_http_send_request(whc_ota_ctx);
	if (!ret) {
		printf("[%s] Send HTTP request failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}


int whc_host_ota_s1_prepare(struct whc_ota_context *whc_ota_ctx)
{
	whc_host_ota_update_http_prepare(whc_ota_ctx);
}

void whc_host_ota_updata_start(struct whc_ota_context *whc_ota_ctx)
{
	int ret = -1;

	ret = whc_host_ota_s1_prepare(whc_ota_ctx);

}

int whc_host_ota(void)
{
	struct whc_host_ota_info pota_info;

	memset(&pota_info, 0, sizeof(struct whc_host_ota_info));
	pota_info.port = PORT;
	memcpy(pota_info.host, ota_host, strlen(ota_host));
	memcpy(pota_info.resource, ota_resource, strlen(ota_resource));
	pota_info.ota_type = OTA_FOR_NORMAL;

	/* sync with dev, set param */
	whc_host_ota_updata_init(&pota_info);

	whc_host_ota_updata_start(whc_ota_ctx);
	return 0;
}

int whc_host_ota_update_conn_read(struct whc_ota_context *ctx, uint8_t *data, int data_len)
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

	read_bytes = whc_host_ota_update_conn_read(whc_ota_ctx, ptr, len);
	if (read_bytes < 0) {
		printf("[%s] Read socket failed\n", __FUNCTION__);
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
		httpiplen = buf[22];
		memset(&ota_info, 0, sizeof(struct whc_host_ota_info));
		if (httpiplen > 15) {
			printf("%s error size too long \r\n", __func__);
		}

		memcpy(ota_info.host, buf + 23, httpiplen);
		memcpy(&ota_info.port, buf + 25 + httpiplen, 2);
		ota_info.port = ntohs(ota_info.port);
		http_resourcelen = buf[28 + httpiplen];
		if (http_resourcelen > 19) {
			printf("%s error http_resourcelen too long \r\n", __func__);
		}

		memcpy(ota_info.resource, buf + 29 + httpiplen, http_resourcelen);
		ota_info.ota_type = OTA_FOR_RMESH;
		ota_info.rmesh_ota_type = buf[31 + httpiplen + http_resourcelen];

		if (!memcmp(target_mac, self_mac_p0, ETH_ALEN)) { /*ota cmd to my self, peform OTA*/
			whc_host_ota_updata_init(&ota_info);
			whc_host_ota_updata_start(whc_ota_ctx);
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
