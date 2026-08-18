// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek WHC log forwarding control (kernel side).
 */
#ifndef __WHC_HOST_LOG_FWD_H__
#define __WHC_HOST_LOG_FWD_H__

#include <linux/types.h>

/* User-visible on/off (logon/logoff via netlink). Blocks until device ACKs. */
int  whc_host_log_forward_set(bool enable);

/* netlink .doit for CMD_WIFI_LOG_FWD: reads one enable byte from
 * WHC_ATTR_LOG_ENABLE and calls whc_host_log_forward_set(). */
struct genl_info;
int  whc_host_nl_log_fwd(struct genl_info *info);

/* suspend/resume helpers: send DISABLE/ENABLE + wait for ACK without
 * changing the internal state, so user intent is preserved across suspend.
 * Both return 0 immediately if forwarding is currently off. */
int  whc_host_log_forward_pause(void);
int  whc_host_log_forward_resume(void);

/* Called by RX dispatcher when WHC_WIFI_TEST_LOG_ACK is received. */
void whc_host_log_forward_ack(u8 op);

#endif /* __WHC_HOST_LOG_FWD_H__ */
