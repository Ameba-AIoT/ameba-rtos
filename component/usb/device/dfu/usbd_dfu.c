/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_dfu.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_dfu_set_config(usb_dev_t *dev, u8 config);
static int usbd_dfu_clear_config(usb_dev_t *dev, u8 config);
static int usbd_dfu_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_dfu_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_dfu_ep0_data_in(usb_dev_t *dev, u8 status);
static int usbd_dfu_ep0_data_out(usb_dev_t *dev);
static void usbd_dfu_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void usbd_dfu_reconf_task(void *param);

static void usbd_dfu_manifest_task(void *param);
static void usbd_dfu_manifest_timer_cb(void *timer);
#if !CONFIG_USBD_DFU_WILL_DETACH
static void usbd_dfu_detach_timeout_cb(void *timer);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "DFU";

/* USB Standard Device Descriptor */
static const u8 usbd_dfu_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB 2.00 */
	0x00,                                           /* bDeviceClass: class at interface level */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_DFU_VID),                     /* idVendor */
	USB_HIGH_BYTE(USBD_DFU_VID),
	USB_LOW_BYTE(USBD_DFU_PID),                     /* idProduct */
	USB_HIGH_BYTE(USBD_DFU_PID),
	0x00, 0x02,                                     /* bcdDevice 2.00 */
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01,                                           /* bNumConfigurations */
};

/* USB Standard String Descriptor 0 */
static const u8 usbd_dfu_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_DFU_LANGID_STRING),            /* wLANGID: English */
	USB_HIGH_BYTE(USBD_DFU_LANGID_STRING),
};

#ifndef CONFIG_USB_FS
/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_dfu_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB 2.00 */
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize0 */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};
#endif

/*
 * DFU Configuration Descriptor: Config(9) + Interface(9) + DFU Func(9) = 27 bytes
 * No endpoints: DFU is EP0-only.
 */
static const u8 usbd_dfu_config_desc[USBD_DFU_CONFIG_DESC_SIZE] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USB_LOW_BYTE(USBD_DFU_CONFIG_DESC_SIZE),        /* wTotalLength */
	USB_HIGH_BYTE(USBD_DFU_CONFIG_DESC_SIZE),
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
	0x80,                                           /* bmAttributes: bus powered */
	0x32,                                           /* bMaxPower: 100 mA */

	/* USB Standard Interface Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	0x00,                                           /* bInterfaceNumber */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* bNumEndpoints: EP0 only */
	USB_DFU_CLASS_CODE,                            /* bInterfaceClass: 0xFE (Application Specific) */
	USB_DFU_SUBCLASS_CODE,                         /* bInterfaceSubClass: 0x01 (DFU) */
	USB_DFU_PROTOCOL_DFU,                          /* bInterfaceProtocol: 0x02 (DFU mode); patched
	                                                   to 0x01 in get_descriptor while in Run-Time
	                                                   mode */
	USBD_DFU_IFACE_STRING_IDX,                      /* iInterface */

	/* DFU Functional Descriptor */
	0x09,                                           /* bLength */
	USB_DFU_FUNC_DESC_TYPE,                        /* bDescriptorType: 0x21 */
#if CONFIG_USBD_DFU_WILL_DETACH
	USB_DFU_ATTR_WILL_DETACH |
#endif
#if CONFIG_USBD_DFU_MANIFESTATION_TOL
	USB_DFU_ATTR_MANIFEST_TOL |
#endif
#if CONFIG_USBD_DFU_CAN_UPLOAD
	USB_DFU_ATTR_CAN_UPLOAD |
#endif
	USB_DFU_ATTR_CAN_DNLOAD |
	0,                                             /* bmAttributes */
	USB_LOW_BYTE(USBD_DFU_DETACH_TIMEOUT),          /* wDetachTimeOut */
	USB_HIGH_BYTE(USBD_DFU_DETACH_TIMEOUT),
	USB_LOW_BYTE(USBD_DFU_XFER_SIZE),              /* wTransferSize */
	USB_HIGH_BYTE(USBD_DFU_XFER_SIZE),
	USB_LOW_BYTE(USB_DFU_BCD_VERSION),            /* bcdDFUVersion */
	USB_HIGH_BYTE(USB_DFU_BCD_VERSION),
};

/* DFU Class Driver */
static const usbd_class_driver_t usbd_dfu_driver = {
	.get_descriptor = usbd_dfu_get_descriptor,
	.set_config     = usbd_dfu_set_config,
	.clear_config   = usbd_dfu_clear_config,
	.setup          = usbd_dfu_setup,
	.ep0_data_in    = usbd_dfu_ep0_data_in,
	.ep0_data_out   = usbd_dfu_ep0_data_out,
	.status_changed = usbd_dfu_status_changed,
};

/* DFU Device */
static usbd_dfu_dev_t usbd_dfu_dev;


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Set DFU configuration (called from ISR on SET_CONFIGURATION).
 */
static int usbd_dfu_set_config(usb_dev_t *dev, u8 config)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	UNUSED(config);

	dfu->dev = dev;
	dfu->alt_setting = 0U;
	/* Save the default EP0 buffer and its capacity (shared by IN/OUT endpoints). */
	dfu->ep0_default_buf         = dev->ep0_in.xfer_buf;
	dfu->ep0_default_buf_len = dev->ep0_out.xfer_buf_len;
	dfu->manifest_done           = 0U;
	/* Run-Time set → appIDLE; DFU-mode set (after detach/re-enum) → dfuIDLE. */
	dfu->state = (dfu->mode == USB_DFU_PROTOCOL_RUNTIME)
				 ? USB_DFU_STATE_APP_IDLE : USB_DFU_STATE_DFU_IDLE;
	dfu->status = USBD_DFU_STATUS_OK;

	return HAL_OK;
}

/**
 * @brief  Clear DFU configuration (called from ISR on USB disconnect/reset).
 */
static int usbd_dfu_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	UNUSED(dev);
	UNUSED(config);

	/* Idle state of whichever descriptor set is currently active.  During the
	 * intentional detach/attach the mode is already DFU, so the re-enumeration
	 * comes up in dfuIDLE; a true cable unplug in run-time mode stays appIDLE. */
	dfu->state = (dfu->mode == USB_DFU_PROTOCOL_RUNTIME)
				 ? USB_DFU_STATE_APP_IDLE : USB_DFU_STATE_DFU_IDLE;
#if !CONFIG_USBD_DFU_WILL_DETACH
	/* USB Reset arrived before wDetachTimeOut: stop the one-shot timer early so
	 * it doesn't fire needlessly after the re-enumeration is already complete. */
	if ((dfu->mode == USB_DFU_PROTOCOL_DFU) && (dfu->detach_timer != NULL)) {
		rtos_timer_stop(dfu->detach_timer, 0U);
	}
#endif

	return HAL_OK;
}

/**
 * @brief  Fill and transmit the 6-byte GETSTATUS response.
 *         poll_timeout is in milliseconds (little-endian 3 bytes).
 */
static void usbd_dfu_send_status(usb_dev_t *dev, usbd_dfu_dev_t *dfu, u32 poll_ms)
{
	usbd_ep_t *ep0_in = &dev->ep0_in;

	ep0_in->xfer_buf[0] = (u8)dfu->status;
	ep0_in->xfer_buf[1] = (u8)(poll_ms & 0xFFU);
	ep0_in->xfer_buf[2] = (u8)((poll_ms >> 8U) & 0xFFU);
	ep0_in->xfer_buf[3] = (u8)((poll_ms >> 16U) & 0xFFU);
	ep0_in->xfer_buf[4] = (u8)dfu->state;
	ep0_in->xfer_buf[5] = 0x00U;  /* iString */
	ep0_in->xfer_len    = USB_DFU_STATUS_PKT_SIZE;
	usbd_ep_transmit(dev, ep0_in);
}

/**
 * @brief  Enter dfuERROR and signal the core to stall EP0.
 *         The core's setup() error path calls usbd_pcd_ep0_set_stall() which
 *         stalls both EP0 IN and OUT, so no direction-specific stall is needed here.
 */
static int usbd_dfu_stall_error(usb_dev_t *dev, usbd_dfu_dev_t *dfu, usbd_dfu_status_t err)
{
	UNUSED(dev);
	dfu->state  = USB_DFU_STATE_ERROR;
	dfu->status = err;
	return HAL_ERR_HW;
}

/**
 * @brief  Handle DFU class SETUP requests (called from ISR).
 */
static int usbd_dfu_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;
	usbd_ep_t *ep0_in  = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {

	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				dfu->alt_setting = USB_LOW_BYTE(req->wValue);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = dfu->alt_setting;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		default:
			ret = HAL_ERR_HW;
			break;
		}
		break;

	case USB_REQ_TYPE_CLASS:
		/* DFU 1.1 §A.2.1/§A.2.2: Run-Time state guard.
		 *   appIDLE  : only DETACH / GETSTATUS / GETSTATE are valid; all others
		 *              stall and stay in appIDLE (no dfuERROR transition).
		 *   appDETACH: only GETSTATUS / GETSTATE are valid; all others stall and
		 *              revert to appIDLE (device is about to re-enumerate). */
		if (dfu->state == USB_DFU_STATE_APP_IDLE) {
			if (req->bRequest != USB_DFU_REQ_DETACH   &&
				req->bRequest != USB_DFU_REQ_GETSTATUS &&
				req->bRequest != USB_DFU_REQ_GETSTATE) {
				ret = HAL_ERR_HW;
				break;
			}
		} else if (dfu->state == USB_DFU_STATE_APP_DETACH) {
			if (req->bRequest != USB_DFU_REQ_GETSTATUS &&
				req->bRequest != USB_DFU_REQ_GETSTATE) {
				dfu->state = USB_DFU_STATE_APP_IDLE;
				ret = HAL_ERR_HW;
				break;
			}
		}
		switch (req->bRequest) {

		case USB_DFU_REQ_DETACH:
			/* DFU_DETACH: valid only in appIDLE; transitions to appDETACH */
			if (dfu->state == USB_DFU_STATE_APP_IDLE) {
				dfu->state = USB_DFU_STATE_APP_DETACH;
				if (dfu->cb->detach != NULL) {
					dfu->cb->detach();
				}
				/* DFU 1.1 §5: Switch the descriptor set to DFU mode and defer
				 * the actual re-enumeration to reconf_task (off ISR context). */
				dfu->mode = USB_DFU_PROTOCOL_DFU;
				dfu->reconf_pending = 1U;
				usb_os_sema_give(dfu->reconf_sema);
			} else {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			}
			break;

		case USB_DFU_REQ_DNLOAD:
			if (req->wLength > 0U) {
				/* Reject oversized blocks before redirecting EP0 OUT buffer */
				if (req->wLength > USBD_DFU_XFER_SIZE) {
					ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
					break;
				}
				/* Block download */
				if (dfu->state == USB_DFU_STATE_DFU_IDLE ||
					dfu->state == USB_DFU_STATE_DNLOAD_IDLE) {
					/* Block number continuity check */
					if (dfu->state == USB_DFU_STATE_DNLOAD_IDLE) {
						u16 expected = (u16)(dfu->block_num + 1U);
						if (req->wValue != expected) {
							RTK_LOGS(TAG, RTK_LOG_ERROR, "Block num mismatch: got %u, expected %u\n",
									 req->wValue, expected);
							ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
							break;
						}
					} else {
						/* DFU 1.1 §7: first block in dfuIDLE must always use wBlockNum 0 */
						if (req->wValue != 0U) {
							RTK_LOGS(TAG, RTK_LOG_ERROR, "First DNLOAD block must be wBlockNum 0, got %u\n",
									 req->wValue);
							ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
							break;
						}
					}
					dfu->block_num = req->wValue;
					dfu->block_len = req->wLength;
					/* Redirect EP0 OUT to our block buffer; also update xfer_buf_len
					 * so usbd_ep_receive does not reject the oversized transfer. */
					ep0_out->xfer_buf     = dfu->xfer_buf;
					ep0_out->xfer_buf_len = USBD_DFU_XFER_SIZE;
					ep0_out->xfer_len     = req->wLength;
					if (usbd_ep_receive(dev, ep0_out) != HAL_OK) {
						/* ARM failed — ep0_data_out will never fire; restore buffer now */
						ep0_out->xfer_buf     = dfu->ep0_default_buf;
						ep0_out->xfer_buf_len = dfu->ep0_default_buf_len;
						ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
						break;
					}
					dfu->state = USB_DFU_STATE_DNLOAD_SYNC;
				} else {
					ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
				}
			} else {
				/* Zero-length DNLOAD = manifest trigger */
				if (dfu->state == USB_DFU_STATE_DNLOAD_IDLE) {
					dfu->state = USB_DFU_STATE_MANIFEST_SYNC;
					/* Manifest is called when host polls GETSTATUS in MANIFEST_SYNC */
				} else {
					/* DFU 1.1 Table 3: zero-length DNLOAD in dfuIDLE → stall → dfuERROR;
					 * all other states are equally invalid. */
					ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
				}
			}
			break;

		case USB_DFU_REQ_UPLOAD:
#if CONFIG_USBD_DFU_CAN_UPLOAD
			/* Reject oversized requests before using the buffer */
			if (req->wLength > USBD_DFU_XFER_SIZE) {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
				break;
			}
			if (dfu->state == USB_DFU_STATE_DFU_IDLE ||
				dfu->state == USB_DFU_STATE_UPLOAD_IDLE) {
				int len = dfu->cb->read(req->wValue, dfu->xfer_buf, req->wLength);
				if (len < 0) {
					ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_FILE);
				} else {
					/* Redirect EP0 IN to our block buffer for the upload data; also
					 * update xfer_buf_len so usbd_ep_transmit does not reject the
					 * oversized transfer.  When len==0 (UPLOAD EOF) the core sends
					 * a ZLP as DATA_IN: this is a D2H request (wLength>0) still in
					 * EP0 SETUP state, which usbd_pcd_ep0_transmit detects and routes
					 * to DATA_IN (not STATUS_IN) so the IN endpoint is enabled and
					 * the ZLP is physically sent, firing ep0_data_in to complete UPLOAD. */
					ep0_in->xfer_buf     = dfu->xfer_buf;
					ep0_in->xfer_buf_len = USBD_DFU_XFER_SIZE;
					ep0_in->xfer_len     = MIN(len, req->wLength);
					if (usbd_ep_transmit(dev, ep0_in) != HAL_OK) {
						/* ARM failed — ep0_data_in will never fire; restore buffer now */
						ep0_in->xfer_buf     = dfu->ep0_default_buf;
						ep0_in->xfer_buf_len = dfu->ep0_default_buf_len;
						ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
						break;
					}
					dfu->upload_last = (req->wLength == 0U || (u32)len < req->wLength) ? 1U : 0U;
					dfu->state = USB_DFU_STATE_UPLOAD_IDLE;
				}
			} else {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			}
#else
			/* DFU 1.1 Table 3: bitCanUpload=0 → reject all UPLOAD, stall → dfuERROR */
			ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
#endif /* CONFIG_USBD_DFU_CAN_UPLOAD */
			break;

		case USB_DFU_REQ_GETSTATUS:
			if (dfu->state == USB_DFU_STATE_MANIFEST_SYNC) {
				/* Async manifest (§6.1.2): first GETSTATUS after zero-length
				 * DNLOAD wakes manifest_task and enters MANIFEST; subsequent
				 * polls wait for manifest_done (set by the timer callback). */
				if (dfu->manifest_done) {
					/* Timer transitioned MANIFEST -> MANIFEST_SYNC with
					 * work done (TOL=1 path).  Advance to dfuIDLE now. */
					dfu->manifest_done = 0U;
					dfu->state  = USB_DFU_STATE_DFU_IDLE;
					dfu->status = USBD_DFU_STATUS_OK;
					usbd_dfu_send_status(dev, dfu, 0U);
				} else {
					/* Enter MANIFEST, wake manifest_task, return bwPollTimeout. */
					dfu->state = USB_DFU_STATE_MANIFEST;
					usb_os_sema_give(dfu->manifest_sema);
					usbd_dfu_send_status(dev, dfu, dfu->manifest_poll_ms);
				}
				break;
			}
			if (dfu->state == USB_DFU_STATE_MANIFEST) {
				if (dfu->manifest_done) {
					/* Manifest task + timer completed; transition per bmAttributes. */
#if CONFIG_USBD_DFU_MANIFESTATION_TOL
					dfu->manifest_done = 0U;
					dfu->state  = USB_DFU_STATE_MANIFEST_SYNC;
					dfu->status = USBD_DFU_STATUS_OK;
					usbd_dfu_send_status(dev, dfu, 0U);
#else
					dfu->manifest_done = 0U;
					dfu->state  = USB_DFU_STATE_MANIFEST_WAIT_RESET;
					dfu->status = USBD_DFU_STATUS_OK;
					usbd_dfu_send_status(dev, dfu, 0U);
#if CONFIG_USBD_DFU_WILL_DETACH
					/* DFU 1.1 §7: device generates a detach-attach sequence.
					 * Set mode to RUNTIME so the device re-enumerates in Run-Time mode
					 * after deinit/init (booting the new firmware). */
					dfu->mode = USB_DFU_PROTOCOL_RUNTIME;
					dfu->reconf_pending = 1U;
					usb_os_sema_give(dfu->reconf_sema);
#endif
#endif
				} else {
					/* Manifest still in progress; host must wait bwPollTimeout. */
					usbd_dfu_send_status(dev, dfu, dfu->manifest_poll_ms);
				}
				break;
			} else if (dfu->state == USB_DFU_STATE_MANIFEST_WAIT_RESET) {
				/* Non-tolerant: waiting for USB bus reset to activate firmware */
				usbd_dfu_send_status(dev, dfu, 0U);
			} else if (dfu->state == USB_DFU_STATE_DNLOAD_SYNC) {
				/* DFU 1.1 §A.2.4: in dfuDNLOAD-SYNC the GETSTATUS response depends
				 * on whether the previous block has finished programming.  The
				 * write_done flag (set by write_task) is the authoritative source
				 * and is race-free regardless of whether write_task completed
				 * before or after this request arrived. */
				if (dfu->write_done) {
					/* Block fully programmed: advance to dfuDNLOAD-IDLE. */
					dfu->write_done = 0U;
					dfu->state = USB_DFU_STATE_DNLOAD_IDLE;
					usbd_dfu_send_status(dev, dfu, 0U);
				} else {
					/* write_task still running: report dfuDNBUSY so the host
					 * backs off before polling again.  Use per-block callback when
					 * available (e.g. sector-boundary vs. plain write), otherwise
					 * fall back to the static write_poll_ms. */
					u32 pm = (dfu->cb->get_write_poll_ms != NULL)
							 ? dfu->cb->get_write_poll_ms(dfu->block_num)
							 : dfu->write_poll_ms;
					/* write_task needs at least one scheduler tick to run.
					 * pm=0 lets the host repoll before write_done is set,
					 * not stall: replies with write_poll_ms. */
					if (pm == 0U) {
						pm = 1U;
					}
					dfu->state = USB_DFU_STATE_DNBUSY;
					usbd_dfu_send_status(dev, dfu, pm);
				}
			} else if (dfu->state == USB_DFU_STATE_DNBUSY) {
				if (dfu->write_done) {
					/* bwPollTimeout has elapsed and programming is complete:
					 * advance to dfuDNLOAD-IDLE with no forced wait. */
					dfu->write_done = 0U;
					dfu->state = USB_DFU_STATE_DNLOAD_IDLE;
					usbd_dfu_send_status(dev, dfu, 0U);
				} else {
					/* Still busy; tell the host to wait before polling
					 * again (DFU 1.1 §A.2.5). */
					usbd_dfu_send_status(dev, dfu, dfu->write_poll_ms);
				}
			} else {
				usbd_dfu_send_status(dev, dfu, 0U);
			}
			break;

		case USB_DFU_REQ_CLRSTATUS:
			if (dfu->state == USB_DFU_STATE_ERROR) {
				dfu->state  = USB_DFU_STATE_DFU_IDLE;
				dfu->status = USBD_DFU_STATUS_OK;
			} else {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			}
			break;

		case USB_DFU_REQ_GETSTATE:
			/* DFU 1.1 §A.2.5: dfuDNBUSY rejects all requests including GETSTATE */
			if (dfu->state == USB_DFU_STATE_DNBUSY) {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			} else {
				ep0_in->xfer_buf[0] = (u8)dfu->state;
				ep0_in->xfer_len    = 1U;
				usbd_ep_transmit(dev, ep0_in);
			}
			break;

		case USB_DFU_REQ_ABORT:
			/* DFU 1.1 Table 2: ABORT valid only in dfuIDLE, dfuDNLOAD-IDLE, dfuUPLOAD-IDLE */
			if (dfu->state == USB_DFU_STATE_DFU_IDLE        ||
				dfu->state == USB_DFU_STATE_DNLOAD_IDLE  ||
				dfu->state == USB_DFU_STATE_UPLOAD_IDLE) {
				dfu->state  = USB_DFU_STATE_DFU_IDLE;
				dfu->status = USBD_DFU_STATUS_OK;
			} else {
				ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			}
			break;

		default:
			ret = usbd_dfu_stall_error(dev, dfu, USBD_DFU_STATUS_ERR_STALLEDPKT);
			break;
		}
		break;

	default:
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
 * @brief  EP0 IN transfer complete (called from ISR).
 *         Restores the EP0 IN buffer and handles UPLOAD last-block state transition.
 */
static int usbd_dfu_ep0_data_in(usb_dev_t *dev, u8 status)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	/* Restore the EP0 IN buffer and its capacity to the core-allocated default */
	dev->ep0_in.xfer_buf     = dfu->ep0_default_buf;
	dev->ep0_in.xfer_buf_len = dfu->ep0_default_buf_len;

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP0 IN err: %d\n", status);
		dfu->state  = USB_DFU_STATE_ERROR;
		dfu->status = USBD_DFU_STATUS_ERR_STALLEDPKT;
		return HAL_OK;
	}

#if CONFIG_USBD_DFU_CAN_UPLOAD
	if (dfu->upload_last) {
		dfu->upload_last = 0U;
		dfu->state = USB_DFU_STATE_DFU_IDLE;
	}
#endif

	return HAL_OK;
}

/**
 * @brief  EP0 OUT transfer complete: DNLOAD block received (called from ISR).
 *         Restores the EP0 OUT buffer and signals write_task to process the block.
 *         State is DNLOAD_SYNC here; the next GETSTATUS reports dfuDNBUSY while
 *         write_done is clear, then advances to DNLOAD_IDLE once it is set.
 */
static int usbd_dfu_ep0_data_out(usb_dev_t *dev)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	/* Restore the EP0 OUT buffer and its capacity to the core-allocated default */
	dev->ep0_out.xfer_buf     = dfu->ep0_default_buf;
	dev->ep0_out.xfer_buf_len = dfu->ep0_default_buf_len;

	if (dfu->state != USB_DFU_STATE_DNLOAD_SYNC) {
		return HAL_ERR_HW;
	}

	/* Clear write_done before the block is programmed; GETSTATUS will report
	 * dfuDNBUSY until write_task sets it again on completion. */
	dfu->write_done = 0U;

	/* Wake write_task; GETSTATUS reports DNBUSY until write_done is set, then DNLOAD_IDLE. */
	usb_os_sema_give(dfu->write_sema);

	return HAL_OK;
}

/**
 * @brief  Dedicated task that calls cb->write() outside of ISR context.
 *         Woken by ep0_data_out(); sets write_done on success (GETSTATUS then
 *         advances the state to DNLOAD_IDLE) or moves to DFU_ERROR on failure.
 *         Exits when write_stop is set.
 */
static void usbd_dfu_write_task(void *param)
{
	usbd_dfu_dev_t *dfu = (usbd_dfu_dev_t *)param;

	for (;;) {
		if (usb_os_sema_take(dfu->write_sema, USB_OS_SEMA_TIMEOUT) != HAL_OK) {
			break;
		}
		if (dfu->write_stop) {
			break;
		}
		int ret = dfu->cb->write(dfu->block_num, dfu->xfer_buf, dfu->block_len);
		if (ret != HAL_OK) {
			dfu->state  = USB_DFU_STATE_ERROR;
			dfu->status = USBD_DFU_STATUS_ERR_WRITE;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Write blk %u err: %d\n", dfu->block_num, ret);
		} else {
			/* Programming complete.  Setting write_done (rather than mutating
			 * dfu->state here) is race-free: GETSTATUS reads it whether the
			 * host's first poll arrived before or after write_task finished, and
			 * it never overwrites a concurrent ABORT/CLRSTATUS state change. */
			dfu->write_done = 1U;
		}
	}

	rtos_task_delete(NULL);
}
/**
 * @brief  Manifest task -- calls cb->manifest() in task context.
 *
 *         Signalled by the GETSTATUS handler when the device enters
 *         MANIFEST state.  After cb->manifest() returns it sets
 *         manifest_done so that the manifest timer or subsequent
 *         GETSTATUS can transition to the next DFU state.
 *
 *         The one-shot manifest_timer is started here before the
 *         callback, implementing the "Status poll timeout" mechanism:
 *         the timer fires every manifest_poll_ms and checks
 *         manifest_done -- if done it transitions; if not it restarts.
 */
static void usbd_dfu_manifest_task(void *param)
{
	usbd_dfu_dev_t *dfu = (usbd_dfu_dev_t *)param;

	for (;;) {
		if (usb_os_sema_take(dfu->manifest_sema, USB_OS_SEMA_TIMEOUT) != HAL_OK) {
			break;
		}
		if (dfu->manifest_stop) {
			break;
		}

		/* Start the "Status poll timeout" timer with the current poll period. */
		rtos_timer_change_period(dfu->manifest_timer, dfu->manifest_poll_ms, 0U);

		/* Application callback -- may block (e.g. wait for OTA completion) */
		if (dfu->cb->manifest != NULL) {
			dfu->cb->manifest();
		}

		dfu->manifest_done = 1U;
	}

	rtos_task_delete(NULL);
}

/**
 * @brief  Manifest timer callback -- "Status poll timeout" event (DFU 1.1 §7).
 *
 *         Fires when the bwPollTimeout reported in the previous GETSTATUS
 *         response has elapsed.  If manifestation is complete it transitions
 *         to the next DFU state; otherwise it extends the poll period.
 *
 *         Runs in timer task context (not ISR).
 */
static void usbd_dfu_manifest_timer_cb(void *timer)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	UNUSED(timer);

	if (dfu->state != USB_DFU_STATE_MANIFEST) {
		return;
	}

	if (dfu->manifest_done) {
		/* "Status poll timeout" + work done -> transition per bmAttributes */
#if CONFIG_USBD_DFU_MANIFESTATION_TOL
		dfu->state = USB_DFU_STATE_MANIFEST_SYNC;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Manifest done -> MANIFEST-SYNC\n");
#else
		dfu->manifest_done = 0U;
		dfu->state = USB_DFU_STATE_MANIFEST_WAIT_RESET;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Manifest done -> WAIT-RESET\n");
#if CONFIG_USBD_DFU_WILL_DETACH
		dfu->mode = USB_DFU_PROTOCOL_RUNTIME;
		dfu->reconf_pending = 1U;
		usb_os_sema_give(dfu->reconf_sema);
#endif
#endif
	} else {
		/* Work still in progress -> extend the poll period */
		rtos_timer_change_period(dfu->manifest_timer, dfu->manifest_poll_ms, 0U);
	}
}

/**
 * @brief  Clean up manifest task, timer, and semaphore.
 *         Safe to call when resources may be partially allocated (NULL-guarded).
 */
static void usbd_dfu_cleanup_manifest(usbd_dfu_dev_t *dfu)
{
	if (dfu->manifest_timer != NULL) {
		rtos_timer_stop(dfu->manifest_timer, 0U);
		rtos_timer_delete(dfu->manifest_timer, 0U);
		dfu->manifest_timer = NULL;
	}
	if (dfu->manifest_task != NULL) {
		rtos_task_t t = dfu->manifest_task;
		dfu->manifest_task = NULL;
		dfu->manifest_stop = 1U;
		usb_os_sema_give(dfu->manifest_sema);
		rtos_task_delete(t);
	}
	if (dfu->manifest_sema != NULL) {
		usb_os_sema_delete(dfu->manifest_sema);
		dfu->manifest_sema = NULL;
	}
}

/**
 * @brief  Clean up write task and semaphore.
 *         Safe to call when resources may be partially allocated (NULL-guarded).
 */
static void usbd_dfu_cleanup_write(usbd_dfu_dev_t *dfu)
{
	if (dfu->write_task != NULL) {
		rtos_task_t t = dfu->write_task;
		dfu->write_task = NULL;
		dfu->write_stop = 1U;
		usb_os_sema_give(dfu->write_sema);
		rtos_task_delete(t);
	}
	if (dfu->write_sema != NULL) {
		usb_os_sema_delete(dfu->write_sema);
		dfu->write_sema = NULL;
	}
}

/**
 * @brief  Get descriptor callback (called from ISR).
 */
static u16 usbd_dfu_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	u16 len = 0U;

	dev->self_powered = 0U;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_dfu_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_dfu_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		len = sizeof(usbd_dfu_config_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_dfu_config_desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN]     = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		/* Static array carries the DFU-mode protocol (0x02); rewrite it to the
		 * run-time protocol (0x01) until the device has switched to DFU mode. */
		if (usbd_dfu_dev.mode == USB_DFU_PROTOCOL_RUNTIME) {
			buf[USBD_DFU_CFG_IF_PROTOCOL_OFFSET] = USB_DFU_PROTOCOL_RUNTIME;
		}
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_dfu_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_dfu_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* DFU has no endpoints, so the other-speed config is identical */
		len = sizeof(usbd_dfu_config_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_dfu_config_desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN]     = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TYPE]          = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		if (usbd_dfu_dev.mode == USB_DFU_PROTOCOL_RUNTIME) {
			buf[USBD_DFU_CFG_IF_PROTOCOL_OFFSET] = USB_DFU_PROTOCOL_RUNTIME;
		}
		break;
#endif
	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_dfu_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_dfu_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_DFU_MFR_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			len = usbd_get_str_desc(USBD_DFU_PRODUCT_STRING, buf);
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc("00000000001", buf);
			break;
		case USBD_DFU_IFACE_STRING_IDX:
			len = usbd_get_str_desc(USBD_DFU_IFACE_STRING, buf);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return len;
}

/**
 * @brief  USB attach/detach status change (called from ISR).
 */
static void usbd_dfu_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	UNUSED(dev);

	if (dfu->cb != NULL && dfu->cb->status_changed != NULL) {
		dfu->cb->status_changed(old_status, status);
	}
}


#if !CONFIG_USBD_DFU_WILL_DETACH
/**
 * @brief  One-shot timer callback: fires when wDetachTimeOut expires without a USB Reset.
 *         If state is still appDETACH the host never issued a USB Reset, so revert to
 *         Run-Time mode.  If USB Reset already arrived, state is dfuIDLE — do nothing.
 */
static void usbd_dfu_detach_timeout_cb(void *timer)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	UNUSED(timer);

	if (dfu->state == USB_DFU_STATE_APP_DETACH) {
		dfu->mode  = USB_DFU_PROTOCOL_RUNTIME;
		dfu->state = USB_DFU_STATE_APP_IDLE;
		RTK_LOGS(TAG, RTK_LOG_WARN, "Detach: no USB Reset within %ums, reverted to Run-Time\n",
				 USBD_DFU_DETACH_TIMEOUT);
	}
}
#endif /* !CONFIG_USBD_DFU_WILL_DETACH */

/**
 * @brief  Reconfiguration task: handles the Run-Time → DFU mode transition off ISR context.
 *         Woken by the DFU_DETACH handler once dfu->mode has been switched to DFU.
 *
 *   WILL_DETACH=1: calls cb->reconf() to notify the application to perform
 *                  usbd_deinit/init, triggering re-enumeration in DFU mode.
 *   WILL_DETACH=0: starts a one-shot wDetachTimeOut timer and returns.  The host is
 *                  expected to issue a USB Reset within the timeout; if it does,
 *                  clear_config() handles the state switch (mode=DFU → dfuIDLE).
 *                  If the timer fires first, usbd_dfu_detach_timeout_cb() reverts to
 *                  Run-Time mode.
 */
static void usbd_dfu_reconf_task(void *param)
{
	usbd_dfu_dev_t *dfu = (usbd_dfu_dev_t *)param;

	for (;;) {
		if (usb_os_sema_take(dfu->reconf_sema, USB_OS_SEMA_TIMEOUT) != HAL_OK) {
			break;
		}
		if (dfu->reconf_stop) {
			break;
		}
		if (!dfu->reconf_pending) {
			continue;
		}
		dfu->reconf_pending = 0U;

#if CONFIG_USBD_DFU_WILL_DETACH
		/* Let the DFU_DETACH status stage (EP0 IN ZLP) complete before the
		 * application tears down the stack. */
		usb_os_sleep_ms(USBD_DFU_DETACH_STATUS_DELAY_MS);
		/* dfu->mode determines the target protocol.  Notify the application to
		 * perform usbd_deinit/init so the host re-enumerates in the target mode. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Detach: re-enumerating in %s mode\n",
				 (dfu->mode == USB_DFU_PROTOCOL_RUNTIME) ? "Run-Time" : "DFU");
		if (dfu->cb->reconf != NULL) {
			dfu->cb->reconf();
		}
#else
		/* Start (or restart) the one-shot timeout.  If the host issues a USB Reset
		 * before the timer fires, clear_config() transitions to dfuIDLE and the
		 * timer callback becomes a no-op.  If the timer fires first it reverts to
		 * Run-Time mode. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Detach: waiting for host USB Reset (%ums timeout)\n",
				 USBD_DFU_DETACH_TIMEOUT);
		rtos_timer_start(dfu->detach_timer, 0U);
#endif /* CONFIG_USBD_DFU_WILL_DETACH */
	}

	rtos_task_delete(NULL);
}
/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Set the bwPollTimeout (ms) reported to the host while the device is
 *         busy writing a firmware block (dfuDNBUSY response to GETSTATUS).
 *         Should reflect worst-case flash write time (e.g., 50-100 ms for a
 *         4 KB sector erase + program).  Defaults to 0 (host polls immediately).
 */
void usbd_dfu_set_write_poll_ms(u32 poll_ms)
{
	usbd_dfu_dev.write_poll_ms = poll_ms;
}

/**
 * @brief  Set the bwPollTimeout (ms) reported to the host while the device
 *         is in the MANIFEST state ("Status poll timeout").  The timer fires
 *         after this interval to check whether manifestation is complete.
 *         Defaults to USBD_DFU_MANIFEST_POLL_MS_DEFAULT.
 */
void usbd_dfu_set_manifest_poll_ms(u32 poll_ms)
{
	usbd_dfu_dev.manifest_poll_ms = poll_ms;
}

/**
 * @brief  Read the current DFU device state.
 *
 *         Used by the application (e.g. OTA task) to determine when the
 *         DFU state machine has reached a terminal state after manifest,
 *         so it can safely call System_Reset() without racing against
 *         the host's final GETSTATUS read.
 *
 * @retval Current @ref usb_dfu_state_t value (e.g. USB_DFU_STATE_DFU_IDLE).
 */
u8 usbd_dfu_get_state(void)
{
	return (u8)usbd_dfu_dev.state;
}

/**
 * @brief  Initialize the DFU device class driver.
 */
int usbd_dfu_init(usbd_dfu_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid CB\n");
		return HAL_ERR_PARA;
	}

	/* Preserve mode across a reconf reinit (WILL_DETACH=1).  On a fresh first
	 * init the struct is zero-initialised (BSS), so saved_mode is 0 which is
	 * neither RUNTIME nor DFU — default to RUNTIME below. */
	u8 saved_mode = dfu->mode;
	usb_os_memset(dfu, 0, sizeof(usbd_dfu_dev_t));

	dfu->xfer_buf = (u8 *)usb_os_malloc(USBD_DFU_XFER_SIZE);
	if (dfu->xfer_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No mem for xfer_buf\n");
		return HAL_ERR_MEM;
	}
	if (!USB_IS_MEM_DMA_ALIGNED(dfu->xfer_buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "xfer_buf not DMA-aligned\n");
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_MEM;
	}

	dfu->cb = cb;
	if (cb->write == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "write cb required (CAN_DNLOAD=1)\n");
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_PARA;
	}
	dfu->write_poll_ms = USBD_DFU_WRITE_POLL_MS_DEFAULT;
#if CONFIG_USBD_DFU_CAN_UPLOAD
	if (cb->read == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "read cb required (CAN_UPLOAD=1)\n");
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_PARA;
	}
#endif
	/* Restore DFU mode when reinitialising after a reconf callback; default to
	 * Run-Time on all other init paths (first boot or plain hotplug reinit). */
	dfu->mode = (saved_mode == USB_DFU_PROTOCOL_DFU)
				? USB_DFU_PROTOCOL_DFU : USB_DFU_PROTOCOL_RUNTIME;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail: %d\n", ret);
			usb_os_mfree(dfu->xfer_buf);
			dfu->xfer_buf = NULL;
			return ret;
		}
	}

	ret = usb_os_sema_create(&dfu->write_sema);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No mem for write_sema\n");
		if (cb->deinit != NULL) {
			cb->deinit();
		}
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_MEM;
	}

	ret = rtos_task_create(&dfu->write_task, "dfu_write", usbd_dfu_write_task,
						   dfu, USBD_DFU_WRITE_TASK_STACK, USBD_DFU_WRITE_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create write_task fail\n");
		usb_os_sema_delete(dfu->write_sema);
		dfu->write_sema = NULL;
		if (cb->deinit != NULL) {
			cb->deinit();
		}
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
		return HAL_ERR_MEM;
	}

	/* Create manifest task and timer for async manifest support */
	ret = usb_os_sema_create(&dfu->manifest_sema);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create manifest_sema fail\n");
		goto reconf_fail;
	}
	dfu->manifest_poll_ms = USBD_DFU_MANIFEST_POLL_MS_DEFAULT;
	ret = rtos_task_create(&dfu->manifest_task, "dfu_manifest", usbd_dfu_manifest_task,
						   dfu, USBD_DFU_MANIFEST_TASK_STACK, USBD_DFU_MANIFEST_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create manifest_task fail\n");
		usb_os_sema_delete(dfu->manifest_sema);
		dfu->manifest_sema = NULL;
		goto reconf_fail;
	}
	ret = rtos_timer_create(&dfu->manifest_timer, "dfu_mftmr", 0U,
							dfu->manifest_poll_ms, 0U, usbd_dfu_manifest_timer_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create manifest_timer fail\n");
		goto reconf_fail;
	}

	ret = usb_os_sema_create(&dfu->reconf_sema);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No mem for reconf_sema\n");
		goto reconf_fail;
	}

	ret = rtos_task_create(&dfu->reconf_task, "dfu_reconf", usbd_dfu_reconf_task,
						   dfu, USBD_DFU_REENUM_TASK_STACK, USBD_DFU_REENUM_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create reconf_task fail\n");
		usb_os_sema_delete(dfu->reconf_sema);
		dfu->reconf_sema = NULL;
		goto reconf_fail;
	}

#if !CONFIG_USBD_DFU_WILL_DETACH
	/* One-shot timer: if host doesn't issue USB Reset within wDetachTimeOut, revert
	 * to Run-Time mode.  Created here (task context); started in reconf_task. */
	ret = rtos_timer_create(&dfu->detach_timer, "dfu_detach", 0U,
							USBD_DFU_DETACH_TIMEOUT, 0U, usbd_dfu_detach_timeout_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach_timer fail\n");
		rtos_task_t t = dfu->reconf_task;
		dfu->reconf_task = NULL;
		dfu->reconf_stop = 1U;
		usb_os_sema_give(dfu->reconf_sema);
		rtos_task_delete(t);
		usb_os_sema_delete(dfu->reconf_sema);
		dfu->reconf_sema = NULL;
		goto reconf_fail;
	}
#endif /* !CONFIG_USBD_DFU_WILL_DETACH */

	dfu->state  = USB_DFU_STATE_APP_IDLE;
	dfu->status = USBD_DFU_STATUS_OK;

	usbd_register_class(&usbd_dfu_driver);

	return HAL_OK;

reconf_fail:
	usbd_dfu_cleanup_manifest(dfu);
	usbd_dfu_cleanup_write(dfu);
	if (cb->deinit != NULL) {
		cb->deinit();
	}
	usb_os_mfree(dfu->xfer_buf);
	dfu->xfer_buf = NULL;
	return HAL_ERR_MEM;
}

/**
 * @brief  Deinitialize the DFU device class driver.
 */
int usbd_dfu_deinit(void)
{
	usbd_dfu_dev_t *dfu = &usbd_dfu_dev;

	if (dfu->cb != NULL) {
		if (dfu->cb->deinit != NULL) {
			dfu->cb->deinit();
		}
		dfu->cb = NULL;
	}

	usbd_unregister_class();

#if !CONFIG_USBD_DFU_WILL_DETACH
	/* Stop and delete the detach timeout timer before tearing down reconf_task. */
	if (dfu->detach_timer != NULL) {
		rtos_timer_stop(dfu->detach_timer, 0U);
		rtos_timer_delete(dfu->detach_timer, 0U);
		dfu->detach_timer = NULL;
	}
#endif /* !CONFIG_USBD_DFU_WILL_DETACH */
	/* Stop reconf_task: set flag, unblock it, then delete. */
	if (dfu->reconf_task != NULL) {
		rtos_task_t t = dfu->reconf_task;
		dfu->reconf_task = NULL;
		dfu->reconf_stop = 1U;
		usb_os_sema_give(dfu->reconf_sema);
		rtos_task_delete(t);
	}
	if (dfu->reconf_sema != NULL) {
		usb_os_sema_delete(dfu->reconf_sema);
		dfu->reconf_sema = NULL;
	}
	usbd_dfu_cleanup_manifest(dfu);
	usbd_dfu_cleanup_write(dfu);

	if (dfu->xfer_buf != NULL) {
		/* ep0_data_in/out restore pointers on normal completion, but a disconnect
		 * or error can leave ep0_in/out.xfer_buf dangling into xfer_buf.
		 * Restore both to the saved default before freeing. */
		if (dfu->dev != NULL) {
			if (dfu->dev->ep0_in.xfer_buf == dfu->xfer_buf) {
				dfu->dev->ep0_in.xfer_buf = dfu->ep0_default_buf;
			}
			if (dfu->dev->ep0_out.xfer_buf == dfu->xfer_buf) {
				dfu->dev->ep0_out.xfer_buf = dfu->ep0_default_buf;
			}
		}
		usb_os_mfree(dfu->xfer_buf);
		dfu->xfer_buf = NULL;
	}

	return HAL_OK;
}
