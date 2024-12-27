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

#define DEFAULT_PING_INTERVAL       10
#define DEFAULT_PING_TIMEOUT        120
#define DEFAULT_BUFFER_SIZE         1500
#define DEFAULT_MAX_QUEUE_SIZE      3
#define DEFAULT_WEBSOCKET_PROTOCOL  NULL
#define DEFAULT_WEBSOCKET_VERSION   "13"
#define DEFAULT_STABLE_BUF_NUM      DEFAULT_MAX_QUEUE_SIZE

struct websocket_config {
	int ping_intv_sec; //send interval of ping in seconds
	int ping_timeout_sec; //timeout of ping in seconds
	int buffer_size;
	int max_queue_size;
	char *protocol;
	char *version;
	int stable_buf_num;
	int version_len;
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