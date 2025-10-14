/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef LWIP_HDR_LWIPOPTS_H
#define LWIP_HDR_LWIPOPTS_H

#include "platform_autoconf.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "rand.h" //use soc _rand, not lib rand & srand, because stack size

#ifdef __cplusplus
extern "C" unsigned int sys_now(void);
#else
extern unsigned int sys_now(void);
#endif

/* ------------------------------------ Common options for all chips ------------------------------------ */
/* Core locking */
#define LWIP_TCPIP_CORE_LOCKING         1
/* Memory options */
#define MEM_LIBC_MALLOC                 1
#define MEMP_MEM_MALLOC                 1
#define MEM_ALIGNMENT                   4
/* Internal Memory Pool Sizes */
#define MEMP_NUM_RAW_PCB                16
#define MEMP_NUM_UDP_PCB                16
#define MEMP_NUM_TCP_PCB                16
#define MEMP_NUM_TCP_PCB_LISTEN         16
#define MEMP_NUM_NETCONN                20	//It is recommended not to make adjustments. If adjustments are necessary, avoid using the MQTT and WebSocket functionalities.
/* ARP options */
#define LWIP_ARP                        1
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
/* IP options */
#define LWIP_IPV4                       1
#define IP_REASS_MAX_PBUFS              10
/* ICMP options */
#define LWIP_ICMP                       1
/* RAW options */
#define LWIP_RAW                        1
/* DHCP options */
#define LWIP_DHCP                       1
/* AUTOIP options */
#define LWIP_AUTOIP                     1
/* IGMP options */
#define LWIP_IGMP                       1
/* DNS options */
#define LWIP_DNS                        1
/* UDP options */
#define LWIP_UDP                        1
/* TCP options */
#define LWIP_TCP                        1
#define TCP_MSS                         (1500 - 40) //TCP_MSS = (Ethernet MTU - IP header size - TCP header size)
#define TCP_WND                         (5 * TCP_MSS)
#define TCP_SND_BUF                     (5 * TCP_MSS)
#define TCP_SND_QUEUELEN                (4 * TCP_SND_BUF / TCP_MSS)
/* Pbuf options */
#if defined(CONFIG_PBUF_POOL_BUFSIZE)
#define PBUF_POOL_BUFSIZE               CONFIG_PBUF_POOL_BUFSIZE
#else
#define PBUF_POOL_BUFSIZE               508
#endif
/* Network Interfaces options */
#define LWIP_NETIF_API                  1
#define LWIP_NUM_NETIF_CLIENT_DATA      1
/* LOOPIF options */
#define LWIP_NETIF_LOOPBACK             1
/* Thread options */
#define TCPIP_THREAD_NAME               "TCP_IP"
#define TCPIP_THREAD_STACKSIZE          1000
#define TCPIP_THREAD_PRIO               (RTOS_TASK_MAX_PRIORITIES - 2)
#define TCPIP_MBOX_SIZE                 6
#define DEFAULT_RAW_RECVMBOX_SIZE       6
#define DEFAULT_UDP_RECVMBOX_SIZE       6
#define DEFAULT_TCP_RECVMBOX_SIZE       6
#define DEFAULT_ACCEPTMBOX_SIZE         6
/* Sequential layer options */
#define LWIP_NETCONN                    1
#define LWIP_TCPIP_TIMEOUT              1
/* Socket options */
#define LWIP_SOCKET                     1
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1
#define SO_REUSE                        1
/* Statistics options */
#define LWIP_STATS                      0
/* Other lwip options */
#define DHCP_COARSE_TIMER_SECS          60
#define LWIP_COMPAT_MUTEX               0
#define LWIP_ERRNO_STDINCLUDE
#define LWIP_RAND()                     _rand()
#define LWIP_SRAND()
#define LWIP_MDNS_RESPONDER             1
/* Realtek added options */
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS  1
#define LWIP_ICMP_SUPPRESS                  0
#define LWIP_ICMP_SUPPRESS_INTERVAL         900 //allow one icmp per second with tolerance of 100 ms
/* SNTP options */
#define SNTP_SERVER_DNS                   1
#define SNTP_MAX_SERVERS                  3
#define SNTP_SUPPRESS_DELAY_CHECK
#define SNTP_UPDATE_DELAY                 sntp_get_update_interval()
#define SNTP_SET_SYSTEM_TIME_US(sec, us)  sntp_set_system_time(sec, us)
#define SNTP_GET_SYSTEM_TIME(sec, us)     sntp_get_system_time(&(sec), &(us))

/* ------------------------------------ Options for different chips ------------------------------------ */


/* ------------------------------------ Options for different sdk configurations ------------------------------------ */
#if defined(CONFIG_LWIP_IPV6) && (CONFIG_LWIP_IPV6 == 1)
/* IPv6 options */
#define LWIP_IPV6                       1
#else
#define LWIP_IPV6                       0
#endif

#ifdef CONFIG_LWIP_DEBUG
#define LWIP_DEBUG                      1
#endif

#define WIFI_LOGO_CERTIFICATION_CONFIG      0 //for ping 10k test buffer setting
#if WIFI_LOGO_CERTIFICATION_CONFIG
#define ARP_MAXAGE                      720 //modify 5min to 12min, for wifi6 logo 5.64.1 AP not response arp request in step10
#define IP_REASS_MAXAGE                 1
#undef IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS              60
#endif

#if defined(CONFIG_IP_NAT) && (CONFIG_IP_NAT == 1)
#define IP_FORWARD                      1
#define IP_NAT                          1
#if defined(LWIP_IPV6) && (LWIP_IPV6 == 1)
#define CONFIG_IP6_RLOCAL 1
#endif
#endif

#if defined(CONFIG_IP6_RLOCAL) && (CONFIG_IP6_RLOCAL == 1)
#define LWIP_IPV6_FORWARD               1
#define LWIP_HOOK_IP6_ROUTE nd6_find_nhb_netif
#define is_sta_ap() (wifi_is_running(1)&&wifi_is_running(0))
#define is_sta_only() (wifi_is_running(0)&&(!wifi_is_running(1)))
#endif

#if defined(CONFIG_HIGH_TP_TEST)
#if defined (CONFIG_WHC_HOST)
#undef TCP_WND
#define TCP_WND                         (16 * TCP_MSS)
#undef TCP_SND_BUF
#define TCP_SND_BUF                     (24 * TCP_MSS)
#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE                 30
#undef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE       18
#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       18
#endif
#endif

#if defined(CONFIG_WPAN_THREAD_BORDER_ROUTER_EN) && CONFIG_WPAN_THREAD_BORDER_ROUTER_EN
#undef LWIP_IPV6
#define LWIP_IPV6                       1
#define LWIP_IPV6_FORWARD               1
#undef IP_FORWARD
#define IP_FORWARD                      1
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_IPV6_NUM_ADDRESSES         10
#define LWIP_ND6_NUM_PREFIXES           10
#define LWIP_ND6_NUM_ROUTERS            5
#define LWIP_MULTICAST_PING             1
#define LWIP_HOOK_ND6_GET_GW            rtk_otbr_lwip_hook_nd6_get_gw
#define LWIP_HOOK_IP6_ROUTE             rtk_otbr_lwip_hook_ip6_route
#define LWIP_HOOK_IP6_INPUT             rtk_otbr_lwip_hook_ip6_input
#define LWIP_HOOK_IP4_INPUT             rtk_otbr_lwip_hook_ip4_input
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define LWIP_HOOK_FILENAME              "rtk_otbr_lwip_hook.h"
#endif

#if defined(CONFIG_LWIP_NETCONN_SEM_PER_THREAD) && CONFIG_LWIP_NETCONN_SEM_PER_THREAD
#define LWIP_NETCONN_SEM_PER_THREAD       1
#define LWIP_NETCONN_THREAD_SEM_GET()     sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC()   sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE()    sys_thread_sem_deinit()
#endif

#endif /* LWIP_HDR_LWIPOPTS_H */