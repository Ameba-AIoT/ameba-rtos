/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_dfu.h"

/* Private defines -----------------------------------------------------------*/

/* DFU 1.0 Functional Descriptor length: the DFU 1.1 one without bcdDFUVersion */
#define USBH_DFU_FUNC_DESC_SIZE_V10         7U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_dfu_attach(usb_host_t *host);
static void usbh_dfu_detach(usb_host_t *host);
static int usbh_dfu_setup(usb_host_t *host);
static void usbh_dfu_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_dfu_process_getstatus(usb_host_t *host);
static int usbh_dfu_process_clrstatus(usb_host_t *host);
static int usbh_dfu_process_dnload(usb_host_t *host, u16 block_num, u8 *buf, u16 len);
static int usbh_dfu_process_upload(usb_host_t *host, u16 block_num, u8 *buf, u16 len);
static int usbh_dfu_process_abort(usb_host_t *host);
static int usbh_dfu_process_detach(usb_host_t *host, u16 timeout);
static void usbh_dfu_finish(usbh_dfu_host_t *dfu, int status);
static int usbh_dfu_ctrl_request(usb_host_t *host, usbh_setup_req_t *setup, u8 *buf);

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
static const usbh_class_driver_t usbh_dfu_driver = {
	.id_table = dfu_devs,
	.attach   = usbh_dfu_attach,
	.detach   = usbh_dfu_detach,
	.setup    = usbh_dfu_setup,
	.process  = usbh_dfu_process,
};

static usbh_dfu_host_t usbh_dfu_host;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Publish the host handle to the API, once the class state it guards is set
  * @note   Taken under the critical section so the state stored before is visible to an API caller on the other
  *         core first.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_dfu_publish_host(usb_host_t *host)
{
	usb_os_enter_critical();
	usbh_dfu_host.host = host;
	usb_os_exit_critical();
}

/**
  * @brief  Send one DFU class request and track whether it is still in flight
  * @note   Every DFU request goes through here, so ctrl_pending tells whether EP0 still carries a request of this
  *         class: a new request, DFU_ABORT included, may only be started while it is 0. The core resumes an
  *         in-flight request whatever setup is passed, so sending another one meanwhile would only poll the
  *         old one. While it is 1 the DMA buffer of that request must not be touched either.
  * @param  host: Host handle
  * @param  setup: Request
  * @param  buf: Data stage buffer
  * @retval Status of usbh_ctrl_request()
  */
static int usbh_dfu_ctrl_request(usb_host_t *host, usbh_setup_req_t *setup, u8 *buf)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	int ret = usbh_ctrl_request(host, setup, buf);

	dfu->ctrl_pending = (ret == -RTK_ERR_BUSY) ? 1U : 0U;

	return ret;
}

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
		return RTK_FAIL;
	}
	dfu->itf_num = itf_data->itf_desc_array[0].bInterfaceNumber;

	/*
	 * Scan the raw interface descriptor data for the DFU Functional Descriptor
	 * (bDescriptorType == 0x21).  The raw data starts at the interface descriptor
	 * itself and includes all class-specific and endpoint descriptors that follow.
	 */
	buf       = itf_data->raw_data;
	remaining = itf_data->raw_data_len;
	offset    = 0U;

	usb_os_memset((void *)&dfu->func_desc, 0, sizeof(dfu->func_desc));

	while (offset + 2U <= remaining) {
		u8 desc_len  = buf[offset];
		u8 desc_type = buf[offset + 1U];

		if (desc_len == 0U) {
			break;
		}

		if (desc_type == USB_DFU_FUNC_DESC_TYPE) {
			/* Parse within the descriptor's own bLength, not just the raw data: DFU 1.1 4.1.3
			 * fixes it to 9, a DFU 1.0 device sends 7 and has no bcdDFUVersion. */
			if ((desc_len >= USBH_DFU_FUNC_DESC_SIZE_V10) && ((u16)(offset + desc_len) <= remaining)) {
				dfu->func_desc.bmAttributes   = buf[offset + 2U];
				dfu->func_desc.wDetachTimeOut = (u16)(buf[offset + 3U] | ((u16)buf[offset + 4U] << 8));
				dfu->func_desc.wTransferSize  = (u16)(buf[offset + 5U] | ((u16)buf[offset + 6U] << 8));
				if (desc_len >= USB_DFU_FUNC_DESC_SIZE) {
					dfu->func_desc.bcdDFUVersion = (u16)(buf[offset + 7U] | ((u16)buf[offset + 8U] << 8));
				} else {
					dfu->func_desc.bcdDFUVersion = 0x0100U;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "DFU func: attr=0x%02x detach=%u xferSize=%u ver=0x%04x\n",
						 dfu->func_desc.bmAttributes,
						 dfu->func_desc.wDetachTimeOut,
						 dfu->func_desc.wTransferSize,
						 dfu->func_desc.bcdDFUVersion);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad DFU func desc len %u\n", desc_len);
			}
			break;
		}

		offset += desc_len;
	}

	/* Clamp wTransferSize to the driver's internal buffer limit */
	if ((dfu->func_desc.wTransferSize == 0U) ||
		(dfu->func_desc.wTransferSize > dfu->xfer_size)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Clamp wTransferSize %u -> %u\n",
				 dfu->func_desc.wTransferSize, (u16)dfu->xfer_size);
		dfu->func_desc.wTransferSize = (u16)dfu->xfer_size;
	}

	dfu->block_num    = 0U;
	dfu->ctrl_pending = 0U;
	dfu->xfer_active  = 0U;
	dfu->abort_req    = 0U;

	if (dfu->is_runtime != 0U) {
		/* DFU 1.1 §5 Reconfiguration Phase: send DFU_DETACH via process() to
		 * switch the device from Run-Time mode to DFU mode.
		 * cb->attached() is NOT called yet; it will be called after the device
		 * re-enumerates with Protocol=0x02. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Run-Time DFU device: starting reconfiguration\n");
		/* Mark the reconfiguration in progress here, not after DFU_DETACH succeeds:
		 * cb->attached() is deliberately not called for a Run-Time interface, so a
		 * disconnect from now until the device re-enumerates in DFU mode must not
		 * deliver cb->detached() either (DFU 1.1 5). The DFU_DETACH request needs
		 * several process() calls to retire and the device may drop off the bus
		 * inside that window. */
		dfu->reconf_pending = 1U;
		dfu->state = USBH_DFU_STATE_RECONFIGURE;
		usbh_dfu_publish_host(host);
		usbh_notify(host, 0, &usbh_dfu_driver);
		return RTK_SUCCESS;
	}
	/* DFU mode device (Protocol=0x02): may be a fresh DFU-only device or a
	 * Run-Time device that completed reconfiguration and re-enumerated. */
	dfu->reconf_pending = 0U;

	dfu->state = USBH_DFU_STATE_IDLE;
	usbh_dfu_publish_host(host);
	if ((dfu->cb != NULL) && (dfu->cb->attached != NULL)) {
		dfu->cb->attached();
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_dfu_detach(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	UNUSED(host);

	/* Under the lock the API claims a transfer with, so a download()/upload() racing this either
	 * was accepted before (and is failed below) or sees no device */
	usb_os_enter_critical();
	dfu->host = NULL;
	usb_os_exit_critical();

	dfu->ctrl_pending = 0U;
	dfu->abort_req    = 0U;
	dfu->upload_phase = 0U;
	dfu->poll_remain  = 0U; /* drop a sliced wait owed to a device that is gone */

	/* A transfer the device left mid-way still owes its done callback, else the application only
	 * learns of the failure from its own timeout */
	if (dfu->xfer_active != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Detach in transfer\n");
		usbh_dfu_finish(dfu, RTK_FAIL);
	}
	dfu->state = USBH_DFU_STATE_IDLE;

	if (dfu->reconf_pending != 0U) {
		/* Disconnect is part of the Run-Time → DFU reconfiguration sequence;
		 * suppress cb->detached() since the application never received cb->attached(). */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Run-Time reconfiguration: waiting for DFU re-enumeration\n");
		return;
	}

	if ((dfu->cb != NULL) && (dfu->cb->detached != NULL)) {
		dfu->cb->detached();
	}
}

/**
  * @brief  Standard control requests handling callback — nothing class-specific needed.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_setup(usb_host_t *host)
{
	UNUSED(host);
	return RTK_SUCCESS;
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
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = USB_DFU_STATUS_PKT_SIZE;

	return usbh_dfu_ctrl_request(host, &setup, dfu->xfer_buf);
}

/**
  * @brief  Validate and parse the DFU_GETSTATUS response held in xfer_buf.
  * @note   DFU 1.1 6.1.2: the GETSTATUS payload is always 6 bytes. A device answering
  *         short would leave xfer_buf holding stale firmware bytes, which must never be
  *         parsed as dev_state/bwPollTimeout. bwPollTimeout is the minimum wait before the
  *         next GETSTATUS and is never shortened: a value past USBH_DFU_MAX_POLL_TIMEOUT
  *         fails the response instead, see that define.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_parse_status(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	const u8 *buf = dfu->xfer_buf;
	u32 len = usbh_get_last_ctrl_in_size(host);
	u32 poll;

	if (len != USB_DFU_STATUS_PKT_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad GETSTATUS len %u\n", len);
		return -RTK_ERR_BADARG;
	}

	poll = (u32)buf[1] | ((u32)buf[2] << 8) | ((u32)buf[3] << 16);
	if (poll > USBH_DFU_MAX_POLL_TIMEOUT) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "PollT %u > %u\n", poll, USBH_DFU_MAX_POLL_TIMEOUT);
		return -RTK_ERR_BADARG;
	}

	dfu->dev_status   = buf[0];
	dfu->poll_timeout = poll;
	dfu->dev_state    = buf[4];

	return RTK_SUCCESS;
}

/**
  * @brief  Arm a bwPollTimeout wait, to be served in slices by usbh_dfu_poll_wait().
  * @note   DFU 1.1 6.1.2 makes bwPollTimeout the *minimum* interval between two
  *         DFU_GETSTATUS requests; it does not require one continuous sleep, so slicing
  *         the wait keeps the protocol timing while releasing the shared host task.
  *         The wait is measured against a timestamp taken here rather than by summing
  *         slice lengths: an OS sleep may return up to one tick early, and a device polled
  *         inside dfuDNBUSY stalls the request and enters dfuERROR (DFU 1.1 A.2.5).
  * @param  dfu: DFU host instance
  * @param  timeout: Device-reported bwPollTimeout in ms, already bounded by parse_status
  * @retval None
  */
static void usbh_dfu_poll_arm(usbh_dfu_host_t *dfu, u32 timeout)
{
	dfu->poll_start_ms = usb_os_get_timestamp_ms();
	dfu->poll_remain = (timeout > USBH_DFU_POLL_MIN_MS) ? timeout : USBH_DFU_POLL_MIN_MS;
}

/**
  * @brief  Get how long the armed bwPollTimeout wait still has to run.
  * @note   One ms more than the plain difference: both timestamps are truncated to whole
  *         ms, so an elapsed reading equal to the timeout may still be up to 1 ms short.
  * @param  dfu: DFU host instance
  * @retval Remaining wait in ms, 0 once the wait has fully elapsed.
  */
static u32 usbh_dfu_poll_left(const usbh_dfu_host_t *dfu)
{
	u32 elapsed = usb_os_get_timestamp_ms() - dfu->poll_start_ms;

	return (elapsed > dfu->poll_remain) ? 0U : (dfu->poll_remain - elapsed + 1U);
}

/**
  * @brief  Serve one slice of a pending bwPollTimeout wait.
  * @note   Sleeps at most USBH_DFU_POLL_SLICE_MS, then either reports the wait complete or
  *         re-posts a driver message so the remaining slices run on later process() calls.
  *         Self-notifying is what keeps the wait alive: in CLASS_READY the core's watchdog
  *         message is routed by pipe and EP0 carries no class owner, so a class that
  *         returns without notifying has no other wake source.
  * @param  host: Host handle
  * @retval 1 wait still in progress - caller must stay in its current state and return;
  *         0 no wait pending, or the wait just completed.
  */
static u8 usbh_dfu_poll_wait(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	u32 left;

	if (dfu->poll_remain == 0U) {
		return 0U;
	}

	left = usbh_dfu_poll_left(dfu);
	if (left > 0U) {
		/* A short bwPollTimeout must not be stretched to a whole slice: it is paid once per
		 * block, so the excess would add up over a large image. */
		usb_os_sleep_ms((left > USBH_DFU_POLL_SLICE_MS) ? USBH_DFU_POLL_SLICE_MS : left);
		if (usbh_dfu_poll_left(dfu) > 0U) {
			usbh_notify(host, 0, &usbh_dfu_driver);
			return 1U;
		}
	}

	dfu->poll_remain = 0U;
	return 0U;
}

/**
  * @brief  Send DFU_CLRSTATUS request (H2D, wLength=0).
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_process_clrstatus(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_CLRSTATUS;
	setup.req.wValue        = 0U;
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = 0U;

	return usbh_dfu_ctrl_request(host, &setup, NULL);
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
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_DNLOAD;
	setup.req.wValue        = block_num;
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = len;

	return usbh_dfu_ctrl_request(host, &setup, buf);
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
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_UPLOAD;
	setup.req.wValue        = block_num;
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = len;

	return usbh_dfu_ctrl_request(host, &setup, buf);
}

/**
  * @brief  Send DFU_ABORT request (H2D, wLength=0).
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_dfu_process_abort(usb_host_t *host)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_ABORT;
	setup.req.wValue        = 0U;
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = 0U;

	return usbh_dfu_ctrl_request(host, &setup, NULL);
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
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest      = USB_DFU_REQ_DETACH;
	setup.req.wValue        = timeout;
	setup.req.wIndex        = dfu->itf_num;
	setup.req.wLength       = 0U;

	return usbh_dfu_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  End the current download or upload and report it to the application.
  * @note   Sets IDLE before the callback: if the application calls usbh_dfu_upload()
  *         from within download_done(), it must see IDLE to pass the state guard.
  * @param  dfu: DFU host instance
  * @param  status: RTK_SUCCESS on success, RTK_FAIL on failure
  * @retval None
  */
static void usbh_dfu_finish(usbh_dfu_host_t *dfu, int status)
{
	/* Exactly one done callback per accepted transfer: detach, abort and the state machine may all try */
	if (dfu->xfer_active == 0U) {
		dfu->state = USBH_DFU_STATE_IDLE;
		return;
	}

	usb_os_enter_critical();
	dfu->xfer_active = 0U;
	dfu->state = USBH_DFU_STATE_IDLE;
	usb_os_exit_critical();

	if (dfu->is_download != 0U) {
		if ((dfu->cb != NULL) && (dfu->cb->download_done != NULL)) {
			dfu->cb->download_done(status);
		}
	} else {
		if ((dfu->cb != NULL) && (dfu->cb->upload_done != NULL)) {
			dfu->cb->upload_done(status);
		}
	}
}

/**
  * @brief  State machine handling callback — drives the DFU protocol.
  * @param  host:  Host handle
  * @param  msg: USB host driver message (unused — DFU uses EP0 only)
  * @retval None
  */
static void usbh_dfu_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	int req_status = RTK_SUCCESS;
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	u8 *buf = dfu->xfer_buf;
	int block_len;
	u8 prev_state;

	UNUSED(msg);

	/* An abort waits for the request in flight to retire, as sending DFU_ABORT meanwhile would only poll that
	 * request, and for a bwPollTimeout owed to the device, which DFU 1.1 6.1.2 forbids cutting short. The
	 * result is dropped, a stale DNLOAD block too, and the sequence restarts from DFU_ABORT. */
	if ((dfu->abort_req != 0U) && (dfu->ctrl_pending == 0U) && (dfu->poll_remain == 0U) && (dfu->state != USBH_DFU_STATE_ABORT)) {
		/* Under the lock usbh_dfu_start() tests the state with, so no transfer is claimed in between */
		usb_os_enter_critical();
		dfu->abort_req    = 0U;
		dfu->upload_phase = 0U;
		dfu->state        = USBH_DFU_STATE_ABORT;
		usb_os_exit_critical();
	}

	switch (dfu->state) {

	case USBH_DFU_STATE_IDLE:
		break;

	case USBH_DFU_STATE_RECONFIGURE:
		/*
		 * DFU 1.1 §5 Reconfiguration Phase:
		 * Send DFU_DETACH to the Run-Time device so it switches to DFU mode.
		 * bitWillDetach=1 (our device): the device performs bus detach/attach
		 *   itself; the host simply waits for the natural re-enumeration.
		 * bitWillDetach=0: the device waits in appDETACH for a USB reset and falls
		 *   back to appIDLE if none comes within wDetachTimeOut (DFU 1.1 5.1), so
		 *   the host resets it now. The reset detaches this class from the host
		 *   task after process() returns; reconf_pending keeps that detach from
		 *   reaching the application, which is told only of the DFU-mode device.
		 */
		req_status = usbh_dfu_process_detach(host, dfu->func_desc.wDetachTimeOut);
		if (req_status == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "DFU_DETACH sent (wDetachTimeOut=%u ms)\n",
					 dfu->func_desc.wDetachTimeOut);
			if ((dfu->func_desc.bmAttributes & USB_DFU_ATTR_WILL_DETACH) != 0U) {
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "bitWillDetach=1: waiting for device to re-enumerate in DFU mode\n");
			} else {
				req_status = usbh_reenumerate();
				if (req_status != RTK_SUCCESS) {
					/* Same outcome as a device that never re-enumerates in DFU mode, see the
					 * DFU_DETACH failure branch below. */
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Reset after DFU_DETACH fail %d\n", req_status);
				}
			}
			/* reconf_pending was already set in attach(): it marks "cb->attached() not
			 * delivered", which holds from attach until the DFU-mode re-enumeration. */
			dfu->state = USBH_DFU_STATE_IDLE;
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU_DETACH failed (%d)\n", req_status);
			/* Do NOT go to USBH_DFU_STATE_ERROR: the application requested neither a
			 * download nor an upload, so that state's completion callbacks would be
			 * spurious - is_download is still 0, which would fire upload_done(). The
			 * reconfiguration simply did not happen; drop back to IDLE and leave the
			 * device in Run-Time mode. reconf_pending stays set, so a later disconnect
			 * still suppresses cb->detached(), and the application keeps waiting on
			 * cb->attached() - the same observable outcome as a device that never
			 * re-enumerated in DFU mode. */
			dfu->state = USBH_DFU_STATE_IDLE;
		}
		break;

	case USBH_DFU_STATE_ABORT:
		req_status = usbh_dfu_process_abort(host);
		if (req_status != -RTK_ERR_BUSY) {
			if (req_status == RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ABORT OK\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "ABORT failed (%d)\n", req_status);
			}
			/* A transfer cut short reports its failure, a plain abort while idle reports nothing */
			usbh_dfu_finish(dfu, RTK_FAIL);
		}
		break;

	case USBH_DFU_STATE_RECOVER:
		/* DFU 1.1 A.2.7/A.2.9: DFU_ABORT returns dfuDNLOAD-IDLE or dfuUPLOAD-IDLE to dfuIDLE, where the
		 * requested transfer starts with a fresh GETSTATUS. Bounded by the transfer retry budget. */
		req_status = usbh_dfu_process_abort(host);
		if (req_status == RTK_SUCCESS) {
			if (++dfu->retry_cnt > USBH_DFU_MAX_RETRY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Recover dev state %u fail\n", dfu->dev_state);
				dfu->state = USBH_DFU_STATE_ERROR;
			} else {
				dfu->state = USBH_DFU_STATE_GET_STATUS;
			}
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Recover ABORT failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else {
			/* In flight: the EP0 completion re-drives process() */
		}
		break;

	case USBH_DFU_STATE_GET_STATUS:
		/*
		 * usbh_ctrl_request() returns -RTK_ERR_BUSY on the first call (transfer
		 * initiated) and again on each intermediate call while the control
		 * transfer is progressing through its SETUP/DATA/STATUS phases.
		 * Only when all phases complete does it return RTK_SUCCESS.
		 * Do not treat -RTK_ERR_BUSY as an error — the CTRL and URB driver messages
		 * interrupts will keep re-invoking process() until the transfer
		 * finishes; no extra notify is needed in the BUSY case.
		 */
		req_status = usbh_dfu_process_getstatus(host);
		if (req_status == RTK_SUCCESS) {
			/* Parse the 6-byte GETSTATUS response */
			if (usbh_dfu_parse_status(host) != RTK_SUCCESS) {
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_DEBUG,
					 "GETSTATUS: status=0x%02x state=%u pollT=%u\n",
					 dfu->dev_status, dfu->dev_state, dfu->poll_timeout);

			switch (dfu->dev_state) {
			case USB_DFU_STATE_DFU_IDLE:
				/* Device is idle — start the requested operation */
				dfu->starting = 0U;
				if (dfu->is_download != 0U) {
					dfu->state = USBH_DFU_STATE_DNLOAD_BLOCK;
				} else {
					dfu->state = USBH_DFU_STATE_UPLOAD_BLOCK;
				}
				break;

			case USB_DFU_STATE_DNLOAD_SYNC:
			case USB_DFU_STATE_DNBUSY:
				/* Device is still programming — honour bwPollTimeout then check again */
				usbh_dfu_poll_arm(dfu, dfu->poll_timeout);
				dfu->state = USBH_DFU_STATE_DNLOAD_POLL;
				break;

			case USB_DFU_STATE_DNLOAD_IDLE:
				if ((dfu->starting != 0U) || (dfu->is_download == 0U)) {
					/* Left over from an earlier download that did not finish: continuing it would
					 * write this transfer's blocks into that one, or turn an upload into a download */
					RTK_LOGS(TAG, RTK_LOG_WARN, "Dev in dnload-idle, recover\n");
					dfu->state = USBH_DFU_STATE_RECOVER;
					break;
				}
				/* Block confirmed written: update resume checkpoint. wBlockNum wraps from
				 * 65535 to 0 (DFU 1.1 6.1.1), so a block_num of 0 here confirmed 65535:
				 * the u16 wrap is the right answer, and its +1 resumes at block 0. */
				dfu->last_good_block = (u16)(dfu->block_num - 1U);
				dfu->retry_cnt       = 0U;
				/* Block written: the DNBUSY poll bounds restart for the next one. */
				dfu->dnload_poll_cnt = 0U;
				dfu->dnload_poll_ms  = 0U;
				dfu->state = USBH_DFU_STATE_DNLOAD_BLOCK;
				break;

			case USB_DFU_STATE_UPLOAD_IDLE:
				/* This state is only entered to start a transfer, an upload checks each block itself in
				 * UPLOAD_BLOCK: dfuUPLOAD-IDLE here is an earlier upload left mid-way, and a transfer
				 * starting at block 0 needs the device back in dfuIDLE first */
				RTK_LOGS(TAG, RTK_LOG_WARN, "Dev in upload-idle, recover\n");
				dfu->state = (dfu->starting != 0U) ? USBH_DFU_STATE_RECOVER : USBH_DFU_STATE_ERROR;
				break;

			case USB_DFU_STATE_MANIFEST_SYNC:
			case USB_DFU_STATE_MANIFEST:
				/* Device is manifesting the firmware — honour bwPollTimeout first */
				usbh_dfu_poll_arm(dfu, dfu->poll_timeout);
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
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "GETSTATUS failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		/* -RTK_ERR_BUSY: ctrl transfer in flight — CTRL/URB msg re-triggers process() */
		break;

	case USBH_DFU_STATE_CLR_STATUS:
		req_status = usbh_dfu_process_clrstatus(host);
		if (req_status == RTK_SUCCESS) {
			if (++dfu->retry_cnt > USBH_DFU_MAX_RETRY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "Max retries (%u) at blk %u, aborting\n",
						 USBH_DFU_MAX_RETRY,
						 (u32)(dfu->last_good_block + 1U));
				dfu->state = USBH_DFU_STATE_ERROR;
			} else {
				/* DFU_CLRSTATUS leaves the device in dfuIDLE (DFU 1.1 A.2.10), so the transfer has started:
				 * a dfuDNLOAD-IDLE after the next block is its own, not one left over. Resume from first
				 * unconfirmed block (0xFFFF+1=0 for fresh start). */
				dfu->starting  = 0U;
				dfu->block_num = dfu->last_good_block + 1U;
				/* Fresh attempt at that block: restart its DNBUSY poll bounds. The
				 * overall retry count (retry_cnt) still bounds this loop. */
				dfu->dnload_poll_cnt = 0U;
				dfu->dnload_poll_ms  = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "Retry %u/%u from blk %u\n",
						 dfu->retry_cnt, USBH_DFU_MAX_RETRY, (u32)dfu->block_num);
				dfu->state = (dfu->is_download != 0U) ? USBH_DFU_STATE_DNLOAD_BLOCK
							 : USBH_DFU_STATE_UPLOAD_BLOCK;
			}
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CLRSTATUS failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		break;

	case USBH_DFU_STATE_DNLOAD_BLOCK:
		if (dfu->cb == NULL || dfu->cb->get_block == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "No get_block callback\n");
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
			break;
		}

		/*
		 * Only call get_block() when no ctrl transfer is already in
		 * flight.  Once the DMA buffer (xfer_buf) has been handed to the
		 * hardware, it must not be overwritten until the transfer
		 * completes.  ctrl_pending tracks this: it is set to 1 when
		 * the first usbh_ctrl_request() call returns -RTK_ERR_BUSY and cleared
		 * when the transfer finishes (RTK_SUCCESS) or fails.
		 */
		if (dfu->ctrl_pending == 0U) {
			block_len = dfu->cb->get_block(dfu->block_num, buf,
										   (u32)dfu->func_desc.wTransferSize);
			/* Longer than the buffer it was given: the DNLOAD would read past xfer_buf */
			if ((block_len < 0) || ((u32)block_len > (u32)dfu->func_desc.wTransferSize)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "get_block() error (%d) at blk %u\n",
						 block_len, (u32)dfu->block_num);
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}
			if (block_len == 0) {
				/* End of firmware — send zero-length final DNLOAD */
				dfu->state = USBH_DFU_STATE_DNLOAD_FINAL;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}
			dfu->xfer_len = (u16)block_len;
		}

		req_status = usbh_dfu_process_dnload(host, dfu->block_num,
											 buf, dfu->xfer_len);
		if (req_status == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "DNLOAD block %u (%u B) OK\n",
					 dfu->block_num, (u32)dfu->xfer_len);
			dfu->block_num++;
			dfu->state = USBH_DFU_STATE_GET_STATUS;
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status == -RTK_ERR_BUSY) {
			/* ctrl transfer in flight (ctrl_pending set) — CTRL/URB msg re-triggers process() */
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR,
					 "DNLOAD block %u failed (%d)\n", dfu->block_num, req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		break;

	case USBH_DFU_STATE_DNLOAD_POLL:
		/* Serve the device-reported poll timeout in slices; nothing else may run in this
		 * state until the full wait has elapsed. */
		if (usbh_dfu_poll_wait(host) != 0U) {
			break;
		}

		/* First poll for this block: start the wall-clock budget. A timestamp of exactly
		 * 0 re-reads it one poll later, which only extends the budget by one interval. */
		if (dfu->dnload_poll_ms == 0U) {
			dfu->dnload_poll_ms = usb_os_get_timestamp_ms();
		}
		dfu->dnload_poll_cnt++;

		if ((dfu->dnload_poll_cnt > USBH_DFU_MAX_DNLOAD_POLL) ||
			((usb_os_get_timestamp_ms() - dfu->dnload_poll_ms) > USBH_DFU_DNLOAD_POLL_BUDGET_MS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "DNBUSY timeout at blk %u after %u polls\n",
					 (u32)dfu->block_num, (u32)dfu->dnload_poll_cnt);
			dfu->state = USBH_DFU_STATE_ERROR;
		} else {
			dfu->state = USBH_DFU_STATE_GET_STATUS;
		}
		usbh_notify(host, 0, &usbh_dfu_driver);
		break;

	case USBH_DFU_STATE_DNLOAD_FINAL:
		/* Send zero-length DNLOAD to indicate end-of-firmware */
		req_status = usbh_dfu_process_dnload(host, dfu->block_num, NULL, 0U);
		if (req_status == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DNLOAD final (block %u) OK\n",
					 dfu->block_num);
			dfu->state = USBH_DFU_STATE_MANIFEST_POLL;
			dfu->retry_cnt = 0U;
			dfu->manifest_retry_cnt = 0U;
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "DNLOAD final failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		break;

	case USBH_DFU_STATE_MANIFEST_POLL:
		/* Serve any pending bwPollTimeout BEFORE re-issuing GETSTATUS: re-entering this
		 * state with a wait still owed must not send another request. */
		if (usbh_dfu_poll_wait(host) != 0U) {
			break;
		}

		/* The state the previous poll reported, to tell progress from a device repeating itself */
		prev_state = dfu->dev_state;
		req_status = usbh_dfu_process_getstatus(host);
		if (req_status == RTK_SUCCESS) {
			if (usbh_dfu_parse_status(host) != RTK_SUCCESS) {
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_DEBUG,
					 "MANIFEST_POLL: status=0x%02x state=%u pollT=%u\n",
					 dfu->dev_status, dfu->dev_state, dfu->poll_timeout);

			if (dfu->dev_state == USB_DFU_STATE_DFU_IDLE) {
				/* Manifestation complete — firmware has been accepted */
				dfu->state = USBH_DFU_STATE_DONE;
			} else if ((dfu->dev_state == USB_DFU_STATE_MANIFEST) || (dfu->dev_state == USB_DFU_STATE_MANIFEST_SYNC)) {
				/* Only dfuMANIFEST -> dfuMANIFEST-SYNC is progress (DFU 1.1 A.2.6/A.2.7) and restarts the
				 * count; a device repeating either state is bounded. The bound is checked before arming
				 * the wait, so an exhausted poll does not spend another bwPollTimeout before failing. */
				if ((dfu->dev_state == USB_DFU_STATE_MANIFEST_SYNC) && (prev_state == USB_DFU_STATE_MANIFEST)) {
					dfu->manifest_retry_cnt = 0U;
				}
				if (++dfu->manifest_retry_cnt > USBH_DFU_MAX_MANIFEST_RETRY) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Manifest timeout after %u retries\n",
							 dfu->manifest_retry_cnt);
					dfu->state = USBH_DFU_STATE_ERROR;
				} else {
					/* Still manifesting — wait poll_timeout and retry (stay in this state) */
					usbh_dfu_poll_arm(dfu, dfu->poll_timeout);
				}
			} else if (dfu->dev_state == USB_DFU_STATE_MANIFEST_WAIT_RESET) {
				/* DFU 1.1 §7: device is in dfuMANIFEST-WAIT-RESET.
				 * bitWillDetach=1: device generates its own detach-attach sequence.
				 * bitWillDetach=0: the new firmware only runs once the host resets the
				 *                  device, so a download is not complete without it. */
				if ((dfu->func_desc.bmAttributes & USB_DFU_ATTR_WILL_DETACH) != 0U) {
					RTK_LOGS(TAG, RTK_LOG_INFO,
							 "MANIFEST-WAIT-RESET: device will self-detach\n");
					dfu->state = USBH_DFU_STATE_DONE;
				} else {
					/* The reset is asynchronous: the host task runs it, detaching this class,
					 * before any message queued from here is dispatched. Report the result now,
					 * not through the DONE / ERROR states, or the detach would swallow it. */
					req_status = usbh_reenumerate();
					if (req_status == RTK_SUCCESS) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "MANIFEST-WAIT-RESET: reset device\n");
						usbh_dfu_finish(dfu, RTK_SUCCESS);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "MANIFEST-WAIT-RESET: reset fail %d\n", req_status);
						usbh_dfu_finish(dfu, RTK_FAIL);
					}
					break;
				}
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
			usbh_notify(host, 0, &usbh_dfu_driver);
		} else if (req_status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR,
					 "GETSTATUS during manifest failed (%d)\n", req_status);
			dfu->state = USBH_DFU_STATE_ERROR;
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		break;

	case USBH_DFU_STATE_UPLOAD_BLOCK:
		/*
		 * UPLOAD_BLOCK has two sequential ctrl transfers per block:
		 *   phase 0 — DFU_UPLOAD (D2H, receives one firmware block)
		 *   phase 1 — DFU_GETSTATUS (D2H, checks device state for EOF)
		 * upload_phase tracks which ctrl transfer is currently in flight.
		 * When a transfer returns -RTK_ERR_BUSY the field is left unchanged and
		 * subsequent CTRL/URB messages will re-invoke process() to advance
		 * the ctrl state machine until RTK_SUCCESS is returned.
		 */
		if (dfu->upload_phase == 0U) {
			req_status = usbh_dfu_process_upload(host, dfu->block_num, buf,
												 dfu->func_desc.wTransferSize);
			if (req_status == -RTK_ERR_BUSY) {
				break; /* upload ctrl transfer in flight */
			}
			if (req_status != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "UPLOAD block %u failed (%d)\n", dfu->block_num, req_status);
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
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
					if (cb_ret != RTK_SUCCESS) {
						/* The application refused the data: the upload failed, upload_done must say so */
						RTK_LOGS(TAG, RTK_LOG_ERROR,
								 "recv_block returned %d, aborting upload\n", cb_ret);
						dfu->state = USBH_DFU_STATE_ERROR;
						usbh_notify(host, 0, &usbh_dfu_driver);
						break;
					}
				}
				/* DFU 1.1 §6.2: a short block is the unambiguous end-of-transfer
				 * signal on the host side; no GETSTATUS needed to confirm. */
				if (actual_len < (u32)dfu->func_desc.wTransferSize) {
					dfu->block_num++;
					dfu->state = USBH_DFU_STATE_DONE;
					usbh_notify(host, 0, &usbh_dfu_driver);
					break;
				}
			}
			/* Advance to follow-up GETSTATUS */
			dfu->upload_phase = 1U;
		}

		/* phase 1: GETSTATUS to determine device state / EOF */
		{
			int gs_status = usbh_dfu_process_getstatus(host);
			if (gs_status == -RTK_ERR_BUSY) {
				break; /* getstatus ctrl transfer in flight */
			}
			if (gs_status != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR,
						 "GETSTATUS after UPLOAD failed (%d)\n", gs_status);
				dfu->upload_phase = 0U;
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}

			dfu->upload_phase = 0U;
			if (usbh_dfu_parse_status(host) != RTK_SUCCESS) {
				dfu->state = USBH_DFU_STATE_ERROR;
				usbh_notify(host, 0, &usbh_dfu_driver);
				break;
			}

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
			usbh_notify(host, 0, &usbh_dfu_driver);
		}
		break;

	case USBH_DFU_STATE_DONE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Transfer done (is_download=%u)\n",
				 dfu->is_download);
		usbh_dfu_finish(dfu, RTK_SUCCESS);
		break;

	case USBH_DFU_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Transfer error (is_download=%u)\n",
				 dfu->is_download);
		usbh_dfu_finish(dfu, RTK_FAIL);
		break;

	default:
		break;
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init DFU host class.
  * @param  cb: User callback table
  * @retval Status
  */
int usbh_dfu_init(const usbh_dfu_cb_t *cb, const usbh_dfu_config_t *cfg)
{
	int ret = RTK_SUCCESS;
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return -RTK_ERR_BADARG;
	}

	/* xfer_buf is only allocated by a successful init and freed by deinit: a second init would leak it and
	   register the driver twice */
	if (dfu->xfer_buf != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return -RTK_ERR_BADARG;
	}

	/* wTransferSize is a 16-bit descriptor field, so a larger buffer could never be
	   advertised by any device and would only waste heap. Checked before any field is written. */
	if (cfg->xfer_size > 0xFFFFU) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Xfer size %u exceeds u16\n", cfg->xfer_size);
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)dfu, 0, sizeof(usbh_dfu_host_t));
	dfu->cb = cb;

	/* Resolved after the memset so the default survives it, and before the allocation and
	   the attach-time wTransferSize clamp that both read it. */
	dfu->cfg = cfg;
	dfu->xfer_size = (cfg->xfer_size != 0U) ? cfg->xfer_size : (u32)USBH_DFU_DEFAULT_XFER_SIZE;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Config:\n");
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* xfer_size: %d\n", dfu->xfer_size);

	/* Allocate the DMA-aligned transfer buffer. UPLOAD and GETSTATUS are D2H requests whose data
	   stage reserves whole EP0 packets, so round it up to the EP0 MPS, refer to usbh_ctrl_request() */
	dfu->xfer_buf = (u8 *)usb_os_malloc(usb_get_dma_len(dfu->xfer_size, USB_MAX_EP0_SIZE));
	if (dfu->xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc xfer_buf fail\n");
		dfu->cb = NULL;
		dfu->cfg = NULL;
		return -RTK_ERR_NOMEM;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			goto exit_free;
		}
	}

	ret = usbh_register_class(&usbh_dfu_driver);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		goto exit_user_deinit;
	}

	return RTK_SUCCESS;

exit_user_deinit:
	if (cb->deinit != NULL) {
		cb->deinit();
	}
exit_free:
	usb_os_mfree((void *)dfu->xfer_buf);
	dfu->xfer_buf = NULL;
	dfu->cb = NULL;
	dfu->cfg = NULL;

	return ret;
}

/**
  * @brief  Deinit DFU host class.
  */
void usbh_dfu_deinit(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;

	/* First, before anything is freed: on RTK_SUCCESS no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_dfu_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	if ((dfu->cb != NULL) && (dfu->cb->deinit != NULL)) {
		dfu->cb->deinit();
	}

	usb_os_mfree((void *)dfu->xfer_buf);
	dfu->xfer_buf = NULL;

	usb_os_memset((void *)dfu, 0, sizeof(usbh_dfu_host_t));
}

/**
  * @brief  Claim the class for one download or upload
  * @note   The check and the claim are one step under the critical section the host task finishes a transfer and
  *         a detach withdraws the host with: two callers, possibly on different cores, could otherwise both see
  *         IDLE, and one of them would overwrite the other's direction and block numbers. A Run-Time device still
  *         being reconfigured, or an abort not yet served, refuse the claim too.
  * @param  is_download: 1 for a download, 0 for an upload
  * @param  attr: bmAttributes capability the transfer needs
  * @retval RTK_SUCCESS when claimed, -RTK_ERR_BADARG when the device lacks the capability, RTK_FAIL otherwise
  */
static int usbh_dfu_start(u8 is_download, u8 attr)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usb_host_t *host;
	int ret = RTK_FAIL;

	usb_os_enter_critical();
	host = dfu->host;
	if ((host != NULL) && ((dfu->func_desc.bmAttributes & attr) == 0U)) {
		ret = -RTK_ERR_BADARG;
	} else if ((host != NULL) && (dfu->reconf_pending == 0U) && (dfu->state == USBH_DFU_STATE_IDLE) &&
			   (dfu->xfer_active == 0U) && (dfu->abort_req == 0U)) {
		dfu->is_download     = is_download;
		dfu->block_num       = 0U;
		dfu->last_good_block = 0xFFFFU; /* sentinel: no block confirmed yet, a CLR_STATUS retry restarts at 0 */
		dfu->retry_cnt       = 0U;
		dfu->upload_phase    = 0U;
		dfu->poll_remain     = 0U;
		dfu->dnload_poll_cnt = 0U;
		dfu->dnload_poll_ms  = 0U;
		dfu->starting        = 1U;
		dfu->xfer_active     = 1U;
		dfu->state           = USBH_DFU_STATE_GET_STATUS; /* confirm dfuIDLE before the transfer */
		ret = RTK_SUCCESS;
	} else {
		/* No DFU-mode device, or a transfer, abort or reconfiguration in progress */
	}
	usb_os_exit_critical();

	if (ret == RTK_SUCCESS) {
		usbh_notify(host, 0, &usbh_dfu_driver);
	} else if (ret == -RTK_ERR_BADARG) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device does not support DFU_%s\n", (is_download != 0U) ? "DNLOAD" : "UPLOAD");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU busy or not connected\n");
	}

	return ret;
}

/**
  * @brief  Start a DFU firmware download (host -> device).
  * @retval Status
  */
int usbh_dfu_download(void)
{
	return usbh_dfu_start(1U, USB_DFU_ATTR_CAN_DNLOAD);
}

/**
  * @brief  Start a DFU firmware upload (device -> host).
  * @retval Status
  */
int usbh_dfu_upload(void)
{
	return usbh_dfu_start(0U, USB_DFU_ATTR_CAN_UPLOAD);
}

/**
  * @brief  Request DFU_ABORT and return host to IDLE.
  * @note   Only marks the request: process() sends DFU_ABORT once no DFU request is in flight and no bwPollTimeout
  *         is owed, see usbh_dfu_process(). Rejected for a Run-Time device being reconfigured, which has no
  *         transfer to abort and whose DFU_DETACH must not be replaced.
  * @retval Status
  */
int usbh_dfu_abort(void)
{
	usbh_dfu_host_t *dfu = &usbh_dfu_host;
	usb_host_t *host;
	int ret = RTK_FAIL;

	usb_os_enter_critical();
	host = dfu->host;
	if ((host != NULL) && (dfu->reconf_pending == 0U)) {
		dfu->abort_req = 1U;
		ret = RTK_SUCCESS;
	}
	usb_os_exit_critical();

	if (ret == RTK_SUCCESS) {
		usbh_notify(host, 0, &usbh_dfu_driver);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No DFU device connected\n");
	}

	return ret;
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
