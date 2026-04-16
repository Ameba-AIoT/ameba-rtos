#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <whc_host_linux.h>
#include "whc_usb_host.h"

#include "whc_host_xfer.h"

/* External USB XFER operations */
extern const struct whc_xfer_ops_t whc_usb_xfer_ops;

/**
 * @brief	Download firmware using XFER protocol for USB
 * @param	priv: USB device handle
 * @return	0: Download success, device will re-enumerate
 * 		1: Firmware already ready
 * 		negative error code on failure
 */
int whc_usb_xfer_download(struct whc_usb *priv)
{
	struct whc_xfer_adapter_t *adapter;
	struct whc_xfer_ops_t *ops;
	int ret, image_type;

	/* Initialize XFER adapter */
	adapter = whc_xfer_adapter_alloc(priv, &whc_usb_xfer_ops);
	if (adapter == NULL) {
		dev_err(priv->dev, "Fail to allocate XFER adapter\n");
		return -ENOMEM;
	}
	ops = adapter->ops;

	/* Initialize USB XFER resources (ctrl_mutex, ctrl_in_buf) */
	ret = ops->init(adapter, NULL);
	if (ret != 0) {
		dev_err(priv->dev, "Fail to init XFER ops\n");
		goto exit_free_adapter;
	}

	ret = ops->check_firmware(adapter);
	if (ret == WHC_XFER_FW_TYPE_ROM) {
		dev_info(priv->dev, "Firmware not ready, start to download bootloader\n");
		image_type = WHC_IMAGE_TYPE_BOOTLOADER;
	} else if (ret == WHC_XFER_FW_TYPE_BOOTLOADER) {
		dev_info(priv->dev, "Firmware not ready, start to download application\n");
		image_type = WHC_IMAGE_TYPE_APPLICATION;
	} else if (ret == WHC_XFER_FW_TYPE_APPLICATION) {
		dev_info(priv->dev, "Firmware is ready\n");
		ret = 1;
		goto exit;
	} else {
		dev_err(priv->dev, "Fail to check firmware: %d\n", ret);
		ret = -1;
		goto exit;
	}

	ret = whc_xfer_download_image(adapter, image_type);
	if (ret < 0) {
		dev_err(priv->dev, "Download firmware failed (%d)\n", ret);
	}

exit:
	ops->deinit(adapter);

exit_free_adapter:
	whc_xfer_adapter_free(adapter);
	return ret;
}
