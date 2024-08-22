#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#define SERVER_HOST    "192.168.1.100"
#define SERVER_PORT    "443"
#define RESOURCE       "/"
#define BUFFER_SIZE    2048

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	(void)p_rng;
	TRNG_get_random_bytes(output, output_len);
	return 0;
}

static void example_ssl_client_thread(void *param)
{
	int ret;
	mbedtls_net_context server_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;

	(void)param;

	RTK_LOGS(NOTAG, "\nExample: SSL client\n");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		RTK_LOGS(NOTAG, "Please use AT+WLCONN=ssid,***,pw,*** to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);

	if ((ret = mbedtls_net_connect(&server_fd, SERVER_HOST, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: mbedtls_net_connect ret(%d)\n", ret);
		goto exit;
	}

	mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	if ((ret = mbedtls_ssl_config_defaults(&conf,
										   MBEDTLS_SSL_IS_CLIENT,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		RTK_LOGS(NOTAG, "ERROR: mbedtls_ssl_config_defaults ret(%d)\n", ret);
		goto exit;
	}

	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(&conf, my_random, NULL);

	if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
		RTK_LOGS(NOTAG, "ERRPR: mbedtls_ssl_setup ret(%d)\n", ret);
		goto exit;
	}

	if ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: mbedtls_ssl_handshake ret(-0x%x)", -ret);
		goto exit;
	} else {
		unsigned char buf[BUFFER_SIZE + 1];
		int pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		RTK_LOGS(NOTAG, "SSL ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));
		sprintf((char *) buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		mbedtls_ssl_write(&ssl, buf, strlen((char *) buf));

		while ((read_size = mbedtls_ssl_read(&ssl, buf + pos, BUFFER_SIZE - pos)) > 0) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				buf[pos] = 0;
				header = strstr((char *) buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					RTK_LOGS(NOTAG, "\nHTTP Header: %s\n", buf);

					// Remove header size to get first read size of data from body head
					read_size = pos - ((unsigned char *) body - buf);
					pos = 0;

					content_len_pos = strstr((char *) buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
					}
				} else {
					if (pos >= BUFFER_SIZE) {
						RTK_LOGS(NOTAG, "ERROR: HTTP header\n");
						goto exit;
					}

					continue;
				}
			}

			RTK_LOGS(NOTAG, "read resource %d bytes\n", read_size);
			resource_size += read_size;
		}

		RTK_LOGS(NOTAG, "exit read. ret = %d\n", read_size);
		RTK_LOGS(NOTAG, "http content-length = %d bytes, download resource size = %d bytes\n", content_len, resource_size);
	}

exit:
	mbedtls_net_free(&server_fd);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);

	rtos_task_delete(NULL);
}


void example_ssl_client(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"example_ssl_client_thread"), example_ssl_client_thread,
						 NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(example_ssl_client_thread) failed", __FUNCTION__);
	}
}
