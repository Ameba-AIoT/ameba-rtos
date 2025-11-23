#include "wifi_api.h"
#include "lwip_netconf.h"
#include "lwip/mld6.h"
#include "example_ipv6.h"

#if defined(LWIP_IPV6) && (LWIP_IPV6 == 1)

extern struct netif xnetif[];

#define RECV_TO        1000	// ms

#if !LWIP_IPV6_SCOPES
static void ipv6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	struct netif *netifp = NULL;
	err_t err;
	char send_data[MAX_SEND_SIZE] = "Hi client!";

	LWIP_UNUSED_ARG(arg);

	if (p == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] No pbuf, p == NULL\n");
		return;
	}

	netifp = pnetif_sta;

	if (netifp == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] No route, netif == NULL\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive data: %s\n", (char *)p->payload);

		memcpy(p->payload, send_data, MAX_SEND_SIZE);
		err = udp_sendto_if(pcb, p, addr, port, netifp);
		if (err == ERR_OK) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data to client successfully\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Failed to send data, error:%d\n", err);
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
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind failed\n");
			return;
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Bind successfully\n");

		udp_recv(pcb, ipv6_recv, NULL);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Unable to allocate pcb\n");
	}
}
#else
static void example_ipv6_udp_server(void)
{
	int                   server_fd;
	struct sockaddr_in6   ser_addr, client_addr;

	u32_t addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!";
	char recv_data[MAX_RECV_SIZE];
	int ret = 0;


	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(UDP_SERVER_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Bind socket successfully\n");

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if ((ret = recvfrom(server_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen)) >= 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) data=%s from client ipv6 address=", ret, recv_data);
			LwIP_DUMP_IPV6_ADDRESS(client_addr.sin6_addr.s6_addr);
			//Send Response
			if (sendto(server_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&client_addr, addrlen) < 0) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data successfully\n");
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] recvfrom() failed\n");
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

	u32_t addrlen = sizeof(struct sockaddr_in6);
	struct timeval recv_timeout;

	char recv_data[MAX_RECV_SIZE];
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

	recv_timeout.tv_sec = RECV_TO / 1000;
	recv_timeout.tv_usec = (RECV_TO % 1000) * 1000;
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
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&pnetif_sta->ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	int ret = 0;

	while (1) {
		//Send data to server
		if (sendto(client_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data to server successfully\n");
		}

		//Receive data from server response
		memset(recv_data, 0, MAX_RECV_SIZE);
		if ((ret = recvfrom(client_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&ser_addr, &addrlen)) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Receive data timeout\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) data=%s from server ipv6 address=", ret, recv_data);
			LwIP_DUMP_IPV6_ADDRESS(ser_addr.sin6_addr.s6_addr);
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

	u32_t addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!!";
	char recv_data[MAX_RECV_SIZE] = {0};

	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(TCP_SERVER_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Bind socket successfully\n");

	//Make it listen to socket with max 20 connections
	if (listen(server_fd, 20) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Listen socket failed\n");
		closesocket(server_fd);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Listen socket successfully\n");

	//Accept
	if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) == -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Accept connection failed\n");
		closesocket(server_fd);
		closesocket(client_fd);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Accept connection successfully from client ipv6 address=");
	LwIP_DUMP_IPV6_ADDRESS(client_addr.sin6_addr.s6_addr);

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if (recv(client_fd, recv_data, MAX_RECV_SIZE, 0) > 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) data : %s\n", strlen(recv_data), recv_data);
			//Send Response
			if (send(client_fd, send_data, strlen(send_data), 0) <= 0) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
				break;
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data successfully\n");
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Receive data failed\n");
			break;
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

	char recv_data[MAX_RECV_SIZE] = {0};
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) ==  -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

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
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&pnetif_sta->ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	//Connecting to server
	if (connect(client_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) == -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Connect to server failed\n");
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[INFO] Connect successfully to server ipv6 address=");
	LwIP_DUMP_IPV6_ADDRESS(ser_addr.sin6_addr.s6_addr);

	while (1) {
		//Send data to server
		if (send(client_fd, send_data, strlen(send_data), 0) <= 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
			break;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data to server successfully\n");
		}

		memset(recv_data, 0, MAX_RECV_SIZE);
		//Receive data from server response
		if (recv(client_fd, recv_data, MAX_RECV_SIZE, 0) <= 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Receive data failed\n");
			break;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) from server: %s\n", strlen(recv_data), recv_data);
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
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind failed\n");
			return;
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Bind successfully\n");

		inet_pton(AF_INET6, MCAST_GROUP_IP, &(mcast_addr.addr));
		if (mld6_joingroup(IP6_ADDR_ANY6, &mcast_addr) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Register to ipv6 multicast group failed\n");
		}

		udp_recv(pcb, ipv6_recv, NULL);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Unable to allocate pcb\n");
	}
}
#else
static void example_ipv6_mcast_server(void)
{
	int                   server_fd;
	struct sockaddr_in6   ser_addr, client_addr;

	u32_t addrlen = sizeof(struct sockaddr_in6);

	char send_data[MAX_SEND_SIZE] = "Hi client!";
	char recv_data[MAX_RECV_SIZE] = {0};
	int ret = 0;


	//Register to multicast group membership
	ip6_addr_t mcast_addr;
	inet_pton(AF_INET6, MCAST_GROUP_IP, &(mcast_addr.addr));
	ip6_addr_assign_zone(&mcast_addr, IP6_MULTICAST, pnetif_sta);

	if (mld6_joingroup(IP6_ADDR_ANY6, &mcast_addr) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Register to ipv6 multicast group failed\n");
	}

	//create socket
	if ((server_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

	//initialize structure dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(MCAST_GROUP_PORT);
	ser_addr.sin6_addr = (struct in6_addr) IN6ADDR_ANY_INIT;

	//Assign a port number to socket
	if (bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
		closesocket(server_fd);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Bind socket successfully\n");

	while (1) {
		memset(recv_data, 0, MAX_RECV_SIZE);
		if ((ret = recvfrom(server_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&client_addr, &addrlen)) > 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) data=%s from client ipv6 address=", ret, recv_data);
			LwIP_DUMP_IPV6_ADDRESS(client_addr.sin6_addr.s6_addr);
			//Send Response
			if (sendto(server_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&client_addr, addrlen) < 0) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data successfully\n");
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] recvfrom() failed\n");
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

	u32_t addrlen = sizeof(struct sockaddr_in6);
	struct timeval recv_timeout;

	char recv_data[MAX_RECV_SIZE] = {0};
	char send_data[MAX_SEND_SIZE] = "Hi Server!!";

	//create socket
	if ((client_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) ==  -1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Create socket failed\n");
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Create socket successfully\n");

	recv_timeout.tv_sec = RECV_TO / 1000;
	recv_timeout.tv_usec = (RECV_TO % 1000) * 1000;
	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

	//initialize value in dest
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin6_family = AF_INET6;
	ser_addr.sin6_port = htons(MCAST_GROUP_PORT);
	inet_pton(AF_INET6, MCAST_GROUP_IP, &(ser_addr.sin6_addr));
	inet6_addr_to_ip6addr(&dest_addr6, &(ser_addr.sin6_addr));
	ip6_addr_assign_zone(&dest_addr6, IP6_MULTICAST, pnetif_sta);

	if (ip6_addr_ismulticast_linklocal(&dest_addr6) || ip6_addr_ismulticast_iflocal(&dest_addr6)
		|| ip6_addr_islinklocal(&dest_addr6)) {
		memset(&src_addr6, 0, sizeof(src_addr6));
		src_addr6.sin6_family = AF_INET6;
		src_addr6.sin6_port = htons(MCAST_GROUP_PORT);
		inet6_addr_from_ip6addr(&src_addr6.sin6_addr, (ip6_addr_t *)&pnetif_sta->ip6_addr[0]);

		if (bind(client_fd, (struct sockaddr *)&src_addr6, sizeof(src_addr6)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Bind socket failed\n");
			closesocket(client_fd);
			return;
		}
	}

	int ret = 0;

	while (1) {
		//Send data to server
		if (sendto(client_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Send data failed\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Send data to server successfully\n");
		}

		//Receive data from server response
		memset(recv_data, 0, MAX_RECV_SIZE);
		if ((ret = recvfrom(client_fd, recv_data, MAX_RECV_SIZE, 0, (struct sockaddr *)&ser_addr, &addrlen)) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[ERROR] Receive data timeout\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r[INFO] Receive (len=%d) data=%s from server ipv6 address=", ret, recv_data);
			LwIP_DUMP_IPV6_ADDRESS(ser_addr.sin6_addr.s6_addr);
		}
		rtos_time_delay_ms(1000);
	}
	closesocket(client_fd);
	return;

}

static void example_ipv6_udp_server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 UDP Server\n");

	example_ipv6_udp_server();

	rtos_task_delete(NULL);
}

static void example_ipv6_tcp_server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 TCP Server\n");

	example_ipv6_tcp_server();

	rtos_task_delete(NULL);
}

static void example_ipv6_mcast_server_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 MCAST Server\n");

	example_ipv6_mcast_server();

	rtos_task_delete(NULL);
}

static void example_ipv6_udp_client_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 UDP Client\n");

	example_ipv6_udp_client();

	rtos_task_delete(NULL);
}

static void example_ipv6_tcp_client_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 TCP Client\n");

	example_ipv6_tcp_client();

	rtos_task_delete(NULL);
}

static void example_ipv6_mcast_client_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: IPV6 MCAST Client\n");

	example_ipv6_mcast_client();

	rtos_task_delete(NULL);
}

static void example_ipv6_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: ipv6====================\r\n");

	LwIP_AUTOIP_IPv6(NETIF_WLAN_STA_INDEX);
	//Wait for ipv6 addr process conflict done
	while (!ip6_addr_isvalid(netif_ip6_addr_state(pnetif_sta, 0))) {
		rtos_time_delay_ms(10);
	}
	/***---open a example service once!!---***/
#if EXAMPLE_IPV6_UDP

#if UDP_SERVER
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_udp_server_thread"), example_ipv6_udp_server_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_udp_server_thread) failed", __FUNCTION__);
	}
#else
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_udp_client_thread"), example_ipv6_udp_client_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_udp_client_thread) failed", __FUNCTION__);
	}
#endif

#elif EXAMPLE_IPV6_TCP

#if TCP_SERVER
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_tcp_server_thread"), example_ipv6_tcp_server_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_tcp_server_thread) failed", __FUNCTION__);
	}
#else
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_tcp_client_thread"), example_ipv6_tcp_client_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_tcp_client_thread) failed", __FUNCTION__);
	}
#endif

#elif EXAMPLE_IPV6_MCAST

#if MCAST_SERVER
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_mcast_server_thread"), example_ipv6_mcast_server_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_mcast_server_thread) failed", __FUNCTION__);
	}
#else
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_mcast_client_thread"), example_ipv6_mcast_client_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_mcast_client_thread) failed", __FUNCTION__);
	}
#endif

#endif
	rtos_task_delete(NULL);
}

void example_ipv6(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_ipv6_thread"), example_ipv6_thread, NULL, 1024 * 4, 1 + 4) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_ipv6_thread) failed", __FUNCTION__);
	}
}
#endif
