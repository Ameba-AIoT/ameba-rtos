#include "platform_autoconf.h"
#include "littlefs_adapter.h"

#ifdef CONFIG_AMEBASMART
#include "lfs_nand_ftl.h"
#endif

lfs_t g_lfs;
u32 LFS_FLASH_BASE_ADDR;
u32 LFS_FLASH_SIZE;

#ifdef CONFIG_AMEBASMART
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
	.block_cycles = 100,
};

int lfs_nand_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	u32 NandAddr, PageAddr;

	NandAddr = LFS_FLASH_BASE_ADDR + c->block_size * block + off;
	PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	if (NAND_FTL_ReadPage(PageAddr, (uint8_t *)buffer)) {
		return LFS_ERR_CORRUPT;
	}


	return LFS_ERR_OK;
}

int lfs_nand_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	if (size == 0) {
		return LFS_ERR_OK;
	}

	if ((off + size) > c->block_size) {
		VFS_DBG(VFS_ERROR, "prog range exceed block size");
		return LFS_ERR_IO;
	}

	u32 NandAddr, PageAddr;


	NandAddr = LFS_FLASH_BASE_ADDR + c->block_size * block + off;
	PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);

	if (NAND_FTL_WritePage(PageAddr, (uint8_t *)buffer, 0)) {
		return LFS_ERR_CORRUPT;
	}


	return LFS_ERR_OK;
}

int lfs_nand_erase(const struct lfs_config *c, lfs_block_t block)
{
	if (c->block_size != 0x20000) {
		VFS_DBG(VFS_ERROR, "block size config wrong");
		return LFS_ERR_IO;
	}

	u32 NandAddr, PageAddr;

	NandAddr = LFS_FLASH_BASE_ADDR + c->block_size * block;
	PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);

	if (NAND_FTL_EraseBlock(PageAddr, 0)) {
		return LFS_ERR_CORRUPT;
	}


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
#if !defined(CONFIG_AS_INIC_AP) && !defined(CONFIG_SINGLE_CORE_WIFI)
	(void) lfs;
	return 0;
#else
	struct lfs_config *lfs_cfg;
	int ret = 0;

#ifdef CONFIG_AMEBASMART
	if (!SYSCFG_BootFromNor()) {
		VFS_DBG(VFS_INFO, "init nand lfs cfg");
		NAND_FTL_Init();
		g_nand_lfs_cfg.block_count = LFS_FLASH_SIZE / 128 / 1024;
		lfs_cfg = &g_nand_lfs_cfg;
	} else
#endif
	{
		g_nor_lfs_cfg.block_count = LFS_FLASH_SIZE / 4096;
		VFS_DBG(VFS_INFO, "init nor lfs cfg");
		lfs_cfg = &g_nor_lfs_cfg;
	}

	ret = lfs_mount(lfs, lfs_cfg);
	if (ret == LFS_ERR_NOTFMT) {
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
#endif
}
