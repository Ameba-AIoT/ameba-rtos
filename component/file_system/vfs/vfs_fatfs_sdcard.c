/*
 *  Routines to associate SD card driver with FatFs
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "vfs.h"
#include "vfs_fatfs.h"
#include "os_wrapper.h"
#include "ameba.h"
#include "ameba_sd.h"

#ifndef SD_BLOCK_SIZE
#define SD_BLOCK_SIZE	512
#endif

static rtos_mutex_t sd_mutex = NULL;
static int init_status = 0;
static rtos_sema_t vfs_sd_sema;

static int sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return  rtos_sema_give(vfs_sd_sema);
}

static int sd_take_sema(u32 timeout)
{
	return rtos_sema_take(vfs_sd_sema, timeout);
}

static void sd_sema_init(void)
{
	rtos_sema_create(&vfs_sd_sema, 0, 1);
	SD_SetSema(sd_take_sema, sd_give_sema);
}

static void sd_sema_deinit(void)
{
	rtos_sema_delete(vfs_sd_sema);
	SD_SetSema(NULL, NULL);
}

static void sd_lock(void)
{
	if (sd_mutex == NULL) {
		rtos_mutex_create(&sd_mutex);
	}

	if (RTK_FAIL == rtos_mutex_take(sd_mutex, MUTEX_WAIT_TIMEOUT)) {
		RTK_LOGE(NOTAG, "%s take mutex fail!\n");
	}
}

static void sd_unlock(void)
{
	if (sd_mutex != NULL) {
		rtos_mutex_give(sd_mutex);
	}
}

DSTATUS interpret_sd_status(SD_RESULT result)
{
	DSTATUS ret = 0;
	if (result == SD_OK) {
		ret = 0;
	} else if (result == SD_NODISK) {
		ret = STA_NODISK;
	} else if (result == SD_INSERT) {
		ret = STA_INSERT;
	} else if (result == SD_INITERR) {
		ret = STA_NOINIT;
	} else if (result == SD_PROTECTED) {
		ret = STA_PROTECT;
	}

	return ret;
}

DRESULT interpret_sd_result(SD_RESULT result)
{
	DRESULT ret = 0;
	if (result == SD_OK) {
		ret = RES_OK;
	} else if (result == SD_PROTECTED) {
		ret = RES_WRPRT;
	} else if (result == SD_ERROR) {
		ret = RES_ERROR;
	}
	return ret;
}

DSTATUS SD_disk_status(void)
{
	SD_RESULT res;
	res = SD_Status();
	return interpret_sd_status(res);;
}

DSTATUS SD_disk_initialize(void)
{
	SD_RESULT res;

	sd_sema_init();
	sd_lock();
	res = SD_Init();
	init_status = 1;//The card is not init
	sd_unlock();
	return interpret_sd_status(res);
}

DSTATUS SD_disk_deinitialize(void)
{
	SD_RESULT res;
	sd_lock();
	init_status = 0;//The card is not init
	res = SD_DeInit();
	sd_unlock();
	sd_sema_deinit();
	return interpret_sd_status(res);
}

/* Read sector(s) --------------------------------------------*/
DRESULT SD_disk_read(BYTE *buff, DWORD sector, /*unsigned int*/UINT count)
{
	DRESULT res;
	char retry_cnt = 0;
	sd_lock();
	do {
		if (!init_status) {
			res = STA_NODISK;
			break;
		}
		res = interpret_sd_result(SD_ReadBlocks(sector, buff, count));
		if (++retry_cnt >= 3) {
			break;
		}
	} while (res != RES_OK);
	sd_unlock();

	return res;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT SD_disk_write(const BYTE *buff, DWORD sector, /*unsigned int*/UINT count)
{
	DRESULT res;
	char retry_cnt = 0;
	sd_lock();
	do {
		if (!init_status) {
			res = STA_NODISK;
		}
		res = interpret_sd_result(SD_WriteBlocks(sector, (uint8_t *)buff, count));
		if (++retry_cnt >= 3) {
			break;
		}
	} while (res != RES_OK);
	sd_unlock();

	return res;
}
#endif

/* Write sector(s) --------------------------------------------*/
#if _USE_IOCTL == 1
DRESULT SD_disk_ioctl(BYTE cmd, void *buff)
{
	DRESULT res = RES_ERROR;
	SD_RESULT result;
	sd_lock();

	switch (cmd) {
	/* Generic command (used by FatFs) */

	/* Make sure that no pending write process in the physical drive */
	case CTRL_SYNC:		/* Flush disk cache (for write functions) */
		/* No write buffer for writing operation */
		res = RES_OK;
		break;
	case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
		result = SD_GetCapacity((unsigned long *) buff);
		res = interpret_sd_result(result);
		break;
	/* for case _MAX_SS != _MIN_SS */
	case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
		*(WORD *)buff = 512; //2048;//4096;
		res = RES_OK;
		break;
	case GET_BLOCK_SIZE:	/* Get erase block size (for only f_mkfs()) */
		*(DWORD *)buff = SD_BLOCK_SIZE;
		res = RES_OK;
		break;
	case CTRL_ERASE_SECTOR:/* Force erased a block of sectors (for only _USE_ERASE) */
		res = RES_OK;
		break;

	/* MMC/SDC specific ioctl command */

	case MMC_GET_TYPE:	/* Get card type */
		res = RES_OK;
		break;
	case MMC_GET_CSD:	/* Get CSD */
		res = RES_OK;
		break;
	case MMC_GET_CID:	/* Get CID */
		res = RES_OK;
		break;
	case MMC_GET_OCR:	/* Get OCR */
		res = RES_OK;
		break;
	case MMC_GET_SDSTAT:/* Get SD status */
		res = RES_OK;
		break;
	default:
		res = RES_PARERR;
		break;
	}

	sd_unlock();

	return res;
}
#endif

ll_diskio_drv SD_disk_Driver = {
	.disk_initialize = SD_disk_initialize,
	.disk_deinitialize = SD_disk_deinitialize,
	.disk_status = SD_disk_status,
	.disk_read = SD_disk_read,
#if _USE_WRITE == 1
	.disk_write = SD_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = SD_disk_ioctl,
#endif
	.TAG	= (unsigned char *)"SD"
};
