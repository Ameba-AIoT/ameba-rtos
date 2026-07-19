#ifndef VFS_REALFS_H
#define VFS_REALFS_H

#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const vfs_opt realfs_drv;
extern volatile uint8_t realfs_mount_flag;

#if defined(CONFIG_REALFS_SD_MODE)
/* Format the SD card with REALFS (block_size 4096). hash_buckets must be a
 * power of two, or 0 to auto-size from the card capacity (recommended). Must be
 * called while REALFS is NOT mounted. 0 on success. */
int realfs_sd_format(unsigned int hash_buckets);
#endif

#if defined(CONFIG_REALFS_FLASH)
/* Format the NOR-flash REALFS partition (VFS flash-layout region). Same rules
 * as realfs_sd_format (pass 0 to auto-size). 0 on success. */
int realfs_flash_format(unsigned int hash_buckets);
#endif

#ifdef REALFS_ENABLE_DEBUG
/* Dump the mounted REALFS state (geometry/buckets/files/bitmap + check) to the
 * serial console. Compiled only when CONFIG_REALFS_DEBUG is set. */
void realfs_vfs_dump(void);
/* Run the consistency check on the mounted FS; returns #problems (0 = clean). */
int  realfs_vfs_check(void);
#endif

#if defined(CONFIG_REALFS_FLASH_NAND_LBM) || defined(CONFIG_REALFS_FLASH_NOR_MAP)
/* Erase-aware (route-B) segment/GC accounting -- raw NAND (LBM) or NOR@512B map.
 * Print segment state (free/open/sealed segs, live pages, etc.). */
void realfs_vfs_ea_dump(void);
/* Return number of free (erased, available) data segments.  0 = no GC headroom. */
uint32_t realfs_vfs_ea_free_segs(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* VFS_REALFS_H */
