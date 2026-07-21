/* REALFS erase-device backend: SPI NAND via LBM (firmware only). */
#include "realfs_erasedev_lbm.h"
#include "ameba.h"    /* flash_get_layout_info, VFS1, SPI_FLASH_BASE */
#include "vfs.h"      /* vfs_nand_flash_pagesize, vfs_nand_flash_pagenum, log */
#include "lbm.h"
#include <string.h>

/* LBM reads the VFS1 partition geometry from these littlefs globals
 * (see lbm_core.c: LFS_FLASH_BASE_ADDR / LFS_FLASH_SIZE).  In a littlefs
 * build they are populated by vfs.c when littlefs registers; in a REALFS
 * build littlefs never registers, so we must populate them ourselves before
 * lbm_init() -- otherwise lbm_init sees total_blocks==0 and returns error. */
extern u32 LFS_FLASH_BASE_ADDR;
extern u32 LFS_FLASH_SIZE;

static const char *TAG = "REALFS_LBM";

/* One global LBM context (mirrors littlefs_adapter.c pattern).
 * If both littlefs and REALFS are enabled, they must not share the same LBM
 * context — each needs its own partition. Here we assume REALFS uses the
 * VFS1 NAND partition exclusively when REALFS_FLASH_NAND_LBM is selected. */
static lbm_ctx_t g_lbm;

/* ---- realfs_erasedev callbacks ----------------------------------------- */

static int lbm_edev_read(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, void *buf)
{
	(void)dev;
	if (lbm_block_read(&g_lbm, (int)seg, buf, off, len) != LBM_OK) {
		return -1;
	}
	return 0;
}

static int lbm_edev_prog(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, const void *buf)
{
	(void)dev;
	if (lbm_block_write(&g_lbm, (int)seg, buf, off, len) != LBM_OK) {
		return -1;
	}
	return 0;
}

static int lbm_edev_erase(const struct realfs_erasedev *dev, uint32_t seg)
{
	(void)dev;
	if (lbm_block_erase(&g_lbm, (int)seg) != LBM_OK) {
		return -1;
	}
	return 0;
}

/* ---- public API -------------------------------------------------------- */

int realfs_erasedev_lbm_open(struct realfs_erasedev *dev)
{
	if (!dev) {
		return -1;
	}

	/* Publish VFS1 partition geometry for lbm_init() (see extern note above). */
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

	if (lbm_init(&g_lbm) != LBM_OK) {
		RTK_LOGE(TAG, "lbm_init failed (page=%u pages=%u base=0x%x size=0x%x)\n",
				 (unsigned)vfs_nand_flash_pagesize, (unsigned)vfs_nand_flash_pagenum,
				 (unsigned)LFS_FLASH_BASE_ADDR, (unsigned)LFS_FLASH_SIZE);
		return -1;
	}

	/* LBM exposes (block_count × block_size) of contiguous logical space.
	 * Map onto realfs_erasedev geometry:
	 *   page_size  = vfs_nand_flash_pagesize (e.g. 2048)
	 *   seg_pages  = LBM block_size / page_size  (= 62 for 124 KB block)
	 *   seg_count  = LBM block_count (usable logical blocks = 236) */
	uint32_t lblk_size  = lbm_block_size(&g_lbm);
	uint32_t page_size  = vfs_nand_flash_pagesize;
	uint32_t seg_pages  = lblk_size / page_size;

	if (page_size == 0 || seg_pages == 0 || lblk_size % page_size != 0) {
		RTK_LOGE(TAG, "LBM geometry mismatch: lblk=%u page=%u\n",
				 (unsigned)lblk_size, (unsigned)page_size);
		lbm_deinit(&g_lbm);
		return -1;
	}

	memset(dev, 0, sizeof(*dev));
	dev->seg_read  = lbm_edev_read;
	dev->seg_prog  = lbm_edev_prog;
	dev->seg_erase = lbm_edev_erase;
	dev->sync      = NULL;   /* LBM writes are synchronous */
	dev->page_size = page_size;
	dev->seg_pages = seg_pages;
	dev->seg_count = lbm_block_count(&g_lbm);
	dev->ctx       = &g_lbm;

	RTK_LOGI(TAG, "LBM backend: %u segs × %u pages × %u B = %u KB\n",
			 (unsigned)dev->seg_count, (unsigned)seg_pages,
			 (unsigned)page_size,
			 (unsigned)(dev->seg_count * seg_pages * page_size / 1024));
	return 0;
}

void realfs_erasedev_lbm_close(struct realfs_erasedev *dev)
{
	lbm_deinit(&g_lbm);
	if (dev) {
		dev->ctx = NULL;
	}
}
