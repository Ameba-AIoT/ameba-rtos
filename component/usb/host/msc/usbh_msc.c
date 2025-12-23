/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"
#include "usbh_msc_scsi.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_MSC_DEBUG 0
#define MSC_XFER_MAX_TIMEOUT_TICK            100  //sof
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_msc_attach(usb_host_t *host);
static int usbh_msc_detach(usb_host_t *host);
static int usbh_msc_process(usb_host_t *host, u32 msg);
static int usbh_msc_setup(usb_host_t *host);
static int usbh_msc_process_rw(usb_host_t *host, u8 lun);
/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

/* USB MSC device identification */
static const usbh_dev_id_t msc_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = MSC_CLASS_CODE,
	},
	{
	},
};

/* USB Host MSC class driver */
static usbh_class_driver_t usbh_msc_driver = {
	.id_table = msc_devs,
	.attach = usbh_msc_attach,
	.detach = usbh_msc_detach,
	.setup = usbh_msc_setup,
	.process = usbh_msc_process,
};

static usbh_msc_host_t usbh_msc_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_msc_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_itf_data_t *itf_data;
	usbh_ep_desc_t *ep_desc;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_bot_cbw_t *cbw = msc->hbot.cbw;
	usbh_itf_desc_t *msc_itf_desc;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;
	usbh_dev_id_t dev_id = {0,};

	dev_id.bInterfaceClass = MSC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_MSC_TRANSPARENT;
	dev_id.bInterfaceProtocol = USBH_MSC_BOT;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get itf desc fail\n");
	} else {
		msc_itf_desc = itf_data->itf_desc_array;
		msc->host = host;

		/* Set data in/out endpoints */
		for (int i = 0; i < 2; i++) {
			ep_desc = &msc_itf_desc->ep_desc_array[i];
			if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
				usbh_open_pipe(host, bulk_in, ep_desc);
				bulk_in->max_timeout_tick = MSC_XFER_MAX_TIMEOUT_TICK;
			} else {
				usbh_open_pipe(host, bulk_out, ep_desc);
				bulk_out->max_timeout_tick = MSC_XFER_MAX_TIMEOUT_TICK;
			}
		}

		msc->current_lun = 0U;
		msc->state = MSC_INIT;
		msc->error = MSC_OK;
		msc->req_state = MSC_REQ_IDLE;

		cbw->field.Signature = USBH_BOT_CBW_SIGNATURE;
		cbw->field.Tag = USBH_BOT_CBW_TAG;
		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_SEND;

		/* De-Initialize LUNs information */
		usb_os_memset(msc->unit, 0, sizeof(msc->unit));

		if ((msc->cb != NULL) && (msc->cb->attach != NULL)) {
			msc->cb->attach();
		}

		status = HAL_OK;
	}
	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_msc_detach(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;

	if ((msc->cb != NULL) && (msc->cb->detach != NULL)) {
		msc->cb->detach();
	}

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}
	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_msc_setup(usb_host_t *host)
{
	int status = HAL_BUSY;
	usbh_setup_req_t setup;
	usbh_msc_host_t *msc = &usbh_msc_host;
	u8 i;

	/* Switch MSC REQ state machine */
	switch (msc->req_state) {
	case MSC_REQ_IDLE:
	case MSC_REQ_GET_MAX_LUN:
		/* Issue GetMaxLUN request */
		setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS
								  | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USBH_MSC_GET_MAX_LUN;
		setup.req.wValue = 0U;
		setup.req.wIndex = 0U;
		setup.req.wLength = 1U;
		status = usbh_ctrl_request(host, &setup, (u8 *)(void *)msc->max_lun_buf);
		/* When devices do not support the GetMaxLun request, this should
		   be considred as only one logical unit is supported */
		if (status == HAL_ERR_PARA) {
			msc->max_lun = 0U;
			status = HAL_OK;
		}

		if (status == HAL_OK) {
			msc->max_lun = *(msc->max_lun_buf);
			msc->max_lun = (msc->max_lun > USBH_MSC_MAX_LUN) ? USBH_MSC_MAX_LUN : (msc->max_lun + 1U);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Max lun %d\n", msc->max_lun);

			for (i = 0U; i < msc->max_lun; i++) {
				msc->unit[i].prev_ready_state = HAL_ERR_UNKNOWN;
				msc->unit[i].state_changed = 0U;
			}
		}
		break;

	case MSC_REQ_ERROR :
		/* a Clear Feature should be issued here */
		if (usbh_ctrl_clear_feature(host, 0x00U) == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX clear feature fail\n");
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: Message data
  * @retval Status
  */
static int usbh_msc_process(usb_host_t *host, u32 msg)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int status = HAL_BUSY;
	int scsi_status = HAL_BUSY;
	UNUSED(msg);

	switch (msc->state) {
	case MSC_INIT:
		if (msc->current_lun < msc->max_lun) {
			msc->unit[msc->current_lun].error = MSC_NOT_READY;
			/* Switch MSC REQ state machine */
			switch (msc->unit[msc->current_lun].state) {
			case MSC_INIT:
				RTK_LOGS(TAG, RTK_LOG_INFO, "Lun %d\n", msc->current_lun);
				msc->unit[msc->current_lun].state = MSC_READ_INQUIRY;
				msc->tick = usbh_get_tick(host);
				break;

			case MSC_READ_INQUIRY:
				scsi_status = usbh_scsi_inquiry(msc, (u8)msc->current_lun, &msc->unit[msc->current_lun].inquiry);

				if (scsi_status == HAL_OK) {
#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq ven %s\n", msc->unit[msc->current_lun].inquiry.vendor_id);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq prod %s\n", msc->unit[msc->current_lun].inquiry.product_id);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq ver %s\n", msc->unit[msc->current_lun].inquiry.revision_id);
#endif
					msc->unit[msc->current_lun].state = MSC_TEST_UNIT_READY;
				}
				if (scsi_status == HAL_ERR_UNKNOWN) {
					msc->unit[msc->current_lun].state = MSC_REQUEST_SENSE;
				} else {
					if (scsi_status == HAL_ERR_HW) {
						msc->unit[msc->current_lun].state = MSC_IDLE;
						msc->unit[msc->current_lun].error = MSC_ERROR;
					}
				}
				break;

			case MSC_TEST_UNIT_READY:
				scsi_status = usbh_scsi_test_unit_ready(msc, (u8)msc->current_lun);

				if (scsi_status == HAL_OK) {
					if (msc->unit[msc->current_lun].prev_ready_state != HAL_OK) {
						msc->unit[msc->current_lun].state_changed = 1U;
						//RTK_LOGS(TAG, RTK_LOG_INFO, "Device ready\n");
					} else {
						msc->unit[msc->current_lun].state_changed = 0U;
					}
					msc->unit[msc->current_lun].state = MSC_READ_CAPACITY10;
					msc->unit[msc->current_lun].error = MSC_OK;
					msc->unit[msc->current_lun].prev_ready_state = HAL_OK;
				}
				if (scsi_status == HAL_ERR_UNKNOWN) {
					/* Media not ready, so try to check again during 10s */
					if (msc->unit[msc->current_lun].prev_ready_state != HAL_ERR_UNKNOWN) {
						msc->unit[msc->current_lun].state_changed = 1U;
						RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
					} else {
						msc->unit[msc->current_lun].state_changed = 0U;
					}
					msc->unit[msc->current_lun].state = MSC_REQUEST_SENSE;
					msc->unit[msc->current_lun].error = MSC_NOT_READY;
					msc->unit[msc->current_lun].prev_ready_state = HAL_ERR_UNKNOWN;
				} else {
					if (scsi_status == HAL_ERR_HW) {
						msc->unit[msc->current_lun].state = MSC_IDLE;
						msc->unit[msc->current_lun].error = MSC_ERROR;
					}
				}
				break;

			case MSC_READ_CAPACITY10:
				scsi_status = usbh_scsi_read_capacity(msc, (u8)msc->current_lun, &msc->unit[msc->current_lun].capacity);

				if (scsi_status == HAL_OK) {
					if (msc->unit[msc->current_lun].state_changed == 1U) {
#if USBH_MSC_DEBUG
						RTK_LOGS(TAG, RTK_LOG_INFO, "Capacity %dB\n",
								 (msc->unit[msc->current_lun].capacity.block_nbr * msc->unit[msc->current_lun].capacity.block_size));
						RTK_LOGS(TAG, RTK_LOG_INFO, "Block num %d\n", msc->unit[msc->current_lun].capacity.block_nbr);
						RTK_LOGS(TAG, RTK_LOG_INFO, "Block size %d\n", msc->unit[msc->current_lun].capacity.block_size);
#endif
					}
					msc->unit[msc->current_lun].state = MSC_IDLE;
					msc->unit[msc->current_lun].error = MSC_OK;
					msc->current_lun++;
				} else if (scsi_status == HAL_ERR_UNKNOWN) {
					msc->unit[msc->current_lun].state = MSC_REQUEST_SENSE;
				} else {
					if (scsi_status == HAL_ERR_HW) {
						msc->unit[msc->current_lun].state = MSC_IDLE;
						msc->unit[msc->current_lun].error = MSC_ERROR;
					}
				}
				break;

			case MSC_REQUEST_SENSE:
				scsi_status = usbh_scsi_request_sense(msc, (u8)msc->current_lun, &msc->unit[msc->current_lun].sense);

				if (scsi_status == HAL_OK) {
					if ((msc->unit[msc->current_lun].sense.key == SCSI_SENSE_KEY_UNIT_ATTENTION) ||
						(msc->unit[msc->current_lun].sense.key == SCSI_SENSE_KEY_NOT_READY)) {

						if (usbh_get_elapsed_ticks(host, msc->tick) < 10000U) {
							msc->unit[msc->current_lun].state = MSC_TEST_UNIT_READY;
							break;
						}
					}

#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense key %x\n", msc->unit[msc->current_lun].sense.key);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code %x\n", msc->unit[msc->current_lun].sense.asc);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code qua %x\n", msc->unit[msc->current_lun].sense.ascq);
#endif
					msc->unit[msc->current_lun].state = MSC_IDLE;
					msc->current_lun++;
				}
				if (scsi_status == HAL_ERR_UNKNOWN) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
					msc->unit[msc->current_lun].state = MSC_UNRECOVERED_ERROR;
				} else {
					if (scsi_status == HAL_ERR_HW) {
						msc->unit[msc->current_lun].state = MSC_IDLE;
						msc->unit[msc->current_lun].error = MSC_ERROR;
					}
				}
				break;

			case MSC_UNRECOVERED_ERROR:
				msc->current_lun++;
				break;

			default:
				break;
			}

			usbh_notify_class_state_change(host, 0);
		} else {
			msc->current_lun = 0U;
			msc->state = MSC_IDLE;

			usbh_notify_class_state_change(host, 0);
			if ((msc->cb != NULL) && (msc->cb->setup != NULL)) {
				msc->cb->setup();
			}
		}
		break;

	case MSC_IDLE:
		status = HAL_OK;
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Transfer state machine handling
  * @param  host: Host handle  for MSC I/O Process
  * @param  lun: logical Unit Number
  * @retval Status
*/
static int usbh_msc_process_rw(usb_host_t *host, u8 lun)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int status = HAL_BUSY;
	int scsi_status = HAL_BUSY;

	/* Switch MSC REQ state machine */
	switch (msc->unit[lun].state) {

	case MSC_READ:
		scsi_status = usbh_scsi_read(msc, lun, 0U, NULL, 0U);

		if (scsi_status == HAL_OK) {
			msc->unit[lun].state = MSC_IDLE;
			status = HAL_OK;
		} else if (scsi_status == HAL_ERR_UNKNOWN) {
			msc->unit[lun].state = MSC_REQUEST_SENSE;
		} else {
			if (scsi_status == HAL_ERR_HW) {
				msc->unit[lun].state = MSC_UNRECOVERED_ERROR;
				status = HAL_ERR_UNKNOWN;
			}
		}

		usbh_notify_class_state_change(host, 0);
		break;

	case MSC_WRITE:
		scsi_status = usbh_scsi_write(msc, lun, 0U, NULL, 0U);

		if (scsi_status == HAL_OK) {
			msc->unit[lun].state = MSC_IDLE;
			status = HAL_OK;
		} else if (scsi_status == HAL_ERR_UNKNOWN) {
			msc->unit[lun].state = MSC_REQUEST_SENSE;
		} else {
			if (scsi_status == HAL_ERR_HW) {
				msc->unit[lun].state = MSC_UNRECOVERED_ERROR;
				status = HAL_ERR_UNKNOWN;
			}
		}

		usbh_notify_class_state_change(host, 0);
		break;

	case MSC_REQUEST_SENSE:
		scsi_status = usbh_scsi_request_sense(msc, lun, &msc->unit[lun].sense);

		if (scsi_status == HAL_OK) {
#if USBH_MSC_DEBUG
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense key: %x\n", msc->unit[lun].sense.key);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code: %x\n", msc->unit[lun].sense.asc);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code qua: %x\n", msc->unit[lun].sense.ascq);
#endif
			msc->unit[lun].state = MSC_IDLE;
			msc->unit[lun].error = MSC_ERROR;

			status = HAL_ERR_UNKNOWN;
		}
		if (scsi_status == HAL_ERR_UNKNOWN) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
		} else {
			if (scsi_status == HAL_ERR_HW) {
				msc->unit[lun].state = MSC_UNRECOVERED_ERROR;
				status = HAL_ERR_UNKNOWN;
			}
		}

		usbh_notify_class_state_change(host, 0);
		break;

	default:
		break;

	}
	return status;
}

/**
  * @brief  Decodes the CSW received by the device and updates the
  *         same to upper layer.
  * @param  host: Host handle
  * @retval Status
  * @notes
  *     Refer to USB Mass-Storage Class : BOT (www.usb.org)
  *    6.3.1 Valid CSW Conditions :
  *     The host shall consider the CSW valid when:
  *     1. dCSWSignature is equal to 53425355h
  *     2. the CSW is 13 (Dh) bytes in length,
  *     3. dCSWTag matches the dCBWTag from the corresponding CBW.
  */

static usbh_bot_csw_state_t usbh_msc_decode_csw(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_bot_csw_t *csw = msc->hbot.csw;
	usbh_bot_cbw_t *cbw = msc->hbot.cbw;
	usbh_bot_csw_state_t status = BOT_CSW_CMD_FAILED;

	/*Checking if the transfer length is different than 13*/
	if (usbh_get_last_transfer_size(host, &msc->bulk_in) != USBH_BOT_CSW_LENGTH) {
		/*(4) Hi > Dn (Host expects to receive data from the device,
		Device intends to transfer no data)
		(5) Hi > Di (Host expects to receive data from the device,
		Device intends to send data to the host)
		(9) Ho > Dn (Host expects to send data to the device,
		Device intends to transfer no data)
		(11) Ho > Do  (Host expects to send data to the device,
		Device intends to receive data from the host)*/


		status = BOT_CSW_PHASE_ERROR;
	} else {
		/* CSW length is Correct */

		/* Check validity of the CSW Signature and CSWStatus */
		if (csw->field.Signature == USBH_BOT_CSW_SIGNATURE) {
			/* Check Condition 1. dCSWSignature is equal to 53425355h */

			if (csw->field.Tag == cbw->field.Tag) {
				/* Check Condition 3. dCSWTag matches the dCBWTag from the
				corresponding CBW */

				if (csw->field.Status == 0U) {
					/* Refer to USB Mass-Storage Class : BOT (www.usb.org)

					Hn Host expects no data transfers
					Hi Host expects to receive data from the device
					Ho Host expects to send data to the device

					Dn Device intends to transfer no data
					Di Device intends to send data to the host
					Do Device intends to receive data from the host

					Section 6.7
					(1) Hn = Dn (Host expects no data transfers,
					Device intends to transfer no data)
					(6) Hi = Di (Host expects to receive data from the device,
					Device intends to send data to the host)
					(12) Ho = Do (Host expects to send data to the device,
					Device intends to receive data from the host)

					*/

					status = BOT_CSW_CMD_PASSED;
				} else if (csw->field.Status == 1U) {
					status = BOT_CSW_CMD_FAILED;
				}

				else if (csw->field.Status == 2U) {
					/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
					Section 6.7
					(2) Hn < Di ( Host expects no data transfers,
					Device intends to send data to the host)
					(3) Hn < Do ( Host expects no data transfers,
					Device intends to receive data from the host)
					(7) Hi < Di ( Host expects to receive data from the device,
					Device intends to send data to the host)
					(8) Hi <> Do ( Host expects to receive data from the device,
					Device intends to receive data from the host)
					(10) Ho <> Di (Host expects to send data to the device,
					Di Device intends to send data to the host)
					(13) Ho < Do (Host expects to send data to the device,
					Device intends to receive data from the host)
					*/

					status = BOT_CSW_PHASE_ERROR;
				} else {
				}
			} /* CSW Tag Matching is Checked  */
		} /* CSW Signature Correct Checking */
		else {
			/* If the CSW Signature is not valid, We sall return the Phase Error to
			Upper Layers for Reset Recovery */

			status = BOT_CSW_PHASE_ERROR;
		}
	} /* CSW Length Check*/

	return status;
}
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Handle the BOT protocol.
  * @param  host: Host handle
  * @param  lun: Logical Unit Number
  * @retval Status
  */
int usbh_msc_bot_process(usb_host_t *host, u8 lun)
{
	int status = HAL_BUSY;
	int error  = HAL_BUSY;
	usbh_bot_csw_state_t CSW_Status = BOT_CSW_CMD_FAILED;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_setup_req_t setup;
	usbh_bot_csw_t *csw = msc->hbot.csw;
	usbh_bot_cbw_t *cbw = msc->hbot.cbw;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;
	int ret;

	switch (msc->hbot.state) {
	case BOT_SEND_CBW:

		if (bulk_out->xfer_state == USBH_EP_XFER_START) {
			cbw->field.LUN = lun;
			bulk_out->xfer_buf = cbw->data;
			bulk_out->xfer_len = USBH_BOT_CBW_LENGTH;
			usbh_transfer_process(host, bulk_out);
		} else {
			ret = usbh_transfer_process(host, bulk_out);

			if ((ret == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
				if (cbw->field.DataTransferLength != 0U) {
					/* If there is Data Transfer Stage */
					if (((cbw->field.Flags) & USB_REQ_DIR_MASK) == USB_D2H) {
						/* Data Direction is IN */
						msc->hbot.state = BOT_DATA_IN;
						bulk_in->xfer_state = USBH_EP_XFER_START;
					} else {
						/* Data Direction is OUT */
						msc->hbot.state = BOT_DATA_OUT;
						bulk_out->xfer_state = USBH_EP_XFER_START;
					}
				} else {
					/* If there is NO Data Transfer Stage */
					msc->hbot.state = BOT_RECEIVE_CSW;
					bulk_in->xfer_state = USBH_EP_XFER_START;
				}
				usbh_notify_class_state_change(host, 0);
			} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX CBW err: %d\n", usbh_get_urb_state(host, bulk_out));
				msc->hbot.state  = BOT_ERROR_OUT;
				usbh_notify_class_state_change(host, 0);
			}
		}
		break;
	case BOT_DATA_IN:

		if (bulk_in->xfer_state == USBH_EP_XFER_START) {
			bulk_in->xfer_buf = msc->hbot.pbuf;
			bulk_in->xfer_len = cbw->field.DataTransferLength;
			usbh_transfer_process(host, bulk_in);
		} else {
			ret = usbh_transfer_process(host, bulk_in);

			if ((ret == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
				msc->hbot.state  = BOT_RECEIVE_CSW;
				bulk_in->xfer_state = USBH_EP_XFER_START;
				usbh_notify_class_state_change(host, 0);
			} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "RX data err: %d\n", usbh_get_urb_state(host, bulk_in));
				msc->hbot.state  = BOT_ERROR_IN;
				usbh_notify_class_state_change(host, 0);
			}
		}
		break;
	case BOT_DATA_OUT:

		if (bulk_out->xfer_state == USBH_EP_XFER_START) {
			msc->tick = usbh_get_tick(host);
			bulk_out->xfer_buf = msc->hbot.pbuf;
			bulk_out->xfer_len = cbw->field.DataTransferLength;
			usbh_transfer_process(host, bulk_out);
		} else {
			ret = usbh_transfer_process(host, bulk_out);

			if ((ret == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
				msc->hbot.state = BOT_RECEIVE_CSW;
				bulk_in->xfer_state = USBH_EP_XFER_START;
				usbh_notify_class_state_change(host, 0);
			} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX data err: %d\n", usbh_get_urb_state(host, bulk_out));
				msc->hbot.state  = BOT_ERROR_OUT;
				usbh_notify_class_state_change(host, 0);
			}
		}
		break;
	case BOT_RECEIVE_CSW:

		if (bulk_in->xfer_state == USBH_EP_XFER_START) {
			bulk_in->xfer_buf = csw->data;
			bulk_in->xfer_len = USBH_BOT_CSW_LENGTH;
			ret = usbh_transfer_process(host, bulk_in);
		} else {
			/* Decode CSW */

			if (usbh_get_urb_state(host, bulk_in) == USBH_URB_DONE) {
				bulk_in->xfer_state = USBH_EP_XFER_IDLE;
				msc->hbot.state = BOT_SEND_CBW;
				msc->hbot.cmd_state = BOT_CMD_SEND;
				bulk_out->xfer_state = USBH_EP_XFER_START;
				CSW_Status = usbh_msc_decode_csw(host);

				if (CSW_Status == BOT_CSW_CMD_PASSED) {
					status = HAL_OK;
				} else {
					status = HAL_ERR_UNKNOWN;
				}
				usbh_notify_class_state_change(host, 0);
			} else if (usbh_get_urb_state(host, bulk_in) == USBH_URB_STALL) {
				msc->hbot.state  = BOT_ERROR_IN;
				usbh_notify_class_state_change(host, 0);
			}
		}
		break;
	case BOT_ERROR_IN:
		error = usbh_ctrl_clear_feature(host, bulk_in->ep_addr);
		if (error == HAL_OK) {
			msc->hbot.state = BOT_RECEIVE_CSW;
			bulk_in->xfer_state = USBH_EP_XFER_START;
		} else if (error == HAL_ERR_HW) {
			/* This means that there is a STALL Error limit, Do Reset Recovery */
			msc->hbot.state = BOT_UNRECOVERED_ERROR;
		} else {
		}
		break;

	case BOT_ERROR_OUT:
		error = usbh_ctrl_clear_feature(host, bulk_out->ep_addr);
		if (error == HAL_OK) {
			msc->hbot.state = BOT_ERROR_IN;
		} else {
			if (error == HAL_ERR_HW) {
				msc->hbot.state = BOT_UNRECOVERED_ERROR;
			}
		}
		break;


	case BOT_UNRECOVERED_ERROR:
		setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USBH_MSC_BOT_RESET;
		setup.req.wValue = 0U;
		setup.req.wIndex = 0U;
		setup.req.wLength = 0U;

		status = usbh_ctrl_request(host, &setup, NULL);
		if (status == HAL_OK) {
			msc->hbot.state = BOT_SEND_CBW;
			bulk_out->xfer_state = USBH_EP_XFER_START;
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Check if the MSC function is ready
  * @retval Status
  */
int usbh_msc_is_rdy(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int res;

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->state == MSC_IDLE)) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

/**
  * @brief  Get the Max LUN supported
  * @retval logical Unit Number supported
  */
u32 usbh_msc_get_max_lun(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->state == MSC_IDLE)) {
		return msc->max_lun;
	}

	return 0xFFFFFFFFU;
}

/**
  * @brief  Check whether a LUN is ready
  * @param  lun: logical Unit Number
  * @retval Lun status (0: not ready / 1: ready)
  */
int usbh_msc_unit_is_rdy(u8 lun)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int res;

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->unit[lun].error == MSC_OK)) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

/**
  * @brief  Get a LUN information
  * @param  lun: logical Unit Number
  * @param  info: logical Unit information
  * @retval Status
  */
int usbh_msc_get_lun_info(u8 lun, usbh_msc_lun_t *info)
{
	usbh_msc_host_t *msc = &usbh_msc_host;

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (USBH_MSC_MAX_LUN > lun)) {
		usb_os_memcpy(info, &msc->unit[lun], sizeof(usbh_msc_lun_t));
		return HAL_OK;
	} else {
		return HAL_ERR_UNKNOWN;
	}
}

/**
  * @brief  Performs a Read operation
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval Status
  */
int usbh_msc_read(u8 lun, u32 address, u8 *pbuf, u32 length)
{
	u32 timeout;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_host_t *host = msc->host;

	if (((host->connect_state != USBH_STATE_ATTACH) &&
		 (host->connect_state != USBH_STATE_SETUP)) ||
		(msc->unit[lun].state != MSC_IDLE)) {
		return  HAL_ERR_UNKNOWN;
	}

	msc->state = MSC_READ;
	msc->unit[lun].state = MSC_READ;

	usbh_scsi_read(msc, lun, address, pbuf, length);

	timeout = usbh_get_tick(msc->host);

	while (usbh_msc_process_rw(msc->host, lun) == HAL_BUSY) {
#if defined(CONFIG_ARM_CORE_CA32) && CONFIG_ARM_CORE_CA32
		//FIXME, remove this in AP
		usb_os_delay_us(200);
#endif
		if ((usbh_get_elapsed_ticks(msc->host, timeout) > (10000U * length)) || (host->connect_state < USBH_STATE_ATTACH)) {
			msc->state = MSC_IDLE;
			return HAL_ERR_UNKNOWN;
		}
	}
	msc->state = MSC_IDLE;
	return HAL_OK;
}

/**
  * @brief  Performs a Write operation.
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval Status
  */
int usbh_msc_write(u8 lun, u32 address, u8 *pbuf, u32 length)
{
	u32 timeout;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_host_t *host = msc->host;

	if (((host->connect_state != USBH_STATE_ATTACH) &&
		 (host->connect_state != USBH_STATE_SETUP)) ||
		(msc->unit[lun].state != MSC_IDLE)) {
		return  HAL_ERR_UNKNOWN;
	}

	msc->state = MSC_WRITE;
	msc->unit[lun].state = MSC_WRITE;

	usbh_scsi_write(msc, lun, address, pbuf, length);

	timeout = usbh_get_tick(msc->host);
	while (usbh_msc_process_rw(msc->host, lun) == HAL_BUSY) {
#if defined(CONFIG_ARM_CORE_CA32) && CONFIG_ARM_CORE_CA32
		//FIXME, remove this in AP
		usb_os_delay_us(200);
#endif
		if ((usbh_get_elapsed_ticks(msc->host, timeout) > (10000U * length)) || (host->connect_state < USBH_STATE_ATTACH)) {
			msc->state = MSC_IDLE;
			return HAL_ERR_UNKNOWN;
		}
	}
	msc->state = MSC_IDLE;
	return HAL_OK;
}

/**
  * @brief  Init MSC class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_msc_init(usbh_msc_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_msc_host_t *msc = &usbh_msc_host;

	if (cb != NULL) {
		msc->cb = cb;
	}

	msc->hbot.cbw = usb_os_malloc(USBH_BOT_CSW_LENGTH);
	if (msc->hbot.cbw == NULL) {
		return HAL_ERR_MEM;
	}

	msc->hbot.csw = usb_os_malloc(USBH_BOT_CBW_LENGTH);
	if (msc->hbot.csw == NULL) {
		ret = HAL_ERR_MEM;
		goto exit_free;
	}

	msc->hbot.data = usb_os_malloc(USBH_BOT_DATA_LENGTH);
	if (msc->hbot.data == NULL) {
		ret = HAL_ERR_MEM;
		goto exit_free;
	}

	msc->max_lun_buf = usb_os_malloc(1);
	if (msc->max_lun_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto exit_free;
	}

	ret = usbh_register_class(&usbh_msc_driver);
	return ret;

exit_free:
	if (msc->max_lun_buf != NULL) {
		usb_os_mfree(msc->max_lun_buf);
		msc->max_lun_buf = NULL;
	}
	if (msc->hbot.data != NULL) {
		usb_os_mfree(msc->hbot.data);
		msc->hbot.data = NULL;
	}
	if (msc->hbot.csw != NULL) {
		usb_os_mfree(msc->hbot.csw);
		msc->hbot.csw = NULL;
	}
	if (msc->hbot.cbw != NULL) {
		usb_os_mfree(msc->hbot.cbw);
		msc->hbot.cbw = NULL;
	}
	return ret;
}

/**
  * @brief  Deinit MSC class
  * @retval Status
  */
int usbh_msc_deinit(void)
{
	int ret = HAL_OK;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_host_t *host = msc->host;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	if (msc->max_lun_buf != NULL) {
		usb_os_mfree(msc->max_lun_buf);
		msc->max_lun_buf = NULL;
	}

	if (msc->hbot.data != NULL) {
		usb_os_mfree(msc->hbot.data);
		msc->hbot.data = NULL;
	}

	if (msc->hbot.csw != NULL) {
		usb_os_mfree(msc->hbot.csw);
		msc->hbot.csw = NULL;
	}

	if (msc->hbot.cbw != NULL) {
		usb_os_mfree(msc->hbot.cbw);
		msc->hbot.cbw = NULL;
	}

	ret = usbh_unregister_class(&usbh_msc_driver);

	return ret;
}

