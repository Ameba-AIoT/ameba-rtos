/* example/network_protocol/ssdp/example_ssdp.c */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "ssdp.h"
#include "example_ssdp.h"

#if EXAMPLE_SSDP_DEVICE
#include "httpd/httpd.h"
#endif

#define TAG "SSDP_DEMO"

/* ── Device mode: description.xml HTTP callback ─────────────── */

#if EXAMPLE_SSDP_DEVICE && SSDP_DEVICE_DESC_ENABLED
static void desc_xml_cb(struct httpd_conn *conn)
{
	if (httpd_request_is_method(conn, (char *)"GET")) {
		const char *xml = ssdp_get_description_xml();
		size_t      len = strlen(xml);

		httpd_response_write_header_start(conn, (char *)"200 OK",
										  (char *)"text/xml; charset=\"utf-8\"", len);
		httpd_response_write_header(conn, (char *)"Connection", (char *)"close");
		httpd_response_write_header_finish(conn);
		httpd_response_write_data(conn, (uint8_t *)xml, len);
	} else {
		httpd_response_method_not_allowed(conn, NULL);
	}

	httpd_conn_close(conn);
}
#endif

/* ── Control Point mode: discovery callback ─────────────────── */

#if EXAMPLE_SSDP_CONTROL_POINT
static void ssdp_found_cb(const char *usn, const char *location, const char *st)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "[Found device]\r\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  USN:      %s\r\n", usn      ? usn      : "(none)");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  LOCATION: %s\r\n", location ? location : "(none)");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  ST:       %s\r\n", st       ? st       : "(none)");
}
#endif

/* ── Example thread ─────────────────────────────────────────── */

static void example_ssdp_thread(void *param)
{
	ssdp_info_t info;
	(void)param;

	/* Wait for Wi-Fi to be ready */
	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "\r\n====================Example: SSDP====================\r\n");

	memset(&info, 0, sizeof(info));

	/* ── Device mode configuration ──────────────────────────────── */
#if EXAMPLE_SSDP_DEVICE
	{
		uint8_t *ip   = lwip_get_ip(NETIF_WLAN_STA_INDEX);
		/* Static buffer: ssdp_start copies via strncpy internally; info.location not referenced after the call */
		static char location[64];

		DiagSnPrintf(location, sizeof(location),
					 "http://%d.%d.%d.%d:%d/description.xml",
					 ip[0], ip[1], ip[2], ip[3], EXAMPLE_SSDP_HTTP_PORT);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Device location: %s\r\n", location);

		/* Start httpd to serve description.xml */
		if (httpd_start(EXAMPLE_SSDP_HTTP_PORT, 5, 4096,
						1 /* thread-per-connection */, 0 /* HTTP */) != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "httpd_start failed\r\n");
			rtos_task_delete(NULL);
			return;
		}
#if SSDP_DEVICE_DESC_ENABLED
		httpd_reg_page_callback((char *)"/description.xml", desc_xml_cb);
#endif

		info.device_type   = "urn:schemas-upnp-org:device:Basic:1";
		info.friendly_name = "Ameba SSDP Device";
		info.manufacturer  = "Realtek";
		info.model_name    = "RTL8730E";
		info.uuid          = NULL;      /* auto-generated from MAC */
		info.location      = location;
	}
#endif /* EXAMPLE_SSDP_DEVICE */

	/* ── Control Point mode configuration ───────────────────────── */
#if EXAMPLE_SSDP_CONTROL_POINT
	info.found_cb = ssdp_found_cb;
#endif

	/* Start SSDP */
	if (ssdp_start(&info) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ssdp_start failed\r\n");
#if EXAMPLE_SSDP_DEVICE
		httpd_stop();
#endif
		rtos_task_delete(NULL);
		return;
	}

#if EXAMPLE_SSDP_DEVICE && SSDP_DEVICE_DESC_ENABLED
	RTK_LOGS(TAG, RTK_LOG_INFO, "Description XML:\r\n%s\r\n",
			 ssdp_get_description_xml());
#endif

	/* Control Point: send initial M-SEARCH */
#if EXAMPLE_SSDP_CONTROL_POINT
	rtos_time_delay_ms(1000);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Searching for ssdp:all ...\r\n");
	ssdp_search("ssdp:all");
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO,
#if EXAMPLE_SSDP_DEVICE && EXAMPLE_SSDP_CONTROL_POINT
			 "SSDP running (Device + CP mode). "
#elif EXAMPLE_SSDP_DEVICE
			 "SSDP running (Device mode). "
#else
			 "SSDP running (CP mode). "
#endif
			 "Call ssdp_stop() to stop.\r\n");

	/* Run for 30s then gracefully stop, triggering ssdp:byebye */
	rtos_time_delay_ms(30000);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Stopping SSDP...\r\n");
	ssdp_stop();
#if EXAMPLE_SSDP_DEVICE
	httpd_stop();
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "SSDP stopped.\r\n");

	rtos_task_delete(NULL);
}

/* ── Example entry point ────────────────────────────────────── */

void example_ssdp(void)
{
	if (rtos_task_create(NULL, "example_ssdp",
						 example_ssdp_thread, NULL,
						 4 * 1024, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "rtos_task_create failed\r\n");
	}
}
