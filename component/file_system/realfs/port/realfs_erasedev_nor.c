/* REALFS erase-device backend: raw NOR flash, 512 B map mode (firmware only). */
#include "realfs_erasedev_nor.h"
#include <string.h>

#include "ameba.h"
#include "flash_api.h"
#include "log.h"

static const char *const TAG = "REALFS_NOR";

#define NOR_SECTOR_SIZE   4096u   /* NOR erase unit == one segment */

struct nor_ctx {
	uint32_t base;        /* byte offset from SPI flash base */
	uint32_t seg_size;    /* == NOR_SECTOR_SIZE */
};

static struct nor_ctx g_nor_ctx;

/* Read any byte range within a segment. */
static int nor_edev_read(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, void *buf)
{
	struct nor_ctx *c = (struct nor_ctx *)dev->ctx;
	if (seg >= dev->seg_count || off + len > c->seg_size) {
		return -1;
	}
	flash_t flash;
	flash_stream_read(&flash, c->base + seg * c->seg_size + off, len, (uint8_t *)buf);
	return 0;
}

/* Program a page-aligned range within an already-erased segment (program-once;
 * the core erases the segment before opening it). */
static int nor_edev_prog(const struct realfs_erasedev *dev, uint32_t seg,
						 uint32_t off, uint32_t len, const void *buf)
{
	struct nor_ctx *c = (struct nor_ctx *)dev->ctx;
	if (seg >= dev->seg_count || off + len > c->seg_size) {
		return -1;
	}
	flash_t flash;
	flash_stream_write(&flash, c->base + seg * c->seg_size + off, len, (uint8_t *)buf);
	return 0;
}

static int nor_edev_erase(const struct realfs_erasedev *dev, uint32_t seg)
{
	struct nor_ctx *c = (struct nor_ctx *)dev->ctx;
	if (seg >= dev->seg_count) {
		return -1;
	}
	flash_t flash;
	flash_erase_sector(&flash, c->base + seg * c->seg_size);
	return 0;
}

int realfs_erasedev_nor_open(struct realfs_erasedev *dev, uint32_t flash_offset,
							 uint32_t size, uint32_t page_size)
{
	if (!dev || page_size == 0 || (NOR_SECTOR_SIZE % page_size) ||
		(flash_offset % NOR_SECTOR_SIZE) || (size % NOR_SECTOR_SIZE) || size == 0) {
		return -1;
	}

	g_nor_ctx.base     = flash_offset;
	g_nor_ctx.seg_size = NOR_SECTOR_SIZE;

	memset(dev, 0, sizeof(*dev));
	dev->seg_read  = nor_edev_read;
	dev->seg_prog  = nor_edev_prog;
	dev->seg_erase = nor_edev_erase;
	dev->sync      = NULL;   /* flash_stream_write programs synchronously */
	dev->page_size = page_size;
	dev->seg_pages = NOR_SECTOR_SIZE / page_size;   /* 8 for 512 B pages */
	dev->seg_count = size / NOR_SECTOR_SIZE;
	dev->ctx       = &g_nor_ctx;

	RTK_LOGI(TAG, "NOR map bdev: off=0x%x size=%uKB -> %u segs x %u pages x %u B\n",
			 (unsigned)flash_offset, (unsigned)(size / 1024),
			 (unsigned)dev->seg_count, (unsigned)dev->seg_pages,
			 (unsigned)page_size);
	return 0;
}

void realfs_erasedev_nor_close(struct realfs_erasedev *dev)
{
	if (dev) {
		dev->ctx = NULL;
	}
}
