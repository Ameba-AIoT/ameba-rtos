/*
 * VFS RAM-disk block device backend for REALFS (firmware only).
 *
 * RAM is overwritable in place, so this is a plain realfs_bdev (no erase-aware
 * edev / GC). block_size may be any power of two in [512, 4096].
 */
#ifndef REALFS_BDEV_RAM_H
#define REALFS_BDEV_RAM_H

#include "realfs_bdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate the VFS RAM disk and fill *bdev. Returns 0 on success, <0 on error. */
int realfs_bdev_ram_open(struct realfs_bdev *bdev, uint32_t block_size);

/* Free the VFS RAM disk. */
void realfs_bdev_ram_close(struct realfs_bdev *bdev);

#ifdef __cplusplus
}
#endif

#endif /* REALFS_BDEV_RAM_H */
