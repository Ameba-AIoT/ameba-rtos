#include "lwip_netconf.h"
#include "log.h"

#define SERVER_PORT     5001
#define LISTEN_QLEN     2

static int tx_exit = 0, rx_exit = 0;
static rtos_sema_t tcp_tx_rx_sema;

static void tx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	memset(buffer, 1, sizeof(buffer));
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n%s start\n", __FUNCTION__);

	while (1) {
		int ret = 0;

		rtos_sema_take(tcp_tx_rx_sema, RTOS_MAX_DELAY);
		ret = send(client_fd, buffer, sizeof(buffer), 0);
		rtos_sema_give(tcp_tx_rx_sema);

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "tx_thread() send data(len=%d)\n", ret);

		if (ret <= 0) {
			goto exit;
		}

		rtos_time_delay_ms(1000);
	}

exit:
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n%s exit\n", __FUNCTION__);
	tx_exit = 1;
	rtos_task_delete(NULL);
}

static void rx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n%s start\n", __FUNCTION__);

	while (1) {
		int ret = 0;

		rtos_sema_take(tcp_tx_rx_sema, RTOS_MAX_DELAY);
		ret = recv(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		rtos_sema_give(tcp_tx_rx_sema);

		// ret == -1 and socket error == EAGAIN when no data received for nonblocking
		if ((ret == -1) && ((errno == EAGAIN) || (errno == 0))) {
			continue;
		} else if (ret <= 0) {
			goto exit;
		} else if (ret > 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "rx_thread() recv data(len=%d)\n", ret);
		}

		rtos_time_delay_ms(10);
	}

exit:
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n%s exit\n", __FUNCTION__);
	rx_exit = 1;
	rtos_task_delete(NULL);
}

static void example_socket_tcp_trx_thread(void *param)
{
	int server_fd = -1, client_fd = -1;
	struct sockaddr_in server_addr, client_addr;
	size_t client_addr_size;

	(void) param;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: socket tcp tx/rx multithread====================\r\n");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: bind\n");
		goto exit;
	}

	if (listen(server_fd, LISTEN_QLEN) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: listen\n");
		goto exit;
	}

	while (1) {
		client_addr_size = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

		if (client_fd >= 0) {
			tx_exit = 1;
			rx_exit = 1;

			rtos_sema_create(&tcp_tx_rx_sema, 1, RTOS_SEMA_MAX_COUNT);

			if (rtos_task_create(NULL, ((const char *)"tx_thread"), tx_thread, &client_fd, 2048 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(tx_thread) failed", __FUNCTION__);
			} else {
				tx_exit = 0;
			}

			rtos_time_delay_ms(10);

			if (rtos_task_create(NULL, ((const char *)"rx_thread"), rx_thread, &client_fd, 2048 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(rx_thread) failed", __FUNCTION__);
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

			rtos_sema_delete(tcp_tx_rx_sema);
		}
	}

exit:
	close(server_fd);
	rtos_task_delete(NULL);
}

void example_socket_tcp_multithread(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_socket_tcp_trx_thread"), example_socket_tcp_trx_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_socket_tcp_trx_thread) failed", __FUNCTION__);
	}
}
