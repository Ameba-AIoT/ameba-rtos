/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_internal.h"
#include "vfs.h"

int ota_vfs_open(ota_context_t *ctx)
{
	vfs_file *finfo;

	finfo = (vfs_file *)fopen(ctx->resource, "r");
	if (!finfo) {
		RTK_LOGE(OTA_TAG, "File %s not exist.\n", ctx->resource);
		return OTA_ERR;
	}

	ctx->fd = (int)finfo;
	return OTA_OK;
}

void ota_vfs_close(ota_context_t *ctx)
{
	if (ctx->fd > 0) {
		fclose((FILE *)ctx->fd);
	}
}

int ota_vfs_read(ota_context_t *ctx, u8 *buf, int len)
{
	return fread(buf, len, 1, (FILE *)ctx->fd);
}