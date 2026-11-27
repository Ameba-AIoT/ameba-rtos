/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_HTTP_H__
#define __ATCMD_HTTP_H__


#ifdef CONFIG_LWIP_LAYER

#define HTTP_OVER_TCP                   (1)
#define HTTP_OVER_TLS_NO_VERIFY         (2)
#define HTTP_OVER_TLS_VERIFY_SERVER     (3)
#define HTTP_OVER_TLS_VERIFY_CLIENT     (4)
#define HTTP_OVER_TLS_VERIFY_BOTH       (5)


#define HTTP_READ_RESPONSE_DATA         (ATCMD_DATA_REPORT_CHUNK + 1)


#define HTTP_GLOBAL_REQ_HEADER_NUM      (10)

/* Stepwise request links (AT+HTTPOPEN/HTTPREQ/HTTPBODY/HTTPCLOSE/HTTPSTATE) */
#define AT_HTTP_MAX_LINK                (8)
#define AT_HTTP_REPORT_CHUNK            (2048)
/* Worst case is a server-initiated TLS renegotiation inside mbedtls_ssl_read (~5 KB). */
#define AT_HTTP_WORKER_STACK_SIZE       (6 * 1024)
#define AT_HTTP_WORKER_PRIORITY         (4)
#define AT_HTTP_METHOD_MAX_LEN          (15)

/* TCP keepalive of stepwise links, fixed at build time (seconds / probe count).
 * Link is judged dead after about IDLE + COUNT * INTERVAL seconds without any answer; with a live peer
 * a probe is sent every IDLE seconds. Ranges accepted by lwIP: idle 1~7200, interval 1~75, count 1~10. */
#define AT_HTTP_KEEPALIVE_IDLE          (10)
#define AT_HTTP_KEEPALIVE_INTERVAL      (5)
#define AT_HTTP_KEEPALIVE_COUNT         (3)

/* Upper bound of the per-link <timeout> of AT+HTTPOPEN (seconds). 0 is rejected: it would leave SO_RCVTIMEO unset. */
#define AT_HTTP_MAX_TIMEOUT             (3600)

/* ERROR:<n> codes of the stepwise commands. 1~6 mean the same as in AT+HTTPGET/POST/PUT/DEL, 7~9 are new. */
#define AT_HTTP_ERR_PARAM               (1)
#define AT_HTTP_ERR_NOMEM               (2)
#define AT_HTTP_ERR_CERT                (3)
#define AT_HTTP_ERR_CONN                (4)
#define AT_HTTP_ERR_TT                  (5)
#define AT_HTTP_ERR_WRITE               (6)
#define AT_HTTP_ERR_STATE               (7)
#define AT_HTTP_ERR_PEER_CLOSED         (8)
#define AT_HTTP_ERR_ABORT               (9)

void print_http_at(void);
void at_http_init(void);

#endif /* CONFIG_LWIP_LAYER */


#endif /*#ifndef __ATCMD_HTTP_H__*/
