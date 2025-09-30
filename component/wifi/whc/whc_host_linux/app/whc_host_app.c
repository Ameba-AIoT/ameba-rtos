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

static char *whc_cmd_args[MAX_ARG_COUNT] = {0};
char whc_cmd_backup[MAX_INPUT_SIZE] = {0};
static uint32_t whc_cmd_argc;

static struct whc_host_command_entry cmd_table[] = {
	{"getip", whc_host_get_ip},
	{"getmac", whc_host_get_mac},
	{"setrdy", whc_host_set_state},
	{"tickps", whc_host_set_tickps_cmd},
	{"setmac", whc_host_set_mac},
	{"netifon", whc_host_set_netif_on},
	{"init", whc_host_nl_init},
	{"wifion", whc_host_wifi_on},
	{"scan", whc_host_wifi_scan},
	{"dhcp", whc_host_wifi_dhcp},
	{"connect", whc_host_wifi_connect},
	{"iwpriv", whc_host_wifi_mp},
	{"dbg", whc_host_wifi_dbg},
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

int whc_host_get_mac(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	uint8_t idx;

	if (whc_cmd_argc < 2) {
		printf("err: wlan index is needed !\n");
		return -1;
	} else {
		idx = *whc_cmd_args[1] - '0';
	}

	if ((idx != 1) && (idx != 0)) {
		printf("wrong wlan index %d, must be 0 or 1!\n", idx);
		return -1;
	}

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

int whc_host_get_ip(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	uint8_t idx;

	if (whc_cmd_argc < 2) {
		printf("err: wlan index is needed !\n");
		return -1;
	} else {
		idx = *whc_cmd_args[1] - '0';
	}

	if ((idx != 1) && (idx != 0)) {
		printf("wrong wlan index %d, must be 0 or 1!\n", idx);
		return -1;
	}

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


int whc_host_set_state(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	uint8_t state;

	if (whc_cmd_argc < 2) {
		printf("err: host state need !\n");
		return -1;
	}

	if (strcmp(whc_cmd_args[1], "ready") == 0) {
		state = WHC_WIFI_TEST_SET_READY;
	} else {
		state = WHC_WIFI_TEST_SET_UNREADY;
	}

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = state;
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);

	return ret;
}

int whc_host_set_tickps_cmd(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	int ret = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	if (whc_cmd_argc < 2) {
		printf("err: tickps cmd needed to set subtype!\n");
		return -1;
	}

	*ptr = WHC_WIFI_TEST_SET_TICKPS_CMD;
	ptr += 1;
	buf_len += 1;

	if (strcmp(whc_cmd_args[1], "r") == 0) {
		*ptr = WHC_CMD_TICKPS_R;
	} else if (strcmp(whc_cmd_args[1], "cg") == 0) {
		*ptr = WHC_CMD_TICKPS_TYPE_CG;
	} else if (strcmp(whc_cmd_args[1], "pg") == 0) {
		*ptr = WHC_CMD_TICKPS_TYPE_PG;
	}
	ptr += 1;
	buf_len += 1;

	ret = whc_host_api_send_nl_data(buf, buf_len);
	return ret;
}

/* below for kernel setting */
int whc_host_set_netif_on(void)
{
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;
	uint8_t idx;

	if (whc_cmd_argc < 2) {
		printf("err: wlan index is needed !\n");
		return -1;
	} else {
		idx = *whc_cmd_args[1] - '0';
	}

	if ((idx != 1) && (idx != 0)) {
		printf("wrong wlan index %d, must be 0 or 1!\n", idx);
		return -1;
	}

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
int whc_host_set_mac(void)
{
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;
	uint8_t idx;

	if (whc_cmd_argc < 3) {
		printf("err: hw mac and wlan index are needed !\n");
		return -1;
	} else {
		idx = *whc_cmd_args[1] - '0';
	}

	if ((idx != 1) && (idx != 0)) {
		printf("wrong wlan index %d, must be 0 or 1!\n", idx);
		return -1;
	}

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SET_MAC);
	nla_put_u8(&ptr, WHC_ATTR_WLAN_IDX, idx);
	nla_put_string(&ptr, WHC_ATTR_STRING, whc_cmd_args[2]);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;
}

/* msg to kernel, call api for dbg and mp */
int whc_host_wifi_mp(void)
{
	//rm header "iwpriv"
	char *buf = whc_cmd_backup + strlen("iwpriv") + 1;

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
int whc_host_wifi_dbg(void)
{
	//rm header "dbg"
	char *buf = whc_cmd_backup + 4;

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

int whc_host_wifi_connect(void)
{
	uint8_t buf[128] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0, len = 0;
	char *pwd = NULL;

	int ret = 0;

	if (whc_cmd_argc > 2) {
		pwd = whc_cmd_args[2];
	}

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	*ptr = WHC_WIFI_TEST_CONNECT;
	ptr += 1;

	len = strlen(whc_cmd_args[1]);
	*ptr = len;
	ptr += 1;

	memcpy(ptr, whc_cmd_args[1], len);
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

	return ret;
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

/* msg to kernel, set mac address */
int whc_host_set_mac_internal(uint8_t idx, char *mac)
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

void whc_host_mp_result(uint8_t *buf)
{
	uint8_t size = strlen(buf);
	if (size > 1600) {
		printf("mp result error \r\n");
		return;
	}

	printf("%s \r\n", buf);
	return;
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
				whc_host_set_mac_internal(idx, mac);
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
			case WHC_WIFI_TEST_MP:
				whc_host_mp_result(pos + sizeof(uint32_t));
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

