/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "lwip/opt.h"

#if defined(CONFIG_IP6_RLOCAL) && (CONFIG_IP6_RLOCAL == 1)

#include "lwip/nd6.h"
#include "lwip/priv/nd6_priv.h"
#include "lwip/prot/nd6.h"
#include "lwip/prot/icmp6.h"
#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/icmp6.h"
#include "lwip/mld6.h"
#include "lwip/dhcp6.h"
#include "lwip/ip.h"
#include "lwip/stats.h"
#include "lwip/dns.h"
#include <string.h>
#include "lwip/prot/ethernet.h"
#include "netif/ethernet.h"
#include "lwip/udp.h"
#include "lwip/lwip_ip6_rlocal.h"

/* ---------------copy from nd6.c ------------------*/
#define ND6_SEND_FLAG_MULTICAST_DEST 0x01
#define ND6_SEND_FLAG_ALLNODES_DEST 0x02
#define ND6_SEND_FLAG_ANY_SRC 0x04

/* Multicast address holder. */
static ip6_addr_t multicast_address;

#if LWIP_ND6_QUEUEING
static void nd6_free_q(struct nd6_q_entry *q);
#else /* LWIP_ND6_QUEUEING */
#define nd6_free_q(q) pbuf_free(q)
#endif /* LWIP_ND6_QUEUEING */

#if LWIP_ND6_QUEUEING
/**
 * Free a complete queue of nd6 q entries
 *
 * @param q a queue of nd6_q_entry to free
 */
static void
nd6_free_q(struct nd6_q_entry *q)
{
  struct nd6_q_entry *r;
  LWIP_ASSERT("q != NULL", q != NULL);
  LWIP_ASSERT("q->p != NULL", q->p != NULL);
  while (q) {
    r = q;
    q = q->next;
    LWIP_ASSERT("r->p != NULL", (r->p != NULL));
    pbuf_free(r->p);
    memp_free(MEMP_ND6_QUEUE, r);
  }
}
#endif /* LWIP_ND6_QUEUEING */

/**
 * Will free any resources associated with a neighbor cache
 * entry, and will mark it as unused.
 *
 * @param i the neighbor cache entry index to free
 */
static void
nd6_free_neighbor_cache_entry(s8_t i)
{
  if ((i < 0) || (i >= LWIP_ND6_NUM_NEIGHBORS)) {
    return;
  }
  char ipbuf[IP6ADDR_STRLEN_MAX];
  ip6addr_ntoa_r((const ip6_addr_t *)&(neighbor_cache[i].next_hop_address), ipbuf, sizeof(ipbuf));
  if (neighbor_cache[i].isrouter) {
    /* isrouter needs to be cleared before deleting a neighbor cache entry */
    return;
  }

  /* Free any queued packets. */
  if (neighbor_cache[i].q != NULL) {
    nd6_free_q(neighbor_cache[i].q);
    neighbor_cache[i].q = NULL;
  }

  struct nd6_ipq_entry *r;
  while (neighbor_cache[i].ipq != NULL) {
    r = neighbor_cache[i].ipq;
    neighbor_cache[i].ipq = r->next;
    free(r);
  }

  neighbor_cache[i].state = ND6_NO_ENTRY;
  neighbor_cache[i].isrouter = 0;
  neighbor_cache[i].netif = NULL;
  neighbor_cache[i].counter.reachable_time = 0;
  ip6_addr_set_zero(&(neighbor_cache[i].next_hop_address));
}

/**
 * Search for a neighbor cache entry
 *
 * @param ip6addr the IPv6 address of the neighbor
 * @return The neighbor cache entry index that matched, -1 if no
 * entry is found
 */
static s8_t
nd6_find_neighbor_cache_entry(const ip6_addr_t *ip6addr)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (ip6_addr_cmp(ip6addr, &(neighbor_cache[i].next_hop_address))) {
      return i;
    }
  }
  return -1;
}

/**
 * Send a neighbor advertisement message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_na(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags)
{
  struct na_header *na_hdr;
  struct lladdr_option *lladdr_opt;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  const ip6_addr_t *dest_addr;
  u16_t lladdr_opt_len;

  LWIP_ASSERT("target address is required", target_addr != NULL);

  /* Use link-local address as source address. */
  /* src_addr = netif_ip6_addr(netif, 0); */
  /* Use target address as source address. */
  src_addr = target_addr;
  char ip6_str[IP6ADDR_STRLEN_MAX];
  ip6addr_ntoa_r((const ip6_addr_t *)target_addr, ip6_str, sizeof(ip6_str));
  /* Allocate a packet. */
  lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  p = pbuf_alloc(PBUF_IP, sizeof(struct na_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  na_hdr = (struct na_header *)p->payload;
  lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

  na_hdr->type = ICMP6_TYPE_NA;
  na_hdr->code = 0;
  na_hdr->chksum = 0;
  na_hdr->flags = flags & 0xf0;
  na_hdr->reserved[0] = 0;
  na_hdr->reserved[1] = 0;
  na_hdr->reserved[2] = 0;
  ip6_addr_copy_to_packed(na_hdr->target_address, *target_addr);

  lladdr_opt->type = ND6_OPTION_TYPE_TARGET_LLADDR;
  lladdr_opt->length = (u8_t)lladdr_opt_len;
  SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    dest_addr = &multicast_address;
  } else if (flags & ND6_SEND_FLAG_ALLNODES_DEST) {
    ip6_addr_set_allnodes_linklocal(&multicast_address);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    dest_addr = &multicast_address;
  } else {
    dest_addr = ip6_current_src_addr();
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    na_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      dest_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, src_addr, dest_addr,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

/**
 * Create a new neighbor cache entry.
 *
 * If no unused entry is found, will try to recycle an old entry
 * according to ad-hoc "age" heuristic.
 *
 * @return The neighbor cache entry index that was created, -1 if no
 * entry could be created
 */
static s8_t
nd6_new_neighbor_cache_entry(void)
{
  s8_t i;
  s8_t j;
  u32_t time;


  /* First, try to find an empty entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (neighbor_cache[i].state == ND6_NO_ENTRY) {
      return i;
    }
  }

  /* We need to recycle an entry. in general, do not recycle if it is a router. */

  /* Next, try to find a Stale entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_STALE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Probe entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_PROBE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Delayed entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_DELAY) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find the oldest reachable entry. */
  time = 0xfffffffful;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_REACHABLE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.reachable_time < time) {
        j = i;
        time = neighbor_cache[i].counter.reachable_time;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry without queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (
        (neighbor_cache[i].q == NULL) &&
        (neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry with queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* No more entries to try. */
  return -1;
}

/**
 * Send a neighbor solicitation message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_ns(struct netif *netif, const ip6_addr_t *target_addr, u8_t flags)
{
  struct ns_header *ns_hdr;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  u16_t lladdr_opt_len;

  LWIP_ASSERT("target address is required", target_addr != NULL);
  char ip6_str[IP6ADDR_STRLEN_MAX];
  ip6addr_ntoa_r((const ip6_addr_t *)target_addr, ip6_str, sizeof(ip6_str));
  if (!(flags & ND6_SEND_FLAG_ANY_SRC) &&
      ip6_addr_isvalid(netif_ip6_addr_state(netif,0))) {
    /* Use link-local address as source address. */
    src_addr = netif_ip6_addr(netif, 0);
    /* calculate option length (in 8-byte-blocks) */
    lladdr_opt_len = ((netif->hwaddr_len + 2) + 7) >> 3;
  } else {
    src_addr = IP6_ADDR_ANY6;
    /* Option "MUST NOT be included when the source IP address is the unspecified address." */
    lladdr_opt_len = 0;
  }

  /* Allocate a packet. */
  p = pbuf_alloc(PBUF_IP, sizeof(struct ns_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  ns_hdr = (struct ns_header *)p->payload;

  ns_hdr->type = ICMP6_TYPE_NS;
  ns_hdr->code = 0;
  ns_hdr->chksum = 0;
  ns_hdr->reserved = 0;
  ip6_addr_copy_to_packed(ns_hdr->target_address, *target_addr);

  if (lladdr_opt_len != 0) {
    struct lladdr_option *lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct ns_header));
    lladdr_opt->type = ND6_OPTION_TYPE_SOURCE_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);
  }

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    ip6_addr_assign_zone(&multicast_address, IP6_MULTICAST, netif);
    target_addr = &multicast_address;
  }

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    ns_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      target_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, (src_addr == IP6_ADDR_ANY6) ? NULL : src_addr, target_addr,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

/** Send a neighbor solicitation message for a specific neighbor cache entry
 *
 * @param entry the neightbor cache entry for wich to send the message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_neighbor_cache_probe(struct nd6_neighbor_cache_entry *entry, u8_t flags)
{
  nd6_send_ns(entry->netif, &entry->next_hop_address, flags);
}
/* ---------------copy from nd6.c end ------------------*/

static void
nd6_send_na_with_dest(struct netif *netif, const ip6_addr_t *target_addr, const ip6_addr_t *dest_addr, u8_t flags)
{
  struct na_header *na_hdr;
  struct lladdr_option *lladdr_opt;
  struct pbuf *p;
  const ip6_addr_t *src_addr;
  u16_t lladdr_opt_len;

  LWIP_ASSERT("target address is required", target_addr != NULL);

  /* Use link-local address as source address. */
  /* src_addr = netif_ip6_addr(netif, 0); */
  /* Use target address as source address. */
  src_addr = target_addr;
  char ip6_str[IP6ADDR_STRLEN_MAX];
  ip6addr_ntoa_r((const ip6_addr_t *)target_addr, ip6_str, sizeof(ip6_str));
  /* Allocate a packet. */
  lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  p = pbuf_alloc(PBUF_IP, sizeof(struct na_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if (p == NULL) {
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  na_hdr = (struct na_header *)p->payload;
  lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

  na_hdr->type = ICMP6_TYPE_NA;
  na_hdr->code = 0;
  na_hdr->chksum = 0;
  na_hdr->flags = flags & 0xf0;
  na_hdr->reserved[0] = 0;
  na_hdr->reserved[1] = 0;
  na_hdr->reserved[2] = 0;
  ip6_addr_copy_to_packed(na_hdr->target_address, *target_addr);

  lladdr_opt->type = ND6_OPTION_TYPE_TARGET_LLADDR;
  lladdr_opt->length = (u8_t)lladdr_opt_len;
  SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);

#if CHECKSUM_GEN_ICMP6
  IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_ICMP6) {
    na_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
      dest_addr);
  }
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, src_addr, dest_addr,
      ND6_HOPLIM, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

/**
 * 1. Handling NA Messages
 * 
 * used on sta-ap device
 */
static void 
handle_NA(struct pbuf *p, struct netif *inp)
{
    struct na_header *na_hdr;
    struct lladdr_option *lladdr_opt;
    ip6_addr_t target_address;
    int idx = inp - xnetif;
    s8_t i = -1;

    /* Check that na header fits in packet. */
    if (p->len < (sizeof(struct na_header))) {
      return;
    }

    na_hdr = (struct na_header *)p->payload;

    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, na_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    /* Check a subset of the other RFC 4861 Sec. 7.1.2 requirements. */
    if (IP6H_HOPLIM(ip6_current_header()) != ND6_HOPLIM || na_hdr->code != 0 ||
        ip6_addr_ismulticast(&target_address)) {
      return;
    }

    char ip6_str[IP6ADDR_STRLEN_MAX];
    ip6addr_ntoa_r((const ip6_addr_t *)&target_address, ip6_str, sizeof(ip6_str));

    if (!ip6_addr_ismulticast(ip6_current_dest_addr())) {
      /* Find the cache entry corresponding to this na. */
      i = nd6_find_neighbor_cache_entry(&target_address);
      if (i < 0) {
        return;
      }

      /* Update cache entry. Just check */
      if ((na_hdr->flags & ND6_FLAG_OVERRIDE) ||
          (neighbor_cache[i].state == ND6_INCOMPLETE)) {
        /* Check that link-layer address option also fits in packet. */
        if (p->len < (sizeof(struct na_header) + 2)) {
          return;
        }

        lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));

        if (p->len < (sizeof(struct na_header) + (lladdr_opt->length << 3))) {
          return;
        }
      }

      struct nd6_ipq_entry *r;
      while (neighbor_cache[i].ipq != NULL) {
        r = neighbor_cache[i].ipq;
        neighbor_cache[i].ipq = r->next;
        nd6_send_na_with_dest(&xnetif[!idx], &target_address, &r->ipq_address, ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE);
        free(r);
      }
    }
}

/**
 * 1. Handling NS adn DAD Messages
 * 
 * used on sta-ap device
 */
static void 
handle_NS(struct pbuf *p, struct netif *inp)
{
    struct ns_header *ns_hdr;
    ip6_addr_t target_address;
    u8_t ns_is_dad;
    s8_t i;
    int idx = inp - xnetif;

    ns_hdr = (struct ns_header *)p->payload;
    ns_is_dad = ip6_addr_isany(ip6_current_src_addr());

    /* Check that ns header fits in packet. */
    if (p->len < sizeof(struct ns_header)) {
      /* @todo debug message */
      pbuf_free(p);
      return;
    }

    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, ns_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    if (ip6_addr_islinklocal(&target_address) ||
        ip6_addr_isloopback(&target_address) ||
        ip6_addr_ismulticast(&target_address))
    {
        return;
    }
    // if to ourself return;
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(inp, i)) &&
          ip6_addr_cmp(&target_address, netif_ip6_addr(inp, i))) {
            return;
      }
    }

	char ipbuf[IP6ADDR_STRLEN_MAX];
	ip6addr_ntoa_r((const ip6_addr_t *)&target_address, ipbuf, sizeof(ipbuf));
    
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(&xnetif[!idx], i))  &&
        ip6_addr_cmp(&target_address, netif_ip6_addr(&xnetif[!idx], i))) {

      if (ns_is_dad) {
        nd6_send_na(inp, &target_address, ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
      } else {
        nd6_send_na(inp, &target_address, ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE);
      }
      return;
    }
  }

  i = nd6_find_neighbor_cache_entry(&target_address);
  if (i >= 0) {
    if((neighbor_cache[i].state != ND6_NO_ENTRY) && (neighbor_cache[i].state != ND6_INCOMPLETE)) {
      if (ns_is_dad) {
        // recv a new dad, may sta link to other sta-ap, delete neighbor cache and forward dad
        nd6_free_neighbor_cache_entry(i);
        nd6_send_ns(&xnetif[!idx], &target_address, ND6_SEND_FLAG_MULTICAST_DEST | ND6_SEND_FLAG_ANY_SRC);
      } else {
        /* if get nhb same intf means that subsequent logic will process this packet*/
        if (!neighbor_cache[i].netif || neighbor_cache[i].netif == inp)
        {
          return;
        }
    
        nd6_send_na(inp, &target_address, ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE);
      }
    }
  } else {

      /* Neighbor not in cache. Make a new entry. */
    i = nd6_new_neighbor_cache_entry();
    if (i < 0) {
        return;
    }
    /* Initialize fields. */
    ip6_addr_copy(neighbor_cache[i].next_hop_address, target_address);
    neighbor_cache[i].isrouter = 0;
    neighbor_cache[i].netif = &xnetif[!idx];
    neighbor_cache[i].state = ND6_INCOMPLETE;
    neighbor_cache[i].counter.probes_sent = 1;
    if (!ns_is_dad) {
      struct nd6_ipq_entry *ipq = NULL;
      ipq = (struct nd6_ipq_entry *)malloc(sizeof(struct nd6_ipq_entry));
      if(NULL == ipq){
        return;
      }
      ip6_addr_copy(ipq->ipq_address, *ip6_current_src_addr());
      ipq->next = neighbor_cache[i].ipq;
      neighbor_cache[i].ipq = ipq;
      nd6_send_neighbor_cache_probe(&neighbor_cache[i], ND6_SEND_FLAG_MULTICAST_DEST);
    } else {
      nd6_send_ns(&xnetif[!idx], &target_address, ND6_SEND_FLAG_MULTICAST_DEST | ND6_SEND_FLAG_ANY_SRC);
    }
  }
  return;
}
/**
 * Station handling DAD packets: free nhb cache for sta switching
 * 
 * used on sta device
 */
static void 
handle_NS_sta(struct pbuf *p, struct netif *inp)
{
  struct ns_header *ns_hdr;
  ip6_addr_t target_address;
  u8_t ns_is_dad;
  s8_t i;

  ns_hdr = (struct ns_header *)p->payload;
  ns_is_dad = ip6_addr_isany(ip6_current_src_addr());
  
  if (ns_is_dad) {
    /* Check that ns header fits in packet. */
    if (p->len < sizeof(struct ns_header)) {
      /* @todo debug message */
      pbuf_free(p);
      return;
    }

    /* Create an aligned, zoned copy of the target address. */
    ip6_addr_copy_from_packed(target_address, ns_hdr->target_address);
    ip6_addr_assign_zone(&target_address, IP6_UNICAST, inp);

    char ipbuf[IP6ADDR_STRLEN_MAX];
    ip6addr_ntoa_r((const ip6_addr_t *)&target_address, ipbuf, sizeof(ipbuf));
      
    i = nd6_find_neighbor_cache_entry(&target_address);
    if (i >= 0) {
          nd6_free_neighbor_cache_entry(i);
    } 
  }
  return;
}

/**
 * 1. Handling NS/NA Messages
 * 2. Multicast Forwarding
 * 
 * used on sta-ap device
 */
void 
handle_ip6_sta_ap(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp)
{
  const u8_t *nexth;
  int idx = inp - xnetif;
  struct netif *netif = &xnetif[!idx];
  
  nexth = &IP6H_NEXTH(iphdr);

  if(IP6_NEXTH_ICMP6 == *nexth) {
    struct icmp6_hdr *icmp6hdr;

    /* Move to payload. */
    pbuf_remove_header(p, IP6_HLEN);
    /* Check that ICMPv6 header fits in payload */
    if (p->len < sizeof(struct icmp6_hdr)) {
      pbuf_add_header(p, IP6_HLEN);
      return;
    }

    icmp6hdr = (struct icmp6_hdr *)p->payload;
  #if CHECKSUM_CHECK_ICMP6
    IF__NETIF_CHECKSUM_ENABLED(inp, NETIF_CHECKSUM_CHECK_ICMP6) {
      if (ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->tot_len, ip6_current_src_addr(),
                            ip6_current_dest_addr()) != 0) {
        /* Checksum failed */
        pbuf_add_header(p, IP6_HLEN);
        return;
      }
    }
  #endif /* CHECKSUM_CHECK_ICMP6 */
    switch (icmp6hdr->type) {
    case ICMP6_TYPE_NS: /* Neighbor solicitation */
      handle_NS(p, inp);
      break;
    case ICMP6_TYPE_NA:/* Neighbor Advertisement. */
      handle_NA(p, inp);
      break;
    }
    pbuf_add_header(p, IP6_HLEN);
  } else if(IP6_NEXTH_UDP == *nexth) {
    if (ip6_addr_ismulticast(ip6_current_dest_addr())) {
      
      /* if pkt is from ap port, use udp_input trans to sta port */
      if(1 == idx) {
        struct pbuf *r;
        /* Allocate a new pbuf */
        r = pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_RAM);
        if (r != NULL) {
          if (pbuf_copy(r, p) == ERR_OK) {
            // if from ap, loop output to sta
            /* Move to payload. */
            pbuf_remove_header(r, IP6_HLEN);
            udp_input(r, netif);
          } else {
            pbuf_free(r);
          }
        }
      }
          
      // send use other interface 
      struct eth_addr dest;
      ip6_addr_t *ip6addr = ip6_current_dest_addr();

      /* The destination IP address must be properly zoned from here on down. */
      IP6_ADDR_ZONECHECK_NETIF(ip6addr, netif);

      /* Hash IP multicast address to MAC address.*/
      dest.addr[0] = 0x33;
      dest.addr[1] = 0x33;
      dest.addr[2] = ((const u8_t *)(&(ip6addr->addr[3])))[0];
      dest.addr[3] = ((const u8_t *)(&(ip6addr->addr[3])))[1];
      dest.addr[4] = ((const u8_t *)(&(ip6addr->addr[3])))[2];
      dest.addr[5] = ((const u8_t *)(&(ip6addr->addr[3])))[3];

      /* Send out. */
      ethernet_output(netif, p, (const struct eth_addr*)(netif->hwaddr), &dest, ETHTYPE_IPV6);
      //netif->output_ip6(netif, p, ip6_current_dest_addr());
      pbuf_remove_header(p, SIZEOF_ETH_HDR);
    }
  }
}

/**
 * 1. Handling NS(DAD) Messages
 * 
 * used on sta mode device
 */
void 
handle_ip6_sta(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp) 
{
  const u8_t *nexth;
  
  nexth = &IP6H_NEXTH(iphdr);

  if(IP6_NEXTH_ICMP6 == *nexth) {
    struct icmp6_hdr *icmp6hdr;

    /* Move to payload. */
    pbuf_remove_header(p, IP6_HLEN);
    /* Check that ICMPv6 header fits in payload */
    if (p->len < sizeof(struct icmp6_hdr)) {
      pbuf_add_header(p, IP6_HLEN);
      return;
    }

    icmp6hdr = (struct icmp6_hdr *)p->payload;
  #if CHECKSUM_CHECK_ICMP6
    IF__NETIF_CHECKSUM_ENABLED(inp, NETIF_CHECKSUM_CHECK_ICMP6) {
      if (ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->tot_len, ip6_current_src_addr(),
                            ip6_current_dest_addr()) != 0) {
        /* Checksum failed */
        pbuf_add_header(p, IP6_HLEN);
        return;
      }
    }
  #endif /* CHECKSUM_CHECK_ICMP6 */
    switch (icmp6hdr->type) {
    case ICMP6_TYPE_NS: /* Neighbor solicitation */
      handle_NS_sta(p, inp);
      break;
    case ICMP6_TYPE_NA:/* Neighbor Advertisement. */
      break;
    }
    pbuf_add_header(p, IP6_HLEN);
  } 
}

void 
handle_ip6(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp) 
{
  if(is_sta_ap()) {
    handle_ip6_sta_ap(p, iphdr, inp);
  } else if(is_sta_only()) {
    handle_ip6_sta(p, iphdr, inp);
  }
  return;
}

struct netif *
nd6_find_nhb_netif(const ip6_addr_t *src, const ip6_addr_t *dest)
{
  UNUSED(src);
  s8_t i;

  if(!is_sta_ap())
    return NULL;

  i = nd6_find_neighbor_cache_entry(dest);
  if(i < 0) {
    return xnetif;
  }
  if ((i >= 0) && (neighbor_cache[i].state != ND6_NO_ENTRY || neighbor_cache[i].state != ND6_INCOMPLETE)) {
    return neighbor_cache[i].netif;
  }

  return NULL;
}

void
lwip_free_neighbor_cache(unsigned char *mac)
{
  s8_t i = -1;
  //LOCK_TCPIP_CORE();
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (!memcmp(mac, neighbor_cache[i].lladdr, NETIF_MAX_HWADDR_LEN)) {
      break;
    }
  }
  if(i >= 0) {
    nd6_free_neighbor_cache_entry(i);
  }
  //UNLOCK_TCPIP_CORE();
}

void 
print_rlocal_nhb(void)
{
  int i = 0;
	RTK_LOGI(NOTAG, "print nhb\r\n");
  for(i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++)
  {
    if(neighbor_cache[i].state != ND6_NO_ENTRY)
    {
      char ipbuf[IP6ADDR_STRLEN_MAX];
      ip6addr_ntoa_r((const ip6_addr_t *)&(neighbor_cache[i].next_hop_address), ipbuf, sizeof(ipbuf));
      RTK_LOGI(NOTAG, "nhb id %d state %d addr %s netif %s\r\n", i, neighbor_cache[i].state, ipbuf, neighbor_cache[i].netif->name);
    }
  }
}

void
print_rlocal_ipv6_addresses(void)
{
	int k = 0;

    for (k = 0; k < NET_IF_NUM; k++) {

        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_isvalid(xnetif[k].ip6_addr_state[i])) {
                char ip6_str[IP6ADDR_STRLEN_MAX];
                ip6addr_ntoa_r((const ip6_addr_t *)&xnetif[k].ip6_addr[i], ip6_str, sizeof(ip6_str));
                RTK_LOGI(NOTAG, "IPv6 Address %d: %s Interface(%d): %s\n", i, ip6_str, k, xnetif[k].name);
            }
        }
    }
}
#endif
