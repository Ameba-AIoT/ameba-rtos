/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_msc.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usbd_scsi.h"
#include "os_wrapper.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#ifdef CONFIG_USBD_MSC_SD_CARD
#include "ameba_sd.h"
#endif
#if defined(CONFIG_USBD_MSC_SECOND_FLASH) && !defined(CONFIG_FATFS_SECOND_FLASH)
/* FATFS has no second-NAND backend; a Kconfig dep would cycle through VFS_SECOND_FLASH. */
#error "USBD_MSC_SECOND_FLASH needs a NOR second flash (VFS_SECOND_FLASH=y, VFS_SECOND_FLASH_NAND=n)"
#endif
#if !defined(CONFIG_USBD_MSC_FLASH) && !defined(CONFIG_USBD_MSC_SECOND_FLASH) && \
	!defined(CONFIG_USBD_MSC_SD_CARD) && !defined(CONFIG_USBD_MSC_SD_SPI) && !defined(CONFIG_USBD_MSC_RAM)
/* The media backends below are picked by one macro only; there is no default. Silently
 * falling back would change the backend of an existing config, so fail the build instead. */
#error "USBD MSC needs a storage media: select one under CONFIG USB"
#endif

/* Private defines -----------------------------------------------------------*/

/* Interface number of the single Mass Storage interface */
#define USBD_MSC_ITF_NUM               0U

/* BOT status */
#define USBD_MSC_STATUS_NORMAL         0U          /**< Normal working status */
#define USBD_MSC_STATUS_RECOVERY       1U          /**< Get MSC Reset request for recovery */
#define USBD_MSC_STATUS_ERROR          2U          /**< Error status */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_msc_set_config(usb_dev_t *dev, u8 config);
static void usbd_msc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_msc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
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
	USB_LOW_BYTE(USB_VID),                           /* idVendor (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),                           /* idProduct (patched by core from usbd_dev_info_t) */
	USB_HIGH_BYTE(USB_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_msc_dev_desc */

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
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
	USB_H2D,                                    /* bEndpointAddress: dir OUT (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_HS_MAX_PACKET_SIZE),
	0x00                                            /* bInterval */
};  /* usbd_msc_hs_config_desc */
#endif

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
	0x80,                                           /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
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
	USB_H2D,                                    /* bEndpointAddress: dir OUT (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
	USB_LOW_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_MSC_FS_MAX_PACKET_SIZE),
	0x00,						/* bInterval */

	/* BULK IN Endpoint Descriptor */
	USB_LEN_EP_DESC,			/* bLength */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType */
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */
	USB_CH_EP_TYPE_BULK,                            /* bmAttributes: BULK */
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

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static int usbd_msc_media_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Disk init\n");

#ifdef CONFIG_USBD_MSC_FLASH
	/* FATFS is mounted by the app; MSC only does raw I/O. */
	return FLASH_disk_Driver.disk_initialize();
#elif defined CONFIG_USBD_MSC_SECOND_FLASH
	return FLASH_second_disk_Driver.disk_initialize();
#elif defined CONFIG_USBD_MSC_SD_CARD
	return SD_disk_Driver.disk_initialize();
#elif defined CONFIG_USBD_MSC_SD_SPI
	return SD_disk_spi_Driver.disk_initialize();
#elif defined CONFIG_USBD_MSC_RAM
	/* FATFS is mounted by the app; MSC only does raw I/O. */
	return RAM_disk_Driver.disk_initialize();
#else
	return HAL_ERR_PARA;		/* unreachable: a media is enforced at the top of the file */
#endif
}

static void usbd_msc_media_deinit(void)
{
	DSTATUS status;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Disk deinit\n");

#ifdef CONFIG_USBD_MSC_FLASH
	status = FLASH_disk_Driver.disk_deinitialize();
#elif defined CONFIG_USBD_MSC_SECOND_FLASH
	status = FLASH_second_disk_Driver.disk_deinitialize();
#elif defined CONFIG_USBD_MSC_SD_CARD
	status = SD_disk_Driver.disk_deinitialize();
#elif defined CONFIG_USBD_MSC_SD_SPI
	status = SD_disk_spi_Driver.disk_deinitialize();
#elif defined CONFIG_USBD_MSC_RAM
	status = RAM_disk_Driver.disk_deinitialize();
#else
	status = HAL_ERR_PARA;	/* unreachable: a media is enforced at the top of the file */
#endif

	/* Best effort: a backend that cannot unmount cleanly is reported here. Nothing can be done
	   about it on a teardown path, and the returned DSTATUS also carries plain status bits such
	   as STA_NODISK/STA_PROTECT, which are not failures in this context. */
	if (status != 0U) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Disk deinit status 0x%02x\n", status);
	}
}

static int usbd_msc_media_getcapacity(u32 *sector_count)
{
	int ret;
	/* GET_SECTOR_COUNT writes an LBA_t, which is 64-bit when FF_LBA64 is set
	   (CONFIG_FATFS_SUPPORT_EXFAT). Bounce through LBA_t so a 64-bit store
	   cannot overrun the caller's u32. */
	LBA_t count = 0;

#ifdef CONFIG_USBD_MSC_FLASH
	ret = FLASH_disk_Driver.disk_ioctl(GET_SECTOR_COUNT, &count);
#elif defined CONFIG_USBD_MSC_SECOND_FLASH
	ret = FLASH_second_disk_Driver.disk_ioctl(GET_SECTOR_COUNT, &count);
#elif defined CONFIG_USBD_MSC_SD_CARD
	ret = SD_disk_Driver.disk_ioctl(GET_SECTOR_COUNT, &count);
#elif defined CONFIG_USBD_MSC_SD_SPI
	ret = SD_disk_spi_Driver.disk_ioctl(GET_SECTOR_COUNT, &count);
#elif defined CONFIG_USBD_MSC_RAM
	ret = RAM_disk_Driver.disk_ioctl(GET_SECTOR_COUNT, &count);
#else
	ret = HAL_ERR_PARA;		/* unreachable: a media is enforced at the top of the file */
#endif
	if (ret == 0) {
		*sector_count = (u32)count;
	}

	return ret;
}

static int usbd_msc_media_readblocks(u32 sector, u8 *data, u32 count)
{
#ifdef CONFIG_USBD_MSC_FLASH
	return FLASH_disk_Driver.disk_read(data, sector, count);
#elif defined CONFIG_USBD_MSC_SECOND_FLASH
	return FLASH_second_disk_Driver.disk_read(data, sector, count);
#elif defined CONFIG_USBD_MSC_SD_CARD
	return SD_disk_Driver.disk_read(data, sector, count);
#elif defined CONFIG_USBD_MSC_SD_SPI
	return SD_disk_spi_Driver.disk_read(data, sector, count);
#elif defined CONFIG_USBD_MSC_RAM
	return RAM_disk_Driver.disk_read(data, sector, count);
#else
	return HAL_ERR_PARA;		/* unreachable: a media is enforced at the top of the file */
#endif
}

static int usbd_msc_media_writeblocks(u32 sector, const u8 *data, u32 count)
{
#ifdef CONFIG_USBD_MSC_FLASH
	return FLASH_disk_Driver.disk_write(data, sector, count);
#elif defined CONFIG_USBD_MSC_SECOND_FLASH
	return FLASH_second_disk_Driver.disk_write(data, sector, count);
#elif defined CONFIG_USBD_MSC_SD_CARD
	return SD_disk_Driver.disk_write(data, sector, count);
#elif defined CONFIG_USBD_MSC_SD_SPI
	return SD_disk_spi_Driver.disk_write(data, sector, count);
#elif defined CONFIG_USBD_MSC_RAM
	return RAM_disk_Driver.disk_write(data, sector, count);
#else
	return HAL_ERR_PARA;		/* unreachable: a media is enforced at the top of the file */
#endif
}

/**
* @brief  Abort the current transfer
* @param  dev: device instance
* @retval None
*/
static void usbd_msc_abort(usb_dev_t *dev)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usb_msc_bot_cbw_t *cbw = cdev->cbw;

	/* BOT §6.6.1: stall OUT on any write-phase mismatch OR on invalid CBW (RECOVERY). For an invalid CBW the
	 * caller has also set `wedged`, so the halt survives a plain CLEAR_FEATURE and only Reset Recovery, a
	 * Bulk-Only Mass Storage Reset followed by CLEAR_FEATURE on both endpoints, resumes communication. */
	if (((cbw->field.bmCBWFlags == 0U) &&
		 (cbw->field.dCBWDataTransferLength != 0U) &&
		 (cdev->bot_status == USBD_MSC_STATUS_NORMAL)) ||
		(cdev->bot_status == USBD_MSC_STATUS_RECOVERY)) {
		usbd_ep_set_stall(dev, &cdev->ep_bulk_out);
		/* Arm OUT while halted, as send_csw() does: the controller keeps answering
		 * STALL until the host clears the halt, then this transfer picks up the next
		 * CBW. Without it the EP is neither stalled nor listening once the halt is
		 * cleared by CLEAR_FEATURE or SET_CONFIGURATION. A failure means the device is no longer ready
		 * (suspend/disconnect), the host then recovers by reset. */
		if (usbd_msc_bulk_receive(dev, (u8 *)cbw, USB_MSC_CBW_LEN, USBD_MSC_CBW_BUF_LEN) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_out_addr);
		}
	}

	usbd_ep_set_stall(dev, &cdev->ep_bulk_in);
}

/**
  * @brief  Fail a Data-In command whose data stage cannot be completed, deferring its CSW
  * @note   Ref MSC BOT 1.0 6.7.2 Case 4 (Hi > Dn) and 6.7.2 Case 5/7 cut short by a medium error: Bulk-In
  *         carries both the data and the CSW, so a CSW sent now would be read by the host as data. Bulk-In is
  *         halted instead to end the data stage, and the CSW is kept until the host clears the halt, see the
  *         USB_REQ_CLEAR_FEATURE case in usbd_msc_setup(). dCSWDataResidue already accounts for the bytes sent.
  * @note   The command is complete from the device point of view: the CSW is built, the BOT state machine is
  *         back to idle and Bulk-Out is armed for the next CBW, as usbd_msc_send_csw() does. Only the CSW
  *         transmission is left to the ISR. Bulk-Out is armed now rather than with the CSW so that a host that
  *         recovers by SET_CONFIGURATION, which the core handles without the class, still reaches the device
  *         with its next CBW; usbd_msc_rx_process() then drops the stale CSW.
  * @note   Called from the MSC threads while no Bulk-In transfer is in flight. Every store is made before the
  *         halt: the host can only clear a halt it has seen, and the critical section of the stall path orders
  *         the stores before the halt on SMP, so the ISR always sees a complete CSW.
  * @param  dev: USB device instance
  * @param  status: bCSWStatus to report once the halt is cleared, BOT_CSW_CMD_FAILED or BOT_CSW_PHASE_ERROR
  * @retval None
  */
static void usbd_msc_defer_csw(usb_dev_t *dev, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usb_msc_bot_csw_t *csw = cdev->csw;

	/* Logged before Bulk-Out is re-armed below, the DMA may overwrite the CBW with the next one afterwards */
	RTK_LOGS(TAG, RTK_LOG_WARN, "Cmd %02x fail, CSW %d deferred\n", cdev->cbw->field.CBWCB[0], status);

	csw->field.dCSWSignature = USB_MSC_CSW_SIGN;
	csw->field.bCSWStatus = status;
	cdev->bot_state = USBD_MSC_IDLE;
	cdev->bot_status = USBD_MSC_STATUS_NORMAL;
	cdev->csw_pending = 1U;
	usbd_ep_set_stall(dev, &cdev->ep_bulk_in);

	/* A failure means the device is no longer ready (suspend/disconnect), the host then recovers by reset */
	if (usbd_msc_bulk_receive(dev, (u8 *)cdev->cbw, USB_MSC_CBW_LEN, USBD_MSC_CBW_BUF_LEN) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_out_addr);
	}
}

/**
  * @brief  Set MSC class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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
	usb_ep_info_t *info;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	cdev->dev = dev;

	/* Init BULK IN EP */
	info = &ep_bulk_in->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_in);

	/* Init BULK OUT EP */
	info = &ep_bulk_out->info;
	info->mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_MSC_HS_MAX_PACKET_SIZE : USBD_MSC_FS_MAX_PACKET_SIZE;
	usbd_ep_init(dev, ep_bulk_out);

	cdev->bot_state = USBD_MSC_IDLE;
	cdev->bot_status = USBD_MSC_STATUS_NORMAL;
	usb_os_memset((void *)&cdev->scsi_sense, 0U, sizeof(cdev->scsi_sense));
	cdev->is_open = 1U;
	cdev->ro = 0U;
	cdev->phase_error = 0U;
	cdev->bot_reset_pending = 0U;
	cdev->wedged = 0U;
	cdev->csw_pending = 0U;

	/* Prepare to receive next BULK OUT packet. Without it the device would enumerate but never see a CBW, so
	   fail the configuration and let the core stall SET_CONFIGURATION instead. */
	ret = usbd_msc_bulk_receive(dev, (u8 *)cdev->cbw, USB_MSC_CBW_LEN, USBD_MSC_CBW_BUF_LEN);
	if (ret != HAL_OK) {
		usbd_msc_clear_config(dev, config);
	}

	return ret;
}

/**
  * @brief  Clear MSC configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_msc_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	UNUSED(config);

	/* DeInit BULK IN EP */
	usbd_ep_deinit(dev, ep_bulk_in);

	/* DeInit BULK OUT EP */
	usbd_ep_deinit(dev, ep_bulk_out);

	cdev->bot_state  = USBD_MSC_IDLE;
	cdev->is_open = 0U;
	cdev->phase_error = 0U;
}

/**
  * @brief  Handle MSC specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_msc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u8 ep_addr = USB_LOW_BYTE(req->wIndex);
	int ret = HAL_OK;

	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x\n",
	//		 req->bmRequestType, req->bRequest, req->wLength, req->wValue);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	/* Standard request */
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5 and Table 9-3: an interface-recipient GET_STATUS returns two reserved zero
			   bytes, wLength is two, and a request for a non-existent interface is a request error. The core
			   checks the direction and answers device/endpoint recipients itself. */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wLength == 2U) && (req->wIndex == USBD_MSC_ITF_NUM)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			/* Ref USB 2.0 9.4.4 and Table 9-3: wLength is one, the only alternate setting is 0, and a request
			   for a non-existent interface is a request error. */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wLength == 1U) && (req->wIndex == USBD_MSC_ITF_NUM)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_CLEAR_FEATURE:
			/* Offered by the core only once it has validated an ENDPOINT_HALT request on an opened endpoint.
			   HAL_OK means the class has handled the halt itself and the core only ACKs the request; any other
			   value, including a foreign endpoint in composite mode, lets the core clear the halt as usual. */
			if (((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_ENDPOINT) ||
				((ep_addr != cdev->cfg->bulk_in_addr) && (ep_addr != cdev->cfg->bulk_out_addr))) {
				ret = HAL_ERR_PARA;
			} else if (cdev->wedged != 0U) {
				/* Ref MSC BOT 1.0 6.6.1: after an invalid CBW both bulk endpoints stay halted until Reset
				   Recovery, so keep the halt. */
				ret = HAL_OK;
			} else if ((ep_addr == cdev->cfg->bulk_in_addr) && (cdev->csw_pending != 0U)) {
				/* Ref MSC BOT 1.0 6.7.2: the halt ended a failed Data-In stage, see usbd_msc_defer_csw(). Once
				   the host clears it the CSW follows on Bulk-In, so clear the halt (data toggle DATA0) before
				   queueing the CSW behind it. The command is already complete and Bulk-Out armed, so only the
				   CSW is sent; the MSC threads do not touch Bulk-In or the CSW meanwhile. */
				cdev->csw_pending = 0U;
				usbd_ep_clear_stall(dev, &cdev->ep_bulk_in);
				if (usbd_msc_bulk_transmit(dev, (u8 *)cdev->csw, USB_MSC_CSW_LEN) != HAL_OK) {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, cdev->cfg->bulk_in_addr);
				}
				ret = HAL_OK;
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else if ((req->wIndex == USBD_MSC_ITF_NUM) && (USB_LOW_BYTE(req->wValue) == 0U)) {
				/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to
				   their default state, not halted and data toggle DATA0. This holds even
				   for an interface with the default setting only, hosts do send the
				   request in that case. The BOT state machine is left alone on purpose:
				   the OUT endpoint is already armed with the next CBW, see
				   usbd_msc_abort(), and a RECOVERY status is only cleared by a
				   Bulk-Only Mass Storage Reset as per BOT 3.1.
				   Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, so a
				   foreign interface or a non-zero high byte leaves the endpoints untouched.
				   The halts are cleared here regardless of an invalid CBW, so drop the
				   wedge too, it would otherwise outlive the halt it protects. */
				cdev->wedged = 0U;
				usbd_ep_clear_stall(dev, &cdev->ep_bulk_in);
				usbd_ep_clear_stall(dev, &cdev->ep_bulk_out);
				/* Resetting the endpoints abandons a failed command whose CSW is still deferred,
				   see usbd_msc_defer_csw(): the host no longer reads it, and a late one would
				   carry a stale tag. */
				cdev->csw_pending = 0U;
			} else {
				/* Ref USB 2.0 9.4.9: this function declares alternate setting 0 only, so any
				   other bAlternateSetting is a request error; likewise a foreign interface
				   number, which must be rejected for composite dispatch to continue. */
				ret = HAL_ERR_PARA;
			}
			break;


		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;
	/* Class request */
	case USB_REQ_TYPE_CLASS:
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = HAL_ERR_PARA;
			break;
		}
		/* Ref MSC BOT 1.0 3.1 and 3.2: wIndex of both class requests is the interface number.
		   A foreign interface must be rejected so that composite dispatch can continue. */
		if (req->wIndex != USBD_MSC_ITF_NUM) {
			ret = HAL_ERR_PARA;
			break;
		}
		switch (req->bRequest) {
		case USB_MSC_REQUEST_GET_MAX_LUN:
			if ((req->wValue  == 0U) && (req->wLength == 1U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				/* A failed submit makes the core stall the request, as for the standard requests above */
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_MSC_REQUEST_BOT_RESET :
			if ((req->wValue  == 0U) && (req->wLength == 0U) &&
				((req->bmRequestType & USB_REQ_DIR_MASK) != USB_D2H)) {
				cdev->bot_state  = USBD_MSC_IDLE;
				cdev->bot_status = USBD_MSC_STATUS_RECOVERY;
				cdev->bot_reset_pending = 1U;
				/* Ref BOT 5.3.4: the reset is the first step of Reset Recovery, the CLEAR_FEATURE requests
				   that follow shall now clear the halts again */
				cdev->wedged = 0U;
				/* Ref BOT 3.1: the reset aborts the current command, so its deferred CSW is dropped */
				cdev->csw_pending = 0U;
				/* Prepare to receive BOT cmd. On failure stall the reset so the host escalates to a port reset. */
				ret = usbd_msc_bulk_receive(dev, (u8 *)cdev->cbw, USB_MSC_CBW_LEN, USBD_MSC_CBW_BUF_LEN);
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
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_msc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by MSC, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->bulk_in_addr) {
		return HAL_ERR_PARA;
	}

	cdev->tx_status = status;
	rtos_sema_give(cdev->tx_sema);

	return HAL_OK;
}

/**
  * @brief  TX process, run by the TX thread once a Bulk-In transfer has completed
  * @note   Continues a multi-chunk READ, or sends the CSW after the last data chunk. A medium error in the
  *         middle of a READ defers the CSW, see usbd_msc_defer_csw().
  * @retval None
  */
static void usbd_msc_tx_process(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usb_msc_bot_cbw_t *cbw = cdev->cbw;
	usb_dev_t *dev = cdev->dev;
	u8 csw_status;

	usb_os_lock(usbd_msc_sd_lock);

	if (cdev->tx_status == HAL_OK) {
		switch (cdev->bot_state) {
		case USBD_MSC_DATA_IN:
			if (usbd_scsi_process_cmd(cdev, cbw->field.CBWCB) != HAL_OK) {
				/* The medium failed after part of the data was sent, and the host still expects the rest on
				 * Bulk-In, so end the data stage with a halt rather than a CSW. A phase error recorded for this
				 * command (Case 7, Hi < Di) takes precedence over the medium error, ref BOT 6.7.2, and is
				 * consumed before the halt, the last access to the BOT state from this thread, so it cannot leak
				 * into the next command. */
				csw_status = (cdev->phase_error != 0U) ? BOT_CSW_PHASE_ERROR : BOT_CSW_CMD_FAILED;
				cdev->phase_error = 0U;
				usbd_msc_defer_csw(dev, csw_status);
			}
			break;

		case USBD_MSC_SEND_DATA:
		case USBD_MSC_LAST_DATA_IN:
			if (cdev->phase_error != 0U) {
				usbd_msc_send_csw(dev, BOT_CSW_PHASE_ERROR);
				cdev->phase_error = 0U;
			} else {
				usbd_msc_send_csw(dev, BOT_CSW_CMD_PASSED);
			}
			break;

		default:
			break;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_in_addr);
	}

	usb_os_unlock(usbd_msc_sd_lock);
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_msc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by MSC, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->bulk_out_addr) {
		return HAL_ERR_PARA;
	}

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
	usb_msc_bot_cbw_t *cbw = cdev->cbw;
	usb_msc_bot_csw_t *csw = cdev->csw;
	usb_dev_t *dev = cdev->dev;
	u8 *cbwcb = cbw->field.CBWCB;
	u32 cbw_data_len = cbw->field.dCBWDataTransferLength;
	u32 length;

	usb_os_lock(usbd_msc_sd_lock);

	switch (cdev->bot_state) {
	case USBD_MSC_IDLE:
		/* A new CBW means the host has abandoned a command whose CSW was still deferred, e.g. after it reset the
		   endpoints by SET_CONFIGURATION, see usbd_msc_defer_csw(). Drop that CSW before the new command reuses
		   the buffer, so a later CLEAR_FEATURE cannot send it with a stale tag. Reaching here with the CSW still
		   pending means the host recovered without the CLEAR_FEATURE that BOT 6.7.2 expects. */
		if (cdev->csw_pending != 0U) {
			cdev->csw_pending = 0U;
			RTK_LOGS(TAG, RTK_LOG_WARN, "Stale CSW dropped\n");
		}
		/* Decode the CBW command */
		csw->field.dCSWTag = cbw->field.dCBWTag;
		csw->field.dCSWDataResidue = cbw_data_len;

		if ((cdev->rx_data_length != USB_MSC_CBW_LEN) ||
			(cbw->field.dCBWSignature != USB_MSC_CBW_SIGN) ||
			((cbw->field.bmCBWFlags & 0x7FU) != 0U) ||  /* §6.2.2: bmCBWFlags reserved bits 6:0 must be 0 */
			(cbw->field.bCBWLUN != 0U) ||  /* Only LUN 0 exists (GET_MAX_LUN reports 0) */
			(cbw->field.bCBWCBLength < 1U) || (cbw->field.bCBWCBLength > 16U)) {
			usbd_scsi_sense_code(cdev, SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
			/* BOT §6.6.1: invalid CBW — use RECOVERY so abort() stalls both endpoints, and wedge them so the
			 * halts hold until Reset Recovery. Set before the stalls: the host can only send the CLEAR_FEATURE
			 * that reads the flag after it has seen a STALL, and the stall critical section orders the store. */
			cdev->bot_status = USBD_MSC_STATUS_RECOVERY;
			cdev->wedged = 1U;
			usbd_msc_abort(dev);
		} else {
			cdev->bot_status = USBD_MSC_STATUS_NORMAL;
			/* A valid CBW proves the host has already cleared the halts, e.g. by SET_CONFIGURATION, which the
			   core handles without the class, so an older wedge no longer applies. */
			cdev->wedged = 0U;
			if (usbd_scsi_process_cmd(cdev, cbwcb) != HAL_OK) {
				if (cdev->phase_error != 0U) {
					if ((cbw_data_len != 0U) && ((cbw->field.bmCBWFlags & 0x80U) == 0U)) {
						/* Case 13: Ho < Do — the host still owns an OUT data stage that the
						 * device will not consume. STALL Bulk-Out so those bytes are not
						 * mis-read as the next CBW; the Phase Error CSW flows on the
						 * un-stalled Bulk-In pipe and the host then performs reset recovery. */
						usbd_ep_set_stall(dev, &cdev->ep_bulk_out);
					}
					usbd_msc_send_csw(dev, BOT_CSW_PHASE_ERROR);
					cdev->phase_error = 0U;
				} else if (cbw_data_len == 0U) {
					/* Case 1: Hn = Dn — no data stage, report failure directly */
					usbd_msc_send_csw(dev, BOT_CSW_CMD_FAILED);
				} else if ((cbw->field.bmCBWFlags & 0x80U) == 0U) {
					/* Case 9: Ho > Dn — a failed OUT command (e.g. write-protected
					 * or no-medium WRITE) consumes no data. STALL Bulk-Out to abort
					 * the host data stage; the CSW still flows on the un-stalled
					 * Bulk-In pipe. */
					usbd_ep_set_stall(dev, &cdev->ep_bulk_out);
					usbd_msc_send_csw(dev, BOT_CSW_CMD_FAILED);
				} else {
					/* Case 4: Hi > Dn — a failed IN command (e.g. no-medium READ)
					 * returns no data. Bulk-In carries both the (absent) data and
					 * the CSW, so STALL it; the CSW follows once the host clears
					 * the halt. */
					usbd_msc_defer_csw(dev, BOT_CSW_CMD_FAILED);
				}
			}
			/*Burst xfer handled internally*/
			else if ((cdev->bot_state != USBD_MSC_DATA_IN) &&
					 (cdev->bot_state != USBD_MSC_DATA_OUT) &&
					 (cdev->bot_state != USBD_MSC_LAST_DATA_IN)) {
				if (cdev->data_length > 0U) {
					length = MIN(cbw_data_len, cdev->data_length);
					/* Residue reflects bytes actually sent (clamped), not the SCSI payload size */
					csw->field.dCSWDataResidue -= length;
					csw->field.bCSWStatus = BOT_CSW_CMD_PASSED;
					cdev->bot_state = USBD_MSC_SEND_DATA;

					if (usbd_msc_bulk_transmit(dev, cdev->data, length) != HAL_OK) {
						RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_in_addr);
					}
				} else if (cdev->data_length == 0U) {
					if ((cbw_data_len != 0U) && ((cbw->field.bmCBWFlags & 0x80U) == 0U)) {
						/* Case 9: Ho > Dn — command produced no data but the host
						 * declared an OUT transfer (e.g. a stubbed MODE SELECT); STALL
						 * Bulk-Out so its parameter bytes are not mis-read as the next
						 * CBW. The CSW flows on the un-stalled Bulk-In pipe below. */
						usbd_ep_set_stall(dev, &cdev->ep_bulk_out);
					}
					usbd_msc_send_csw(dev, BOT_CSW_CMD_PASSED);
				} else {
					usbd_msc_abort(dev);
				}
			}
		}
		break;

	case USBD_MSC_DATA_OUT:
		if (usbd_scsi_process_cmd(cdev, cbwcb) != HAL_OK) {
			usbd_msc_send_csw(dev, BOT_CSW_CMD_FAILED);
		}

		break;

	default:
		break;
	}

	usb_os_unlock(usbd_msc_sd_lock);
}

/** @cond PATCH_EP_ADDRESSES */

/**
  * @brief  Patch the runtime-assigned fields in a configuration descriptor block
  * @note   Replaces direction-only placeholders (USB_D2H/USB_H2D) with actual EP addresses from
  *         the class configuration, and rewrites the iInterface string index with the index the
  *         core assigned.
  * @param  desc: Pointer to config descriptor body (starting after config header)
  * @param  len: Length of the descriptor block
  * @retval None
  */
static void usbd_msc_patch_desc(u8 *desc, u16 len)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	const usbd_msc_config_t *cfg = cdev->cfg;

	for (u16 i = 0U; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];
		if (dlen == 0U) {
			break;
		}

		if ((dtype == USB_DESC_TYPE_ENDPOINT) && ((i + 3U) <= len)) {
			u8 addr  = desc[i + 2];
			u8 dir   = addr & USB_REQ_DIR_MASK;
			u8 type  = desc[i + 3] & 0x03U;

			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_BULK)) {
				desc[i + 2] = cfg->bulk_out_addr;
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC)) {
			desc[i + 8] = cdev->itf_str_idx;         /* iInterface */
		}
		i += dlen;
	}
}

/** @endcond */

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_msc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	usb_speed_type_t speed = dev->dev_speed;
#endif
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
	UNUSED(dev);
#endif

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_msc_dev_desc;
		len = sizeof(usbd_msc_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_msc_hs_config_desc;
			len = sizeof(usbd_msc_hs_config_desc);
		} else
#endif
		{
			desc = usbd_msc_fs_config_desc;
			len = sizeof(usbd_msc_fs_config_desc);
		}
		is_cfg = 1;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_msc_device_qualifier_desc;
		len = sizeof(usbd_msc_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = usbd_msc_fs_config_desc;
			len = sizeof(usbd_msc_fs_config_desc);
		} else {
			desc = usbd_msc_hs_config_desc;
			len = sizeof(usbd_msc_hs_config_desc);
		}
		is_cfg = 1;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		/* Every string this class owns is registered with usbd_add_string() and answered by
		   the core, which only forwards an index it does not know, e.g. the MS OS string */
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

	if (is_cfg != 0U) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		/* Patch EP addresses and the class string index to actual values */
		usbd_msc_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC);
	}

	return len;
}

/**
  * @brief  USB attach status change
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_msc_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed != NULL) {
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

/**
  * @brief  Copy one SCSI INQUIRY identification field, space-padded to its fixed width
  * @note   Ref SPC Table 59: the vendor/product/revision fields are fixed-width and
  *         left-aligned, padded with ASCII spaces rather than NUL-terminated.
  * @param  dst: Destination field inside the INQUIRY response
  * @param  len: Field width in bytes
  * @param  str: Configured string, NULL to leave the default in place
  * @retval HAL_OK on success, HAL_ERR_PARA when the string is longer than the field
  */
static int usbd_msc_set_inquiry_field(u8 *dst, u32 len, const char *str)
{
	u32 i;

	if (str == NULL) {
		return HAL_OK;
	}

	for (i = 0U; (i < len) && (str[i] != '\0'); i++) {
		dst[i] = (u8)str[i];
	}

	/* A longer string would be silently truncated into a different identity, so reject it. */
	if (str[i] != '\0') {
		return HAL_ERR_PARA;
	}

	for (; i < len; i++) {
		dst[i] = (u8)' ';
	}

	return HAL_OK;
}

/**
  * @brief  Build the per-instance SCSI standard INQUIRY response
  * @note   Starts from the class default template and overwrites only the identification
  *         fields the configuration supplies, so every other byte keeps its spec-defined value.
  * @param  cdev: Device instance
  * @param  cfg: Class configuration
  * @retval HAL_OK on success, HAL_ERR_PARA when an identification string does not fit
  */
static int usbd_msc_build_inquiry_data(usbd_msc_dev_t *cdev, const usbd_msc_config_t *cfg)
{
	u8 *data = cdev->inquiry_data;

	usb_os_memcpy((void *)data, (const void *)usbd_msc_default_inquiry_data, INQUIRY_DATA_LEN);

	if ((usbd_msc_set_inquiry_field(&data[8], USBD_MSC_INQUIRY_VENDOR_LEN, cfg->inquiry_vendor) != HAL_OK) ||
		(usbd_msc_set_inquiry_field(&data[16], USBD_MSC_INQUIRY_PRODUCT_LEN, cfg->inquiry_product) != HAL_OK) ||
		(usbd_msc_set_inquiry_field(&data[32], USBD_MSC_INQUIRY_REVISION_LEN, cfg->inquiry_revision) != HAL_OK)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Inquiry str too long\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

/**
  * @brief  Initialize MSC device
  * @retval Status
  */
static int usbd_msc_private_init(const usbd_msc_cb_t *cb, const usbd_msc_config_t *cfg)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_msc_disk_ops_t *ops = &cdev->disk_ops;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
	usb_ep_info_t *info;
	int ret = HAL_OK;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return HAL_ERR_PARA;
	}

	/* Both endpoints are mandatory, and a wrong direction bit would make the class claim an
	   endpoint the controller drives the other way round. */
	if (((cfg->bulk_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
		((cfg->bulk_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
		((cfg->bulk_in_addr & (~USB_REQ_DIR_MASK)) == 0U) ||
		((cfg->bulk_out_addr & (~USB_REQ_DIR_MASK)) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x\n", cfg->bulk_in_addr, cfg->bulk_out_addr);
		return HAL_ERR_PARA;
	}

	/* READ/WRITE move whole blocks per round trip: a buffer shorter than one block cannot carry a single
	   sector, and a partial trailing block would be sent uninitialized on READ, or make the Bulk-OUT DMA window
	   exceed the buffer on WRITE. */
	cdev->buf_size = (cfg->buf_size != 0U) ? cfg->buf_size : USBD_MSC_DEFAULT_BUF_SIZE;
	if ((cdev->buf_size < USBD_MSC_BLK_SIZE) || ((cdev->buf_size & (USBD_MSC_BLK_SIZE - 1U)) != 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad buf size %u\n", cdev->buf_size);
		return HAL_ERR_PARA;
	}

	if (usbd_msc_build_inquiry_data(cdev, cfg) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init\n");

	cdev->cb = cb;
	cdev->cfg = cfg;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	cdev->itf_str_idx = usbd_add_string(cfg->itf_str);

	ops->disk_getcapacity = usbd_msc_media_getcapacity;
	ops->disk_read = usbd_msc_media_readblocks;
	ops->disk_write = usbd_msc_media_writeblocks;

	ret = usb_os_lock_create(&usbd_msc_sd_lock);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create lock fail\n");
		return ret;
	}

	cdev->data = (u8 *)usb_os_malloc(cdev->buf_size);
	if (cdev->data == NULL) {
		ret = HAL_ERR_MEM;
		goto data_buf_fail;
	}

	cdev->cbw = (usb_msc_bot_cbw_t *)usb_os_malloc(USBD_MSC_CBW_BUF_LEN);
	if (cdev->cbw == NULL) {
		ret = HAL_ERR_MEM;
		goto cbw_fail;
	}

	cdev->csw = (usb_msc_bot_csw_t *)usb_os_malloc(USB_MSC_CSW_LEN);
	if (cdev->csw == NULL) {
		ret = HAL_ERR_MEM;
		goto csw_fail;
	}

	ret = rtos_sema_create(&cdev->rx_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX sema fail\n");
		goto create_rx_sema_fail;
	}

	ret = rtos_sema_create(&cdev->tx_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX sema fail\n");
		goto create_tx_sema_fail;
	}

	ret = rtos_task_create(&cdev->rx_task, "usbd_msc_rx_thread", usbd_msc_rx_thread, NULL,
						   (cfg->trx_task_stack_size != 0U) ? cfg->trx_task_stack_size : USBD_MSC_DEFAULT_TRX_TASK_STACK_SIZE,
						   (cfg->rx_task_priority != 0U) ? cfg->rx_task_priority : USBD_MSC_DEFAULT_RX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create RX thread fail\n");
		goto create_rx_thread_fail;
	}

	ret = rtos_task_create(&cdev->tx_task, "usbd_msc_tx_thread", usbd_msc_tx_thread, NULL,
						   (cfg->trx_task_stack_size != 0U) ? cfg->trx_task_stack_size : USBD_MSC_DEFAULT_TRX_TASK_STACK_SIZE,
						   (cfg->tx_task_priority != 0U) ? cfg->tx_task_priority : USBD_MSC_DEFAULT_TX_TASK_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create TX thread fail\n");
		goto create_tx_thread_fail;
	}

	info = &ep_bulk_in->info;
	info->addr = cfg->bulk_in_addr;
	info->type = USB_CH_EP_TYPE_BULK;
	ep_bulk_in->dis_zlp = 1;

	info = &ep_bulk_out->info;
	info->addr = cfg->bulk_out_addr;
	info->type = USB_CH_EP_TYPE_BULK;


	return HAL_OK;

create_tx_thread_fail:
	rtos_task_delete(cdev->rx_task);
	cdev->rx_task = NULL;

create_rx_thread_fail:
	rtos_sema_delete(cdev->tx_sema);
	cdev->tx_sema = NULL;

create_tx_sema_fail:
	rtos_sema_delete(cdev->rx_sema);
	cdev->rx_sema = NULL;

create_rx_sema_fail:
	usb_os_mfree((void *)cdev->csw);
	cdev->csw = NULL;

csw_fail:
	usb_os_mfree((void *)cdev->cbw);
	cdev->cbw = NULL;

cbw_fail:
	usb_os_mfree((void *)cdev->data);
	cdev->data = NULL;

data_buf_fail:
	usb_os_lock_delete(usbd_msc_sd_lock);
	usbd_msc_sd_lock = NULL;
	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_msc_disk_init(void)
{
	return usbd_msc_media_init();
}

void usbd_msc_disk_deinit(void)
{
	usbd_msc_media_deinit();
}

int usbd_msc_init(const usbd_msc_cb_t *cb, const usbd_msc_config_t *cfg)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	int ret;

	ret = usbd_msc_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 0U;
		usbd_register_class(&usbd_msc_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_msc_init(const usbd_msc_cb_t *cb, const usbd_msc_config_t *cfg)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	int ret;

	ret = usbd_msc_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 1U;
		ret = usbd_composite_register_driver(&usbd_msc_driver);
		if (ret != HAL_OK) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_msc_deinit();
		}
	}
	return ret;
}
#endif

/**
  * @brief  De-Initialize MSC device
  * @retval Status
  */
void usbd_msc_deinit(void)
{
	usbd_msc_dev_t *cdev = &usbd_msc_dev;

#ifdef CONFIG_USBD_COMPOSITE
	/* Leave the composite framework before anything is torn down: it is refused while the
	   composite device is still registered on the core, and the class must then stay intact */
	if (cdev->from_composite != 0U) {
		if (usbd_composite_unregister_driver(&usbd_msc_driver) != HAL_OK) {
			return;
		}
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit\n");

	/* Unregister first: no endpoint completion can give tx_sema/rx_sema afterwards, so they can
	   be deleted below without an ISR giving a freed semaphore */
#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite == 0U)
#endif
	{
		usbd_unregister_class();
	}

	/* Both threads hold the lock while they process a command. Taking it waits for a command in
	   progress to finish, so neither thread is deleted while it owns the lock or uses the buffers
	   freed below. A thread blocked on its semaphore or on the lock itself can be deleted safely.
	   The lock is created first and released last by private_init, so without it no thread exists. */
	if (usbd_msc_sd_lock != NULL) {
		usb_os_lock(usbd_msc_sd_lock);
		rtos_task_delete(cdev->tx_task);
		cdev->tx_task = NULL;
		rtos_task_delete(cdev->rx_task);
		cdev->rx_task = NULL;
		usb_os_unlock(usbd_msc_sd_lock);
	}

	/* Cleared once deleted, so that a repeated deinit does not delete them again */
	if (cdev->tx_sema != NULL) {
		rtos_sema_delete(cdev->tx_sema);
		cdev->tx_sema = NULL;
	}
	if (cdev->rx_sema != NULL) {
		rtos_sema_delete(cdev->rx_sema);
		cdev->rx_sema = NULL;
	}

	usb_os_mfree((void *)cdev->csw);
	cdev->csw = NULL;

	usb_os_mfree((void *)cdev->cbw);
	cdev->cbw = NULL;

	usb_os_mfree((void *)cdev->data);
	cdev->data = NULL;

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
int usbd_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u32 len)
{
	int ret = HAL_ERR_HW;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_bulk_in;

	if (dev->is_ready != 0U) {
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
* @param  buf_len: data buffer capacity, shall cover the DMA window, refer to usbd_ep_receive
* @retval status
*/
int usbd_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u32 len, u32 buf_len)
{
	int ret = HAL_ERR_HW;
	usbd_msc_dev_t *cdev = &usbd_msc_dev;
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;

	if (dev->is_ready != 0U) {
		ep_bulk_out->xfer_buf = buf;
		ep_bulk_out->xfer_len = len;
		ep_bulk_out->xfer_buf_len = buf_len;
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
	usb_msc_bot_cbw_t *cbw = cdev->cbw;
	usb_msc_bot_csw_t *csw = cdev->csw;
#if USBD_MSC_FIX_CV_TEST_ISSUE
	usbd_ep_t *ep_bulk_out = &cdev->ep_bulk_out;
#endif

	csw->field.dCSWSignature = USB_MSC_CSW_SIGN;
	csw->field.bCSWStatus = status;
	cdev->bot_state = USBD_MSC_IDLE;
	cdev->bot_status = USBD_MSC_STATUS_NORMAL;

	if (usbd_msc_bulk_transmit(dev, (u8 *)csw, USB_MSC_CSW_LEN) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_in_addr);
	}

#if USBD_MSC_FIX_CV_TEST_ISSUE
	/* After BOT Reset, do a one-time OUT endpoint reinit for CV test compliance. */
	if (cdev->bot_reset_pending != 0U) {
		cdev->bot_reset_pending = 0U;
		usbd_ep_deinit(dev, ep_bulk_out);
		usbd_ep_init(dev, ep_bulk_out);
	}
#endif

	/* Prepare EP to Receive next Cmd. A failure here means the device is no longer ready
	 * (suspend/disconnect); the endpoint is left un-armed and the controller NAKs, which is
	 * the correct flow control, ref USB 2.0 8.4.6. Keep bot_status in RECOVERY so the next
	 * Bulk-Only Mass Storage Reset re-arms Bulk-Out instead of resuming a lost BOT phase. */
	if (usbd_msc_bulk_receive(dev, (u8 *)cbw, USB_MSC_CBW_LEN, USBD_MSC_CBW_BUF_LEN) != HAL_OK) {
		cdev->bot_status = USBD_MSC_STATUS_RECOVERY;
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x xfer err\n", cdev->cfg->bulk_out_addr);
	}
}
