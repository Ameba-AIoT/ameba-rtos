// SPDX-License-Identifier: GPL-2.0-only
/* Realtek WHC log forwarding control (kernel side). */
#include <whc_host_linux.h>
#include <net/genetlink.h>
#include "whc_host_log_fwd.h"
#include "whc_host_cmd_path_api.h"

#define LOG_FWD_ACK_TIMEOUT_MS  500

static DEFINE_MUTEX(g_lock);
static DECLARE_COMPLETION(g_ack);
static bool g_enabled;      /* mirror of device-side state, guarded by g_lock */
static u8   g_pending_op;   /* op currently awaiting ACK; 0 = no one waiting */

void whc_host_log_forward_ack(u8 op)
{
	/* match op so a delayed ACK from a previously timed-out request cannot
	 * be mistaken for the ACK of a subsequent request with a different op */
	if (g_pending_op && op == g_pending_op) {
		complete(&g_ack);
	}
}

/* send command + wait for device ACK; caller must hold g_lock */
static int __log_fwd_send_locked(bool enable, bool update_state)
{
	u8 buf[5];
	unsigned long remain;
	u8 op = enable ? WHC_WIFI_TEST_LOG_ENABLE : WHC_WIFI_TEST_LOG_DISABLE;

	reinit_completion(&g_ack);
	g_pending_op = op;

	*(u32 *)buf = WHC_WIFI_TEST;
	buf[4] = op;
	whc_host_send_cmd_data_to_dev(buf, sizeof(buf), 0);

	remain = wait_for_completion_timeout(&g_ack,
										 msecs_to_jiffies(LOG_FWD_ACK_TIMEOUT_MS));
	g_pending_op = 0;

	if (!remain) {
		pr_warn("whc: log_fwd %s: no ACK in %d ms\n",
				enable ? "on" : "off", LOG_FWD_ACK_TIMEOUT_MS);
		return -ETIMEDOUT;
	}

	if (update_state) {
		g_enabled = enable;
	}
	return 0;
}

int whc_host_log_forward_set(bool enable)
{
	int ret;

	mutex_lock(&g_lock);
	if (g_enabled == enable) {
		mutex_unlock(&g_lock);
		return 0;
	}
	ret = __log_fwd_send_locked(enable, true);
	mutex_unlock(&g_lock);
	return ret;
}

int whc_host_log_forward_pause(void)
{
	int ret;

	mutex_lock(&g_lock);
	if (!g_enabled) {
		mutex_unlock(&g_lock);
		return 0;
	}
	ret = __log_fwd_send_locked(false, false);
	mutex_unlock(&g_lock);
	return ret;
}

int whc_host_log_forward_resume(void)
{
	int ret;

	mutex_lock(&g_lock);
	if (!g_enabled) {
		mutex_unlock(&g_lock);
		return 0;
	}
	ret = __log_fwd_send_locked(true, false);
	mutex_unlock(&g_lock);
	return ret;
}

int whc_host_nl_log_fwd(struct genl_info *info)
{
	u8 enable;

	if (!info->attrs[WHC_ATTR_LOG_ENABLE]) {
		return -EINVAL;
	}
	enable = *(u8 *)nla_data(info->attrs[WHC_ATTR_LOG_ENABLE]);
	return whc_host_log_forward_set(enable ? true : false);
}
