/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ota_api.h"

/**
 * @brief  Read exactly `need` bytes from transport into buf, stopping early on EOF.
 * @retval OTA_OK / OTA_ERR
 */
static int read_exact(ota_context_t *ctx, u8 *buf, u32 need)
{
	int read_bytes;
	while (need) {
		read_bytes = ota_transport_read(ctx, buf, need);
		if (read_bytes < 0) {
			RTK_LOGE(OTA_TAG, "[CORE] Read failed\n");
			return OTA_ERR;
		}
		if (read_bytes == 0) {
			RTK_LOGE(OTA_TAG, "[CORE] Unexpected EOF, %lu bytes remaining\n", need);
			return OTA_ERR;
		}
		need -= read_bytes;
		buf  += read_bytes;
	}
	return OTA_OK;
}

/**
  * @brief  receive OTA file header from server
  * @param   ctx: ota context
  * @param  recv_buf: buffer for receiving OTA file header
  * @param  write_len: the length already read from server
  * @param  len: data length to be received from server
  * @retval  OTA_ERR:failed;OTA_OK:success
*/
static int ota_core_recv_header(ota_context_t *ctx, u8 *recv_buf, u32 write_len, u32 *len)
{
	ota_hdr_t *file_hdr;
	ota_hdr_manager_t *hdr_mgr = ctx->otaHdrManager;

	/*receive the remaining OTA file header info if needed*/
	if (write_len < OTA_HEADER_LEN) {
		if (read_exact(ctx, recv_buf + write_len, OTA_HEADER_LEN - write_len) != OTA_OK) {
			return OTA_ERR;
		}
		write_len = OTA_HEADER_LEN;
	}

	file_hdr = (ota_hdr_t *)recv_buf;

	hdr_mgr->FileHdr.FwVer  = file_hdr->FwVer;
	hdr_mgr->FileHdr.HdrNum = file_hdr->HdrNum;

	RTK_LOGI(OTA_TAG, "[CORE] OTA header num: %lu\n", hdr_mgr->FileHdr.HdrNum);

	if (hdr_mgr->FileHdr.HdrNum > OTA_IMGID_MAX) {
		RTK_LOGE(OTA_TAG, "[CORE] OTA header num: %lu is invaild\n", hdr_mgr->FileHdr.HdrNum);
		return OTA_ERR;
	}

	/*read the remaining Header info*/
	u32 full_hdr_len = file_hdr->HdrNum * OTA_SUB_HEADER_LEN + OTA_HEADER_LEN;
	if (write_len < full_hdr_len) {
		if (read_exact(ctx, recv_buf + write_len, full_hdr_len - write_len) != OTA_OK) {
			return OTA_ERR;
		}
	}

	*len = full_hdr_len;
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
	ota_sub_hdr_t *img_hdr;
	ota_hdr_t *file_hdr;
	ota_hdr_manager_t *hdr_mgr = ctx->otaHdrManager;
	u8 *p;
	u32 i = 0;

	/*check if buf and len is valid or not*/
	if ((len < OTA_HEADER_LEN) || (!buf)) {
		return OTA_ERR;
	}

	file_hdr = (ota_hdr_t *)buf;
	p = buf + OTA_HEADER_LEN;
	hdr_mgr->ValidImgCnt = 0;

	if (len < (hdr_mgr->FileHdr.HdrNum * OTA_SUB_HEADER_LEN + OTA_HEADER_LEN)) {
		return OTA_ERR;
	}

	/*get the target OTA header from the file header*/
	for (i = 0; i < file_hdr->HdrNum; i++) {
		p = buf + OTA_HEADER_LEN + OTA_SUB_HEADER_LEN * i;
		img_hdr = (ota_sub_hdr_t *)p;

		if (strncmp("OTA", (const char *)img_hdr->Signature, 3) != 0) {
			return OTA_ERR;
		}

		_memcpy((void *)(&hdr_mgr->FileImgHdr[hdr_mgr->ValidImgCnt]), (void *)p, sizeof(ota_sub_hdr_t));
		hdr_mgr->ValidImgCnt++;
	}

	if (hdr_mgr->ValidImgCnt == 0) {
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

	/* flush the last buffered NAND page to flash before verify (no-op on NOR) */
	if (ota_storage_flush() != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] flush fail\n");
		return OTA_ERR;
	}

	/* verify checksum only if OTA header was successfully parsed; without a
	 * valid header the slot address and image length are unreliable. */
	if (otaCtrl->IsGetOTAHdr) {
		if (ota_storage_verify_checksum(ctx->otaHdrManager, otaCtrl->SlotIdx, otaCtrl->Index) != OTA_OK) {
			return OTA_ERR;
		}
	}

	if (ota_storage_update_manifest(ctx->otaHdrManager, otaCtrl->SlotIdx, otaCtrl->Index) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Change signature failed\n");
		return OTA_ERR;
	}
download_app:
	/*check if another image is needed to download*/
	if (otaCtrl->Index < ctx->otaHdrManager->ValidImgCnt - 1) {
		otaCtrl->Index++;
		otaCtrl->IsDnldInit = 0;
		RTK_LOGI(OTA_TAG, "[CORE] Download image index : %d\n", otaCtrl->Index);
		return OTA_OK;
	} else {
		RTK_LOGI(OTA_TAG, "[CORE] Download image end, total image num: %d\n", ctx->otaHdrManager->ValidImgCnt);
		return OTA_FINISH; // download end
	}
}

static void download_parameter_init(ota_context_t *ctx)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	ota_hdr_manager_t *hdr_mgr = ctx->otaHdrManager;

	otaCtrl->ImgId = hdr_mgr->FileImgHdr[otaCtrl->Index].ImgID;

	/* check OTA index we should update */
	otaCtrl->SlotIdx = ota_get_target_slot(otaCtrl->ImgId);
	RTK_LOGI(OTA_TAG, "[CORE] OTA%d address space will be upgraded\n", otaCtrl->SlotIdx + 1);

	/*update flag status*/
	otaCtrl->ManifestReady = 0;
	otaCtrl->ManifestRecvLen = 0;
	otaCtrl->SkipBootOTAFg = 0;

	otaCtrl->FlashAddr = ota_storage_get_image_addr(otaCtrl->ImgId, otaCtrl->SlotIdx) - SPI_FLASH_BASE;
	otaCtrl->ImageLen = hdr_mgr->FileImgHdr[otaCtrl->Index].ImgLen;

	RTK_LOGI(OTA_TAG, "[CORE] ImgId: %lu\n", otaCtrl->ImgId);
	RTK_LOGI(OTA_TAG, "[CORE] FlashAddr: 0x%08x\n", (unsigned int)otaCtrl->FlashAddr);
	RTK_LOGI(OTA_TAG, "[CORE] ImageLen: %lu\n", otaCtrl->ImageLen);
	RTK_LOGI(OTA_TAG, "[CORE] Offset: %lu\n", hdr_mgr->FileImgHdr[otaCtrl->Index].Offset);

	/*initialize the receiving counter*/
	otaCtrl->ImgOffset = 0;
	otaCtrl->ReadBytes = 0;

	/*skip the manifest structure*/
	otaCtrl->RemainBytes = otaCtrl->ImageLen - sizeof(ota_manifest_t);
	otaCtrl->FlashAddr = otaCtrl->FlashAddr + sizeof(ota_manifest_t);

	/*check bootloader OTA2*/
	if (otaCtrl->ImgId == OTA_IMGID_BOOT && otaCtrl->SlotIdx == OTA_INDEX_2) {
		otaCtrl->SkipBootOTAFg = ota_check_bootloader_ota2();
	}

	RTK_LOGI(OTA_TAG, "[CORE] ReadBytes: %d, ImgOffset: %lu\n", otaCtrl->ReadBytes, otaCtrl->ImgOffset);
}

/**
 * @brief  Accumulate manifest bytes from the current packet into RAM.
 *         Adjusts buf/len to the image payload portion (bytes after the
 *         manifest). The manifest is never written to flash here; that happens
 *         only after checksum passes in download_finish (power-loss safety).
 * @retval 0: manifest not yet complete — no payload to write this packet
 *         1: payload available in *buf / *len
 */
static int manifest_accumulate(ota_download_ctrl_t *otaCtrl, ota_manifest_t *manifest,
							   u8 **buf, int *len)
{
	const int manifest_size = sizeof(ota_manifest_t);
	int TempCnt;
	int need;

	if (!otaCtrl->ManifestReady) {
		/* first packet reaching the manifest: start accumulating */
		otaCtrl->ManifestReady = 1;
		TempCnt = otaCtrl->ReadBytes - otaCtrl->ImgOffset;
		otaCtrl->ManifestRecvLen = TempCnt < manifest_size ? TempCnt : manifest_size;
		_memcpy((void *)(u8 *)manifest, (void *)(*buf + (*len - TempCnt)), otaCtrl->ManifestRecvLen);

		if (otaCtrl->ManifestRecvLen < manifest_size || TempCnt - manifest_size == 0) {
			return 0; /* manifest incomplete or no payload follows */
		}

		*buf += *len - TempCnt + manifest_size;
		*len  = TempCnt - manifest_size;
		return 1;
	}

	/* manifest may still span multiple packets */
	if (otaCtrl->ManifestRecvLen < manifest_size) {
		need = manifest_size - otaCtrl->ManifestRecvLen;
		if (*len < need) {
			_memcpy((void *)((u8 *)manifest + otaCtrl->ManifestRecvLen), (void *)*buf, *len);
			otaCtrl->ManifestRecvLen += *len;
			return 0; /* still incomplete */
		}
		RTK_LOGI(OTA_TAG, "[CORE] manifest_size: %d, ManifestRecvLen: %d, len: %d, remaining: %d\n",
				 manifest_size, otaCtrl->ManifestRecvLen, *len, need);
		_memcpy((void *)((u8 *)manifest + otaCtrl->ManifestRecvLen), (void *)*buf, need);
		*buf += need;
		*len -= need;
		otaCtrl->ManifestRecvLen = manifest_size;

		if (!*len) {
			return 0; /* manifest just completed, no payload follows */
		}
	}

	return 1; /* manifest complete, payload available */
}

static int download_packet_process(ota_context_t *ctx, u8 *buf, int len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	ota_hdr_manager_t *hdr_mgr = ctx->otaHdrManager;
	static ota_manifest_t *manifest = NULL;
	static int size = 0;

	/* initialize download parameters */
	if (otaCtrl->IsDnldInit == 0) {
		download_parameter_init(ctx);
		otaCtrl->IsDnldInit = 1;
		manifest = &hdr_mgr->Manifest[otaCtrl->Index];
		size = 0;
		ota_storage_begin_image(otaCtrl->ImgId, otaCtrl->SlotIdx);
	}

	otaCtrl->ReadBytes += len;

	/* accumulate manifest in RAM; skip writing until checksum passes */
	if (otaCtrl->ReadBytes > (int)otaCtrl->ImgOffset) {
		if (!manifest_accumulate(otaCtrl, manifest, &buf, &len)) {
			return size;
		}
	}

	otaCtrl->RemainBytes -= len;

	/* last packet: check if next image content is concatenated */
	if (otaCtrl->RemainBytes < 0) {
		otaCtrl->NextImgLen = -otaCtrl->RemainBytes;
		len -= otaCtrl->NextImgLen;
		if (otaCtrl->NextImgLen > 0) {
			_memcpy((void *)otaCtrl->NextImgBuf, (void *)(buf + len), otaCtrl->NextImgLen);
			otaCtrl->NextImgFg = 1;
		}
	}

	/* skip flash write if bootloader OTA2 slot is invalid */
	if (otaCtrl->SkipBootOTAFg == 1) {
		size += len;
		return size;
	}

	/* Append payload; storage owns the cursor, erase-ahead and bad-block skip. */
	if (ota_storage_write_stream(buf, (u32)len) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] Flash write failed\n");
		return OTA_ERR;
	}
	size += len;
	return size;
}

/**
 * @brief  Parse the OTA file header from the first packet(s).
 * @param  ctx: OTA context
 * @param  buf: received data buffer
 * @param  len: length of received data
 * @retval OTA_ERR (-1): header parse failed
 *         OTA_OK  ( 0): header parsed, no remaining payload in this packet
 *         >0           : header parsed, value is remaining payload byte count
 * @note   Caller should check IsGetOTAHdr to distinguish OTA_ERR from OTA_OK.
 */
static int ota_core_parse_header(ota_context_t *ctx, u8 *buf, u32 len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	u32 recv_hdr_len = 0;
	int write_len = 0;
	int remain_len = 0;
	int max_hdr_len = OTA_HEADER_LEN + OTA_SUB_HEADER_LEN * OTA_IMGID_MAX;
	u8 *temp = NULL;

	write_len = len;
	/* temp provides extra headroom in case ota_core_recv_header needs to
	 * read more bytes from transport to complete the header. */
	temp = (u8 *)rtos_mem_zmalloc(write_len + max_hdr_len);
	if (!temp) {
		RTK_LOGE(OTA_TAG, "[CORE] Alloc buffer failed\n");
		return OTA_ERR;
	}
	_memcpy(temp, buf, len);

	/* receive ota image file header */
	if (ota_core_recv_header(ctx, temp, write_len, &recv_hdr_len) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Recv OTA header failed\n");
		return OTA_ERR;
	}

	/* parse file header and get the target OTA image header */
	if (ota_core_get_target_header(ctx, temp, recv_hdr_len) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Get OTA header failed\n");
		return OTA_ERR;
	}

	if (ota_storage_check_layout(ctx->otaHdrManager) != OTA_OK) {
		rtos_mem_free(temp);
		RTK_LOGE(OTA_TAG, "[CORE] Check image layout failed\n");
		return OTA_ERR;
	}

	otaCtrl->IsGetOTAHdr = 1;
	rtos_mem_free(temp);

	if (write_len <= (int)recv_hdr_len) {
		RTK_LOGI(OTA_TAG, "[CORE] Get ota header, write_len: %d, recv_hdr_len: %d\n", write_len, (int)recv_hdr_len);
		return OTA_OK;
	}

	/* strip the header from buf; return remaining payload length */
	remain_len = len - recv_hdr_len;
	_memcpy(buf, buf + recv_hdr_len, remain_len);
	RTK_LOGI(OTA_TAG, "[CORE] Get ota header, write_len: %d, recv_hdr_len: %d\n", write_len, (int)recv_hdr_len);
	return remain_len;
}

static int ota_fw_program(ota_context_t *ctx, u8 *buf, u32 len)
{
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	int size = 0;
	int remain_len = 0;

	if (!otaCtrl->IsGetOTAHdr) {
		remain_len = ota_core_parse_header(ctx, buf, len);
		if (remain_len <= 0) {
			return remain_len;
		}
		len = remain_len;
	}

	/* Multi-image: if the previous image's last packet contained the start of
	 * this image, process those bytes first before the new transport data. */
	if (otaCtrl->NextImgFg == 1) {
		size = download_packet_process(ctx, otaCtrl->NextImgBuf, otaCtrl->NextImgLen);
		otaCtrl->NextImgFg = 0;
		if (size < 0) {
			return OTA_ERR;
		}
	}

	size = download_packet_process(ctx, buf, len);
	if (size < 0) {
		return OTA_ERR;
	}
	download_percentage(ctx, size, otaCtrl->ImageLen);

	if (otaCtrl->RemainBytes <= 0) {
		/* manifest is held in RAM and written only after checksum passes (power-loss safety);
		 * add it back here so total equals ImageLen for the completeness check. */
		size += sizeof(ota_manifest_t);
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
	u8 *buf_ptr = NULL;
	ota_download_ctrl_t *otaCtrl = ctx->otaCtrl;
	int read_bytes;
	int ret = OTA_ERR;

	RTK_LOGI(OTA_TAG, "[CORE] Process start\n");

	buf = (u8 *)rtos_mem_malloc(OTA_BUF_SIZE);
	if (!buf) {
		RTK_LOGE(OTA_TAG, "[CORE] Buffer malloc failed\n");
		return OTA_ERR;
	}

	otaCtrl->Index = 0;
	otaCtrl->IsGetOTAHdr = 0;
	otaCtrl->IsDnldInit = 0;
	RTK_LOGI(OTA_TAG, "[CORE] Download image index : %d\n", otaCtrl->Index);

	while (1) {
		if (ctx->prefill_buf) {
			/* First iteration: consume HTTP body bytes received with the header
			 * (S3-1: transport layer no longer writes directly into otaCtrl). */
			buf_ptr    = ctx->prefill_buf;
			read_bytes = ctx->prefill_len;
		} else {
			buf_ptr    = buf;
			read_bytes = ota_transport_read(ctx, buf, OTA_BUF_SIZE);
			if (read_bytes == 0) {
				RTK_LOGE(OTA_TAG, "[CORE] Unexpected EOF\n");
				break;
			}
			if (read_bytes < 0) {
				RTK_LOGE(OTA_TAG, "[CORE] Read failed\n");
				break;
			}
		}

		ret = ota_fw_program(ctx, buf_ptr, read_bytes);

		if (ctx->prefill_buf) {
			rtos_mem_free(ctx->prefill_buf);
			ctx->prefill_buf = NULL;
			ctx->prefill_len = 0;
		}

		if (ret == OTA_FINISH) {
			RTK_LOGI(OTA_TAG, "[CORE] All images downloaded\n");
			break;
		}
		if (ret == OTA_ERR) {
			RTK_LOGE(OTA_TAG, "[CORE] Process failed\n");
			break; /* ret != OTA_FINISH, ota_start will report failure */
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
			ota_transport_disconnect(ctx);
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
	if (ota_core_process(ctx) != OTA_FINISH) {
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
	/* NOTE: ca_cert / client_cert / private_key are borrowed read-only pointers
	 * supplied by the caller (typically static PEM data); the module never
	 * allocates them, so ota_deinit must NOT free them. */
	/* Release prefill buffer if transport was disconnected before Core consumed it */
	if (ctx->prefill_buf) {
		rtos_mem_free(ctx->prefill_buf);
		ctx->prefill_buf = NULL;
	}
	/* Release ota controls resources */
	if (ctx->otaCtrl) {
		rtos_mem_free(ctx->otaCtrl);
	}

	/* Release ota header manager resources */
	if (ctx->otaHdrManager) {
		rtos_mem_free(ctx->otaHdrManager);
	}

	/* Release storage layer buffers (idempotent) */
	ota_storage_deinit();

	RTK_LOGI(OTA_TAG, "[CORE] Deinit complete\n");
}

int ota_set_image_info(ota_context_t *ctx, u32 img_id, u32 img_len)
{
	ota_hdr_manager_t *hdr_mgr;
	ota_sub_hdr_t *img_hdr;

	if (!ctx || !ctx->otaCtrl || !ctx->otaHdrManager) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_set_image_info: ctx not initialized\n");
		return OTA_ERR;
	}
	if (img_id >= OTA_IMGID_MAX || img_len <= sizeof(ota_manifest_t)) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_set_image_info: invalid img_id %lu or img_len %lu\n", img_id, img_len);
		return OTA_ERR;
	}

	hdr_mgr = ctx->otaHdrManager;

	/* Build a single-image header from caller-supplied metadata. The push path
	 * (ota_write) feeds header-stripped payload, so there is no file header to
	 * parse; this provides the slot and length that download_parameter_init
	 * needs. Image integrity is the caller's responsibility (no checksum here).
	 * Zero the whole manager so the state is clean regardless of any prior
	 * (e.g. aborted) session on this context. */
	_memset((void *)hdr_mgr, 0, sizeof(ota_hdr_manager_t));
	img_hdr = &hdr_mgr->FileImgHdr[0];
	_memcpy((void *)img_hdr->Signature, "OTA", 3);
	img_hdr->ImgID    = img_id;
	img_hdr->ImgLen   = img_len;
	img_hdr->Offset   = 0;
	hdr_mgr->ValidImgCnt = 1;

	/* Reject a declared length that does not fit the target slot before any write. */
	if (ota_storage_check_layout(hdr_mgr) != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_set_image_info: image layout check failed\n");
		hdr_mgr->ValidImgCnt = 0;
		return OTA_ERR;
	}

	ctx->otaCtrl->Index = 0;
	RTK_LOGI(OTA_TAG, "[CORE] ota_set_image_info: ImgID %lu, ImgLen %lu\n", img_id, img_len);
	return OTA_OK;
}

int ota_write(ota_context_t *ctx, const u8 *buf, int len)
{
	int ret;

	if (!ctx || !ctx->otaCtrl || !ctx->otaHdrManager) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_write: ctx not initialized\n");
		return OTA_ERR;
	}
	if (!buf || len <= 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_write: invalid buf or len\n");
		return OTA_ERR;
	}
	/* Image metadata must be provided up front; the push path never parses a file header. */
	if (ctx->otaHdrManager->ValidImgCnt == 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_write: image info not set, call ota_set_image_info first\n");
		return OTA_ERR;
	}
	/* Strict guard: the caller must feed header-stripped payload. If the first
	 * chunk still carries an OTA file header (sub-header "OTA" signature right
	 * after the 8-byte file header), reject rather than write it as image body. */
	if (!ctx->otaCtrl->IsDnldInit && len >= OTA_HEADER_LEN + 3 &&
		strncmp("OTA", (const char *)(buf + OTA_HEADER_LEN), 3) == 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_write: data still contains an OTA file header, expected stripped payload\n");
		return OTA_ERR;
	}

	ret = download_packet_process(ctx, (u8 *)buf, len);
	return (ret < 0) ? OTA_ERR : OTA_OK;
}

int ota_read(ota_context_t *ctx, int offset, u8 *buf, int len)
{
	ota_download_ctrl_t *otaCtrl;
	ota_manifest_t *manifest;
	const int manifest_size = sizeof(ota_manifest_t);
	int copied = 0;

	if (!ctx || !ctx->otaCtrl || !ctx->otaHdrManager) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_read: ctx not initialized\n");
		return OTA_ERR;
	}
	if (!buf || offset < 0 || len <= 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_read: invalid buf, offset or len\n");
		return OTA_ERR;
	}

	otaCtrl = ctx->otaCtrl;

	if (!otaCtrl->IsDnldInit) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_read: no data written yet, call ota_write first\n");
		return OTA_ERR;
	}

	/* The written firmware spans image offset [0, ImageLen): the leading
	 * manifest (manifest_size bytes) is still held in RAM (not committed to
	 * flash until ota_finish), the remainder is the image body already in
	 * flash. Reject a range that runs past the declared image length. */
	if ((u32)offset + (u32)len > otaCtrl->ImageLen) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_read: range [%d, %d) exceeds image length %lu\n",
				 offset, offset + len, otaCtrl->ImageLen);
		return OTA_ERR;
	}

	manifest = &ctx->otaHdrManager->Manifest[otaCtrl->Index];

	/* Part 1: bytes that fall inside the RAM-held manifest region. */
	if (offset < manifest_size) {
		int n = (offset + len > manifest_size) ? (manifest_size - offset) : len;
		_memcpy((void *)buf, (void *)((u8 *)manifest + offset), n);
		copied += n;
	}

	/* Part 2: bytes that fall inside the flash-written image body. FlashAddr is
	 * a flash offset that already points past the manifest (see
	 * download_parameter_init), so the body byte at image offset `off` lives at
	 * FlashAddr + (off - manifest_size). */
	if (offset + len > manifest_size) {
		int body_start = (offset > manifest_size) ? offset : manifest_size;
		u32 flash_off = otaCtrl->FlashAddr + (body_start - manifest_size);
		int n = offset + len - body_start;
		if (ota_storage_read(flash_off, buf + copied, n) != OTA_OK) {
			RTK_LOGE(OTA_TAG, "[CORE] ota_read: flash read failed\n");
			return OTA_ERR;
		}
	}

	return OTA_OK;
}

int ota_finish(ota_context_t *ctx)
{
	ota_download_ctrl_t *otaCtrl;

	if (!ctx || !ctx->otaCtrl) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_finish: ctx not initialized\n");
		return OTA_ERR;
	}

	otaCtrl = ctx->otaCtrl;

	if (ctx->otaHdrManager->ValidImgCnt == 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_finish: image info not set, call ota_set_image_info first\n");
		return OTA_ERR;
	}
	if (!otaCtrl->IsDnldInit) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_finish: no data written, call ota_write first\n");
		return OTA_ERR;
	}
	if (otaCtrl->RemainBytes > 0) {
		RTK_LOGE(OTA_TAG, "[CORE] ota_finish: data incomplete, %d bytes remaining\n", otaCtrl->RemainBytes);
		return OTA_ERR;
	}

	/* Reuse the transport path's commit logic. Image integrity is verified by
	 * the caller, so IsGetOTAHdr stays 0 and download_finish skips checksum;
	 * the manifest is written last for power-loss safety. */
	return (download_finish(ctx, otaCtrl->ImageLen) == OTA_ERR) ? OTA_ERR : OTA_OK;
}