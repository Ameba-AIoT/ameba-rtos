/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#if ENABLE_TCPIP_SSL
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/debug.h"
#include "random.h"

#if ((!defined MBEDTLS_BIGNUM_C) || (!defined MBEDTLS_CERTS_C) \
	|| (!defined MBEDTLS_SSL_TLS_C) || (!defined MBEDTLS_SSL_SRV_C) \
	|| (!defined MBEDTLS_SSL_CLI_C) || (!defined MBEDTLS_NET_C) \
	|| (!defined MBEDTLS_RSA_C) || (!defined MBEDTLS_X509_CRT_PARSE_C))
#error "Please make sure the MBEDTLS options are enabled."
#endif
#endif /* ENABLE_TCPIP_SSL */


#ifdef CONFIG_LWIP_LAYER
static struct _node node_pool[NUM_NS];
static struct _node *mainlist = NULL;
static int atcmd_lwip_auto_recv = FALSE;
rtos_task_t atcmd_lwip_tt_task = NULL;
rtos_sema_t atcmd_lwip_tt_sema = NULL;
volatile int atcmd_lwip_tt_mode = FALSE; /* Transparent Transfer Mode. */
volatile int atcmd_lwip_tt_datasize = 0;
volatile int atcmd_lwip_tt_lasttickcnt = 0;

u8 *rx_buffer = NULL;
u8 *tx_buffer = NULL;

#if ENABLE_TCPIP_SSL
/* certificate and key for ssl server. */
static char *atcmd_ssl_server_crt[NUM_NS] = {NULL}; /* TODO: should be input by user */
static char *atcmd_ssl_server_ca_list[NUM_NS] = {NULL}; /* TODO: should be input by user */
static mbedtls_x509_crt *atcmd_ssl_srv_crt[NUM_NS] = {NULL};
static char *atcmd_ssl_server_key[NUM_NS] = {NULL}; /* TODO: should be input by user */
static mbedtls_pk_context *atcmd_ssl_srv_key[NUM_NS] = {NULL};
/* certificate and key for ssl client. */
static char *atcmd_ssl_client_ca_crt[NUM_NS] = {NULL}; /* TODO: should be input by user */
static mbedtls_x509_crt *atcmd_ssl_cli_ca_crt[NUM_NS] = {NULL};
static char *atcmd_ssl_client_crt[NUM_NS] = {NULL}; /* TODO: should be input by user */
static mbedtls_x509_crt *atcmd_ssl_cli_crt[NUM_NS] = {NULL};
static char *atcmd_ssl_client_key[NUM_NS] = {NULL}; /* TODO: should be input by user */
static mbedtls_pk_context *atcmd_ssl_clikey_rsa[NUM_NS] = {NULL};
#endif /* ENABLE_TCPIP_SSL */

extern struct netif xnetif[NET_IF_NUM];

#ifdef CONFIG_SUPPORT_ATCMD
extern char atcmd_buf[UART_LOG_CMD_BUFLEN];
#else
char atcmd_buf[UART_LOG_CMD_BUFLEN];
#endif

#if ENABLE_TCPIP_SSL
static void atcmd_ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
	UNUSED(ctx);
	UNUSED(file);
	UNUSED(line);

	if (level <= ATCMD_SSL_DEBUG_LEVEL) {
		RTK_LOGA(NOTAG, "%s", str);
	}
}

static int atcmd_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
	UNUSED(p_rng);
	random_get_bytes(output, output_len);
	return 0;
}

static void *atcmd_lwip_calloc(size_t num, size_t ele_size)
{
	void *p = NULL;

	if ((num > 0) && (ele_size > 0)) {
		p = rtos_mem_zmalloc(num * ele_size);
	}

	return p;
}

static char *atcmd_lwip_itoa(int value)
{
	char *val_str;
	int tmp = value, len = 1;

	while ((tmp /= 10) > 0) {
		len++;
	}

	val_str = (char *)rtos_mem_malloc(len + 1);
	if (val_str != NULL) {
		DiagSnPrintf(val_str, (len + 1), "%d", value);
	}

	return val_str;
}
#endif /* ENABLE_TCPIP_SSL */

int atcmd_lwip_tt_proc(void)
{
	if (atcmd_lwip_tt_mode == TRUE) {
		atcmd_lwip_tt_datasize = strlen(atcmd_buf);
		atcmd_lwip_tt_lasttickcnt = rtos_time_get_current_system_time_ms();
		if (atcmd_lwip_tt_sema != NULL) {
			rtos_sema_give(atcmd_lwip_tt_sema);
		}
		return SUCCESS;
	}
	return FAIL;
}

static int atcmd_lwip_is_autorecv_mode(void)
{
	return (atcmd_lwip_auto_recv == TRUE);
}

static void atcmd_lwip_set_autorecv_mode(int enable)
{
	atcmd_lwip_auto_recv = enable;
}

static void init_node_pool(void)
{
	int i;
	memset(node_pool, 0, sizeof(node_pool));
	for (i = 0; i < NUM_NS; i++) {
		node_pool[i].con_id = INVALID_CON_ID;
	}
}

static struct _node *create_list_node(int mode, s8_t role)
{
	int i = 0;
	SYS_ARCH_DECL_PROTECT(lev);

	for (i = 0; i < NUM_NS; i++) {
		SYS_ARCH_PROTECT(lev);
		if (node_pool[i].con_id == INVALID_CON_ID) {
			node_pool[i].con_id = i;
			SYS_ARCH_UNPROTECT(lev);
			node_pool[i].sockfd = INVALID_SOCKET_ID;
			node_pool[i].protocol = mode; /* 0:TCP, 1:UDP */
			node_pool[i].role = role; /* 0:server, 1:client, 2:SEED */
			node_pool[i].addr = 0;
			node_pool[i].port = -1;
			node_pool[i].handletask = NULL;
			node_pool[i].next = NULL;
			node_pool[i].nextseed = NULL;
			node_pool[i].context = NULL;
			return &node_pool[i];
		}
		SYS_ARCH_UNPROTECT(lev);
	}

	RTK_LOGI(NOTAG, "[create_list_node] No con_id available\r\n");
	return NULL;
}

static void delete_list_node(struct _node *n)
{
	struct _node *currNode, *prevNode, *currSeed, *precvSeed;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	if (n == NULL) {
		SYS_ARCH_UNPROTECT(lev);
		return;
	}

	/* need to remove it from mainlist first  */
	for (currNode = mainlist; currNode != NULL; prevNode = currNode, currNode = currNode->next) {
		if (currNode == n) {
			prevNode->next = currNode->next;
		}

		if (currNode->role != NODE_ROLE_SERVER) {
			continue;
		}

		precvSeed = currNode;
		currSeed = currNode->nextseed;
		while (currSeed != NULL) {
			if (currSeed == n) {
				precvSeed->nextseed = n->nextseed;
			}
			precvSeed = currSeed;
			currSeed = currSeed->nextseed;
		}
	}
	SYS_ARCH_UNPROTECT(lev);

	if (n->role == NODE_ROLE_SERVER) {
		/* node may have seed if it's under server mode */
		while (n->nextseed != NULL) {
			currSeed = n->nextseed;
			/* only tcp/ssl seed has its own socket, udp seed uses its server's
			so delete udp seed can't close socket which is used by server */
			if (currSeed->protocol == NODE_MODE_TCP && currSeed->sockfd != INVALID_SOCKET_ID) {
				close(currSeed->sockfd);
				currSeed->sockfd = INVALID_SOCKET_ID;
			}
#if ENABLE_TCPIP_SSL
			else if (currSeed->protocol == NODE_MODE_SSL && currSeed->sockfd != INVALID_SOCKET_ID) {
				mbedtls_ssl_context *ssl = (mbedtls_ssl_context *)currSeed->context;
				mbedtls_ssl_close_notify(ssl);
				mbedtls_net_context server_fd;
				server_fd.fd = currSeed->sockfd;
				mbedtls_net_free(&server_fd);
				mbedtls_ssl_free(ssl);
				rtos_mem_free(currSeed->context);
				currSeed->context = NULL;
			}
#endif
			n->nextseed = currSeed->nextseed;
			currSeed->con_id = INVALID_CON_ID;
		};
	}

	if (!((n->protocol == NODE_MODE_UDP) && (n->role == NODE_ROLE_SEED))) {
		if (n->sockfd != INVALID_SOCKET_ID) {
#if ENABLE_TCPIP_SSL
			if (n->protocol == NODE_MODE_SSL) {
				if (n->role == NODE_ROLE_SEED) {
					mbedtls_ssl_context *ssl = (mbedtls_ssl_context *)n->context;
					mbedtls_ssl_close_notify(ssl);
					mbedtls_net_context server_fd;
					server_fd.fd = currSeed->sockfd;
					mbedtls_net_free(&server_fd);
					mbedtls_ssl_free(ssl);
				} else if (n->role == NODE_ROLE_CLIENT) {
					mbedtls_ssl_context *ssl = (mbedtls_ssl_context *)n->context;
					mbedtls_ssl_close_notify(ssl);
					mbedtls_net_context server_fd;
					server_fd.fd = n->sockfd;
					mbedtls_net_free(&server_fd);
					if (atcmd_ssl_cli_ca_crt[n->con_id]) {
						mbedtls_x509_crt_free(atcmd_ssl_cli_ca_crt[n->con_id]);
						rtos_mem_free((void *)atcmd_ssl_cli_ca_crt[n->con_id]);
						atcmd_ssl_cli_ca_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_client_ca_crt[n->con_id]) {
						rtos_mem_free(atcmd_ssl_client_ca_crt[n->con_id]);
						atcmd_ssl_client_ca_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_cli_crt[n->con_id]) {
						mbedtls_x509_crt_free(atcmd_ssl_cli_crt[n->con_id]);
						rtos_mem_free((void *)atcmd_ssl_cli_crt[n->con_id]);
						atcmd_ssl_cli_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_client_crt[n->con_id]) {
						rtos_mem_free(atcmd_ssl_client_crt[n->con_id]);
						atcmd_ssl_client_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_clikey_rsa[n->con_id]) {
						mbedtls_pk_free(atcmd_ssl_clikey_rsa[n->con_id]);
						rtos_mem_free((void *)atcmd_ssl_clikey_rsa[n->con_id]);
						atcmd_ssl_clikey_rsa[n->con_id] = NULL;
					}
					if (atcmd_ssl_client_key[n->con_id]) {
						rtos_mem_free(atcmd_ssl_client_key[n->con_id]);
						atcmd_ssl_client_key[n->con_id] = NULL;
					}
					mbedtls_ssl_free(ssl);
				} else {
					mbedtls_net_context server_fd;
					server_fd.fd = n->sockfd;
					mbedtls_net_free(&server_fd);
					if (atcmd_ssl_srv_crt[n->con_id]) {
						mbedtls_x509_crt_free(atcmd_ssl_srv_crt[n->con_id]);
						rtos_mem_free((void *)atcmd_ssl_srv_crt[n->con_id]);
						atcmd_ssl_srv_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_server_crt[n->con_id]) {
						rtos_mem_free(atcmd_ssl_server_crt[n->con_id]);
						atcmd_ssl_server_crt[n->con_id] = NULL;
					}
					if (atcmd_ssl_server_ca_list[n->con_id]) {
						rtos_mem_free(atcmd_ssl_server_ca_list[n->con_id]);
						atcmd_ssl_server_ca_list[n->con_id] = NULL;
					}
					if (atcmd_ssl_srv_key[n->con_id]) {
						mbedtls_pk_free(atcmd_ssl_srv_key[n->con_id]);
						rtos_mem_free((void *)atcmd_ssl_srv_key[n->con_id]);
						atcmd_ssl_srv_key[n->con_id] = NULL;
					}
					if (atcmd_ssl_server_key[n->con_id]) {
						rtos_mem_free(atcmd_ssl_server_key[n->con_id]);
						atcmd_ssl_server_key[n->con_id] = NULL;
					}
				}
			} else
#endif
			{
				close(n->sockfd);
			}
			n->sockfd = INVALID_SOCKET_ID;
		}
	}
	/* task will exit itself in fail case */
	if (n->handletask != NULL) {
		rtos_task_delete(n->handletask);
		n->handletask = NULL;
	}
#if ENABLE_TCPIP_SSL
	if (n->context) {
		rtos_mem_free(n->context);
		n->context = NULL;
	}
#endif
	n->con_id = INVALID_CON_ID;
	return;
}

int hang_list_node(struct _node *insert_node)
{
	struct _node *n = mainlist;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	while (n->next != NULL) {
		n = n->next;
		/*need to check for server in case that two conns are binded to same port, because SO_REUSEADDR is enabled */
		if (insert_node->role == NODE_ROLE_SERVER) {
			if ((n->port == insert_node->port) && ((n->addr == insert_node->addr) && (n->role == insert_node->role) && (n->protocol == insert_node->protocol))) {
				SYS_ARCH_UNPROTECT(lev);
				struct in_addr addr;
				addr.s_addr = htonl(insert_node->addr);
				RTK_LOGI(NOTAG, "This conn(IP:%s PORT:%d) already exist\r\n", inet_ntoa(addr), insert_node->port);
				return -1;
			}
		}
	}

	n->next = insert_node;
	SYS_ARCH_UNPROTECT(lev);
	return 0;
}

struct _node *tryget_list_node(int n)
{
	SYS_ARCH_DECL_PROTECT(lev);

	if ((n <= 0) || (n > NUM_NS)) {
		return NULL;
	}

	SYS_ARCH_PROTECT(lev);
	if (node_pool[n].con_id == INVALID_CON_ID || node_pool[n].sockfd == INVALID_SOCKET_ID) {
		SYS_ARCH_UNPROTECT(lev);
		return NULL;
	}
	SYS_ARCH_UNPROTECT(lev);

	return &node_pool[n];
}

struct _node *seek_list_node(int con_id)
{
	struct _node *n = mainlist, *seed = NULL;

	while (n != NULL) {
		n = n->next;
		if (n->con_id == con_id) {
			return n;
		}
		if (n->nextseed != NULL) {
			seed = n;
			do {
				seed = seed->nextseed;
				if (seed->con_id == con_id) {
					return seed;
				}
			} while (seed->nextseed != NULL);
		}
	}

	return NULL;
}

void socket_close_all(void)
{
	struct _node *currNode = mainlist->next;

	while (currNode != NULL) {
		delete_list_node(currNode);
		currNode = mainlist->next;
	}
}

void atcmd_lwip_stop_tt_task(void)
{
	if (atcmd_lwip_tt_task != NULL) {
		rtos_task_delete(atcmd_lwip_tt_task);
		atcmd_lwip_tt_task = NULL;
	}

	if (atcmd_lwip_tt_sema != NULL) {
		rtos_sema_delete(atcmd_lwip_tt_sema);
		atcmd_lwip_tt_sema = NULL;
	}

	atcmd_lwip_tt_mode = FALSE;
}

int atcmd_lwip_receive_data(struct _node *curnode, u8 *buffer, u16 buffer_size, int *recv_size, u8_t *udp_clientaddr, u16_t *udp_clientport)
{
	struct timeval tv;
	fd_set readfds;
	int error_no = 0, ret = 0, size = 0;

	FD_ZERO(&readfds);
	FD_SET(curnode->sockfd, &readfds);
	tv.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	tv.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	ret = select(curnode->sockfd + 1, &readfds, NULL, NULL, &tv);
	if (!((ret > 0) && (FD_ISSET(curnode->sockfd, &readfds)))) {
#if ENABLE_TCPIP_SSL
		if (curnode->protocol == NODE_MODE_SSL) {
			ret = mbedtls_ssl_get_bytes_avail((mbedtls_ssl_context *)curnode->context);
			if (ret == 0) {
				/* RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] No receive event for con_id %d\r\n", curnode->con_id); */
				goto end;
			}
		} else
#endif
		{
			/* RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] No receive event for con_id %d\r\n", curnode->con_id); */
			goto end;
		}
	}

	/* udp server receive from client */
	if (curnode->protocol == NODE_MODE_UDP) {
		if (curnode->role == NODE_ROLE_SERVER) {
			struct sockaddr_in client_addr;
			u32_t addr_len = sizeof(struct sockaddr_in);
			memset((char *) &client_addr, 0, sizeof(client_addr));
			size = recvfrom(curnode->sockfd, buffer, buffer_size, 0, (struct sockaddr *) &client_addr, &addr_len);
			if (size == 0) {
				RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
			} else if (size < 0) {
				RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
				error_no = 4;
			}
			inet_ntoa_r(client_addr.sin_addr.s_addr, (char *)udp_clientaddr, 16);
			*udp_clientport = ntohs(client_addr.sin_port);
		} else {
			struct sockaddr_in serv_addr;
			u32_t addr_len = sizeof(struct sockaddr_in);
			memset((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(curnode->port);
			serv_addr.sin_addr.s_addr = htonl(curnode->addr);
			size = recvfrom(curnode->sockfd, buffer, buffer_size, 0, (struct sockaddr *) &serv_addr, &addr_len);
			if (size == 0) {
				RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
			} else if (size < 0) {
				RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
				error_no = 5;
			}
		}
	}

	else {
#if ENABLE_TCPIP_SSL
		if (curnode->protocol == NODE_MODE_SSL) {
			size = mbedtls_ssl_read((mbedtls_ssl_context *)curnode->context, buffer, buffer_size);
		} else
#endif
		{
			size = read(curnode->sockfd, buffer, buffer_size);
		}

		if (size == 0) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] Connection closed\r\n");
			atcmd_lwip_tt_mode = FALSE;
			error_no = 7;
		} else if (size < 0) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_receive_data] Failed receive %d\r\n", -size);
			error_no = 8;
		}
	}

end:
	if (error_no == 0) {
		*recv_size = size;
	} else {
#if ENABLE_TCPIP_SSL
		if (curnode->protocol == NODE_MODE_SSL) {
			mbedtls_net_context server_fd;
			server_fd.fd = curnode->sockfd;
			mbedtls_ssl_close_notify((mbedtls_ssl_context *)curnode->context);
			mbedtls_net_free(&server_fd);
			mbedtls_ssl_free((mbedtls_ssl_context *)curnode->context);
		} else
#endif
		{
			close(curnode->sockfd);
		}
		curnode->sockfd = INVALID_SOCKET_ID;
	}
	return error_no;
}

void atcmd_lwip_receive_task(void *param)
{
	int i = 0;
	int packet_size = ETH_MAX_MTU;

	node *curnode = NULL;
	int error_no = 0;
	int recv_size = 0;
	u8_t udp_clientaddr[16] = {0};
	u16_t udp_clientport = 0;

	UNUSED(param);

	if (NULL == rx_buffer) {
		rx_buffer = (u8 *)rtos_mem_zmalloc(packet_size);
		if (rx_buffer == NULL) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_receive_task] rx_buffer malloc fail\r\n");
			goto end;
		}
	}

	while (atcmd_lwip_is_autorecv_mode()) {
		for (i = 0; i < NUM_NS; i++) {
			curnode = tryget_list_node(i);
			if (curnode == NULL) {
				continue;
			}
#if ENABLE_TCPIP_SSL
			if ((curnode->protocol == NODE_MODE_TCP || curnode->protocol == NODE_MODE_SSL)
				&& (curnode->role == NODE_ROLE_SERVER))
#else
			if ((curnode->protocol == NODE_MODE_TCP) && (curnode->role == NODE_ROLE_SERVER))
#endif
			{
				continue;
			}

			error_no = atcmd_lwip_receive_data(curnode, rx_buffer, packet_size, &recv_size, udp_clientaddr, &udp_clientport);

			if (atcmd_lwip_tt_mode == TRUE) {
				if (error_no == 0 && recv_size > 0) {
					rx_buffer[recv_size] = '\0';
					at_printf("%s\r\n", rx_buffer);
					rtos_time_delay_ms(ATCMD_LWIP_TT_MAX_DELAY_TIME_MS);
				}
				continue;
			}

			if (error_no == 0) {
				if (recv_size > 0) {
					rx_buffer[recv_size] = '\0';
					if (curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER) {
						at_printf("%sOK,%d,%d,%s,%d:", "+SKTREAD:", recv_size, curnode->con_id, udp_clientaddr, udp_clientport);
					} else {
						at_printf("%sOK,%d,%d:", "+SKTREAD:", recv_size, curnode->con_id);
					}
					at_printf("%s", rx_buffer);
					at_printf("\r\n");
				}
			} else {
				at_printf("%sERROR:%d,%d\r\n", "+SKTREAD:", error_no, curnode->con_id);
			}
		}
	}

end:
	if (rx_buffer) {
		rtos_mem_free(rx_buffer);
		rx_buffer = NULL;
	}

	rtos_task_delete(NULL);
}

void atcmd_tt_wait_rx_complete(void)
{
	int curr = rtos_time_get_current_system_time_ms();

	while (curr - atcmd_lwip_tt_lasttickcnt < ATCMD_LWIP_TT_MAX_DELAY_TIME_MS) {
		rtos_time_delay_ms(5);
		curr = rtos_time_get_current_system_time_ms();
	}
}

int atcmd_lwip_start_autorecv_task(void)
{
	atcmd_lwip_set_autorecv_mode(TRUE);
	if (SUCCESS != rtos_task_create(NULL,
									"atcmd_lwip_receive_task",
									atcmd_lwip_receive_task,
									NULL,
									ATCP_STACK_SIZE,
									ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(NOTAG, "Create atcmd_lwip_receive_task failed.\r\n");
		atcmd_lwip_set_autorecv_mode(FALSE);
		return -1;
	}
	return 0;
}

int atcmd_lwip_send_data(struct _node *curnode, u8 *data, u16 data_sz, struct sockaddr_in cli_addr)
{
	int ret = 0, error_no = 0;

	/* UDP server. */
	if ((curnode->protocol == NODE_MODE_UDP) && (curnode->role == NODE_ROLE_SERVER)) {
		ret = sendto(curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
		if (ret < 0) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_send_data] Failed in sendto 1.\r\n");
			error_no = 5;
			goto end;
		}
	}

	/* UDP client, seed. */
	else if (curnode->protocol == NODE_MODE_UDP) {
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(curnode->port);
		serv_addr.sin_addr.s_addr = htonl(curnode->addr);
		ret = sendto(curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		if (ret < 0) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_send_data] Failed in sendto 1.\r\n");
			error_no = 6;
			goto end;
		}
	}

	/* TCP or SSL. */
#if ENABLE_TCPIP_SSL
	else if ((curnode->protocol == NODE_MODE_TCP) || (curnode->protocol == NODE_MODE_SSL))
#else
	else if (curnode->protocol == NODE_MODE_TCP)
#endif
	{
		if (curnode->role == NODE_ROLE_SERVER) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_send_data] In role server.\r\n");
			error_no = 7;
			goto end;
		}
#if ENABLE_TCPIP_SSL
		if (curnode->protocol == NODE_MODE_SSL) {
			ret = mbedtls_ssl_write((mbedtls_ssl_context *)curnode->context, data, data_sz);
		} else
#endif
		{
			ret = write(curnode->sockfd, data, data_sz);
		}
		if (ret < 0) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_send_data] Failed when writing.\r\n");
			error_no = 8;
			goto end;
		}
	}

	else {
		RTK_LOGI(NOTAG, "[atcmd_lwip_send_data] Error protocal %d.\r\n", curnode->protocol);
		error_no = 9;
		goto end;
	}

end:
	return error_no;
}

void atcmd_lwip_tt_handler(void *param)
{
	int tt_size = 0;
	struct sockaddr_in cli_addr;

	UNUSED(param);

	if (NULL == tx_buffer) {
		tx_buffer = (u8 *)rtos_mem_zmalloc(UART_LOG_CMD_BUFLEN);
		if (tx_buffer == NULL) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_tt_handler] tx_buffer malloc fail\r\n");
			goto end;
		}
	}

	while (rtos_sema_take(atcmd_lwip_tt_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
		atcmd_tt_wait_rx_complete();

		if (atcmd_lwip_tt_mode == FALSE) {
			RTK_LOGI(NOTAG, "[atcmd_lwip_tt_handler] No in TT mode any more\r\n");
			goto end;
		}

		rtos_critical_enter();
		if ((atcmd_lwip_tt_datasize >= 4) && (memcmp(atcmd_buf, "----", 4) == 0)) {
			atcmd_lwip_tt_mode = FALSE;
			atcmd_lwip_tt_datasize = 0;
			rtos_critical_exit();
			at_printf("\r\n#\r\n");
			goto end;
		}
		memcpy(tx_buffer, atcmd_buf, atcmd_lwip_tt_datasize);
		tt_size = atcmd_lwip_tt_datasize;
		atcmd_lwip_tt_datasize = 0;
		rtos_critical_exit();
		tx_buffer[tt_size] = '\0';
		atcmd_lwip_send_data(mainlist->next, tx_buffer, tt_size, cli_addr);
	}

end:
	if (tx_buffer) {
		rtos_mem_free(tx_buffer);
		tx_buffer = NULL;
	}
	rtos_sema_delete(atcmd_lwip_tt_sema);
	atcmd_lwip_tt_sema = NULL;
	atcmd_lwip_set_autorecv_mode(FALSE);
	rtos_task_delete(NULL);
}

static int atcmd_lwip_start_tt_task(void)
{
	int ret = 0, subret = 0;
	int enable = 1;
	node *n = mainlist->next;
#if LWIP_SO_SNDTIMEO
	int send_timeout = ATCMD_LWIP_TT_MAX_DELAY_TIME_MS;
#endif

	if (NODE_MODE_TCP == n->protocol) {
		subret = setsockopt(n->sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
		if (subret < 0) {
			RTK_LOGI(NOTAG, "Failed in setsockopt: TCP_NODELAY\r\n");
			ret = subret;
			goto end;
		}

#if LWIP_TCP_KEEPALIVE
		subret = setsockopt(n->sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
		if (subret < 0) {
			RTK_LOGI(NOTAG, "Failed in setsockopt: SO_KEEPALIVE\r\n");
			ret = subret;
			goto end;
		}
#endif
	}

#if LWIP_SO_SNDTIMEO
	subret = setsockopt(n->sockfd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(int));
	if (subret < 0) {
		RTK_LOGI(NOTAG, "Failed in setsockopt: SO_SNDTIMEO\r\n");
		ret = subret;
		goto end;
	}
#endif

	if (atcmd_lwip_tt_sema == NULL) {
		rtos_sema_create(&atcmd_lwip_tt_sema, 0, RTOS_SEMA_MAX_COUNT);
	}
	atcmd_lwip_tt_mode = TRUE;

	if (atcmd_lwip_tt_task == NULL) {
		if (SUCCESS != rtos_task_create(&atcmd_lwip_tt_task,
										"atcmd_lwip_tt_handler",
										atcmd_lwip_tt_handler,
										NULL,
										ATCP_STACK_SIZE,
										ATCMD_LWIP_TASK_PRIORITY)) {
			RTK_LOGI(NOTAG, "Failed to create atcmd_lwip_tt_handler\r\n");
			ret = -1;
			goto end;
		}
	}

	rtos_time_delay_ms(ATCMD_LWIP_TT_MAX_DELAY_TIME_MS);

	if (atcmd_lwip_tt_task != NULL) {
		if (atcmd_lwip_is_autorecv_mode() != 1) {
			subret = atcmd_lwip_start_autorecv_task();
			if (subret != 0) {
				ret = -1;
				rtos_task_delete(atcmd_lwip_tt_task);
				atcmd_lwip_tt_task = NULL;
				goto end;
			}
		}
	}


end:
	return ret;
}

#if ENABLE_TCPIP_AUTOLINK
void atcmd_update_partition_info(AT_PARTITION id, AT_PARTITION_OP ops, u8 *data, u16 len)
{
	/* !FIXME */
}

void atcmd_lwip_erase_info(void)
{
	atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_ERASE, NULL, 0);
}

void atcmd_lwip_write_info_to_flash(struct _atcmd_lwip_conn_info *cur_conn, int enable)
{
	struct _atcmd_lwip_conf read_data = {0};
	int i = 0, found = 0;

	atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_READ, (u8 *) &read_data, sizeof(struct atcmd_lwip_conf));

	/* fake that the conn exists already when disabling or there is no active conn on this moment */
	if (enable == 0) {
		atcmd_lwip_erase_info();
	}

	/* Enable. */
	else {
		if (read_data.conn_num < 0 || read_data.conn_num > ATCMD_LWIP_CONN_STORE_MAX_NUM) {
			read_data.conn_num = 0;
			read_data.last_index = -1;
		}
		for (i = 0; i < read_data.conn_num; i++) {
			if (0 == memcmp((u8 *)cur_conn, (u8 *)&read_data.conn[i], sizeof(struct _atcmd_lwip_conn_info))) {
				RTK_LOGI(NOTAG, "the same profile found in flash\r\n");
				found = 1;
				break;
			}
		}
		/* If not found. */
		if (found == 0) {
			read_data.last_index++;
			if (read_data.last_index >= ATCMD_LWIP_CONN_STORE_MAX_NUM) {
				read_data.last_index -= ATCMD_LWIP_CONN_STORE_MAX_NUM;
			}
			memcpy((u8 *)&read_data.conn[read_data.last_index], (u8 *)cur_conn, sizeof(struct _atcmd_lwip_conn_info));
			read_data.conn_num++;
			if (read_data.conn_num > ATCMD_LWIP_CONN_STORE_MAX_NUM) {
				read_data.conn_num = ATCMD_LWIP_CONN_STORE_MAX_NUM;
			}
		}
		if (found == 0 || read_data.enable != enable) {
			read_data.enable = enable;
			atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_WRITE, (u8 *) &read_data, sizeof(struct _atcmd_lwip_conf));
		}
	}
}
#endif

int hang_seednode(struct _node *main_node, struct _node *insert_node)
{
	struct _node *n = main_node;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	while (n->nextseed != NULL) {
		n = n->nextseed;
		if ((n->port == insert_node->port) && (n->addr == insert_node->addr)) {
			SYS_ARCH_UNPROTECT(lev);
			struct in_addr addr;
			addr.s_addr = htonl(insert_node->addr);
			RTK_LOGI(NOTAG, "This seed IP:%s PORT:%d already exist\r\n", inet_ntoa(addr), insert_node->port);
			return -1;
		}
	}

	n->nextseed = insert_node;
	SYS_ARCH_UNPROTECT(lev);
	return 0;
}

static void server_start(void *param)
{
	int s_mode = NODE_MODE_INVALID;
	int s_sockfd = INVALID_SOCKET_ID;
	int s_newsockfd = INVALID_SOCKET_ID;
	socklen_t s_client;
	struct sockaddr_in s_serv_addr, s_cli_addr;
	int s_local_port;
	int error_no = 0, ret = 0;
	int s_opt = 1;
#if ENABLE_TCPIP_SSL
	mbedtls_ssl_context *ssl = NULL;
	mbedtls_ssl_config *conf = NULL;
	mbedtls_net_context server_fd;
	mbedtls_x509_crt *server_x509 = NULL;
	mbedtls_pk_context *server_pk = NULL;
#endif
	struct _node *ServerNodeUsed = (struct _node *)param;

	if (ServerNodeUsed != NULL) {
		s_mode = ServerNodeUsed->protocol;
		s_local_port = ServerNodeUsed->port;
	}

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
#if ENABLE_TCPIP_SSL
	if (s_mode == NODE_MODE_SSL) {
		mbedtls_net_init(&server_fd);
		char *s_port_str = atcmd_lwip_itoa(s_local_port);
		if (s_port_str == NULL) {
			RTK_LOGI(NOTAG, "[server_start] Memory failure for s_port_str\r\n");
			error_no = 15;
			goto end;
		}
		ret = mbedtls_net_bind(&server_fd, NULL, s_port_str, MBEDTLS_NET_PROTO_TCP);
		if (ret != 0) {
			RTK_LOGI(NOTAG, "ERROR: net_bind %d\r\n", ret);
			error_no = 15;
			goto end;
		}
		s_sockfd = server_fd.fd;
		rtos_mem_free(s_port_str);
	} else
#endif
	{
		if (s_mode == NODE_MODE_UDP) {
			s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		} else if (s_mode == NODE_MODE_TCP) {
			s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		} else {
			RTK_LOGI(NOTAG, "Unknown connection type[%d]\r\n", s_mode);
		}

		if (s_sockfd == INVALID_SOCKET_ID) {
			RTK_LOGI(NOTAG, "ERROR opening socket\r\n");
			error_no = 5;
			goto end;
		}

		ret = setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&s_opt, sizeof(s_opt));
		if (ret < 0) {
			RTK_LOGI(NOTAG, "ERROR on setting socket option\r\n");
			close(s_sockfd);
			error_no = 6;
			goto end;
		}

		memset((char *)&s_serv_addr, 0, sizeof(s_serv_addr));
		s_serv_addr.sin_family = AF_INET;
		s_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		s_serv_addr.sin_port = htons(s_local_port);

		ret = bind(s_sockfd, (struct sockaddr *)&s_serv_addr, sizeof(s_serv_addr));
		if (ret < 0) {
			RTK_LOGI(NOTAG, "ERROR on binding\r\n");
			close(s_sockfd);
			error_no = 7;
			goto end;
		}
	}

	/***********************************************************
	* Assign IP address and socket fd to the node used for this server
	************************************************************/
	if (ServerNodeUsed != NULL) {
		uint8_t *ip = (uint8_t *)LwIP_GetIP(0);
		ServerNodeUsed->sockfd = s_sockfd;
		ServerNodeUsed->addr = ntohl(*((u32_t *)ip));
	}

#if ENABLE_TCPIP_SSL
	if (s_mode == NODE_MODE_SSL) {
		/***********************************************************
		*  SSL 1. Prepare the certificate and key for ssl server
		************************************************************/
		char *srv_crt = NULL;
		char *ca_list = NULL;
		char *srv_key = NULL;
		atcmd_ssl_srv_crt[ServerNodeUsed->con_id] = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
		if (atcmd_ssl_srv_crt[ServerNodeUsed->con_id] == NULL) {
			RTK_LOGI(NOTAG, "malloc fail for ssl server crt\r\n");
			error_no = 16;
			goto end;
		}
		atcmd_ssl_srv_key[ServerNodeUsed->con_id] = (mbedtls_pk_context *)rtos_mem_zmalloc(sizeof(mbedtls_pk_context));
		if (atcmd_ssl_srv_key[ServerNodeUsed->con_id] == NULL) {
			RTK_LOGI(NOTAG, "malloc fail for ssl server key\r\n");
			error_no = 17;
			goto end;
		}
		mbedtls_platform_set_calloc_free(atcmd_lwip_calloc, rtos_mem_free);
		server_x509 = atcmd_ssl_srv_crt[ServerNodeUsed->con_id];
		server_pk = atcmd_ssl_srv_key[ServerNodeUsed->con_id];

		mbedtls_x509_crt_init(server_x509);
		mbedtls_pk_init(server_pk);
		srv_crt = (atcmd_ssl_server_crt[ServerNodeUsed->con_id]) ? atcmd_ssl_server_crt[ServerNodeUsed->con_id] : (char *)mbedtls_test_srv_crt;
		ret = mbedtls_x509_crt_parse(server_x509, (const unsigned char *)srv_crt, strlen(srv_crt) + 1);
		if (ret != 0) {
			RTK_LOGI(NOTAG, "ERROR: x509_crt_parse server_x509/srv_crt %d\r\n", ret);
			error_no = 18;
			goto end;
		}
		ca_list = (atcmd_ssl_server_ca_list[ServerNodeUsed->con_id]) ? atcmd_ssl_server_ca_list[ServerNodeUsed->con_id] : (char *)mbedtls_test_cas_pem;
		ret = mbedtls_x509_crt_parse(server_x509, (const unsigned char *)ca_list, strlen(ca_list) + 1);
		if (ret != 0) {
			RTK_LOGI(NOTAG, "ERROR: x509_crt_parse server_x509/ca_list %d\r\n", ret);
			error_no = 19;
			goto end;
		}
		srv_key = (atcmd_ssl_server_key[ServerNodeUsed->con_id]) ? atcmd_ssl_server_key[ServerNodeUsed->con_id] : (char *)mbedtls_test_srv_key;
		ret = mbedtls_pk_parse_key(server_pk, (unsigned char *)srv_key, strlen(srv_key) + 1, NULL, 0);
		if (ret != 0) {
			RTK_LOGI(NOTAG, "ERROR: pk_parse_key server_pk %d\r\n", ret);
			error_no = 20;
			goto end;
		}
		/***********************************************************
		*  SSL 2. Hang node on mainlist for global management
		************************************************************/
		ret = hang_list_node(ServerNodeUsed);
		if (ret < 0) {
			RTK_LOGI(NOTAG, "ERROR: failed in hang_list_node\r\n");
			error_no = 21;
			goto end;
		} else {
			at_printf("\r\n%scon_id=%d\r\n", "+SKTSERVER:", ServerNodeUsed->con_id);
		}
		RTK_LOGI(NOTAG, "The SSL SERVER START OK!\r\n");
		/***********************************************************
		*  SSL 3. Waiting for ssl client to connect
		************************************************************/
		while (1) {
			//not using net_accept() here because it can't get client port in net_accept()
			if ((s_newsockfd = accept(s_sockfd, (struct sockaddr *) &s_cli_addr, &s_client)) < 0) {
				RTK_LOGI(NOTAG, "[ATPS] ERROR:ERROR on net_accept ret=%d\r\n", ret);
				error_no = 22;
				goto end;
			}
			RTK_LOGI(NOTAG, "An SSL/TLS client[%s:%d] is connecting\r\n", inet_ntoa(s_cli_addr.sin_addr), ntohs(s_cli_addr.sin_port));
			/***********************************************************
			*  SSL 4. Setup stuff for this ssl connection
			************************************************************/
			RTK_LOGI(NOTAG, "Setting up the SSL/TLS structure...\r\n");
			ssl = (mbedtls_ssl_context *)rtos_mem_malloc(sizeof(mbedtls_ssl_context));
			conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
			if ((ssl == NULL) || (conf == NULL)) {
				RTK_LOGI(NOTAG, "[ATPS] malloc fail for ssl client context!\r\n");
				error_no = 23;
				goto end;
			}

			mbedtls_ssl_init(ssl);
			mbedtls_ssl_config_init(conf);

			if ((ret = mbedtls_ssl_config_defaults(conf,
												   MBEDTLS_SSL_IS_SERVER,
												   MBEDTLS_SSL_TRANSPORT_STREAM,
												   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
				RTK_LOGI(NOTAG, "ERROR: ssl_config_defaults %d\r\n", ret);
				error_no = 24;
				rtos_mem_free((void *)ssl);
				rtos_mem_free((void *)conf);
				goto end;
			}

			mbedtls_ssl_conf_ca_chain(conf, server_x509->next, NULL);
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
			mbedtls_ssl_conf_rng(conf, atcmd_ssl_random, NULL);
			mbedtls_ssl_set_bio(ssl, &s_newsockfd, mbedtls_net_send, mbedtls_net_recv, NULL);
			ret = mbedtls_ssl_conf_own_cert(conf, server_x509, server_pk);
			if (ret != 0) {
				RTK_LOGI(NOTAG, "ERROR: ssl_set_own_cert %d\r\n", ret);
				error_no = 25;
				rtos_mem_free((void *)ssl);
				rtos_mem_free((void *)conf);
				goto end;
			}

			ret = mbedtls_ssl_setup(ssl, conf);
			if (ret != 0) {
				RTK_LOGI(NOTAG, "ERROR: ssl_setup %d\r\n", ret);
				error_no = 26;
				rtos_mem_free((void *)ssl);
				rtos_mem_free((void *)conf);
				goto end;
			}
			/***********************************************************
			*  SSL 5. Wait for the ssl handshake done
			************************************************************/
			RTK_LOGI(NOTAG, "Performing the SSL/TLS handshake...\r\n");
			ret = mbedtls_ssl_handshake(ssl);
			if (ret != 0) {
				RTK_LOGI(NOTAG, "ERROR: ssl_handshake -0x%x\r\n", -ret);
				rtos_mem_free((void *)ssl);
				rtos_mem_free((void *)conf);
				error_no = 27;
				goto end;
			}

			/***********************************************************
			*  SSL 6. Hang node on mainlist for global management
			************************************************************/
			struct _node *seednode = create_list_node(s_mode, NODE_ROLE_SEED);
			if (seednode == NULL) {
				RTK_LOGI(NOTAG, "[ATPS]create node failed!\r\n");
				rtos_mem_free((void *)ssl);
				error_no = 28;
				goto end;
			}
			seednode->sockfd = s_newsockfd;
			seednode->port = ntohs(s_cli_addr.sin_port);
			seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
			seednode->context = (void *)ssl;
			ret = hang_seednode(ServerNodeUsed, seednode);
			if (ret < 0) {
				delete_list_node(seednode);
				seednode = NULL;
			} else {
				RTK_LOGI(NOTAG, "The SSL/TLS client is connected\r\n");
				at_printf("\r\n%s A client connected to server[%d]\r\n"
						  "con_id:%d, seed, ssl, address:%s, port:%d, socket:%d\r\n",
						  "+SKTSERVER:",
						  ServerNodeUsed->con_id,
						  seednode->con_id,
						  inet_ntoa(s_cli_addr.sin_addr),
						  ntohs(s_cli_addr.sin_port),
						  seednode->sockfd);
			}
		}
	} else
#endif //ENABLE_TCPIP_SSL
	{
		if (s_mode == NODE_MODE_TCP) {//TCP MODE
			/***********************************************************
			*  TCP 1. Set this socket into listen mode
			************************************************************/
			ret = listen(s_sockfd, 5);
			if (ret < 0) {
				RTK_LOGI(NOTAG, "ERROR on listening\r\n");
				close(s_sockfd);
				error_no = 8;
				goto end;
			}
			/***********************************************************
			*  TCP 2. Hang node on mainlist for global management
			************************************************************/

			if (param != NULL) {
				ret = hang_list_node(ServerNodeUsed);
				if (ret < 0) {
					error_no = 9;
					goto end;
				} else {
					at_printf("\r\n+SKTSERVER:con_id=%d\r\n", ServerNodeUsed->con_id);
				}
			}

			RTK_LOGI(NOTAG, "The TCP SERVER START OK!\r\n");
			s_client = sizeof(s_cli_addr);
			/***********************************************************
			*  TCP 3. Waiting for TCP client to connect
			************************************************************/
			while (1) {
				s_newsockfd = accept(s_sockfd, (struct sockaddr *) &s_cli_addr, &s_client);
				if (s_newsockfd < 0) {
					if (param != NULL) {
						RTK_LOGI(NOTAG, "[AT+SKTSERVER] ERROR on accept\r\n");
					}
					error_no = 10;
					goto end;
				} else {
					/***********************************************************
					*  TCP 4. Hang node on mainlist for global management of this TCP connection
					************************************************************/
					if (param != NULL) {
						struct _node *seednode = create_list_node(s_mode, NODE_ROLE_SEED);
						if (seednode == NULL) {
							RTK_LOGI(NOTAG, "[AT+SKTSERVER]create node failed!\r\n");
							error_no = 11;
							goto end;
						}
						seednode->sockfd = s_newsockfd;
						seednode->port = ntohs(s_cli_addr.sin_port);
						seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
						ret = hang_seednode(ServerNodeUsed, seednode);
						if (ret < 0) {
							delete_list_node(seednode);
							seednode = NULL;
						} else {
							at_printf("\r\n%s A client connected to server[%d]\r\n"
									  "con_id:%d, seed, tcp, address:%s, port:%d, socket:%d\r\n",
									  "+SKTSERVER:",
									  ServerNodeUsed->con_id,
									  seednode->con_id,
									  inet_ntoa(s_cli_addr.sin_addr.s_addr),
									  ntohs(s_cli_addr.sin_port),
									  seednode->sockfd);
						}
					}
				}
			}
		} else {
			/***********************************************************
			*  UDP 1. Enable broadcast on this socket
			************************************************************/
#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
			int so_broadcast = 1;
			ret = setsockopt(s_sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,  sizeof(so_broadcast));
			if (ret < 0) {
				error_no = 14;
				goto end;
			}
#endif
			/***********************************************************
			*  UDP 2. Hang node on mainlist for global management
			************************************************************/
			if (ServerNodeUsed != NULL) {
				ret = hang_list_node(ServerNodeUsed);
				if (ret < 0) {
					error_no = 12;
					goto end;
				}
				at_printf("\r\n+SKTSERVER:con_id=%d\r\n", ServerNodeUsed->con_id);
				//task will exit itself
				ServerNodeUsed->handletask = NULL;
			}
			RTK_LOGI(NOTAG, "The UDP SERVER START OK!\r\n");
		}
	}

end:
	if (error_no != 0) {
		if (ServerNodeUsed != NULL) {
			//task will exit itself if getting here
			ServerNodeUsed->handletask = NULL;
			delete_list_node(ServerNodeUsed);
		}
		at_printf("\r\n%sERROR:%d\r\n", "+SKTSERVER:", error_no);
	}
}

static void server_start_task(void *param)
{
	rtos_time_delay_ms(1000);

	if (param != NULL) {
		server_start(param);
		rtos_task_delete(NULL);
		return;
	}

	rtos_task_delete(NULL);
}

/* Create a socket server. */
void at_sktserver(void *arg)
{
	const int mode_idx = 1, port_idx = 2;
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	struct _node *servernode = NULL;
	int mode = 0;
	int local_port = 0;
	int error_no = 0;
	u32 server_task_stksz = ATCP_STACK_SIZE;

	if (atcmd_lwip_tt_mode == TRUE) {
		RTK_LOGW(NOTAG, "[+SKTSERVER] The mode is not correct\r\n");
		error_no = 13;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGW(NOTAG, "[+SKTSERVER] The parameter ERROR\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[mode_idx]) == 0) {
		RTK_LOGW(NOTAG, "[+SKTSERVER] missing mode\r\n");
		error_no = 1;
		goto end;
	}
	mode = atoi(argv[mode_idx]);
#if ENABLE_TCPIP_SSL
	if ((mode < NODE_MODE_TCP) || (mode > NODE_MODE_SSL))
#else
	if ((mode < NODE_MODE_TCP) || (mode > NODE_MODE_UDP))
#endif
	{
		RTK_LOGW(NOTAG, "[+SKTSERVER] Unknown connection type\r\n");
		error_no = 14;
		goto end;
	}

	if (strlen(argv[port_idx]) == 0) {
		RTK_LOGW(NOTAG, "[+SKTSERVER] missing port\r\n");
		error_no = 2;
		goto end;
	}
	local_port = atoi(argv[port_idx]);
	if ((local_port <= 0) || (local_port > 65535)) {
		RTK_LOGW(NOTAG, "[+SKTSERVER] Invalid local port\r\n");
		error_no = 2;
		goto end;
	}

	servernode = create_list_node(mode, NODE_ROLE_SERVER);
	if (servernode == NULL) {
		error_no = 3;
		goto end;
	}
	servernode->port = local_port;

	/* Create task */
	if (SUCCESS != rtos_task_create(&servernode->handletask,
									"server_start_task",
									server_start_task,
									servernode,
									server_task_stksz,
									ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(NOTAG, "\r\n[SKTSERVER] Create task failed.\r\n");
		error_no = 4;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTSERVER:");
	} else {
		delete_list_node(servernode);
		at_printf("\r\n%sERROR:%d\r\n", "+SKTSERVER:", error_no);
	}
}

static void client_start(void *param)
{
	int c_mode = NODE_MODE_INVALID;
	int c_remote_port = 0;
	char c_remote_addr[16];
	int c_sockfd = INVALID_SOCKET_ID;
	struct sockaddr_in c_serv_addr;
	int error_no = 0, ret = 0;
#if ENABLE_TCPIP_SSL
	mbedtls_ssl_context *ssl = NULL;
	mbedtls_ssl_config *conf = NULL;
	mbedtls_net_context server_fd;
#endif
	struct in_addr c_addr;
	struct _node *ClientNodeUsed = (struct _node *)param;

	if (ClientNodeUsed != NULL) {
		c_mode = ClientNodeUsed->protocol;
		c_remote_port = ClientNodeUsed->port;
		c_addr.s_addr = htonl(ClientNodeUsed->addr);
		if (inet_ntoa_r(c_addr, c_remote_addr, sizeof(c_remote_addr)) == NULL) {
			RTK_LOGW(NOTAG, "[client_start] Error address\r\n");
			error_no = 6;
			goto end;
		}
	}

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
#if ENABLE_TCPIP_SSL
	if (c_mode == NODE_MODE_SSL) {
		mbedtls_net_init(&server_fd);
		char *c_port_str = atcmd_lwip_itoa(c_remote_port);
		if (c_port_str == NULL) {
			RTK_LOGI(NOTAG, "[server_start] Memory failure for c_port_str\r\n");
			error_no = 18;
			goto end;
		}
		ret = mbedtls_net_connect(&server_fd, c_remote_addr, c_port_str, MBEDTLS_NET_PROTO_TCP);
		if (ret != 0) {
			RTK_LOGW(NOTAG, "[client_start] Failed in mbedtls_net_connect\r\n");
			error_no = 18;
			rtos_mem_free(c_port_str);
			goto end;
		}
		c_sockfd = server_fd.fd;
		rtos_mem_free(c_port_str);
	} else
#endif
	{
		if (c_mode == NODE_MODE_UDP) {
			c_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		} else if (c_mode == NODE_MODE_TCP) {
			c_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		} else {
			RTK_LOGI(NOTAG, "Unknown connection type[%d]\r\n", c_mode);
		}
		if (c_sockfd == INVALID_SOCKET_ID) {
			RTK_LOGW(NOTAG, "[client_start] Failed to create socket!\r\n");
			error_no = 7;
			goto end;
		}
		memset(&c_serv_addr, 0, sizeof(c_serv_addr));
		c_serv_addr.sin_family = AF_INET;
		c_serv_addr.sin_addr.s_addr = inet_addr(c_remote_addr);
		c_serv_addr.sin_port = htons(c_remote_port);
	}

	/***********************************************************
	* Assign socket fd to the node used for this client
	************************************************************/
	if (ClientNodeUsed != NULL) {
		ClientNodeUsed->sockfd = c_sockfd;
	}

#if ENABLE_TCPIP_SSL
	if (c_mode == NODE_MODE_SSL) {
		/***********************************************************
		*  SSL 1. Setup stuff for this ssl connection
		************************************************************/
		int retry_count = 0;
		mbedtls_platform_set_calloc_free(atcmd_lwip_calloc, rtos_mem_free);
		ssl = (mbedtls_ssl_context *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_context));
		if (ssl == NULL) {
			RTK_LOGW(NOTAG, "[client_start] Memory allocation failed for ssl\r\n");
			error_no = 19;
			goto end;
		}
		conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
		if (conf == NULL) {
			RTK_LOGW(NOTAG, "[client_start] Memory allocation failed for conf\r\n");
			error_no = 19;
			goto end;
		}
		ClientNodeUsed->context = (void *)ssl;

		mbedtls_ssl_init(ssl);
		mbedtls_ssl_config_init(conf);

		ret = mbedtls_ssl_config_defaults(conf,
										  MBEDTLS_SSL_IS_CLIENT,
										  MBEDTLS_SSL_TRANSPORT_STREAM,
										  MBEDTLS_SSL_PRESET_DEFAULT);
		if (ret != 0) {
			RTK_LOGW(NOTAG, "[client_start] Failed for mbedtls_ssl_config_defaults\r\n");
			error_no = 20;
			goto end;
		}

		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(conf, atcmd_ssl_random, NULL);
		mbedtls_ssl_set_bio(ssl, &server_fd/*&ClientNodeUsed->sockfd*/, mbedtls_net_send, mbedtls_net_recv, NULL);
		mbedtls_ssl_conf_dbg(conf, atcmd_ssl_debug, NULL);

		ret = mbedtls_ssl_setup(ssl, conf);
		if (ret != 0) {
			RTK_LOGW(NOTAG, "[client_start] Failed for mbedtls_ssl_setup\r\n");
			error_no = 21;
			goto end;
		}

#ifdef MBEDTLS_DEBUG_C
		mbedtls_debug_set_threshold(ATCMD_SSL_DEBUG_LEVEL);
#endif

		/***********************************************************
		*  SSL 2. Wait for the ssl handshake done
		************************************************************/
		RTK_LOGI(NOTAG, "[client_start] Performing the SSL, TLS handshake...\r\n");
		while ((ret = mbedtls_ssl_handshake(ssl)) != 0) {
			if (retry_count >= 5) {
				RTK_LOGI(NOTAG, "ssl_handshake failed -0x%x\r\n", -ret);
				error_no = 22;
				goto end;
			}
			retry_count++;
		}
		RTK_LOGI(NOTAG, "Connect to Server successful!\r\n");

		/***********************************************************
		*  SSL 3. Hang node on mainlist for global management
		************************************************************/
		ret = hang_list_node(ClientNodeUsed);
		if (ret < 0) {
			RTK_LOGW(NOTAG, "[client_start] Hang node failed for SSL client\r\n");
			error_no = 22;
			goto end;
		}
		at_printf("%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
	} else
#endif
	{
		/* TCP mode. */
		if (c_mode == NODE_MODE_TCP) {
			/***********************************************************
			*  TCP 1. Connect a netconn to a specific remote IP address and port
			************************************************************/
			ret = connect(c_sockfd, (struct sockaddr *)&c_serv_addr,  sizeof(c_serv_addr));
			if (ret == 0) {
				RTK_LOGI(NOTAG, "Connect to Server successful!\r\n");
				/***********************************************************
				*  TCP 2. Hand node on mainlist for global management if connect success
				************************************************************/
				if (ClientNodeUsed != NULL) {
					ret = hang_list_node(ClientNodeUsed);
					if (ret < 0) {
						RTK_LOGW(NOTAG, "Failed in hang_list_node\r\n");
						error_no = 8;
						close(c_sockfd);
						goto end;
					}
					at_printf("%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
				}
			} else {
				/***********************************************************
				*  TCP 2. Free node if connect fail
				************************************************************/
				RTK_LOGW(NOTAG, "Connect to Server failed!\r\n");
				error_no = 9;
				close(c_sockfd);
				goto end;
			}
		}

		/* UDP mode. */
		else {
			if (ClientNodeUsed != NULL) {
#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
				if ((c_serv_addr.sin_addr.s_addr == htonl(INADDR_BROADCAST)) ||
					(c_serv_addr.sin_addr.s_addr == htonl(INADDR_ANY))) {
					int so_broadcast = 1;
					ret = setsockopt(c_sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
					if (ret < 0) {
						RTK_LOGW(NOTAG, "Failed in setsockopt!\r\n");
						error_no = 14;
						goto end;
					}
				}
#endif
#if LWIP_IGMP
				ip_addr_t dst_addr;
				ip_2_ip4(&dst_addr)->addr = c_serv_addr.sin_addr.s_addr;
				if (ip_addr_ismulticast(&dst_addr)) {
					struct ip_mreq imr;
					struct in_addr intfAddr;
					/* Set NETIF_FLAG_IGMP flag for netif which should process IGMP messages */
					xnetif[0].flags |= NETIF_FLAG_IGMP;
					imr.imr_multiaddr.s_addr = c_serv_addr.sin_addr.s_addr;
					imr.imr_interface.s_addr = INADDR_ANY;
					ret = setsockopt(c_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
					if (ret < 0) {
						RTK_LOGW(NOTAG, "Failed in setsockopt!\r\n");
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 15;
						goto end;
					}
					intfAddr.s_addr = INADDR_ANY;
					ret = setsockopt(c_sockfd, IPPROTO_IP, IP_MULTICAST_IF, &intfAddr, sizeof(struct in_addr));
					if (ret < 0) {
						RTK_LOGW(NOTAG, "Failed in setsockopt!\r\n");
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 16;
						goto end;
					}
				}
#endif
				if (ClientNodeUsed->local_port) {
					struct sockaddr_in addr;
					memset(&addr, 0, sizeof(addr));
					addr.sin_family = AF_INET;
					addr.sin_port = htons(ClientNodeUsed->local_port);
					addr.sin_addr.s_addr = htonl(INADDR_ANY);
					ret = bind(ClientNodeUsed->sockfd, (struct sockaddr *)&addr, sizeof(addr));
					if (ret < 0) {
						RTK_LOGW(NOTAG, "bind sock error!\r\n");
						error_no = 12;
						goto end;
					}
				}
				ret = hang_list_node(ClientNodeUsed);
				if (ret < 0) {
					RTK_LOGW(NOTAG, "Failed in hang_list_node!\r\n");
					error_no = 10;
					goto end;
				}
				at_printf("%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
			}
		}
	}

end:
	if (error_no != 0) {
#if ENABLE_TCPIP_SSL
		rtos_mem_free(ssl);
		rtos_mem_free(conf);
#endif
		delete_list_node(ClientNodeUsed);
		at_printf("\r\n%sERROR:%d\r\n", "+SKTCLIENT:", error_no);
	}
}

static void client_start_task(void *param)
{
	rtos_time_delay_ms(1000);

	if (param != NULL) {
		client_start(param);
	} else {
		RTK_LOGW(NOTAG, "client_start_task(): param is NULL!\r\n");
	}

	rtos_task_delete(NULL);
	return;
}

/* Create a socket client. */
void at_sktclient(void *arg)
{
	const int mode_idx = 1, addr_idx = 2, rmt_port_idx = 3, lcl_port_idx = 4;
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	struct _node *clientnode = NULL;
	int mode = 0;
	int remote_port;
	int local_port = 0;
	struct in_addr addr;
	int error_no = 0;
	u32 client_task_stksz = ATCP_STACK_SIZE;
#if LWIP_DNS
	struct hostent *server_host = NULL;
#endif

	if (atcmd_lwip_tt_mode == TRUE && mainlist->next != NULL) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Only one client connection can be created in TT mode\r\n");
		error_no = 13;
		goto end;
	}

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 4 || argc > 5) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	/* tcp / udp / ssl. */
	if (strlen(argv[mode_idx]) == 0) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] missing mode\r\n");
		error_no = 17;
		goto end;
	}
	mode = atoi(argv[mode_idx]);
#if ENABLE_TCPIP_SSL
	if (mode < NODE_MODE_TCP || mode > NODE_MODE_SSL)
#else
	if (mode < NODE_MODE_TCP || mode > NODE_MODE_UDP)
#endif
	{
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Unknown connection type\r\n");
		error_no = 17;
		goto end;
	}

	if (strlen(argv[rmt_port_idx]) == 0) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] missing remote port\r\n");
		error_no = 3;
		goto end;
	}
	remote_port = atoi(argv[rmt_port_idx]);
	if (remote_port < 0 || remote_port > 65535) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Invalid port\r\n");
		error_no = 3;
		goto end;
	}

	if (inet_aton(argv[addr_idx], &addr) == 0) {
#if LWIP_DNS
		server_host = gethostbyname(argv[addr_idx]);
		if (server_host != NULL) {
			memcpy(&addr, server_host->h_addr, 4);
			RTK_LOGI(NOTAG, "[+SKTCLIENT] Found %s = %s\r\n", argv[addr_idx], inet_ntoa(addr));
		} else
#endif
		{
			RTK_LOGW(NOTAG, "[+SKTCLIENT] Host '%s' Not found\r\n", argv[addr_idx]);
			error_no = 2;
			goto end;
		}
	}

	if (argc > lcl_port_idx && strlen(argv[lcl_port_idx]) != 0) {
		local_port = atoi(argv[lcl_port_idx]);
		if (local_port <= 0 || local_port > 65535) {
			RTK_LOGW(NOTAG, "[+SKTCLIENT] Invalid local port\r\n");
			error_no = 11;
			goto end;
		}
	}

	clientnode = create_list_node(mode, NODE_ROLE_CLIENT);
	if (clientnode == NULL) {
		RTK_LOGW(NOTAG, "[+SKTCLIENT] Create clientnode failed\r\n");
		error_no = 4;
		goto end;
	}
	clientnode->port = remote_port;
	clientnode->addr = ntohl(addr.s_addr);
	clientnode->local_port = local_port;

	if (SUCCESS != rtos_task_create(NULL,
									"client_start_task",
									client_start_task,
									clientnode,
									client_task_stksz,
									ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(NOTAG, "\r\n[SKTSERVER] Create task failed.\r\n");
		error_no = 5;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTCLIENT:");
	} else {
		delete_list_node(clientnode);
		at_printf("\r\n%sERROR:%d\r\n", "+SKTCLIENT:", error_no);
	}
}

/* Delete a (all) socket server(s) / client(s). */
void at_sktdel(void *arg)
{
	int con_id = INVALID_CON_ID;
	int error_no = 0;
	struct _node *s_node = NULL;

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[SKTDEL] Create task failed.\r\n");
		error_no = 1;
		goto end;
	}

	con_id = atoi(arg);
	/* 0 - Close all; others - Close specific id. */
	if (con_id == 0) {
		if (atcmd_lwip_is_autorecv_mode()) {
			atcmd_lwip_set_autorecv_mode(FALSE);
		}
		socket_close_all();
	} else {
		s_node = seek_list_node(con_id);
		if (s_node != NULL) {
			delete_list_node(s_node);
		}
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTDEL:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTDEL:", error_no);
	}
}

/* transparent transmission mode. */
void at_skttt(void *arg)
{
	int argc;
	int error_no = 0;
	int enable = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[SKTTT] Error parameter.\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGI(NOTAG, "[SKTTT] Error parameter number.\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(NOTAG, "[SKTTT] Missing enable\r\n");
		error_no = 1;
		goto end;
	}

	enable = atoi(argv[1]);

	if (enable != 1) {
		RTK_LOGI(NOTAG, "[SKTTT] Error parameter.\r\n");
		error_no = 1;
		goto end;
	}

	if (mainlist->next == NULL) {
		RTK_LOGI(NOTAG, "[SKTTT] No connection found.\r\n");
		error_no = 2;
		goto end;
	} else if (mainlist->next->role == NODE_ROLE_SERVER) {
		RTK_LOGI(NOTAG, "[SKTTT] Cannot enter TT mode for server.\r\n");
		error_no = 3;
		goto end;
	} else if (mainlist->next->next || mainlist->next->nextseed) {
		RTK_LOGI(NOTAG, "[SKTTT] More than one connection found.\r\n");
		error_no = 4;
		goto end;
	} else {
		if (atcmd_lwip_start_tt_task() != 0) {
			RTK_LOGI(NOTAG, "[SKTTT] Start TT task failed.\r\n");
			error_no = 5;
			goto end;
		}
	}


end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTTT:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTTT:", error_no);
	}
}

void at_sktsend(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	int con_id = INVALID_CON_ID;
	node *curnode = NULL;
	struct sockaddr_in cli_addr;
	int data_sz;
	int data_pos = 0;
	u8 colon_found = 0;  //1 if colon found in argv[argc-1]

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[at_sktsend] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc != 3) && (argc != 5)) {
		RTK_LOGI(NOTAG, "[at_sktsend] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	data_sz = atoi((char *)argv[1]);
	if (data_sz > UART_LOG_CMD_BUFLEN) {
		RTK_LOGI(NOTAG, "[at_sktsend] data_sz is invalid\r\n");
		error_no = 2;
		goto end;
	}

	con_id = atoi((char *)argv[2]);
	curnode = seek_list_node(con_id);
	if (curnode == NULL) {
		RTK_LOGI(NOTAG, "[at_sktsend] <con_id> is not found\r\n");
		error_no = 3;
		goto end;
	}

	while (argv[argc - 1][data_pos]) {
		if (argv[argc - 1][data_pos] == ':') {
			colon_found = 1;
			break;
		}
		data_pos++;
	}
	if (0 == colon_found) {
		RTK_LOGI(NOTAG, "[at_sktsend] Incorrect parameter format: missing colon before data\r\n");
		error_no = 1;
		goto end;
	}

	if (data_sz != (int)strlen(&argv[argc - 1][data_pos + 1])) {
		RTK_LOGI(NOTAG, "[at_sktsend] <data_size> does not match the actual <data> length\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol == NODE_MODE_UDP)
		&& (curnode->role == NODE_ROLE_SERVER)) {
		if (argc != 5) {
			RTK_LOGI(NOTAG, "[at_sktsend] Must contain address and port\r\n");
			error_no = 1;
			goto end;
		}
		char udp_clientaddr[17];
		memset(udp_clientaddr, 0, sizeof(udp_clientaddr));
		strncpy((char *)udp_clientaddr, (char *)argv[3], sizeof(udp_clientaddr) - 1);
		cli_addr.sin_family = AF_INET;
		cli_addr.sin_port = htons(atoi((char *)argv[4]));
		ret = inet_aton(udp_clientaddr, &cli_addr.sin_addr);
		if (ret == 0) {
			RTK_LOGI(NOTAG, "[at_sktsend] Failed in inet_aton\r\n");
			error_no = 4;
			goto end;
		}
	}

	error_no = atcmd_lwip_send_data(curnode, (u8 *)&argv[argc - 1][data_pos + 1], data_sz, cli_addr);

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTSEND:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTSEND:", error_no);
	}
}

void at_sktread(void *arg)
{
	int argc = 0, error_no = 0, recv_size = 0, packet_size = 0;
	char *argv[MAX_ARGC] = {0};
	int con_id = INVALID_CON_ID;
	node *curnode = NULL;
	u8_t udp_clientaddr[16] = {0};
	u16_t udp_clientport = 0;

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[at_sktread] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGI(NOTAG, "[at_sktread] Invalid parameter number\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(NOTAG, "[at_sktread] missing con_id\r\n");
		error_no = 9;
		goto end;
	}
	con_id = atoi((char *)argv[1]);
	if (con_id <= 0 || con_id > NUM_NS) {
		RTK_LOGI(NOTAG, "[at_sktread] Invalid con_id\r\n");
		error_no = 9;
		goto end;
	}

	if (strlen(argv[2]) == 0) {
		RTK_LOGI(NOTAG, "[at_sktread] missing packet_size\r\n");
		error_no = 2;
		goto end;
	}
	packet_size = atoi((char *)argv[2]);
	if (packet_size <= 0 || packet_size > UART_LOG_CMD_BUFLEN) {
		RTK_LOGI(NOTAG, "[at_sktread] Invalid packet_size\r\n");
		error_no = 2;
		goto end;
	}

	curnode = seek_list_node(con_id);
	if (curnode == NULL) {
		RTK_LOGI(NOTAG, "[at_sktread] No matched con_id\r\n");
		error_no = 3;
		goto end;
	}

	if (curnode->protocol == NODE_MODE_TCP && curnode->role == NODE_ROLE_SERVER) {
		RTK_LOGI(NOTAG, "[at_sktread] TCP Server must receive data from the seed\r\n");
		error_no = 6;
		goto end;
	}

	if (atcmd_lwip_is_autorecv_mode()) {
		RTK_LOGI(NOTAG, "[at_sktread] Command not permitted in auto receive mode \r\n");
		error_no = 11;
		at_printf("\r\n%sERROR:%d\r\n", "+SKTREAD:", error_no);
		return;
	}

	if (NULL == rx_buffer) {
		rx_buffer = (u8 *)rtos_mem_zmalloc(packet_size + 1);
		if (rx_buffer == NULL) {
			RTK_LOGI(NOTAG, "[at_sktread] rx_buffer malloc fail\r\n");
			error_no = 10;
			goto end;
		}
	}

	error_no = atcmd_lwip_receive_data(curnode, rx_buffer, (u16)packet_size, &recv_size, udp_clientaddr, &udp_clientport);

end:
	if (error_no == 0) {
		if (curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER) {
			at_printf("%sOK,%d,%d,%s,%d:", "+SKTREAD:", recv_size, con_id, udp_clientaddr, udp_clientport);
		} else {
			at_printf("%sOK,%d,%d:", "+SKTREAD:", recv_size, con_id);
		}
		if (recv_size > 0) {
			at_printf("%s", rx_buffer);
		}
		at_printf("\r\n");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTREAD:", error_no);
	}

	if (rx_buffer) {
		rtos_mem_free(rx_buffer);
		rx_buffer = NULL;
	}
}

void at_sktrecvcfg(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	int enable = 0;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[at_sktrecvcfg] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGI(NOTAG, "[at_sktrecvcfg] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(NOTAG, "[at_sktrecvcfg] missing enable\r\n");
		error_no = 1;
		goto end;
	}
	enable = atoi((char *)argv[1]);
	/* Start autorecv. */
	if (enable == 1) {
		if (atcmd_lwip_is_autorecv_mode()) {
			RTK_LOGI(NOTAG, "[at_sktrecvcfg] Already in autorecv mode\r\n");
		} else {
			ret = atcmd_lwip_start_autorecv_task();
			if (ret != 0) {
				RTK_LOGI(NOTAG, "[at_sktrecvcfg] Failed in atcmd_lwip_start_autorecv_task\r\n");
				error_no = 2;
				goto end;
			}
		}
	}
	/* Stop autorecv. */
	else if (enable == 0) {
		if (atcmd_lwip_is_autorecv_mode()) {
			atcmd_lwip_set_autorecv_mode(FALSE);
		} else {
			RTK_LOGI(NOTAG, "[at_sktrecvcfg] Already leaving autorecv mode\r\n");
		}
	}
	/* Invalid input. */
	else {
		RTK_LOGI(NOTAG, "[at_sktrecvcfg] Invalid enable value\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTRECVCFG:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTRECVCFG:", error_no);
	}
}

void at_sktstate(void *arg)
{
	struct _node *n = mainlist->next;
	struct in_addr addr;

	UNUSED(arg);

	while (n != NULL) {
		if (n->con_id != INVALID_CON_ID) {
			at_printf("con_id:%d,", n->con_id);
			/* Server or client. */
			if (n->role == NODE_ROLE_SERVER) {
				at_printf("server,");
			} else {
				at_printf("client,");
			}
			/* Role type. */
			if (n->protocol == NODE_MODE_TCP) {
				at_printf("tcp,");
			} else if (n->protocol == NODE_MODE_UDP) {
				at_printf("udp,");
			}
#if ENABLE_TCPIP_SSL
			else { /* if (n->protocol == NODE_MODE_SSL) */
				at_printf("ssl,");
			}
#endif
			/* Address. */
			addr.s_addr = htonl(n->addr);
			at_printf("address:%s,port:%d,socket:%d\r\n", inet_ntoa(addr), n->port, n->sockfd);
			/* Seed. */
			if (n->nextseed != NULL) {
				struct _node *seed = n;
				do {
					seed = seed->nextseed;
					at_printf("%scon_id:%d,seed,", "+SKTSTATE:", seed->con_id);
					if (seed->protocol == NODE_MODE_TCP) {
						at_printf("tcp,");
					} else if (n->protocol == NODE_MODE_UDP) {
						at_printf("udp,");
					}
#if ENABLE_TCPIP_SSL
					else { /* if(n->protocol == NODE_MODE_SSL) */
						at_printf("ssl,");
					}
#endif
					addr.s_addr = htonl(seed->addr);
					at_printf("address:%s,port:%d,socket:%d\r\n", inet_ntoa(addr), seed->port, seed->sockfd);
				} while (seed->nextseed != NULL);
			}
		}
		n = n->next;
	}

	at_printf("\r\n%sOK\r\n", "+SKTSTATE:");
}

#if ENABLE_TCPIP_AUTOLINK
void at_sktautolink(void *arg)
{
	int argc, error_no = 0, enable = 0;
	char *argv[MAX_ARGC] = {0};
	struct _atcmd_lwip_conn_info cur_conn = {0};
	struct _node *cur_node = mainlist->next;

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[at_sktautolink] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2 || strlen(argv[1]) == 0) {
		RTK_LOGI(NOTAG, "[at_sktautolink] Invalid parameter number\r\n");
		error_no = 2;
		goto end;
	}

	if (enable != 0 && cur_node == NULL) {
		RTK_LOGI(NOTAG, "[at_sktautolink] node should not be NULL when enable\r\n");
		error_no = 3;
		goto end;
	}

	cur_conn.role = cur_node->role;
	cur_conn.protocol = cur_node->protocol;
	cur_conn.remote_addr = cur_node->addr;
	cur_conn.remote_port = cur_node->port;
	cur_conn.local_addr = cur_node->local_addr;
	cur_conn.local_port = cur_node->local_port;
	atcmd_lwip_write_info_to_flash(&cur_conn, enable);

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+SKTAUTOLINK:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+SKTAUTOLINK:", error_no);
	}
}
#endif

void at_ipdomain(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	struct in_addr addr;
	struct hostent *host_entry = NULL;
	char **addr_list;

	if (arg == NULL) {
		RTK_LOGI(NOTAG, "[at_ipdomain] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGW(NOTAG, "[at_ipdomain] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	memset(&addr, 0, sizeof(struct in_addr));

	host_entry = gethostbyname(argv[1]);
	if (host_entry != NULL) {
		for (addr_list = host_entry->h_addr_list; *addr_list != NULL; addr_list++) {
			memcpy(&addr, *addr_list, sizeof(struct in_addr));
			at_printf("\r\n+IPDOMAIN:%s\r\n", inet_ntoa(addr));
		}
	} else {
		RTK_LOGW(NOTAG, "[at_ipdomain] Domain Name '%s' Not be resolved\r\n", argv[1]);
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf("\r\n%sOK\r\n", "+IPDOMAIN:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+IPDOMAIN:", error_no);
	}
}

log_item_t at_lwip_items[ ] = {
	{"+SKTSERVER", at_sktserver, {NULL, NULL}},
	{"+SKTCLIENT", at_sktclient, {NULL, NULL}},
	{"+SKTDEL", at_sktdel, {NULL, NULL}},
	{"+SKTTT", at_skttt, {NULL, NULL}},
	{"+SKTSEND", at_sktsend, {NULL, NULL}},
	{"+SKTREAD", at_sktread, {NULL, NULL}},
	{"+SKTRECVCFG", at_sktrecvcfg, {NULL, NULL}},
	{"+SKTSTATE", at_sktstate, {NULL, NULL}},
#if ENABLE_TCPIP_AUTOLINK
	{"+SKTAUTOLINK", at_sktautolink, {NULL, NULL}},
#endif
	{"+IPDOMAIN", at_ipdomain, {NULL, NULL}},
};

void print_lwip_at(void)
{
	int i, cmdSize;

	cmdSize = sizeof(at_lwip_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_lwip_items[i].log_cmd);
	}
}

void at_tcpip_init(void)
{
	init_node_pool();
	mainlist = create_list_node(-1, -1);

	atcmd_service_add_table(at_lwip_items, sizeof(at_lwip_items) / sizeof(at_lwip_items[0]));
}

#endif /* CONFIG_LWIP_LAYER */
