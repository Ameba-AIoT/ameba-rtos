/**
  ******************************************************************************
  * @file    usbh_msc.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB MSC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"
#include "usbh_msc_scsi.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_MSC_DEBUG 0

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_msc_attach(usb_host_t *host);
static int usbh_msc_detach(usb_host_t *host);
static int usbh_msc_process(usb_host_t *host);
static int usbh_msc_setup(usb_host_t *host);
static int usbh_msc_process_rw(usb_host_t *host, u8 lun);
static int usbh_msc_sof(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "MSC";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_msc_driver = {
	.class_code = MSC_CLASS_CODE,
	.attach = usbh_msc_attach,
	.detach = usbh_msc_detach,
	.setup = usbh_msc_setup,
	.process = usbh_msc_process,
	.sof = usbh_msc_sof
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
	u8 pipe_num;
	u8 interface;
	usbh_if_desc_t *msc_if_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_bot_cbw_t *cbw = msc->hbot.cbw;

	interface = usbh_get_interface(host, MSC_CLASS_CODE, USBH_MSC_TRANSPARENT, USBH_MSC_BOT);
	if (interface == 0xFFU) {
		RTK_LOGS(TAG, "[MSC] Get itf fail\n");
		return status;
	}

	msc->host = host;
	usbh_set_interface(host, interface);

	msc_if_desc = usbh_get_interface_descriptor(host, interface, 0);
	if (msc_if_desc == NULL) {
		RTK_LOGS(TAG, "[MSC] Get itf desc fail\n");
		return status;
	}

	/* Set data in/out endpoints */
	ep_desc = &msc_if_desc->ep_desc_array[0];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		msc->bulk_in_ep = ep_desc->bEndpointAddress;
		msc->bulk_in_packet_size  = ep_desc->wMaxPacketSize;
	} else {
		msc->bulk_out_ep = ep_desc->bEndpointAddress;
		msc->bulk_out_packet_size  = ep_desc->wMaxPacketSize;
	}

	ep_desc = &msc_if_desc->ep_desc_array[1];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		msc->bulk_in_ep = ep_desc->bEndpointAddress;
		msc->bulk_in_packet_size  = ep_desc->wMaxPacketSize;
	} else {
		msc->bulk_out_ep = ep_desc->bEndpointAddress;
		msc->bulk_out_packet_size  = ep_desc->wMaxPacketSize;
	}

	/*Allocate the length for host channel number out*/
	pipe_num = usbh_alloc_pipe(host, msc->bulk_out_ep);
	if (pipe_num != 0xFFU) {
		msc->bulk_out_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, "[MSC] Alloc BULK out pipe fail\n");
		return HAL_ERR_MEM;
	}

	/*Allocate the length for host channel number in*/
	pipe_num = usbh_alloc_pipe(host, msc->bulk_in_ep);
	if (pipe_num != 0xFFU) {
		msc->bulk_in_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, "[MSC] Alloc BULK in pipe fail\n");
		usbh_free_pipe(host, msc->bulk_out_pipe);
		return HAL_ERR_MEM;
	}

	msc->current_lun = 0U;
	msc->state = MSC_INIT;
	msc->error = MSC_OK;
	msc->req_state = MSC_REQ_IDLE;

	cbw->field.Signature = USBH_BOT_CBW_SIGNATURE;
	cbw->field.Tag = USBH_BOT_CBW_TAG;
	msc->hbot.state = BOT_SEND_CBW;
	msc->hbot.cmd_state = BOT_CMD_SEND;

	/* De-Initialize LUNs information */
	usb_os_memset(msc->unit, 0, sizeof(msc->unit));

	/* Open channel for OUT endpoint */
	usbh_open_pipe(host,
				   msc->bulk_out_pipe,
				   msc->bulk_out_ep,
				   USB_CH_EP_TYPE_BULK,
				   msc->bulk_out_packet_size);

	/* Open channel for IN endpoint */
	usbh_open_pipe(host,
				   msc->bulk_in_pipe,
				   msc->bulk_in_ep,
				   USB_CH_EP_TYPE_BULK,
				   msc->bulk_in_packet_size);

	if ((msc->cb != NULL) && (msc->cb->attach != NULL)) {
		msc->cb->attach();
	}

	status = HAL_OK;

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

	if ((msc->cb != NULL) && (msc->cb->detach != NULL)) {
		msc->cb->detach();
	}

	if (msc->bulk_in_pipe) {
		usbh_close_pipe(host, msc->bulk_in_pipe);
		usbh_free_pipe(host, msc->bulk_in_pipe);
		msc->bulk_in_pipe = 0U;
	}

	if (msc->bulk_out_pipe) {
		usbh_close_pipe(host, msc->bulk_out_pipe);
		usbh_free_pipe(host, msc->bulk_out_pipe);
		msc->bulk_out_pipe = 0U;
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
		setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS
								| USB_REQ_RECIPIENT_INTERFACE;
		setup.b.bRequest = USBH_MSC_GET_MAX_LUN;
		setup.b.wValue = 0U;
		setup.b.wIndex = 0U;
		setup.b.wLength = 1U;
		status = usbh_ctrl_request(host, &setup, (u8 *)(void *)&msc->max_lun);

		/* When devices do not support the GetMaxLun request, this should
		   be considred as only one logical unit is supported */
		if (status == HAL_ERR_PARA) {
			msc->max_lun = 0U;
			status = HAL_OK;
		}

		if (status == HAL_OK) {
			msc->max_lun = (msc->max_lun > USBH_MSC_MAX_LUN) ? USBH_MSC_MAX_LUN : (msc->max_lun + 1U);
			RTK_LOGS(TAG, "[MSC] Max lun %d\n", msc->max_lun);

			for (i = 0U; i < msc->max_lun; i++) {
				msc->unit[i].prev_ready_state = HAL_ERR_UNKNOWN;
				msc->unit[i].state_changed = 0U;
			}
		}
		break;

	case MSC_REQ_ERROR :
		/* a Clear Feature should be issued here */
		if (usbh_ctrl_clear_feature(host, 0x00U) == HAL_OK) {
			RTK_LOGS(TAG, "[MSC] TX clear feature fail\n");
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
  * @retval Status
  */
static int usbh_msc_process(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int status = HAL_BUSY;
	int scsi_status = HAL_BUSY;

	switch (msc->state) {
	case MSC_INIT:
		if (msc->current_lun < msc->max_lun) {
			msc->unit[msc->current_lun].error = MSC_NOT_READY;
			/* Switch MSC REQ state machine */
			switch (msc->unit[msc->current_lun].state) {
			case MSC_INIT:
				RTK_LOGS(TAG, "[MSC] Lun %d\n", msc->current_lun);
				msc->unit[msc->current_lun].state = MSC_READ_INQUIRY;
				msc->tick = host->tick;
				break;

			case MSC_READ_INQUIRY:
				scsi_status = usbh_scsi_inquiry(msc, (u8)msc->current_lun, &msc->unit[msc->current_lun].inquiry);

				if (scsi_status == HAL_OK) {
#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, "[MSC] Inq ven %s\n", msc->unit[msc->current_lun].inquiry.vendor_id);
					RTK_LOGS(TAG, "[MSC] Inq prod %s\n", msc->unit[msc->current_lun].inquiry.product_id);
					RTK_LOGS(TAG, "[MSC] Inq ver %s\n", msc->unit[msc->current_lun].inquiry.revision_id);
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
						//RTK_LOGS(TAG, "[MSC] Device ready\n");
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
						RTK_LOGS(TAG, "[MSC] Device not ready\n");
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
				scsi_status = usbh_scsi_read_capacity(msc, (u8)msc->current_lun, &msc->unit[msc->current_lun].capacity) ;

				if (scsi_status == HAL_OK) {
					if (msc->unit[msc->current_lun].state_changed == 1U) {
#if USBH_MSC_DEBUG
						RTK_LOGS(TAG, "[MSC] Capacity %dB\n",
								 (msc->unit[msc->current_lun].capacity.block_nbr * msc->unit[msc->current_lun].capacity.block_size));
						RTK_LOGS(TAG, "[MSC] Block num %d\n", msc->unit[msc->current_lun].capacity.block_nbr);
						RTK_LOGS(TAG, "[MSC] Block size %d\n", msc->unit[msc->current_lun].capacity.block_size);
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

						if ((host->tick - msc->tick) < 10000U) {
							msc->unit[msc->current_lun].state = MSC_TEST_UNIT_READY;
							break;
						}
					}

#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, "[MSC] Sense key %x\n", msc->unit[msc->current_lun].sense.key);
					RTK_LOGS(TAG, "[MSC] Sense code %x\n", msc->unit[msc->current_lun].sense.asc);
					RTK_LOGS(TAG, "[MSC] Sense code qua %x\n", msc->unit[msc->current_lun].sense.ascq);
#endif
					msc->unit[msc->current_lun].state = MSC_IDLE;
					msc->current_lun++;
				}
				if (scsi_status == HAL_ERR_UNKNOWN) {
					RTK_LOGS(TAG, "[MSC] Device not ready\n");
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

			usbh_notify_class_state_change(host);
		} else {
			msc->current_lun = 0U;
			msc->state = MSC_IDLE;

			usbh_notify_class_state_change(host);
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
  * @brief  The function is for SOF state
  * @param  host: Host handle  for MSC I/O Process
  * @retval Status
  */
static int usbh_msc_sof(usb_host_t *host)
{
	UNUSED(host);
	return HAL_OK;
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
	int status = HAL_BUSY ;
	int scsi_status = HAL_BUSY ;

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

		usbh_notify_class_state_change(host);
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

		usbh_notify_class_state_change(host);
		break;

	case MSC_REQUEST_SENSE:
		scsi_status = usbh_scsi_request_sense(msc, lun, &msc->unit[lun].sense);

		if (scsi_status == HAL_OK) {
#if USBH_MSC_DEBUG
			RTK_LOGS(TAG, "[MSC] Sense key: %x\n", msc->unit[lun].sense.key);
			RTK_LOGS(TAG, "[MSC] Sense code: %x\n", msc->unit[lun].sense.asc);
			RTK_LOGS(TAG, "[MSC] Sense code qua: %x\n", msc->unit[lun].sense.ascq);
#endif
			msc->unit[lun].state = MSC_IDLE;
			msc->unit[lun].error = MSC_ERROR;

			status = HAL_ERR_UNKNOWN;
		}
		if (scsi_status == HAL_ERR_UNKNOWN) {
			RTK_LOGS(TAG, "[MSC] Device not ready\n");
		} else {
			if (scsi_status == HAL_ERR_HW) {
				msc->unit[lun].state = MSC_UNRECOVERED_ERROR;
				status = HAL_ERR_UNKNOWN;
			}
		}

		usbh_notify_class_state_change(host);
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
	if (usbh_get_last_transfer_size(host, msc->bulk_in_pipe) != USBH_BOT_CSW_LENGTH) {
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
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_setup_req_t setup;
	usbh_bot_csw_t *csw = msc->hbot.csw;
	usbh_bot_cbw_t *cbw = msc->hbot.cbw;
	u8 toggle = 0U;

	switch (msc->hbot.state) {
	case BOT_SEND_CBW:
		cbw->field.LUN = lun;
		msc->hbot.state = BOT_SEND_CBW_WAIT;
		usbh_bulk_send_data(host, cbw->data, USBH_BOT_CBW_LENGTH, msc->bulk_out_pipe);

		break;

	case BOT_SEND_CBW_WAIT:

		urb_state = usbh_get_urb_state(host, msc->bulk_out_pipe);

		if (urb_state == USBH_URB_DONE) {
			if (cbw->field.DataTransferLength != 0U) {
				/* If there is Data Transfer Stage */
				if (((cbw->field.Flags) & USB_REQ_DIR_MASK) == USB_D2H) {
					/* Data Direction is IN */
					msc->hbot.state = BOT_DATA_IN;
				} else {
					/* Data Direction is OUT */
					msc->hbot.state = BOT_DATA_OUT;
				}
			} else {
				/* If there is NO Data Transfer Stage */
				msc->hbot.state = BOT_RECEIVE_CSW;
			}

			usbh_notify_urb_state_change(host);
		} else if (urb_state == USBH_URB_BUSY) {
			/* Re-send CBW */
			msc->hbot.state = BOT_SEND_CBW;

			usbh_notify_urb_state_change(host);
		} else {
			if (urb_state == USBH_URB_STALL) {
				msc->hbot.state  = BOT_ERROR_OUT;

				usbh_notify_urb_state_change(host);
			}
		}
		break;

	case BOT_DATA_IN:
		/* Receive first packet */
		usbh_bulk_receive_data(host, msc->hbot.pbuf, msc->bulk_in_packet_size, msc->bulk_in_pipe);

		msc->hbot.state = BOT_DATA_IN_WAIT;

		break;

	case BOT_DATA_IN_WAIT:

		urb_state = usbh_get_urb_state(host, msc->bulk_in_pipe);

		if (urb_state == USBH_URB_DONE) {
			/* Adjust Data pointer and data length */
			if (cbw->field.DataTransferLength > msc->bulk_in_packet_size) {
				msc->hbot.pbuf += msc->bulk_in_packet_size;
				cbw->field.DataTransferLength -= msc->bulk_in_packet_size;
			} else {
				cbw->field.DataTransferLength = 0U;
			}

			/* More Data To be Received */
			if (cbw->field.DataTransferLength > 0U) {
				/* Send next packet */
				usbh_bulk_receive_data(host, msc->hbot.pbuf, msc->bulk_in_packet_size, msc->bulk_in_pipe);
			} else {
				/* If value was 0, and successful transfer, then change the state */
				msc->hbot.state  = BOT_RECEIVE_CSW;

				usbh_notify_urb_state_change(host);
			}
		} else if (urb_state == USBH_URB_STALL) {
			/* This is Data IN Stage STALL Condition */
			msc->hbot.state  = BOT_ERROR_IN;

			/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
			6.7.2 Host expects to receive data from the device
			3. On a STALL condition receiving data, then:
			The host shall accept the data received.
			The host shall clear the Bulk-In pipe.
			4. The host shall attempt to receive a CSW.*/

			usbh_notify_urb_state_change(host);
		} else {
		}
		break;

	case BOT_DATA_OUT:

		usbh_bulk_send_data(host, msc->hbot.pbuf, msc->bulk_out_packet_size, msc->bulk_out_pipe);

		msc->hbot.state  = BOT_DATA_OUT_WAIT;
		break;

	case BOT_DATA_OUT_WAIT:
		urb_state = usbh_get_urb_state(host, msc->bulk_out_pipe);

		if (urb_state == USBH_URB_DONE) {
			/* Adjust Data pointer and data length */
			if (cbw->field.DataTransferLength > msc->bulk_out_packet_size) {
				msc->hbot.pbuf += msc->bulk_out_packet_size;
				cbw->field.DataTransferLength -= msc->bulk_out_packet_size;
			} else {
				cbw->field.DataTransferLength = 0U;
			}

			/* More Data To be Sent */
			if (cbw->field.DataTransferLength > 0U) {
				usbh_bulk_send_data(host, msc->hbot.pbuf, msc->bulk_out_packet_size, msc->bulk_out_pipe);
			} else {
				/* If value was 0, and successful transfer, then change the state */
				msc->hbot.state  = BOT_RECEIVE_CSW;
			}

			usbh_notify_urb_state_change(host);
		}

		else if (urb_state == USBH_URB_BUSY) {
			/* Resend same data */
			msc->hbot.state  = BOT_DATA_OUT;

			usbh_notify_urb_state_change(host);
		}

		else if (urb_state == USBH_URB_STALL) {
			msc->hbot.state  = BOT_ERROR_OUT;

			/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
			6.7.3 Ho - Host expects to send data to the device
			3. On a STALL condition sending data, then:
			" The host shall clear the Bulk-Out pipe.
			4. The host shall attempt to receive a CSW.
			*/

			usbh_notify_urb_state_change(host);
		} else {
		}
		break;

	case BOT_RECEIVE_CSW:

		usbh_bulk_receive_data(host, csw->data, USBH_BOT_CSW_LENGTH, msc->bulk_in_pipe);

		msc->hbot.state  = BOT_RECEIVE_CSW_WAIT;
		break;

	case BOT_RECEIVE_CSW_WAIT:

		urb_state = usbh_get_urb_state(host, msc->bulk_in_pipe);

		/* Decode CSW */
		if (urb_state == USBH_URB_DONE) {
			msc->hbot.state = BOT_SEND_CBW;
			msc->hbot.cmd_state = BOT_CMD_SEND;
			CSW_Status = usbh_msc_decode_csw(host);

			if (CSW_Status == BOT_CSW_CMD_PASSED) {
				status = HAL_OK;
			} else {
				status = HAL_ERR_UNKNOWN;
			}

			usbh_notify_urb_state_change(host);
		} else if (urb_state == USBH_URB_STALL) {
			msc->hbot.state  = BOT_ERROR_IN;

			usbh_notify_urb_state_change(host);
		} else {
		}
		break;

	case BOT_ERROR_IN:
		error = usbh_ctrl_clear_feature(host, msc->bulk_in_ep);
		if (error == HAL_OK) {
			msc->hbot.state = BOT_RECEIVE_CSW;
		} else if (error == HAL_ERR_HW) {
			/* This means that there is a STALL Error limit, Do Reset Recovery */
			msc->hbot.state = BOT_UNRECOVERED_ERROR;
		} else {
		}
		break;

	case BOT_ERROR_OUT:
		error = usbh_ctrl_clear_feature(host, msc->bulk_out_ep);
		if (error == HAL_OK) {
			toggle = usbh_get_toggle(host, msc->bulk_out_pipe);
			usbh_set_toggle(host, msc->bulk_out_pipe, 1U - toggle);
			usbh_set_toggle(host, msc->bulk_in_pipe, 0U);
			msc->hbot.state = BOT_ERROR_IN;
		} else {
			if (error == HAL_ERR_HW) {
				msc->hbot.state = BOT_UNRECOVERED_ERROR;
			}
		}
		break;


	case BOT_UNRECOVERED_ERROR:
		setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		setup.b.bRequest = USBH_MSC_BOT_RESET;
		setup.b.wValue = 0U;
		setup.b.wIndex = 0U;
		setup.b.wLength = 0U;

		status = usbh_ctrl_request(host, &setup, NULL);
		if (status == HAL_OK) {
			msc->hbot.state = BOT_SEND_CBW;
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Check if the MSC function is ready
  * @param  None
  * @retval Status
  */
int usbh_msc_is_rdy(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int res;

	if ((msc->host->state == USBH_CLASS_READY) && (msc->state == MSC_IDLE)) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

/**
  * @brief  Get the Max LUN supported
  * @param  None
  * @retval logical Unit Number supported
  */
u32 usbh_msc_get_max_lun(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;

	if ((msc->host->state == USBH_CLASS_READY) && (msc->state == MSC_IDLE)) {
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

	if ((msc->host->state == USBH_CLASS_READY) && (msc->unit[lun].error == MSC_OK)) {
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

	if ((msc->host->state == USBH_CLASS_READY) && (USBH_MSC_MAX_LUN > lun)) {
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
	int is_device_connected;
	usbh_msc_host_t *msc = &usbh_msc_host;

	is_device_connected = usbh_get_status();

	if ((is_device_connected == 0U) ||
		(msc->host->state != USBH_CLASS_READY) ||
		(msc->unit[lun].state != MSC_IDLE)) {
		return  HAL_ERR_UNKNOWN;
	}

	msc->state = MSC_READ;
	msc->unit[lun].state = MSC_READ;

	usbh_scsi_read(msc, lun, address, pbuf, length);

	timeout = msc->host->tick;

	while (usbh_msc_process_rw(msc->host, lun) == HAL_BUSY) {
#if defined (ARM_CORE_CA32)
		//FIXME, remove this in AP
		usb_os_delay_us(200);
#endif
		if (((msc->host->tick - timeout) > (10000U * length)) || (is_device_connected == 0U)) {
			msc->state = MSC_IDLE;
			return HAL_ERR_UNKNOWN;
		}
	}
	msc->state = MSC_IDLE;

	return HAL_OK;
}

/**
  * @brief  USBH_MSC_Write
  *         The function performs a Write operation
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval Status
  */
int usbh_msc_write(u8 lun, u32 address, u8 *pbuf, u32 length)
{
	u32 timeout;
	int is_device_connected;
	usbh_msc_host_t *msc = &usbh_msc_host;

	is_device_connected = usbh_get_status();

	if ((is_device_connected == 0U) ||
		(msc->host->state != USBH_CLASS_READY) ||
		(msc->unit[lun].state != MSC_IDLE)) {
		return  HAL_ERR_UNKNOWN;
	}

	msc->state = MSC_WRITE;
	msc->unit[lun].state = MSC_WRITE;

	usbh_scsi_write(msc, lun, address, pbuf, length);

	timeout = msc->host->tick;
	while (usbh_msc_process_rw(msc->host, lun) == HAL_BUSY) {
#if defined (ARM_CORE_CA32)
		//FIXME, remove this in AP
		usb_os_delay_us(200);
#endif
		if (((msc->host->tick - timeout) > (10000U * length)) || (is_device_connected == 0U)) {
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
	void *buf = NULL;
	usbh_msc_host_t *msc = &usbh_msc_host;

	if (cb != NULL) {
		msc->cb = cb;
	}

	buf = usb_os_malloc(USBH_BOT_CSW_LENGTH);
	if (buf != NULL) {
		msc->hbot.csw = buf;
	} else {
		return HAL_ERR_MEM;
	}

	buf = usb_os_malloc(USBH_BOT_CBW_LENGTH);
	if (buf != NULL) {
		msc->hbot.cbw = buf;
	} else {
		usb_os_mfree(msc->hbot.csw);
		return HAL_ERR_MEM;
	}

	ret = usbh_register_class(&usbh_msc_driver);

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

	ret = usbh_unregister_class(&usbh_msc_driver);

	usb_os_mfree(msc->hbot.csw);
	usb_os_mfree(msc->hbot.cbw);

	return ret;
}

