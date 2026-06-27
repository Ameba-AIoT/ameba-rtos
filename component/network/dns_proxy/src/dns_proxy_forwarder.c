/**
 * @file dns_proxy_forwarder.c
 * @brief Upstream DNS Forwarder implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_FORWARDER"
#endif

/**
 * @brief  Forward DNS request to upstream DNS server
 * @param  p Packet buffer containing DNS query (with modified TxID)
 * @param  upstream_txid Upstream transaction ID
 * @return 0 on success, negative on error
 * @note   Uses single upstream PCB to send to active DNS server
 */
int dns_proxy_forward_request(struct pbuf *p, uint16_t upstream_txid)
{
	ip_addr_t upstream_server;
	err_t err;

	if (!p) {
		return DNS_PROXY_ERR_INVAL;
	}

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	if (g_dns_proxy_ctx.upstream_count == 0) {
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "No upstream DNS servers configured\n");
		return DNS_PROXY_ERR_NO_UPSTREAM;
	}

	if (ip_addr_isany_val(g_dns_proxy_ctx.upstream_servers[g_dns_proxy_ctx.active_upstream_idx])) {
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Active upstream server is invalid\n");
		return DNS_PROXY_ERR_NO_UPSTREAM;
	}

	ip_addr_copy(upstream_server,
				 g_dns_proxy_ctx.upstream_servers[g_dns_proxy_ctx.active_upstream_idx]);

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Forwarding request to upstream %s:%d (txid=0x%x)\n",
				  ipaddr_ntoa(&upstream_server),
				  DNS_PROXY_UPSTREAM_DNS_PORT,
				  upstream_txid);

	/* Send to upstream DNS server via upstream PCB */
	err = udp_sendto(g_dns_proxy_ctx.upstream_pcb, p,
					 &upstream_server, DNS_PROXY_UPSTREAM_DNS_PORT);
	if (err != ERR_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to send to upstream DNS: %d\n", err);
		return DNS_PROXY_ERR_FAIL;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Request forwarded successfully\n");
	return DNS_PROXY_OK;
}

/**
 * @brief  Update upstream DNS servers from LwIP
 * @return 0 on success, negative on error
 * @note   Called when DHCP bound or network status changes
 *         Reads dns_getserver(0), dns_getserver(1), etc.
 */
int dns_proxy_update_upstream_servers(void)
{
	const ip_addr_t *server;
	uint8_t count = 0;
	bool servers_changed = false;

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Updating upstream DNS servers from LwIP\n");

	/* Lock to protect context modification */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Read DNS servers from LwIP */
	for (int i = 0; i < DNS_MAX_SERVERS; i++) {
		server = dns_getserver(i);

		if (server && !ip_addr_isany(server)) {
			/* Check if server changed */
			if (!ip_addr_cmp(server, &g_dns_proxy_ctx.upstream_servers[i])) {
				servers_changed = true;
			}

			ip_addr_copy(g_dns_proxy_ctx.upstream_servers[i], *server);
			count++;

			DNS_PROXY_LOG(DNS_PROXY_INFO, "Upstream DNS server[%d]: %s\n",
						  i, ipaddr_ntoa(server));
		} else {
			/* Clear unused slots */
			if (!ip_addr_isany_val(g_dns_proxy_ctx.upstream_servers[i])) {
				servers_changed = true;
			}
			ip_addr_set_any(false, &g_dns_proxy_ctx.upstream_servers[i]);
		}
	}

	g_dns_proxy_ctx.upstream_count = count;
	g_dns_proxy_ctx.active_upstream_idx = 0;

	/* Unlock before clearing cache/transactions to avoid nested locking */
	rtos_mutex_give(g_dns_proxy_ctx.lock);

	if (count == 0) {
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "No upstream DNS servers available\n");
		return DNS_PROXY_ERR_NO_UPSTREAM;
	}

	/* If servers changed, clear cache and pending transactions */
	if (servers_changed) {
		DNS_PROXY_LOG(DNS_PROXY_INFO, "Upstream servers changed, clearing cache and transactions\n");
		dns_proxy_cache_clear();
		dns_proxy_tx_clear();
	}

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Upstream DNS servers updated: %d servers\n", count);
	return DNS_PROXY_OK;
}

/**
 * @brief  Get current active upstream DNS server
 * @param  server_ip Output: DNS server IP
 * @return 0 if valid server configured, negative if error
 */
int dns_proxy_get_active_upstream_server(ip_addr_t *server_ip)
{
	if (!server_ip) {
		return DNS_PROXY_ERR_INVAL;
	}

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	if (g_dns_proxy_ctx.upstream_count == 0) {
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		return DNS_PROXY_ERR_NO_UPSTREAM;
	}

	if (g_dns_proxy_ctx.active_upstream_idx >= g_dns_proxy_ctx.upstream_count) {
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		return DNS_PROXY_ERR_INVAL;
	}

	ip_addr_copy(*server_ip,
				 g_dns_proxy_ctx.upstream_servers[g_dns_proxy_ctx.active_upstream_idx]);

	rtos_mutex_give(g_dns_proxy_ctx.lock);
	return DNS_PROXY_OK;
}

/**
 * @brief  Switch to next upstream server (for failover)
 * @return 0 on success, negative on error
 * @note  Called when current upstream fails to respond
 */
int dns_proxy_switch_upstream_server(void)
{
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	if (g_dns_proxy_ctx.upstream_count <= 1) {
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "Only one upstream server, cannot switch\n");
		return DNS_PROXY_ERR_INVAL;
	}

	/* Round-robin to next server */
	g_dns_proxy_ctx.active_upstream_idx =
		(g_dns_proxy_ctx.active_upstream_idx + 1) % g_dns_proxy_ctx.upstream_count;

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Switched to upstream server[%d]: %s\n",
				  g_dns_proxy_ctx.active_upstream_idx,
				  ipaddr_ntoa(&g_dns_proxy_ctx.upstream_servers[g_dns_proxy_ctx.active_upstream_idx]));

	rtos_mutex_give(g_dns_proxy_ctx.lock);
	return DNS_PROXY_OK;
}
