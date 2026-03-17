#ifndef _WHC_HOST_APP_OTA_H_
#define _WHC_HOST_APP_OTA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <linux/genetlink.h>

#define WHC_OTA_START		0
#define WHC_OTA_READ		1
#define WHC_OTA_WRITE		2
#define WHC_OTA_END			3

/* OTA download type */
#define OTA_USER			0
#define OTA_HTTP			1
#define OTA_HTTPS			2
#define OTA_VFS				3

struct whc_host_ota_hdr {
	/* fix WHC_WIFI_TEST_OTA*/
	uint8_t type;
	uint8_t subtype;
	int16_t len;
};

struct whc_host_ota_info {
	uint32_t port;
	char host[16];
	/* fix less than 20B now */
	char resource[20];
};

struct update_redirect_conn {
	char *url;
	int len;
	uint16_t port;
	char *host;
	char *resource;
};

struct whc_ota_context {
	char *host;
	int port;
	char *resource;
	char *ca_cert;
	char *client_cert;
	char *private_key;
	int fd;
	uint8_t type;
};

void whc_host_ota(void);

#endif
