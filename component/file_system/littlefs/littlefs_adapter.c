#include "platform_autoconf.h"
#include "littlefs_adapter.h"
#include "vfs_second_nor_flash.h"

#ifdef CONFIG_SUPPORT_NAND_FLASH
#include "vfs_nand_ftl.h"
#include "lbm.h"
lbm_ctx_t g_lbm_ctx;   /* logical-block-mapping context, set up in rt_lfs_init() */
#endif

lfs_t g_lfs;
u32 LFS_FLASH_BASE_ADDR;
u32 LFS_FLASH_SIZE;

#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
/* read-only littlefs ROLFS image (blob embedded within app image, NOR flash). */
lfs_t g_rolfs_lfs;
u32 LFS_ROLFS_FLASH_BASE_ADDR;
u32 LFS_ROLFS_FLASH_SIZE;
#endif

#ifdef CONFIG_SUPPORT_NAND_FLASH
struct lfs_config g_nand_lfs_cfg = {
	.read  = lfs_nand_read,
	.prog  = lfs_nand_prog,
	.erase = lfs_nand_erase,
	.sync  = lfs_diskio_sync,

#ifdef LFS_THREADSAFE
	.lock = lfs_diskio_lock,
	.unlock = lfs_diskio_unlock,
#endif

	.read_size = 2048,
	.prog_size = 2048,
	.block_size = 2048 * 64,
	.lookahead_size = 8,
	.cache_size = 2048,
	.block_cycles = 500,
	.metadata_max = 4096,
};

/* NAND callbacks route through the LBM layer (logical block mapping): littlefs
 * sees a contiguous, bad-block-free logical block device. block == lblk (1:1). */
int lfs_nand_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	(void)c;
	if (size == 0) {
		return LFS_ERR_OK;
	}

	if (lbm_block_read(&g_lbm_ctx, (int)block, buffer, (u32)off, (u32)size) != LBM_OK) {
		return LFS_ERR_CORRUPT;
	}
	return LFS_ERR_OK;
}

int lfs_nand_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	int r;

	if (size == 0) {
		return LFS_ERR_OK;
	}

	if ((off + size) > c->block_size) {
		VFS_DBG(VFS_ERROR, "prog range exceed block size");
		return LFS_ERR_IO;
	}

	r = lbm_block_write(&g_lbm_ctx, (int)block, buffer, (u32)off, (u32)size);
	if (r == LBM_ERR_BAD) {
		return LFS_ERR_CORRUPT;   /* block retired; littlefs relocates */
	}
	if (r == LBM_ERR_NOSPACE) {
		return LFS_ERR_NOSPC;
	}
	return (r == LBM_OK) ? LFS_ERR_OK : LFS_ERR_IO;
}

int lfs_nand_erase(const struct lfs_config *c, lfs_block_t block)
{
	(void)c;
	if (lbm_block_erase(&g_lbm_ctx, (int)block) != LBM_OK) {
		return LFS_ERR_CORRUPT;
	}
	return LFS_ERR_OK;
}
#endif

#ifdef CONFIG_LITTLEFS_SECOND_FLASH
lfs_t g_second_lfs;

u32 LFS_SECOND_FLASH_BASE_ADDR;
u32 LFS_SECOND_FLASH_SIZE;

struct lfs_config g_second_nor_lfs_cfg = {
	.read  = lfs_second_nor_read,
	.prog  = lfs_second_nor_prog,
	.erase = lfs_second_nor_erase,
	.sync  = lfs_diskio_sync,

#ifdef LFS_THREADSAFE
	.lock = lfs_diskio_lock,
	.unlock = lfs_diskio_unlock,
#endif

	.read_size = 1,
	.prog_size = 1,
	.block_size = 4096,
	.lookahead_size = 8,
	.cache_size = 4096,
	.block_cycles = 100,
};

int lfs_second_nor_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

#ifdef CONFIG_SECOND_FLASH_NOR
	DATA_FLASH_ReadStream(c->block_size * block + off, size, (u8 *)buffer);
#else
	second_flash_read_stream(LFS_SECOND_FLASH_BASE_ADDR + c->block_size * block + off, size, (char *)buffer);
#endif

	return LFS_ERR_OK;
}

int lfs_second_nor_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	if ((off + size) > c->block_size) {
		VFS_DBG(VFS_ERROR, "prog range exceed block size");
		return LFS_ERR_IO;
	}

#ifdef CONFIG_SECOND_FLASH_NOR
	DATA_FLASH_WriteStream(c->block_size * block + off, size, (u8 *)buffer);
#else
	second_flash_write_stream(LFS_SECOND_FLASH_BASE_ADDR + c->block_size * block + off, size, (char *)buffer);
#endif

	return LFS_ERR_OK;
}

int lfs_second_nor_erase(const struct lfs_config *c, lfs_block_t block)
{
	if (c->block_size != 0x1000) {
		VFS_DBG(VFS_ERROR, "block size config wrong");
		return LFS_ERR_IO;
	}

#ifdef CONFIG_SECOND_FLASH_NOR
	DATA_FLASH_EraseXIP(EraseSector, c->block_size * block);
#else
	second_flash_erase_sector(LFS_SECOND_FLASH_BASE_ADDR + c->block_size * block);
#endif

	return LFS_ERR_OK;
}
#endif

struct lfs_config g_nor_lfs_cfg = {
	.read  = lfs_nor_read,
	.prog  = lfs_nor_prog,
	.erase = lfs_nor_erase,
	.sync  = lfs_diskio_sync,

#ifdef LFS_THREADSAFE
	.lock = lfs_diskio_lock,
	.unlock = lfs_diskio_unlock,
#endif

	.read_size = 1,
	.prog_size = 1,
	.block_size = 4096,
	.lookahead_size = 8,
	.cache_size = 256,
	.block_cycles = 100,
};

int lfs_nor_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	flash_t flash;

	flash_stream_read(&flash, LFS_FLASH_BASE_ADDR + c->block_size * block + off, size, (uint8_t *)buffer);

	return LFS_ERR_OK;
}

int lfs_nor_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	if ((off + size) > c->block_size) {
		VFS_DBG(VFS_ERROR, "prog range exceed block size");
		return LFS_ERR_IO;
	}

	flash_t flash;

	flash_stream_write(&flash, LFS_FLASH_BASE_ADDR + c->block_size * block + off, size, (uint8_t *)buffer);

	return LFS_ERR_OK;
}

int lfs_nor_erase(const struct lfs_config *c, lfs_block_t block)
{
	if (c->block_size != 0x1000) {
		VFS_DBG(VFS_ERROR, "block size config wrong");
		return LFS_ERR_IO;
	}

	flash_t flash;

	flash_erase_sector(&flash, LFS_FLASH_BASE_ADDR + c->block_size * block);

	return LFS_ERR_OK;
}


#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
/* ---- read-only ROLFS littlefs (blob within app image, always NOR) ---- */
struct lfs_config g_nor_rolfs_lfs_cfg = {
	.read  = lfs_nor_rolfs_read,
	.prog  = lfs_nor_rolfs_prog,
	.erase = lfs_nor_rolfs_erase,
	.sync  = lfs_diskio_sync,

#ifdef LFS_THREADSAFE
	.lock = lfs_diskio_lock,
	.unlock = lfs_diskio_unlock,
#endif

	.read_size = 1,
	.prog_size = 1,
	.block_size = 4096,
	.lookahead_size = 8,
	.cache_size = 256,
	.block_cycles = 100,
};

int lfs_nor_rolfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	flash_t flash;

	flash_stream_read(&flash, LFS_ROLFS_FLASH_BASE_ADDR + c->block_size * block + off, size, (uint8_t *)buffer);

	return LFS_ERR_OK;
}

int lfs_nor_rolfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	(void) c;
	(void) block;
	(void) off;
	(void) buffer;
	(void) size;
	/* read-only: writes are rejected so littlefs never mutates the ROLFS blob */
	VFS_DBG(VFS_ERROR, "rolfs littlefs is read-only, prog rejected");
	return LFS_ERR_INVAL;
}

int lfs_nor_rolfs_erase(const struct lfs_config *c, lfs_block_t block)
{
	(void) c;
	(void) block;
	/* read-only: erase is rejected so littlefs never mutates the ROLFS blob */
	VFS_DBG(VFS_ERROR, "rolfs littlefs is read-only, erase rejected");
	return LFS_ERR_INVAL;
}
#endif

int lfs_diskio_sync(const struct lfs_config *c)
{
	(void) c;
	return LFS_ERR_OK;
}

#ifdef LFS_THREADSAFE
rtos_mutex_t lfs_op_mux = NULL;
int lfs_diskio_lock(const struct lfs_config *c)
{
	(void) c;
	if (lfs_op_mux == NULL) {
		rtos_mutex_create(&lfs_op_mux);
	}
	rtos_mutex_take(lfs_op_mux, MUTEX_WAIT_TIMEOUT);
	return LFS_ERR_OK;
}

int lfs_diskio_unlock(const struct lfs_config *c)
{
	(void) c;
	if (lfs_op_mux == NULL) {
		rtos_mutex_create(&lfs_op_mux);
	}
	rtos_mutex_give(lfs_op_mux);
	return LFS_ERR_OK;
}
#endif

int rt_lfs_init(lfs_t *lfs)
{
	struct lfs_config *lfs_cfg;
	int ret = 0;

#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
	if (lfs == &g_rolfs_lfs) {
		/* read-only ROLFS image: mount only, never format (prog/erase rejected) */
		g_nor_rolfs_lfs_cfg.block_count = LFS_ROLFS_FLASH_SIZE / 4096;
		VFS_DBG(VFS_INFO, "init rolfs (read-only) nor lfs cfg");
		ret = lfs_mount(lfs, &g_nor_rolfs_lfs_cfg);
		if (ret) {
			VFS_DBG(VFS_ERROR, "rolfs lfs_mount fail %d", ret);
		}
		return ret;
	}
#endif

	if (lfs == &g_lfs) {
#ifdef CONFIG_SUPPORT_NAND_FLASH
		if (SHOULD_USE_NAND()) {
			if (lbm_init(&g_lbm_ctx) != LBM_OK) {
				VFS_DBG(VFS_ERROR, "lbm_init fail");
				return -1;
			}
			/* block_size/count come from the LBM layer (logical block geometry, bad blocks excluded) */
			g_nand_lfs_cfg.block_size = lbm_block_size(&g_lbm_ctx);
			g_nand_lfs_cfg.block_count = lbm_block_count(&g_lbm_ctx);
			lfs_cfg = &g_nand_lfs_cfg;
			VFS_DBG(VFS_INFO, "nand lfs cfg: block_size=%u block_count=%u read=%u prog=%u cache=%u lookahead=%u block_cycles=%d",
					(unsigned int)g_nand_lfs_cfg.block_size, (unsigned int)g_nand_lfs_cfg.block_count,
					(unsigned int)g_nand_lfs_cfg.read_size, (unsigned int)g_nand_lfs_cfg.prog_size,
					(unsigned int)g_nand_lfs_cfg.cache_size, (unsigned int)g_nand_lfs_cfg.lookahead_size,
					(int)g_nand_lfs_cfg.block_cycles);
		} else
#endif
		{
			g_nor_lfs_cfg.block_count = LFS_FLASH_SIZE / 4096;
			lfs_cfg = &g_nor_lfs_cfg;
			VFS_DBG(VFS_INFO, "init nor lfs cfg");
		}
	}

#ifdef CONFIG_LITTLEFS_SECOND_FLASH
	if (lfs == &g_second_lfs) {
		g_second_nor_lfs_cfg.block_count = LFS_SECOND_FLASH_SIZE / 4096;
		VFS_DBG(VFS_INFO, "init second nor lfs cfg");
		lfs_cfg = &g_second_nor_lfs_cfg;
	}
#endif

	ret = lfs_mount(lfs, lfs_cfg);
	int need_reformat = (ret == LFS_ERR_NOTFMT);
	/* NOTE: do NOT reformat on LFS_ERR_NOSPC.  A no-space mount failure means
	 * the filesystem is full (possibly with a pending power-loss fixup that
	 * needs a spare block); auto-reformatting would silently destroy all user
	 * data.  Surface NOSPC as a mount failure instead so data is preserved. */
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
	/* Reformat the writable region not only when it is blank (LFS_ERR_NOTFMT),
	 * but also when an on-flash superblock is incompatible/corrupt:
	 *   - LFS_ERR_INVAL  : the stored block_count/block_size no longer matches
	 *     lfs_cfg (e.g. VFS1 grew from 512KB/128 blocks to 1MB/256 blocks across
	 *     a flash-layout change). Without this, mount keeps failing with
	 *     "Invalid block count" and the region never becomes usable.
	 *   - LFS_ERR_CORRUPT: superblock damaged.
	 * This is safe for a writable scratch/user region (data is recreated on demand)
	 * but must NEVER apply to the read-only ROLFS image — that path returns above. */
	need_reformat = need_reformat || (ret == LFS_ERR_INVAL) || (ret == LFS_ERR_CORRUPT);
#endif
	if (need_reformat) {
		VFS_DBG(VFS_WARNING, "lfs_mount ret %d, reformatting region", ret);
		ret = lfs_format(lfs, lfs_cfg);
		if (ret) {
			VFS_DBG(VFS_ERROR, "lfs_format fail %d", ret);
			return ret;
		}
		ret = lfs_mount(lfs, lfs_cfg);
		if (ret) {
			VFS_DBG(VFS_ERROR, "lfs_mount fail %d", ret);
			return ret;
		}
	}

	return ret;
}
