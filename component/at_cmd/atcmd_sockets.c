/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_SOCKET) && (CONFIG_ATCMD_SOCKET == 1)

#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_sockets.h"


static const char *const AT_SOCKET_TAG = "AT_SKT";

static struct _node node_pool[NUM_NS];
static struct _node *mainlist = NULL;
static int atcmd_lwip_auto_recv = FALSE;


static u8 *rx_buffer = NULL;


// Socket global configuration
static int skt_sendtimeout = 0;
static int skt_recvtimeout = 0;
static int tcp_nodelay = 0;
static int skt_keepalive = 0;
static int tcp_keepalive_idle = 0;
static int tcp_keepalive_interval = 0;
static int tcp_keepalive_count = 0;


/* certificate & key index for ssl client. */
static int client_auth_mode = SSL_AUTH_MODE_NO;   /* set by AT+SKTCLICONF */
static int client_pki_index = 0;   /* set by AT+SKTCLICONF */
static int client_ca_index = 0;   /* set by AT+SKTCLICONF */

/* certificate & key index for ssl server. */
static int server_auth_mode = SSL_AUTH_MODE_LOAD_CERT;   /* set by AT+SKTSRVCONF */
static int server_pki_index = 0;   /* set by AT+SKTSRVCONF */
static int server_ca_index = 0;   /* set by AT+SKTSRVCONF */

extern struct netif xnetif[NET_IF_NUM];



static void atcmd_ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
	UNUSED(ctx);
	UNUSED(file);
	UNUSED(line);

	if (level <= ATCMD_SSL_DEBUG_LEVEL) {
		RTK_LOGA(AT_SOCKET_TAG, "%s", str);
	}
}


static int atcmd_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
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

	val_str = (char *)rtos_mem_zmalloc(len + 1);
	if (val_str != NULL) {
		DiagSnPrintf(val_str, (len + 1), "%d", value);
	}

	return val_str;
}

static int atcmd_lwip_is_autorecv_mode(void)
{
	return (atcmd_lwip_auto_recv == TRUE);
}

static void atcmd_lwip_set_autorecv_mode(int enable)
{
	atcmd_lwip_auto_recv = enable;
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
			node_pool[i].protocol = mode;
			node_pool[i].role = role;
			node_pool[i].addr = 0;
			node_pool[i].port = -1;
			node_pool[i].handletask = NULL;
			node_pool[i].next = NULL;
			node_pool[i].nextseed = NULL;

			int j = 0;
			node_pool[i].ssl = NULL;
			node_pool[i].conf = NULL;
			for (j = 0; j < SSL_CERT_NUM; j++)  {
				node_pool[i].ca_crt[j] = NULL;
				node_pool[i].cert_crt[j] = NULL;
				node_pool[i].private_key[j] = NULL;
			}
			return &node_pool[i];
		}
		SYS_ARCH_UNPROTECT(lev);
	}

	RTK_LOGI(AT_SOCKET_TAG, "[create_list_node] No con_id available\r\n");
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
			} else if (currSeed->protocol == NODE_MODE_SSL && currSeed->sockfd != INVALID_SOCKET_ID) {
				mbedtls_ssl_close_notify(currSeed->ssl);
				mbedtls_net_context server_fd;
				server_fd.fd = currSeed->sockfd;
				mbedtls_net_free(&server_fd);
				mbedtls_ssl_free(currSeed->ssl);
				rtos_mem_free(currSeed->ssl);
				currSeed->ssl = NULL;
			}
			n->nextseed = currSeed->nextseed;
			currSeed->con_id = INVALID_CON_ID;
		};
	}

	if (!((n->protocol == NODE_MODE_UDP) && (n->role == NODE_ROLE_SEED))) {
		if (n->sockfd != INVALID_SOCKET_ID) {
			int i = 0;
			if (n->protocol == NODE_MODE_SSL) {
				if (n->role == NODE_ROLE_SEED) {
					mbedtls_ssl_close_notify(n->ssl);
					mbedtls_net_context server_fd;
					server_fd.fd = currSeed->sockfd;
					mbedtls_net_free(&server_fd);
					mbedtls_ssl_free(n->ssl);
				} else if (n->role == NODE_ROLE_CLIENT) {
					mbedtls_ssl_close_notify(n->ssl);
					mbedtls_net_context server_fd;
					server_fd.fd = n->sockfd;
					mbedtls_net_free(&server_fd);
					for (i = 0; i < SSL_CERT_NUM; i++)  {
						if (n->ca_crt[i]) {
							mbedtls_x509_crt_free(n->ca_crt[i]);
							rtos_mem_free((void *)n->ca_crt[i]);
							n->ca_crt[i] = NULL;
						}
						if (n->cert_crt[i]) {
							mbedtls_x509_crt_free(n->cert_crt[i]);
							rtos_mem_free((void *)n->cert_crt[i]);
							n->cert_crt[i] = NULL;
						}
						if (n->private_key[i]) {
							mbedtls_pk_free(n->private_key[i]);
							rtos_mem_free((void *)n->private_key[i]);
							n->private_key[i] = NULL;
						}
					}
					mbedtls_ssl_free(n->ssl);
				} else {
					mbedtls_net_context server_fd;
					server_fd.fd = n->sockfd;
					mbedtls_net_free(&server_fd);
					for (i = 0; i < SSL_CERT_NUM; i++)  {
						if (n->ca_crt[i]) {
							mbedtls_x509_crt_free(n->ca_crt[i]);
							rtos_mem_free((void *)n->ca_crt[i]);
							n->ca_crt[i] = NULL;
						}
						if (n->cert_crt[i]) {
							mbedtls_x509_crt_free(n->cert_crt[i]);
							rtos_mem_free((void *)n->cert_crt[i]);
							n->cert_crt[i] = NULL;
						}
						if (n->private_key[i]) {
							mbedtls_pk_free(n->private_key[i]);
							rtos_mem_free((void *)n->private_key[i]);
							n->private_key[i] = NULL;
						}
					}

				}
			} else {
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
				RTK_LOGI(AT_SOCKET_TAG, "This conn(IP:%s PORT:%d) already exist\r\n", inet_ntoa(addr), insert_node->port);
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
	if (ret < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] select() error = %d\r\n", ret);
		error_no = 8;
		goto end;
	}
	if (!((ret > 0) && (FD_ISSET(curnode->sockfd, &readfds)))) {
		if (curnode->protocol == NODE_MODE_SSL) {
			ret = mbedtls_ssl_get_bytes_avail(curnode->ssl);
			if (ret == 0) {
				// RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] No receive event for con_id %d\r\n", curnode->con_id);
				goto end;
			}
		} else {
			// RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] No receive event for con_id %d\r\n", curnode->con_id);
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
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
			} else if (size < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
				error_no = 9;
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
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
			} else if (size < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] recvfrom() return size = %d\r\n", size);
				error_no = 10;
			}
		}
	} else {
		if (curnode->protocol == NODE_MODE_SSL) {
			size = mbedtls_ssl_read(curnode->ssl, buffer, buffer_size);
		} else {//TCP
			size = read(curnode->sockfd, buffer, buffer_size);
		}

		if (size == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] read()=0; Connection closed\r\n");
			//error_no = 0;
		} else if (size < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] read() failed %d\r\n", size);
			error_no = 11;
		}
	}

end:
	if (error_no == 0) {
		*recv_size = size;
	} else {
		if (curnode->protocol == NODE_MODE_SSL) {
			mbedtls_net_context server_fd;
			server_fd.fd = curnode->sockfd;
			mbedtls_ssl_close_notify(curnode->ssl);
			mbedtls_net_free(&server_fd);
			mbedtls_ssl_free(curnode->ssl);
		} else {
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
		rx_buffer = (u8 *)rtos_mem_zmalloc(packet_size + 1);
		if (rx_buffer == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_task] rx_buffer malloc fail\r\n");
			goto end;
		}
	}

	while (atcmd_lwip_is_autorecv_mode()) {
		for (i = 0; i < NUM_NS; i++) {
			curnode = tryget_list_node(i);
			if (curnode == NULL) {
				continue;
			}

			if ((curnode->protocol == NODE_MODE_TCP || curnode->protocol == NODE_MODE_SSL)
				&& (curnode->role == NODE_ROLE_SERVER)) {
				continue;
			}

			error_no = atcmd_lwip_receive_data(curnode, rx_buffer, packet_size, &recv_size, udp_clientaddr, &udp_clientport);

			if (error_no == 0) {
				if (recv_size > 0) {
					rx_buffer[recv_size] = '\0';
					if (curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER) {
						at_printf("+SKTREAD:%d,%d,%s,%d:", recv_size, curnode->con_id, udp_clientaddr, udp_clientport);
					} else {
						at_printf("+SKTREAD:%d,%d:", recv_size, curnode->con_id);
					}
					at_printf("%s", rx_buffer);
				}
			} else {
				at_printf("+SKTREAD:%d\r\n", curnode->con_id);
				at_printf(ATCMD_ERROR_END_STR, error_no);
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

int atcmd_lwip_start_autorecv_task(void)
{
	atcmd_lwip_set_autorecv_mode(TRUE);
	if (RTK_SUCCESS != rtos_task_create(NULL,
										"atcmd_lwip_receive_task",
										atcmd_lwip_receive_task,
										NULL,
										ATCP_STACK_SIZE,
										ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(AT_SOCKET_TAG, "Create atcmd_lwip_receive_task failed.\r\n");
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
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] UDP server send failed in sendto()=%d\r\n", ret);
			error_no = 2;
			goto end;
		}
	}
	/* UDP client. */
	else if (curnode->protocol == NODE_MODE_UDP) {
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(curnode->port);
		serv_addr.sin_addr.s_addr = htonl(curnode->addr);
		ret = sendto(curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] UDP client send failed in sendto()=%d\r\n", ret);
			error_no = 2;
			goto end;
		}
	}
	/* TCP or SSL. */
	else if ((curnode->protocol == NODE_MODE_TCP) || (curnode->protocol == NODE_MODE_SSL)) {
		if (curnode->protocol == NODE_MODE_SSL) {
			ret = mbedtls_ssl_write(curnode->ssl, (unsigned char *)data, (size_t)data_sz);
		} else {
			ret = write(curnode->sockfd, data, data_sz);
		}
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] Failed in mbedtls_ssl_write()/write()=%d for protocol=%d\r\n", ret, curnode->protocol);
			error_no = 2;
			goto end;
		}
	}

end:
	return error_no;
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
				RTK_LOGI(AT_SOCKET_TAG, "the same profile found in flash\r\n");
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


static void init_node_pool(void)
{
	int i;
	memset(node_pool, 0, sizeof(node_pool));
	for (i = 0; i < NUM_NS; i++) {
		node_pool[i].con_id = INVALID_CON_ID;
		node_pool[i].sockfd = INVALID_SOCKET_ID;
	}
}


static void init_single_node(struct _node *socket_node)
{
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);
	memset(socket_node, 0, sizeof(struct _node));
	socket_node->con_id = INVALID_CON_ID;
	socket_node->role = NODE_ROLE_INVALID;
	socket_node->sockfd = INVALID_SOCKET_ID;
	socket_node->protocol = SOCKET_PROTOCOL_INVALID;
	SYS_ARCH_UNPROTECT(lev);
}


static int search_available_node(void)
{
	for (int i = 0; i < NUM_NS; i++) {
		if (node_pool[i].con_id == INVALID_CON_ID) {
			return i;
		}
	}
	return -1;
}


int hang_seednode(struct _node *main_node, struct _node *insert_node)
{
	struct _node *n = main_node;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	while (n->nextseed != NULL) {
		n = n->nextseed;
		/*if ((n->port == insert_node->port) && (n->addr == insert_node->addr)) {
			SYS_ARCH_UNPROTECT(lev);
			struct in_addr addr;
			addr.s_addr = htonl(insert_node->addr);
			RTK_LOGI(AT_SOCKET_TAG, "This seed IP:%s PORT:%d already exist\r\n", inet_ntoa(addr), insert_node->port);
			return -1;
		}*/
	}

	n->nextseed = insert_node;
	SYS_ARCH_UNPROTECT(lev);
	return 0;
}


void at_sktcfg(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int input_val = 0;

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 8)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1])) {
		if ((input_val = atoi(argv[1])) < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <so_sndtimeo>\r\n");
			error_no = 1;
			goto end;
		}
		skt_sendtimeout = input_val;
	}
	if (strlen(argv[2])) {
		if ((input_val = atoi(argv[2])) < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <so_rcvtimeo>\r\n");
			error_no = 1;
			goto end;
		}
		skt_recvtimeout = input_val;
	}
	if (strlen(argv[3])) {
		input_val = atoi(argv[3]);
		if ((input_val != 0) && (input_val != 1)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <tcp_nodelay>\r\n");
			error_no = 1;
			goto end;
		}
		tcp_nodelay = input_val;
	}
	if (strlen(argv[4])) {
		input_val = atoi(argv[4]);
		if ((input_val != 0) && (input_val != 1)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <so_keepalive>\r\n");
			error_no = 1;
			goto end;
		}
		skt_keepalive = input_val;
	}
	if (strlen(argv[5])) {
		input_val = atoi(argv[5]);
		if ((input_val < 0) || (input_val > 7200)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <tcp_keepidle>\r\n");
			error_no = 1;
			goto end;
		}
		tcp_keepalive_idle = input_val;
	}
	if (strlen(argv[6])) {
		input_val = atoi(argv[6]);
		if ((input_val < 0) || (input_val > 75)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <tcp_keepintvl>\r\n");
			error_no = 1;
			goto end;
		}
		tcp_keepalive_interval = input_val;
	}
	if (strlen(argv[7])) {
		input_val = atoi(argv[7]);
		if ((input_val < 0) || (input_val > 10)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] Incorrect <tcp_keepcnt>\r\n");
			error_no = 1;
			goto end;
		}
		tcp_keepalive_count = input_val;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


void print_global_socket_config(void)
{
	at_printf("\r\n");
	at_printf("Global SOCKET configuration:\r\n");
	at_printf("so_sndtimeo: %d\r\n", skt_sendtimeout);
	at_printf("so_rcvtimeo: %d\r\n", skt_recvtimeout);
	at_printf("tcp_nodelay: %d\r\n", tcp_nodelay);
	at_printf("so_keepalive: %d\r\n", skt_keepalive);
	at_printf("tcp_keepidle: %d\r\n", tcp_keepalive_idle);
	at_printf("tcp_keepintvl: %d\r\n", tcp_keepalive_interval);
	at_printf("tcp_keepcnt: %d\r\n", tcp_keepalive_count);
}


void at_sktquery(void *arg)
{
	int error_no = 0;

	if (arg != NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktquery] No need input parameter\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		print_global_socket_config();
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
	return;
}


int set_socket_udp_option(int sockfd)
{
	int ret = 0;

#if LWIP_SO_SNDTIMEO
	if (skt_sendtimeout > 0) {
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 1)
		struct timeval timeout;
		timeout.tv_sec  = skt_sendtimeout / 1000;
		timeout.tv_usec = (skt_sendtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#else
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &skt_sendtimeout, sizeof(skt_sendtimeout));
#endif
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_SNDTIMEO) failed\r\n");
			goto end;
		}
	}
#endif
#if LWIP_SO_RCVTIMEO
	if (skt_recvtimeout > 0) {
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 1)
		struct timeval timeout;
		timeout.tv_sec  = skt_recvtimeout / 1000;
		timeout.tv_usec = (skt_recvtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#else
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &skt_recvtimeout, sizeof(skt_recvtimeout));
#endif
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_RCVTIMEO) failed\r\n");
			goto end;
		}
	}
#endif

end:
	return ret;
}


int set_socket_tcp_option(int sockfd)
{
	int ret = 0;

#if LWIP_SO_SNDTIMEO
	if (skt_sendtimeout > 0) {
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 1)
		struct timeval timeout;
		timeout.tv_sec  = skt_sendtimeout / 1000;
		timeout.tv_usec = (skt_sendtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#else
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &skt_sendtimeout, sizeof(skt_sendtimeout));
#endif
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_SNDTIMEO) failed\r\n");
			goto end;
		}
	}
#endif
#if LWIP_SO_RCVTIMEO
	if (skt_recvtimeout > 0) {
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 1)
		struct timeval timeout;
		timeout.tv_sec  = skt_recvtimeout / 1000;
		timeout.tv_usec = (skt_recvtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#else
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &skt_recvtimeout, sizeof(skt_recvtimeout));
#endif
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_RCVTIMEO) failed\r\n");
			goto end;
		}
	}
#endif
	if (tcp_nodelay == 1) {
		ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(tcp_nodelay));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(TCP_NODELAY) failed\r\n");
			goto end;
		}
	}
	if (skt_keepalive == 1) {
		ret = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &skt_keepalive, sizeof(skt_keepalive));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_KEEPALIVE) failed\r\n");
			goto end;
		}
	}
#if LWIP_TCP_KEEPALIVE
	if (skt_keepalive == 1) {
		ret = setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepalive_idle, sizeof(tcp_keepalive_idle));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(TCP_KEEPIDLE) failed\r\n");
			goto end;
		}
		ret = setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepalive_interval, sizeof(tcp_keepalive_interval));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(TCP_KEEPINTVL) failed\r\n");
			goto end;
		}
		ret = setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &tcp_keepalive_count, sizeof(tcp_keepalive_count));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(TCP_KEEPCNT) failed\r\n");
			goto end;
		}
	}
#endif

end:
	return ret;
}


static void server_start(void *param)
{
	int s_mode = NODE_MODE_INVALID;
	int s_sockfd = INVALID_SOCKET_ID;
	int s_newsockfd = INVALID_SOCKET_ID;
	socklen_t s_cli_addr_len;
	struct sockaddr_in s_serv_addr, s_cli_addr;
	int s_local_port;
	int error_no = 0, ret = 0;
	int s_opt = 1;

	mbedtls_ssl_context *ssl = NULL;
	mbedtls_ssl_config *conf = NULL;
	mbedtls_net_context server_fd, client_fd;

	char *ssl_server_ca_pem[SSL_CERT_NUM] = {NULL};   // read server CA
	char *ssl_server_cert_pem[SSL_CERT_NUM] = {NULL};   // read server cert
	char *ssl_server_pkey_pem[SSL_CERT_NUM] = {NULL};   // read server private key

	struct _node *ServerNodeUsed = (struct _node *)param;

	if (ServerNodeUsed != NULL) {
		s_mode = ServerNodeUsed->protocol;
		s_local_port = ServerNodeUsed->port;
	}

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
	if (s_mode == NODE_MODE_SSL) {
		mbedtls_net_init(&server_fd);
		char *s_port_str = atcmd_lwip_itoa(s_local_port);
		if (s_port_str == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] Memory failure for s_port_str\r\n");
			error_no = 15;
			goto last_end;
		}
		ret = mbedtls_net_bind(&server_fd, NULL, s_port_str, MBEDTLS_NET_PROTO_TCP);
		if (ret != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] mbedtls_net_bind returned -%d\r\n", -ret);
			rtos_mem_free(s_port_str);
			error_no = 15;
			goto last_end;
		}
		s_sockfd = server_fd.fd;
		rtos_mem_free(s_port_str);
	} else	{
		if (s_mode == NODE_MODE_UDP) {
			s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		} else if (s_mode == NODE_MODE_TCP) {
			s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		} else {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] Unknown connection type[%d]\r\n", s_mode);
		}

		if (s_sockfd == INVALID_SOCKET_ID) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] Create socket error.\r\n");
			error_no = 5;
			goto last_end;
		}

		ret = setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&s_opt, sizeof(s_opt));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] Set socket option error.\r\n");
			close(s_sockfd);
			error_no = 6;
			goto last_end;
		}

		memset((char *)&s_serv_addr, 0, sizeof(s_serv_addr));
		s_serv_addr.sin_family = AF_INET;
		s_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		s_serv_addr.sin_port = htons(s_local_port);

		ret = bind(s_sockfd, (struct sockaddr *)&s_serv_addr, sizeof(s_serv_addr));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] ERROR on binding\r\n");
			close(s_sockfd);
			error_no = 7;
			goto last_end;
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

	if (s_mode == NODE_MODE_SSL) {
		/***********************************************************
		*  SSL 1. Prepare the certificate and key for ssl server
		************************************************************/
		mbedtls_platform_set_calloc_free(atcmd_lwip_calloc, rtos_mem_free);

		ssl_server_cert_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
		if (ssl_server_cert_pem[0] == NULL) {
			RTK_LOGW(AT_SOCKET_TAG, "[server_start] ssl_server_cert_pem[0] malloc fail\r\n");
			error_no = 29;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(ssl_server_cert_pem[0], SERVER_CERT, server_pki_index);
		if (ret > 0) {
			if (ret > SSL_CERT_SIZE) {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_CERT_%d length larger than SSL_CERT_SIZE\r\n", server_pki_index);
				error_no = 30;
				goto end;
			}
			ssl_server_pkey_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
			if (ssl_server_pkey_pem[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] ssl_server_pkey_pem[0] malloc fail\r\n");
				error_no = 33;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ssl_server_pkey_pem[0], SERVER_KEY, server_pki_index);
			if (ret > 0) {
				if (ret > SSL_CERT_SIZE) {
					RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_KEY_%d length larger than SSL_CERT_SIZE\r\n", server_pki_index);
					error_no = 30;
					goto end;
				}
			} else {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_KEY_%d read failed\r\n", server_pki_index);
				error_no = 32;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGW(AT_SOCKET_TAG, "[server_start] NO SERVER_CERT_%d!\r\n", server_pki_index);
			error_no = 31;
			goto end;
		} else {
			RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_CERT_%d read failed\r\n", server_pki_index);
			error_no = 32;
			goto end;
		}

		//if((server_auth_mode == SSL_AUTH_MODE_LOAD_CERT) ||(server_auth_mode == SSL_AUTH_MODE_BOTH) )
		{
			ServerNodeUsed->cert_crt[0] = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
			if (ServerNodeUsed->cert_crt[0] == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] Memory allocation failed for ServerNodeUsed->cert_crt[0]\r\n");
				error_no = 16;
				goto end;
			}
			ServerNodeUsed->private_key[0] = (mbedtls_pk_context *)rtos_mem_zmalloc(sizeof(mbedtls_pk_context));
			if (ServerNodeUsed->private_key[0] == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] Memory allocation failed for ServerNodeUsed->private_key[0]\r\n");
				error_no = 17;
				goto end;
			}
			mbedtls_x509_crt_init(ServerNodeUsed->cert_crt[0]);
			mbedtls_pk_init(ServerNodeUsed->private_key[0]);

			ret = mbedtls_x509_crt_parse(ServerNodeUsed->cert_crt[0], (const unsigned char *)ssl_server_cert_pem[0], strlen(ssl_server_cert_pem[0]) + 1);
			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] mbedtls_x509_crt_parse returned -0x%x\r\n", -ret);
				error_no = 18;
				goto end;
			}
			rtos_mem_free(ssl_server_cert_pem[0]);
			ssl_server_cert_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "server_cert_0.crt parse successful!\r\n");

			ret = mbedtls_pk_parse_key(ServerNodeUsed->private_key[0], (unsigned char *)ssl_server_pkey_pem[0], strlen(ssl_server_pkey_pem[0]) + 1, NULL, 0, NULL, NULL);

			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] mbedtls_pk_parse_key returned -0x%x\r\n", -ret);
				error_no = 20;
				goto end;
			}
			rtos_mem_free(ssl_server_pkey_pem[0]);
			ssl_server_pkey_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "server_key_0.key parse successful!\r\n");
		}

		if ((server_auth_mode == SSL_AUTH_MODE_LOAD_CA) || (server_auth_mode == SSL_AUTH_MODE_BOTH))  {
			ssl_server_ca_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
			if (ssl_server_ca_pem[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] ssl_server_ca_pem[0] malloc fail\r\n");
				error_no = 34;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ssl_server_ca_pem[0], SERVER_CA, server_ca_index);
			if (ret > 0) {
				if (ret > SSL_CERT_SIZE) {
					RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_CA_%d length larger than SSL_CERT_SIZE\r\n", server_ca_index);
					error_no = 30;
					goto end;
				}
			} else if (ret == 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] NO SERVER_CA_%d!\r\n", server_ca_index);
				error_no = 31;
				goto end;
			} else {
				RTK_LOGW(AT_SOCKET_TAG, "[server_start] SERVER_CA_%d read failed\r\n", server_ca_index);
				error_no = 32;
				goto end;
			}

			ServerNodeUsed->ca_crt[0] = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
			if (ServerNodeUsed->ca_crt[0] == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] Memory allocation failed for ServerNodeUsed->ca_crt[0]\r\n");
				error_no = 16;
				goto end;
			}
			mbedtls_x509_crt_init(ServerNodeUsed->ca_crt[0]);
			ret = mbedtls_x509_crt_parse(ServerNodeUsed->ca_crt[0], (const unsigned char *)ssl_server_ca_pem[0], strlen(ssl_server_ca_pem[0]) + 1);
			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] mbedtls_x509_crt_parse returned -0x%x\r\n", -ret);
				error_no = 19;
				goto end;
			}
			rtos_mem_free(ssl_server_ca_pem[0]);
			ssl_server_ca_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "server_ca_0.crt parse successful!\r\n");
		}
		/***********************************************************
		*  SSL 2. Hang node on mainlist for global management
		************************************************************/
		ret = hang_list_node(ServerNodeUsed);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "ERROR: failed in hang_list_node\r\n");
			error_no = 21;
			goto end;
		} else {
			at_printf("\r\n%scon_id=%d\r\n", "+SKTSERVER:", ServerNodeUsed->con_id);
		}
		RTK_LOGI(AT_SOCKET_TAG, "\r\nThe SSL SERVER START OK!\r\n");
		/***********************************************************
		*  SSL 3. Waiting for ssl client to connect
		************************************************************/
		mbedtls_net_init(&client_fd);
		while (1) {
			//not using mbedtls_net_accept() here because it can't get client port
			if ((s_newsockfd = accept(s_sockfd, (struct sockaddr *) &s_cli_addr, &s_cli_addr_len)) < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] ERROR: on accept() ret=%d\r\n", ret);
				error_no = 22;
				goto end;
			}
			RTK_LOGI(AT_SOCKET_TAG, "An SSL/TLS client[%s:%d] is connecting\r\n", inet_ntoa(s_cli_addr.sin_addr), ntohs(s_cli_addr.sin_port));
			/***********************************************************
			*  SSL 4. Setup stuff for this ssl connection
			************************************************************/
			RTK_LOGI(AT_SOCKET_TAG, "Setting up the SSL/TLS structure...\r\n");
			ssl = (mbedtls_ssl_context *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_context));
			conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
			if ((ssl == NULL) || (conf == NULL)) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] malloc fail for ssl client context or config!\r\n");
				error_no = 23;
				goto end;
			}

			mbedtls_ssl_init(ssl);
			mbedtls_ssl_config_init(conf);

			if ((ret = mbedtls_ssl_config_defaults(conf,
												   MBEDTLS_SSL_IS_SERVER,
												   MBEDTLS_SSL_TRANSPORT_STREAM,
												   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] mbedtls_ssl_config_defaults returned %d\r\n", ret);
				error_no = 24;
				goto end;
			}

			mbedtls_ssl_conf_rng(conf, atcmd_ssl_random, NULL);
			client_fd.fd = s_newsockfd;
			mbedtls_ssl_set_bio(ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
			//if((server_auth_mode == SSL_AUTH_MODE_LOAD_CERT) ||(server_auth_mode == SSL_AUTH_MODE_BOTH) )
			{
				ret = mbedtls_ssl_conf_own_cert(conf, ServerNodeUsed->cert_crt[0], ServerNodeUsed->private_key[0]);
				if (ret != 0) {
					RTK_LOGI(AT_SOCKET_TAG, "ERROR: mbedtls_ssl_conf_own_cert returned %d\r\n", ret);
					error_no = 25;
					goto end;
				}
			}
			if ((server_auth_mode == SSL_AUTH_MODE_LOAD_CA) || (server_auth_mode == SSL_AUTH_MODE_BOTH))  {
				mbedtls_ssl_conf_ca_chain(conf, ServerNodeUsed->ca_crt[0], NULL);
				mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
			}

			mbedtls_ssl_conf_dbg(conf, atcmd_ssl_debug, NULL);
#ifdef MBEDTLS_DEBUG_C
			mbedtls_debug_set_threshold(ATCMD_SSL_DEBUG_LEVEL);
#endif

			ret = mbedtls_ssl_setup(ssl, conf);
			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "ERROR: mbedtls_ssl_setup returned %d\r\n", ret);
				error_no = 26;
				goto end;
			}
			/***********************************************************
			*  SSL 5. Wait for the ssl handshake done
			************************************************************/
			RTK_LOGI(AT_SOCKET_TAG, "Performing the SSL/TLS handshake...\r\n");
			ret = mbedtls_ssl_handshake(ssl);
			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "ERROR: mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
				error_no = 27;
				goto end;
			}

			/***********************************************************
			*  SSL 6. Hang node on mainlist for global management
			************************************************************/
			struct _node *seednode = create_list_node(s_mode, NODE_ROLE_SEED);
			if (seednode == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[server_start] create seednode failed!\r\n");
				error_no = 28;
				goto end;
			}
			seednode->sockfd = s_newsockfd;
			seednode->port = ntohs(s_cli_addr.sin_port);
			seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
			seednode->ssl = ssl;
			seednode->conf = conf;
			ret = hang_seednode(ServerNodeUsed, seednode);
			if (ret < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[AT+SKTSERVER] hang_seednode() return 0\r\n");
				delete_list_node(seednode);
				seednode = NULL;
			} else {
				RTK_LOGI(AT_SOCKET_TAG, "The SSL/TLS client is connected\r\n");
				at_printf("\r\n%s A client connected to server[con_id:%d]\r\n"
						  "con_id:%d, seed, ssl, address:%s, port:%d, socket_fd:%d\r\n",
						  "+SKTSERVER:",
						  ServerNodeUsed->con_id,
						  seednode->con_id,
						  inet_ntoa(s_cli_addr.sin_addr),
						  ntohs(s_cli_addr.sin_port),
						  seednode->sockfd);
			}
		}
	} else	{
		if (s_mode == NODE_MODE_TCP) {//TCP MODE
			/***********************************************************
			*  TCP 1. Set this socket into listen mode
			************************************************************/
			ret = listen(s_sockfd, 5);
			if (ret < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "ERROR on listening\r\n");
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

			RTK_LOGI(AT_SOCKET_TAG, "The TCP SERVER START OK!\r\n");
			s_cli_addr_len = sizeof(s_cli_addr);
			/***********************************************************
			*  TCP 3. Waiting for TCP client to connect
			************************************************************/
			while (1) {
				s_newsockfd = accept(s_sockfd, (struct sockaddr *) &s_cli_addr, &s_cli_addr_len);
				if (s_newsockfd < 0) {
					if (param != NULL) {
						RTK_LOGI(AT_SOCKET_TAG, "[AT+SKTSERVER] ERROR on accept\r\n");
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
							RTK_LOGI(AT_SOCKET_TAG, "[AT+SKTSERVER]create node failed!\r\n");
							error_no = 11;
							goto end;
						}
						seednode->sockfd = s_newsockfd;
						seednode->port = ntohs(s_cli_addr.sin_port);
						seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
						ret = hang_seednode(ServerNodeUsed, seednode);
						if (ret < 0) {
							RTK_LOGI(AT_SOCKET_TAG, "[AT+SKTSERVER] hang_seednode() return 0\r\n");
							delete_list_node(seednode);
							seednode = NULL;
						} else {
							at_printf("\r\n%s A client connected to server[con_id:%d]\r\n"
									  "con_id:%d, seed, tcp, address:%s, port:%d, socket_fd:%d\r\n",
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
			RTK_LOGI(AT_SOCKET_TAG, "The UDP SERVER START OK!\r\n");
		}
	}

end:
	if (ssl_server_ca_pem[0]) {
		rtos_mem_free(ssl_server_ca_pem[0]);
		ssl_server_ca_pem[0] = NULL;
	}
	if (ssl_server_cert_pem[0]) {
		rtos_mem_free(ssl_server_cert_pem[0]);
		ssl_server_cert_pem[0] = NULL;
	}
	if (ssl_server_pkey_pem[0]) {
		rtos_mem_free(ssl_server_pkey_pem[0]);
		ssl_server_pkey_pem[0] = NULL;
	}
	if (s_mode == NODE_MODE_SSL)  {
		mbedtls_ssl_close_notify(ssl);
		mbedtls_net_free(&server_fd);
		mbedtls_ssl_free(ssl);
		rtos_mem_free(ssl);
		ssl = NULL;
		mbedtls_ssl_config_free(conf);
		rtos_mem_free(conf);
		conf = NULL;
		mbedtls_x509_crt_free(ServerNodeUsed->ca_crt[0]);
		rtos_mem_free(ServerNodeUsed->ca_crt[0]);
		ServerNodeUsed->ca_crt[0] = NULL;
		mbedtls_x509_crt_free(ServerNodeUsed->cert_crt[0]);
		rtos_mem_free(ServerNodeUsed->cert_crt[0]);
		ServerNodeUsed->cert_crt[0] = NULL;
		mbedtls_pk_free(ServerNodeUsed->private_key[0]);
		rtos_mem_free(ServerNodeUsed->private_key[0]);
		ServerNodeUsed->private_key[0] = NULL;
	}

last_end:
	//reset server_auth_mode, server_pki_index, server_ca_index
	server_auth_mode = SSL_AUTH_MODE_LOAD_CERT;
	server_pki_index = 0;
	server_ca_index = 0;

	if (error_no != 0) {
		if (ServerNodeUsed != NULL) {
			//task will exit itself if getting here
			ServerNodeUsed->handletask = NULL;
			delete_list_node(ServerNodeUsed);
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

static void server_start_task(void *param)
{
	rtos_time_delay_ms(100);

	if (param != NULL) {
		server_start(param);
	} else {
		RTK_LOGW(AT_SOCKET_TAG, "server_start_task(): param is NULL!\r\n");
	}

	rtos_task_delete(NULL);
	return;
}


void socket_server_tcp_auto_rcv_client_and_data(void *param)
{
	int new_client_sockfd = INVALID_SOCKET_ID;
	static int max_fd = INVALID_SOCKET_ID;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = 0;
	static fd_set read_fds;
	int ret = 0;
	int index = -1;
	struct _node *current_node = (struct _node *)param;
	struct _node *seednode = NULL;
	int actual_bytes_received = 0;
	char *read_buf = NULL;

	client_addr_len = sizeof(client_addr);
	max_fd = current_node->sockfd;

	if (current_node->auto_rcv == TRUE) {
		read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
		if (read_buf == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] read_buf malloc failed\r\n");
			goto end;
		}
	}

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);
		seednode = current_node->nextseed;
		while (seednode) {
			if (seednode->sockfd >= 0) {
				FD_SET(seednode->sockfd, &read_fds);
			}
			seednode = seednode->nextseed;
		}
		ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to select() = %d\r\n", ret);
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			new_client_sockfd = accept(current_node->sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] accept() = %d\r\n", new_client_sockfd);
			if (new_client_sockfd < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to accept() = %d\r\n", new_client_sockfd);
				continue;
			}
			index = search_available_node();
			if (index < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to search_available_node()\r\n");
				close(new_client_sockfd);
				continue;
			}
			SYS_ARCH_DECL_PROTECT(lev);
			SYS_ARCH_PROTECT(lev);
			node_pool[index].con_id = index;
			node_pool[index].role = NODE_ROLE_SEED;
			node_pool[index].sockfd = new_client_sockfd;
			node_pool[index].protocol = SOCKET_SERVER_OVER_TCP;
			node_pool[index].dst_ip = client_addr.sin_addr.s_addr;
			node_pool[index].dst_port = ntohs(client_addr.sin_port);
			SYS_ARCH_UNPROTECT(lev);
			ret = hang_seednode(current_node, &node_pool[index]);
			if (ret < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to hang_seednode()\r\n");
				close(new_client_sockfd);
				init_single_node(&node_pool[index]);
				continue;
			}
			at_printf_indicate("[SKT][EVENT]: A client[link_id:%d, seed, tcp, address:%s, port:%d] connected to server[link_id:%d]\r\n",
							   node_pool[index].con_id, inet_ntoa(client_addr.sin_addr), node_pool[index].dst_port, current_node->con_id);

			if (current_node->auto_rcv == TRUE) {
				if (new_client_sockfd > max_fd) {
					max_fd = new_client_sockfd;
				}
				continue;
			}
		}
		if (current_node->auto_rcv == TRUE) {
			seednode = current_node->nextseed;
			while (seednode) {
				if ((seednode->sockfd >= 0) && FD_ISSET(seednode->sockfd, &read_fds)) {
					actual_bytes_received = read(seednode->sockfd, read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE);

					if (actual_bytes_received == 0) {
						RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] read() = 0; Connection closed\r\n");
						FD_CLR(seednode->sockfd, &read_fds);
						close(seednode->sockfd);
						seednode->sockfd = INVALID_SOCKET_ID;
					} else if (actual_bytes_received < 0) {
						RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to read() = %d\r\n", actual_bytes_received);
						FD_CLR(seednode->sockfd, &read_fds);
						close(seednode->sockfd);
						seednode->sockfd = INVALID_SOCKET_ID;
					}
					if (actual_bytes_received > 0) {
						at_printf_lock();
						at_printf_indicate("[SKT][DATA][%d][%d]:", seednode->con_id, actual_bytes_received);
						at_printf_data(read_buf, (u32)actual_bytes_received);
						at_printf("\r\n");
						at_printf_unlock();
					}
				}
				seednode = seednode->nextseed;
			}
		}
	}

end:
	if (current_node->auto_rcv == TRUE) {
		rtos_mem_free(read_buf);
	}
	current_node->auto_rcv_task = NULL;
	rtos_task_delete(NULL);
	return;
}


int create_socket_server_tcp(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	int sockfd = INVALID_SOCKET_ID;
	int optval = 1;
	struct sockaddr_in local_server_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] Failed to create TCP socket\r\n");
		error_no = 2;
		goto end;
	}
	if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] setsockopt(SO_REUSEADDR) failed = %d\r\n", ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}

	if ((ret = set_socket_tcp_option(sockfd)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] set_socket_tcp_option() failed = %d\r\n", ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}

	memset(&local_server_addr, 0, sizeof(local_server_addr));
	local_server_addr.sin_family = AF_INET;
	local_server_addr.sin_addr.s_addr = INADDR_ANY;
	local_server_addr.sin_port = htons(current_node->src_port);

	if ((ret = bind(sockfd, (struct sockaddr *)&local_server_addr, sizeof(local_server_addr))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] Failed to bind() to port: %d, ret = %d\r\n", current_node->src_port, ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}

	if ((ret = listen(sockfd, SOCKET_AT_BACKLOG)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] Failed to listen() to port: %d, ret = %d\r\n", current_node->src_port, ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}

	current_node->sockfd = sockfd;

	if (rtos_task_create(&(current_node->auto_rcv_task), "socket_server_tcp_auto_rcv_client_and_data", socket_server_tcp_auto_rcv_client_and_data, current_node,
						 1024 * 4, ATCMD_LWIP_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] rtos_task_create(socket_server_tcp_auto_rcv_client_and_data) failed\r\n");
		close(sockfd);
		error_no = 3;
		goto end;
	}

	struct in_addr local_addr;
	local_addr.s_addr = current_node->src_ip;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] The TCP server created on (%s:%d) successfully\r\n", inet_ntoa(local_addr), current_node->src_port);

end:
	if (error_no != 0) {
		init_single_node(current_node);
	}
	return error_no;
}


int create_socket_server(struct _node *current_node)
{
	int error_no = 0;
	if (current_node->protocol == SOCKET_SERVER_OVER_UDP) {
		//error_no = create_socket_server_udp(current_node);
	} else if (current_node->protocol == SOCKET_SERVER_OVER_TCP) {
		error_no = create_socket_server_tcp(current_node);
	} else {
		//error_no = create_socket_server_tls(current_node);
	}
	return error_no;
}


//AT+SKTSERVER=<link_id>,<conn_type>[,<cert_index>],<src_port>,<auto_rcv>
void at_sktserver_1(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = 0, conn_type = 0, cert_index = 0, src_port = 0, auto_rcv = 0;
	uint8_t *local_ip = NULL;

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 6) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0) ||
		(strlen(argv[4]) == 0) || (strlen(argv[5]) == 0)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}
	link_id = atoi(argv[1]);
	if ((link_id < 0) || (link_id > (NUM_NS - 1))) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Incorrect <link_id>\r\n");
		error_no = 1;
		goto end;
	}
	if (node_pool[link_id].con_id >= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The <link_id> is in use\r\n");
		error_no = 1;
		goto end;
	}
	conn_type = atoi(argv[2]);
	if ((conn_type < SOCKET_SERVER_OVER_UDP) || (conn_type > SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Incorrect <conn_type>\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= SOCKET_SERVER_OVER_TLS) && (conn_type <= SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT)) {
		if (argv[3] == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The <cert_index> is NULL for SOCKET_SERVER_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[3]);
		if (cert_index <= 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Incorrect <cert_index>\r\n");
			error_no = 1;
			goto end;
		}
	}
	src_port = atoi(argv[4]);
	if (src_port <= 0 || src_port > 65535) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Incorrect <src_port>\r\n");
		error_no = 1;
		goto end;
	}
	auto_rcv = atoi(argv[5]);
	if (auto_rcv < 0 || auto_rcv > 1) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Incorrect <auto_rcv>\r\n");
		error_no = 1;
		goto end;
	}

	//Generate the node
	node_pool[link_id].con_id = link_id;
	node_pool[link_id].role = NODE_ROLE_SERVER;
	node_pool[link_id].cert_index = cert_index;
	node_pool[link_id].auto_rcv = auto_rcv;
	node_pool[link_id].protocol = conn_type;
	node_pool[link_id].src_port = src_port;
	local_ip = LwIP_GetIP(0);
	node_pool[link_id].src_ip = *((u32_t *)local_ip);

	error_no = create_socket_server(&node_pool[link_id]);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
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

	if (arg == NULL) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] The parameter ERROR\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[mode_idx]) == 0) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] missing mode\r\n");
		error_no = 1;
		goto end;
	}
	mode = atoi(argv[mode_idx]);

	if ((mode < NODE_MODE_TCP) || (mode > NODE_MODE_SSL)) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] Unknown connection type\r\n");
		error_no = 14;
		goto end;
	}

	if (strlen(argv[port_idx]) == 0) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] missing port\r\n");
		error_no = 2;
		goto end;
	}
	local_port = atoi(argv[port_idx]);
	if ((local_port <= 0) || (local_port > 65535)) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTSERVER] Invalid local port\r\n");
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
	if (RTK_SUCCESS != rtos_task_create(&servernode->handletask,
										"server_start_task",
										server_start_task,
										servernode,
										server_task_stksz,
										ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(AT_SOCKET_TAG, "\r\n[SKTSERVER] Create task failed.\r\n");
		error_no = 4;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		delete_list_node(servernode);
		at_printf(ATCMD_ERROR_END_STR, error_no);
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

	mbedtls_net_context server_fd;
	char *ssl_client_ca_pem[SSL_CERT_NUM] = {NULL};   // read client CA
	char *ssl_client_cert_pem[SSL_CERT_NUM] = {NULL};   // read client cert
	char *ssl_client_pkey_pem[SSL_CERT_NUM] = {NULL};   // read client private key

	struct in_addr c_addr;
	struct _node *ClientNodeUsed = (struct _node *)param;

	if (ClientNodeUsed != NULL) {
		c_mode = ClientNodeUsed->protocol;
		c_remote_port = ClientNodeUsed->port;
		c_addr.s_addr = htonl(ClientNodeUsed->addr);
		if (inet_ntoa_r(c_addr, c_remote_addr, sizeof(c_remote_addr)) == NULL) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Error address\r\n");
			error_no = 6;
			goto last_end;
		}
	}

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
	if (c_mode == NODE_MODE_SSL) {
		mbedtls_net_init(&server_fd);
		char *c_port_str = atcmd_lwip_itoa(c_remote_port);
		if (c_port_str == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[server_start] Memory failure for c_port_str\r\n");
			error_no = 18;
			goto last_end;
		}
		ret = mbedtls_net_connect(&server_fd, c_remote_addr, c_port_str, MBEDTLS_NET_PROTO_TCP);
		if (ret != 0) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed in mbedtls_net_connect\r\n");
			error_no = 18;
			rtos_mem_free(c_port_str);
			goto last_end;
		}
		c_sockfd = server_fd.fd;
		rtos_mem_free(c_port_str);
	} else	{
		if (c_mode == NODE_MODE_UDP) {
			c_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		} else if (c_mode == NODE_MODE_TCP) {
			c_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		} else {
			RTK_LOGI(AT_SOCKET_TAG, "Unknown connection type[%d]\r\n", c_mode);
		}
		if (c_sockfd == INVALID_SOCKET_ID) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed to create socket!\r\n");
			error_no = 7;
			goto last_end;
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

	if (c_mode == NODE_MODE_SSL) {
		/***********************************************************
		*  SSL 1. Setup stuff for this ssl connection
		************************************************************/
		mbedtls_platform_set_calloc_free(atcmd_lwip_calloc, rtos_mem_free);

		ClientNodeUsed->ssl = (mbedtls_ssl_context *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_context));
		if (ClientNodeUsed->ssl == NULL) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Memory allocation failed for ClientNodeUsed->ssl\r\n");
			error_no = 19;
			goto end;
		}
		ClientNodeUsed->conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
		if (ClientNodeUsed->conf == NULL) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Memory allocation failed for ClientNodeUsed->conf\r\n");
			error_no = 19;
			goto end;
		}

		mbedtls_ssl_init(ClientNodeUsed->ssl);
		mbedtls_ssl_config_init(ClientNodeUsed->conf);

		ret = mbedtls_ssl_config_defaults(ClientNodeUsed->conf,
										  MBEDTLS_SSL_IS_CLIENT,
										  MBEDTLS_SSL_TRANSPORT_STREAM,
										  MBEDTLS_SSL_PRESET_DEFAULT);
		if (ret != 0) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed for mbedtls_ssl_config_defaults\r\n");
			error_no = 20;
			goto end;
		}

		mbedtls_ssl_conf_authmode(ClientNodeUsed->conf, MBEDTLS_SSL_VERIFY_NONE);

		if ((client_auth_mode == SSL_AUTH_MODE_LOAD_CERT) || (client_auth_mode == SSL_AUTH_MODE_BOTH))  {
			ssl_client_cert_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
			if (ssl_client_cert_pem[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] ssl_client_cert_pem[0] malloc fail\r\n");
				error_no = 25;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ssl_client_cert_pem[0], CLIENT_CERT, client_pki_index);
			if (ret > 0) {
				if (ret > SSL_CERT_SIZE) {
					RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_CERT_%d length larger than SSL_CERT_SIZE\r\n", client_pki_index);
					error_no = 26;
					goto end;
				}
				ssl_client_pkey_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
				if (ssl_client_pkey_pem[0] == NULL) {
					RTK_LOGW(AT_SOCKET_TAG, "[client_start] ssl_client_pkey_pem[0] malloc fail\r\n");
					error_no = 29;
					goto end;
				}
				ret = atcmd_get_ssl_certificate(ssl_client_pkey_pem[0], CLIENT_KEY, client_pki_index);
				if (ret > 0) {
					if (ret > SSL_CERT_SIZE) {
						RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_KEY_%d length larger than SSL_CERT_SIZE\r\n", client_pki_index);
						error_no = 26;
						goto end;
					}
				} else {
					RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_KEY_%d read failed\r\n", client_pki_index);
					error_no = 28;
					goto end;
				}
			} else if (ret == 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] NO CLIENT_CERT_%d!\r\n", client_pki_index);
				error_no = 27;
				goto end;
			} else {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_CERT_%d read failed\r\n", client_pki_index);
				error_no = 28;
				goto end;
			}

			ClientNodeUsed->cert_crt[0] = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
			if (ClientNodeUsed->cert_crt[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Memory allocation failed for ClientNodeUsed->cert_crt[0]\r\n");
				error_no = 19;
				goto end;
			}
			ClientNodeUsed->private_key[0] = (mbedtls_pk_context *)rtos_mem_zmalloc(sizeof(mbedtls_pk_context));
			if (ClientNodeUsed->private_key[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Memory allocation failed for ClientNodeUsed->private_key[0]\r\n");
				error_no = 19;
				goto end;
			}
			mbedtls_x509_crt_init(ClientNodeUsed->cert_crt[0]);
			mbedtls_pk_init(ClientNodeUsed->private_key[0]);

			ret = mbedtls_x509_crt_parse(ClientNodeUsed->cert_crt[0], (const unsigned char *)ssl_client_cert_pem[0],
										 strlen(ssl_client_cert_pem[0]) + 1);
			if (ret != 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed for mbedtls_x509_crt_parse returned %d\r\n", ret);
				error_no = 23;
				goto end;
			}
			rtos_mem_free(ssl_client_cert_pem[0]);
			ssl_client_cert_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "client_cert_0.crt parse successful!\r\n");

			ret = mbedtls_pk_parse_key(ClientNodeUsed->private_key[0], (const unsigned char *)ssl_client_pkey_pem[0],
									   strlen(ssl_client_pkey_pem[0]) + 1, NULL, 0, NULL, NULL);

			if (ret != 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed for mbedtls_pk_parse_key returned %d\r\n", ret);
				error_no = 24;
				goto end;
			}
			rtos_mem_free(ssl_client_pkey_pem[0]);
			ssl_client_pkey_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "client_key_0.key parse successful!\r\n");

			mbedtls_ssl_conf_own_cert(ClientNodeUsed->conf, ClientNodeUsed->cert_crt[0], ClientNodeUsed->private_key[0]);
		}

		if ((client_auth_mode == SSL_AUTH_MODE_LOAD_CA) || (client_auth_mode == SSL_AUTH_MODE_BOTH))  {
			ssl_client_ca_pem[0] = rtos_mem_zmalloc(SSL_CERT_SIZE);
			if (ssl_client_ca_pem[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] ssl_client_ca_pem[0] malloc fail\r\n");
				error_no = 30;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(ssl_client_ca_pem[0], CLIENT_CA, client_ca_index);
			if (ret > 0) {
				if (ret > SSL_CERT_SIZE) {
					RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_CA_%d length larger than SSL_CERT_SIZE\r\n", client_ca_index);
					error_no = 26;
					goto end;
				}
			} else if (ret == 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] NO CLIENT_CA_%d!\r\n", client_ca_index);
				error_no = 27;
				goto end;
			} else {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] CLIENT_CA_%d read failed\r\n", client_ca_index);
				error_no = 28;
				goto end;
			}

			ClientNodeUsed->ca_crt[0] = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
			if (ClientNodeUsed->ca_crt[0] == NULL) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Memory allocation failed for ClientNodeUsed->ca_crt[0]\r\n");
				error_no = 19;
				goto end;
			}
			mbedtls_x509_crt_init(ClientNodeUsed->ca_crt[0]);
			ret = mbedtls_x509_crt_parse(ClientNodeUsed->ca_crt[0], (const unsigned char *)ssl_client_ca_pem[0],
										 strlen(ssl_client_ca_pem[0]) + 1);
			if (ret != 0) {
				RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed for mbedtls_x509_crt_parse returned %d\r\n", ret);
				error_no = 23;
				goto end;
			}
			rtos_mem_free(ssl_client_ca_pem[0]);
			ssl_client_ca_pem[0] = NULL;
			RTK_LOGI(AT_SOCKET_TAG, "client_ca_0.crt parse successful!\r\n");

			mbedtls_ssl_conf_ca_chain(ClientNodeUsed->conf, ClientNodeUsed->ca_crt[0], NULL);
		}
		if ((client_auth_mode == SSL_AUTH_MODE_LOAD_CA) || (client_auth_mode == SSL_AUTH_MODE_BOTH))  {
			mbedtls_ssl_conf_authmode(ClientNodeUsed->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		}

		mbedtls_ssl_conf_rng(ClientNodeUsed->conf, atcmd_ssl_random, NULL);
		mbedtls_ssl_set_bio(ClientNodeUsed->ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
		mbedtls_ssl_conf_dbg(ClientNodeUsed->conf, atcmd_ssl_debug, NULL);

		ret = mbedtls_ssl_setup(ClientNodeUsed->ssl, ClientNodeUsed->conf);
		if (ret != 0) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Failed for mbedtls_ssl_setup\r\n");
			error_no = 31;
			goto end;
		}

#ifdef MBEDTLS_DEBUG_C
		mbedtls_debug_set_threshold(ATCMD_SSL_DEBUG_LEVEL);
#endif

		/***********************************************************
		*  SSL 2. Wait for the ssl handshake done
		************************************************************/
		RTK_LOGI(AT_SOCKET_TAG, "[client_start] Performing the SSL/TLS handshake...\r\n");

		int retry_count = 0;
		while ((ret = mbedtls_ssl_handshake(ClientNodeUsed->ssl)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "ERROR: mbedtls_ssl_handshake returned -0x%x\r\n", -ret);
			if (retry_count > 3) {
				error_no = 21;
				goto end;
			}
			retry_count++;
		}
		RTK_LOGI(AT_SOCKET_TAG, "Connect to Server successful!\r\n");

		/***********************************************************
		*  SSL 3. Hang node on mainlist for global management
		************************************************************/
		ret = hang_list_node(ClientNodeUsed);
		if (ret < 0) {
			RTK_LOGW(AT_SOCKET_TAG, "[client_start] Hang node failed for SSL client\r\n");
			error_no = 22;
			goto end;
		}
		at_printf("\r\n%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
		return;
	} else	{
		/* TCP mode. */
		if (c_mode == NODE_MODE_TCP) {
			/***********************************************************
			*  TCP 1. Connect a netconn to a specific remote IP address and port
			************************************************************/
			ret = connect(c_sockfd, (struct sockaddr *)&c_serv_addr,  sizeof(c_serv_addr));
			if (ret == 0) {
				RTK_LOGI(AT_SOCKET_TAG, "Connect to Server successful!\r\n");
				/***********************************************************
				*  TCP 2. Hand node on mainlist for global management if connect success
				************************************************************/
				if (ClientNodeUsed != NULL) {
					ret = hang_list_node(ClientNodeUsed);
					if (ret < 0) {
						RTK_LOGW(AT_SOCKET_TAG, "Failed in hang_list_node\r\n");
						error_no = 8;
						close(c_sockfd);
						goto last_end;
					}
					at_printf("%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
				}
			} else {
				/***********************************************************
				*  TCP 2. Free node if connect fail
				************************************************************/
				RTK_LOGW(AT_SOCKET_TAG, "Connect to Server failed!\r\n");
				error_no = 9;
				close(c_sockfd);
				goto last_end;
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
						RTK_LOGW(AT_SOCKET_TAG, "Failed in setsockopt!\r\n");
						error_no = 14;
						goto last_end;
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
						RTK_LOGW(AT_SOCKET_TAG, "Failed in setsockopt!\r\n");
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 15;
						goto last_end;
					}
					intfAddr.s_addr = INADDR_ANY;
					ret = setsockopt(c_sockfd, IPPROTO_IP, IP_MULTICAST_IF, &intfAddr, sizeof(struct in_addr));
					if (ret < 0) {
						RTK_LOGW(AT_SOCKET_TAG, "Failed in setsockopt!\r\n");
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 16;
						goto last_end;
					}
				}
#endif
				if (ClientNodeUsed->local_port) {
					struct sockaddr_in client_addr;
					memset(&client_addr, 0, sizeof(client_addr));
					client_addr.sin_family = AF_INET;
					client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
					client_addr.sin_port = htons(ClientNodeUsed->local_port);
					ret = bind(c_sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
					if (ret < 0) {
						RTK_LOGW(AT_SOCKET_TAG, "Error binding to client address/port\r\n");
						error_no = 12;
						close(c_sockfd);
						goto last_end;
					}
				}
				ret = hang_list_node(ClientNodeUsed);
				if (ret < 0) {
					RTK_LOGW(AT_SOCKET_TAG, "Failed in hang_list_node!\r\n");
					error_no = 10;
					close(c_sockfd);
					goto last_end;
				}
				at_printf("%scon_id=%d\r\n", "+SKTCLIENT:", ClientNodeUsed->con_id);
			}
		}
		return;
	}

end:
	if (ssl_client_ca_pem[0]) {
		rtos_mem_free(ssl_client_ca_pem[0]);
		ssl_client_ca_pem[0] = NULL;
	}
	if (ssl_client_cert_pem[0]) {
		rtos_mem_free(ssl_client_cert_pem[0]);
		ssl_client_cert_pem[0] = NULL;
	}
	if (ssl_client_pkey_pem[0]) {
		rtos_mem_free(ssl_client_pkey_pem[0]);
		ssl_client_pkey_pem[0] = NULL;
	}

	if (c_mode == NODE_MODE_SSL)  {
		mbedtls_ssl_close_notify(ClientNodeUsed->ssl);
		mbedtls_net_free(&server_fd);
		mbedtls_ssl_free(ClientNodeUsed->ssl);
		rtos_mem_free(ClientNodeUsed->ssl);
		ClientNodeUsed->ssl = NULL;
		mbedtls_ssl_config_free(ClientNodeUsed->conf);
		rtos_mem_free(ClientNodeUsed->conf);
		ClientNodeUsed->conf = NULL;
		mbedtls_x509_crt_free(ClientNodeUsed->ca_crt[0]);
		rtos_mem_free(ClientNodeUsed->ca_crt[0]);
		ClientNodeUsed->ca_crt[0] = NULL;
		mbedtls_x509_crt_free(ClientNodeUsed->cert_crt[0]);
		rtos_mem_free(ClientNodeUsed->cert_crt[0]);
		ClientNodeUsed->cert_crt[0] = NULL;
		mbedtls_pk_free(ClientNodeUsed->private_key[0]);
		rtos_mem_free(ClientNodeUsed->private_key[0]);
		ClientNodeUsed->private_key[0] = NULL;
	}

last_end:
	//reset client_auth_mode, client_pki_index, client_ca_index
	client_auth_mode = SSL_AUTH_MODE_NO;
	client_pki_index = 0;
	client_ca_index = 0;

	if (error_no != 0) {
		delete_list_node(ClientNodeUsed);
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

static void client_start_task(void *param)
{
	rtos_time_delay_ms(100);

	if (param != NULL) {
		client_start(param);
	} else {
		RTK_LOGW(AT_SOCKET_TAG, "client_start_task(): param is NULL!\r\n");
	}

	rtos_task_delete(NULL);
	return;
}


void socket_client_tcp_auto_rcv(void *param)
{
	char *read_buf = NULL;
	int actual_bytes_received = 0;
	struct _node *current_node = (struct _node *)param;

	read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
	if (read_buf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] read_buf malloc failed\r\n");
		goto end;
	}

	while (1) {
		actual_bytes_received = read(current_node->sockfd, read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE);
		if (actual_bytes_received == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] read() = 0; Connection closed\r\n");
			close(current_node->sockfd);
			current_node->sockfd = INVALID_SOCKET_ID;
		} else if (actual_bytes_received < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] Failed to read() = %d\r\n", actual_bytes_received);
			close(current_node->sockfd);
			current_node->sockfd = INVALID_SOCKET_ID;
			goto end;
		}
		at_printf_lock();
		at_printf_indicate("[SKT][DATA][%d][%d]:", current_node->con_id, actual_bytes_received);
		at_printf_data(read_buf, (u32)actual_bytes_received);
		at_printf("\r\n");
		at_printf_unlock();
	}

end:
	rtos_mem_free(read_buf);
	current_node->auto_rcv_task = NULL;
	rtos_task_delete(NULL);
	return;
}


int create_socket_client_tcp(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	int sockfd = INVALID_SOCKET_ID;
	struct sockaddr_in server_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] Failed to create TCP socket\r\n");
		error_no = 2;
		goto end;
	}

	if ((ret = set_socket_tcp_option(sockfd)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] set_socket_tcp_option() failed = %d\r\n", ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = current_node->dst_ip;
	server_addr.sin_port = htons(current_node->dst_port);

	if ((ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] Failed to connect to TCP server(%s:%d), ret = %d\r\n", inet_ntoa(server_addr.sin_addr),
				 current_node->dst_port, ret);
		close(sockfd);
		error_no = 2;
		goto end;
	}
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] Connect to server(%s:%d) successfully\r\n", inet_ntoa(server_addr.sin_addr), current_node->dst_port);

	current_node->sockfd = sockfd;

	if (current_node->auto_rcv == TRUE) {
		if (rtos_task_create(&(current_node->auto_rcv_task), "socket_client_tcp_auto_rcv", socket_client_tcp_auto_rcv, current_node, 1024,
							 ATCMD_LWIP_TASK_PRIORITY) != RTK_SUCCESS) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] rtos_task_create(socket_client_tcp_auto_rcv) failed\r\n");
			close(sockfd);
			error_no = 3;
			goto end;
		}
	}

end:
	return error_no;
}


int create_socket_client(struct _node *current_node)
{
	int error_no = 0;
	if (current_node->protocol == SOCKET_CLIENT_OVER_UDP) {
		//error_no = create_socket_client_udp(current_node);
	} else if (current_node->protocol == SOCKET_CLIENT_OVER_TCP) {
		error_no = create_socket_client_tcp(current_node);
	} else {
		//error_no = create_socket_client_tls(current_node);
	}
	return error_no;
}


//AT+SKTCLIENT=<link_id>,<conn_type>[,<cert_index>],<dst_host>,<dst_port>[,<src_port>],<auto_rcv>
void at_sktclient_1(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = 0, conn_type = 0, cert_index = 0, dst_port = 0, src_port = 0, auto_rcv = 0;
	struct in_addr dst_ipaddr;
#if LWIP_DNS
	struct hostent *server_host = NULL;
#endif

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 8) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0) ||
		(strlen(argv[4]) == 0) || (strlen(argv[5]) == 0) ||
		(strlen(argv[7]) == 0)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}
	link_id = atoi(argv[1]);
	if ((link_id < 0) || (link_id > (NUM_NS - 1))) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <link_id>\r\n");
		error_no = 1;
		goto end;
	}
	if (node_pool[link_id].con_id >= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The <link_id> is in use\r\n");
		error_no = 1;
		goto end;
	}
	conn_type = atoi(argv[2]);
	if ((conn_type < SOCKET_CLIENT_OVER_UDP) || (conn_type > SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <conn_type>\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= SOCKET_CLIENT_OVER_TLS_NO_VERIFY) && (conn_type <= SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		if (argv[3] == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The <cert_index> is NULL for SOCKET_CLIENT_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[3]);
		if (cert_index <= 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <cert_index>\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (conn_type == SOCKET_CLIENT_OVER_UDP) {
		if (argv[6] == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Missing <src_port>\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (inet_aton(argv[4], &dst_ipaddr) == 0) {
#if LWIP_DNS
		server_host = gethostbyname(argv[4]);
		if (server_host != NULL) {
			memcpy(&dst_ipaddr, server_host->h_addr, 4);
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Found %s = %s\r\n", argv[4], inet_ntoa(dst_ipaddr));
		} else
#endif
		{
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Destination host '%s' not found\r\n", argv[4]);
			error_no = 1;
			goto end;
		}
	}
	dst_port = atoi(argv[5]);
	if (dst_port <= 0 || dst_port > 65535) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <dst_port>\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[6])) {
		src_port = atoi(argv[6]);
		if (src_port <= 0 || src_port > 65535) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <src_port>\r\n");
			error_no = 1;
			goto end;
		}
	}
	auto_rcv = atoi(argv[7]);
	if (auto_rcv < 0 || auto_rcv > 1) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Incorrect <auto_rcv>\r\n");
		error_no = 1;
		goto end;
	}

	//Generate the node
	node_pool[link_id].con_id = link_id;
	node_pool[link_id].role = NODE_ROLE_CLIENT;
	node_pool[link_id].cert_index = cert_index;
	node_pool[link_id].auto_rcv = auto_rcv;
	node_pool[link_id].protocol = conn_type;
	node_pool[link_id].dst_ip = dst_ipaddr.s_addr;
	node_pool[link_id].dst_port = dst_port;
	node_pool[link_id].src_port = src_port;

	error_no = create_socket_client(&node_pool[link_id]);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		init_single_node(&node_pool[link_id]);
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

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

	if (arg == NULL) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 4 || argc > 5) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	/* tcp / udp / ssl. */
	if (strlen(argv[mode_idx]) == 0) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] missing mode\r\n");
		error_no = 17;
		goto end;
	}
	mode = atoi(argv[mode_idx]);

	if (mode < NODE_MODE_TCP || mode > NODE_MODE_SSL) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Unknown connection type\r\n");
		error_no = 17;
		goto end;
	}

	if (mode == NODE_MODE_UDP)   {
		if (argc != 5) {
			RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] missing local port\r\n");
			error_no = 11;
			goto end;
		}
	}

	if (strlen(argv[rmt_port_idx]) == 0) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] missing remote port\r\n");
		error_no = 3;
		goto end;
	}
	remote_port = atoi(argv[rmt_port_idx]);
	if (remote_port <= 0 || remote_port > 65535) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Invalid port range\r\n");
		error_no = 3;
		goto end;
	}

	if (inet_aton(argv[addr_idx], &addr) == 0) {
#if LWIP_DNS
		server_host = gethostbyname(argv[addr_idx]);
		if (server_host != NULL) {
			memcpy(&addr, server_host->h_addr, 4);
			RTK_LOGI(AT_SOCKET_TAG, "[+SKTCLIENT] Found %s = %s\r\n", argv[addr_idx], inet_ntoa(addr));
		} else
#endif
		{
			RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Host '%s' Not found\r\n", argv[addr_idx]);
			error_no = 2;
			goto end;
		}
	}

	if (argc == 5 && strlen(argv[lcl_port_idx]) != 0) {
		local_port = atoi(argv[lcl_port_idx]);
		if (local_port <= 0 || local_port > 65535) {
			RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Invalid local port\r\n");
			error_no = 11;
			goto end;
		}
	}

	clientnode = create_list_node(mode, NODE_ROLE_CLIENT);
	if (clientnode == NULL) {
		RTK_LOGW(AT_SOCKET_TAG, "[+SKTCLIENT] Create client node failed\r\n");
		error_no = 4;
		goto end;
	}
	clientnode->port = remote_port;
	clientnode->addr = ntohl(addr.s_addr);
	clientnode->local_port = local_port;

	if (RTK_SUCCESS != rtos_task_create(NULL,
										"client_start_task",
										client_start_task,
										clientnode,
										client_task_stksz,
										ATCMD_LWIP_TASK_PRIORITY)) {
		RTK_LOGI(AT_SOCKET_TAG, "\r\n[+SKTCLIENT] Create task failed.\r\n");
		error_no = 5;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		delete_list_node(clientnode);
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


// Delete a specific TCP/UDP/SSL connection or all connections
void at_sktdel(void *arg)
{
	int con_id = INVALID_CON_ID;
	int error_no = 0;
	struct _node *node = NULL;

	if (arg == NULL) {
		RTK_LOGE(AT_SOCKET_TAG, "[at_sktdel] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	con_id = atoi(arg);
	if (con_id < 0) {
		RTK_LOGE(AT_SOCKET_TAG, "[at_sktdel] The con_id should be non-negative integer\r\n");
		error_no = 2;
		goto end;
	}
	/* 0 - Close all; others - Close specific id. */
	if (con_id == 0) {
		if (atcmd_lwip_is_autorecv_mode()) {
			atcmd_lwip_set_autorecv_mode(FALSE);
		}
		socket_close_all();
	} else {
		node = seek_list_node(con_id);
		if (node != NULL) {
			delete_list_node(node);
		} else {
			RTK_LOGE(AT_SOCKET_TAG, "[at_sktdel] The con_id is not found\r\n");
			error_no = 3;
			goto end;
		}
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


int atcmd_lwip_start_tt_handle(struct _node *curnode, int total_data_len, struct sockaddr_in cli_addr)
{
	int error_no = 0;
	uint8_t *tt_data = NULL;
	int frag_tt_data_len = 0;

	if (total_data_len > 0) {
		tt_data = rtos_mem_zmalloc((total_data_len <= MAX_TT_BUF_LEN ? total_data_len : MAX_TT_BUF_LEN) + 1);
		if (tt_data == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] tt_data malloc failed\r\n");
			error_no = 5;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)total_data_len) < 0)  {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_start() failed\r\n");
			error_no = 6;
			goto end;
		}
		if (total_data_len <= MAX_TT_BUF_LEN) {
			if (atcmd_tt_mode_get(tt_data, (u32)total_data_len) != total_data_len) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_get() failed\r\n");
				error_no = 6;
				goto end;
			}
			if ((error_no = atcmd_lwip_send_data(curnode, tt_data, (u16)total_data_len, cli_addr)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_lwip_send_data() failed\r\n");
				//goto end;
			}
		} else {
			while (total_data_len > 0) {
				frag_tt_data_len = total_data_len <= MAX_TT_BUF_LEN ? total_data_len : MAX_TT_BUF_LEN;
				if (atcmd_tt_mode_get(tt_data, (u32)frag_tt_data_len) != frag_tt_data_len) {
					RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_get() failed\r\n");
					error_no = 6;
					goto end;
				}
				if ((error_no = atcmd_lwip_send_data(curnode, tt_data, (u16)frag_tt_data_len, cli_addr)) != 0) {
					RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_lwip_send_data() failed\r\n");
					//goto end;
				}
				total_data_len -= frag_tt_data_len;
			}
		}
		atcmd_tt_mode_end();
	}

end:
	if (tt_data) {
		rtos_mem_free(tt_data);
		tt_data = NULL;
	}
	return error_no;
}


void at_sktsendraw(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	int con_id = INVALID_CON_ID;
	node *curnode = NULL;
	struct sockaddr_in cli_addr;
	int data_sz = 0;


	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc != 3) && (argc != 5)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	con_id = atoi((char *)argv[1]);
	curnode = seek_list_node(con_id);
	if (curnode == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] <link_id> is not found\r\n");
		error_no = 1;
		goto end;
	}

	data_sz = atoi((char *)argv[2]);
	if (data_sz <= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Data size is incorrect\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol < NODE_MODE_TCP) || (curnode->protocol > NODE_MODE_SSL)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Error protocal=%d\r\n", curnode->protocol);
		error_no = 3;
		goto end;
	}

	if ((curnode->protocol == NODE_MODE_UDP) && (curnode->role == NODE_ROLE_SERVER)) {
		if (argc != 5) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Must contain address and port\r\n");
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
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Failed in inet_aton()\r\n");
			error_no = 4;
			goto end;
		}
	}

	if ((curnode->protocol == NODE_MODE_TCP) || (curnode->protocol == NODE_MODE_SSL)) {
		if (curnode->role == NODE_ROLE_SERVER) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Please use the correct seed link_id for TCP/SSL server\r\n");
			error_no = 1;
			goto end;
		}
	}

	error_no = atcmd_lwip_start_tt_handle(curnode, data_sz, cli_addr);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


void at_sktsend(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	int con_id = INVALID_CON_ID;
	node *curnode = NULL;
	struct sockaddr_in cli_addr;
	int data_sz = 0;
	int data_pos = 0;
	u8 colon_found = 0;  //1 if colon found in argv[argc-1]

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if ((argc != 3) && (argc != 5)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	con_id = atoi((char *)argv[1]);
	curnode = seek_list_node(con_id);
	if (curnode == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] <link_id> is not found\r\n");
		error_no = 1;
		goto end;
	}

	data_sz = atoi((char *)argv[2]);
	if ((data_sz <= 0) || (data_sz >= UART_LOG_CMD_BUFLEN)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Data size is incorrect\r\n");
		error_no = 1;
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
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Incorrect parameter format: missing colon before data\r\n");
		error_no = 1;
		goto end;
	}

	if (data_sz != (int)strlen(&argv[argc - 1][data_pos + 1])) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] <data_size> does not match the actual <data> length\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol < NODE_MODE_TCP) || (curnode->protocol > NODE_MODE_SSL)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Error protocal=%d\r\n", curnode->protocol);
		error_no = 3;
		goto end;
	}

	if ((curnode->protocol == NODE_MODE_UDP) && (curnode->role == NODE_ROLE_SERVER)) {
		if (argc != 5) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Must contain address and port\r\n");
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
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Failed in inet_aton()\r\n");
			error_no = 4;
			goto end;
		}
	}

	if ((curnode->protocol == NODE_MODE_TCP) || (curnode->protocol == NODE_MODE_SSL)) {
		if (curnode->role == NODE_ROLE_SERVER) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Please use the correct seed link_id for TCP/SSL server\r\n");
			error_no = 1;
			goto end;
		}
	}

	error_no = atcmd_lwip_send_data(curnode, (u8 *)&argv[argc - 1][data_pos + 1], data_sz, cli_addr);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] missing con_id\r\n");
		error_no = 2;
		goto end;
	}
	con_id = atoi((char *)argv[1]);
	if (con_id <= 0 || con_id > NUM_NS) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Invalid con_id\r\n");
		error_no = 2;
		goto end;
	}

	if (strlen(argv[2]) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] missing packet_size\r\n");
		error_no = 3;
		goto end;
	}
	packet_size = atoi((char *)argv[2]);
	if (packet_size <= 0 || packet_size > UART_LOG_CMD_BUFLEN) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Invalid packet_size\r\n");
		error_no = 3;
		goto end;
	}

	curnode = seek_list_node(con_id);
	if (curnode == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] <con_id> is not found\r\n");
		error_no = 4;
		goto end;
	}

	if ((curnode->protocol == NODE_MODE_TCP || curnode->protocol == NODE_MODE_SSL) && curnode->role == NODE_ROLE_SERVER) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] TCP/SSL Server must receive data from the seed\r\n");
		error_no = 5;
		goto end;
	}

	if (atcmd_lwip_is_autorecv_mode()) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Command not permitted in auto receive mode \r\n");
		error_no = 6;
		at_printf(ATCMD_ERROR_END_STR, error_no);
		return;
	}

	if (NULL == rx_buffer) {
		rx_buffer = (u8 *)rtos_mem_zmalloc(packet_size + 1);
		if (rx_buffer == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] rx_buffer malloc fail\r\n");
			error_no = 7;
			goto end;
		}
	}

	error_no = atcmd_lwip_receive_data(curnode, rx_buffer, (u16)packet_size, &recv_size, udp_clientaddr, &udp_clientport);

end:
	if (error_no == 0) {
		if (curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER) {
			at_printf("+SKTREAD:%d,%d,%s,%d:", recv_size, con_id, udp_clientaddr, udp_clientport);
		} else {
			at_printf("+SKTREAD:%d,%d:", recv_size, con_id);
		}
		if (recv_size > 0) {
			at_printf("%s", rx_buffer);
		}
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] missing enable\r\n");
		error_no = 1;
		goto end;
	}
	enable = atoi((char *)argv[1]);
	/* Start autorecv. */
	if (enable == 1) {
		if (atcmd_lwip_is_autorecv_mode()) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Already in autorecv mode\r\n");
		} else {
			ret = atcmd_lwip_start_autorecv_task();
			if (ret != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Failed in atcmd_lwip_start_autorecv_task\r\n");
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
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Already leaving autorecv mode\r\n");
		}
	}
	/* Invalid input. */
	else {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktrecvcfg] Invalid enable value\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
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
			} else { /* if (n->protocol == NODE_MODE_SSL) */
				at_printf("ssl,");
			}

			/* Address. */
			addr.s_addr = htonl(n->addr);
			at_printf("address:%s,port:%d,socket_fd:%d\r\n", inet_ntoa(addr), n->port, n->sockfd);
			/* Seed. */
			if (n->nextseed != NULL) {
				struct _node *seed = n;
				do {
					seed = seed->nextseed;
					at_printf("con_id:%d,seed,", seed->con_id);
					if (seed->protocol == NODE_MODE_TCP) {
						at_printf("tcp,");
					} else if (n->protocol == NODE_MODE_UDP) {
						at_printf("udp,");
					} else { /* if(n->protocol == NODE_MODE_SSL) */
						at_printf("ssl,");
					}

					addr.s_addr = htonl(seed->addr);
					at_printf("address:%s,port:%d,socket_fd:%d\r\n", inet_ntoa(addr), seed->port, seed->sockfd);
				} while (seed->nextseed != NULL);
			}
		}
		n = n->next;
	}

	at_printf(ATCMD_OK_END_STR);
}

#if ENABLE_TCPIP_AUTOLINK
void at_sktautolink(void *arg)
{
	int argc, error_no = 0, enable = 0;
	char *argv[MAX_ARGC] = {0};
	struct _atcmd_lwip_conn_info cur_conn = {0};
	struct _node *cur_node = mainlist->next;

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktautolink] Invalid parameter\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2 || strlen(argv[1]) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktautolink] Invalid parameter number\r\n");
		error_no = 2;
		goto end;
	}

	if (enable != 0 && cur_node == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktautolink] node should not be NULL when enable\r\n");
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
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif


log_item_t at_socket_items[ ] = {
	{"+SKTCFG", at_sktcfg, {NULL, NULL}},
	{"+SKTQUERY", at_sktquery, {NULL, NULL}},
	{"+SKTSERVER", at_sktserver, {NULL, NULL}},
	{"+SKTCLIENT", at_sktclient, {NULL, NULL}},
	{"+SKTDEL", at_sktdel, {NULL, NULL}},
	{"+SKTSENDRAW", at_sktsendraw, {NULL, NULL}},
	{"+SKTSEND", at_sktsend, {NULL, NULL}},
	{"+SKTREAD", at_sktread, {NULL, NULL}},
	{"+SKTRECVCFG", at_sktrecvcfg, {NULL, NULL}},
	{"+SKTSTATE", at_sktstate, {NULL, NULL}},
#if ENABLE_TCPIP_AUTOLINK
	{"+SKTAUTOLINK", at_sktautolink, {NULL, NULL}},
#endif
};

void print_socket_at(void)
{
	int i, cmdSize;

	cmdSize = sizeof(at_socket_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_socket_items[i].log_cmd);
	}
}

void at_socket_init(void)
{
	init_node_pool();
	mainlist = create_list_node(-1, -1);

	atcmd_service_add_table(at_socket_items, sizeof(at_socket_items) / sizeof(at_socket_items[0]));
}

#endif /* CONFIG_ATCMD_SOCKET */
#endif /* CONFIG_LWIP_LAYER */