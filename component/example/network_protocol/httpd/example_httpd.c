#include "platform_stdlib.h"
#include "basic_types.h"
#include "httpd/httpd.h"
#include "os_wrapper.h"
#include "ameba.h"

#define USE_HTTPS    0

#if USE_HTTPS

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

static const char *mbedtls_test_ca_crt =                                              \
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

#endif

void homepage_cb(struct httpd_conn *conn)
{
	char *user_agent = NULL;

	// test log to show brief header parsing
	httpd_conn_dump_header(conn);

	// test log to show extra User-Agent header field
	if (httpd_request_get_header_field(conn, (char *)"User-Agent", &user_agent) != -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nUser-Agent=[%s]\n", user_agent);
		httpd_free(user_agent);
	}

	// GET homepage
	if (httpd_request_is_method(conn, (char *)"GET")) {
		const char *body = \
						   "<HTML><BODY>" \
						   "It Works<BR>" \
						   "<BR>" \
						   "Can test GET by <A href=\"/test_get?test1=one%20%26%202&test2=three%3D3\">/test_get?test1=one%20%26%202&test2=three%3D3</A><BR>" \
						   "Can test POST by UI in <A href=\"/test_post.htm\">/test_post.htm</A><BR>" \
						   "</BODY></HTML>";

		// write HTTP response
		httpd_response_write_header_start(conn, (char *)"200 OK", (char *)"text/html", strlen(body));
		httpd_response_write_header(conn, (char *)"Connection", (char *)"close");
		httpd_response_write_header_finish(conn);
		httpd_response_write_data(conn, (uint8_t *)body, strlen(body));
	} else {
		// HTTP/1.1 405 Method Not Allowed
		httpd_response_method_not_allowed(conn, NULL);
	}

	httpd_conn_close(conn);
}

void test_get_cb(struct httpd_conn *conn)
{
	// GET /test_post
	if (httpd_request_is_method(conn, (char *)"GET")) {
		char *test1 = NULL, *test2 = NULL;

		// get 'test1' and 'test2' in query string
		if ((httpd_request_get_query_key(conn, (char *)"test1", &test1) != -1) &&
			(httpd_request_get_query_key(conn, (char *)"test2", &test2) != -1)) {

			// write HTTP response
			httpd_response_write_header_start(conn, (char *)"200 OK", (char *)"text/plain", 0);
			httpd_response_write_header(conn, (char *)"Connection", (char *)"close");
			httpd_response_write_header_finish(conn);
			httpd_response_write_data(conn, (uint8_t *)"\r\nGET query string", strlen("\r\nGET query string"));
			httpd_response_write_data(conn, (uint8_t *)"\r\ntest1: ", strlen("\r\ntest1: "));
			httpd_response_write_data(conn, (uint8_t *)test1, strlen(test1));
			httpd_response_write_data(conn, (uint8_t *)"\r\ntest2: ", strlen("\r\ntest2: "));
			httpd_response_write_data(conn, (uint8_t *)test2, strlen(test2));
		} else {
			// HTTP/1.1 400 Bad Request
			httpd_response_bad_request(conn, (char *)"Bad Request - test1 or test2 not in query string");
		}

		if (test1) {
			httpd_free(test1);
		}

		if (test2) {
			httpd_free(test2);
		}
	} else {
		// HTTP/1.1 405 Method Not Allowed
		httpd_response_method_not_allowed(conn, NULL);
	}

	httpd_conn_close(conn);
}

void test_post_htm_cb(struct httpd_conn *conn)
{
	// GET /test_post.htm
	if (httpd_request_is_method(conn, (char *)"GET")) {
		const char *body = \
						   "<HTML><BODY>" \
						   "<FORM action=\"/test_post\" method=\"post\">" \
						   "Text1: <INPUT type=\"text\" name=\"text1\" size=\"50\" maxlength=\"50\"><BR>" \
						   "Text2: <INPUT type=\"text\" name=\"text2\" size=\"50\" maxlength=\"50\"><BR>" \
						   "<INPUT type=\"submit\" value=\"POST\"><BR>" \
						   "</FORM>" \
						   "</BODY></HTML>";

		// write HTTP response
		httpd_response_write_header_start(conn, (char *)"200 OK", (char *)"text/html", strlen(body));
		httpd_response_write_header(conn, (char *)"Connection", (char *)"close");
		httpd_response_write_header_finish(conn);
		httpd_response_write_data(conn, (uint8_t *)body, strlen(body));
	} else {
		// HTTP/1.1 405 Method Not Allowed
		httpd_response_method_not_allowed(conn, NULL);
	}

	httpd_conn_close(conn);
}

void test_post_cb(struct httpd_conn *conn)
{
	// POST /test_post
	if (httpd_request_is_method(conn, (char *)"POST")) {
		size_t read_size;
		uint8_t buf[50];
		size_t content_len = conn->request.content_len;
		uint8_t *body = (uint8_t *) malloc(content_len + 1);

		if (body) {
			// read HTTP body
			memset(body, 0, content_len + 1);
			read_size = httpd_request_read_data(conn, body, content_len);

			// write HTTP response
			httpd_response_write_header_start(conn, (char *)"200 OK", (char *)"text/plain", 0);
			httpd_response_write_header(conn, (char *)"Connection", (char *)"close");
			httpd_response_write_header_finish(conn);
			memset(buf, 0, sizeof(buf));
			sprintf((char *)buf, "%d bytes from POST: ", read_size);
			httpd_response_write_data(conn, buf, strlen((char const *)buf));
			httpd_response_write_data(conn, body, strlen((char const *)body));
			free(body);
		} else {
			// HTTP/1.1 500 Internal Server Error
			httpd_response_internal_server_error(conn, NULL);
		}
	} else {
		// HTTP/1.1 405 Method Not Allowed
		httpd_response_method_not_allowed(conn, NULL);
	}

	httpd_conn_close(conn);
}

static void example_httpd_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: HTTPD\n");
#if USE_HTTPS
	if (httpd_setup_cert(mbedtls_test_srv_crt, mbedtls_test_srv_key, mbedtls_test_ca_crt) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\nERROR: httpd_setup_cert\n");
		goto exit;
	}
#endif
	httpd_reg_page_callback((char *)"/", homepage_cb);
	httpd_reg_page_callback((char *)"/index.htm", homepage_cb);
	httpd_reg_page_callback((char *)"/test_get", test_get_cb);
	httpd_reg_page_callback((char *)"/test_post.htm", test_post_htm_cb);
	httpd_reg_page_callback((char *)"/test_post", test_post_cb);
#if USE_HTTPS
	if (httpd_start(443, 5, 4096, HTTPD_THREAD_SINGLE, HTTPD_SECURE_TLS) != 0) {
#else
	if (httpd_start(80, 5, 4096, HTTPD_THREAD_SINGLE, HTTPD_SECURE_NONE) != 0) {
#endif
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: httpd_start");
		httpd_clear_page_callbacks();
	}
#if USE_HTTPS
exit:
#endif
	rtos_task_delete(NULL);
}

void example_httpd(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_httpd_thread"), example_httpd_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_httpd_thread) failed", __FUNCTION__);
	}
}
