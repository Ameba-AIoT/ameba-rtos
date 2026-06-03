#ifndef _RTW_CLI_NETLINK_H_
#define _RTW_CLI_NETLINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <linux/genetlink.h>
#include <rtw_cli_wifi_types.h>

//Netlink
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>


#define GENLMSG_DATA(glh)       ((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh)    (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na)            ((void *)((char*)(na) + NLA_HDRLEN))
#define NLA_PAYLOAD(len)        (len - NLA_HDRLEN)

#define MAX_MSG_SIZE    2000

struct rtw_cli_netlink_global {
	struct nl_cb *nl_cb;
	struct nl_sock *nl;
	int family_id;
};

extern struct rtw_cli_netlink_global *global;


int rtw_cli_init_nl_global(struct rtw_cli_netlink_global *global);

void rtw_cli_deinit_nl_global(struct rtw_cli_netlink_global *global);

int rtw_cli_send_nl_string(uint8_t whc_cmd, uint8_t *buf,
						   uint32_t buf_len, uint8_t api_id,
						   int (*valid_handler)(struct nl_msg *, void *), void *valid_data);

int rtw_cli_send_nl_data(uint8_t whc_cmd, uint8_t *buf,
						 uint32_t buf_len, uint8_t api_id,
						 int (*valid_handler)(struct nl_msg *, void *), void *valid_data);

int send_and_recv_msgs(struct nl_sock *nl_handle, struct nl_msg *msg,
					   int (*valid_handler)(struct nl_msg *, void *), void *valid_data);

void rtw_cli_nl_recv(struct rtw_cli_netlink_global *global);

extern int direct_cli_cmd;

#endif // _RTW_CLI_NETLINK_H_
