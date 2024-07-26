/**
  ******************************************************************************
  * @file    usbd_msc.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USBD MSC
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_msc.h"
#include "usbd_scsi.h"
#include "os_wrapper.h"
#if defined(CONFIG_RTL8721D)
#include "rtl8721dhp_sd.h"
#else
#include "ameba_sd.h"
#endif

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_msc_set_config(usb_dev_t *dev, u8 config);
static int usbd_msc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_msc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_msc_status_changed(usb_dev_t *dev, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "MSC";

/* USB Standard Device Descriptor */
static u8 usbd_msc_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_msc_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_MSC_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_MSC_LANGID_STRING),
};  /* usbd_msc_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_msc_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_msc_hs_config_desc[USBD_MSC_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USBD_MSC_CONFIG_DESC_SIZE,                       /* wTotalLength */
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
static u8 usbd_msc_fs_config_desc[USBD_MSC_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USBD_MSC_CONFIG_DESC_SIZE,                       /* wTotalLength */
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
usbd_class_driver_t usbd_msc_driver = {
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

#if !USBD_MSC_RAM_DISK
#if defined(CONFIG_AMEBASMART)
static int usbd_msc_sd_init_status = 0;
static rtos_sema_t usbd_msc_sd_sema;

static SDIOHCFG_TypeDef sd_config = {
	.sdioh_bus_speed = SD_SPEED_HS,				//SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, 	//SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_cd_pin = _PC_0,						//_PC_0/_PNC
	.sdioh_wp_pin = _PNC,						//_PB_31/_PNC
};

#endif
#endif

/* Private functions ---------------------------------------------------------*/
#if USBD_MSC_RAM_DISK
static u8 *usbd_msc_ram_disk_buf;

static int RAM_init(void)
{
	int result = SD_OK;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	usbd_msc_ram_disk_buf = (u8 *)usb_os_malloc(USBD_MSC_RAM_DISK_SIZE);
	if (usbd_msc_ram_disk_buf != NULL) {
		cdev->is_ready = 1U;
	} else {
		RTK_LOGS(TAG, "[MSC] Alloc RAM disk buf fail");
		result = SD_NODISK;
	}

	return result;
}

static int RAM_deinit(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	cdev->is_ready = 0U;

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

#if defined(CONFIG_AMEBASMART)

static int usbd_msc_sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return rtos_sema_give(usbd_msc_sd_sema);
}

static int usbd_msc_sd_take_sema(u32 timeout)
{
	return rtos_sema_take(usbd_msc_sd_sema, timeout);
}

static void usbd_msc_sd_sema_init(void)
{
	rtos_sema_create(&usbd_msc_sd_sema, 0U, 1U);
	SD_SetSema(usbd_msc_sd_take_sema, usbd_msc_sd_give_sema);
}

static void usbd_msc_sd_sema_deinit(void)
{
	rtos_sema_delete(usbd_msc_sd_sema);
	SD_SetSema(NULL, NULL);
}

static int usbd_msc_sd_init(void)
{
	SD_RESULT ret;

	RTK_LOGS(TAG, "[MSC] SD init\n");

	usbd_msc_sd_sema_init();

	ret = SD_Init(&sd_config);
	if (ret == SD_OK) {
		usbd_msc_sd_init_status = 1;
	} else {
		RTK_LOGS(TAG, "[MSC] Fail to init SD: %d\n", ret);
	}

	return ret;
}

static int usbd_msc_sd_deinit(void)
{
	SD_RESULT ret;

	RTK_LOGS(TAG, "[MSC] SD deinit\n");

	usbd_msc_sd_init_status = 0;
	ret = SD_DeInit();
	if (ret != SD_OK) {
		RTK_LOGS(TAG, "[MSC] Fail to deinit SD: %d\n", ret);
	}

	usbd_msc_sd_sema_deinit();

	return ret;
}

static int usbd_msc_sd_getcapacity(u32 *sector_count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_GetCapacity(sector_count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= USBD_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG, "[MSC] Fail to get SD capacity: %d\n", ret);
	}

	return ret;
}

static int usbd_msc_sd_readblocks(u32 sector, u8 *data, u32 count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_ReadBlocks(sector, data, count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= USBD_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG, "[MSC] Fail to R SD blocks: %d\n", ret);
	}

	return ret;
}

static int usbd_msc_sd_writeblocks(u32 sector, const u8 *data, u32 count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_WriteBlocks(sector, data, count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= USBD_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG, "[MSC] Fail to W SD blocks: %d\n", ret);
	}

	return ret;
}

#endif // CONFIG_AMEBASMART

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
		usbd_ep_set_stall(dev, USBD_MSC_BULK_OUT_EP);
	}

	usbd_ep_set_stall(dev, USBD_MSC_BULK_IN_EP);

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
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(config);

	cdev->dev = dev;

	/* Init BULK IN EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, USBD_MSC_BULK_IN_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	/* Init BULK OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, USBD_MSC_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);

	cdev->bot_state = USBD_MSC_IDLE;
	cdev->bot_status = USBD_MSC_STATUS_NORMAL;
	cdev->scsi_sense_tail = 0U;
	cdev->scsi_sense_head = 0U;
	cdev->is_open = 1;
	cdev->ro = 0;
	cdev->phase_error = 0;

	cdev->is_ready = 1U;

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

	UNUSED(config);

	cdev->is_ready = 0U;

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, USBD_MSC_BULK_IN_EP);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, USBD_MSC_BULK_OUT_EP);

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
	int ret = HAL_OK;
	u16 ep_mps;

	//RTK_LOGD(TAG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	/* Standard request */
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
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
			/* DeInit EP */
			usbd_ep_deinit(dev, (u8)req->wIndex & 0xFF);
			if ((((u8)req->wIndex) & USB_REQ_DIR_MASK) == USB_D2H) {
				ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
				usbd_ep_init(dev, USBD_MSC_BULK_IN_EP, USB_CH_EP_TYPE_BULK, ep_mps);
			} else {
				ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
				usbd_ep_init(dev, USBD_MSC_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);
			}

			/* Handle BOT error */
			if (cdev->bot_status == USBD_MSC_STATUS_ERROR) { /* Bad CBW Signature */
				usbd_ep_set_stall(dev, USBD_MSC_BULK_IN_EP);
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
				cdev->ctrl_buf[0] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
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

	UNUSED(ep_addr);

	if (status == HAL_OK) {
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
		RTK_LOGS(TAG, "[MSC] EP%02x TX err: %d\n", ep_addr, status);
	}

	return HAL_OK;
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
	usbd_msc_cbw_t *cbw = cdev->cbw;
	usbd_msc_csw_t *csw = cdev->csw;

	UNUSED(ep_addr);

	switch (cdev->bot_state) {
	case USBD_MSC_IDLE:
		/* Decode the CBW command */
		csw->dCSWTag = cbw->dCBWTag;
		csw->dCSWDataResidue = cbw->dCBWDataTransferLength;

		if ((len != USBD_MSC_CB_WRAP_LEN) ||
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
			} else {
				return HAL_OK;
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

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  req: Setup request handle
  * @param  speed: Device speed
  * @param  len: Descriptor length
  * @retval Status
  */
static u8 *usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_msc_dev.ctrl_buf;

	dev->self_powered = USBD_MSC_SELF_POWERED;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		buf = usbd_msc_dev_desc;
		*len = sizeof(usbd_msc_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_msc_hs_config_desc;
			*len = sizeof(usbd_msc_hs_config_desc);
		} else {
			buf = usbd_msc_fs_config_desc;
			*len = sizeof(usbd_msc_fs_config_desc);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_msc_device_qualifier_desc;
		*len = sizeof(usbd_msc_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_msc_fs_config_desc;
			*len = sizeof(usbd_msc_fs_config_desc);
		} else {
			buf = usbd_msc_hs_config_desc;
			*len = sizeof(usbd_msc_hs_config_desc);
		}
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_msc_lang_id_desc;
			*len = sizeof(usbd_msc_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_MSC_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_MSC_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_MSC_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_MSC_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[MSC] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_msc_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdev->is_ready = 0;
	}

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(status);
	}
}

/* Exported functions --------------------------------------------------------*/
int usbd_msc_disk_init(void)
{
	SD_RESULT ret;

#if USBD_MSC_RAM_DISK
	ret = RAM_init();
#else
#if defined(CONFIG_AMEBASMART)
	ret = usbd_msc_sd_init();
#else
	ret = SD_Init();
#endif
#endif

	return ret;
}

int usbd_msc_disk_deinit(void)
{
	SD_RESULT ret;

#if USBD_MSC_RAM_DISK
	ret = RAM_deinit();
#else
#if defined(CONFIG_AMEBASMART)
	ret = usbd_msc_sd_deinit();
#else
	ret = SD_DeInit();
#endif
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
	int ret = HAL_OK;

	RTK_LOGS(TAG, "[MSC] Init\n");

	if (cb != NULL) {
		cdev->cb = cb;
	}

#if USBD_MSC_RAM_DISK
	ops->disk_getcapacity = RAM_GetCapacity;
	ops->disk_read = RAM_ReadBlocks;
	ops->disk_write = RAM_WriteBlocks;
#else
#if defined(CONFIG_AMEBASMART)
	ops->disk_getcapacity = usbd_msc_sd_getcapacity;
	ops->disk_read = usbd_msc_sd_readblocks;
	ops->disk_write = usbd_msc_sd_writeblocks;
#else
	ops->disk_getcapacity = SD_GetCapacity;
	ops->disk_read = SD_ReadBlocks;
	ops->disk_write = SD_WriteBlocks;
#endif
#endif

	cdev->ctrl_buf = (u8 *)usb_os_malloc(USBD_MSC_CTRL_BUF_SIZE);
	if (cdev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto ctrl_buf_fail;
	}

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

	cdev->blkbits = USBD_MSC_BLK_BITS;
	cdev->blksize = USBD_MSC_BLK_SIZE;

	usbd_register_class(&usbd_msc_driver);

	return HAL_OK;

csw_fail:
	usb_os_mfree(cdev->cbw);
	cdev->cbw = NULL;

cbw_fail:
	usb_os_mfree(cdev->data);
	cdev->data = NULL;

data_buf_fail:
	usb_os_mfree(cdev->ctrl_buf);
	cdev->ctrl_buf = NULL;

ctrl_buf_fail:
	return ret;
}

/**
  * @brief  De-Initialize MSC device
  * @retval Status
  */
void usbd_msc_deinit(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	RTK_LOGS(TAG, "[MSC] Deinit\n");

	cdev->is_ready = 0U;

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

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
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

	if (cdev->is_ready) {
		ret = usbd_ep_transmit(dev, USBD_MSC_BULK_IN_EP, buf, len);
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

	if (cdev->is_ready) {
		ret = usbd_ep_receive(dev, USBD_MSC_BULK_OUT_EP, buf, len);
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
	u16 ep_mps;
#endif

	csw->dCSWSignature = USBD_MSC_CS_SIGN;
	csw->bCSWStatus = status;
	cdev->bot_state = USBD_MSC_IDLE;

	usbd_msc_bulk_transmit(dev, (u8 *)csw, USBD_MSC_CS_WRAP_LEN);

#if USBD_MSC_FIX_CV_TEST_ISSUE
	/* Fix CV test failure */
	if (cdev->bot_status == USBD_MSC_STATUS_RECOVERY) {
		usbd_ep_deinit(dev, USBD_MSC_BULK_OUT_EP);
		ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
		usbd_ep_init(dev, USBD_MSC_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, ep_mps);
	}
#endif

	/* Prepare EP to Receive next Cmd */
	usbd_msc_bulk_receive(dev, (u8 *)cbw, USBD_MSC_CB_WRAP_LEN);
}

