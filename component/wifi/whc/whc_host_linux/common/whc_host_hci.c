#include <whc_host_linux.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct whc_device global_idev;

int whc_host_init(void)
{
#if defined(CONFIG_WHC_WIFI_API_PATH)
	int ret = 0;
#endif
	struct whc_device *idev = &global_idev;

#if defined(CONFIG_FULLMAC_HCI_SDIO)
	idev->intf_priv = &whc_sdio_priv;
	idev->intf_ops = &whc_sdio_host_intf_ops;
#elif defined(CONFIG_FULLMAC_HCI_SPI)
	idev->intf_priv = &whc_spi_priv;
	idev->intf_ops = &whc_spi_host_intf_ops;
#elif defined(CONFIG_FULLMAC_HCI_USB)
	idev->intf_priv = &whc_usb_host_priv;
	idev->intf_ops = &whc_usb_host_intf_ops;
#else
#error Not support other interfaces!
#endif

	whc_host_recv_init();

#if defined(CONFIG_WHC_WIFI_API_PATH)
	ret = whc_fullmac_host_event_init(idev);
	if (ret < 0) {
		dev_err(idev->fullmac_dev, "whc_fullmac_host_event_init error(%d).\n", ret);
		goto exit;
	}
#endif

	whc_host_xmit_init();

	global_idev.host_init_done = 1;

	memset(&global_idev.wifi_user_config, 0, sizeof(struct wifi_user_conf));
#if defined(CONFIG_WHC_WIFI_API_PATH)
	whc_fullmac_host_set_user_config(&global_idev.wifi_user_config);
#endif

#if defined(CONFIG_WHC_WIFI_API_PATH)
	/* tell KM4 to open wifi */
	whc_fullmac_host_wifi_on();
#endif
	return 0;

#if defined(CONFIG_WHC_WIFI_API_PATH)
exit:
	return ret;
#endif
}

void whc_host_deinit(void)
{
	whc_host_xmit_deinit();

#if defined(CONFIG_WHC_WIFI_API_PATH)
	whc_fullmac_host_event_deinit();
#endif

	whc_host_recv_deinit();
}

