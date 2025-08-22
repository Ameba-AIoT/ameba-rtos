/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usb_hal.h"
#include "usbd_composite_config.h"
#include "usbd_composite_msc.h"
#include "usbd_composite_scsi.h"
#include "os_wrapper.h"
#include "ameba_sd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_composite_msc_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_msc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_msc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_composite_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "COMP";

/* USB MSC Device High Speed Interface Descriptor */
static const u8 usbd_composite_msc_hs_itf_desc[] = {
	/********************  Mass Storage interface ********************/
	USB_LEN_IF_DESC,			/* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType: */
	USBD_COMP_MSC_ITF,			/* bInterfaceNumber: Number of Interface */
	0x00,						/* bAlternateSetting: Alternate setting */
	0x02,						/* bNumEndpoints*/
	0x08,						/* bInterfaceClass: MSC Class */
	0x06,						/* bInterfaceSubClass : SCSI transparent*/
	0x50,						/* nInterfaceProtocol */
	USBD_IDX_MSC_ITF_STR,		/* iInterface: */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType */
	USBD_COMP_MSC_BULK_OUT_EP,	/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(COMP_MSC_HS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType */
	USBD_COMP_MSC_BULK_IN_EP,		/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(COMP_MSC_HS_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_composite_msc_hs_itf_desc */

/* USB MSC Device Full Speed Interface Descriptor */
static const u8 usbd_composite_msc_fs_itf_desc[] = {
	/********************  Mass Storage interface ********************/
	USB_LEN_IF_DESC,			/* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,	/* bDescriptorType: */
	USBD_COMP_MSC_ITF,			/* bInterfaceNumber: Number of Interface */
	0x00,						/* bAlternateSetting: Alternate setting */
	0x02,						/* bNumEndpoints*/
	0x08,						/* bInterfaceClass: MSC Class */
	0x06,						/* bInterfaceSubClass : SCSI transparent*/
	0x50,						/* nInterfaceProtocol */
	USBD_IDX_MSC_ITF_STR,		/* iInterface: */

	/* BULK OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType */
	USBD_COMP_MSC_BULK_OUT_EP,	/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_MSC_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(COMP_MSC_FS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,		/* bDescriptorType */
	USBD_COMP_MSC_BULK_IN_EP,	/* bEndpointAddress */
	0x02,						/* bmAttributes: BULK */
	USB_LOW_BYTE(COMP_MSC_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(COMP_MSC_FS_MAX_PACKET_SIZE),
	0x00                        /* bInterval */
};  /* usbd_composite_msc_fs_itf_desc */

// physical disk access methods
static usbd_composite_msc_dev_t usbd_composite_msc_dev;

#if !COMP_MSC_RAM_DISK
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
static int usbd_composite_msc_sd_init_status = 0;
static rtos_sema_t usbd_composite_msc_sd_sema;

static const SDIOHCFG_TypeDef usbd_composite_sd_config = {
	.sdioh_bus_speed = SD_SPEED_HS,				//SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, 	//SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_cd_pin = _PC_0,						//_PC_0/_PNC
	.sdioh_wp_pin = _PNC,						//_PB_31/_PNC
};

#endif
#endif

/* Exported variables --------------------------------------------------------*/

/* MSC Class Driver */
const usbd_class_driver_t usbd_composite_msc_driver = {
	.get_descriptor = usbd_composite_msc_get_descriptor,
	.set_config = usbd_composite_msc_set_config,
	.clear_config = usbd_composite_msc_clear_config,
	.setup = usbd_composite_msc_setup,
	.ep_data_in = usbd_composite_msc_handle_ep_data_in,
	.ep_data_out = usbd_composite_msc_handle_ep_data_out,
};

/* Private functions ---------------------------------------------------------*/
#if COMP_MSC_RAM_DISK
static u8 *usbd_composite_msc_ram_disk_buf;

static int RAM_init(void)
{
	int result = SD_OK;

	usbd_composite_msc_ram_disk_buf = (u8 *)usb_os_malloc(COMP_MSC_RAM_DISK_SIZE);
	if (usbd_composite_msc_ram_disk_buf == NULL) {
		RTK_LOGS(TAG,  RTK_LOG_ERROR, "Alloc RAM disk buf fail");
		result = SD_NODISK;
	}

	return result;
}

static int RAM_deinit(void)
{
	if (usbd_composite_msc_ram_disk_buf != NULL) {
		usb_os_mfree((void *)usbd_composite_msc_ram_disk_buf);
		usbd_composite_msc_ram_disk_buf = NULL;
	}

	return HAL_OK;
}

static int RAM_GetCapacity(u32 *sector_count)
{
	*sector_count = COMP_MSC_RAM_DISK_SECTORS;
	return HAL_OK;
}

static int RAM_ReadBlocks(u32 sector, u8 *data, u32 count)
{
	int result = HAL_ERR_PARA;
	if (sector + count <= COMP_MSC_RAM_DISK_SECTORS) {
		usb_os_memcpy((void *)data, (void *)(usbd_composite_msc_ram_disk_buf + sector * COMP_MSC_BLK_SIZE), count * COMP_MSC_BLK_SIZE);
		result = HAL_OK;
	}
	return result;
}

static int RAM_WriteBlocks(u32 sector, const u8 *data, u32 count)
{
	int result = HAL_ERR_PARA;
	if (sector + count <= COMP_MSC_RAM_DISK_SECTORS) {
		usb_os_memcpy((void *)(usbd_composite_msc_ram_disk_buf + sector * COMP_MSC_BLK_SIZE), (void *)data, count * COMP_MSC_BLK_SIZE);
		result = HAL_OK;
	}
	return result;
}

#else

#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)

static int usbd_composite_msc_sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return rtos_sema_give(usbd_composite_msc_sd_sema);
}

static int usbd_composite_msc_sd_take_sema(u32 timeout)
{
	return rtos_sema_take(usbd_composite_msc_sd_sema, timeout);
}

static void usbd_composite_msc_sd_sema_init(void)
{
	rtos_sema_create(&usbd_composite_msc_sd_sema, 0U, 1U);
	SD_SetSema(usbd_composite_msc_sd_take_sema, usbd_composite_msc_sd_give_sema);
}

static void usbd_composite_msc_sd_sema_deinit(void)
{
	rtos_sema_delete(usbd_composite_msc_sd_sema);
	SD_SetSema(NULL, NULL);
}

static int usbd_composite_msc_sd_init(void)
{
	SD_RESULT ret;

	RTK_LOGS(TAG,  RTK_LOG_DEBUG, "SD init\n");

	usbd_composite_msc_sd_sema_init();

	ret = SD_Init((SDIOHCFG_TypeDef *)&usbd_composite_sd_config);
	if (ret == SD_OK) {
		usbd_composite_msc_sd_init_status = 1;
	} else {
		RTK_LOGS(TAG,  RTK_LOG_ERROR, "Fail to init SD: %d\n", ret);
	}

	return ret;
}

static int usbd_composite_msc_sd_deinit(void)
{
	SD_RESULT ret;

	RTK_LOGS(TAG,  RTK_LOG_DEBUG, "SD deinit\n");

	usbd_composite_msc_sd_init_status = 0;
	ret = SD_DeInit();
	if (ret != SD_OK) {
		RTK_LOGS(TAG,  RTK_LOG_WARN, "Fail to deinit SD: %d\n", ret);
	}

	usbd_composite_msc_sd_sema_deinit();

	return ret;
}

static int usbd_composite_msc_sd_getcapacity(u32 *sector_count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_composite_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_GetCapacity(sector_count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= COMP_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG,  RTK_LOG_WARN, "Fail to get SD capacity: %d\n", ret);
	}

	return ret;
}

static int usbd_composite_msc_sd_readblocks(u32 sector, u8 *data, u32 count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_composite_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_ReadBlocks(sector, data, count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= COMP_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG,  RTK_LOG_WARN, "Fail to R SD blocks: %d\n", ret);
	}

	return ret;
}

static int usbd_composite_msc_sd_writeblocks(u32 sector, const u8 *data, u32 count)
{
	u32 retry = 0U;
	SD_RESULT ret;

	do {
		if (usbd_composite_msc_sd_init_status == 0U) {
			ret = SD_NODISK;
			break;
		}

		ret = SD_WriteBlocks(sector, data, count);
		if (ret == SD_OK) {
			break;
		}
	} while (++retry <= COMP_MSC_SD_ACCESS_RETRY);

	if (ret != SD_OK) {
		RTK_LOGS(TAG,  RTK_LOG_WARN, "Fail to W SD blocks: %d\n", ret);
	}

	return ret;
}

#endif // CONFIG_AMEBASMART

#endif // COMP_MSC_RAM_DISK

/**
* @brief  Abort the current transfer
* @param  dev: device instance
* @retval Status
*/
static void usbd_composite_msc_abort(usb_dev_t *dev)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;

	if ((cbw->bmCBWFlags == 0U) &&
		(cbw->dCBWDataTransferLength != 0U) &&
		(mdev->bot_status == COMP_MSC_STATUS_NORMAL)) {
		usbd_ep_set_stall(dev, ep_bulk_out);
	}

	usbd_ep_set_stall(dev, ep_bulk_in);

	if (mdev->bot_status == COMP_MSC_STATUS_ERROR) {
		usbd_composite_msc_bulk_receive(dev, (u8 *)cbw, COMP_MSC_CB_WRAP_LEN);
	}
}

/**
  * @brief  Set MSC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_msc_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;

	UNUSED(config);

	mdev->dev = dev;

	/* Init BULK IN EP */
	ep_bulk_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ? COMP_MSC_HS_MAX_PACKET_SIZE : COMP_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_in);

	/* Init BULK OUT EP */
	ep_bulk_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ? COMP_MSC_HS_MAX_PACKET_SIZE : COMP_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_out);

	mdev->bot_state = COMP_MSC_IDLE;
	mdev->bot_status = COMP_MSC_STATUS_NORMAL;
	mdev->scsi_sense_tail = 0U;
	mdev->scsi_sense_head = 0U;
	mdev->is_open = 1;
	mdev->ro = 0;
	mdev->phase_error = 0;

	/* Prepare to receive next BULK OUT packet */
	usbd_composite_msc_bulk_receive(dev, (u8 *)mdev->cbw, COMP_MSC_CB_WRAP_LEN);

	return ret;
}

/**
  * @brief  Clear MSC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_msc_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;

	UNUSED(config);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, ep_bulk_in);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, ep_bulk_out);

	mdev->bot_state  = COMP_MSC_IDLE;
	mdev->is_open = 0;
	mdev->phase_error = 0;

	return ret;
}

/**
  * @brief  Handle MSC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_composite_msc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;
	int ret = HAL_OK;
	u8 ep_addr;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	/* Standard request */
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_CLEAR_FEATURE:
			/* DeInit EP */
			ep_addr = (u8)req->wIndex & 0xFF;
			if (ep_addr == ep_bulk_out->addr) {
				usbd_ep_deinit(dev, ep_bulk_out);
			} else if (ep_addr == ep_bulk_in->addr) {
				usbd_ep_deinit(dev, ep_bulk_in);
			}

			if ((((u8)req->wIndex) & USB_REQ_DIR_MASK) == USB_D2H) {
				usbd_ep_init(dev, ep_bulk_in);
			} else {
				usbd_ep_init(dev, ep_bulk_out);
			}

			/* Handle BOT error */
			if (mdev->bot_status == COMP_MSC_STATUS_ERROR) { /* Bad CBW Signature */
				usbd_ep_set_stall(dev, ep_bulk_in);
				mdev->bot_status = COMP_MSC_STATUS_NORMAL;
			} else if (((((u8)req->wIndex) & USB_REQ_DIR_MASK) == USB_D2H) && (mdev->bot_status != COMP_MSC_STATUS_RECOVERY)) {
				usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_FAILED);
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
		case COMP_MSC_REQUEST_GET_MAX_LUN:
			if ((req->wValue  == 0U) && (req->wLength == 1U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case COMP_MSC_REQUEST_RESET:
			if ((req->wValue  == 0U) && (req->wLength == 0U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				mdev->bot_state  = COMP_MSC_IDLE;
				mdev->bot_status = COMP_MSC_STATUS_RECOVERY;
				/* Prepare to receive BOT cmd */
				usbd_composite_msc_bulk_receive(dev, (u8 *)mdev->cbw, COMP_MSC_CB_WRAP_LEN);
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
static int usbd_composite_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;

	UNUSED(ep_addr);

	if (status == HAL_OK) {
		switch (mdev->bot_state) {
		case COMP_MSC_DATA_IN:
			if (usbd_composite_scsi_process_cmd(mdev, &mdev->cbw->CBWCB[0]) < 0) {
				usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_FAILED);
			}
			break;

		case COMP_MSC_SEND_DATA:
		case COMP_MSC_LAST_DATA_IN:
			if (mdev->phase_error == 1) {
				usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_PHASE_ERROR);
				mdev->phase_error = 0;
			} else {
				usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_PASSED);
			}
			break;

		default:
			break;
		}
	} else {
		RTK_LOGS(TAG,  RTK_LOG_WARN, "EP%02x TX err: %d\n", ep_addr, status);
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;
	usbd_composite_msc_csw_t *csw = mdev->csw;

	UNUSED(ep_addr);

	switch (mdev->bot_state) {
	case COMP_MSC_IDLE:
		/* Decode the CBW command */
		csw->dCSWTag = cbw->dCBWTag;
		csw->dCSWDataResidue = cbw->dCBWDataTransferLength;

		if ((len != COMP_MSC_CB_WRAP_LEN) ||
			(cbw->dCBWSignature != COMP_MSC_CB_SIGN) ||
			(cbw->bCBWLUN > 1U) ||
			(cbw->bCBWCBLength < 1U) || (cbw->bCBWCBLength > 16U)) {
			usbd_composite_scsi_sense_code(mdev, ILLEGAL_REQUEST, INVALID_CDB);
			mdev->bot_status = COMP_MSC_STATUS_ERROR;
			usbd_composite_msc_abort(dev);
		} else {
			if (usbd_composite_scsi_process_cmd(mdev, &cbw->CBWCB[0]) < 0) {
				if (mdev->phase_error == 1) {
					usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_PHASE_ERROR);
					mdev->phase_error = 0;
				} else if (mdev->bot_state == COMP_MSC_NO_DATA) {
					usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_FAILED);
				} else {
					usbd_composite_msc_abort(dev);
				}
			}
			/*Burst xfer handled internally*/
			else if ((mdev->bot_state != COMP_MSC_DATA_IN) &&
					 (mdev->bot_state != COMP_MSC_DATA_OUT) &&
					 (mdev->bot_state != COMP_MSC_LAST_DATA_IN)) {
				if (mdev->data_length > 0U) {
					u16 length = (u16)MIN(cbw->dCBWDataTransferLength, mdev->data_length);
					csw->dCSWDataResidue -= mdev->data_length;
					csw->bCSWStatus = COMP_MSC_CSW_CMD_PASSED;
					mdev->bot_state = COMP_MSC_SEND_DATA;

					usbd_composite_msc_bulk_transmit(dev, mdev->data, length);
				} else if (mdev->data_length == 0U) {
					usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_PASSED);
				} else {
					usbd_composite_msc_abort(dev);
				}
			} else {
				return HAL_OK;
			}
		}
		break;

	case COMP_MSC_DATA_OUT:
		if (usbd_composite_scsi_process_cmd(mdev, &cbw->CBWCB[0]) < 0) {
			usbd_composite_msc_send_csw(dev, COMP_MSC_CSW_CMD_FAILED);
		}

		break;

	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_composite_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	u16 len = 0;

	switch (USB_HIGH_BYTE(req->wValue)) {
	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_msc_hs_itf_desc;
			len = sizeof(usbd_composite_msc_hs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_msc_fs_itf_desc;
			len = sizeof(usbd_composite_msc_fs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_msc_fs_itf_desc;
			len = sizeof(usbd_composite_msc_fs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_msc_hs_itf_desc;
			len = sizeof(usbd_composite_msc_hs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;
	default:
		break;
	}

	return len;
}

int usbd_composite_msc_disk_init(void)
{
	SD_RESULT ret;

#if COMP_MSC_RAM_DISK
	ret = RAM_init();
#else
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
	ret = usbd_composite_msc_sd_init();
#else
	ret = SD_Init();
#endif
#endif

	return ret;
}

int usbd_composite_msc_disk_deinit(void)
{
	SD_RESULT ret;

#if COMP_MSC_RAM_DISK
	ret = RAM_deinit();
#else
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
	ret = usbd_composite_msc_sd_deinit();
#else
	ret = SD_DeInit();
#endif
#endif

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize MSC device
  * @retval Status
  */
int usbd_composite_msc_init(usbd_composite_dev_t *cdev)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_composite_msc_disk_ops_t *ops = &mdev->disk_ops;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;
	int ret = HAL_OK;

	RTK_LOGS(TAG,  RTK_LOG_DEBUG, "MSC Init\n");

	ret = usbd_composite_msc_disk_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG,  RTK_LOG_ERROR, "Disk init error: %d\n", ret);
		return ret;
	}

	mdev->cdev = cdev;

#if COMP_MSC_RAM_DISK
	ops->disk_getcapacity = RAM_GetCapacity;
	ops->disk_read = RAM_ReadBlocks;
	ops->disk_write = RAM_WriteBlocks;
#else
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAGREEN2)
	ops->disk_getcapacity = usbd_composite_msc_sd_getcapacity;
	ops->disk_read = usbd_composite_msc_sd_readblocks;
	ops->disk_write = usbd_composite_msc_sd_writeblocks;
#else
	ops->disk_getcapacity = SD_GetCapacity;
	ops->disk_read = SD_ReadBlocks;
	ops->disk_write = SD_WriteBlocks;
#endif
#endif

	ep_bulk_out->addr = USBD_COMP_MSC_BULK_OUT_EP;
	ep_bulk_out->type = USB_CH_EP_TYPE_BULK;

	ep_bulk_in->addr = USBD_COMP_MSC_BULK_IN_EP;
	ep_bulk_in->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->dis_zlp = 1;

	mdev->data = (u8 *)usb_os_malloc(COMP_MSC_BUFLEN);
	if (mdev->data == NULL) {
		ret = HAL_ERR_MEM;
		goto data_buf_fail;
	}

	mdev->cbw = (usbd_composite_msc_cbw_t *)usb_os_malloc(COMP_MSC_CB_WRAP_LEN);
	if (mdev->cbw == NULL) {
		ret = HAL_ERR_MEM;
		goto cbw_fail;
	}

	mdev->csw = (usbd_composite_msc_csw_t *)usb_os_malloc(COMP_MSC_CS_WRAP_LEN);
	if (mdev->csw == NULL) {
		ret = HAL_ERR_MEM;
		goto csw_fail;
	}

	mdev->blkbits = COMP_MSC_BLK_BITS;
	mdev->blksize = COMP_MSC_BLK_SIZE;

	usbd_register_class(&usbd_composite_msc_driver);

	return HAL_OK;

csw_fail:
	usb_os_mfree(mdev->cbw);
	mdev->cbw = NULL;

cbw_fail:
	usb_os_mfree(mdev->data);
	mdev->data = NULL;

data_buf_fail:
	return ret;
}

/**
  * @brief  De-Initialize MSC device
  * @retval Status
  */
void usbd_composite_msc_deinit(void)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;

	RTK_LOGS(TAG,  RTK_LOG_DEBUG, "Deinit\n");

	usbd_unregister_class();

	usbd_composite_msc_disk_deinit();

	if (mdev->csw != NULL) {
		usb_os_mfree(mdev->csw);
		mdev->csw = NULL;
	}

	if (mdev->cbw != NULL) {
		usb_os_mfree(mdev->cbw);
		mdev->cbw = NULL;
	}

	if (mdev->data != NULL) {
		usb_os_mfree(mdev->data);
		mdev->data = NULL;
	}
}

/**
* @brief  BULK transmit
* @param  dev: device instance
* @param  buf: data buffer
* @param  len: data length
* @retval status
*/
int usbd_composite_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u16 len)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_ep_t *ep_bulk_in = &mdev->ep_bulk_in;
	int ret = HAL_ERR_HW;

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
int usbd_composite_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u16 len)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
	int ret = HAL_ERR_HW;

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
void usbd_composite_msc_send_csw(usb_dev_t *dev, u8 status)
{
	usbd_composite_msc_dev_t *mdev = &usbd_composite_msc_dev;
	usbd_composite_msc_cbw_t *cbw = mdev->cbw;
	usbd_composite_msc_csw_t *csw = mdev->csw;
#if COMP_MSC_FIX_CV_TEST_ISSUE
	usbd_ep_t *ep_bulk_out = &mdev->ep_bulk_out;
#endif

	csw->dCSWSignature = COMP_MSC_CS_SIGN;
	csw->bCSWStatus = status;
	mdev->bot_state = COMP_MSC_IDLE;

	usbd_composite_msc_bulk_transmit(dev, (u8 *)csw, COMP_MSC_CS_WRAP_LEN);

#if COMP_MSC_FIX_CV_TEST_ISSUE
	/* Fix CV test failure */
	if (mdev->bot_status == COMP_MSC_STATUS_RECOVERY) {
		usbd_ep_deinit(dev, ep_bulk_out);
		usbd_ep_init(dev, ep_bulk_out);
	}
#endif

	/* Prepare EP to Receive next Cmd */
	usbd_composite_msc_bulk_receive(dev, (u8 *)cbw, COMP_MSC_CB_WRAP_LEN);
}

