// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

struct whc_device global_idev;

extern struct aipc_ch_ops whc_ipc_host_recv_ops;
extern struct aipc_ch_ops whc_ipc_host_event_ops;
extern struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));
_WEAK void wifi_set_user_config(void);
static aipc_ch_t *whc_host_ipc_ch_init(int ch_id, size_t priv_size, struct aipc_ch_ops *ops)
{
	aipc_ch_t *ch = NULL;

	/* allocate the ipc channel */
	ch = ameba_ipc_alloc_ch(priv_size);
	if (!ch) {
		dev_err(global_idev.pwhc_dev, "alloc ch %d failed!!!", ch_id);
		return NULL;
	}

	/* initialize the ipc channel */
	ch->port_id = AIPC_PORT_NP;
	ch->ch_id = ch_id;
	ch->ch_config = AIPC_CONFIG_NOTHING;
	ch->ops = ops;
	ch->priv_data = NULL;

	/* regist the ipc channel */
	if (ameba_ipc_channel_register(ch) < 0) {
		dev_err(global_idev.pwhc_dev, "register ipc channel %d failed!!!", ch_id);
		goto free_ipc_ch;
	}

	return ch;

free_ipc_ch:
	kfree(ch);
	return NULL;
}

static void whc_host_ipc_ch_deinit(aipc_ch_t **ch)
{
	if (!*ch) {
		dev_err(global_idev.pwhc_dev, "ERROR: ipc ch has been deinit.");
		return;
	}

	/* unregist the channel */
	ameba_ipc_channel_unregister(*ch);

	kfree(*ch);
	*ch = NULL;
}

int whc_host_init(void)
{
	int ret = 0;
	struct whc_device *idev = &global_idev;
	int i;

	/* IPC channel of data and event init. */
	global_idev.data_ch = whc_host_ipc_ch_init(0, sizeof(struct whc_device *), &whc_ipc_host_recv_ops);
	global_idev.event_ch = whc_host_ipc_ch_init(1, sizeof(struct event_priv_t *), &whc_ipc_host_event_ops);
	if (global_idev.data_ch == NULL || global_idev.event_ch == NULL) {
		dev_err(global_idev.pwhc_dev, "IPC Allocate channel failed.");
		return -ENOMEM;
	}
	global_idev.ipc_dev = global_idev.data_ch->pdev;

	/* initialize the message queue, and assign the task haddle function */
	ret = whc_ipc_host_msg_q_init(global_idev.ipc_dev, whc_ipc_host_recv_task_hdl);
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "msg queue init fail.");
		goto ipc_deinit;
	}

	ret = whc_host_event_init(idev);
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "ipc host: init ipc host event_priv error(%d).\n", ret);
		goto ipc_deinit;
	}

	for (i = 0; i < WHC_MAX_NET_PORT_NUM; i++) {
		global_idev.is_need_4way[i] = 0;
		global_idev.is_4way_ongoing[i] = 0;
	}

	wifi_set_user_config();
	memcpy(&global_idev.wifi_user_config, &wifi_user_config, sizeof(struct wifi_user_conf));

	ret = whc_host_xmit_init();
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "malloc ipc xmit memory failed.(%d).\n", ret);
		goto ipc_deinit;
	}

	global_idev.host_init_done = 1;

	ret = whc_host_set_user_config(&global_idev.wifi_user_config);
	if (ret < 0) {
		dev_err(global_idev.pwhc_dev, "set wifi user config failed.(%d).\n", ret);
		goto ipc_deinit;
	}

	/* tell KM4 to open wifi */
	whc_host_wifi_on();

	return 0;

ipc_deinit:
	whc_host_ipc_ch_deinit(&global_idev.event_ch);
	whc_host_ipc_ch_deinit(&global_idev.data_ch);
	return ret;
}

void whc_host_deinit(void)
{
	whc_host_xmit_deinit();
	whc_host_event_deinit();

	whc_ipc_host_msg_q_deinit();

	/* Deinit ipc channel of data and event. */
	whc_host_ipc_ch_deinit(&global_idev.event_ch);
	whc_host_ipc_ch_deinit(&global_idev.data_ch);

}
