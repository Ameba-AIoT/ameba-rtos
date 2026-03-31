/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_api.h"

/**
  * @brief  receive OTA file header from server
  * @param   ctx: ota context
  * @param  Recvbuf: buffer for receiving OTA file header
  * @param  writelen:the length already read from server
  * @param  len: data length to be received from server
  * @retval  OTA_ERR:failed;OTA_OK:success
*/
static int ota_core_recv_header(ota_context_t *ctx, u8 *Recvbuf, u32 writelen, u32 *len)
{
	int read_bytes = 0;
	u32 TempLen;
	u8 *buf;
	ota_hdr_t *pOtaFileHdr;
	ota_hdr_manager_t *pOtaHdrManager = ctx->otaHdrManager;

	buf = Recvbuf + writelen;
	/*receive the remaining OTA file header info if needed*/
	if (writelen < OTA_HEADER_LEN) {
		TempLen = OTA_HEADER_LEN - writelen;
		while (TempLen) {
			read_bytes = ota_transport_read(ctx, buf, TempLen);
			if (read_bytes < 0) {
				RTK_LOGE(OTA_TAG, "[CORE] Read failed\n");
				return OTA_ERR;
			}
			if (read_bytes == 0) {
				break;
			}
			TempLen -= read_bytes;
			buf += read_bytes;
			writelen += read_bytes;
		}
	}

	pOtaFileHdr = (ota_hdr_t *)(Recvbuf);

	pOtaHdrManager->FileHdr.FwVer = pOtaFileHdr->FwVer;
	pOtaHdrManager->FileHdr.HdrNum = pOtaFileHdr->HdrNum;

	RTK_LOGI(OTA_TAG, "[CORE] OTA header num: %lu\n", pOtaHdrManager->FileHdr.HdrNum);

	if (pOtaHdrManager->FileHdr.HdrNum > OTA_IMGID_MAX) {
		RTK_LOGE(OTA_TAG, "[CORE] OTA header num: %lu is invaild\n", pOtaHdrManager->FileHdr.HdrNum);
		return OTA_ERR;
	}

	/*read the remaining Header info*/
	if (writelen < (pOtaFileHdr->HdrNum * OTA_SUB_HEADER_LEN + OTA_HEADER_LEN)) {
		buf = Recvbuf + writelen;
		TempLen = pOtaFileHdr->HdrNum * OTA_SUB_HEADER_LEN + OTA_HEADER_LEN - writelen;

		while (TempLen) {
			read_bytes = ota_transport_read(ctx, buf, TempLen);
			if (read_bytes < 0) {
				RTK_LOGE(OTA_TAG, "[CORE] Read failed\n");
				return OTA_ERR;
			}
			if (read_bytes == 0) {
				break;
			}
			TempLen -= read_bytes;
			buf += read_bytes;
		}
	}

	*len = (pOtaFileHdr->HdrNum * OTA_SUB_HEADER_LEN) + OTA_HEADER_LEN;
	return OTA_OK;
}

/**
  * @brief  parse file header and get the desired OTA header
  * @param   ctx: ota context
  * @param   buf: buffer of received OTA header
  * @param   len: data length of received buffer
  * @retval OTA_ERR: receive fail, OTA_OK: receive ok
  */
static int ota_core_get_target_header(ota_context_t *ctx, u8 *buf, u32 len)
{
	ota_sub_hdr_t *ImgHdr;
	ota_hdr_t *pOtaFileHdr;
	ota_hdr_manager_t *pOtaHdrManager = ctx->otaHdrManager;
	u8 *pTempAddr;
	u32 i = 0;

	/*check if buf and len is valid or not*/
	if ((len < OTA_HEADER_LEN) || (!buf)) {
		return OTA_ERR;
	}

	pOtaFileHdr = (ota_hdr_t *)buf;
	pTempAddr = buf + OTA_HEADER_LEN;
	pOtaHdrManager->ValidImgCnt = 0;

	if (len < (pOtaHdrManager->FileHdr.HdrNum * OTA_SUB_HEADER_LEN + OTA_HEADER_LEN)) {
		return OTA_ERR;
	}

	/*get the target OTA header from the file header*/
	for (i = 0; i < pOtaFileHdr->HdrNum; i++) {
		pTempAddr = buf + OTA_HEADER_LEN + OTA_SUB_HEADER_LEN * i;
		ImgHdr = (ota_sub_hdr_t *)(pTempAddr);

		if (strncmp("OTA", (const char *)ImgHdr->Signature, 3) != 0) {
			return OTA_ERR;
		}

		_memcpy((void *)(u8 *)(&pOtaHdrManager->FileImgHdr[pOtaHdrManager->ValidImgCnt]), (void *)pTempAddr, sizeof(ota_sub_hdr_t));
		pOtaHdrManager->ValidImgCnt++;
	}

	if (pOtaHdrManager->ValidImgCnt == 0) {
		RTK_LOGI(OTA_TAG, "[CORE] no valid image\n");
		return OTA_ERR;
	}

	return OTA_OK;
}

static void download_percentage(ota_context_t *ctx, int cur_size, int total_size)
{
	static int percent_bak = -1;
	int percent = cur_size * 100 / total_size;
	if (percent_bak != percent && percent % 10 == 0) {
		percent_bak = percent;
		RTK_LOGI(OTA_TAG, "[CORE] OTA download percentage: %d\n", percent);
	}
	if (ctx->progress_cb) {
		ctx->progress_cb(percent);
	}
}

static int download_finish(ota_context_t *ctx, int size)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;

	RTK_LOGI(OTA_TAG, "[CORE] Update file size: %d bytes, start addr:0x%08x\n", size, (unsigned int)(otaCtrl->FlashAddr + SPI_FLASH_BASE));
	if ((u32)(size) != otaCtrl->ImageLen) {
		RTK_LOGE(OTA_TAG, "[CORE] Download new image failed\n");
		return OTA_ERR;
	}

	if (otaCtrl->SkipBootOTAFg) {
		RTK_LOGW(OTA_TAG, "[CORE] Bootloader OTA2 address is invalid, skip Bootloader OTA2\n");
		goto download_app;
	}

	/* verify checksum and update manifest */
	if (ota_storage_verify_checksum(ctx->otaHdrManager, otaCtrl->slotIdx, otaCtrl->index) != OTA_OK) {
		return OTA_ERR;
	}

	if (ota_storage_update_manifest(ctx->otaHdrManager, otaCtrl->slotIdx, otaCtrl->index) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Change signature failed\n");
		return OTA_ERR;
	}
download_app:
	/*check if another image is needed to download*/
	if (otaCtrl->index < ctx->otaHdrManager->ValidImgCnt - 1) {
		otaCtrl->index++;
		otaCtrl->IsDnldInit = 0;
		RTK_LOGI(OTA_TAG, "[CORE] Download image index : %d\n", otaCtrl->index);
		return OTA_OK;
	} else {
		RTK_LOGI(OTA_TAG, "[CORE] Download image end, total image num: %d\n", ctx->otaHdrManager->ValidImgCnt);
		return OTA_FINISH; // download end
	}
}

static void download_parameter_init(ota_context_t *ctx)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	ota_hdr_manager_t *pOtaHdrManager = ctx->otaHdrManager;

	otaCtrl->ImgId = pOtaHdrManager->FileImgHdr[otaCtrl->index].ImgID;

	/* check OTA index we should update */
	if (ota_get_cur_index(otaCtrl->ImgId) == OTA_INDEX_1) {
		otaCtrl->slotIdx = OTA_INDEX_2;
	} else {
		otaCtrl->slotIdx = OTA_INDEX_1;
	}
	RTK_LOGI(OTA_TAG, "[CORE] OTA%d address space will be upgraded\n", otaCtrl->slotIdx + 1);

	/*update flag status*/
	otaCtrl->SigFg = 0;
	otaCtrl->SigCnt = 0;
	otaCtrl->SkipBootOTAFg = 0;

	otaCtrl->FlashAddr = ota_storage_get_image_addr(otaCtrl->ImgId, otaCtrl->slotIdx) - SPI_FLASH_BASE;
	otaCtrl->ImageLen = pOtaHdrManager->FileImgHdr[otaCtrl->index].ImgLen;
	otaCtrl->ImgOffset = pOtaHdrManager->FileImgHdr[otaCtrl->index].Offset;

	RTK_LOGI(OTA_TAG, "[CORE] ImgId: %lu\n", otaCtrl->ImgId);
	RTK_LOGI(OTA_TAG, "[CORE] FlashAddr: 0x%08x\n", (unsigned int)otaCtrl->FlashAddr);
	RTK_LOGI(OTA_TAG, "[CORE] ImageLen: %lu\n", otaCtrl->ImageLen);
	RTK_LOGI(OTA_TAG, "[CORE] ImgOffset: %lu\n", otaCtrl->ImgOffset);

	/*initialize the reveiving counter*/
	otaCtrl->ImgOffset = 0;
	otaCtrl->ReadBytes = 0;

	/*skip the manifest structure*/
	otaCtrl->RemainBytes = otaCtrl->ImageLen - sizeof(ota_manifest_t);
	otaCtrl->FlashAddr = otaCtrl->FlashAddr + sizeof(ota_manifest_t);

	/*check bootloader OTA2*/
	if (otaCtrl->ImgId == OTA_IMGID_BOOT && otaCtrl->slotIdx == OTA_INDEX_2) {
		otaCtrl->SkipBootOTAFg = ota_check_bootloader_ota2();
	}

	RTK_LOGI(OTA_TAG, "[CORE] ReadBytes: %d, ImgOffset: %lu\n", otaCtrl->ReadBytes, otaCtrl->ImgOffset);
}

static int download_packet_process(ota_context_t *ctx, u8 *buf, int len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	ota_hdr_manager_t *pOtaHdrManager = ctx->otaHdrManager;
	static ota_manifest_t *manifest = NULL;
	static int manifest_size = sizeof(ota_manifest_t);
	static u32 write_sector = 0;
	static u32 next_erase_sector = 0;
	static int size = 0;
	int TempCnt = 0;

	/* initialize download parameters */
	if (otaCtrl->IsDnldInit == 0) {
		download_parameter_init(ctx);
		otaCtrl->IsDnldInit = 1;
		manifest = &pOtaHdrManager->Manifest[otaCtrl->index];
		write_sector = 0;
		next_erase_sector = 0;
		size = 0;
	}

	otaCtrl->ReadBytes += len;

	/* cache the manifest, not write manifest first for power down protection */
	if (otaCtrl->ReadBytes > (int)otaCtrl->ImgOffset) {
		if (!otaCtrl->SigFg) {
			/* first packet process, reach the desired image */
			otaCtrl->SigFg = 1;
			TempCnt = otaCtrl->ReadBytes - otaCtrl->ImgOffset;
			otaCtrl->SigCnt = TempCnt < manifest_size ? TempCnt : manifest_size;
			_memcpy((void *)(u8 *)manifest, (void *)(buf + (len - TempCnt)), otaCtrl->SigCnt);

			if (otaCtrl->SigCnt < manifest_size || TempCnt - manifest_size == 0) {
				return size;
			}

			buf = buf + (len - TempCnt + manifest_size);
			len = TempCnt - manifest_size;
		} else {
			/* normal packet process */
			if (otaCtrl->SigCnt < manifest_size) {
				if (len < (int)(manifest_size - otaCtrl->SigCnt)) {
					_memcpy((void *)((u8 *)manifest + otaCtrl->SigCnt), (void *)buf, len);
					otaCtrl->SigCnt += len;
					return size;
				} else {
					RTK_LOGI(OTA_TAG, "[CORE] manifest_size: %d, otaCtrl->SigCnt: %d, len: %d, (manifest_size - otaCtrl->SigCnt): %d\n",
							 manifest_size, otaCtrl->SigCnt, len, manifest_size - otaCtrl->SigCnt);
					_memcpy((void *)((u8 *)manifest + otaCtrl->SigCnt), (void *)buf, manifest_size - otaCtrl->SigCnt);

					buf = buf + (manifest_size - otaCtrl->SigCnt);
					len -= (manifest_size - otaCtrl->SigCnt) ;

					otaCtrl->SigCnt = manifest_size;

					if (!len) {
						return size;
					}
				}
			}
		}
	}

	otaCtrl->RemainBytes -= len;

	/* last packet process, check if next image content concatenated */
	if (otaCtrl->RemainBytes < 0) {
		otaCtrl->NextImgLen = -otaCtrl->RemainBytes;
		len = len - (-otaCtrl->RemainBytes);
		if (otaCtrl->NextImgLen > 0) {
			_memcpy((void *)otaCtrl->NextImgBuf, (void *)(buf + len), otaCtrl->NextImgLen);
			otaCtrl->NextImgFg = 1;
		}
	}

	/* check if skip bootloader needed */
	if (otaCtrl->SkipBootOTAFg == 1) {
		size += len;
		return size;
	}

	/* erase and write flash */
	write_sector = (otaCtrl->ImageLen - otaCtrl->RemainBytes - 1 + manifest_size) / 4096;
	if (write_sector >= next_erase_sector) {
		ota_storage_erase_sector(otaCtrl->FlashAddr - manifest_size + write_sector * 4096);
		next_erase_sector++;
	}

	ota_storage_write(otaCtrl->FlashAddr + size, buf, len);
	size += len;
	return size;
}

static int ota_core_parse_header(ota_context_t *ctx, u8 *buf, u32 len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	u32 RecvHdrLen = 0;
	int writelen = 0;
	int remainlen = 0;
	int MaxHdrLen = OTA_HEADER_LEN + OTA_SUB_HEADER_LEN * OTA_IMGID_MAX;
	u8 *temp = NULL;

	if (otaCtrl->NextImgFg == 1) {
		writelen = len + otaCtrl->NextImgLen;
	} else {
		writelen = len;
	}
	/* temp is used for cat the NextImgBuf with buf */
	temp = (u8 *)rtos_mem_zmalloc(writelen + MaxHdrLen);
	if (!temp) {
		RTK_LOGE(OTA_TAG, "[CORE] Alloc buffer failed\n");
		return OTA_ERR;
	}

	if (otaCtrl->NextImgFg == 1) {
		_memcpy(temp, otaCtrl->NextImgBuf, otaCtrl->NextImgLen);
		_memcpy(temp + otaCtrl->NextImgLen, buf, len);
	} else {
		_memcpy(temp, buf, len);
	}

	/* receive ota image file header */
	if (ota_core_recv_header(ctx, temp, writelen, &RecvHdrLen) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Recv OTA header failed\n");
		return OTA_ERR;
	}

	/* parse file header and get the target OTA image header */
	if (ota_core_get_target_header(ctx, temp, RecvHdrLen) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Get OTA header failed\n");
		return OTA_ERR;
	}

	if (ota_storage_check_layout(ctx->otaHdrManager) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Check image layout failed\n");
		return OTA_ERR;
	}

	if (writelen <= (int)RecvHdrLen) {
		rtos_mem_free(temp);
		otaCtrl->IsGetOTAHdr = 1;
		RTK_LOGI(OTA_TAG, "[CORE] Get ota header, writelen: %d, RecvHdrLen: %d\n", writelen, (int)RecvHdrLen);
		return OTA_OK;
	}

	if (otaCtrl->NextImgFg) {
		if (otaCtrl->NextImgLen > (int)RecvHdrLen) {
			/* remove ota header from NextImgBuf*/
			_memcpy((void *)otaCtrl->NextImgBuf, (void *)(otaCtrl->NextImgBuf + RecvHdrLen), otaCtrl->NextImgLen - RecvHdrLen);
			otaCtrl->NextImgLen -= RecvHdrLen;
			remainlen = len;
		} else {
			/* remove ota header part from buf*/
			otaCtrl->NextImgFg = 0;
			remainlen = len - (RecvHdrLen - otaCtrl->NextImgLen);
			_memcpy(buf, buf + (RecvHdrLen - otaCtrl->NextImgLen), remainlen);
		}
	} else {
		/* remove ota header part from buf*/
		remainlen = len - RecvHdrLen;
		_memcpy(buf, buf + RecvHdrLen, remainlen);
	}
	otaCtrl->IsGetOTAHdr = 1;
	rtos_mem_free(temp);
	RTK_LOGI(OTA_TAG, "[CORE] Get ota header, writelen: %d, RecvHdrLen: %d\n", writelen, (int)RecvHdrLen);
	return remainlen;
}

static int ota_fw_program(ota_context_t *ctx, u8 *buf, u32 len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	int size = 0;
	int remainlen = 0;

	if (!otaCtrl->IsGetOTAHdr) {
		remainlen = ota_core_parse_header(ctx, buf, len);
		if (remainlen <= 0) {
			return remainlen;
		}
		len = remainlen;
	}

	if (otaCtrl->NextImgFg == 1) {
		size = download_packet_process(ctx, otaCtrl->NextImgBuf, otaCtrl->NextImgLen);
		otaCtrl->NextImgFg = 0;
		RTK_LOGI(OTA_TAG, "%s, size: %d\n", __func__, size);
	}

	size = download_packet_process(ctx, buf, len);
	download_percentage(ctx, size, otaCtrl->ImageLen);

	if (otaCtrl->RemainBytes <= 0) {
		size += sizeof(ota_manifest_t);    //add the manifest length
		download_percentage(ctx, size, otaCtrl->ImageLen);
		return download_finish(ctx, size);
	}

	return OTA_OK;
}

int ota_register_progress_cb(ota_context_t *ctx, ota_progress_cb_t cb)
{
	if (!ctx) {
		return OTA_ERR;
	}
	ctx->progress_cb = cb;
	return OTA_OK;
}

int ota_register_user_read_func(ota_context_t *ctx, ota_user_read_func_t func)
{
	if (!ctx) {
		return OTA_ERR;
	}
	ctx->user_read_func = func;
	return OTA_OK;
}

int ota_register_user_open_func(ota_context_t *ctx, ota_user_open_func_t func)
{
	if (!ctx) {
		return OTA_ERR;
	}
	ctx->user_open_func = func;
	return OTA_OK;
}

int ota_register_user_close_func(ota_context_t *ctx, ota_user_close_func_t func)
{
	if (!ctx) {
		return OTA_ERR;
	}
	ctx->user_close_func = func;
	return OTA_OK;
}

int ota_register_yield_func(ota_context_t *ctx, ota_yield_func_t func)
{
	if (!ctx) {
		return OTA_ERR;
	}
	ctx->yield_func = func;
	return OTA_OK;
}

static int ota_core_process(ota_context_t *ctx)
{
	u8 *buf = NULL;
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	int read_bytes;
	int ret = OTA_ERR;

	RTK_LOGI(OTA_TAG, "[CORE] Process start\n");

	/*allocate buffer for downloading image from server*/
	buf = (u8 *)rtos_mem_malloc(OTA_BUF_SIZE);
	if (!buf) {
		RTK_LOGE(OTA_TAG, "[CORE] Buffer malloc failed\n");
		return OTA_ERR;
	}

	otaCtrl->index = 0;
	otaCtrl->IsGetOTAHdr = 0;
	otaCtrl->IsDnldInit = 0;
	RTK_LOGI(OTA_TAG, "[CORE] Download image index : %d\n", otaCtrl->index);

	while (1) {
		_memset(buf, 0, OTA_BUF_SIZE);
		read_bytes = ota_transport_read(ctx, buf, OTA_BUF_SIZE);
		if (read_bytes == 0) {
			ret = OTA_OK;
			break; // Read end
		}
		if (read_bytes < 0) {
			RTK_LOGE(OTA_TAG, "[CORE] Read failed\n");
			ret = OTA_ERR;
			break;
		}
		ret = ota_fw_program(ctx, buf, read_bytes);
		if (ret == OTA_FINISH) {
			ret = OTA_OK;
			RTK_LOGI(OTA_TAG, "[CORE] All images downloaded\n");
			break;
		}
		if (ret == OTA_ERR) {
			RTK_LOGI(OTA_TAG, "[CORE] Process failed\n");
			break;
		}
		if (ctx->yield_func) {
			ctx->yield_func();
		}
	}

	rtos_mem_free(buf);
	RTK_LOGI(OTA_TAG, "[CORE] Process complete\n");
	return ret;
}

static int ota_core_prepare(ota_context_t *ctx)
{
	int ret = OTA_ERR;
	ota_http_redirect_t *redirect = ctx->redirect;

	RTK_LOGI(OTA_TAG, "[CORE] Prepare start\n");

	/* Transport connect */
	ret = ota_transport_connect(ctx);
	if (ret != OTA_OK) {
		/* redirect.port != 0 means there is redirect URL can be downloaded*/
		if (redirect->port != 0) {
			RTK_LOGI(OTA_TAG, "[CORE] Redirect host: %s, port: %d, resource: %s\r\n", ctx->host, ctx->port, ctx->resource);
			ret = ota_transport_connect(ctx);
		}

		if (ret != OTA_OK) {
			RTK_LOGE(OTA_TAG, "[CORE] Connect failed\n");
			return ret;
		}
	}
	RTK_LOGI(OTA_TAG, "[CORE] Prepare complete\n");
	return ret;
}

int ota_start(ota_context_t *ctx)
{
	int ret = OTA_OK;

	if (!ctx) {
		RTK_LOGE(OTA_TAG, "[CORE] ctx is NULL\n");
		return OTA_ERR;
	}

	RTK_LOGI(OTA_TAG, "\n========== OTA START ==========\n");
	/*----------------step1: ota prepare---------------------*/
	if (ota_core_prepare(ctx) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Prepare failed\n");
		ret = OTA_ERR;
		goto exit;
	}

	/*----------------step2: ota process (download and program)---------------------*/
	if (ota_core_process(ctx) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Process failed\n");
		ret = OTA_ERR;
		goto exit;
	}
	RTK_LOGI(OTA_TAG, "\n========== OTA SUCCESS ==========\n");

exit:
	ota_transport_disconnect(ctx);
	return ret;
}

int ota_init(ota_context_t *ctx, char *host, int port, char *resource, u8 type)
{
	if (!ctx) {
		RTK_LOGE(OTA_TAG, "ctx can't be null\n");
		return OTA_ERR;
	}

	RTK_LOGI(OTA_TAG, "[CORE] Initializing OTA...\n");

	_memset(ctx, 0, sizeof(ota_context_t));
	ctx->type = type;

	/* Initialize storage params */
	if (ota_storage_init() != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Storage init failed\n");
		goto exit;
	}

	/* Initialize transport params */
	if (ota_transport_init(ctx, host, port, resource) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Transport init failed\n");
		goto exit;
	}

	/* Initialize ota download controls and header manager */
	ctx->otaCtrl = (ota_download_ctrl_t *)rtos_mem_zmalloc(sizeof(ota_download_ctrl_t));
	ctx->otaHdrManager = (ota_hdr_manager_t *)rtos_mem_zmalloc(sizeof(ota_hdr_manager_t));

	if (!ctx->otaCtrl || !ctx->otaHdrManager) {
		RTK_LOGE(OTA_TAG, "[CORE] Malloc failed\n");
		goto exit;
	}

	RTK_LOGI(OTA_TAG, "[CORE] host: %s(%d), resource: %s\n", host ? ctx->host : "null", ctx->port, resource ? ctx->resource : "null");
	RTK_LOGI(OTA_TAG, "[CORE] Init success\n");
	return OTA_OK;
exit:
	ota_deinit(ctx);
	return OTA_ERR;
}

void ota_deinit(ota_context_t *ctx)
{
	if (ctx == NULL) {
		return;
	}

	RTK_LOGI(OTA_TAG, "[CORE] Deinitializing OTA...\n");

	/* Release transport resources */
	if (ctx->host) {
		rtos_mem_free(ctx->host);
	}
	if (ctx->resource) {
		rtos_mem_free(ctx->resource);
	}
	if (ctx->redirect) {
		rtos_mem_free(ctx->redirect);
	}
	/* Release certs */
	if (ctx->ca_cert) {
		rtos_mem_free(ctx->ca_cert);
	}
	if (ctx->client_cert) {
		rtos_mem_free(ctx->client_cert);
	}
	if (ctx->private_key) {
		rtos_mem_free(ctx->private_key);
	}
	/* Release ota controls resources */
	if (ctx->otaCtrl) {
		rtos_mem_free(ctx->otaCtrl);
	}

	/* Release ota header manager resources */
	if (ctx->otaHdrManager) {
		rtos_mem_free(ctx->otaHdrManager);
	}
	RTK_LOGI(OTA_TAG, "[CORE] Deinit complete\n");
}