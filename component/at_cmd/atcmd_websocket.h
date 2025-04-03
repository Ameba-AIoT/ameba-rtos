/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WEBSOCKET_H__
#define __ATCMD_WEBSOCKET_H__

#ifdef CONFIG_LWIP_LAYER

#include "wsclient_api.h"
#include "wifi_api.h"

#define TAG_AT_WEBSOCKET   "AT_WEBSOCKET"

/* for websocket client */
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

/* for websocket server */
enum CONNECTION_TYPE_SERVER {
	WEBSOCKET_SERVER_OVER_TCP = 1,
	WEBSOCKET_SERVER_OVER_TLS,    // do not verify client certificate
	WEBSOCKET_SERVER_OVER_TLS_VERIFY_CLIENT,    // verify client certificate
};

#define MAX_SERVER_PING_INTERVAL           7200 //send interval of ping of server in seconds
#define MAX_SERVER_CONN                    3
#define MAX_SERVER_IDLE_TIMEOUT            7200 //the timeout if there is no data between server and client in seconds
#define MAX_SERVER_TX_SIZE                 (16 * 1024)
#define MAX_SERVER_RX_SIZE                 (16 * 1024)

#define DEFAULT_SERVER_CONN                3
#define DEFAULT_SERVER_PING_INTERVAL       30
#define DEFAULT_SERVER_IDLE_TIMEOUT        0
#define DEFAULT_SERVER_TX_SIZE             (4 * 1024)
#define DEFAULT_SERVER_RX_SIZE             (4 * 1024)
#define DEFAULT_SERVER_STACK_BYTES         (8 * 1024)

struct wssrv_request {
	char *header;				/*!< handshake header string parsed in websocket handshake */
	size_t header_len;				/*!< handshake header string length */
	char *path;					/*!< Pointer to resource path in the parsed handshake header string */
	size_t path_len;				/*!< Resource path data length */
	char *query;					/*!< Pointer to query string in the parsed handshake header string */
	size_t query_len;				/*!< Query string data length */
	char *version;				/*!< Pointer to HTTP version in the parsed handshake header string */
	size_t version_len;				/*!< HTTP version data length */
	uint8_t *host;					/*!< Pointer to Host header field in the parsed handshake header string */
	size_t host_len;				/*!< Host header field data length */
	char *ws_key;				/*!< Pointer to Sec-WebSocket-Key field in the parsed handshake header string */
	size_t ws_key_len;				/*!< Sec-WebSocket-Key field data length */
	char *ws_version;			/*!< Pointer to Sec-WebSocket-Versions field in the parsed handshake header string */
	size_t ws_version_len;				/*!< Sec-WebSocket-Key field data length */
};

enum wssrv_conn_state {
	WSSRV_CLOSED = 0,		/*!< Client Connection closed */
	WSSRV_CONNECTING,	/*!< Client is connecting */
	WSSRV_CONNECTED1,	/*!< Client is connected */
	WSSRV_CONNECTED2,	/*!< Connected and server sent ping to client */
	WSSRV_CLOSING,	/*!< Client will be closed */
} ;


struct wssrv_conn {
	int sock;						/*!< Client socket descriptor for connection */
	struct wssrv_request request;		/*!< Context for websocket server request */
	void *tls;						/*!< Context for TLS connection */
	char *response_header;		/*!< Pointer to transmission buffer of ws handshake response header */
	uint32_t last_ping_sent_time;	/*!< Last ping sent time in system ticks */
	uint32_t last_data_comm_time;	/*!< Last data received or sent time in system ticks */
	int tx_len;						/*!< The length of the transmission data */
	int rx_len;						/*!< The length of the received data */
	uint8_t *txbuf;					/*!< Pointer to transmission buffer of ws server will send */
	uint8_t *rxbuf;					/*!< Pointer to receiving buffer which received from client */
	uint8_t *receivedData;			/*!< Pointer to decoded receiving data which received from client */
	enum wssrv_conn_state state;		/*!< Connection state */
	struct task_struct task;        /*!< Connection task context */
};


extern void ws_server_setup_ping_interval(uint32_t interval_ms);
extern void ws_server_setup_idle_timeout(uint32_t timeout_ms);
extern void ws_server_setup_tx_rx_size(size_t tx_size, size_t rx_size);
extern int ws_server_start(uint16_t port, uint8_t max_conn, uint32_t stack_bytes, uint8_t secure);
extern void ws_server_dispatch(void (*callback)(struct wssrv_conn *, int, enum opcode_type));
extern void ws_server_dispatch_connect(void (*callback)(struct wssrv_conn *));
extern void ws_server_dispatch_disconnect(void (*callback)(struct wssrv_conn *));
extern void ws_server_stop(void);
extern int ws_server_setup_cert(const char *server_cert, const char *server_key, const char *ca_certs);
extern struct wssrv_conn *ws_server_get_conn_info(int conn_no);
extern void ws_server_sendText(char *message, size_t message_len, int use_mask, struct wssrv_conn *conn);
extern void ws_server_sendBinary(uint8_t *message, size_t message_len, int use_mask, struct wssrv_conn *conn);
extern void ws_server_sendClose(struct wssrv_conn *conn);
extern void ws_server_sendData(uint8_t type, size_t message_size, uint8_t *message, int useMask, uint8_t send_mode, uint8_t fin_flag, struct wssrv_conn *conn);
extern int ws_server_get_close_reason(struct wssrv_conn *conn);
extern void ws_server_set_close_reason(struct wssrv_conn *conn, int reason);

void print_websocket_at(void);
void at_websocket_init(void);

#endif /* CONFIG_LWIP_LAYER */
#endif /* __ATCMD_WEBSOCKET_H__ */