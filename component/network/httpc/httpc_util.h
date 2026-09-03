#ifndef _HTTPC_UTIL_H_
#define _HTTPC_UTIL_H_

#include "os_wrapper.h"
#include "httpc.h"

extern uint8_t httpc_debug;

#define httpc_log(fmt, ...) \
	do { \
		if(httpc_debug) { \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[HTTPC] " fmt "\n\r", ##__VA_ARGS__); \
		} \
	} while(0)

#define httpc_log_verbose(fmt, ...) \
	do { \
		if(httpc_debug == HTTPC_DEBUG_VERBOSE) { \
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[HTTPC] " fmt "\n\r", ##__VA_ARGS__); \
		} \
	} while(0)

void *httpc_malloc(size_t size);
int httpc_write(struct httpc_conn *conn, uint8_t *buf, size_t buf_len);
int httpc_read(struct httpc_conn *conn, uint8_t *buf, size_t buf_len);

void *httpc_tls_new(int *sock, char *client_cert, char *client_key, char *ca_certs);
void httpc_tls_free(void *tls_in);
int httpc_tls_handshake(void *tls_in, char *host);
void httpc_tls_close(void *tls_in);
int httpc_tls_read(void *tls_in, uint8_t *buf, size_t buf_len);
int httpc_tls_write(void *tls_in, uint8_t *buf, size_t buf_len);
int httpc_base64_encode(uint8_t *data, size_t data_len, char *base64_buf, size_t buf_len);

#endif /* _HTTPD_UTIL_H_ */
