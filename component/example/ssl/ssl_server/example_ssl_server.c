#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"

#define SERVER_PORT   "443"

#if !(!defined(MBEDTLS_BIGNUM_C) || \
	!defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
	!defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_NET_C) || \
	!defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C))
static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
	return 0;
}
#endif

static void example_ssl_server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_CERTS_C) || \
	!defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
	!defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_NET_C) || \
	!defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

	RTK_LOGS(NOTAG, "MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or "
			 "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_SRV_C and/or "
			 "MBEDTLS_RSA_C and/or MBEDTLS_NET_C and/or "
			 "MBEDTLS_PEM_PARSE_C and/or MBEDTLS_X509_CRT_PARSE_C not defined.\n");

#else
	int ret;
	unsigned char buf[512];
	uint8_t *ip;
	mbedtls_net_context server_fd, client_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt server_x509;
	mbedtls_pk_context server_pk;

	const char *response = "<HTML><BODY>TLS OK</BODY></HTML>";

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		RTK_LOGS(NOTAG, "Please use AT+WLCONN=ssid,***,pw,*** to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(NOTAG, "\nExample: SSL server\n");

	/*
	 * 1. Prepare the certificate and key
	 */
	RTK_LOGS(NOTAG, "\n\r  . Preparing the certificate and key...");

	mbedtls_x509_crt_init(&server_x509);
	mbedtls_pk_init(&server_pk);

	if ((ret = mbedtls_x509_crt_parse(&server_x509, (const unsigned char *) mbedtls_test_srv_crt, mbedtls_test_srv_crt_len)) != 0) {
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

#if defined(MBEDTLS_VERSION_NUMBER) && (MBEDTLS_VERSION_NUMBER == 0x03000000)
	if ((ret = mbedtls_pk_parse_key(&server_pk, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0, NULL, NULL)) != 0) {
#else
	if ((ret = mbedtls_pk_parse_key(&server_pk, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0)) != 0) {
#endif
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}

	RTK_LOGS(NOTAG, " ok\n");

	/*
	 * 2. Start the connection
	 */
	ip = LwIP_GetIP(0);
	RTK_LOGS(NOTAG, "\n\r  . Starting tcp server /%d.%d.%d.%d/%s...", ip[0], ip[1], ip[2], ip[3], SERVER_PORT);
	mbedtls_net_init(&server_fd);

	if ((ret = mbedtls_net_bind(&server_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
		goto exit;
	}

	RTK_LOGS(NOTAG, " ok\n");

	/*
	 * 3. Waiting for client to connect
	 */
	RTK_LOGS(NOTAG, "\n\r  . Waiting for client to connect...\n\r");
	mbedtls_net_init(&client_fd);

	while ((ret = mbedtls_net_accept(&server_fd, &client_fd, NULL, 0, NULL)) == 0) {
		RTK_LOGS(NOTAG, "\n\r  . A client is connecting\n\r");
		/*
		 * 4. Setup stuff
		 */
		RTK_LOGS(NOTAG, "\n\r  . Setting up the SSL/TLS structure...");
		mbedtls_ssl_init(&ssl);
		mbedtls_ssl_config_init(&conf);

		if ((ret = mbedtls_ssl_config_defaults(&conf,
											   MBEDTLS_SSL_IS_SERVER,
											   MBEDTLS_SSL_TRANSPORT_STREAM,
											   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			RTK_LOGS(NOTAG, " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(&conf, my_random, NULL);

		if ((ret = mbedtls_ssl_conf_own_cert(&conf, &server_x509, &server_pk)) != 0) {
			RTK_LOGS(NOTAG, " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
			goto close_client;
		}

		if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
			RTK_LOGS(NOTAG, " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
		RTK_LOGS(NOTAG, " ok\n");

		/*
		 * 5. Handshake
		 */
		RTK_LOGS(NOTAG, "\n\r  . Performing the SSL/TLS handshake...");

		if ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
			RTK_LOGS(NOTAG, " failed\n  ! mbedtls_ssl_handshake returned %d\n\n", ret);
			goto close_client;
		}
		RTK_LOGS(NOTAG, " ok\n");
		RTK_LOGS(NOTAG, "\n\r  . Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));

		/*
		 * 6. Read the request from client
		 */
		RTK_LOGS(NOTAG, "\n\r  > Read request from client:");

		memset(buf, 0, sizeof(buf));
		if ((ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf))) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				RTK_LOGS(NOTAG, " failed\n\r  ! mbedtls_ssl_read returned %d\n", ret);
				goto close_client;
			}
		}
		RTK_LOGS(NOTAG, " %d bytes read\n\r\n\r%s\n", ret, (char *) buf);

		/*
		 * 7. Response the request
		 */
		RTK_LOGS(NOTAG, "\n\r  > Response to client:");

		sprintf((char *)buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(response), response);
		if ((ret = mbedtls_ssl_write(&ssl, buf, strlen((const char *)buf))) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				RTK_LOGS(NOTAG, " failed\n\r  ! mbedtls_ssl_write returned %d\n", ret);
				goto close_client;
			}
		}
		RTK_LOGS(NOTAG, " %d bytes written\n\r\n\r%s\n", ret, (char *)buf);

close_client:
		mbedtls_ssl_close_notify(&ssl);
		mbedtls_net_free(&client_fd);
		mbedtls_ssl_free(&ssl);
		mbedtls_ssl_config_free(&conf);
	}

	mbedtls_net_free(&server_fd);

exit:
	mbedtls_x509_crt_free(&server_x509);
	mbedtls_pk_free(&server_pk);
#endif
	rtos_task_delete(NULL);
}

void example_ssl_server(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"example_ssl_server_thread"), example_ssl_server_thread,
						 NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create example_ssl_server_thread failed", __FUNCTION__);
	}
}
