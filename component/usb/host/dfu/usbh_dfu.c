/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_dfu.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_dfu_attach(usb_host_t *host);
static int usbh_dfu_detach(usb_host_t *host);
static int usbh_dfu_setup(usb_host_t *host);
static int usbh_dfu_process(usb_host_t *host, usbh_event_t *event);

static int usbh_dfu_process_getstatus(usb_host_t *host);
static int usbh_dfu_process_clrstatus(usb_host_t *host);
static int usbh_dfu_process_dnload(usb_host_t *host, u16 block_num, u8 *buf, u16 len);
static int usbh_dfu_process_upload(usb_host_t *host, u16 block_num, u8 *buf, u16 len);
static int usbh_dfu_process_abort(usb_host_t *host);
static int usbh_dfu_process_detach(usb_host_t *host, u16 timeout);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "DFU";

/* USB DFU device identification table.
 * Always, also match Run-Time interfaces (Protocol=0x01)
 * so the host can initiate the DFU 1.1 §5 Reconfiguration Phase automatically. */
static const usbh_dev_id_t dfu_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_DFU_CLASS_CODE,
		.bInterfaceSubClass = USB_DFU_SUBCLASS_CODE,
		.bInterfaceProtocol = USB_DFU_PROTOCOL_DFU,
	},
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_DFU_CLASS_CODE,
		.bInterfaceSubClass = USB_DFU_SUBCLASS_CODE,
		.bInterfaceProtocol = USB_DFU_PROTOCOL_RUNTIME,
	},
	{ },
};

/* USB Host DFU class driver */
static usbh_class_driver_t usbh_dfu_driver = {
	.id_table = dfu_devs,
	.attach   = usbh_dfu_attach,
	.detach   = usbh_dfu_detach,
	.setup    = usbh_dfu_setup,
	.process  = usbh_dfu_process,
};

static usbh_dfu_host_t usbh_dfu_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback — parse DFU Functional Descriptor and notify application.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_attach(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_itf_data_t *itf_data;
	usbh_dev_id_t dev_id = {0,};
	u8 *buf;
	u16 remaining;
	u16 offset;

	/* Retrieve the DFU interface descriptor raw data.
	 * Always, probe Run-Time protocol (0x01) first;
	 * fall back to DFU mode (0x02) if not found. */
	dev_id.mMatchFlags       = USBH_DEV_ID_MATCH_ITF_INFO;
	dev_id.bInterfaceClass    = USB_DFU_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_DFU_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = USB_DFU_PROTOCOL_RUNTIME;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data != NULL) {
		dfu->is_runtime = 1U;
	} else {
		dev_id.bInterfaceProtocol = USB_DFU_PROTOCOL_DFU;
		itf_data = usbh_get_interface_descriptor(host, &dev_id);
		dfu->is_runtime = 0U;
	}
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get DFU itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	/*
	 * Scan the raw interface descriptor data for the DFU Functional Descriptor
	 * (bDescriptorType == 0x21).  The raw data starts at the interface descriptor
	 * itself and includes all class-specific and endpoint descriptors that follow.
	 */
	buf       = itf_data->raw_data;
	remaining = itf_data->raw_data_len;
	offset    = 0U;

	usb_os_memset(&dfu->func_desc, 0, sizeof(dfu->func_desc));

	while (offset + 2U <= remaining) {
		u8 desc_len  = buf[offset];
		u8 desc_type = buf[offset + 1U];

		if (desc_len == 0U) {
			break;
		}

		if (desc_type == USB_DFU_FUNC_DESC_TYPE) {
			if ((u16)(offset + USB_DFU_FUNC_DESC_SIZE) <= remaining) {
				dfu->func_desc.bmAttributes   = buf[offset + 2U];
				dfu->func_desc.wDetachTimeOut = (u16)(buf[offset + 3U] | ((u16)buf[offset + 4U] << 8));
				dfu->func_desc.wTransferSize  = (u16)(buf[offset + 5U] | ((u16)buf[offset + 6U] << 8));
				dfu->func_desc.bcdDFUVersion  = (u16)(buf[offset + 7U] | ((u16)buf[offset + 8U] << 8));
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "DFU func: attr=0x%02x detach=%u xferSize=%u ver=0x%04x\n",
						 dfu->func_desc.bmAttributes,
						 dfu->func_desc.wDetachTimeOut,
						 dfu->func_desc.wTransferSize,
						 dfu->func_desc.bcdDFUVersion);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU func desc truncated\n");
			}
			break;
		}

		offset += desc_len;
	}

	/* Clamp wTransferSize to the driver's internal buffer limit */
	if ((dfu->func_desc.wTransferSize == 0U) ||
		(dfu->func_desc.wTransferSize > USBH_DFU_XFER_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Clamp wTransferSize %u -> %u\n",
				 dfu->func_desc.wTransferSize, (u16)USBH_DFU_XFER_SIZE);
		dfu->func_desc.wTransferSize = USBH_DFU_XFER_SIZE;
	}

	dfu->host      = host;
	dfu->block_num = 0U;

	if (dfu->is_runtime) {
		/* DFU 1.1 §5 Reconfiguration Phase: send DFU_DETACH via process() to
		 * switch the device from Run-Time mode to DFU mode.
		 * cb->attach() is NOT called yet; it will be called after the device
		 * re-enumerates with Protocol=0x02. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Run-Time DFU device: starting reconfiguration\n");
		dfu->state = USBH_DFU_STATE_RECONFIGURE;
		usbh_notify_class_state_change(host, 0);
		return HAL_OK;
	}
	/* DFU mode device (Protocol=0x02): may be a fresh DFU-only device or a
	 * Run-Time device that completed reconfiguration and re-enumerated. */
	dfu->reconf_pending = 0U;

	dfu->state = USBH_DFU_STATE_IDLE;
	if ((dfu->cb != NULL) && (dfu->cb->attach != NULL)) {
		dfu->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_detach(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	UNUSED(host);

	dfu->state        = USBH_DFU_STATE_IDLE;
	dfu->xfer_pending = 0U;
	dfu->host         = NULL;

	if (dfu->reconf_pending) {
		/* Disconnect is part of the Run-Time → DFU reconfiguration sequence;
		 * suppress cb->detach() since the application never received cb->attach(). */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Run-Time reconfiguration: waiting for DFU re-enumeration\n");
		return HAL_OK;
	}

	if ((dfu->cb != NULL) && (dfu->cb->detach != NULL)) {
		dfu->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback — nothing class-specific needed.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_setup(usb_host_t *host)
{
	UNUSED(host);
	return HAL_OK;
}

/**
  * @brief  Send DFU_GETSTATUS request (D2H, 6-byte response).
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_process_getstatus(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_GETSTATUS;
	setup.req.wValue        = 0U;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = USB_DFU_STATUS_PKT_SIZE;

	return usbh_ctrl_request(host, &setup, dfu->xfer_buf);
}

/**
  * @brief  Send DFU_CLRSTATUS request (H2D, wLength=0).
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_process_clrstatus(usb_host_t *host)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_CLRSTATUS;
	setup.req.wValue        = 0U;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Send DFU_DNLOAD request (H2D, firmware data block).
  * @param  host:      Host handle
  * @param  block_num: Block number (wValue)
  * @param  buf:       Data to send (may be NULL for zero-length final block)
  * @param  len:       Byte count (0 for final block)
  * @retval Status
  */
static int usbh_dfu_process_dnload(usb_host_t *host, u16 block_num, u8 *buf, u16 len)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_DNLOAD;
	setup.req.wValue        = block_num;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = len;

	return usbh_ctrl_request(host, &setup, buf);
}

/**
  * @brief  Send DFU_UPLOAD request (D2H, read one firmware block from device).
  * @param  host:      Host handle
  * @param  block_num: Block number (wValue)
  * @param  buf:       Receive buffer
  * @param  len:       Requested byte count
  * @retval Status
  */
static int usbh_dfu_process_upload(usb_host_t *host, u16 block_num, u8 *buf, u16 len)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_UPLOAD;
	setup.req.wValue        = block_num;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = len;

	return usbh_ctrl_request(host, &setup, buf);
}

/**
  * @brief  Send DFU_ABORT request (H2D, wLength=0).
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_process_abort(usb_host_t *host)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_ABORT;
	setup.req.wValue        = 0U;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}


/**
  * @brief  Send DFU_DETACH request (H2D, wValue=wDetachTimeOut, wLength=0).
  *         Valid only while device is in Run-Time appIDLE state.
  * @param  host:    Host handle
  * @param  timeout: wDetachTimeOut from the DFU Functional Descriptor (ms)
  * @retval Status
  */
static int usbh_dfu_process_detach(usb_host_t *host, u16 timeout)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_DETACH;
	setup.req.wValue        = timeout;
	setup.req.wIndex        = 0U;
	setup.req.wLength       = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  State machine handling callback — drives the DFU protocol.
  * @param  host:  Host handle
  * @param  event: USB host event (unused — DFU uses EP0 only)
  * @retval HAL_OK when IDLE/DONE/ERROR; HAL_BUSY while a transfer sequence is active
  */
static int usbh_dfu_process(usb_host_t *host, usbh_event_t *event)
{
	int status     = HAL_BUSY;
	int req_status = HAL_OK;
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	u8 *buf = dfu->xfer_buf;
	int block_len;

	UNUSED(event);

	switch (dfu->state) {

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_IDLE:
		status = HAL_OK;
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_RECONFIGURE:
		/*
		 * DFU 1.1 §5 Reconfiguration Phase:
		 * Send DFU_DETACH to the Run-Time device so it switches to DFU mode.
		 * bitWillDetach=1 (our device): the device performs bus detach/attach
		 *   itself; the host simply waits for the natural re-enumeration.
		 * bitWillDetach=0: the host must issue a USB reset, but no public bus-
		 *   reset API is available at the class-driver level — log a warning.
		 */
		req_status = usbh_dfu_process_detach(host, dfu->func_desc.wDetachTimeOut);
		if (req_status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "DFU_DETACH sent (wDetachTimeOut=%u ms)\n",
					 dfu->func_desc.wDetachTimeOut);
			if (dfu->func_desc.bmAttributes & USB_DFU_ATTR_WILL_DETACH) {
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "bitWillDetach=1: waiting for device to re-enumerate in DFU mode\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN,
						 "bitWillDetach=0: USB reset required — trigger externally or power cycle\n");
			}
			/* Mark that the next detach event is part of reconfiguration so
			 * usbh_dfu_detach() can suppress the spurious cb->detach() call. */
			dfu->reconf_pending = 1U;
			dfu->state = USBH_DFU_STATE_IDLE;
			status = HAL_OK;
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU_DETACH failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_GET_STATUS:
		/*
		 * usbh_ctrl_request() returns HAL_BUSY on the first call (transfer
		 * initiated) and again on each intermediate call while the control
		 * transfer is progressing through its SETUP/DATA/STATUS phases.
		 * Only when all phases complete does it return HAL_OK.
		 * Do not treat HAL_BUSY as an error — the CTRL_EVENT and URB_EVENT
		 * interrupts will keep re-invoking process() until the transfer
		 * finishes; no extra notify is needed in the BUSY case.
		 */
		req_status = usbh_dfu_process_getstatus(host);
		if (req_status == HAL_OK) {
			/* Parse the 6-byte GETSTATUS response */
			dfu->dev_status    = buf[0];
			dfu->poll_timeout  = (u32)buf[1] |
								 ((u32)buf[2] << 8) |
								 ((u32)buf[3] << 16);
			dfu->dev_state     = buf[4];

			RTK_LOGS(TAG, RTK_LOG_DEBUG,
					 "GETSTATUS: status=0x%02x state=%u pollT=%u\n",
					 dfu->dev_status, dfu->dev_state, dfu->poll_timeout);

			switch (dfu->dev_state) {
			case USB_DFU_STATE_DFU_IDLE:
				/* Device is idle — start the requested operation */
				if (dfu->is_download) {
					dfu->state = USBH_DFU_STATE_DNLOAD_BLOCK;
				} else {
					dfu->state = USBH_DFU_STATE_UPLOAD_BLOCK;
				}
				break;

			case USB_DFU_STATE_DNLOAD_SYNC:
			case USB_DFU_STATE_DNBUSY:
				/* Device is still programming — wait then check again */
				dfu->state = USBH_DFU_STATE_DNLOAD_POLL;
				break;

			case USB_DFU_STATE_DNLOAD_IDLE:
				/* Block confirmed written: update resume checkpoint.
				 * Guard against u16 wrap when block_num is 0 (0-1 = 0xFFFF collides
				 * with the "no block confirmed yet" sentinel). */
				dfu->last_good_block = (dfu->block_num > 0U) ? (dfu->block_num - 1U) : 0U;
				dfu->retry_cnt       = 0U;
				dfu->state = USBH_DFU_STATE_DNLOAD_BLOCK;
				break;

			case USB_DFU_STATE_UPLOAD_IDLE:
				/* Device ACKed the last upload block — request the next one */
				dfu->state = USBH_DFU_STATE_UPLOAD_BLOCK;
				break;

			case USB_DFU_STATE_MANIFEST_SYNC:
			case USB_DFU_STATE_MANIFEST:
				/* Device is manifesting the firmware */
				dfu->state = USBH_DFU_STATE_MANIFEST_POLL;
				break;

			case USB_DFU_STATE_ERROR:
				/* Device error — clear it before retrying */
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Device in error state (status=0x%02x), clearing\n",
						 dfu->dev_status);
				dfu->state = USBH_DFU_STATE_CLR_STATUS;
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Unexpected dev_state %u, aborting\n", dfu->dev_state);
				dfu->state = USBH_DFU_STATE_ERROR;
				break;
			}
			usbh_notify_class_state_change(host, 0);
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "GETSTATUS failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		/* HAL_BUSY: ctrl transfer in flight — CTRL/URB event re-triggers process() */
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_CLR_STATUS:
		req_status = usbh_dfu_process_clrstatus(host);
		if (req_status == HAL_OK) {
			if (++dfu->retry_cnt > USBH_DFU_MAX_RETRY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Max retries (%u) at blk %u, aborting\n",
						 USBH_DFU_MAX_RETRY,
						 (u32)(dfu->last_good_block + 1U));
				dfu->state = USBH_DFU_STATE_ERROR;
			} else {
				/* Resume from first unconfirmed block (0xFFFF+1=0 for fresh start) */
				dfu->block_num = dfu->last_good_block + 1U;
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "Retry %u/%u from blk %u\n",
						 dfu->retry_cnt, USBH_DFU_MAX_RETRY, (u32)dfu->block_num);
				dfu->state = dfu->is_download ? USBH_DFU_STATE_DNLOAD_BLOCK
							 : USBH_DFU_STATE_UPLOAD_BLOCK;
			}
			usbh_notify_class_state_change(host, 0);
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CLRSTATUS failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_DNLOAD_BLOCK:
		if (dfu->cb == NULL || dfu->cb->get_block == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "No get_block callback\n");
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
			break;
		}

		/*
		 * Only call get_block() when no ctrl transfer is already in
		 * flight.  Once the DMA buffer (xfer_buf) has been handed to the
		 * hardware, it must not be overwritten until the transfer
		 * completes.  xfer_pending tracks this: it is set to 1 when
		 * the first usbh_ctrl_request() call returns HAL_BUSY and cleared
		 * when the transfer finishes (HAL_OK) or fails.
		 */
		if (!dfu->xfer_pending) {
			block_len = dfu->cb->get_block(dfu->block_num, buf,
										   (u32)dfu->func_desc.wTransferSize);
			if (block_len < 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "get_block() error (%d) at blk %u\n",
						 block_len, (u32)dfu->block_num);
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify_class_state_change(host, 0);
				break;
			}
			if (block_len == 0) {
				/* End of firmware — send zero-length final DNLOAD */
				dfu->state = USBH_DFU_STATE_DNLOAD_FINAL;
				usbh_notify_class_state_change(host, 0);
				break;
			}
			dfu->xfer_len = (u16)block_len;
		}

		req_status = usbh_dfu_process_dnload(host, dfu->block_num,
											 buf, dfu->xfer_len);
		if (req_status == HAL_OK) {
			dfu->xfer_pending = 0U;
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DNLOAD block %u (%u B) OK\n",
					 dfu->block_num, (u32)dfu->xfer_len);
			dfu->block_num++;
			dfu->state = USBH_DFU_STATE_GET_STATUS;
			usbh_notify_class_state_change(host, 0);
		} else if (req_status == HAL_BUSY) {
			dfu->xfer_pending = 1U;
			/* ctrl transfer in flight — CTRL/URB event re-triggers process() */
		} else {
			dfu->xfer_pending = 0U;
			RTK_LOGS(TAG, RTK_LOG_ERROR,
					 "DNLOAD block %u failed (%d)\n", dfu->block_num, req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_DNLOAD_POLL:
		/* Wait the device-reported poll timeout then re-query status */
		if (dfu->poll_timeout > 0U) {
			usb_os_sleep_ms(dfu->poll_timeout);
		}
		dfu->state = USBH_DFU_STATE_GET_STATUS;
		usbh_notify_class_state_change(host, 0);
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_DNLOAD_FINAL:
		/* Send zero-length DNLOAD to indicate end-of-firmware */
		req_status = usbh_dfu_process_dnload(host, dfu->block_num, NULL, 0U);
		if (req_status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DNLOAD final (block %u) OK\n",
					 dfu->block_num);
			dfu->state = USBH_DFU_STATE_MANIFEST_POLL;
			dfu->retry_cnt = 0U;
			dfu->manifest_retry_cnt = 0U;
			usbh_notify_class_state_change(host, 0);
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "DNLOAD final failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_MANIFEST_POLL:
		req_status = usbh_dfu_process_getstatus(host);
		if (req_status == HAL_OK) {
			dfu->dev_status   = buf[0];
			dfu->poll_timeout = (u32)buf[1] |
								((u32)buf[2] << 8) |
								((u32)buf[3] << 16);
			dfu->dev_state    = buf[4];

			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "MANIFEST_POLL: status=0x%02x state=%u pollT=%u\n",
					 dfu->dev_status, dfu->dev_state, dfu->poll_timeout);

			if (dfu->dev_state == USB_DFU_STATE_DFU_IDLE) {
				/* Manifestation complete — firmware has been accepted */
				dfu->state = USBH_DFU_STATE_DONE;
			} else if (dfu->dev_state == USB_DFU_STATE_MANIFEST) {
				/* Still manifesting — wait poll_timeout and retry */
				if (dfu->poll_timeout > 0U) {
					usb_os_sleep_ms(dfu->poll_timeout);
				}
				if (++dfu->manifest_retry_cnt > USBH_DFU_MAX_MANIFEST_RETRY) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Manifest timeout after %u retries\n",
							 dfu->manifest_retry_cnt);
					dfu->state = USBH_DFU_STATE_ERROR;
				}
				/* Stay in MANIFEST_POLL */
			} else if (dfu->dev_state == USB_DFU_STATE_MANIFEST_SYNC) {
				/* Manifestation complete (TOL=1): device in MANIFEST-SYNC
				 * waiting for another GETSTATUS to transition to dfuIDLE.
				 * Do not count against retry limit. */
				/* Stay in MANIFEST_POLL to re-issue GETSTATUS */
			} else if (dfu->dev_state == USB_DFU_STATE_MANIFEST_WAIT_RESET) {
				/* DFU 1.1 §7: device is in dfuMANIFEST-WAIT-RESET.
				 * bitWillDetach=1: device generates its own detach-attach sequence.
				 * bitWillDetach=0: host must issue a USB reset (no public API available
				 *                  at class-driver level; application must trigger a
				 *                  bus reset or power cycle externally). */
				if (dfu->func_desc.bmAttributes & USB_DFU_ATTR_WILL_DETACH) {
					RTK_LOGS(TAG, RTK_LOG_INFO,
							 "MANIFEST-WAIT-RESET: device will self-detach\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN,
							 "MANIFEST-WAIT-RESET: USB reset required (bitWillDetach=0) — "
							 "trigger bus reset via application or power cycle\n");
				}
				dfu->state = USBH_DFU_STATE_DONE;
			} else if (dfu->dev_state == USB_DFU_STATE_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Device error during manifest (status=0x%02x)\n",
						 dfu->dev_status);
				dfu->state = USBH_DFU_STATE_ERROR;
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Unexpected state %u during manifest\n", dfu->dev_state);
				dfu->state = USBH_DFU_STATE_ERROR;
			}
			usbh_notify_class_state_change(host, 0);
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR,
					 "GETSTATUS during manifest failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_UPLOAD_BLOCK:
		/*
		 * UPLOAD_BLOCK has two sequential ctrl transfers per block:
		 *   phase 0 — DFU_UPLOAD (D2H, receives one firmware block)
		 *   phase 1 — DFU_GETSTATUS (D2H, checks device state for EOF)
		 * upload_phase tracks which ctrl transfer is currently in flight.
		 * When a transfer returns HAL_BUSY the field is left unchanged and
		 * subsequent CTRL/URB events will re-invoke process() to advance
		 * the ctrl state machine until HAL_OK is returned.
		 */
		if (dfu->upload_phase == 0U) {
			req_status = usbh_dfu_process_upload(host, dfu->block_num, buf,
												 dfu->func_desc.wTransferSize);
			if (req_status == HAL_BUSY) {
				break; /* upload ctrl transfer in flight */
			}
			if (req_status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "UPLOAD block %u failed (%d)\n", dfu->block_num, req_status);
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify_class_state_change(host, 0);
				break;
			}
			/*
			 * Deliver firmware data to the application NOW — before the
			 * follow-up GETSTATUS transfer overwrites xfer_buf[0..5] with
			 * its 6-byte response.  Pass the actual received byte count so
			 * the last (short or ZLP) block is not padded with stale buffer
			 * data up to wTransferSize.
			 */
			{
				u32 actual_len = usbh_get_last_ctrl_in_size(host);
				if ((dfu->cb != NULL) && (dfu->cb->recv_block != NULL)) {
					int cb_ret = dfu->cb->recv_block(dfu->block_num, buf, actual_len);
					if (cb_ret != HAL_OK) {
						RTK_LOGS(TAG, RTK_LOG_INFO,
								 "recv_block returned %d, aborting upload\n", cb_ret);
						dfu->state = USBH_DFU_STATE_DONE;
						usbh_notify_class_state_change(host, 0);
						break;
					}
				}
				/* DFU 1.1 §6.2: a short block is the unambiguous end-of-transfer
				 * signal on the host side; no GETSTATUS needed to confirm. */
				if (actual_len < (u32)dfu->func_desc.wTransferSize) {
					dfu->block_num++;
					dfu->state = USBH_DFU_STATE_DONE;
					usbh_notify_class_state_change(host, 0);
					break;
				}
			}
			/* Advance to follow-up GETSTATUS */
			dfu->upload_phase = 1U;
		}

		/* phase 1: GETSTATUS to determine device state / EOF */
		{
			int gs_status = usbh_dfu_process_getstatus(host);
			if (gs_status == HAL_BUSY) {
				break; /* getstatus ctrl transfer in flight */
			}
			if (gs_status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "GETSTATUS after UPLOAD failed (%d)\n", gs_status);
				dfu->upload_phase = 0U;
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify_class_state_change(host, 0);
				break;
			}

			dfu->dev_status   = buf[0];
			dfu->poll_timeout = (u32)buf[1] |
								((u32)buf[2] << 8) |
								((u32)buf[3] << 16);
			dfu->dev_state    = buf[4];
			dfu->upload_phase = 0U;

			RTK_LOGS(TAG, RTK_LOG_DEBUG,
					 "UPLOAD block %u OK, dev_state=%u\n",
					 dfu->block_num, dfu->dev_state);
			dfu->block_num++;

			if (dfu->dev_state == USB_DFU_STATE_DFU_IDLE) {
				/* Last block has been transferred */
				dfu->state = USBH_DFU_STATE_DONE;
			} else if (dfu->dev_state == USB_DFU_STATE_UPLOAD_IDLE) {
				/* More data available */
				dfu->state = USBH_DFU_STATE_UPLOAD_BLOCK;
			} else if (dfu->dev_state == USB_DFU_STATE_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Device error during upload (status=0x%02x)\n",
						 dfu->dev_status);
				dfu->state = USBH_DFU_STATE_ERROR;
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Unexpected state %u during upload\n", dfu->dev_state);
				dfu->state = USBH_DFU_STATE_ERROR;
			}
			usbh_notify_class_state_change(host, 0);
		}
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_DONE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Transfer done (is_download=%u)\n",
				 dfu->is_download);
		if (dfu->is_download) {
			if ((dfu->cb != NULL) && (dfu->cb->download_done != NULL)) {
				dfu->cb->download_done(HAL_OK);
			}
		} else {
			if ((dfu->cb != NULL) && (dfu->cb->upload_done != NULL)) {
				dfu->cb->upload_done(HAL_OK);
			}
		}
		dfu->state = USBH_DFU_STATE_IDLE;
		status = HAL_OK;
		break;

	/* ------------------------------------------------------------------ */
	case USBH_DFU_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Transfer error (is_download=%u)\n",
				 dfu->is_download);
		if (dfu->is_download) {
			if ((dfu->cb != NULL) && (dfu->cb->download_done != NULL)) {
				dfu->cb->download_done(HAL_ERR_UNKNOWN);
			}
		} else {
			if ((dfu->cb != NULL) && (dfu->cb->upload_done != NULL)) {
				dfu->cb->upload_done(HAL_ERR_UNKNOWN);
			}
		}
		dfu->state = USBH_DFU_STATE_IDLE;
		status = HAL_OK;
		break;

	default:
		break;
	}

	return status;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init DFU host class.
  * @param  cb: User callback table
  * @retval Status
  */
int usbh_dfu_init(usbh_dfu_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset(dfu, 0, sizeof(usbh_dfu_host_t));
	dfu->cb = cb;

	/* Allocate the DMA-aligned transfer buffer */
	dfu->xfer_buf = (u8 *)usb_os_malloc(USBH_DFU_XFER_SIZE);
	if (dfu->xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc xfer_buf fail\n");
		return HAL_ERR_MEM;
	}
	if (!USB_IS_MEM_DMA_ALIGNED(dfu->xfer_buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "xfer_buf not DMA-aligned\n");
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_MEM;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			usb_os_mfree(dfu->xfer_buf);
			dfu->xfer_buf = NULL;
			return ret;
		}
	}

	usbh_register_class(&usbh_dfu_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit DFU host class.
  * @retval Status
  */
int usbh_dfu_deinit(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	if ((dfu->cb != NULL) && (dfu->cb->deinit != NULL)) {
		dfu->cb->deinit();
	}

	usbh_unregister_class(&usbh_dfu_driver);

	if (dfu->xfer_buf != NULL) {
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
	}

	usb_os_memset(dfu, 0, sizeof(usbh_dfu_host_t));

	return HAL_OK;
}

/**
  * @brief  Start a DFU firmware download (host -> device).
  * @retval Status
  */
int usbh_dfu_download(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usb_host_t *host = dfu->host;

	if (host == NULL || host->connect_state != USBH_STATE_SETUP) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No DFU device connected\n");
		return HAL_ERR_UNKNOWN;
	}

	if (dfu->state != USBH_DFU_STATE_IDLE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU transfer already in progress (state=%d)\n", dfu->state);
		return HAL_ERR_UNKNOWN;
	}

	if (!(dfu->func_desc.bmAttributes & USB_DFU_ATTR_CAN_DNLOAD)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device does not support DFU_DNLOAD\n");
		return HAL_ERR_PARA;
	}

	dfu->is_download     = 1U;
	dfu->block_num       = 0U;
	dfu->last_good_block = 0xFFFFU; /* sentinel: no block confirmed yet */
	dfu->retry_cnt       = 0U;
	dfu->state           = USBH_DFU_STATE_GET_STATUS;

	usbh_notify_class_state_change(host, 0);

	return HAL_OK;
}

/**
  * @brief  Start a DFU firmware upload (device -> host).
  * @retval Status
  */
int usbh_dfu_upload(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usb_host_t *host = dfu->host;

	if (host == NULL || host->connect_state != USBH_STATE_SETUP) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No DFU device connected\n");
		return HAL_ERR_UNKNOWN;
	}

	if (dfu->state != USBH_DFU_STATE_IDLE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU transfer already in progress (state=%d)\n", dfu->state);
		return HAL_ERR_UNKNOWN;
	}

	if (!(dfu->func_desc.bmAttributes & USB_DFU_ATTR_CAN_UPLOAD)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device does not support DFU_UPLOAD\n");
		return HAL_ERR_PARA;
	}

	dfu->is_download = 0U;
	dfu->block_num   = 0U;
	dfu->upload_phase = 0U;
	dfu->state       = USBH_DFU_STATE_GET_STATUS; /* confirm dfuIDLE before upload */

	usbh_notify_class_state_change(host, 0);

	return HAL_OK;
}

/**
  * @brief  Send DFU_ABORT and return host to IDLE.
  * @retval Status
  */
int usbh_dfu_abort(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usb_host_t *host = dfu->host;
	int ret;

	if (host == NULL || host->connect_state != USBH_STATE_SETUP) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No DFU device connected\n");
		return HAL_ERR_UNKNOWN;
	}

	ret = usbh_dfu_process_abort(host);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ABORT OK\n");
	} else if (ret != HAL_BUSY) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ABORT failed (%d)\n", ret);
	}
	/* HAL_BUSY means the ABORT request has been queued; the USB main task
	 * will complete the transfer.  Host state is already IDLE regardless. */

	dfu->state = USBH_DFU_STATE_IDLE;
	usbh_notify_class_state_change(host, 0);

	return (ret == HAL_BUSY) ? HAL_OK : ret;
}

/**
  * @brief  Get the DFU device's bmAttributes from the functional descriptor.
  * @retval bmAttributes byte, or 0 if no device is connected.
  */
u8 usbh_dfu_get_attr(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	if (dfu->host == NULL) {
		return 0U;
	}

	return dfu->func_desc.bmAttributes;
}
