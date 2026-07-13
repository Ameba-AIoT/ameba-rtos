/* component/network/ssdp/ssdp.c */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "ssdp.h"

#include <string.h>
#include <errno.h>

#define SSDP_TAG             "SSDP"
#define SSDP_MULTICAST_ADDR  "239.255.255.250"
#define SSDP_PORT            1900
#define SSDP_ALIVE_BURST     3
#define SSDP_BURST_DELAY_MS  200
#define SSDP_RECV_BUF_SIZE   1024
#define SSDP_RECV_TIMEOUT_SEC 1
#define SSDP_TASK_STACK_SIZE (4 * 1024)
#define SSDP_TASK_PRIORITY   2

typedef struct {
	char             device_type[64];
	char             friendly_name[64];
	char             manufacturer[64];
	char             model_name[64];
	char             uuid[48];
	char             location[64];
	ssdp_found_cb_t  found_cb;
} ssdp_ctx_t;

static ssdp_ctx_t   g_ctx;
static int          g_sock    = -1;
static volatile int g_running = 0;
static rtos_task_t  g_task    = NULL;
static rtos_timer_t g_timer   = NULL;

#if SSDP_DEVICE_DESC_ENABLED
/* Fixed template 315 bytes + max field lengths (63+63+63+63+47=299) = 614 bytes → 640 with margin */
static char g_desc_xml[640];
#endif

/* ── UUID generation ───────────────────────────────────────── */

static void ssdp_gen_uuid(const uint8_t *mac, char *buf, size_t len)
{
	DiagSnPrintf(buf, len,
				 "uuid:%02X%02X%02X%02X-%02X%02X-0000-0000-%02X%02X%02X%02X%02X%02X",
				 mac[0], mac[1], mac[2], mac[3],
				 mac[4], mac[5],
				 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/* ── Message formatting (pure string, no socket dependency) ── */

static int ssdp_fmt_notify(char *buf, size_t sz, int alive,
						   const char *uuid, const char *device_type,
						   const char *location)
{
	if (alive) {
		return DiagSnPrintf(buf, sz,
							"NOTIFY * HTTP/1.1\r\n"
							"HOST: %s:%d\r\n"
							"CACHE-CONTROL: max-age=%d\r\n"
							"LOCATION: %s\r\n"
							"NT: %s\r\n"
							"NTS: ssdp:alive\r\n"
							"USN: %s::%s\r\n"
							"SERVER: FreeRTOS/1.0 UPnP/1.1 AmebaSSDP/1.0\r\n"
							"\r\n",
							SSDP_MULTICAST_ADDR, SSDP_PORT,
							SSDP_CACHE_CONTROL_MAX_AGE,
							location ? location : "",
							device_type,
							uuid, device_type);
	} else {
		return DiagSnPrintf(buf, sz,
							"NOTIFY * HTTP/1.1\r\n"
							"HOST: %s:%d\r\n"
							"NT: %s\r\n"
							"NTS: ssdp:byebye\r\n"
							"USN: %s::%s\r\n"
							"\r\n",
							SSDP_MULTICAST_ADDR, SSDP_PORT,
							device_type,
							uuid, device_type);
	}
}

static int ssdp_fmt_msearch(char *buf, size_t sz, const char *st)
{
	return DiagSnPrintf(buf, sz,
						"M-SEARCH * HTTP/1.1\r\n"
						"HOST: %s:%d\r\n"
						"MAN: \"ssdp:discover\"\r\n"
						"MX: 3\r\n"
						"ST: %s\r\n"
						"\r\n",
						SSDP_MULTICAST_ADDR, SSDP_PORT, st);
}

static int ssdp_fmt_reply(char *buf, size_t sz,
						  const char *uuid, const char *device_type,
						  const char *location)
{
	return DiagSnPrintf(buf, sz,
						"HTTP/1.1 200 OK\r\n"
						"CACHE-CONTROL: max-age=%d\r\n"
						"EXT:\r\n"
						"LOCATION: %s\r\n"
						"SERVER: FreeRTOS/1.0 UPnP/1.1 AmebaSSDP/1.0\r\n"
						"ST: %s\r\n"
						"USN: %s::%s\r\n"
						"\r\n",
						SSDP_CACHE_CONTROL_MAX_AGE,
						location ? location : "",
						device_type,
						uuid, device_type);
}

static const char *ssdp_parse_header(const char *msg, const char *key,
									 char *val, size_t val_sz)
{
	const char *p = msg;
	size_t klen   = strlen(key);

	while ((p = strstr(p, key)) != NULL) {
		if (p == msg || *(p - 1) == '\n') {
			p += klen;
			while (*p == ' ') {
				p++;
			}
			const char *end = strstr(p, "\r\n");
			if (!end) {
				end = p + strlen(p);
			}
			size_t len = (size_t)(end - p);
			if (len >= val_sz) {
				len = val_sz - 1;
			}
			memcpy(val, p, len);
			val[len] = '\0';
			return val;
		}
		p++;
	}
	return NULL;
}

/* ── Description XML ─────────────────────────────────────────── */

#if SSDP_DEVICE_DESC_ENABLED
static void ssdp_build_desc_xml(void)
{
	DiagSnPrintf(g_desc_xml, sizeof(g_desc_xml),
				 "<?xml version=\"1.0\"?>\r\n"
				 "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
				 "  <specVersion><major>1</major><minor>0</minor></specVersion>\r\n"
				 "  <device>\r\n"
				 "    <deviceType>%s</deviceType>\r\n"
				 "    <friendlyName>%s</friendlyName>\r\n"
				 "    <manufacturer>%s</manufacturer>\r\n"
				 "    <modelName>%s</modelName>\r\n"
				 "    <UDN>%s</UDN>\r\n"
				 "  </device>\r\n"
				 "</root>\r\n",
				 g_ctx.device_type,
				 g_ctx.friendly_name,
				 g_ctx.manufacturer,
				 g_ctx.model_name,
				 g_ctx.uuid);
}

const char *ssdp_get_description_xml(void)
{
	return g_desc_xml;
}
#endif /* SSDP_DEVICE_DESC_ENABLED */

/* ── Socket initialization ──────────────────────────────────── */

static int ssdp_open_socket(void)
{
	int sock;
	int reuse = 1;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	struct timeval timeout = { SSDP_RECV_TIMEOUT_SEC, 0 };

	pnetif_sta->flags |= NETIF_FLAG_IGMP;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR, "socket() failed\r\n");
		return -1;
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	/* Recv timeout: ssdp_task wakes periodically to re-check g_running, so it
	   exits cleanly even on lwIP builds without LWIP_NETCONN_FULLDUPLEX */
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(SSDP_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR, "bind() failed\r\n");
		close(sock);
		return -1;
	}

	mreq.imr_multiaddr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);
	mreq.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				   &mreq, sizeof(mreq)) < 0) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR, "IP_ADD_MEMBERSHIP failed\r\n");
		close(sock);
		return -1;
	}

	return sock;
}

/* ── Receive task ───────────────────────────────────────────── */

static void ssdp_reply_msearch(struct sockaddr_in *requester)
{
	char buf[512];
	int  n;

	if (!g_ctx.device_type[0]) {
		return;
	}

	n = ssdp_fmt_reply(buf, sizeof(buf),
					   g_ctx.uuid, g_ctx.device_type, g_ctx.location);
	sendto(g_sock, buf, n, 0,
		   (struct sockaddr *)requester, sizeof(*requester));
}

static void ssdp_dispatch_found(const char *msg)
{
	char usn[96]  = {0};
	char loc[64]  = {0};
	char st[64]   = {0};
	char nts[32]  = {0};

	if (!g_ctx.found_cb) {
		return;
	}

	ssdp_parse_header(msg, "USN: ",      usn, sizeof(usn));
	ssdp_parse_header(msg, "LOCATION: ", loc, sizeof(loc));

	if (strstr(msg, "NOTIFY * HTTP/1.1")) {
		ssdp_parse_header(msg, "NTS: ", nts, sizeof(nts));
		if (strncmp(nts, "ssdp:alive", 10) == 0) {
			ssdp_parse_header(msg, "NT: ", st, sizeof(st));
			g_ctx.found_cb(usn, loc, st);
		}
		return;
	}

	if (strstr(msg, "HTTP/1.1 200 OK")) {
		ssdp_parse_header(msg, "ST: ", st, sizeof(st));
		g_ctx.found_cb(usn, loc, st);
	}
}

static void ssdp_task(void *arg)
{
	char               buf[SSDP_RECV_BUF_SIZE];
	struct sockaddr_in sender;
	socklen_t          sender_len;
	int                len;

	(void)arg;

	while (g_running) {
		sender_len = sizeof(sender);
		len = recvfrom(g_sock, buf, sizeof(buf) - 1, 0,
					   (struct sockaddr *)&sender, &sender_len);
		if (len > 0) {
			buf[len] = '\0';
			if (strstr(buf, "M-SEARCH * HTTP/1.1")) {
				ssdp_reply_msearch(&sender);
			} else {
				ssdp_dispatch_found(buf);
			}
		} else if (len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			continue;   /* recv timeout: loop back to re-check g_running */
		} else {
			break;      /* socket closed (ENOTCONN) or fatal error */
		}
	}

	rtos_task_delete(NULL);
}

/* ── Lifecycle management ───────────────────────────────────── */

static void ssdp_keepalive_cb(void *arg)
{
	(void)arg;
	if (g_running && g_ctx.device_type[0]) {
		char buf[512];
		int  n;
		struct sockaddr_in dest;

		n = ssdp_fmt_notify(buf, sizeof(buf), 1,
							g_ctx.uuid, g_ctx.device_type, g_ctx.location);

		memset(&dest, 0, sizeof(dest));
		dest.sin_family      = AF_INET;
		dest.sin_port        = htons(SSDP_PORT);
		dest.sin_addr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);
		sendto(g_sock, buf, n, 0, (struct sockaddr *)&dest, sizeof(dest));
	}
}

int ssdp_start(const ssdp_info_t *info)
{
	uint8_t *mac;
	char     notify_buf[512];
	int      n, i;
	struct   sockaddr_in dest;

	if (!info || (!info->device_type && !info->found_cb)) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR,
				 "ssdp_start: device_type (Device) or found_cb (CP) required\r\n");
		return -1;
	}

	memset(&g_ctx, 0, sizeof(g_ctx));

	if (info->device_type) {
		strncpy(g_ctx.device_type, info->device_type,
				sizeof(g_ctx.device_type) - 1);
	}
	if (info->friendly_name) {
		strncpy(g_ctx.friendly_name, info->friendly_name,
				sizeof(g_ctx.friendly_name) - 1);
	}
	strncpy(g_ctx.manufacturer,
			info->manufacturer ? info->manufacturer : "Realtek",
			sizeof(g_ctx.manufacturer) - 1);
	strncpy(g_ctx.model_name,
			info->model_name ? info->model_name : "",
			sizeof(g_ctx.model_name) - 1);
	if (info->location) {
		strncpy(g_ctx.location, info->location, sizeof(g_ctx.location) - 1);
	}
	g_ctx.found_cb = info->found_cb;

	if (info->uuid) {
		strncpy(g_ctx.uuid, info->uuid, sizeof(g_ctx.uuid) - 1);
	} else {
		mac = lwip_get_mac(NETIF_WLAN_STA_INDEX);
		ssdp_gen_uuid(mac, g_ctx.uuid, sizeof(g_ctx.uuid));
	}

	g_sock = ssdp_open_socket();
	if (g_sock < 0) {
		return -1;
	}

	g_running = 1;

#if SSDP_DEVICE_DESC_ENABLED
	if (g_ctx.device_type[0]) {
		ssdp_build_desc_xml();
	}
#endif

	if (rtos_task_create(&g_task, "ssdp_task", ssdp_task, NULL,
						 SSDP_TASK_STACK_SIZE,
						 SSDP_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR, "Failed to create ssdp_task\r\n");
		close(g_sock);
		g_sock    = -1;
		g_running = 0;
		return -1;
	}

	if (g_ctx.device_type[0]) {
		memset(&dest, 0, sizeof(dest));
		dest.sin_family      = AF_INET;
		dest.sin_port        = htons(SSDP_PORT);
		dest.sin_addr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);

		for (i = 0; i < SSDP_ALIVE_BURST; i++) {
			n = ssdp_fmt_notify(notify_buf, sizeof(notify_buf), 1,
								g_ctx.uuid, g_ctx.device_type, g_ctx.location);
			sendto(g_sock, notify_buf, n, 0,
				   (struct sockaddr *)&dest, sizeof(dest));
			rtos_time_delay_ms(SSDP_BURST_DELAY_MS);
		}

		rtos_timer_create(&g_timer, "ssdp_ka", 0,
						  (SSDP_CACHE_CONTROL_MAX_AGE / 2) * 1000,
						  1,
						  ssdp_keepalive_cb);
		if (g_timer) {
			rtos_timer_start(g_timer, 0);
		}
	}

	RTK_LOGS(SSDP_TAG, RTK_LOG_INFO, "Started (uuid: %s)\r\n", g_ctx.uuid);
	return 0;
}

void ssdp_stop(void)
{
	char buf[256];
	int  n, i;
	struct sockaddr_in dest;

	if (!g_running) {
		return;
	}

	g_running = 0;

	if (g_ctx.device_type[0] && g_sock >= 0) {
		memset(&dest, 0, sizeof(dest));
		dest.sin_family      = AF_INET;
		dest.sin_port        = htons(SSDP_PORT);
		dest.sin_addr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);

		for (i = 0; i < SSDP_ALIVE_BURST; i++) {
			n = ssdp_fmt_notify(buf, sizeof(buf), 0,
								g_ctx.uuid, g_ctx.device_type, g_ctx.location);
			sendto(g_sock, buf, n, 0, (struct sockaddr *)&dest, sizeof(dest));
			rtos_time_delay_ms(SSDP_BURST_DELAY_MS);
		}
	}

	if (g_timer) {
		rtos_timer_delete(g_timer, 0);
		g_timer = NULL;
	}

	if (g_sock >= 0) {
		close(g_sock);
		g_sock = -1;
	}

	RTK_LOGS(SSDP_TAG, RTK_LOG_INFO, "Stopped\r\n");
}

int ssdp_search(const char *st)
{
	char buf[256];
	int  n;
	struct sockaddr_in dest;

	if (g_sock < 0 || !g_running) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR,
				 "ssdp_search: not running\r\n");
		return -1;
	}

	n = ssdp_fmt_msearch(buf, sizeof(buf), st ? st : "ssdp:all");

	memset(&dest, 0, sizeof(dest));
	dest.sin_family      = AF_INET;
	dest.sin_port        = htons(SSDP_PORT);
	dest.sin_addr.s_addr = inet_addr(SSDP_MULTICAST_ADDR);

	if (sendto(g_sock, buf, n, 0,
			   (struct sockaddr *)&dest, sizeof(dest)) < 0) {
		RTK_LOGS(SSDP_TAG, RTK_LOG_ERROR, "ssdp_search: sendto failed\r\n");
		return -1;
	}

	RTK_LOGS(SSDP_TAG, RTK_LOG_INFO, "Searching: %s\r\n",
			 st ? st : "ssdp:all");
	return 0;
}
