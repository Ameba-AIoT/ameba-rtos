#include "ameba_soc.h"
#include "lwip_netconf.h"
#include "httpd_util.h"

#define HTTPD_DUMP_BUFFER_LEN 100

extern uint8_t httpd_get_thread_mode(void);
extern struct httpd_page *httpd_page_database;
extern struct httpd_conn *httpd_connections;
extern uint8_t httpd_max_conn;

void *httpd_malloc(size_t size)
{
	void *ptr = rtos_mem_malloc(size);
//	httpd_log_verbose("%p = malloc(%d)", ptr, size);
	return ptr;
}

void httpd_free(void *ptr)
{
	rtos_mem_free(ptr);
//	httpd_log_verbose("free(%p)", ptr);
}

int httpd_page_add(char *path, void (*callback)(struct httpd_conn *conn))
{
	struct httpd_page *page = (struct httpd_page *) httpd_malloc(sizeof(struct httpd_page));

	if (page == NULL) {
		return -1;
	}

	memset(page, 0, sizeof(struct httpd_page));
	page->path = path;
	page->callback = callback;

	if (httpd_page_database == NULL) {
		httpd_page_database = page;
	} else {
		struct httpd_page *cur_page = httpd_page_database;
		struct httpd_page *last_page = NULL;

		while (cur_page) {
			last_page = cur_page;
			cur_page = cur_page->next;
		}

		last_page->next = page;
	}

	return 0;
}

void httpd_page_remove(struct httpd_page *page)
{
	if (page == httpd_page_database) {
		httpd_page_database = page->next;
		httpd_free(page);
	} else {
		struct httpd_page *cur_page = httpd_page_database;
		struct httpd_page *prev_page = NULL;

		while (cur_page) {
			if (cur_page == page) {
				prev_page->next = cur_page->next;
				httpd_free(page);
				break;
			}

			prev_page = cur_page;
			cur_page = cur_page->next;
		}
	}
}

void httpd_page_clear(void)
{
	struct httpd_page *cur_page = httpd_page_database;
	struct httpd_page *tmp_page = NULL;

	while (cur_page) {
		tmp_page = cur_page;
		cur_page = cur_page->next;
		httpd_free(tmp_page);
	}

	httpd_page_database = NULL;
}

struct httpd_conn *httpd_conn_add(int sock)
{
	int i;
	struct httpd_conn *conn = NULL;

	for (i = 0; i < httpd_max_conn; i ++) {
		if (httpd_connections[i].sock == -1) {
			httpd_connections[i].sock = sock;
			conn = &httpd_connections[i];
			break;
		}
	}

	return conn;
}

void httpd_conn_detach(struct httpd_conn *conn)
{
	conn->sock = -1;
}

void httpd_conn_remove(struct httpd_conn *conn)
{
	int i;

	for (i = 0; i < httpd_max_conn; i ++) {
		if (&httpd_connections[i] == conn) {
			if (httpd_connections[i].tls) {
				httpd_tls_close(httpd_connections[i].tls);
				httpd_tls_free(httpd_connections[i].tls);
			}

			if (httpd_connections[i].sock >= 0) {
				close(httpd_connections[i].sock);
			}

			if (httpd_connections[i].request.header) {
				httpd_free(httpd_connections[i].request.header);
			}

			if (httpd_connections[i].response_header) {
				httpd_free(httpd_connections[i].response_header);
			}

			memset(&httpd_connections[i], 0, sizeof(struct httpd_conn));

			if (httpd_get_thread_mode() == HTTPD_THREAD_SINGLE) {
				httpd_connections[i].sock = -1;
			} else {
				httpd_connections[i].sock = -2;    // set to -1 by httpd_conn_detach
			}

			break;
		}
	}
}

void httpd_conn_clear(void)
{
	int i;

	for (i = 0; i < httpd_max_conn; i ++) {
		if (httpd_connections[i].tls) {
			httpd_tls_close(httpd_connections[i].tls);
			httpd_tls_free(httpd_connections[i].tls);
		}

		if (httpd_connections[i].sock >= 0) {
			close(httpd_connections[i].sock);
		}

		if (httpd_connections[i].request.header) {
			httpd_free(httpd_connections[i].request.header);
		}

		if (httpd_connections[i].response_header) {
			httpd_free(httpd_connections[i].response_header);
		}

		memset(&httpd_connections[i], 0, sizeof(struct httpd_conn));
		httpd_connections[i].sock = -1;
	}
}

void httpd_conn_close(struct httpd_conn *conn)
{
	httpd_log_verbose("%s(%d)", __FUNCTION__, conn->sock);
	httpd_conn_remove(conn);
}

void httpd_conn_dump_header(struct httpd_conn *conn)
{
	char *buf_dump = NULL;
	if (conn->request.header) {
		char buf[HTTPD_DUMP_BUFFER_LEN];

		if (conn->request.method) {
			if (conn->request.method_len < HTTPD_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->request.method, conn->request.method_len);
				printf("\nmethod=[%s]\n", buf);
			} else {
				printf("\nmethod_len exceed %d\n", HTTPD_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->request.path) {
			if (conn->request.path_len < HTTPD_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->request.path, conn->request.path_len);
				printf("\npath=[%s]\n", buf);
			} else {
				printf("\npath_len exceed %d\n", HTTPD_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->request.query) {
			buf_dump = (char *) httpd_malloc(conn->request.query_len + 1);

			if (buf_dump) {
				memset(buf_dump, 0, conn->request.query_len);
				memcpy(buf_dump, conn->request.query, conn->request.query_len);
				*(char *)(buf_dump + conn->request.query_len) = '\0';
				printf("\nquery=[%s]\n", buf_dump);
			} else {
				httpd_log("ERROR: httpd_malloc");
				goto exit;
			}
		}

		if (conn->request.version) {
			if (conn->request.version_len < HTTPD_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->request.version, conn->request.version_len);
				printf("\nversion=[%s]\n", buf);
			} else {
				printf("\nversion_len exceed %d\n", HTTPD_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->request.host) {
			if (conn->request.host_len < HTTPD_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->request.host, conn->request.host_len);
				printf("\nhost=[%s]\n", buf);
			} else {
				printf("\nhost_len exceed %d\n", HTTPD_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->request.content_type) {
			if (conn->request.content_type_len < HTTPD_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->request.content_type, conn->request.content_type_len);
				printf("\ncontent_type=[%s]\n", buf);
			} else {
				printf("\ncontent_type_len exceed %d\n", HTTPD_DUMP_BUFFER_LEN - 1);
			}
		}

		printf("\ncontent_lenght=%d\n", conn->request.content_len);
	}
exit:
	if (buf_dump) {
		httpd_free(buf_dump);
	}
}

/* Reserved characters after percent-encoding
 * !	#	$	&	'	(	)	*	+	,	/	:	;	=	?	@	[	]
 * %21	%23	%24	%26	%27	%28	%29	%2A	%2B	%2C	%2F	%3A	%3B	%3D	%3F	%40	%5B	%5D
 */
static char special2char(uint8_t *special)
{
	char ch;
	ch = (special[0] >= 'A') ? ((special[0] & 0xdf) - 'A' + 10) : (special[0] - '0');
	ch *= 16;
	ch += (special[1] >= 'A') ? ((special[1] & 0xdf) - 'A' + 10) : (special[1] - '0');
	return ch;
}

void httpd_query_remove_special(uint8_t *input, size_t input_len, uint8_t *output, size_t output_len)
{
	size_t len = 0;
	uint8_t *ptr = input;

	while ((ptr < (input + input_len)) && (len < output_len)) {
		if (*ptr == '%') {
			output[len] = special2char(ptr + 1);
			ptr += 3;
			len ++;
		} else {
			output[len] = *ptr;
			ptr ++;
			len ++;
		}
	}
}

int httpd_write(struct httpd_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return httpd_tls_write(conn->tls, buf, buf_len);
	} else {
		int ret = write(conn->sock, buf, buf_len);
		if (ret < 0) {
			int err = lwip_getsocklasterr(conn->sock);
			if (err == EAGAIN || err == ENOMEM) {
				httpd_log_verbose("%s(%d): socket error %d, retry once", __FUNCTION__, conn->sock, err);
				ret = write(conn->sock, buf, buf_len);
			}
		}
		return ret;
	}
}

int httpd_read(struct httpd_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return httpd_tls_read(conn->tls, buf, buf_len);
	} else {
		int ret = read(conn->sock, buf, buf_len);
		if (ret < 0) {
			int err = lwip_getsocklasterr(conn->sock);
			if (err == EAGAIN || err == ENOMEM) {
				httpd_log_verbose("%s(%d): socket error %d, retry once", __FUNCTION__, conn->sock, err);
				ret = read(conn->sock, buf, buf_len);
			}
		}
		return ret;
	}
}

int httpd_read_with_timeout(struct httpd_conn *conn, uint8_t *buf, uint16_t buf_len, int recv_timeout)
{
	struct timeval recv_timeout_backup, recv_tv;
	size_t recv_timeout_len;
	int read_bytes = 0;

	if (conn->sock >= 0) {
		recv_timeout_len = sizeof(recv_timeout_backup);

		if (getsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_backup, &recv_timeout_len) != 0) {
			httpd_log_verbose("ERROR: SO_RCVTIMEO");
		}

		recv_tv.tv_sec = recv_timeout / 1000;
		recv_tv.tv_usec = (recv_timeout % 1000) * 1000;

		if (setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_tv, sizeof(recv_tv)) != 0) {
			httpd_log_verbose("ERROR: SO_RCVTIMEO");
		}

		read_bytes = httpd_read(conn, buf, buf_len);

		if (setsockopt(conn->sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_backup, sizeof(recv_timeout_backup)) != 0) {
			httpd_log_verbose("ERROR: SO_RCVTIMEO");
		}
	}

	return read_bytes;
}

void httpd_buf_tolower(uint8_t *buf, size_t buf_len)
{
	size_t i;
	for (i = 0; i < buf_len; i ++) {
		if ((buf[i] >= 'A') && (buf[i] <= 'Z')) {
			buf[i] = buf[i] - 'A' + 'a';
		}
	}
}
