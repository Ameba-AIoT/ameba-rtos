/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite.h"
#include "usb_cdc.h"
#include <os_wrapper.h>
#include <stdarg.h>
#include <stdlib.h>

/* Private types -------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define USBD_COMP_ITF_NONE         0xFF

/* Private macros ------------------------------------------------------------*/

/* Private types ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Logging TAG */
static const char *const TAG = "COMP";

static usbd_composite_dev_t usbd_composite_dev;

/* String descriptors (internal) */
static const char *const usbd_composite_strings[] = {
	USBD_COMP_MFG_STRING,
	USBD_COMP_PROD_STRING,
	USBD_COMP_SN_STRING,
};

/* USB Standard Device Descriptor */
static const u8 usbd_composite_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB */
	0xEF,                                           /* bDeviceClass: MISC */
	0x02,                                           /* bDeviceSubClass: Common Class */
	0x01,                                           /* bDeviceProtocol: IAD */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	USB_LOW_BYTE(USBD_COMP_VID), USB_HIGH_BYTE(USBD_COMP_VID),  /* idVendor */
	USB_LOW_BYTE(USBD_COMP_PID), USB_HIGH_BYTE(USBD_COMP_PID),  /* idProduct */
	0x00, 0x02,                                     /* bcdDevice */
	0x01,                                           /* iManufacturer */
	0x02,                                           /* iProduct */
	0x03,                                           /* iSerialNumber */
	0x01,                                           /* bNumConfigurations */
};

/* Device Qualifier Descriptor */
static const u8 usbd_composite_dev_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB */
	0xEF,                                           /* bDeviceClass */
	0x02,                                           /* bDeviceSubClass */
	0x01,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* bReserved */
};

/* Configuration descriptor template.
 * bDescriptorType (byte 1), wTotalLength (bytes 2-3), bNumInterfaces (byte 4),
 * and bmAttributes (byte 7) are patched at runtime in usbd_composite_build_config_desc().
 */
static const u8 usbd_composite_config_desc[USB_LEN_CFG_DESC] = {
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType (placeholder, patched) */
	0x00, 0x00,                                     /* wTotalLength (patched) */
	0x00,                                           /* bNumInterfaces (patched) */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                           /* bMaxPower (100mA) */
};

/* Language ID String Descriptor */
static const u8 usbd_composite_langid_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_COMP_LANGID),
	USB_HIGH_BYTE(USBD_COMP_LANGID),
};

/* Private function prototypes -----------------------------------------------*/

static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int usbd_composite_sof(usb_dev_t *dev);
static int usbd_composite_ep0_data_in(usb_dev_t *dev, u8 status);
static int usbd_composite_ep0_data_out(usb_dev_t *dev);
static int usbd_composite_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void usbd_composite_wakeup(usb_dev_t *dev);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Reset the active-setup tracking (no pending data-OUT).
 */
static void usbd_composite_reset_active_func(void)
{
	usbd_composite_dev.active_func = USBD_COMP_ITF_NONE;
}

/**
 * @brief  Find the minimum interface number in a descriptor block.
 * @param  desc: Pointer to descriptor block (after config descriptor).
 * @param  len: Length of descriptor block.
 * @retval Minimum interface number, or USBD_COMP_ITF_NONE if none found.
 */
static u8 usbd_composite_find_min_interface(const u8 *desc, u16 len)
{
	u16 i;
	u8 dlen;
	u8 dtype;

	i = 0;
	while (i < len) {
		dlen = desc[i];
		dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}
		if (dtype == USB_DESC_TYPE_INTERFACE) {
			return desc[i + 2];
		}
		i += dlen;
	}

	return USBD_COMP_ITF_NONE;
}

/**
 * @brief  Patch interface numbers in a descriptor block by adding an offset.
 * @param  desc: Pointer to descriptor block.
 * @param  len: Length of descriptor block.
 * @param  offset: Offset to add to each interface number.
 */
static void usbd_composite_patch_if_numbers(u8 *desc, u16 len, u8 offset)
{
	u16 i;
	u8 dlen;
	u8 dtype;

	for (i = 0; i < len;) {
		dlen = desc[i];
		dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}

		switch (dtype) {
		case USB_DESC_TYPE_INTERFACE:
			desc[i + 2] += offset;         /* bInterfaceNumber */
			/* bAlternateSetting (i+3) must NOT be patched */
			break;
		case USB_DESC_TYPE_IAD:
			desc[i + 2] += offset;         /* bFirstInterface */
			break;
		case USB_DESC_TYPE_CS_INTERFACE:
			if ((dlen >= 5) && ((desc[i + 2] == USB_CDC_FUNC_DESC_CALL_MGMT) || (desc[i + 2] == USB_CDC_FUNC_DESC_UNION))) {
				/* Call Management FD: bDataInterface at offset 4 */
				desc[i + 4] += offset;
			}
			if ((dlen >= 4) && (desc[i + 2] == USB_CDC_FUNC_DESC_UNION)) {
				/* Union FD: bMasterInterface at offset 3, bSlaveInterface0 at offset 4+ */
				desc[i + 3] += offset;     /* bMasterInterface */
			}
			break;
		default:
			break;
		}
		i += dlen;
	}
}

/**
 * @brief  Append a sub-function's descriptor block to the configuration buffer,
 *         patching IAD bFirstInterface and interface numbers as needed.
 * @param  dev: USB device instance.
 * @param  req: Setup request.
 * @param  dest: Destination buffer for full configuration descriptor.
 * @param  dest_off: Current offset in destination buffer.
 * @param  driver: Class driver pointer.
 * @param  if_base: Interface number base for this function.
 * @param  if_count: Number of interfaces for this function (from pre-scan).
 * @retval Number of bytes appended, or 0 on error.
 */
static u16 usbd_composite_append_func_desc(usb_dev_t *dev, usb_setup_req_t *req,
		u8 *dest, u16 dest_off, const usbd_class_driver_t *driver,
		u8 if_base, u8 if_count)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 *temp = cdev->desc_buf;
	u16 total;
	u8 *src;
	u16 src_len;
	u8 orig_first;
	u16 pos;
	u8 dlen;
	u8 dtype;
	u8 iad_found = 0;

	total = driver->get_descriptor(dev, req, temp);
	if (total <= USB_LEN_CFG_DESC) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "Func get_desc fail\n");
		return 0;
	}

	src = temp + USB_LEN_CFG_DESC;
	src_len = total - USB_LEN_CFG_DESC;

	/* Scan for IAD presence — multi-interface drivers MUST embed an IAD */
	for (pos = 0; pos < src_len;) {
		dlen = src[pos];
		dtype = src[pos + 1];
		if (dlen == 0) {
			break;
		}
		if (dtype == USB_DESC_TYPE_IAD) {
			iad_found = 1;
			break;
		}
		pos += dlen;
	}

	if ((if_count > 1) && (iad_found == 0)) {
		//RTK_LOGS(TAG, RTK_LOG_WARN, "Func IF=%u: missing IAD\n", if_count);
	}

	/* Patch interface numbers */
	orig_first = usbd_composite_find_min_interface(src, src_len);
	if (orig_first != USBD_COMP_ITF_NONE && if_base != orig_first) {
		usbd_composite_patch_if_numbers(src, src_len, if_base - orig_first);
	}

	if ((u32)(dest_off + src_len) > cdev->desc_buf_size) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc buf overflow\n");
		return 0;
	}

	memcpy(dest + dest_off, src, src_len);
	return src_len;
}

/**
 * @brief  Calculate total interface count from parsed if_counts[].
 */
static u8 usbd_composite_total_if_count(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 total = 0;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		total += cdev->if_counts[i];
	}
	return total;
}

/**
 * @brief  Build the full configuration descriptor by calling each sub-function.
 */
static u16 usbd_composite_build_config_desc(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u16 total_len = USB_LEN_CFG_DESC;
	u16 added;
	u16 desc_len;
	u8 if_base = 0;
	u8 if_cnt;
	u8 attr;
	u8 i;

	memcpy(buf, usbd_composite_config_desc, USB_LEN_CFG_DESC);

	/* Patch bDescriptorType (byte 1) for Other Speed Config */
	if (USB_HIGH_BYTE(req->wValue) == USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION) {
		buf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
	}

	/* Patch bmAttributes (byte 7) */
	attr = 0x80U;
#ifdef CONFIG_USBD_SELF_POWERED
	attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
	attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
#endif
	buf[7] = attr;

	/* First pass: populate if_counts[] from each sub-function's config descriptor
	 * header, so that the config header's bNumInterfaces is correct before we
	 * start appending descriptor blocks.
	 */
	for (i = 0; i < cdev->func_count; i++) {
		desc_len = cdev->drivers[i]->get_descriptor(dev, req, cdev->desc_buf);
		if (desc_len < USB_LEN_CFG_DESC) {
			cdev->if_counts[i] = 0;
		} else {
			cdev->if_counts[i] = cdev->desc_buf[4]; /* bNumInterfaces */
		}
	}

	buf[4] = usbd_composite_total_if_count(); /* bNumInterfaces */

	/* Second pass: append each sub-function's descriptor block */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if_cnt = cdev->if_counts[i];

		added = usbd_composite_append_func_desc(dev, req,
												buf, total_len, driver, if_base, if_cnt);
		if (added == 0) {
			return 0;
		}
		total_len += added;
		if_base += if_cnt;
	}

	/* Fill in wTotalLength */
	buf[2] = USB_LOW_BYTE(total_len);
	buf[3] = USB_HIGH_BYTE(total_len);

	return total_len;
}

/**
 * @brief  Get string descriptor.
 * @param  str_idx: String index.
 * @param  buf: Output buffer.
 * @retval Actual string descriptor length, or 0 if not found.
 */
static u16 usbd_composite_get_string_desc(u8 str_idx, u8 *buf)
{
	if (str_idx == 0) {
		memcpy(buf, usbd_composite_langid_desc, USB_LEN_LANGID_STR_DESC);
		return USB_LEN_LANGID_STR_DESC;
	}

	if (str_idx > USBD_COMP_STRING_COUNT) {
		return 0;
	}

	return usbd_get_str_desc(usbd_composite_strings[str_idx - 1], buf);
}

/**
 * @brief  Class driver get_descriptor callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	switch (USB_HIGH_BYTE(req->wValue)) {
	case USB_DESC_TYPE_DEVICE:
		memcpy(buf, usbd_composite_dev_desc, USB_LEN_DEV_DESC);
		return USB_LEN_DEV_DESC;

	case USB_DESC_TYPE_CONFIGURATION:
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		return usbd_composite_build_config_desc(dev, req, buf);

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		memcpy(buf, usbd_composite_dev_qualifier_desc, USB_LEN_DEV_QUALIFIER_DESC);
		return USB_LEN_DEV_QUALIFIER_DESC;

	case USB_DESC_TYPE_STRING:
		return usbd_composite_get_string_desc(req->wValue & 0xFF, buf);

	default:
		break;
	}

	return 0;
}

/**
 * @brief  Class driver set_config callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static int usbd_composite_set_config(usb_dev_t *dev, u8 config)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = HAL_OK;
	int status;
	u8 i;

	cdev->dev = dev;

	usbd_composite_reset_active_func();

#ifdef CONFIG_USBD_SELF_POWERED
	dev->self_powered = 1;
#else
	dev->self_powered = 0;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
	dev->remote_wakeup_en = 1;
#else
	dev->remote_wakeup_en = 0;
#endif

	/* Call each sub-function's set_config.
	 * Continue on error so all functions get a chance to init their endpoints.
	 * The caller is expected to issue clear_config (which iterates unconditionally)
	 * on failure to roll back any partially-initialised functions.
	 */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->set_config) {
			status = driver->set_config(dev, config);
			if (status != HAL_OK) {
				//RTK_LOGS(TAG, RTK_LOG_ERROR, "Func %d set_config fail\n", i);
				ret = status;
			}
		}
	}

	return ret;
}

/**
 * @brief  Class driver clear_config callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static int usbd_composite_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 i;

	usbd_composite_reset_active_func();

	/* Dispatch to each sub-function — always iterate ALL functions so that
	 * every sub-function's resources (endpoints, buffers, etc.) are released,
	 * regardless of individual failures.
	 */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->clear_config) {
			driver->clear_config(dev, config);
		}
	}

	return HAL_OK;
}

/**
 * @brief  Class driver setup callback (class/vendor-specific requests on EP0).
 *         Called within ISR context; time-consuming operations not permitted.
 *
 *         Routing policy:
 *           - SET_INTERFACE / GET_INTERFACE (standard): routed by wIndex to the
 *             sub-function whose interface range contains req->wIndex.
 *           - Other standard requests: iterated across all sub-functions
 *             (first one to return HAL_OK wins).
 *           - Class requests: iterated across all sub-functions (first-accept).
 *             NOTE: Audio Class (UAC) uses wIndex to carry an Entity ID
 *             (e.g. Feature Unit ID), NOT the interface number. Therefore
 *             interface-range routing does NOT work for Audio Class requests.
 *           - Vendor requests: iterated across all sub-functions (first-accept).
 *
 *         When a sub-function handles a request that has a data-OUT stage
 *         (bmRequestType direction = H2D, wLength > 0), its index is recorded
 *         in active_func so the subsequent ep0_data_out is routed to the same
 *         sub-function only.
 */
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = HAL_ERR_PARA;
	u8 has_data_out;
	u8 if_base;
	u8 i;

	has_data_out = ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_H2D) && (req->wLength > 0);

	usbd_composite_reset_active_func();

	/* ── Standard requests: route interface-specific ones by wIndex ── */
	if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
		if ((req->bRequest == USB_REQ_SET_INTERFACE) ||
			(req->bRequest == USB_REQ_GET_INTERFACE)) {
			if_base = 0;
			for (i = 0; i < cdev->func_count; i++) {
				if (req->wIndex >= if_base &&
					req->wIndex < if_base + cdev->if_counts[i]) {
					driver = cdev->drivers[i];
					if (driver->setup) {
						ret = driver->setup(dev, req);
						if ((ret == HAL_OK) && has_data_out) {
							cdev->active_func = i;
						}
					}
					break;
				}
				if_base += cdev->if_counts[i];
			}
			return ret;
		}
		/* Other standard requests (GET_STATUS etc.): fall through to first-accept */
	}

	/* ── Class requests: iterate (first-accept) ──
	 * Audio Class (UAC) uses Entity ID in wIndex, which is NOT an interface
	 * number, so interface-range routing would silently drop all Audio class
	 * requests.  First-accept iteration fixes this.
	 */
	if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) {
		for (i = 0; i < cdev->func_count; i++) {
			driver = cdev->drivers[i];
			if (driver->setup) {
				ret = driver->setup(dev, req);
				if (ret == HAL_OK) {
					if (has_data_out) {
						cdev->active_func = i;
					}
					break;
				}
			}
		}
		return ret;
	}

	/* ── Standard (non-SET/GET_INTERFACE) & Vendor requests: iterate (first-accept) ── */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->setup) {
			ret = driver->setup(dev, req);
			if (ret == HAL_OK) {
				if (has_data_out) {
					cdev->active_func = i;
				}
				break;
			}
		}
	}

	return ret;
}

/**
 * @brief  Class driver ep_data_in callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static int usbd_composite_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = HAL_ERR_PARA;
	u8 i;

	/* Dispatch to each sub-function */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->ep_data_in) {
			ret = driver->ep_data_in(dev, ep_addr, status);
			if (ret == HAL_OK) {
				break;
			}
		}
	}

	return ret;
}

/**
 * @brief  Class driver ep_data_out callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static int usbd_composite_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = HAL_ERR_PARA;
	u8 i;

	/* Dispatch to each sub-function */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->ep_data_out) {
			ret = driver->ep_data_out(dev, ep_addr, len);
			if (ret == HAL_OK) {
				break;
			}
		}
	}

	return ret;
}

/**
 * @brief  USB attach status change callback.
 */
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	const usbd_composite_cb_t *cb;
	u8 i;

	/* First: dispatch to each sub-function's status_changed callback */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->status_changed) {
			driver->status_changed(dev, old_status, status);
		}
	}

	/* Then: notify the composite-level user callback */
	cb = cdev->cb;
	if (cb && cb->status_changed) {
		cb->status_changed(old_status, status);
	}
}

/**
 * @brief  SOF callback — broadcast to all sub-functions.
 * @note   SOF is a broadcast event: EVERY sub-function that registered a SOF
 *         handler must receive it (e.g. UAC needs SOF for clock synchronisation).
 *         Do NOT stop on the first HAL_OK.
 */
static int usbd_composite_sof(usb_dev_t *dev)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->sof) {
			driver->sof(dev);
		}
	}

	return HAL_OK;
}

/**
 * @brief  EP0 data IN complete — dispatched to each sub-function.
 */
static int usbd_composite_ep0_data_in(usb_dev_t *dev, u8 status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 i;
	int ret = HAL_ERR_PARA;

	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->ep0_data_in) {
			ret = driver->ep0_data_in(dev, status);
			if (ret == HAL_OK) {
				break;
			}
		}
	}

	return ret;
}

/**
 * @brief  EP0 data OUT complete — forwarded to the sub-function that handled
 *         the preceding setup request.
 * @note   Only the sub-function recorded by usbd_composite_setup() receives
 *         this callback, since EP0 data belongs to exactly one class request.
 */
static int usbd_composite_ep0_data_out(usb_dev_t *dev)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 idx;

	idx = cdev->active_func;
	if ((idx < cdev->func_count) &&
		(cdev->drivers[idx]->ep0_data_out)) {
		usbd_composite_reset_active_func();
		return cdev->drivers[idx]->ep0_data_out(dev);
	}
	return HAL_ERR_PARA;
}

/**
 * @brief  Wakeup (resume from suspend) — dispatched to each sub-function.
 */
static void usbd_composite_wakeup(usb_dev_t *dev)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->wakeup) {
			driver->wakeup(dev);
		}
	}
}

/**
 * @brief  Composite class driver structure.
 */
static const usbd_class_driver_t usbd_composite_driver = {
	.get_descriptor  = usbd_composite_get_descriptor,
	.set_config      = usbd_composite_set_config,
	.clear_config    = usbd_composite_clear_config,
	.setup           = usbd_composite_setup,
	.sof             = usbd_composite_sof,
	.ep0_data_in     = usbd_composite_ep0_data_in,
	.ep0_data_out    = usbd_composite_ep0_data_out,
	.ep_data_in      = usbd_composite_ep_data_in,
	.ep_data_out     = usbd_composite_ep_data_out,
	.status_changed  = usbd_composite_status_changed,
	.wakeup          = usbd_composite_wakeup,
};

/* Exported functions ---------------------------------------------------------*/

/**
 * @brief  Register a class driver with the composite framework.
 * @note   Called from within each class driver's composite_init function.
 *         Repeated registration of the same driver pointer is idempotent.
 */
int usbd_composite_register_driver(const usbd_class_driver_t *driver)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (driver == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid driver\n");
		return HAL_ERR_PARA;
	}

	if (cdev->func_count >= USBD_COMP_MAX_FUNCS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Too many funcs (%u)\n", cdev->func_count);
		return HAL_ERR_PARA;
	}

	/* Avoid duplicate registration */
	for (u8 i = 0; i < cdev->func_count; i++) {
		if (cdev->drivers[i] == driver) {
			return HAL_OK;
		}
	}

	cdev->drivers[cdev->func_count] = driver;
	cdev->func_count++;
	return HAL_OK;
}

/**
 * @brief  Unregister a class driver from the composite framework.
 * @note   Called from within each class driver's deinit function when
 *         the driver was registered as part of a composite device.
 */
void usbd_composite_unregister_driver(const usbd_class_driver_t *driver)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 i;

	if (driver == NULL) {
		return;
	}

	for (i = 0; i < cdev->func_count; i++) {
		if (cdev->drivers[i] == driver) {
			/* Shift remaining entries to fill the gap */
			cdev->func_count--;
			for (; i < cdev->func_count; i++) {
				cdev->drivers[i] = cdev->drivers[i + 1];
			}
			cdev->drivers[cdev->func_count] = NULL;
			break;
		}
	}
}

int usbd_composite_init(const usbd_composite_cb_t *cb)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (cdev->func_count == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No funcs registered\n");
		return HAL_ERR_PARA;
	}

	/* Determine descriptor scratch buffer size from core EP0 transfer buffer */
	cdev->desc_buf_size = usbd_get_ctrl_xfer_buf_len();

	/* Allocate descriptor scratch buffer */
	cdev->desc_buf = (u8 *)rtos_mem_malloc(cdev->desc_buf_size);
	if (cdev->desc_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc buf alloc fail\n");
		return HAL_ERR_MEM;
	}

	cdev->cb          = cb;
	cdev->dev         = NULL;
	usbd_composite_reset_active_func();

	usbd_register_class(&usbd_composite_driver);

	return HAL_OK;
}

void usbd_composite_deinit(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	if (cdev->desc_buf != NULL) {
		rtos_mem_free(cdev->desc_buf);
	}
	memset(cdev, 0, sizeof(usbd_composite_dev_t));
}
