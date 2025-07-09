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

#include <rtw_ctrl_api.h>
#include <rtw_ctrl_wifi_types.h>
#include <whc_bridge_wpa_ops_api.h>
#include <whc_host_netlink.h>

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

int family_id = 0;
int sock_fd;
int num_of_ap = 0;
int num_of_ap_index = 0;
struct rtw_scan_result *scan_results = NULL;


/* Define API handler */
const struct cmd_func_t whc_bridge_api_handlers[] = {
	{"getmac", whc_bridge_host_cmd_getmac, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_GET_MAC_ADDR},
	{"set_network", whc_bridge_host_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"scan", whc_bridge_host_cmd_scan, CMD_WIFI_DO_SCAN, WHC_WPA_OPS_CUSTOM_API, WHC_WPA_OPS_CUSTOM_API_SCAN},
	{"scan_result", whc_bridge_host_cmd_scan_result, CMD_WIFI_SCAN_RESULT, 0, 0},
	{NULL, NULL, 0, 0},
};


/* Define EVENT */
const struct event_func_t whc_bridge_event_handlers[] = {
	{WHC_WPA_OPS_EVENT_SCANING, MSG_EVENT_SCANING, NULL},
	{WHC_WPA_OPS_EVENT_SCAN_COMPLETE, MSG_EVENT_SCAN_COMPLETE, event_handler_scan_complete},
	{0, 0, NULL},
};


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


void whc_bridge_host_cmd_hdl(char *input)
{
	char *args[MAX_INPUT_SIZE / 2];
	int arg_count = 0;
	uint8_t idx = 0;
	char *token = strtok(input, " ");

	while (token != NULL) {
		args[arg_count++] = token;
		token = strtok(NULL, " ");
	}

	if (arg_count == 0) {
		return;
	}

	for (int idx = 0; whc_bridge_api_handlers[idx].cmd != NULL; idx++) {
		if (strcmp(args[0], whc_bridge_api_handlers[idx].cmd) == 0) {
			if (whc_bridge_api_handlers[idx].handler) {
				u8 api_id = whc_bridge_api_handlers[idx].api_id;
				u32 cmd_category = whc_bridge_api_handlers[idx].cmd_category;
				u8 cmd_id = whc_bridge_api_handlers[idx].cmd_id;

				whc_bridge_api_handlers[idx].handler(arg_count, args, api_id,
													 cmd_category, cmd_id);
			} else {
				printf("Command '%s' is not implemented.\n", args[0]);
			}
			return;
		}
	}

	printf("Unknown command: %s\n", args[0]);

}


void whc_bridge_host_handle_rx_payload(char *pos, int len, int api_id, int chunk_index, char last_chunk)
{
	uint32_t bridge_event;
	int idx = 0;

	if (api_id == CMD_WIFI_SCAN_RESULT) {
		whc_bridge_host_print_scan_result(pos, chunk_index, last_chunk);

	} else {
		bridge_event = *(uint32_t *)pos;
		if (bridge_event == WHC_WIFI_TEST) {
			pos = pos + sizeof(uint32_t);
			switch (*pos) {
			case WHC_WIFI_TEST_GET_MAC_ADDR:
				printf("\nMAC ADDR %02x:%02x:%02x:%02x:%02x:%02x\n",
					   (u8)pos[1], (u8)pos[2], (u8)pos[3], (u8)pos[4], (u8)pos[5], (u8)pos[6]);
				break;
			case WHC_WIFI_TEST_GET_IP:
				printf("\nIP ADDR %d.%d.%d.%d\n", pos[1], pos[2], pos[3], pos[4]);
				break;
			default:
				break;
			}
		} else if (bridge_event == WHC_WPA_OPS_EVENT) {
			pos = pos + sizeof(uint32_t);
			for (idx = 0; whc_bridge_event_handlers[idx].wifi_event != 0; idx++) {
				if (*pos == whc_bridge_event_handlers[idx].wifi_event) {
					printf("\n%s \n", whc_bridge_event_handlers[idx].event_msg);
					if (whc_bridge_event_handlers[idx].handler) {
						whc_bridge_event_handlers[idx].handler();
					}
				}
			}
		} //End WHC_WPA_OPS_EVENT

	}

}

void whc_bridge_host_rx_buf_hdl(struct msgtemplate *msg)
{
	struct nlattr *nla;
	size_t payload_len;
	uint32_t api_id = 0;
	int len = 0;
	char *payload;
	int chunk_index;
	char last_chuck;

	if (msg->n.nlmsg_type == NLMSG_ERROR) {
		printf("Netlink message nlmsg_type: NLMSG_ERROR\n");
		return;
	}

	payload_len = msg->n.nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;

	int remaining = payload_len;
	nla = (struct nlattr *)((char *)msg + NLMSG_HDRLEN + GENL_HDRLEN);

	while (nla_ok(nla, remaining)) {
		switch (nla->nla_type) {
		case BRIDGE_ATTR_API_ID: {
			api_id = *(uint32_t *)NLA_DATA(nla);
			//printf("BRIDGE_ATTR_API_ID = %u\n", api_id);
		}
		break;
		case BRIDGE_ATTR_CHUNK_INDEX: {
			chunk_index = *(uint32_t *)NLA_DATA(nla);
		}
		break;
		case BRIDGE_ATTR_LAST_CHUNK: {
			last_chuck = *(uint8_t *)NLA_DATA(nla);
		}
		break;
		case BRIDGE_ATTR_PAYLOAD: {
			len = nla_len(nla);
			payload = NLA_DATA(nla);

		}
		break;
		default:
			printf("Unknow atrr %d\n", nla->nla_type);
			break;
		}

		nla = nla_next(nla, &remaining);
	}

	whc_bridge_host_handle_rx_payload(payload, len, api_id, chunk_index, last_chuck);
}

void sigint_handler(int sig)
{
	printf("\n[INFO] Caught SIGINT (Ctrl+C), cleaning up...\n");

	if (sock_fd >= 0) {
		close(sock_fd);
		printf("[INFO] Socket closed.\n");
	}

	exit(0);
}

int main(void)
{
	char input_buf[MAX_INPUT_SIZE];
	struct msgtemplate msg;
	int rx_len;
	struct pollfd fds[2];
	printf("Waiting for message from kernel or input command from user space\n");

	// Create and bind a socket
	sock_fd = whc_bridge_host_api_create_nl_socket(NETLINK_GENERIC, getpid());
	if (sock_fd < 0) {
		printf("Failed to create netlink socket");
		return -1;
	}

	signal(SIGINT, sigint_handler);

	// Retrieve the family id
	family_id = whc_bridge_host_api_get_family_id(sock_fd, WHC_CMD_GENL_NAME);
	if (family_id == 0) {
		printf("Failed to retrieve family id\n");
		close(sock_fd);
		return -1;
	}

	fds[0].fd = sock_fd;
	fds[0].events = POLLIN;  // Monitor for incoming Netlink messages
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN;  // Monitor for standard input

	printf("> ");
	fflush(stdout);

	while (1) {

		int ret = poll(fds, 2, -1);  // Block until a file descriptor is ready
		if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				rx_len = recv(sock_fd, &msg, sizeof(msg), 0);
				whc_bridge_host_rx_buf_hdl(&msg);
			}
			if (fds[1].revents & POLLIN) {
				if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
					input_buf[strcspn(input_buf, "\n")] = 0; // Remove newline
					if (strcmp(input_buf, "exit") == 0) {
						break;
					}
					whc_bridge_host_cmd_hdl(input_buf);
					printf("> ");
					fflush(stdout);
				}
			}
		} else {
			perror("poll error");
			break;
		}
	}

	close(sock_fd);
	return 0;
}

