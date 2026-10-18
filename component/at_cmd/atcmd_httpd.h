#ifndef __ATCMD_HTTPD_H__
#define __ATCMD_HTTPD_H__

#include "platform_stdlib.h"

/* Server limits */
#define HTTPD_AT_MAX_CONN            4      /* upper bound for max_conn / pending table size */
#define HTTPD_AT_DEFAULT_CONN        2      /* default concurrent connections */
#define HTTPD_AT_DEFAULT_STACK       4096   /* per-connection thread stack (bytes) */
#define HTTPD_AT_DEFAULT_TIMEOUT_MS  10000  /* wait for host HTTPDRSP before 504 */
#define HTTPD_AT_MAX_TIMEOUT_MS      120000

/* On STOP, wait up to this long for the httpd server thread to actually exit and
 * release the listen socket. The thread only notices the stop flag on its next
 * select() wakeup (idle_timeout, up to ~10s), so a shorter STOP->START would hit
 * "listen" EADDRINUSE. Poll httpd_is_running() until it clears or we time out. */
#define HTTPD_AT_STOP_WAIT_MS        12000
#define HTTPD_AT_STOP_POLL_MS        50

/* Server-thread select() period (seconds). Shortened from the httpd default (10s)
 * so STOP returns quickly (the thread notices the stop flag within this window).
 * Safe for our use: HTTP is not long-lived, and waiting for the host reply happens
 * inside the request handler (bounded by AT+HTTPDCFG timeout), not in this select.
 * Only a client that connects but sends nothing is dropped after this period. */
#define HTTPD_AT_IDLE_TIMEOUT_S      2

/* Upstream report chunk size (device -> host); see ATCMD_DATA_REPORT_CHUNK */
#define HTTPD_AT_REPORT_CHUNK        ATCMD_DATA_REPORT_CHUNK

/* Max stored Content-Type length (incl. terminator) */
#define HTTPD_AT_CTYPE_LEN           64

/* AT+HTTPDSTART <conn_type> tiers, aligned with the websocket server enum:
 * plain HTTP, HTTPS (server cert only), HTTPS + verify client cert. */
enum {
	HTTPD_AT_OVER_TCP = 1,              /* plain HTTP */
	HTTPD_AT_OVER_TLS,                  /* HTTPS, do not verify client */
	HTTPD_AT_OVER_TLS_VERIFY_CLIENT,    /* HTTPS, verify client certificate */
};

void at_httpd_init(void);

#endif /* __ATCMD_HTTPD_H__ */
