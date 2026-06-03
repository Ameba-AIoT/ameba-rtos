#ifndef _WHC_HOST_APP_OTA_H_
#define _WHC_HOST_APP_OTA_H_

#define WHC_OTA_START		0
#define WHC_OTA_READ		1
#define WHC_OTA_WRITE		2
#define WHC_OTA_END			3
#define WHC_OTA_RMESH_INIT	4

/* OTA download type */
#define OTA_USER			0
#define OTA_HTTP			1
#define OTA_HTTPS			2
#define OTA_VFS				3

/* ota type */
#define OTA_FOR_NORMAL		1
#define OTA_FOR_RMESH		2

#ifndef ETH_ALEN
#define ETH_ALEN 			6
#endif
struct whc_host_ota_hdr {
	/* fix WHC_WIFI_TEST_OTA*/
	uint8_t type;
	uint8_t subtype;
	int16_t len;
};

struct whc_host_ota_info {
	uint32_t port;

	uint32_t host_len;
	uint32_t resource_len;

	uint8_t ota_type;
	uint8_t rmesh_ota_type;
	uint8_t rsvd[2];

	char *host;
	char *resource;
};

struct whc_ota_http_redirect {
	char *url;
	int len;
	uint16_t port;
	char *host;
	char *resource;
};

struct whc_ota_http_response {
	uint32_t  status_code;        /* HTTP status code */
	uint32_t  parse_status;       /* Parse status */
	uint32_t  body_len;           /* Response body length */
	uint32_t  header_len;         /* Response header length */
	uint8_t  *header_bak;         /* Response header backup */
	uint8_t  *body;               /* Response body pointer */
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
	int NextImgLen;
	char *NextImgBuf;
};

int whc_host_ota(void);
void whc_host_ota_from_dev(uint8_t *buf);
#ifdef CONFIG_RMESH
void whc_host_rmesh_ota_sock_hdl(struct msgtemplate *msg, uint32_t recv_len);
void whc_host_rmesh_ota_init_notify(void);
#endif

#endif
