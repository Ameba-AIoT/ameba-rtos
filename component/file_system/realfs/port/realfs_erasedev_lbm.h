/*
 * REALFS erase-device backend: raw SPI NAND via LBM (firmware only).
 *
 * Wraps lbm_block_read / lbm_block_write / lbm_block_erase as a
 * realfs_erasedev so REALFS can run in erase-aware mode (route B) on
 * amebasmart/RTL8730E raw NAND with on-die ECC and bad-block management
 * handled transparently by LBM.
 *
 * Usage:
 *   struct realfs_erasedev edev;
 *   struct realfs           fs;
 *
 *   realfs_erasedev_lbm_open(&edev);          // init LBM + fill edev
 *   realfs_nand_mount(&fs, &edev);            // mount (scan rebuild LBT)
 *   ...
 *   realfs_nand_unmount(&fs);
 *   realfs_erasedev_lbm_close(&edev);
 */
#ifndef REALFS_ERASEDEV_LBM_H
#define REALFS_ERASEDEV_LBM_H

#include "realfs_erasedev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialise LBM for the VFS1 NAND partition and populate *dev.
 * Returns 0 on success, -1 on LBM init failure.
 */
int  realfs_erasedev_lbm_open(struct realfs_erasedev *dev);
void realfs_erasedev_lbm_close(struct realfs_erasedev *dev);

#ifdef __cplusplus
}
#endif

#endif /* REALFS_ERASEDEV_LBM_H */
