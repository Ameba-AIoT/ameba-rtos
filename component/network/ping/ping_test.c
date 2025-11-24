#include "lwip_netconf.h"

#define printf	DiagPrintfNano

#define PING_IP		"192.168.159.1"
#define PING_TO		3000
#define PING_ID		0xABCD
#undef BUF_SIZE
#define BUF_SIZE	10000
#define STACKSIZE	1024

static unsigned short ping_seq = 0;
static int infinite_loop, ping_count, data_size, ping_interval, ping_call;
static int ping_interface;
static int ping_total_time = 0, ping_received_count = 0;
static unsigned char g_ping_terminate = 0;
rtos_task_t g_ping_task = NULL;
unsigned char *ping_buf = NULL;
unsigned char  *reply_buf = NULL;
int ping_socket;
extern struct netif xnetif[];

static void generate_ping_echo(unsigned char *buf, int size, int domain)
{
	int i;
	struct icmp_echo_hdr *pecho;

	for (i = 0; i < size; i ++) {
		buf[sizeof(struct icmp_echo_hdr) + i] = (unsigned char) i;
	}

	pecho = (struct icmp_echo_hdr *) buf;
	ICMPH_CODE_SET(pecho, 0);
	pecho->chksum = 0;
	pecho->id = PING_ID;
	pecho->seqno = htons(++ ping_seq);

	if (AF_INET == domain) {
		ICMPH_TYPE_SET(pecho, ICMP_ECHO);
		pecho->chksum = inet_chksum(pecho, sizeof(struct icmp_echo_hdr) + size);
	}
#if LWIP_IPV6
	else if (AF_INET6 == domain) {
		ICMPH_TYPE_SET(pecho, ICMP6_TYPE_EREQ);
	}
#endif
}

static void *get_addr_ptr(struct sockaddr *sa)
{
	void *ret = (void *) & ((struct sockaddr_in *)sa)->sin_addr;
#if LWIP_IPV6
	if (sa->sa_family == AF_INET6) {
		ret = (void *) & ((struct sockaddr_in6 *)sa)->sin6_addr;
	}
#endif
	return ret;
}

void ping_test(void *param)
{
	int i = 0;
	int min_time = PING_TO, max_time = 0;
	char *host = param;

	rtos_time_delay_ms(100);//wait log service thread done

	ping_total_time = 0;
	ping_received_count = 0;

	if (data_size < 0 || data_size > BUF_SIZE) {
		printf("\n\r[ERROR] %s: data size error, effective range from 0 to %d\n", __func__, BUF_SIZE);
		goto cleanup_host;
	}

	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_RAW;

	if (getaddrinfo(host, NULL, &hints, &res) != 0) {
		printf("\n\r[%s] Failed to resolve: %s\n", __func__, host);
		goto cleanup_host;
	}

	int family = res->ai_family;
	int proto = IP_PROTO_ICMP;
	int iphdr_len = IP_HLEN;
#if LWIP_IPV6
	if (family == AF_INET6) {
		proto = IP6_NEXTH_ICMP6;
		iphdr_len = IP6_HLEN;
		if (255 == ping_interface) {
			ping_interface = 0;
		}
	}
#endif
	ping_socket = socket(family, SOCK_RAW, proto);
	if (ping_socket < 0) {
		perror("Socket creation failed");
		goto cleanup_addrinfo;
	}

	if ((ping_interface == 0) || (ping_interface == 1)) {
		if (family == AF_INET) {
			struct sockaddr_in bind_addr = {
				.sin_family = AF_INET,
				.sin_addr.s_addr = *(u32_t *)LwIP_GetIP(ping_interface)
			};
			bind(ping_socket, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
		}
#if LWIP_IPV6
		else {
			struct netif * pnetif = LwIP_idx_get_netif(ping_interface);
			struct sockaddr_in6 bind_addr = { .sin6_family = AF_INET6 };
			memcpy(&bind_addr.sin6_addr, netif_ip6_addr(pnetif, 0)->addr, 16);
			bind(ping_socket, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
		}
#endif
	}

	int icmp_hdr_len = sizeof(struct icmp_echo_hdr);
	int ping_size = icmp_hdr_len + data_size;
	int reply_size = ping_size + iphdr_len;
	int header_size = icmp_hdr_len + iphdr_len;
	unsigned char *ping_buf = rtos_mem_malloc(ping_size);
	unsigned char *reply_buf = rtos_mem_malloc(reply_size);
	if (!ping_buf || !reply_buf) {
		printf("\n\r[%s] Buffer allocation failed\n", __func__);
		goto cleanup_buffers;
	}

	printf("\n\r[%s] PING %s %d(%d) bytes of data\n", __FUNCTION__, host, data_size, header_size + data_size);
	for (i = 0; ((i < ping_count) || (infinite_loop == 1)) && (!g_ping_terminate); i ++) {
		int pint_timeout = PING_TO;
		struct timeval timeout = {
			.tv_sec = pint_timeout / 1000,
			.tv_usec = (pint_timeout % 1000) * 1000
		};
		setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		generate_ping_echo(ping_buf, data_size, family);
		sendto(ping_socket, ping_buf, ping_size, 0, res->ai_addr, res->ai_addrlen);

		unsigned int ping_time = rtos_time_get_current_system_time_ms();
		while (1) {
			struct sockaddr_storage from_addr;
			socklen_t from_len = sizeof(from_addr);
			int recv_size = recvfrom(ping_socket, reply_buf, reply_size, 0, (struct sockaddr *)&from_addr, &from_len);
			if (recv_size >= header_size) {
				unsigned int reply_time = rtos_time_get_current_system_time_ms();
				int delta_time = reply_time - ping_time;
				if (memcmp(get_addr_ptr(res->ai_addr), get_addr_ptr((struct sockaddr *)&from_addr), (family == AF_INET) ? 4 : 16) == 0) {
					struct ip_hdr *iphdr = (struct ip_hdr *)reply_buf;
					struct icmp_echo_hdr *pecho = (family == AF_INET) ?
												  (struct icmp_echo_hdr *)(reply_buf + (IPH_HL(iphdr) * 4)) :
												  (struct icmp_echo_hdr *)(reply_buf + iphdr_len);

					if ((pecho->id == PING_ID) && (pecho->seqno == htons(ping_seq))) {
						char *ip_str = inet_ntoa(((struct sockaddr_in *)&from_addr)->sin_addr);
#if LWIP_IPV6
						if (family == AF_INET6) {
							ip_str = inet6_ntoa(((struct sockaddr_in6 *)&from_addr)->sin6_addr);
						}
#endif
						printf("\n\r[%s] %d bytes from %s: icmp_seq=%d time=%d ms\r\n", __FUNCTION__, data_size, ip_str,
							   htons(pecho->seqno), delta_time);
						ping_received_count++;
						ping_total_time += delta_time;
						if (delta_time > max_time) {
							max_time = delta_time;
						}
						if (delta_time < min_time) {
							min_time = delta_time;
						}
						break;
					}
				}
				pint_timeout -= delta_time;
				if (pint_timeout > 0) {
					struct timeval timeout;
					timeout.tv_sec = pint_timeout / 1000;
					timeout.tv_usec = pint_timeout % 1000 * 1000;
					setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
					continue;
				}
			}
			printf("\n\r[%s] Request timeout for icmp_seq %d\n", __FUNCTION__, ping_seq);
			break;
		}

		rtos_time_delay_ms(ping_interval * 1000);
	}
	if (g_ping_terminate) {
		printf("\n[%s] ping test terminate!\n", __FUNCTION__);
		ping_count = i;
	}
	if (ping_count == 0) {
		printf("\n\rNumber of echo requests to send cannot be zero\n\r");
	} else {
		printf("\n\r[%s] %d packets transmitted, %d received, %d%% packet loss, average %d ms", __FUNCTION__, ping_count, ping_received_count,
			   (ping_count - ping_received_count) * 100 / ping_count, ping_received_count ? ping_total_time / ping_received_count : 0);
		printf("\n\r[%s] min: %u ms, max: %u ms\n\r", __FUNCTION__, min_time, max_time);
	}

cleanup_buffers:
	if (ping_buf) {
		rtos_mem_free(ping_buf);
	}
	if (reply_buf) {
		rtos_mem_free(reply_buf);
	}
	close(ping_socket);

cleanup_addrinfo:
	freeaddrinfo(res);

cleanup_host:
	if (host) {
		rtos_mem_free(host);
	}

	if (!ping_call) {
		g_ping_task = NULL;
		rtos_task_delete(NULL);
	}
}

void do_ping_call(char *ip, int loop, int count)
{
	ping_call = 1;
	ping_seq = 0;
	data_size = 120;
	ping_interval = 1;
	infinite_loop = loop;
	ping_count = count;
	char *host;
	host = rtos_mem_malloc(strlen(ip) + 1);
	memset(host, 0, (strlen(ip) + 1));
	memcpy(host, ip, strlen(ip));
	ping_test(host);
}

int get_ping_report(int *ping_lost)
{
	*ping_lost = ping_count - ping_received_count;
	return 0;
}

int cmd_ping(int argc, char **argv)
{
	int error_no = 0;
	int argv_count = 2;
	int argv_count_len = 0;
	char *host;
	char *endptr = NULL;

	g_ping_terminate = 0;
	ping_interface = 255;

	if (argc < 2) {
		error_no = 2;
		goto Exit;
	}

	while (argv_count <= argc) {
		//first operation
		if (argv_count == 2) {
			if (strcmp(argv[argv_count - 1], "stop") == 0) {
				if (argc == 2) {
					g_ping_terminate = 1;
					//rtos_time_delay_ms(100);
					return error_no;
				} else {
					error_no = 2;
					goto Exit;
				}
			} else {
				if (g_ping_task) {
					printf("\n\rPing: Ping task is already running.\n");
					return error_no;
				} else {
					//ping cmd default value
					infinite_loop = 0;
					ping_count    = 4;
					data_size     = 32;
					ping_interval = 1;
					ping_call     = 0;
					ping_seq      = 0;
					argv_count_len = strlen(argv[argv_count - 1]);
					host = rtos_mem_malloc(argv_count_len + 1);
					memset(host, 0, (argv_count_len + 1));
					strncpy(host, argv[argv_count - 1], argv_count_len);
					argv_count++;
				}
			}
		} else {
			if (strcmp(argv[argv_count - 1], "-t") == 0) {
				infinite_loop = 1;
				argv_count++;
			} else if (strcmp(argv[argv_count - 1], "-n") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 2;
					goto Exit;
				}
				ping_count = (int)strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || ping_count <= 0) {
					error_no = 2;
					goto Exit;
				}
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "-l") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 2;
					goto Exit;
				}
				data_size = (int)strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || data_size < 0 || data_size > BUF_SIZE) {
					printf("\n\r[ERROR] data size error, effective range from 0 to %d\n", BUF_SIZE);
					error_no = 2;
					goto Exit;
				}
				argv_count += 2;
			} else if (strcmp(argv[argv_count - 1], "if") == 0) {
				if (argc < (argv_count + 1)) {
					error_no = 2;
					goto Exit;
				}
				ping_interface = (int)strtol(argv[argv_count], &endptr, 10);
				if (*endptr != '\0' || endptr == argv[argv_count] || (ping_interface != 0 && ping_interface != 1)) {
					error_no = 2;
					goto Exit;
				}
				argv_count += 2;
			} else {
				error_no = 2;
				goto Exit;
			}
		}
	}

	if (g_ping_task == NULL) {
		if (rtos_task_create(&g_ping_task, (char const *)((const signed char *)"ping_test"), ping_test, host, STACKSIZE * 4,
							 1 + 4) != RTK_SUCCESS) {
			printf("\n\r Ping ERROR: Create ping task failed.");
		}
	}

Exit:
	return error_no;
}

void do_ping_test(char *ip, int size, int count, int interval)
{
	char *host;
	if ((sizeof(struct icmp_echo_hdr) + size) > BUF_SIZE) {
		printf("\n\r%s BUF_SIZE(%d) is too small", __FUNCTION__, BUF_SIZE);
		return;
	}

	if (ip == NULL) {
		host = rtos_mem_malloc(strlen(PING_IP) + 1);
		memset(host, 0, (strlen(PING_IP) + 1));
		strncpy(host, PING_IP, (strlen(PING_IP) + 1));
	} else {
		host = rtos_mem_malloc(strlen(ip) + 1);
		memset(host, 0, (strlen(ip) + 1));
		strncpy(host, ip, (strlen(PING_IP) + 1));
	}

	ping_call = 0;
	ping_seq = 0;
	data_size = size;
	ping_interval = interval;

	if (count == 0) {
		infinite_loop = 1;
		ping_count = 0;
	} else {
		infinite_loop = 0;
		ping_count = count;
	}

	if (rtos_task_create(NULL, (char const *)((const signed char *)"ping_test"), ping_test, host, STACKSIZE * 4, 1) != RTK_SUCCESS) {
		printf("\n\r%s rtos_task_create failed", __FUNCTION__);
	}
}
