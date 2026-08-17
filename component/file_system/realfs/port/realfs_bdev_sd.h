/*
 * SD-card block device backend for REALFS (firmware only).
 *
 * Maps REALFS logical blocks (block_size, e.g. 4096) onto 512-byte SD sectors
 * via the SoC SD HAL (SD_ReadBlocks / SD_WriteBlocks).  When the SoC has no SD
 * host (CONFIG_SUPPORT_SDIOH undefined) the two entry points are stubbed to
 * return failure so callers compile and link unchanged.
 */
#ifndef REALFS_BDEV_SD_H
#define REALFS_BDEV_SD_H

#include "realfs_bdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialise the SD card (once) and fill *bdev. block_size must be a multiple
 * of 512. Returns 0 on success, <0 on failure (no card / HAL error / no SD). */
int realfs_bdev_sd_open(struct realfs_bdev *bdev, uint32_t block_size);

/* De-initialise the SD card and tear down the bdev. */
void realfs_bdev_sd_close(struct realfs_bdev *bdev);

#ifdef __cplusplus
}
#endif

#endif /* REALFS_BDEV_SD_H */
