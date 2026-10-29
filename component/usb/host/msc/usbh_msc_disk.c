/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"
#include "usbh_msc_disk.h"
#include "usbh_msc_scsi.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

DSTATUS USB_disk_status(void);
DSTATUS USB_disk_initialize(void);
DSTATUS USB_disk_deinitialize(void);
DRESULT USB_disk_read(BYTE *buff, DWORD sector, u32 count);
#if _USE_WRITE == 1
DRESULT USB_disk_write(const BYTE *buff, DWORD sector, u32 count);
#endif
#if _USE_IOCTL == 1
DRESULT USB_disk_ioctl(BYTE cmd, void *buff);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

/* Exported variables --------------------------------------------------------*/

/* USB Disk Driver */
ll_diskio_drv USB_disk_Driver = {
	.disk_initialize = USB_disk_initialize,
	.disk_deinitialize = USB_disk_deinitialize,
	.disk_status = USB_disk_status,
	.disk_read = USB_disk_read,
#if _USE_WRITE == 1
	.disk_write = USB_disk_write,
#endif
#if _USE_IOCTL == 1
	.disk_ioctl = USB_disk_ioctl,
#endif
	.TAG = (unsigned char *)"USB"
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Map a failed READ/WRITE to a FatFs result.
  * @note   The sense data is cleared when a command starts and filled only by its REQUEST
  *         SENSE, so it belongs to the command that just failed.
  * @param  is_write: 1 for a WRITE, so a write-protected medium is reported as such
  * @retval DRESULT
  */
static DRESULT USB_disk_error(u8 is_write)
{
	usbh_msc_lun_t info = {0};
	DRESULT res;

	if (usbh_msc_get_lun_info(0U, &info) != HAL_OK) {
		/* Device gone or not set up any more */
		return RES_NOTRDY;
	}

	switch (info.sense.asc) {
	case SCSI_ASC_WRITE_PROTECTED:
		res = (is_write != 0U) ? RES_WRPRT : RES_ERROR;
		break;

	case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
	case SCSI_ASC_MEDIUM_NOT_PRESENT:
	case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
		res = RES_NOTRDY;
		break;

	default:
		res = RES_ERROR;
		break;
	}

	return res;
}

/* Exported functions --------------------------------------------------------*/

DSTATUS USB_disk_status(void)
{
	DSTATUS stat;

	if (usbh_msc_unit_is_ready(0U) != 0) {
		stat = 0U;
	} else {
		stat = STA_NOINIT;
	}

	return stat;
}

DSTATUS USB_disk_initialize(void)
{
	usbh_msc_lun_t info = {0};
	DSTATUS stat;

	/* Ready once the LUN came up with a usable geometry and was not retired. A failed command
	   alone does not count: the mount that follows is what tells a working medium again. */
	if ((usbh_msc_get_lun_info(0U, &info) == HAL_OK) && (info.retired == 0U) && (info.capacity.block_size != 0U)) {
		stat = 0U;
	} else {
		stat = STA_NOINIT | STA_NODISK;
	}

	return stat;
}

DSTATUS USB_disk_deinitialize(void)
{
	return 0U;
}

/* Read sector(s) --------------------------------------------*/
DRESULT USB_disk_read(BYTE *buff, DWORD sector, u32 count)
{
	DRESULT res = RES_ERROR;
	int status = HAL_OK;

#if USBH_MSC_RETRY_ENABLE
	int i;

	for (i = 0; i < USBH_MSC_RETRY_CNT; ++i) {
		status = usbh_msc_read(0U, sector, buff, count);

		if (status == HAL_OK) {
			res = RES_OK;
			break;
		} else {
			res = USB_disk_error(0U);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk read fail %d/%d, try %d\n", status, res, i + 1);
		}
	}
#else
	status = usbh_msc_read(0U, sector, buff, count);
	if (status == HAL_OK) {
		res = RES_OK;
	} else {
		res = USB_disk_error(0U);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk read fail %d/%d\n", status, res);
	}
#endif

	return res;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT USB_disk_write(const BYTE *buff, DWORD sector, u32 count)
{
	DRESULT res = RES_ERROR;
	int status = HAL_OK;

#if USBH_MSC_RETRY_ENABLE
	int i;

	for (i = 0; i < USBH_MSC_RETRY_CNT; ++i) {
		status = usbh_msc_write(0U, sector, buff, count);
		if (status == HAL_OK) {
			res = RES_OK;
			break;
		} else {
			res = USB_disk_error(1U);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk write fail %d/%d, try %d\n", status, res, i + 1);
		}
	}
#else
	status = usbh_msc_write(0U, sector, buff, count);
	if (status == HAL_OK) {
		res = RES_OK;
	} else {
		res = USB_disk_error(1U);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk write fail %d/%d\n", status, res);
	}
#endif

	return res;
}
#endif

/* Write sector(s) --------------------------------------------*/
#if _USE_IOCTL == 1
DRESULT USB_disk_ioctl(BYTE cmd, void *buff)
{
	DRESULT res = RES_ERROR;
	usbh_msc_lun_t info = {0};

	switch (cmd) {
	/* Make sure that no pending write process */
	case CTRL_SYNC:
		res = RES_OK;
		break;

	/* Get total number of sectors on the disk (LBA_t, 64-bit when FF_LBA64) */
	case GET_SECTOR_COUNT :
		if (usbh_msc_get_lun_info(0U, &info) == HAL_OK) {
			*(LBA_t *)buff = (LBA_t)info.capacity.block_nbr;
			res = RES_OK;
		} else {
			res = RES_ERROR;
		}
		break;

	/* Get R/W sector size (WORD): ff.c passes &FATFS.ssize, a 16-bit field */
	case GET_SECTOR_SIZE :
		if (usbh_msc_get_lun_info(0U, &info) == HAL_OK) {
			*(WORD *)buff = (WORD)info.capacity.block_size;
			res = RES_OK;
		} else {
			res = RES_ERROR;
		}
		break;

	/* Get erase block size in unit of sector (DWORD) */
	case GET_BLOCK_SIZE :

		if (usbh_msc_get_lun_info(0U, &info) == HAL_OK) {
			*(DWORD *)buff = info.capacity.block_size / 512;
			res = RES_OK;
		} else {
			res = RES_ERROR;
		}
		break;

	default:
		res = RES_PARERR;
	}

	return res;
}
#endif

