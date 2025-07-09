/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_fullmac_class.h"
#include "usbd_otp.h"
#include "usbd_core.h"
#include "usbd_pcd.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE		0x2EU

#define USBD_VID									0x0BDA
#define USBD_PID									0x8006
#define USBD_LANGID_STRING							0x409
#define USBD_MANUFACTURER_STRING					"Realtek"
#define USBD_PRODUCT_STRING							"802.11n  WLAN Adapter"
#define USBD_SERIAL_STRING							"00E04C000001"

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static u8 USBD_Class_Init(USBD_HandleTypeDef *pdev, u8 cfgidx);
static u8 USBD_Class_DeInit(USBD_HandleTypeDef *pdev, u8 cfgidx);
static u8 USBD_Class_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static u8 USBD_Class_DataIn(USBD_HandleTypeDef *pdev, u8 epnum);
static u8 USBD_Class_DataOut(USBD_HandleTypeDef *pdev, u8 epnum, u32 len);
static u8 USBD_Class_EP0_RxReady(USBD_HandleTypeDef *pdev);
static u8 USBD_Class_PrepareReceive(void);
static u8 *USBD_Class_DeviceDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_LangIDStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_ManufacturerStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_ProductStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_SerialStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_GetConfigDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
static u8 *USBD_Class_GetOtherSpeedConfigDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
static u8 *USBD_Class_GetDeviceQualifierDescriptor(USBD_HandleTypeDef *pdev, u16 *length);
#endif

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "USB";

static u8 USBD_SingleWiFiMode_DeviceDesc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,             // bLength
	USB_DESC_TYPE_DEVICE,         // bDescriptorType
	0x00,                         // bcdUSB
	0x02,
	0x00,                         // bDeviceClass
	0x00,                         // bDeviceSubClass
	0x00,                         // bDeviceProtocol
	USB_MAX_EP0_SIZE,             // bMaxPacketSize
	LOBYTE(USBD_VID),             // idVendor
	HIBYTE(USBD_VID),             // idVendor
	LOBYTE(USBD_PID),             // idVendor
	HIBYTE(USBD_PID),             // idVendor
	0x00,                         // bcdDevice
	0x00,
	USBD_IDX_MFC_STR,             // Index of manufacturer string
	USBD_IDX_PRODUCT_STR,         // Index of product string
	USBD_IDX_SERIAL_STR,          // Index of serial number string
	1                             // bNumConfigurations
}; // USB_DeviceDescriptor

/* USB Standard Device Descriptor */
static u8 USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING),
};

/* USB Standard Device Descriptor */
static u8 USBD_Class_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,
	USB_DESC_TYPE_DEVICE_QUALIFIER,
	0x00,
	0x02,
	0x00,
	0x00,
	0x00,
	USB_MAX_EP0_SIZE,
	0x01,
	0x00,
};

/* USB Configuration Descriptor for WiFi-only mode */
static u8 USBD_Class_WiFiOnlyMode_CfgDesc[USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	LOBYTE(USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),	// wTotalLength: number of returned bytes
	HIBYTE(USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),
	0x01,									// bNumInterfaces: 1 interface
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	0x00,									// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	0xFF,									// bInterfaceClass: Vendor Specific
	0xFF,									// bInterfaceSubClass
	0xFF,									// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_PRODUCT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x84,									// bEndpointAddress: EP4 IN
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x00,									// wMaxPacketSize: 512 bytes
	0x02,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x05,									// bEndpointAddress: EP5 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x00,									// wMaxPacketSize: 512 bytes
	0x02,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x06,									// bEndpointAddress: EP6 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x00,									// wMaxPacketSize: 512 bytes
	0x02,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x07,									// bEndpointAddress: EP7 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x00,									// wMaxPacketSize: 512 bytes
	0x02,
	0x00,									// bInterval
};

/* USB Full Speed Configuration Descriptor for WiFi-only mode */
static u8 USBD_Class_WiFiOnlyMode_FullSpeedCfgDesc[USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE] = {
	/* Configuration Descriptor */
	USB_LEN_CFG_DESC,						// bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,			// bDescriptorType: Configuration
	LOBYTE(USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),	// wTotalLength: number of returned bytes
	HIBYTE(USBD_WIFI_ONLY_MODE_CONFIG_DESC_SIZE),
	0x01,									// bNumInterfaces: 1 interface
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes: decided by eFuse
	0xFA,									// MaxPower 500 mA

	/*---------------------------------------------------------------------------*/

	/* Interface Descriptor */
	USB_LEN_IF_DESC,						// bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,				// bDescriptorType: Interface
	0x00,									// bInterfaceNumber: Number of Interface
	0x00,									// bAlternateSetting: Alternate setting
	0x04,									// bNumEndpoints: 4 endpoints
	0xFF,									// bInterfaceClass: Vendor Specific
	0xFF,									// bInterfaceSubClass
	0xFF,									// bInterfaceProtocol
	USBD_IDX_PRODUCT_STR,					// iInterface: USBD_PRODUCT_STRING

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x84,									// bEndpointAddress: EP4 IN
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x40,									// wMaxPacketSize: 64 bytes
	0x00,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x05,									// bEndpointAddress: EP5 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x40,									// wMaxPacketSize: 64 bytes
	0x00,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x06,									// bEndpointAddress: EP6 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x40,									// wMaxPacketSize: 64 bytes
	0x00,
	0x00,									// bInterval

	/* Endpoint Descriptor */
	USB_LEN_EP_DESC,						// bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, 				// bDescriptorType: Endpoint
	0x07,									// bEndpointAddress: EP7 OUT
	USBD_EP_TYPE_BULK,						// bmAttributes: BULK
	0x40,									// wMaxPacketSize: 64 bytes
	0x00,
	0x00,									// bInterval
};

/* INIC interface class callbacks structure */
USBD_ClassTypeDef  USBD_FULLMAC = {
	.Init = USBD_Class_Init,
	.DeInit = USBD_Class_DeInit,
	.Setup = USBD_Class_Setup,
	.EP0_TxSent = NULL,
	.EP0_RxReady = USBD_Class_EP0_RxReady,
	.DataIn = USBD_Class_DataIn,
	.DataOut = USBD_Class_DataOut,
	.SOF = NULL,
	.GetDeviceDescriptor = USBD_Class_DeviceDescriptor,
	.GetLangIDStrDescriptor = USBD_Class_LangIDStrDescriptor,
	.GetManufacturerStrDescriptor = USBD_Class_ManufacturerStrDescriptor,
	.GetProductStrDescriptor = USBD_Class_ProductStrDescriptor,
	.GetSerialStrDescriptor = USBD_Class_SerialStrDescriptor,
	.GetConfigDescriptor = USBD_Class_GetConfigDescriptor,
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	.GetOtherSpeedConfigDescriptor = USBD_Class_GetOtherSpeedConfigDescriptor,
	.GetDeviceQualifierDescriptor = USBD_Class_GetDeviceQualifierDescriptor,
#endif
};

USBD_Class_HandleTypeDef USBD_Class_Handle;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  USBD_Class_Init
  *         Initialize the INIC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static u8  USBD_Class_Init(USBD_HandleTypeDef *pdev, u8 cfgidx)
{
	u8 ret = 0U;
	USBD_Class_HandleTypeDef   *hcdc;

	UNUSED(cfgidx);

	if (pdev->dev_speed == USBD_SPEED_HIGH) {
		/* Open EP IN */
		USBD_OpenEP(pdev, USBD_BULK_IN_EP, USBD_EP_TYPE_BULK, USBD_BULK_HS_MPS);

		pdev->ep_in[USBD_BULK_IN_EP & 0xFU].is_used = 1U;

		/* Open EP OUT */
		USBD_OpenEP(pdev, USBD_BULK_OUT_EP, USBD_EP_TYPE_BULK, USBD_BULK_HS_MPS);

		pdev->ep_out[USBD_BULK_OUT_EP & 0xFU].is_used = 1U;

	} else {
		/* Open EP IN */
		USBD_OpenEP(pdev, USBD_BULK_IN_EP, USBD_EP_TYPE_BULK, USBD_BULK_FS_MPS);

		pdev->ep_in[USBD_BULK_IN_EP & 0xFU].is_used = 1U;

		/* Open EP OUT */
		USBD_OpenEP(pdev, USBD_BULK_OUT_EP, USBD_EP_TYPE_BULK, USBD_BULK_FS_MPS);

		pdev->ep_out[USBD_BULK_OUT_EP & 0xFU].is_used = 1U;
	}

	hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;

	/* Init  physical Interface components */
	((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Init();

	/* Init Xfer states */
	hcdc->TxState = 0U;
	hcdc->RxState = 0U;

	USBD_Class_PrepareReceive();

	return ret;
}

/**
  * @brief  USBD_Class_Init
  *         DeInitialize the INIC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static u8  USBD_Class_DeInit(USBD_HandleTypeDef *pdev, u8 cfgidx)
{
	u8 ret = 0U;

	UNUSED(cfgidx);

	/* Close EP IN */
	USBD_CloseEP(pdev, USBD_BULK_IN_EP);
	pdev->ep_in[USBD_BULK_IN_EP & 0xFU].is_used = 0U;

	/* Close EP OUT */
	USBD_CloseEP(pdev, USBD_BULK_OUT_EP);
	pdev->ep_out[USBD_BULK_OUT_EP & 0xFU].is_used = 0U;

	/* DeInit  physical Interface components */
	((USBD_ClassCallbackTypeDef *)pdev->pUserData)->DeInit();

	return ret;
}

/**
  * @brief  USBD_Class_Setup
  *         Handle the INIC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static u8  USBD_Class_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_Class_HandleTypeDef   *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;
	USBD_SetupReqTypedef *preq = &hcdc->Request;
	u8 ifalt = 0U;
	u16 status_info = 0U;
	u8 ret = HAL_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS :
	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if (req->bmRequest & 0x80U) {
				ret = ((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Setup(req, hcdc->Data);
				if (ret == HAL_OK) {
					USBD_CtlSendData(pdev, hcdc->Data, req->wLength);
				}
			} else {
				preq->bmRequest = req->bmRequest;
				preq->bRequest = req->bRequest;
				preq->wValue = req->wValue;
				preq->wIndex = req->wIndex;
				preq->wLength = req->wLength;
				USBD_CtlPrepareRx(pdev, hcdc->Data, req->wLength);
			}
		} else {
			((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Setup(req, NULL);
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
  * @brief  USBD_Class_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static u8  USBD_Class_DataIn(USBD_HandleTypeDef *pdev, u8 epnum)
{
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *)pdev->pClassData;

	if (hcdc != NULL) {
		pdev->ep_in[epnum].total_length = 0U;
		//RTK_LOGS(TAG, RTK_LOG_DEBUG,"EP%d TX done\n", epnum);
		hcdc->TxState = 0U;
		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

/**
  * @brief  USBD_Class_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static u8 USBD_Class_DataOut(USBD_HandleTypeDef *pdev, u8 epnum, u32 len)
{
	UNUSED(epnum);
	u8 ret = HAL_ERR_HW;
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *)pdev->pClassData;

	/* USB data will be immediately processed, this allow next USB traffic being
	NAKed till the end of the application Xfer */
	if ((hcdc != NULL) && (pdev->pUserData != NULL)) {
		/* Get the received data length */
		hcdc->RxLength = len;
		((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Receive(hcdc);//((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Receive(hcdc->RxBuffer, len);
		ret = HAL_OK;
	}

	USBD_Class_PrepareReceive();

	return ret;
}

/**
  * @brief  USBD_Class_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static u8  USBD_Class_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	USBD_Class_HandleTypeDef   *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;

	if ((hcdc != NULL) && (pdev->pUserData != NULL) && (hcdc->Request.bRequest != 0xFFU)) {
		((USBD_ClassCallbackTypeDef *)pdev->pUserData)->Setup(&hcdc->Request, hcdc->Data);
		hcdc->Request.bRequest = 0xFFU;
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
static u8 *USBD_Class_DeviceDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;
	USB_OTP_TypeDef *otp = &pdev->OTP;
	u8 *desc;

	desc = USBD_SingleWiFiMode_DeviceDesc;
	*length = USB_LEN_DEV_DESC;

	if (otp->otp_param) {
		_memcpy((void *)hcdc->Data, (const void *)desc, *length);
		// Update PID/VID from eFuse
		hcdc->Data[USB_DEV_DESC_OFFSET_VID] = LOBYTE(otp->vid);
		hcdc->Data[USB_DEV_DESC_OFFSET_VID + 1] = HIBYTE(otp->vid);
		hcdc->Data[USB_DEV_DESC_OFFSET_PID] = LOBYTE(otp->pid);
		hcdc->Data[USB_DEV_DESC_OFFSET_PID + 1] = HIBYTE(otp->pid);
		return hcdc->Data;
	} else {
		return desc;
	}

	return hcdc->Data;
}

/**
  * @brief  Returns the LangID string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static u8 *USBD_Class_LangIDStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	UNUSED(pdev);

	*length = sizeof(USBD_LangIDDesc);
	return (u8 *) USBD_LangIDDesc;
}

/**
  * @brief  Returns the product string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static u8 *USBD_Class_ProductStrDescriptor(USBD_HandleTypeDef *pdev,	u16 *length)
{
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;
	USB_OTP_TypeDef *otp = &pdev->OTP;

	if (otp->otp_param) {
		*length = otp->prod_str_len;
		return otp->prod_str;
	} else {
		USBD_GetString((u8 *)USBD_PRODUCT_STRING, hcdc->Data, length);
		return hcdc->Data;
	}
}

/**
  * @brief  Returns the manufacturer string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static u8 *USBD_Class_ManufacturerStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;

	UNUSED(pdev);
	USB_OTP_TypeDef *otp = &pdev->OTP;

	if (otp->otp_param) {
		*length = otp->mfg_str_len;
		return otp->mfg_str;
	} else {
		USBD_GetString((u8 *)USBD_MANUFACTURER_STRING, hcdc->Data, length);
		return hcdc->Data;
	}
}

/**
  * @brief  Returns the serial number string descriptor.
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
static u8 *USBD_Class_SerialStrDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	UNUSED(pdev);
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;
	USB_OTP_TypeDef *otp = &pdev->OTP;
	if (otp->otp_sn) {
		*length = otp->sn_str_len;
		return otp->sn_str;
	} else {
		USBD_GetString((u8 *) USBD_SERIAL_STRING, hcdc->Data, length);
		return hcdc->Data;
	}
}

/**
  * @brief  Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static u8 *USBD_Class_GetConfigDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	u8 *desc;
	u16 len;
	USB_OTP_TypeDef *otp = &pdev->OTP;
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *)pdev->pClassData;

	if (pdev->dev_speed == USBD_SPEED_HIGH) {
		desc = USBD_Class_WiFiOnlyMode_CfgDesc;
		len = sizeof(USBD_Class_WiFiOnlyMode_CfgDesc);
	} else {
		desc = USBD_Class_WiFiOnlyMode_FullSpeedCfgDesc;
		len = sizeof(USBD_Class_WiFiOnlyMode_FullSpeedCfgDesc);
	}

	_memcpy((void *)hcdc->Data, (void *)desc, len);
	//RTK_LOGD(TAG, "USBD_Class_GetConfigDescriptor before 0x%02X self_powered=%d remote_wakeup_en=%d\n",
	//		   hcdc->DescBuffer[USB_CFG_DESC_OFFSET_ATTR], otp->self_powered, otp->remote_wakeup_en);

	hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] &= ~(USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED | USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP);
	if (otp->self_powered) {
		hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
	}
	if (otp->remote_wakeup_en) {
		hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
	}
	//RTK_LOGD(TAG, "USBD_Class_GetConfigDescriptor after 0x%02X\n", hcdc->DescBuffer[USB_CFG_DESC_OFFSET_ATTR]);

	*length = len;

	return hcdc->Data;
}

#ifndef CONFIG_SUPPORT_USB_FS_ONLY

/**
  * @brief  USBD_Class_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static u8 *USBD_Class_GetOtherSpeedConfigDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;
	u8 *desc;
	u16 len;
	USB_OTP_TypeDef *otp = &pdev->OTP;

	if (pdev->dev_speed == USBD_SPEED_HIGH) {
		desc = USBD_Class_WiFiOnlyMode_FullSpeedCfgDesc;
		len = sizeof(USBD_Class_WiFiOnlyMode_FullSpeedCfgDesc);
	} else {
		desc = USBD_Class_WiFiOnlyMode_CfgDesc;
		len = sizeof(USBD_Class_WiFiOnlyMode_CfgDesc);
	}

	_memcpy((void *)hcdc->Data, (const void *)desc, len);

	hcdc->Data[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
	hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] &= ~(USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED | USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP);
	if (otp->self_powered) {
		hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
	}
	if (otp->remote_wakeup_en) {
		hcdc->Data[USB_CFG_DESC_OFFSET_ATTR] |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
	}

	*length = len;
	return hcdc->Data;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static u8 *USBD_Class_GetDeviceQualifierDescriptor(USBD_HandleTypeDef *pdev, u16 *length)
{
	UNUSED(pdev);
	*length = sizeof(USBD_Class_DeviceQualifierDesc);
	return (u8 *)USBD_Class_DeviceQualifierDesc;
}

#endif // #ifdef CONFIG_SUPPORT_USB_FS_ONLY

/**
  * @brief  USBD_Class_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
static u8 USBD_Class_PrepareReceive(void)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;

	/* Suspend or Resume USB Out process */
	if (hcdc != NULL) {
		USBD_PrepareReceive(pdev, USBD_BULK_OUT_EP, hcdc->RxBuffer, USBD_DATA_OUT_BUF_SIZE);
		return HAL_OK;
	} else {
		return HAL_ERR_HW;
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USBD_Class_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
u8 USBD_Class_Transmit(u8 *buf, u32 len)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	USBD_Class_HandleTypeDef *hcdc = (USBD_Class_HandleTypeDef *) pdev->pClassData;

	if (hcdc != NULL) {
		if (hcdc->TxState == 0U) {
			//RTK_LOGS(TAG, RTK_LOG_DEBUG,"EP%d TX: %d\n", USBD_BULK_IN_EP & 0xFU, len);
			/* Tx Transfer in progress */
			hcdc->TxState = 1U;

			/* Update the packet total length */
			pdev->ep_in[USBD_BULK_IN_EP & 0xFU].total_length = len;

			/* Transmit next packet */
			USBD_Transmit(pdev, USBD_BULK_IN_EP, buf, len);

			return HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "EP%d TX %d BUSY\n", USBD_BULK_IN_EP & 0xFU, len);
			return HAL_BUSY;
		}
	} else {
		return HAL_ERR_HW;
	}
}
