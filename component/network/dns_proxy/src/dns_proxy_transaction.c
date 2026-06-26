/**
 * @file dns_proxy_transaction.c
 * @brief DNS Transaction ID Manager implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_TX"
#endif

/**
 * @brief  Check if transaction entry is timed out
 * @param  entry Transaction entry to check
 * @return true if timed out, false if valid
 */
static bool tx_entry_is_timeout(const dns_transaction_entry_t *entry)
{
	uint32_t elapsed_sec;
	uint32_t now_ms;

	now_ms = sys_now();
	elapsed_sec = (now_ms - entry->timestamp) / 1000;

	return (elapsed_sec >= DNS_PROXY_TX_TIMEOUT_SEC);
}

/**
 * @brief  Generate next upstream transaction ID
 * @return Next upstream TxID (auto-increment)
 */
static uint16_t tx_generate_upstream_id(void)
{
	uint16_t txid;

	txid = g_dns_proxy_ctx.next_upstream_txid;
	g_dns_proxy_ctx.next_upstream_txid++;

	/* Wrap around, avoid 0 */
	if (g_dns_proxy_ctx.next_upstream_txid == 0) {
		g_dns_proxy_ctx.next_upstream_txid = 1;
	}

	return txid;
}

/**
 * @brief Clean timed out transaction entries
 * @note  Called periodically or during lookup to keep transaction table clean
 */
static void tx_clean_timeout(void)
{
	dns_transaction_entry_t *entry;
	dns_transaction_entry_t *prev;
	dns_transaction_entry_t *next;

	entry = g_dns_proxy_ctx.tx_head;
	prev = NULL;

	while (entry) {
		next = entry->next;

		if (tx_entry_is_timeout(entry)) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx timeout: local_txid=0x%x, domain=%s\n",
						  entry->local_txid, entry->domain_name);

			/* Remove from linked list */
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.tx_head = next;
			}
			g_dns_proxy_ctx.tx_count--;
			rtos_mem_free(entry);
		} else {
			prev = entry;
		}

		entry = next;
	}
}

/**
 * @brief  Create new transaction mapping entry
 * @param  local_txid Local transaction ID
 * @param  client_ip Client IP address
 * @param  client_port Client port
 * @param  domain_name Queried domain name
 * @param  qtype DNS query type (A=1, AAAA=28, etc.)
 * @param  out_upstream_txid Output: Generated upstream TxID
 * @return 0 on success, negative on error
 * @note   Generates unique upstream TxID and records mapping
 */
int dns_proxy_tx_create(uint16_t local_txid, const ip_addr_t *client_ip,
						uint16_t client_port, const char *domain_name,
						uint16_t qtype, uint16_t *out_upstream_txid)
{
	dns_transaction_entry_t *new_entry;
	uint16_t upstream_txid;

	if (!client_ip || !domain_name || !out_upstream_txid) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Lock to protect transaction table */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Check transaction capacity */
	if (g_dns_proxy_ctx.tx_count >= g_dns_proxy_ctx.tx_max) {
		/* Clean timed out entries first */
		tx_clean_timeout();

		/* Still full? Reject new transaction */
		if (g_dns_proxy_ctx.tx_count >= g_dns_proxy_ctx.tx_max) {
			DNS_PROXY_LOG(DNS_PROXY_WARNING, "Transaction table full (%d/%d)\n",
						  g_dns_proxy_ctx.tx_count, g_dns_proxy_ctx.tx_max);
			rtos_mutex_give(g_dns_proxy_ctx.lock);
			return DNS_PROXY_ERR_NOMEM;
		}
	}

	/* Generate upstream TxID */
	upstream_txid = tx_generate_upstream_id();

	/* Allocate new transaction entry */
	new_entry = (dns_transaction_entry_t *)rtos_mem_malloc(sizeof(dns_transaction_entry_t));
	if (!new_entry) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to allocate transaction entry\n");
		rtos_mutex_give(g_dns_proxy_ctx.lock);
		return DNS_PROXY_ERR_NOMEM;
	}

	/* Initialize new entry */
	new_entry->local_txid = local_txid;
	new_entry->upstream_txid = upstream_txid;
	ip_addr_copy(new_entry->client_ip, *client_ip);
	new_entry->client_port = client_port;
	new_entry->qtype = qtype;
	new_entry->timestamp = sys_now();
	strncpy(new_entry->domain_name, domain_name, DNS_MAX_DOMAIN_LEN - 1);
	new_entry->domain_name[DNS_MAX_DOMAIN_LEN - 1] = '\0';
	new_entry->next = g_dns_proxy_ctx.tx_head;

	/* Insert at head of linked list */
	g_dns_proxy_ctx.tx_head = new_entry;
	g_dns_proxy_ctx.tx_count++;

	*out_upstream_txid = upstream_txid;

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx created: local=0x%x -> upstream=0x%x, qtype=%d, client=%s:%d, domain=%s\n",
				  local_txid, upstream_txid, qtype,
				  ipaddr_ntoa(client_ip), client_port, domain_name);

	rtos_mutex_give(g_dns_proxy_ctx.lock);
	return DNS_PROXY_OK;
}

/**
 * @brief  Lookup transaction by upstream TxID
 * @param  upstream_txid Upstream transaction ID to search
 * @param  out_local_txid Output: Local TxID if found
 * @param  out_client_ip Output: Client IP if found
 * @param  out_client_port Output: Client port if found
 * @param  out_domain_name Output: Domain name if found (can be NULL)
 * @param  domain_name_len Size of domain_name buffer
 * @param  out_qtype Output: DNS query type if found (can be NULL)
 * @return 0 if found, negative if not found
 * @note   Removes the transaction entry after lookup (one-time use)
 */
int dns_proxy_tx_lookup_by_upstream(uint16_t upstream_txid, uint16_t *out_local_txid,
									ip_addr_t *out_client_ip, uint16_t *out_client_port,
									char *out_domain_name, uint16_t domain_name_len,
									uint16_t *out_qtype)
{
	dns_transaction_entry_t *entry;
	dns_transaction_entry_t *prev = NULL;
	int ret = DNS_PROXY_ERR_TX_NOTFOUND;

	if (!out_local_txid || !out_client_ip || !out_client_port) {
		return DNS_PROXY_ERR_INVAL;
	}

	/* Lock to protect transaction table */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	/* Search in transaction linked list, cleaning timed-out entries along the way */
	entry = g_dns_proxy_ctx.tx_head;
	while (entry) {
		dns_transaction_entry_t *next = entry->next;

		/* Remove timed-out entries during traversal */
		if (tx_entry_is_timeout(entry)) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx timeout (cleanup): local=0x%x, domain=%s\n",
						  entry->local_txid, entry->domain_name);
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.tx_head = next;
			}
			g_dns_proxy_ctx.tx_count--;

			rtos_mem_free(entry);
			entry = next;
			continue;
		}

		if (entry->upstream_txid == upstream_txid) {
			/* Found valid matching entry */

			/* Copy results */
			*out_local_txid = entry->local_txid;
			ip_addr_copy(*out_client_ip, entry->client_ip);
			*out_client_port = entry->client_port;

			/* Copy domain name if buffer provided */
			if (out_domain_name && domain_name_len > 0) {
				strncpy(out_domain_name, entry->domain_name, domain_name_len - 1);
				out_domain_name[domain_name_len - 1] = '\0';
			}

			/* Copy qtype if requested */
			if (out_qtype) {
				*out_qtype = entry->qtype;
			}

			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx found: upstream=0x%x -> local=0x%x, client=%s:%d, domain=%s\n",
						  upstream_txid, entry->local_txid,
						  ipaddr_ntoa(&entry->client_ip), entry->client_port,
						  entry->domain_name);

			/* Remove from linked list (one-time use) */
			if (prev) {
				prev->next = next;
			} else {
				g_dns_proxy_ctx.tx_head = next;
			}
			g_dns_proxy_ctx.tx_count--;
			rtos_mem_free(entry);

			ret = DNS_PROXY_OK;
			break;
		}

		prev = entry;
		entry = next;
	}

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	if (ret != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx not found: upstream=0x%x\n", upstream_txid);
	}

	return ret;
}

/**
 * @brief  Remove a specific transaction entry by upstream TxID
 * @param  upstream_txid Upstream TxID of the entry to remove
 * @return 0 on success, DNS_PROXY_ERR_TX_NOTFOUND if not found
 * @note   Used to roll back a single entry after tx_create succeeds but a later step fails
 */
int dns_proxy_tx_remove(uint16_t upstream_txid)
{
	dns_transaction_entry_t *entry;
	dns_transaction_entry_t *prev = NULL;
	int ret = DNS_PROXY_ERR_TX_NOTFOUND;

	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	entry = g_dns_proxy_ctx.tx_head;
	while (entry) {
		if (entry->upstream_txid == upstream_txid) {
			if (prev) {
				prev->next = entry->next;
			} else {
				g_dns_proxy_ctx.tx_head = entry->next;
			}
			g_dns_proxy_ctx.tx_count--;
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Tx removed: upstream=0x%x, domain=%s\n",
						  upstream_txid, entry->domain_name);
			rtos_mem_free(entry);
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
 * @brief Clear all transaction entries
 */
void dns_proxy_tx_clear(void)
{
	dns_transaction_entry_t *entry;
	dns_transaction_entry_t *next;

	/* Lock to protect transaction table */
	rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);

	entry = g_dns_proxy_ctx.tx_head;
	while (entry) {
		next = entry->next;
		rtos_mem_free(entry);
		entry = next;
	}

	g_dns_proxy_ctx.tx_head = NULL;
	g_dns_proxy_ctx.tx_count = 0;

	rtos_mutex_give(g_dns_proxy_ctx.lock);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Transaction table cleared\n");
}
