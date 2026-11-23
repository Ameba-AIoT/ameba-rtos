/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_flash_dev.h"
#include "ota_internal.h"

/* IMG Flash Physical Address use for OTA */
extern u32 IMG_ADDR[][2];

/* Unified flash device for all OTA flash access (probed at init). */
static struct flash_dev *g_ota_dev = NULL;

/*
 * ota_write_ctx_t — the OTA storage layer's complete write state.
 *
 * The SAME page-buffered flow serves NOR and NAND, parameterised by the device
 * page_size / erase_size (NOR: 256 / 4 KB, NAND: 2-4 KB / 128-256 KB = 64 pages):
 *   - payload is coalesced into full pages before hitting flash;
 *   - each erase block is prepared (bad-block skip + erase) lazily, right before
 *     its first page is written (ensure_block_ready);
 *   - the first erase block of the image is buffered in RAM (head_buf: one page on
 *     NOR, one whole block on NAND) and NOT written during download.  Its page 0
 *     = [manifest 24B][image body ...] is programmed with the real manifest by
 *     ota_storage_update_manifest AFTER checksum passes.  NAND requires the pages
 *     of a block to be programmed in ascending order, so page 0 cannot be held back
 *     while pages 1..N of the same block are written: the whole block is held and
 *     then programmed page 0..N in order.
 *
 * Media contract this module relies on (see flash_dev backends):
 *   NOR : is_bad() always reports good, mark_bad() always fails, page_size |
 *         erase_size. The shared paths (prepare_block / verify bad-block skip)
 *         therefore degrade naturally to plain erase / plain read on NOR.
 *   NAND: provides real bad-block ops (FLASH_CAP_BADBLOCK).
 *   The ONLY path that MUST branch on media is ota_storage_update_manifest,
 *   marked with FLASH_CAP_BADBLOCK (greppable).
 *
 * Fields fall into three lifetimes:
 *   [device geometry] set once by ota_storage_init(), read-only afterwards;
 *                     page_buf is allocated here to page_size, head_buf to head_size.
 *   [first block]     manifest_addr is settled when the first good block is
 *                     prepared; head_buf content is committed by update_manifest.
 *   [write session]   reset by ota_storage_begin_image() for each image.
 */
typedef struct {
	/* ---- device geometry (set by ota_storage_init) ---- */
	u32  page_size;			/* program/read granularity (NOR 256, NAND 2-4 KB) */
	u32  erase_size;		/* minimum erase unit (NOR 4 KB, NAND 128 KB for 2 KB page, 256 KB for 4 KB page) */
	u8  *page_buf;			/* page coalescing buffer, size = page_size */

	/* ---- first block deferred write (manifest_addr settled at first good block) ---- */
	u32  head_size;			/* bytes held back in RAM: NOR page_size, NAND erase_size */
	u8  *head_buf;			/* image bytes [0, head_size); page 0 = [manifest 24B][body head] */
	u32  manifest_addr;		/* physical offset of page 0 = first usable block start */

	/* ---- write session (reset by ota_storage_begin_image) ---- */
	u32  slot_start;		/* target slot physical start (block aligned) */
	u32  slot_end;			/* slot physical end (exclusive) */
	u32  phys_write;		/* physical cursor for the next flash write */
	u8   write_started;		/* 0 = no block prepared yet; 1 = first block ready */
	u32  img_committed;		/* image bytes already committed to head_buf / flash (whole pages) */
	u32  page_buf_valid_len;	/* valid bytes in page_buf (0 .. page_size) */

	/* ---- read-back block map cache (NAND; ota_storage_read_image walks forward from here) ---- */
	u8   rd_valid;			/* 0 = rd_blk/rd_phys not set */
	u32  rd_blk;			/* logical block index (image offset / erase_size) */
	u32  rd_phys;			/* physical start of that block */
} ota_write_ctx_t;

static ota_write_ctx_t   s_wctx;

/**
 * @brief  Physical end address (exclusive) of an image's slot.
 */
static u32 ota_image_slot_end(u8 img_id, u8 slot_idx)
{
	u32 end = 0;
	u32 region;

	if (img_id == OTA_IMGID_BOOT) {
		region = (slot_idx == OTA_INDEX_1) ? IMG_BOOT : IMG_BOOT_OTA2;
#ifdef CONFIG_LINUX_FW_EN
	} else if (img_id == OTA_IMGID_APIMG) {
		region = (slot_idx == OTA_INDEX_1) ? IMG_APIMG_OTA1 : IMG_APIMG_OTA2;
#endif
#ifdef CONFIG_IOT_MCU_COMBINED
	} else if (img_id == OTA_IMGID_NP) {
		region = (slot_idx == OTA_INDEX_1) ? IMG_NP_OTA1 : IMG_NP_OTA2;
#endif
	} else {
		region = (slot_idx == OTA_INDEX_1) ? IMG_APP_OTA1 : IMG_APP_OTA2;
	}
	flash_get_layout_info(region, NULL, &end);
	if (end >= SPI_FLASH_BASE) {
		end -= SPI_FLASH_BASE;	/* to physical offset (NAND xip_base = 0, NOR strips base) */
	}
	return end;
}

/**
 * @brief  Make phys_write point at an erased, good block.  Skips bad blocks
 *         (advancing the cursor) and erases the chosen block; if an erase fails
 *         the block is marked bad and skipped too.  Called on a block boundary.
 *         Bounded by slot_end.  Pure geometry, shared by NOR and NAND (on NOR
 *         is_bad is always false and mark_bad always fails, so it degrades to a
 *         plain erase and a real error is surfaced).  Does NOT set manifest_addr
 *         (that is ensure_block_ready's job).
 */
static int prepare_block(ota_write_ctx_t *w)
{
	u8 bad = 0;

	while (1) {
		if (w->phys_write >= w->slot_end) {
			RTK_LOGE(OTA_TAG, "[STORAGE] slot exhausted at 0x%08x\n", (unsigned int)w->phys_write);
			return OTA_ERR;
		}
		if (flash_dev_is_bad(g_ota_dev, w->phys_write, &bad) == ERR_NONE && bad) {
			RTK_LOGD(OTA_TAG, "[STORAGE] skip bad block at 0x%08x\n", (unsigned int)w->phys_write);
			w->phys_write += w->erase_size;
			continue;
		}
		if (flash_dev_erase(g_ota_dev, w->phys_write, w->erase_size) == ERR_NONE) {
			return OTA_OK;
		}
		/* erase failed: on NAND mark the worn block bad and skip; on NOR this is
		 * a hard error (mark_bad not supported). */
		if (flash_dev_mark_bad(g_ota_dev, w->phys_write) == ERR_NONE) {
			RTK_LOGW(OTA_TAG, "[STORAGE] erase fail at 0x%08x, mark bad and skip\n", (unsigned int)w->phys_write);
			w->phys_write += w->erase_size;
			continue;
		}
		RTK_LOGE(OTA_TAG, "[STORAGE] erase fail at 0x%08x\n", (unsigned int)w->phys_write);
		return OTA_ERR;
	}
}

/**
 * @brief  Lazy block preparation (the single block-prep rule).  If phys_write
 *         sits on a block boundary (the first page of some block), skip bad
 *         blocks and erase that block; otherwise (a subsequent page inside a
 *         block already prepared) it is a no-op.  The first usable block also
 *         settles manifest_addr.  Called before writing every page (write_stream
 *         and flush), so no speculative erase-ahead is done: the tail block is
 *         never over-erased and "slot exhausted" is never mis-triggered when the
 *         image fills exactly to a block boundary.
 */
static int ensure_block_ready(ota_write_ctx_t *w)
{
	if ((w->phys_write % w->erase_size) != 0U) {
		return OTA_OK;			/* subsequent page inside an already-prepared block */
	}
	if (prepare_block(w) != OTA_OK) {	/* skip bad + erase, may advance phys_write */
		return OTA_ERR;
	}
	if (!w->write_started) {		/* first usable block -> anchor page 0 here */
		w->manifest_addr = w->phys_write;
		w->write_started = 1;
		RTK_LOGD(OTA_TAG, "[STORAGE] manifest_addr=0x%08x\n", (unsigned int)w->manifest_addr);
	}
	return OTA_OK;
}

/**
 * @brief  Commit one full page held in page_buf (page_buf now holds page_size
 *         bytes of content): prepare its block (the first page also settles
 *         manifest_addr) -> defer a page of the first block to head_buf or program
 *         the page -> advance the cursor and clear the buffer.  The single place the
 *         payload actually lands, shared by write_stream (a naturally full page) and
 *         flush (the last page padded with 0xFF); the NOR/NAND write is identical.
 *         The first block has no bad block inside, so its pages sit at
 *         head_buf[phys_write - manifest_addr]; later blocks start past head_size.
 */
static int commit_page(ota_write_ctx_t *w)
{
	if (ensure_block_ready(w) != OTA_OK) {
		return OTA_ERR;
	}
	if ((w->phys_write - w->manifest_addr) < w->head_size) {
		_memcpy(w->head_buf + (w->phys_write - w->manifest_addr), w->page_buf, w->page_size);	/* first block: defer */
	} else if (flash_dev_write(g_ota_dev, w->phys_write, w->page_buf, w->page_size) != ERR_NONE) {
		RTK_LOGE(OTA_TAG, "[STORAGE] page write fail at 0x%08x\n", (unsigned int)w->phys_write);
		return OTA_ERR;
	}
	w->phys_write        += w->page_size;
	w->img_committed     += w->page_size;
	w->page_buf_valid_len = 0;
	return OTA_OK;
}

int ota_storage_init(void)
{
	ota_write_ctx_t *w = &s_wctx;

	RTK_LOGI(OTA_TAG, "[STORAGE] Initializing...\n");

	/* Guard against a missing deinit from a prior session: free before re-alloc. */
	if (w->page_buf || w->head_buf) {
		ota_storage_deinit();
	}

	g_ota_dev = flash_dev_get(flash_dev_probe_media());
	if (g_ota_dev == NULL) {
		RTK_LOGE(OTA_TAG, "[STORAGE] flash device get fail\n");
		return OTA_ERR;
	}

	if (IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1] == 0) {
		flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
#ifdef CONFIG_LINUX_FW_EN
		flash_get_layout_info(IMG_APIMG_OTA1, &IMG_ADDR[OTA_IMGID_APIMG][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APIMG_OTA2, &IMG_ADDR[OTA_IMGID_APIMG][OTA_INDEX_2], NULL);
#endif
#ifdef CONFIG_IOT_MCU_COMBINED
		flash_get_layout_info(IMG_NP_OTA1, &IMG_ADDR[OTA_IMGID_NP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_NP_OTA2, &IMG_ADDR[OTA_IMGID_NP][OTA_INDEX_2], NULL);
#endif
	}
	RTK_LOGI(OTA_TAG, "[STORAGE] BOOT OTA1: 0x%08x, OTA2: 0x%08x\n",
			 (unsigned int)IMG_ADDR[OTA_IMGID_BOOT][0],
			 (unsigned int)IMG_ADDR[OTA_IMGID_BOOT][1]);
	RTK_LOGI(OTA_TAG, "[STORAGE] APP  OTA1: 0x%08x, OTA2: 0x%08x\n",
			 (unsigned int)IMG_ADDR[OTA_IMGID_APP][0],
			 (unsigned int)IMG_ADDR[OTA_IMGID_APP][1]);

	/* Device geometry: taken as-is from flash_dev (NOR page_size = 256). */
	w->page_size  = g_ota_dev->page_size;
	w->erase_size = g_ota_dev->erase_size;
	assert_param(w->page_size != 0U && (w->erase_size % w->page_size) == 0U);

	/* Persistent buffers (shared across begin_image calls).  NAND must program the pages of a
	 * block in ascending order, so it holds the whole first block back; NOR holds page 0 only. */
	w->head_size = (g_ota_dev->caps & FLASH_CAP_BADBLOCK) ? w->erase_size : w->page_size;
	w->page_buf  = (u8 *)rtos_mem_malloc(w->page_size);
	w->head_buf  = (u8 *)rtos_mem_malloc(w->head_size);
	if (!w->page_buf || !w->head_buf) {
		RTK_LOGE(OTA_TAG, "[STORAGE] buffer alloc fail (page_size=%u, head_size=%u)\n", (unsigned int)w->page_size, (unsigned int)w->head_size);
		ota_storage_deinit();		/* free the half that succeeded, clear state */
		return OTA_ERR;
	}

	RTK_LOGI(OTA_TAG, "[STORAGE] media=%s page_size=%u erase_size=%u, Init complete\n",
			 (g_ota_dev->caps & FLASH_CAP_BADBLOCK) ? "NAND" : "NOR",
			 (unsigned int)w->page_size, (unsigned int)w->erase_size);
	return OTA_OK;
}

void ota_storage_deinit(void)
{
	rtos_mem_free(s_wctx.page_buf);		/* rtos_mem_free(NULL) is a safe no-op */
	rtos_mem_free(s_wctx.head_buf);
	_memset(&s_wctx, 0, sizeof(s_wctx));	/* pointers cleared -> idempotent */
	g_ota_dev = NULL;
}

u32 ota_storage_get_image_addr(u8 img_id, u8 slot_idx)
{
	if (img_id >= OTA_IMGID_MAX || slot_idx >= 2) {
		RTK_LOGE(OTA_TAG, "[STORAGE] Invalid params: ID=%u, idx=%u\n", (unsigned int)img_id, (unsigned int)slot_idx);
		return 0;
	}
	return IMG_ADDR[img_id][slot_idx];
}

/**
 * @brief  Start writing a new image: reset the write-session fields and reserve
 *         the manifest placeholder.  page_size / erase_size / head_size / page_buf /
 *         head_buf belong to ota_storage_init() and are not touched here.
 */
void ota_storage_begin_image(u8 img_id, u8 slot_idx)
{
	ota_write_ctx_t *w = &s_wctx;
	u32 start = ota_storage_get_image_addr(img_id, slot_idx) - SPI_FLASH_BASE;	/* physical */

	/* Reset session fields only (a full memset would clear page_buf/head_buf). */
	w->slot_start     = start;
	w->slot_end       = ota_image_slot_end(img_id, slot_idx);
	w->phys_write     = start;
	w->manifest_addr  = 0;			/* settled when the first good block is ready */
	w->write_started  = 0;
	w->img_committed  = 0;
	w->rd_valid       = 0;

	/* Reserve the manifest bytes at the head of page 0 (body lands at offset 24). */
	_memset(w->page_buf, 0xFF, sizeof(ota_manifest_t));
	w->page_buf_valid_len = sizeof(ota_manifest_t);
	/* head_buf starts erased; update_manifest fills the real manifest later. */
	_memset(w->head_buf, 0xFF, w->head_size);

	RTK_LOGD(OTA_TAG, "[STORAGE] begin slot=0x%08x~0x%08x\n",
			 (unsigned int)w->slot_start, (unsigned int)w->slot_end);
}

/**
  * @brief  Check if target image length exceeds the layout
  * @param  hdr_mgr: Target header manager
  * @retval OTA_OK / OTA_ERR
  */
int ota_storage_check_layout(ota_hdr_manager_t *hdr_mgr)
{
	for (int index = 0; index < hdr_mgr->ValidImgCnt; index++) {
		u8 slot_idx = ota_get_target_slot(hdr_mgr->FileImgHdr[index].ImgID);
		u32 start_addr = IMG_ADDR[hdr_mgr->FileImgHdr[index].ImgID][slot_idx];
		u32 end_addr = ota_image_slot_end(hdr_mgr->FileImgHdr[index].ImgID, slot_idx) + SPI_FLASH_BASE;

		if ((end_addr - start_addr) < hdr_mgr->FileImgHdr[index].ImgLen) {
			RTK_LOGE(OTA_TAG, "ImgID: %lu, OTA%d start addr: 0x%08X, end addr: 0x%08X, OTA image Length(%d) > Layout(%d)!!!\n",
					 hdr_mgr->FileImgHdr[index].ImgID, slot_idx + 1, (unsigned int)start_addr, (unsigned int)end_addr, hdr_mgr->FileImgHdr[index].ImgLen,
					 (end_addr - start_addr));
			return OTA_ERR;
		}
	}

	return OTA_OK;
}

/**
 * @brief  Read one flash unit, treating a corrected bitflip (a "weak block":
 *         WARN/ERROR ECC status on NAND) as success -- its data is valid.  Only
 *         an uncorrectable read or a hard error means buf is unusable.  Shared
 *         by the verify read-back (read_image_page) and the image read
 *         (ota_storage_read_image).  NOR always returns ERR_NONE here.
 */
static int read_flash_checked(u32 offset, u8 *buf, u32 len)
{
	u8 rc = flash_dev_read(g_ota_dev, offset, buf, len);

	if (rc != ERR_NONE && rc != ERR_NAND_BITFLIP_WARN && rc != ERR_NAND_BITFLIP_ERROR) {
		RTK_LOGE(OTA_TAG, "[STORAGE] read fail (%d) at 0x%08x\n", rc, (unsigned int)offset);
		return OTA_ERR;
	}
	if (rc != ERR_NONE) {
		RTK_LOGW(OTA_TAG, "[STORAGE] weak block (corrected) at 0x%08x\n", (unsigned int)offset);
	}
	return OTA_OK;
}

/**
 * @brief  Read back one on-flash image page into buf and advance *phys_off by
 *         page_size.  Bad-block skip (at block boundaries) lives here under a
 *         FLASH_CAP_BADBLOCK guard (a no-op on NOR), so verify_checksum stays
 *         media-agnostic and only accumulates.  The skip is bounded by slot_end
 *         so trailing bad blocks cannot run past it.  A corrected bitflip (a
 *         "weak block": WARN/ERROR ECC status) still yields valid data and is
 *         NOT treated as a read failure; only an uncorrectable/hard error is.
 *         Such an error is reported directly (the block is NOT marked bad: a
 *         read-back ECC miss does not mean the block is unwritable, and
 *         prepare_block already marks blocks bad on erase failure).
 */
static int read_image_page(u32 *phys_off, u8 *buf)
{
	ota_write_ctx_t *w = &s_wctx;

	/* skip bad blocks at block boundaries (mirrors the write path; NOR never
	 * enters, is_bad reports good). */
	if ((*phys_off % w->erase_size) == 0U && (g_ota_dev->caps & FLASH_CAP_BADBLOCK)) {
		u8 bad = 0;
		while (*phys_off < w->slot_end &&
			   flash_dev_is_bad(g_ota_dev, *phys_off, &bad) == ERR_NONE && bad) {
			RTK_LOGD(OTA_TAG, "[STORAGE] verify: skip bad block at 0x%08x\n", (unsigned int)*phys_off);
			*phys_off += w->erase_size;
		}
		if (*phys_off >= w->slot_end) {
			RTK_LOGE(OTA_TAG, "[STORAGE] verify: slot exhausted at 0x%08x\n", (unsigned int)*phys_off);
			return OTA_ERR;
		}
	}
	/* read the page (a corrected weak block still yields valid data) */
	if (read_flash_checked(*phys_off, buf, w->page_size) != OTA_OK) {
		return OTA_ERR;
	}
	*phys_off += w->page_size;
	return OTA_OK;
}

/**
  * @brief  Verify checksum (called BEFORE the manifest is committed to flash).
  * @param  hdr_mgr: Target header manager
  * @param  slot_idx: OTA slot index (0=OTA1, 1=OTA2)
  * @param  hdr_idx: Header index
  * @retval OTA_ERR: verify fail, OTA_OK: verify ok
  *
  * Same path for NOR and NAND: the 24B manifest and the body bytes of the first
  * block (head_buf: one page on NOR, one block on NAND) are summed from RAM (they
  * are not yet on flash); the rest is read back from flash page-aligned into
  * page_buf (free now that writing is done), skipping bad blocks under a
  * FLASH_CAP_BADBLOCK guard (a no-op on NOR).
  */
int ota_storage_verify_checksum(ota_hdr_manager_t *hdr_mgr, u8 slot_idx, u8 hdr_idx)
{
	ota_write_ctx_t *w = &s_wctx;
	u32 i;
	u32 flash_checksum = 0;
	u32 addr;
	u32 body_len;
	u32 head_body;
	u32 remaining;
	u32 phys_off;
	ota_manifest_t *manifest;

	addr     = IMG_ADDR[hdr_mgr->FileImgHdr[hdr_idx].ImgID][slot_idx];
	manifest = &hdr_mgr->Manifest[hdr_idx];
	body_len = hdr_mgr->FileImgHdr[hdr_idx].ImgLen - sizeof(ota_manifest_t);

	assert_param((w->manifest_addr % w->erase_size) == 0U);

	/* manifest bytes come from RAM (written to flash only after verify passes) */
	for (i = 0; i < sizeof(ota_manifest_t); i++) {
		flash_checksum += *((u8 *)manifest + i);
	}

	/* the first block's body is still buffered in RAM (not yet programmed) */
	head_body = ((w->head_size - sizeof(ota_manifest_t)) < body_len) ?
				(w->head_size - sizeof(ota_manifest_t)) : body_len;
	for (i = 0; i < head_body; i++) {
		flash_checksum += w->head_buf[sizeof(ota_manifest_t) + i];
	}

	/* the rest is on flash; read back page-aligned via read_image_page,
	 * which owns all media specifics (reuse page_buf as the read-back buffer) */
	remaining = body_len - head_body;
	phys_off  = w->manifest_addr + w->head_size;

	while (remaining > 0U) {
		u32 valid = (remaining < w->page_size) ? remaining : w->page_size;

		if (read_image_page(&phys_off, w->page_buf) != OTA_OK) {
			return OTA_ERR;
		}
		for (i = 0U; i < valid; i++) {
			flash_checksum += w->page_buf[i];
		}
		remaining -= valid;
	}

	if (flash_checksum != hdr_mgr->FileImgHdr[hdr_idx].Checksum) {
		RTK_LOGE(OTA_TAG, "OTA image(%08x) checksum error!!!\nCalculated checksum 0x%8x, host checksum 0x%8x\n",
				 (unsigned int)addr, (unsigned int)flash_checksum, (unsigned int)hdr_mgr->FileImgHdr[hdr_idx].Checksum);
		return OTA_ERR;
	}
	RTK_LOGI(OTA_TAG, "OTA image(%08x) checksum ok!!!\n", (unsigned int)addr);
	return OTA_OK;
}

/**
 * @brief  Append image body bytes.  Storage owns the physical cursor and all
 *         placement: page coalescing, lazy erase and bad-block skipping happen
 *         here.  Call ota_storage_flush() after the last append.
 */
int ota_storage_write_stream(const u8 *buf, u32 len)
{
	ota_write_ctx_t *w = &s_wctx;
	u32 src = 0;

	if (!buf || len == 0) {
		return OTA_ERR;
	}

	while (src < len) {
		u32 cp = w->page_size - w->page_buf_valid_len;
		if (cp > (len - src)) {
			cp = len - src;
		}
		_memcpy(w->page_buf + w->page_buf_valid_len, buf + src, cp);
		w->page_buf_valid_len += cp;
		src                   += cp;

		if (w->page_buf_valid_len == w->page_size) {
			if (commit_page(w) != OTA_OK) {		/* full page: prepare block + land it */
				return OTA_ERR;
			}
		}
	}
	return OTA_OK;
}

/**
 * @brief  Flush the last buffered page (padding with 0xFF).  No-op when the
 *         buffer is empty.  Call before verify.
 */
int ota_storage_flush(void)
{
	ota_write_ctx_t *w = &s_wctx;

	if (w->page_buf_valid_len == 0U) {
		return OTA_OK;
	}

	_memset(w->page_buf + w->page_buf_valid_len, 0xFF, w->page_size - w->page_buf_valid_len);
	RTK_LOGD(OTA_TAG, "[STORAGE] flush last page at 0x%08x (%u valid bytes)\n",
			 (unsigned int)w->phys_write, (unsigned int)w->page_buf_valid_len);

	/* last page is now full (padded with 0xFF); commit_page's ensure_block_ready
	 * also covers the case where it is the first page of a new block. */
	return commit_page(w);
}

/**
 * @brief  Physical start of the blk-th block of the image (logical index, 0 = the
 *         first block at manifest_addr).  Skips bad blocks like the write path does
 *         (prepare_block) and verify does (read_image_page).  Sequential reads resume
 *         from the last mapped block instead of walking from the start.  NOR has no
 *         bad blocks: the mapping is linear.
 */
static int image_block_phys(ota_write_ctx_t *w, u32 blk, u32 *phys)
{
	u32 cur_blk = 0;
	u32 cur = w->manifest_addr;
	u8 bad = 0;

	if (!(g_ota_dev->caps & FLASH_CAP_BADBLOCK)) {
		*phys = w->manifest_addr + blk * w->erase_size;
		return OTA_OK;
	}
	if (w->rd_valid && w->rd_blk <= blk) {
		cur_blk = w->rd_blk;
		cur     = w->rd_phys;
	}
	while (cur_blk < blk) {
		cur += w->erase_size;
		cur_blk++;
		while (cur < w->slot_end && flash_dev_is_bad(g_ota_dev, cur, &bad) == ERR_NONE && bad) {
			cur += w->erase_size;
		}
		if (cur >= w->slot_end) {
			RTK_LOGE(OTA_TAG, "[STORAGE] read: slot exhausted at block %u\n", (unsigned int)blk);
			return OTA_ERR;
		}
	}
	w->rd_valid = 1;
	w->rd_blk   = cur_blk;
	w->rd_phys  = cur;
	*phys = cur;
	return OTA_OK;
}

/**
 * @brief  Read back bytes [offset, offset + len) of the image being written, before
 *         the manifest is committed.  offset 0 is the first byte of the manifest.
 *         The bytes live in three places and are stitched together here: the held-back
 *         first block (head_buf), flash (past the first block, bad blocks skipped), and
 *         the not yet full last page (page_buf).  head_buf[0, 24) is still the 0xFF
 *         placeholder: the real manifest is only known to the caller.  Fails if the
 *         range reaches past the bytes written so far.
 */
int ota_storage_read_image(u32 offset, u8 *data, u32 len)
{
	ota_write_ctx_t *w = &s_wctx;
	u32 written = w->img_committed + w->page_buf_valid_len;
	u32 done = 0;
	u8 *scratch = NULL;
	int ret = OTA_OK;

	if (!data || len == 0U || offset > written || len > written - offset) {
		RTK_LOGE(OTA_TAG, "[STORAGE] read: range [%u, +%u) exceeds %u bytes written\n", (unsigned int)offset, (unsigned int)len, (unsigned int)written);
		return OTA_ERR;
	}

	while (done < len) {
		u32 cur   = offset + done;
		u32 chunk = len - done;

		if (cur >= w->img_committed) {
			/* last page, still being coalesced */
			_memcpy(data + done, w->page_buf + (cur - w->img_committed), chunk);
		} else if (cur < w->head_size) {
			/* first block, held back in RAM */
			u32 end = (w->img_committed < w->head_size) ? w->img_committed : w->head_size;

			chunk = (chunk < end - cur) ? chunk : (end - cur);
			_memcpy(data + done, w->head_buf + cur, chunk);
		} else {
			/* on flash: one block at a time (physical blocks are only contiguous inside a block) */
			u32 in_blk = cur % w->erase_size;
			u32 phys;
			u8 partial = 0;

			chunk = (chunk < w->img_committed - cur) ? chunk : (w->img_committed - cur);
			chunk = (chunk < w->erase_size - in_blk) ? chunk : (w->erase_size - in_blk);
			if (image_block_phys(w, cur / w->erase_size, &phys) != OTA_OK) {
				ret = OTA_ERR;
				break;
			}
			phys += in_blk;

			/* NAND: flash_dev_read is single-page and page-aligned, stage a partial page */
			if (g_ota_dev->caps & FLASH_CAP_BADBLOCK) {
				u32 in_page = phys % w->page_size;

				chunk   = (chunk < w->page_size - in_page) ? chunk : (w->page_size - in_page);
				partial = (in_page != 0U || chunk != w->page_size);
			}
			if (!partial) {
				if (read_flash_checked(phys, data + done, chunk) != OTA_OK) {
					ret = OTA_ERR;
					break;
				}
			} else {
				if (scratch == NULL) {
					scratch = (u8 *)rtos_mem_malloc(w->page_size);
					if (scratch == NULL) {
						RTK_LOGE(OTA_TAG, "[STORAGE] read scratch alloc fail\n");
						ret = OTA_ERR;
						break;
					}
				}
				if (read_flash_checked(phys - (phys % w->page_size), scratch, w->page_size) != OTA_OK) {
					ret = OTA_ERR;
					break;
				}
				_memcpy(data + done, scratch + (phys % w->page_size), chunk);
			}
		}
		done += chunk;
	}

	rtos_mem_free(scratch);
	return ret;
}

/* NAND: program the held-back first block, page 0 .. N in ascending order (NAND
 * requires that order inside a block, and a page can only be programmed once).  Only
 * the pages that carry image bytes are written.  A cut before the last page leaves a
 * valid-looking page 0 with missing pages behind it.  That slot is only rejected if the
 * loader checks the image (the APP is verified by the bootloader; the ROM does not hash
 * the bootloader unless secure boot is on), otherwise the boot-count fallback applies.
 * A failed page cannot be redone elsewhere (the following pages are tied to this
 * block), so the OTA fails. */
static int update_manifest_nand(ota_write_ctx_t *w)
{
	u32 len = (w->img_committed < w->head_size) ? w->img_committed : w->head_size;
	u32 off;

	for (off = 0; off < len; off += w->page_size) {
		if (flash_dev_write(g_ota_dev, w->manifest_addr + off, w->head_buf + off, w->page_size) != ERR_NONE) {
			RTK_LOGE(OTA_TAG, "[STORAGE] first block write fail at 0x%08x\n", (unsigned int)(w->manifest_addr + off));
			return OTA_ERR;
		}
	}
	return OTA_OK;
}

/* NOR: byte-writable, so write the body first and the Pattern (the bootloader
 * validity marker) last, making the slot commit power-safe.  head_buf is one page. */
static int update_manifest_nor(ota_write_ctx_t *w)
{
	if (flash_dev_write(g_ota_dev, w->manifest_addr + OTA_MANIFEST_PATTERN_LEN,
						w->head_buf + OTA_MANIFEST_PATTERN_LEN, w->page_size - OTA_MANIFEST_PATTERN_LEN) != ERR_NONE ||
		flash_dev_write(g_ota_dev, w->manifest_addr,
						w->head_buf, OTA_MANIFEST_PATTERN_LEN) != ERR_NONE) {
		RTK_LOGE(OTA_TAG, "[STORAGE] manifest write fail\n");
		return OTA_ERR;
	}
	return OTA_OK;
}

/**
  * @brief  Commit the manifest (called only AFTER checksum passes): program the
  *         deferred first block (NAND) / page 0 (NOR) = [manifest 24B][body ...],
  *         for the first and only time.  The explicit NOR/NAND fork is dispatched
  *         here to update_manifest_nor / update_manifest_nand.
  * @param  hdr_mgr: Target header manager
  * @param  slot_idx: OTA slot index (0=OTA1, 1=OTA2)
  * @param  hdr_idx: Header index
  * @retval OTA_ERR / OTA_OK
  */
int ota_storage_update_manifest(ota_hdr_manager_t *hdr_mgr, u8 slot_idx, u8 hdr_idx)
{
	ota_write_ctx_t *w = &s_wctx;
	ota_manifest_t *manifest = NULL;
	int ret;

#if defined(CONFIG_OTA_SWITCH_BY_VALID_HEADER) && (CONFIG_OTA_SWITCH_BY_VALID_HEADER == 1)
	u8 ota_old_index = slot_idx ^ 1;
	u8 empty_sig[8] = {0x0};
	u32 old_addr;
#else
	UNUSED(slot_idx);
#endif

	manifest = &hdr_mgr->Manifest[hdr_idx];

#if defined(CONFIG_OTA_SWITCH_BY_VALID_HEADER) && (CONFIG_OTA_SWITCH_BY_VALID_HEADER == 1)
	/* Clearing the old Pattern re-programs an already programmed page, which NAND cannot do: refuse before anything is committed */
	if (g_ota_dev->caps & FLASH_CAP_BADBLOCK) {
		RTK_LOGE(OTA_TAG, "[STORAGE] Valid Header switch is not supported on NAND\n");
		return OTA_ERR;
	}
#endif

	RTK_LOGI(OTA_TAG, "[STORAGE] update manifest at 0x%08x, version major: %x, minor: %x\n",
			 (unsigned int)w->manifest_addr, manifest->MajorKeyVer, manifest->MinorKeyVer);

	_memcpy(w->head_buf, (u8 *)manifest, sizeof(ota_manifest_t));

	/* the only explicit media fork: commit the first block (NAND) / page 0 (NOR) */
	ret = (g_ota_dev->caps & FLASH_CAP_BADBLOCK) ? update_manifest_nand(w) : update_manifest_nor(w);
	if (ret != OTA_OK) {
		return OTA_ERR;
	}

#if defined(CONFIG_OTA_SWITCH_BY_VALID_HEADER) && (CONFIG_OTA_SWITCH_BY_VALID_HEADER == 1)
	if (strncmp("OTA", (const char *)hdr_mgr->FileImgHdr[hdr_idx].Signature, 3) != 0) {
		return OTA_ERR;
	}

	/*
	 * Clear the old image Pattern (write 8B of 0 to the old page-0 head).
	 * WARNING (NAND): this is a partial rewrite (second program) of an already
	 * programmed NAND page and would corrupt its OOB ECC.  This path is only
	 * semantically correct on NOR; NAND is rejected above, before the manifest is committed.
	 */

	old_addr = IMG_ADDR[hdr_mgr->FileImgHdr[hdr_idx].ImgID][ota_old_index];
	RTK_LOGI(OTA_TAG, "[STORAGE] clear old image pattern, addr: 0x%08x\n", (unsigned int)old_addr);
	flash_dev_write(g_ota_dev, old_addr - SPI_FLASH_BASE, empty_sig, sizeof(empty_sig));
#endif

	RTK_LOGI(OTA_TAG, "[STORAGE] Update OTA success!\n");
	return OTA_OK;
}
