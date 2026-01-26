#include "lwip_netconf.h"
#include "httpc_util.h"

#define HTTPC_DUMP_BUFFER_LEN 100

void *httpc_malloc(size_t size)
{
	void *ptr = (void *)rtos_mem_malloc(size);
//	httpc_log_verbose("%p = malloc(%d)", ptr, size);
	return ptr;
}

void httpc_free(void *ptr)
{
	rtos_mem_free((uint8_t *)ptr);
//	httpc_log_verbose("free(%p)", ptr);
}

void httpc_conn_dump_header(struct httpc_conn *conn)
{
	if (conn->response.header) {
		char buf[HTTPC_DUMP_BUFFER_LEN];

		if (conn->response.version) {
			if (conn->response.version_len < HTTPC_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->response.version, conn->response.version_len);
				printf("\nversion=[%s]\n", buf);
			} else {
				printf("\nversion_len exceed %d\n", HTTPC_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->response.status) {
			if (conn->response.status_len < HTTPC_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->response.status, conn->response.status_len);
				printf("\nstatus=[%s]\n", buf);
			} else {
				printf("\nstatus_len exceed %d\n", HTTPC_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->response.content_type) {
			if (conn->response.content_type_len < HTTPC_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->response.content_type, conn->response.content_type_len);
				printf("\ncontent_type=[%s]\n", buf);
			} else {
				printf("\ncontent_type_len exceed %d\n", HTTPC_DUMP_BUFFER_LEN - 1);
			}
		}

		if (conn->response.content_len) {
			printf("\ncontent_lenght=%d\n", conn->response.content_len);
		}

		if (conn->response.trans_enc) {
			if (conn->response.trans_enc_len < HTTPC_DUMP_BUFFER_LEN) {
				memset(buf, 0, sizeof(buf));
				memcpy(buf, conn->response.trans_enc, conn->response.trans_enc_len);
				printf("\nTransfer-Encoding=%s\n", buf);
			}
		}
	}
}

int httpc_write(struct httpc_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return httpc_tls_write(conn->tls, buf, buf_len);
	}

	return write(conn->sock, buf, buf_len);
}

int httpc_read(struct httpc_conn *conn, uint8_t *buf, size_t buf_len)
{
	if (conn->tls) {
		return httpc_tls_read(conn->tls, buf, buf_len);
	}

	return read(conn->sock, buf, buf_len);
}
