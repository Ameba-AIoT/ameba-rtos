#include <rtw_cfg80211_fullmac.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct inic_device global_idev;

int llhw_init(void)
{
	int ret = 0;
	struct inic_device *idev = &global_idev;

#if defined(CONFIG_FULLMAC_HCI_SDIO)
	idev->intf_priv = &inic_sdio_priv;
	idev->intf_ops = &sdio_intf_ops;
#elif defined(CONFIG_FULLMAC_HCI_SPI)
	idev->intf_priv = &inic_spi_priv;
	idev->intf_ops = &spi_intf_ops;
#elif defined(CONFIG_FULLMAC_HCI_USB)
	idev->intf_priv = &inic_usb_priv;
	idev->intf_ops = &usb_intf_ops;
#else
#error Not support other interfaces!
#endif

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

