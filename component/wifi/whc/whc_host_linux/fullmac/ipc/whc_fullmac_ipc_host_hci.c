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

extern struct aipc_ch_ops whc_fullmac_ipc_host_recv_ops;
extern struct aipc_ch_ops whc_fullmac_ipc_host_event_ops;
extern struct wifi_user_conf wifi_user_config __attribute__((aligned(64)));
_WEAK void wifi_set_user_config(void);
static aipc_ch_t *whc_fullmac_host_ipc_data_ch_init(struct aipc_ch_ops *ops)
{
	aipc_ch_t *data_ch = NULL;

	/* allocate the ipc channel */
	data_ch = ameba_ipc_alloc_ch(sizeof(struct whc_device *));
	if (!data_ch) {
		dev_err(global_idev.fullmac_dev, "alloc ch 0 failed!!!");
		return NULL;
	}
	/* initialize the ipc channel */
	data_ch->port_id = AIPC_PORT_NP;
	data_ch->ch_id = 0; /* configure channel 0 */
	data_ch->ch_config = AIPC_CONFIG_NOTHING;
	data_ch->ops = ops;
	data_ch->priv_data = NULL;

	/* regist the ipc channel */
	if (ameba_ipc_channel_register(data_ch) < 0) {
		dev_err(global_idev.fullmac_dev, "register ipc data channel failed!!!");
		goto free_ipc_ch;
	}

	return data_ch;

free_ipc_ch:
	kfree(data_ch);
	return NULL;
}

void whc_fullmac_host_ipc_data_ch_deinit(void)
{
	if (!global_idev.data_ch) {
		dev_err(global_idev.fullmac_dev, "ERROR: event ch has been deinit.");
		return;
	}

	/* unregist the channel */
	ameba_ipc_channel_unregister(global_idev.data_ch);

	kfree(global_idev.data_ch);
	global_idev.data_ch = NULL;
}

static aipc_ch_t *whc_fullmac_host_ipc_event_ch_init(struct aipc_ch_ops *ops)
{
	aipc_ch_t *event_ch = NULL;

	/* allocate the ipc channel */
	event_ch = ameba_ipc_alloc_ch(sizeof(struct event_priv_t *));
	if (!event_ch) {
		dev_err(global_idev.fullmac_dev, "%s: no memory for ipc channel 1.\n", __func__);
		return NULL;
	}

	/* initialize the ipc channel */
	event_ch->port_id = AIPC_PORT_NP;
	event_ch->ch_id = 1; /* configure channel 1 */
	event_ch->ch_config = AIPC_CONFIG_NOTHING;
	event_ch->ops = ops;
	event_ch->priv_data = NULL;

	/* regist the event_priv ipc channel */
	if (ameba_ipc_channel_register(event_ch) < 0) {
		dev_err(global_idev.fullmac_dev, "%s: regist event_priv channel error.\n", __func__);
		goto free_ipc_ch;
	}

	return event_ch;

free_ipc_ch:
	kfree(event_ch);
	return NULL;
}

void whc_fullmac_host_ipc_event_ch_deinit(void)
{
	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "ERROR: event ch has been deinit.");
		return;
	}

	/* unregist the channel */
	ameba_ipc_channel_unregister(global_idev.event_ch);

	kfree(global_idev.event_ch);
	global_idev.event_ch = NULL;
}

int whc_host_init(void)
{
	int ret = 0;
	struct whc_device *idev = &global_idev;

	/* IPC channel of data and event init. */
	global_idev.data_ch = whc_fullmac_host_ipc_data_ch_init(&whc_fullmac_ipc_host_recv_ops);
	global_idev.event_ch = whc_fullmac_host_ipc_event_ch_init(&whc_fullmac_ipc_host_event_ops);
	if (global_idev.data_ch == NULL || global_idev.event_ch == NULL) {
		dev_err(global_idev.fullmac_dev, "IPC Allocate channel failed.");
		return -ENOMEM;
	}
	global_idev.ipc_dev = global_idev.data_ch->pdev;

	/* initialize the message queue, and assign the task haddle function */
	ret = whc_fullmac_ipc_host_msg_q_init(global_idev.ipc_dev, whc_fullmac_ipc_host_recv_task_from_msg);
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "msg queue init fail.");
		goto ipc_deinit;
	}

	ret = whc_fullmac_host_event_init(idev);
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "ipc host: init ipc host event_priv error(%d).\n", ret);
		goto ipc_deinit;
	}

	wifi_set_user_config();
	memcpy(&global_idev.wifi_user_config, &wifi_user_config, sizeof(struct wifi_user_conf));

	ret = whc_host_xmit_init();
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "malloc ipc xmit memory failed.(%d).\n", ret);
		goto ipc_deinit;
	}

	global_idev.host_init_done = 1;

	ret = whc_fullmac_host_set_user_config(&global_idev.wifi_user_config);
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "set wifi user config failed.(%d).\n", ret);
		goto ipc_deinit;
	}

	/* tell KM4 to open wifi */
	whc_fullmac_host_wifi_on();

	return 0;

ipc_deinit:
	whc_fullmac_host_ipc_event_ch_deinit();
	whc_fullmac_host_ipc_data_ch_deinit();
	return ret;
}

void whc_host_deinit(void)
{
	whc_host_xmit_deinit();
	whc_fullmac_host_event_deinit();

	whc_fullmac_ipc_host_msg_q_deinit();

	/* Deinit ipc channel of data and event. */
	whc_fullmac_host_ipc_event_ch_deinit();
	whc_fullmac_host_ipc_data_ch_deinit();

}
