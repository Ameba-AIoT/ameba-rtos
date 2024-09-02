/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_LWIP_H__
#define __ATCMD_LWIP_H__

#include "main.h"
#include <lwip/opt.h>
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/netdb.h"
#include "lwip_netconf.h"

#define ENABLE_TCPIP_SSL                  0
#define ATCMD_SSL_DEBUG_LEVEL             RTK_LOG_DEBUG //0
#define ENABLE_TCPIP_AUTOLINK             0

#ifdef CONFIG_LWIP_LAYER
#define NUM_NS                            MEMP_NUM_NETCONN

#define INVALID_CON_ID                    -1

#define INVALID_SOCKET_ID                 -1

#define ATCP_STACK_SIZE                   4096

#define NODE_MODE_TCP                     0
#define NODE_MODE_UDP                     1
#define NODE_MODE_SSL                     2
#define NODE_MODE_INVALID                 -1

#define NODE_ROLE_SERVER                  0
#define NODE_ROLE_CLIENT                  1
#define NODE_ROLE_SEED                    2

#define ATCMD_LWIP_TASK_PRIORITY          1

#define ETH_MAX_MTU                       1500

#define ATCMD_LWIP_TT_MAX_DELAY_TIME_MS   20

#define RECV_SELECT_TIMEOUT_SEC           0
#define RECV_SELECT_TIMEOUT_USEC          20000

#define ATCMD_LWIP_CONN_STORE_MAX_NUM     1


typedef struct _node {
	int con_id;
	int sockfd;
	s8_t role;
	int protocol;
	u32_t addr;
	u16_t  port;
	u32_t local_addr;
	u16_t local_port;
	rtos_task_t handletask;
	struct _node *next;
	struct _node *nextseed;
	void *context;
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

int atcmd_lwip_tt_proc(void);
void print_lwip_at(void);
void at_tcpip_init(void);

extern rtos_sema_t atcmd_lwip_tt_sema;
extern volatile int atcmd_lwip_tt_mode;
extern volatile int atcmd_lwip_tt_datasize;
extern volatile int atcmd_lwip_tt_lasttickcnt;
#endif /* CONFIG_LWIP_LAYER */

#endif
