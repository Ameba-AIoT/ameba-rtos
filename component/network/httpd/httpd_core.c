#include "ameba_soc.h"
#include "lwip_netconf.h"
#include "httpd_int.h"
#include "httpd_util.h"


// setting
#define HTTPD_DEFAULT_PORT             80
#define HTTPD_DEFAULT_SECURE_PORT      443
#define HTTPD_DEFAULT_MAX_CONN         1
#define HTTPD_DEFAULT_STACK_BYTES      4096
#define HTTPD_LISTEN_LEN               3
#define HTTPD_DEFAULT_SELECT_TIMEOUT   10
#define HTTPD_RECV_TIMEOUT             5
#define HTTPD_PRIORITY                 (1)

static uint16_t httpd_port = HTTPD_DEFAULT_PORT;
static uint32_t httpd_stack_bytes = HTTPD_DEFAULT_STACK_BYTES;
static uint8_t httpd_thread_mode = HTTPD_THREAD_SINGLE;
static uint8_t httpd_secure = HTTPD_SECURE_NONE;
static int httpd_select_timeout = HTTPD_DEFAULT_SELECT_TIMEOUT;
uint8_t httpd_debug = HTTPD_DEBUG_ON;
uint8_t httpd_max_conn = HTTPD_DEFAULT_MAX_CONN;
static uint8_t httpd_method_cb = 0;

// run-time
static uint8_t httpd_running = 0;
static uint8_t httpd_do_stop = 0;
static uint8_t httpd_idle_timeout_enabled = 0;
static int httpd_sock = -1;
static int httpd_max_sock = -1;
struct httpd_page *httpd_page_database = NULL;
struct httpd_conn *httpd_connections = NULL;
static char *httpd_user_password = NULL;
static uint8_t httpd_priority = HTTPD_PRIORITY;

httpd_handle_t httpd_handle;

httpd_uri_t *httpd_find_uri_handler(httpd_method_t method)
{
	struct httpd_data *hd = (struct httpd_data *)httpd_handle;

	for (int i = 0; i < hd->config.max_uri_handlers; i++) {
		if (!hd->hd_calls[i]) {
			break;
		}

		if (hd->hd_calls[i]->method == method) {
			return hd->hd_calls[i];
		}
	}
	return NULL;
}

int httpd_register_uri_handler(const httpd_uri_t *uri_handler)
{
	if (uri_handler == NULL) {
		return -1;
	}

	struct httpd_data *hd = (struct httpd_data *) httpd_handle;

	/* Make sure another handler with matching URI and method
	* is not already registered. This will also catch cases
	* when a registered URI wildcard pattern already accounts
	* for the new URI being registered */
	if (httpd_find_uri_handler(uri_handler->method) != NULL) {
		httpd_log("handler with method %d already registered\n", uri_handler->method);
		return -1;
	}

	for (int i = 0; i < hd->config.max_uri_handlers; i++) {
		if (hd->hd_calls[i] == NULL) {
			hd->hd_calls[i] = malloc(sizeof(httpd_uri_t));
			if (hd->hd_calls[i] == NULL) {
				httpd_log("Failed to allocate memory for hd_calls\n");
				return -1;
			}

			/* Copy members */
			hd->hd_calls[i]->method   = uri_handler->method;
			hd->hd_calls[i]->handler  = uri_handler->handler;

			httpd_log_verbose("[%d] installed %d\n", i, uri_handler->method);
			return 0;
		}
		httpd_log_verbose("[%d] exists %d\n", i, hd->hd_calls[i]->method);
	}
	httpd_log("no slots left for registering handler\n");
	return -1;
}


int httpd_unregister_uri_handler(const httpd_uri_t *uri_handler)
{
	struct httpd_data *hd = (struct httpd_data *) httpd_handle;
	for (int i = 0; i < hd->config.max_uri_handlers; i++) {
		if (!hd->hd_calls[i]) {
			break;
		}
		if ((hd->hd_calls[i]->method == uri_handler->method)) { // First match methods
			httpd_log_verbose("[%d] removing %d\n", i, hd->hd_calls[i]->method);

			free(hd->hd_calls[i]);
			hd->hd_calls[i] = NULL;

			/* Shift the remaining non null handlers in the array
			* forward by 1 so that order of insertion is maintained */
			for (i += 1; i < hd->config.max_uri_handlers; i++) {
				if (!hd->hd_calls[i]) {
					break;
				}
				hd->hd_calls[i - 1] = hd->hd_calls[i];
			}
			/* Nullify the following non null entry */
			hd->hd_calls[i - 1] = NULL;
			return 0;
		}
	}
	httpd_log("handler with method %d not found\n", uri_handler->method);
	return -1;
}

static int httpd_conn_verify(struct httpd_conn *conn)
{
	int ret = 0;
	char *auth = NULL;
	char *auth_basic = NULL;
	char *auth_base64 = NULL;

	if (httpd_request_get_header_field(conn, "Authorization", &auth) != 0) {
		ret = -1;
		goto exit;
	}

	auth_basic = strstr(auth, "Basic ");
	auth_base64 = auth_basic + strlen("Basic ");

	if ((auth_basic == NULL) || (strcmp(auth_base64, httpd_user_password) != 0)) {
		ret = -1;
		goto exit;
	}

exit:
	if (auth) {
		httpd_free(auth);
	}

	return ret;
}

static void httpd_conn_handler(struct httpd_conn *conn)
{
	struct httpd_page *page;
	struct timeval recv_timeout, recv_timeout_backup;
	size_t recv_timeout_len;
	httpd_uri_t	*uri = NULL;
	httpd_method_t	method;

	recv_timeout_len = sizeof(recv_timeout_backup);

	if (getsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_backup, &recv_timeout_len) != 0) {
		httpd_log("ERROR: SO_RCVTIMEO");
	}

	recv_timeout.tv_sec = HTTPD_RECV_TIMEOUT;
	recv_timeout.tv_usec = 0;

	if (setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) != 0) {
		httpd_log("ERROR: SO_RCVTIMEO");
	}

	if (httpd_request_read_header(conn) == -1) {
		httpd_response_bad_request(conn, NULL);
		goto exit;
	}

	if (httpd_user_password) {
		if (httpd_conn_verify(conn) == -1) {
			httpd_response_unauthorized(conn, NULL);
			goto exit;
		}
	}

	if (setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_backup, sizeof(recv_timeout_backup)) != 0) {
		httpd_log("ERROR: SO_RCVTIMEO");
	}

	if (httpd_method_cb) {
		if (strncmp(conn->request.method, "GET", conn->request.method_len) == 0) {
			method = HTTP_GET;
		} else if (strncmp(conn->request.method, "POST", conn->request.method_len) == 0) {
			method = HTTP_POST;
		} else if (strncmp(conn->request.method, "PUT", conn->request.method_len) == 0) {
			method = HTTP_PUT;
		} else {
			httpd_response_method_not_allowed(conn, NULL);
			return;
		}

		uri = httpd_find_uri_handler(method);
		if (uri) {
			uri->handler(conn);
		} else {
			httpd_response_method_not_allowed(conn, NULL);
		}
	} else {
		// call page callback
		page = httpd_page_database;

		while (page) {
			if ((strlen(page->path) == conn->request.path_len) &&
				(memcmp(page->path, conn->request.path, conn->request.path_len) == 0)) {

				(page->callback)(conn);
				break;
			}

			page = page->next;
		}

		if (page == NULL) {
			httpd_response_not_found(conn, NULL);
			goto exit;
		}
	}

	// update last request time
	conn->last_req_time = rtos_time_get_current_system_time_ms();

	return;

exit:
	httpd_conn_close(conn);
}

static void httpd_conn_thread(void *param)
{
	struct httpd_conn *conn = (struct httpd_conn *) param;

	httpd_log_verbose("%s started", __FUNCTION__);

	while (conn->sock >= 0) {
		fd_set read_fds;
		struct timeval timeout;

		timeout.tv_sec = httpd_select_timeout;
		timeout.tv_usec = 0;
		FD_ZERO(&read_fds);
		FD_SET(conn->sock, &read_fds);

		if (select(conn->sock + 1, &read_fds, NULL, NULL, &timeout)) {
			if (FD_ISSET(conn->sock, &read_fds)) {
				httpd_conn_handler(conn);
			}
		}
		// for multi-thread mode, it must be idle timeout when select timeout.
		else if (httpd_idle_timeout_enabled) {
			httpd_conn_close(conn);
		}
	}

	httpd_conn_detach(conn);
	httpd_log_verbose("%s stopped", __FUNCTION__);
	rtos_task_delete(NULL);
}

static void httpd_server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	httpd_log("%s started", __FUNCTION__);
	httpd_running = 1;

	while (!httpd_do_stop) {
		fd_set read_fds;
		struct timeval timeout;

		timeout.tv_sec = httpd_select_timeout;
		timeout.tv_usec = 0;
		FD_ZERO(&read_fds);
		FD_SET(httpd_sock, &read_fds);

		if (httpd_thread_mode == HTTPD_THREAD_SINGLE) {
			int i;

			for (i = 0; i < httpd_max_conn; i ++) {
				if (httpd_connections[i].sock >= 0) {
					FD_SET(httpd_connections[i].sock, &read_fds);
				}
			}
		}

		if (select(httpd_max_sock + 1, &read_fds, NULL, NULL, &timeout)) {
			if (FD_ISSET(httpd_sock, &read_fds)) {
				struct sockaddr_in client_addr;
				unsigned int client_addr_size = sizeof(client_addr);
				int client_sock;
				struct httpd_conn *conn = NULL;

				if ((client_sock = accept(httpd_sock, (struct sockaddr *) &client_addr, &client_addr_size)) >= 0) {
					httpd_log_verbose("accept(%d) from 0x%08X:%d", client_sock, (unsigned int)client_addr.sin_addr.s_addr, htons(client_addr.sin_port));
					conn = httpd_conn_add(client_sock);

					if (conn == NULL) {
						struct httpd_conn tmp_conn;

						httpd_log("ERROR: httpd_conn_add");
						memset(&tmp_conn, 0, sizeof(struct httpd_conn));
						tmp_conn.sock = client_sock;

						if (httpd_secure) {
							tmp_conn.tls = httpd_tls_new_handshake(&tmp_conn.sock, httpd_secure);

							if (tmp_conn.tls == NULL) {
								httpd_log("ERROR: httpd_tls_new_handshake");
								close(client_sock);
								continue;
							}
						}

						httpd_response_too_many_requests(&tmp_conn, NULL);

						if (tmp_conn.tls) {
							httpd_tls_close(tmp_conn.tls);
							httpd_tls_free(tmp_conn.tls);
						}

						close(client_sock);
					} else {
						// enable socket keepalive with keepalive timeout = idle(3) + interval(5) * count(3) = 18 seconds
						int keepalive = 1, keepalive_idle = 3, keepalive_interval = 5, keepalive_count = 3;

						if (setsockopt(conn->sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
							httpd_log("ERROR: SO_KEEPALIVE");
						}
						if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
							httpd_log("ERROR: TCP_KEEPIDLE");
						}
						if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
							httpd_log("ERROR: TCP_KEEPINTVL");
						}
						if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
							httpd_log("ERROR: TCP_KEEPCNT");
						}

						// set conn ssl
						if (httpd_secure) {
							conn->tls = httpd_tls_new_handshake(&conn->sock, httpd_secure);

							if (conn->tls == NULL) {
								httpd_log("ERROR: httpd_tls_new_handshake");
								httpd_conn_remove(conn);

								if (httpd_thread_mode == HTTPD_THREAD_MULTIPLE) {
									httpd_conn_detach(conn);
								}

								continue;
							}
						}

						// update max sock for select
						if (client_sock > httpd_max_sock) {
							httpd_max_sock = client_sock;
						}

						// update last request time
						conn->last_req_time = rtos_time_get_current_system_time_ms();

						// create thread for multi-thread mode
						if (httpd_thread_mode == HTTPD_THREAD_MULTIPLE) {
							if (rtos_task_create(NULL, ((const char *) "httpd_conn"), httpd_conn_thread, conn, httpd_stack_bytes, httpd_priority) != RTK_SUCCESS) {
								httpd_log("ERROR: rtos_task_create httpd_conn");
								httpd_conn_remove(conn);
								httpd_conn_detach(conn);
							} else {
								httpd_log("Create httpd_conn_thread successful, priority is %d", httpd_priority);
							}
						}
					}
				} else {
					httpd_log("ERROR: accept(%d)", client_sock);
					// goto exit;
				}
			} else if (httpd_thread_mode == HTTPD_THREAD_SINGLE) {
				int i;

				for (i = 0; i < httpd_max_conn; i ++) {
					if ((httpd_connections[i].sock >= 0) && (FD_ISSET(httpd_connections[i].sock, &read_fds))) {
						httpd_conn_handler(&httpd_connections[i]);
					}
				}
			}
		} else {
			httpd_log_verbose("select");
		}

		// check idle timeout for single-thread mode. idle timeout for multi-thread mode will be check in conn thread
		if ((httpd_thread_mode == HTTPD_THREAD_SINGLE) && httpd_idle_timeout_enabled) {
			int i;
			uint32_t cur_time = rtos_time_get_current_system_time_ms();

			for (i = 0; i < httpd_max_conn; i ++) {
				if ((httpd_connections[i].sock >= 0) && (cur_time >= (httpd_connections[i].last_req_time + httpd_select_timeout * 1000))) {
					httpd_conn_close(&httpd_connections[i]);
				}
			}
		}
	}

	httpd_deinit();
	httpd_log("%s stopped", __FUNCTION__);
	httpd_running = 0;
	rtos_task_delete(NULL);
}

/* Internal Interface */
int httpd_init(void)
{
	int ret = 0;

	httpd_connections = (struct httpd_conn *) httpd_malloc(sizeof(struct httpd_conn) * httpd_max_conn);

	if (httpd_connections) {
		int i;

		memset(httpd_connections, 0, sizeof(struct httpd_conn) * httpd_max_conn);

		for (i = 0; i < httpd_max_conn; i ++) {
			httpd_connections[i].sock = -1;
		}
	} else {
		ret = -1;
		goto exit;
	}

	if ((httpd_sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		struct sockaddr_in httpd_addr;
		int enable = 1;

		httpd_addr.sin_family = AF_INET;
		httpd_addr.sin_port = htons(httpd_port);
		httpd_addr.sin_addr.s_addr = INADDR_ANY;

		if (setsockopt(httpd_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&enable, sizeof(enable)) != 0) {
			httpd_log("ERROR: SO_REUSEADDR\n");
		}

		if ((ret = bind(httpd_sock, (struct sockaddr *) &httpd_addr, sizeof(httpd_addr))) != 0) {
			httpd_log("ERROR: bind");
			goto exit;
		}

		if ((ret = listen(httpd_sock, HTTPD_LISTEN_LEN)) != 0) {
			httpd_log("ERROR: listen");
			goto exit;
		}

		httpd_max_sock = httpd_sock;
	} else {
		ret = -1;
		goto exit;
	}

exit:
	if (ret) {
		if (httpd_sock != -1) {
			close(httpd_sock);
			httpd_sock = -1;
		}

		if (httpd_connections) {
			httpd_free(httpd_connections);
			httpd_connections = NULL;
		}
	}

	return ret;
}

void httpd_deinit(void)
{
	// free httpd resource
	httpd_page_clear();
	httpd_conn_clear();
	httpd_free(httpd_connections);
	httpd_connections = NULL;

	if (httpd_sock != -1) {
		close(httpd_sock);
		httpd_sock = -1;
	}

	if (httpd_secure) {
		httpd_tls_setup_free();
	}

	if (httpd_user_password) {
		httpd_free(httpd_user_password);
		httpd_user_password = NULL;
	}
}

void httpd_setup(uint16_t port, uint8_t max_conn, uint8_t secure)
{
	if (port > 0) {
		httpd_port = port;
	} else if (secure) {
		httpd_port = HTTPD_DEFAULT_SECURE_PORT;
	}

	if (max_conn > 0) {
		httpd_max_conn = max_conn;
	}

	httpd_secure = secure;
}

int httpd_return_server_sock(void)
{
	return httpd_sock;
}

int httpd_accept(int server_sock, struct httpd_conn **client_conn, int accept_timeout)
{
	int ret = 0;
	fd_set read_fds;
	struct timeval timeout;

	timeout.tv_sec = accept_timeout;
	timeout.tv_usec = 0;
	FD_ZERO(&read_fds);
	FD_SET(server_sock, &read_fds);

	if (select(server_sock + 1, &read_fds, NULL, NULL, &timeout)) {
		if (FD_ISSET(server_sock, &read_fds)) {
			struct sockaddr_in client_addr;
			unsigned int client_addr_size = sizeof(client_addr);
			int client_sock;
			struct httpd_conn *conn = NULL;

			if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size)) >= 0) {
				httpd_log_verbose("accept(%d)", client_sock);
				conn = httpd_conn_add(client_sock);

				if (conn == NULL) {
					struct httpd_conn tmp_conn;

					httpd_log("ERROR: httpd_conn_add");
					memset(&tmp_conn, 0, sizeof(struct httpd_conn));
					tmp_conn.sock = client_sock;

					if (httpd_secure) {
						tmp_conn.tls = httpd_tls_new_handshake(&tmp_conn.sock, httpd_secure);

						if (tmp_conn.tls == NULL) {
							httpd_log("ERROR: httpd_tls_new_handshake");
							close(client_sock);
							ret = -1;
							goto exit;
						}
					}

					httpd_response_too_many_requests(&tmp_conn, NULL);

					if (tmp_conn.tls) {
						httpd_tls_close(tmp_conn.tls);
						httpd_tls_free(tmp_conn.tls);
					}

					close(client_sock);
					ret = -1;
					goto exit;
				} else {
					// enable socket keepalive with keepalive timeout = idle(3) + interval(5) * count(3) = 18 seconds
					int keepalive = 1, keepalive_idle = 3, keepalive_interval = 5, keepalive_count = 3;

					if (setsockopt(conn->sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
						httpd_log("ERROR: SO_KEEPALIVE");
					}
					if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
						httpd_log("ERROR: TCP_KEEPIDLE");
					}
					if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
						httpd_log("ERROR: TCP_KEEPINTVL");
					}
					if (setsockopt(conn->sock, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
						httpd_log("ERROR: TCP_KEEPCNT");
					}

					// set conn ssl
					if (httpd_secure) {
						httpd_log_verbose("httpd_tls_new_handshake ...");
						conn->tls = httpd_tls_new_handshake(&conn->sock, httpd_secure);

						if (conn->tls == NULL) {
							httpd_log("ERROR: httpd_tls_new_handshake");
							httpd_conn_remove(conn);
							ret = -1;
							goto exit;
						}
					}

					*client_conn = conn;
					httpd_log_verbose("Accepted the new connection successfully.");
				}
			} else {
				httpd_log("ERROR: accept");
				ret = -1;
				goto exit;
			}
		} else {
			httpd_log("ERROR: select");
			ret = -1;
			goto exit;
		}
	} /* end of select */
	else {
		httpd_log_verbose("select timeout");
		ret = -1;
		goto exit;
	}
exit:
	return ret;
}

/* Interface */
int httpd_start(uint16_t port, uint8_t max_conn, uint32_t stack_bytes, uint8_t thread_mode, uint8_t secure)
{
	httpd_setup(port, max_conn, secure);

	if (stack_bytes > 0) {
		httpd_stack_bytes = stack_bytes;
	}

	httpd_thread_mode = thread_mode;
	httpd_do_stop = 0;

	if (httpd_init()) {
		httpd_log("ERROR: httpd_init");
		return -1;
	}

	if (rtos_task_create(NULL, ((const char *) "httpd_server"), httpd_server_thread, NULL, httpd_stack_bytes, httpd_priority) != RTK_SUCCESS) {
		httpd_log("ERROR: rtos_task_create httpd_server");
		return -1;
	} else {
		httpd_log("Create httpd_server_thread successful, priority is %d", httpd_priority);
	}
	return 0;
}

int httpd_start_with_callback(uint16_t port, uint8_t max_conn, uint32_t stack_bytes, uint8_t thread_mode, uint8_t secure)
{
	httpd_setup(port, max_conn, secure);

	if (stack_bytes > 0) {
		httpd_stack_bytes = stack_bytes;
	}

	httpd_thread_mode = thread_mode;
	httpd_do_stop = 0;
	httpd_method_cb = 1;

	if (httpd_init()) {
		httpd_log("ERROR: httpd_init");
		return -1;
	}

	if (rtos_task_create(NULL, ((const char *) "httpd_server"), httpd_server_thread, NULL, httpd_stack_bytes, httpd_priority) != RTK_SUCCESS) {
		httpd_log("ERROR: rtos_task_create httpd_server");
		return -1;
	} else {
		httpd_log("Create httpd_server_thread successful, priority is %d", httpd_priority);
	}
	return 0;
}

void httpd_stop(void)
{
	httpd_do_stop = 1;
}

int httpd_is_running(void)
{
	return httpd_running;
}

int httpd_reg_page_callback(char *path, void (*callback)(struct httpd_conn *conn))
{
	return httpd_page_add(path, callback);
}

void httpd_clear_page_callbacks(void)
{
	httpd_page_clear();
}

void httpd_setup_debug(uint8_t debug)
{
	httpd_debug = debug;
}

void httpd_setup_priority(uint8_t priority)
{
	httpd_priority = priority;
}

void httpd_setup_idle_timeout(int idle_timeout)
{
	httpd_select_timeout = idle_timeout;
	httpd_idle_timeout_enabled = 1;
}

int httpd_setup_cert(const char *server_cert, const char *server_key, const char *ca_certs)
{
	return httpd_tls_setup_init(server_cert, server_key, ca_certs);
}

int httpd_setup_user_password(char *user, char *password)
{
	int ret = 0;
	size_t auth_len = strlen(user) + 1 + strlen(password);
	size_t base64_len = (auth_len + 2) / 3 * 4 + 1;
	uint8_t *auth = NULL;

	auth = (uint8_t *) httpd_malloc(auth_len);

	if (auth) {
		memcpy(auth, user, strlen(user));
		memcpy(auth + strlen(user), ":", 1);
		memcpy(auth + strlen(user) + 1, password, strlen(password));
		httpd_user_password = (char *) httpd_malloc(base64_len);

		if (httpd_user_password) {
			memset(httpd_user_password, 0, base64_len);
			ret = httpd_base64_encode(auth, auth_len, httpd_user_password, base64_len);

			if (ret) {
				httpd_log("ERROR: httpd_base64_encode");
				ret = -1;
				goto exit;
			}

			httpd_log_verbose("setup httpd_user_password[%s]", httpd_user_password);
		} else {
			httpd_log("ERROR: httpd_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpd_log("ERROR: httpd_malloc");
		ret = -1;
		goto exit;
	}

exit:
	if (auth) {
		httpd_free(auth);
	}

	if (ret) {
		httpd_free(httpd_user_password);
		httpd_user_password = NULL;
	}

	return ret;
}

uint8_t httpd_get_thread_mode(void)
{
	return httpd_thread_mode;
}
