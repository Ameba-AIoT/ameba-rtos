/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_COMP_ITF_NONE         0xFFU

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_composite_set_config(usb_dev_t *dev, u8 config);
static void usbd_composite_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req);
static void usbd_composite_sof(usb_dev_t *dev);
static int usbd_composite_ep0_data_in(usb_dev_t *dev, int status);
static int usbd_composite_ep0_data_out(usb_dev_t *dev);
static int usbd_composite_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
static int usbd_composite_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_composite_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static void usbd_composite_wakeup(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/

/* Logging TAG */
static const char *const TAG = "COMP";

static usbd_composite_dev_t usbd_composite_dev;

/* USB Standard Device Descriptor */
static const u8 usbd_composite_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00, 0x02,                                     /* bcdUSB */
	0xEF,                                           /* bDeviceClass: MISC */
	0x02,                                           /* bDeviceSubClass: Common Class */
	0x01,                                           /* bDeviceProtocol: IAD */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize0 */
	USB_LOW_BYTE(USB_VID), USB_HIGH_BYTE(USB_VID),  /* idVendor (patched by core from usbd_dev_info_t) */
	USB_LOW_BYTE(USB_PID), USB_HIGH_BYTE(USB_PID),  /* idProduct (patched by core from usbd_dev_info_t) */
	0x00, 0x02,                                     /* bcdDevice (patched by core from usbd_dev_info_t) */
	USBD_IDX_MFC_STR,                               /* iManufacturer (patched by core) */
	USBD_IDX_PRODUCT_STR,                           /* iProduct (patched by core) */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber (patched by core) */
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
	0x80,                                           /* bmAttributes (patched by core from usbd_dev_info_t) */
	0x32,                                           /* bMaxPower (patched by core from usbd_dev_info_t) */
};

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

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Reset the active-setup tracking (no pending data-OUT/data-IN).
 */
static void usbd_composite_reset_active_func(void)
{
	usbd_composite_dev.active_func = USBD_COMP_ITF_NONE;
	usbd_composite_dev.active_func_in = USBD_COMP_ITF_NONE;
}

/**
 * @brief  Validate a function descriptor block and collect its interface numbering.
 * @note   Ref USB 2.0 9.5: every descriptor starts with bLength and bDescriptorType. A
 *         descriptor shorter than its header, or running past the block, makes the whole
 *         block malformed, so nothing is patched in it.
 * @param  desc: Pointer to descriptor block (after config descriptor).
 * @param  len: Length of descriptor block.
 * @param  min_if: Receives the lowest bInterfaceNumber, USBD_COMP_ITF_NONE if none found.
 * @param  max_if: Receives the highest bInterfaceNumber.
 * @param  iad_end: Receives the highest bFirstInterface + bInterfaceCount of the IADs, 0 if none found.
 * @retval RTK_SUCCESS, or -RTK_ERR_BADARG on a malformed block.
 */
static int usbd_composite_scan_func_desc(const u8 *desc, u16 len, u8 *min_if, u8 *max_if, u16 *iad_end)
{
	int ret = RTK_SUCCESS;
	u16 i = 0U;
	u16 end;
	u8 dlen;
	u8 dtype;
	u8 itf;

	*min_if = USBD_COMP_ITF_NONE;
	*max_if = 0U;
	*iad_end = 0U;

	while ((i < len) && (ret == RTK_SUCCESS)) {
		dlen = ((u16)(len - i) >= USB_LEN_DESC_HEADER) ? desc[i] : 0U;
		if ((dlen < USB_LEN_DESC_HEADER) || ((u16)dlen > (u16)(len - i))) {
			ret = -RTK_ERR_BADARG;
		} else {
			dtype = desc[i + 1U];
			if (dtype == USB_DESC_TYPE_INTERFACE) {
				if (dlen < USB_LEN_IF_DESC) {
					ret = -RTK_ERR_BADARG;
				} else {
					itf = desc[i + 2U];
					*min_if = (itf < *min_if) ? itf : *min_if;
					*max_if = (itf > *max_if) ? itf : *max_if;
				}
			} else if (dtype == USB_DESC_TYPE_IAD) {
				if (dlen < USB_LEN_IAD_DESC) {
					ret = -RTK_ERR_BADARG;
				} else {
					/* bFirstInterface at offset 2, bInterfaceCount at offset 3 */
					end = (u16)((u16)desc[i + 2U] + (u16)desc[i + 3U]);
					*iad_end = (end > *iad_end) ? end : *iad_end;
				}
			} else {
				/* Class-specific and endpoint descriptors carry no interface number to patch */
			}
			i = (u16)(i + dlen);
		}
	}

	return ret;
}

/**
 * @brief  Patch the class-independent interface number fields of a descriptor block.
 * @note   Only the Standard Interface bInterfaceNumber and the IAD bFirstInterface are
 *         class-independent, so only those are patched here. Every class-specific
 *         cross-interface reference (CDC Union/Call Management, UAC1 and UVC
 *         baInterfaceNr[], ...) is rebased by the owning class itself, which learns its
 *         interface base through set_interface_base(); keeping that knowledge in the class
 *         avoids a per-class switch here that silently misses any new class.
 *         The block must have been validated by usbd_composite_scan_func_desc().
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
		if (dlen == 0U) {
			break;
		}

		/* bInterfaceNumber and bFirstInterface both sit at offset 2; bAlternateSetting
		 * (INTERFACE offset 3) must NOT be patched. */
		if ((dtype == USB_DESC_TYPE_INTERFACE) || (dtype == USB_DESC_TYPE_IAD)) {
			/* Cannot wrap: the caller bounds every patched number by USBD_MAX_NUM_INTERFACES */
			desc[i + 2U] = (u8)(desc[i + 2U] + offset);
		}
		i += dlen;
	}
}

/**
 * @brief  Append a sub-function's descriptor block to the configuration buffer in place,
 *         patching IAD bFirstInterface and interface numbers as needed.
 * @note   The sub-function emits a complete configuration descriptor, header included. It is
 *         built at dest + dest_off - USB_LEN_CFG_DESC, so that its header overlays the last
 *         USB_LEN_CFG_DESC bytes already assembled, which are saved before and restored after,
 *         and its body lands right where it belongs. This needs neither a scratch buffer nor an
 *         overlapping move. Every class patches its descriptor byte by byte, so building it at
 *         an unaligned offset is safe; DMA alignment only concerns the start of dest.
 * @param  dev: USB device instance.
 * @param  req: Setup request.
 * @param  dest: Destination buffer for full configuration descriptor.
 * @param  dest_len: Capacity of dest in bytes.
 * @param  dest_off: Current offset in destination buffer, at least USB_LEN_CFG_DESC.
 * @param  func: Sub-function index; its if_counts[] entry is updated from its descriptor.
 * @param  if_base: Interface number base for this function.
 * @retval Number of bytes appended, or 0 on error.
 */
static u16 usbd_composite_append_func_desc(usb_dev_t *dev, usb_setup_req_t *req, u8 *dest, u16 dest_len, u16 dest_off, u8 func, u8 if_base)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver = cdev->drivers[func];
	u8 saved[USB_LEN_CFG_DESC];
	u8 *hdr = dest + dest_off - USB_LEN_CFG_DESC;
	u16 room = (u16)(dest_len - dest_off + USB_LEN_CFG_DESC);
	u16 total;
	u8 *src;
	u16 src_len;
	u8 if_count;
	u8 min_if;
	u8 max_if;
	u16 iad_end;

	/* A class rebases its own cross-interface references while building its descriptor */
	if (driver->set_interface_base != NULL) {
		driver->set_interface_base(if_base);
	}

	usb_os_memcpy((void *)saved, (const void *)hdr, USB_LEN_CFG_DESC);
	total = driver->get_descriptor(dev, req, hdr, room);
	if_count = hdr[4]; /* bNumInterfaces */
	usb_os_memcpy((void *)hdr, (const void *)saved, USB_LEN_CFG_DESC);

	/* Truncation is not allowed: a configuration descriptor beyond the EP0 buffer can only
	 * be stalled, so usbd_config_t::ctrl_xfer_buf_len must cover its wTotalLength */
	if ((total <= USB_LEN_CFG_DESC) || (total > room)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Func %u desc fail, %u/%u B used\n", func, dest_off, dest_len);
		return 0;
	}

	src = dest + dest_off;
	src_len = (u16)(total - USB_LEN_CFG_DESC);

	if (usbd_composite_scan_func_desc(src, src_len, &min_if, &max_if, &iad_end) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Func %u desc malformed\n", func);
		return 0;
	}

	if (min_if != 0U) {
		/* A class rebases its own cross-interface references by adding if_base to the
		 * local interface numbers of its template, which is only equivalent to the
		 * offset applied here when the template starts numbering at 0. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Func IF not 0-based: %u\n", min_if);
		return 0;
	}

	/* Every interface and IAD must stay inside [if_base, if_base + if_count), the range the
	 * setup routing assigns to this function, and the total within what the core accepts */
	if ((max_if >= if_count) || (iad_end > if_count) || (((u16)if_base + if_count) > USBD_MAX_NUM_INTERFACES)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Func %u IF range err %u/%u\n", func, max_if, if_count);
		return 0;
	}

	/* Multi-interface drivers MUST embed an IAD */
	if ((if_count > 1U) && (iad_end == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Func IF=%u: missing IAD\n", if_count);
	}

	cdev->if_counts[func] = if_count;

	if (if_base != 0U) {
		usbd_composite_patch_if_numbers(src, src_len, if_base);
	}

	return src_len;
}

/**
 * @brief  Hand each sub-function the first interface number assigned to it.
 * @note   Requires if_counts[] to be up to date, so it must run after a configuration
 *         descriptor build. Idempotent: as long as the registration order is unchanged, every
 *         call hands out the same base, which is why the sub-function only has to store
 *         it. A class owning no cross-interface reference leaves the callback NULL.
 */
static void usbd_composite_assign_if_bases(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 if_base = 0;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->set_interface_base != NULL) {
			driver->set_interface_base(if_base);
		}
		if_base += cdev->if_counts[i];
	}
}

/**
 * @brief  Build the full configuration descriptor by calling each sub-function once.
 */
static u16 usbd_composite_build_config_desc(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u16 total_len = USB_LEN_CFG_DESC;
	u16 added;
	u8 if_base = 0;
	u8 i;

	/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
	if (USB_LEN_CFG_DESC > buf_len) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg desc OVSZ %d > %d\n", USB_LEN_CFG_DESC, buf_len);
		return 0;
	}

	usb_os_memcpy((void *)buf, (const void *)usbd_composite_config_desc, USB_LEN_CFG_DESC);

	/* Patch bDescriptorType (byte 1) for Other Speed Config */
	if (USB_HIGH_BYTE(req->wValue) == USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION) {
		buf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
	}

	/* bmAttributes and bMaxPower are patched by the core from usbd_dev_info_t */

	/* Append each sub-function's descriptor block. Its interface base only depends on the
	 * functions before it, whose interface counts are known by the time it is built. */
	for (i = 0; i < cdev->func_count; i++) {
		added = usbd_composite_append_func_desc(dev, req, buf, buf_len, total_len, i, if_base);
		if (added == 0U) {
			return 0;
		}
		total_len = (u16)(total_len + added);
		/* Bounded by USBD_MAX_NUM_INTERFACES in usbd_composite_append_func_desc() */
		if_base = (u8)(if_base + cdev->if_counts[i]);
	}

	/* Fill in bNumInterfaces and wTotalLength: if_base now counts every interface */
	buf[4] = if_base;
	buf[2] = USB_LOW_BYTE(total_len);
	buf[3] = USB_HIGH_BYTE(total_len);

	return total_len;
}

/**
 * @brief  Class driver get_descriptor callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static u16 usbd_composite_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);

	/* Ref USB 2.0 9.4.3: the index only selects among string and configuration descriptors;
	   with a single configuration, a non-string index other than 0 names no descriptor. */
	if ((type != USB_DESC_TYPE_STRING) && (USB_LOW_BYTE(req->wValue) != 0U)) {
		return 0;
	}

	switch (type) {
	case USB_DESC_TYPE_DEVICE:
		desc = usbd_composite_dev_desc;
		len = USB_LEN_DEV_DESC;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		return usbd_composite_build_config_desc(dev, req, buf, buf_len);

	/* Ref USB 2.0 9.6.2 and 9.6.4: only a high-speed capable device has a device qualifier and
	   an other speed configuration, a single speed one answers them with a request error. The
	   configured speed counts, not the negotiated one: an HS device behind an FS hub is still
	   high-speed capable, while an HS PHY driven in Full-Speed mode never chirps. */
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (dev->info.speed == (u8)USB_SPEED_HIGH) {
			return usbd_composite_build_config_desc(dev, req, buf, buf_len);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		if (dev->info.speed == (u8)USB_SPEED_HIGH) {
			desc = usbd_composite_dev_qualifier_desc;
			len = USB_LEN_DEV_QUALIFIER_DESC;
		}
		break;

	case USB_DESC_TYPE_STRING:
		/* Every string is owned by the core: the device-global ones come from
		   usbd_dev_info_t, the class ones from usbd_add_string(). The core only forwards an
		   index it does not know, e.g. the MS OS string. */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	return len;
}

/**
 * @brief  Class driver set_config callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static int usbd_composite_set_config(usb_dev_t *dev, u8 config)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = RTK_SUCCESS;
	int status;
	u8 i;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if ((config != 1U) || (cdev->func_count == 0U)) {
		return -RTK_ERR_BADARG;
	}

	usbd_composite_reset_active_func();

	/* Ref USB 2.0 9.4.7: a host may set the configuration without having read its descriptor,
	 * e.g. from a cache, but if_counts[] is only learnt by building it. It is filled in order
	 * and a valid function never reports 0 interfaces, so a known last entry means all are.
	 * SET_CONFIGURATION has no data stage and its status stage is only sent once this returns,
	 * so the EP0 IN buffer is free to build into. */
	if (cdev->if_counts[cdev->func_count - 1U] == 0U) {
		usb_setup_req_t cfg_req = {
			.bmRequestType = USB_D2H,
			.bRequest = USB_REQ_GET_DESCRIPTOR,
			.wValue = (u16)((u16)USB_DESC_TYPE_CONFIGURATION << 8U),
			.wIndex = 0U,
			.wLength = 0U,
		};
		if (usbd_composite_build_config_desc(dev, &cfg_req, dev->ep0_in.xfer_buf, (u16)dev->ep0_in.xfer_buf_len) == 0U) {
			return -RTK_ERR_BADARG;
		}
	}

	/* Re-assert the interface bases before any sub-function starts running: a class
	 * naming one of its interfaces in a notification payload needs its base from here on. */
	usbd_composite_assign_if_bases();

	/* Call each sub-function's set_config.
	 * Continue on error so all functions get a chance to init their endpoints.
	 */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->set_config != NULL) {
			status = driver->set_config(dev, config);
			if (status != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Func %d set_config fail\n", i);
				ret = status;
			}
		}
	}

	if (ret != RTK_SUCCESS) {
		/* Roll back the functions that did come up (clear_config iterates
		 * unconditionally), so a failed set_config leaves nothing initialized */
		usbd_composite_clear_config(dev, config);
	}

	return ret;
}

/**
 * @brief  Class driver clear_config callback.
 *         Called within ISR context; time-consuming operations not permitted.
 */
static void usbd_composite_clear_config(usb_dev_t *dev, u8 config)
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
		if (driver->clear_config != NULL) {
			driver->clear_config(dev, config);
		}
	}
}

/**
 * @brief  Find the sub-function that owns interface target_if.
 * @param  cdev: composite device instance
 * @param  target_if: interface number to look up
 * @param  if_base_out: receives the interface base of the owning sub-function
 * @retval Owning sub-function index, or cdev->func_count if none owns it
 */
static u8 usbd_composite_find_owner_func(usbd_composite_dev_t *cdev, u16 target_if, u8 *if_base_out)
{
	u8 if_base = 0;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		if ((target_if >= if_base) && (target_if < if_base + cdev->if_counts[i])) {
			*if_base_out = if_base;
			return i;
		}
		if_base += cdev->if_counts[i];
	}

	*if_base_out = if_base;
	return cdev->func_count;
}

/**
 * @brief  Class driver setup callback (class/vendor-specific requests on EP0).
 *         Called within ISR context; time-consuming operations not permitted.
 *
 *         Routing policy:
 *           - Standard interface-recipient requests (GET_STATUS, CLEAR_FEATURE,
 *             SET_FEATURE, SET_INTERFACE, GET_INTERFACE, GET_DESCRIPTOR, ...):
 *             routed by wIndex to the sub-function whose interface range
 *             contains req->wIndex, and the wIndex passed down is rebased to
 *             the sub-function's local (0-based) interface number, so class
 *             drivers can use the same interface constants as in standalone
 *             mode (e.g. HID picks its Report Descriptor by the local
 *             interface number, independent of registration order).
 *             wIndex is consumed inside setup(), so the mutate/restore is safe.
 *           - Class requests with INTERFACE recipient: routed ONLY to the
 *             sub-function that owns the interface in the low byte of wIndex.
 *             This removes the reliance on the fragile "return non-RTK_SUCCESS for
 *             foreign requests" first-accept contract. For Audio Class the low
 *             byte of wIndex is still the AC interface number (the high byte is
 *             the Entity ID), so interface routing is correct; UAC dispatches
 *             internally by Entity ID.
 *           - Class requests with ENDPOINT/DEVICE recipient, vendor requests,
 *             and other standard requests: iterated across all sub-functions
 *             (first one to return RTK_SUCCESS wins). The standard endpoint halt
 *             requests offered by the core (SET_FEATURE/CLEAR_FEATURE with
 *             ENDPOINT_HALT) also land here: wIndex carries an endpoint address,
 *             not an interface number, so there is nothing to rebase and no
 *             owner lookup. A sub-function shall accept one only if the address
 *             in the low byte of wIndex is one of its own endpoints, otherwise
 *             it would swallow another function's halt request.
 *
 *         When a sub-function handles a request that has a data-OUT stage
 *         (bmRequestType direction = H2D, wLength > 0), its index is recorded
 *         in active_func so the subsequent ep0_data_out is routed to the same
 *         sub-function only. Symmetrically, a request with a data-IN stage
 *         (D2H, wLength > 0) records active_func_in for ep0_data_in.
 */
static int usbd_composite_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = -RTK_ERR_BADARG;
	u8 has_data_out;
	u8 has_data_in;
	u8 if_base;
	u8 target_if;
	u8 recipient;
	u16 saved_windex;
	u8 i;

	has_data_out = (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_H2D) && (req->wLength > 0U)) ? 1U : 0U;
	has_data_in = (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) && (req->wLength > 0U)) ? 1U : 0U;
	recipient = req->bmRequestType & USB_REQ_RECIPIENT_MASK;

	usbd_composite_reset_active_func();

	/* Interface-recipient standard reqs (GET_STATUS/CLEAR_FEATURE/SET_FEATURE/
	 * GET_INTERFACE/SET_INTERFACE/GET_DESCRIPTOR/...): route by wIndex, rebased
	 * to sub-function local. Recipient alone identifies these; no need to
	 * enumerate bRequest. */
	if (((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) &&
		(recipient == USB_REQ_RECIPIENT_INTERFACE)) {
		i = usbd_composite_find_owner_func(cdev, req->wIndex, &if_base);
		if (i < cdev->func_count) {
			driver = cdev->drivers[i];
			if (driver->setup != NULL) {
				saved_windex = req->wIndex;
				req->wIndex -= if_base;      /* absolute -> sub-function local */
				ret = driver->setup(dev, req);
				req->wIndex = saved_windex;  /* restore */
				if ((ret == RTK_SUCCESS) && (has_data_in != 0U)) {
					cdev->active_func_in = i;
				}
			}
		}
		return ret;
	}

	/* Class requests with INTERFACE recipient: single-cast to the owning sub-function.
	 * The interface number is the low byte of wIndex (true for all interface class
	 * requests, including UAC whose high byte carries the Entity ID). Routing to the
	 * single owner removes first-accept mis-routing (a foreign class driver can no
	 * longer swallow another function's class request).
	 *
	 * The low byte is rebased to the sub-function's local interface number, symmetric
	 * with the standard interface path above, so a class driver validating wIndex
	 * against its own interface constants (e.g. CDC NCM, whose every class request is
	 * addressed to the communication interface) accepts the request at any offset in
	 * the composite. The high byte is preserved: for Audio Class it carries the Entity
	 * ID, which is function-local already and must not be touched.
	 */
	if (((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) &&
		(recipient == USB_REQ_RECIPIENT_INTERFACE)) {
		target_if = (u8)(req->wIndex & 0xFFU);
		i = usbd_composite_find_owner_func(cdev, target_if, &if_base);
		if (i < cdev->func_count) {
			driver = cdev->drivers[i];
			if (driver->setup != NULL) {
				saved_windex = req->wIndex;
				/* if_base <= target_if is guaranteed by the owner lookup, so the low
				 * byte cannot borrow into the Entity ID. */
				req->wIndex = (u16)((saved_windex & 0xFF00U) | (u16)(target_if - if_base));
				ret = driver->setup(dev, req);
				req->wIndex = saved_windex;  /* restore */
				if (ret == RTK_SUCCESS) {
					if (has_data_out != 0U) {
						cdev->active_func = i;
					} else if (has_data_in != 0U) {
						cdev->active_func_in = i;
					} else {
						/* No data stage, nothing to route */
					}
				}
			}
		}
		return ret;
	}

	/* Remaining: class endpoint/device recipient, vendor, and other standard requests
	 * (first-accept). Kept for requests whose recipient is not an interface
	 * (e.g. UAC sampling-rate control addressed to an endpoint). */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->setup != NULL) {
			ret = driver->setup(dev, req);
			if (ret == RTK_SUCCESS) {
				if (has_data_out != 0U) {
					cdev->active_func = i;
				} else if (has_data_in != 0U) {
					cdev->active_func_in = i;
				} else {
					/* No data stage, nothing to route */
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
static int usbd_composite_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	int ret = -RTK_ERR_BADARG;
	u8 i;

	/* Dispatch to each sub-function */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->ep_data_in != NULL) {
			ret = driver->ep_data_in(dev, ep_addr, status);
			if (ret == RTK_SUCCESS) {
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
	int ret = -RTK_ERR_BADARG;
	u8 i;

	/* Dispatch to each sub-function */
	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->ep_data_out != NULL) {
			ret = driver->ep_data_out(dev, ep_addr, len);
			if (ret == RTK_SUCCESS) {
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
		if (driver->status_changed != NULL) {
			driver->status_changed(dev, old_status, status);
		}
	}

	/* Then: notify the composite-level user callback */
	cb = cdev->cb;
	if ((cb != NULL) && (cb->status_changed != NULL)) {
		cb->status_changed(old_status, status);
	}
}

/**
 * @brief  SOF callback — broadcast to all sub-functions.
 * @note   SOF is a broadcast event: EVERY sub-function that registered a SOF
 *         handler must receive it (e.g. UAC needs SOF for clock synchronisation).
 *         Do NOT stop on the first RTK_SUCCESS.
 */
static void usbd_composite_sof(usb_dev_t *dev)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	const usbd_class_driver_t *driver;
	u8 i;

	for (i = 0; i < cdev->func_count; i++) {
		driver = cdev->drivers[i];
		if (driver->sof != NULL) {
			driver->sof(dev);
		}
	}
}

/**
 * @brief  EP0 data IN complete — forwarded to the sub-function that handled
 *         the preceding setup request.
 * @note   Only the sub-function recorded by usbd_composite_setup() receives this
 *         callback, symmetric with usbd_composite_ep0_data_out(): the EP0 IN data
 *         stage belongs to exactly one request, and a class may have redirected
 *         dev->ep0_in.xfer_buf for it, which only its own handler can restore.
 */
static int usbd_composite_ep0_data_in(usb_dev_t *dev, int status)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 idx;

	idx = cdev->active_func_in;
	if ((idx < cdev->func_count) &&
		(cdev->drivers[idx]->ep0_data_in != NULL)) {
		cdev->active_func_in = USBD_COMP_ITF_NONE;
		return cdev->drivers[idx]->ep0_data_in(dev, status);
	}
	return -RTK_ERR_BADARG;
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
		(cdev->drivers[idx]->ep0_data_out != NULL)) {
		usbd_composite_reset_active_func();
		return cdev->drivers[idx]->ep0_data_out(dev);
	}
	return -RTK_ERR_BADARG;
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
		if (driver->wakeup != NULL) {
			driver->wakeup(dev);
		}
	}
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Register a class driver with the composite framework.
 * @note   Called from within each class driver's composite_init function.
 *         Repeated registration of the same driver pointer is idempotent.
 */
int usbd_composite_register_driver(const usbd_class_driver_t *driver)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;

	/* get_descriptor is the only callback invoked without a NULL check */
	if ((driver == NULL) || (driver->get_descriptor == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid driver\n");
		return -RTK_ERR_BADARG;
	}

	/* Avoid duplicate registration, before the capacity check so that it stays idempotent on a full table */
	for (u8 i = 0; i < cdev->func_count; i++) {
		if (cdev->drivers[i] == driver) {
			return RTK_SUCCESS;
		}
	}

	if (cdev->func_count >= USBD_COMP_MAX_FUNCS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Too many funcs (%u)\n", cdev->func_count);
		return -RTK_ERR_BADARG;
	}

	cdev->drivers[cdev->func_count] = driver;
	cdev->func_count++;
	return RTK_SUCCESS;
}

/**
 * @brief  Unregister a class driver from the composite framework.
 * @note   Called from within each class driver's deinit function when
 *         the driver was registered as part of a composite device.
 *         Refused while the composite device is registered with the core: the ISR walks
 *         drivers[] and if_counts[] on any core, so they are only changed while it is off.
 *         The class deinit then returns without releasing anything, refer to the header.
 * @retval RTK_SUCCESS on success, -RTK_ERR_BUSY while usbd_composite_deinit() has not been called
 */
int usbd_composite_unregister_driver(const usbd_class_driver_t *driver)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	u8 i;

	if (cdev->registered != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Unreg func while running\n");
		return -RTK_ERR_BUSY;
	}

	if (driver == NULL) {
		return RTK_SUCCESS;
	}

	for (i = 0; i < cdev->func_count; i++) {
		if (cdev->drivers[i] == driver) {
			/* Shift remaining entries to fill the gap, keeping if_counts[] aligned */
			cdev->func_count--;
			for (; i < cdev->func_count; i++) {
				cdev->drivers[i] = cdev->drivers[i + 1U];
				cdev->if_counts[i] = cdev->if_counts[i + 1U];
			}
			cdev->drivers[cdev->func_count] = NULL;
			cdev->if_counts[cdev->func_count] = 0U;
			usbd_composite_reset_active_func();
			break;
		}
	}

	return RTK_SUCCESS;
}

int usbd_composite_init(const usbd_composite_cb_t *cb)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	int unregister_ret;
	int ret;

	if (cdev->func_count == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No funcs registered\n");
		return -RTK_ERR_BADARG;
	}

	cdev->cb = cb;

	usbd_composite_reset_active_func();

	/* Set before the ISR can run, cleared by usbd_composite_deinit() once it is blocked */
	cdev->registered = 1U;

	ret = usbd_register_class(&usbd_composite_driver);
	if (ret != RTK_SUCCESS) {
		/* The guard is only dropped once the core really let the framework go, refer to
		   usbd_composite_unregister_driver() */
		unregister_ret = usbd_unregister_class();
		if (unregister_ret == RTK_SUCCESS) {
			cdev->registered = 0U;
		}
	}

	return ret;
}

int usbd_composite_deinit(void)
{
	usbd_composite_dev_t *cdev = &usbd_composite_dev;
	int ret;

	/* Refused in interrupt context: the ISR may still dispatch to the functions, so the
	   framework, and registered with it, is kept for the class deinit APIs to refuse too */
	ret = usbd_unregister_class();
	if (ret == RTK_SUCCESS) {
		usb_os_memset((void *)cdev, 0, sizeof(usbd_composite_dev_t));
	}

	return ret;
}
