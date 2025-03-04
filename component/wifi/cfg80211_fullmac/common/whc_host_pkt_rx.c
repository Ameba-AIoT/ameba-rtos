#include <whc_host_linux.h>

void whc_host_recv_notify(void)
{
	up(&global_idev.recv_priv.rx_sema);
}

int whc_host_recv_process(struct sk_buff *pskb)
{
	int ret = 0;
#ifdef CONFIG_WHC_BRIDGE
	ret = whc_bridge_host_recv_process(pskb);
#else
	ret = whc_fullmac_host_recv_process(pskb);
#endif
	return ret;
}

static int whc_host_recv_thread(void *data)
{
	int ret = 0;

	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	while (!kthread_should_stop()) {

		/* wait for sema*/
		down_interruptible(&recv_priv->rx_sema);

		whc_host_recv_data_process(global_idev.intf_priv);
	}

	return ret;
}

void whc_host_recv_init(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	/* Create Rx thread */
	sema_init(&recv_priv->rx_sema, 0);

	recv_priv->rx_thread = kthread_run(whc_host_recv_thread, recv_priv, "RTW_RX_THREAD");
	if (IS_ERR(recv_priv->rx_thread)) {
		dev_err(global_idev.fullmac_dev, "FAIL to create llhw_recv_thread!\n");
		recv_priv->rx_thread = NULL;
	}

	recv_priv->initialized = true;
}

void whc_host_recv_deinit(void)
{
	struct recv_priv_t *recv_priv = &global_idev.recv_priv;

	if (recv_priv->rx_thread) {
		up(&recv_priv->rx_sema);
		kthread_stop(recv_priv->rx_thread);
		recv_priv->rx_thread = NULL;
	}
}

