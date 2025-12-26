/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_msc.h"
#include "usbd_scsi.h"
#include "os_wrapper.h"
#include "ameba_sd.h"
#if !USBD_MSC_RAM_DISK
#include "vfs_fatfs.h"
#endif
/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_msc_set_config(usb_dev_t *dev, u8 config);
static int usbd_msc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_msc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_msc_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

/* USB Standard Device Descriptor */
static const u8 usbd_msc_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_MSC_VID),                      /* idVendor */
	USB_HIGH_BYTE(USBD_MSC_VID),
	USB_LOW_BYTE(USBD_MSC_PID),                      /* idProduct */
	USB_HIGH_BYTE(USBD_MSC_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_msc_dev_desc */

/* USB Standard String Descriptor 0 */
static const u8 usbd_msc_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_MSC_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_MSC_LANGID_STRING),
};  /* usbd_msc_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_msc_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};  /* usbd_msc_device_qualifier_desc */

/* USB MSC Device High Speed Configuration Descriptor */
static const u8 usbd_msc_hs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00,                                           /* wTotalLength, runtime assigned */
	0x00,
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_MSC_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/********************  Mass Storage interface ********************/
	USB_LEN_IF_DESC,			/* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType: */
	0x00,						/* bInterfaceNumber: Number of Interface */
	0x00,						/* bAlternateSetting: Alternate setting */
	0x02,						/* bNumEndpoints*/
	0x08,						/* bInterfaceClass: MSC Class */
	0x06,						/* bInterfaceSubClass : SCSI transparent*/
	0x50,						/* nInterfaceProtocol */
	0x00,						/* iInterface: */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USBD_MSC_BULK_OUT_EP,		/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USBD_MSC_BULK_IN_EP,		/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_msc_hs_config_desc */

/* USB MSC Device Full Speed Configuration Descriptor */
static const u8 usbd_msc_fs_config_desc[] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	0x00,                                           /* wTotalLength, runtime assigned */
	0x00,
	0x01,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_MSC_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/********************  Mass Storage interface ********************/
	USB_LEN_IF_DESC,			/* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType: */
	0x00,						/* bInterfaceNumber: Number of Interface */
	0x00,						/* bAlternateSetting: Alternate setting */
	0x02,						/* bNumEndpoints*/
	0x08,						/* bInterfaceClass: MSC Class */
	0x06,						/* bInterfaceSubClass : SCSI transparent*/
	0x50,						/* nInterfaceProtocol */
	0x00,						/* iInterface: */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USBD_MSC_BULK_OUT_EP,		/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USBD_MSC_BULK_IN_EP,		/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_msc_fs_config_desc */

/* MSC Class Driver */
static const usbd_class_driver_t usbd_msc_driver = {
	.get_descriptor = usbd_msc_get_descriptor,
	.set_config = usbd_msc_set_config,
	.clear_config = usbd_msc_clear_config,
	.setup = usbd_msc_setup,
	.ep_data_in = usbd_msc_handle_ep_data_in,
	.ep_data_out = usbd_msc_handle_ep_data_out,
	.status_changed = usbd_msc_status_changed,
};

// physical disk access methods
static usbd_msc_dev_t usbd_msc_dev;

/* Add lock to avoid msc tx_thread preempts msc rx_thread when read SD-card*/
static usb_os_lock_t usbd_msc_sd_lock = NULL;

/* Private functions ---------------------------------------------------------*/
#if USBD_MSC_RAM_DISK
static u8 *usbd_msc_ram_disk_buf;

static int RAM_init(void)
{
	int result = HAL_OK;

	usbd_msc_ram_disk_buf = (u8 *)usb_os_malloc(USBD_MSC_RAM_DISK_SIZE);
	if (usbd_msc_ram_disk_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc RAM disk buf fail");
		result = HAL_ERR_MEM;
	}

	return result;
}

static int RAM_deinit(void)
{
	if (usbd_msc_ram_disk_buf != NULL) {
		usb_os_mfree((void *)usbd_msc_ram_disk_buf);
		usbd_msc_ram_disk_buf = NULL;
	}

	return HAL_OK;
}

static int RAM_GetCapacity(u32 *sector_count)
{
	*sector_count = USBD_MSC_RAM_DISK_SECTORS;
	return HAL_OK;
}

static int RAM_ReadBlocks(u32 sector, u8 *data, u32 count)
{
	int result = HAL_ERR_PARA;
	if (sector + count <= USBD_MSC_RAM_DISK_SECTORS) {
		usb_os_memcpy((void *)data, (void *)(usbd_msc_ram_disk_buf + sector * USBD_MSC_BLK_SIZE), count * USBD_MSC_BLK_SIZE);
		result = HAL_OK;
	}
	return result;
}

static int RAM_WriteBlocks(u32 sector, const u8 *data, u32 count)
{
	int result = HAL_ERR_PARA;
	if (sector + count <= USBD_MSC_RAM_DISK_SECTORS) {
		usb_os_memcpy((void *)(usbd_msc_ram_disk_buf + sector * USBD_MSC_BLK_SIZE), (void *)data, count * USBD_MSC_BLK_SIZE);
		result = HAL_OK;
	}
	return result;
}

#else

static int usbd_msc_sd_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SD init\n");

#ifdef CONFIG_FATFS_SECONDARY_FLASH
	return FLASH_disk_secondary_Driver.disk_initialize();
#else
	return SD_disk_Driver.disk_initialize();
#endif
}

static int usbd_msc_sd_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SD deinit\n");

#ifdef CONFIG_FATFS_SECONDARY_FLASH
	return FLASH_disk_secondary_Driver.disk_deinitialize();
#else
	return SD_disk_Driver.disk_deinitialize();
#endif
}

static int usbd_msc_sd_getcapacity(u32 *sector_count)
{
#ifdef CONFIG_FATFS_SECONDARY_FLASH
	return FLASH_disk_secondary_Driver.disk_ioctl(GET_SECTOR_COUNT, sector_count);
#else
	return SD_disk_Driver.disk_ioctl(GET_SECTOR_COUNT, sector_count);
#endif
}

static int usbd_msc_sd_readblocks(u32 sector, u8 *data, u32 count)
{
#ifdef CONFIG_FATFS_SECONDARY_FLASH
	return FLASH_disk_secondary_Driver.disk_read(data, sector, count);
#else
	return SD_disk_Driver.disk_read(data, sector, count);
#endif
}

static int usbd_msc_sd_writeblocks(u32 sector, const u8 *data, u32 count)
{
#ifdef CONFIG_FATFS_SECONDARY_FLASH
	return FLASH_disk_secondary_Driver.disk_write(data, sector, count);
#else
	return SD_disk_Driver.disk_write(data, sector, count);
#endif
}

#endif // USBD_MSC_RAM_DISK

/**
* @brief  Abort the current transfer
* @param  dev: device instance
* @retval Status
*/
static void usbd_msc_abort(usb_dev_t *dev)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_msc_cbw_t *cbw = cdev->cbw;

	if ((cbw->bmCBWFlags == 0U) &&
		(cbw->dCBWDataTransferLength != 0U) &&
		(cdev->bot_status == USBD_MSC_STATUS_NORMAL)) {
		usbd_ep_set_stall(dev, &cdev->ep_bulk_out);
	}

	usbd_ep_set_stall(dev, &cdev->ep_bulk_in);

	if (cdev->bot_status == USBD_MSC_STATUS_ERROR) {
		usbd_msc_bulk_receive(dev, (u8 *)cbw, USBD_MSC_CB_WRAP_LEN);
	}
}

/**
  * @brief  Set MSC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_msc_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	UNUSED(config);

	cdev->dev = dev;

	/* Init BULK IN EP */
	ep_bulk_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_in);

	/* Init BULK OUT EP */
	ep_bulk_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_out);

	cdev->bot_state = USBD_MSC_IDLE;
	cdev->bot_status = USBD_MSC_STATUS_NORMAL;
	cdev->scsi_sense_tail = 0U;
	cdev->scsi_sense_head = 0U;
	cdev->is_open = 1;
	cdev->ro = 0;
	cdev->phase_error = 0;

	/* Prepare to receive next BULK OUT packet */
	usbd_msc_bulk_receive(dev, (u8 *)cdev->cbw, USBD_MSC_CB_WRAP_LEN);

	return ret;
}

/**
  * @brief  Clear MSC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_msc_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	UNUSED(config);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, ep_bulk_in);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, ep_bulk_out);

	cdev->bot_state  = USBD_MSC_IDLE;
	cdev->is_open = 0;
	cdev->phase_error = 0;

	return ret;
}

/**
  * @brief  Handle MSC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_msc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	int ret = HAL_OK;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	/* Standard request */
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_CLEAR_FEATURE:

			ep_bulk_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
			ep_bulk_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;

			if ((((u8)req->wIndex) & USB_REQ_DIR_MASK) == USB_D2H) {
				usbd_ep_deinit(dev, ep_bulk_in);
				usbd_ep_init(dev, ep_bulk_in);
			} else {
				usbd_ep_deinit(dev, ep_bulk_out);
				usbd_ep_init(dev, ep_bulk_out);
			}

			/* Handle BOT error */
			if (cdev->bot_status == USBD_MSC_STATUS_ERROR) { /* Bad CBW Signature */
				usbd_ep_set_stall(dev, ep_bulk_in);
				cdev->bot_status = USBD_MSC_STATUS_NORMAL;
			} else if (((((u8)req->wIndex) & USB_REQ_DIR_MASK) == USB_D2H) && (cdev->bot_status != USBD_MSC_STATUS_RECOVERY)) {
				usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_FAILED);
			} else {
				// Do nothing
			}
			break;

		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	/* Class request */
	case USB_REQ_TYPE_CLASS:
		switch (req->bRequest) {
		case USBD_MSC_REQUEST_GET_MAX_LUN:
			if ((req->wValue  == 0U) && (req->wLength == 1U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USBD_MSC_REQUEST_RESET :
			if ((req->wValue  == 0U) && (req->wLength == 0U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				cdev->bot_state  = USBD_MSC_IDLE;
				cdev->bot_status = USBD_MSC_STATUS_RECOVERY;
				/* Prepare to receive BOT cmd */
				usbd_msc_bulk_receive(dev, (u8 *)cdev->cbw, USBD_MSC_CB_WRAP_LEN);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;

	default:
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);
	UNUSED(ep_addr);

	cdev->tx_status = status;
	rtos_sema_give(cdev->tx_sema);

	return HAL_OK;
}

/**
  * @brief  RX process
  * @retval Status
  */
static void usbd_msc_tx_process(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usb_dev_t *dev = cdev->dev;

	usb_os_lock(usbd_msc_sd_lock);

	if (cdev->tx_status == HAL_OK) {
		switch (cdev->bot_state) {
		case USBD_MSC_DATA_IN:
			if (usbd_scsi_process_cmd(cdev, &cdev->cbw->CBWCB[0]) < 0) {
				usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_FAILED);
			}
			break;

		case USBD_MSC_SEND_DATA:
		case USBD_MSC_LAST_DATA_IN:
			if (cdev->phase_error == 1) {
				usbd_msc_send_csw(dev, USBD_MSC_CSW_PHASE_ERROR);
				cdev->phase_error = 0;
			} else {
				usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_PASSED);
			}
			break;

		default:
			break;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX err: %d\n", cdev->tx_status);
	}

	usb_os_unlock(usbd_msc_sd_lock);
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);
	UNUSED(ep_addr);

	cdev->rx_data_length = len;
	rtos_sema_give(cdev->rx_sema);

	return HAL_OK;
}

/**
  * @brief  RX process
  * @retval void
  */
static void usbd_msc_rx_process(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_msc_cbw_t *cbw = cdev->cbw;
	usbd_msc_csw_t *csw = cdev->csw;
	usb_dev_t *dev = cdev->dev;

	usb_os_lock(usbd_msc_sd_lock);

	switch (cdev->bot_state) {
	case USBD_MSC_IDLE:
		/* Decode the CBW command */
		csw->dCSWTag = cbw->dCBWTag;
		csw->dCSWDataResidue = cbw->dCBWDataTransferLength;

		if ((cdev->rx_data_length != USBD_MSC_CB_WRAP_LEN) ||
			(cbw->dCBWSignature != USBD_MSC_CB_SIGN) ||
			(cbw->bCBWLUN > 1U) ||
			(cbw->bCBWCBLength < 1U) || (cbw->bCBWCBLength > 16U)) {
			usbd_scsi_sense_code(cdev, ILLEGAL_REQUEST, INVALID_CDB);
			cdev->bot_status = USBD_MSC_STATUS_ERROR;
			usbd_msc_abort(dev);
		} else {
			if (usbd_scsi_process_cmd(cdev, &cbw->CBWCB[0]) < 0) {
				if (cdev->phase_error == 1) {
					usbd_msc_send_csw(dev, USBD_MSC_CSW_PHASE_ERROR);
					cdev->phase_error = 0;
				} else if (cdev->bot_state == USBD_MSC_NO_DATA) {
					usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_FAILED);
				} else {
					usbd_msc_abort(dev);
				}
			}
			/*Burst xfer handled internally*/
			else if ((cdev->bot_state != USBD_MSC_DATA_IN) &&
					 (cdev->bot_state != USBD_MSC_DATA_OUT) &&
					 (cdev->bot_state != USBD_MSC_LAST_DATA_IN)) {
				if (cdev->data_length > 0U) {
					u16 length = (u16)MIN(cbw->dCBWDataTransferLength, cdev->data_length);
					csw->dCSWDataResidue -= cdev->data_length;
					csw->bCSWStatus = USBD_MSC_CSW_CMD_PASSED;
					cdev->bot_state = USBD_MSC_SEND_DATA;

					usbd_msc_bulk_transmit(dev, cdev->data, length);
				} else if (cdev->data_length == 0U) {
					usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_PASSED);
				} else {
					usbd_msc_abort(dev);
				}
			}
		}
		break;

	case USBD_MSC_DATA_OUT:
		if (usbd_scsi_process_cmd(cdev, &cbw->CBWCB[0]) < 0) {
			usbd_msc_send_csw(dev, USBD_MSC_CSW_CMD_FAILED);
		}

		break;

	default:
		break;
	}

	usb_os_unlock(usbd_msc_sd_lock);
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	u16 len = 0;

	dev->self_powered = USBD_MSC_SELF_POWERED;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_msc_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_msc_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_msc_hs_config_desc;
			len = sizeof(usbd_msc_hs_config_desc);
		} else {
			desc = (u8 *)usbd_msc_fs_config_desc;
			len = sizeof(usbd_msc_fs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_msc_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_msc_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_msc_fs_config_desc;
			len = sizeof(usbd_msc_fs_config_desc);
		} else {
			desc = (u8 *)usbd_msc_hs_config_desc;
			len = sizeof(usbd_msc_hs_config_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_msc_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_msc_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_MSC_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_MSC_PROD_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_MSC_PROD_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_MSC_SN_STRING, buf);
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			//RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid str idx %d\n", USB_LOW_BYTE(req->wValue));
			break;
		}
		break;

	default:
		break;
	}

	return len;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_msc_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(old_status, status);
	}
}

static void usbd_msc_rx_thread(void *param)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdev->rx_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbd_msc_rx_process();
		}
	}
}

static void usbd_msc_tx_thread(void *param)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdev->tx_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbd_msc_tx_process();
		}
	}
}

/* Exported functions --------------------------------------------------------*/

int usbd_msc_disk_init(void)
{
	int ret;

#if USBD_MSC_RAM_DISK
	ret = RAM_init();
#else
	ret = usbd_msc_sd_init();
#endif

	return ret;
}

int usbd_msc_disk_deinit(void)
{
	int ret;

#if USBD_MSC_RAM_DISK
	ret = RAM_deinit();
#else
	ret = usbd_msc_sd_deinit();
#endif

	return ret;
}

/**
  * @brief  Initialize MSC device
  * @retval Status
  */
int usbd_msc_init(usbd_msc_cb_t *cb)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_msc_disk_ops_t *ops = &cdev->disk_ops;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	int ret = HAL_OK;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init\n");

	if (cb != NULL) {
		cdev->cb = cb;
	}

#if USBD_MSC_RAM_DISK
	ops->disk_getcapacity = RAM_GetCapacity;
	ops->disk_read = RAM_ReadBlocks;
	ops->disk_write = RAM_WriteBlocks;
#else
	ops->disk_getcapacity = usbd_msc_sd_getcapacity;
	ops->disk_read = usbd_msc_sd_readblocks;
	ops->disk_write = usbd_msc_sd_writeblocks;
#endif

	usb_os_lock_create(&usbd_msc_sd_lock);

	cdev->data = (u8 *)usb_os_malloc(USBD_MSC_BUFLEN);
	if (cdev->data == NULL) {
		ret = HAL_ERR_MEM;
		goto data_buf_fail;
	}

	cdev->cbw = (usbd_msc_cbw_t *)usb_os_malloc(USBD_MSC_CB_WRAP_LEN);
	if (cdev->cbw == NULL) {
		ret = HAL_ERR_MEM;
		goto cbw_fail;
	}

	cdev->csw = (usbd_msc_csw_t *)usb_os_malloc(USBD_MSC_CS_WRAP_LEN);
	if (cdev->csw == NULL) {
		ret = HAL_ERR_MEM;
		goto csw_fail;
	}

	rtos_sema_create(&cdev->rx_sema, 0U, 1U);
	rtos_sema_create(&cdev->tx_sema, 0U, 1U);

	ret = rtos_task_create(&cdev->rx_task, "usbd_msc_rx_thread", usbd_msc_rx_thread, NULL, 1024U, USBD_MSC_RX_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		goto create_rx_thread_fail;
	}

	ret = rtos_task_create(&cdev->tx_task, "usbd_msc_tx_thread", usbd_msc_tx_thread, NULL, 1024U, USBD_MSC_TX_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX thread fail\n");
		goto create_tx_thread_fail;
	}

	cdev->blkbits = USBD_MSC_BLK_BITS;
	cdev->blksize = USBD_MSC_BLK_SIZE;

	ep_bulk_in->addr = USBD_MSC_BULK_IN_EP;
	ep_bulk_in->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->dis_zlp = 1;

	ep_bulk_out->addr = USBD_MSC_BULK_OUT_EP;
	ep_bulk_out->type = USB_CH_EP_TYPE_BULK;

	usbd_register_class(&usbd_msc_driver);

	return HAL_OK;

create_tx_thread_fail:
	rtos_task_delete(cdev->rx_task);

create_rx_thread_fail:
	rtos_sema_delete(cdev->tx_sema);
	rtos_sema_delete(cdev->rx_sema);

csw_fail:
	usb_os_mfree(cdev->cbw);
	cdev->cbw = NULL;

cbw_fail:
	usb_os_mfree(cdev->data);
	cdev->data = NULL;

data_buf_fail:
	return ret;
}

/**
  * @brief  De-Initialize MSC device
  * @retval Status
  */
void usbd_msc_deinit(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit\n");

	rtos_task_delete(cdev->tx_task);
	rtos_task_delete(cdev->rx_task);

	rtos_sema_delete(cdev->tx_sema);
	rtos_sema_delete(cdev->rx_sema);

	usbd_unregister_class();

	if (cdev->csw != NULL) {
		usb_os_mfree(cdev->csw);
		cdev->csw = NULL;
	}

	if (cdev->cbw != NULL) {
		usb_os_mfree(cdev->cbw);
		cdev->cbw = NULL;
	}

	if (cdev->data != NULL) {
		usb_os_mfree(cdev->data);
		cdev->data = NULL;
	}

	if (usbd_msc_sd_lock != NULL) {
		usb_os_lock_delete(usbd_msc_sd_lock);
		usbd_msc_sd_lock = NULL;
	}
}

/**
* @brief  BULK transmit
* @param  dev: device instance
* @param  buf: data buffer
* @param  len: data length
* @retval status
*/
int usbd_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;

	if (dev->is_ready) {
		ep_bulk_in->xfer_buf = buf;
		ep_bulk_in->xfer_len = len;
		ret = usbd_ep_transmit(dev, ep_bulk_in);
	}

	return ret;
}

/**
* @brief  BULK receive
* @param  dev: device instance
* @param  buf: data buffer
* @param  len: data length
* @retval status
*/
int usbd_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u16 len)
{
	int ret = HAL_ERR_HW;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	if (dev->is_ready) {
		ep_bulk_out->xfer_buf = buf;
		ep_bulk_out->xfer_len = len;
		ret = usbd_ep_receive(dev, ep_bulk_out);
	}

	return ret;
}

/**
* @brief  Send the Command Status Wrapper
* @param  dev: device instance
* @param  status: CSW status
* @retval None
*/
void usbd_msc_send_csw(usb_dev_t *dev, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_msc_cbw_t *cbw = cdev->cbw;
	usbd_msc_csw_t *csw = cdev->csw;
#if USBD_MSC_FIX_CV_TEST_ISSUE
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
#endif

	csw->dCSWSignature = USBD_MSC_CS_SIGN;
	csw->bCSWStatus = status;
	cdev->bot_state = USBD_MSC_IDLE;

	usbd_msc_bulk_transmit(dev, (u8 *)csw, USBD_MSC_CS_WRAP_LEN);

#if USBD_MSC_FIX_CV_TEST_ISSUE
	/* Fix CV test failure */
	if (cdev->bot_status == USBD_MSC_STATUS_RECOVERY) {
		usbd_ep_deinit(dev, ep_bulk_out);
		usbd_ep_init(dev, ep_bulk_out);
	}
#endif

	/* Prepare EP to Receive next Cmd */
	usbd_msc_bulk_receive(dev, (u8 *)cbw, USBD_MSC_CB_WRAP_LEN);
}

