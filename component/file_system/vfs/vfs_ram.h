#ifndef _VFS_RAM_H
#define _VFS_RAM_H

#include "basic_types.h"

/* Shared RAM-disk medium used by the littlefs / fatfs / realfs RAM backends.
 * Byte addressed: each FS glue converts its own block/sector number to an
 * offset. Single instance: only one FS may own the disk at a time. Not locked;
 * callers are serialised by their FS lock and vfs_mutex. */

int vfs_ram_open(int vfs_type);		/* allocate CONFIG_VFS_RAM_SIZE_KB; -1 if busy / no memory */
void vfs_ram_close(void);			/* free the buffer, safe to call twice */
u32 vfs_ram_size(void);
int vfs_ram_read(u32 off, void *dst, u32 len);
int vfs_ram_write(u32 off, const void *src, u32 len);

#endif
