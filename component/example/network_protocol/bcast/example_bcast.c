#include "lwip_netconf.h"
#include "log.h"

static void example_bcast_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: bcast \n");

	int socket = -1;
	int broadcast = 1;
	struct sockaddr_in bindAddr;
	uint16_t port = 49152;
	unsigned char packet[1024];

	// Create socket
	if ((socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: socket failed\n");
		goto err;
	}

	// Set broadcast socket option
	if (setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: setsockopt failed\n");
		goto err;
	}

	// Set the bind address
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(socket, (struct sockaddr *) &bindAddr, sizeof(bindAddr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: bind failed\n");
		goto err;
	}


	while (1) {
		// Receive broadcast
		int packetLen;
		struct sockaddr from;
		struct sockaddr_in *from_sin = (struct sockaddr_in *) &from;
		socklen_t fromLen = sizeof(from);

		if ((packetLen = recvfrom(socket, &packet, sizeof(packet), 0, &from, &fromLen)) >= 0) {
			uint8_t *ip = (uint8_t *) &from_sin->sin_addr.s_addr;
			uint16_t from_port = ntohs(from_sin->sin_port);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "recvfrom - %d bytes from %d.%d.%d.%d:%d\n", packetLen, ip[0], ip[1], ip[2], ip[3], from_port);
		}

		// Send broadcast
		if (packetLen > 0) {
			int sendLen;
			struct sockaddr to;
			struct sockaddr_in *to_sin = (struct sockaddr_in *) &to;
			to_sin->sin_family = AF_INET;
			to_sin->sin_port = htons(port);
			to_sin->sin_addr.s_addr = INADDR_BROADCAST;

			if ((sendLen = sendto(socket, packet, (packetLen <= 1024) ? packetLen : 1024, 0, &to, sizeof(struct sockaddr))) < 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: sendto broadcast\n");
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "sendto - %d bytes to broadcast:%d\n", sendLen, port);
			}
		}
	}


err:
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ERROR: broadcast example failed\n");
	close(socket);
	rtos_task_delete(NULL);
	return;
}

void example_bcast(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_bcast_thread"), example_bcast_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_bcast_thread) failed", __FUNCTION__);
	}
}
