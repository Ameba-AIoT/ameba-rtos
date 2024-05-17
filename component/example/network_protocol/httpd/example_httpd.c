#include "platform_stdlib.h"
#include "basic_types.h"
#include "httpd/httpd.h"

#define USE_HTTPS    0

#if USE_HTTPS
// use test_srv_crt, test_srv_key, test_ca_list in PolarSSL certs.c
#include <mbedtls/certs.h>
#endif

void homepage_cb(struct httpd_conn *conn)
{
	char *user_agent = NULL;

	// test log to show brief header parsing
	httpd_conn_dump_header(conn);

	// test log to show extra User-Agent header field
	if (httpd_request_get_header_field(conn, (char *)"User-Agent", &user_agent) != -1) {
		printf("\nUser-Agent=[%s]\n", user_agent);
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
	printf("\nExample: HTTPD\n");
#if USE_HTTPS
	if (httpd_setup_cert(mbedtls_test_srv_crt, mbedtls_test_srv_key, mbedtls_test_ca_crt) != 0) {
		printf("\nERROR: httpd_setup_cert\n");
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
		printf("ERROR: httpd_start");
		httpd_clear_page_callbacks();
	}
#if USE_HTTPS
exit:
#endif
	rtos_task_delete(NULL);
}

void example_httpd(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_httpd_thread"), example_httpd_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_httpd_thread) failed", __FUNCTION__);
	}
}
