#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <whc_host_linux.h>
#include "whc_sdio_host.h"

#include "whc_host_xfer.h"

/* External SDIO XFER operations */
extern const struct whc_xfer_ops_t whc_sdio_xfer_ops;

/**
 * @brief	Download firmware using XFER protocol
 * @param	priv: SDIO device handle
 * @return	0: Success
 * negative error code on failure
 */
int whc_sdio_xfer_download(struct whc_sdio *priv)
{
	struct whc_xfer_adapter_t *adapter;
	int ret;

	/* Check if firmware download is needed */
	if (!whc_sdio_check_dl_mode(priv)) {
		return 0;
	}

	adapter = whc_xfer_adapter_alloc(priv, &whc_sdio_xfer_ops);
	if (adapter == NULL) {
		dev_err(&priv->func->dev, "Fail to allocate XFER adapter\n");
		return -ENOMEM;
	}

	priv->dev_state = WHC_XFER_FW_TYPE_ROM;

	/* Download bootloader image */
	ret = whc_xfer_download_image(adapter, WHC_IMAGE_TYPE_BOOTLOADER);
	if (ret < 0) {
		dev_err(&priv->func->dev, "Download bootloader failed (%d)\n", ret);
		goto exit_free_adapter;
	}

	/* Start XFER download mode (switch to XFER interrupt handler) */
	ret = adapter->ops->init(adapter, NULL);
	if (ret != 0) {
		dev_err(&priv->func->dev, "XFER start failed (%d)\n", ret);
		goto exit_free_adapter;
	}

	/* Download application image */
	ret = whc_xfer_download_image(adapter, WHC_IMAGE_TYPE_APPLICATION);
	if (ret < 0) {
		dev_err(&priv->func->dev, "Download application failed (%d)\n", ret);
		goto exit_free_adapter;
	}

	ret = 0;

exit_free_adapter:
	whc_xfer_adapter_free(adapter);

	/* Restore dev_state for normal operation */
	priv->dev_state = PWR_STATE_ACTIVE;

	return ret;
}

