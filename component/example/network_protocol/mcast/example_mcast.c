#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"

extern struct netif xnetif[];

static void example_mcast_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

#if LWIP_IGMP
	int err = 0;
	int socket = -1;
	const char *group_ip = "224.0.0.251";
	uint16_t port = 5353;

	// Delay to wait for IP by DHCP
	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "Wait for WIFI connection ...\n");
		rtos_time_delay_ms(2000);
	}
	RTK_LOGS(NOTAG, "\n Example: mcast \n ");

	// Set NETIF_FLAG_IGMP flag for netif which should process IGMP messages
	xnetif[0].flags |= NETIF_FLAG_IGMP;

	if ((socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		RTK_LOGS(NOTAG, "ERROR: socket - AF_INET, SOCK_DGRAM\n");
		err = -1;
	}

	// Add multicast group membership on this interface
	if (err == 0) {
		struct ip_mreq imr;
		imr.imr_multiaddr.s_addr = inet_addr(group_ip);
		imr.imr_interface.s_addr = INADDR_ANY;
		err = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
		if (err < 0) {
			RTK_LOGS(NOTAG, "ERROR: setsockopt - IP_ADD_MEMBERSHIP\n");
		}
	}

	// Specify outgoing interface too
	if (err == 0) {
		struct in_addr intfAddr;
		intfAddr.s_addr = INADDR_ANY;
		err = setsockopt(socket, IPPROTO_IP, IP_MULTICAST_IF, &intfAddr, sizeof(struct in_addr));
		if (err < 0) {
			RTK_LOGS(NOTAG, "ERROR: setsockopt - IP_MULTICAST_IF\n");
		}
	}

	// And start listening for packets
	if (err == 0) {
		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = htons(port);
		bindAddr.sin_addr.s_addr = INADDR_ANY;
		err = bind(socket, (struct sockaddr *) &bindAddr, sizeof(bindAddr));
		if (err < 0) {
			RTK_LOGS(NOTAG, "ERROR: bind\n");
		}
	}

	if (err == 0) {
		unsigned char packet[1024];

		while (1) {
			// Receive multicast
			int packetLen;
			struct sockaddr from;
			struct sockaddr_in *from_sin = (struct sockaddr_in *) &from;
			socklen_t fromLen = sizeof(from);

			if ((packetLen = recvfrom(socket, &packet, sizeof(packet), 0, &from, &fromLen)) >= 0) {
				uint8_t *ip = (uint8_t *) &from_sin->sin_addr.s_addr;
				uint16_t from_port = ntohs(from_sin->sin_port);
				RTK_LOGS(NOTAG, "recvfrom - %d bytes from %d.%d.%d.%d:%d\n", packetLen, ip[0], ip[1], ip[2], ip[3], from_port);
			}

			// Send multicast
			if (packetLen > 0) {
				int sendLen;
				struct sockaddr to;
				struct sockaddr_in *to_sin = (struct sockaddr_in *) &to;
				to_sin->sin_family = AF_INET;
				to_sin->sin_port = htons(port);
				to_sin->sin_addr.s_addr = inet_addr(group_ip);

				if ((sendLen = sendto(socket, packet, packetLen, 0, &to, sizeof(struct sockaddr))) < 0) {
					RTK_LOGS(NOTAG, "ERROR: sendto %s\n", group_ip);
				} else {
					RTK_LOGS(NOTAG, "sendto - %d bytes to %s:%d\n", sendLen, group_ip, port);
				}
			}
		}
	} else if (socket != -1) {
		close(socket);
	}
#else
	RTK_LOGS(NOTAG, "\nSHOULD ENABLE LWIP_IGMP\n");
#endif
	rtos_task_delete(NULL);
}

void example_mcast(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_mcast_thread"), example_mcast_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(init_thread) failed", __FUNCTION__);
	}
}
