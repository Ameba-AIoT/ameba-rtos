#ifndef __EXAMPLE_IPV6_H__
#define __EXAMPLE_IPV6_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <rtw_autoconf.h>

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
void example_ipv6_callback(char *buf, int buf_len, int flags, void *userdata);

#endif //#ifndef __EXAMPLE_IPV6_H__
