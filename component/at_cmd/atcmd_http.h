/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_HTTP_H__
#define __ATCMD_HTTP_H__


#ifdef CONFIG_LWIP_LAYER

#define HTTP_OVER_TCP                   (1)
#define HTTP_OVER_TLS_NO_VERIFY         (2)
#define HTTP_OVER_TLS_VERIFY_SERVER     (3)
#define HTTP_OVER_TLS_VERIFY_CLIENT     (4)
#define HTTP_OVER_TLS_VERIFY_BOTH       (5)


#define HTTP_READ_RESPONSE_DATA         (4000+1)


#define HTTP_GLOBAL_REQ_HEADER_NUM      (10)

void print_http_at(void);
void at_http_init(void);

#endif /* CONFIG_LWIP_LAYER */


#endif /*#ifndef __ATCMD_HTTP_H__*/