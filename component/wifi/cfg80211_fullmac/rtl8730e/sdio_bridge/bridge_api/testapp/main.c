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

#include <rtw_sdio_bridge_api.h>
#include <rtw_sdio_bridge_netlink.h>

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

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

void wifi_disconnect_hdl(int event)
{
	printf("wifi disconnected from ap\n");
}

int main(int argc, char **argv)
{
	char *pwd = NULL;
	char input_buf[MAX_INPUT_SIZE];
	char input_copy[MAX_INPUT_SIZE];
	char args[MAX_ARG_COUNT][MAX_ARG_LENGTH];
	int arg_count = 0;
	int i = 0;
	pthread_t *handle = NULL;

	handle = bridge_wifi_event_callback_init();
	bridge_wifi_register_event_callback(BRIDGE_WIFI_EVENT_DISCONNECT, wifi_disconnect_hdl);

	while (1) {
start_again:
		memset(input_buf, 0, sizeof(input_buf));
		printf("input cmd:\n");
		if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
			input_buf[strcspn(input_buf, "\n")] = 0;
			if (strcmp(input_buf, "exit") == 0) {
				break;
			}

			strncpy(input_copy, input_buf, MAX_INPUT_SIZE);
			input_copy[MAX_INPUT_SIZE - 1] = '\0';
			parse_arguments(input_copy, args, &arg_count);

			if (arg_count < 1) {
				printf("err: NO parameter!\n");
				goto start_again;
			}

			if (strcmp(args[0], "wifi_connect") == 0) {
				if (arg_count > 2) {
					pwd = args[2];
				}
				bridge_wifi_connect(args[1], pwd);
			} else if (strcmp(args[0], "scan") == 0) {
				bridge_wifi_scan();
			} else if (strcmp(args[0], "scanres") == 0) {
				bridge_wifi_get_scanres();
			} else if (strcmp(args[0], "disconnect") == 0) {
				bridge_wifi_disconnect();
			}
		}
	}

	bridge_wifi_event_callback_deinit(handle);
	return 0;
}
