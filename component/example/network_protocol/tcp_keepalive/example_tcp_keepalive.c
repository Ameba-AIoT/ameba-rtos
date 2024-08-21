#include "platform_stdlib.h"
#include "basic_types.h"
#include "lwip_netconf.h"
#include "lwip/netif.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"


#define TEST_MODE       0	// 0 to test client keepalive, 1 to test server keepalive
#define SERVER_IP       "192.168.0.110"
#define SERVER_PORT     5000
#define LISTEN_QLEN     2
#define MAX_SOCKETS     10
#define SELECT_TIMEOUT  10

static void example_tcp_keepalive_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	// Delay to wait for IP by DHCP
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}
	RTK_LOGS(NOTAG, "\nExample: TCP Keepalive\n");
#if !LWIP_TCP_KEEPALIVE
	RTK_LOGS(NOTAG, "\nPlease enable LWIP_TCP_KEEPALIVE\n");
#else

#if (TEST_MODE == 0)
	int server_socket, keepalive = 1, keepalive_idle = 3, keepalive_interval = 5, keepalive_count = 3;
	struct sockaddr_in server_addr;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// enable socket keepalive with keepalive timeout = idle(3) + interval(5) * count(3) = 18 seconds
	if (setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: SO_KEEPALIVE\n");
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: TCP_KEEPIDLE\n");
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: TCP_KEEPINTVL\n");
	}
	if (setsockopt(server_socket, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: TCP_KEEPCNT\n");
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);

	if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
		unsigned char response_buf[100];
		int read_size;
		RTK_LOGS(NOTAG, "connect OK\n");

		while ((read_size = read(server_socket, response_buf, sizeof(response_buf))) > 0) {
			RTK_LOGS(NOTAG, "read %d bytes\n", read_size);
		}

		RTK_LOGS(NOTAG, "ERROR: read %d\n", read_size);
		close(server_socket);
	} else {
		RTK_LOGS(NOTAG, "ERROR: connect\n");
		close(server_socket);
	}

#elif (TEST_MODE == 1)
	int max_socket_fd = -1;
	struct sockaddr_in server_addr;
	int server_fd = -1;
	int socket_used[MAX_SOCKETS];

	memset(socket_used, 0, sizeof(socket_used));

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
			RTK_LOGS(NOTAG, "bind error\n");
			goto exit;
		}

		if (listen(server_fd, LISTEN_QLEN) != 0) {
			RTK_LOGS(NOTAG, "listen error\n");
			goto exit;
		}

		socket_used[server_fd] = 1;

		if (server_fd > max_socket_fd) {
			max_socket_fd = server_fd;
		}
	} else {
		RTK_LOGS(NOTAG, "socket error\n");
		goto exit;
	}

	while (1) {
		int socket_fd;
		unsigned char buf[512];
		fd_set read_fds;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		for (socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++)
			if (socket_used[socket_fd]) {
				FD_SET(socket_fd, &read_fds);
			}

		if (select(max_socket_fd + 1, &read_fds, NULL, NULL, &timeout)) {
			for (socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++) {
				if (socket_used[socket_fd] && FD_ISSET(socket_fd, &read_fds)) {
					if (socket_fd == server_fd) {
						struct sockaddr_in client_addr;
						unsigned int client_addr_size = sizeof(client_addr);
						int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

						if (fd >= 0) {
							int keepalive = 1, keepalive_idle = 3, keepalive_interval = 5, keepalive_count = 3;
							RTK_LOGS(NOTAG, "accept socket fd(%d)\n", fd);
							socket_used[fd] = 1;

							// enable socket keepalive with keepalive timeout = idle(3) + interval(5) * count(3) = 18 seconds
							if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) != 0) {
								RTK_LOGS(NOTAG, "ERROR: SO_KEEPALIVE\n");
							}
							if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_idle, sizeof(keepalive_idle)) != 0) {
								RTK_LOGS(NOTAG, "ERROR: TCP_KEEPIDLE\n");
							}
							if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof(keepalive_interval)) != 0) {
								RTK_LOGS(NOTAG, "ERROR: TCP_KEEPINTVL\n");
							}
							if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepalive_count, sizeof(keepalive_count)) != 0) {
								RTK_LOGS(NOTAG, "ERROR: TCP_KEEPCNT\n");
							}

							if (fd > max_socket_fd) {
								max_socket_fd = fd;
							}
						} else {
							RTK_LOGS(NOTAG, "accept error\n");
						}
					} else {
						int read_size = read(socket_fd, buf, sizeof(buf));

						if (read_size > 0) {
							write(socket_fd, buf, read_size);
						} else {
							RTK_LOGS(NOTAG, "socket fd(%d) disconnected\n", socket_fd);
							socket_used[socket_fd] = 0;
							close(socket_fd);
						}
					}
				}
			}
		}

		rtos_time_delay_ms(10);
	}

exit:
	if (server_fd >= 0) {
		close(server_fd);
	}
#endif	/* TEST_MODE */
#endif	/* LWIP_TCP_KEEPALIVE */

	rtos_task_delete(NULL);
}

void example_tcp_keepalive(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_tcp_keepalive_thread"), example_tcp_keepalive_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(init_thread) failed", __FUNCTION__);
	}
}
