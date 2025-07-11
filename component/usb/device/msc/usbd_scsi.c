/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_scsi.h"
#include "usbd_msc.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_scsi_test_unit_ready(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_inquiry(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_read_format_capacity(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_read_capacity10(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_request_sense(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_start_stop_unit(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_prevent_allow(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_mode_sense6(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_mode_sense10(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_write(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_read(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_verify10(usbd_msc_dev_t  *cdev, u8 *params);
static int usbd_scsi_check_address_range(usbd_msc_dev_t  *cdev, u32 blk_offset, u32 blk_nbr);
static int usbd_scsi_process_read(usbd_msc_dev_t  *cdev);
static int usbd_scsi_process_write(usbd_msc_dev_t  *cdev);

/* Private variables ---------------------------------------------------------*/

/* MSC page 0 inquiry data */
static const u8 usbd_msc_page0_inquiry_data[] = {
	0x00,
	0x00,
	0x00,
	(PAGE00_INQUIRY_DATA_LEN - 4U),
	0x00,
	0x80,
	0x83
};

/* MSC standard inquiry data */
static const u8 usbd_msc_standard_inquiry_data[] = {
	0x00,          /* Direct Access Device */
	0x80,          /* RMB = 1: Removable Medium */
	0x02,          /* Version: No conformance claim to standard */
	0x02,
	36 - 4,          /* Additional Length */
	0x00,          /* SCCS = 1: Storage Controller Component */
	0x00,
	0x00,
	/* Vendor Identification */
	'R', 'e', 'a', 'l', 'T', 'e', 'k', ' ',
	/* Product Identification */
	'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
	'D', 'i', 's', 'k', ' ', ' ', ' ',
	/* Product Revision Level */
	'1', '.', '0', ' '
};

/* MSC sense 6 data */
static const u8 usbd_msc_mode_sense6_data[] = {
	0x03,
	0x00,
	0x00,
	0x00
};

/* MSC sense 10 data */
static const u8 usbd_msc_mode_sense10_data[] = {
	0x00,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Process SCSI Test Unit Ready Command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_test_unit_ready(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	/* case 9 : Hi > D0 */
	if (cdev->cbw->dCBWDataTransferLength != 0U) {
		usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);

		return -1;
	}

	if (cdev->is_open != 1) {
		usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
		cdev->bot_state = USBD_MSC_NO_DATA;

		return -1;
	}
	cdev->data_length = 0U;

	return 0;
}

/**
* @brief  Process Inquiry command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int  usbd_scsi_inquiry(usbd_msc_dev_t *cdev, u8 *params)
{
	if (params[1] & 0x01U) { /*Evpd is set*/
		cdev->data_length = PAGE00_INQUIRY_DATA_LEN;
		usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_page0_inquiry_data, cdev->data_length);
	} else {
		cdev->data_length = MIN(params[4], STANDARD_INQUIRY_DATA_LEN);
		usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_standard_inquiry_data, cdev->data_length);
	}

	return 0;
}

/**
* @brief  Process Read Capacity 10 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_read_capacity10(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	if (cdev->disk_ops.disk_getcapacity) {
		if (cdev->disk_ops.disk_getcapacity(&cdev->num_sectors) != 0) {
			usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		} else {
			cdev->data[0] = (u8)((cdev->num_sectors - 1U) >> 24);
			cdev->data[1] = (u8)((cdev->num_sectors - 1U) >> 16);
			cdev->data[2] = (u8)((cdev->num_sectors - 1U) >>  8);
			cdev->data[3] = (u8)(cdev->num_sectors - 1U);

			cdev->data[4] = (u8)(cdev->blksize >>  24);
			cdev->data[5] = (u8)(cdev->blksize >>  16);
			cdev->data[6] = (u8)(cdev->blksize >>  8);
			cdev->data[7] = (u8)(cdev->blksize);

			cdev->data_length = READ_CAPACITY10_DATA_LEN;
			return 0;
		}
	} else {
		usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
		return -1;
	}
}
/**
* @brief  Process Read Format Capacity command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_read_format_capacity(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	usb_os_memset((void *)cdev->data, 0U, READ_FORMAT_CAPACITY_DATA_LEN);

	if (cdev->disk_ops.disk_getcapacity) {
		if (cdev->disk_ops.disk_getcapacity(&cdev->num_sectors) != 0) {
			usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		} else {
			cdev->data[3] = 0x08U;
			cdev->data[4] = (u8)((cdev->num_sectors - 1U) >> 24);
			cdev->data[5] = (u8)((cdev->num_sectors - 1U) >> 16);
			cdev->data[6] = (u8)((cdev->num_sectors - 1U) >>  8);
			cdev->data[7] = (u8)(cdev->num_sectors - 1U);

			cdev->data[8] = 0x02U;
			cdev->data[9] = (u8)(cdev->blksize >>  16);
			cdev->data[10] = (u8)(cdev->blksize >>  8);
			cdev->data[11] = (u8)(cdev->blksize);

			cdev->data_length = READ_FORMAT_CAPACITY_DATA_LEN;
			return 0;
		}
	} else {
		usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
		return -1;
	}
}
/**
* @brief  Process Mode Sense6 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_mode_sense6(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	cdev->data_length = MODE_SENSE6_DATA_LEN;
	usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_mode_sense6_data, cdev->data_length);

	return 0;
}

/**
* @brief  Process Mode Sense10 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_mode_sense10(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	cdev->data_length = MODE_SENSE10_DATA_LEN;
	usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_mode_sense10_data, cdev->data_length);

	return 0;
}

/**
* @brief  Process Request Sense command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_request_sense(usbd_msc_dev_t *cdev, u8 *params)
{
	usbd_msc_scsi_sense_data_t *data;

	UNUSED(params);

	usb_os_memset((void *)cdev->data, 0U, REQUEST_SENSE_DATA_LEN);

	cdev->data[0]	= 0x70U;
	cdev->data[7]	= REQUEST_SENSE_DATA_LEN - 6U;

	if ((cdev->scsi_sense_head != cdev->scsi_sense_tail)) {
		data = &cdev->scsi_sense_data[cdev->scsi_sense_head];
		cdev->data[2]     = data->skey;
		cdev->data[12]    = data->w.b.ascq;
		cdev->data[13]    = data->w.b.asc;
		cdev->scsi_sense_head++;

		if (cdev->scsi_sense_head == USBD_MSC_SENSE_LIST_DEPTH) {
			cdev->scsi_sense_head = 0U;
		}
	}
	cdev->data_length = MIN(params[4], REQUEST_SENSE_DATA_LEN);

	return 0;
}

/**
* @brief  Process Start Stop Unit command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_start_stop_unit(usbd_msc_dev_t *cdev, u8 *params)
{
	int     loej, start;

	cdev->data_length = 0U;

	loej  = params[4] & 0x02;
	start = params[4] & 0x01;

	/*
	 * Our emulation doesn't support mounting; the medium is
	 * available for use as soon as it is loaded.
	 */
	if (start) {
		if (cdev->is_open != 1) {
			usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		return 0;
	}

	if (!loej) {
		return 0;
	}
	cdev->is_open = 0;

	return 0;
}

/**
* @brief  Process Start Stop Unit command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_prevent_allow(usbd_msc_dev_t *cdev, u8 *params)
{
	UNUSED(params);

	cdev->data_length = 0U;

	return 0;
}

/**
* @brief  Process Read10/12 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_read(usbd_msc_dev_t *cdev, u8 *params)
{
	usbd_msc_cbw_t *cbw = cdev->cbw;

	if (cdev->bot_state == USBD_MSC_IDLE) { /* Idle */
		/* case 2 */
		if ((cbw->dCBWDataTransferLength) == 0) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			cdev->phase_error = 1;
			return -1;
		}

		/* case 10 : Ho <> Di */
		if ((cbw->bmCBWFlags & 0x80U) != 0x80U) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		if (cdev->is_open != 1) {
			usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		cdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_READ10) {
			cdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_READ12) {
			cdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		if (usbd_scsi_check_address_range(cdev, cdev->lba, cdev->blklen) < 0) {
			return -1; /* error */
		}

		/* case 4 : Hi > Dn */
		if (cdev->blklen == 0 || (cbw->dCBWDataTransferLength > (cdev->blklen * cdev->blksize))) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		cdev->bot_state = USBD_MSC_DATA_IN;
	}
	cdev->data_length = USBD_MSC_BUFLEN;

	return usbd_scsi_process_read(cdev);
}

/**
* @brief  Process Write10/12 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_write(usbd_msc_dev_t *cdev, u8 *params)
{
	u32 len;
	usbd_msc_cbw_t *cbw = cdev->cbw;

	if (cdev->bot_state == USBD_MSC_IDLE) { /* Idle */
		/* case 3 : Hn < Do */
		if (cbw->dCBWDataTransferLength == 0) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			cdev->phase_error = 1;
			return -1;
		}

		/* case 8 : Hi <> Do */
		if ((cbw->bmCBWFlags & 0x80U) == 0x80U) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		/* Check whether Media is ready */
		if (cdev->is_open != 1) {
			usbd_scsi_sense_code(cdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		/* Check If media is write-protected */
		if (cdev->ro != 0) {
			usbd_scsi_sense_code(cdev, NOT_READY, WRITE_PROTECTED);
			return -1;
		}

		cdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_WRITE10) {
			cdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_WRITE12) {
			cdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		/* check if LBA address is in the right range */
		if (usbd_scsi_check_address_range(cdev, cdev->lba, cdev->blklen) < 0) {
			return -1; /* error */
		}

		len = cdev->blklen * cdev->blksize;

		/* case 11,13 : Ho <> Do */
		if (cbw->dCBWDataTransferLength != len) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		len = MIN(len, USBD_MSC_BUFLEN);

		/* Prepare EP to receive first data packet */
		cdev->bot_state = USBD_MSC_DATA_OUT;
		usbd_msc_bulk_receive(cdev->dev, cdev->data, len);
	} else { /* Write Process ongoing */
		return usbd_scsi_process_write(cdev);
	}
	return 0;
}

/**
* @brief  Process Verify10 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_verify10(usbd_msc_dev_t *cdev, u8 *params)
{
	if ((params[1] & 0x02U) == 0x02U) {
		usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		return -1; /* Error, Verify Mode Not supported*/
	}

	if (usbd_scsi_check_address_range(cdev, cdev->lba, cdev->blklen) < 0) {
		return -1; /* error */
	}
	cdev->data_length = 0U;
	return 0;
}

/**
* @brief  Check address range
* @param  cdev: Device instance
* @param  blk_offset: first block address
* @param  blk_nbr: number of block to be processed
* @retval Status
*/
static int usbd_scsi_check_address_range(usbd_msc_dev_t *cdev, u32 blk_offset, u32 blk_nbr)
{
	if ((blk_offset + blk_nbr) > cdev->num_sectors) {
		usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
		return -1;
	}
	return 0;
}

/**
* @brief  Handle Read Process
* @param  cdev: Device instance
* @retval Status
*/
static int usbd_scsi_process_read(usbd_msc_dev_t *cdev)
{
	u32 len = cdev->blklen * cdev->blksize;
	usbd_msc_cbw_t *cbw = cdev->cbw;

	if (cbw->dCBWDataTransferLength < len) {
		cdev->phase_error = 1;
		len = cbw->dCBWDataTransferLength;
		cdev->blklen = (len >> cdev->blkbits);
	}

	len = MIN(len, USBD_MSC_BUFLEN);

	if (cdev->disk_ops.disk_read(cdev->lba, cdev->data, (len >> cdev->blkbits))) {
		usbd_scsi_sense_code(cdev, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
		return -1;
	}

	usbd_msc_bulk_transmit(cdev->dev, cdev->data, len);

	cdev->lba += (len >> cdev->blkbits);
	cdev->blklen -= (len >> cdev->blkbits);

	/* case 6 : Hi = Di */
	cdev->csw->dCSWDataResidue -= len;

	if (cdev->blklen == 0U) {
		cdev->bot_state = USBD_MSC_LAST_DATA_IN;
	}
	return 0;
}

/**
* @brief  Handle Write Process
* @param  cdev: Device instance
* @retval Status
*/
static int usbd_scsi_process_write(usbd_msc_dev_t *cdev)
{
	u32 len = cdev->blklen * cdev->blksize;

	len = MIN(len, USBD_MSC_BUFLEN);

	if (cdev->disk_ops.disk_write(cdev->lba, cdev->data, (len >> cdev->blkbits))) {
		usbd_scsi_sense_code(cdev, HARDWARE_ERROR, WRITE_FAULT);
		return -1;
	}

	cdev->lba += (len >> cdev->blkbits);
	cdev->blklen -= (len >> cdev->blkbits);

	/* case 12 : Ho = Do */
	cdev->csw->dCSWDataResidue -= len;

	if (cdev->blklen == 0U) {
		usbd_msc_send_csw(cdev->dev, USBD_MSC_CSW_CMD_PASSED);
	} else {
		len = MIN((cdev->blklen * cdev->blksize), USBD_MSC_BUFLEN);
		/* Prepare EP to Receive next packet */
		usbd_msc_bulk_receive(cdev->dev, cdev->data, len);
	}

	return 0;
}

/* Exported functions --------------------------------------------------------*/

/**
* @brief  Process SCSI commands
* @param  cdev: Device instance
* @param  cmd: Command parameters
* @retval Status
*/
int usbd_scsi_process_cmd(usbd_msc_dev_t *cdev, u8 *cmd)
{
	int ret = 0;

	switch (cmd[0]) {
	case SCSI_TEST_UNIT_READY:
		ret = usbd_scsi_test_unit_ready(cdev, cmd);
		break;

	case SCSI_REQUEST_SENSE:
		ret = usbd_scsi_request_sense(cdev, cmd);
		break;
	case SCSI_INQUIRY:
		ret = usbd_scsi_inquiry(cdev, cmd);
		break;

	case SCSI_START_STOP_UNIT:
		ret = usbd_scsi_start_stop_unit(cdev, cmd);
		break;

	case SCSI_ALLOW_MEDIUM_REMOVAL:
		ret = usbd_scsi_prevent_allow(cdev, cmd);
		break;

	case SCSI_MODE_SENSE6:
		ret = usbd_scsi_mode_sense6(cdev, cmd);
		break;

	case SCSI_MODE_SENSE10:
		ret = usbd_scsi_mode_sense10(cdev, cmd);
		break;

	case SCSI_READ_FORMAT_CAPACITIES:
		ret = usbd_scsi_read_format_capacity(cdev, cmd);
		break;

	case SCSI_READ_CAPACITY10:
		ret = usbd_scsi_read_capacity10(cdev, cmd);
		break;

	case SCSI_READ12:
	case SCSI_READ10:
		ret = usbd_scsi_read(cdev, cmd);
		break;

	case SCSI_WRITE12:
	case SCSI_WRITE10:
		ret = usbd_scsi_write(cdev, cmd);
		break;

	case SCSI_MODE_SELECT6:
	case SCSI_MODE_SELECT10:
		cdev->data_length = 0U;
		ret = 0;
		break;

	case SCSI_VERIFY10:
		ret = usbd_scsi_verify10(cdev, cmd);
		break;

	default:
		usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
		return -1;
	}
	return ret;
}

/**
* @brief  Load the last error code in the error list
* @param  cdev: Device instance
* @param  skey: Sense Key
* @param  asc: Additional Sense Key
* @retval none

*/
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 skey, u8 asc)
{
	usbd_msc_scsi_sense_data_t *data = &cdev->scsi_sense_data[cdev->scsi_sense_tail];
	data->skey  = skey;
	data->w.asc = asc << 8;
	cdev->scsi_sense_tail++;
	if (cdev->scsi_sense_tail == USBD_MSC_SENSE_LIST_DEPTH) {
		cdev->scsi_sense_tail = 0U;
	}
}