#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <poll.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <whc_host_app_api.h>
#include <whc_host_netlink.h>

#define WHC_WIFI_SYSTEM_CMD_SIZE          200
/* depends on real name, wlan0 for common, maybe wlan1 in some raspi*/
/* enable auto setting after WHC_WIFI_NETDEV_NAME check */
#define WHC_WIFI_NETDEV_NAME              "wlan0"

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

struct whc_netlink whc_netlink_info;

int rmesh_tx_socketfd;
uint32_t rmesh_server_port;
uint8_t rmesh_server_ip[4];
#define RMESH_PC_IP_ENTRY 2
#define RMESH_PC_PORT_ENTRY 3
#define RMESH_BCMC_PORT 12345

void whc_host_rx_buf_hdl(struct msgtemplate *msg);

static char *whc_cmd_args[MAX_ARG_COUNT] = {0};
char whc_cmd_backup[MAX_INPUT_SIZE] = {0};
static uint32_t whc_cmd_argc;

static struct whc_host_command_entry cmd_table[] = {
	{"init", whc_host_nl_init},
	{NULL, NULL}
};

void whc_host_execute_command(char *cmd)
{
	struct whc_host_command_entry *entry;

	for (entry = cmd_table; entry->cmd_name != NULL; entry++) {
		if (strcmp(cmd, entry->cmd_name) == 0) {
			entry->handler();
			return;
		}
	}
	printf("Unknown command: %s\n", cmd);
}

int whc_host_nl_init(void)
{
	int nl_fd;
	int nl_family_id = 0;
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_INFO_INIT);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;
}

void whc_host_cmd_hdl(char *input)
{
	char *token;
	char *cmd;

	/* backup original string before strtok */
	memset(whc_cmd_backup, 0, MAX_INPUT_SIZE);
	memcpy(whc_cmd_backup, input, strlen(input));
	whc_cmd_argc = 0;

	token = strtok(input, " ");
	while (token != NULL) {
		whc_cmd_args[whc_cmd_argc++] = token;
		token = strtok(NULL, " ");
	}

	if (whc_cmd_argc > 0) {
		cmd = whc_cmd_args[0];
		whc_host_execute_command(cmd);
	}
}

void whc_rmesh_socket_tx_handler(uint8_t *send_buf, uint32_t len)
{
	struct sockaddr_in dest_addr;
	struct ifreq ifr;
	socklen_t addr_len = sizeof(dest_addr);

	/*need fill IP address in this packet*/
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, WHC_WIFI_NETDEV_NAME, IFNAMSIZ - 1);
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
		printf("get ip fail\n");
	}
	close(fd);

	struct sockaddr_in *ipaddr = (struct sockaddr_in *)&ifr.ifr_addr;
	*(uint32_t *)(send_buf + 42) = (uint32_t)(ipaddr->sin_addr.s_addr);


	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(rmesh_server_port);
	dest_addr.sin_addr.s_addr = rmesh_server_ip[3] << 24 | rmesh_server_ip[2] << 16 | rmesh_server_ip[1] << 8 | rmesh_server_ip[0];
	if (sendto(rmesh_tx_socketfd, send_buf, len, 0, (struct sockaddr *)&dest_addr, addr_len) < 0) {
		printf("rmesh node rpt send fail");
	}
}

void whc_host_rx_buf_hdl(struct msgtemplate *msg)
{
	struct nlattr *na;
	uint32_t event;
	uint32_t whc_event;
	size_t payload_len;
	uint8_t *pos = NULL;
	int real_len;
	char mac[20] = {0};
	uint8_t idx;

	if (msg->n.nlmsg_type == NLMSG_ERROR) {
		printf("Netlink message nlmsg_type: NLMSG_ERROR\n");
		return;
	}

	if (msg->g.cmd == WHC_CMD_EVENT) {
		na = (struct nlattr *) GENLMSG_DATA(msg);
		event = *(uint32_t *) NLA_DATA(na);
	}
	payload_len = msg->n.nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;

	/* msg->buf: NLA_HDRLEN for nla type and len, left for pkt payload*/
	if (payload_len > 0) {
		pos = &msg->buf[NLA_HDRLEN];
		whc_event = *(uint32_t *)pos;
		if (event == WHC_RMESH_TEST) {
			real_len = *(uint32_t *)(pos + sizeof(uint32_t));
			pos = pos + sizeof(uint32_t) * 2;
			whc_rmesh_socket_tx_handler(pos, real_len);
		}
	}
}

void whc_host_bcmc_sock_hdl(struct msgtemplate *msg)
{
	if (msg->buf[13] == RMESH_PC_IP_ENTRY) {
		memcpy(rmesh_server_ip, msg->buf + 15, 4);
	}
	if (msg->buf[19] == RMESH_PC_PORT_ENTRY) {
		memcpy(&rmesh_server_port, msg->buf + 21, 2);
		rmesh_server_port = ntohs(rmesh_server_port);
	}
}

int main(void)
{
	int family_id;
	char input_buf[MAX_INPUT_SIZE];
	struct msgtemplate msg;
	int rx_len;
	struct pollfd fds[3];
	struct sockaddr_in bcmc_addr;
	int rmesh_bcmc_sock;

	printf("Waiting for message from kernel or input command from user space\n");

	memset(&whc_netlink_info, 0, sizeof(struct whc_netlink));

	// Create and bind a socket
	whc_netlink_info.sockfd = whc_host_api_create_nl_socket(NETLINK_GENERIC, getpid());
	if (whc_netlink_info.sockfd < 0) {
		printf("Failed to create netlink socket");
		return -1;
	}

	// Retrieve the family id
	whc_netlink_info.family_id = whc_host_api_get_family_id(whc_netlink_info.sockfd, WHC_CMD_GENL_NAME);
	if (whc_netlink_info.family_id == 0) {
		printf("Failed to retrieve family id\n");
		close(whc_netlink_info.sockfd);
		return -1;
	}
	//printf("Family ID for %s: %d\n", WHC_CMD_GENL_NAME, family_id);

	rmesh_tx_socketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (rmesh_tx_socketfd < 0) {
		printf("rmesh tx socket create fail\n");
	}

	rmesh_bcmc_sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&bcmc_addr, 0, sizeof(bcmc_addr));
	bcmc_addr.sin_family = AF_INET;
	bcmc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bcmc_addr.sin_port = htons(RMESH_BCMC_PORT);
	if (bind(rmesh_bcmc_sock, (struct sockaddr *)&bcmc_addr, sizeof(bcmc_addr)) < 0) {
		printf("UDP bind error");
	}

	while (1) {
		memset(input_buf, 0, MAX_INPUT_SIZE);

		fds[0].fd = whc_netlink_info.sockfd;
		fds[0].events = POLLIN;  // Monitor for incoming Netlink messages
		fds[1].fd = STDIN_FILENO;
		fds[1].events = POLLIN;  // Monitor for standard input
		fds[2].fd = rmesh_bcmc_sock;
		fds[2].events = POLLIN;  // Monitor for standard input

		int ret = poll(fds, 3, -1);  // Block until a file descriptor is ready
		if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				rx_len = recv(whc_netlink_info.sockfd, &msg, sizeof(msg), 0);
				whc_host_rx_buf_hdl(&msg);
			}
			if (fds[1].revents & POLLIN) {
				if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
					input_buf[strcspn(input_buf, "\n")] = 0; // Remove newline
					if (strcmp(input_buf, "exit") == 0) {
						break;
					}
					printf("Received cmd \n");
					whc_host_cmd_hdl(input_buf);
				}
			}
			if (fds[2].revents & POLLIN) {
				struct sockaddr_in from;
				socklen_t fromlen = sizeof(from);
				rx_len = recvfrom(rmesh_bcmc_sock, msg.buf, MAX_MSG_SIZE, 0, (struct sockaddr *)&from, &fromlen);
				if (rx_len >= 23) {
					whc_host_bcmc_sock_hdl(&msg);
					if (msg.buf[13] == RMESH_PC_IP_ENTRY) {
						memcpy(rmesh_server_ip, msg.buf + 15, 4);
					}
					if (msg.buf[19] == RMESH_PC_PORT_ENTRY) {
						memcpy(&rmesh_server_port, msg.buf + 21, 2);
						rmesh_server_port = ntohs(rmesh_server_port);
					}

				}

			}
		} else {
			perror("poll error");
			break;
		}
	}

	close(whc_netlink_info.sockfd);
	return 0;
}
