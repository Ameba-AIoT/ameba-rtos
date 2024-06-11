#include <rtw_cfg80211_fullmac.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct inic_device global_idev;

void llhw_host_send(u8 *buf, u32 len)
{
	struct inic_device *idev = &global_idev;

	if (idev->sdio_priv->dev_state == PWR_STATE_SLEEP) {
		dev_dbg(idev->fullmac_dev, "%s: wakeup device", __func__);
		if (rtw_resume_common(idev->sdio_priv)) {
			dev_err(idev->fullmac_dev, "%s: fail to wakeup device, stop send", __func__);
			return;
		}
	}

	rtw_sdio_send_msg(buf, len);
}

int llhw_init(void)
{
	int ret = 0;
	struct inic_device *idev = &global_idev;

	idev->sdio_priv = &inic_sdio_priv;

	llhw_recv_init();

	ret = llhw_event_init(idev);
	if (ret < 0) {
		dev_err(idev->fullmac_dev, "llhw_event_init error(%d).\n", ret);
		goto exit;
	}

	memset(&global_idev.wifi_user_config, 0, sizeof(struct wifi_user_conf));
	llhw_wifi_set_user_config(&global_idev.wifi_user_config);

	llhw_xmit_init();

	/* tell KM4 to open wifi */
	llhw_wifi_on();

	return 0;

exit:
	return ret;
}

void llhw_deinit(void)
{
	llhw_xmit_deinit();

	llhw_event_deinit();

	llhw_recv_deinit();
}

