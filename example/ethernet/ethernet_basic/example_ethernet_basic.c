#include "ethernet.h"
#include "lwip_netconf.h"
#include "timer_api.h"
#include "kv.h"

static const char *TAG = "ETH_BASIC";

#define ETH_DHCP_POLL_MS        100
#define ETH_DHCP_POLL_CNT       300     /* give up reporting after 30 s; the DHCP client keeps trying */

static u32 eth_dhcp_poll_cnt;

/* lwIP timeout, runs in tcpip_thread: report the lease once DHCP has bound. */
static void eth_dhcp_check(void *arg)
{
	(void)arg;

	if (dhcp_supplied_address(pnetif_eth)) {
		netif_set_default(pnetif_eth);
		uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
		RTK_LOGI(TAG, "Ethernet DHCP got IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	} else if (++eth_dhcp_poll_cnt < ETH_DHCP_POLL_CNT) {
		sys_timeout(ETH_DHCP_POLL_MS, eth_dhcp_check, NULL);
	} else {
		RTK_LOGW(TAG, "Ethernet DHCP failed to get IP\n");
	}
}

/**
 * @brief  User-defined link status callback function
 * @param  link_up: 1 means link UP, 0 means link DOWN
 * @note   Runs in tcpip_thread with the netif link state already updated: must not block or call netifapi_*.
 */
static void my_eth_link_callback(int link_up)
{
	sys_untimeout(eth_dhcp_check, NULL);

	if (link_up) {
		RTK_LOGI(TAG, "=== Ethernet Link UP ===\n");

		if (!netif_is_up(pnetif_eth)) {
			netif_set_up(pnetif_eth);
		}
#if LWIP_IPV6
		netif_create_ip6_linklocal_address(pnetif_eth, 1);
#endif
		if (dhcp_start(pnetif_eth) == ERR_OK) {
			eth_dhcp_poll_cnt = 0;
			sys_timeout(ETH_DHCP_POLL_MS, eth_dhcp_check, NULL);
		} else {
			RTK_LOGW(TAG, "Ethernet DHCP start failed\n");
		}
	} else {
		RTK_LOGI(TAG, "=== Ethernet Link DOWN ===\n");

		/* Stop the client and clear the address */
		dhcp_release_and_stop(pnetif_eth);
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
