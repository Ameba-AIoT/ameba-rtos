#ifndef DNS_PROXY_CONFIG_H
#define DNS_PROXY_CONFIG_H

/**
 * @file dns_proxy_config.h
 * @brief DNS Proxy compile-time configuration
 */

#include "ameba_soc.h"

/* ======================================================================== */
/*                         Cache Configuration                              */
/* ======================================================================== */

/** Maximum number of cache entries (default: 128) */
#ifndef DNS_PROXY_CACHE_MAX_ENTRIES
#define DNS_PROXY_CACHE_MAX_ENTRIES		128
#endif

/** Default TTL in seconds when DNS response doesn't include TTL (5 minutes) */
#ifndef DNS_PROXY_CACHE_DEFAULT_TTL
#define DNS_PROXY_CACHE_DEFAULT_TTL		300
#endif

/* ======================================================================== */
/*                       Transaction Configuration                          */
/* ======================================================================== */

/** Maximum number of pending transactions (default: 64) */
#ifndef DNS_PROXY_TX_MAX_PENDING
#define DNS_PROXY_TX_MAX_PENDING		64
#endif

/** Transaction timeout in seconds (default: 10 seconds) */
#ifndef DNS_PROXY_TX_TIMEOUT_SEC
#define DNS_PROXY_TX_TIMEOUT_SEC		10
#endif

/* ======================================================================== */
/*                        DNS Protocol Configuration                        */
/* ======================================================================== */

/** Downstream listen port (standard DNS port) */
#ifndef DNS_PROXY_LISTEN_PORT
#define DNS_PROXY_LISTEN_PORT			53
#endif

/** Upstream communication fixed port (for easier debugging and firewall rules) */
#ifndef DNS_PROXY_UPSTREAM_PORT
#define DNS_PROXY_UPSTREAM_PORT			10053
#endif

/** Upstream DNS server port (standard DNS port) */
#ifndef DNS_PROXY_UPSTREAM_DNS_PORT
#define DNS_PROXY_UPSTREAM_DNS_PORT		53
#endif

/** Maximum domain name length */
#ifndef DNS_MAX_DOMAIN_LEN
#define DNS_MAX_DOMAIN_LEN			256
#endif

/** Maximum number of upstream DNS servers */
#ifndef DNS_MAX_SERVERS
#define DNS_MAX_SERVERS				3
#endif

/* ======================================================================== */
/*                          Log Configuration                               */
/* ======================================================================== */

/** DNS Proxy log levels */
enum {
	DNS_PROXY_DEBUG,
	DNS_PROXY_INFO,
	DNS_PROXY_WARNING,
	DNS_PROXY_ERROR
};

/** Default log level */
#ifndef DNS_PROXY_DEBUG_LEVEL
#define DNS_PROXY_DEBUG_LEVEL	DNS_PROXY_INFO
#endif

/** Log macro with level control */
#define DNS_PROXY_LOG(level, fmt, arg...)	\
	do {					\
		if (level >= DNS_PROXY_DEBUG_LEVEL) {	\
			RTK_LOGS("DNS_PROXY", RTK_LOG_ALWAYS, fmt, ##arg); \
		}					\
	} while (0)

/* ======================================================================== */
/*                         Error Code Definition                            */
/* ======================================================================== */

/** Success */
#define DNS_PROXY_OK				0

/** General initialization error */
#define DNS_PROXY_ERR_INIT			(-1)

/** UDP bind error */
#define DNS_PROXY_ERR_BIND			(-2)

/** Memory allocation error */
#define DNS_PROXY_ERR_NOMEM			(-3)

/** Invalid parameter */
#define DNS_PROXY_ERR_INVAL			(-4)

/** Cache not found */
#define DNS_PROXY_ERR_NOTFOUND			(-5)

/** Transaction not found */
#define DNS_PROXY_ERR_TX_NOTFOUND		(-6)

/** No available upstream server */
#define DNS_PROXY_ERR_NO_UPSTREAM		(-7)

/** Operation failed */
#define DNS_PROXY_ERR_FAIL			(-8)

/** DNS protocol error response from upstream (NXDOMAIN=3, SERVFAIL=2, REFUSED=5, etc.) */
#define DNS_PROXY_ERR_DNS_RCODE			(-9)

#endif /* DNS_PROXY_CONFIG_H */
