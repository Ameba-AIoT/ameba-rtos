/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_scsi.h"
#include "usbd_composite_msc.h"

/* Private defines -----------------------------------------------------------*/

/* SCSI Commands */
#define SCSI_FORMAT_UNIT                            0x04U
#define SCSI_INQUIRY                                0x12U
#define SCSI_MODE_SELECT6                           0x15U
#define SCSI_MODE_SELECT10                          0x55U
#define SCSI_MODE_SENSE6                            0x1AU
#define SCSI_MODE_SENSE10                           0x5AU
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1EU
#define SCSI_READ6                                  0x08U
#define SCSI_READ10                                 0x28U
#define SCSI_READ12                                 0xA8U
#define SCSI_READ16                                 0x88U

#define SCSI_READ_CAPACITY10                        0x25U
#define SCSI_READ_CAPACITY16                        0x9EU

#define SCSI_REQUEST_SENSE                          0x03U
#define SCSI_START_STOP_UNIT                        0x1BU
#define SCSI_TEST_UNIT_READY                        0x00U
#define SCSI_WRITE6                                 0x0AU
#define SCSI_WRITE10                                0x2AU
#define SCSI_WRITE12                                0xAAU
#define SCSI_WRITE16                                0x8AU

#define SCSI_VERIFY10                               0x2FU
#define SCSI_VERIFY12                               0xAFU
#define SCSI_VERIFY16                               0x8FU

#define SCSI_SEND_DIAGNOSTIC                        0x1DU
#define SCSI_READ_FORMAT_CAPACITIES                 0x23U

#define READ_FORMAT_CAPACITY_DATA_LEN               0x0CU
#define READ_CAPACITY10_DATA_LEN                    0x08U
#define MODE_SENSE10_DATA_LEN                       0x08U
#define MODE_SENSE6_DATA_LEN                        0x04U
#define REQUEST_SENSE_DATA_LEN                      0x12U
#define STANDARD_INQUIRY_DATA_LEN                   0x24U
#define PAGE00_INQUIRY_DATA_LEN		                  7U

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

/* USB Mass storage Page 0 Inquiry Data */
static const u8  Page00_Inquiry_Data[] = {
	0x00,
	0x00,
	0x00,
	(PAGE00_INQUIRY_DATA_LEN - 4U),
	0x00,
	0x80,
	0x83
};

static const u8 Standard_Inquiry_Data[] = {
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

/* USB Mass storage sense 6  Data */
static const u8  Mode_Sense6_data[] = {
	0x03,
	0x00,
	0x00,
	0x00
};

/* USB Mass storage sense 10  Data */
static const u8  Mode_Sense10_data[] = {
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

	/* case 9 : Hi > D0 */
	if (mdev->cbw->dCBWDataTransferLength != 0U) {
		usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);

		return -1;
	}

	if (mdev->is_open != 1) {
		usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
		mdev->bot_state = COMP_MSC_NO_DATA;

		return -1;
	}
	mdev->data_length = 0U;

	return 0;
}

/**
* @brief  Process Inquiry command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int  usbd_composite_scsi_inquiry(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	if (params[1] & 0x01U) { /*Evpd is set*/
		mdev->data_length = PAGE00_INQUIRY_DATA_LEN;
		usb_os_memcpy((void *)mdev->data, (void *)Page00_Inquiry_Data, mdev->data_length);
	} else {
		mdev->data_length = MIN(params[4], STANDARD_INQUIRY_DATA_LEN);
		usb_os_memcpy((void *)mdev->data, (void *)Standard_Inquiry_Data, mdev->data_length);
	}

	return 0;
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
			usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		} else {
			mdev->data[0] = (u8)((mdev->num_sectors - 1U) >> 24);
			mdev->data[1] = (u8)((mdev->num_sectors - 1U) >> 16);
			mdev->data[2] = (u8)((mdev->num_sectors - 1U) >>  8);
			mdev->data[3] = (u8)(mdev->num_sectors - 1U);

			mdev->data[4] = (u8)(mdev->blksize >>  24);
			mdev->data[5] = (u8)(mdev->blksize >>  16);
			mdev->data[6] = (u8)(mdev->blksize >>  8);
			mdev->data[7] = (u8)(mdev->blksize);

			mdev->data_length = READ_CAPACITY10_DATA_LEN;
			return 0;
		}
	} else {
		usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
		return -1;
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
			usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		} else {
			mdev->data[3] = 0x08U;
			mdev->data[4] = (u8)((mdev->num_sectors - 1U) >> 24);
			mdev->data[5] = (u8)((mdev->num_sectors - 1U) >> 16);
			mdev->data[6] = (u8)((mdev->num_sectors - 1U) >>  8);
			mdev->data[7] = (u8)(mdev->num_sectors - 1U);

			mdev->data[8] = 0x02U;
			mdev->data[9] = (u8)(mdev->blksize >>  16);
			mdev->data[10] = (u8)(mdev->blksize >>  8);
			mdev->data[11] = (u8)(mdev->blksize);

			mdev->data_length = READ_FORMAT_CAPACITY_DATA_LEN;
			return 0;
		}
	} else {
		usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
		return -1;
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
	usb_os_memcpy((void *)mdev->data, (void *)Mode_Sense6_data, mdev->data_length);

	return 0;
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
	usb_os_memcpy((void *)mdev->data, (void *)Mode_Sense10_data, mdev->data_length);

	return 0;
}

/**
* @brief  Process Request Sense command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_request_sense(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	usbd_composite_msc_scsi_sense_data_t *data;

	UNUSED(params);

	usb_os_memset((void *)mdev->data, 0U, REQUEST_SENSE_DATA_LEN);

	mdev->data[0]	= 0x70U;
	mdev->data[7]	= REQUEST_SENSE_DATA_LEN - 6U;

	if ((mdev->scsi_sense_head != mdev->scsi_sense_tail)) {
		data = &mdev->scsi_sense_data[mdev->scsi_sense_head];
		mdev->data[2]     = data->skey;
		mdev->data[12]    = data->w.b.ascq;
		mdev->data[13]    = data->w.b.asc;
		mdev->scsi_sense_head++;

		if (mdev->scsi_sense_head == COMP_MSC_SENSE_LIST_DEPTH) {
			mdev->scsi_sense_head = 0U;
		}
	}
	mdev->data_length = MIN(params[4], REQUEST_SENSE_DATA_LEN);

	return 0;
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
			usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		return 0;
	}

	if (!loej) {
		return 0;
	}
	mdev->is_open = 0;

	return 0;
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

	return 0;
}

/**
* @brief  Process Read10/12 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_read(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;

	if (mdev->bot_state == COMP_MSC_IDLE) { /* Idle */
		/* case 2 */
		if ((cbw->dCBWDataTransferLength) == 0) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			mdev->phase_error = 1;
			return -1;
		}

		/* case 10 : Ho <> Di */
		if ((cbw->bmCBWFlags & 0x80U) != 0x80U) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		if (mdev->is_open != 1) {
			usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		mdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_READ10) {
			mdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_READ12) {
			mdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		if (usbd_composite_scsi_check_address_range(mdev, mdev->lba, mdev->blklen) < 0) {
			return -1; /* error */
		}

		/* case 4 : Hi > Dn */
		if (mdev->blklen == 0 || (cbw->dCBWDataTransferLength > (mdev->blklen * mdev->blksize))) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
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
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;

	if (mdev->bot_state == COMP_MSC_IDLE) { /* Idle */
		/* case 3 : Hn < Do */
		if (cbw->dCBWDataTransferLength == 0) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			mdev->phase_error = 1;
			return -1;
		}

		/* case 8 : Hi <> Do */
		if ((cbw->bmCBWFlags & 0x80U) == 0x80U) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		/* Check whether Media is ready */
		if (mdev->is_open != 1) {
			usbd_composite_scsi_sense_code(mdev, NOT_READY, MEDIUM_NOT_PRESENT);
			return -1;
		}

		/* Check If media is write-protected */
		if (mdev->ro != 0) {
			usbd_composite_scsi_sense_code(mdev, NOT_READY, WRITE_PROTECTED);
			return -1;
		}

		mdev->lba = ((u32)params[2] << 24) | ((u32)params[3] << 16) | ((u32)params[4] << 8) | (u32)params[5];

		if (params[0] == SCSI_WRITE10) {
			mdev->blklen = ((u32)params[7] << 8) | (u32)params[8];
		} else if (params[0] == SCSI_WRITE12) {
			mdev->blklen = ((u32)params[6] << 24) | ((u32)params[7] << 16) | ((u32)params[8] << 8) | (u32)params[9];
		}

		/* check if LBA address is in the right range */
		if (usbd_composite_scsi_check_address_range(mdev, mdev->lba, mdev->blklen) < 0) {
			return -1; /* error */
		}

		len = mdev->blklen * mdev->blksize;

		/* case 11,13 : Ho <> Do */
		if (cbw->dCBWDataTransferLength != len) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			return -1;
		}

		len = MIN(len, COMP_MSC_BUFLEN);

		/* Prepare EP to receive first data packet */
		mdev->bot_state = COMP_MSC_DATA_OUT;
		usbd_composite_msc_bulk_receive(mdev->dev, mdev->data, len);
	} else { /* Write Process ongoing */
		return usbd_composite_scsi_process_write(mdev);
	}
	return 0;
}

/**
* @brief  Process Verify10 command
* @param  mdev: Device instance
* @param  params: Command parameters
* @retval Status
*/
static int usbd_composite_scsi_verify10(usbd_composite_msc_dev_t *mdev, u8 *params)
{
	if ((params[1] & 0x02U) == 0x02U) {
		usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		return -1; /* Error, Verify Mode Not supported*/
	}

	if (usbd_composite_scsi_check_address_range(mdev, mdev->lba, mdev->blklen) < 0) {
		return -1; /* error */
	}
	mdev->data_length = 0U;
	return 0;
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
		usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
		return -1;
	}
	return 0;
}

/**
* @brief  Handle Read Process
* @param  mdev: Device instance
* @retval Status
*/
static int usbd_composite_scsi_process_read(usbd_composite_msc_dev_t *mdev)
{
	u32 len = mdev->blklen * mdev->blksize;
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;

	if (cbw->dCBWDataTransferLength < len) {
		mdev->phase_error = 1;
		len = cbw->dCBWDataTransferLength;
		mdev->blklen = (len >> mdev->blkbits);
	}

	len = MIN(len, COMP_MSC_BUFLEN);

	if (mdev->disk_ops.disk_read(mdev->lba, mdev->data, (len >> mdev->blkbits))) {
		usbd_composite_scsi_sense_code(mdev, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
		return -1;
	}

	usbd_composite_msc_bulk_transmit(mdev->dev, mdev->data, len);

	mdev->lba += (len >> mdev->blkbits);
	mdev->blklen -= (len >> mdev->blkbits);

	/* case 6 : Hi = Di */
	mdev->csw->dCSWDataResidue -= len;

	if (mdev->blklen == 0U) {
		mdev->bot_state = COMP_MSC_LAST_DATA_IN;
	}
	return 0;
}

/**
* @brief  Handle Write Process
* @param  mdev: Device instance
* @retval Status
*/
static int usbd_composite_scsi_process_write(usbd_composite_msc_dev_t *mdev)
{
	u32 len = mdev->blklen * mdev->blksize;

	len = MIN(len, COMP_MSC_BUFLEN);

	if (mdev->disk_ops.disk_write(mdev->lba, mdev->data, (len >> mdev->blkbits))) {
		usbd_composite_scsi_sense_code(mdev, HARDWARE_ERROR, WRITE_FAULT);
		return -1;
	}

	mdev->lba += (len >> mdev->blkbits);
	mdev->blklen -= (len >> mdev->blkbits);

	/* case 12 : Ho = Do */
	mdev->csw->dCSWDataResidue -= len;

	if (mdev->blklen == 0U) {
		usbd_composite_msc_send_csw(mdev->dev, COMP_MSC_CSW_CMD_PASSED);
	} else {
		len = MIN((mdev->blklen * mdev->blksize), COMP_MSC_BUFLEN);
		/* Prepare EP to Receive next packet */
		usbd_composite_msc_bulk_receive(mdev->dev, mdev->data, len);
	}

	return 0;
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
	int ret = 0;

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
		ret = 0;
		break;

	case SCSI_VERIFY10:
		ret = usbd_composite_scsi_verify10(mdev, cmd);
		break;

	default:
		usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
		return -1;
	}
	return ret;
}

/**
* @brief  Load the last error code in the error list
* @param  mdev: Device instance
* @param  skey: Sense Key
* @param  asc: Additional Sense Key
* @retval none

*/
void usbd_composite_scsi_sense_code(usbd_composite_msc_dev_t *mdev, u8 skey, u8 asc)
{
	usbd_composite_msc_scsi_sense_data_t *data = &mdev->scsi_sense_data[mdev->scsi_sense_tail];
	data->skey  = skey;
	data->w.asc = asc << 8;
	mdev->scsi_sense_tail++;
	if (mdev->scsi_sense_tail == COMP_MSC_SENSE_LIST_DEPTH) {
		mdev->scsi_sense_tail = 0U;
	}
}