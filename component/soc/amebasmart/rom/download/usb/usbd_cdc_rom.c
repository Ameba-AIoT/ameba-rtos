/**
  ******************************************************************************
  * @file    usbd_cdc_rom.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides the functionalities of the USB CDC Class
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

#include "usbd_cdc_rom.h"
#include "usbd_core_rom.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_VID                      0x0BDA
#define USBD_PID                      0x8730
#define USBD_LANGID_STRING            0x409
#define USBD_MAX_STR_DESC_SIZE        0x100
#define USBD_SERIAL_STRING_SIZE       12
#define USBD_MANUFACTURER_STRING      "Realtek"
#define USBD_PRODUCT_HS_STRING        "Realtek VCP (HS)"
#define USBD_PRODUCT_FS_STRING        "Realtek VCP (FS)"

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 USBD_CDC_Init(USBD_HandleTypeDef *pdev, u8 cfgidx);
static u8 USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, u8 cfgidx);
static u8 USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static u8 USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, u8 epnum);
static u8 USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, u8 epnum);
static u8 USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);
static u8 *USBD_CDC_DeviceDescriptor(USBD_SpeedTypeDef speed, u16 *length);
static u8 *USBD_CDC_LangIDStrDescriptor(USBD_SpeedTypeDef speed, u16 *length);
static u8 *USBD_CDC_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, u16 *length);
static u8 *USBD_CDC_ProductStrDescriptor(USBD_SpeedTypeDef speed, u16 *length);
static u8 *USBD_CDC_SerialStrDescriptor(USBD_SpeedTypeDef speed, u16 *length);
static u8 *USBD_CDC_GetFullSpeedConfigDescriptor(u16 *length);
static u8 *USBD_CDC_GetHighSpeedConfigDescriptor(u16 *length);
static u8 *USBD_CDC_GetOtherSpeedConfigDescriptor(u16 *length);
static u8 *USBD_CDC_GetDeviceQualifierDescriptor(u16 *length);

/* Private variables ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
	0x12,                         /* bLength */
	USB_DESC_TYPE_DEVICE,         /* bDescriptorType */
	0x00,                         /* bcdUSB */
	0x02,
	0x02,                         /* bDeviceClass */
	0x02,                         /* bDeviceSubClass */
	0x00,                         /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,             /* bMaxPacketSize */
	LOBYTE(USBD_VID),             /* idVendor */
	HIBYTE(USBD_VID),             /* idVendor */
	LOBYTE(USBD_PID),             /* idVendor */
	HIBYTE(USBD_PID),             /* idVendor */
	0x00,                         /* bcdDevice rel. 2.00 */
	0x02,
	USBD_IDX_MFC_STR,             /* Index of manufacturer string */
	USBD_IDX_PRODUCT_STR,         /* Index of product string */
	USBD_IDX_SERIAL_STR,          /* Index of serial number string */
	USBD_MAX_NUM_CONFIGURATION    /* bNumConfigurations */
};                              /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING),
};

USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_StringSerial[USBD_SERIAL_STRING_SIZE] __ALIGN_END = {
	USBD_SERIAL_STRING_SIZE,
	USB_DESC_TYPE_STRING,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'A'
};

USBD_BSS_SECTION
__ALIGN_BEGIN static u8 USBD_StrDesc[USBD_MAX_STR_DESC_SIZE] __ALIGN_END;

/* USB Standard Device Descriptor */
USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_CDC_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
	USB_LEN_DEV_QUALIFIER_DESC,
	USB_DESC_TYPE_DEVICE_QUALIFIER,
	0x00,
	0x02,
	0x00,
	0x00,
	0x00,
	0x40,
	0x01,
	0x00,
};

/* USB CDC device Configuration Descriptor */
USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_CDC_CfgHSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
	/*Configuration Descriptor*/
	0x09,   /* bLength: Configuration Descriptor size */
	USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
	USB_CDC_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
	0x00,
	0x02,   /* bNumInterfaces: 2 interface */
	0x01,   /* bConfigurationValue: Configuration value */
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
#if USBD_SELF_POWERED
	0xC0,   /* bmAttributes: self powered */
#else
	0x80,   /* bmAttributes: bus powered */
#endif
	0x32,   /* MaxPower 0 mA */

	/*---------------------------------------------------------------------------*/

	/*Interface Descriptor */
	0x09,   /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x01,   /* bNumEndpoints: One endpoints used */
	0x02,   /* bInterfaceClass: Communication Interface Class */
	0x02,   /* bInterfaceSubClass: Abstract Control Model */
	0x01,   /* bInterfaceProtocol: Common AT commands */
	0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,

	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	0x01,   /* bDataInterface: 1 */

	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	0x02,   /* bmCapabilities */

	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	0x00,   /* bMasterInterface: Communication class interface */
	0x01,   /* bSlaveInterface0: Data Class Interface */

	/*Endpoint 2 Descriptor*/
	0x07,                           /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint */
	CDC_CMD_EP,                     /* bEndpointAddress */
	0x03,                           /* bmAttributes: Interrupt */
	LOBYTE(CDC_CMD_PACKET_SIZE),    /* wMaxPacketSize: */
	HIBYTE(CDC_CMD_PACKET_SIZE),
	CDC_HS_BINTERVAL,               /* bInterval: */
	/*---------------------------------------------------------------------------*/

	/*Data class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x01,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x02,   /* bNumEndpoints: Two endpoints used */
	0x0A,   /* bInterfaceClass: CDC */
	0x00,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */

	/*Endpoint OUT Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
	CDC_OUT_EP,                        /* bEndpointAddress */
	0x02,                              /* bmAttributes: Bulk */
	LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
	0x00,                              /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
	CDC_IN_EP,                         /* bEndpointAddress */
	0x02,                              /* bmAttributes: Bulk */
	LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
	0x00                               /* bInterval: ignore for Bulk transfer */
} ;

/* USB CDC device Configuration Descriptor */
USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_CDC_CfgFSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
	/*Configuration Descriptor*/
	0x09,   /* bLength: Configuration Descriptor size */
	USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
	USB_CDC_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
	0x00,
	0x02,   /* bNumInterfaces: 2 interface */
	0x01,   /* bConfigurationValue: Configuration value */
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
#if USBD_SELF_POWERED
	0xC0,	/* bmAttributes: self powered */
#else
	0x80,	/* bmAttributes: bus powered */
#endif
	0x32,   /* MaxPower 0 mA */

	/*---------------------------------------------------------------------------*/

	/*Interface Descriptor */
	0x09,   /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x01,   /* bNumEndpoints: One endpoints used */
	0x02,   /* bInterfaceClass: Communication Interface Class */
	0x02,   /* bInterfaceSubClass: Abstract Control Model */
	0x01,   /* bInterfaceProtocol: Common AT commands */
	0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,

	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	0x01,   /* bDataInterface: 1 */

	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	0x02,   /* bmCapabilities */

	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	0x00,   /* bMasterInterface: Communication class interface */
	0x01,   /* bSlaveInterface0: Data Class Interface */

	/*Endpoint 2 Descriptor*/
	0x07,                           /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
	CDC_CMD_EP,                     /* bEndpointAddress */
	0x03,                           /* bmAttributes: Interrupt */
	LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
	HIBYTE(CDC_CMD_PACKET_SIZE),
	CDC_FS_BINTERVAL,                           /* bInterval: */
	/*---------------------------------------------------------------------------*/

	/*Data class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x01,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x02,   /* bNumEndpoints: Two endpoints used */
	0x0A,   /* bInterfaceClass: CDC */
	0x00,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */

	/*Endpoint OUT Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
	CDC_OUT_EP,                        /* bEndpointAddress */
	0x02,                              /* bmAttributes: Bulk */
	LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
	0x00,                              /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
	CDC_IN_EP,                         /* bEndpointAddress */
	0x02,                              /* bmAttributes: Bulk */
	LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
	0x00                               /* bInterval: ignore for Bulk transfer */
} ;

USBD_DATA_SECTION
__ALIGN_BEGIN static u8 USBD_CDC_OtherSpeedCfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END = {
	0x09,   /* bLength: Configuation Descriptor size */
	USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
	USB_CDC_CONFIG_DESC_SIZ,
	0x00,
	0x02,   /* bNumInterfaces: 2 interfaces */
	0x01,   /* bConfigurationValue: */
	0x00,   /* iConfiguration: */
#if USBD_SELF_POWERED
	0xC0,	/* bmAttributes: self powered */
#else
	0x80,	/* bmAttributes: bus powered */
#endif
	0x32,   /* MaxPower 100 mA */

	/*Interface Descriptor */
	0x09,   /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x01,   /* bNumEndpoints: One endpoints used */
	0x02,   /* bInterfaceClass: Communication Interface Class */
	0x02,   /* bInterfaceSubClass: Abstract Control Model */
	0x01,   /* bInterfaceProtocol: Common AT commands */
	0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,

	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	0x01,   /* bDataInterface: 1 */

	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	0x02,   /* bmCapabilities */

	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	0x00,   /* bMasterInterface: Communication class interface */
	0x01,   /* bSlaveInterface0: Data Class Interface */

	/*Endpoint 2 Descriptor*/
	0x07,                           /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint */
	CDC_CMD_EP,                     /* bEndpointAddress */
	0x03,                           /* bmAttributes: Interrupt */
	LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
	HIBYTE(CDC_CMD_PACKET_SIZE),
	CDC_FS_BINTERVAL,                           /* bInterval: */

	/*---------------------------------------------------------------------------*/

	/*Data class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x01,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x02,   /* bNumEndpoints: Two endpoints used */
	0x0A,   /* bInterfaceClass: CDC */
	0x00,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */

	/*Endpoint OUT Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
	CDC_OUT_EP,                        /* bEndpointAddress */
	0x02,                              /* bmAttributes: Bulk */
	0x40,                              /* wMaxPacketSize: */
	0x00,
	0x00,                              /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,     /* bDescriptorType: Endpoint */
	CDC_IN_EP,                        /* bEndpointAddress */
	0x02,                             /* bmAttributes: Bulk */
	0x40,                             /* wMaxPacketSize: */
	0x00,
	0x00                              /* bInterval */
};

/* CDC interface class callbacks structure */
USBD_WEEK USBD_DATA_SECTION
USBD_ClassTypeDef  USBD_CDC = {
	.Init = USBD_CDC_Init,
	.DeInit = USBD_CDC_DeInit,
	.Setup = USBD_CDC_Setup,
	.EP0_TxSent = NULL,
	.EP0_RxReady = USBD_CDC_EP0_RxReady,
	.DataIn = USBD_CDC_DataIn,
	.DataOut = USBD_CDC_DataOut,
	.SOF = NULL,
	.GetDeviceDescriptor = USBD_CDC_DeviceDescriptor,
	.GetLangIDStrDescriptor = USBD_CDC_LangIDStrDescriptor,
	.GetManufacturerStrDescriptor = USBD_CDC_ManufacturerStrDescriptor,
	.GetProductStrDescriptor = USBD_CDC_ProductStrDescriptor,
	.GetSerialStrDescriptor = USBD_CDC_SerialStrDescriptor,
	.GetHSConfigDescriptor = USBD_CDC_GetHighSpeedConfigDescriptor,
	.GetFSConfigDescriptor = USBD_CDC_GetFullSpeedConfigDescriptor,
	.GetOtherSpeedConfigDescriptor = USBD_CDC_GetOtherSpeedConfigDescriptor,
	.GetDeviceQualifierDescriptor = USBD_CDC_GetDeviceQualifierDescriptor,
};

USBD_BSS_SECTION
static USBD_CDC_HandleTypeDef USBD_CDC_Handle;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  USBD_CDC_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_Init(USBD_HandleTypeDef *pdev, u8 cfgidx)
{
	u8 ret = 0U;
	USBD_CDC_HandleTypeDef   *hcdc;

	UNUSED(cfgidx);

	if (pdev->dev_speed == USBD_SPEED_HIGH) {
		/* Open EP IN */
		USBD_OpenEP(pdev, CDC_IN_EP, USBD_EP_TYPE_BULK,
					CDC_DATA_HS_IN_PACKET_SIZE);

		pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 1U;

		/* Open EP OUT */
		USBD_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK,
					CDC_DATA_HS_OUT_PACKET_SIZE);

		pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 1U;

	} else {
		/* Open EP IN */
		USBD_OpenEP(pdev, CDC_IN_EP, USBD_EP_TYPE_BULK,
					CDC_DATA_FS_IN_PACKET_SIZE);

		pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 1U;

		/* Open EP OUT */
		USBD_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK,
					CDC_DATA_FS_OUT_PACKET_SIZE);

		pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 1U;
	}
	/* Open Command IN EP */
	USBD_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
	pdev->ep_in[CDC_CMD_EP & 0xFU].is_used = 1U;

	pdev->pClassData = &USBD_CDC_Handle;

	hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	/* Init  physical Interface components */
	((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Init();

	/* Init Xfer states */
	hcdc->TxState = 0U;
	hcdc->RxState = 0U;

	/* Prepare Out endpoint to receive next packet */
	USBD_PrepareReceive(pdev, CDC_OUT_EP, hcdc->RxBuffer,
						CDC_DATA_OUT_TRANSFER_SIZE);

	return ret;
}

/**
  * @brief  USBD_CDC_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, u8 cfgidx)
{
	u8 ret = 0U;

	UNUSED(cfgidx);

	/* Close EP IN */
	USBD_CloseEP(pdev, CDC_IN_EP);
	pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 0U;

	/* Close EP OUT */
	USBD_CloseEP(pdev, CDC_OUT_EP);
	pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 0U;

	/* Close Command IN EP */
	USBD_CloseEP(pdev, CDC_CMD_EP);
	pdev->ep_in[CDC_CMD_EP & 0xFU].is_used = 0U;

	/* DeInit  physical Interface components */
	((USBD_CDC_ItfTypeDef *)pdev->pUserData)->DeInit();

	return ret;
}

/**
  * @brief  USBD_CDC_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
	u8 ifalt = 0U;
	u16 status_info = 0U;
	u8 ret = HAL_OK;

	//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "USBD_CDC_Setup bmRequest=0x%02X bRequest=0x%02X wLength=0x%04X\n", req->bmRequest, req->bRequest, req->wLength);

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS :
		if (req->wLength) {
			if (req->bmRequest & 0x80U) {
				((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
						(u8 *)(void *)hcdc->Data,
						req->wLength);

				USBD_CtlSendData(pdev, (u8 *)(void *)hcdc->Data, req->wLength);
			} else {
				hcdc->CmdOpCode = req->bRequest;
				hcdc->CmdLength = (u8)req->wLength;

				USBD_CtlPrepareRx(pdev, (u8 *)(void *)hcdc->Data, req->wLength);
			}
		} else {
			((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest, (u8 *)(void *)req, 0U);
		}
		break;

	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			if (pdev->dev_state == USBD_STATE_CONFIGURED) {
				USBD_CtlSendData(pdev, (u8 *)(void *)&status_info, 2U);
			} else {
				USBD_CtlError(pdev, req);
				ret = HAL_ERR_HW;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (pdev->dev_state == USBD_STATE_CONFIGURED) {
				USBD_CtlSendData(pdev, &ifalt, 1U);
			} else {
				USBD_CtlError(pdev, req);
				ret = HAL_ERR_HW;
			}
			break;

		case USB_REQ_SET_INTERFACE:
			if (pdev->dev_state != USBD_STATE_CONFIGURED) {
				USBD_CtlError(pdev, req);
				ret = HAL_ERR_HW;
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = HAL_ERR_HW;
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  USBD_CDC_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, u8 epnum)
{
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassData;

	if (hcdc != NULL) {
		pdev->ep_in[epnum].total_length = 0U;
		//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "EP%d TX done\n", epnum);
		hcdc->TxState = 0U;
		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, u8 epnum)
{
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	/* USB data will be immediately processed, this allow next USB traffic being
	NAKed till the end of the application Xfer */
	if ((hcdc != NULL) && (pdev->pUserData != NULL)) {
		/* Get the received data length */
		hcdc->RxLength = USBD_GetRxDataSize(pdev, epnum);
		((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Receive(hcdc->RxBuffer, &hcdc->RxLength);

		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

/**
  * @brief  USBD_CDC_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
USBD_TEXT_SECTION
static u8  USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	if ((hcdc != NULL) && (pdev->pUserData != NULL) && (hcdc->CmdOpCode != 0xFFU)) {
		((USBD_CDC_ItfTypeDef *)pdev->pUserData)->Control(hcdc->CmdOpCode,
				(u8 *)(void *)hcdc->Data,
				(u16)hcdc->CmdLength);
		hcdc->CmdOpCode = 0xFFU;

		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

/**
  * @brief  Returns the device descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_DeviceDescriptor(USBD_SpeedTypeDef speed, u16 *length)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(speed);

	*length = sizeof(USBD_DeviceDesc);
	return (u8 *) USBD_DeviceDesc;
}

/**
  * @brief  Returns the LangID string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_LangIDStrDescriptor(USBD_SpeedTypeDef speed,
										u16 *length)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(speed);

	*length = sizeof(USBD_LangIDDesc);
	return (u8 *) USBD_LangIDDesc;
}

/**
  * @brief  Returns the product string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_ProductStrDescriptor(USBD_SpeedTypeDef speed,
		u16 *length)
{
	if (speed == 0) {
		USBD_GetString((u8 *) USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((u8 *) USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
	}
	return USBD_StrDesc;
}

/**
  * @brief  Returns the manufacturer string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed,
		u16 *length)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(speed);

	USBD_GetString((u8 *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
	return (u8 *)USBD_StrDesc;
}

/**
  * @brief  Returns the serial number string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_SerialStrDescriptor(USBD_SpeedTypeDef speed,
										u16 *length)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(speed);

	*length = USBD_SERIAL_STRING_SIZE;

	return (u8 *) USBD_StringSerial;
}

/**
  * @brief  USBD_CDC_GetFullSpeedConfigDescriptor
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_GetFullSpeedConfigDescriptor(u16 *length)
{
	*length = sizeof(USBD_CDC_CfgFSDesc);
	return (u8 *)USBD_CDC_CfgFSDesc;
}

/**
  * @brief  USBD_CDC_GetHighSpeedConfigDescriptor
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_GetHighSpeedConfigDescriptor(u16 *length)
{
	*length = sizeof(USBD_CDC_CfgHSDesc);
	return (u8 *)USBD_CDC_CfgHSDesc;
}

/**
  * @brief  USBD_CDC_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
USBD_TEXT_SECTION
static u8 *USBD_CDC_GetOtherSpeedConfigDescriptor(u16 *length)
{
	*length = sizeof(USBD_CDC_OtherSpeedCfgDesc);
	return (u8 *)USBD_CDC_OtherSpeedCfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
USBD_TEXT_SECTION
static u8 *USBD_CDC_GetDeviceQualifierDescriptor(u16 *length)
{
	*length = sizeof(USBD_CDC_DeviceQualifierDesc);
	return (u8 *)USBD_CDC_DeviceQualifierDesc;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
USBD_WEEK USBD_TEXT_SECTION
u8 USBD_CDC_SetTxBuffer(u8  *pbuff, u16 length)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	hcdc->TxBuffer = pbuff;
	hcdc->TxLength = length;

	return HAL_OK;
}

/**
  * @brief  USBD_CDC_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
USBD_WEEK USBD_TEXT_SECTION
u8 USBD_CDC_SetRxBuffer(u8  *pbuff)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	hcdc->RxBuffer = pbuff;

	return HAL_OK;
}

/**
  * @brief  USBD_CDC_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
USBD_WEEK USBD_TEXT_SECTION
u8 USBD_CDC_TransmitPacket(void)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	if (hcdc != NULL) {
		if (hcdc->TxState == 0U) {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "EP%d TX: %d\n", CDC_IN_EP & 0xFU, hcdc->TxLength);
			/* Tx Transfer in progress */
			hcdc->TxState = 1U;

			/* Update the packet total length */
			pdev->ep_in[CDC_IN_EP & 0xFU].total_length = hcdc->TxLength;

			/* Transmit next packet */
			USBD_Transmit(pdev, CDC_IN_EP, hcdc->TxBuffer, (u16)hcdc->TxLength);

			return HAL_OK;
		} else {
			//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "EP%d TX: %d BUSY\n", CDC_IN_EP & 0xFU, hcdc->TxLength);
			return HAL_BUSY;
		}
	} else {
		return HAL_ERR_HW;
	}
}

/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
USBD_WEEK USBD_TEXT_SECTION
u8  USBD_CDC_ReceivePacket(void)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

	/* Suspend or Resume USB Out process */
	if (hcdc != NULL) {
		USBD_PrepareReceive(pdev,
							CDC_OUT_EP,
							hcdc->RxBuffer,
							CDC_DATA_OUT_TRANSFER_SIZE);
		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

