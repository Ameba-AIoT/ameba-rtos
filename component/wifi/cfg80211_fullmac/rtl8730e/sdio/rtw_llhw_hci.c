#include <rtw_cfg80211_fullmac.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct inic_device global_idev;

void llhw_free_rxbuf(u8 *rxbuf)
{
	rtw_sdio_free_rxbuf(rxbuf);
}

void llhw_host_send(u8 *buf, u32 len)
{
	rtw_sdio_send_msg(buf, len);
}

int llhw_init(void)
{
	int ret = 0;
	struct inic_device *idev = &global_idev;

	/* Enable interrupt that will call the worker */
	//enable_irq(idev->spidev->irq);

	/* initialize the message queue, and assign the task haddle function */
	ret = inic_msg_q_init(&idev->msg_priv, llhw_recv_pkts);
	if (ret < 0) {
		dev_err(idev->fullmac_dev, "msg queue init fail.");
		goto exit;
	}

	ret = llhw_event_init(idev);
	if (ret < 0) {
		dev_err(idev->fullmac_dev, "llhw_event_init error(%d).\n", ret);
		goto exit;
	}

	memset(&global_idev.wifi_user_config, 0, sizeof(struct wifi_user_conf));
	llhw_wifi_set_user_config(&global_idev.wifi_user_config);

	llhw_xmit_init();

	/* tell KM4 to do wifi on? wifi on when insmod ? */
	llhw_wifi_on();

	return 0;

exit:
	return ret;
}

void llhw_deinit(void)
{
	llhw_xmit_deinit();

	llhw_event_deinit();

	inic_msg_q_deinit(&global_idev.msg_priv);
}

