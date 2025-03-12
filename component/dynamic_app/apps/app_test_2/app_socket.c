
#include "app_syscall.h"
#include "lwip/sockets.h"


#define SERVER_IP       "192.168.137.1"
#define SERVER_PORT     8081

static const char *TAG = "APP_SOCKET";


void *socket_task(void *param)
{
	int server_socket;
	struct sockaddr_in server_addr;
	struct timespec tm;

	tm.tv_sec = 10;
	tm.tv_nsec = 100000000; //100ms

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);

	if (lwip_connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
		unsigned char response_buf[100];
		int read_size;
		APP_LOGI(TAG, "connect OK\n");

		while ((read_size = lwip_read(server_socket, response_buf, sizeof(response_buf))) > 0) {
			APP_LOGI(TAG, "read %d bytes\n", read_size);
			nanosleep(&tm, NULL);
			lwip_send(server_socket, response_buf, read_size, 0);
		}

		APP_LOGI(TAG, "ERROR: read %d\n", read_size);
		lwip_close(server_socket);
	} else {
		APP_LOGI(TAG, "ERROR: connect\n");
		close(server_socket);
	}

	return NULL;
}
