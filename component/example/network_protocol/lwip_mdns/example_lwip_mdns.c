#include "wifi_api.h"
#include "lwip_netconf.h"
#include "mdns.h"

extern struct netif xnetif[NET_IF_NUM];

static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
	(void)txt_userdata;
	char content[50] = {"Here is ameba lwip_mdns service!"};
	mdns_resp_add_service_txtitem(service, content, strlen(content));
}

static void example_lwip_mdns_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: LWIP mDNS\r\n");

	mdns_resp_init();

	if (mdns_resp_add_netif(&xnetif[0], "realtek", 30)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "mdns_resp_add_netif fail \r\n");
		goto exit;
	}

	if (mdns_resp_add_service(&xnetif[0], "ameba", "_service1", DNSSD_PROTO_TCP, 5000, 30, srv_txt, NULL)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "mdns_resp_add_service fail \r\n");
		goto exit;
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: LWIP mDNS service start \n");

	rtos_time_delay_ms(60 * 1000);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nExample: LWIP mDNS service stop \n");

	mdns_resp_remove_netif(&xnetif[0]);
exit:
	rtos_task_delete(NULL);
	return;
}

void example_lwip_mdns(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_lwip_mdns_thread"), example_lwip_mdns_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_lwip_mdns_thread) failed", __FUNCTION__);
	}
}
