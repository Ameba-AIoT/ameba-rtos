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
static int usbd_scsi_check_cdb_length(usbd_msc_dev_t  *cdev, u8 opcode);
static int usbd_scsi_process_read(usbd_msc_dev_t  *cdev);
static int usbd_scsi_process_write(usbd_msc_dev_t  *cdev);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

/* Minimum meaningful CDB length of each supported opcode (SPC-4/SBC-4 CDB
 * formats). BOT §5.1 says only the first bCBWCBLength bytes of CBWCB are
 * meaningful, so a command whose CDB is declared shorter than its format
 * requires must be rejected before any handler parses those bytes.
 * A longer declared CDB is accepted: the extra bytes are simply not parsed. */
static const u8 usbd_msc_cdb_len_tbl[][2] = {
	{SCSI_TEST_UNIT_READY,        6U},
	{SCSI_REQUEST_SENSE,          6U},
	{SCSI_INQUIRY,                6U},
	{SCSI_START_STOP_UNIT,        6U},
	{SCSI_ALLOW_MEDIUM_REMOVAL,   6U},
	{SCSI_MODE_SENSE6,            6U},
	{SCSI_MODE_SELECT6,           6U},
	{SCSI_MODE_SENSE10,          10U},
	{SCSI_MODE_SELECT10,         10U},
	{SCSI_READ_FORMAT_CAPACITIES, 10U},
	{SCSI_READ_CAPACITY10,       10U},
	{SCSI_READ10,                10U},
	{SCSI_WRITE10,               10U},
	{SCSI_VERIFY10,              10U},
	{SCSI_READ12,                12U},
	{SCSI_WRITE12,               12U},
};

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

/* Exported variables --------------------------------------------------------*/

/* MSC standard inquiry data */
const u8 usbd_msc_default_inquiry_data[INQUIRY_DATA_LEN] = {
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
	if (cdev->is_open != 1U) {
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
	const u8 *page;
	u32 page_len;
	/* INQUIRY allocation length is a 16-bit big-endian field (CDB bytes 3-4). Ref SPC-4 INQUIRY: it bounds the
	 * returned data of the standard INQUIRY and of every VPD page alike. */
	u32 alloc_len = ((u32)params[3] << 8) | (u32)params[4];

	if ((params[1] & 0x01U) != 0U) { /* EVPD is set */
		switch (params[2]) {
		case 0x00U:
			page = usbd_msc_page0_inquiry_data;
			page_len = PAGE00_INQUIRY_DATA_LEN;
			break;
		case 0x80U:
			page = usbd_msc_page80_inquiry_data;
			page_len = PAGE80_INQUIRY_DATA_LEN;
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
		/* Built at init from the configured identification strings, so it is per-instance
		   rather than the const default template. */
		page = cdev->inquiry_data;
		page_len = INQUIRY_DATA_LEN;
	}

	cdev->data_length = MIN(alloc_len, page_len);
	usb_os_memcpy((void *)cdev->data, (const void *)page, cdev->data_length);

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

	if (cdev->disk_ops.disk_getcapacity != NULL) {
		if (cdev->disk_ops.disk_getcapacity(&cdev->num_sectors) != 0) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			/* num_sectors - 1 wraps to a huge LBA if the medium reports 0
			 * blocks; report the absent medium instead (SBC-4 last-LBA is
			 * block_count - 1, and 0 blocks is not a valid capacity). */
			if (cdev->num_sectors == 0U) {
				usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
				return HAL_ERR_PARA;
			}

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

	if (cdev->disk_ops.disk_getcapacity != NULL) {
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
	usb_os_memcpy((void *)cdev->data, (const void *)usbd_msc_mode_sense6_data, cdev->data_length);
	if (cdev->ro != 0U) {
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
	usb_os_memcpy((void *)cdev->data, (const void *)usbd_msc_mode_sense10_data, cdev->data_length);
	if (cdev->ro != 0U) {
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
	usb_msc_scsi_sense_data_t *sense = &cdev->scsi_sense;

	usb_os_memset((void *)cdev->data, 0U, REQUEST_SENSE_DATA_LEN);

	cdev->data[0]	= 0x70U;
	/* Additional sense length = total length - 8 (bytes 0-7 are the header) */
	cdev->data[7]	= REQUEST_SENSE_DATA_LEN - 8U;

	cdev->data[2] = sense->key;
	cdev->data[12] = sense->asc;
	cdev->data[13] = sense->ascq;
	/* Ref SPC-4 REQUEST SENSE: the reported sense data is cleared once returned */
	usb_os_memset((void *)sense, 0U, sizeof(*sense));

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
	u8      loej, start;

	cdev->data_length = 0U;

	/* SPC-4 START STOP: bit 1 = LOEJ, bit 0 = START. Keep the extracted
	 * flags essentially unsigned so their bit values are not assigned to a
	 * signed int (MISRA Rule 10.3/10.4). */
	loej  = (u8)(params[4] & 0x02U);
	start = (u8)(params[4] & 0x01U);

	/* Ref SBC-4 START STOP UNIT: START=1 with LOEJ=1 loads the medium, START=0 with LOEJ=1 ejects it, and
	 * LOEJ=0 leaves the medium where it is. The medium is fixed in the device, so loading it back only needs
	 * the backing storage to answer: its capacity is read again, as it may have changed meanwhile. */
	if (start != 0U) {
		if (loej != 0U) {
			cdev->num_sectors = 0U;
			if ((cdev->disk_ops.disk_getcapacity == NULL) || (cdev->disk_ops.disk_getcapacity(&cdev->num_sectors) != 0)) {
				cdev->num_sectors = 0U;
				usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
				return HAL_ERR_PARA;
			}
			cdev->is_open = 1U;
		} else if (cdev->is_open != 1U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		} else {
			/* medium already loaded */
		}

		return HAL_OK;
	}

	if (loej == 0U) {
		return HAL_OK;
	}

	/* Ref SPC-4 PREVENT ALLOW MEDIUM REMOVAL: an eject is refused while the host keeps the medium locked */
	if (cdev->prevent_removal != 0U) {
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_MEDIUM_REMOVAL_PREVENTED);
		cdev->scsi_sense.ascq = SCSI_ASCQ_MEDIUM_REMOVAL_PREVENTED;
		return HAL_ERR_PARA;
	}
	cdev->is_open = 0U;

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
	cdev->data_length = 0U;

	/* Ref SPC-4 PREVENT ALLOW MEDIUM REMOVAL: byte 4 bits 1:0 are PREVENT, 01b locks the medium and 00b
	 * unlocks it; 10b and 11b are obsolete, so they are rejected */
	switch (params[4] & 0x03U) {
	case 0x00U:
		cdev->prevent_removal = 0U;
		break;
	case 0x01U:
		cdev->prevent_removal = 1U;
		break;
	default:
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
		return HAL_ERR_PARA;
	}

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
		if (cbw->field.dCBWDataTransferLength == 0U) {
			/* Ref SBC-4: a TRANSFER LENGTH of 0 reads nothing and is not an error. With no data stage either,
			 * this is BOT 6.7.2 Case 1 (Hn = Dn): pass with no data. A CDB asking for blocks is Case 2
			 * (Hn < Di): phase error. */
			if (params[0] == SCSI_READ10) {
				cdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
			} else {
				cdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
			}
			if (cdev->blklen == 0U) {
				cdev->data_length = 0U;
				/* No block is read, but the unit shall still be ready, as for any other medium access */
				if (cdev->is_open != 1U) {
					usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
					return HAL_ERR_PARA;
				}
				return HAL_OK;
			}
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}

		/* case 10 : Ho <> Di */
		if ((cbw->field.bmCBWFlags & 0x80U) != 0x80U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}

		/* A non-whole-block dCBWDataTransferLength cannot be satisfied: the
		 * net is larger than the whole blocks we would actually read, and
		 * process_read would transmit uninitialized bytes beyond them. A
		 * fractional length is therefore a phase error (BOT 6.7.3). */
		if ((cbw->field.dCBWDataTransferLength & (USBD_MSC_BLK_SIZE - 1U)) != 0U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}

		if (cdev->is_open != 1U) {
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
		if (cdev->blklen == 0U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}
		/* case 5: Hi > Di — host requests more data than SCSI delivers; send Di bytes, residue = Hi-Di */

		cdev->bot_state = USBD_MSC_DATA_IN;
	}
	cdev->data_length = cdev->buf_size;

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
	u32 xfer_blks;
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	if (cdev->bot_state == USBD_MSC_IDLE) { /* Idle */
		/* case 3 : Hn < Do */
		if (cbw->field.dCBWDataTransferLength == 0U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}

		/* case 8 : Hi <> Do */
		if ((cbw->field.bmCBWFlags & 0x80U) == 0x80U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			cdev->phase_error = 1U;
			return HAL_ERR_PARA;
		}

		/* Check whether Media is ready */
		if (cdev->is_open != 1U) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}

		/* Check If media is write-protected */
		if (cdev->ro != 0U) {
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

		/* case 11,13 : Ho <> Do. Compared in blocks: blklen * USBD_MSC_BLK_SIZE wraps in u32 for a WRITE(12) of
		 * 8M blocks or more, which a medium above 4GB lets through the range check. Once equal, the byte count
		 * equals dCBWDataTransferLength, so every later blklen * USBD_MSC_BLK_SIZE fits in u32. */
		xfer_blks = cbw->field.dCBWDataTransferLength >> USBD_MSC_BLK_BITS;
		if ((xfer_blks != cdev->blklen) || ((cbw->field.dCBWDataTransferLength & (USBD_MSC_BLK_SIZE - 1U)) != 0U)) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			if (xfer_blks < cdev->blklen) {
				/* case 13 : Ho < Do — BOT §6.7.3 mandates bCSWStatus = Phase Error.
				 * case 11 (Ho > Do) stays a Failed CSW with residue, which BOT allows. */
				cdev->phase_error = 1U;
			}
			return HAL_ERR_PARA;
		}

		len = MIN(cbw->field.dCBWDataTransferLength, cdev->buf_size);

		/* Prepare EP to receive first data packet */
		cdev->bot_state = USBD_MSC_DATA_OUT;
		/* Fails only once the device is no longer ready (suspend/disconnect), the host then recovers by reset */
		if (usbd_msc_bulk_receive(cdev->dev, cdev->data, len, cdev->buf_size) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_out_addr);
		}
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

	if (verify_blklen == 0U) {
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
* @brief  Check that the CBW declares a CDB long enough for the opcode
* @param  cdev: Device instance
* @param  opcode: SCSI operation code
* @retval Status
*/
static int usbd_scsi_check_cdb_length(usbd_msc_dev_t *cdev, u8 opcode)
{
	u32 i;

	for (i = 0U; i < (sizeof(usbd_msc_cdb_len_tbl) / sizeof(usbd_msc_cdb_len_tbl[0])); i++) {
		if (usbd_msc_cdb_len_tbl[i][0] == opcode) {
			if (cdev->cbw->field.bCBWCBLength < usbd_msc_cdb_len_tbl[i][1]) {
				usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
				return HAL_ERR_PARA;
			}
			break;
		}
	}

	/* Unlisted opcodes are rejected by the dispatcher's default branch */
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
	/* num_sectors is otherwise only refreshed by READ CAPACITY(10)/READ FORMAT CAPACITIES,
	 * which SBC-4 does not require an initiator to issue before its first READ/WRITE. Query
	 * the medium once so the LBA validity check below uses the real capacity whatever the
	 * command order, instead of rejecting every transfer against a zero capacity. */
	if (cdev->num_sectors == 0U) {
		if ((cdev->disk_ops.disk_getcapacity == NULL) ||
			(cdev->disk_ops.disk_getcapacity(&cdev->num_sectors) != 0)) {
			cdev->num_sectors = 0U;
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_NOT_READY, SCSI_ASC_MEDIUM_NOT_PRESENT);
			return HAL_ERR_PARA;
		}
	}

	/* Overflow-safe: blk_offset + blk_nbr can wrap on host-controlled
	 * READ(12)/WRITE(12) values and pass a naive sum comparison */
	if ((blk_offset > cdev->num_sectors) || (blk_nbr > (cdev->num_sectors - blk_offset))) {
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
	u32 len;
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	/* Compared in blocks, dCBWDataTransferLength is a whole number of blocks (see usbd_scsi_read): blklen *
	 * USBD_MSC_BLK_SIZE wraps in u32 for a READ(12) of 8M blocks or more. After the clamp the product never
	 * exceeds dCBWDataTransferLength, so it fits in u32. */
	if ((cbw->field.dCBWDataTransferLength >> USBD_MSC_BLK_BITS) < cdev->blklen) {
		cdev->phase_error = 1U;
		cdev->blklen = cbw->field.dCBWDataTransferLength >> USBD_MSC_BLK_BITS;
	}

	len = MIN(cdev->blklen * USBD_MSC_BLK_SIZE, cdev->buf_size);

	if (cdev->disk_ops.disk_read(cdev->lba, cdev->data, (len >> USBD_MSC_BLK_BITS)) != 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Rd err LBA %u cnt %u\n", cdev->lba, len >> USBD_MSC_BLK_BITS);
		usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_HARDWARE_ERROR, SCSI_ASC_UNRECOVERED_READ_ERROR);
		return HAL_ERR_PARA;
	}

	/* Fails only once the device is no longer ready (suspend/disconnect), the host then recovers by reset */
	if (usbd_msc_bulk_transmit(cdev->dev, cdev->data, len) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_in_addr);
	}

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

	len = MIN(len, cdev->buf_size);

	if (cdev->disk_ops.disk_write(cdev->lba, cdev->data, (len >> USBD_MSC_BLK_BITS)) != 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Wr err LBA %u cnt %u\n", cdev->lba, len >> USBD_MSC_BLK_BITS);
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
		len = MIN((cdev->blklen * USBD_MSC_BLK_SIZE), cdev->buf_size);
		/* Prepare EP to Receive next packet */
		if (usbd_msc_bulk_receive(cdev->dev, cdev->data, len, cdev->buf_size) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_out_addr);
		}
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
* @brief  Check whether the host still has OUT data to send after the chunk being written
* @note   Valid while a WRITE is in its data stage: blklen counts the blocks not yet written, the chunk just
*         received included, and each chunk holds at most buf_size bytes, see usbd_scsi_process_write().
* @param  cdev: Device instance
* @retval 1 if more OUT data follows the current chunk, 0 otherwise
*/
u8 usbd_scsi_out_data_pending(const usbd_msc_dev_t *cdev)
{
	/* Compared in blocks, the byte count of a WRITE(12) would not fit in u32 */
	return (cdev->blklen > (cdev->buf_size >> USBD_MSC_BLK_BITS)) ? 1U : 0U;
}

/**
* @brief  Process SCSI commands
* @param  cdev: Device instance
* @param  cmd: Command parameters
* @retval Status
*/
int usbd_scsi_process_cmd(usbd_msc_dev_t *cdev, u8 *cmd)
{
	int ret = HAL_OK;

	/* Ref SPC-4 4.5: sense data belongs to the command that just failed. Drop it when any other command starts,
	   so a later REQUEST SENSE never reports a stale error, e.g. MEDIUM NOT PRESENT after the medium is back. */
	if (cmd[0] != SCSI_REQUEST_SENSE) {
		usb_os_memset((void *)&cdev->scsi_sense, 0U, sizeof(cdev->scsi_sense));
	}

	if (usbd_scsi_check_cdb_length(cdev, cmd[0]) != HAL_OK) {
		return HAL_ERR_PARA;
	}

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
* @brief  Record the sense data of the failing command
* @note   Ref SPC-4 4.5: sense data describes the most recent CHECK CONDITION only, so a single slot is kept and
*         a newer error replaces an older one instead of queueing behind it.
* @param  cdev: Device instance
* @param  key: Sense Key
* @param  asc: Additional Sense Key
* @retval none
*/
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 key, u8 asc)
{
	usb_msc_scsi_sense_data_t *sense = &cdev->scsi_sense;

	sense->key = key;
	sense->asc = asc;
	sense->ascq = 0U;
}
