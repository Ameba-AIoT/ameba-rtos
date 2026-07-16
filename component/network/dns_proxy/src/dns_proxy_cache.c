/**
 * @file dns_proxy_cache.c
 * @brief DNS Cache Manager implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_CACHE"
#endif

/**
 * @brief Convert domain name to lowercase
 * @param dst Destination buffer
 * @param src Source domain name
 * @param max_len Maximum length
 */
static void domain_to_lowercase(char *dst, const char *src, size_t max_len)
{
	size_t i;

	for (i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
		dst[i] = lwip_tolower((unsigned char)src[i]);
	}
	dst[i] = '\0';
}

/**
 * @brief  Check if cache entry is expired
 * @param  entry Cache entry to check
 * @return true if expired, false if valid
 */
static bool cache_entry_is_expired(const dns_cache_entry_t *entry)
{
	uint32_t elapsed_sec;
	uint32_t now_ms;

	now_ms = sys_now();
	elapsed_sec = (now_ms - entry->timestamp) / 1000;

	return (elapsed_sec >= entry->original_ttl);
}

/**
 * @brief  Lookup domain in cache
 * @param  domain Domain name to search
 * @param  out_ip Output: Resolved IP if found
 * @param  out_remaining_ttl Output: Remaining TTL in seconds
 * @return 0 if found and valid, negative if not found or expired
 * @note   Checks TTL validity: (sys_now() - timestamp) / 1000 < original_ttl
 */
int dns_proxy_cache_lookup(const char *domain, ip_addr_t *out_ip, uint32_t *out_remaining_ttl)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *prev = NULL;
	char domain_lower[DNS_MAX_DOMAIN_LEN];
	uint32_t now_ms;
	uint32_t elapsed_sec;
	uint32_t remaining_ttl;
	int ret = DNS_PROXY_ERR_NOTFOUND;

	if (!domain || !out_ip) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Convert domain to lowercase for case-insensitive comparison */
	domain_to_lowercase(domain_lower, domain, DNS_MAX_DOMAIN_LEN);

	/* Lock to protect cache access */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Search in cache linked list, cleaning expired entries along the way */
	entry = g_dns_proxy_ctx.cache_head;
	while (entry) {
		dns_cache_entry_t *next = entry->next;

		/* Remove expired entries during traversal */
		if (cache_entry_is_expired(entry)) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache expired (cleanup): %s\n", entry->domain_name);
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.cache_head = next;
			}
			g_dns_proxy_ctx.cache_count--;

			rtos_mem_free(entry);
			entry = next;
			continue;
		}

		if (strcmp(entry->domain_name, domain_lower) == 0) {
			/* Entry is valid, update last access time for LRU */
			now_ms = sys_now();
			entry->last_access_time = now_ms;

			/* Calculate remaining TTL */
			elapsed_sec = (now_ms - entry->timestamp) / 1000;
			remaining_ttl = entry->original_ttl - elapsed_sec;

			/* Copy results */
			ip_addr_copy(*out_ip, entry->resolved_ip);
			if (out_remaining_ttl) {
				*out_remaining_ttl = remaining_ttl;
			}

			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache hit: %s -> %s (TTL: %d sec)\n",
						  domain_lower, ipaddr_ntoa(out_ip),
						  (unsigned long)remaining_ttl);

			ret = DNS_PROXY_OK;
			break;
		}

		prev = entry;
		entry = next;
	}

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	if (ret != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache miss: %s\n", domain_lower);
	}

	return ret;
}

/**
 * @brief Clean expired cache entries
 * @note  Called during insert to keep cache clean
 */
static void cache_clean_expired(void)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *prev;
	dns_cache_entry_t *next;

	entry = g_dns_proxy_ctx.cache_head;
	prev = NULL;

	while (entry) {
		next = entry->next;

		if (cache_entry_is_expired(entry)) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Clean expired: %s\n", entry->domain_name);

			/* Remove from linked list */
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.cache_head = next;
			}
			g_dns_proxy_ctx.cache_count--;
			rtos_mem_free(entry);
		} else {
			prev = entry;
		}

		entry = next;
	}
}

/**
 * @brief  Evict LRU cache entry when cache is full
 * @return 0 on success, negative on failure
 */
static int cache_evict_lru(void)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *lru_entry = NULL;
	dns_cache_entry_t *lru_prev = NULL;
	dns_cache_entry_t *prev = NULL;
	uint32_t oldest_time = UINT32_MAX;

	/* Find the least recently used entry */
	entry = g_dns_proxy_ctx.cache_head;
	while (entry) {
		if (entry->last_access_time < oldest_time) {
			oldest_time = entry->last_access_time;
			lru_entry = entry;
			lru_prev = prev;
		}

		prev = entry;
		entry = entry->next;
	}

	if (!lru_entry) {
		return DNS_PROXY_ERR_FAIL;
	}

	DNS_PROXY_LOG(DNS_PROXY_INFO, "Evict LRU: %s\n", lru_entry->domain_name);

	/* Remove LRU entry from linked list */
	if (lru_prev) {
		lru_prev->next = lru_entry->next;
	} else {
		g_dns_proxy_ctx.cache_head = lru_entry->next;
	}
	g_dns_proxy_ctx.cache_count--;
	rtos_mem_free(lru_entry);

	return DNS_PROXY_OK;
}

/**
 * @brief  Insert or update cache entry
 * @param  domain Domain name
 * @param  ip Resolved IP
 * @param  ttl Original TTL in seconds (from DNS response)
 * @return 0 on success, negative on failure
 * @note   If domain exists, updates IP and TTL
 *         If cache is full, cleans expired entries first, then evicts LRU
 */
int dns_proxy_cache_insert(const char *domain, const ip_addr_t *ip, uint32_t ttl)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *new_entry;
	dns_cache_entry_t *prev = NULL;
	char domain_lower[DNS_MAX_DOMAIN_LEN];
	uint32_t now_ms;

	if (!domain || !ip) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* TTL=0 means do not cache */
	if (ttl == 0) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache insert skipped: %s TTL=0\n", domain);
		return DNS_PROXY_OK;
	}

	/* Convert domain to lowercase */
	domain_to_lowercase(domain_lower, domain, DNS_MAX_DOMAIN_LEN);

	/* Lock to protect cache access */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Check if domain already exists, cleaning expired entries along the way */
	entry = g_dns_proxy_ctx.cache_head;
	while (entry) {
		dns_cache_entry_t *next = entry->next;

		/* Remove expired entries during traversal */
		if (cache_entry_is_expired(entry)) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache expired (cleanup): %s\n", entry->domain_name);
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.cache_head = next;
			}
			g_dns_proxy_ctx.cache_count--;
			rtos_mem_free(entry);
			entry = next;
			continue;
		}

		if (strcmp(entry->domain_name, domain_lower) == 0) {
			/* Update existing entry */
			ip_addr_copy(entry->resolved_ip, *ip);
			entry->original_ttl = ttl;
			now_ms = sys_now();
			entry->timestamp = now_ms;
			entry->last_access_time = now_ms;

			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache updated: %s -> %s (TTL: %d sec)\n",
						  domain_lower, ipaddr_ntoa(ip), (unsigned long)ttl);

			rtos_mutex_give(g_dns_proxy_ctx.lock);
			return DNS_PROXY_OK;
		}

		prev = entry;
		entry = next;
	}

	/* New entry needed, check cache capacity */
	if (g_dns_proxy_ctx.cache_count >= g_dns_proxy_ctx.cache_max) {
		/* First try to clean expired entries */
		cache_clean_expired();

		/* Still full, evict LRU */
		if (g_dns_proxy_ctx.cache_count >= g_dns_proxy_ctx.cache_max) {
			if (cache_evict_lru() != DNS_PROXY_OK) {
				DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to evict LRU entry\n");
				rtos_mutex_give(g_dns_proxy_ctx.lock);
				return DNS_PROXY_ERR_NOMEM;
			}
		}
	}

	/* Allocate new cache entry */
	new_entry = (dns_cache_entry_t *)rtos_mem_malloc(sizeof(dns_cache_entry_t));
	if (!new_entry) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to allocate cache entry\n");
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		return DNS_PROXY_ERR_NOMEM;
	}

	/* Initialize new entry */
	strncpy(new_entry->domain_name, domain_lower, DNS_MAX_DOMAIN_LEN - 1);
	new_entry->domain_name[DNS_MAX_DOMAIN_LEN - 1] = '\0';
	ip_addr_copy(new_entry->resolved_ip, *ip);
	new_entry->original_ttl = ttl;
	now_ms = sys_now();
	new_entry->timestamp = now_ms;
	new_entry->last_access_time = now_ms;
	new_entry->record_type = 1; /* A record */
	new_entry->next = g_dns_proxy_ctx.cache_head;

	/* Insert at head of linked list */
	g_dns_proxy_ctx.cache_head = new_entry;
	g_dns_proxy_ctx.cache_count++;

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache inserted: %s -> %s (TTL: %d sec)\n",
				  domain_lower, ipaddr_ntoa(ip), (unsigned long)ttl);

	rtos_mutex_give(g_dns_proxy_ctx.lock);
	return DNS_PROXY_OK;
}

/**
 * @brief  Remove a specific cache entry
 * @param  domain Domain name to remove
 * @return 0 if found and removed, negative if not found
 */
int dns_proxy_cache_remove(const char *domain)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *prev = NULL;
	char domain_lower[DNS_MAX_DOMAIN_LEN];
	int ret = DNS_PROXY_ERR_NOTFOUND;

	if (!domain) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Convert domain to lowercase */
	domain_to_lowercase(domain_lower, domain, DNS_MAX_DOMAIN_LEN);

	/* Lock to protect cache access */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Search and remove */
	entry = g_dns_proxy_ctx.cache_head;
	while (entry) {
		if (strcmp(entry->domain_name, domain_lower) == 0) {
			/* Found, remove from linked list */
			if (prev) {
				prev->next = entry->next;
			} else {
				g_dns_proxy_ctx.cache_head = entry->next;
			}
			g_dns_proxy_ctx.cache_count--;
			rtos_mem_free(entry);

			DNS_PROXY_LOG(DNS_PROXY_INFO, "Cache removed: %s\n", domain_lower);
			ret = DNS_PROXY_OK;
			break;
		}

		prev = entry;
		entry = entry->next;
	}

	rtos_mutex_give(g_dns_proxy_ctx.lock);
	return ret;
}

/**
 * @brief Clear all cache entries
 */
void dns_proxy_cache_clear(void)
{
	dns_cache_entry_t *entry;
	dns_cache_entry_t *next;

	/* Lock to protect cache access */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	entry = g_dns_proxy_ctx.cache_head;
	while (entry) {
		next = entry->next;
		rtos_mem_free(entry);
		entry = next;
	}

	g_dns_proxy_ctx.cache_head = NULL;
	g_dns_proxy_ctx.cache_count = 0;

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache cleared\n");
}
