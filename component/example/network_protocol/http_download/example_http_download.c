#include "lwip_netconf.h"

#define SERVER_HOST    "httpbin.org"	//"192.168.1.100"(ex: local test)
#define SERVER_PORT    80
#define RESOURCE       "/image/jpeg"	//"/download.bin"(ex: local test)
#define BUFFER_SIZE    2048
#define RECV_TO        5000	// ms

static void example_http_download_thread(void *param)
{
	int server_fd = -1;
	struct sockaddr_in server_addr;
	struct hostent *server_host;

	(void) param;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	printf("\n====================Example: http_download====================\n");

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("ERROR: socket\n");
		goto exit;
	} else {
		struct timeval recv_timeout;
		recv_timeout.tv_sec = RECV_TO / 1000;
		recv_timeout.tv_usec = (RECV_TO % 1000) * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(SERVER_HOST);

	if (server_host != NULL) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		printf("ERROR: server host\n");
		goto exit;
	}

	if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		unsigned char buf[BUFFER_SIZE + 1];
		int pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		sprintf((char *)buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		printf("\nHTTP GET Request header:\n%s", buf);

		write(server_fd, (char const *)buf, strlen((char const *)buf));

		while ((read_size = read(server_fd, buf + pos, BUFFER_SIZE - pos)) > 0) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				buf[pos] = 0;
				header = strstr((char const *)buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					printf("\nHTTP GET Response header:\n%s\n", buf);

					// Remove header size to get first read size of data from body head
					read_size = pos - ((unsigned char *) body - buf);
					pos = 0;

					content_len_pos = strstr((char const *)buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(char *)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
					}
				} else {
					if (pos >= BUFFER_SIZE) {
						printf("ERROR: HTTP header\n");
						goto exit;
					}

					continue;
				}
			}

			printf("read resource %d bytes\n", read_size);
			resource_size += read_size;
		}

		printf("exit read. ret = %d\n", read_size);
		printf("http content-length = %d bytes, download resource size = %d bytes\n", content_len, resource_size);
	} else {
		printf("ERROR: connect\n");
	}

exit:
	if (server_fd >= 0) {
		close(server_fd);
	}

	rtos_task_delete(NULL);
}

void example_http_download(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_http_download_thread"), example_http_download_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create(example_http_download_thread) failed", __FUNCTION__);
	}
}
