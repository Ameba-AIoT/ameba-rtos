#include "ameba_soc.h"
#include "lwip_netconf.h"
#include "httpd_util.h"

#define HTTP_CRLF    "\r\n"

#define HTTPD_LOCAL_SMALL_BUF_LEN 50
#define HTTPD_LOCAL_LARGE_BUF_LEN 200


extern uint8_t httpd_max_conn;

size_t httpd_tmp_buf_size = 1024;

int httpd_request_is_method(struct httpd_conn *conn, char *method)
{
	int ret = 0;

	if ((strlen(method) == conn->request.method_len) &&
		(memcmp(method, conn->request.method, conn->request.method_len) == 0)) {

		ret = 1;
	}

	return ret;
}

int httpd_request_read_header(struct httpd_conn *conn)
{
	int ret = 0;
	char *tmp_buf = NULL;
	size_t header_len = 0;
	int header_existed = 0;
	int header_end_steps = 0;
	uint8_t data_byte = 0;
	int httpd_read_res = -1;

	httpd_log_verbose("%s", __FUNCTION__);

	// remove previous request header for keepalive connection
	if (conn->request.header) {
		httpd_free(conn->request.header);
		memset(&conn->request, 0, sizeof(struct http_request));
	}

	tmp_buf = (char *) httpd_malloc(httpd_tmp_buf_size + 1);

	if (tmp_buf == NULL) {
		httpd_log("ERROR: httpd_malloc");
		ret = -1;
		goto exit;
	} else {
		memset(tmp_buf, 0, httpd_tmp_buf_size + 1);
	}

	header_end_steps = 0;

	while (((httpd_read_res = httpd_read(conn, &data_byte, 1)) == 1) && (header_len < httpd_tmp_buf_size)) {
		if ((data_byte == '\r') && (header_end_steps == 0)) {
			header_end_steps = 1;
		} else if ((data_byte == '\n') && (header_end_steps == 1)) {
			header_end_steps = 2;
		} else if ((data_byte == '\r') && (header_end_steps == 2)) {
			header_end_steps = 3;
		} else if ((data_byte == '\n') && (header_end_steps == 3)) {
			header_end_steps = 4;
		} else {
			header_end_steps = 0;
		}

		tmp_buf[header_len] = data_byte;
		header_len ++;

		if (header_end_steps == 4) {
			header_existed = 1;
			tmp_buf[header_len] = 0;
			httpd_log_verbose("header[%d]: %s", header_len, tmp_buf);
			break;
		}
	}

	if (header_existed) {
		conn->request.header = (char *) httpd_malloc(header_len + 1);

		if (conn->request.header) {
			char *ptr;

			memset(conn->request.header, 0, header_len + 1);
			memcpy(conn->request.header, tmp_buf, header_len);
			conn->request.header_len = header_len;
			httpd_buf_tolower((uint8_t *)tmp_buf, header_len);

			// get method
			ptr = conn->request.header;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr != ' ') {
					conn->request.method = ptr;
					break;
				}

				ptr ++;
			}

			ptr = conn->request.method + 1;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr == ' ') {
					conn->request.method_len = ptr - conn->request.method;
					break;
				}

				ptr ++;
			}

			if ((conn->request.method == NULL) ||
				(conn->request.method && (conn->request.method_len == 0))) {

				ret = -1;
				goto exit;
			}

			// get path
			ptr = conn->request.method + conn->request.method_len;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if (*ptr != ' ') {
					conn->request.path = ptr;
					break;
				}

				ptr ++;
			}

			ptr = conn->request.path + 1;

			while (ptr < (conn->request.header + conn->request.header_len)) {
				if ((*ptr == ' ') || (*ptr == '?')) {
					conn->request.path_len = ptr - conn->request.path;
					break;
				}

				ptr ++;
			}

			if ((conn->request.path == NULL) ||
				(conn->request.path && (conn->request.path_len == 0))) {

				ret = -1;
				goto exit;
			}

			// get query string
			ptr = conn->request.path + conn->request.path_len;

			if (*ptr == '?') {
				conn->request.query = ptr;
				ptr = conn->request.query + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr == ' ') {
						conn->request.query_len = ptr - conn->request.query;
						break;
					}

					ptr ++;
				}
			}

			if (conn->request.query && (conn->request.query_len == 0)) {
				ret = -1;
				goto exit;
			}

			// get http version
			if (conn->request.query) {
				ptr = conn->request.query + conn->request.query_len;
			} else {
				ptr = conn->request.path + conn->request.path_len;
			}

			if ((conn->request.version = (char *) strstr(ptr, "HTTP/1.1\r\n")) == NULL) {
				conn->request.version = (char *) strstr(ptr, "HTTP/1.0\r\n");
			}

			if (conn->request.version) {
				conn->request.version_len = strlen("HTTP/1.x");
			} else {
				ret = -1;
				goto exit;
			}

			// get host
			ptr = conn->request.version + conn->request.version_len;

			if (strstr((ptr - conn->request.header + tmp_buf), "host:")) {
				ptr = (char *) strstr((ptr - conn->request.header + tmp_buf), "host:") - tmp_buf + conn->request.header + strlen("Host:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						conn->request.host = ptr;
						break;
					}

					ptr ++;
				}

				ptr = conn->request.host + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->request.host_len = ptr - conn->request.host;
						break;
					}

					ptr ++;
				}

				if (conn->request.host && (conn->request.host_len == 0)) {
					ret = -1;
					goto exit;
				}
			}

			// get content-type
			ptr = conn->request.version + conn->request.version_len;

			if (strstr((ptr - conn->request.header + tmp_buf), "content-type:")) {
				ptr = (char *) strstr((ptr - conn->request.header + tmp_buf), "content-type:") - tmp_buf + conn->request.header + strlen("Content-Type:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						conn->request.content_type = ptr;
						break;
					}

					ptr ++;
				}

				ptr = conn->request.content_type + 1;

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->request.content_type_len = ptr - conn->request.content_type;
						break;
					}

					ptr ++;
				}

				if (conn->request.content_type && (conn->request.content_type_len == 0)) {
					ret = -1;
					goto exit;
				}
			}

			// get content-length
			ptr = conn->request.version + conn->request.version_len;

			if (strstr((char *)(ptr - conn->request.header + tmp_buf), "content-length:")) {
				char *length_ptr = NULL;
				ptr = (char *) strstr((char *)(ptr - conn->request.header + tmp_buf), "content-length:") - tmp_buf + conn->request.header + strlen("Content-Length:");

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if (*ptr != ' ') {
						length_ptr = ptr;
						break;
					}

					ptr ++;
				}

				while (ptr < (conn->request.header + conn->request.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						if ((ptr - length_ptr) > 0) {
							char length[10];
							memset(length, 0, sizeof(length));
							memcpy(length, length_ptr, ptr - length_ptr);
							conn->request.content_len = atoi(length);
						}
						break;
					}

					ptr ++;
				}

				if (length_ptr && (length_ptr == ptr)) {

					ret = -1;
					goto exit;
				}
			}
		} else {
			httpd_log("ERROR: httpd_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		tmp_buf[header_len] = 0;
		if (httpd_read_res != 0) { //Realtek:exclude the ERR_CLSD case of lwip_recv_from()
			httpd_log_verbose("httpd_read: %d", httpd_read_res);
			httpd_log("ERROR: parse header[%d]: %s", header_len, tmp_buf);
		}
		ret = -1;
		goto exit;
	}

exit:
	if (ret && conn->request.header) {
		httpd_free(conn->request.header);
		memset(&conn->request, 0, sizeof(struct http_request));
	}

	if (tmp_buf) {
		httpd_free(tmp_buf);
	}

	return ret;
}

int httpd_request_read_data(struct httpd_conn *conn, uint8_t *data, size_t data_len)
{
	return httpd_read(conn, data, data_len);
}

int httpd_request_get_header_field(struct httpd_conn *conn, char *field, char **value)
{
	int ret = 0;
	char field_buf[HTTPD_LOCAL_SMALL_BUF_LEN];
	char *ptr = NULL;
	size_t value_len;

	if ((strlen(field) + strlen((char const *)":")) >= HTTPD_LOCAL_SMALL_BUF_LEN) {
		httpd_log("ERROR: field length exceed %d", HTTPD_LOCAL_SMALL_BUF_LEN - strlen((char const *)':') - 1);
		ret = -1;
		goto exit;
	}

	*value = NULL;
	memset(field_buf, 0, sizeof(field_buf));
	DiagSnPrintf(field_buf, HTTPD_LOCAL_SMALL_BUF_LEN, "%s:", field);

	if (strstr(conn->request.header, field_buf)) {
		ptr = (char *) strstr(conn->request.header, field_buf) + strlen(field_buf);

		while (ptr < (conn->request.header + conn->request.header_len)) {
			if (*ptr != ' ') {
				break;
			}

			ptr ++;
		}

		if (strstr(ptr, HTTP_CRLF)) {
			value_len = (char *) strstr(ptr, HTTP_CRLF) - ptr;
			*value = (char *) httpd_malloc(value_len + 1);

			if (*value) {
				memset(*value, 0, value_len + 1);
				memcpy(*value, ptr, value_len);
			} else {
				httpd_log("ERROR: httpd_malloc");
				ret = -1;
				goto exit;
			}
		} else {
			ret = -1;
			goto exit;
		}
	} else {
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpd_request_get_query_key(struct httpd_conn *conn, char *key, char **value)
{
	int ret = 0;
	size_t value_len;

	*value = NULL;

	if (conn->request.query) {
		char *ptr = conn->request.query + 1;
		char *ptr_tmp = NULL;
		int special_num = 0;

		while (ptr < (conn->request.query + conn->request.query_len)) {
			if ((memcmp(ptr, key, strlen(key)) == 0) && (*(ptr + strlen(key)) == '=')) {
				ptr = ptr + strlen(key) + 1;
				ptr_tmp = ptr;

				while (ptr < (conn->request.query + conn->request.query_len)) {
					if (*ptr == '&') {
						break;
					}

					if (*ptr == '%') {
						special_num ++;
					}

					ptr ++;
				}

				if (ptr - ptr_tmp) {
					value_len = ptr - ptr_tmp;
					*value = (char *) httpd_malloc(value_len - 2 * special_num + 1);

					if (*value) {
						memset(*value, 0, value_len - 2 * special_num + 1);
						httpd_query_remove_special((uint8_t *)ptr_tmp, value_len, (uint8_t *)*value, value_len - 2 * special_num + 1);
					} else {
						httpd_log("ERROR: httpd_malloc");
						goto exit;
					}
				}

				break;
			} else {
				// skip one key
				while (ptr < (conn->request.query + conn->request.query_len)) {
					if (*ptr == '=') {
						ptr ++;
						break;
					}

					ptr ++;
				}

				while (ptr < (conn->request.query + conn->request.query_len)) {
					if (*ptr == '&') {
						ptr ++;
						break;
					}

					ptr ++;
				}
			}
		}
	}

exit:
	if (*value == NULL) {
		ret = -1;
	}

	return ret;
}

int httpd_response_write_header_start(struct httpd_conn *conn, char *status, char *content_type, size_t content_len)
{
	char buf[HTTPD_LOCAL_LARGE_BUF_LEN];
	int ret = 0;
	size_t header_len = 0;

	// remove previous response header
	if (conn->response_header) {
		httpd_free(conn->response_header);
		conn->response_header = NULL;
	}

	if ((strlen("HTTP/1.1 ") + strlen(status) + strlen(HTTP_CRLF)) < HTTPD_LOCAL_LARGE_BUF_LEN) {
		memset(buf, 0, HTTPD_LOCAL_LARGE_BUF_LEN);
		header_len = DiagSnPrintf(buf, HTTPD_LOCAL_LARGE_BUF_LEN, "HTTP/1.1 %s%s", status, HTTP_CRLF);
	} else {
		httpd_log("ERROR: status length exceed %d", HTTPD_LOCAL_LARGE_BUF_LEN - strlen("HTTP/1.1 ") - strlen(HTTP_CRLF) - 1);
		ret = -1;
		goto exit;
	}

	if (content_type) {
		if ((strlen("Content-Type: ") + strlen(content_type) + strlen(HTTP_CRLF)) < HTTPD_LOCAL_LARGE_BUF_LEN) {
			memset(buf, 0, HTTPD_LOCAL_LARGE_BUF_LEN);
			header_len += DiagSnPrintf(buf, HTTPD_LOCAL_LARGE_BUF_LEN, "Content-Type: %s%s", content_type, HTTP_CRLF);
		} else {
			httpd_log("ERROR: content_type length exceed %d", HTTPD_LOCAL_LARGE_BUF_LEN - strlen("Content-Type: ") - strlen(HTTP_CRLF) - 1);
			ret = -1;
			goto exit;
		}
	}

	if (content_len) {
		memset(buf, 0, HTTPD_LOCAL_LARGE_BUF_LEN);
		header_len += DiagSnPrintf(buf, HTTPD_LOCAL_LARGE_BUF_LEN, "Content-Length: %d%s", content_len, HTTP_CRLF);
	}

	conn->response_header = (char *) httpd_malloc(header_len + 1);

	if (conn->response_header) {
		memset(conn->response_header, 0, header_len + 1);
		DiagSnPrintf(conn->response_header, header_len + 1, "HTTP/1.1 %s%s", status, HTTP_CRLF);

		if (content_type) {
			DiagSnPrintf(conn->response_header + strlen(conn->response_header), header_len + 1 - strlen(conn->response_header), "Content-Type: %s%s", content_type,
						 HTTP_CRLF);
		}

		if (content_len) {
			DiagSnPrintf(conn->response_header + strlen(conn->response_header), header_len + 1 - strlen(conn->response_header), "Content-Length: %d%s", content_len,
						 HTTP_CRLF);
		}
	} else {
		httpd_log("ERROR: httpd_malloc");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpd_response_write_header(struct httpd_conn *conn, char *name, char *value)
{
	char buf[HTTPD_LOCAL_LARGE_BUF_LEN];
	int ret = 0;
	char *response_header = NULL;
	size_t header_len = 0;

	if (conn->response_header) {
		if ((strlen(name) + strlen(": ") + strlen(value) + strlen(HTTP_CRLF)) < HTTPD_LOCAL_LARGE_BUF_LEN) {
			memset(buf, 0, HTTPD_LOCAL_LARGE_BUF_LEN);
			header_len = DiagSnPrintf(buf, HTTPD_LOCAL_LARGE_BUF_LEN, "%s: %s%s", name, value, HTTP_CRLF);
			header_len += strlen(conn->response_header);
			response_header = (char *) httpd_malloc(header_len + 1);
		} else {
			httpd_log("ERROR: response length exceed %d", HTTPD_LOCAL_LARGE_BUF_LEN - 1);
			ret = -1;
			goto exit;
		}

		if (response_header) {
			memset(response_header, 0, header_len + 1);
			DiagSnPrintf(response_header, header_len + 1, "%s%s", conn->response_header, buf);
			httpd_free(conn->response_header);
			conn->response_header = response_header;
		} else {
			httpd_log("ERROR: httpd_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpd_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpd_response_write_header_finish(struct httpd_conn *conn)
{
	int ret = 0;
	char *response_header = NULL;
	size_t header_len = 0;

	if (conn->response_header) {
		header_len = strlen(HTTP_CRLF);
		header_len += strlen(conn->response_header);
		response_header = (char *) httpd_malloc(header_len + 1);

		if (response_header) {
			memset(response_header, 0, header_len + 1);
			DiagSnPrintf(response_header, header_len + 1, "%s%s", conn->response_header, HTTP_CRLF);
			ret = httpd_write(conn, (uint8_t *)response_header, strlen(response_header));
			httpd_free(response_header);
			httpd_free(conn->response_header);
			conn->response_header = NULL;
		} else {
			httpd_log("ERROR: httpd_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpd_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpd_response_write_data(struct httpd_conn *conn, uint8_t *data, size_t data_len)
{
	return httpd_write(conn, data, data_len);
}

void httpd_response_bad_request(struct httpd_conn *conn, char *msg)
{
	if (msg == NULL) {
		msg = "Bad Request - Invalid Header";
	}

	httpd_response_write_header_start(conn, "400 Bad Request", "text/plain", strlen(msg));
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

void httpd_response_unauthorized(struct httpd_conn *conn, char *msg)
{
	if (msg == NULL) {
		msg = "Authorization Required";
	}

	httpd_response_write_header_start(conn, "401 Unauthorized", "text/plain", strlen(msg));
	httpd_response_write_header(conn, "WWW-Authenticate", "Basic realm=\"generic\"");
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

__attribute__((weak)) void httpd_response_not_found(struct httpd_conn *conn, char *msg)
{
	char msg_buf[HTTPD_LOCAL_LARGE_BUF_LEN];

	if (msg == NULL) {
		memset(msg_buf, 0, sizeof(msg_buf));
		DiagSnPrintf(msg_buf, HTTPD_LOCAL_LARGE_BUF_LEN, "Page Not Found\r\n");
		if (conn->request.path_len >= (sizeof(msg_buf) - strlen(msg_buf))) {
			httpd_log("ERROR: conn->request.path_len==%d >= sizeof(msg_buf)==%d\n", conn->request.path_len, sizeof(msg_buf) - strlen(msg_buf));
		} else {
			memcpy(msg_buf + strlen(msg_buf), conn->request.path, conn->request.path_len);
		}
		msg = msg_buf;
	}

	httpd_response_write_header_start(conn, "404 Not Found", "text/plain", strlen(msg));
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

void httpd_response_method_not_allowed(struct httpd_conn *conn, char *msg)
{
	char msg_buf[HTTPD_LOCAL_SMALL_BUF_LEN];

	if (msg == NULL) {
		memset(msg_buf, 0, sizeof(msg_buf));
		if (conn->request.method_len >= (sizeof(msg_buf) - strlen(" Method Not Allowed"))) {
			httpd_log("ERROR: conn->request.method_len==%d >= sizeof(msg_buf)==%d\n", conn->request.method_len, sizeof(msg_buf) - strlen(" Method Not Allowed"));
			DiagSnPrintf(msg_buf, HTTPD_LOCAL_SMALL_BUF_LEN, " Method Not Allowed");
		} else {
			memcpy(msg_buf, conn->request.method, conn->request.method_len);
			DiagSnPrintf(msg_buf + conn->request.method_len, HTTPD_LOCAL_SMALL_BUF_LEN - conn->request.method_len, " Method Not Allowed");
		}
		msg = msg_buf;
	}

	httpd_response_write_header_start(conn, "405 Method Not Allowed", "text/plain", strlen(msg));
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

void httpd_response_too_many_requests(struct httpd_conn *conn, char *msg)
{
	char msg_buf[HTTPD_LOCAL_SMALL_BUF_LEN];

	if (msg == NULL) {
		memset(msg_buf, 0, sizeof(msg_buf));
		DiagSnPrintf(msg_buf, HTTPD_LOCAL_SMALL_BUF_LEN, "Too Many Requests\r\nMAX connections: %d", httpd_max_conn);
		msg = msg_buf;
	}

	httpd_response_write_header_start(conn, "429 Too Many Requests", "text/plain", strlen(msg));
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}

void httpd_response_internal_server_error(struct httpd_conn *conn, char *msg)
{
	if (msg == NULL) {
		msg = "Internal Server Error";
	}

	httpd_response_write_header_start(conn, "500 Internal Server Error", "text/plain", strlen(msg));
	httpd_response_write_header_finish(conn);
	httpd_response_write_data(conn, (uint8_t *)msg, strlen(msg));
}
