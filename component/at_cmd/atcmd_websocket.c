/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_websocket.h"
#include "lwip/sockets.h"

#if defined(CONFIG_ATCMD_WEBSOCKET) && (CONFIG_ATCMD_WEBSOCKET == 1)

struct websocket_config ws_config[MAX_WEBSOCKET_LINK_NUM];

struct list_head ping_time_list[MAX_WEBSOCKET_LINK_NUM];

struct pingtime_item {
	uint32_t ping_send_time;    //ms
	struct list_head node;
};

void at_wscfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "AT+WSCFG=<link_id>,<ping_intv_sec>,<ping_timeout_sec>[,<buffer_size>][,<max_queue_size>][,<protocol>][,<version>][,<stable_buf_num>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<ping_intv_sec>:\tsend interval of ping in seconds, must be 1~%d, default is %d\r\n", MAX_PING_INTERVAL,
			 DEFAULT_PING_INTERVAL);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<ping_timeout_sec>:\ttimeout of ping in seconds, must be 1~%d, default is %d\r\n", MAX_PING_TIMEOUT, DEFAULT_PING_TIMEOUT);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<buffer_size>:\tbuffer size of tx and rx, must be 1~%d, default is %d\r\n", MAX_BUFFER_SIZE, DEFAULT_BUFFER_SIZE);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<max_queue_size>:\tmax size of queue, must be 1~%d, default is %d\r\n", MAX_QUEUE_SIZE, DEFAULT_MAX_QUEUE_SIZE);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<protocol>:\tsubprotocols using the websocket protocol, default is %s\r\n",
			 (DEFAULT_WEBSOCKET_PROTOCOL == NULL) ? "NULL" : DEFAULT_WEBSOCKET_PROTOCOL);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<version>:\tversion of the websocket protocol, default is %s\r\n",
			 (DEFAULT_WEBSOCKET_VERSION == NULL) ? "NULL" : DEFAULT_WEBSOCKET_VERSION);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<stable_buf_num>:\tthe stable buffer number in send queue, must be 1~%d, default is %d\r\n", DEFAULT_MAX_QUEUE_SIZE,
			 DEFAULT_STABLE_BUF_NUM);
}

void at_wscfg(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id, ping_intv_sec, ping_timeout_sec, buffer_size, max_queue_size, stable_buf_num;
	char *protocol, *version;

	UNUSED(argc);

	if (arg == NULL) {
		at_wscfg_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);

	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	}

	ping_intv_sec = atoi(argv[2]);
	if (ping_intv_sec < 1 || ping_intv_sec > MAX_PING_INTERVAL) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "ping_intv_sec must be 1~%d\r\n", MAX_PING_INTERVAL);
		error_no = 1;
		goto end;
	}
	ws_config[link_id].ping_intv_sec = ping_intv_sec;

	ping_timeout_sec = atoi(argv[3]);
	if (ping_timeout_sec < 1 || ping_timeout_sec > MAX_PING_TIMEOUT) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "ping_timeout_sec must be 1~%d\r\n", MAX_PING_TIMEOUT);
		error_no = 1;
		goto end;
	}
	ws_config[link_id].ping_timeout_sec = ping_timeout_sec;

	if (argv[4] != NULL && (strlen(argv[4]) > 0)) {
		buffer_size = atoi(argv[4]);
		if (buffer_size < 1 || buffer_size > MAX_BUFFER_SIZE) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "buffer_size must be 1~%d\r\n", MAX_BUFFER_SIZE);
			error_no = 1;
			goto end;
		}

		ws_config[link_id].buffer_size = buffer_size;
	}

	if (argv[5] != NULL && (strlen(argv[5]) > 0)) {
		max_queue_size = atoi(argv[5]);
		if (max_queue_size < 1 || max_queue_size > MAX_QUEUE_SIZE) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "max_queue_size must be 1~%d\r\n", MAX_QUEUE_SIZE);
			error_no = 1;
			goto end;
		}

		ws_config[link_id].max_queue_size = max_queue_size;
	}

	if (argv[6] != NULL  && (strlen(argv[6]) > 0)) {
		protocol = (char *)argv[6];
		if (ws_config[link_id].protocol != NULL) {
			ws_free(ws_config[link_id].protocol);
		}
		ws_config[link_id].protocol = (char *) ws_malloc(strlen(protocol) + 1);
		if (ws_config[link_id].protocol == NULL) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "malloc failed\r\n");
			error_no = 2;
			goto end;
		}
		memcpy(ws_config[link_id].protocol, protocol, strlen(protocol));
	}

	if (argv[7] != NULL && (strlen(argv[7]) > 0)) {
		version = (char *)(argv[7]);
		if (ws_config[link_id].version != NULL) {
			ws_free(ws_config[link_id].version);
		}

		ws_config[link_id].version = (char *) ws_malloc(strlen(version) + 1);
		if (ws_config[link_id].version == NULL) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "malloc failed\r\n");
			error_no = 2;
			goto end;
		}
		memcpy(ws_config[link_id].version, version, strlen(version));
	}

	if (argv[8] != NULL && (strlen(argv[8]) > 0)) {
		stable_buf_num = atoi(argv[8]);
		if (stable_buf_num < 0 || stable_buf_num > ws_config[link_id].max_queue_size) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "stable_buf_num must be 1~%d\r\n", ws_config[link_id].max_queue_size);
			error_no = 1;
			goto end;
		}
		ws_config[link_id].stable_buf_num = stable_buf_num;
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wsglcfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "AT+WSGLCFG=[<connect_timeout>][,<recv_timeout>][,<send_timeout>][,<send_blocktime>][,<keepalive_idle>][,<keepalive_interval>][,<keepalive_count>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<connect_timeout>:\ttimeout for establishing websocket connection\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<recv_timeout>:\ttimeout for receiving data\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<send_timeout>:\ttimeout for sending data\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<send_blocktime>:\tblock time for sending data in milliseconds\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<keepalive_idle>:\tvalue for TCP_KEEPIDLE option\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<keepalive_interval>:\tvalue for TCP_KEEPINTVL option\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<keepalive_count>:\tvalue for TCP_KEEPCNT option\r\n");
}

void at_wsglcfg(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};

	UNUSED(argc);

	if (arg == NULL) {
		at_wsglcfg_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	if (argv[1] != NULL && (strlen(argv[1]) > 0)) {
		wsclient_connecttimeout = atoi(argv[1]);
	}

	if (argv[2] != NULL && (strlen(argv[2]) > 0)) {
		wsclient_recvtimeout = atoi(argv[2]);
	}

	if (argv[3] != NULL && (strlen(argv[3]) > 0)) {
		wsclient_sendtimeout = atoi(argv[3]);
	}

	if (argv[4] != NULL && (strlen(argv[4]) > 0)) {
		wsclient_sendblocktime = atoi(argv[4]);
	}

	if (argv[5] != NULL && (strlen(argv[5]) > 0)) {
		wsclient_keepalive_idle = atoi(argv[5]);
	}

	if (argv[6] != NULL && (strlen(argv[6]) > 0)) {
		wsclient_keepalive_interval = atoi(argv[6]);
	}

	if (argv[7] != NULL && (strlen(argv[7]) > 0)) {
		wsclient_keepalive_count = atoi(argv[7]);
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wshead_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSHEAD=<link_id>,<req_header_len>[,<req_header>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "\t<req_header_len>:\t0 means clearing all websocket request headers previously set, other value means setting a new request header\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<req_header>:\trequest header content\r\n");
}

void at_wshead(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id, i;
	u32 req_header_len;
	char *req_header;

	UNUSED(argc);

	if (arg == NULL) {
		at_wshead_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);

	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	}

	req_header_len = atoi(argv[2]);

	if (req_header_len == 0) {
		/* clear all headers */
		for (i = 0; i < MAX_HEADER_NUM; i++) {
			if (ws_config[link_id].ws_header[i] != NULL) {
				ws_free(ws_config[link_id].ws_header[i]);
				ws_config[link_id].ws_header[i] = NULL;
			}
		}
	} else {
		req_header = (char *)argv[3];
		if (req_header == NULL) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "req_header is empty\r\n");
			error_no = 1;
			goto end;
		}

		/* set a new header */
		for (i = 0; i < MAX_HEADER_NUM; i++) {
			if (ws_config[link_id].ws_header[i] == NULL) {
				ws_config[link_id].ws_header[i] = (char *) ws_malloc(req_header_len + 1);
				if (ws_config[link_id].ws_header[i] == NULL) {
					RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "malloc failed\r\n");
					error_no = 2;
					goto end;
				}
				memcpy(ws_config[link_id].ws_header[i], req_header, req_header_len);
				break;
			}
		}

		if (i == MAX_HEADER_NUM) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "header number can not exceed %d\r\n", MAX_HEADER_NUM);
			error_no = 1;
			goto end;
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wsheadraw_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSHEADRAW=<link_id>,<req_header_len>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "\t<req_header_len>:\t0 means clearing all websocket request headers previously set, other value means setting a new request header\r\n");
}

void at_wsheadraw(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id;
	u32 req_header_len;
	int i, res;

	UNUSED(argc);

	if (arg == NULL) {
		at_wsheadraw_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);

	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	}

	req_header_len = atoi(argv[2]);

	if (req_header_len == 0) {
		/* clear all headers */
		for (i = 0; i < MAX_HEADER_NUM; i++) {
			if (ws_config[link_id].ws_header[i] != NULL) {
				ws_free(ws_config[link_id].ws_header[i]);
				ws_config[link_id].ws_header[i] = NULL;
			}
		}
	} else {
		/* set a new header */
		for (i = 0; i < MAX_HEADER_NUM; i++) {
			if (ws_config[link_id].ws_header[i] == NULL) {
				ws_config[link_id].ws_header[i] = (char *) ws_malloc(req_header_len + 1);
				if (ws_config[link_id].ws_header[i] == NULL) {
					RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "malloc failed\r\n");
					error_no = 2;
					goto end;
				}
				/* tt mode */
				res = atcmd_tt_mode_start(req_header_len);
				if (res < 0) {
					RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "atcmd_tt_mode_start failed\r\n");
					error_no = 3;
					goto end;
				}
				atcmd_tt_mode_get((u8 *)ws_config[link_id].ws_header[i], req_header_len);
				atcmd_tt_mode_end();
				break;
			}
		}

		if (i == MAX_HEADER_NUM) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "header number can not exceed %d\r\n", MAX_HEADER_NUM);
			error_no = 1;
			goto end;
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wsopen_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSOPEN=<link_id>,<host>[,<path>][,<port>],<conn_type>[,<certificate_index>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<host>:\thost of websocket server\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<path>:\tpath\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<port>:\tport of host\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<conn_type>:\twhether to use ssl and certificate, must be %d~%d\r\n", WEBSOCKET_OVER_TCP,
			 WEBSOCKET_OVER_TLS_VERIFY_SERVER_AND_CLIENT_CERT);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<certificate_index>:\tselect certificate\r\n");
}

static void at_ws_handler_data(wsclient_context **wsclient, int data_len)
{
	wsclient_context *wsc = *wsclient;
	at_printf_indicate(">>>>>> Receiving: %s with length: %d\n", wsc->receivedData, data_len);
}

static void at_ws_handler_pong(wsclient_context **wsclient)
{
	int link_id;
	int res = 0;
	wsclient_context *wsc = *wsclient;

	for (link_id = 0; link_id < MAX_WEBSOCKET_LINK_NUM; link_id++) {
		if (ws_config[link_id].ws_client == wsc) {
			res = 1;
			break;
		}
	}

	if (res > 0) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_DEBUG, ">>>>>> Receiving: pong, link_id  = %d\n", link_id);
		struct pingtime_item *item = list_first_entry(&ping_time_list[link_id], struct pingtime_item, node);
		list_del(&item->node);
		ws_free(item);
	}
}

static void at_ws_handler_close(wsclient_context *wsclient)
{
	UNUSED(wsclient);
	at_printf_indicate(">>>>>> client will close\n");
}

static void wsclient_conn_thread(void *param)
{
	wsclient_context *wsclient;
	int link_id = *(int *)param;

	wsclient = ws_config[link_id].ws_client;

	while (wsclient && wsclient->readyState != WSC_CLOSED) {
		/* check ping timeout */
		struct pingtime_item *item = list_first_entry(&ping_time_list[link_id], struct pingtime_item, node);
		if (!list_empty(&ping_time_list[link_id])) {
			if (rtos_time_get_current_system_time_ms() >= item->ping_send_time + ws_config[link_id].ping_timeout_sec * 1000) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "ping timeout, not receive pong for %d seconds\n", ws_config[link_id].ping_timeout_sec);
				ws_close(&(ws_config[link_id].ws_client));
			}
		}

		/* send ping */
		if (rtos_time_get_current_system_time_ms() >= ws_config[link_id].next_ping_time) {
			/* update next ping time */
			ws_config[link_id].next_ping_time = rtos_time_get_current_system_time_ms() + ws_config[link_id].ping_intv_sec * 1000;

			struct pingtime_item *new_item;
			new_item = (struct pingtime_item *)ws_malloc(sizeof(struct pingtime_item));
			if (new_item == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "malloc failed\r\n");
				ws_close(&(ws_config[link_id].ws_client));
			}
			new_item->ping_send_time = rtos_time_get_current_system_time_ms();
			list_add_tail(&new_item->node, &ping_time_list[link_id]);

			ws_sendPing(1, wsclient);
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_DEBUG, "send ping\n");
		}

		ws_poll(1000, &wsclient);
	}

	if (wsclient) {
		ws_free(wsclient);
		wsclient = NULL;
		ws_config[link_id].ws_client = NULL;
	}

	rtos_task_delete(NULL);
}

void at_wsopen(void *arg)
{
	int argc = 0;
	int error_no = 0, ret;
	char *argv[MAX_ARGC] = {0};
	int link_id, port, conn_type, cert_index, size;
	char *host, *path, *req_header;
	char url[DNS_MAX_NAME_LENGTH];
	int i, total_header_len = 0, pos = 0;
	wsclient_context *wsclient;
	char *client_cert = NULL;
	char *client_key = NULL;
	char *ca_cert = NULL;

	UNUSED(argc);
	UNUSED(cert_index);

	if (arg == NULL) {
		at_wsopen_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {  //todo: check link status
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	} else {
		if (ws_config[link_id].ws_client && ws_config[link_id].ws_client->readyState != WSC_CLOSED) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link %d is open\r\n", link_id);
			error_no = 1;
			goto end;
		}
	}

	host = (char *)argv[2];
	if (host == NULL) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "host is empty\r\n");
		error_no = 1;
		goto end;
	}

	path = (char *)argv[3];

	port = atoi(argv[4]);

	conn_type = atoi(argv[5]);
	if (conn_type < WEBSOCKET_OVER_TCP || conn_type > WEBSOCKET_OVER_TLS_VERIFY_SERVER_AND_CLIENT_CERT) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "conn_type must be %d~%d\r\n", WEBSOCKET_OVER_TCP, WEBSOCKET_OVER_TLS_VERIFY_SERVER_AND_CLIENT_CERT);
		error_no = 1;
		goto end;
	}

	memset(url, 0, DNS_MAX_NAME_LENGTH);
	if (conn_type == WEBSOCKET_OVER_TCP) {
		memcpy(url, "ws://", strlen("ws://"));
		memcpy(url + strlen("ws://"), host, strlen(host));
	} else {
#ifndef USING_SSL
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "\r\nNot Support the wss server!\r\n");
		error_no = 1;
		goto end;
#endif

		if (conn_type > WEBSOCKET_OVER_TLS) {
			if (!(argv[6] != NULL && (strlen(argv[6]) > 0))) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "certificate_index is empty!\r\n");
				error_no = 1;
				goto end;
			}
			cert_index = atoi(argv[6]);
		}

		memcpy(url, "wss://", strlen("wss://"));
		memcpy(url + strlen("wss://"), host, strlen(host));
	}

	wsclient = create_wsclient(url, port, path, NULL, ws_config[link_id].buffer_size, ws_config[link_id].max_queue_size);
	if (wsclient != NULL) {
		if (ws_config[link_id].protocol != NULL) {
			ws_handshake_header_set_protocol(wsclient, ws_config[link_id].protocol, strlen(ws_config[link_id].protocol));
		}

		if (ws_config[link_id].version != NULL) {
			ws_handshake_header_set_version(wsclient, ws_config[link_id].version, strlen(ws_config[link_id].version));
		}

		if (ws_config[link_id].stable_buf_num > ws_config[link_id].max_queue_size) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "stable_buf_num(%d) must not exceed max_queue_size(%d)!\r\n", ws_config[link_id].stable_buf_num,
					 ws_config[link_id].max_queue_size);
			error_no = 1;
			goto end;
		} else {
			ws_multisend_opts(wsclient, ws_config[link_id].stable_buf_num);
		}

		for (i = 0; i < MAX_HEADER_NUM; i++) {
			if (ws_config[link_id].ws_header[i]) {
				total_header_len += strlen(ws_config[link_id].ws_header[i]) + strlen("\r\n");
			} else {
				break;
			}
		}

		if (total_header_len > 0) {
			req_header = (char *) ws_malloc(total_header_len + 1);
			if (req_header == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", total_header_len + 1);
				error_no = 2;
				goto end;
			}

			for (i = 0; i < MAX_HEADER_NUM; i++) {
				if (ws_config[link_id].ws_header[i]) {
					sprintf(req_header + pos, "%s\r\n", ws_config[link_id].ws_header[i]);
					pos += strlen(ws_config[link_id].ws_header[i]) + strlen("\r\n");;
				} else {
					break;
				}
			}
			ws_handshake_set_header_fields(wsclient, req_header, total_header_len);
		}

		if (conn_type == WEBSOCKET_OVER_TLS_VERIFY_SERVER) {
			/* get ca */
			size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CA size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			ca_cert = (char *) ws_malloc(size + 1);
			if (ca_cert == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ca_cert, CLIENT_CA, cert_index);
			if (ret > 0) {
				wsclient->ca_cert = ca_cert;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CA failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}
		} else if (conn_type == WEBSOCKET_OVER_TLS_CLIENT_CERT) {
			/* get client cert */
			size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CERT size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			client_cert = (char *) ws_malloc(size + 1);
			if (client_cert == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(client_cert, CLIENT_CERT, cert_index);
			if (ret > 0) {
				wsclient->client_cert = client_cert;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CERT failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}

			/* get client key */
			size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_KEY size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			client_key = (char *) ws_malloc(size + 1);
			if (client_key == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(client_key, CLIENT_KEY, cert_index);
			if (ret > 0) {
				wsclient->client_key = client_key;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_KEY failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}
		} else if (conn_type == WEBSOCKET_OVER_TLS_VERIFY_SERVER_AND_CLIENT_CERT) {
			/* get ca */
			size = atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CA size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			ca_cert = (char *) ws_malloc(size + 1);
			if (ca_cert == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ca_cert, CLIENT_CA, cert_index);
			if (ret > 0) {
				wsclient->ca_cert = ca_cert;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CA failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}

			/* get client cert */
			size = atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CERT size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			client_cert = (char *) ws_malloc(size + 1);
			if (client_cert == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(client_cert, CLIENT_CERT, cert_index);
			if (ret > 0) {
				wsclient->client_cert = client_cert;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_CERT failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}

			/* get client key */
			size = atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index);
			if (size <= 0) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_KEY size failed, size = %d!\r\n", size);
				error_no = 4;
				goto end;
			}
			client_key = (char *) ws_malloc(size + 1);
			if (client_key == NULL) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "Malloc(%d bytes) failed!\r\n", size + 1);
				error_no = 2;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(client_key, CLIENT_KEY, cert_index);
			if (ret > 0) {
				wsclient->client_key = client_key;
			} else {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "get CLIENT_KEY failed, ret = %d!\r\n", ret);
				error_no = 4;
				goto end;
			}
		}

		ret = ws_connect_url(wsclient);
		if (ret >= 0) {
			ws_config[link_id].ws_client = wsclient;
			ws_config[link_id].next_ping_time = rtos_time_get_current_system_time_ms() + ws_config[link_id].ping_intv_sec * 1000;

			ws_dispatch(at_ws_handler_data);
			ws_pong(at_ws_handler_pong);
			ws_dispatch_close(at_ws_handler_close);

			for (i = 0; i < MAX_WEBSOCKET_LINK_NUM; i++) {
				INIT_LIST_HEAD(&ping_time_list[i]);
			}

			at_printf_indicate("\r\n>>>>>> Connect to websocket server success\n");

			if (rtos_task_create(NULL, ((const char *)"wsclient_conn_thread"), wsclient_conn_thread, &link_id, 1024 * 4, 1) != SUCCESS) {
				RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "\n\r%s rtos_task_create(wsclient_conn_thread) failed", __FUNCTION__);
				error_no = 3;
				goto end;
			}
		} else {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "\r\nConnect to websocket server failed!\r\n");
			error_no = 3;
			goto end;
		}
	} else {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "\r\nCreate websocket context failed!\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);

		if (ca_cert) {
			ws_free(ca_cert);
			ca_cert = NULL;
			wsclient->ca_cert = NULL;
		}
		if (client_cert) {
			ws_free(client_cert);
			client_cert = NULL;
			wsclient->client_cert = NULL;
		}
		if (client_key) {
			ws_free(client_key);
			client_key = NULL;
			wsclient->client_key = NULL;
		}
	}
}

void at_wssend_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSSEND=<link_id>,<length>[,<use_mask>][,<opcode>],<data>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<length>:\tlength of sending data\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<use_mask>:\twhether to use mask, default is 1(use)\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<opcode>:\topcode in the websocket frame, must be 0~0xF, default is 1(text)\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<data>:\tdata want to send to the server\r\n");
}

void at_wssend(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id, length, opcode, use_mask;
	char *send_buf;

	UNUSED(argc);

	if (arg == NULL) {
		at_wssend_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	}

	if (ws_config[link_id].ws_client == NULL) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link %d is not open\r\n", link_id);
		error_no = 1;
		goto end;
	}

	length = atoi(argv[2]);
	if (length <= 0 || length > ws_config[link_id].buffer_size) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "length must be 1~%d\r\n", ws_config[link_id].buffer_size);
		error_no = 1;
		goto end;
	}

	if (argv[3] != NULL && (strlen(argv[3]) > 0)) {
		use_mask = atoi(argv[3]);
	} else {
		use_mask = 1;
	}

	if (argv[4] != NULL && (strlen(argv[4]) > 0)) {
		opcode = atoi(argv[4]);
		if (opcode < 0 || opcode > 0xF) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "opcode must be 0~0xF\r\n");
			error_no = 1;
			goto end;
		}
	} else {
		opcode = TEXT_FRAME;
	}

	if (argv[5] != NULL && (strlen(argv[5]) > 0)) {
		send_buf = (char *)argv[5];
		ws_sendData(opcode, length, (u8 *)send_buf, use_mask, ws_config[link_id].ws_client);
	} else {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "data is empty\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wssendraw_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSSENDRAW=<link_id>,<length>[,<use_mask>][,<opcode>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<length>:\tlength of sending data\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<use_mask>:\twhether to use mask, default is 1(use)\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<opcode>:\topcode in the websocket frame, must be 0~0xF, default is 1(text)\r\n");
}

void at_wssendraw(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id, length, opcode, use_mask;
	u8 *send_buf = NULL;
	int frag_len, res;

	UNUSED(argc);

	if (arg == NULL) {
		at_wssendraw_help();
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {  //todo: check link status
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	}

	if (ws_config[link_id].ws_client == NULL) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link %d is not open\r\n", link_id);
		error_no = 1;
		goto end;
	}

	length = atoi(argv[2]);
	if (length <= 0 || length > ws_config[link_id].buffer_size) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "length must be 1~%d\r\n", ws_config[link_id].buffer_size);
		error_no = 1;
		goto end;
	}

	if (argv[3] != NULL && (strlen(argv[3]) > 0)) {
		use_mask = atoi(argv[3]);
	} else {
		use_mask = 1;
	}

	if (argv[4] != NULL && (strlen(argv[4]) > 0)) {
		opcode = atoi(argv[4]);
		if (opcode < 0 || opcode > 0xF) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "opcode must be 0~0xF\r\n");
			error_no = 1;
			goto end;
		}
	} else {
		opcode = TEXT_FRAME;
	}

	send_buf = (u8 *)ws_malloc(length <= MAX_TT_BUF_LEN ? length : MAX_TT_BUF_LEN);
	if (send_buf == NULL) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "ws_malloc failed, length = %d\r\n", length);
		error_no = 2;
		goto end;
	}

	/* tt mode */
	res = atcmd_tt_mode_start(length);
	if (res < 0) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "atcmd_tt_mode_start failed\r\n");
		error_no = 3;
		goto end;
	}

	if (length <= MAX_TT_BUF_LEN) {
		atcmd_tt_mode_get(send_buf, length);
		ws_sendData(opcode, length, send_buf, use_mask, ws_config[link_id].ws_client);
	} else {
		while (length > 0) {
			frag_len = (length <= MAX_TT_BUF_LEN) ? length : MAX_TT_BUF_LEN;
			atcmd_tt_mode_get(send_buf, frag_len);
			ws_sendData(opcode, frag_len, send_buf, use_mask, ws_config[link_id].ws_client);
			length -= frag_len;
		}
	}
	atcmd_tt_mode_end();

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
	if (send_buf) {
		ws_free(send_buf);
	}
}

void at_wsclose_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSCLOSE=<link_id>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
}


void at_wsclose(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id;

	UNUSED(argc);

	argc = parse_param(arg, argv);
	if (arg == NULL) {
		at_wsclose_help();
		error_no = 1;
		goto end;
	}

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	} else {
		if (ws_config[link_id].ws_client) {
			ws_close(&(ws_config[link_id].ws_client));
		} else {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link %d is not open\r\n", link_id);
			error_no = 1;
			goto end;
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void print_global_info(void)
{
	at_printf("\r\n");
	at_printf("Global config:\r\n");
	at_printf("\tconnect_timeout: %d\r\n", wsclient_connecttimeout);
	at_printf("\trecv_timeout: %d\r\n", wsclient_recvtimeout);
	at_printf("\tsend_timeout: %d\r\n", wsclient_sendtimeout);
	at_printf("\tsend_blocktime: %d\r\n", wsclient_sendblocktime);
	at_printf("\tkeepalive_idle: %d\r\n", wsclient_keepalive_idle);
	at_printf("\tkeepalive_interval: %d\r\n", wsclient_keepalive_interval);
	at_printf("\tkeepalive_count: %d\r\n", wsclient_keepalive_count);
}

void print_link_info(int link_id)
{
	int i;

	at_printf("\r\n");
	at_printf("link_id: %d\r\n", link_id);
	at_printf("\tstatus: %d\r\n", ws_config[link_id].ws_client ? ws_config[link_id].ws_client->readyState : WSC_CLOSED);
	at_printf("\tping_intv_sec: %d\r\n", ws_config[link_id].ping_intv_sec);
	at_printf("\tping_timeout_sec: %d\r\n", ws_config[link_id].ping_timeout_sec);
	at_printf("\tbuffer_size: %d\r\n", ws_config[link_id].buffer_size);
	at_printf("\tmax_queue_size: %d\r\n", ws_config[link_id].max_queue_size);
	at_printf("\tprotocol: %s\r\n", ws_config[link_id].protocol ? ws_config[link_id].protocol : "");
	at_printf("\tversion: %s\r\n", ws_config[link_id].version ? ws_config[link_id].version : "");
	at_printf("\tstable_buf_num: %d\r\n", ws_config[link_id].stable_buf_num);

	at_printf("\theader:\r\n");
	for (i = 0; i < MAX_HEADER_NUM; i++) {
		if (ws_config[link_id].ws_header[i] != NULL) {
			at_printf("\t\t%s\r\n", ws_config[link_id].ws_header[i]);
		} else {
			break;
		}
	}
}

void at_wsquery(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id;

	UNUSED(argc);

	argc = parse_param(arg, argv);

	if (arg == NULL) {
		int i;
		print_global_info();
		for (i = 0; i < MAX_WEBSOCKET_LINK_NUM; i++) {
			print_link_info(i);
		}
	} else {
		link_id = atoi(argv[1]);
		if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
			error_no = 1;
			goto end;
		} else {
			print_global_info();
			print_link_info(link_id);
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

void at_wsqueuecheck_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+WSTXQUEUECHECK=<link_id>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\tconnect id, must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
}

void at_wsqueuecheck(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id;

	UNUSED(argc);

	argc = parse_param(arg, argv);
	if (arg == NULL) {
		at_wsqueuecheck_help();
		error_no = 1;
		goto end;
	}

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MAX_WEBSOCKET_LINK_NUM) {
		RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link_id must be 0~%d\r\n", MAX_WEBSOCKET_LINK_NUM - 1);
		error_no = 1;
		goto end;
	} else {
		if (ws_config[link_id].ws_client) {
			if (ws_config[link_id].ws_client->ready_send_buf_num < ws_config[link_id].ws_client->max_queue_size) {
				at_printf("Tx queue is not full\n");
			} else {
				at_printf("Tx queue is full\n");
			}
		} else {
			RTK_LOGS(TAG_AT_WEBSOCKET, RTK_LOG_ERROR, "link %d is not open\r\n", link_id);
			error_no = 1;
			goto end;
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\nOK\r\n");
	} else {
		at_printf("\r\nERROR:%d\r\n", error_no);
	}
}

log_item_t at_websocket_items[ ] = {
	{"+WSCFG", at_wscfg, {NULL, NULL}},
	{"+WSGLCFG", at_wsglcfg, {NULL, NULL}},
	{"+WSHEAD", at_wshead, {NULL, NULL}},
	{"+WSHEADRAW", at_wsheadraw, {NULL, NULL}},
	{"+WSOPEN", at_wsopen, {NULL, NULL}},
	{"+WSSEND", at_wssend, {NULL, NULL}},
	{"+WSSENDRAW", at_wssendraw, {NULL, NULL}},
	{"+WSCLOSE", at_wsclose, {NULL, NULL}},
	{"+WSQUERY", at_wsquery, {NULL, NULL}},
	{"+WSTXQUEUECHECK", at_wsqueuecheck, {NULL, NULL}},
};

void print_websocket_at(void)
{
	int i;
	int itemNum = 0;

	itemNum = sizeof(at_websocket_items) / sizeof(at_websocket_items[0]);
	for (i = 0; i < itemNum; i++) {
		at_printf("AT%s\r\n", at_websocket_items[i].log_cmd);
	}
}

void init_websocket_struct(void)
{
	int i, j;

	for (i = 0; i < MAX_WEBSOCKET_LINK_NUM; i++) {
		ws_config[i].ping_intv_sec      = DEFAULT_PING_INTERVAL;
		ws_config[i].ping_timeout_sec   = DEFAULT_PING_TIMEOUT;
		ws_config[i].buffer_size        = DEFAULT_BUFFER_SIZE;
		ws_config[i].max_queue_size     = DEFAULT_MAX_QUEUE_SIZE;
		ws_config[i].protocol           = DEFAULT_WEBSOCKET_PROTOCOL;
		ws_config[i].version            = DEFAULT_WEBSOCKET_VERSION;
		ws_config[i].stable_buf_num     = DEFAULT_STABLE_BUF_NUM;
		ws_config[i].ws_client          = NULL;

		for (j = 0; j < MAX_HEADER_NUM; j++) {
			ws_config[i].ws_header[j] = NULL;
		}
	}
}

void at_websocket_init(void)
{
	init_websocket_struct();
	atcmd_service_add_table(at_websocket_items, sizeof(at_websocket_items) / sizeof(at_websocket_items[0]));
}

#endif /* CONFIG_ATCMD_WEBSOCKET */
#endif /* CONFIG_LWIP_LAYER */
