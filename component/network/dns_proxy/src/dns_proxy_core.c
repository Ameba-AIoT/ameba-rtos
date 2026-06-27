/**
 * @file dns_proxy_core.c
 * @brief DNS Proxy Core implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_PROXY"
#endif

/* ======================================================================== */
/*                          Global Context                                  */
/* ======================================================================== */

/**
 * @brief Global DNS Proxy context
 */
dns_proxy_ctx_t g_dns_proxy_ctx;

/* ======================================================================== */
/*                     Internal Helper Functions                            */
/* ======================================================================== */

/**
 * @brief Initialize DNS Proxy context
 * @param cache_max Maximum cache entries
 * @param tx_max Maximum pending transactions
 */
static int dns_proxy_ctx_init(uint16_t cache_max, uint16_t tx_max)
{
	memset(&g_dns_proxy_ctx, 0, sizeof(dns_proxy_ctx_t));

	g_dns_proxy_ctx.cache_max = cache_max;
	g_dns_proxy_ctx.tx_max = tx_max;
	g_dns_proxy_ctx.next_upstream_txid = 1;
	g_dns_proxy_ctx.active_upstream_idx = 0;

	/* Initialize mutex */
	if (rtos_mutex_create(&g_dns_proxy_ctx.lock) != RTK_SUCCESS) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to create mutex\n");
		return DNS_PROXY_ERR_INIT;
	}

	g_dns_proxy_ctx.initialized = true;
	g_dns_proxy_ctx.running = false;

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Context initialized: cache_max=%d, tx_max=%d\n",
				  cache_max, tx_max);
	return DNS_PROXY_OK;
}

/**
 * @brief Cleanup DNS Proxy context
 */
static void dns_proxy_ctx_cleanup(void)
{
	/* Clear cache and transactions */
	dns_proxy_cache_clear();
	dns_proxy_tx_clear();

	/* Destroy mutex */
	rtos_mutex_delete(&g_dns_proxy_ctx.lock);

	/* Reset context */
	memset(&g_dns_proxy_ctx, 0, sizeof(dns_proxy_ctx_t));

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Context cleaned up\n");
}

/* ======================================================================== */
/*                      Public API Implementation                           */
/* ======================================================================== */

/**
 * @brief  Initialize DNS Proxy module
 * @param  cache_max_entries Maximum number of cache entries (0 = default 128)
 * @param  tx_max_pending Maximum pending transactions (0 = default 64)
 * @return 0 on success, negative on error
 */
int dns_proxy_init(uint16_t cache_max_entries, uint16_t tx_max_pending)
{
	if (g_dns_proxy_ctx.initialized) {
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "DNS Proxy already initialized\n");
		return DNS_PROXY_OK;
	}

	/* Use defaults if zero */
	if (cache_max_entries == 0) {
		cache_max_entries = DNS_PROXY_CACHE_MAX_ENTRIES;
	}
	if (tx_max_pending == 0) {
		tx_max_pending = DNS_PROXY_TX_MAX_PENDING;
	}

	/* Initialize context */
	if (dns_proxy_ctx_init(cache_max_entries, tx_max_pending) != DNS_PROXY_OK) {
		return DNS_PROXY_ERR_INIT;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS Proxy initialized successfully\n");
	return DNS_PROXY_OK;
}

/**
 * @brief  Start DNS Proxy service
 * @param  bind_port Port to listen on (default 53)
 * @return 0 on success, negative on error
 * @note   Creates downstream/upstream UDP PCBs
 *         Binds downstream to IP_ANY:bind_port
 *         Registers recv callbacks with LwIP
 *         Reads upstream DNS servers from LwIP dns_getserver()
 */
int dns_proxy_start(uint16_t bind_port)
{
	int ret;

	if (!g_dns_proxy_ctx.initialized) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "DNS Proxy not initialized\n");
		return DNS_PROXY_ERR_INIT;
	}

	if (g_dns_proxy_ctx.running) {
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "DNS Proxy already running\n");
		return DNS_PROXY_OK;
	}

	/* Use default port if zero */
	if (bind_port == 0) {
		bind_port = DNS_PROXY_LISTEN_PORT;
	}

	/* Update upstream DNS servers from LwIP */
	ret = dns_proxy_update_upstream_servers();
	if (ret != DNS_PROXY_OK) {
		/* Servers will be configured later via DHCP */
	}

	/* Initialize UDP PCBs */
	ret = dns_proxy_listener_init(bind_port);
	if (ret != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to initialize UDP PCBs: %d\n", ret);
		return ret;
	}

	g_dns_proxy_ctx.running = true;

	DNS_PROXY_LOG(DNS_PROXY_INFO, "DNS Proxy started successfully\n");
	return DNS_PROXY_OK;
}

/**
 * @brief  Stop DNS Proxy service
 * @return 0 on success
 * @note   Removes UDP PCBs, clears all caches and transactions
 */
int dns_proxy_stop(void)
{
	if (!g_dns_proxy_ctx.running) {
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "DNS Proxy not running\n");
		return DNS_PROXY_OK;
	}

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Stopping DNS Proxy...\n");

	/* Cleanup UDP PCBs */
	dns_proxy_listener_cleanup();

	/* Clear cache and transactions */
	dns_proxy_cache_clear();
	dns_proxy_tx_clear();

	g_dns_proxy_ctx.running = false;

	DNS_PROXY_LOG(DNS_PROXY_INFO, "DNS Proxy stopped\n");
	return DNS_PROXY_OK;
}

/**
 * @brief Deinitialize DNS Proxy module
 * @note  Frees all allocated memory
 */
void dns_proxy_deinit(void)
{
	if (!g_dns_proxy_ctx.initialized) {
		DNS_PROXY_LOG(DNS_PROXY_WARNING, "DNS Proxy not initialized\n");
		return;
	}

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Deinitializing DNS Proxy...\n");

	/* Stop if running */
	if (g_dns_proxy_ctx.running) {
		dns_proxy_stop();
	}

	/* Cleanup context */
	dns_proxy_ctx_cleanup();

	DNS_PROXY_LOG(DNS_PROXY_INFO, "DNS Proxy deinitialized\n");
}
