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
 * @return	0: Success
 * 		negative error code on failure
 */
int whc_usb_xfer_download(struct whc_usb *priv)
{
	struct whc_xfer_adapter_t *adapter;
	struct whc_xfer_ops_t *ops;
	int ret, image_type;

	mutex_init(&(priv->ctrl_mutex));

	/* Allocate control IN buffer for firmware check/reset */
	priv->ctrl_in_buf = kzalloc(WHC_XFER_DESC_SIZE, GFP_KERNEL);
	if (priv->ctrl_in_buf == NULL) {
		dev_err(priv->dev, "Fail to allocate ctrl_in_buf\n");
		return -ENOMEM;
	}

	/* Initialize XFER adapter */
	adapter = whc_xfer_adapter_alloc(priv, &whc_usb_xfer_ops);
	if (adapter == NULL) {
		dev_err(priv->dev, "Fail to allocate XFER adapter\n");
		ret = -ENOMEM;
		goto exit_free_buf;
	}
	ops = adapter->ops;

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
	whc_xfer_adapter_free(adapter);

exit_free_buf:
	kfree(priv->ctrl_in_buf);
	priv->ctrl_in_buf = NULL;
	return ret;
}
