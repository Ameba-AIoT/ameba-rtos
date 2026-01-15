/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_SOCKET) && (CONFIG_ATCMD_SOCKET == 1)
#include "atcmd_service.h"
#include "atcmd_sockets.h"

static const char *const AT_SOCKET_TAG = "AT_SKT";

static struct _node node_pool[MEMP_NUM_NETCONN];

// Socket global configuration
static int skt_sendtimeout = 0;
static int skt_recvtimeout = 0;
static int tcp_nodelay = 0;
static int skt_keepalive = 0;
static int tcp_keepalive_idle = 0;
static int tcp_keepalive_interval = 0;
static int tcp_keepalive_count = 0;



static void atcmd_ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
	UNUSED(ctx);
	UNUSED(level);
	UNUSED(file);
	UNUSED(line);

	RTK_LOGA(AT_SOCKET_TAG, "%s", str);

}


static int atcmd_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
	return 0;
}


static void init_node_pool(void)
{
	int i;
	memset(node_pool, 0, sizeof(node_pool));
	for (i = 0; i < MEMP_NUM_NETCONN; i++) {
		node_pool[i].link_id = INVALID_LINK_ID;
		node_pool[i].role = NODE_ROLE_INVALID;
		node_pool[i].sockfd = INVALID_SOCKET_ID;
		node_pool[i].protocol = SOCKET_PROTOCOL_INVALID;
	}
}


static void init_single_node(struct _node *socket_node)
{
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);
	memset(socket_node, 0, sizeof(struct _node));
	socket_node->link_id = INVALID_LINK_ID;
	socket_node->role = NODE_ROLE_INVALID;
	socket_node->sockfd = INVALID_SOCKET_ID;
	socket_node->protocol = SOCKET_PROTOCOL_INVALID;
	SYS_ARCH_UNPROTECT(lev);
}


static int search_available_node(void)
{
	for (int i = 0; i < MEMP_NUM_NETCONN; i++) {
		if (node_pool[i].link_id == INVALID_LINK_ID) {
			return i;
		}
	}
	return -1;
}


void hang_seednode(struct _node *main_node, struct _node *seed_node)
{
	struct _node *n = main_node;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);

	while (n->nextseed != NULL) {
		n = n->nextseed;
	}

	seed_node->prevnode = n;
	n->nextseed = seed_node;
	SYS_ARCH_UNPROTECT(lev);
	return;
}


//AT+SKTCFG=[<so_sndtimeo>],[<so_rcvtimeo>],[<tcp_nodelay>],[<so_keepalive>],[<tcp_keepidle>],[<tcp_keepintvl>],[<tcp_keepcnt>]
void at_sktcfg(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int so_sndtimeo = 0, so_rcvtimeo = 0, input_tcp_nodelay = 0, so_keepalive = 0, tcp_keepidle = 0, tcp_keepintvl = 0, tcp_keepcnt = 0;

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
		if ((so_sndtimeo = atoi(argv[1])) < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The <so_sndtimeo> should be >= 0\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[2])) {
		if ((so_rcvtimeo = atoi(argv[2])) < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The <so_rcvtimeo> should be >= 0\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[3])) {
		input_tcp_nodelay = atoi(argv[3]);
		if ((input_tcp_nodelay != 0) && (input_tcp_nodelay != 1)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The <tcp_nodelay>'s possible values are 0 and 1\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[4])) {
		so_keepalive = atoi(argv[4]);
		if ((so_keepalive != 0) && (so_keepalive != 1)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The <so_keepalive>'s possible values are 0 and 1\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[5])) {
		tcp_keepidle = atoi(argv[5]);
		if ((tcp_keepidle < 0) || (tcp_keepidle > 7200)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The range of <tcp_keepidle> is [0, 7200]\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[6])) {
		tcp_keepintvl = atoi(argv[6]);
		if ((tcp_keepintvl < 0) || (tcp_keepintvl > 75)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The range of <tcp_keepintvl> is [0, 75]\r\n");
			error_no = 1;
			goto end;
		}
	}
	if (strlen(argv[7])) {
		tcp_keepcnt = atoi(argv[7]);
		if ((tcp_keepcnt < 0) || (tcp_keepcnt > 10)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktcfg] The range of <tcp_keepcnt> is [0, 10]\r\n");
			error_no = 1;
			goto end;
		}
	}

	skt_sendtimeout = so_sndtimeo;
	skt_recvtimeout = so_rcvtimeo;
	tcp_nodelay = input_tcp_nodelay;
	skt_keepalive = so_keepalive;
	tcp_keepalive_idle = tcp_keepidle;
	tcp_keepalive_interval = tcp_keepintvl;
	tcp_keepalive_count = tcp_keepcnt;

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


//AT+SKTQUERY
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
		struct timeval timeout;
		timeout.tv_sec  = skt_sendtimeout / 1000;
		timeout.tv_usec = (skt_sendtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_SNDTIMEO) failed\r\n");
			goto end;
		}
	}
#endif
#if LWIP_SO_RCVTIMEO
	if (skt_recvtimeout > 0) {
		struct timeval timeout;
		timeout.tv_sec  = skt_recvtimeout / 1000;
		timeout.tv_usec = (skt_recvtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
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
		struct timeval timeout;
		timeout.tv_sec  = skt_sendtimeout / 1000;
		timeout.tv_usec = (skt_sendtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[set_socket_tcp_option] setsockopt(SO_SNDTIMEO) failed\r\n");
			goto end;
		}
	}
#endif
#if LWIP_SO_RCVTIMEO
	if (skt_recvtimeout > 0) {
		struct timeval timeout;
		timeout.tv_sec  = skt_recvtimeout / 1000;
		timeout.tv_usec = (skt_recvtimeout % 1000) * 1000;
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
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


void socket_server_udp_auto_rcv_data(void *param)
{
	char *read_buf = NULL;
	struct timeval timeout;
	fd_set read_fds;
	int ret = 0;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int actual_bytes_received = 0;
	struct _node *current_node = (struct _node *)param;

	read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
	if (read_buf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[socket_server_udp_auto_rcv_data] read_buf malloc failed\r\n");
		goto end;
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);

		ret = select(current_node->sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_udp_auto_rcv_data] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			actual_bytes_received = recvfrom(current_node->sockfd, read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
			if (actual_bytes_received < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_udp_auto_rcv_data] Failed to read() = %d\r\n", actual_bytes_received);
				close(current_node->sockfd);
				current_node->sockfd = INVALID_SOCKET_ID;
				goto end;
			} else {
				at_printf_lock();
				at_printf_indicate("[SKT][DATA][%d][%d][%s][%d]:", current_node->link_id, actual_bytes_received, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				at_printf_data(read_buf, (u32)actual_bytes_received);
				at_printf("\r\n");
				at_printf_unlock();
			}
		}
	}

end:
	rtos_mem_free(read_buf);
	read_buf = NULL;
	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_server_udp_auto_rcv_data)\r\n");
	rtos_sema_give(current_node->del_task_sema);
	rtos_task_delete(NULL);
	return;
}


int create_socket_server_udp(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	int sockfd = INVALID_SOCKET_ID;
	int optval = 1;
	struct sockaddr_in local_server_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] Failed to create UDP socket\r\n");
		error_no = 4;
		goto end;
	}
	if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] setsockopt(SO_REUSEADDR) failed = %d\r\n", ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}

	if ((ret = set_socket_udp_option(sockfd)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] set_socket_udp_option() failed = %d\r\n", ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}
#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
	if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] setsockopt(SO_BROADCAST) failed = %d\r\n", ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}
#endif

	memset(&local_server_addr, 0, sizeof(local_server_addr));
	local_server_addr.sin_family = AF_INET;
	local_server_addr.sin_addr.s_addr = INADDR_ANY;
	local_server_addr.sin_port = htons(current_node->src_port);

	if ((ret = bind(sockfd, (struct sockaddr *)&local_server_addr, sizeof(local_server_addr))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] Failed to bind() to port: %d, ret = %d\r\n", current_node->src_port, ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}

	current_node->sockfd = sockfd;

	if (current_node->auto_rcv == TRUE) {
		if (rtos_task_create(NULL, "socket_server_udp_auto_rcv_data", socket_server_udp_auto_rcv_data, current_node,
							 1024 * 3, ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] rtos_task_create(socket_server_udp_auto_rcv_data) failed\r\n");
			close(sockfd);
			error_no = 5;
			goto end;
		}
		rtos_sema_create_binary(&(current_node->del_task_sema));
	}

	struct in_addr local_addr;
	local_addr.s_addr = current_node->src_ip;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_udp] The UDP server created on STA(%s:%d) successfully\r\n", inet_ntoa(local_addr), current_node->src_port);

end:
	if (error_no != 0) {
		init_single_node(current_node);
	}
	return error_no;
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
	struct timeval timeout;

	client_addr_len = sizeof(client_addr);
	max_fd = current_node->sockfd;

	if (current_node->auto_rcv == TRUE) {
		read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
		if (read_buf == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] read_buf malloc failed\r\n");
			goto end;
		}
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);
		seednode = current_node->nextseed;
		while (seednode) {
			if (seednode->sockfd >= 0) {
				FD_SET(seednode->sockfd, &read_fds);
			}
			seednode = seednode->nextseed;
		}
		ret = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
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
			node_pool[index].link_id = index;
			node_pool[index].role = NODE_ROLE_SEED;
			node_pool[index].sockfd = new_client_sockfd;
			node_pool[index].protocol = SOCKET_SERVER_OVER_TCP;
			node_pool[index].dst_ip = client_addr.sin_addr.s_addr;
			node_pool[index].dst_port = ntohs(client_addr.sin_port);
			SYS_ARCH_UNPROTECT(lev);

			hang_seednode(current_node, &node_pool[index]);

			at_printf_indicate("[SKT][EVENT]: A client[link_id:%d,seed,tcp,dst_address:%s,dst_port:%d] connected to server[link_id:%d]\r\n",
							   node_pool[index].link_id, inet_ntoa(client_addr.sin_addr), node_pool[index].dst_port, current_node->link_id);

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

					if (actual_bytes_received > 0) {
						at_printf_lock();
						at_printf_indicate("[SKT][DATA][%d][%d]:", seednode->link_id, actual_bytes_received);
						at_printf_data(read_buf, (u32)actual_bytes_received);
						at_printf("\r\n");
						at_printf_unlock();
					} else {
						if (actual_bytes_received == 0) {
							RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] read() = 0; Connection closed\r\n");
						} else if (actual_bytes_received < 0) {
							RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tcp_auto_rcv_client_and_data] Failed to read() = %d\r\n", actual_bytes_received);
						}
						FD_CLR(seednode->sockfd, &read_fds);
						close(seednode->sockfd);
						seednode->sockfd = INVALID_SOCKET_ID;
					}
				}
				seednode = seednode->nextseed;
			}
		}
	}

end:
	if (current_node->auto_rcv == TRUE) {
		rtos_mem_free(read_buf);
		read_buf = NULL;
	}
	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_server_tcp_auto_rcv_client_and_data)\r\n");
	rtos_sema_give(current_node->del_task_sema);
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

	if (rtos_task_create(NULL, "socket_server_tcp_auto_rcv_client_and_data", socket_server_tcp_auto_rcv_client_and_data, current_node,
						 1024 * 3, ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] rtos_task_create(socket_server_tcp_auto_rcv_client_and_data) failed\r\n");
		close(sockfd);
		error_no = 3;
		goto end;
	}

	rtos_sema_create_binary(&(current_node->del_task_sema));

	struct in_addr local_addr;
	local_addr.s_addr = current_node->src_ip;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tcp] The TCP server created on STA(%s:%d) successfully\r\n", inet_ntoa(local_addr), current_node->src_port);

end:
	if (error_no != 0) {
		init_single_node(current_node);
	}
	return error_no;
}


void socket_server_tls_auto_rcv_client_and_data(void *param)
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
	struct timeval timeout;
	mbedtls_ssl_context *ssl = NULL;

	client_addr_len = sizeof(client_addr);
	max_fd = current_node->sockfd;

	if (current_node->auto_rcv == TRUE) {
		read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
		if (read_buf == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] read_buf malloc failed\r\n");
			goto end;
		}
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);
		seednode = current_node->nextseed;
		while (seednode) {
			if (seednode->sockfd >= 0) {
				FD_SET(seednode->sockfd, &read_fds);
			}
			seednode = seednode->nextseed;
		}

		ret = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			new_client_sockfd = accept(current_node->sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] accept() = %d\r\n", new_client_sockfd);
			if (new_client_sockfd < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] Failed to accept() = %d\r\n", new_client_sockfd);
				continue;
			}
			index = search_available_node();
			if (index < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] Failed to search_available_node()\r\n");
				close(new_client_sockfd);
				continue;
			}
			ssl = (mbedtls_ssl_context *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_context));
			if (ssl == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] ssl malloc fail\r\n");
				close(new_client_sockfd);
				continue;
			}

			mbedtls_ssl_init(ssl);

			if ((ret = mbedtls_ssl_setup(ssl, current_node->conf)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] mbedtls_ssl_setup() failed ret = -0x%04x\r\n", -ret);
				close(new_client_sockfd);
				rtos_mem_free(ssl);
				ssl = NULL;
				continue;
			}

			SYS_ARCH_DECL_PROTECT(lev);
			SYS_ARCH_PROTECT(lev);
			node_pool[index].link_id = index;
			node_pool[index].role = NODE_ROLE_SEED;
			node_pool[index].cert_index = current_node->cert_index;
			node_pool[index].sockfd = new_client_sockfd;
			node_pool[index].protocol = current_node->protocol;
			node_pool[index].dst_ip = client_addr.sin_addr.s_addr;
			node_pool[index].dst_port = ntohs(client_addr.sin_port);
			node_pool[index].ssl = ssl;
			node_pool[index].conf = current_node->conf;
			node_pool[index].ca_crt = current_node->ca_crt;
			node_pool[index].cert_crt = current_node->cert_crt;
			node_pool[index].private_key = current_node->private_key;
			mbedtls_ssl_set_bio(node_pool[index].ssl, &node_pool[index].sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);
			SYS_ARCH_UNPROTECT(lev);

			RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] Performing the SSL/TLS handshake...\r\n");

			if ((ret = mbedtls_ssl_handshake(node_pool[index].ssl)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] mbedtls_ssl_handshake() failed ret = -0x%04x\r\n", -ret);
				mbedtls_ssl_free(node_pool[index].ssl);
				close(node_pool[index].sockfd);
				node_pool[index].sockfd = INVALID_SOCKET_ID;
				rtos_mem_free(node_pool[index].ssl);
				node_pool[index].ssl = NULL;
				init_single_node(&node_pool[index]);
				continue;
			}

			hang_seednode(current_node, &node_pool[index]);

			at_printf_indicate("[SKT][EVENT]: A client[link_id:%d,seed,tls,dst_address:%s,dst_port:%d] connected to server[link_id:%d]\r\n",
							   node_pool[index].link_id, inet_ntoa(client_addr.sin_addr), node_pool[index].dst_port, current_node->link_id);

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
					actual_bytes_received = mbedtls_ssl_read(seednode->ssl, (unsigned char *)read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE);

					if (actual_bytes_received > 0) {
						at_printf_lock();
						at_printf_indicate("[SKT][DATA][%d][%d]:", seednode->link_id, actual_bytes_received);
						at_printf_data(read_buf, (u32)actual_bytes_received);
						at_printf("\r\n");
						at_printf_unlock();
					} else {
						if ((actual_bytes_received == 0) || (actual_bytes_received == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
							RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] mbedtls_ssl_read() = 0; Connection closed\r\n");
						} else if (actual_bytes_received < 0) {
							if (actual_bytes_received == MBEDTLS_ERR_SSL_WANT_READ || actual_bytes_received == MBEDTLS_ERR_SSL_WANT_WRITE) {
								RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] MBEDTLS_ERR_SSL_WANT_READ / MBEDTLS_ERR_SSL_WANT_WRITE\r\n");
								continue;
							}
							RTK_LOGI(AT_SOCKET_TAG, "[socket_server_tls_auto_rcv_client_and_data] Failed to mbedtls_ssl_read() = -0x%04x\r\n", -actual_bytes_received);
						}
						FD_CLR(seednode->sockfd, &read_fds);
						mbedtls_ssl_free(seednode->ssl);
						close(seednode->sockfd);
						seednode->sockfd = INVALID_SOCKET_ID;
						rtos_mem_free(seednode->ssl);
						seednode->ssl = NULL;
					}
				}
				seednode = seednode->nextseed;
			}
		}
	}

end:
	if (current_node->auto_rcv == TRUE) {
		rtos_mem_free(read_buf);
		read_buf = NULL;
	}
	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_server_tls_auto_rcv_client_and_data)\r\n");
	rtos_sema_give(current_node->del_task_sema);
	rtos_task_delete(NULL);
	return;
}


int create_socket_server_tls(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	mbedtls_net_context tls_server_fd;
	int cert_size = 0, pk_size = 0;
	char *tls_server_cert_pem = NULL;	// read server cert
	char *tls_server_pkey_pem = NULL;	// read server private key
	char *tls_server_ca_pem = NULL;	// read server CA
	char src_port_str[6] = {0};


	if (((cert_size = atcmd_get_ssl_certificate_size(SERVER_CERT, current_node->cert_index)) <= 0) ||
		((pk_size = atcmd_get_ssl_certificate_size(SERVER_KEY, current_node->cert_index)) <= 0))  {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] Failed to get SERVER_CERT_%d / SERVER_KEY_%d size\r\n", current_node->cert_index, current_node->cert_index);
		error_no = 6;
		goto end;
	}
	tls_server_cert_pem = rtos_mem_zmalloc(cert_size + 1);
	if (tls_server_cert_pem == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] tls_server_cert_pem malloc fail\r\n");
		error_no = 7;
		goto end;
	}
	ret = atcmd_get_ssl_certificate(tls_server_cert_pem, SERVER_CERT, current_node->cert_index);
	if (ret > 0) {
		tls_server_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
		if (tls_server_pkey_pem == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] tls_server_pkey_pem malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(tls_server_pkey_pem, SERVER_KEY, current_node->cert_index);
		if (ret <= 0)  {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] SERVER_KEY_%d read failed\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
	} else if (ret == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] NO SERVER_CERT_%d!\r\n", current_node->cert_index);
		error_no = 6;
		goto end;
	} else {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] SERVER_CERT_%d read failed\r\n", current_node->cert_index);
		error_no = 6;
		goto end;
	}

	if (current_node->protocol == SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT) {
		if ((cert_size = atcmd_get_ssl_certificate_size(SERVER_CA, current_node->cert_index)) <= 0)  {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] Failed to get SERVER_CA_%d size\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
		tls_server_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (tls_server_ca_pem == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] tls_server_ca_pem malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(tls_server_ca_pem, SERVER_CA, current_node->cert_index);
		if (ret == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] NO SERVER_CA_%d\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		} else if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] SERVER_CA_%d read failed\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
	}

	current_node->conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
	if (current_node->conf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] current_node->conf malloc fail\r\n");
		error_no = 7;
		goto end;
	}

	mbedtls_ssl_config_init(current_node->conf);

	if ((ret = mbedtls_ssl_config_defaults(current_node->conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_ssl_config_defaults() failed ret = -0x%04x\r\n", -ret);
		error_no = 8;
		goto end;
	}

	mbedtls_ssl_conf_authmode(current_node->conf, MBEDTLS_SSL_VERIFY_NONE);

	current_node->cert_crt = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
	if (current_node->cert_crt == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] current_node->cert_crt malloc fail\r\n");
		error_no = 7;
		goto end;
	}
	current_node->private_key = (mbedtls_pk_context *)rtos_mem_zmalloc(sizeof(mbedtls_pk_context));
	if (current_node->private_key == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] current_node->private_key malloc fail\r\n");
		error_no = 7;
		goto end;
	}
	mbedtls_x509_crt_init(current_node->cert_crt);
	mbedtls_pk_init(current_node->private_key);
	if ((ret = mbedtls_x509_crt_parse(current_node->cert_crt, (const unsigned char *)tls_server_cert_pem, strlen(tls_server_cert_pem) + 1)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_x509_crt_parse() failed ret = -0x%04x\r\n", -ret);
		error_no = 9;
		goto end;
	}
	rtos_mem_free(tls_server_cert_pem);
	tls_server_cert_pem = NULL;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] server_cert_%d.crt parse successful\r\n", current_node->cert_index);

	if ((ret = mbedtls_pk_parse_key(current_node->private_key, (const unsigned char *)tls_server_pkey_pem, strlen(tls_server_pkey_pem) + 1, NULL, 0, NULL,
									NULL)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_pk_parse_key() failed ret = -0x%04x\r\n", -ret);
		error_no = 9;
		goto end;
	}
	rtos_mem_free(tls_server_pkey_pem);
	tls_server_pkey_pem = NULL;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] server_key_%d.key parse successful\r\n", current_node->cert_index);

	if ((ret = mbedtls_ssl_conf_own_cert(current_node->conf, current_node->cert_crt, current_node->private_key)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_ssl_conf_own_cert() failed ret = -0x%04x\r\n", -ret);
		error_no = 10;
		goto end;
	}

	if (current_node->protocol == SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT) {
		current_node->ca_crt = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
		if (current_node->ca_crt == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] current_node->ca_crt malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		mbedtls_x509_crt_init(current_node->ca_crt);
		if ((ret = mbedtls_x509_crt_parse(current_node->ca_crt, (const unsigned char *)tls_server_ca_pem, strlen(tls_server_ca_pem) + 1)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_x509_crt_parse() failed ret = -0x%04x\r\n", -ret);
			error_no = 9;
			goto end;
		}
		rtos_mem_free(tls_server_ca_pem);
		tls_server_ca_pem = NULL;
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] server_ca_%d.crt parse successful\r\n", current_node->cert_index);

		mbedtls_ssl_conf_ca_chain(current_node->conf, current_node->ca_crt, NULL);
		mbedtls_ssl_conf_authmode(current_node->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	}

	mbedtls_ssl_conf_rng(current_node->conf, atcmd_ssl_random, NULL);
#ifdef MBEDTLS_DEBUG_C
	mbedtls_ssl_conf_dbg(current_node->conf, atcmd_ssl_debug, NULL);
	mbedtls_debug_set_threshold(0);
#endif

	//Create socket and bind socket to local port
	DiagSnPrintf(src_port_str, sizeof(src_port_str), "%d", current_node->src_port);
	mbedtls_net_init(&tls_server_fd);

	if ((ret = mbedtls_net_bind(&tls_server_fd, NULL, src_port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] mbedtls_net_bind() failed ret = -0x%04x\r\n", -ret);
		error_no = 11;
		goto end;
	}
	current_node->sockfd = tls_server_fd.fd;

	if (rtos_task_create(NULL, "socket_server_tls_auto_rcv_client_and_data", socket_server_tls_auto_rcv_client_and_data, current_node,
						 1024 * 7, ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] rtos_task_create(socket_server_tls_auto_rcv_client_and_data) failed\r\n");
		error_no = 12;
		goto end;
	}

	rtos_sema_create_binary(&(current_node->del_task_sema));

	struct in_addr local_addr;
	local_addr.s_addr = current_node->src_ip;
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_server_tls] The SSL/TLS server created on STA(%s:%d) successfully\r\n", inet_ntoa(local_addr), current_node->src_port);


end:
	if (tls_server_cert_pem) {
		rtos_mem_free(tls_server_cert_pem);
		tls_server_cert_pem = NULL;
	}
	if (tls_server_pkey_pem) {
		rtos_mem_free(tls_server_pkey_pem);
		tls_server_pkey_pem = NULL;
	}
	if (tls_server_ca_pem) {
		rtos_mem_free(tls_server_ca_pem);
		tls_server_ca_pem = NULL;
	}

	if (error_no != 0) {
		mbedtls_net_free(&tls_server_fd);

		if (current_node->ca_crt) {
			mbedtls_x509_crt_free(current_node->ca_crt);
			rtos_mem_free(current_node->ca_crt);
			current_node->ca_crt = NULL;
		}
		if (current_node->cert_crt) {
			mbedtls_x509_crt_free(current_node->cert_crt);
			rtos_mem_free(current_node->cert_crt);
			current_node->cert_crt = NULL;
		}
		if (current_node->private_key) {
			mbedtls_pk_free(current_node->private_key);
			rtos_mem_free(current_node->private_key);
			current_node->private_key = NULL;
		}

		if (current_node->conf) {
			mbedtls_ssl_config_free(current_node->conf);
			rtos_mem_free(current_node->conf);
			current_node->conf = NULL;
		}

		init_single_node(current_node);
	}
	return error_no;
}


int create_socket_server(struct _node *current_node)
{
	int error_no = 0;
	if (current_node->protocol == SOCKET_SERVER_OVER_UDP) {
		error_no = create_socket_server_udp(current_node);
	} else if (current_node->protocol == SOCKET_SERVER_OVER_TCP) {
		error_no = create_socket_server_tcp(current_node);
	} else {
		error_no = create_socket_server_tls(current_node);
	}
	return error_no;
}


//AT+SKTSERVER=<link_id>,<conn_type>[,<cert_index>],<src_port>,<auto_rcv>
void at_sktserver(void *arg)
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
	if ((link_id < 0) || (link_id > (MEMP_NUM_NETCONN - 1))) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	if (node_pool[link_id].link_id >= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The <link_id> is in use\r\n");
		error_no = 1;
		goto end;
	}
	conn_type = atoi(argv[2]);
	if ((conn_type < SOCKET_SERVER_OVER_UDP) || (conn_type > SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The range of <conn_type> is [0, 3]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= SOCKET_SERVER_OVER_TLS) && (conn_type <= SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT)) {
		if (strlen(argv[3]) == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] Missing <cert_index> for SOCKET_SERVER_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[3]);
		if (cert_index <= 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The <cert_index> should be greater than or equal to 1\r\n");
			error_no = 1;
			goto end;
		}
		if ((atcmd_get_ssl_certificate_size(SERVER_CERT, cert_index) <= 0) || (atcmd_get_ssl_certificate_size(SERVER_KEY, cert_index) <= 0))  {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] SERVER_CERT_%d/SERVER_KEY_%d does not exist in flash\r\n", cert_index, cert_index);
			error_no = 1;
			goto end;
		}
		if (conn_type == SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT) {
			if (atcmd_get_ssl_certificate_size(SERVER_CA, cert_index) <= 0)  {
				RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] SERVER_CA_%d does not exist in flash\r\n", cert_index);
				error_no = 1;
				goto end;
			}
		}
	}
	src_port = atoi(argv[4]);
	if (src_port <= 0 || src_port > 65535) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The range of <src_port> is [1, 65535]\r\n");
		error_no = 1;
		goto end;
	}
	auto_rcv = atoi(argv[5]);
	if (auto_rcv < 0 || auto_rcv > 1) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktserver] The <auto_rcv>'s possible values are 0 and 1\r\n");
		error_no = 1;
		goto end;
	}

	//Generate the node
	node_pool[link_id].link_id = link_id;
	node_pool[link_id].role = NODE_ROLE_SERVER;
	node_pool[link_id].cert_index = cert_index;
	node_pool[link_id].auto_rcv = auto_rcv;
	node_pool[link_id].protocol = conn_type;
	node_pool[link_id].src_port = src_port;
	local_ip = LwIP_GetIP(NETIF_WLAN_STA_INDEX);
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


void socket_client_udp_auto_rcv(void *param)
{
	char *read_buf = NULL;
	struct timeval timeout;
	fd_set read_fds;
	int ret = 0;
	struct sockaddr_in server_addr;
	socklen_t addr_len = sizeof(server_addr);
	int actual_bytes_received = 0;
	struct _node *current_node = (struct _node *)param;

	read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
	if (read_buf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[socket_client_udp_auto_rcv] read_buf malloc failed\r\n");
		goto end;
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);

		ret = select(current_node->sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_client_udp_auto_rcv] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			actual_bytes_received = recvfrom(current_node->sockfd, read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
			if (actual_bytes_received < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_client_udp_auto_rcv] Failed to read() = %d\r\n", actual_bytes_received);
				close(current_node->sockfd);
				current_node->sockfd = INVALID_SOCKET_ID;
				goto end;
			} else {
				at_printf_lock();
				at_printf_indicate("[SKT][DATA][%d][%d][%s][%d]:", current_node->link_id, actual_bytes_received, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
				at_printf_data(read_buf, (u32)actual_bytes_received);
				at_printf("\r\n");
				at_printf_unlock();
			}
		}
	}

end:
	rtos_mem_free(read_buf);
	read_buf = NULL;
	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_client_udp_auto_rcv)\r\n");
	rtos_sema_give(current_node->del_task_sema);
	rtos_task_delete(NULL);
	return;
}


int create_socket_client_udp(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	int sockfd = INVALID_SOCKET_ID;
	struct sockaddr_in client_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_udp] Failed to create UDP socket\r\n");
		error_no = 4;
		goto end;
	}

	if ((ret = set_socket_udp_option(sockfd)) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_udp] set_socket_udp_option() failed = %d\r\n", ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}

#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
	int optval = 1;
	if ((current_node->dst_ip == htonl(INADDR_BROADCAST)) || (current_node->dst_ip == htonl(INADDR_ANY))) {
		if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval))) < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_udp] setsockopt(SO_BROADCAST) failed = %d\r\n", ret);
			close(sockfd);
			error_no = 4;
			goto end;
		}
	}
#endif

	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = INADDR_ANY;
	client_addr.sin_port = htons(current_node->src_port);
	if ((ret = bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr))) < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_udp] bind() to client(%s:%d) failed = %d\r\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, ret);
		close(sockfd);
		error_no = 4;
		goto end;
	}

	current_node->sockfd = sockfd;

	if (current_node->auto_rcv == TRUE) {
		if (rtos_task_create(NULL, "socket_client_udp_auto_rcv", socket_client_udp_auto_rcv, current_node, 1024 * 3,
							 ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_udp] rtos_task_create(socket_client_udp_auto_rcv) failed\r\n");
			close(sockfd);
			error_no = 5;
			goto end;
		}
		rtos_sema_create_binary(&(current_node->del_task_sema));
	}

end:
	if (error_no != 0) {
		init_single_node(current_node);
	}
	return error_no;
}


void socket_client_tcp_auto_rcv(void *param)
{
	char *read_buf = NULL;
	struct timeval timeout;
	fd_set read_fds;
	int ret = 0;
	int actual_bytes_received = 0;
	struct _node *current_node = (struct _node *)param;

	read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
	if (read_buf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] read_buf malloc failed\r\n");
		goto end;
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);

		ret = select(current_node->sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			actual_bytes_received = read(current_node->sockfd, read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE);
			if (actual_bytes_received == 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] read() = 0; Connection closed\r\n");
				goto end;
			} else if (actual_bytes_received < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tcp_auto_rcv] Failed to read() = %d\r\n", actual_bytes_received);
				goto end;
			} else {
				at_printf_lock();
				at_printf_indicate("[SKT][DATA][%d][%d]:", current_node->link_id, actual_bytes_received);
				at_printf_data(read_buf, (u32)actual_bytes_received);
				at_printf("\r\n");
				at_printf_unlock();
			}
		}
	}

end:
	rtos_mem_free(read_buf);
	read_buf = NULL;

	close(current_node->sockfd);
	current_node->sockfd = INVALID_SOCKET_ID;

	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_client_tcp_auto_rcv)\r\n");
	rtos_sema_give(current_node->del_task_sema);
	rtos_task_delete(NULL);
	return;
}


int create_socket_client_tcp(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	int sockfd = INVALID_SOCKET_ID;
	struct sockaddr_in server_addr;
	struct sockaddr_in local_addr;
	socklen_t addr_len = sizeof(local_addr);

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

	if ((ret = lwip_getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_len)) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] Local client STA(%s:%d)\r\n", inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port));
	} else {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] lwip_getsockname() failed = %d\r\n", ret);
	}
	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] Connect to server(%s:%d) successfully\r\n", inet_ntoa(server_addr.sin_addr), current_node->dst_port);

	current_node->src_ip = local_addr.sin_addr.s_addr;
	current_node->src_port = ntohs(local_addr.sin_port);
	current_node->sockfd = sockfd;

	if (current_node->auto_rcv == TRUE) {
		if (rtos_task_create(NULL, "socket_client_tcp_auto_rcv", socket_client_tcp_auto_rcv, current_node, 1024 * 3,
							 ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] rtos_task_create(socket_client_tcp_auto_rcv) failed\r\n");
			close(sockfd);
			error_no = 3;
			goto end;
		}
		rtos_sema_create_binary(&(current_node->del_task_sema));
	}

end:
	if (error_no != 0) {
		init_single_node(current_node);
	}
	return error_no;
}


void socket_client_tls_auto_rcv(void *param)
{
	char *read_buf = NULL;
	struct timeval timeout;
	fd_set read_fds;
	int ret = 0;
	int actual_bytes_received = 0;
	struct _node *current_node = (struct _node *)param;
	mbedtls_net_context tls_client_fd;

	mbedtls_net_init(&tls_client_fd);

	read_buf = rtos_mem_zmalloc(AT_SOCKET_RECEIVE_BUFFER_SIZE + 1);
	if (read_buf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tls_auto_rcv] read_buf malloc failed\r\n");
		goto end;
	}

	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	while (1) {
		if (TRUE == current_node->stop_task) {
			break;
		}
		FD_ZERO(&read_fds);
		FD_SET(current_node->sockfd, &read_fds);

		ret = select(current_node->sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tls_auto_rcv] Failed to select() = %d\r\n", ret);
			continue;
		}
		if (ret == 0) {
			continue;
		}

		if ((current_node->sockfd >= 0) && FD_ISSET(current_node->sockfd, &read_fds)) {
			actual_bytes_received = mbedtls_ssl_read(current_node->ssl, (unsigned char *)read_buf, AT_SOCKET_RECEIVE_BUFFER_SIZE);
			if ((actual_bytes_received == 0) || (actual_bytes_received == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
				RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tls_auto_rcv] mbedtls_ssl_read() = 0; Connection closed\r\n");
				goto end;
			} else if (actual_bytes_received < 0) {
				if (actual_bytes_received == MBEDTLS_ERR_SSL_WANT_READ || actual_bytes_received == MBEDTLS_ERR_SSL_WANT_WRITE) {
					RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tls_auto_rcv] MBEDTLS_ERR_SSL_WANT_READ / MBEDTLS_ERR_SSL_WANT_WRITE\r\n");
					continue;
				}
				RTK_LOGI(AT_SOCKET_TAG, "[socket_client_tls_auto_rcv] Failed to mbedtls_ssl_read() = -0x%04x\r\n", -actual_bytes_received);
				goto end;
			} else {
				at_printf_lock();
				at_printf_indicate("[SKT][DATA][%d][%d]:", current_node->link_id, actual_bytes_received);
				at_printf_data(read_buf, (u32)actual_bytes_received);
				at_printf("\r\n");
				at_printf_unlock();
			}
		}
	}

end:
	rtos_mem_free(read_buf);
	read_buf = NULL;

	mbedtls_ssl_close_notify(current_node->ssl);
	tls_client_fd.fd = current_node->sockfd;
	mbedtls_net_free(&tls_client_fd);
	current_node->sockfd = INVALID_SOCKET_ID;

	if (current_node->ca_crt) {
		mbedtls_x509_crt_free(current_node->ca_crt);
		rtos_mem_free(current_node->ca_crt);
		current_node->ca_crt = NULL;
	}
	if (current_node->cert_crt) {
		mbedtls_x509_crt_free(current_node->cert_crt);
		rtos_mem_free(current_node->cert_crt);
		current_node->cert_crt = NULL;
	}
	if (current_node->private_key) {
		mbedtls_pk_free(current_node->private_key);
		rtos_mem_free(current_node->private_key);
		current_node->private_key = NULL;
	}
	if (current_node->ssl) {
		mbedtls_ssl_free(current_node->ssl);
		rtos_mem_free(current_node->ssl);
		current_node->ssl = NULL;
	}
	if (current_node->conf) {
		mbedtls_ssl_config_free(current_node->conf);
		rtos_mem_free(current_node->conf);
		current_node->conf = NULL;
	}

	RTK_LOGI(AT_SOCKET_TAG, "rtos_task_delete(socket_client_tls_auto_rcv)\r\n");
	rtos_sema_give(current_node->del_task_sema);
	rtos_task_delete(NULL);
	return;
}


int create_socket_client_tls(struct _node *current_node)
{
	int ret = 0;
	int error_no = 0;
	char dst_addr_str[16] = {0};
	char dst_port_str[6] = {0};
	mbedtls_net_context tls_client_fd;
	int cert_size = 0, pk_size = 0;
	char *tls_client_ca_pem = NULL;	// read client CA
	char *tls_client_cert_pem = NULL;	// read client cert
	char *tls_client_pkey_pem = NULL;	// read client private key


	if ((current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_SERVER) || (current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		if ((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CA, current_node->cert_index)) <= 0)  {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] Failed to get CLIENT_CA_%d size\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
		tls_client_ca_pem = rtos_mem_zmalloc(cert_size + 1);
		if (tls_client_ca_pem == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] tls_client_ca_pem malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(tls_client_ca_pem, CLIENT_CA, current_node->cert_index);
		if (ret == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] NO CLIENT_CA_%d\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		} else if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] CLIENT_CA_%d read failed\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
	}
	if ((current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_CLIENT) || (current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		if (((cert_size = atcmd_get_ssl_certificate_size(CLIENT_CERT, current_node->cert_index)) <= 0) ||
			((pk_size = atcmd_get_ssl_certificate_size(CLIENT_KEY, current_node->cert_index)) <= 0))  {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] Failed to get CLIENT_CERT_%d / CLIENT_KEY_%d size\r\n", current_node->cert_index, current_node->cert_index);
			error_no = 6;
			goto end;
		}
		tls_client_cert_pem = rtos_mem_zmalloc(cert_size + 1);
		if (tls_client_cert_pem == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] tls_client_cert_pem malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		ret = atcmd_get_ssl_certificate(tls_client_cert_pem, CLIENT_CERT, current_node->cert_index);
		if (ret > 0) {
			tls_client_pkey_pem = rtos_mem_zmalloc(pk_size + 1);
			if (tls_client_pkey_pem == NULL) {
				RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] tls_client_pkey_pem malloc fail\r\n");
				error_no = 7;
				goto end;
			}
			ret = atcmd_get_ssl_certificate(tls_client_pkey_pem, CLIENT_KEY, current_node->cert_index);
			if (ret <= 0)  {
				RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] CLIENT_KEY_%d read failed\r\n", current_node->cert_index);
				error_no = 6;
				goto end;
			}
		} else if (ret == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] NO CLIENT_CERT_%d!\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		} else {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] CLIENT_CERT_%d read failed\r\n", current_node->cert_index);
			error_no = 6;
			goto end;
		}
	}

	current_node->ssl = (mbedtls_ssl_context *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_context));
	if (current_node->ssl == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] current_node->ssl malloc fail\r\n");
		error_no = 7;
		goto end;
	}
	current_node->conf = (mbedtls_ssl_config *)rtos_mem_zmalloc(sizeof(mbedtls_ssl_config));
	if (current_node->conf == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] current_node->conf malloc fail\r\n");
		error_no = 7;
		goto end;
	}

	mbedtls_ssl_init(current_node->ssl);
	mbedtls_ssl_config_init(current_node->conf);

	if ((ret = mbedtls_ssl_config_defaults(current_node->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_ssl_config_defaults() failed ret = -0x%04x\r\n", -ret);
		error_no = 8;
		goto end;
	}

	mbedtls_ssl_conf_authmode(current_node->conf, MBEDTLS_SSL_VERIFY_NONE);

	if ((current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_SERVER) || (current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		current_node->ca_crt = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
		if (current_node->ca_crt == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] current_node->ca_crt malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		mbedtls_x509_crt_init(current_node->ca_crt);
		if ((ret = mbedtls_x509_crt_parse(current_node->ca_crt, (const unsigned char *)tls_client_ca_pem, strlen(tls_client_ca_pem) + 1)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_x509_crt_parse() failed ret = -0x%04x\r\n", -ret);
			error_no = 9;
			goto end;
		}
		rtos_mem_free(tls_client_ca_pem);
		tls_client_ca_pem = NULL;
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] client_ca_%d.crt parse successful\r\n", current_node->cert_index);

		mbedtls_ssl_conf_ca_chain(current_node->conf, current_node->ca_crt, NULL);
		mbedtls_ssl_conf_authmode(current_node->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	}
	if ((current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_CLIENT) || (current_node->protocol == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		current_node->cert_crt = (mbedtls_x509_crt *)rtos_mem_zmalloc(sizeof(mbedtls_x509_crt));
		if (current_node->cert_crt == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] current_node->cert_crt malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		current_node->private_key = (mbedtls_pk_context *)rtos_mem_zmalloc(sizeof(mbedtls_pk_context));
		if (current_node->private_key == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] current_node->private_key malloc fail\r\n");
			error_no = 7;
			goto end;
		}
		mbedtls_x509_crt_init(current_node->cert_crt);
		mbedtls_pk_init(current_node->private_key);
		if ((ret = mbedtls_x509_crt_parse(current_node->cert_crt, (const unsigned char *)tls_client_cert_pem, strlen(tls_client_cert_pem) + 1)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_x509_crt_parse() failed ret = -0x%04x\r\n", -ret);
			error_no = 9;
			goto end;
		}
		rtos_mem_free(tls_client_cert_pem);
		tls_client_cert_pem = NULL;
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] client_cert_%d.crt parse successful\r\n", current_node->cert_index);

		if ((ret = mbedtls_pk_parse_key(current_node->private_key, (const unsigned char *)tls_client_pkey_pem, strlen(tls_client_pkey_pem) + 1, NULL, 0, NULL,
										NULL)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_pk_parse_key() failed ret = -0x%04x\r\n", -ret);
			error_no = 9;
			goto end;
		}
		rtos_mem_free(tls_client_pkey_pem);
		tls_client_pkey_pem = NULL;
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] client_key_%d.key parse successful\r\n", current_node->cert_index);

		if ((ret = mbedtls_ssl_conf_own_cert(current_node->conf, current_node->cert_crt, current_node->private_key)) != 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_ssl_conf_own_cert() failed ret = -0x%04x\r\n", -ret);
			error_no = 10;
			goto end;
		}
	}

	mbedtls_ssl_conf_rng(current_node->conf, atcmd_ssl_random, NULL);
#ifdef MBEDTLS_DEBUG_C
	mbedtls_ssl_conf_dbg(current_node->conf, atcmd_ssl_debug, NULL);
	mbedtls_debug_set_threshold(0);
#endif

	if ((ret = mbedtls_ssl_setup(current_node->ssl, current_node->conf)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_ssl_setup() failed ret = -0x%04x\r\n", -ret);
		error_no = 11;
		goto end;
	}

	struct in_addr dst_addr;
	dst_addr.s_addr = current_node->dst_ip;
	if (inet_ntoa_r(dst_addr, dst_addr_str, sizeof(dst_addr_str) - 1) == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] inet_ntoa_r() failed\r\n");
		error_no = 12;
		goto end;
	}
	DiagSnPrintf(dst_port_str, sizeof(dst_port_str), "%d", current_node->dst_port);

	mbedtls_net_init(&tls_client_fd);
	if ((ret = mbedtls_net_connect(&tls_client_fd, dst_addr_str, dst_port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_net_connect() failed ret = -0x%04x\r\n", -ret);
		error_no = 13;
		goto end;
	}

	current_node->sockfd = tls_client_fd.fd;

	mbedtls_ssl_set_bio(current_node->ssl, &current_node->sockfd, mbedtls_net_send, mbedtls_net_recv, NULL);

	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] Performing the SSL/TLS handshake...\r\n");
	if ((ret = mbedtls_ssl_handshake(current_node->ssl)) != 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_ssl_handshake() failed ret = -0x%04x\r\n", -ret);
		error_no = 14;
		goto end;
	}

	if (current_node->auto_rcv == TRUE) {
		if (rtos_task_create(NULL, "socket_client_tls_auto_rcv", socket_client_tls_auto_rcv, current_node, 1024 * 7,
							 ATCMD_SOCKET_TASK_PRIORITY) != RTK_SUCCESS) {
			RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tcp] rtos_task_create(socket_client_tls_auto_rcv) failed\r\n");
			error_no = 15;
			goto end;
		}
		rtos_sema_create_binary(&(current_node->del_task_sema));
	}

	RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] Connect to SSL/TLS server(%s:%s) successful\r\n", dst_addr_str, dst_port_str);

end:
	if (tls_client_ca_pem) {
		rtos_mem_free(tls_client_ca_pem);
		tls_client_ca_pem = NULL;
	}
	if (tls_client_cert_pem) {
		rtos_mem_free(tls_client_cert_pem);
		tls_client_cert_pem = NULL;
	}
	if (tls_client_pkey_pem) {
		rtos_mem_free(tls_client_pkey_pem);
		tls_client_pkey_pem = NULL;
	}

	if (error_no != 0) {
		if (current_node->ssl) {
			if ((ret = mbedtls_ssl_close_notify(current_node->ssl)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[create_socket_client_tls] mbedtls_ssl_close_notify() failed ret = -0x%04x\r\n", -ret);
			}
		}
		mbedtls_net_free(&tls_client_fd);

		if (current_node->ca_crt) {
			mbedtls_x509_crt_free(current_node->ca_crt);
			rtos_mem_free(current_node->ca_crt);
			current_node->ca_crt = NULL;
		}
		if (current_node->cert_crt) {
			mbedtls_x509_crt_free(current_node->cert_crt);
			rtos_mem_free(current_node->cert_crt);
			current_node->cert_crt = NULL;
		}
		if (current_node->private_key) {
			mbedtls_pk_free(current_node->private_key);
			rtos_mem_free(current_node->private_key);
			current_node->private_key = NULL;
		}

		if (current_node->ssl) {
			mbedtls_ssl_free(current_node->ssl);
			rtos_mem_free(current_node->ssl);
			current_node->ssl = NULL;
		}
		if (current_node->conf) {
			mbedtls_ssl_config_free(current_node->conf);
			rtos_mem_free(current_node->conf);
			current_node->conf = NULL;
		}

		init_single_node(current_node);
	}

	return error_no;
}


int create_socket_client(struct _node *current_node)
{
	int error_no = 0;
	if (current_node->protocol == SOCKET_CLIENT_OVER_UDP) {
		error_no = create_socket_client_udp(current_node);
	} else if (current_node->protocol == SOCKET_CLIENT_OVER_TCP) {
		error_no = create_socket_client_tcp(current_node);
	} else {
		error_no = create_socket_client_tls(current_node);
	}
	return error_no;
}


//AT+SKTCLIENT=<link_id>,<conn_type>[,<cert_index>],<dst_host>,<dst_port>[,<src_port>],<auto_rcv>
void at_sktclient(void *arg)
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
	if ((link_id < 0) || (link_id > (MEMP_NUM_NETCONN - 1))) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	if (node_pool[link_id].link_id >= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The <link_id> is in use\r\n");
		error_no = 1;
		goto end;
	}
	conn_type = atoi(argv[2]);
	if ((conn_type < SOCKET_CLIENT_OVER_UDP) || (conn_type > SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The range of <conn_type> is [0, 5]\r\n");
		error_no = 1;
		goto end;
	}
	if ((conn_type >= SOCKET_CLIENT_OVER_TLS_VERIFY_SERVER) && (conn_type <= SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
		if (strlen(argv[3]) == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] Missing <cert_index> for SOCKET_CLIENT_OVER_TLS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[3]);
		if (cert_index <= 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The <cert_index> should be greater than or equal to 1\r\n");
			error_no = 1;
			goto end;
		}
		if ((conn_type == SOCKET_CLIENT_OVER_TLS_VERIFY_SERVER) || (conn_type == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
			if (atcmd_get_ssl_certificate_size(CLIENT_CA, cert_index) <= 0)  {
				RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] CLIENT_CA_%d does not exist in flash\r\n", cert_index);
				error_no = 1;
				goto end;
			}
		}
		if ((conn_type == SOCKET_CLIENT_OVER_TLS_VERIFY_CLIENT) || (conn_type == SOCKET_CLIENT_OVER_TLS_VERIFY_BOTH)) {
			if ((atcmd_get_ssl_certificate_size(CLIENT_CERT, cert_index) <= 0) || (atcmd_get_ssl_certificate_size(CLIENT_KEY, cert_index) <= 0))  {
				RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] CLIENT_CERT_%d/CLIENT_KEY_%d does not exist in flash\r\n", cert_index, cert_index);
				error_no = 1;
				goto end;
			}
		}
	}
	if (conn_type == SOCKET_CLIENT_OVER_UDP) {
		if (strlen(argv[6]) == 0) {
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
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The range of <dst_port> is [1, 65535]\r\n");
		error_no = 1;
		goto end;
	}
	if (strlen(argv[6])) {
		src_port = atoi(argv[6]);
		if (src_port <= 0 || src_port > 65535) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The range of <src_port> is [1, 65535]\r\n");
			error_no = 1;
			goto end;
		}
	}
	auto_rcv = atoi(argv[7]);
	if (auto_rcv < 0 || auto_rcv > 1) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktclient] The <auto_rcv>'s possible values are 0 and 1\r\n");
		error_no = 1;
		goto end;
	}

	//Generate the node
	node_pool[link_id].link_id = link_id;
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
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}

	return;
}


int atcmd_lwip_send_data(struct _node *curnode, u8 *data, int data_sz, struct sockaddr_in dst_addr)
{
	int ret = 0, error_no = 0;

	// UDP
	if (curnode->protocol == 0) {
		ret = sendto(curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr));
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] UDP send failed in sendto() = %d\r\n", ret);
			error_no = 2;
			goto end;
		}
	}
	// TCP
	else if (curnode->protocol == 1) {
		ret = write(curnode->sockfd, data, data_sz);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] TCP send failed in write() = %d\r\n", ret);
			error_no = 2;
			goto end;
		}

	}
	// TLS
	else {
		ret = mbedtls_ssl_write(curnode->ssl, (unsigned char *)data, (size_t)data_sz);
		if (ret < 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_send_data] TLS/SSL send failed in mbedtls_ssl_write() = -0x%04x for protocol = %d\r\n", -ret, curnode->protocol);
			error_no = 2;
			goto end;
		}
	}

end:
	return error_no;
}


int atcmd_lwip_start_tt_handle(struct _node *curnode, int total_data_len, struct sockaddr_in dst_addr)
{
	int error_no = 0;
	uint8_t *tt_data = NULL;
	int frag_tt_data_len = 0;
	int recv_tt_len = 0;

	if (total_data_len > 0) {
		tt_data = rtos_mem_zmalloc((total_data_len <= MAX_TT_BUF_LEN ? total_data_len : MAX_TT_BUF_LEN) + 1);
		if (tt_data == NULL) {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] tt_data malloc failed\r\n");
			error_no = 3;
			goto end;
		}
		if (atcmd_tt_mode_start((u32)total_data_len) < 0)  {
			RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_start() failed\r\n");
			error_no = 4;
			goto end;
		}
		if (total_data_len <= MAX_TT_BUF_LEN) {
			recv_tt_len = atcmd_tt_mode_get(tt_data, (u32)total_data_len);
			if (recv_tt_len == 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_get() failed\r\n");
				error_no = 4;
				goto tt_end;
			}
			if ((error_no = atcmd_lwip_send_data(curnode, tt_data, recv_tt_len, dst_addr)) != 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_lwip_send_data() failed\r\n");
				goto tt_end;
			}
		} else {
			while (total_data_len > 0) {
				frag_tt_data_len = total_data_len <= MAX_TT_BUF_LEN ? total_data_len : MAX_TT_BUF_LEN;
				recv_tt_len = atcmd_tt_mode_get(tt_data, (u32)frag_tt_data_len);
				if (recv_tt_len == 0) {
					RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_tt_mode_get() failed\r\n");
					error_no = 4;
					goto tt_end;
				}
				if ((error_no = atcmd_lwip_send_data(curnode, tt_data, recv_tt_len, dst_addr)) != 0) {
					RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_start_tt_handle] atcmd_lwip_send_data() failed\r\n");
					goto tt_end;
				}
				total_data_len -= recv_tt_len;
			}
		}
tt_end:
		atcmd_tt_mode_end();
	}

end:
	if (tt_data) {
		rtos_mem_free(tt_data);
		tt_data = NULL;
	}
	return error_no;
}


//AT+SKTSENDRAW=<link_id>,<data_size>[,<dst_ip>,<dst_port>]
void at_sktsendraw(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = INVALID_LINK_ID;
	struct _node *curnode = NULL;
	int data_sz = 0;
	struct sockaddr_in dst_addr;
	int dst_port = 0;


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
	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id > (MEMP_NUM_NETCONN - 1)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	curnode = &node_pool[link_id];
	if ((curnode->link_id == INVALID_LINK_ID) || (curnode->sockfd == INVALID_SOCKET_ID) || (curnode->role == NODE_ROLE_INVALID) ||
		(curnode->protocol == SOCKET_PROTOCOL_INVALID)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] The link=%d does not exist or socket not established\r\n", link_id);
		error_no = 1;
		goto end;
	}

	data_sz = atoi(argv[2]);
	if (data_sz <= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] The <data_size> should be greater than 0\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol == 0) && (curnode->role == NODE_ROLE_SERVER)) {
		if ((strlen(argv[3]) == 0) || (strlen(argv[4]) == 0)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Missing <dst_ip> or/and <dst_port> for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		if (strlen(argv[3]) > DNS_MAX_NAME_LENGTH) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Input <dst_ip> length should not exceed DNS_MAX_NAME_LENGTH for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		dst_port = atoi(argv[4]);
		if (dst_port <= 0 || dst_port > 65535) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] The range of <dst_port> is [1, 65535] for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		char udp_clientaddr[16] = {0};
		strncpy(udp_clientaddr, argv[3], strlen(argv[3]));
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(dst_port);
		ret = inet_aton(udp_clientaddr, &dst_addr.sin_addr);
		if (ret == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Failed in inet_aton()\r\n");
			error_no = 1;
			goto end;
		}
	} else if ((curnode->protocol == 0) && (curnode->role == NODE_ROLE_CLIENT)) {
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(curnode->dst_port);
		dst_addr.sin_addr.s_addr = curnode->dst_ip;
	}

	if (curnode->protocol >= 1) {
		if (curnode->role == NODE_ROLE_SERVER) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsendraw] Please use the correct seed <link_id> for TCP/SSL server\r\n");
			error_no = 1;
			goto end;
		}
	}

	error_no = atcmd_lwip_start_tt_handle(curnode, data_sz, dst_addr);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


//AT+SKTSEND=<link_id>,<data_size>[,<dst_ip>,<dst_port>],<data>
void at_sktsend(void *arg)
{
	int argc = 0, error_no = 0, ret = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = INVALID_LINK_ID;
	struct _node *curnode = NULL;
	int data_sz = 0;
	struct sockaddr_in dst_addr;
	int dst_port = 0;


	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc != 6) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}
	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0) || (strlen(argv[5]) == 0)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id > (MEMP_NUM_NETCONN - 1)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	curnode = &node_pool[link_id];
	if ((curnode->link_id == INVALID_LINK_ID) || (curnode->sockfd == INVALID_SOCKET_ID) || (curnode->role == NODE_ROLE_INVALID) ||
		(curnode->protocol == SOCKET_PROTOCOL_INVALID)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] The link=%d does not exist or socket not established\r\n", link_id);
		error_no = 1;
		goto end;
	}

	data_sz = atoi(argv[2]);
	if ((data_sz <= 0) || (data_sz >= CMD_BUFLEN)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] The range of <data_size> is [1, CMD_BUFLEN-1]\r\n");
		error_no = 1;
		goto end;
	}

	if (data_sz != (int)strlen(argv[5])) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] <data_size> does not match the actual <data> length\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol == 0) && (curnode->role == NODE_ROLE_SERVER)) {
		if ((strlen(argv[3]) == 0) || (strlen(argv[4]) == 0)) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Missing <dst_ip> or/and <dst_port> for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		if (strlen(argv[3]) > DNS_MAX_NAME_LENGTH) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Input <dst_ip> length should not exceed DNS_MAX_NAME_LENGTH for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		dst_port = atoi(argv[4]);
		if (dst_port <= 0 || dst_port > 65535) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] The range of <dst_port> is [1, 65535] for UDP server\r\n");
			error_no = 1;
			goto end;
		}
		char udp_clientaddr[16] = {0};
		strncpy(udp_clientaddr, argv[3], strlen(argv[3]));
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(dst_port);
		ret = inet_aton(udp_clientaddr, &dst_addr.sin_addr);
		if (ret == 0) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Failed in inet_aton()\r\n");
			error_no = 1;
			goto end;
		}
	} else if ((curnode->protocol == 0) && (curnode->role == NODE_ROLE_CLIENT)) {
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(curnode->dst_port);
		dst_addr.sin_addr.s_addr = curnode->dst_ip;
	}

	if (curnode->protocol >= 1) {
		if (curnode->role == NODE_ROLE_SERVER) {
			RTK_LOGI(AT_SOCKET_TAG, "[at_sktsend] Please use the correct seed <link_id> for TCP/SSL server\r\n");
			error_no = 1;
			goto end;
		}
	}

	error_no = atcmd_lwip_send_data(curnode, (u8 *)argv[5], data_sz, dst_addr);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


int atcmd_lwip_receive_data(struct _node *curnode, u8 *buffer, int buffer_size, int *actual_recv_size, struct sockaddr_in *skt_dst_addr)
{
	struct timeval timeout;
	fd_set readfds;
	int error_no = 0, ret = 0, size = 0;

	FD_ZERO(&readfds);
	FD_SET(curnode->sockfd, &readfds);
	timeout.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	timeout.tv_usec = RECV_SELECT_TIMEOUT_USEC;

	ret = select(curnode->sockfd + 1, &readfds, NULL, NULL, &timeout);
	if (ret == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] select() timeout\r\n");
		error_no = 0;
		goto end;
	} else if (ret < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] Failed to select() = %d\r\n", ret);
		error_no = 3;
		goto end;
	}

	if ((curnode->sockfd >= 0) && FD_ISSET(curnode->sockfd, &readfds)) {
		// UDP
		if (curnode->protocol == 0) {
			u32_t addr_len = sizeof(struct sockaddr_in);
			size = recvfrom(curnode->sockfd, buffer, buffer_size, 0, (struct sockaddr *) skt_dst_addr, &addr_len);
			if (size == 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] recvfrom() = 0\r\n");
			} else if (size < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] Failed to recvfrom() = %d\r\n", size);
				error_no = 4;
			}
		}
		// TCP
		else if (curnode->protocol == 1) {
			size = read(curnode->sockfd, buffer, buffer_size);
			if (size == 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] read() = 0; Connection closed\r\n");
				error_no = 4;
			} else if (size < 0) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] Failed to read() = %d\r\n", size);
				error_no = 4;
			}
		}
		//TLS
		else {
			size = mbedtls_ssl_read(curnode->ssl, buffer, buffer_size);
			if ((size == 0) || (size == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] mbedtls_ssl_read() = 0; Connection closed\r\n");
				error_no = 4;
			} else if (size < 0) {
				if (size == MBEDTLS_ERR_SSL_WANT_READ || size == MBEDTLS_ERR_SSL_WANT_WRITE) {
					RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] MBEDTLS_ERR_SSL_WANT_READ / MBEDTLS_ERR_SSL_WANT_WRITE\r\n");
					error_no = 0;
					return 0;
				}
				RTK_LOGI(AT_SOCKET_TAG, "[atcmd_lwip_receive_data] Failed to mbedtls_ssl_read() = -0x%04x\r\n", -size);
				error_no = 4;
			}
		}
	}

end:
	if (error_no == 0) {
		*actual_recv_size = size;
	} else {
		if (curnode->protocol >= 2) {
			mbedtls_net_context server_fd;
			server_fd.fd = curnode->sockfd;
			mbedtls_ssl_close_notify(curnode->ssl);
			mbedtls_net_free(&server_fd);
			mbedtls_ssl_free(curnode->ssl);
			rtos_mem_free(curnode->ssl);
			curnode->ssl = NULL;
		} else {
			close(curnode->sockfd);
		}
		curnode->sockfd = INVALID_SOCKET_ID;
	}
	return error_no;
}


//AT+SKTREAD=<link_id>,<data_size>
void at_sktread(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = INVALID_LINK_ID;
	int wanted_recv_size = 0, actual_recv_size = 0;
	struct _node *curnode = NULL;
	struct sockaddr_in skt_dst_addr;
	char udp_dstaddr[16] = {0};
	u16_t udp_dstport = 0;
	u8 *rx_buffer = NULL;

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

	if ((strlen(argv[1]) == 0) || (strlen(argv[2]) == 0)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}

	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id > (MEMP_NUM_NETCONN - 1)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	curnode = &node_pool[link_id];
	if ((curnode->link_id == INVALID_LINK_ID) || (curnode->sockfd == INVALID_SOCKET_ID) || (curnode->role == NODE_ROLE_INVALID) ||
		(curnode->protocol == SOCKET_PROTOCOL_INVALID)) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] The link=%d does not exist or socket not established\r\n", link_id);
		error_no = 1;
		goto end;
	}

	wanted_recv_size = atoi(argv[2]);
	if (wanted_recv_size <= 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] The <data_size> should be greater than 0\r\n");
		error_no = 1;
		goto end;
	}

	if ((curnode->protocol == SOCKET_SERVER_OVER_TCP || curnode->protocol == SOCKET_SERVER_OVER_TLS ||
		 curnode->protocol == SOCKET_SERVER_OVER_TLS_VERIFY_CLIENT) && curnode->role == NODE_ROLE_SERVER) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] Please use the correct seed <link_id> for TCP/SSL server\r\n");
		error_no = 1;
		goto end;
	}

	rx_buffer = (u8 *)rtos_mem_zmalloc(wanted_recv_size + 1);
	if (rx_buffer == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktread] rx_buffer malloc fail\r\n");
		error_no = 2;
		goto end;
	}

	memset(&skt_dst_addr, 0, sizeof(skt_dst_addr));

	error_no = atcmd_lwip_receive_data(curnode, rx_buffer, wanted_recv_size, &actual_recv_size, &skt_dst_addr);

end:
	if (error_no == 0) {
		if (curnode->protocol == 0) {
			inet_ntoa_r(skt_dst_addr.sin_addr.s_addr, udp_dstaddr, 16);
			udp_dstport = ntohs(skt_dst_addr.sin_port);
			at_printf("+SKTREAD:%d,%d,%s,%d,", link_id, actual_recv_size, udp_dstaddr, udp_dstport);
		} else {
			at_printf("+SKTREAD:%d,%d,", link_id, actual_recv_size);
		}
		if (actual_recv_size > 0) {
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


//AT+SKTSTATE
void at_sktstate(void *arg)
{
	struct _node *n = NULL;
	struct in_addr addr;

	UNUSED(arg);

	for (int i = 0; i < MEMP_NUM_NETCONN; i++) {
		n = &node_pool[i];
		if (n->link_id != INVALID_LINK_ID) {
			if (n->role == NODE_ROLE_SEED) {
				continue;
			}
			at_printf("link_id:%d,", n->link_id);
			if (n->role == NODE_ROLE_SERVER) {
				at_printf("server,");
			} else if (n->role == NODE_ROLE_CLIENT) {
				at_printf("client,");
			}
			if (n->protocol == 0) {
				at_printf("udp,");
			} else if (n->protocol == 1) {
				at_printf("tcp,");
			} else {
				at_printf("ssl,");
			}

			addr.s_addr = n->src_ip;
			at_printf("src_address:%s,src_port:%d,", inet_ntoa(addr), n->src_port);
			addr.s_addr = n->dst_ip;
			at_printf("dst_address:%s,dst_port:%d,", inet_ntoa(addr), n->dst_port);
			at_printf("socket_fd:%d\r\n", n->sockfd);
			/* Seed. */
			if (n->nextseed != NULL) {
				struct _node *seed = n;
				do {
					seed = seed->nextseed;
					at_printf("link_id:%d,seed,", seed->link_id);
					if (seed->protocol == 0) {
						at_printf("udp,");
					} else if (n->protocol == 1) {
						at_printf("tcp,");
					} else {
						at_printf("ssl,");
					}
					addr.s_addr = seed->src_ip;
					at_printf("src_address:%s,src_port:%d,", inet_ntoa(addr), seed->src_port);
					addr.s_addr = seed->dst_ip;
					at_printf("dst_address:%s,dst_port:%d,", inet_ntoa(addr), seed->dst_port);
					at_printf("socket_fd:%d\r\n", seed->sockfd);
				} while (seed->nextseed != NULL);
			}
		}
	}

	at_printf(ATCMD_OK_END_STR);
}


void close_and_free_tls_resource(struct _node *current_node)
{
	mbedtls_net_context tls_fd;
	mbedtls_ssl_close_notify(current_node->ssl);
	tls_fd.fd = current_node->sockfd;
	mbedtls_net_free(&tls_fd);
	current_node->sockfd = INVALID_SOCKET_ID;

	if (current_node->role != NODE_ROLE_SEED) {
		if (current_node->ca_crt) {
			mbedtls_x509_crt_free(current_node->ca_crt);
			rtos_mem_free(current_node->ca_crt);
			current_node->ca_crt = NULL;
		}
		if (current_node->cert_crt) {
			mbedtls_x509_crt_free(current_node->cert_crt);
			rtos_mem_free(current_node->cert_crt);
			current_node->cert_crt = NULL;
		}
		if (current_node->private_key) {
			mbedtls_pk_free(current_node->private_key);
			rtos_mem_free(current_node->private_key);
			current_node->private_key = NULL;
		}
		if (current_node->conf) {
			mbedtls_ssl_config_free(current_node->conf);
			rtos_mem_free(current_node->conf);
			current_node->conf = NULL;
		}
	}

	if (current_node->ssl) {
		mbedtls_ssl_free(current_node->ssl);
		rtos_mem_free(current_node->ssl);
		current_node->ssl = NULL;
	}

	return;
}



void close_and_free_node(struct _node *freenode)
{
	struct _node *prevnode = NULL, *nextnode = NULL, *tempnode = NULL;

	//UDP
	if (freenode->protocol == 0) {
		if (freenode->auto_rcv == TRUE) {
			freenode->stop_task = TRUE;
			rtos_sema_take(freenode->del_task_sema, 100);
			rtos_sema_delete(freenode->del_task_sema);
		}

		if (freenode->sockfd >= 0) {
			close(freenode->sockfd);
		}
	}
	//TCP
	else if (freenode->protocol == 1) {
		if (freenode->role == NODE_ROLE_CLIENT) {
			if (freenode->auto_rcv == TRUE) {
				freenode->stop_task = TRUE;
				rtos_sema_take(freenode->del_task_sema, 100);
				rtos_sema_delete(freenode->del_task_sema);
			}

			if (freenode->sockfd >= 0) {
				close(freenode->sockfd);
			}
		} else if (freenode->role == NODE_ROLE_SEED) {
			prevnode = freenode->prevnode;
			prevnode->nextseed = freenode->nextseed;
			if (freenode->sockfd >= 0) {
				close(freenode->sockfd);
			}
		} else if (freenode->role == NODE_ROLE_SERVER) {
			freenode->stop_task = TRUE;
			rtos_sema_take(freenode->del_task_sema, 100);
			nextnode = freenode->nextseed;
			while (nextnode) {
				tempnode = nextnode->nextseed;
				if (nextnode->sockfd >= 0) {
					close(nextnode->sockfd);
				}
				init_single_node(nextnode);
				nextnode = tempnode;
			}

			rtos_sema_delete(freenode->del_task_sema);

			if (freenode->sockfd >= 0) {
				close(freenode->sockfd);
			}
		}
	}
	//TLS
	else {
		if (freenode->role == NODE_ROLE_CLIENT) {
			if (freenode->auto_rcv == TRUE) {
				freenode->stop_task = TRUE;
				rtos_sema_take(freenode->del_task_sema, 100);
				rtos_sema_delete(freenode->del_task_sema);
			}

			close_and_free_tls_resource(freenode);
		} else if (freenode->role == NODE_ROLE_SEED) {
			prevnode = freenode->prevnode;
			prevnode->nextseed = freenode->nextseed;
			close_and_free_tls_resource(freenode);
		} else if (freenode->role == NODE_ROLE_SERVER) {
			freenode->stop_task = TRUE;
			rtos_sema_take(freenode->del_task_sema, 100);
			nextnode = freenode->nextseed;
			while (nextnode) {
				tempnode = nextnode->nextseed;
				close_and_free_tls_resource(nextnode);
				init_single_node(nextnode);
				nextnode = tempnode;
			}
			rtos_sema_delete(freenode->del_task_sema);
			close_and_free_tls_resource(freenode);
		}
	}

	init_single_node(freenode);
	return;
}


//AT+SKTDEL=<link_id>
void at_sktdel(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int link_id = INVALID_LINK_ID;

	if (arg == NULL) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktdel] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktdel] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (strlen(argv[1]) == 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktdel] Missing input parameters\r\n");
		error_no = 1;
		goto end;
	}
	link_id = atoi(argv[1]);
	if ((link_id < 0) || (link_id > (MEMP_NUM_NETCONN - 1))) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktdel] The range of <link_id> is [0, MEMP_NUM_NETCONN - 1]\r\n");
		error_no = 1;
		goto end;
	}
	if (node_pool[link_id].link_id < 0) {
		RTK_LOGI(AT_SOCKET_TAG, "[at_sktdel] The <link_id>=%d does not exist or socket not established\r\n", link_id);
		error_no = 1;
		goto end;
	}

	close_and_free_node(&node_pool[link_id]);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}


ATCMD_TABLE_DATA_SECTION
const log_item_t at_socket_items[ ] = {
	{"+SKTCFG", at_sktcfg, {NULL, NULL}},
	{"+SKTQUERY", at_sktquery, {NULL, NULL}},
	{"+SKTSERVER", at_sktserver, {NULL, NULL}},
	{"+SKTCLIENT", at_sktclient, {NULL, NULL}},
	{"+SKTSENDRAW", at_sktsendraw, {NULL, NULL}},
	{"+SKTSEND", at_sktsend, {NULL, NULL}},
	{"+SKTREAD", at_sktread, {NULL, NULL}},
	{"+SKTSTATE", at_sktstate, {NULL, NULL}},
	{"+SKTDEL", at_sktdel, {NULL, NULL}},
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

	atcmd_service_add_table((log_item_t *)at_socket_items, sizeof(at_socket_items) / sizeof(at_socket_items[0]));
}

#endif /* CONFIG_ATCMD_SOCKET */
#endif /* CONFIG_LWIP_LAYER */