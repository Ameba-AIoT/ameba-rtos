#ifndef DNS_PROXY_TYPES_H
#define DNS_PROXY_TYPES_H

/**
 * @file dns_proxy_types.h
 * @brief DNS Proxy data type definitions
 */

#include "dns_proxy_config.h"
#include "lwip_netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/*                        DNS Cache Entry                                   */
/* ======================================================================== */

/**
 * @brief DNS Cache Entry
 * @note Uses timestamp + original_ttl, calculates remaining TTL dynamically on lookup
 */
typedef struct dns_cache_entry {
	char domain_name[DNS_MAX_DOMAIN_LEN];		/**< Domain name (stored in lowercase) */
	ip_addr_t resolved_ip;				/**< Resolved IP address */
	uint32_t original_ttl;				/**< Original TTL from DNS response (unchanged) */
	uint32_t timestamp;				/**< Creation timestamp in milliseconds (sys_now(), unchanged) */
	uint32_t last_access_time;			/**< Last access time for LRU eviction */
	uint16_t record_type;				/**< Record type (A=1, AAAA=28, etc.) */
	struct dns_cache_entry *next;			/**< Linked list pointer */
} dns_cache_entry_t;

/* ======================================================================== */
/*                     DNS Transaction Mapping Entry                        */
/* ======================================================================== */

/**
 * @brief DNS Transaction Mapping Entry
 * @note Records mapping between local TxID and upstream TxID
 */
typedef struct dns_transaction_entry {
	uint16_t local_txid;				/**< Local transaction ID (sent to LAN client) */
	uint16_t upstream_txid;				/**< Upstream transaction ID (sent to upstream DNS) */
	ip_addr_t client_ip;				/**< Client IP address */
	uint16_t client_port;				/**< Client port */
	uint16_t qtype;						/**< DNS query type (A=1, AAAA=28, etc.) */
	uint32_t timestamp;					/**< Request timestamp in milliseconds (sys_now()) */
	char domain_name[DNS_MAX_DOMAIN_LEN];		/**< Queried domain name (for logging) */
	struct dns_transaction_entry *next;		/**< Linked list pointer */
} dns_transaction_entry_t;

/* ======================================================================== */
/*                         DNS Proxy Context                                */
/* ======================================================================== */

/**
 * @brief DNS Proxy Context
 * @note Singleton pattern, globally unique instance
 */
typedef struct dns_proxy_ctx {
	/* UDP PCBs */
	struct udp_pcb *downstream_pcb;		/**< Downstream PCB: listens for LAN requests (Bind to IP_ANY:53) */
	struct udp_pcb *upstream_pcb;		/**< Upstream PCB: sends to upstream DNS (Bind to fixed port 10053) */

	/* Upstream DNS Servers (from LwIP dns_getserver()) */
	ip_addr_t upstream_servers[DNS_MAX_SERVERS];	/**< Upstream DNS server list */
	uint8_t upstream_count;				/**< Number of upstream DNS servers */
	uint8_t active_upstream_idx;			/**< Index of current active upstream server */

	/* Cache */
	dns_cache_entry_t *cache_head;		/**< Cache linked list head */
	uint16_t cache_count;			/**< Current number of cache entries */
	uint16_t cache_max;			/**< Maximum number of cache entries */

	/* Transaction Map */
	dns_transaction_entry_t *tx_head;	/**< Transaction mapping linked list head */
	uint16_t tx_count;			/**< Current number of transactions */
	uint16_t tx_max;			/**< Maximum concurrent transactions */

	uint16_t next_upstream_txid;		/**< Next upstream TxID (auto-increment) */

	rtos_mutex_t lock;			/**< Mutex to protect shared data across threads */
	bool initialized;			/**< Initialization flag */
	bool running;				/**< Running status */
} dns_proxy_ctx_t;

#ifdef __cplusplus
}
#endif

#endif /* DNS_PROXY_TYPES_H */
