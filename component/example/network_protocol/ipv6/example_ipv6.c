#include "platform_stdlib.h"
#include "basic_types.h"
#include "example_ipv6.h"
#include "lwip_netconf.h"
#include "wifi_ind.h"

#if defined(LWIP_IPV6) && (LWIP_IPV6 == 1)

extern struct netif xnetif[];

#define RECV_TO        1000	// ms

#if !LWIP_IPV6_SCOPES
static void ipv6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	struct netif *netifp = NULL;
	struct in6_addr in6addr;
	ip6_addr_t src, dest;
	err_t err;
	char send_data[MAX_SEND_SIZE] = "Hi client!";

	LWIP_UNUSED_ARG(arg);

	if (p == NULL) {
		printf("\n\r[ERROR] No pbuf, p == NULL\n");
		return;
	}

	netifp = &xnetif[0];

	if (netifp == NULL) {
		printf("\n\r[ERROR] No route, netif == NULL\n");
	} else {
		printf("\n\r[INFO] Receive data: %s\n", p->payload);

		memcpy(p->payload, send_data, MAX_SEND_SIZE);
		err = udp_sendto_if(pcb, p, addr, port, netifp);
		if (err == ERR_OK) {
			printf("\n\r[INFO] Send data to client successfully\n");
		} else {
			printf("\n\r[ERROR] Failed to send data, error:%d\n", err);
		}
	}
	pbuf_free(p);
}

static void example_ipv6_udp_server(void)
{
	struct udp_pcb *pcb;

	pcb = udp_new_ip6();
	if (pcb != NULL) {
		if (udp_bind(pcb, IP6_ADDR_ANY, UDP_SERVER_PORT) != 0) {
			printf("\n\r[ERROR] Bind failed\n");
			return;
		}
		printf("\n\r[INFO] Bind successfully\n");

		udp_recv(pcb, ipv6_recv, NULL);
	} else {
		printf("\n\r[ERROR] Unable to allocate pcb\n");
	}
}
#else
static void example_ipv6_udp_server(void)
{
	int                   server_fd;
	struct sockaddr_in6   ser_addr, client_addr;

	int addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!";
	char recv_data[MAX_RECV_SIZE];

	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(UDP_SERVER_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		printf("\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	printf("\n\r[INFO] Bind socket successfully\n");

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recvfrom(server_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen) > 0) {
			printf("\n\r[INFO] Receive data : %s\n", recv_data);
			//Send Response
			if (sendto(server_fd, send_data, MAX_SEND_SIZE, 0, (struct sockaddr *)&client_addr, addrlen) == -1) {
				printf("\n\r[ERROR] Send data failed\n");
			} else {
				printf("\n\r[INFO] Send data successfully\n");
			}
		}
	}

	closesocket(server_fd);

	return;
}
#endif

static void example_ipv6_udp_client(void)
{
	int                 client_fd;
	struct sockaddr_in6 ser_addr;
	struct sockaddr_in6 src_addr6;
	ip6_addr_t dest_addr6;

	int addrlen = sizeof(struct sockaddr_in6);
	int recv_timeout = RECV_TO;

	char recv_data[MAX_RECV_SIZE];
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(UDP_SERVER_PORT);
	inet_pton(AF_INET6, UDP_SERVER_IP, &(ser_addr.sin6_addr));
	inet6_addr_to_ip6addr(&dest_addr6, &(ser_addr.sin6_addr));

	if (ip6_addr_ismulticast_linklocal(&dest_addr6) || ip6_addr_ismulticast_iflocal(&dest_addr6)
		|| ip6_addr_islinklocal(&dest_addr6)) {
		memset(&src_addr6, 0, sizeof(src_addr6));
		src_addr6.sin6_family = AF_INET6;
		src_addr6.sin6_port = htons(UDP_SERVER_PORT);
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&xnetif[0].ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			printf("\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	while (1) {
		//Send data to server
		if (sendto(client_fd, send_data, MAX_SEND_SIZE, 0, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
			printf("\n\r[ERROR] Send data failed\n");
		} else {
			printf("\n\r[INFO] Send data to server successfully\n");
		}

		//Receive data from server response
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recvfrom(client_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&ser_addr, &addrlen) <= 0) {
			printf("\n\r[ERROR] Receive data timeout\n");
		} else {
			printf("\n\r[INFO] Receive from server: %s\n", recv_data);
		}
		rtos_time_delay_ms(1000);
	}

	closesocket(client_fd);

	return;
}

static void example_ipv6_tcp_server(void)
{
	int                   server_fd, client_fd;
	struct sockaddr_in6   ser_addr, client_addr;

	int addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!!";
	char recv_data[MAX_RECV_SIZE];

	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(TCP_SERVER_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		printf("\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	printf("\n\r[INFO] Bind socket successfully\n");

	//Make it listen to socket with max 20 connections
	if (listen(server_fd, 20) != 0) {
		printf("\n\r[ERROR] Listen socket failed\n");
		closesocket(server_fd);
		return;
	}
	printf("\n\r[INFO] Listen socket successfully\n");

	//Accept
	if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) == -1) {
		printf("\n\r[ERROR] Accept connection failed\n");
		closesocket(server_fd);
		closesocket(client_fd);
		return;
	}
	printf("\n\r[INFO] Accept connection successfully\n");

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recv(client_fd, recv_data, MAX_RECV_SIZE, 0) > 0) {
			printf("\n\r[INFO] Receive data : %s\n", recv_data);
			//Send Response
			if (send(client_fd, send_data, MAX_SEND_SIZE, 0) == -1) {
				printf("\n\r[ERROR] Send data failed\n");
			} else {
				printf("\n\r[INFO] Send data successfully\n");
			}
		}
	}
	closesocket(client_fd);
	closesocket(server_fd);
	return;
}

static void example_ipv6_tcp_client(void)
{
	int                 client_fd;
	struct sockaddr_in6 ser_addr;
	struct sockaddr_in6 src_addr6;
	ip6_addr_t dest_addr6;

	int addrlen = sizeof(struct sockaddr_in6);

	char recv_data[MAX_RECV_SIZE];
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) ==  -1) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(TCP_SERVER_PORT);
	inet_pton(AF_INET6, TCP_SERVER_IP, &(ser_addr.sin6_addr));

	inet6_addr_to_ip6addr(&dest_addr6, &(ser_addr.sin6_addr));

	if (ip6_addr_ismulticast_linklocal(&dest_addr6) || ip6_addr_ismulticast_iflocal(&dest_addr6)
		|| ip6_addr_islinklocal(&dest_addr6)) {
		memset(&src_addr6, 0, sizeof(src_addr6));
		src_addr6.sin6_family = AF_INET6;
		src_addr6.sin6_port = htons(TCP_SERVER_PORT);
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&xnetif[0].ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			printf("\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	//Connecting to server
	if (connect(client_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
		printf("\n\r[ERROR] Connect to server failed\n");
	}
	printf("[INFO] Connect to server successfully\n");

	while (1) {
		//Send data to server
		if (send(client_fd, send_data, MAX_SEND_SIZE, 0) == -1) {
			printf("\n\r[ERROR] Send data failed\n");
		} else {
			printf("\n\r[INFO] Send data to server successfully\n");
		}

		//Receive data from server response
		if (recv(client_fd, recv_data, MAX_RECV_SIZE, 0) <= 0) {
			//printf("\n\r[ERROR] Receive data failed\n");
		} else {
			printf("\n\r[INFO] Receive from server: %s\n", recv_data);
		}
		rtos_time_delay_ms(1000);
	}
	closesocket(client_fd);
	return;
}

#if !LWIP_IPV6_SCOPES
static void example_ipv6_mcast_server(void)
{
	struct udp_pcb *pcb;
	ip6_addr_t mcast_addr;

	pcb = udp_new_ip6();
	if (pcb != NULL) {
		if (udp_bind(pcb, IP6_ADDR_ANY, MCAST_GROUP_PORT) != 0) {
			printf("\n\r[ERROR] Bind failed\n");
			return;
		}
		printf("\n\r[INFO] Bind successfully\n");

		inet_pton(AF_INET6, MCAST_GROUP_IP, &(mcast_addr.addr));
		if (mld6_joingroup(IP6_ADDR_ANY, &mcast_addr) != 0) {
			printf("\n\r[ERROR] Register to ipv6 multicast group failed\n");
		}

		udp_recv(pcb, ipv6_recv, NULL);
	} else {
		printf("\n\r[ERROR] Unable to allocate pcb\n");
	}
}
#else
static void example_ipv6_mcast_server()
{
	int                   server_fd;
	struct sockaddr_in6   ser_addr, client_addr;

	int addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!";
	char recv_data[MAX_RECV_SIZE];

	//Register to multicast group membership
	ip6_addr_t mcast_addr;
	inet_pton(AF_INET6, MCAST_GROUP_IP, &(mcast_addr.addr));
	ip6_addr_assign_zone(&mcast_addr, IP6_MULTICAST, &xnetif[0]);

	if (mld6_joingroup(IP6_ADDR_ANY, &mcast_addr) != 0) {
		printf("\n\r[ERROR] Register to ipv6 multicast group failed\n");
	}

	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(MCAST_GROUP_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		printf("\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	printf("\n\r[INFO] Bind socket successfully\n");

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recvfrom(server_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen) > 0) {
			printf("\n\r[INFO] Receive data : %s\n", recv_data);
			//Send Response
			if (sendto(server_fd, send_data, MAX_SEND_SIZE, 0, (struct sockaddr *)&client_addr, addrlen) == -1) {
				printf("\n\r[ERROR] Send data failed\n");
			} else {
				printf("\n\r[INFO] Send data successfully\n");
			}
		}
	}
	closesocket(server_fd);
	return;
}
#endif

static void example_ipv6_mcast_client(void)
{
	int                 client_fd;
	struct sockaddr_in6 ser_addr;
	struct sockaddr_in6 src_addr6;
	ip6_addr_t dest_addr6;

	int addrlen = sizeof(struct sockaddr_in6);
	int recv_timeout = RECV_TO;

	char recv_data[MAX_RECV_SIZE];
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) ==  -1) {
		printf("\n\r[ERROR] Create socket failed\n");
		return;
	}
	printf("\n\r[INFO] Create socket successfully\n");

	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(MCAST_GROUP_PORT);
	inet_pton(AF_INET6, MCAST_GROUP_IP, &(ser_addr.sin6_addr));
	inet6_addr_to_ip6addr(&dest_addr6, &(ser_addr.sin6_addr));
	ip6_addr_assign_zone(&dest_addr6, IP6_MULTICAST, &xnetif[0]);

	if (ip6_addr_ismulticast_linklocal(&dest_addr6) || ip6_addr_ismulticast_iflocal(&dest_addr6)
		|| ip6_addr_islinklocal(&dest_addr6)) {
		memset(&src_addr6, 0, sizeof(src_addr6));
		src_addr6.sin6_family = AF_INET6;
		src_addr6.sin6_port = htons(MCAST_GROUP_PORT);
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&xnetif[0].ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			printf("\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	while (1) {
		//Send data to server
		if (sendto(client_fd, send_data, MAX_SEND_SIZE, 0, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
			printf("\n\r[ERROR] Send data failed\n");
		} else {
			printf("\n\r[INFO] Send data to server successfully\n");
		}

		//Receive data from server response
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recvfrom(client_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&ser_addr, &addrlen) <= 0) {
			printf("\n\r[ERROR] Receive data timeout\n");
		} else {
			printf("\n\r[INFO] Receive from server: %s\n", recv_data);
		}
		rtos_time_delay_ms(1000);
	}
	closesocket(client_fd);
	return;
}

static void example_ipv6_thread(void *param)
{
	printf("\nExample: IPV6 \n");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		printf("Wait for WIFI connection ...\n");
		printf("Please use ATW0=ssid, ATW1=password, ATWC to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	LwIP_AUTOIP_IPv6(&xnetif[0]);
	//Wait for ipv6 addr process conflict done
	while (!ip6_addr_isvalid(netif_ip6_addr_state(&xnetif[0], 0))) {
		rtos_time_delay_ms(10);
	}

	/***---open a example service once!!---***/
	//example_ipv6_udp_server();
	//example_ipv6_tcp_server();
	//example_ipv6_mcast_server();

	//example_ipv6_udp_client();
	//example_ipv6_tcp_client();
	example_ipv6_mcast_client();

	rtos_task_delete(NULL);
}

void example_ipv6(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_thread"), example_ipv6_thread, NULL, 1024 * 4, 1 + PRIORITIE_OFFSET) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_ipv6_thread) failed", __FUNCTION__);
	}
}
#endif
