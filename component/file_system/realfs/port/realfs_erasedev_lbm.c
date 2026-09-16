/* REALFS erase-device backend: SPI NAND via LBM (firmware only). */
#include "realfs_erasedev_lbm.h"
#include "ameba.h"    /* flash_get_layout_info, VFS1, SPI_FLASH_BASE */
#include "vfs.h"      /* vfs_nand_flash_pagesize, vfs_nand_flash_pagenum, log */
#include "lbm.h"
#include <string.h>

/* The VFS1 partition geometry handed to lbm_init() below.  In a littlefs build
 * these are populated by vfs.c when littlefs registers; in a REALFS build
 * littlefs never registers, so we resolve the layout ourselves. */
extern u32 LFS_FLASH_BASE_ADDR;
extern u32 LFS_FLASH_SIZE;

static const char *TAG = "REALFS_LBM";

/* One global LBM context (mirrors littlefs_adapter.c pattern).
 * If both littlefs and REALFS are enabled, they must not share the same LBM
 * context — each needs its own partition. Here we assume REALFS uses the
 * VFS1 NAND partition exclusively when REALFS_FLASH_NAND_LBM is selected. */
static lbm_ctx_t g_lbm;

/* ---- realfs_erasedev callbacks ----------------------------------------- */

/* Callbacks route to the LBM context stored in dev->ctx, so the same code
 * serves both the on-chip NAND (g_lbm) and the external SPI NAND (g_lbm_second). */
static int lbm_edev_read(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, void *buf)
{
	if (lbm_block_read((lbm_ctx_t *)dev->ctx, (int)seg, buf, off, len) != LBM_OK) {
		return -1;
	}
	return 0;
}

static int lbm_edev_prog(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, const void *buf)
{
	if (lbm_block_write((lbm_ctx_t *)dev->ctx, (int)seg, buf, off, len) != LBM_OK) {
		return -1;
	}
	return 0;
}

static int lbm_edev_erase(const struct realfs_erasedev *dev, uint32_t seg)
{
	if (lbm_block_erase((lbm_ctx_t *)dev->ctx, (int)seg) != LBM_OK) {
		return -1;
	}
	return 0;
}

/* Fill *dev from an already-initialised LBM context. Returns 0 / -1. */
static int lbm_edev_fill(struct realfs_erasedev *dev, lbm_ctx_t *ctx, uint32_t page_size)
{
	uint32_t lblk_size = lbm_block_size(ctx);
	uint32_t seg_pages = (page_size != 0) ? (lblk_size / page_size) : 0;

	if (page_size == 0 || seg_pages == 0 || lblk_size % page_size != 0) {
		RTK_LOGE(TAG, "LBM geometry mismatch: lblk=%u page=%u\n",
				 (unsigned)lblk_size, (unsigned)page_size);
		return -1;
	}

	memset(dev, 0, sizeof(*dev));
	dev->seg_read  = lbm_edev_read;
	dev->seg_prog  = lbm_edev_prog;
	dev->seg_erase = lbm_edev_erase;
	dev->sync      = NULL;   /* LBM writes are synchronous */
	dev->page_size = page_size;
	dev->seg_pages = seg_pages;
	dev->seg_count = lbm_block_count(ctx);
	dev->ctx       = ctx;

	RTK_LOGI(TAG, "LBM backend: %u segs x %u pages x %u B = %u KB\n",
			 (unsigned)dev->seg_count, (unsigned)seg_pages, (unsigned)page_size,
			 (unsigned)(dev->seg_count * seg_pages * page_size / 1024));
	return 0;
}

/* ---- public API -------------------------------------------------------- */

int realfs_erasedev_lbm_open(struct realfs_erasedev *dev)
{
	if (!dev) {
		return -1;
	}

	/* Resolve VFS1 partition geometry (see extern note above). */
	if (LFS_FLASH_BASE_ADDR == 0 || LFS_FLASH_SIZE == 0) {
		u32 start = 0, end = 0;
		flash_get_layout_info(VFS1, &start, &end);
		if (start == 0xFFFFFFFF || end == 0xFFFFFFFF || end <= start) {
			RTK_LOGE(TAG, "VFS1 partition not allocated\n");
			return -1;
		}
		LFS_FLASH_BASE_ADDR = start - SPI_FLASH_BASE;
		LFS_FLASH_SIZE      = (end - start) + 1;
	}

	/* geometry + back-end for LBM; NAND_FTL_Init() published the page size */
	g_lbm.ops = &lbm_nand_ops;
	g_lbm.cfg_base_addr = LFS_FLASH_BASE_ADDR;
	g_lbm.cfg_size = LFS_FLASH_SIZE;
	g_lbm.cfg_page_size = vfs_nand_flash_pagesize;
	g_lbm.cfg_block_pages = vfs_nand_flash_pagenum;

	if (lbm_init(&g_lbm) != LBM_OK) {
		RTK_LOGE(TAG, "lbm_init failed (page=%u pages=%u base=0x%x size=0x%x)\n",
				 (unsigned)vfs_nand_flash_pagesize, (unsigned)vfs_nand_flash_pagenum,
				 (unsigned)LFS_FLASH_BASE_ADDR, (unsigned)LFS_FLASH_SIZE);
		return -1;
	}

	/* LBM exposes (block_count × block_size) of contiguous logical space;
	 * map it onto realfs_erasedev geometry (page = vfs_nand_flash_pagesize). */
	if (lbm_edev_fill(dev, &g_lbm, vfs_nand_flash_pagesize) != 0) {
		lbm_deinit(&g_lbm);
		return -1;
	}
	return 0;
}

void realfs_erasedev_lbm_close(struct realfs_erasedev *dev)
{
	lbm_deinit(&g_lbm);
	if (dev) {
		dev->ctx = NULL;
	}
}

#if defined(CONFIG_REALFS_SECOND_FLASH_NAND)
#include "vfs_second_nand_ftl.h"   /* SECOND_NAND_FTL_Init, vfs_second_nand_* */

/* Separate LBM context for the external SPI NAND; the whole chip is the volume
 * (base 0, size = probed capacity), driven by lbm_second_nand_ops. */
static lbm_ctx_t g_lbm_second;

int realfs_erasedev_lbm_second_open(struct realfs_erasedev *dev)
{
	if (!dev) {
		return -1;
	}

	/* probe the external NAND (idempotent) so geometry/capacity are known */
	if (SECOND_NAND_FTL_Init() != HAL_OK) {
		RTK_LOGE(TAG, "second nand init fail\n");
		return -1;
	}

	g_lbm_second.ops = &lbm_second_nand_ops;
	g_lbm_second.cfg_base_addr = 0;
	g_lbm_second.cfg_size = vfs_second_nand_capacity;
	g_lbm_second.cfg_page_size = vfs_second_nand_pagesize;
	g_lbm_second.cfg_block_pages = vfs_second_nand_pagenum;

	if (lbm_init(&g_lbm_second) != LBM_OK) {
		RTK_LOGE(TAG, "second nand lbm_init fail (page=%u pages=%u cap=0x%x)\n",
				 (unsigned)vfs_second_nand_pagesize, (unsigned)vfs_second_nand_pagenum,
				 (unsigned)vfs_second_nand_capacity);
		return -1;
	}

	if (lbm_edev_fill(dev, &g_lbm_second, vfs_second_nand_pagesize) != 0) {
		lbm_deinit(&g_lbm_second);
		return -1;
	}
	return 0;
}

void realfs_erasedev_lbm_second_close(struct realfs_erasedev *dev)
{
	lbm_deinit(&g_lbm_second);
	if (dev) {
		dev->ctx = NULL;
	}
}
#endif /* CONFIG_REALFS_SECOND_FLASH_NAND */
