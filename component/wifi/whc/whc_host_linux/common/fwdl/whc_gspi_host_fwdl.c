// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 *
 * Copyright(c) Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 ******************************************************************************/

#include <whc_host_linux.h>

#include "whc_host_xfer.h"

/* Download firmware using XFER protocol over GSPI.
 * GSPI device boots from flash so there is no ROM DMA phase (Phase 1);
 * we go straight to detecting bootloader/application state and downloading. */
int whc_gspi_xfer_download(struct whc_gspi *priv)
{
	struct whc_xfer_adapter_t *adapter;
	int ret;

	adapter = whc_xfer_adapter_alloc(priv, &whc_gspi_xfer_ops);
	if (adapter == NULL) {
		dev_err(&priv->spi_dev->dev, "Fail to allocate XFER adapter\n");
		return -ENOMEM;
	}

	priv->dev_state = WHC_XFER_FW_TYPE_ROM;

	ret = adapter->ops->check_firmware(adapter);
	if (ret == WHC_XFER_FW_TYPE_APPLICATION) {
		dev_info(&priv->spi_dev->dev, "%s: firmware already running\n", __FUNCTION__);
		ret = 0;
		goto exit_free_adapter;
	} else if (ret != WHC_XFER_FW_TYPE_BOOTLOADER) {
		ret = -1;
		goto exit_free_adapter;
	}

	ret = adapter->ops->init(adapter, NULL);
	if (ret != 0) {
		dev_err(&priv->spi_dev->dev, "XFER start failed (%d)\n", ret);
		goto exit_free_adapter;
	}

	ret = whc_xfer_download_image(adapter, WHC_IMAGE_TYPE_APPLICATION);
	if (ret < 0) {
		dev_err(&priv->spi_dev->dev, "Download application failed (%d)\n", ret);
	}
	ret = (ret < 0) ? ret : 0;

exit_free_adapter:
	whc_xfer_adapter_free(adapter);
	return ret;
}
