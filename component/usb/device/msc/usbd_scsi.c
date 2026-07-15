/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_scsi.h"

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

/* MSC page 0 inquiry data: pages 0x00 and 0x80 are supported */
static const u8 usbd_msc_page0_inquiry_data[] = {
	0x00,  /* Peripheral type */
	0x00,  /* Page Code 0x00 */
	0x00,  /* Reserved */
	0x02,  /* Page Length = 2 */
	0x00,  /* Supported page: 0x00 */
	0x80   /* Supported page: 0x80 */
};

/* MSC page 0x80 inquiry data: unit serial number */
static const u8 usbd_msc_page80_inquiry_data[] = {
	0x00,  /* Peripheral type */
	0x80,  /* Page Code 0x80 */
	0x00,  /* Reserved */
	0x04,  /* Page Length = 4 */
	'0', '0', '0', '1'  /* Serial number */
};

/* MSC standard inquiry data */
static const u8 usbd_msc_standard_inquiry_data[] = {
	0x00,          /* Direct Access Device */
	0x80,          /* RMB = 1: Removable Medium */
	0x02,          /* Version: No conformance claim to standard */
	0x02,
	36 - 5,          /* Additional Length = n-4 where n = last byte index = 35 */
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

	/* A non-zero dCBWDataTransferLength on this no-data command (BOT §6.7
	 * Case 4 / Case 9) is handled centrally in usbd_msc_rx_process, which
	 * reports the command result and STALLs Bulk-Out when the host declared
	 * an OUT transfer — the CSW status here stays the command result. */
	if (cdev->is_open != 1) {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
	}
	cdev->data_length = 0U;

	return HAL_OK;
}

/**
* @brief  Process Inquiry command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int  usbd_scsi_inquiry(usbd_msc_dev_t *cdev, u8 *params)
{
	u16 alloc_len;

	if (params[1] & 0x01U) { /*Evpd is set*/
		switch (params[2]) {
		case 0x00U:
			cdev->data_length = PAGE00_INQUIRY_DATA_LEN;
			usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_page0_inquiry_data, cdev->data_length);
			break;
		case 0x80U:
			cdev->data_length = PAGE80_INQUIRY_DATA_LEN;
			usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_page80_inquiry_data, cdev->data_length);
			break;
		default:
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
			return HAL_ERR_PARA;
		}
	} else {
		/* EVPD=0: the PAGE CODE field (byte 2) must be zero; a non-zero page
		 * code is invalid — CHECK CONDITION / INVALID FIELD IN CDB (SPC-4). */
		if (params[2] != 0U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
			return HAL_ERR_PARA;
		}
		/* INQUIRY allocation length is a 16-bit big-endian field (CDB bytes 3-4) */
		alloc_len = ((u16)params[3] << 8) | params[4];
		cdev->data_length = MIN(alloc_len, INQUIRY_DATA_LEN);
		usb_os_memcpy((void *)cdev->data, (void *)usbd_msc_standard_inquiry_data, cdev->data_length);
	}

	return HAL_OK;
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
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			cdev->data[0] = (u8)((cdev->num_sectors - 1U) >> 24);
			cdev->data[1] = (u8)((cdev->num_sectors - 1U) >> 16);
			cdev->data[2] = (u8)((cdev->num_sectors - 1U) >>  8);
			cdev->data[3] = (u8)(cdev->num_sectors - 1U);

			cdev->data[4] = (u8)(USBD_MSC_BLK_SIZE >>  24);
			cdev->data[5] = (u8)(USBD_MSC_BLK_SIZE >>  16);
			cdev->data[6] = (u8)(USBD_MSC_BLK_SIZE >>  8);
			cdev->data[7] = (u8)(USBD_MSC_BLK_SIZE);

			cdev->data_length = READ_CAPACITY10_DATA_LEN;
			return HAL_OK;
		}
	} else {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
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
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			cdev->data[3] = 0x08U;
			cdev->data[4] = (u8)(cdev->num_sectors >> 24);
			cdev->data[5] = (u8)(cdev->num_sectors >> 16);
			cdev->data[6] = (u8)(cdev->num_sectors >>  8);
			cdev->data[7] = (u8)(cdev->num_sectors);

			cdev->data[8] = 0x02U;
			cdev->data[9] = (u8)(USBD_MSC_BLK_SIZE >>  16);
			cdev->data[10] = (u8)(USBD_MSC_BLK_SIZE >>  8);
			cdev->data[11] = (u8)(USBD_MSC_BLK_SIZE);

			cdev->data_length = READ_FORMAT_CAPACITY_DATA_LEN;
			return HAL_OK;
		}
	} else {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
		return HAL_ERR_PARA;
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
	if (cdev->ro) {
		cdev->data[2] |= 0x80U;  /* SPC-4: byte 2 bit 7 = WP */
	}

	return HAL_OK;
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
	if (cdev->ro) {
		cdev->data[3] |= 0x80U;  /* SPC-4: byte 3 bit 7 = WP */
	}

	return HAL_OK;
}

/**
* @brief  Process Request Sense command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_request_sense(usbd_msc_dev_t *cdev, u8 *params)
{
	usb_msc_scsi_sense_data_t *data;

	UNUSED(params);

	usb_os_memset((void *)cdev->data, 0U, REQUEST_SENSE_DATA_LEN);

	cdev->data[0]	= 0x70U;
	/* Additional sense length = total length - 8 (bytes 0-7 are the header) */
	cdev->data[7]	= REQUEST_SENSE_DATA_LEN - 8U;

	if ((cdev->scsi_sense_head != cdev->scsi_sense_tail)) {
		data = &cdev->scsi_sense_data[cdev->scsi_sense_head];
		cdev->data[2] = data->key;
		cdev->data[12] = data->asc;
		cdev->data[13] = data->ascq;
		cdev->scsi_sense_head++;

		if (cdev->scsi_sense_head == USBD_MSC_SENSE_LIST_DEPTH) {
			cdev->scsi_sense_head = 0U;
		}
	}
	cdev->data_length = MIN(params[4], REQUEST_SENSE_DATA_LEN);

	return HAL_OK;
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
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		return HAL_OK;
	}

	if (!loej) {
		return HAL_OK;
	}
	cdev->is_open = 0;

	return HAL_OK;
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

	return HAL_OK;
}

/**
* @brief  Process Read10/12 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_read(usbd_msc_dev_t *cdev, u8 *params)
{
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	if (cdev->bot_state == USBD_MSC_IDLE) { /* Idle */
		/* case 2 */
		if ((cbw->field.dCBWDataTransferLength) == 0) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* case 10 : Ho <> Di */
		if ((cbw->field.bmCBWFlags & 0x80U) != 0x80U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		if (cdev->is_open != 1) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		cdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_READ10) {
			cdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_READ12) {
			cdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		if (usbd_scsi_check_address_range(cdev, cdev->lba, cdev->blklen) != HAL_OK) {
			return HAL_ERR_PARA; /* error */
		}

		/* case 4: Hi > D0 — CDB requests no data, stall IN with PHASE_ERROR */
		if (cdev->blklen == 0) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1;
			return HAL_ERR_PARA;
		}
		/* case 5: Hi > Di — host requests more data than SCSI delivers; send Di bytes, residue = Hi-Di */

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
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	if (cdev->bot_state == USBD_MSC_IDLE) { /* Idle */
		/* case 3 : Hn < Do */
		if (cbw->field.dCBWDataTransferLength == 0) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* case 8 : Hi <> Do */
		if ((cbw->field.bmCBWFlags & 0x80U) == 0x80U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1;
			return HAL_ERR_PARA;
		}

		/* Check whether Media is ready */
		if (cdev->is_open != 1) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		/* Check If media is write-protected */
		if (cdev->ro != 0) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_DATA_PROTECT, SCSI_ASC_WRITE_PROTECTED);
			return HAL_ERR_PARA;
		}

		cdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_WRITE10) {
			cdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_WRITE12) {
			cdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		/* check if LBA address is in the right range */
		if (usbd_scsi_check_address_range(cdev, cdev->lba, cdev->blklen) != HAL_OK) {
			return HAL_ERR_PARA; /* error */
		}

		len = cdev->blklen * USBD_MSC_BLK_SIZE;

		/* case 11,13 : Ho <> Do */
		if (cbw->field.dCBWDataTransferLength != len) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			return HAL_ERR_PARA;
		}

		len = MIN(len, USBD_MSC_BUFLEN);

		/* Prepare EP to receive first data packet */
		cdev->bot_state = USBD_MSC_DATA_OUT;
		usbd_msc_bulk_receive(cdev->dev, cdev->data, len);
	} else { /* Write Process ongoing */
		return usbd_scsi_process_write(cdev);
	}
	return HAL_OK;
}

/**
* @brief  Process Verify10 command
* @param  cdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_scsi_verify10(usbd_msc_dev_t *cdev, u8 *params)
{
	u32 verify_lba;
	u32 verify_blklen;

	if ((params[1] & 0x02U) == 0x02U) {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
		return HAL_ERR_PARA; /* Error, Verify Mode Not supported*/
	}

	verify_lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];
	verify_blklen = ((u32)params[7] << 8) | (u32)params[8];

	if (verify_blklen == 0) {
		cdev->data_length = 0U;
		return HAL_OK; /* No-op: verification length of 0 means verify nothing */
	}

	if (usbd_scsi_check_address_range(cdev, verify_lba, verify_blklen) != HAL_OK) {
		return HAL_ERR_PARA; /* error */
	}
	cdev->data_length = 0U;
	return HAL_OK;
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
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_ADDRESS_OUT_OF_RANGE);
		return HAL_ERR_PARA;
	}
	return HAL_OK;
}

/**
* @brief  Handle Read Process
* @param  cdev: Device instance
* @retval Status
*/
static int usbd_scsi_process_read(usbd_msc_dev_t *cdev)
{
	u32 len = cdev->blklen * USBD_MSC_BLK_SIZE;
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	if (cbw->field.dCBWDataTransferLength < len) {
		cdev->phase_error = 1;
		len = cbw->field.dCBWDataTransferLength;
		cdev->blklen = (len >> USBD_MSC_BLK_BITS);
	}

	len = MIN(len, USBD_MSC_BUFLEN);

	if (cdev->disk_ops.disk_read(cdev->lba, cdev->data, (len >> USBD_MSC_BLK_BITS))) {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_HARDWARE_ERROR, SCSI_ASC_UNRECOVERED_READ_ERROR);
		return HAL_ERR_PARA;
	}

	usbd_msc_bulk_transmit(cdev->dev, cdev->data, len);

	cdev->lba += (len >> USBD_MSC_BLK_BITS);
	cdev->blklen -= (len >> USBD_MSC_BLK_BITS);

	/* case 6 : Hi = Di */
	cdev->csw->field.dCSWDataResidue -= len;

	if (cdev->blklen == 0U) {
		cdev->bot_state = USBD_MSC_LAST_DATA_IN;
	}
	return HAL_OK;
}

/**
* @brief  Handle Write Process
* @param  cdev: Device instance
* @retval Status
*/
static int usbd_scsi_process_write(usbd_msc_dev_t *cdev)
{
	u32 len = cdev->blklen * USBD_MSC_BLK_SIZE;

	len = MIN(len, USBD_MSC_BUFLEN);

	if (cdev->disk_ops.disk_write(cdev->lba, cdev->data, (len >> USBD_MSC_BLK_BITS))) {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_HARDWARE_ERROR, SCSI_ASC_WRITE_FAULT);
		return HAL_ERR_PARA;
	}

	cdev->lba += (len >> USBD_MSC_BLK_BITS);
	cdev->blklen -= (len >> USBD_MSC_BLK_BITS);

	/* case 12 : Ho = Do */
	cdev->csw->field.dCSWDataResidue -= len;

	if (cdev->blklen == 0U) {
		usbd_msc_send_csw(cdev->dev, BOT_CSW_CMD_PASSED);
	} else {
		len = MIN((cdev->blklen * USBD_MSC_BLK_SIZE), USBD_MSC_BUFLEN);
		/* Prepare EP to Receive next packet */
		usbd_msc_bulk_receive(cdev->dev, cdev->data, len);
	}

	return HAL_OK;
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
	int ret = HAL_OK;

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
		ret = HAL_OK;
		break;

	case SCSI_VERIFY10:
		ret = usbd_scsi_verify10(cdev, cmd);
		break;

	default:
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
		return HAL_ERR_PARA;
	}
	return ret;
}

/**
* @brief  Load the last error code in the error list
* @param  cdev: Device instance
* @param  key: Sense Key
* @param  asc: Additional Sense Key
* @retval none

*/
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 key, u8 asc)
{
	usb_msc_scsi_sense_data_t *data = &cdev->scsi_sense_data[cdev->scsi_sense_tail];
	data->key  = key;
	data->asc = asc;
	cdev->scsi_sense_tail++;
	if (cdev->scsi_sense_tail == USBD_MSC_SENSE_LIST_DEPTH) {
		cdev->scsi_sense_tail = 0U;
	}
}
