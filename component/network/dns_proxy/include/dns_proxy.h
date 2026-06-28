#ifndef DNS_PROXY_H
#define DNS_PROXY_H

/**
 * @file dns_proxy.h
 * @brief DNS Proxy public API header
 */

#include "dns_proxy_config.h"
#include "dns_proxy_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/*                      Global Context Declaration                          */
/* ======================================================================== */

extern dns_proxy_ctx_t g_dns_proxy_ctx;

/* ======================================================================== */
/*                    Initialization and Startup APIs                       */
/* ======================================================================== */

int dns_proxy_init(uint16_t cache_max_entries, uint16_t tx_max_pending);
int dns_proxy_start(uint16_t bind_port);
int dns_proxy_stop(void);
void dns_proxy_deinit(void);

/* ======================================================================== */
/*                  Upstream DNS Server Management APIs                     */
/* ======================================================================== */

int dns_proxy_update_upstream_servers(void);
int dns_proxy_get_active_upstream_server(ip_addr_t *server_ip);
int dns_proxy_switch_upstream_server(void);

/* ======================================================================== */
/*                         Cache Management APIs                            */
/* ======================================================================== */

int dns_proxy_cache_lookup(const char *domain, ip_addr_t *out_ip, uint32_t *out_remaining_ttl);
int dns_proxy_cache_insert(const char *domain, const ip_addr_t *ip, uint32_t ttl);
int dns_proxy_cache_remove(const char *domain);
void dns_proxy_cache_clear(void);

/* ======================================================================== */
/*                    Listener Management APIs (Internal)                   */
/* ======================================================================== */

int dns_proxy_listener_init(uint16_t listen_port);
void dns_proxy_listener_cleanup(void);

/* ======================================================================== */
/*                        DNS Parser APIs (Internal)                        */
/* ======================================================================== */

int dns_parser_extract_question(const uint8_t *msg, uint16_t msg_len,
								uint16_t *out_txid, char *out_domain,
								uint16_t *out_qtype);
int dns_parser_get_txid(const uint8_t *msg, uint16_t msg_len, uint16_t *out_txid);
int dns_parser_modify_txid(uint8_t *msg, uint16_t msg_len, uint16_t new_txid);
int dns_parser_extract_a_record(const uint8_t *msg, uint16_t msg_len,
								ip_addr_t *out_ip, uint32_t *out_ttl);
int dns_parser_construct_response(const uint8_t *query_msg, uint16_t query_len,
								  const ip_addr_t *cached_ip, uint32_t ttl,
								  uint8_t **out_response, uint16_t *out_response_len);

/* ======================================================================== */
/*                   Transaction Management APIs (Internal)                 */
/* ======================================================================== */

int dns_proxy_tx_create(uint16_t local_txid, const ip_addr_t *client_ip,
						uint16_t client_port, const char *domain_name,
						uint16_t qtype, uint16_t *out_upstream_txid);
int dns_proxy_tx_lookup_by_upstream(uint16_t upstream_txid, uint16_t *out_local_txid,
									ip_addr_t *out_client_ip, uint16_t *out_client_port,
									char *out_domain_name, uint16_t domain_name_len,
									uint16_t *out_qtype);
int dns_proxy_tx_remove(uint16_t upstream_txid);
void dns_proxy_tx_clear(void);

/* ======================================================================== */
/*                  Upstream Forwarder APIs (Internal)                      */
/* ======================================================================== */

int dns_proxy_forward_request(struct pbuf *p, uint16_t upstream_txid);

/* ======================================================================== */
/*                        Status Query APIs                                 */
/* ======================================================================== */

void dns_proxy_print_status(void);
void dns_proxy_dump_cache(void);
void dns_proxy_dump_transactions(void);

#ifdef __cplusplus
}
#endif

#endif /* DNS_PROXY_H */
