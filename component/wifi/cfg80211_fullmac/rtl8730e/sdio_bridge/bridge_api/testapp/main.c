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

int main(int argc, char **argv)
{
	char *pwd = NULL;

	if (argc < 2) {
		printf("err: NO parameter!\n");
		return -1;
	}

	if (strcmp(argv[1], "wifi_connect") == 0) {
		if (argc > 3) {
			pwd = argv[3];
		}
		bridge_wifi_connect(argv[2], pwd);
	} else if (strcmp(argv[1], "scan") == 0) {
		bridge_wifi_scan();
	} else if (strcmp(argv[1], "scanres") == 0) {
		bridge_wifi_get_scanres();
	} else if (strcmp(argv[1], "disconnect") == 0) {
		bridge_wifi_disconnect();
	}

	return 0;
}
