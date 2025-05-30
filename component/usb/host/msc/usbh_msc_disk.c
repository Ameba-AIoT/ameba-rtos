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

/* USB Disk Deiver */
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

/* Exported functions --------------------------------------------------------*/

DSTATUS USB_disk_status(void)
{
	DRESULT res = RES_ERROR;

	if (usbh_msc_unit_is_rdy(0U)) {
		res = RES_OK;
	} else {
		res = RES_ERROR;
	}

	return res;
}

DSTATUS USB_disk_initialize(void)
{
	return RES_OK;
}

DSTATUS USB_disk_deinitialize(void)
{
	return RES_OK;
}

/* Read sector(s) --------------------------------------------*/
DRESULT USB_disk_read(BYTE *buff, DWORD sector, u32 count)
{
	DRESULT res = RES_ERROR;
	usbh_msc_lun_t info;
	int status = HAL_OK;

#if CONFIG_USBH_MSC_RETRY
	int i;

	for (i = 0; i < USBH_MSC_RETRY_CNT; ++i) {
		status = usbh_msc_read(0U, sector, buff, count);

		if (status == HAL_OK) {
			res = RES_OK;
			break;
		} else {
			usbh_msc_get_lun_info(0U, &info);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk read fail %d/%d, try %d\n", status, info.sense.asc, i + 1);

			switch (info.sense.asc) {
			case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
			case SCSI_ASC_MEDIUM_NOT_PRESENT:
			case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
				res = RES_NOTRDY;
				break;

			default:
				res = RES_ERROR;
				break;
			}
		}
	}
#else
	status = usbh_msc_read(0U, sector, buff, count);
	if (status == HAL_OK) {
		res = RES_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk read fail %d\n", status);

		usbh_msc_get_lun_info(0U, &info);

		switch (info.sense.asc) {
		case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
		case SCSI_ASC_MEDIUM_NOT_PRESENT:
		case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
			res = RES_NOTRDY;
			break;

		default:
			res = RES_ERROR;
			break;
		}
	}
#endif

	return res;
}

/* Write sector(s) --------------------------------------------*/
#if _USE_WRITE == 1
DRESULT USB_disk_write(const BYTE *buff, DWORD sector, u32 count)
{
	DRESULT res = RES_ERROR;
	usbh_msc_lun_t info;
	int status = HAL_OK;

#if CONFIG_USBH_MSC_RETRY
	int i;

	for (i = 0; i < USBH_MSC_RETRY_CNT; ++i) {
		status = usbh_msc_write(0U, sector, (BYTE *)buff, count);
		if (status == HAL_OK) {
			res = RES_OK;
			break;
		} else {
			usbh_msc_get_lun_info(0U, &info);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk write fail %d/%d, try %d\n", status, info.sense.asc, i + 1);

			switch (info.sense.asc) {
			case SCSI_ASC_WRITE_PROTECTED:
				res = RES_WRPRT;
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
		}
	}
#else
	status = usbh_msc_write(0U, sector, (BYTE *)buff, count);
	if (status == HAL_OK) {
		res = RES_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Disk write fail %d\n", status);

		usbh_msc_get_lun_info(0U, &info);

		switch (info.sense.asc) {
		case SCSI_ASC_WRITE_PROTECTED:
			res = RES_WRPRT;
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
	usbh_msc_lun_t info;

	switch (cmd) {
	/* Make sure that no pending write process */
	case CTRL_SYNC:
		res = RES_OK;
		break;

	/* Get number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT :
		if (usbh_msc_get_lun_info(0U, &info) == HAL_OK) {
			*(DWORD *)buff = info.capacity.block_nbr;
			res = RES_OK;
		} else {
			res = RES_ERROR;
		}
		break;

	/* Get R/W sector size (WORD) */
	case GET_SECTOR_SIZE :
		if (usbh_msc_get_lun_info(0U, &info) == HAL_OK) {
			*(DWORD *)buff = info.capacity.block_size;
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

