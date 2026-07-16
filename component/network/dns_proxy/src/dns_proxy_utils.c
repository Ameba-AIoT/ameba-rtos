/**
 * @file dns_proxy_utils.c
 * @brief DNS Proxy utility functions
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_UTILS"
#endif

/**
 * @brief Print DNS Proxy status
 */
void dns_proxy_print_status(void)
{
	if (!g_dns_proxy_ctx.initialized) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "DNS Proxy not initialized\n");
		return;
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "DNS Proxy Status:\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Running: %s\n", g_dns_proxy_ctx.running ? "YES" : "NO");

	/* Upstream servers */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Upstream DNS Servers: %d\n", g_dns_proxy_ctx.upstream_count);
	for (int i = 0; i < g_dns_proxy_ctx.upstream_count; i++) {
		const char *marker = (i == g_dns_proxy_ctx.active_upstream_idx) ? " [ACTIVE]" : "";
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Server[%d]: %s%s\n",
				 i, ipaddr_ntoa(&g_dns_proxy_ctx.upstream_servers[i]), marker);
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");

	/* Cache statistics */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Cache: %d/%d entries\n",
			 g_dns_proxy_ctx.cache_count, g_dns_proxy_ctx.cache_max);

	/* Transaction statistics */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Transactions: %d/%d pending\n",
			 g_dns_proxy_ctx.tx_count, g_dns_proxy_ctx.tx_max);

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");
}

/**
 * @brief Dump all valid cache entries (expired entries are skipped)
 */
void dns_proxy_dump_cache(void)
{
	if (!g_dns_proxy_ctx.initialized) {
		return;
	}

	dns_cache_entry_t *entry;
	uint32_t now_ms;
	uint32_t elapsed_sec;
	uint32_t remaining_ttl;
	uint32_t last_access_sec;
	int count = 0;
	int expired_count = 0;
	const char *type_str;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "DNS Cache:\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	now_ms = sys_now();
	entry = g_dns_proxy_ctx.cache_head;

	while (entry) {
		elapsed_sec = (now_ms - entry->timestamp) / 1000;

		if (elapsed_sec >= entry->original_ttl) {
			expired_count++;
			entry = entry->next;
			continue;
		}

		remaining_ttl = entry->original_ttl - elapsed_sec;
		last_access_sec = (now_ms - entry->last_access_time) / 1000;
		type_str = (entry->record_type == 1) ? "A" :
				   (entry->record_type == 28) ? "AAAA" : "?";

		RTK_LOGS(NOTAG, RTK_LOG_INFO, " [%d] %s -> %s (%s, %ds/%ds rem, hit %ds ago)\n",
				 ++count, entry->domain_name, ipaddr_ntoa(&entry->resolved_ip),
				 type_str, (unsigned long)remaining_ttl,
				 (unsigned long)entry->original_ttl, (unsigned long)last_access_sec);

		entry = entry->next;
	}

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "--------------------------------------\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, " Total: %d valid / %d (%d expired)\n",
			 count, count + expired_count, expired_count);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");
}

/**
 * @brief Dump all active (non-timed-out) transactions
 */
void dns_proxy_dump_transactions(void)
{
	if (!g_dns_proxy_ctx.initialized) {
		return;
	}

	dns_transaction_entry_t *entry;
	uint32_t now_ms;
	uint32_t elapsed_sec;
	int count = 0;
	int timedout_count = 0;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Transactions:\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	now_ms = sys_now();
	entry = g_dns_proxy_ctx.tx_head;

	while (entry) {
		elapsed_sec = (now_ms - entry->timestamp) / 1000;

		if (elapsed_sec >= DNS_PROXY_TX_TIMEOUT_SEC) {
			timedout_count++;
			entry = entry->next;
			continue;
		}

		RTK_LOGS(NOTAG, RTK_LOG_INFO, " [%d] 0x%x->0x%x, Client: %s:%d, Domain: %s, Age: %ds\n",
				 ++count, entry->local_txid, entry->upstream_txid,
				 ipaddr_ntoa(&entry->client_ip), entry->client_port,
				 entry->domain_name, (unsigned long)elapsed_sec);

		entry = entry->next;
	}

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "--------------------------------------\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, " Total: %d active / %d (%d timed out)\n",
			 count, count + timedout_count, timedout_count);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "======================================\n");
}
