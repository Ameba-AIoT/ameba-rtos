/*
 *  Routines to associate Flash driver with FatFs
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "os_wrapper.h"
#include "flash_api.h"
#ifdef CONFIG_FATFS_SECOND_FLASH
#include "vfs_second_nor_flash.h"
#endif

#define SECTOR_SIZE_FLASH	512
#define SECTOR_NUM 8
#define FLASH_BLOCK_SIZE (SECTOR_NUM * SECTOR_SIZE_FLASH)

#ifndef FLASH_APP_BASE
u32 FLASH_APP_BASE;
#endif
u32 FLASH_SECTOR_COUNT;
u32 SECOND_FLASH_SECTOR_COUNT;

flash_t	flash;

DSTATUS FLASH_disk_status(void)
{
	return RES_OK;
}

DSTATUS FLASH_disk_initialize(void)
{
	return RES_OK;
}

DSTATUS FLASH_disk_deinitialize(void)
{
	return RES_OK;
}

#if _USE_IOCTL == 1
DRESULT FLASH_disk_ioctl(BYTE cmd, void *buff)
{
	DRESULT res = RES_ERROR;
	// FLASH_RESULT result;

	switch (cmd) {
	/* Generic command (used by FatFs) */

	/* Make sure that no pending write process in the physical drive */
	case CTRL_SYNC:		/* Flush disk cache (for write functions) */
		res = RES_OK;
		break;
	case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
		*(DWORD *)buff = FLASH_SECTOR_COUNT;
		res = RES_OK;
		break;
	/* for case _MAX_SS != _MIN_SS */
	case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
		*(WORD *)buff = SECTOR_SIZE_FLASH;	//4096 or 2048 or 1024
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:	/* Get erase block size (for only f_mkfs()) */
		*(DWORD *)buff = FLASH_BLOCK_SIZE;
		res = RES_ERROR;
		break;
	case CTRL_ERASE_SECTOR:/* Force erased a block of sectors (for only _USE_ERASE) */
		res = RES_OK;
		break;
	default:
		res = RES_PARERR;
		break;
	}
	return res;
}
#endif

/* Read sector(s) --------------------------------------------*/
DRESULT FLASH_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	flash_stream_read(&flash, FLASH_APP_BASE + sector * SECTOR_SIZE_FLASH, count * SECTOR_SIZE_FLASH, (uint8_t *) buff);

	return RES_OK;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT FLASH_disk_write(const BYTE *buff, DWORD sector, UINT count)
{
	u8 sector_index = sector % SECTOR_NUM;
	u8 *flash_sector_buffer = (u8 *)rtos_mem_malloc(FLASH_BLOCK_SIZE);

	//deal with fisrt flash sector
	flash_stream_read(&flash, FLASH_APP_BASE + (sector / SECTOR_NUM) * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, flash_sector_buffer);
	memcpy(flash_sector_buffer + (sector_index * SECTOR_SIZE_FLASH), (BYTE *)buff,
		   ((count + sector_index <= SECTOR_NUM) ? count : (u32)(SECTOR_NUM - sector_index))*SECTOR_SIZE_FLASH);
	flash_erase_sector(&flash, FLASH_APP_BASE + (sector / SECTOR_NUM) * FLASH_BLOCK_SIZE);
	flash_stream_write(&flash, FLASH_APP_BASE + (sector / SECTOR_NUM) * FLASH_BLOCK_SIZE, FLASH_BLOCK_SIZE, flash_sector_buffer);
	rtos_mem_free(flash_sector_buffer);
	flash_sector_buffer = NULL;
	return RES_OK;
}
#endif

ll_diskio_drv FLASH_disk_Driver = {
	.disk_initialize = FLASH_disk_initialize,
	.disk_status = FLASH_disk_status,
	.disk_read = FLASH_disk_read,
	.disk_deinitialize = FLASH_disk_deinitialize,
#if _USE_WRITE == 1
	.disk_write = FLASH_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = FLASH_disk_ioctl,
#endif
	.TAG	= (unsigned char *)"FLASH"
};

#ifdef CONFIG_FATFS_SECOND_FLASH
#if _USE_IOCTL == 1
DRESULT FLASH_second_disk_ioctl(BYTE cmd, void *buff)
{
	DRESULT res = RES_ERROR;
	// FLASH_RESULT result;

	switch (cmd) {
	/* Generic command (used by FatFs) */

	/* Make sure that no pending write process in the physical drive */
	case CTRL_SYNC:		/* Flush disk cache (for write functions) */
		res = RES_OK;
		break;
	case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
		*(DWORD *)buff = SECOND_FLASH_SECTOR_COUNT;
		res = RES_OK;
		break;
	/* for case _MAX_SS != _MIN_SS */
	case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
		*(WORD *)buff = SECTOR_SIZE_FLASH;	//4096 or 2048 or 1024
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:	/* Get erase block size (for only f_mkfs()) */
		*(DWORD *)buff = FLASH_BLOCK_SIZE;
		res = RES_ERROR;
		break;
	case CTRL_ERASE_SECTOR:/* Force erased a block of sectors (for only _USE_ERASE) */
		res = RES_OK;
		break;
	default:
		res = RES_PARERR;
		break;
	}
	return res;
}
#endif

/* Read sector(s) --------------------------------------------*/
DRESULT FLASH_second_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	int res = 0;
#ifdef CONFIG_SECOND_FLASH_NOR
	DATA_FLASH_ReadStream(sector * SECTOR_SIZE_FLASH, count * SECTOR_SIZE_FLASH, (u8 *)buff);
#else
	res = second_flash_read_stream(sector * SECTOR_SIZE_FLASH, count * SECTOR_SIZE_FLASH, (char *)buff);
#endif
	return res == 0 ? RES_OK : RES_ERROR;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT FLASH_second_disk_write(const BYTE *buff, DWORD sector, UINT count)
{
	int res = 0;
	u8 sector_index = sector % SECTOR_NUM;
	u32 start_addr = (sector / SECTOR_NUM) * FLASH_BLOCK_SIZE;
	char *flash_sector_buffer = rtos_mem_malloc(FLASH_BLOCK_SIZE);

#ifdef CONFIG_SECOND_FLASH_NOR
	DATA_FLASH_ReadStream(start_addr, FLASH_BLOCK_SIZE, (u8 *)flash_sector_buffer);
	memcpy(flash_sector_buffer + (sector_index * SECTOR_SIZE_FLASH), (BYTE *)buff, count * SECTOR_SIZE_FLASH);
	DATA_FLASH_EraseXIP(EraseSector, start_addr);
	DATA_FLASH_WriteStream(start_addr, FLASH_BLOCK_SIZE, (u8 *)flash_sector_buffer);
#else
	res |= second_flash_read_stream(start_addr, FLASH_BLOCK_SIZE, flash_sector_buffer);
	memcpy(flash_sector_buffer + (sector_index * SECTOR_SIZE_FLASH), (BYTE *)buff, count * SECTOR_SIZE_FLASH);
	res |= second_flash_erase_sector(start_addr);
	res |= second_flash_write_stream(start_addr, FLASH_BLOCK_SIZE, flash_sector_buffer);
#endif
	rtos_mem_free(flash_sector_buffer);
	flash_sector_buffer = NULL;
	return res == 0 ? RES_OK : RES_ERROR;
}
#endif

ll_diskio_drv FLASH_second_disk_Driver = {
	.disk_initialize = FLASH_disk_initialize,
	.disk_status = FLASH_disk_status,
	.disk_read = FLASH_second_disk_read,
	.disk_deinitialize = FLASH_disk_deinitialize,
#if _USE_WRITE == 1
	.disk_write = FLASH_second_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = FLASH_second_disk_ioctl,
#endif
	.TAG	= (unsigned char *)"SECOND_FLASH"
};
#endif