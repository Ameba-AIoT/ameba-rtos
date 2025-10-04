/**
 * @file
 *
 * @defgroup dhcp6 DHCPv6
 * @ingroup ip6
 * DHCPv6 client: IPv6 address autoconfiguration as per
 * RFC 3315 (stateful DHCPv6) and
 * RFC 3736 (stateless DHCPv6).
 *
 * For now, only stateless DHCPv6 is implemented!
 *
 * TODO:
 * - enable/disable API to not always start when RA is received
 * - stateful DHCPv6 (for now, only stateless DHCPv6 for DNS and NTP servers works)
 * - create Client Identifier?
 * - only start requests if a valid local address is available on the netif
 * - only start information requests if required (not for every RA)
 *
 * dhcp6_enable_stateful() enables stateful DHCPv6 for a netif (stateless disabled)\n
 * dhcp6_enable_stateless() enables stateless DHCPv6 for a netif (stateful disabled)\n
 * dhcp6_disable() disable DHCPv6 for a netif
 *
 * When enabled, requests are only issued after receipt of RA with the
 * corresponding bits set.
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

#include "lwip/opt.h"

#if LWIP_IPV6 && LWIP_IPV6_DHCP6 /* don't build if not configured for use in lwipopts.h */

#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp6.h"
#include "lwip/def.h"
#include "lwip/udp.h"
#include "lwip/dns.h"

#include <string.h>

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif
#ifndef LWIP_HOOK_DHCP6_APPEND_OPTIONS
#define LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, state, msg, msg_type, options_len_ptr, max_len)
#endif
#ifndef LWIP_HOOK_DHCP6_PARSE_OPTION
#define LWIP_HOOK_DHCP6_PARSE_OPTION(netif, dhcp6, state, msg, msg_type, option, len, pbuf, offset) do { LWIP_UNUSED_ARG(msg); } while(0)
#endif

#if LWIP_IPV4 && LWIP_IPV6
#if LWIP_DNS && LWIP_DHCP6_MAX_DNS_SERVERS
#if DNS_IPV4_IPV6_MAX_SERVERS > LWIP_DHCP6_MAX_DNS_SERVERS
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS LWIP_DHCP6_MAX_DNS_SERVERS
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS DNS_IPV4_IPV6_MAX_SERVERS
#endif
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS 0
#endif
#else
#if LWIP_DNS && LWIP_DHCP6_MAX_DNS_SERVERS
#if DNS_MAX_SERVERS > LWIP_DHCP6_MAX_DNS_SERVERS
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS LWIP_DHCP6_MAX_DNS_SERVERS
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS DNS_MAX_SERVERS
#endif
#else
#define LWIP_DHCP6_PROVIDE_DNS_SERVERS 0
#endif
#endif

/** Option handling: options are parsed in dhcp6_parse_reply
 * and saved in an array where other functions can load them from.
 * This might be moved into the struct dhcp6 (not necessarily since
 * lwIP is single-threaded and the array is only used while in recv
 * callback). */
enum dhcp6_option_idx {
  DHCP6_OPTION_IDX_CLI_ID = 0,
  DHCP6_OPTION_IDX_SERVER_ID,
  DHCP6_OPTION_IDX_IA_NA,
#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  DHCP6_OPTION_IDX_DNS_SERVER,
  DHCP6_OPTION_IDX_DOMAIN_LIST,
#endif /* LWIP_DHCP_PROVIDE_DNS_SERVERS */
#if LWIP_DHCP6_GET_NTP_SRV
  DHCP6_OPTION_IDX_NTP_SERVER,
#endif /* LWIP_DHCP_GET_NTP_SRV */
  DHCP6_OPTION_IDX_MAX
};

struct dhcp6_option_info {
  u8_t option_given;
  u16_t val_start;
  u16_t val_length;
};

/** Holds the decoded option info, only valid while in dhcp6_recv. */
struct dhcp6_option_info dhcp6_rx_options[DHCP6_OPTION_IDX_MAX];

#define dhcp6_option_given(dhcp6, idx)           (dhcp6_rx_options[idx].option_given != 0)
#define dhcp6_got_option(dhcp6, idx)             (dhcp6_rx_options[idx].option_given = 1)
#define dhcp6_clear_option(dhcp6, idx)           (dhcp6_rx_options[idx].option_given = 0)
#define dhcp6_clear_all_options(dhcp6)           (memset(dhcp6_rx_options, 0, sizeof(dhcp6_rx_options)))
#define dhcp6_get_option_start(dhcp6, idx)       (dhcp6_rx_options[idx].val_start)
#define dhcp6_get_option_length(dhcp6, idx)      (dhcp6_rx_options[idx].val_length)
#define dhcp6_set_option(dhcp6, idx, start, len) do { dhcp6_rx_options[idx].val_start = (start); dhcp6_rx_options[idx].val_length = (len); }while(0)

extern int TRNG_get_random_bytes(void* dst, u32 size);
extern void *rtos_mem_malloc( size_t xWantedSize );
extern void nd6_restart_netif(struct netif *netif);

const ip_addr_t dhcp6_All_DHCP6_Relay_Agents_and_Servers = IPADDR6_INIT_HOST(0xFF020000, 0, 0, 0x00010002);
const ip_addr_t dhcp6_All_DHCP6_Servers = IPADDR6_INIT_HOST(0xFF020000, 0, 0, 0x00010003);

static struct udp_pcb *dhcp6_pcb;
static u8_t dhcp6_pcb_refcount;


/* receive, unfold, parse and free incoming messages */
static void dhcp6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

#if LWIP_IPV6_DHCP6_STATEFUL
static void dhcp6_stateful_solicit(struct netif *netif);
static void dhcp6_stateful_request(struct netif *netif);
static void dhcp6_stateful_bind(struct netif *netif);
static void dhcp6_stateful_renew(struct netif *netif);
static void dhcp6_stateful_rebind(struct netif *netif);
static void dhcp6_stateful_confirm(struct netif *netif);
static void dhcp6_stateful_release(struct netif *netif);
err_t dhcp6_release(struct netif *netif);
static void dhcp6_stateful_decline(struct netif *netif);
static void dhcp6_stateful_handle_advertise(struct netif *netif, struct pbuf *p_msg_in);
static void dhcp6_stateful_handle_reply(struct netif *netif, struct pbuf *p_msg_in);
static err_t dhcp6_stateful_config(struct netif *netif, struct dhcp6 *dhcp6);
static void dhcp6_stateful_abort_config(struct dhcp6 *dhcp6);
u8_t dhcp6_supplied_address(const struct netif *netif);
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

/** Ensure DHCP PCB is allocated and bound */
static err_t
dhcp6_inc_pcb_refcount(void)
{
  if (dhcp6_pcb_refcount == 0) {
    LWIP_ASSERT("dhcp6_inc_pcb_refcount(): memory leak", dhcp6_pcb == NULL);

    /* allocate UDP PCB */
    dhcp6_pcb = udp_new_ip6();

    if (dhcp6_pcb == NULL) {
      return ERR_MEM;
    }

    ip_set_option(dhcp6_pcb, SOF_BROADCAST);

    /* set up local and remote port for the pcb -> listen on all interfaces on all src/dest IPs */
    udp_bind(dhcp6_pcb, IP6_ADDR_ANY, DHCP6_CLIENT_PORT);
    udp_recv(dhcp6_pcb, dhcp6_recv, NULL);
  }

  dhcp6_pcb_refcount++;

  return ERR_OK;
}

/** Free DHCP PCB if the last netif stops using it */
static void
dhcp6_dec_pcb_refcount(void)
{
  LWIP_ASSERT("dhcp6_pcb_refcount(): refcount error", (dhcp6_pcb_refcount > 0));
  dhcp6_pcb_refcount--;

  if (dhcp6_pcb_refcount == 0) {
    udp_remove(dhcp6_pcb);
    dhcp6_pcb = NULL;
  }
}

/**
 * @ingroup dhcp6
 * Set a statically allocated struct dhcp6 to work with.
 * Using this prevents dhcp6_start to allocate it using mem_malloc.
 *
 * @param netif the netif for which to set the struct dhcp
 * @param dhcp6 (uninitialised) dhcp6 struct allocated by the application
 */
void
dhcp6_set_struct(struct netif *netif, struct dhcp6 *dhcp6)
{
  LWIP_ASSERT("netif != NULL", netif != NULL);
  LWIP_ASSERT("dhcp6 != NULL", dhcp6 != NULL);
  LWIP_ASSERT("netif already has a struct dhcp6 set", netif_dhcp6_data(netif) == NULL);

  /* clear data structure */
  memset(dhcp6, 0, sizeof(struct dhcp6));
  /* dhcp6_set_state(&dhcp, DHCP6_STATE_OFF); */
  netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, dhcp6);
}

/**
 * @ingroup dhcp6
 * Removes a struct dhcp6 from a netif.
 *
 * ATTENTION: Only use this when not using dhcp6_set_struct() to allocate the
 *            struct dhcp6 since the memory is passed back to the heap.
 *
 * @param netif the netif from which to remove the struct dhcp
 */
void dhcp6_cleanup(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", netif != NULL);

  if (netif_dhcp6_data(netif) != NULL) {
    mem_free(netif_dhcp6_data(netif));
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL);
  }
}

static struct dhcp6*
dhcp6_get_struct(struct netif *netif, const char *dbg_requester)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: mallocing new DHCPv6 client\n", dbg_requester));
    dhcp6 = (struct dhcp6 *)mem_malloc(sizeof(struct dhcp6));
    if (dhcp6 == NULL) {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: could not allocate dhcp6\n", dbg_requester));
      return NULL;
    }

    /* clear data structure, this implies DHCP6_STATE_OFF */
    memset(dhcp6, 0, sizeof(struct dhcp6));
    /* store this dhcp6 client in the netif */
    netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, dhcp6);
  } else {
    /* already has DHCP6 client attached */
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("%s: using existing DHCPv6 client\n", dbg_requester));
  }

  if (!dhcp6->pcb_allocated) {
    if (dhcp6_inc_pcb_refcount() != ERR_OK) { /* ensure DHCP6 PCB is allocated */
      mem_free(dhcp6);
      netif_set_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6, NULL);
      return NULL;
    }
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("%s: allocated dhcp6", dbg_requester));
    dhcp6->pcb_allocated = 1;
  }
  return dhcp6;
}

/*
 * Set the DHCPv6 state
 * If the state changed, reset the number of tries.
 */
static void
dhcp6_set_state(struct dhcp6 *dhcp6, u8_t new_state, const char *dbg_caller)
{
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("DHCPv6 state: %d -> %d (%s)\n",
    dhcp6->state, new_state, dbg_caller));
  if (new_state != dhcp6->state) {
    dhcp6->state = new_state;
    dhcp6->tries = 0;
    dhcp6->request_timeout = 0;
  }
}

//static int
//dhcp6_stateless_enabled(struct dhcp6 *dhcp6)
//{
//  if ((dhcp6->state == DHCP6_STATE_STATELESS_IDLE) ||
//      (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG)) {
//    return 1;
//  }
//  return 0;
//}

/*static int
dhcp6_stateful_enabled(struct dhcp6 *dhcp6)
{
  if (dhcp6->state == DHCP6_STATE_OFF) {
    return 0;
  }
  if (dhcp6_stateless_enabled(dhcp6)) {
    return 0;
  }
  return 1;
}*/

/**
 * @ingroup dhcp6
 * Enable stateful DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_MANAGED_ADDR_CONFIG flag set.
 *
 * A struct dhcp6 will be allocated for this netif if not
 * set via @ref dhcp6_set_struct before.
 *
 * @todo: stateful DHCPv6 not supported, yet
 */
//err_t
//dhcp6_enable_stateful(struct netif *netif)
//{
//  LWIP_UNUSED_ARG(netif);
//  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("stateful dhcp6 not implemented yet"));
//  return ERR_VAL;
//}

/**
 * @ingroup dhcp6
 * Enable stateless DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_OTHER_CONFIG flag set.
 *
 * A struct dhcp6 will be allocated for this netif if not
 * set via @ref dhcp6_set_struct before.
 */
//err_t
//dhcp6_enable_stateless(struct netif *netif)
//{
//  struct dhcp6 *dhcp6;
//
//  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_enable_stateless(netif=%p) %c%c%"U16_F"\n", (void *)netif, netif->name[0], netif->name[1], (u16_t)netif->num));
//
//  dhcp6 = dhcp6_get_struct(netif, "dhcp6_enable_stateless()");
//  if (dhcp6 == NULL) {
//    return ERR_MEM;
//  }
//  if (dhcp6_stateless_enabled(dhcp6)) {
//    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): stateless DHCPv6 already enabled"));
//    return ERR_OK;
//  } else if (dhcp6->state != DHCP6_STATE_OFF) {
//    /* stateful running */
//    /* @todo: stop stateful once it is implemented */
//    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): switching from stateful to stateless DHCPv6"));
//  }
//  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable_stateless(): stateless DHCPv6 enabled\n"));
//  dhcp6_set_state(dhcp6, DHCP6_STATE_STATELESS_IDLE, "dhcp6_enable_stateless");
//  return ERR_OK;
//}

static int
dhcp6_enabled(struct dhcp6 *dhcp6)
{
  if ((dhcp6->state == DHCP6_STATE_IDLE) ||
      (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG)||
      (dhcp6->state == DHCP6_STATE_STATEFUL_SOLICITING)||
      (dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING)||
      (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING)||
      (dhcp6->state == DHCP6_STATE_STATEFUL_REBINDING)) {
    return 1;
  }
  return 0;
}

err_t
dhcp6_enable(struct netif *netif)
{
  struct dhcp6 *dhcp6;

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_enable(netif=%p) %c%c%"U16_F"\n", (void *)netif, netif->name[0], netif->name[1], (u16_t)netif->num));

  dhcp6 = dhcp6_get_struct(netif, "dhcp6_enable()");
  if (dhcp6 == NULL) {
    return ERR_MEM;
  }

  if ((netif->rs_count < LWIP_ND6_MAX_MULTICAST_SOLICIT) && netif_is_up(netif) && netif_is_link_up(netif) &&
    !ip6_addr_isinvalid(netif_ip6_addr_state(netif, 0)) &&
    !ip6_addr_isduplicated(netif_ip6_addr_state(netif, 0))){
    nd6_restart_netif(netif);
  }

  if (dhcp6_enabled(dhcp6)) {
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable(): DHCPv6 already enabled"));
    return ERR_OK;
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_IDLE, "dhcp6_enable");
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_enable(): DHCPv6 enabled\n"));
  return ERR_OK;
}

/**
 * @ingroup dhcp6
 * Disable stateful or stateless DHCPv6 on this netif
 * Requests are sent on receipt of an RA message with the
 * ND6_RA_FLAG_OTHER_CONFIG flag set.
 */
void
dhcp6_disable(struct netif *netif)
{
  struct dhcp6 *dhcp6;

  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_disable(netif=%p) %c%c%"U16_F"\n", (void *)netif, netif->name[0], netif->name[1], (u16_t)netif->num));

  dhcp6 = netif_dhcp6_data(netif);
  if (dhcp6 != NULL) {
    if (dhcp6->state != DHCP6_STATE_OFF) {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_disable(): DHCPv6 disabled\n"));
      dhcp6_set_state(dhcp6, DHCP6_STATE_OFF, "dhcp6_disable");
      if (dhcp6->pcb_allocated != 0) {
        dhcp6_dec_pcb_refcount(); /* free DHCPv6 PCB if not needed any more */
        dhcp6->pcb_allocated = 0;
      }
    }
  }
}

/**
 * Create a DHCPv6 request, fill in common headers
 *
 * @param netif the netif under DHCPv6 control
 * @param dhcp6 dhcp6 control struct
 * @param message_type message type of the request
 * @param opt_len_alloc option length to allocate
 * @param options_out_len option length on exit
 * @return a pbuf for the message
 */
static struct pbuf *
dhcp6_create_msg(struct netif *netif, struct dhcp6 *dhcp6, u8_t message_type,
                 u16_t opt_len_alloc, u16_t *options_out_len)
{
  struct pbuf *p_out;
  struct dhcp6_msg *msg_out;

  LWIP_ERROR("dhcp6_create_msg: netif != NULL", (netif != NULL), return NULL;);
  LWIP_ERROR("dhcp6_create_msg: dhcp6 != NULL", (dhcp6 != NULL), return NULL;);
  p_out = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcp6_msg) + opt_len_alloc, PBUF_RAM);
  if (p_out == NULL) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("dhcp6_create_msg(): could not allocate pbuf\n"));
    return NULL;
  }
  LWIP_ASSERT("dhcp6_create_msg: check that first pbuf can hold struct dhcp6_msg",
              (p_out->len >= sizeof(struct dhcp6_msg) + opt_len_alloc));

  /* @todo: limit new xid for certain message types? */
  /* reuse transaction identifier in retransmissions */
  if (dhcp6->tries == 0) {
    dhcp6->xid = LWIP_RAND() & 0xFFFFFF;
  }

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE,
              ("transaction id xid(%"X32_F")\n", dhcp6->xid));

  msg_out = (struct dhcp6_msg *)p_out->payload;
  memset(msg_out, 0, sizeof(struct dhcp6_msg) + opt_len_alloc);

  msg_out->msgtype = message_type;
  msg_out->transaction_id[0] = (u8_t)(dhcp6->xid >> 16);
  msg_out->transaction_id[1] = (u8_t)(dhcp6->xid >> 8);
  msg_out->transaction_id[2] = (u8_t)dhcp6->xid;
  *options_out_len = 0;
  return p_out;
}

static u16_t
dhcp6_option_short(u16_t options_out_len, u8_t *options, u16_t value)
{
  options[options_out_len++] = (u8_t)((value & 0xff00U) >> 8);
  options[options_out_len++] = (u8_t) (value & 0x00ffU);
  return options_out_len;
}

static u16_t
dhcp6_option_long(u16_t options_out_len, u8_t *options, u32_t value)
{
  options[options_out_len++] = (u8_t)((value & 0xff000000UL) >> 24);
  options[options_out_len++] = (u8_t)((value & 0x00ff0000UL) >> 16);
  options[options_out_len++] = (u8_t)((value & 0x0000ff00UL) >> 8);
  options[options_out_len++] = (u8_t)(value & 0x000000ffUL);
  return options_out_len;
}

u16_t
dhcp6_get_short(const struct pbuf *buf, u16_t dataptr, u16_t offset)
{
  pbuf_copy_partial(buf, &dataptr, 2, offset);
  return dataptr;
}

u32_t
dhcp6_get_long(const struct pbuf *buf, u32_t dataptr, u16_t offset)
{
  pbuf_copy_partial(buf, &dataptr, 4, offset);
  return dataptr;
}

static u16_t
dhcp6_option_optionrequest(u16_t options_out_len, u8_t *options, const u16_t *req_options,
                           u16_t num_req_options, u16_t max_len)
{
  size_t i;
  u16_t ret;

  LWIP_ASSERT("dhcp6_option_optionrequest: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + (2U * num_req_options) <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_ORO);
  ret = dhcp6_option_short(ret, options, 2 * num_req_options);
  for (i = 0; i < num_req_options; i++) {
    ret = dhcp6_option_short(ret, options, req_options[i]);
  }
  return ret;
}

static u16_t
dhcp6_option_clientid(u16_t options_out_len, u8_t *options, u16_t max_len, struct netif *netif, u16_t clientid_len)
{
  size_t i;
  u16_t ret;

  LWIP_ASSERT("dhcp6_option_clientid: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + clientid_len <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_CLIENTID);
  ret = dhcp6_option_short(ret, options, clientid_len);
  ret = dhcp6_option_short(ret, options, DHCP6_DUID_LL);
  ret = dhcp6_option_short(ret, options, DHCP6_HARDWARE_ETHERNET);
  for (i = 0; i < netif->hwaddr_len; i++) {
    options[ret++] = netif->hwaddr[i];
  }

  return ret;
}

static u16_t
dhcp6_option_ia_na(u16_t options_out_len, u8_t *options, u16_t max_len, u16_t ia_na_len)
{
  //size_t i;
  u16_t ret;
  static u32_t iaid;
  u32_t t1 = 0;
  u32_t t2 = 0;

  LWIP_ASSERT("dhcp6_option_ia_na: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + ia_na_len <= max_len);
  LWIP_UNUSED_ARG(max_len);

  iaid = LWIP_RAND() & 0xFFFFFFFF;

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_IA_NA);
  ret = dhcp6_option_short(ret, options, ia_na_len);
  ret = dhcp6_option_long(ret, options, iaid);
  ret = dhcp6_option_long(ret, options, t1);
  ret = dhcp6_option_long(ret, options, t2);

  return ret;
}

static u16_t
dhcp6_option_elapsed_time(u16_t options_out_len, u8_t *options, u16_t max_len, u16_t elapsed_time_len)
{
  //size_t i;
  u16_t ret;
  u16_t elapsed_time = 0;

  LWIP_ASSERT("dhcp6_option_elapsed_time: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + elapsed_time_len <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_ELAPSED_TIME);
  ret = dhcp6_option_short(ret, options, elapsed_time_len);
  ret = dhcp6_option_short(ret, options, elapsed_time);

  return ret;
}

static u16_t
dhcp6_option_serverid(u16_t options_out_len, u8_t *options, u16_t max_len, u8_t *serverid_buf, u16_t serverid_len)
{
  u16_t ret;

  LWIP_ASSERT("dhcp6_option_serverid: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + serverid_len <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_SERVERID);
  ret = dhcp6_option_short(ret, options, serverid_len);
  /* Copy option data got from Advertise message*/
  memcpy(&((u8_t *)options)[ret], serverid_buf, serverid_len);
  ret = ret + serverid_len;

  return ret;
}

static u16_t
dhcp6_option_ia_na_iaaddr(u16_t options_out_len, u8_t *options, u16_t max_len, u8_t *ia_na_buf, u16_t ia_na_ia_len)
{
  u16_t ret;

  LWIP_ASSERT("dhcp6_option_ia_na_iaaddr: options_out_len + sizeof(struct dhcp6_msg) + addlen <= max_len",
    sizeof(struct dhcp6_msg) + options_out_len + 4U + ia_na_ia_len <= max_len);
  LWIP_UNUSED_ARG(max_len);

  ret = dhcp6_option_short(options_out_len, options, DHCP6_OPTION_IA_NA);
  ret = dhcp6_option_short(ret, options, ia_na_ia_len);
  /* Copy option data got from Advertise message*/
  memcpy(&((u8_t *)options)[ret], ia_na_buf, ia_na_ia_len);
  ret = ret + ia_na_ia_len;

  return ret;
}

/* All options are added, shrink the pbuf to the required size */
static void
dhcp6_msg_finalize(u16_t options_out_len, struct pbuf *p_out)
{
  /* shrink the pbuf to the actual content length */
  pbuf_realloc(p_out, (u16_t)(sizeof(struct dhcp6_msg) + options_out_len));
}


#if LWIP_IPV6_DHCP6_STATELESS
static void
dhcp6_information_request(struct netif *netif, struct dhcp6 *dhcp6)
{
  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t opt_len_alloc = 4 + optionrequest_len + 4 + elapsed_time_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_information_request()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_INFOREQUEST, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_information_request: making request\n"));

    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_REQUESTING_CONFIG, msg_out,
      DHCP6_INFOREQUEST, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out);

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_information_request: INFOREQUESTING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_information_request: could not allocate DHCP6 request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_REQUESTING_CONFIG, "dhcp6_information_request");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_information_request(): set request timeout %"U16_F" msecs\n", msecs));
}

static err_t
dhcp6_request_config(struct netif *netif, struct dhcp6 *dhcp6)
{
  /* stateless mode enabled and no request running? */
  if (dhcp6->state == DHCP6_STATE_IDLE) {
    /* send Information-request and wait for answer; setup receive timeout */
    dhcp6_information_request(netif, dhcp6);
  }

  return ERR_OK;
}

static void
dhcp6_abort_config_request(struct dhcp6 *dhcp6)
{
  if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
    /* abort running request */
    dhcp6_set_state(dhcp6, DHCP6_STATE_IDLE, "dhcp6_abort_config_request");
  }
}

/* Handle a REPLY to INFOREQUEST
 * This parses DNS and NTP server addresses from the reply.
 */
static void
dhcp6_handle_config_reply(struct netif *netif, struct pbuf *p_msg_in)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  LWIP_UNUSED_ARG(dhcp6);
  LWIP_UNUSED_ARG(p_msg_in);

#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER)) {
    ip_addr_t dns_addr;
    ip6_addr_t *dns_addr6;
    u16_t op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
    u16_t op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
    u16_t idx;
    u8_t n;

    memset(&dns_addr, 0, sizeof(dns_addr));
    dns_addr6 = ip_2_ip6(&dns_addr);
#if LWIP_IPV4 && LWIP_IPV6
    for (n = DNS_MAX_SERVERS, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_PROVIDE_DNS_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
#else
    for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_PROVIDE_DNS_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
#endif
      u16_t copied = pbuf_copy_partial(p_msg_in, dns_addr6, sizeof(struct ip6_addr_packed), idx);
      if (copied != sizeof(struct ip6_addr_packed)) {
        /* pbuf length mismatch */
        return;
      }
      ip6_addr_assign_zone(dns_addr6, IP6_UNKNOWN, netif);
      /* @todo: do we need a different offset than DHCP(v4)? */
      dns_setserver(n, &dns_addr);
    }
  }
  /* @ todo: parse and set Domain Search List */
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */

#if LWIP_DHCP6_GET_NTP_SRV
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER)) {
    ip_addr_t ntp_server_addrs[LWIP_DHCP6_MAX_NTP_SERVERS];
    u16_t op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
    u16_t op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
    u16_t idx;
    u8_t n;

    for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_MAX_NTP_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
      u16_t copied;
      ip6_addr_t *ntp_addr6 = ip_2_ip6(&ntp_server_addrs[n]);
      ip_addr_set_zero_ip6(&ntp_server_addrs[n]);
      copied = pbuf_copy_partial(p_msg_in, ntp_addr6, sizeof(struct ip6_addr_packed), idx);
      if (copied != sizeof(struct ip6_addr_packed)) {
        /* pbuf length mismatch */
        return;
      }
      ip6_addr_assign_zone(ntp_addr6, IP6_UNKNOWN, netif);
    }
    dhcp6_set_ntp_servers(n, ntp_server_addrs);
  }
#endif /* LWIP_DHCP6_GET_NTP_SRV */
}
#endif /* LWIP_IPV6_DHCP6_STATELESS */

#if LWIP_IPV6_DHCP6_STATEFUL
static void
dhcp6_stateful_solicit(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len = 4 + netif->hwaddr_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t ia_na_len = 12;
  u16_t opt_len_alloc = 4 + clientid_len + 4 + optionrequest_len + 4 + elapsed_time_len + 4 + ia_na_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_solicit()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_SOLICIT, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_solicit: making solicit\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na(options_out_len, options, p_out->len, ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_SOLICITING, msg_out,
      DHCP6_SOLICIT, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out);

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_solicit: SOLICITING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_solicit: could not allocate DHCP6 solicit\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_SOLICITING, "dhcp6_stateful_solicit");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_solicit(): set solicit timeout %"U16_F" msecs\n", msecs));
}

static void
dhcp6_stateful_request(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t serverid_len = dhcp6->server_id_len;
  u16_t opt_len_alloc = 4 + clientid_len + 4 + optionrequest_len + 4 + elapsed_time_len + 4 + ia_na_ia_len + 4 + serverid_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_request()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_REQUEST, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_request: making request\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_serverid(options_out_len, options, p_out->len,
      dhcp6->server_id_buf, dhcp6->server_id_len);
    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
      dhcp6->ia_na_buf, dhcp6->ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_REQUESTING, msg_out,
      DHCP6_REQUEST, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out);

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_request: REQUESTING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_request: could not allocate DHCP6 request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_REQUESTING, "dhcp6_stateful_request");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_request(): set request timeout %"U16_F" msecs\n", msecs));
}

/**
 * Bind the interface to the offered IPv6 address.
 *
 * @param netif network interface to bind to the offered address
 */
static void
dhcp6_stateful_bind(struct netif *netif)
{
  u32_t timeout;
  s8_t i, free_idx;

  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  /* reset time used of lease */
  dhcp6->lease_used = 0;

  /* set lease period timer */
  if (dhcp6->offered_t0_lease != 0xffffffffUL) {
    timeout = (dhcp6->offered_t0_lease + DHCP6_LEASE_TIMER_SECS / 2) / DHCP6_LEASE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp6->t0_timeout = (u16_t)timeout;
    if (dhcp6->t0_timeout == 0) {
      dhcp6->t0_timeout = 1;
    }
  }
  /* set renewal period timer */
  if (dhcp6->offered_t1_renew != 0xffffffffUL) {
    timeout = (dhcp6->offered_t1_renew + DHCP6_LEASE_TIMER_SECS / 2) / DHCP6_LEASE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp6->t1_timeout = (u16_t)timeout;
    if (dhcp6->t1_timeout == 0) {
      dhcp6->t1_timeout = 1;
    }
    dhcp6->t1_renew_time = dhcp6->t1_timeout;
  }
  /* set rebind period timer */
  if (dhcp6->offered_t2_rebind != 0xffffffffUL) {
    timeout = (dhcp6->offered_t2_rebind + DHCP6_LEASE_TIMER_SECS / 2) / DHCP6_LEASE_TIMER_SECS;
    if (timeout > 0xffff) {
      timeout = 0xffff;
    }
    dhcp6->t2_timeout = (u16_t)timeout;
    if (dhcp6->t2_timeout == 0) {
      dhcp6->t2_timeout = 1;
    }
    dhcp6->t2_rebind_time = dhcp6->t2_timeout;
  }

  /* Find a free address slots */
  free_idx = 0;
  for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)))
      free_idx = i;
    if(free_idx)
      break;
  }
  if (free_idx == 0) {
    return; /* no address slots available, try again on next advertisement */
  }

  /* Assign the new address to the interface. */  
  ip_addr_copy_from_ip6(netif->ip6_addr[free_idx], dhcp6->offered_ip6_addr);
  netif_ip6_addr_set_valid_life(netif, free_idx, dhcp6->offered_t0_lease);
  netif_ip6_addr_set_pref_life(netif, free_idx, dhcp6->offered_t1_renew);
  netif_ip6_addr_set_state(netif, free_idx, IP6_ADDR_TENTATIVE);
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_BOUND, "dhcp6_stateful_bind()");
}

/**
 * @ingroup dhcp6
 * Renew an existing DHCPv6 lease at the involved DHCPv6 server.
 *
 * @param netif network interface which must renew its lease
 */
static void
dhcp6_stateful_renew(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t serverid_len = dhcp6->server_id_len;
  u16_t opt_len_alloc = 4 + clientid_len + 4 + optionrequest_len + 4 + elapsed_time_len + 4 + ia_na_ia_len + 4 + serverid_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_renew()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_RENEW, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_renew: making renew request\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_serverid(options_out_len, options, p_out->len, 
      dhcp6->server_id_buf, dhcp6->server_id_len);
    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
      dhcp6->ia_na_buf, dhcp6->ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_RENEWING, msg_out,
      DHCP6_RENEW, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out); 

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_renew: RENEWING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_renew: could not allocate DHCP6 renew request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_RENEWING, "dhcp6_stateful_renew");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)(dhcp6->tries < 10 ? dhcp6->tries * 2000 : 20 * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_renew(): set renew request timeout %"U16_F" msecs\n", msecs));
}

/**
 * Rebind with a DHCPv6 server for an existing DHCPv6 lease.
 *
 * @param netif network interface which must rebind with a DHCPv6 server
 */
static void
dhcp6_stateful_rebind(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
  
  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS, DHCP6_OPTION_CLIENTID};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t opt_len_alloc = 4 + clientid_len + 4 + optionrequest_len + 4 + elapsed_time_len + 4 + ia_na_ia_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_rebind()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_REBIND, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_rebind: making rebind request\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
      dhcp6->ia_na_buf, dhcp6->ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_REBINDING, msg_out,
      DHCP6_REBIND, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out); 

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_rebind: REBINDING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_rebind: could not allocate DHCP6 rebind request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_REBINDING, "dhcp6_stateful_rebind");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 10 ? dhcp6->tries * 1000 : 10 * 1000));
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_rebind(): set rebind request timeout %"U16_F" msecs\n", msecs));
}

/**
 * Confirm with a DHCPv6 server to determine whether the addresses it was assigned 
 * are still appropriate to the link to which the client is connected.
 *
 * @param netif network interface which must rebind with a DHCPv6 server
 */
static void
dhcp6_stateful_confirm(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  const u16_t requested_options[] = {DHCP6_OPTION_DNS_SERVERS, DHCP6_OPTION_DOMAIN_LIST, DHCP6_OPTION_SNTP_SERVERS};
  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t optionrequest_len = sizeof(requested_options);
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t opt_len_alloc = 4 + clientid_len + 4 + optionrequest_len + 4 + elapsed_time_len + 4 + ia_na_ia_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_confirm()\n"));
  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_CONFIRM, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_confirm: making confirm request\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_optionrequest(options_out_len, options, requested_options,
      LWIP_ARRAYSIZE(requested_options), p_out->len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
      dhcp6->ia_na_buf, dhcp6->ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_CONFIRMING, msg_out,
      DHCP6_CONFIRM, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out); 

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_confirm: CONFIRMING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_confirm: could not allocate DHCP6 confirm request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_CONFIRMING, "dhcp6_stateful_confirm");
  if (dhcp6->tries < 255) {
    dhcp6->tries++;
  }
  msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_confirm(): set confirm request timeout %"U16_F" msecs\n", msecs));
}

/**
 * @ingroup dhcp6
 * Release a DHCPv6 lease and stop DHCPv6 statemachine.
 *
 * @param netif network interface
 */
static void
dhcp6_stateful_release(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  if (dhcp6 == NULL) {
    return;
  }

//  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t serverid_len = dhcp6->server_id_len;
  u16_t opt_len_alloc = 4 + clientid_len +  4 + elapsed_time_len + 4 + ia_na_ia_len + 4 + serverid_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_release()\n"));

  /* already off? -> nothing to do */
  if (dhcp6->state == DHCP6_STATE_OFF) {
    return;
  }

  /* get index of the IPv6 address to release */
  s8_t idx = 0;
  for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
      if (ip6_addr_cmp(&dhcp6->offered_ip6_addr, netif_ip6_addr(netif, i))) {
        idx = i;
      }
    }
  }
  if (idx == 0) {
    return; /* no address slot is assigned by the releasing address*/
  }

  /* clean old DHCPv6 offer */
  ip6_addr_set_zero(&dhcp6->offered_ip6_addr);
  dhcp6->offered_t0_lease = dhcp6->offered_t1_renew = dhcp6->offered_t2_rebind = 0;
  dhcp6->t1_renew_time = dhcp6->t2_rebind_time = dhcp6->lease_used = dhcp6->t0_timeout = 0;

  /* send release message when current IP was assigned via DHCPv6 */
  if (dhcp6_supplied_address(netif)) { 
  /* create and initialize the DHCP message header */
    p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_RELEASE, opt_len_alloc, &options_out_len);
    if (p_out != NULL) {
      err_t err;
      struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
      u8_t *options = (u8_t *)(msg_out + 1);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_release: making release request\n"));

      options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
      options_out_len = dhcp6_option_serverid(options_out_len, options, p_out->len, 
        dhcp6->server_id_buf, dhcp6->server_id_len);
      options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
      options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
        dhcp6->ia_na_buf, dhcp6->ia_na_len);
      LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_RELEASING, msg_out,
        DHCP6_RELEASE, options_out_len, p_out->len);
      dhcp6_msg_finalize(options_out_len, p_out); 

      err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
      pbuf_free(p_out);
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_release: RELEASING -> %d\n", (int)err));
      LWIP_UNUSED_ARG(err);
    } else {
      LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_release: could not allocate DHCP6 release request\n"));
    }
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_RELEASING, "dhcp6_stateful_release");
  //if (dhcp6->tries < 255) {
  //  dhcp6->tries++;
  //}
  //msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  //dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  //LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_release(): set release request timeout %"U16_F" msecs\n", msecs));

  /* remove IP address from interface (prevents routing from selecting this interface) */
  netif_ip6_addr_set(netif, idx, IP6_ADDR_ANY6);
  netif_ip6_addr_set_valid_life(netif, idx, 0);
  netif_ip6_addr_set_pref_life(netif, idx, 0);
  netif_ip6_addr_set_state(netif, idx, IP6_ADDR_INVALID);

  dhcp6_set_state(dhcp6, DHCP6_STATE_OFF, "dhcp6_stateful_release");

  if (dhcp6->pcb_allocated != 0) {
    dhcp6_dec_pcb_refcount(); /* free DHCP PCB if not needed any more */
    dhcp6->pcb_allocated = 0;
  }
}


/**
 * @ingroup dhcp6
 * This function calls dhcp6_stateful_release() internally.
 * @deprecated Use dhcp6_stateful_release() instead.
 */

err_t
dhcp6_release(struct netif *netif)
{
  dhcp6_stateful_release(netif);
  return ERR_OK;
}

static void
dhcp6_stateful_decline(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  u16_t msecs;
  struct pbuf *p_out;
  u16_t options_out_len;
  u16_t clientid_len =  4 + netif->hwaddr_len;
  u16_t elapsed_time_len = 2;
  u16_t ia_na_ia_len = dhcp6->ia_na_len;
  u16_t serverid_len = dhcp6->server_id_len;
  u16_t opt_len_alloc = 4 + clientid_len +  4 + elapsed_time_len + 4 + ia_na_ia_len + 4 + serverid_len;

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_decline()\n"));

  /* get index of the IPv6 address to decline */
  s8_t idx = 0;
  for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
      if (ip6_addr_cmp(&dhcp6->offered_ip6_addr, netif_ip6_addr(netif, i))) {
        idx = i;
      }
    }
  }
  if (idx == 0) {
    return; /* no address slot is assigned by the declining address*/
  }

  /* create and initialize the DHCP message header */
  p_out = dhcp6_create_msg(netif, dhcp6, DHCP6_DECLINE, opt_len_alloc, &options_out_len);
  if (p_out != NULL) {
    err_t err;
    struct dhcp6_msg *msg_out = (struct dhcp6_msg *)p_out->payload;
    u8_t *options = (u8_t *)(msg_out + 1);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_stateful_decline: making decline request\n"));

    options_out_len = dhcp6_option_clientid(options_out_len, options, p_out->len, netif, clientid_len);
    options_out_len = dhcp6_option_serverid(options_out_len, options, p_out->len, 
      dhcp6->server_id_buf, dhcp6->server_id_len);
    options_out_len = dhcp6_option_elapsed_time(options_out_len, options, p_out->len, elapsed_time_len);
    options_out_len = dhcp6_option_ia_na_iaaddr(options_out_len, options, p_out->len,
      dhcp6->ia_na_buf, dhcp6->ia_na_len);
    LWIP_HOOK_DHCP6_APPEND_OPTIONS(netif, dhcp6, DHCP6_STATE_STATEFUL_RELEASING, msg_out,
      DHCP6_DECLINE, options_out_len, p_out->len);
    dhcp6_msg_finalize(options_out_len, p_out); 

    err = udp_sendto_if(dhcp6_pcb, p_out, &dhcp6_All_DHCP6_Relay_Agents_and_Servers, DHCP6_SERVER_PORT, netif);
    pbuf_free(p_out);
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_decline: DECLINING -> %d\n", (int)err));
    LWIP_UNUSED_ARG(err);
  } else {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS, ("dhcp6_stateful_decline: could not allocate DHCP6 decline request\n"));
  }
  dhcp6_set_state(dhcp6, DHCP6_STATE_STATEFUL_DECLINING, "dhcp6_stateful_decline");
  //if (dhcp6->tries < 255) {
  //  dhcp6->tries++;
  //}
  //msecs = (u16_t)((dhcp6->tries < 6 ? 1 << dhcp6->tries : 60) * 1000);
  //dhcp6->request_timeout = (u16_t)((msecs + DHCP6_TIMER_MSECS - 1) / DHCP6_TIMER_MSECS);
  //LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_stateful_decline(): set decline request timeout %"U16_F" msecs\n", msecs));

  /* remove IP address from interface (prevents routing from selecting this interface) */
  netif_ip6_addr_set(netif, idx, IP6_ADDR_ANY6);
  netif_ip6_addr_set_valid_life(netif, idx, 0);
  netif_ip6_addr_set_pref_life(netif, idx, 0);
  netif_ip6_addr_set_state(netif, idx, IP6_ADDR_INVALID);

  dhcp6_set_state(dhcp6, DHCP6_STATE_OFF, "dhcp6_stateful_decline");

  if (dhcp6->pcb_allocated != 0) {
    dhcp6_dec_pcb_refcount(); /* free DHCP PCB if not needed any more */
    dhcp6->pcb_allocated = 0;
  }
}

static void
dhcp6_stateful_handle_advertise(struct netif *netif, struct pbuf *p_msg_in)
{
  u16_t op_start;
  u16_t op_len;

  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  if ((dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_SERVER_ID))&&(dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_IA_NA))){
    /* Get option server id buffer*/
    op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
    op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
    dhcp6->server_id_len = pbuf_copy_partial(p_msg_in, dhcp6->server_id_buf, op_len, op_start);

    /* Get option IA_NA buffer*/
    op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_IA_NA);
    op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_IA_NA);
    if(op_len <= 12){
      /* IA Address option field is not contained */
      return;
    }
    dhcp6->ia_na_len = pbuf_copy_partial(p_msg_in, dhcp6->ia_na_buf, op_len, op_start);

    dhcp6_stateful_request(netif);
  }
}

/* Handle a REPLY
 * This parses IPv6 address, DNS and NTP server addresses from the reply.
 */
static void
dhcp6_stateful_handle_reply(struct netif *netif, struct pbuf *p_msg_in)
{
  u16_t op_start;
  u16_t op_len;
//  ip6_addr_t ip6addr;
  u16_t dataptr_short;
  u32_t dataptr_long;
  u32_t renew;
  u32_t rebind;
  u32_t valid_life;
  u32_t pref_life;
//  s8_t i, free_idx;

  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER)) {
    ip_addr_t dns_addr;
    ip6_addr_t *dns_addr6;
    u16_t op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
    u16_t op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
    u16_t idx;
    u8_t n;

    memset(&dns_addr, 0, sizeof(dns_addr));
    dns_addr6 = ip_2_ip6(&dns_addr);
#if LWIP_IPV4 && LWIP_IPV6
    for (n = DNS_MAX_SERVERS, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_PROVIDE_DNS_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
#else
    for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_PROVIDE_DNS_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
#endif
      u16_t copied = pbuf_copy_partial(p_msg_in, dns_addr6, sizeof(struct ip6_addr_packed), idx);
      if (copied != sizeof(struct ip6_addr_packed)) {
        /* pbuf length mismatch */
        return;
      }
      ip6_addr_assign_zone(dns_addr6, IP6_UNKNOWN, netif);
      /* @todo: do we need a different offset than DHCP(v4)? */
      dns_setserver(n, &dns_addr);
    }
  }
  /* @ todo: parse and set Domain Search List */
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */

#if LWIP_DHCP6_GET_NTP_SRV
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER)) {
    ip_addr_t ntp_server_addrs[LWIP_DHCP6_MAX_NTP_SERVERS];
    u16_t op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
    u16_t op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
    u16_t idx;
    u8_t n;

    for (n = 0, idx = op_start; (idx < op_start + op_len) && (n < LWIP_DHCP6_MAX_NTP_SERVERS);
         n++, idx += sizeof(struct ip6_addr_packed)) {
      u16_t copied;
      ip6_addr_t *ntp_addr6 = ip_2_ip6(&ntp_server_addrs[n]);
      ip_addr_set_zero_ip6(&ntp_server_addrs[n]);
      copied = pbuf_copy_partial(p_msg_in, ntp_addr6, sizeof(struct ip6_addr_packed), idx);
      if (copied != sizeof(struct ip6_addr_packed)) {
        /* pbuf length mismatch */
        return;
      }
      ip6_addr_assign_zone(ntp_addr6, IP6_UNKNOWN, netif);
    }
    dhcp6_set_ntp_servers(n, ntp_server_addrs);
  }
#endif /* LWIP_DHCP6_GET_NTP_SRV */
  /* Get option server id buffer*/
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_SERVER_ID)){
    op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
    op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
    dhcp6->server_id_len = pbuf_copy_partial(p_msg_in, dhcp6->server_id_buf, op_len, op_start);
  }

  /* Get IA address buffer and options*/
  if (dhcp6_option_given(dhcp6, DHCP6_OPTION_IDX_IA_NA)) {
    op_start = dhcp6_get_option_start(dhcp6, DHCP6_OPTION_IDX_IA_NA);
    op_len = dhcp6_get_option_length(dhcp6, DHCP6_OPTION_IDX_IA_NA);
    dhcp6->ia_na_len = pbuf_copy_partial(p_msg_in, dhcp6->ia_na_buf, op_len, op_start);

    if(op_len <= 12){
      /* IA Address option field is not contained */
      return;
    }

    if(lwip_htons(dhcp6_get_short(p_msg_in, dataptr_short, op_start + 12)) != DHCP6_OPTION_IAADDR){
      /* IA address option mismatch */
      return;
    }

    renew = lwip_htonl(dhcp6_get_long(p_msg_in, dataptr_long, op_start + 4));
    rebind = lwip_htonl(dhcp6_get_long(p_msg_in, dataptr_long, op_start + 8));
    pref_life = lwip_htonl(dhcp6_get_long(p_msg_in, dataptr_long, op_start + 32));
    valid_life = lwip_htonl(dhcp6_get_long(p_msg_in, dataptr_long, op_start + 36));

    /* RFC 8415 section 24.1:
     * If a client receives an IA_NA with T1 greater than T2 and both T1 and T2
     * are greater than 0, the client discards the IA_NA option and processes
     * the remainder of the message as though the server had not included the
     * invalid IA_NA option. */
    if((renew > rebind)&&(renew > 0)&&(rebind > 0)){
      return;
    }

    /* RFC 8415 section 26.1:
     * The client MUST discard any addresses for which the preferred lifetime
     * is greater than the valid lifetime. */
    if(pref_life > valid_life){
      return;
    }

    dhcp6->offered_t0_lease = valid_life;
    dhcp6->offered_t1_renew = renew;
    dhcp6->offered_t2_rebind = rebind;

    /* Get offered ipv6 address */
    dhcp6->offered_ip6_addr.addr[0] = dhcp6_get_long(p_msg_in, dataptr_long, op_start + 16);
    dhcp6->offered_ip6_addr.addr[1] = dhcp6_get_long(p_msg_in, dataptr_long, op_start + 20);
    dhcp6->offered_ip6_addr.addr[2] = dhcp6_get_long(p_msg_in, dataptr_long, op_start + 24);
    dhcp6->offered_ip6_addr.addr[3] = dhcp6_get_long(p_msg_in, dataptr_long, op_start + 28);

    dhcp6_stateful_bind(netif);
  }
}

static err_t
dhcp6_stateful_config(struct netif *netif, struct dhcp6 *dhcp6)
{
  /* stateful mode enabled and no solicit/request running? */
  if (dhcp6->state == DHCP6_STATE_IDLE) {
    /* send solicit and wait for answer; setup receive timeout */
    dhcp6_stateful_solicit(netif);
  }
  return ERR_OK;
}

static void
dhcp6_stateful_abort_config(struct dhcp6 *dhcp6)
{
  if ((dhcp6->state == DHCP6_STATE_STATEFUL_SOLICITING) || (dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING)
      || (dhcp6->state == DHCP6_STATE_STATEFUL_CONFIRMING)){
    /* abort running request */
    dhcp6_set_state(dhcp6, DHCP6_STATE_IDLE, "dhcp6_stateful_abort_config_request");
  }
}

/** Handle a possible change in the network configuration.
 *
 * This enters the REBOOTING state to verify that the currently bound
 * address is still valid.
 */
void
dhcp6_network_changed(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  if (!dhcp6) {
    return;
  }
  switch (dhcp6->state) {
    case DHCP6_STATE_STATEFUL_REBINDING:
    case DHCP6_STATE_STATEFUL_RENEWING:
    case DHCP6_STATE_STATEFUL_BOUND:
    case DHCP6_STATE_STATEFUL_CONFIRMING:
      dhcp6->tries = 0;
      dhcp6_stateful_confirm(netif);
      break;
    case DHCP6_STATE_OFF:
    case DHCP6_STATE_REQUESTING_CONFIG:
    case DHCP6_STATE_HANDLING_CONFIG:
      /* stay off */
      break;
    default:
      LWIP_ASSERT("invalid dhcp6->state", dhcp6->state <= DHCP6_STATE_STATEFUL_DECLINING);
      /* ILDE/REQUESTING: restart with new 'rid' because the state changes, 
       * SOLICITING: continue with current 'rid' as we stay in the same state */

      /* ensure we start with short timeouts, even if already discovering */
      dhcp6->tries = 0;
      dhcp6_stateful_solicit(netif);
      break;
  }
}

/**
 * The renewal period has timed out.
 *
 * @param netif the netif under DHCPv6 control
 */
static void
dhcp6_t1_timeout(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_STATE, ("dhcp6_t1_timeout()\n"));
  if ((dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING) || (dhcp6->state == DHCP6_STATE_STATEFUL_BOUND) ||
      (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING)) {
    /* just retry to renew - note that the rebind timer (t2) will
     * eventually time-out if renew tries fail. */
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_t1_timeout(): must renew\n"));
    /* This slightly different to RFC2131: DHCPREQUEST will be sent from state
       DHCP_STATE_RENEWING, not DHCP_STATE_BOUND */
    dhcp6_stateful_renew(netif);
    /* Calculate next timeout */
    if (((dhcp6->t2_timeout - dhcp6->lease_used) / 2) >= ((60 + DHCP6_LEASE_TIMER_SECS / 2) / DHCP6_LEASE_TIMER_SECS)) {
      dhcp6->t1_renew_time = (u16_t)((dhcp6->t2_timeout - dhcp6->lease_used) / 2);
    }
  }
}

/**
 * The rebind period has timed out.
 *
 * @param netif the netif under DHCPv6 control
 */
static void
dhcp6_t2_timeout(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_t2_timeout()\n"));
  if ((dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING) || (dhcp6->state == DHCP6_STATE_STATEFUL_BOUND) ||
      (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING) || (dhcp6->state == DHCP6_STATE_STATEFUL_REBINDING)) {
    /* just retry to rebind */
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_t2_timeout(): must rebind\n"));
    /* This slightly different to RFC2131: DHCPREQUEST will be sent from state
       DHCP_STATE_REBINDING, not DHCP_STATE_BOUND */
    dhcp6_stateful_rebind(netif);
    /* Calculate next timeout */
    if (((dhcp6->t0_timeout - dhcp6->lease_used) / 2) >= ((60 + DHCP6_LEASE_TIMER_SECS / 2) / DHCP6_LEASE_TIMER_SECS)) {
      dhcp6->t2_rebind_time = (u16_t)((dhcp6->t0_timeout - dhcp6->lease_used) / 2);
    }
  }
}

/**
 * The DHCPv6 timer that checks for lease renewal/rebind timeouts.
 * Must be called once a minute (see @ref DHCP6_LEASE_TIMER_SECS).
 */
void
dhcp6_lease_tmr(void)
{
  struct netif *netif;
  /* loop through netif's */
  NETIF_FOREACH(netif) {
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    /* only act on DHCPv6 configured interfaces */
    if ((dhcp6 != NULL) && (dhcp6->state != DHCP6_STATE_OFF)){
      /* compare lease time to expire timeout */
      if (dhcp6->t0_timeout && (++dhcp6->lease_used == dhcp6->t0_timeout)) {
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_lease_tmr(): t0 timeout\n"));
        /* this clients' lease time has expired */
        dhcp6_release(netif);
        dhcp6_enable(netif);
        dhcp6_stateful_solicit(netif);
        /* timer is active (non zero), and triggers (zeroes) now? */
      } else if (dhcp6->t2_rebind_time && (dhcp6->t2_rebind_time-- == 1)) {
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_lease_tmr(): t2 timeout\n"));
        /* this clients' rebind timeout triggered */
        dhcp6_t2_timeout(netif);
        /* timer is active (non zero), and triggers (zeroes) now */
      } else if (dhcp6->t1_renew_time && (dhcp6->t1_renew_time-- == 1)) {
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_lease_tmr(): t1 timeout\n"));
        /* this clients' renewal timeout triggered */
        dhcp6_t1_timeout(netif);
      }
    }
  }
}

/** check if DHCPv6 supplied netif->ip_addr
 *
 * @param netif the netif to check
 * @return 1 if DHCPv6 supplied netif->ip_addr (states BOUND or RENEWING),
 *         0 otherwise
 */
u8_t
dhcp6_supplied_address(const struct netif *netif)
{
  if ((netif != NULL) && (netif_dhcp6_data(netif) != NULL)) {
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    return (dhcp6->state == DHCP6_STATE_STATEFUL_BOUND) || (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING) ||
           (dhcp6->state == DHCP6_STATE_STATEFUL_REBINDING);
  }
  return 0;
}
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

/**
 * @ingroup dhcp6
 * This function calls dhcp6_stateful_release() internally.
 * @deprecated Use dhcp6_stateful_release() instead.
 */

void
dhcp6_stop(struct netif *netif)
{
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);

  if (dhcp6 == NULL) {
    return;
  }
  
#if LWIP_IPV6_DHCP6_STATEFUL  
  dhcp6_stateful_release(netif);
#endif

  for (int idx = 1; idx < LWIP_IPV6_NUM_ADDRESSES; idx++) {
    netif_ip6_addr_set(netif, idx, IP6_ADDR_ANY6);
    netif_ip6_addr_set_valid_life(netif, idx, 0);
    netif_ip6_addr_set_pref_life(netif, idx, 0);
    netif_ip6_addr_set_state(netif, idx, IP6_ADDR_INVALID);
  }
  
  if (dhcp6->state == DHCP6_STATE_OFF) {
    return;
  }
  
  dhcp6_set_state(dhcp6, DHCP6_STATE_OFF, "dhcp6_stateful_release");
}

/** This function is called from nd6 module when an RA messsage is received
 * It triggers DHCPv6 requests (if enabled).
 */
void
dhcp6_nd6_ra_trigger(struct netif *netif, u8_t managed_addr_config, u8_t other_config)
{
  struct dhcp6 *dhcp6;

  LWIP_ASSERT("netif != NULL", netif != NULL);
  dhcp6 = netif_dhcp6_data(netif);

  LWIP_UNUSED_ARG(managed_addr_config);
  LWIP_UNUSED_ARG(other_config);
  LWIP_UNUSED_ARG(dhcp6);

  if (dhcp6 != NULL) {
    if (dhcp6_enabled(dhcp6)) {
#if LWIP_IPV6_DHCP6_STATELESS
      if ((!managed_addr_config) && other_config) {
        dhcp6_request_config(netif, dhcp6);
      } else {
        dhcp6_abort_config_request(dhcp6);
      }
#endif /* LWIP_IPV6_DHCP6_STATELESS */

#if LWIP_IPV6_DHCP6_STATEFUL
      if (managed_addr_config && other_config) {
        dhcp6_stateful_config(netif, dhcp6);
      } else {
        dhcp6_stateful_abort_config(dhcp6);
      }
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
    }
  }
}

/**
 * Parse the DHCPv6 message and extract the DHCPv6 options.
 *
 * Extract the DHCPv6 options (offset + length) so that we can later easily
 * check for them or extract the contents.
 */
static err_t
dhcp6_parse_reply(struct pbuf *p, struct dhcp6 *dhcp6)
{
  u16_t offset;
  u16_t offset_max;
  u16_t options_idx;
  struct dhcp6_msg *msg_in;

  LWIP_UNUSED_ARG(dhcp6);

  /* clear received options */
  dhcp6_clear_all_options(dhcp6);
  msg_in = (struct dhcp6_msg *)p->payload;

  /* parse options */

  options_idx = sizeof(struct dhcp6_msg);
  /* parse options to the end of the received packet */
  offset_max = p->tot_len;

  offset = options_idx;
  /* at least 4 byte to read? */
  while ((offset + 4 <= offset_max)) {
    u8_t op_len_buf[4];
    u8_t *op_len;
    u16_t op;
    u16_t len;
    u16_t val_offset = (u16_t)(offset + 4);
    if (val_offset < offset) {
      /* overflow */
      return ERR_BUF;
    }
    /* copy option + length, might be split accross pbufs */
    op_len = (u8_t *)pbuf_get_contiguous(p, op_len_buf, 4, 4, offset);
    if (op_len == NULL) {
      /* failed to get option and length */
      return ERR_VAL;
    }
    op = (op_len[0] << 8) | op_len[1];
    len = (op_len[2] << 8) | op_len[3];
    offset = val_offset + len;
    if (offset < val_offset) {
      /* overflow */
      return ERR_BUF;
    }

    switch (op) {
      case (DHCP6_OPTION_CLIENTID):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_CLI_ID);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_CLI_ID, val_offset, len);
        break;
      case (DHCP6_OPTION_SERVERID):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_SERVER_ID);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_SERVER_ID, val_offset, len);
        break;
      case (DHCP6_OPTION_IA_NA):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_IA_NA);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_IA_NA, val_offset, len);
        break;
#if LWIP_DHCP6_PROVIDE_DNS_SERVERS
      case (DHCP6_OPTION_DNS_SERVERS):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_DNS_SERVER, val_offset, len);
        break;
      case (DHCP6_OPTION_DOMAIN_LIST):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_DOMAIN_LIST);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_DOMAIN_LIST, val_offset, len);
        break;
#endif /* LWIP_DHCP6_PROVIDE_DNS_SERVERS */
#if LWIP_DHCP6_GET_NTP_SRV
      case (DHCP6_OPTION_SNTP_SERVERS):
        dhcp6_got_option(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER);
        dhcp6_set_option(dhcp6, DHCP6_OPTION_IDX_NTP_SERVER, val_offset, len);
        break;
#endif /* LWIP_DHCP6_GET_NTP_SRV*/
      default:
        LWIP_DEBUGF(DHCP6_DEBUG, ("skipping option %"U16_F" in options\n", op));
        LWIP_HOOK_DHCP6_PARSE_OPTION(ip_current_netif(), dhcp6, dhcp6->state, msg_in,
          msg_in->msgtype, op, len, q, val_offset);
        break;
    }
  }
  return ERR_OK;
}

static void
dhcp6_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct netif *netif = ip_current_input_netif();
  struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
  struct dhcp6_msg *reply_msg = (struct dhcp6_msg *)p->payload;
  u8_t msg_type;
  u32_t xid;

  LWIP_UNUSED_ARG(arg);

  /* Caught DHCPv6 message from netif that does not have DHCPv6 enabled? -> not interested */
  if ((dhcp6 == NULL) || (dhcp6->pcb_allocated == 0)) {
    goto free_pbuf_and_return;
  }

  LWIP_ERROR("invalid server address type", IP_IS_V6(addr), goto free_pbuf_and_return;);

  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_recv(pbuf = %p) from DHCPv6 server %s port %"U16_F"\n", (void *)p,
    ipaddr_ntoa(addr), port));
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("pbuf->len = %"U16_F"\n", p->len));
  LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("pbuf->tot_len = %"U16_F"\n", p->tot_len));
  /* prevent warnings about unused arguments */
  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);
  LWIP_UNUSED_ARG(port);

  if (p->len < sizeof(struct dhcp6_msg)) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCPv6 reply message or pbuf too short\n"));
    goto free_pbuf_and_return;
  }

  /* match transaction ID against what we expected */
  xid = reply_msg->transaction_id[0] << 16;
  xid |= reply_msg->transaction_id[1] << 8;
  xid |= reply_msg->transaction_id[2];
  if (xid != dhcp6->xid) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING,
                ("transaction id mismatch reply_msg->xid(%"X32_F")!= dhcp6->xid(%"X32_F")\n", xid, dhcp6->xid));
    goto free_pbuf_and_return;
  }
  /* option fields could be unfold? */
  if (dhcp6_parse_reply(p, dhcp6) != ERR_OK) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                ("problem unfolding DHCPv6 message - too short on memory?\n"));
    goto free_pbuf_and_return;
  }

  /* read DHCP message type */
  msg_type = reply_msg->msgtype;

  /* message type is DHCP6 REPLY? */
  if (msg_type == DHCP6_REPLY) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DHCP6_REPLY received\n"));
#if LWIP_IPV6_DHCP6_STATELESS
    /* in info-requesting state? */
    if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
      dhcp6_handle_config_reply(netif, p);
      dhcp6_set_state(dhcp6, DHCP6_STATE_HANDLING_CONFIG, "dhcp6_recv");
    } else
#endif /* LWIP_IPV6_DHCP6_STATELESS */
    {
#if LWIP_IPV6_DHCP6_STATEFUL
      /* in stateful state? */
      if ((dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING) || (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING) 
          || (dhcp6->state == DHCP6_STATE_STATEFUL_REBINDING) || (dhcp6->state == DHCP6_STATE_STATEFUL_CONFIRMING)
          || (dhcp6->state == DHCP6_STATE_STATEFUL_RELEASING) || (dhcp6->state == DHCP6_STATE_STATEFUL_DECLINING)) {
        dhcp6_stateful_handle_reply(netif, p);
      }
#endif /* LWIP_IPV6_DHCP6_STATEFUL */
    }
  }

  /* message type is DHCP6 ADVERTISE? */
  if (msg_type == DHCP6_ADVERTISE) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("DHCP6_ADVERTISE received\n"));
    dhcp6_stateful_handle_advertise(netif, p);
  }

free_pbuf_and_return:
  pbuf_free(p);
}

/**
 * A DHCPv6 request has timed out.
 *
 * The timer that was started with the DHCPv6 request has
 * timed out, indicating no response was received in time.
 */
static void
dhcp6_timeout(struct netif *netif, struct dhcp6 *dhcp6)
{
  LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout()\n"));

  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(dhcp6);

#if LWIP_IPV6_DHCP6_STATELESS
  /* back-off period has passed, or server selection timed out */
  if (dhcp6->state == DHCP6_STATE_REQUESTING_CONFIG) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying information request\n"));
    dhcp6_information_request(netif, dhcp6);
  }
#endif /* LWIP_IPV6_DHCP6_STATELESS */
#if LWIP_IPV6_DHCP6_STATEFUL
  /* back-off period has passed, or server selection timed out */
  if (dhcp6->state == DHCP6_STATE_STATEFUL_SOLICITING){
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying solicit\n"));
    if (dhcp6->tries <= 5) {
      dhcp6_stateful_solicit(netif);
    } else {
      dhcp6_stateful_abort_config(dhcp6);
    }
  }
  else if (dhcp6->state == DHCP6_STATE_STATEFUL_REQUESTING){
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying request\n"));
    if (dhcp6->tries <= 5) {
      dhcp6_stateful_request(netif);
    } else {
      dhcp6_stateful_abort_config(dhcp6);
    }
  }
  else if (dhcp6->state == DHCP6_STATE_STATEFUL_RENEWING){
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying renew\n"));
    if (dhcp6->tries <= 5) {
      dhcp6_stateful_renew(netif);
    }
  }
  else if (dhcp6->state == DHCP6_STATE_STATEFUL_REBINDING){
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying rebind\n"));
    if (dhcp6->tries <= 5) {
      dhcp6_stateful_rebind(netif);
    }
  }
  else if (dhcp6->state == DHCP6_STATE_STATEFUL_CONFIRMING) {
    LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE, ("dhcp6_timeout(): retrying confirm\n"));
    if (dhcp6->tries <= 5) {
      dhcp6_stateful_confirm(netif);
    } else {
      dhcp6_stateful_abort_config(dhcp6);
    }
  }
#endif /* LWIP_IPV6_DHCP6_STATEFUL */

}

/**
 * DHCPv6 timeout handling (this function must be called every 500ms,
 * see @ref DHCP6_TIMER_MSECS).
 *
 * A DHCPv6 server is expected to respond within a short period of time.
 * This timer checks whether an outstanding DHCPv6 request is timed out.
 * This timer checks whether a local address is determined to be a duplicate.
 */
void
dhcp6_tmr(void)
{
 struct netif *netif;
  /* loop through netif's */
  NETIF_FOREACH(netif) { 
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    /* only act on DHCPv6 configured interfaces */
    if (dhcp6 != NULL) {
      /* timer is active (non zero), and is about to trigger now */
      if (dhcp6->request_timeout > 1) {
        dhcp6->request_timeout--;
      } else if (dhcp6->request_timeout == 1) {
        dhcp6->request_timeout--;
        /* { dhcp6->request_timeout == 0 } */
        LWIP_DEBUGF(DHCP6_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("dhcp6_tmr(): request timeout\n"));
        /* this client's request timeout triggered */
        dhcp6_timeout(netif, dhcp6);
      }
      /* If a local address has been determined to be a duplicate, then sends a  
       * Decline message to the server to inform it that the address is suspect */
      for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if(ip6_addr_isduplicated(netif_ip6_addr_state(netif, i))){
          dhcp6_stateful_decline(netif);
          dhcp6_enable(netif);
          dhcp6_stateful_solicit(netif);  
        }
      }
    }
  }
}

/* Realtek add */
#ifdef CONFIG_STANDARD_TICKLESS
/*
Check whether dhcp6_tmr can be removed before enter sleep
return 0: no, 1: yes
 */
u8_t check_dhcp6_tmr_removable(void)
{
  u8_t ret = 1;
  struct netif *netif;
  NETIF_FOREACH(netif) {
    struct dhcp6 *dhcp6 = netif_dhcp6_data(netif);
    if (dhcp6 != NULL) {
      if (dhcp->state != DHCP6_STATE_OFF && dhcp->state != DHCP6_STATE_HANDLING_CONFIG && dhcp->state != DHCP6_STATE_STATEFUL_BOUND) {
        ret = 0;
        goto exit;
      }
      for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if(ip6_addr_isduplicated(netif_ip6_addr_state(netif, i))) {
          ret = 0;
          goto exit;
        }
      }
    }
  }

exit:
  return ret;
}
#endif

#endif /* LWIP_IPV6 && LWIP_IPV6_DHCP6 */
