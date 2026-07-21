/* SPDX-License-Identifier: Apache-2.0 */
/*
 * REALFS block device abstraction.
 *
 * REALFS only ever speaks in logical blocks of block_size bytes.  Every concrete
 * backend (eMMC/SD sector device, SPI-NOR, raw-NAND thin FTL, or a PC image
 * file) implements this interface.  See design doc section 7 for the
 * block_size vs erase_unit constraints that keep COW power-loss-safe.
 */
#ifndef REALFS_BDEV_H
#define REALFS_BDEV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct realfs_bdev {
	/* Read/write `count` consecutive logical blocks starting at block_num.
	 * Caller guarantees buf >= count*block_size bytes and
	 * block_num + count <= block_count.  Return 0 on success, <0 on error. */
	int (*read_blocks)(const struct realfs_bdev *bdev, uint32_t block_num,
					   uint32_t count, void *buf);
	int (*write_blocks)(const struct realfs_bdev *bdev, uint32_t block_num,
						uint32_t count, const void *buf);
	/* Flush any device/cache so prior writes are durable.  0 on success. */
	int (*sync)(const struct realfs_bdev *bdev);

	/* Optional (may be NULL): hint that `count` logical blocks starting at
	 * block_num are no longer in use, so a log-structured backend (LevelX NOR,
	 * an eMMC discard, ...) can reclaim their physical space and avoid copying
	 * dead data during garbage collection.  Best-effort: REALFS ignores the
	 * return value and never depends on it for correctness. */
	void (*trim)(const struct realfs_bdev *bdev, uint32_t block_num,
				 uint32_t count);

	uint32_t block_count;   /* number of logical blocks */
	uint32_t block_size;    /* bytes per logical block (<= REALFS_MAX_BLOCK_SIZE) */

	void *ctx;              /* backend private data */
};

#ifdef __cplusplus
}
#endif

#endif /* REALFS_BDEV_H */
