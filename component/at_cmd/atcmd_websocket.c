/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifndef CONFIG_MP_INCLUDED
#ifdef CONFIG_LWIP_LAYER
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_websocket.h"

#if defined(CONFIG_ATCMD_WEBSOCKET) && (CONFIG_ATCMD_WEBSOCKET == 1)

struct websocket_config ws_config[MAX_WEBSOCKET_LINK_NUM];

void at_wscfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "AT+WSCFG=<link_id>,<ping_intv_sec>,<ping_timeout_sec>[,<buffer_size>,<max_queue_size>,<protocol>,<version>,<stable_buf_num>]\r\n");
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
			 "AT+WSGLCFG=[<connect_timeout>,<recv_timeout>,<send_timeout>,<send_blocktime>,<keepalive_idle>,<keepalive_interval>,<keepalive_count>]\r\n");
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

log_item_t at_websocket_items[ ] = {
	{"+WSCFG", at_wscfg, {NULL, NULL}},
	{"+WSGLCFG", at_wsglcfg, {NULL, NULL}},
	{"+WSQUERY", at_wsquery, {NULL, NULL}},
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
	int i;

	for (i = 0; i < MAX_WEBSOCKET_LINK_NUM; i++) {
		ws_config[i].ping_intv_sec      = DEFAULT_PING_INTERVAL;
		ws_config[i].ping_timeout_sec   = DEFAULT_PING_TIMEOUT;
		ws_config[i].buffer_size        = DEFAULT_BUFFER_SIZE;
		ws_config[i].max_queue_size     = DEFAULT_MAX_QUEUE_SIZE;
		ws_config[i].protocol           = DEFAULT_WEBSOCKET_PROTOCOL;
		ws_config[i].version            = DEFAULT_WEBSOCKET_VERSION;
		ws_config[i].stable_buf_num     = DEFAULT_STABLE_BUF_NUM;
		ws_config[i].ws_client          = NULL;
	}
}

void at_websocket_init(void)
{
	init_websocket_struct();
	atcmd_service_add_table(at_websocket_items, sizeof(at_websocket_items) / sizeof(at_websocket_items[0]));
}

#endif /* CONFIG_ATCMD_WEBSOCKET */
#endif /* CONFIG_LWIP_LAYER */
#endif /* CONFIG_MP_INCLUDED */
