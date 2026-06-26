/**
 * @file dns_proxy_listener.c
 * @brief UDP Listener implementation
 */

#include "dns_proxy.h"

#ifndef TAG
#define TAG "DNS_LISTENER"
#endif

/**
 * @brief  Linearize pbuf chain into a single contiguous buffer.
 * @param  p Source pbuf (freed internally if linearization is needed).
 * @return Linearized pbuf, or NULL if p is NULL or allocation failed (p already freed).
 */
static struct pbuf *linearize_pbuf(struct pbuf *p)
{
	struct pbuf *q;

	if (!p) {
		return NULL;
	}
	if (p->len == p->tot_len) {
		return p;
	}
	q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
	if (!q) {
		pbuf_free(p);
		return NULL;
	}
	pbuf_copy(q, p);
	pbuf_free(p);
	return q;
}

/**
 * @brief Downstream UDP receive callback (from LAN clients)
 * @param arg User argument (unused)
 * @param pcb UDP PCB
 * @param p Received packet buffer
 * @param addr Source IP address
 * @param port Source port
 * @note  This callback handles DNS queries from LAN devices
 */
static void dns_proxy_downstream_recv_callback(void *arg, struct udp_pcb *pcb,
		struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	struct pbuf *q;
	struct pbuf *resp_pbuf;
	uint8_t *resp_buf;
	uint16_t resp_len;
	uint16_t local_txid;
	uint16_t qtype;
	uint16_t upstream_txid;
	char domain[DNS_MAX_DOMAIN_LEN];
	ip_addr_t cached_ip;
	uint32_t remaining_ttl;
	err_t err;

	(void)arg;

	q = linearize_pbuf(p);
	if (!q) {
		return;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Downstream recv: %d bytes from %s:%d\n",
				  q->tot_len, ipaddr_ntoa(addr), port);

	/* Extract question from DNS query */
	if (dns_parser_extract_question((uint8_t *)q->payload, q->len,
									&local_txid, domain, &qtype) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to parse DNS query\n");
		pbuf_free(q);
		return;
	}

	/* A record cache hit: construct and send response directly */
	if (qtype == 1 && dns_proxy_cache_lookup(domain, &cached_ip, &remaining_ttl) == DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache hit for %s\n", domain);

		/* Construct DNS response from cache */
		if (dns_parser_construct_response((uint8_t *)q->payload, q->len,
										  &cached_ip, remaining_ttl,
										  &resp_buf, &resp_len) != DNS_PROXY_OK) {
			DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to construct cached response for %s\n", domain);
			pbuf_free(q);
			return;
		}

		/* Allocate pbuf for response */
		resp_pbuf = pbuf_alloc(PBUF_TRANSPORT, resp_len, PBUF_RAM);
		if (!resp_pbuf) {
			DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to allocate pbuf for cached response\n");
			rtos_mem_free(resp_buf);
			pbuf_free(q);
			return;
		}

		memcpy(resp_pbuf->payload, resp_buf, resp_len);
		rtos_mem_free(resp_buf);

		/* Send response to client */
		err = udp_sendto(pcb, resp_pbuf, addr, port);
		if (err != ERR_OK) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to send cached response: %d\n", err);
		} else {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cached response sent to %s:%d\n", ipaddr_ntoa(addr), port);
		}

		pbuf_free(resp_pbuf);
		pbuf_free(q);
		return;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache miss (qtype=%d): %s, forwarding to upstream\n", qtype, domain);

	/* Create transaction mapping */
	if (dns_proxy_tx_create(local_txid, addr, port, domain, qtype,
							&upstream_txid) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to create transaction\n");
		pbuf_free(q);
		return;
	}

	/* Modify TxID in DNS message */
	dns_parser_modify_txid((uint8_t *)q->payload, q->len, upstream_txid);

	/* Forward request to upstream DNS */
	if (dns_proxy_forward_request(q, upstream_txid) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to forward request\n");
		dns_proxy_tx_remove(upstream_txid);
		pbuf_free(q);
		return;
	}

	pbuf_free(q);
}

/**
 * @brief Upstream UDP receive callback (from upstream DNS servers)
 * @param arg User argument (unused)
 * @param pcb UDP PCB
 * @param p Received packet buffer
 * @param addr Source IP address
 * @param port Source port
 * @note  This callback handles DNS responses from upstream DNS servers
 */
static void dns_proxy_upstream_recv_callback(void *arg, struct udp_pcb *pcb,
		struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	struct pbuf *q;
	uint16_t upstream_txid;
	uint16_t local_txid;
	uint16_t client_port;
	uint16_t qtype;
	char domain[DNS_MAX_DOMAIN_LEN];
	ip_addr_t client_ip;
	ip_addr_t resolved_ip;
	uint32_t ttl;
	err_t err;
	int extract_ret;
	bool from_upstream = false;

	(void)arg;
	(void)pcb;

	q = linearize_pbuf(p);
	if (!q) {
		return;
	}

	/* Validate source: only accept responses from configured upstream servers on port 53 */
	if (port == DNS_PROXY_UPSTREAM_DNS_PORT) {
		rtos_mutex_take(g_dns_proxy_ctx.lock, MUTEX_WAIT_TIMEOUT);
		for (int i = 0; i < g_dns_proxy_ctx.upstream_count; i++) {
			if (ip_addr_cmp(addr, &g_dns_proxy_ctx.upstream_servers[i])) {
				from_upstream = true;
				break;
			}
		}
		rtos_mutex_give(g_dns_proxy_ctx.lock);
	}
	if (!from_upstream) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Upstream response from unexpected source %s:%d, dropping\n",
					  ipaddr_ntoa(addr), port);
		pbuf_free(q);
		return;
	}

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Upstream recv: %d bytes from %s:%d\n",
				  q->tot_len, ipaddr_ntoa(addr), port);

	/* Extract upstream TxID from response */
	if (dns_parser_get_txid((uint8_t *)q->payload, q->len, &upstream_txid) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS response too short\n");
		pbuf_free(q);
		return;
	}

	/* Lookup transaction by upstream TxID and get domain name */
	if (dns_proxy_tx_lookup_by_upstream(upstream_txid, &local_txid,
										&client_ip, &client_port,
										domain, sizeof(domain), &qtype) != DNS_PROXY_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "No matching transaction for upstream_txid=0x%x\n", upstream_txid);
		pbuf_free(q);
		return;
	}

	/* For A record queries: extract IP and update cache */
	if (qtype == 1) {
		extract_ret = dns_parser_extract_a_record((uint8_t *)q->payload, q->len, &resolved_ip, &ttl);
		if (extract_ret == DNS_PROXY_ERR_DNS_RCODE) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "DNS error response for %s, forwarding to client\n", domain);
		} else if (extract_ret != DNS_PROXY_OK) {
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Malformed A record response for %s, dropping\n", domain);
			pbuf_free(q);
			return;
		} else {
			dns_proxy_cache_insert(domain, &resolved_ip, ttl);
			DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Cache updated: %s -> %s (TTL=%d)\n",
						  domain, ipaddr_ntoa(&resolved_ip), (unsigned long)ttl);
		}
	} else {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Non-A response (qtype=%d) for %s, forwarding\n", qtype, domain);
	}

	/* Restore original TxID */
	dns_parser_modify_txid((uint8_t *)q->payload, q->len, local_txid);

	/* Send response back to LAN client via downstream PCB */
	err = udp_sendto(g_dns_proxy_ctx.downstream_pcb, q, &client_ip, client_port);
	if (err != ERR_OK) {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Failed to send response to client %s:%d: %d\n",
					  ipaddr_ntoa(&client_ip), client_port, err);
	} else {
		DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Response sent to %s:%d for %s\n",
					  ipaddr_ntoa(&client_ip), client_port, domain);
	}

	pbuf_free(q);
}

/**
 * @brief  Initialize UDP PCBs
 * @param  listen_port Port to listen on for downstream requests
 * @return 0 on success, negative on error
 */
int dns_proxy_listener_init(uint16_t listen_port)
{
	err_t err;

	/* Downstream PCB (listen on LAN side) */
	g_dns_proxy_ctx.downstream_pcb = udp_new();
	if (!g_dns_proxy_ctx.downstream_pcb) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to create downstream PCB\n");
		return DNS_PROXY_ERR_INIT;
	}

	err = udp_bind(g_dns_proxy_ctx.downstream_pcb, IP_ANY_TYPE, listen_port);
	if (err != ERR_OK) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to bind downstream PCB: %d\n", err);
		udp_remove(g_dns_proxy_ctx.downstream_pcb);
		g_dns_proxy_ctx.downstream_pcb = NULL;
		return DNS_PROXY_ERR_BIND;
	}

	udp_recv(g_dns_proxy_ctx.downstream_pcb, dns_proxy_downstream_recv_callback, NULL);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Downstream PCB bound to port %d\n", listen_port);

	/* Upstream PCB (listen on WAN side) */
	g_dns_proxy_ctx.upstream_pcb = udp_new();
	if (!g_dns_proxy_ctx.upstream_pcb) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to create upstream PCB\n");
		udp_remove(g_dns_proxy_ctx.downstream_pcb);
		g_dns_proxy_ctx.downstream_pcb = NULL;
		return DNS_PROXY_ERR_INIT;
	}

	err = udp_bind(g_dns_proxy_ctx.upstream_pcb, IP_ANY_TYPE, DNS_PROXY_UPSTREAM_PORT);
	if (err != ERR_OK) {
		DNS_PROXY_LOG(DNS_PROXY_ERROR, "Failed to bind upstream PCB to port %d, err %d\n",
					  DNS_PROXY_UPSTREAM_PORT, err);
		udp_remove(g_dns_proxy_ctx.upstream_pcb);
		udp_remove(g_dns_proxy_ctx.downstream_pcb);
		g_dns_proxy_ctx.upstream_pcb = NULL;
		g_dns_proxy_ctx.downstream_pcb = NULL;
		return DNS_PROXY_ERR_BIND;
	}

	/* Register callback to receive responses from upstream DNS */
	udp_recv(g_dns_proxy_ctx.upstream_pcb, dns_proxy_upstream_recv_callback, NULL);

	DNS_PROXY_LOG(DNS_PROXY_DEBUG, "Upstream PCB bound to port %d\n", DNS_PROXY_UPSTREAM_PORT);

	return DNS_PROXY_OK;
}

/**
 * @brief Cleanup UDP PCBs
 */
void dns_proxy_listener_cleanup(void)
{
	if (g_dns_proxy_ctx.downstream_pcb) {
		udp_recv(g_dns_proxy_ctx.downstream_pcb, NULL, NULL);
		udp_remove(g_dns_proxy_ctx.downstream_pcb);
		g_dns_proxy_ctx.downstream_pcb = NULL;
		DNS_PROXY_LOG(DNS_PROXY_INFO, "Downstream PCB removed\n");
	}

	if (g_dns_proxy_ctx.upstream_pcb) {
		udp_recv(g_dns_proxy_ctx.upstream_pcb, NULL, NULL);
		udp_remove(g_dns_proxy_ctx.upstream_pcb);
		g_dns_proxy_ctx.upstream_pcb = NULL;
		DNS_PROXY_LOG(DNS_PROXY_INFO, "Upstream PCB removed\n");
	}
}
