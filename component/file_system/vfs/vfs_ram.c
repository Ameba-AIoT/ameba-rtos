#include "ameba_soc.h"
#include "os_wrapper.h"
#include "vfs.h"
#include "vfs_ram.h"

static const char *const TAG = "VFS_RAM";

static u8 *ram_buf;
static u32 ram_size;
static int ram_owner = -1;		/* vfs_type currently owning the disk, -1 = idle */

volatile uint8_t ram_mount_flag = 0;

int vfs_ram_open(int vfs_type)
{
	u32 size = (u32)CONFIG_VFS_RAM_SIZE_KB * 1024;

	if (ram_owner >= 0) {
		RTK_LOGE(TAG, "ram busy\n");
		return -1;
	}

#if defined(CONFIG_VFS_RAM_PSRAM)
	if (!ChipInfo_PsramExists()) {
		RTK_LOGE(TAG, "no psram\n");
		return -1;
	}
	/* malloc, not zmalloc: every register formats the disk anyway */
	ram_buf = (u8 *)rtos_heap_types_malloc(size, TYPE_DRAM);
#else
	ram_buf = (u8 *)rtos_heap_types_malloc(size, TYPE_SRAM);
	/* heap5 TYPE_SRAM only bounds the start address: it falls through to PSRAM
	 * once SRAM is short, so reject anything not wholly below PSRAM_BASE */
	if (ram_buf != NULL && ((u32)ram_buf >= PSRAM_BASE || size > PSRAM_BASE - (u32)ram_buf)) {
		rtos_mem_free(ram_buf);
		ram_buf = NULL;
		RTK_LOGE(TAG, "ram not in sram\n");
		return -1;
	}
#endif
	if (ram_buf == NULL) {
		RTK_LOGE(TAG, "ram alloc fail\n");
		return -1;
	}

	ram_size = size;
	ram_owner = vfs_type;
	return 0;
}

void vfs_ram_close(void)
{
	if (ram_buf != NULL) {
		rtos_mem_free(ram_buf);
	}
	ram_buf = NULL;
	ram_size = 0;
	ram_owner = -1;
	ram_mount_flag = 0;
}

u32 vfs_ram_size(void)
{
	return ram_size;
}

int vfs_ram_read(u32 off, void *dst, u32 len)
{
	if (ram_buf == NULL || off > ram_size || len > ram_size - off) {
		return -1;
	}
	memcpy(dst, ram_buf + off, len);
	return 0;
}

int vfs_ram_write(u32 off, const void *src, u32 len)
{
	if (ram_buf == NULL || off > ram_size || len > ram_size - off) {
		return -1;
	}
	memcpy(ram_buf + off, src, len);
	return 0;
}
