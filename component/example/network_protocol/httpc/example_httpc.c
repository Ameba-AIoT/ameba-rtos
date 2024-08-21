#include "platform_stdlib.h"
#include "basic_types.h"
#include "httpc/httpc.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define STACKSIZE    2048
#define USE_HTTPS    0
static const char *server_host = "httpbin.org";

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
#endif

static void example_httpc_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	extern void rtos_create_secure_context(u32 secure_stack_size);
	rtos_create_secure_context(STACKSIZE * 2);
	extern int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void);
	secure_mbedtls_platform_set_calloc_free();
#endif

	struct httpc_conn *conn = NULL;

	// Delay to wait for IP by DHCP
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		printf("Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}
	printf("\nExample: HTTPC\n");

	/* test GET to http://httpbin.org/get?param1=test_data1&param2=test_data2 */
#if USE_HTTPS
	conn = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
#else
	conn = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
#endif
	if (conn) {
#if USE_HTTPS
		if (httpc_conn_connect(conn, (char *)server_host, 443, 0) == 0) {
#else
		if (httpc_conn_connect(conn, (char *)server_host, 80, 0) == 0) {
#endif
			/* HTTP GET request */
			// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
			httpc_request_write_header_start(conn, (char *)"GET", (char *)"/get?param1=test_data1&param2=test_data2", NULL, 0);
			// add other required header fields if necessary
			httpc_request_write_header(conn, (char *)"Connection", (char *)"close");
			// finish and send header
			httpc_request_write_header_finish(conn);

			// receive response header
			if (httpc_response_read_header(conn) == 0) {
				httpc_conn_dump_header(conn);

				// receive response body
				if (httpc_response_is_status(conn, (char *)"200 OK")) {
					uint8_t buf[1024];
					int read_size = 0;
					uint32_t total_size = 0;

					while (1) {
						memset(buf, 0, sizeof(buf));
						read_size = httpc_response_read_data(conn, buf, sizeof(buf) - 1);

						if (read_size > 0) {
							total_size += read_size;
							printf("%s", buf);
						} else {
							break;
						}

						if (conn->response.content_len && (total_size >= conn->response.content_len)) {
							break;
						}
					}
				}
			}
		} else {
			printf("\nERROR: httpc_conn_connect\n");
		}

		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	/* test POST to http://httpbin.org/post with data of param1=test_data1&param2=test_data2 */
#if USE_HTTPS
	conn = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);
#else
	conn = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);
#endif
	if (conn) {
#if USE_HTTPS
		if (httpc_conn_connect(conn, (char *)server_host, 443, 0) == 0) {
#else
		if (httpc_conn_connect(conn, (char *)server_host, 80, 0) == 0) {
#endif
			/* HTTP POST request */
			const char *post_data = "param1=test_data1&param2=test_data2";
			// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
			httpc_request_write_header_start(conn, (char *)"POST", (char *)"/post", NULL, strlen(post_data));
			// add other header fields if necessary
			httpc_request_write_header(conn, (char *)"Connection", (char *)"close");
			// finish and send header
			httpc_request_write_header_finish(conn);
			// send http body
			httpc_request_write_data(conn, (uint8_t *)post_data, strlen(post_data));

			// receive response header
			if (httpc_response_read_header(conn) == 0) {
				httpc_conn_dump_header(conn);

				// receive response body
				if (httpc_response_is_status(conn, (char *)"200 OK")) {
					uint8_t buf[1024];
					int read_size = 0;
					uint32_t total_size = 0;

					while (1) {
						memset(buf, 0, sizeof(buf));
						read_size = httpc_response_read_data(conn, buf, sizeof(buf) - 1);

						if (read_size > 0) {
							total_size += read_size;
							printf("%s", buf);
						} else {
							break;
						}

						if (conn->response.content_len && (total_size >= conn->response.content_len)) {
							break;
						}
					}
				}
			}
		} else {
			printf("\nERROR: httpc_conn_connect\n");
		}

		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	rtos_task_delete(NULL);
}

void example_httpc(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_httpc_thread"), example_httpc_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_httpc_thread) failed", __FUNCTION__);
	}
}
