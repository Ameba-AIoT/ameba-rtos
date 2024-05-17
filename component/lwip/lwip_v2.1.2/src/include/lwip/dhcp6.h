/**
 * @file
 *
 * DHCPv6 client: IPv6 address autoconfiguration as per
 * RFC 3315 (stateful DHCPv6) and
 * RFC 3736 (stateless DHCPv6).
 */

/*
 * Copyright (c) 2018 Simon Goldschmidt
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
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 */

#ifndef LWIP_HDR_IP6_DHCP6_H
#define LWIP_HDR_IP6_DHCP6_H

#include "lwip/opt.h"

#if LWIP_IPV6_DHCP6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/err.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif
#if LWIP_IPV6_DHCP6_STATEFUL
/** period (in seconds) of the application calling dhcp6_lease_tmr() */
#define DHCP6_LEASE_TIMER_SECS  60
  
/** period (in milliseconds) of the application calling dhcp6_lease_tmr() */
#define DHCP6_LEASE_TIMER_MSECS (DHCP6_LEASE_TIMER_SECS * 1000UL)
#endif
/** period (in milliseconds) of the application calling dhcp6_tmr() */
#define DHCP6_TIMER_MSECS   500

struct dhcp6
{
  /** transaction identifier of last sent request */
  u32_t xid;
  /** track PCB allocation state */
  u8_t pcb_allocated;
  /** current DHCPv6 state machine state */
  u8_t state;
  /** retries of current request */
  u8_t tries;
  /** if request config is triggered while another action is active, this keeps track of it */
  u8_t request_config_pending;
  /** #ticks with period DHCP6_TIMER_MSECS for request timeout */
  u16_t request_timeout;
#if LWIP_IPV6_DHCP6_STATEFUL
  /* @todo: add more members here to keep track of stateful DHCPv6 data, like lease times */
  u8_t server_id_buf[150];
  u16_t server_id_len;
  u8_t ia_na_buf[100];
  u16_t ia_na_len;
  
  u16_t t1_timeout;  /* #ticks with period DHCP6_LEASE_TIMER_SECS for renewal time */
  u16_t t2_timeout;  /* #ticks with period DHCP6_LEASE_TIMER_SECS for rebind time */
  u16_t t1_renew_time;  /* #ticks with period DHCP6_LEASE_TIMER_SECS until next renew try */
  u16_t t2_rebind_time; /* #ticks with period DHCP6_LEASE_TIMER_SECS until next rebind try */
  u16_t lease_used; /* #ticks with period DHCP6_LEASE_TIMER_SECS since last received DHCPV6 ack */
  u16_t t0_timeout; /* #ticks with period DHCP6_LEASE_TIMER_SECS for lease time */

  ip6_addr_t offered_ip6_addr;
  
  u32_t offered_t0_lease; /* lease period (in seconds) */
  u32_t offered_t1_renew; /* recommended renew time (usually 50% of lease period) */
  u32_t offered_t2_rebind; /* recommended rebind time (usually 87.5 of lease period)  */

#endif /* LWIP_IPV6_DHCP6_STATEFUL */
};

void dhcp6_set_struct(struct netif *netif, struct dhcp6 *dhcp6);
/** Remove a struct dhcp6 previously set to the netif using dhcp6_set_struct() */
#define dhcp6_remove_struct(netif) netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL)
void dhcp6_cleanup(struct netif *netif);

//err_t dhcp6_enable_stateful(struct netif *netif);
//err_t dhcp6_enable_stateless(struct netif *netif);
err_t dhcp6_enable(struct netif *netif);
void dhcp6_disable(struct netif *netif);
void dhcp6_stop(struct netif *netif);
void dhcp6_tmr(void);
void dhcp6_network_changed(struct netif *netif);
void dhcp6_lease_tmr(void);
err_t dhcp6_release(struct netif *netif);  
void dhcp6_nd6_ra_trigger(struct netif *netif, u8_t managed_addr_config, u8_t other_config);

#if LWIP_DHCP6_GET_NTP_SRV
/** This function must exist, in other to add offered NTP servers to
 * the NTP (or SNTP) engine.
 * See LWIP_DHCP6_MAX_NTP_SERVERS */
extern void dhcp6_set_ntp_servers(u8_t num_ntp_servers, const ip_addr_t* ntp_server_addrs);
#endif /* LWIP_DHCP6_GET_NTP_SRV */

#define netif_dhcp6_data(netif) ((struct dhcp6*)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6))

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV6_DHCP6 */

#endif /* LWIP_HDR_IP6_DHCP6_H */
