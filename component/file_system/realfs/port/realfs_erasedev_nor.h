/*
 * REALFS erase-device backend: raw NOR flash, 512 B map mode (firmware only).
 *
 * Exposes a NOR partition as a realfs_erasedev so REALFS can run in erase-aware
 * (route-B) mode with a 512 B block instead of the 4 KB direct-NOR block.  Each
 * 4 KB NOR erase sector is one segment of 8 pages (512 B each, 16 B header ->
 * 496 B block).  REALFS's own LBT + segment GC provide the thin logical->
 * physical map; there is NO second FTL/log below (unlike LevelX), so writes are
 * single-layer COW.  The LBT is not persisted; it is rebuilt at mount by scanning
 * the page headers (same as the NAND path).
 *
 * seg_prog never erases -- the core erases a whole segment (seg_erase) before
 * opening it and then programs each page once, which NOR's bit-clear semantics
 * satisfy directly.
 *
 * `flash_offset` is a byte offset from the SPI flash base (SPI_FLASH_BASE),
 * i.e. the same address space flash_stream_read / flash_erase_sector use.
 */
#ifndef REALFS_ERASEDEV_NOR_H
#define REALFS_ERASEDEV_NOR_H

#include "realfs_erasedev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Fill *dev for a NOR region [flash_offset, flash_offset+size).
 * flash_offset and size must be 4 KB (sector) aligned; page_size must divide
 * 4096 (typically 512).  Returns 0 on success, <0 on bad args. */
int  realfs_erasedev_nor_open(struct realfs_erasedev *dev, uint32_t flash_offset,
							  uint32_t size, uint32_t page_size);
void realfs_erasedev_nor_close(struct realfs_erasedev *dev);

#ifdef __cplusplus
}
#endif

#endif /* REALFS_ERASEDEV_NOR_H */
