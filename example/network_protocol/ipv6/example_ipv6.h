#ifndef __EXAMPLE_IPV6_H__
#define __EXAMPLE_IPV6_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

#define EXAMPLE_IPV6_UDP    0
#define EXAMPLE_IPV6_TCP    1
#define EXAMPLE_IPV6_MCAST  0

#if EXAMPLE_IPV6_UDP
#define UDP_SERVER          1
#elif EXAMPLE_IPV6_TCP
#define TCP_SERVER          1
#elif EXAMPLE_IPV6_MCAST
#define MCAST_SERVER        1
#endif

#define MAX_RECV_SIZE    1500
#define MAX_SEND_SIZE    256
#define UDP_SERVER_PORT  5002
#define TCP_SERVER_PORT  5003
#define UDP_SERVER_IP    "fe80:0000:0000:0000:02e0:4cff:fef3:a546"
#define TCP_SERVER_IP    "fe80:0000:0000:0000:02e0:4cff:fef3:a546"

//MDNS
#define MCAST_GROUP_PORT 5353
#define MCAST_GROUP_IP   "ff02::fb"

#define IPV6_SEMA_TIMEOUT 15000

void example_ipv6(void);


#endif //#ifndef __EXAMPLE_IPV6_H__
