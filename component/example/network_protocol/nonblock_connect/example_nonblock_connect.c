#include "lwip_netconf.h"
#include "log.h"

#define SERVER_IP              "192.168.1.100"
#define SERVER_PORT            80

static void example_nonblock_connect_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	int server_fd = -1;
	struct sockaddr_in server_addr;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n====================Example: nonblock connect====================\n");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);
	connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if (errno == EINPROGRESS) {
		fd_set wfds;
		struct timeval time_out;

		time_out.tv_sec = 3;	// Set select timeout of 3 seconds
		time_out.tv_usec = 0;
		FD_ZERO(&wfds) ;
		FD_SET(server_fd, &wfds);	// Only set server fd

		// Use select to wait for non-blocking connect
		if (select(server_fd + 1, NULL, &wfds, NULL, &time_out) == 1) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Server connection successful\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Server connection failed\n");
		}
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: connect\n");
	}

	close(server_fd);
	rtos_task_delete(NULL);
}

void example_nonblock_connect(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_nonblock_connect_thread"), example_nonblock_connect_thread, NULL, 1024 * 4,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_nonblock_connect_thread) failed", __FUNCTION__);
	}
}
