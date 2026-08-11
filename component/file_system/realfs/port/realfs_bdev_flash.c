/* NOR-flash block device backend for REALFS (firmware only). */
#include "realfs_bdev_flash.h"
#include <string.h>

#include "ameba.h"
#include "flash_api.h"
#include "os_wrapper.h"
#include "log.h"

static const char *const TAG = "REALFS_FLASH";

#define FLASH_SECTOR_SIZE   4096u

struct flash_ctx {
	uint32_t base;          /* byte offset from SPI flash base */
	uint32_t block_size;
};

static struct flash_ctx g_flash_ctx;

static int flash_read(const struct realfs_bdev *bdev, uint32_t block_num,
					  uint32_t count, void *buf)
{
	struct flash_ctx *c = (struct flash_ctx *)bdev->ctx;
	if (block_num + count > bdev->block_count) {
		return -1;
	}
	flash_t flash;
	flash_stream_read(&flash, c->base + block_num * c->block_size,
					  count * c->block_size, (uint8_t *)buf);
	return 0;
}

static int flash_write(const struct realfs_bdev *bdev, uint32_t block_num,
					   uint32_t count, const void *buf)
{
	struct flash_ctx *c = (struct flash_ctx *)bdev->ctx;
	if (block_num + count > bdev->block_count) {
		return -1;
	}
	flash_t flash;
	const uint8_t *p = (const uint8_t *)buf;
	for (uint32_t i = 0; i < count; i++) {
		uint32_t addr = c->base + (block_num + i) * c->block_size;
		flash_erase_sector(&flash, addr);
		flash_stream_write(&flash, addr, c->block_size, (uint8_t *)(p + i * c->block_size));
	}
	return 0;
}

static int flash_sync(const struct realfs_bdev *bdev)
{
	(void)bdev;   /* flash_stream_write programs synchronously; nothing to flush */
	return 0;
}

int realfs_bdev_flash_open(struct realfs_bdev *bdev, uint32_t flash_offset,
						   uint32_t size, uint32_t block_size)
{
	if (!bdev || block_size != FLASH_SECTOR_SIZE ||
		(flash_offset % block_size) || (size % block_size) || size == 0) {
		return -1;
	}

	g_flash_ctx.base       = flash_offset;
	g_flash_ctx.block_size = block_size;

	memset(bdev, 0, sizeof(*bdev));
	bdev->read_blocks  = flash_read;
	bdev->write_blocks = flash_write;
	bdev->sync         = flash_sync;
	bdev->block_size   = block_size;
	bdev->block_count  = size / block_size;
	bdev->ctx          = &g_flash_ctx;

	RTK_LOGI(TAG, "flash bdev: off=0x%x size=%uKB -> %u blocks of %u B\n",
			 (unsigned)flash_offset, (unsigned)(size / 1024),
			 (unsigned)bdev->block_count, (unsigned)block_size);
	return 0;
}

void realfs_bdev_flash_close(struct realfs_bdev *bdev)
{
	if (bdev) {
		bdev->ctx = NULL;
	}
}
