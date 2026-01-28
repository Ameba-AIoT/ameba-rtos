#include "lwip_netconf.h"
#include "httpc_util.h"

#define HTTP_CRLF    "\r\n"
#define HTTP_CHUNKED_LENGTH_DEFAUL 0xFFFFFFFF

extern uint8_t httpc_http_1_0_request_used;

size_t httpc_tmp_buf_size = 2048;

static size_t atoh(char *str)
{
	size_t i;
	unsigned int value = 0;

	for (i = 0; i < strlen(str); i ++) {
		if ((str[i] >= '0') && (str[i] <= '9')) {
			value = value * 16 + (str[i] - '0');
		} else if ((str[i] >= 'a') && (str[i] <= 'f')) {
			value = value * 16 + (str[i] - 'a' + 10);
		} else if ((str[i] >= 'A') && (str[i] <= 'F')) {
			value = value * 16 + (str[i] - 'A' + 10);
		} else {
			value = 0;
			break;
		}
	}

	return value;
}

int httpc_request_write_header_start(struct httpc_conn *conn, char *method, char *resource, char *content_type, size_t content_len)
{
	char num_buf[32];
	int ret = 0;
	size_t header_len = 0;

	// remove previous request header
	if (conn->request_header) {
		httpc_free(conn->request_header);
		conn->request_header = NULL;
	}

	memset(num_buf, 0, sizeof(num_buf));
	snprintf(num_buf, sizeof(num_buf), "%d", conn->port);
	if ((conn->port == 80) || (conn->port == 443)) {
		header_len = strlen("%s %s HTTP/1.1%sHost: %s%s") + strlen(method) - 2 + strlen(resource) - 2 + strlen(HTTP_CRLF) - 2 + strlen(conn->host) - 2 + strlen(
						 HTTP_CRLF) - 2;
	} else {
		header_len = strlen("%s %s HTTP/1.1%sHost: %s:%d%s") + strlen(method) - 2 + strlen(resource) - 2 + strlen(HTTP_CRLF) - 2 + strlen(conn->host) - 2 + strlen(
						 num_buf) - 2 + strlen(HTTP_CRLF) - 2;
	}

	if (content_type) {
		header_len += strlen("Content-Type: %s%s") + strlen(content_type) - 2 + strlen(HTTP_CRLF) - 2;
	}

	memset(num_buf, 0, sizeof(num_buf));
	snprintf(num_buf, sizeof(num_buf), "%d", content_len);
	if (content_len) {
		header_len += strlen("Content-Length: %d%s") + strlen(num_buf) - 2 + strlen(HTTP_CRLF) - 2;
	}

	if (conn->user_password) {
		header_len += strlen("Authorization: Basic %s%s") + strlen(conn->user_password) - 2 + strlen(HTTP_CRLF) - 2;
	}

	conn->request_header = (char *) httpc_malloc(header_len + 1);

	if (conn->request_header) {
		memset(conn->request_header, 0, header_len + 1);
		//sprintf(conn->request_header, "%s %s HTTP/1.1%sHost: %s:%d%s", method, resource, HTTP_CRLF, conn->host, conn->port, HTTP_CRLF);
		snprintf(conn->request_header, header_len + 1, "%s ", method);
		strcat(conn->request_header, resource);

		if ((conn->port == 80) || (conn->port == 443)) {
			snprintf(conn->request_header + strlen(conn->request_header), header_len + 1 - strlen(conn->request_header), " HTTP/1.%d%sHost: %s%s",
					 httpc_http_1_0_request_used ? 0 : 1, HTTP_CRLF, conn->host, HTTP_CRLF);
		} else {
			snprintf(conn->request_header + strlen(conn->request_header), header_len + 1 - strlen(conn->request_header), " HTTP/1.%d%sHost: %s:%d%s",
					 httpc_http_1_0_request_used ? 0 : 1, HTTP_CRLF, conn->host, conn->port, HTTP_CRLF);
		}

		if (content_type) {
			snprintf(conn->request_header + strlen(conn->request_header), header_len + 1 - strlen(conn->request_header), "Content-Type: %s%s", content_type, HTTP_CRLF);
		}

		if (content_len) {
			snprintf(conn->request_header + strlen(conn->request_header), header_len + 1 - strlen(conn->request_header), "Content-Length: %d%s", content_len, HTTP_CRLF);
		}

		if (conn->user_password) {
			snprintf(conn->request_header + strlen(conn->request_header), header_len + 1 - strlen(conn->request_header), "Authorization: Basic %s%s", conn->user_password,
					 HTTP_CRLF);
		}
	} else {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpc_request_write_header_raw(struct httpc_conn *conn, char *raw_req_header)
{
	int ret = 0;
	char *request_header = NULL;
	size_t header_len = 0;

	if (conn->request_header) {
		header_len = strlen("%s%s") + strlen(raw_req_header) - 2 + strlen(HTTP_CRLF) - 2;
		header_len += strlen(conn->request_header);
		request_header = (char *) httpc_malloc(header_len + 1);

		if (request_header) {
			memset(request_header, 0, header_len + 1);
			snprintf(request_header, header_len + 1, "%s", conn->request_header);
			snprintf(request_header + strlen(request_header), header_len + 1 - strlen(request_header), "%s%s", raw_req_header, HTTP_CRLF);
			httpc_free(conn->request_header);
			conn->request_header = request_header;
		} else {
			httpc_log("ERROR: httpc_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpc_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}


int httpc_request_write_header(struct httpc_conn *conn, char *name, char *value)
{
	int ret = 0;
	char *request_header = NULL;
	size_t header_len = 0;

	if (conn->request_header) {
		header_len = strlen("%s: %s%s") + strlen(name) - 2 + strlen(value) - 2 + strlen(HTTP_CRLF) - 2;
		header_len += strlen(conn->request_header);
		request_header = (char *) httpc_malloc(header_len + 1);

		if (request_header) {
			memset(request_header, 0, header_len + 1);
			snprintf(request_header, header_len + 1, "%s", conn->request_header);
			snprintf(request_header + strlen(request_header), header_len + 1 - strlen(request_header), "%s: %s%s", name, value, HTTP_CRLF);
			httpc_free(conn->request_header);
			conn->request_header = request_header;
		} else {
			httpc_log("ERROR: httpc_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpc_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpc_request_write_header_finish(struct httpc_conn *conn)
{
	int ret = 0;
	char *request_header = NULL;
	size_t header_len = 0;

	if (conn->request_header) {
		header_len = strlen(HTTP_CRLF);
		header_len += strlen(conn->request_header);
		request_header = (char *) httpc_malloc(header_len + 1);

		if (request_header) {
			memset(request_header, 0, header_len + 1);
			snprintf(request_header, header_len + 1, "%s", conn->request_header);
			snprintf(request_header + strlen(request_header), header_len + 1 - strlen(request_header), "%s", HTTP_CRLF);
			ret = httpc_write(conn, (uint8_t *)request_header, strlen(request_header));
			httpc_free(request_header);
			httpc_free(conn->request_header);
			conn->request_header = NULL;
		} else {
			httpc_log("ERROR: httpc_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		httpc_log("ERROR: no header start");
		ret = -1;
		goto exit;
	}

exit:
	return ret;
}

int httpc_request_write_data(struct httpc_conn *conn, uint8_t *data, size_t data_len)
{
	return httpc_write(conn, data, data_len);
}

int httpc_response_is_status(struct httpc_conn *conn, char *status)
{
	int ret = 0;

	if ((strlen(status) == conn->response.status_len) &&
		(memcmp(status, conn->response.status, conn->response.status_len) == 0)) {

		ret = 1;
	}

	return ret;
}

int httpc_response_read_header(struct httpc_conn *conn)
{
	int ret = 0;
	uint8_t *tmp_buf = NULL;
	size_t header_len = 0;
	int header_existed = 0;
	int header_end_steps = 0;
	uint8_t data_byte = 0;

	httpc_log_verbose("%s", __FUNCTION__);

	// remove previous response header for keepalive connection
	if (conn->response.header) {
		httpc_free(conn->response.header);
		memset(&conn->response, 0, sizeof(struct http_response));
	}

	tmp_buf = (uint8_t *) httpc_malloc(httpc_tmp_buf_size + 1);

	if (tmp_buf == NULL) {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	} else {
		memset(tmp_buf, 0, httpc_tmp_buf_size + 1);
	}

	header_end_steps = 0;

	while ((httpc_read(conn, &data_byte, 1) == 1) && (header_len < httpc_tmp_buf_size)) {
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
			httpc_log_verbose("header[%d]: %s", header_len, tmp_buf);
			break;
		}
	}

	if (header_existed) {
		conn->response.header = (char *) httpc_malloc(header_len + 1);

		if (conn->response.header) {
			char *ptr;

			memset(conn->response.header, 0, header_len + 1);
			memcpy(conn->response.header, tmp_buf, header_len);
			conn->response.header_len = header_len;

			// get version
			ptr = conn->response.header;

			if ((conn->response.version = (char *) strstr(ptr, "HTTP/1.1 ")) == NULL) {
				conn->response.version = (char *) strstr(ptr, "HTTP/1.0 ");
			}

			if (conn->response.version) {
				conn->response.version_len = strlen("HTTP/1.x");
			} else {
				ret = -1;
				goto exit;
			}

			// get status
			ptr = conn->response.version + conn->response.version_len;

			while (ptr < (conn->response.header + conn->response.header_len)) {
				if (*ptr != ' ') {
					conn->response.status = ptr;
					break;
				}

				ptr ++;
			}

			ptr = conn->response.status + 1;

			while (ptr < (conn->response.header + conn->response.header_len)) {
				if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
					conn->response.status_len = ptr - conn->response.status;
					break;
				}

				ptr ++;
			}

			if ((conn->response.status == NULL) ||
				(conn->response.status && (conn->response.status_len == 0))) {

				ret = -1;
				goto exit;
			}

			// if status is 301/302, store new URL
			char *status_301 = "301";
			char *status_302 = "302";
			if (memcmp(conn->response.status, status_301, strlen(status_301)) == 0 ||
				memcmp(conn->response.status, status_302, strlen(status_302)) == 0) {
				ptr = conn->response.status + conn->response.status_len;
				if (strstr(ptr, "Location:")) {
					ptr = (char *) strstr(ptr, "Location:") + strlen("Location:");

					while (ptr < (conn->response.header + conn->response.header_len)) {
						if (*ptr != ' ') {
							conn->response.location = ptr;
							break;
						}

						ptr ++;
					}

					ptr = conn->response.location + 1;

					while (ptr < (conn->response.header + conn->response.header_len)) {
						if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
							conn->response.location_len = ptr - conn->response.location;
							break;
						}

						ptr ++;
					}

					if (conn->response.location && (conn->response.location_len == 0)) {
						ret = -1;
						goto exit;
					}
				}
			}

			// get content-type
			ptr = conn->response.status + conn->response.status_len;

			if (strstr(ptr, "Content-Type:")) {
				ptr = (char *) strstr(ptr, "Content-Type:") + strlen("Content-Type:");

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if (*ptr != ' ') {
						conn->response.content_type = ptr;
						break;
					}

					ptr ++;
				}

				ptr = conn->response.content_type + 1;

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->response.content_type_len = ptr - conn->response.content_type;
						break;
					}

					ptr ++;
				}

				if (conn->response.content_type && (conn->response.content_type_len == 0)) {
					ret = -1;
					goto exit;
				}
			}

			// get content-length
			ptr = conn->response.status + conn->response.status_len;

			if (strstr(ptr, "Content-Length:") || strstr(ptr, "content-length:")) {
				char *length_ptr = NULL;
				ptr = (char *) strstr(ptr, "Content-Length:");
				if (ptr != NULL) {
					ptr += strlen("Content-Length:");
				} else {
					ptr = conn->response.status + conn->response.status_len;
					ptr = (char *) strstr(ptr, "content-length:") + strlen("content-length:");
				}

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if (*ptr != ' ') {
						length_ptr = ptr;
						break;
					}

					ptr ++;
				}

				ptr = length_ptr + 1;

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						char length[10];
						memset(length, 0, sizeof(length));
						if (ptr - length_ptr > 9) {
							httpc_log("ERROR: Content-Length overflows !");
							ret = -1;
							goto exit;
						}
						memcpy(length, length_ptr, ptr - length_ptr);
						conn->response.content_len = atoi(length);
						break;
					}

					ptr ++;
				}

				if (length_ptr && (conn->response.content_len == 0)) {
					if (conn->ignore_content_len == 0) {
						ret = -1;
						goto exit;
					} else {
						httpc_log("WARNING: Content-Length eqauls 0!");
					}
				}
			}
			// get Transfer-Encoding
			ptr = conn->response.status + conn->response.status_len;

			if (strstr(ptr, "Transfer-Encoding:")) {
				ptr = strstr(ptr, "Transfer-Encoding:") + strlen("Transfer-Encoding:");

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if (*ptr != ' ') {
						conn->response.trans_enc = ptr;
						break;
					}

					ptr ++;
				}

				ptr = (char *)conn->response.trans_enc + 1;

				while (ptr < (conn->response.header + conn->response.header_len)) {
					if ((*ptr == '\r') && (*(ptr + 1) == '\n')) {
						conn->response.trans_enc_len = ptr - conn->response.trans_enc;
						break;
					}

					ptr ++;
				}

				if (conn->response.trans_enc && (conn->response.trans_enc_len == 0)) {
					ret = -1;
					goto exit;
				} else {
					char *chunk = "chunked";
					if (memcmp(conn->response.trans_enc, chunk, conn->response.trans_enc_len) == 0) {
						conn->response.trans_chunk_len = HTTP_CHUNKED_LENGTH_DEFAUL;
					}
				}
			}
		} else {
			httpc_log("ERROR: httpc_malloc");
			ret = -1;
			goto exit;
		}
	} else {
		tmp_buf[header_len] = 0;
		httpc_log("ERROR: parse header[%d]: %s", header_len, tmp_buf);
		ret = -1;
		goto exit;
	}

exit:
	if (ret && conn->response.header) {
		httpc_free(conn->response.header);
		memset(&conn->response, 0, sizeof(struct http_response));
	}

	if (tmp_buf) {
		httpc_free(tmp_buf);
	}

	return ret;
}

int httpc_response_read_data(struct httpc_conn *conn, uint8_t *data, size_t data_len)
{
	int recv_len = 0;
	char chunk[] = "chunked";

	/* it is chunked read */
	if ((conn->response.trans_enc) && (memcmp(conn->response.trans_enc, chunk, strlen(chunk)) == 0)) {
		uint8_t    tmp_buf[10] = {0,};
		uint8_t    data_byte = 0;
		size_t  tmp_len = 0;

		/* chunked read: chunked segment length */
		if (HTTP_CHUNKED_LENGTH_DEFAUL == conn->response.trans_chunk_len) {
			/* chunked data size */
			int chunk_len_step = 0;
			memset(tmp_buf, 0, 10);
			while (tmp_len < 10) {
				recv_len = httpc_read(conn, &data_byte, 1);
				if (recv_len <= 0) {
					goto error;
				}

				if (('\r' == data_byte) && (0 == chunk_len_step)) {
					chunk_len_step = 1;
				} else if (('\n' == data_byte) && (1 == chunk_len_step)) {
					chunk_len_step = 2;
				} else {
					chunk_len_step = 0;
				}

				tmp_buf[tmp_len] = data_byte;
				tmp_len ++;

				if (chunk_len_step == 2) {
					tmp_buf[tmp_len - 2] = '\0';
					conn->response.trans_chunk_len = atoh((char *)tmp_buf);
					httpc_log_verbose("chunk data bytes[%d]", conn->response.trans_chunk_len);
					break;
				}
			}
			if ((10 == tmp_len) && (HTTP_CHUNKED_LENGTH_DEFAUL == conn->response.trans_chunk_len)) {
				httpc_log_verbose("chunck length overflow!\n");
				goto error;
			}
		}

		/* chunked read: chunked segment data */
		if (0 != conn->response.trans_chunk_len) {
			/* chunked data reading */
			recv_len = httpc_read(conn, data, (conn->response.trans_chunk_len < data_len ? conn->response.trans_chunk_len : data_len));

			if (recv_len <= 0) {
				goto error;
			}

			conn->response.trans_chunk_len -= recv_len;

			if (0 == conn->response.trans_chunk_len) {
				int len = httpc_read(conn, tmp_buf, 1);
				if ((len <= 0) || (tmp_buf[0] != '\r')) {
					httpc_log_verbose("chunk end error!");
					goto error;
				}

				len = httpc_read(conn, tmp_buf, 1);
				if ((len <= 0) || tmp_buf[0] != '\n') {
					httpc_log_verbose("chunk end error!");
					goto error;
				}

				conn->response.trans_chunk_len = HTTP_CHUNKED_LENGTH_DEFAUL;
			}
		} else { /* chunked read: chunked end */
			/* chunked data end */
			int chunk_end_step = 0;
			data_byte = 0;
			do {
				recv_len = httpc_read(conn, &data_byte, 1);
				if (recv_len <= 0) {
					goto error;
				}

				if (('\r' == data_byte)  && (0 == chunk_end_step)) {
					chunk_end_step = 1;
				} else if (('\n' == data_byte) && (1 == chunk_end_step)) {
					chunk_end_step = 2;
				} else {
					chunk_end_step = 0;
				}

				if (chunk_end_step == 2) {
					httpc_log_verbose("chunk data end");
					recv_len = 0;
					break;
				}
			} while (1);
		}
	} else {
		recv_len = httpc_read(conn, data, data_len);
	}

	return recv_len;

error:
	httpc_log_verbose("recv error");
	return -1;
}

static int __tolower(int c)
{
	return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}
static uint8_t *strstri(const uint8_t *inBuffer, const uint8_t *inSearchStr)
{
	uint8_t  *currBuffPointer = (uint8_t *)inBuffer;

	while (*currBuffPointer != 0x00) {
		uint8_t *compareOne = (uint8_t *)currBuffPointer;
		uint8_t *compareTwo = (uint8_t *)inSearchStr;

		while (__tolower(*compareOne) == __tolower(*compareTwo)) {
			compareOne++;
			compareTwo++;
			if (*compareTwo == 0x00) {
				return (uint8_t *) currBuffPointer;
			}

		}
		currBuffPointer++;
	}
	return NULL;
}

int httpc_response_get_header_field(struct httpc_conn *conn, char *field, char **value)
{
	int ret = 0;
	char *field_buf = NULL;
	char *ptr = NULL;
	size_t value_len;

	*value = NULL;
	field_buf = (char *) httpc_malloc(strlen(field) + 2);

	if (field_buf) {
		snprintf(field_buf, strlen(field) + 2, "%s:", field);
	} else {
		httpc_log("ERROR: httpc_malloc");
		ret = -1;
		goto exit;
	}

	/*rfc2616:section-4.2: Field names are case-insensitive.*/
	if (strstri((uint8_t *)conn->response.header, (uint8_t *)field_buf)) {
		ptr = (char *) strstri((uint8_t *)conn->response.header, (uint8_t *)field_buf) + strlen(field_buf);

		while (ptr < (conn->response.header + conn->response.header_len)) {
			if (*ptr != ' ') {
				break;
			}

			ptr ++;
		}

		if (strstr(ptr, HTTP_CRLF)) {
			value_len = (char *) strstr(ptr, HTTP_CRLF) - ptr;
			*value = (char *) httpc_malloc(value_len + 1);

			if (*value) {
				memset(*value, 0, value_len + 1);
				memcpy(*value, ptr, value_len);
			} else {
				httpc_log("ERROR: httpc_malloc");
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
	if (field_buf) {
		httpc_free(field_buf);
	}

	return ret;
}
