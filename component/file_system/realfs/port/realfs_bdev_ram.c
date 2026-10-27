/* VFS RAM-disk block device backend for REALFS (firmware only). */
#include "realfs_bdev_ram.h"
#include <string.h>

#include "vfs.h"
#include "vfs_ram.h"

static int ram_read(const struct realfs_bdev *bdev, uint32_t block_num,
					uint32_t count, void *buf)
{
	if (block_num > bdev->block_count || count > bdev->block_count - block_num) {
		return -1;
	}
	return vfs_ram_read(block_num * bdev->block_size, buf, count * bdev->block_size);
}

static int ram_write(const struct realfs_bdev *bdev, uint32_t block_num,
					 uint32_t count, const void *buf)
{
	if (block_num > bdev->block_count || count > bdev->block_count - block_num) {
		return -1;
	}
	return vfs_ram_write(block_num * bdev->block_size, buf, count * bdev->block_size);
}

static int ram_sync(const struct realfs_bdev *bdev)
{
	(void)bdev;
	return 0;
}

int realfs_bdev_ram_open(struct realfs_bdev *bdev, uint32_t block_size)
{
	uint32_t size = (uint32_t)CONFIG_VFS_RAM_SIZE_KB * 1024;

	if (!bdev || block_size < 512 || block_size > 4096 || (block_size & (block_size - 1)) || (size % block_size)) {
		RTK_LOGE("VFS", "ram size invalid\n");
		return -1;
	}
	if (vfs_ram_open(VFS_REALFS)) {
		return -1;
	}

	memset(bdev, 0, sizeof(*bdev));
	bdev->read_blocks  = ram_read;
	bdev->write_blocks = ram_write;
	bdev->sync         = ram_sync;
	bdev->block_size   = block_size;
	bdev->block_count  = size / block_size;
	return 0;
}

void realfs_bdev_ram_close(struct realfs_bdev *bdev)
{
	if (bdev) {
		bdev->ctx = NULL;
	}
	vfs_ram_close();
}
