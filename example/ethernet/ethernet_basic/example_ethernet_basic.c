#include "ethernet.h"
#include "lwip_netconf.h"
#include "timer_api.h"
#include "kv.h"

static const char *TAG = "ETH_BASIC";

/**
 * @brief  User-defined link status callback function
 * @param  link_up: 1 means link UP, 0 means link DOWN
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
		lwip_clear_ip(NETIF_ETH_INDEX);
		RTK_LOGI(TAG, "Ethernet IP released\n");
	}
}

static void example_ethernet_basic_thread(void *param)
{
	(void)param;

	RTK_LOGI(TAG, "========================================\n");
	RTK_LOGI(TAG, "  Ethernet Basic Example\n");
	RTK_LOGI(TAG, "========================================\n");

	eth_register_link_cb(my_eth_link_callback);

	eth_init();

	RTK_LOGI(TAG, "Ethernet initialized. Waiting for link...\n");

	rtos_task_delete(NULL);
}

void example_ethernet_basic(void)
{
	if (rtos_task_create(NULL, "example_ethernet_basic", example_ethernet_basic_thread, NULL, 2048, 4) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create example_ethernet_basic task failed!\n");
	}
}
