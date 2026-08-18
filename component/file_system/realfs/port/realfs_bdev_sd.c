/* SD-card block device backend for REALFS (firmware only). */
#include "platform_autoconf.h"
#include "realfs_bdev_sd.h"
#include <string.h>

#if defined(CONFIG_SUPPORT_SDIOH)

#include "ameba.h"
#include "ameba_sd.h"
#include "os_wrapper.h"
#include "log.h"

static const char *const TAG = "REALFS_SD";

#define SD_SECTOR_SIZE  512u

static int           sd_inited = 0;
static rtos_sema_t   sd_sema   = NULL;

static int sd_take(u32 timeout)
{
	return rtos_sema_take(sd_sema, timeout);
}
static int sd_give(u32 timeout)
{
	(void)timeout;
	return rtos_sema_give(sd_sema);
}

/* Single-block transfers (one 512 B sector/HAL call) + retry: on green2
 * (RTL8721F) multi-block SD transfers are unreliable (card hangs / silent
 * corruption); root cause not yet pinned down. amebasmart (RTL8730E) has no such
 * issue, so single-block there is just a safe, slightly slower path. */
#define SD_XFER_RETRY  3

static int sd_read(const struct realfs_bdev *bdev, uint32_t block_num,
				   uint32_t count, void *buf)
{
	uint32_t spb   = bdev->block_size / SD_SECTOR_SIZE;
	uint32_t nsect = count * spb;
	u8      *p     = (u8 *)buf;
	for (uint32_t s = 0; s < nsect; s++) {
		SD_RESULT r = SD_ERROR;
		for (int i = 0; i < SD_XFER_RETRY; i++) {
			r = SD_ReadBlocks(block_num * spb + s, p, 1);
			if (r == SD_OK) {
				break;
			}
		}
		if (r != SD_OK) {
			RTK_LOGE(TAG, "sd_read fail sector=%u result=%d\n",
					 (unsigned)(block_num * spb + s), (int)r);
			return -1;
		}
		p += SD_SECTOR_SIZE;
	}
	return 0;
}

static int sd_write(const struct realfs_bdev *bdev, uint32_t block_num,
					uint32_t count, const void *buf)
{
	uint32_t spb   = bdev->block_size / SD_SECTOR_SIZE;
	uint32_t nsect = count * spb;
	const u8 *p    = (const u8 *)buf;
	for (uint32_t s = 0; s < nsect; s++) {
		SD_RESULT r = SD_ERROR;
		for (int i = 0; i < SD_XFER_RETRY; i++) {
			r = SD_WriteBlocks(block_num * spb + s, (u8 *)p, 1);
			if (r == SD_OK) {
				break;
			}
		}
		if (r != SD_OK) {
			RTK_LOGE(TAG, "sd_write fail sector=%u result=%d\n",
					 (unsigned)(block_num * spb + s), (int)r);
			return -1;
		}
		p += SD_SECTOR_SIZE;
	}
	return 0;
}

static int sd_sync(const struct realfs_bdev *bdev)
{
	(void)bdev;   /* SD HAL has no exposed write cache (CTRL_SYNC is a no-op) */
	return 0;
}

int realfs_bdev_sd_open(struct realfs_bdev *bdev, uint32_t block_size)
{
	if (!bdev || block_size < SD_SECTOR_SIZE || (block_size % SD_SECTOR_SIZE)) {
		return -1;
	}

	if (!sd_inited) {
		if (rtos_sema_create(&sd_sema, 0, 1) != RTK_SUCCESS) {
			return -1;
		}
		SD_SetSema(sd_take, sd_give);
		if (SD_Init() != SD_OK) {
			RTK_LOGE(TAG, "SD_Init failed (no card?)\n");
			SD_SetSema(NULL, NULL);
			rtos_sema_delete(sd_sema);
			sd_sema = NULL;
			return -1;
		}
		sd_inited = 1;
	}

	u32 sectors = 0;
	if (SD_GetCapacity(&sectors) != SD_OK || sectors == 0) {
		RTK_LOGE(TAG, "SD_GetCapacity failed\n");
		return -1;
	}

	uint32_t spb = block_size / SD_SECTOR_SIZE;
	memset(bdev, 0, sizeof(*bdev));
	bdev->read_blocks  = sd_read;
	bdev->write_blocks = sd_write;
	bdev->sync         = sd_sync;
	bdev->block_size   = block_size;
	bdev->block_count  = sectors / spb;

	RTK_LOGI(TAG, "SD bdev: %u sectors -> %u blocks of %u B\n",
			 (unsigned)sectors, (unsigned)bdev->block_count, (unsigned)block_size);
	return 0;
}

void realfs_bdev_sd_close(struct realfs_bdev *bdev)
{
	(void)bdev;
	if (sd_inited) {
		SD_DeInit();
		SD_SetSema(NULL, NULL);
		if (sd_sema) {
			rtos_sema_delete(sd_sema);
			sd_sema = NULL;
		}
		sd_inited = 0;
	}
}

#else  /* !CONFIG_SUPPORT_SDIOH : stubs so callers still link */

int realfs_bdev_sd_open(struct realfs_bdev *bdev, uint32_t block_size)
{
	(void)bdev;
	(void)block_size;
	return -1;
}

void realfs_bdev_sd_close(struct realfs_bdev *bdev)
{
	(void)bdev;
}

#endif /* CONFIG_SUPPORT_SDIOH */
