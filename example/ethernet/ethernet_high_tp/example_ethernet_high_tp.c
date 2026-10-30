#include "ethernet.h"
#include "lwip_netconf.h"
#include "timer_api.h"
#include "kv.h"

static const char *TAG = "ETH_HIGH_TP";

/**
 * @brief  User-defined link status callback function
 * @param  link_up: 1 means link UP, 0 means link DOWN
 * @note   Runs in the example's own link thread: it may block and call netifapi_*.
 */
static void my_eth_link_callback(int link_up)
{
	if (link_up) {
		RTK_LOGI(TAG, "=== Ethernet Link UP ===\n");

		/* Notify LwIP Link is Up */
		if (!netif_is_up(pnetif_eth)) {
			netifapi_netif_set_up(pnetif_eth);
		}
		netifapi_netif_set_link_up(pnetif_eth);

		/* DHCP */
		u32 dhcp_status = lwip_request_ip(NETIF_ETH_INDEX);
		if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
			netifapi_netif_set_default(pnetif_eth);

			uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
			RTK_LOGI(TAG, "Ethernet DHCP got IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
		} else {
			RTK_LOGW(TAG, "Ethernet DHCP failed to get IP\n");
		}
	} else {
		/* Link Down Handling */
		RTK_LOGI(TAG, "=== Ethernet Link DOWN ===\n");

		/* Release IP and bring down the interface */
		netifapi_netif_set_link_down(pnetif_eth);
		lwip_dhcp_stop(NETIF_ETH_INDEX);
		RTK_LOGI(TAG, "Ethernet IP released\n");
	}
}

static void example_ethernet_link_thread(void *param)
{
	u32 speed, duplex;
	int last = -1;

	(void)param;
	for (;;) {
		if (rtos_sema_take(eth_link_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}

		int up = Ethernet_GetLinkStatus() ? 1 : 0;
		if (up == last) {
			continue;
		}
		last = up;

		if (up) {
			Ethernet_GetSpeedDuplex(&speed, &duplex);
			RTK_LOGI(TAG, "Link Up: %s, %s\n", (speed == ETH_SPEED_100M) ? "100M" : (speed == ETH_SPEED_10M) ? "10M" : "Unknown",
					 (duplex == ETH_FULL_DUPLEX) ? "Full Duplex" : "Half Duplex");
		}
		my_eth_link_callback(up);
	}
}

static void example_ethernet_high_tp_thread(void *param)
{
	(void)param;

	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  Ethernet High Throughput Example\n");
	RTK_LOGI(TAG, "========================================\n");

	if (rtos_sema_create_binary(&eth_link_sema) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create link sema failed!\n");
		rtos_task_delete(NULL);
		return;
	}

	if (rtos_task_create(NULL, "ETH_LINK", example_ethernet_link_thread, NULL, 1536, 3) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create link thread failed!\n");
		rtos_sema_delete(eth_link_sema);
		eth_link_sema = NULL;
		rtos_task_delete(NULL);
		return;
	}

	eth_init();

	RTK_LOGI(TAG, "Ethernet initialized. Waiting for link...\n");

	rtos_task_delete(NULL);
}

void example_ethernet_high_tp(void)
{
	if (rtos_task_create(NULL, "example_ethernet_high_tp", example_ethernet_high_tp_thread, NULL, 2048, 4) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create example_ethernet_high_tp task failed!\n");
	}
}
