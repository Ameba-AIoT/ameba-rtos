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


#ifdef CONFIG_LWIP_LAYER

#define INVALID_LINK_ID                   -1
#define INVALID_SOCKET_ID                 -1

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

#define ATCMD_SOCKET_TASK_PRIORITY          1

#define AT_SOCKET_RECEIVE_BUFFER_SIZE     ATCMD_SPI_DMA_SIZE

#define SOCKET_AT_BACKLOG				  5

#define RECV_SELECT_TIMEOUT_SEC           0
#define RECV_SELECT_TIMEOUT_USEC          20000

typedef struct _node {
	s8_t link_id;	// 0 ~ (MEMP_NUM_NETCONN-1)
	s8_t role;	// 0:server, 1:client, 2:seed
	s8_t cert_index;	// > 0; Security certificate & key suite index for TLS
	s8_t auto_rcv;	// Auto receive flag; 0-don't enable, 1-enable
	int sockfd;	// >= 0; Socket fd
	int protocol;	// 0:UDP, 1:TCP, 2-5:TLS(client) || 2-3:TLS(server)
	u32_t dst_ip;	// Destination IP address
	u16_t dst_port;	// Destination PORT
	u16_t src_port;	// Source IP address
	u32_t src_ip;	// Source PORT
	rtos_task_t auto_rcv_task;
	struct _node *prevnode;
	struct _node *nextseed;
	mbedtls_ssl_context *ssl;
	mbedtls_ssl_config *conf;
	mbedtls_x509_crt *ca_crt;
	mbedtls_x509_crt *cert_crt;
	mbedtls_pk_context *private_key;
} node;


void print_socket_at(void);

void at_socket_init(void);

#endif /* CONFIG_LWIP_LAYER */

#endif
