/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"
#include "usbh_msc_scsi.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* USB Standard Device Descriptor */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Issue TestUnitReady command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @retval Status
  */
int usbh_scsi_test_unit_ready(usbh_msc_host_t *msc, u8 lun)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = TEST_UNIT_READY_LEN;
		cbw->field.bmCBWFlags = USB_H2D;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_CB_LENGTH);
		cbw->field.CBWCB[0] = SCSI_TEST_UNIT_READY;

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		status = HAL_BUSY;
		break;

	case BOT_CMD_BUSY:
		status = usbh_msc_bot_process(msc->host, lun);
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Issue Read Capacity command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @param  capacity: pointer to the capacity structure
  * @retval Status
  */
int usbh_scsi_read_capacity(usbh_msc_host_t *msc, u8 lun, usbh_scsi_capacity_t *capacity)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = READ_CAPACITY10_DATA_LEN;
		cbw->field.bmCBWFlags = USB_D2H;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_CB_LENGTH);
		cbw->field.CBWCB[0] = SCSI_READ_CAPACITY10;

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		msc->hbot.pbuf = msc->hbot.data;
		status = HAL_BUSY;
		break;

	case BOT_CMD_BUSY:

		status = usbh_msc_bot_process(msc->host, lun);

		if (status == HAL_OK) {
			/*assign the capacity*/
			capacity->block_nbr = msc->hbot.pbuf[3] | ((u32)msc->hbot.pbuf[2] << 8U) | \
								  ((u32)msc->hbot.pbuf[1] << 16U) | ((u32)msc->hbot.pbuf[0] << 24U);

			/*assign the page length*/
			capacity->block_size = (u16)(msc->hbot.pbuf[7] | ((u32)msc->hbot.pbuf[6] << 8U));
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Issue Inquiry command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @param  inquiry: pointer to the inquiry structure
  * @retval Status
  */
int usbh_scsi_inquiry(usbh_msc_host_t *msc, u8 lun, usbh_scsi_inquiry_t *inquiry)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = INQUIRY_DATA_LEN;
		cbw->field.bmCBWFlags = USB_D2H;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_LENGTH);
		cbw->field.CBWCB[0] = SCSI_INQUIRY;
		cbw->field.CBWCB[1] = (lun << 5);
		cbw->field.CBWCB[2] = 0U;
		cbw->field.CBWCB[3] = 0U;
		cbw->field.CBWCB[4] = 0x24U;
		cbw->field.CBWCB[5] = 0U;

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		msc->hbot.pbuf = msc->hbot.data;
		status = HAL_BUSY;
		break;

	case BOT_CMD_BUSY:

		status = usbh_msc_bot_process(msc->host, lun);

		if (status == HAL_OK) {
			usb_os_memset(inquiry, 0, sizeof(usbh_scsi_inquiry_t));
			/*assign Inquiry Data */
			inquiry->DeviceType = msc->hbot.pbuf[0] & 0x1FU;
			inquiry->PeripheralQualifier = msc->hbot.pbuf[0] >> 5U;

			if (((u32)msc->hbot.pbuf[1] & 0x80U) == 0x80U) {
				inquiry->RemovableMedia = 1U;
			} else {
				inquiry->RemovableMedia = 0U;
			}

			usb_os_memcpy(inquiry->vendor_id, &msc->hbot.pbuf[8], 8U);
			usb_os_memcpy(inquiry->product_id, &msc->hbot.pbuf[16], 16U);
			usb_os_memcpy(inquiry->revision_id, &msc->hbot.pbuf[32], 4U);
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Issue RequestSense command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @param  sense_data: pointer to the sense data structure
  * @retval Status
  */
int usbh_scsi_request_sense(usbh_msc_host_t *msc, u8 lun, usb_msc_scsi_sense_data_t *sense_data)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = REQUEST_SENSE_DATA_LEN;
		cbw->field.bmCBWFlags = USB_D2H;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_CB_LENGTH);
		cbw->field.CBWCB[0] = SCSI_REQUEST_SENSE;
		cbw->field.CBWCB[1] = (lun << 5);
		cbw->field.CBWCB[2] = 0U;
		cbw->field.CBWCB[3] = 0U;
		cbw->field.CBWCB[4] = REQUEST_SENSE_DATA_LEN;
		cbw->field.CBWCB[5] = 0U;

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		msc->hbot.pbuf = msc->hbot.data;
		status = HAL_BUSY;
		break;

	case BOT_CMD_BUSY:

		status = usbh_msc_bot_process(msc->host, lun);

		if (status == HAL_OK) {
			sense_data->key  = msc->hbot.pbuf[2] & 0x0FU;
			sense_data->asc  = msc->hbot.pbuf[12];
			sense_data->ascq = msc->hbot.pbuf[13];
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Issue write10 command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval Status
  */
int usbh_scsi_write(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = length * msc->unit[0].capacity.block_size;
		cbw->field.bmCBWFlags = USB_H2D;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_CB_LENGTH);
		cbw->field.CBWCB[0] = SCSI_WRITE10;

		/*logical block address*/
		cbw->field.CBWCB[2] = (((u8 *)(void *)&address)[3]);
		cbw->field.CBWCB[3] = (((u8 *)(void *)&address)[2]);
		cbw->field.CBWCB[4] = (((u8 *)(void *)&address)[1]);
		cbw->field.CBWCB[5] = (((u8 *)(void *)&address)[0]);

		/*Transfer length */
		cbw->field.CBWCB[7] = (((u8 *)(void *)&length)[1]);
		cbw->field.CBWCB[8] = (((u8 *)(void *)&length)[0]);

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		msc->hbot.origin_tx_pbuf = pbuf;
		msc->hbot.origin_tx_pbuf_len = cbw->field.dCBWDataTransferLength;
		if ((msc->hbot.pbuf != NULL) && (msc->hbot.pbuf != msc->hbot.data)) {
			usb_os_mfree(msc->hbot.pbuf);
		}
		msc->hbot.pbuf = (u8 *)usb_os_malloc(cbw->field.dCBWDataTransferLength);
		if (msc->hbot.pbuf != NULL) {
			usb_os_memcpy(msc->hbot.pbuf, pbuf, cbw->field.dCBWDataTransferLength);
			status = HAL_BUSY;
		}
		break;

	case BOT_CMD_BUSY:
		status = usbh_msc_bot_process(msc->host, lun);
		if (status == HAL_OK) {
			usb_os_mfree(msc->hbot.pbuf);
			msc->hbot.pbuf = NULL;
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Issue Read10 command.
  * @param  msc: Msc host handle
  * @param  lun: Logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval Status
  */
int usbh_scsi_read(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length)
{
	int status = HAL_ERR_UNKNOWN;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;

	switch (msc->hbot.cmd_state) {
	case BOT_CMD_SEND:

		/*Prepare the CBW and relevent field*/
		cbw->field.dCBWDataTransferLength = length * msc->unit[0].capacity.block_size;
		cbw->field.bmCBWFlags = USB_D2H;
		cbw->field.bCBWCBLength = CBW_LENGTH;

		usb_os_memset(cbw->field.CBWCB, 0, CBW_CB_LENGTH);
		cbw->field.CBWCB[0] = SCSI_READ10;

		/*logical block address*/
		cbw->field.CBWCB[2] = (((u8 *)(void *)&address)[3]);
		cbw->field.CBWCB[3] = (((u8 *)(void *)&address)[2]);
		cbw->field.CBWCB[4] = (((u8 *)(void *)&address)[1]);
		cbw->field.CBWCB[5] = (((u8 *)(void *)&address)[0]);

		/*Transfer length */
		cbw->field.CBWCB[7] = (((u8 *)(void *)&length)[1]);
		cbw->field.CBWCB[8] = (((u8 *)(void *)&length)[0]);

		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		msc->hbot.origin_rx_pbuf = pbuf;
		msc->hbot.origin_rx_pbuf_len = cbw->field.dCBWDataTransferLength;
		if ((msc->hbot.pbuf != NULL) && (msc->hbot.pbuf != msc->hbot.data)) {
			usb_os_mfree(msc->hbot.pbuf);
		}
		msc->hbot.pbuf = (u8 *)usb_os_malloc(cbw->field.dCBWDataTransferLength);
		if (msc->hbot.pbuf != NULL) {
			status = HAL_BUSY;
		}
		break;

	case BOT_CMD_BUSY:
		status = usbh_msc_bot_process(msc->host, lun);
		if (status == HAL_OK) {
			usb_os_memcpy(msc->hbot.origin_rx_pbuf, msc->hbot.pbuf, msc->hbot.origin_rx_pbuf_len);
			usb_os_mfree(msc->hbot.pbuf);
			msc->hbot.pbuf = NULL;
		}
		break;

	default:
		break;
	}

	return status;
}

