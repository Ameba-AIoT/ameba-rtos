/**
 * @file
 * IPv4 NAT implementation.
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"
#include "log.h"

#if LWIP_IPV4

#include "lwip/ip.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/ip4_frag.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/icmp.h"
#include "lwip/igmp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/autoip.h"
#include "lwip/stats.h"
#include "lwip/timeouts.h"
#include "lwip_ipnat.h"
#include "os_wrapper.h"

#if defined(IP_FORWARD) && (IP_FORWARD == 1)

#if defined(IP_NAT) && (IP_NAT == 1)

void ip_nat_debug_print(void);
static u16_t ip_nat_add_entry(u8_t proto, u32_t src, u16_t sport, u32_t dest, u16_t dport, u32_t app_use);
static struct nat_table *ip_nat_entry_search(u8_t proto, u32_t addr, u16_t port, u16_t mport, u8_t dest, u8_t frag, u32_t daddr, u16_t dportmap);

#define NON_INDEX (0xFFFF)

/* NAT session entry. */
struct nat_table {
	u32_t src;
	u32_t dest;
	u32_t ts;
	u16_t sport;
	u16_t dport;
	u16_t mport;
	u8_t proto;
	u8_t fin_wait1 : 1;
	u8_t fin_wait2 : 1;
	u8_t fin_ack1 : 1;
	u8_t fin_ack2 : 1;
	u8_t syn_acked : 1;
	u8_t rst : 1;
	u16_t next, prev;
	u32_t app_use;
	u32_t pkt_count;
};

u16_t nat_entry_list = NON_INDEX;
u16_t nat_entry_list_last = NON_INDEX;
u16_t nat_entry_idle = 0;
rtos_mutex_t nat_entry_lock = NULL;
static struct nat_table *ip_nat_table = NULL;

uint32_t filter_drop_threshold = 0;
uint32_t tcp_entry_count = 0;
uint32_t udp_entry_count = 0;
uint32_t icmp_entry_count = 0;

uint32_t ip_nat_tcp_max_timeout = IP_NAT_TIMEOUT_MS_TCP;
uint32_t ip_nat_udp_max_timeout = IP_NAT_TIMEOUT_MS_UDP;

static inline uint32_t GET_NAT_TIME_ELAPSED(uint32_t now_ts, uint32_t e_ts)
{
	if (now_ts >= e_ts) {
		return ((now_ts) - (e_ts));
	} else {

		return (((now_ts) + ((e_ts) ^ 0xFFFFFFFF) + 1));
	}
}

static inline struct nat_table *GET_NAT_ENTRY(u16_t en_idx)
{
	if (en_idx == NON_INDEX) {
		return NULL;
	} else {
		return &ip_nat_table[en_idx];
	}
}

/**
 * @brief Flush all active NAT sessions and reset the table to an empty state.
 */
void ip_nat_reinitialize(void)
{
	int i;

	if (!ip_nat_table) {
		RTK_LOGE(NOTAG, "NAT table not initialized\n");
		return;
	}

	rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

	memset(ip_nat_table, 0x00, sizeof(struct nat_table)*IP_NAT_MAX);
	for (i = 0; i < IP_NAT_MAX - 1; i++) {
		ip_nat_table[i].next = i + 1;
	}
	ip_nat_table[i].next = NON_INDEX;
	nat_entry_idle = 0;
	nat_entry_list = NON_INDEX;
	nat_entry_list_last = NON_INDEX;

	tcp_entry_count = 0;
	udp_entry_count = 0;
	icmp_entry_count = 0;

	rtos_mutex_give(nat_entry_lock);
}
#if 0
static void ipnat_ageing_tmr(void *arg)
{
	int total_session = 0;
	rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
	total_session = tcp_entry_count + udp_entry_count + icmp_entry_count;

	if (total_session > 0 && total_session < 50) {
		//ip_nat_debug_print();
	} else {
		//RTK_LOGI(NOTAG, "\n\r total :%d %d %d", tcp_entry_count, udp_entry_count, icmp_entry_count);
	}
	rtos_mutex_give(nat_entry_lock);
	sys_timeout((30 * 1000), ipnat_ageing_tmr, arg);
}

#endif

/**
 * @brief NAT subsystem initialization.
 */
void ip_nat_initialize(void)
{
	int i;

	if (ip_nat_table) {
		RTK_LOGE(NOTAG, "NAT already initialized\n");
		return;
	}

	RTK_LOGI(NOTAG, "\n\r");
	RTK_LOGI(NOTAG, "\n\r IP NAT Initialize!!!!!");
	RTK_LOGI(NOTAG, "\n\r");

	ip_nat_table = (struct nat_table *)malloc(sizeof(struct nat_table) * IP_NAT_MAX);
	if (!ip_nat_table) {
		RTK_LOGE(NOTAG, "Failed to allocate NAT table");
		return;
	}

	rtos_mutex_create(&nat_entry_lock);
	filter_drop_threshold = (IP_NAT_MAX * IP_NAT_FILTER_THRESHOLD_PERCENT) / 100;

	memset(ip_nat_table, 0x00, sizeof(struct nat_table)*IP_NAT_MAX);
	for (i = 0; i < IP_NAT_MAX - 1; i++) {
		ip_nat_table[i].next = i + 1;
	}
	ip_nat_table[i].next = NON_INDEX;
	nat_entry_idle = 0;
	tcp_entry_count = 0;
	udp_entry_count = 0;
	icmp_entry_count = 0;
}

/* ======================================================================== */
/*                          Port Allocation                                 */
/* ======================================================================== */

/**
 * @brief  Check whether a mapped port is already used by an active session.
 * @param  proto IP protocol (IP_PROTO_TCP / IP_PROTO_UDP).
 * @param  port Port to check, in network byte order.
 * @return 1 if in use, 0 if free.
 */
static u8_t ip_nat_port_in_use(u8_t proto, u16_t port)
{
	u16_t i;
	struct nat_table *entry;

	for (i = nat_entry_list; i != NON_INDEX; i = entry->next) {
		entry = GET_NAT_ENTRY(i);
		if (entry->proto == proto && entry->mport == port) {
			return 1;
		}
	}
	return 0;
}

/**
 * @brief  Check whether a port is bound by a local lwIP PCB on this router.
 * @param  proto IP protocol (IP_PROTO_TCP / IP_PROTO_UDP).
 * @param  port Port to check, in network byte order.
 * @return 1 if bound, 0 if free.
 */
static u8_t ip_nat_port_listening(u8_t proto, u16_t port)
{
	u16_t host_port = PP_NTOHS(port);

	#if LWIP_TCP
	if (proto == IP_PROTO_TCP) {
		struct tcp_pcb_listen *pcb;
		for (pcb = tcp_listen_pcbs.listen_pcbs; pcb; pcb = pcb->next) {
			if (pcb->local_port == host_port) {
				return 1;
			}
		}
	}
#endif

#if LWIP_UDP
	if (proto == IP_PROTO_UDP) {
		struct udp_pcb *pcb;
		for (pcb = udp_pcbs; pcb; pcb = pcb->next) {
			if (pcb->local_port == host_port) {
				return 1;
			}
		}
	}
#endif

	return 0;
}

/**
 * @brief  Allocate a free mapped port for a new NAT session (random selection).
 * @param  proto IP protocol (IP_PROTO_TCP / IP_PROTO_UDP).
 * @param  original_port Original port.
 * @return Allocated port in network byte order, or 0 on failure.
 */
static u16_t ip_nat_allocate_port(u8_t proto, u16_t original_port)
{
	(void)original_port;

	u16_t port;
	u16_t attempts = 0;
	const u16_t max_attempts = 100;

	do {
		port = PP_HTONS(IP_NAT_PORT_RANGE_START +
			   (LWIP_RAND() % (IP_NAT_PORT_RANGE_END - IP_NAT_PORT_RANGE_START + 1)));

		attempts++;
		if (attempts > max_attempts) {
			RTK_LOGE(NOTAG, "Port allocation failed after %d attempts\n", attempts);
			return 0;
		}
	} while (ip_nat_port_in_use(proto, port) || ip_nat_port_listening(proto, port));

	return port;
}

/**
 * @brief Insert an entry into the active session list.
 * @param rule_entry Entry to activate.
 */
static void ip_nat_insert_new_rule(struct nat_table *rule_entry)
{
	u16_t ti = rule_entry - ip_nat_table;

	if (ti != nat_entry_idle) {

		//RTK_LOGI(NOTAG, "\n\r %s %d", __FUNCTION__, __LINE__);
		//RTK_LOGI(NOTAG, "\n\r");
	}
	nat_entry_idle = rule_entry->next;
	rule_entry->prev = NON_INDEX;
	rule_entry->next = nat_entry_list;

	if (nat_entry_list != NON_INDEX) {
		GET_NAT_ENTRY(nat_entry_list)->prev = ti;
	}
	nat_entry_list = ti;
	if (nat_entry_list_last == NON_INDEX) {
		nat_entry_list_last = ti;
	}

#if LWIP_TCP
	if (rule_entry->proto == IP_PROTO_TCP) {
		tcp_entry_count++;
	}
#endif
#if LWIP_UDP
	if (rule_entry->proto == IP_PROTO_UDP) {
		udp_entry_count++;
	}
#endif
#if LWIP_ICMP
	if (rule_entry->proto == IP_PROTO_ICMP) {
		icmp_entry_count++;
	}
#endif
//RTK_LOGI(NOTAG, "\n\r");
//RTK_LOGI(NOTAG, "\n\r INSERT %d %d TCP=%d UDP=%d ICMP=%d",__LINE__, rule_entry->proto, tcp_entry_count, udp_entry_count, icmp_entry_count);
//RTK_LOGI(NOTAG, "\n\r");
}

/**
 * @brief Remove an entry from the active session list and return it to the idle pool.
 * @param t Entry to deactivate.
 */
static void ip_nat_set_entry_idle(struct nat_table *t)
{
	u16_t ti = t - ip_nat_table;
	if (ti == nat_entry_list) {
		nat_entry_list = t->next;
	}
	if (ti == nat_entry_list_last) {
		nat_entry_list_last = t->prev;
	}
	if (t->next != NON_INDEX) {
		GET_NAT_ENTRY(t->next)->prev = t->prev;
	}
	if (t->prev != NON_INDEX) {
		GET_NAT_ENTRY(t->prev)->next = t->next;
	}
	t->prev = NON_INDEX;
	t->next = nat_entry_idle;
	nat_entry_idle = ti;

#if LWIP_TCP
	if (t->proto == IP_PROTO_TCP) {
		tcp_entry_count--;
	}
#endif

#if LWIP_UDP
	if (t->proto == IP_PROTO_UDP) {
		udp_entry_count--;
	}
#endif

#if LWIP_ICMP
	if (t->proto == IP_PROTO_ICMP) {
		icmp_entry_count--;
	}
#endif

}

/**
 * @brief  Search the active session table for a matching NAT entry.
 * @param  proto IP protocol.
 * @param  addr Source (dest==0) or destination (dest==1) IP to match.
 * @param  port Source (dest==0) or destination (dest==1) port to match.
 * @param  mport Mapped (WAN-facing) port to match; 0 means don't match by mport.
 * @param  dest 0 = searching TX (src) direction, 1 = searching RX (dest) direction.
 * @param  frag 1 if this is a non-first IP fragment (skip port matching).
 * @param  daddr Remote IP to additionally filter on; 0 to skip.
 * @param  dportmap Remote port to additionally filter on; 0 to skip.
 * @return Matching entry, or NULL if not found.
 */
static struct nat_table *ip_nat_entry_search(u8_t proto, u32_t addr, u16_t port, u16_t mport, u8_t dest, u8_t frag, u32_t daddr, u16_t dportmap)
{
	u16_t i, next;
	struct nat_table *NEntry;

	LWIP_DEBUGF(IPNAT_DEBUG, ("ip_nat_entry_search\n"));

	LWIP_DEBUGF(IPNAT_DEBUG, ("searching in table %s: %"U16_F".%"U16_F".%"U16_F".%"U16_F", port: %d, mport: %d\n",
							  (dest ? "dest" : "src"),
							  ip4_addr1_16((ip4_addr_t *)&addr), ip4_addr2_16((ip4_addr_t *)&addr),
							  ip4_addr3_16((ip4_addr_t *)&addr), ip4_addr4_16((ip4_addr_t *)&addr),
							  PP_HTONS(port),
							  PP_HTONS(mport)));

	u32_t now = sys_now();
	for (i = nat_entry_list; i != NON_INDEX; i = next) {
		NEntry = GET_NAT_ENTRY(i);
		next = NEntry->next;
#if LWIP_TCP
		if (NEntry->proto == IP_PROTO_TCP &&
			((((NEntry->fin_ack1 && NEntry->fin_ack2) || !NEntry->syn_acked) &&
			 GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_TCP_DISCON) ||
			 GET_NAT_TIME_ELAPSED(now, NEntry->ts) > ip_nat_tcp_max_timeout)) {
			if (!NEntry->syn_acked && (GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_TCP_DISCON)) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			}
			if (NEntry->fin_ack1 && NEntry->fin_ack2 && GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_TCP_FIN_WAIT) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			}
			if (GET_NAT_TIME_ELAPSED(now, NEntry->ts) > ip_nat_tcp_max_timeout) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			}

		}
		if (NEntry->proto == IP_PROTO_TCP && NEntry->rst && (GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_TCP_RST_DISCON)) {
			ip_nat_set_entry_idle(NEntry);
			continue;
		}
#endif

#if LWIP_UDP
		if (NEntry->proto == IP_PROTO_UDP && GET_NAT_TIME_ELAPSED(now, NEntry->ts) > ip_nat_udp_max_timeout) {
			if (NEntry->app_use == 0) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			} else if (NEntry->app_use == 1 &&
						GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_UDP_ALG) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			}
		}
#endif

#if LWIP_ICMP
		if (NEntry->proto == IP_PROTO_ICMP) {
			if (GET_NAT_TIME_ELAPSED(now, NEntry->ts) > IP_NAT_TIMEOUT_MS_ICMP) {
				ip_nat_set_entry_idle(NEntry);
				continue;
			}
		}
#endif

		if (frag == 0 && dest == 0 && NEntry->proto == proto &&
			NEntry->src == addr && NEntry->sport == port) {
			NEntry->ts = now;
			NEntry->pkt_count++;
			return NEntry;
		}

		if (frag == 0 && dest == 0 && NEntry->proto == proto && NEntry->src == addr &&
			daddr != 0x00 && NEntry->dest == daddr && dportmap != 0 && NEntry->dport == dportmap) {
			NEntry->ts = now;
			NEntry->pkt_count++;
			return NEntry;
		}

		if (frag == 0 && dest == 1 && NEntry->proto == proto &&
			NEntry->dest == addr && NEntry->dport == port) {
			if (mport == 0 || NEntry->mport == mport || NEntry->sport == mport) {
				NEntry->ts = now;
				NEntry->pkt_count++;
				return NEntry;
			}
		}

		if (frag == 1 && dest == 0 && NEntry->proto == proto &&
			NEntry->src == addr && NEntry->dest == daddr) {
			NEntry->ts = now;
			NEntry->pkt_count++;
			return NEntry;
		}

		if (frag == 1 && dest == 1 && NEntry->proto == proto && NEntry->dest == addr) {
			NEntry->ts = now;
			NEntry->pkt_count++;
			return NEntry;
		}
	}

	LWIP_DEBUGF(IPNAT_DEBUG, ("not found\n"));
	return NULL;
}

/**
 * @brief Evict sessions that have been idle longer than @p threshold_value ms.
 * @param threshold_value Idle age threshold in milliseconds.
 * @return Number of entries actually evicted.
 */
int ip_nat_filter_old_entry(unsigned int threshold_value)
{
	u16_t i, next;
	int filter_count = 0;
	struct nat_table *NEntry;
	u32_t now = sys_now();

	for (i = nat_entry_list; i != NON_INDEX; i = next) {
		NEntry = GET_NAT_ENTRY(i);
		next = NEntry->next;
		if (GET_NAT_TIME_ELAPSED(now, NEntry->ts) > threshold_value) {
#if LWIP_TCP
			if (NEntry->proto == IP_PROTO_TCP) {
				if ((NEntry->fin_ack1 == 1 && NEntry->fin_ack2) || NEntry->rst == 1 ||
					GET_NAT_TIME_ELAPSED(now, NEntry->ts) > ip_nat_tcp_max_timeout) {
					ip_nat_set_entry_idle(NEntry);
					filter_count++;
				}
			} else
#endif
			{
				ip_nat_set_entry_idle(NEntry);
				filter_count++;
			}
		}
	}

	return filter_count;
}

/**
 * @brief  Create or refresh a NAT session and return the assigned mapped port.
 * @param  proto IP protocol.
 * @param  src LAN-side source IP.
 * @param  sport LAN-side source port (network byte order).
 * @param  dest WAN-side destination IP.
 * @param  dport WAN-side destination port (network byte order).
 * @param  app_use Application hint passed through to the entry.
 * @return Assigned mport in network byte order, or 0 on failure.
 */
static u16_t ip_nat_add_entry(u8_t proto, u32_t src, u16_t sport, u32_t dest, u16_t dport, u32_t app_use)
{
	struct nat_table *NEntry;
	u16_t mport;

	if ((tcp_entry_count + udp_entry_count + icmp_entry_count) > filter_drop_threshold) {
		ip_nat_filter_old_entry(IP_NAT_MAX_TIMEOUT_MS_FILTER_DROP);
	}

	NEntry = ip_nat_entry_search(proto, src, sport, 0, 0, 0, 0, 0);
	if (NEntry) {
		NEntry->ts = sys_now();
		NEntry->dest = dest;
		NEntry->dport = dport;
		NEntry->app_use = app_use;
#if LWIP_TCP
		if (proto == IP_PROTO_TCP) {
			NEntry->fin_wait1 = 0;
			NEntry->fin_wait2 = 0;
			NEntry->fin_ack1 = 0;
			NEntry->fin_ack2 = 0;
			NEntry->syn_acked = 0;
			NEntry->rst = 0;
		}
#endif
		ip_nat_set_entry_idle(NEntry);
		ip_nat_insert_new_rule(NEntry);

		return NEntry->mport;
	}

	NEntry = GET_NAT_ENTRY(nat_entry_idle);
	if (NEntry) {
		/* ICMP uses the echo ID as the session key; no port rewrite is needed. */
#if LWIP_ICMP
		if (proto == IP_PROTO_ICMP) {
			mport = sport;
		} else
#endif
		{
			mport = ip_nat_allocate_port(proto, sport);
			if (mport == 0) {
				RTK_LOGE(NOTAG, "Port allocation failed for proto=%d\n", proto);
				return 0;
			}
		}

		LWIP_DEBUGF(IPNAT_DEBUG, ("ip_nat_add_entry: proto=%d, sport=0x%04X(%u), mport=0x%04X(%u)\n",
					proto, PP_NTOHS(sport), PP_NTOHS(sport), PP_NTOHS(mport), PP_NTOHS(mport)));

		NEntry->ts = sys_now();
		NEntry->src = src;
		NEntry->dest = dest;
		NEntry->sport = sport;
		NEntry->dport = dport;
		NEntry->mport = mport;
		NEntry->proto = proto;
		NEntry->fin_wait1 = 0;
		NEntry->fin_wait2 = 0;
		NEntry->fin_ack1 = 0;
		NEntry->fin_ack2 = 0;
		NEntry->syn_acked = 0;
		NEntry->rst = 0;
		NEntry->app_use = app_use;
		NEntry->pkt_count = 0;

		ip_nat_insert_new_rule(NEntry);

		LWIP_DEBUGF(IPNAT_DEBUG, ("NAT: %u.%u.%u.%u:%u -> %u.%u.%u.%u [mport=%u]\n",
					ip4_addr1_16((ip4_addr_t*)&src), ip4_addr2_16((ip4_addr_t*)&src),
					ip4_addr3_16((ip4_addr_t*)&src), ip4_addr4_16((ip4_addr_t*)&src),
					PP_NTOHS(sport),
					ip4_addr1_16((ip4_addr_t*)&dest), ip4_addr2_16((ip4_addr_t*)&dest),
					ip4_addr3_16((ip4_addr_t*)&dest), ip4_addr4_16((ip4_addr_t*)&dest),
					PP_NTOHS(mport)));

		return mport;
	}

	ip_nat_filter_old_entry(IP_NAT_MAX_TIMEOUT_MS_FILTER_DROP > 20000 ?
							 IP_NAT_MAX_TIMEOUT_MS_FILTER_DROP - 20000 : 0);
	return 0;
}

#if LWIP_TCP
/**
 * @brief Override the default TCP session idle timeout.
 * @param secs New timeout in seconds.
 */
void ip_nat_set_tcp_timeout(u32_t secs)
{
	ip_nat_tcp_max_timeout = secs * 1000;
}

/**
 * @brief Rewrite a TCP port field and update the TCP checksum.
 * @param tcphdr TCP header to modify.
 * @param dest 0 = rewrite source port, 1 = rewrite destination port.
 * @param newval New port value in network byte order.
 */
static void ip_nat_manipulate_port_tcp(struct tcp_hdr *tcphdr, u8_t dest, u16_t newval)
{
	u16_t local_csum = tcphdr->chksum;
	u16_t oldval;
	unsigned char *optr = (unsigned char *)&oldval;
	unsigned char *nptr = (unsigned char *)&newval;

	if (dest == 0) {
		oldval = tcphdr->src;
		tcphdr->src = newval;
	} else {
		oldval = tcphdr->dest;
		tcphdr->dest = newval;
	}

	/* Checksum update: subtract old field, add new field. */
	unsigned char *chksum = (unsigned char *)&local_csum;
	long x, old, new;
	x = chksum[0] * 256 + chksum[1];
	x = ~x & 0xFFFF;

	old = optr[0] * 256 + optr[1];
	x -= old & 0xffff;
	if (x <= 0) {
		x--;
		x &= 0xffff;
	}

	new = nptr[0] * 256 + nptr[1];
	x += new & 0xffff;
	if (x & 0x10000) {
		x++;
		x &= 0xffff;
	}

	x = ~x & 0xFFFF;
	chksum[0] = x / 256;
	chksum[1] = x & 0xff;
	tcphdr->chksum = local_csum;
}

/**
 * @brief Rewrite an IP address field referenced by the TCP header pseudo-header and update the TCP checksum.
 * @param tcphdr TCP header whose checksum is updated.
 * @param field Pointer to the IP address field (in the IP header) to rewrite.
 * @param newval New IP address value in network byte order.
 */
void ip_nat_manipulate_address_tcp(struct tcp_hdr *tcphdr, ip4_addr_p_t *field, u32_t newval)
{
	u16_t local_csum = tcphdr->chksum;
	u32_t local_target = field->addr;
	u32_t new_target = newval;
	unsigned char *optr = (unsigned char *)&local_target;
	unsigned char *nptr = (unsigned char *)&new_target;
	int olen = 4;
	int nlen = 4;

	unsigned char *chksum = (unsigned char *)&local_csum;
	long x, old, new;
	x = chksum[0] * 256 + chksum[1];
	x = ~x & 0xFFFF;
	while (olen) {
		old = optr[0] * 256 + optr[1];
		optr += 2;
		x -= old & 0xffff;
		if (x <= 0) {
			x--;
			x &= 0xffff;
		}
		olen -= 2;
	}
	while (nlen) {
		new = nptr[0] * 256 + nptr[1];
		nptr += 2;
		x += new & 0xffff;
		if (x & 0x10000) {
			x++;
			x &= 0xffff;
		}
		nlen -= 2;
	}
	x = ~x & 0xFFFF;
	chksum[0] = x / 256;
	chksum[1] = x & 0xff;
	tcphdr->chksum = local_csum;

}
#endif // LWIP_TCP


#if LWIP_UDP
/**
 * @brief Override the default UDP session idle timeout.
 * @param secs New timeout in seconds.
 */
void ip_nat_set_udp_timeout(u32_t secs)
{
	ip_nat_udp_max_timeout = secs * 1000;
}

/**
 * @brief Rewrite a UDP port field and update the UDP checksum.
 * @param udphdr UDP header to modify.
 * @param dest 0 = rewrite source port, 1 = rewrite destination port.
 * @param newval New port value in network byte order.
 */
static void ip_nat_manipulate_port_udp(struct udp_hdr *udphdr, u8_t dest, u16_t newval)
{
	u16_t local_csum = udphdr->chksum;
	u16_t oldval;
	unsigned char *optr = (unsigned char *)&oldval;
	unsigned char *nptr = (unsigned char *)&newval;

	if (dest == 0) {
		oldval = udphdr->src;
		udphdr->src = newval;
	} else {
		oldval = udphdr->dest;
		udphdr->dest = newval;
	}

	/* Checksum update: subtract old field, add new field. */
	unsigned char *chksum = (unsigned char *)&local_csum;
	long x, old, new;
	x = chksum[0] * 256 + chksum[1];
	x = ~x & 0xFFFF;

	old = optr[0] * 256 + optr[1];
	x -= old & 0xffff;
	if (x <= 0) {
		x--;
		x &= 0xffff;
	}

	new = nptr[0] * 256 + nptr[1];
	x += new & 0xffff;
	if (x & 0x10000) {
		x++;
		x &= 0xffff;
	}

	x = ~x & 0xFFFF;
	chksum[0] = x / 256;
	chksum[1] = x & 0xff;
	udphdr->chksum = local_csum;
}

/**
 * @brief Rewrite an IP address field referenced by the UDP header pseudo-header and update the UDP checksum.
 * @param udphdr UDP header whose checksum is updated.
 * @param field Pointer to the IP address field (in the IP header) to rewrite.
 * @param newval New IP address value in network byte order.
 */
void ip_nat_manipulate_address_udp(struct udp_hdr *udphdr, ip4_addr_p_t *field, u32_t newval)
{
	u16_t local_csum = udphdr->chksum;
	u32_t local_target = field->addr;
	u32_t new_target = newval;
	unsigned char *optr = (unsigned char *)&local_target;
	unsigned char *nptr = (unsigned char *)&new_target;
	int olen = 4;
	int nlen = 4;

	unsigned char *chksum = (unsigned char *)&local_csum;
	long x, old, new;
	x = chksum[0] * 256 + chksum[1];
	x = ~x & 0xFFFF;
	while (olen) {
		old = optr[0] * 256 + optr[1];
		optr += 2;
		x -= old & 0xffff;
		if (x <= 0) {
			x--;
			x &= 0xffff;
		}
		olen -= 2;
	}
	while (nlen) {
		new = nptr[0] * 256 + nptr[1];
		nptr += 2;
		x += new & 0xffff;
		if (x & 0x10000) {
			x++;
			x &= 0xffff;
		}
		nlen -= 2;
	}
	x = ~x & 0xFFFF;
	chksum[0] = x / 256;
	chksum[1] = x & 0xff;
	udphdr->chksum = local_csum;

}
#endif // LWIP_UDP

/**
 * @brief Rewrite an IP address field in the IP header and update the IP header checksum.
 * @param iphdr IP header whose checksum is updated.
 * @param field Pointer to the src or dst address field within iphdr to rewrite.
 * @param newval New IP address value in network byte order.
 */
void ip_nat_manipulate_address(struct ip_hdr *iphdr, ip4_addr_p_t *field, u32_t newval)
{
	u16_t local_csum = IPH_CHKSUM(iphdr);
	u32_t local_target = field->addr;
	u32_t new_target = newval;
	unsigned char *optr = (unsigned char *)&local_target;
	unsigned char *nptr = (unsigned char *)&new_target;
	int olen = 4;
	int nlen = 4;
	unsigned char *chksum = (unsigned char *)&local_csum;
	long x, old, new;

	x = chksum[0] * 256 + chksum[1];
	x = ~x & 0xFFFF;
	while (olen) {
		old = optr[0] * 256 + optr[1];
		optr += 2;
		x -= old & 0xffff;
		if (x <= 0) {
			x--;
			x &= 0xffff;
		}
		olen -= 2;
	}
	while (nlen) {
		new = nptr[0] * 256 + nptr[1];
		nptr += 2;
		x += new & 0xffff;
		if (x & 0x10000) {
			x++;
			x &= 0xffff;
		}
		nlen -= 2;
	}
	x = ~x & 0xFFFF;
	chksum[0] = x / 256;
	chksum[1] = x & 0xff;

	IPH_CHKSUM(iphdr) = local_csum;
	field->addr = newval;
}

/**
 * @brief Process an inbound WAN packet destined for the router's WAN IP.
 * @param p Packet buffer (payload starts at IP header).
 * @param iphdr IP header pointer within p.
 */
static void ip_nat_rx_packet(struct pbuf *p, struct ip_hdr *iphdr)
{

	struct nat_table *NEntry;
	u16_t frag_offset;

#if LWIP_ICMP
	if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
		frag_offset = lwip_ntohs(iphdr->_offset) & IP_OFFMASK;
		struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
		if (frag_offset == 0 && iecho->type == ICMP_ER) {

			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
			NEntry = ip_nat_entry_search(IP_PROTO_ICMP, iphdr->src.addr, iecho->id, iecho->id, 1, 0, 0, 0);
			if (!NEntry) {
				rtos_mutex_give(nat_entry_lock);
				return;
			}
			ip_nat_manipulate_address(iphdr, &iphdr->dest, NEntry->src);
			rtos_mutex_give(nat_entry_lock);
			return;
		} else if ((IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {

			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
			NEntry = ip_nat_entry_search(IP_PROTO_ICMP, iphdr->src.addr, 0, 0, 1, 1, 0, 0);

			if (NEntry) {
				ip_nat_manipulate_address(iphdr, &iphdr->dest, NEntry->src);
			} else {
				//RTK_LOGI(NOTAG, "\n\r");
				//RTK_LOGI(NOTAG, "\n\r ICMP ip_nat_rx_packet NOT found!!!");
				//RTK_LOGI(NOTAG, "\n\r");
			}
			rtos_mutex_give(nat_entry_lock);
		}

		return;
	}
#endif // LWIP_ICMP

#if LWIP_TCP
	if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
		struct tcp_hdr *tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);

		/* RX TCP: search by (server_src_ip, server_src_port, mapped_dst_port) */
		rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
		NEntry = ip_nat_entry_search(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src, tcphdr->dest, 1, 0, 0, 0);
		if (!NEntry) {
			rtos_mutex_give(nat_entry_lock);
			return;
		}

		if (NEntry->rst == 1) {
			//RTK_LOGI(NOTAG, "\n\r CLEAN TCP RST!!!!");
			NEntry->rst = 0;
		}

		/* Restore destination port (mport -> sport) and destination IP (WAN IP -> LAN IP). */
		ip_nat_manipulate_port_tcp(tcphdr, 1, NEntry->sport);
		ip_nat_manipulate_address_tcp(tcphdr, &iphdr->dest, NEntry->src);
		ip_nat_manipulate_address(iphdr, &iphdr->dest, NEntry->src);

		if ((TCPH_FLAGS(tcphdr) & (TCP_SYN | TCP_ACK)) == (TCP_SYN | TCP_ACK)) {
			NEntry->syn_acked = 1;
		}

		if ((TCPH_FLAGS(tcphdr) & TCP_FIN)) {
			NEntry->fin_wait1 = 1;
		}

		if (NEntry->fin_wait2 && (TCPH_FLAGS(tcphdr) & TCP_ACK)) {
			NEntry->fin_ack2 = 1;
		}

		if (TCPH_FLAGS(tcphdr) & TCP_RST) {
			NEntry->rst = 1;
		} else {
			NEntry->rst = 0;
		}

		rtos_mutex_give(nat_entry_lock);
		return;
	}
#endif // LWIP_TCP

#if LWIP_UDP
	if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
		struct udp_hdr *udphdr = (struct udp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
		frag_offset = lwip_ntohs(iphdr->_offset) & IP_OFFMASK;

		if (frag_offset == 0) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

			NEntry = ip_nat_entry_search(IP_PROTO_UDP, iphdr->src.addr, udphdr->src, udphdr->dest, 1, 0, 0, 0);
			if (!NEntry) {
				rtos_mutex_give(nat_entry_lock);
				return;
			}

			/* Restore destination port (mport -> sport) and destination IP (WAN IP -> LAN IP). */
			ip_nat_manipulate_port_udp(udphdr, 1, NEntry->sport);
			ip_nat_manipulate_address_udp(udphdr, &iphdr->dest, NEntry->src);
			ip_nat_manipulate_address(iphdr, &iphdr->dest, NEntry->src);
			rtos_mutex_give(nat_entry_lock);

			return;
		} else if ((IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
			NEntry = ip_nat_entry_search(IP_PROTO_UDP, iphdr->src.addr, 0, 0, 1, 1, 0, 0);
			if (NEntry) {
				ip_nat_manipulate_address(iphdr, &iphdr->dest, NEntry->src);
			} else {
				//RTK_LOGI(NOTAG, "\n\r");
				//RTK_LOGI(NOTAG, "\n\r %d UDP ip_nat_rx_packet NOT found!!!",__LINE__);
				//RTK_LOGI(NOTAG, "\n\r");
			}
			rtos_mutex_give(nat_entry_lock);
		}
	}
#endif // LWIP_UDP
}

/**
 * @brief  Rewrite a LAN-originating packet before it leaves on the WAN interface.
 * @param  p             Packet buffer (payload starts at IP header).
 * @param  iphdr         IP header pointer within p.
 * @param  input_iface   LAN interface the packet arrived on (unused, reserved).
 * @param  output_iface  WAN interface the packet will be sent out on.
 * @return ERR_OK on success, ERR_RTE if the packet must be dropped.
 */
err_t ip_nat_forward_packet(struct pbuf *p, struct ip_hdr *iphdr, struct netif *input_iface, struct netif *output_iface)
{
	(void) input_iface;
	struct nat_table *NEntry;
	struct nat_table *NEntry1;
	u16_t frag_offset;

#if LWIP_ICMP
	if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
		frag_offset = lwip_ntohs(iphdr->_offset) & IP_OFFMASK;
		struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);

		if (frag_offset == 0 && iecho->type == ICMP_ECHO) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
			ip_nat_add_entry(IP_PROTO_ICMP, iphdr->src.addr, iecho->id, iphdr->dest.addr, iecho->id, 0);
			rtos_mutex_give(nat_entry_lock);

			ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
		} else if ((IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
			NEntry = ip_nat_entry_search(IP_PROTO_ICMP, iphdr->src.addr, 0, 0, 0, 1, iphdr->dest.addr, 0);

			if (NEntry) {
				ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
			} else {
				//RTK_LOGI(NOTAG, "\n\r");
				//RTK_LOGI(NOTAG, "\n\r ICMP ip_nat_forward_packet NOT found!!!");
				//RTK_LOGI(NOTAG, "\n\r");
			}
			rtos_mutex_give(nat_entry_lock);
		} else {
			//RTK_LOGI(NOTAG, "\n\r");
			//RTK_LOGI(NOTAG, "\n\r ICMP type =%d ip_nat_forward_packet DO NOTthing!!", iecho->type);
			//RTK_LOGI(NOTAG, "\n\r");
		}

		//ip4_debug_print(p);
		return ERR_OK;
	}
#endif

#if LWIP_TCP
	if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
		struct tcp_hdr *tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
		u16_t mport;

		if ((TCPH_FLAGS(tcphdr) & (TCP_SYN | TCP_ACK)) == TCP_SYN) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

			mport = ip_nat_add_entry(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src,
							 iphdr->dest.addr, tcphdr->dest, 0);
			if (mport == 0) {
				rtos_mutex_give(nat_entry_lock);
#if LWIP_ICMP
				icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
				return ERR_RTE;
			}

			rtos_mutex_give(nat_entry_lock);

			/* Rewrite source port (sport -> mport) and source IP (LAN IP -> WAN IP). */
			ip_nat_manipulate_port_tcp(tcphdr, 0, mport);  /* 0 = source port */
			ip_nat_manipulate_address_tcp(tcphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
			ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
		} else {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

			NEntry = ip_nat_entry_search(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src, 0, 0, 0, 0, 0);
			if (!NEntry) {
				NEntry = ip_nat_entry_search(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src, tcphdr->dest, 1, 0, 0, 0);
				if (NEntry) {
					//RTK_LOGI(NOTAG, "\n\r %s %d TCP NAT RX has update DNAT --Do Nothing %08X sport=%d ~~~%08X dport=%d",__FUNCTION__, __LINE__, iphdr->src.addr, lwip_ntohs(tcphdr->src), iphdr->dest.addr, lwip_ntohs(tcphdr->dest));
					rtos_mutex_give(nat_entry_lock);
				} else {
					NEntry1 = ip_nat_entry_search(IP_PROTO_TCP, iphdr->src.addr, tcphdr->src, 0, 0, 0, iphdr->dest.addr, tcphdr->dest);
					if (NEntry1) {
						ip_nat_set_entry_idle(NEntry1);
						NEntry1->src = iphdr->src.addr;
						NEntry1->sport = tcphdr->src;
						NEntry1->ts = sys_now();
						NEntry1->dest = iphdr->dest.addr;
						NEntry1->dport = tcphdr->dest;
						NEntry1->app_use = 0;
						//RTK_LOGI(NOTAG, "\n\r Update E %08X %d %08X %d", NEntry1->src,lwip_ntohs(NEntry1->sport), NEntry1->dest, lwip_ntohs(NEntry1->dport));
						ip_nat_insert_new_rule(NEntry1);
						//RTK_LOGI(NOTAG, "\n\r %d ~~%08X %d %08X %d~~", __LINE__, NEntry1->src, lwip_ntohs(NEntry1->sport), NEntry1->dest, lwip_ntohs(NEntry1->dport));
						//RTK_LOGI(NOTAG, "\n\r %d ~~%08X %d %08X %d~~", __LINE__, iphdr->src.addr, lwip_ntohs(tcphdr->src), iphdr->dest.addr, lwip_ntohs(tcphdr->dest));
						rtos_mutex_give(nat_entry_lock);

						ip_nat_manipulate_port_tcp(tcphdr, 0, NEntry1->mport);
						ip_nat_manipulate_address_tcp(tcphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
						ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
						return ERR_OK;
					}
					rtos_mutex_give(nat_entry_lock);
#if LWIP_ICMP
					icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
					return ERR_RTE; /* Drop unknown TCP session */
				}
			} else {
				if (NEntry->dest != iphdr->dest.addr || NEntry->dport != tcphdr->dest) {
					rtos_mutex_give(nat_entry_lock);
#if LWIP_ICMP
					icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
					return ERR_RTE;
				}

				if ((TCPH_FLAGS(tcphdr) & TCP_FIN)) {
					NEntry->fin_wait2 = 1;
				}

				if (NEntry->fin_wait1 && (TCPH_FLAGS(tcphdr) & TCP_ACK)) {
					NEntry->fin_ack1 = 1;
				}

				if (TCPH_FLAGS(tcphdr) & TCP_RST) {
					NEntry->rst = 1;
				} else {
					NEntry->rst = 0;
				}

				mport = NEntry->mport;
				rtos_mutex_give(nat_entry_lock);

				/* Rewrite source port (sport -> mport) and source IP (LAN IP -> WAN IP). */
				ip_nat_manipulate_port_tcp(tcphdr, 0, mport);
				ip_nat_manipulate_address_tcp(tcphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
				ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
			}
			return ERR_OK;
		}
	}
#endif

#if LWIP_UDP
	if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
		struct udp_hdr *udphdr = (struct udp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
		struct nat_table *NEntry1;
		u16_t mport;

		frag_offset = lwip_ntohs(iphdr->_offset) & IP_OFFMASK;
		if (frag_offset == 0) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

			NEntry = ip_nat_entry_search(IP_PROTO_UDP, iphdr->src.addr, udphdr->src, 0, 0, 0, 0, 0);
			if (!NEntry) {
				NEntry1 = ip_nat_entry_search(IP_PROTO_UDP, iphdr->src.addr, udphdr->src, udphdr->dest, 1, 0, 0, 0);
				if (NEntry1) {
					if (NEntry1->src == iphdr->dest.addr) {
						//RTK_LOGI(NOTAG, "\n\r %s %d NAT RX has update DNAT --Do Nothing ",__FUNCTION__, __LINE__);
					} else {
						RTK_LOGI(NOTAG, "\n\r %s %d NAT RX has NOT update forward update %X", __FUNCTION__, __LINE__, NEntry1->src);
					}
					rtos_mutex_give(nat_entry_lock);
				} else {
					mport = ip_nat_add_entry(IP_PROTO_UDP, iphdr->src.addr, udphdr->src,
									 iphdr->dest.addr, udphdr->dest, 0);
					if (mport == 0) {
						rtos_mutex_give(nat_entry_lock);
						return ERR_RTE;
					}

					rtos_mutex_give(nat_entry_lock);

					/* Rewrite source port (sport -> mport) and source IP (LAN IP -> WAN IP). */
					ip_nat_manipulate_port_udp(udphdr, 0, mport);
					ip_nat_manipulate_address_udp(udphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
					ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
				}
			} else {
				if (NEntry->dest != iphdr->dest.addr || NEntry->dport != udphdr->dest) {
					if (NEntry->src == iphdr->src.addr && NEntry->sport == udphdr->src) {
						ip_nat_set_entry_idle(NEntry);
						NEntry->src = iphdr->src.addr;
						NEntry->sport = udphdr->src;
						NEntry->ts = sys_now();
						NEntry->dest = iphdr->dest.addr;
						NEntry->dport = udphdr->dest;
						NEntry->app_use = 0;
						//RTK_LOGI(NOTAG, "\n\r %s %d GO INSERT NEntry prev=%d next=%d", __FUNCTION__, __LINE__, NEntry->prev, NEntry->next);
						ip_nat_insert_new_rule(NEntry);

					} else {
						rtos_mutex_give(nat_entry_lock);
#if LWIP_ICMP
						//RTK_LOGI(NOTAG, "\n\r%s %d icmp_dest_unreach", __FUNCTION__, __LINE__);
						icmp_dest_unreach(p, ICMP_DUR_PORT);
#endif
						return ERR_RTE;
					}
				}

				mport = NEntry->mport;
				rtos_mutex_give(nat_entry_lock);

				/* Rewrite source port (sport -> mport) and source IP (LAN IP -> WAN IP). */
				ip_nat_manipulate_port_udp(udphdr, 0, mport);
				ip_nat_manipulate_address_udp(udphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
				ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
			}
		} else if ((IPH_OFFSET(iphdr) & PP_HTONS(IP_OFFMASK | IP_MF)) != 0) {
			rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);

			NEntry = ip_nat_entry_search(IP_PROTO_UDP, iphdr->src.addr, 0, 0, 0, 1, iphdr->dest.addr, 0);
			if (NEntry) {
				ip_nat_manipulate_address(iphdr, &iphdr->src, ip_2_ip4(&(output_iface->ip_addr))->addr);
			} else {
				//RTK_LOGI(NOTAG, "\n\r");
				//RTK_LOGI(NOTAG, "\n\r UDP frag ip_nat_forward_packet NOT found!!!");
				//RTK_LOGI(NOTAG, "\n\r");
			}
			rtos_mutex_give(nat_entry_lock);
		} else {
		}
		return ERR_OK;
	}
#endif

	return ERR_OK;
}

/**
 * @brief  Called by the IP layer to NAT-translate a packet being forwarded from LAN (@p src) to WAN (@p target).
 * @param  p Packet buffer.
 * @param  src LAN (input) interface.
 * @param  target WAN (output) interface.
 * @return ERR_OK on success, ERR_RTE if the packet must be dropped.
 */
err_t ip_nat_transfer(struct pbuf *p, struct netif *src, struct netif *target)
{
	struct ip_hdr *iphdr;
	err_t result = ERR_OK;

	iphdr = (struct ip_hdr *)p->payload;

	if ((iphdr->dest.addr & PP_HTONL(0xf0000000UL)) == PP_HTONL(0xe0000000UL)) {
		//RTK_LOGI(NOTAG, "\n\r Skip Multicast ip address %08X", iphdr->dest.addr);
		return ERR_OK;
	}

	result = ip_nat_forward_packet(p, iphdr, src, target);

	return result;
}

/**
 * @brief  Called by the IP layer when a packet arrives on the WAN interface (@p inp).
 * @param  p Packet buffer.
 * @param  inp WAN (input) interface.
 * @return ERR_OK always (drops are handled silently by ip_nat_rx_packet).
 */
err_t ip_nat_enqueue(struct pbuf *p, struct netif *inp)
{
	struct ip_hdr *iphdr;

	iphdr = (struct ip_hdr *)p->payload;

	if ((iphdr->dest.addr & PP_HTONL(0xf0000000UL)) == PP_HTONL(0xe0000000UL)) {
		//RTK_LOGI(NOTAG, "\n\r Skip Multicast ip address %08X", iphdr->dest.addr);
		return ERR_OK;
	}

	if (ip4_addr_isbroadcast_u32(iphdr->dest.addr, inp)) {
		//RTK_LOGI(NOTAG, "\n\r Skip broadcast ip address %08X", iphdr->dest.addr);
		return ERR_OK;
	}
	if (ip4_addr_cmp(&iphdr->dest, ip_2_ip4(&(inp->ip_addr)))) {
		ip_nat_rx_packet(p, iphdr);
	}

	return ERR_OK;
}

/**
 * @brief Print a summary of active NAT sessions.
 */
void ip_nat_dump(void)
{
	int total_session = 0;

	if (!ip_nat_table) {
		RTK_LOGI(NOTAG, "NAT not initialized\n");
		return;
	}

	rtos_mutex_take(nat_entry_lock, MUTEX_WAIT_TIMEOUT);
	total_session = tcp_entry_count + udp_entry_count + icmp_entry_count;

	RTK_LOGI(NOTAG, "NAT session table:\n");
	RTK_LOGI(NOTAG, "======================================\n");
	RTK_LOGI(NOTAG, " %-15s  %-5s  %-5s  %-15s  %-5s  %-5s  %-5s  %s\n",
			 "src", "sport", "mport", "dst", "dport", "proto", "pkts", "age");
	RTK_LOGI(NOTAG, "------------------------------------------------------------------------\n");

	if (total_session > 0) {
		ip_nat_debug_print();
	}

	RTK_LOGI(NOTAG, "------------------------------------------------------------------------\n");
	RTK_LOGI(NOTAG, " Total: %d (TCP:%lu UDP:%lu ICMP:%lu)\n",
			 total_session, tcp_entry_count, udp_entry_count, icmp_entry_count);
	RTK_LOGI(NOTAG, "======================================\n");

	rtos_mutex_give(nat_entry_lock);
}

/**
 * @brief Dump all active NAT sessions in a formatted table.
 */
void ip_nat_debug_print(void)
{
	int i, next;
	u32_t now = sys_now();

	for (i = nat_entry_list; i != NON_INDEX; i = next) {
		struct nat_table *Entry = &ip_nat_table[i];
		next = Entry->next;

		RTK_LOGI(NOTAG, " %-15s  ", ipaddr_ntoa((ip_addr_t *)&Entry->src));
		RTK_LOGI(NOTAG, "%-5u  %-5u  ", PP_HTONS(Entry->sport), PP_HTONS(Entry->mport));
		RTK_LOGI(NOTAG, "%-15s  ", ipaddr_ntoa((ip_addr_t *)&Entry->dest));
		RTK_LOGI(NOTAG, "%-5u  %-5s  %-5u  %lus\n",
				 PP_HTONS(Entry->dport),
				 (Entry->proto == IP_PROTO_TCP)  ? "TCP"  :
				 (Entry->proto == IP_PROTO_UDP)  ? "UDP"  :
				 (Entry->proto == IP_PROTO_ICMP) ? "ICMP" : "?",
				 Entry->pkt_count,
				 (GET_NAT_TIME_ELAPSED(now, Entry->ts) / 1000));
	}
}

#endif /* IP_NAT */
#endif /* IP_FORWARD */
#endif /* LWIP_IPV4 */
