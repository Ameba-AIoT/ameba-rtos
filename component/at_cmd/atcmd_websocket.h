/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WEBSOCKET_H__
#define __ATCMD_WEBSOCKET_H__

#ifdef CONFIG_LWIP_LAYER

#include "wsclient_api.h"

#define TAG_AT_WEBSOCKET   "AT_WEBSOCKET"

#define MAX_WEBSOCKET_LINK_NUM      3
#define MAX_PING_INTERVAL           7200
#define MAX_PING_TIMEOUT            7200
#define MAX_BUFFER_SIZE             (16 * 1024)
#define MAX_QUEUE_SIZE              50
#define MAX_HEADER_NUM              10

#define DEFAULT_PING_INTERVAL       10
#define DEFAULT_PING_TIMEOUT        120
#define DEFAULT_BUFFER_SIZE         1500
#define DEFAULT_MAX_QUEUE_SIZE      3
#define DEFAULT_WEBSOCKET_PROTOCOL  NULL
#define DEFAULT_WEBSOCKET_VERSION   "13"
#define DEFAULT_STABLE_BUF_NUM      DEFAULT_MAX_QUEUE_SIZE

enum CONNECTION_TYPE {
	WEBSOCKET_OVER_TCP = 1,
	WEBSOCKET_OVER_TLS,    // do not verify certificate
	WEBSOCKET_OVER_TLS_VERIFY_SERVER,    // verify server certificate
	WEBSOCKET_OVER_TLS_CLIENT_CERT,    // provide client certificate
	WEBSOCKET_OVER_TLS_VERIFY_SERVER_AND_CLIENT_CERT    // verify server certificate & provide client certificate
};

struct websocket_config {
	int ping_intv_sec; //send interval of ping in seconds
	uint32_t next_ping_time; // next time of sending ping in milliseconds
	int ping_timeout_sec; //timeout of ping in seconds
	int buffer_size;
	int max_queue_size;
	char *protocol;
	char *version;
	int stable_buf_num;
	int version_len;
	char *ws_header[MAX_HEADER_NUM];
	wsclient_context *ws_client;
};

extern uint32_t wsclient_keepalive_idle;
extern uint32_t wsclient_keepalive_interval;
extern uint32_t wsclient_keepalive_count;
extern uint32_t wsclient_recvtimeout;
extern uint32_t wsclient_sendtimeout;
extern uint32_t wsclient_connecttimeout;
extern uint32_t wsclient_sendblocktime;

void print_websocket_at(void);
void at_websocket_init(void);

#endif /* CONFIG_LWIP_LAYER */
#endif /* __ATCMD_WEBSOCKET_H__ */