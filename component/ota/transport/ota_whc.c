/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_internal.h"

#if defined(CONFIG_WHC_CMD_PATH)
#include "whc_dev_ota.h"

static int ota_whc_recv_response(ota_context_t *ctx)
{
	int read_bytes = 0;
	int remain_len = -1;
	ota_http_response_t rsp_result = {0};
	u32 idx = 0;
	u8 *buf = NULL;

	buf = (u8 *)rtos_mem_zmalloc(OTA_BUF_SIZE);
	rsp_result.header_bak = rtos_mem_zmalloc(OTA_HEADER_BAK_LEN + 1);

	if (!buf || !rsp_result.header_bak) {
		RTK_LOGE(OTA_TAG, "[WHC] Malloc failed\n");
		goto exit;
	}

	while (3 >= rsp_result.parse_status) { //still read header
		if (0 == rsp_result.parse_status) { //didn't get the http response
			_memset(buf, 0, OTA_BUF_SIZE);
			read_bytes = whc_dev_ota_read(buf, OTA_BUF_SIZE);
			if (read_bytes <= 0) {
				RTK_LOGE(OTA_TAG, "[WHC]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes;
			_memset(&rsp_result, 0, sizeof(rsp_result));
			if (ota_http_parse_response(ctx, buf, idx, &rsp_result) == OTA_ERR) {
				goto exit;
			}
		} else if ((1 == rsp_result.parse_status) || (3 == rsp_result.parse_status)) { //just get the status code
			_memset(buf, 0, OTA_BUF_SIZE);
			_memcpy(buf, rsp_result.header_bak, OTA_HEADER_BAK_LEN);
			read_bytes = whc_dev_ota_read(buf + OTA_HEADER_BAK_LEN, (OTA_BUF_SIZE - OTA_HEADER_BAK_LEN));
			if (read_bytes <= 0) {
				RTK_LOGE(OTA_TAG, "[WHC]  Read socket failed\n");
				goto exit;
			}
			idx = read_bytes + OTA_HEADER_BAK_LEN;
			if (ota_http_parse_response(ctx, buf, read_bytes + OTA_HEADER_BAK_LEN, &rsp_result) == -1) {
				goto exit;
			}
		}
	}

	if (0 == rsp_result.body_len) {
		RTK_LOGI(OTA_TAG, "[WHC] New firmware size = 0 !\n");
		goto exit;
	} else {
		RTK_LOGI(OTA_TAG, "[WHC] Download new firmware begin, total size : %lu\n", rsp_result.body_len);
	}

	RTK_LOGI(OTA_TAG, "[WHC] idx = %lu, rsp_result.header_len = %lu\n", idx, rsp_result.header_len);

	remain_len = idx - rsp_result.header_len;
	/* remove http header_len from buf*/
	if (remain_len > 0) {
		ctx->otaCtrl->NextImgLen = remain_len;
		_memset(ctx->otaCtrl->NextImgBuf, 0, OTA_BUF_SIZE);
		_memcpy((void *)ctx->otaCtrl->NextImgBuf, (void *)(buf + rsp_result.header_len), remain_len);
		ctx->otaCtrl->NextImgFg = 1;
	}

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (rsp_result.header_bak) {
		rtos_mem_free(rsp_result.header_bak);
	}
	return remain_len;
}

int ota_whc_connect(ota_context_t *ctx)
{
	if (ota_whc_recv_response(ctx) < 0) {
		RTK_LOGE(OTA_TAG, "[WHC] Parse response failed\n");
		return OTA_ERR;
	}

	return OTA_OK;
}

int ota_whc_read(ota_context_t *ctx, u8 *buf, int len)
{
	(void)ctx;
	int readlen = whc_dev_ota_read(buf, len);
	return readlen;
}

void ota_whc_close(ota_context_t *ctx)
{
	(void)ctx;
	RTK_LOGI(OTA_TAG, "[WHC] Closing connection...\n");
	whc_dev_ota_close();
	RTK_LOGI(OTA_TAG, "[WHC] Connection closed\n");
}

#endif