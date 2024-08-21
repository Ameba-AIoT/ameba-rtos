#include "platform_autoconf.h"
#include "lwipconf.h"
#include "http_client.h"
#include "example_http_client.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"


char *http_get_header(char *host, char *resource);

#define THREAD_STACK_SIZE 1024
static const char *host = "www.google.com.tw";
static const int port = 80;

void http_client(void)
{
	//char *message_fmt = "POST / HTTP/1.0\r\n\r\n";
	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes;
	char message[256], *response;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("[ERROR] Create socket failed\n");
	}

	server = gethostbyname(host);
	if (server == NULL) {
		printf("[ERROR] Get host ip failed\n");
		goto exit;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, 4);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("[ERROR] connect failed\n");
		goto exit ;
	}

	//send request
	snprintf(message, sizeof(message), "%s", http_get_header((char *)host, (char *)"/"));
	printf("\nRequest:\n%s\n", message);
	bytes = write(sockfd, (char const *)message, strlen((char const *)message));
	if (bytes < 0) {
		printf("[ERROR] send packet failed\n");
	}

	//receive response
	response = malloc(1500);
	if (response == NULL) {
		printf("[ERROR] malloc failed\n");
	}
	printf("Response:\n");
	do {
		memset(response, 0, 1500);
		bytes = read(sockfd, response, 1500 - 1);
		if (bytes < 0) {
			printf("[ERROR] receive packet failed\n");
		}
		if (bytes == 0) {
			break;
		}
		printf("%s", response);
	} while (bytes > 0);

exit:
	//close the socket
	close(sockfd);

	return;
}

static void http_client_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	//wait for wifi connected, a rough time
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		printf("Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}

	printf("\nExample:  http_client\n");

	http_client();
	rtos_task_delete(NULL);
}

void example_http_client(void)
{
	if (rtos_task_create(NULL, ((const char *)"http_client_thread"), http_client_thread, NULL, THREAD_STACK_SIZE * 4, 0) != SUCCESS) {
		printf("\n\r%s rtos_task_create(http_client_thread) failed\n", __FUNCTION__);
	}
	return;
}
