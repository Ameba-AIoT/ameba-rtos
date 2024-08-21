#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"

#define SERVER_HOST    "192.168.1.100"
#define SERVER_PORT    "443"
#define RESOURCE       "/"
#define BUFFER_SIZE    2048

static const unsigned char *test_client_key = (unsigned char *) \
		"-----BEGIN PRIVATE KEY-----\r\n" \
		"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCsTIOSoYt6mO+C\r\n" \
		"PE4Lg1dk8E6k18k4yyRrt72l6PU4da6qCBUiuVE/GY76BzoUhR2I7aHjUiY7g5dj\r\n" \
		"a/SdNzsdC0jxBax782DcZ/Z2keawq2SrxsxIs65d0d6mmqiua11lO4uvT2LD5T5W\r\n" \
		"HGwJV/pRYsBmc0Nmtm65gSLspwm5ydgAjP5ZcfGrAwvL8vyX0sbDlBKhsZO4nMv6\r\n" \
		"I17Kfn6RFaXYeuf7gXu476QAqxkTFgOJ4e+wp9XkH98atdB8wrKk3CXp0Q5VLV+M\r\n" \
		"l6CS77DNuS9cS5ffVUwemoJNpOg8Ew97Ys6cDRyE6AINUMTXuyicamDM8mkbFOZw\r\n" \
		"RBdkkMIpAgMBAAECggEAAqQPljVEqoXikefzuEh0k/b89ZjbHDQ85CvGS980tv9S\r\n" \
		"xIdoL4aQoLt3Dg0DTmI18C3YP+A39syPCvaSIsDAdrMAFLWmDaZO00ICrJGS7B1M\r\n" \
		"ZzeeVPTwFL8NPxd6w2Wg4lbVzsOUO3VtjatU2pXPH+U4FrnojA/6xXwduWvcAc14\r\n" \
		"QVlbnDhd4sCLlm+3aNwUH0wY/DgEmeqYaCrIsAvKaayovDIkMAHprvDPR4MONv7r\r\n" \
		"whmxi1m7SS8jRRv7Ueci7wylB1Oa/oEoqdG9hJSKb5JKKJ9b4O5qO193ahVrlOr/\r\n" \
		"f1CBD1xuuYZJdyThQEq3g8QVZ0ejpKQdA/Cgr9avWwKBgQDKlsHB4j990F/nfkTd\r\n" \
		"uP5tRTBxga2Wl+0YV256lxyLbP67Fn+IQAtlCwVQaaToYdc9m7La5ewAhCTxTUa2\r\n" \
		"+KosygpK1DCwT4GnNuSqCie/TQIivx1c42z8k605RRw1RoXvTN9mTodkEV+xnuqu\r\n" \
		"pMt3Qqxk0DpKJTT2b+CR8mFLDwKBgQDZuWgb0xPX2ocFeIugkj/GWP9FzpbUfs3y\r\n" \
		"Y9Yvztk7Rfd3OAKqv6QKe9alkhi8l/rq4PGf7jclzXWGZYRl37oDk9K6V9qlm5zl\r\n" \
		"UmkY25fmQrpuX1/MjwKle3UtHTgfkV+n8FQqBowyiXJmPR/GIZ1MDQGay6YsZW9h\r\n" \
		"RSmjJR//RwKBgG0KG5uJdPb40sW8dsCIM5kIpnxReecWOqzhUjt1Xg3FSo3UHCxv\r\n" \
		"nDJ0bhXL5tCU6M7mIKwlGJNmjKO8HhWZXa7hyAxijRo4kOY5a3YCJUaX4lBlYR5/\r\n" \
		"4kdIDPJIOliKbshQLeXY2pKHr2PgzxvU4xZ7smJImTcTM6B4svya4rcNAoGABkx+\r\n" \
		"3o80u8HuthHOuE6C2dIpUHOxmWQxwRlyUN/DGRvlFOXh7d6teEAs7Y1UeeBLug/X\r\n" \
		"7bCOl+FhzQmS0v+t+fyE1C9GfBQMbfX8ZGO1+Utk6VyJAPoCMZdoxG1b+k3SXqFt\r\n" \
		"TenylGO1k4+hI7JFQrX9neTWYeJtXnsJwVaJnnECgYEAtgLApWfQVyr3yyGh7ur3\r\n" \
		"lYqQI5U6EAH8p8yA3otTeI22BgOvARH57yjK2PDyhFuA2efFX2+ipNAIXmiiIl1K\r\n" \
		"wvu0YpiZAkH8D0dmxECS/GaluoqhA7pK6QjlRS2sFZ9n4C8vet79bFYZQ4fXazKw\r\n" \
		"w4KeMcb6l+GyVQv9CSdB5lw=\r\n" \
		"-----END PRIVATE KEY-----\r\n";

static const unsigned char *test_client_crt = (unsigned char *) \
		"-----BEGIN CERTIFICATE-----\r\n" \
		"MIIDozCCAosCFG8LuhxSx/IA0zALJmeB5JUV7jmiMA0GCSqGSIb3DQEBCwUAMIGN\r\n" \
		"MQswCQYDVQQGEwJDTjEQMA4GA1UECAwHSmlhbmdTdTEPMA0GA1UEBwwGU3VaaG91\r\n" \
		"MRAwDgYDVQQKDAdSZWFsc2lsMQ8wDQYDVQQLDAZjbGllbnQxFDASBgNVBAMMC2Ns\r\n" \
		"aWVudF90ZXN0MSIwIAYJKoZIhvcNAQkBFhNjbGllbnRfdGVzdEAxMjMuY29tMB4X\r\n" \
		"DTIzMDYwNjA3MTk0NloXDTMzMDYwMzA3MTk0NlowgY0xCzAJBgNVBAYTAkNOMRAw\r\n" \
		"DgYDVQQIDAdKaWFuZ1N1MQ8wDQYDVQQHDAZTdVpob3UxEDAOBgNVBAoMB1JlYWxz\r\n" \
		"aWwxDzANBgNVBAsMBmNsaWVudDEUMBIGA1UEAwwLY2xpZW50X3Rlc3QxIjAgBgkq\r\n" \
		"hkiG9w0BCQEWE2NsaWVudF90ZXN0QDEyMy5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
		"A4IBDwAwggEKAoIBAQCsTIOSoYt6mO+CPE4Lg1dk8E6k18k4yyRrt72l6PU4da6q\r\n" \
		"CBUiuVE/GY76BzoUhR2I7aHjUiY7g5dja/SdNzsdC0jxBax782DcZ/Z2keawq2Sr\r\n" \
		"xsxIs65d0d6mmqiua11lO4uvT2LD5T5WHGwJV/pRYsBmc0Nmtm65gSLspwm5ydgA\r\n" \
		"jP5ZcfGrAwvL8vyX0sbDlBKhsZO4nMv6I17Kfn6RFaXYeuf7gXu476QAqxkTFgOJ\r\n" \
		"4e+wp9XkH98atdB8wrKk3CXp0Q5VLV+Ml6CS77DNuS9cS5ffVUwemoJNpOg8Ew97\r\n" \
		"Ys6cDRyE6AINUMTXuyicamDM8mkbFOZwRBdkkMIpAgMBAAEwDQYJKoZIhvcNAQEL\r\n" \
		"BQADggEBAAJYPqTxLqrQeTzQ3C9gvueH/30COVIz8q5bzipUafePbEy46ynpwuDc\r\n" \
		"OMVJGqdXb/orO0SEAacj7AXI49W07HRfU5uSu/OjdTa+CGEkabMF+oYYswrAOq5d\r\n" \
		"JjSXUmAn5VWjqJT4lzIegZENrYsTc1uPSg1mIitpOew4KU8LpnGbXNFiZB/8Y5TN\r\n" \
		"5rd3PD7OklBNLBcDf7/hY5PXPy2a7nrpA6Cd2fHULFfTGVxQIbCW+oDXXTjLGeuT\r\n" \
		"ky9y9L1KOHnIIl1T3kM40gSMWfAGP1bANUYhojB4zK+T2PuzsBon9vgzJylhqoOF\r\n" \
		"l7c6sZbFeWhh2E/rMWbat23ALMH0LI8=\r\n" \
		"-----END CERTIFICATE-----\r\n";

static const unsigned char *test_ca_crt = (unsigned char *) \
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

static int _verify_func(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
	/* To avoid gcc warnings */
	(void) data;
	(void) depth;

	char buf[1024];
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if (*flags) {
		RTK_LOGS(NOTAG, "\nERROR: certificate verify\n%s\n", buf);
	} else {
		RTK_LOGS(NOTAG, "\nCertificate verified\n%s\n", buf);
	}

	return 0;
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	UNUSED(p_rng);
	TRNG_get_random_bytes(output, output_len);
	return 0;
}

static void example_ssl_client_verify_both_thread(void *param)
{
	UNUSED(param);

	int ret;
	mbedtls_net_context server_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt client_x509;
	mbedtls_pk_context client_pk;

	RTK_LOGS(NOTAG, "\nExample: SSL client (VERIFY_BOTH)\n");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");

		RTK_LOGS(NOTAG, "Please use AT+WLCONN=ssid,***,pw,*** to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	mbedtls_x509_crt_init(&client_x509);
	mbedtls_pk_init(&client_pk);

	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);

	if ((ret = mbedtls_x509_crt_parse(&client_x509, (const unsigned char *) test_client_crt, strlen((char const *)test_client_crt) + 1)) != 0) {
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if ((ret = mbedtls_x509_crt_parse(&client_x509, (const unsigned char *) test_ca_crt, strlen((char const *)test_ca_crt) + 1)) != 0) {
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

#if defined(MBEDTLS_VERSION_NUMBER) && (MBEDTLS_VERSION_NUMBER == 0x03000000)
	if ((ret = mbedtls_pk_parse_key(&client_pk, (const unsigned char *) test_client_key, strlen((char const *)test_client_key) + 1, NULL, 0, NULL, NULL)) != 0) {
#else
	if ((ret = mbedtls_pk_parse_key(&client_pk, (const unsigned char *) test_client_key, strlen((char const *)test_client_key) + 1, NULL, 0)) != 0) {
#endif
		RTK_LOGS(NOTAG, " failed\n  ! mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}


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

	mbedtls_ssl_conf_ca_chain(&conf, client_x509.next, NULL);
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_verify(&conf, _verify_func, NULL);
	mbedtls_ssl_conf_rng(&conf, my_random, NULL);
	mbedtls_ssl_conf_own_cert(&conf, &client_x509, &client_pk);

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
	mbedtls_x509_crt_free(&client_x509);
	mbedtls_pk_free(&client_pk);

	rtos_task_delete(NULL);
}

void example_ssl_client_verify_both(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"example_ssl_client_verify_both_thread"), example_ssl_client_verify_both_thread,
						 NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(example_ssl_client_verify_both_thread) failed", __FUNCTION__);
	}
}
