/*
 * NOR-flash block device backend for REALFS (firmware only).
 *
 * Maps each REALFS logical block 1:1 onto a NOR-flash erase sector, so the
 * logical block_size MUST equal the flash sector size (4096 B on Ameba).  A
 * block write erases the sector and re-programs it in full; REALFS only ever
 * issues whole-block writes and its A/B COW commit never rewrites the active
 * group in place, so an erase/program torn by power-loss can only damage the
 * not-yet-committed (inactive) copy -- the committed state stays intact.
 *
 * `flash_offset` is a byte offset from the SPI flash base (SPI_FLASH_BASE),
 * i.e. the same address space flash_stream_read/flash_erase_sector use.
 */
#ifndef REALFS_BDEV_FLASH_H
#define REALFS_BDEV_FLASH_H

#include "realfs_bdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Fill *bdev for a NOR-flash region [flash_offset, flash_offset+size).
 * flash_offset and size must be block_size-aligned and block_size must equal
 * the flash sector size (4096). Returns 0 on success, <0 on bad args. */
int realfs_bdev_flash_open(struct realfs_bdev *bdev, uint32_t flash_offset,
						   uint32_t size, uint32_t block_size);

void realfs_bdev_flash_close(struct realfs_bdev *bdev);

#ifdef __cplusplus
}
#endif

#endif /* REALFS_BDEV_FLASH_H */
