

/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_internal.h"


int ota_transport_write(ota_context_t *ctx, u8 *buf, int len)
{
	int bytes_written = -1;

	if (ctx->fd == -1) {
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Invalid fd\n");
		return -1;
	}

	switch (ctx->type) {
	case OTA_HTTPS:
	case OTA_HTTP:
		bytes_written = ota_http_write(ctx, buf, len);
		break;

	default:
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Unsupported type: %d\n", ctx->type);
		return -1;
	}

	return bytes_written;
}

int ota_transport_read(ota_context_t *ctx, u8 *buf, int len)
{
	int bytes_read = -1;

	if ((ctx->type != OTA_WHC) && (ctx->fd == -1)) {
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Invalid fd\n");
		return -1;
	}

	switch (ctx->type) {
#ifdef CONFIG_WHC_INTF_IPC
	case OTA_HTTPS:
	case OTA_HTTP:
		bytes_read = ota_http_read(ctx, buf, len);
		break;

	case OTA_VFS:
		bytes_read = ota_vfs_read(ctx, buf, len);
		break;
#endif
	case OTA_USER:
		if (ctx->user_read_func) {
			bytes_read = ctx->user_read_func(buf, len);
		} else {
			RTK_LOGE(OTA_TAG, "[TRANSPORT] user_read_func is null\n");
		}
		break;

#ifdef CONFIG_WHC_CMD_PATH
	case OTA_WHC:
		bytes_read = ota_whc_read(ctx, buf, len);
		break;
#endif

	default:
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Unsupported type: %d\n", ctx->type);
		return -1;
	}

	if (bytes_read < 0) {
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Read error\n");
		return -2;
	}

	return bytes_read;
}

int ota_transport_connect(ota_context_t *ctx)
{
	int ret = OTA_ERR;

	RTK_LOGI(OTA_TAG, "[TRANSPORT] Connecting...\n");

	switch (ctx->type) {
#ifdef CONFIG_WHC_INTF_IPC
	case OTA_HTTP:
	case OTA_HTTPS:
		ret = ota_http_connect(ctx);
		break;
#endif

#ifdef CONFIG_WHC_CMD_PATH
	case OTA_WHC:
		ret = ota_whc_connect(ctx);
		break;
#endif

	case OTA_VFS:
		ret = ota_vfs_open(ctx);
		break;

	case OTA_USER:
		ret = OTA_OK;
		if (ctx->user_open_func) {
			ret = ctx->user_open_func();
		}
		break;

	default:
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Unknown type: %d\n", ctx->type);
		return OTA_ERR;
	}

	if (ret == OTA_OK) {
		RTK_LOGI(OTA_TAG, "[TRANSPORT] Connected\n");
	} else {
		RTK_LOGE(OTA_TAG, "[TRANSPORT] Connect failed\n");
	}

	return ret;
}

void ota_transport_disconnect(ota_context_t *ctx)
{
	if (!ctx) {
		return;
	}

	RTK_LOGI(OTA_TAG, "[TRANSPORT] Disconnecting...\n");

	switch (ctx->type) {
#ifdef CONFIG_WHC_INTF_IPC
	case OTA_HTTP:
	case OTA_HTTPS:
		ota_http_close(ctx);
		break;
#endif
	case OTA_VFS:
		ota_vfs_close(ctx);
		break;

#ifdef CONFIG_WHC_CMD_PATH
	case OTA_WHC:
		ota_whc_close(ctx);
		break;
#endif

	default:
		break;
	}

	RTK_LOGI(OTA_TAG, "[TRANSPORT] Disconnected\n");
}

int ota_transport_init(ota_context_t *ctx, char *host, int port, char *resource)
{
	if (ctx->type == OTA_USER) {
		return OTA_OK;
	}

	/* 1. check host and redirect. Need for http and https */
	if (ctx->type == OTA_HTTP || ctx->type == OTA_HTTPS || ctx->type == OTA_WHC) {
		if (!host) {
			RTK_LOGE(OTA_TAG, "[Transport] host can't be null");
			return OTA_ERR;
		} else {
			ctx->port = port;
			ctx->host = (char *)rtos_mem_malloc(strlen(host) + 1);
			if (ctx->host) {
				strcpy(ctx->host, host);
			} else {
				RTK_LOGE(OTA_TAG, "[Transport] host malloc failed");
				return OTA_ERR;
			}

			ctx->redirect = (ota_http_redirect_t *)rtos_mem_zmalloc(sizeof(ota_http_redirect_t));
			if (!ctx->redirect) {
				RTK_LOGE(OTA_TAG, "[Transport] redirect malloc failed");
				return OTA_ERR;
			}
		}
	}
	/* 2. check resource. Need for http, https and vfs */
	if (!resource) {
		RTK_LOGE(OTA_TAG, "[Transport] resourse can't be null");
		return OTA_ERR;
	}

	ctx->fd = -1;
	ctx->resource = (char *)rtos_mem_malloc(strlen(resource) + 1);
	if (ctx->resource) {
		strcpy(ctx->resource, resource);
	} else {
		RTK_LOGE(OTA_TAG, "[Transport] resource malloc failed");
		return OTA_ERR;
	}

	return OTA_OK;
}