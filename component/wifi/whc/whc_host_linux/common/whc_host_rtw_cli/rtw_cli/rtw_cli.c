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

#include <rtw_cli_api.h>
#include <rtw_cli_wifi_types.h>
#include <rtw_cli_netlink.h>

#include <whc_host_rtw_cli_api.h>
#include <whc_host_netlink.h>

int num_of_ap = 0;
int num_of_ap_index = 0;
struct rtw_scan_result *scan_results = NULL;
int direct_cli_cmd = 0;

struct rtw_cli_netlink_global *global = NULL;


/* Define API handler */
const struct cmd_func_t rtw_cli_cmd_handlers[] = {
	{"getmac",          rtw_cli_cmd_getmac, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_GET_MAC_ADDR},
	{"set_network",     rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"list_networks",   rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"select_network",  rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"remove_network",  rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"disconnect",      rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"status",          rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"wpas_cmd",        rtw_cli_cmd_set_network, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SET_NETWORK},
	{"scan",            rtw_cli_cmd_scan, CMD_WIFI_DO_SCAN, WHC_WPA_OPS_CUSTOM_API, WHC_WPA_OPS_CUSTOM_API_SCAN},
	{"scan_result",     rtw_cli_cmd_scan_result, CMD_WIFI_SCAN_RESULT, 0, 0},
	{"join_event",      rtw_cli_cmd_get_join_status, CMD_WIFI_GET_JOIN_EVENT, 0, 0},
	{"wpason",          rtw_cli_cmd_wpas_on, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_CUSTOM_API, WHC_WPA_OPS_CUSTOM_API_INIT_WPAS_STD},
	{"wifion",          rtw_cli_cmd_wifi_on, CMD_WIFI_SEND_BUF, WHC_WPA_OPS_CUSTOM_API, WHC_WPA_OPS_CUSTOM_API_WIFION},
	{"mp",              rtw_cli_cmd_mp, CMD_WIFI_MP, 0, 0},
	{"dbg",             rtw_cli_cmd_dbg, CMD_WIFI_DBG, 0, 0},
	{NULL, NULL, 0, 0},
};


/* Define EVENT */
const struct event_func_t rtw_cli_event_handlers[] = {
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


void rtw_cli_host_cmd_hdl(char *input)
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

	for (int idx = 0; rtw_cli_cmd_handlers[idx].cmd != NULL; idx++) {
		if (strcmp(args[0], rtw_cli_cmd_handlers[idx].cmd) == 0) {
			if (rtw_cli_cmd_handlers[idx].handler) {
				u8 api_id = rtw_cli_cmd_handlers[idx].api_id;
				u32 cmd_category = rtw_cli_cmd_handlers[idx].cmd_category;
				u8 cmd_id = rtw_cli_cmd_handlers[idx].cmd_id;

				rtw_cli_cmd_handlers[idx].handler(arg_count, args, api_id,
												  cmd_category, cmd_id);
			} else {
				printf("Command '%s' is not implemented.\n", args[0]);
			}
			return;
		}
	}

	printf("Unknown command: %s\n", args[0]);

}


void whc_cmd_handle_rx_payload(char *pos, int len, int api_id,
							   int chunk_index, char last_chunk)
{
	uint32_t whc_event;
	int idx = 0;

	if (api_id == CMD_WIFI_SCAN_RESULT) {
		whc_cmd_handle_rx_print_scan_result(pos, chunk_index,
											last_chunk);

	} else {
		whc_event = *(uint32_t *)pos;
		if (whc_event == WHC_WPA_OPS_UTIL) {
			pos = pos + sizeof(uint32_t);
			switch (*pos) {
			case WHC_WPA_OPS_UTIL_GET_MAC_ADDR:
				printf("\nMAC ADDR %02x:%02x:%02x:%02x:%02x:%02x\n",
					   (u8)pos[1], (u8)pos[2], (u8)pos[3], (u8)pos[4], (u8)pos[5], (u8)pos[6]);
				break;
			case WHC_WIFI_TEST_GET_IP:
				printf("\nIP ADDR %d.%d.%d.%d\n", pos[1], pos[2], pos[3], pos[4]);
				break;
			case WHC_WPA_OPS_UTIL_LIST_NETWORK:
				whc_cmd_handle_rx_list_network(pos);
				break;
			case WHC_WPA_OPS_UTIL_GET_STATUS:
				whc_cmd_handle_rx_get_status(pos);
				break;
			default:
				break;
			}
		} else if (whc_event == WHC_WPA_OPS_EVENT) {
			pos = pos + sizeof(uint32_t);
			for (idx = 0; rtw_cli_event_handlers[idx].wifi_event != 0; idx++) {
				if (*pos == rtw_cli_event_handlers[idx].wifi_event) {
					if (rtw_cli_event_handlers[idx].event_msg != NULL) {
						printf("\n%s \n", rtw_cli_event_handlers[idx].event_msg);
					}

					if (rtw_cli_event_handlers[idx].handler) {
						rtw_cli_event_handlers[idx].handler();
					}
				}
			}
		} //End WHC_WPA_OPS_EVENT

	}

}

void sigint_handler(int sig)
{
	printf("\n[INFO] Caught SIGINT (Ctrl+C), cleaning up...\n");

	rtw_cli_deinit_nl_global(global);

	if (global != NULL) {
		free(global);
	}

	exit(0);
}

int main(int argc, char *argv[])
{
	char input_buf[MAX_INPUT_SIZE] = {0};
	int rx_len;
	struct pollfd fds[2];
	int ret = 0;


	global = malloc(sizeof(*global));
	if (global == NULL) {
		return 0;
	}

	memset(global, 0, sizeof(*global));

	ret = rtw_cli_init_nl_global(global);
	if (ret < 0) {
		printf("Failed to create netlink socket");
		return -1;
	}

	signal(SIGINT, sigint_handler);

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			strcat(input_buf, argv[i]);
			if (i < argc - 1) {
				strcat(input_buf, " ");
			}
		}
		direct_cli_cmd = 1;

		printf("CLI: %s\n", input_buf);
		rtw_cli_host_cmd_hdl(input_buf);

		rtw_cli_deinit_nl_global(global);
		free(global);

		return 0;
	}

	printf("Waiting for message from kernel or input command from user space\n");

	fds[0].fd = nl_socket_get_fd(global->nl);
	fds[0].events = POLLIN;  // Monitor for incoming Netlink messages
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN;  // Monitor for standard input

	printf("> ");
	fflush(stdout);

	while (1) {

		int ret = poll(fds, 2, -1);  // Block until a file descriptor is ready
		if (ret > 0) {
			if (fds[0].revents & POLLIN) {
				rtw_cli_nl_recv(global);
			}
			if (fds[1].revents & POLLIN) {
				if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
					input_buf[strcspn(input_buf, "\n")] = 0; // Remove newline
					if (strcmp(input_buf, "exit") == 0) {
						break;
					}
					rtw_cli_host_cmd_hdl(input_buf);
					printf("> ");
					fflush(stdout);
				}
			}
		} else {
			perror("poll error");
			break;
		}
	}

	rtw_cli_deinit_nl_global(global);
	free(global);

	return 0;
}

