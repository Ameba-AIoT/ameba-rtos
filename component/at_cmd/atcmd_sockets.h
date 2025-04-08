/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SOCKETS_H__
#define __ATCMD_SOCKETS_H__


#include "lwip_netconf.h"

#include "at_intf_spi.h"

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
//#include "mbedtls/certs.h"
#include "mbedtls/debug.h"
#include "mbedtls/version.h"


#if ((!defined MBEDTLS_BIGNUM_C) \
	|| (!defined MBEDTLS_SSL_TLS_C) || (!defined MBEDTLS_SSL_SRV_C) \
	|| (!defined MBEDTLS_SSL_CLI_C) || (!defined MBEDTLS_NET_C) \
	|| (!defined MBEDTLS_RSA_C) || (!defined MBEDTLS_X509_CRT_PARSE_C))
#error "Please make sure the MBEDTLS options are enabled."
#endif


#define ATCMD_SSL_DEBUG_LEVEL             0	//RTK_LOG_DEBUG
#define ENABLE_TCPIP_AUTOLINK             0

#define SSL_CERT_NUM                      1
#define SSL_CERT_SIZE                     2048
#define SSL_AUTH_MODE_NO                  0
#define SSL_AUTH_MODE_LOAD_CERT           1
#define SSL_AUTH_MODE_LOAD_CA             2
#define SSL_AUTH_MODE_BOTH                3


#ifdef CONFIG_LWIP_LAYER
#define NUM_NS                            MEMP_NUM_NETCONN

#define INVALID_CON_ID                    -1

#define INVALID_SOCKET_ID                 -1

#define ATCP_STACK_SIZE                   4096

#define NODE_MODE_TCP                     0
#define NODE_MODE_UDP                     1
#define NODE_MODE_SSL                     2
#define NODE_MODE_INVALID                 -1

#define SOCKET_SERVER_OVER_UDP                   0
#define SOCKET_SERVER_OVER_TCP                   1
#define SOCKET_SERVER_OVER_TLS                   2	// do not verify client certificate
#define SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT	 3	// verify client certificate

#define SOCKET_CLIENT_OVER_UDP                   0
#define SOCKET_CLIENT_OVER_TCP                   1
#define SOCKET_CLIENT_OVER_TLS_NO_VERIFY         2
#define SOCKET_CLIENT_OVER_TLS_VERIFY_SERVER     3
#define SOCKET_CLIENT_OVER_TLS_VERIFY_CLIENT     4
#define SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH       5

#define SOCKET_PROTOCOL_INVALID                  -1

#define NODE_ROLE_SERVER                  0
#define NODE_ROLE_CLIENT                  1
#define NODE_ROLE_SEED                    2
#define NODE_ROLE_INVALID                 -1

#define ATCMD_LWIP_TASK_PRIORITY          1

#define ETH_MAX_MTU                       1500

#define AT_SOCKET_RECEIVE_BUFFER_SIZE     ATCMD_SPI_DMA_SIZE

#define SOCKET_AT_BACKLOG				  5

#define RECV_SELECT_TIMEOUT_SEC           0
#define RECV_SELECT_TIMEOUT_USEC          20000

#define ATCMD_LWIP_CONN_STORE_MAX_NUM     1


typedef struct _node {
	s8_t con_id;
	s8_t role;	// 0:server, 1:client, 2:seed
	s8_t cert_index;	//Security certificate suite index
	s8_t auto_rcv;	//auto receive flag
	int sockfd;
	int protocol;	// 0:UDP, 1:TCP, 2-5:TLS
	u32_t dst_ip;
	u16_t dst_port;
	u16_t src_port;
	u32_t src_ip;
	rtos_task_t auto_rcv_task;
	u32_t addr;
	u16_t port;
	u16_t local_port;
	u32_t local_addr;
	rtos_task_t handletask;
	struct _node *next;
	struct _node *nextseed;
	mbedtls_ssl_context *ssl;
	mbedtls_ssl_config *conf;
	mbedtls_x509_crt *ca_crt[SSL_CERT_NUM];
	mbedtls_x509_crt *cert_crt[SSL_CERT_NUM];
	mbedtls_pk_context *private_key[SSL_CERT_NUM];
} node;

#if ENABLE_TCPIP_AUTOLINK
typedef enum {
	AT_PARTITION_ALL = 0,
	AT_PARTITION_UART = 1,
	AT_PARTITION_WIFI = 2,
	AT_PARTITION_LWIP = 3
} AT_PARTITION;

typedef enum {
	AT_PARTITION_READ = 0,
	AT_PARTITION_WRITE = 1,
	AT_PARTITION_ERASE = 2
} AT_PARTITION_OP;

typedef struct _atcmd_lwip_conn_info {
	int32_t role; /* client, server or seed */
	uint32_t protocol; /* tcp or udp */
	uint32_t remote_addr; /* remote ip */
	uint32_t remote_port; /*remote port */
	uint32_t local_addr; /* locale ip, not used yet */
	uint32_t local_port; /* locale port, not used yet */
	uint32_t reserved; /* reserve for further use */
} atcmd_lwip_conn_info;

typedef struct _atcmd_lwip_conf {
	int32_t enable; /* enable / disable */
	int32_t conn_num;
	int32_t last_index;
	int32_t reserved;
	struct _atcmd_lwip_conn_info conn[ATCMD_LWIP_CONN_STORE_MAX_NUM];
} atcmd_lwip_conf;
#endif /* ENABLE_TCPIP_AUTOLINK */

void print_socket_at(void);

void at_socket_init(void);

#endif /* CONFIG_LWIP_LAYER */

#endif
