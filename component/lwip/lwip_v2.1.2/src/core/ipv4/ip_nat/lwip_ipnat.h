/**
 * @file lwip_ipnat.h
 * @brief Public API for IPv4 NAT.
 */

#ifndef __LWIP_NAT_H__
#define __LWIP_NAT_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IP_FORWARD
#if IP_NAT

/* ======================================================================== */
/*                           Table Configuration                            */
/* ======================================================================== */

/* Default size of the tables used for NAT */
#ifndef IP_NAT_MAX
#define IP_NAT_MAX 256
#endif

/* Dynamic port allocation range */
#ifndef IP_NAT_PORT_RANGE_START
#define IP_NAT_PORT_RANGE_START 49152
#endif

#ifndef IP_NAT_PORT_RANGE_END
#define IP_NAT_PORT_RANGE_END   65535
#endif

/* ======================================================================== */
/*                           Timeout Configuration                          */
/* ======================================================================== */

/* TCP timeout (milliseconds) */
#ifndef IP_NAT_TIMEOUT_MS_TCP
#define IP_NAT_TIMEOUT_MS_TCP           (8*60*1000)   /* 8min */
#endif

#ifndef IP_NAT_TIMEOUT_MS_TCP_DISCON
#define IP_NAT_TIMEOUT_MS_TCP_DISCON    (12*1000)     /* 12s */
#endif

#ifndef IP_NAT_TIMEOUT_MS_TCP_FIN_WAIT
#define IP_NAT_TIMEOUT_MS_TCP_FIN_WAIT  (5*60*1000)   /* 5min */
#endif

#ifndef IP_NAT_TIMEOUT_MS_TCP_RST_DISCON
#define IP_NAT_TIMEOUT_MS_TCP_RST_DISCON (6*1000)     /* 6s */
#endif

/* UDP timeout */
#ifndef IP_NAT_TIMEOUT_MS_UDP
#define IP_NAT_TIMEOUT_MS_UDP           (8*1000)      /* 8s */
#endif

#ifndef IP_NAT_TIMEOUT_MS_UDP_ALG
#define IP_NAT_TIMEOUT_MS_UDP_ALG       (20*1000)     /* 20s (ALG) */
#endif

/* ICMP timeout */
#ifndef IP_NAT_TIMEOUT_MS_ICMP
#define IP_NAT_TIMEOUT_MS_ICMP          (2*1000)      /* 2s */
#endif

/* ======================================================================== */
/*                           Filter Configuration                           */
/* ======================================================================== */

/* Percentage of IP_NAT_MAX above which old entries are pre-emptively evicted */
#ifndef IP_NAT_FILTER_THRESHOLD_PERCENT
#define IP_NAT_FILTER_THRESHOLD_PERCENT 80
#endif

/* Minimum age (ms) for an entry to be a candidate for forced eviction */
#ifndef IP_NAT_MAX_TIMEOUT_MS_FILTER_DROP
#define IP_NAT_MAX_TIMEOUT_MS_FILTER_DROP (60*1000)   /* 60s */
#endif

/* ======================================================================== */
/*                           Public API                                     */
/* ======================================================================== */

/* Initialization functions */
void ip_nat_initialize(void);
void ip_nat_reinitialize(void);

/* Packet processing */
err_t ip_nat_enqueue(struct pbuf *p, struct netif *inp);
err_t ip_nat_transfer(struct pbuf *p, struct netif *src, struct netif *target);

/* Timeout configuration API */
void ip_nat_set_tcp_timeout(u32_t secs);
void ip_nat_set_udp_timeout(u32_t secs);

/* Debug API */
void ip_nat_dump(void);

#endif /* IP_NAT */
#endif /* IP_FORWARD */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_NAT_H__ */
