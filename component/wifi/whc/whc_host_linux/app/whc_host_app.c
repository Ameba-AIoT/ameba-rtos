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

#include <whc_host_app_api.h>
#include <whc_host_netlink.h>

//#define WHC_AUTO_SETTING	1
#define WHC_WIFI_SYSTEM_CMD_SIZE          200
/* depends on real name, wlan0 for common, maybe wlan1 in some raspi*/
/* enable auto setting after WHC_WIFI_NETDEV_NAME check */
#define WHC_WIFI_NETDEV_NAME              "wlan0"

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

struct whc_netlink whc_netlink_info;

void whc_host_rx_buf_hdl(struct msgtemplate *msg);

void parse_arguments(char *input, char args[MAX_ARG_COUNT][MAX_ARG_LENGTH], int *arg_count)
{
	char *token;
	*arg_count = 0;

	token = strtok(input, " ");
	while (token != NULL) {
		if (*arg_count < MAX_ARG_COUNT) {
			strncpy(args[*arg_count], token, MAX_ARG_LENGTH - 1);
			args[*arg_count][MAX_ARG_LENGTH - 1] = '\0';
			(*arg_count)++;
		}
		token = strtok(NULL, " ");
	}
}

int whc_host_get_mac_addr(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_MAC_ADDR;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;

}

int whc_host_get_ip(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_IP;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);

	return ret;
}


int whc_host_set_state(uint8_t state)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	if (state) {
		*ptr = WHC_WIFI_TEST_SET_READY;
	} else {
		*ptr = WHC_WIFI_TEST_SET_UNREADY;
	}
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);

	return ret;

}

int whc_host_set_tickps_cmd(char *subtype)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SET_TICKPS_CMD;
	ptr += 1;
	buf_len += 1;

	if (strcmp(subtype, "r") == 0) {
		*ptr = WHC_CMD_TICKPS_R;
	} else if (strcmp(subtype, "cg") == 0) {
		*ptr = WHC_CMD_TICKPS_TYPE_CG;
	} else if (strcmp(subtype, "pg") == 0) {
		*ptr = WHC_CMD_TICKPS_TYPE_PG;
	}
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;
}

/* below for kernel setting */
int whc_host_set_netif_on(uint8_t idx)
{
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_NETIF_ON);
	nla_put_u8(&ptr, WHC_ATTR_WLAN_IDX, idx);
	msg.n.nlmsg_len += ptr - msg.buf;
	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;
}

/* msg to kernel, set mac address */
int whc_host_set_mac(uint8_t idx, char *mac)
{
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SET_MAC);
	nla_put_u8(&ptr, WHC_ATTR_WLAN_IDX, idx);
	nla_put_string(&ptr, WHC_ATTR_STRING, mac);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;
}

/* msg to kernel, call api for dbg and mp */
int whc_host_wifi_mp(char *input)
{
	//rm header "mp"
	char *buf = input + 3;

	int nl_fd;
	int nl_family_id = 0;
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;
	struct msgtemplate ans;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_MP);
	nla_put_string(&ptr, WHC_ATTR_STRING, buf);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	int rep_len = recv(whc_netlink_info.sockfd, &ans, sizeof(ans), 0);
	whc_host_rx_buf_hdl(&ans);

	return ret;
}

/* msg to kernel, call api for dbg and mp */
int whc_host_wifi_dbg(char *input)
{
	//rm header "dbg"
	char *buf = input + 4;

	int nl_fd;
	int nl_family_id = 0;
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_DBG);
	nla_put_string(&ptr, WHC_ATTR_STRING, buf);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;
}

int whc_host_send_atcmd(char *data)
{
	int buf_len = strlen(data);
	uint8_t buf[256] = {0};
	uint8_t *ptr = buf;
	int ret = 0;

	printf("send atcmd(%d) : %s \r\n", buf_len, data);

	if (buf_len > 250) {
		printf("cmd len is too long!\r\n");
	} else {
		*(uint32_t *)ptr = WHC_ATCMD_TEST;
		ptr += 4;
		buf_len += 4;
		memcpy(ptr, data, strlen(data));
		ptr += strlen(data);
		*ptr = '\r';
		ptr += 1;
		*ptr = '\n';
		ptr += 1;
		buf_len += 2;
		ret = whc_host_api_send_nl_data(buf, buf_len);
	}

	return ret;
}

void whc_host_wifi_connect(char *ssid, char *pwd)
{
	uint8_t buf[128] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0, len = 0;

	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	*ptr = WHC_WIFI_TEST_CONNECT;
	ptr += 1;

	len = strlen(ssid);
	*ptr = len;
	ptr += 1;

	memcpy(ptr, ssid, len);
	ptr += len;

	if (pwd) {
		len = strlen(pwd);
		*ptr = len;
		ptr += 1;

		memcpy(ptr, pwd, len);
		ptr += len;

	} else {
		*ptr = 0;
		ptr += 1;
	}

	buf_len = ptr - buf;

	ret = whc_host_api_send_nl_data(buf, buf_len);
}

int whc_host_wifi_scan(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SCAN;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;
}

int whc_host_wifi_on(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_WIFION;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;
}

int whc_host_wifi_dhcp(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_DHCP;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;
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
	char *args[MAX_INPUT_SIZE / 2];
	int arg_count = 0;
	uint8_t idx = 0;
	char *token;
	char *pwd = NULL;
	char backup[MAX_INPUT_SIZE] = {0};

	/* backup original string before strtok */
	memcpy(backup, input, strlen(input));

	token = strtok(input, " ");
	while (token != NULL) {
		args[arg_count++] = token;
		token = strtok(NULL, " ");
	}

	if (arg_count > 0) {
		if (strcmp(args[0], "getip") == 0) {
			if (arg_count < 2) {
				printf("err: wlan index is needed !\n");
			} else {
				idx = *args[1] - '0';
				if ((idx == 1) || (idx == 0)) {
					whc_host_get_ip(idx);
				} else {
					printf("wrong wlan index %d, must be 0 or 1!\n", idx);
				}
			}
		} else if (strcmp(args[0], "getmac") == 0) {
			if (arg_count < 2) {
				printf("err: wlan index is needed !\n");
			} else {
				idx = *args[1] - '0';
				if ((idx == 1) || (idx == 0)) {
					whc_host_get_mac_addr(idx);
				} else {
					printf("wrong wlan index %d, must be 0 or 1!\n", idx);
				}
			}
		} else if (strcmp(args[0], "setrdy") == 0) {
			whc_host_set_state(1);
		} else if (strcmp(args[0], "unrdy") == 0) {
			whc_host_set_state(0);
		} else if (strcmp(args[0], "tickps") == 0) {
			if (arg_count < 2) {
				printf("err: tickps cmd needed to set subtype!\n");
			} else {
				whc_host_set_tickps_cmd(args[1]);
			}
		} else if (strcmp(args[0], "setmac") == 0) {
			if (arg_count < 3) {
				printf("err: hw mac and wlan index are needed !\n");
			} else {
				idx = *args[1] - '0';
				if ((idx == 1) || (idx == 0)) {
					whc_host_set_mac(idx, args[2]);
				} else {
					printf("wrong wlan index %d, must be 0 or 1!\n", idx);
				}
			}
		} else if (strcmp(args[0], "netifon") == 0) {
			if (arg_count < 2) {
				printf("err: wlan index is needed !\n");
			} else {
				idx = *args[1] - '0';
				if ((idx == 1) || (idx == 0)) {
					whc_host_set_netif_on(idx);
				} else {
					printf("wrong wlan index %d, must be 0 or 1!\n", idx);
				}
			}
		} else if (strcmp(args[0], "init") == 0) {
			whc_host_nl_init();
		} else if (strncmp(args[0], "AT", 2) == 0) {
			whc_host_send_atcmd((char *)args[0]);
		} else if (strcmp(args[0], "connect") == 0) {
			if (arg_count > 2) {
				pwd = args[2];
			}
			whc_host_wifi_connect(args[1], pwd);
		} else if (strcmp(args[0], "scan") == 0) {
			whc_host_wifi_scan();
		} else if (strcmp(args[0], "dhcp") == 0) {
			whc_host_wifi_dhcp();
		} else if (strcmp(args[0], "mp") == 0) {
			whc_host_wifi_mp(backup);
		} else if (strcmp(args[0], "dbg") == 0) {
			whc_host_wifi_dbg(backup);
		} else if (strcmp(args[0], "wifion") == 0) {
			whc_host_wifi_on();
		} else {
			printf("No command entered.\n");
		}
	}
}

void whc_host_scan_result(uint8_t *buf)
{
	uint32_t idx = buf[1];
	char *ap_info = buf + 4;

	if (idx == 0) {
		return;
	}

	printf("%s", ap_info);
}

int whc_host_init_ip(uint8_t *buf)
{
	int ret = 0;
	char cmd[WHC_WIFI_SYSTEM_CMD_SIZE] = {0};

	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0) && (buf[3] == 0)) {
		return ret;
	}

	if (snprintf(cmd, WHC_WIFI_SYSTEM_CMD_SIZE - 1, "ifconfig %s down", WHC_WIFI_NETDEV_NAME) == -1) {
		printf("whc_host_init_ip fail\n");
		return 1;
	}
	ret = system(cmd);

	if (ret == -1) {
		printf("ifconfig %s down fail\n", WHC_WIFI_NETDEV_NAME);
		return 1;
	}

	memset(cmd, 0, WHC_WIFI_SYSTEM_CMD_SIZE);

	if (snprintf(cmd, WHC_WIFI_SYSTEM_CMD_SIZE - 1, "ifconfig %s %d.%d.%d.%d", WHC_WIFI_NETDEV_NAME,
				 buf[0], buf[1], buf[2], buf[3]) == -1) {
		printf("whc_host_init_ip fail\n");
		return 1;
	}

	ret = system(cmd);

	if (ret == -1) {
		printf("ifconfig %s ip fail\n", WHC_WIFI_NETDEV_NAME);
		return 1;
	}

	if (snprintf(cmd, WHC_WIFI_SYSTEM_CMD_SIZE - 1, "ifconfig %s up", WHC_WIFI_NETDEV_NAME) == -1) {
		printf("whc_host_init_ip fail\n");
		return 1;
	}
	ret = system(cmd);

	if (ret == -1) {
		printf("ifconfig %s down up\n", WHC_WIFI_NETDEV_NAME);
		return 1;
	}

	return 0;
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
		if (whc_event == WHC_WIFI_TEST) {
			pos = pos + sizeof(uint32_t);
			switch (*pos) {
			case WHC_WIFI_TEST_GET_MAC_ADDR:
				idx = pos[1];
				printf("MAC ADDR %02x:%02x:%02x:%02x:%02x:%02x\n", pos[2], pos[3], pos[4], pos[5], pos[6], pos[7]);
				snprintf(mac, sizeof(mac) - 1, "%02x:%02x:%02x:%02x:%02x:%02x", pos[2], pos[3], pos[4], pos[5], pos[6], pos[7]);
				whc_host_set_mac(idx, mac);
				break;
			case WHC_WIFI_TEST_GET_IP:
				printf("IP ADDR %d.%d.%d.%d GW %d %d %d %d\n", pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], pos[8]);
#ifdef WHC_AUTO_SETTING
				whc_host_init_ip(&pos[1]);
#endif
				break;
			case WHC_WIFI_TEST_SCAN_RESULT:
				whc_host_scan_result(pos);
				break;
			default:
				break;
			}
		}
	}
}

int main(void)
{
	int family_id;
	char input_buf[MAX_INPUT_SIZE];
	struct msgtemplate msg;
	int rx_len;
	struct pollfd fds[2];
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

	while (1) {
		memset(input_buf, 0, MAX_INPUT_SIZE);

		fds[0].fd = whc_netlink_info.sockfd;
		fds[0].events = POLLIN;  // Monitor for incoming Netlink messages
		fds[1].fd = STDIN_FILENO;
		fds[1].events = POLLIN;  // Monitor for standard input

		int ret = poll(fds, 2, -1);  // Block until a file descriptor is ready
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
		} else {
			perror("poll error");
			break;
		}
	}

	close(whc_netlink_info.sockfd);
	return 0;
}

