#include "ameba_soc.h"
#include "lwip_netconf.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#define SERVER_PORT   "443"

static const char mbedtls_test_srv_crt[] = \
		"-----BEGIN CERTIFICATE-----\r\n"                                      \
		"MIIDNzCCAh+gAwIBAgIBAjANBgkqhkiG9w0BAQsFADA7MQswCQYDVQQGEwJOTDER\r\n" \
		"MA8GA1UECgwIUG9sYXJTU0wxGTAXBgNVBAMMEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n" \
		"MTkwMjEwMTQ0NDA2WhcNMjkwMjEwMTQ0NDA2WjA0MQswCQYDVQQGEwJOTDERMA8G\r\n" \
		"A1UECgwIUG9sYXJTU0wxEjAQBgNVBAMMCWxvY2FsaG9zdDCCASIwDQYJKoZIhvcN\r\n" \
		"AQEBBQADggEPADCCAQoCggEBAMFNo93nzR3RBNdJcriZrA545Do8Ss86ExbQWuTN\r\n" \
		"owCIp+4ea5anUrSQ7y1yej4kmvy2NKwk9XfgJmSMnLAofaHa6ozmyRyWvP7BBFKz\r\n" \
		"NtSj+uGxdtiQwWG0ZlI2oiZTqqt0Xgd9GYLbKtgfoNkNHC1JZvdbJXNG6AuKT2kM\r\n" \
		"tQCQ4dqCEGZ9rlQri2V5kaHiYcPNQEkI7mgM8YuG0ka/0LiqEQMef1aoGh5EGA8P\r\n" \
		"hYvai0Re4hjGYi/HZo36Xdh98yeJKQHFkA4/J/EwyEoO79bex8cna8cFPXrEAjya\r\n" \
		"HT4P6DSYW8tzS1KW2BGiLICIaTla0w+w3lkvEcf36hIBMJcCAwEAAaNNMEswCQYD\r\n" \
		"VR0TBAIwADAdBgNVHQ4EFgQUpQXoZLjc32APUBJNYKhkr02LQ5MwHwYDVR0jBBgw\r\n" \
		"FoAUtFrkpbPe0lL2udWmlQ/rPrzH/f8wDQYJKoZIhvcNAQELBQADggEBAC465FJh\r\n" \
		"Pqel7zJngHIHJrqj/wVAxGAFOTF396XKATGAp+HRCqJ81Ry60CNK1jDzk8dv6M6U\r\n" \
		"HoS7RIFiM/9rXQCbJfiPD5xMTejZp5n5UYHAmxsxDaazfA5FuBhkfokKK6jD4Eq9\r\n" \
		"1C94xGKb6X4/VkaPF7cqoBBw/bHxawXc0UEPjqayiBpCYU/rJoVZgLqFVP7Px3sv\r\n" \
		"a1nOrNx8rPPI1hJ+ZOg8maiPTxHZnBVLakSSLQy/sWeWyazO1RnrbxjrbgQtYKz0\r\n" \
		"e3nwGpu1w13vfckFmUSBhHXH7AAS/HpKC4IH7G2GAk3+n8iSSN71sZzpxonQwVbo\r\n" \
		"pMZqLmbBm/7WPLc=\r\n"                                                 \
		"-----END CERTIFICATE-----\r\n";

static const char mbedtls_test_srv_key[] =  \
		"-----BEGIN RSA PRIVATE KEY-----\r\n"                                  \
		"MIIEpAIBAAKCAQEAwU2j3efNHdEE10lyuJmsDnjkOjxKzzoTFtBa5M2jAIin7h5r\r\n" \
		"lqdStJDvLXJ6PiSa/LY0rCT1d+AmZIycsCh9odrqjObJHJa8/sEEUrM21KP64bF2\r\n" \
		"2JDBYbRmUjaiJlOqq3ReB30Zgtsq2B+g2Q0cLUlm91slc0boC4pPaQy1AJDh2oIQ\r\n" \
		"Zn2uVCuLZXmRoeJhw81ASQjuaAzxi4bSRr/QuKoRAx5/VqgaHkQYDw+Fi9qLRF7i\r\n" \
		"GMZiL8dmjfpd2H3zJ4kpAcWQDj8n8TDISg7v1t7HxydrxwU9esQCPJodPg/oNJhb\r\n" \
		"y3NLUpbYEaIsgIhpOVrTD7DeWS8Rx/fqEgEwlwIDAQABAoIBAQCXR0S8EIHFGORZ\r\n" \
		"++AtOg6eENxD+xVs0f1IeGz57Tjo3QnXX7VBZNdj+p1ECvhCE/G7XnkgU5hLZX+G\r\n" \
		"Z0jkz/tqJOI0vRSdLBbipHnWouyBQ4e/A1yIJdlBtqXxJ1KE/ituHRbNc4j4kL8Z\r\n" \
		"/r6pvwnTI0PSx2Eqs048YdS92LT6qAv4flbNDxMn2uY7s4ycS4Q8w1JXnCeaAnYm\r\n" \
		"WYI5wxO+bvRELR2Mcz5DmVnL8jRyml6l6582bSv5oufReFIbyPZbQWlXgYnpu6He\r\n" \
		"GTc7E1zKYQGG/9+DQUl/1vQuCPqQwny0tQoX2w5tdYpdMdVm+zkLtbajzdTviJJa\r\n" \
		"TWzL6lt5AoGBAN86+SVeJDcmQJcv4Eq6UhtRr4QGMiQMz0Sod6ettYxYzMgxtw28\r\n" \
		"CIrgpozCc+UaZJLo7UxvC6an85r1b2nKPCLQFaggJ0H4Q0J/sZOhBIXaoBzWxveK\r\n" \
		"nupceKdVxGsFi8CDy86DBfiyFivfBj+47BbaQzPBj7C4rK7UlLjab2rDAoGBAN2u\r\n" \
		"AM2gchoFiu4v1HFL8D7lweEpi6ZnMJjnEu/dEgGQJFjwdpLnPbsj4c75odQ4Gz8g\r\n" \
		"sw9lao9VVzbusoRE/JGI4aTdO0pATXyG7eG1Qu+5Yc1YGXcCrliA2xM9xx+d7f+s\r\n" \
		"mPzN+WIEg5GJDYZDjAzHG5BNvi/FfM1C9dOtjv2dAoGAF0t5KmwbjWHBhcVqO4Ic\r\n" \
		"BVvN3BIlc1ue2YRXEDlxY5b0r8N4XceMgKmW18OHApZxfl8uPDauWZLXOgl4uepv\r\n" \
		"whZC3EuWrSyyICNhLY21Ah7hbIEBPF3L3ZsOwC+UErL+dXWLdB56Jgy3gZaBeW7b\r\n" \
		"vDrEnocJbqCm7IukhXHOBK8CgYEAwqdHB0hqyNSzIOGY7v9abzB6pUdA3BZiQvEs\r\n" \
		"3LjHVd4HPJ2x0N8CgrBIWOE0q8+0hSMmeE96WW/7jD3fPWwCR5zlXknxBQsfv0gP\r\n" \
		"3BC5PR0Qdypz+d+9zfMf625kyit4T/hzwhDveZUzHnk1Cf+IG7Q+TOEnLnWAWBED\r\n" \
		"ISOWmrUCgYAFEmRxgwAc/u+D6t0syCwAYh6POtscq9Y0i9GyWk89NzgC4NdwwbBH\r\n" \
		"4AgahOxIxXx2gxJnq3yfkJfIjwf0s2DyP0kY2y6Ua1OeomPeY9mrIS4tCuDQ6LrE\r\n" \
		"TB6l9VGoxJL4fyHnZb8L5gGvnB1bbD8cL6YPaDiOhcRseC9vBiEuVg==\r\n"         \
		"-----END RSA PRIVATE KEY-----\r\n";

const size_t mbedtls_test_srv_crt_len = sizeof(mbedtls_test_srv_crt);
const size_t mbedtls_test_srv_key_len = sizeof(mbedtls_test_srv_key);

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

#if !defined(MBEDTLS_BIGNUM_C) || \
	!defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
	!defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_NET_C) || \
	!defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "MBEDTLS_BIGNUM_C and/or "
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

	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: SSL server\n");

	/*
	 * 1. Prepare the certificate and key
	 */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Preparing the certificate and key...");

	mbedtls_x509_crt_init(&server_x509);
	mbedtls_pk_init(&server_pk);

	if ((ret = mbedtls_x509_crt_parse(&server_x509, (const unsigned char *) mbedtls_test_srv_crt, mbedtls_test_srv_crt_len)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if ((ret = mbedtls_pk_parse_key(&server_pk, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0, NULL, NULL)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, " ok\n");

	/*
	 * 2. Start the connection
	 */
	ip = LwIP_GetIP(NETIF_WLAN_STA_INDEX);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Starting tcp server /%d.%d.%d.%d/%s...", ip[0], ip[1], ip[2], ip[3], SERVER_PORT);
	mbedtls_net_init(&server_fd);

	if ((ret = mbedtls_net_bind(&server_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
		goto exit;
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, " ok\n");

	/*
	 * 3. Waiting for client to connect
	 */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Waiting for client to connect...\n\r");
	mbedtls_net_init(&client_fd);

	while ((ret = mbedtls_net_accept(&server_fd, &client_fd, NULL, 0, NULL)) == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . A client is connecting\n\r");
		/*
		 * 4. Setup stuff
		 */
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Setting up the SSL/TLS structure...");
		mbedtls_ssl_init(&ssl);
		mbedtls_ssl_config_init(&conf);

		if ((ret = mbedtls_ssl_config_defaults(&conf,
											   MBEDTLS_SSL_IS_SERVER,
											   MBEDTLS_SSL_TRANSPORT_STREAM,
											   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(&conf, my_random, NULL);

		if ((ret = mbedtls_ssl_conf_own_cert(&conf, &server_x509, &server_pk)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
			goto close_client;
		}

		if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, " ok\n");

		/*
		 * 5. Handshake
		 */
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Performing the SSL/TLS handshake...");

		if ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_ssl_handshake returned %d\n\n", ret);
			goto close_client;
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, " ok\n");
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));

		/*
		 * 6. Read the request from client
		 */
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  > Read request from client:");

		memset(buf, 0, sizeof(buf));
		if ((ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf))) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n\r  ! mbedtls_ssl_read returned %d\n", ret);
				goto close_client;
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, " %d bytes read\n\r\n\r%s\n", ret, (char *) buf);

		/*
		 * 7. Response the request
		 */
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  > Response to client:");

		sprintf((char *)buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(response), response);
		if ((ret = mbedtls_ssl_write(&ssl, buf, strlen((const char *)buf))) <= 0) {
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n\r  ! mbedtls_ssl_write returned %d\n", ret);
				goto close_client;
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, " %d bytes written\n\r\n\r%s\n", ret, (char *)buf);

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
						 NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create example_ssl_server_thread failed", __FUNCTION__);
	}
}
