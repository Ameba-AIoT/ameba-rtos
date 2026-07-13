/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OTA_API_H_
#define _OTA_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ota_internal.h"


/* ============ Exported functions ======================== */

/**
 * @brief  Initialize OTA context and allocate internal resources.
 *         Must be called before any other OTA API. On failure, resources are
 *         cleaned up internally; the caller does not need to call ota_deinit().
 * @param  ctx:      OTA context (caller-allocated, zeroed by this call)
 * @param  host:     server hostname or IP (NULL for OTA_USER type)
 * @param  port:     server port (e.g. 80 for HTTP, 443 for HTTPS)
 * @param  resource: resource path on server (e.g. "/image/ota_all.bin")
 * @param  type:     transport type (OTA_HTTP / OTA_HTTPS / OTA_VFS / OTA_USER)
 * @retval OTA_OK / OTA_ERR
 */
int ota_init(ota_context_t *ctx, char *host, int port, char *resource, u8 type);

/**
 * @brief  Release all resources allocated by ota_init().
 *         Safe to call on a partially initialized context or after OTA_ERR.
 * @param  ctx: OTA context
 */
void ota_deinit(ota_context_t *ctx);

/**
 * @brief  Start OTA via transport (HTTP/VFS/USER): connect, download, program.
 *         Transport self-driven; caller provides no data.
 */
int ota_start(ota_context_t *ctx);

/**
 * @brief  Provide image metadata for the push path (ota_write / ota_finish).
 *         Must be called after ota_init() and before the first ota_write().
 *         The push path feeds header-stripped payload, so there is no OTA file
 *         header to parse; this supplies the target slot and length. Image
 *         integrity must be verified by the caller (no checksum is done here).
 *         Currently supports a single image per OTA session.
 * @param  ctx:      OTA context
 * @param  img_id:   image ID (e.g. OTA_IMGID_APP)
 * @param  img_len:  total image length including the leading manifest, in bytes
 * @retval OTA_OK / OTA_ERR (invalid args or image does not fit the target slot)
 */
int ota_set_image_info(ota_context_t *ctx, u32 img_id, u32 img_len);

/**
 * @brief  Write a chunk of header-stripped firmware payload into the OTA engine.
 *         Must be called after ota_set_image_info(). Use with ota_finish() as an
 *         alternative to ota_start() when the caller owns the data source
 *         (e.g. BLE GATT write, UART DMA). Must not be mixed with ota_start().
 *         The payload must start with the per-image manifest; if the data still
 *         carries an OTA file header it is rejected.
 * @param  ctx: OTA context
 * @param  buf: data buffer (must remain valid until this call returns)
 * @param  len: data length
 * @retval OTA_OK: continue | OTA_ERR: abort
 */
int ota_write(ota_context_t *ctx, const u8 *buf, int len);

/**
 * @brief  Read back the firmware content already written in the current push
 *         session, before ota_finish() commits the manifest. The written image
 *         spans image offset [0, ImageLen): the leading manifest is still held
 *         in RAM (not yet in flash), the remainder is the body already flashed.
 *         This call transparently sources the manifest region from RAM and the
 *         body region from flash, so the caller sees the contiguous image.
 *         Intended for read-back verification between ota_write() and
 *         ota_finish() (e.g. the caller computing its own hash/checksum).
 * @param  ctx:    OTA context
 * @param  offset: byte offset from the start of the image (0 = first manifest byte)
 * @param  buf:    destination buffer (at least len bytes)
 * @param  len:    number of bytes to read
 * @retval OTA_OK: success | OTA_ERR: invalid args or range past image length
 */
int ota_read(ota_context_t *ctx, int offset, u8 *buf, int len);

/**
 * @brief  Finalize OTA: commit the manifest to flash so the slot becomes
 *         bootable. Call after all data has been fed via ota_write() and the
 *         caller has verified image integrity. The manifest is written last for
 *         power-loss safety.
 * @param  ctx: OTA context
 * @retval OTA_OK: success | OTA_ERR: data incomplete or commit failed
 */
int ota_finish(ota_context_t *ctx);

/**
 * @brief  Register a download progress callback.
 *         Called with the current percentage (0-100) during ota_start().
 * @param  ctx: OTA context
 * @param  cb:  callback function; NULL to disable
 * @retval OTA_OK / OTA_ERR
 */
int ota_register_progress_cb(ota_context_t *ctx, ota_progress_cb_t cb);

/**
 * @brief  Register a user-defined read function (required for OTA_USER type).
 *         Called repeatedly by the transport layer to pull firmware bytes.
 * @param  ctx:  OTA context
 * @param  func: read function; signature: int func(u8 *buf, int len)
 * @retval OTA_OK / OTA_ERR
 */
int ota_register_user_read_func(ota_context_t *ctx, ota_user_read_func_t func);

/**
 * @brief  Register a user-defined open function (optional for OTA_USER type).
 *         Called once before the read loop begins.
 * @param  ctx:  OTA context
 * @param  func: open function; signature: int func(void)
 * @retval OTA_OK / OTA_ERR
 */
int ota_register_user_open_func(ota_context_t *ctx, ota_user_open_func_t func);

/**
 * @brief  Register a user-defined close function (optional for OTA_USER type).
 *         Called once after the read loop ends (success or failure).
 * @param  ctx:  OTA context
 * @param  func: close function; signature: int func(void)
 * @retval OTA_OK / OTA_ERR
 */
int ota_register_user_close_func(ota_context_t *ctx, ota_user_close_func_t func);

/**
 * @brief  Register a yield function called each loop iteration during ota_start().
 *         Use to feed the watchdog or yield to other tasks.
 * @param  ctx:  OTA context
 * @param  func: yield function; signature: void func(void)
 * @retval OTA_OK / OTA_ERR
 */
int ota_register_yield_func(ota_context_t *ctx, ota_yield_func_t func);

#ifdef __cplusplus
}
#endif

#endif //_OTA_API_H_
