/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_scsi.h"
#include "usbd_composite_msc.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_composite_scsi_test_unit_ready(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_inquiry(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_read_format_capacity(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_read_capacity10(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_request_sense(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_start_stop_unit(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_prevent_allow(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_mode_sense6(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_mode_sense10(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_write(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_read(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_verify10(usbd_composite_msc_dev_t *mdev, u8 *params);
static int usbd_composite_scsi_check_address_range(usbd_composite_msc_dev_t *mdev, u32 blk_offset, u32 blk_nbr);
static int usbd_composite_scsi_process_read(usbd_composite_msc_dev_t *mdev);
static int usbd_composite_scsi_process_write(usbd_composite_msc_dev_t *mdev);

/* Private variables ---------------------------------------------------------*/

/* MSC page 0 inquiry data: pages 0x00 and 0x80 are supported */
static const u8 usbd_composite_msc_page0_inquiry_data[] = {
	0x00,  /* Peripheral type */
	0x00,  /* Page Code 0x00 */
	0x00,  /* Reserved */
	(PAGE00_INQUIRY_DATA_LEN - 4U),  /* Page Length = 2 */
	0x00,  /* Supported page: 0x00 */
	0x80   /* Supported page: 0x80 */
};

/* MSC page 0x80 inquiry data: unit serial number */
static const u8 usbd_composite_msc_page80_inquiry_data[] = {
	0x00,  /* Peripheral type */
	0x80,  /* Page Code 0x80 */
	0x00,  /* Reserved */
	0x04,  /* Page Length = 4 */
	'0', '0', '0', '1'  /* Serial number */
};

/* MSC standard inquiry data */
static const u8 usbd_composite_msc_standard_inquiry_data[] = {
	0x00,          /* Direct Access Device */
	0x80,          /* RMB = 1: Removable Medium */
	0x02,          /* Version: No conformance claim to standard */
	0x02,
	36 - 5,          /* Additional Length: n-4 where n=35 (last byte index) */
	0x00,          /* SCCS = 0: Not a Storage Controller Component */
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
static const u8 usbd_composite_msc_mode_sense6_data[] = {
	0x03,
	0x00,
	0x00,
	0x00
};

/* MSC sense 10 Data */
static const u8 usbd_composite_msc_mode_sense10_data[] = {
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
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_test_unit_ready(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	/* A non-zero dCBWDataTransferLength on this no-data command (BOT §6.7
	 * Case 4 / Case 9) is handled centrally in usbd_composite_msc_rx_process,
	 * which reports the command result and STALLs Bulk-Out when the host
	 * declared an OUT transfer — the CSW status here stays the command result. */
	if (mdev->is_open != 1) {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
	}
	mdev->data_length = 0U;

	return HAL_OK;
}

/**
* @brief  Process Inquiry command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int  usbd_composite_scsi_inquiry(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	u16 alloc_len;

	if (params[1] & 0x01U) { /*Evpd is set*/
		switch (params[2]) {
		case 0x00U:
			mdev->data_length = PAGE00_INQUIRY_DATA_LEN;
			usb_os_memcpy((void *)mdev->data, (void *)usbd_composite_msc_page0_inquiry_data, mdev->data_length);
			break;
		case 0x80U:
			mdev->data_length = PAGE80_INQUIRY_DATA_LEN;
			usb_os_memcpy((void *)mdev->data, (void *)usbd_composite_msc_page80_inquiry_data, mdev->data_length);
			break;
		default:
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
			return HAL_ERR_PARA;
		}
	} else {
		/* EVPD=0: the PAGE CODE field (byte 2) must be zero; a non-zero page
		 * code is invalid — CHECK CONDITION / INVALID FIELD IN CDB (SPC-4). */
		if (params[2] != 0U) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
			return HAL_ERR_PARA;
		}
		alloc_len = ((u16)params[3] << 8) | params[4];
		mdev->data_length = MIN(alloc_len, INQUIRY_DATA_LEN);
		usb_os_memcpy((void *)mdev->data, (void *)usbd_composite_msc_standard_inquiry_data, mdev->data_length);
	}

	return HAL_OK;
}

/**
* @brief  Process Read Capacity 10 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_read_capacity10(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	if (mdev->disk_ops.disk_getcapacity) {
		if (mdev->disk_ops.disk_getcapacity(&mdev->num_sectors) != 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			mdev->data[0] = (u8)((mdev->num_sectors - 1U) >> 24);
			mdev->data[1] = (u8)((mdev->num_sectors - 1U) >> 16);
			mdev->data[2] = (u8)((mdev->num_sectors - 1U) >>  8);
			mdev->data[3] = (u8)(mdev->num_sectors - 1U);

			mdev->data[4] = (u8)(COMP_MSC_BLK_SIZE >>  24);
			mdev->data[5] = (u8)(COMP_MSC_BLK_SIZE >>  16);
			mdev->data[6] = (u8)(COMP_MSC_BLK_SIZE >>  8);
			mdev->data[7] = (u8)(COMP_MSC_BLK_SIZE);

			mdev->data_length = READ_CAPACITY10_DATA_LEN;
			return HAL_OK;
		}
	} else {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
	}
}
/**
* @brief  Process Read Format Capacity command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_read_format_capacity(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	usb_os_memset((void *)mdev->data, 0U, READ_FORMAT_CAPACITY_DATA_LEN);

	if (mdev->disk_ops.disk_getcapacity) {
		if (mdev->disk_ops.disk_getcapacity(&mdev->num_sectors) != 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			mdev->data[3] = 0x08U;
			mdev->data[4] = (u8)(mdev->num_sectors >> 24);
			mdev->data[5] = (u8)(mdev->num_sectors >> 16);
			mdev->data[6] = (u8)(mdev->num_sectors >>  8);
			mdev->data[7] = (u8)(mdev->num_sectors);

			mdev->data[8] = 0x02U;
			mdev->data[9] = (u8)(COMP_MSC_BLK_SIZE >>  16);
			mdev->data[10] = (u8)(COMP_MSC_BLK_SIZE >>  8);
			mdev->data[11] = (u8)(COMP_MSC_BLK_SIZE);

			mdev->data_length = READ_FORMAT_CAPACITY_DATA_LEN;
			return HAL_OK;
		}
	} else {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
	}
}
/**
* @brief  Process Mode Sense6 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_mode_sense6(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	mdev->data_length = MODE_SENSE6_DATA_LEN;
	usb_os_memcpy((void *)mdev->data, (void *)usbd_composite_msc_mode_sense6_data, mdev->data_length);
	if (mdev->ro) {
		mdev->data[2] |= 0x80U; /* WP bit: Device-Specific Parameter, SPC-3 Table 278 */
	}

	return HAL_OK;
}

/**
* @brief  Process Mode Sense10 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_mode_sense10(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	mdev->data_length = MODE_SENSE10_DATA_LEN;
	usb_os_memcpy((void *)mdev->data, (void *)usbd_composite_msc_mode_sense10_data, mdev->data_length);
	if (mdev->ro) {
		mdev->data[3] |= 0x80U; /* WP bit: Device-Specific Parameter, SPC-3 Table 281 */
	}

	return HAL_OK;
}

/**
* @brief  Process Request Sense command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_request_sense(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	usb_msc_scsi_sense_data_t *data;

	usb_os_memset((void *)mdev->data, 0U, REQUEST_SENSE_DATA_LEN);

	mdev->data[0]	= 0x70U;
	mdev->data[7]	= REQUEST_SENSE_DATA_LEN - 8U; /* Additional Sense Length: n-7, n=17 → 10 */

	if ((mdev->scsi_sense_head != mdev->scsi_sense_tail)) {
		data = &mdev->scsi_sense_data[mdev->scsi_sense_head];
		mdev->data[2]     = data->key;
		mdev->data[12]    = data->asc;
		mdev->data[13]    = data->ascq;
		mdev->scsi_sense_head++;

		if (mdev->scsi_sense_head == COMP_MSC_SENSE_LIST_DEPTH) {
			mdev->scsi_sense_head = 0U;
		}
	}
	mdev->data_length = MIN(params[4], REQUEST_SENSE_DATA_LEN);

	return HAL_OK;
}

/**
* @brief  Process Start Stop Unit command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_start_stop_unit(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	int     loej, start;

	mdev->data_length = 0U;

	loej  = params[4] & 0x02;
	start = params[4] & 0x01;

	/*
	 * Our emulation doesn't support mounting; the medium is
	 * available for use as soon as it is loaded.
	 */
	if (start) {
		if (mdev->is_open != 1) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		return HAL_OK;
	}

	if (!loej) {
		return HAL_OK;
	}
	mdev->is_open = 0;

	return HAL_OK;
}

/**
* @brief  Process Start Stop Unit command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_prevent_allow(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	UNUSED(params);

	mdev->data_length = 0U;

	return HAL_OK;
}

/**
* @brief  Process Read10/12 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_read(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	usb_msc_bot_cbw_t *cbw = mdev->cbw;

	if (mdev->bot_state == COMP_MSC_IDLE) { /* Idle */
		/* case 2 */
		if ((cbw->field.dCBWDataTransferLength) == 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			mdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* case 10 : Ho <> Di */
		if ((cbw->field.bmCBWFlags & 0x80U) != 0x80U) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			mdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		if (mdev->is_open != 1) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		mdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_READ10) {
			mdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_READ12) {
			mdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		if (usbd_composite_scsi_check_address_range(mdev, mdev->lba, mdev->blklen) != HAL_OK) {
			return HAL_ERR_PARA; /* error */
		}

		/* case 4 : Hi > D0 — transfer length is 0 but host expects data */
		if (mdev->blklen == 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			mdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		mdev->bot_state = COMP_MSC_DATA_IN;
	}
	mdev->data_length = COMP_MSC_BUFLEN;

	return usbd_composite_scsi_process_read(mdev);
}

/**
* @brief  Process Write10/12 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_write(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	u32 len;
	usb_msc_bot_cbw_t *cbw = mdev->cbw;

	if (mdev->bot_state == COMP_MSC_IDLE) { /* Idle */
		/* case 3 : Hn < Do */
		if (cbw->field.dCBWDataTransferLength == 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			mdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* case 8 : Hi <> Do */
		if ((cbw->field.bmCBWFlags & 0x80U) == 0x80U) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			mdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* Check whether Media is ready */
		if (mdev->is_open != 1) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		/* Check If media is write-protected */
		if (mdev->ro != 0) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_DATA_PROTECT, SCSI_ASC_WRITE_PROTECTED);
			return HAL_ERR_PARA;
		}

		mdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_WRITE10) {
			mdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_WRITE12) {
			mdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		/* check if LBA address is in the right range */
		if (usbd_composite_scsi_check_address_range(mdev, mdev->lba, mdev->blklen) != HAL_OK) {
			return HAL_ERR_PARA; /* error */
		}

		len = mdev->blklen * COMP_MSC_BLK_SIZE;

		/* case 11,13 : Ho <> Do */
		if (cbw->field.dCBWDataTransferLength != len) {
			usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			return HAL_ERR_PARA;
		}

		len = MIN(len, COMP_MSC_BUFLEN);

		/* Prepare EP to receive first data packet */
		mdev->bot_state = COMP_MSC_DATA_OUT;
		usbd_composite_msc_bulk_receive(mdev->dev, mdev->data, len);
	} else { /* Write Process ongoing */
		return usbd_composite_scsi_process_write(mdev);
	}
	return HAL_OK;
}

/**
* @brief  Process Verify10 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_verify10(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	u32 verify_lba;
	u32 verify_blklen;

	if ((params[1] & 0x02U) == 0x02U) {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
		return HAL_ERR_PARA; /* Error, Verify Mode Not supported*/
	}

	verify_lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];
	verify_blklen = ((u32)params[7] << 8) | (u32)params[8];

	if (verify_blklen == 0) {
		mdev->data_length = 0U;
		return HAL_OK; /* No-op: verification length of 0 means verify nothing */
	}

	if (usbd_composite_scsi_check_address_range(mdev, verify_lba, verify_blklen) != HAL_OK) {
		return HAL_ERR_PARA; /* error */
	}
	mdev->data_length = 0U;
	return HAL_OK;
}

/**
* @brief  Check address range
* @param  mdev: Device instance
* @param  blk_offset: first block address
* @param  blk_nbr: number of block to be processed
* @retval Status
*/
static int usbd_composite_scsi_check_address_range(usbd_composite_msc_dev_t *mdev, u32 blk_offset, u32 blk_nbr)
{
	if ((blk_offset + blk_nbr) > mdev->num_sectors) {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_ADDRESS_OUT_OF_RANGE);
		return HAL_ERR_PARA;
	}
	return HAL_OK;
}

/**
* @brief  Handle Read Process
* @param  mdev: Device instance
* @retval Status
*/
static int usbd_composite_scsi_process_read(usbd_composite_msc_dev_t *mdev)
{
	u32 len = mdev->blklen * COMP_MSC_BLK_SIZE;
	usb_msc_bot_cbw_t *cbw = mdev->cbw;

	if (cbw->field.dCBWDataTransferLength < len) {
		mdev->phase_error = 1;
		len = cbw->field.dCBWDataTransferLength;
		mdev->blklen = (len >> COMP_MSC_BLK_BITS);
	}

	len = MIN(len, COMP_MSC_BUFLEN);

	if (mdev->disk_ops.disk_read(mdev->lba, mdev->data, (len >> COMP_MSC_BLK_BITS))) {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_HARDWARE_ERROR, SCSI_ASC_UNRECOVERED_READ_ERROR);
		return HAL_ERR_PARA;
	}

	usbd_composite_msc_bulk_transmit(mdev->dev, mdev->data, len);

	mdev->lba += (len >> COMP_MSC_BLK_BITS);
	mdev->blklen -= (len >> COMP_MSC_BLK_BITS);

	/* case 6 : Hi = Di */
	mdev->csw->field.dCSWDataResidue -= len;

	if (mdev->blklen == 0U) {
		mdev->bot_state = COMP_MSC_LAST_DATA_IN;
	}
	return HAL_OK;
}

/**
* @brief  Handle Write Process
* @param  mdev: Device instance
* @retval Status
*/
static int usbd_composite_scsi_process_write(usbd_composite_msc_dev_t *mdev)
{
	u32 len = mdev->blklen * COMP_MSC_BLK_SIZE;

	len = MIN(len, COMP_MSC_BUFLEN);

	if (mdev->disk_ops.disk_write(mdev->lba, mdev->data, (len >> COMP_MSC_BLK_BITS))) {
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_HARDWARE_ERROR, SCSI_ASC_WRITE_FAULT);
		return HAL_ERR_PARA;
	}

	mdev->lba += (len >> COMP_MSC_BLK_BITS);
	mdev->blklen -= (len >> COMP_MSC_BLK_BITS);

	/* case 12 : Ho = Do */
	mdev->csw->field.dCSWDataResidue -= len;

	if (mdev->blklen == 0U) {
		usbd_composite_msc_send_csw(mdev->dev, BOT_CSW_CMD_PASSED);
	} else {
		len = MIN((mdev->blklen * COMP_MSC_BLK_SIZE), COMP_MSC_BUFLEN);
		/* Prepare EP to Receive next packet */
		usbd_composite_msc_bulk_receive(mdev->dev, mdev->data, len);
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
* @brief  Process SCSI commands
* @param  mdev: Device instance
* @param  cmd: Command parameters
* @retval Status
*/
int usbd_composite_scsi_process_cmd(usbd_composite_msc_dev_t *mdev, u8 *cmd)
{
	int ret = HAL_OK;

	switch (cmd[0]) {
	case SCSI_TEST_UNIT_READY:
		ret = usbd_composite_scsi_test_unit_ready(mdev, cmd);
		break;

	case SCSI_REQUEST_SENSE:
		ret = usbd_composite_scsi_request_sense(mdev, cmd);
		break;
	case SCSI_INQUIRY:
		ret = usbd_composite_scsi_inquiry(mdev, cmd);
		break;

	case SCSI_START_STOP_UNIT:
		ret = usbd_composite_scsi_start_stop_unit(mdev, cmd);
		break;

	case SCSI_ALLOW_MEDIUM_REMOVAL:
		ret = usbd_composite_scsi_prevent_allow(mdev, cmd);
		break;

	case SCSI_MODE_SENSE6:
		ret = usbd_composite_scsi_mode_sense6(mdev, cmd);
		break;

	case SCSI_MODE_SENSE10:
		ret = usbd_composite_scsi_mode_sense10(mdev, cmd);
		break;

	case SCSI_READ_FORMAT_CAPACITIES:
		ret = usbd_composite_scsi_read_format_capacity(mdev, cmd);
		break;

	case SCSI_READ_CAPACITY10:
		ret = usbd_composite_scsi_read_capacity10(mdev, cmd);
		break;

	case SCSI_READ12:
	case SCSI_READ10:
		ret = usbd_composite_scsi_read(mdev, cmd);
		break;

	case SCSI_WRITE12:
	case SCSI_WRITE10:
		ret = usbd_composite_scsi_write(mdev, cmd);
		break;

	case SCSI_MODE_SELECT6:
	case SCSI_MODE_SELECT10:
		mdev->data_length = 0U;
		break;

	case SCSI_VERIFY10:
		ret = usbd_composite_scsi_verify10(mdev, cmd);
		break;

	default:
		usbd_composite_scsi_sense_code(mdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
		ret = HAL_ERR_PARA;
	}
	return ret;
}

/**
* @brief  Load the last error code in the error list
* @param  mdev: Device instance
* @param  key: Sense Key
* @param  asc: Additional Sense Key
* @retval none

*/
void usbd_composite_scsi_sense_code(usbd_composite_msc_dev_t *mdev, u8 key, u8 asc)
{
	usb_msc_scsi_sense_data_t *data = &mdev->scsi_sense_data[mdev->scsi_sense_tail];
	data->key  = key;
	data->asc = asc;
	mdev->scsi_sense_tail++;
	if (mdev->scsi_sense_tail == COMP_MSC_SENSE_LIST_DEPTH) {
		mdev->scsi_sense_tail = 0U;
	}
}
