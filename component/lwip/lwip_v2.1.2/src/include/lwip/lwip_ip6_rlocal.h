#ifndef __LWIP_RLOCAL_H__
#define __LWIP_RLOCAL_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "wifi_api.h"

extern struct netif xnetif[];

void handle_ip6(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp);
struct netif *nd6_find_nhb_netif(const ip6_addr_t *src, const ip6_addr_t *dest);
void print_ipv6_addresses(void);
void print_nhb(void);

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_NAT_H__ */
