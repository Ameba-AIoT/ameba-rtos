#include "ameba_soc.h"
#include "lwip_netconf.h"

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#define SERVER_PORT   "443"

static const char *test_srv_crt =
	"-----BEGIN CERTIFICATE-----\r\n" \
	"MIIDozCCAosCFBgRPzXVfYv/sHMIS+k+Eni4zySPMA0GCSqGSIb3DQEBCwUAMIGN\r\n" \
	"MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHSmlhbmdTdTEPMA0GA1UEBwwGU3VaaG91\r\n" \
	"MRAwDgYDVQQKDAdSZWFsc2lsMQ8wDQYDVQQLDAZzZXJ2ZXIxFDASBgNVBAMMC3Nl\r\n" \
	"cnZlcl90ZXN0MSIwIAYJKoZIhvcNAQkBFhNzZXJ2ZXJfdGVzdEAxMjMuY29tMB4X\r\n" \
	"DTIzMDYwNjA3MTI0M1oXDTMzMDYwMzA3MTI0M1owgY0xCzAJBgNVBAYTAkNOMRAw\r\n" \
	"DgYDVQQIDAdKaWFuZ1N1MQ8wDQYDVQQHDAZTdVpob3UxEDAOBgNVBAoMB1JlYWxz\r\n" \
	"aWwxDzANBgNVBAsMBnNlcnZlcjEUMBIGA1UEAwwLc2VydmVyX3Rlc3QxIjAgBgkq\r\n" \
	"hkiG9w0BCQEWE3NlcnZlcl90ZXN0QDEyMy5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
	"A4IBDwAwggEKAoIBAQCvaMzdXDPJm/O1jIpwh5NIL3aKZCKtZlVqkGEakzdJoomR\r\n" \
	"J2loYtxcwCtdCDPazskEG9O3qa7SVpgKYOzdtyFz/ETvwB5pG88tmpquibvUw6Ef\r\n" \
	"9fq1e3pXZagFbbqljL9N/RQnyjrZ96YLMY5UIdqunRvZFNeI3PoagDcrhiDGHomh\r\n" \
	"891x+UURLKVnvgpQkzwvbV8LuY60L+0SVSjlpXD9G/4pZqOrze5Up3+7tXjyGmZT\r\n" \
	"FdQ8VaXrYolRDcV4+mpsM9BRExcJCZmwEIhzFIYJyg9J2HVKUeJppBHYDUom2wKa\r\n" \
	"c0QsLZB8ZdfUSBeuRXERSBXbibKHMnbmr+DP1SNnAgMBAAEwDQYJKoZIhvcNAQEL\r\n" \
	"BQADggEBAJdmTyHbLlhrGG3vCjw6pVHxokalYSTqdXug7z1MqG80dx6KNFqoi98/\r\n" \
	"Yh049JFgtwbv94ioMm80BF+i12xGGkpFJlhUyFZUU1cBgw1FpR/Kby4il+Klajr8\r\n" \
	"ygP0wOe8vlEPUXosAq6sHjrvfhQR+3W2T5cqtujoSCh0wlgHcgyTDMZUA7cP83CQ\r\n" \
	"SeTjNEPLQhYOYRfqux3SO4Wt6NXyptS6XouxnGRODbsdFJxoTtJgQubpozeDyV+/\r\n" \
	"3YDkEMQkwqe4d3LaoBWVdYcVyEl5s/XLdcum3SL2BHNZo7cnIZmQve50YyIHUrHa\r\n" \
	"lNdugeECgJtLVjRFOwryPz8Yzw1NrVc=\r\n" \
	"-----END CERTIFICATE-----\r\n";


static const char *test_srv_key =
	"-----BEGIN PRIVATE KEY-----\r\n" \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCvaMzdXDPJm/O1\r\n" \
	"jIpwh5NIL3aKZCKtZlVqkGEakzdJoomRJ2loYtxcwCtdCDPazskEG9O3qa7SVpgK\r\n" \
	"YOzdtyFz/ETvwB5pG88tmpquibvUw6Ef9fq1e3pXZagFbbqljL9N/RQnyjrZ96YL\r\n" \
	"MY5UIdqunRvZFNeI3PoagDcrhiDGHomh891x+UURLKVnvgpQkzwvbV8LuY60L+0S\r\n" \
	"VSjlpXD9G/4pZqOrze5Up3+7tXjyGmZTFdQ8VaXrYolRDcV4+mpsM9BRExcJCZmw\r\n" \
	"EIhzFIYJyg9J2HVKUeJppBHYDUom2wKac0QsLZB8ZdfUSBeuRXERSBXbibKHMnbm\r\n" \
	"r+DP1SNnAgMBAAECggEAPXpMLxtFNViY4p0G2mjSCH9GGzv8YWzX7Bj9R5yoSIEU\r\n" \
	"mRDIo2Le3A0oRZt93ScTkgKUwi7RDHx4Hk8ZRZ4NNgMJQEREBE/QXanto9W3RNtp\r\n" \
	"6fNC8v97gg0phfmO7L33QMS+HzIETWe8RPkR6s/QvtlmrL9eyWO1rrsYWwNQEk+E\r\n" \
	"hou5a+vnTSlA1Row4nDtpVbpYaVRFChabtdaBe0hPPYZ8RYnKnM8byh0hHwnPPwB\r\n" \
	"PUodfnlm9l0TssZ/gDP0cAvdO6ENDapQ26AGEcQr3StKX6RQQeIv8oBa5nF1OOMH\r\n" \
	"O5klGRHZiAszIsp4x0jJYTZuBMxNfTaxn7yl0Eyf1QKBgQC3AzpGo6Q2lq8MehWH\r\n" \
	"92bNWLf48J1bPtL4PXvN+NOVvK2HWOgz2bxT6a5jp7q6DgkSFsCuK8PGZbsMDcer\r\n" \
	"wy4F/qniya4iDdVR4w3u6o5ADHv9AP/ArckmsT9qPjbo1zsUqmQ9H3FBA0UQw105\r\n" \
	"7wxT48SS4tbkIpg0Lgv9j5Id8wKBgQD1XVEE/GcnZgFOVzKU8VGoMc8WCyorRceF\r\n" \
	"NFJD+sQrFXaBqBjVjvsycJSSg2m6K1r38zR05NIIAQ26r4q1EQ5QjlMbZDphVScz\r\n" \
	"iK4fVuoa7oTk7gtbJva/5F5rh8zr65Owz3XXBzgjYkWu15GMNKpCJBH9TH7pMQPR\r\n" \
	"moodI82dvQKBgQCorQXiI2LvmTQfaMo9de2/J5zwiTxO4/loKKrZ7RN+N9csHupt\r\n" \
	"F6p3ILLFUMtOPi9oB6ZxIQ8g5ZRfCLhWe98IUW5dAwtGgb9x6LZn1LFlINyKFIik\r\n" \
	"GsiBq+3JyyuqsOzYIC+hEkC87fmS+K0M/2E6VHxR8VakD6p+tU4B1niStwKBgDvg\r\n" \
	"Vtopoh6qNN1iSBSywyORbCDL/rFqqOLnvr+aHW8KIM5JMC5FYyWO9aK/cPieheeG\r\n" \
	"MSHpiRus7iXFmsrO67ha0wsXLWv9NMDNtPgu5JhH5qRbNug6nPJ9D1GcZTSZW4RF\r\n" \
	"TFN6DogL/kdJkdouK/JSDRXMKhhL/suSs+uihhXdAoGAR/PIacSkqcpJahSpdVy6\r\n" \
	"+gAJqF0gvz062mA4N1TXENGNQ3XMpiE0ZRObVq5EjVyJEbrAihJRCCk6bD2Y8yRz\r\n" \
	"rf579kGgUFacJQaTwg8T5emGcB1lGE96YFOSMlktqT6dEj+J2SIG6gTnGZFtaPCB\r\n" \
	"+30jNsypEKumV3E3O7KrQaA=\r\n" \
	"-----END PRIVATE KEY-----\r\n";


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

static void example_ssl_server_verify_required_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

#if !defined(MBEDTLS_BIGNUM_C) || \
	!defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
	!defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_NET_C) || \
	!defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or "
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
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: SSL server (VERIFY_REQUIRED)\n");

	/*
	 * 1. Prepare the certificate and key
	 */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r  . Preparing the certificate and key...");

	mbedtls_x509_crt_init(&server_x509);
	mbedtls_pk_init(&server_pk);

	if ((ret = mbedtls_x509_crt_parse(&server_x509, (const unsigned char *) test_srv_crt, strlen(test_srv_crt) + 1)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if ((ret = mbedtls_pk_parse_key(&server_pk, (const unsigned char *) test_srv_key, strlen(test_srv_key) + 1, NULL, 0, NULL, NULL)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, " failed\n  ! mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}


	RTK_LOGS(NOTAG, RTK_LOG_INFO, " ok\n");

	/*
	 * 2. Start the connection
	 */
	ip = LwIP_GetIP(0);
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

		mbedtls_ssl_conf_ca_chain(&conf, server_x509.next, NULL);
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

		memset(buf, 0, sizeof(buf));
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

void example_ssl_server_verify_required(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"example_ssl_server_verify_required_thread"), example_ssl_server_verify_required_thread,
						 NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create example_ssl_server_verify_required_thread failed", __FUNCTION__);
	}
}
