/**
  ******************************************************************************
  * @file    lbm_core.c
  * @brief   LBM - Logical Block Mapping: thin flash-translation layer for
  *          littlefs over SPI NAND (Tier 1).
  *
  * Single volume, synchronous, no background task, no wear leveling.
  * Responsibilities: logical<->physical block mapping, factory/runtime
  * bad-block masking, transparent bad-block replacement. Power-loss safety of
  * file-system operations is provided by littlefs above; this layer only
  * guarantees that each physical block carries a valid meta/block header and
  * that bad blocks never surface.
  *
  * Bad-block policy (write/program failure) is "mark-bad-now" (design option B):
  * a program failure permanently retires the block and littlefs relocates.
  * Erase failure is auto-marked bad by the NAND HAL.  See the thin-FTL design
  * document under component/file_system/ (sections 7, 11.1).
  ******************************************************************************
  */

#include "platform_autoconf.h"
#include <string.h>
#include "littlefs_adapter.h"   /* LFS_FLASH_BASE_ADDR / _SIZE, NAND_ADDR_TO_PAGE_ADDR */
#include "vfs_nand_ftl.h"       /* NAND_FTL_* , UERR_* , HAL_OK */
#include "os_wrapper_memory.h"  /* rtos_mem_zmalloc / rtos_mem_free */
#include "lbm.h"

/* page-size/pages-per-block published by NAND_FTL_Init() */
extern u32 vfs_nand_flash_pagesize;
extern u32 vfs_nand_flash_pagenum;

#define LBM_WRITEPAGE(ctxp, page, buf, e) NAND_FTL_WritePage((page), (buf), (e))
#define LBM_ERASEBLOCK(ctxp, page, e)     NAND_FTL_EraseBlock((page), (e))

/* ---- small helpers ---------------------------------------------------- */

/* Clean-room CRC32 (standard reflected poly 0xEDB88320), header integrity only. */
static uint32_t lbm_crc32(const void *data, uint32_t len)
{
	const uint8_t *p = (const uint8_t *)data;
	uint32_t crc = 0xFFFFFFFFUL;
	uint32_t i;
	int b;

	for (i = 0; i < len; i++) {
		crc ^= p[i];
		for (b = 0; b < 8; b++) {
			crc = (crc >> 1) ^ (0xEDB88320UL & (0U - (crc & 1U)));
		}
	}
	return crc ^ 0xFFFFFFFFUL;
}

/* A NAND read whose data is usable: clean or ECC-corrected (worn but valid). */
static int lbm_read_ok(u8 r)
{
	return (r == HAL_OK) || (r == UERR_NAND_BITFLIP_WARN) || (r == UERR_NAND_BITFLIP_ERROR);
}

static int lbm_is_blank(const uint8_t *b, uint32_t n)
{
	uint32_t i;
	for (i = 0; i < n; i++) {
		if (b[i] != 0xFFU) {
			return 0;
		}
	}
	return 1;
}

static uint32_t lbm_block_page0(lbm_ctx_t *ctx, uint32_t pblk)
{
	return ctx->part_base_page + pblk * ctx->block_pages;
}

/* Write the meta header to page 0 of pblk (page must be erased beforehand). */
static int lbm_write_meta_hdr(lbm_ctx_t *ctx, uint32_t pblk)
{
	lbm_meta_hdr_t *h = (lbm_meta_hdr_t *)ctx->pagebuf;

	memset(ctx->pagebuf, 0xFF, ctx->page_size);
	h->magic = LBM_META_HDR_MAGIC;
	h->version = LBM_HDR_VERSION;
	h->ec = (uint64_t)ctx->ec[pblk];
	h->blk_hdr_page = 1;
	h->data_page = 2;
	h->hdr_crc = lbm_crc32(h, sizeof(*h) - sizeof(h->hdr_crc));

	return (LBM_WRITEPAGE(ctx, lbm_block_page0(ctx, pblk), ctx->pagebuf, 0) == HAL_OK)
		   ? LBM_OK : LBM_ERR_BAD;
}

/* Write the block header to page 1 of pblk (commits ownership of lblk). */
static int lbm_write_blk_hdr(lbm_ctx_t *ctx, uint32_t pblk, uint32_t lblk)
{
	lbm_blk_hdr_t *h = (lbm_blk_hdr_t *)ctx->pagebuf;

	memset(ctx->pagebuf, 0xFF, ctx->page_size);
	h->magic = LBM_BLK_HDR_MAGIC;
	h->version = LBM_HDR_VERSION;
	h->copy_flag = 0;            /* Tier1: no WL relocation */
	h->lblk = lblk;
	h->seq = ctx->global_seq++;
	h->data_crc = 0xFFFFFFFFUL;  /* Tier1: unused */
	h->hdr_crc = lbm_crc32(h, sizeof(*h) - sizeof(h->hdr_crc));

	return (LBM_WRITEPAGE(ctx, lbm_block_page0(ctx, pblk) + 1, ctx->pagebuf, 0) == HAL_OK)
		   ? LBM_OK : LBM_ERR_BAD;
}

/* Retire a physical block: mark its OOB bad and drop it from the usable pool. */
static void lbm_retire_block(lbm_ctx_t *ctx, uint32_t pblk)
{
	NAND_FTL_MarkBad(lbm_block_page0(ctx, pblk));
	ctx->state[pblk] = LBM_BLK_BAD;
	ctx->bad_count++;
}

/* Erase pblk and (re)write its meta header so it becomes a clean FREE block.
 * On any failure the block is retired. Returns LBM_OK / LBM_ERR_BAD. */
static int lbm_make_free(lbm_ctx_t *ctx, uint32_t pblk, uint32_t new_ec)
{
	u8 er = LBM_ERASEBLOCK(ctx, lbm_block_page0(ctx, pblk), 0);

	if (er != HAL_OK) {
		/* HAL auto-marks bad on UERR_NAND_WORN_BLOCK; mark explicitly otherwise. */
		if (er != UERR_NAND_WORN_BLOCK) {
			NAND_FTL_MarkBad(lbm_block_page0(ctx, pblk));
		}
		ctx->state[pblk] = LBM_BLK_BAD;
		ctx->bad_count++;
		return LBM_ERR_BAD;
	}

	ctx->ec[pblk] = new_ec;
	if (lbm_write_meta_hdr(ctx, pblk) != LBM_OK) {
		lbm_retire_block(ctx, pblk);
		return LBM_ERR_BAD;
	}
	ctx->state[pblk] = LBM_BLK_FREE;
	return LBM_OK;
}

/* Pick an allocatable physical block (FREE preferred, else DIRTY prepared via
 * erase). Returns a pblk that is FREE (meta header present, page1 blank), or -1. */
static int lbm_alloc_block(lbm_ctx_t *ctx)
{
	uint32_t k, pblk;

	/* Keep a spare pool for bad-block replacement, but let blocks that are
	 * ALREADY bad be absorbed by the reserve, so the fixed usable_blocks
	 * (= total - reserved) stays fully allocatable regardless of bad count.
	 * Refuse only when the free pool would drop below (reserved - bad), i.e.
	 * avail + bad < reserved.  (bad=0 -> identical to avail < reserved.)
	 * Max mappable = good - (reserved - bad) = total - reserved = usable_blocks. */
	if (ctx->avail_count + ctx->bad_count < ctx->reserved_blocks) {
		return -1;
	}

	/* pass 1: a ready FREE block */
	for (k = 0; k < ctx->total_blocks; k++) {
		pblk = (ctx->alloc_cursor + k) % ctx->total_blocks;
		if (ctx->state[pblk] == LBM_BLK_FREE) {
			ctx->alloc_cursor = (pblk + 1) % ctx->total_blocks;
			return (int)pblk;
		}
	}

	/* pass 2: a DIRTY block, prepare it (erase + meta header) */
	for (k = 0; k < ctx->total_blocks; k++) {
		pblk = (ctx->alloc_cursor + k) % ctx->total_blocks;
		if (ctx->state[pblk] == LBM_BLK_DIRTY) {
			/* reclaim physically erases the block -> EC must increment,
			 * same as lbm_block_erase, so EC tracks real erase count. */
			if (lbm_make_free(ctx, pblk, ctx->ec[pblk] + 1) == LBM_OK) {
				ctx->alloc_cursor = (pblk + 1) % ctx->total_blocks;
				return (int)pblk;
			}
			/* prepare failed -> block retired inside lbm_make_free */
			ctx->avail_count--;   /* was DIRTY (allocatable), now BAD */
		}
	}

	return -1;
}

/* ---- attach scan ------------------------------------------------------ */

int lbm_init(lbm_ctx_t *ctx)
{
	uint32_t pblk, lblk;
	uint64_t *tmp_seq;

	if (ctx->inited) {
		return LBM_OK;   /* idempotent: tables persist across remount */
	}

	ctx->page_size = vfs_nand_flash_pagesize;
	ctx->block_pages = vfs_nand_flash_pagenum;
	if ((ctx->page_size == 0) || (ctx->block_pages < 3)) {
		VFS_DBG(VFS_ERROR, "lbm: bad geometry page=%u pages=%u", ctx->page_size, ctx->block_pages);
		return LBM_ERR_INIT;
	}

	ctx->lblk_size = (ctx->block_pages - 2) * ctx->page_size;
	ctx->part_base_page = NAND_ADDR_TO_PAGE_ADDR(LFS_FLASH_BASE_ADDR);
	ctx->total_blocks = LFS_FLASH_SIZE / (ctx->page_size * ctx->block_pages);
	if (ctx->total_blocks == 0) {
		VFS_DBG(VFS_ERROR, "lbm: VFS partition smaller than one block");
		return LBM_ERR_INIT;
	}

	/* Fixed spare pool for bad-block replacement.  Sized to cover the chip's
	 * documented bad-block budget (e.g. ~20 bad eraseblocks per 1024) plus a
	 * working margin.  This reserve is FIXED: usable_blocks below is
	 * total - reserved and does NOT subtract the actual bad count, so the
	 * littlefs block_count stays constant across chips and across runtime
	 * bad-block events.  (A changing block_count fails littlefs mount with
	 * LFS_ERR_INVAL -- see lfs.c superblock check.) */
	ctx->reserved_blocks = ctx->total_blocks * 2 / 100;
	if (ctx->reserved_blocks < 2) {
		ctx->reserved_blocks = 2;
	}

	ctx->map = (int32_t *)rtos_mem_zmalloc(ctx->total_blocks * sizeof(int32_t));
	ctx->ec = (uint32_t *)rtos_mem_zmalloc(ctx->total_blocks * sizeof(uint32_t));
	ctx->state = (uint8_t *)rtos_mem_zmalloc(ctx->total_blocks * sizeof(uint8_t));
	ctx->pagebuf = (uint8_t *)rtos_mem_zmalloc(ctx->page_size);
	tmp_seq = (uint64_t *)rtos_mem_zmalloc(ctx->total_blocks * sizeof(uint64_t));
	if (!ctx->map || !ctx->ec || !ctx->state || !ctx->pagebuf || !tmp_seq) {
		VFS_DBG(VFS_ERROR, "lbm: out of memory (total_blocks=%u)", ctx->total_blocks);
		if (tmp_seq) {
			rtos_mem_free(tmp_seq);
		}
		lbm_deinit(ctx);
		return LBM_ERR_INIT;
	}

	memset(ctx->map, 0xFF, ctx->total_blocks * sizeof(int32_t));  /* all -1 (unmapped) */
	/* ec[]=0, state[]=LBM_BLK_DIRTY(0) from zmalloc */

	ctx->global_seq = 0;
	ctx->bad_count = 0;
	ctx->avail_count = 0;
	ctx->alloc_cursor = 0;

	for (pblk = 0; pblk < ctx->total_blocks; pblk++) {
		u8 r = NAND_FTL_ReadPage(lbm_block_page0(ctx, pblk), ctx->pagebuf);
		lbm_meta_hdr_t *mh;
		lbm_blk_hdr_t *bh;

		if (r == UERR_NAND_BAD_BLOCK) {
			ctx->state[pblk] = LBM_BLK_BAD;
			ctx->bad_count++;
			continue;
		}
		if (!lbm_read_ok(r)) {
			ctx->state[pblk] = LBM_BLK_DIRTY;   /* unreadable header -> erase before use */
			ctx->avail_count++;
			continue;
		}

		mh = (lbm_meta_hdr_t *)ctx->pagebuf;
		if ((mh->magic != LBM_META_HDR_MAGIC) ||
			(mh->hdr_crc != lbm_crc32(mh, sizeof(*mh) - sizeof(mh->hdr_crc)))) {
			ctx->state[pblk] = LBM_BLK_DIRTY;   /* blank or foreign */
			ctx->avail_count++;
			continue;
		}
		ctx->ec[pblk] = (uint32_t)mh->ec;

		r = NAND_FTL_ReadPage(lbm_block_page0(ctx, pblk) + 1, ctx->pagebuf);
		if (!lbm_read_ok(r)) {
			ctx->state[pblk] = LBM_BLK_DIRTY;
			ctx->avail_count++;
			continue;
		}

		bh = (lbm_blk_hdr_t *)ctx->pagebuf;
		if ((bh->magic != LBM_BLK_HDR_MAGIC) ||
			(bh->hdr_crc != lbm_crc32(bh, sizeof(*bh) - sizeof(bh->hdr_crc)))) {
			/* meta valid, no valid block header: clean free block if page1 blank, else dirty */
			ctx->state[pblk] = lbm_is_blank(ctx->pagebuf, ctx->page_size)
							   ? LBM_BLK_FREE : LBM_BLK_DIRTY;
			ctx->avail_count++;
			continue;
		}

		lblk = bh->lblk;
		if (lblk >= ctx->total_blocks) {
			ctx->state[pblk] = LBM_BLK_DIRTY;
			ctx->avail_count++;
			continue;
		}

		if (ctx->map[lblk] != -1) {
			/* two physical blocks claim the same lblk (power-loss residue): keep higher seq */
			if (bh->seq > tmp_seq[lblk]) {
				uint32_t old = (uint32_t)ctx->map[lblk];
				ctx->state[old] = LBM_BLK_DIRTY;   /* loser keeps stale header -> needs erase */
				ctx->avail_count++;
				ctx->map[lblk] = (int32_t)pblk;
				tmp_seq[lblk] = bh->seq;
				ctx->state[pblk] = LBM_BLK_USED;
			} else {
				ctx->state[pblk] = LBM_BLK_DIRTY;
				ctx->avail_count++;
			}
		} else {
			ctx->map[lblk] = (int32_t)pblk;
			tmp_seq[lblk] = bh->seq;
			ctx->state[pblk] = LBM_BLK_USED;
		}

		if (bh->seq > ctx->global_seq) {
			ctx->global_seq = bh->seq;
		}
	}

	rtos_mem_free(tmp_seq);
	ctx->global_seq += 1;

	/* usable_blocks is FIXED (total - reserved), independent of bad_count, so
	 * the littlefs block_count is stable across chips and reboots.  Bad blocks
	 * are absorbed by the reserve pool, not by shrinking the address space. */
	if (ctx->total_blocks > ctx->reserved_blocks) {
		ctx->usable_blocks = ctx->total_blocks - ctx->reserved_blocks;
		if (ctx->usable_blocks < 4) {
			VFS_DBG(VFS_ERROR, "lbm: usable blocks too few (%u < 4, total=%u reserved=%u) - enlarge VFS1",
					ctx->usable_blocks, ctx->total_blocks, ctx->reserved_blocks);
			lbm_deinit(ctx);
			return LBM_ERR_INIT;
		}
	} else {
		ctx->usable_blocks = 0;
		VFS_DBG(VFS_ERROR, "lbm: no usable block (total=%u reserved=%u) - enlarge VFS1",
				ctx->total_blocks, ctx->reserved_blocks);
	}
	/* If bad blocks already exceed the reserve, some logical blocks may not be
	 * mappable (writes will hit NOSPACE), but block_count stays fixed so the
	 * volume never silently shrinks / bricks on mount. */
	if (ctx->bad_count > ctx->reserved_blocks) {
		VFS_DBG(VFS_ERROR, "lbm: bad(%u) exceeds reserve(%u): pool may be over-committed",
				ctx->bad_count, ctx->reserved_blocks);
	}

	ctx->inited = 1;
	VFS_DBG(VFS_INFO, "lbm: part LFS_BASE=0x%08x LFS_SIZE=0x%08x base_page=0x%08x",
			(unsigned int)LFS_FLASH_BASE_ADDR, (unsigned int)LFS_FLASH_SIZE, ctx->part_base_page);
	VFS_DBG(VFS_INFO, "lbm: geom total_blocks=%u block_pages=%u page_size=%u lblk_size=%u",
			ctx->total_blocks, ctx->block_pages, ctx->page_size, ctx->lblk_size);
	VFS_DBG(VFS_INFO, "lbm: pool reserved=%u bad=%u avail=%u usable=%u seq=%u",
			ctx->reserved_blocks, ctx->bad_count, ctx->avail_count, ctx->usable_blocks, (unsigned int)ctx->global_seq);
	return LBM_OK;
}

void lbm_deinit(lbm_ctx_t *ctx)
{
	if (ctx->map) {
		rtos_mem_free(ctx->map);
		ctx->map = NULL;
	}
	if (ctx->ec) {
		rtos_mem_free(ctx->ec);
		ctx->ec = NULL;
	}
	if (ctx->state) {
		rtos_mem_free(ctx->state);
		ctx->state = NULL;
	}
	if (ctx->pagebuf) {
		rtos_mem_free(ctx->pagebuf);
		ctx->pagebuf = NULL;
	}
	ctx->inited = 0;
}

/* ---- logical block read/write/erase ---------------------------------- */

int lbm_block_read(lbm_ctx_t *ctx, int lblk, void *buf, uint32_t off, uint32_t len)
{
	int32_t pblk;
	uint32_t base_data, done = 0;

	if ((lblk < 0) || ((uint32_t)lblk >= ctx->usable_blocks)) {
		return LBM_ERR_IO;
	}
	pblk = ctx->map[lblk];
	if (pblk < 0) {
		/* never written / erased: behave like erased NAND (0xFF) */
		memset(buf, 0xFF, len);
		return LBM_OK;
	}

	base_data = lbm_block_page0(ctx, (uint32_t)pblk) + 2;
	while (done < len) {
		uint32_t abs_off = off + done;
		uint32_t page = base_data + abs_off / ctx->page_size;
		uint32_t page_off = abs_off % ctx->page_size;
		uint32_t chunk = ctx->page_size - page_off;
		u8 r;

		if (chunk > len - done) {
			chunk = len - done;
		}

		if ((page_off == 0) && (chunk == ctx->page_size)) {
			r = NAND_FTL_ReadPage(page, (uint8_t *)buf + done);
			if (!lbm_read_ok(r)) {
				return LBM_ERR_IO;
			}
		} else {
			r = NAND_FTL_ReadPage(page, ctx->pagebuf);
			if (!lbm_read_ok(r)) {
				return LBM_ERR_IO;
			}
			memcpy((uint8_t *)buf + done, ctx->pagebuf + page_off, chunk);
		}
		done += chunk;
	}
	return LBM_OK;
}

int lbm_block_write(lbm_ctx_t *ctx, int lblk, const void *buf, uint32_t off, uint32_t len)
{
	int32_t pblk;
	uint32_t base_data, first, npages, i;

	if ((lblk < 0) || ((uint32_t)lblk >= ctx->usable_blocks)) {
		return LBM_ERR_IO;
	}
	if ((off % ctx->page_size) || (len % ctx->page_size)) {
		VFS_DBG(VFS_ERROR, "lbm: unaligned write off=%u len=%u", off, len);
		return LBM_ERR_IO;
	}

	pblk = ctx->map[lblk];
	if (pblk < 0) {
		pblk = lbm_alloc_block(ctx);
		if (pblk < 0) {
			return LBM_ERR_NOSPACE;
		}
		if (lbm_write_blk_hdr(ctx, (uint32_t)pblk, (uint32_t)lblk) != LBM_OK) {
			lbm_retire_block(ctx, (uint32_t)pblk);
			ctx->avail_count--;   /* was FREE/allocatable */
			return LBM_ERR_BAD;
		}
		ctx->state[pblk] = LBM_BLK_USED;
		ctx->avail_count--;
		ctx->map[lblk] = pblk;
	}

	base_data = lbm_block_page0(ctx, (uint32_t)pblk) + 2;
	first = off / ctx->page_size;
	npages = len / ctx->page_size;
	for (i = 0; i < npages; i++) {
		if (LBM_WRITEPAGE(ctx, base_data + first + i,
						  (const u8 *)buf + i * ctx->page_size, 0) != HAL_OK) {
			/* Program failure: do NOT unmap/erase/mark-bad here.  littlefs
			 * recovers by RELOCATING -- it reads this block's already-written
			 * (possibly committed) data into a fresh block -- so the block must
			 * stay mapped and readable.  Destroying it now would corrupt that
			 * data.  Bad-block retirement is deferred to when littlefs later
			 * reclaims (erases) this block: lbm_block_erase -> lbm_make_free
			 * marks it bad if the erase also fails, or recycles it if the erase
			 * succeeds (program fail was transient).  See LBM_OVERVIEW.md. */
			return LBM_ERR_BAD;
		}
	}
	return LBM_OK;
}

int lbm_block_erase(lbm_ctx_t *ctx, int lblk)
{
	int32_t pblk;

	if ((lblk < 0) || ((uint32_t)lblk >= ctx->usable_blocks)) {
		return LBM_ERR_IO;
	}
	pblk = ctx->map[lblk];
	if (pblk < 0) {
		return LBM_OK;   /* nothing mapped */
	}

	ctx->map[lblk] = -1;   /* unmap first; block recycled below */
	if (lbm_make_free(ctx, (uint32_t)pblk, ctx->ec[pblk] + 1) == LBM_OK) {
		ctx->avail_count++;
	}
	/* on failure the physical block is already retired; lblk stays unmapped and
	 * a fresh block is allocated on the next write. Transparent to littlefs. */
	return LBM_OK;
}

uint32_t lbm_block_count(lbm_ctx_t *ctx)
{
	return ctx->usable_blocks;
}

uint32_t lbm_block_size(lbm_ctx_t *ctx)
{
	return ctx->lblk_size;
}
