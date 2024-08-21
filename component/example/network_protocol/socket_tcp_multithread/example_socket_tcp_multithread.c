#include "platform_stdlib.h"
#include "basic_types.h"
#include "lwipconf.h"
//#include <osdep_api.h>
#include "os_wrapper.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"


#define SERVER_PORT     5001
#define LISTEN_QLEN     2

static int tx_exit = 0, rx_exit = 0;
//static _Sema tcp_tx_rx_sema;
static rtos_sema_t tcp_tx_rx_sema;

static void tx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	memset(buffer, 1, sizeof(buffer));
	RTK_LOGS(NOTAG, "\n%s start\n", __FUNCTION__);

	while (1) {
		int ret = 0;

		//RtlDownSema(&tcp_tx_rx_sema);
		rtos_sema_take(tcp_tx_rx_sema, RTOS_MAX_DELAY);
		ret = send(client_fd, buffer, sizeof(buffer), 0);
		//RtlUpSema(&tcp_tx_rx_sema);
		rtos_sema_give(tcp_tx_rx_sema);

		if (ret <= 0) {
			goto exit;
		}

		rtos_time_delay_ms(100);
	}

exit:
	RTK_LOGS(NOTAG, "\n%s exit\n", __FUNCTION__);
	tx_exit = 1;
	rtos_task_delete(NULL);
}

static void rx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	RTK_LOGS(NOTAG, "\n%s start\n", __FUNCTION__);

	while (1) {
		int ret = 0, sock_err = 0;
		size_t err_len = sizeof(sock_err);

		//RtlDownSema(&tcp_tx_rx_sema);
		rtos_sema_take(tcp_tx_rx_sema, RTOS_MAX_DELAY);
		ret = recv(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		getsockopt(client_fd, SOL_SOCKET, SO_ERROR, &sock_err, &err_len);
		//RtlUpSema(&tcp_tx_rx_sema);
		rtos_sema_give(tcp_tx_rx_sema);

		// ret == -1 and socket error == EAGAIN when no data received for nonblocking
		if ((ret == -1) && ((sock_err == EAGAIN)
							|| (sock_err == 0)
						   )) {
			continue;
		} else if (ret <= 0) {
			goto exit;
		}

		rtos_time_delay_ms(10);
	}

exit:
	RTK_LOGS(NOTAG, "\n%s exit\n", __FUNCTION__);
	rx_exit = 1;
	rtos_task_delete(NULL);
}

static void example_socket_tcp_trx_thread(void *param)
{
	int server_fd = -1, client_fd = -1;
	struct sockaddr_in server_addr, client_addr;
	size_t client_addr_size;

	(void) param;

	// Delay to wait for IP by DHCP
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}
	RTK_LOGS(NOTAG, "\nExample: socket tx/rx 1\n");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		RTK_LOGS(NOTAG, "ERROR: bind\n");
		goto exit;
	}

	if (listen(server_fd, LISTEN_QLEN) != 0) {
		RTK_LOGS(NOTAG, "ERROR: listen\n");
		goto exit;
	}

	while (1) {
		client_addr_size = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

		if (client_fd >= 0) {
			tx_exit = 1;
			rx_exit = 1;
			//RtlInitSema(&tcp_tx_rx_sema, 1);
			rtos_sema_create(&tcp_tx_rx_sema, 1, RTOS_SEMA_MAX_COUNT);

			if (rtos_task_create(NULL, ((const char *)"tx_thread"), tx_thread, &client_fd, 2048 * 4, 1) != SUCCESS) {
				RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(tx_thread) failed", __FUNCTION__);
			} else {
				tx_exit = 0;
			}

			rtos_time_delay_ms(10);

			if (rtos_task_create(NULL, ((const char *)"rx_thread"), rx_thread, &client_fd, 2048 * 4, 1) != SUCCESS) {
				RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(rx_thread) failed", __FUNCTION__);
			} else {
				rx_exit = 0;
			}

			while (1) {
				if (tx_exit && rx_exit) {
					close(client_fd);
					break;
				} else {
					rtos_time_delay_ms(1000);
				}
			}

			//RtlFreeSema(&tcp_tx_rx_sema);
			rtos_sema_delete(tcp_tx_rx_sema);
		}
	}

exit:
	close(server_fd);
	rtos_task_delete(NULL);
}

void example_socket_tcp_multithread(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_socket_tcp_trx_thread"), example_socket_tcp_trx_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(example_socket_tcp_trx_thread) failed", __FUNCTION__);
	}
}
