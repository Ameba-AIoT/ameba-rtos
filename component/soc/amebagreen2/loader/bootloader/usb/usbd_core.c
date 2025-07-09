/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_core.h"
#include "usbd_pcd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_SetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static u8 USBD_GetStrLen(u8 *buf);
static HAL_Status USBD_SetUSBAddress(USBD_HandleTypeDef *pdev, u8 dev_addr);
static HAL_Status USBD_CtlContinueRx(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len);
static HAL_Status USBD_CtlContinueSendData(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len);
static HAL_Status USBD_CtlSendStatus(USBD_HandleTypeDef *pdev);
static HAL_Status USBD_CtlReceiveStatus(USBD_HandleTypeDef *pdev);
static void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, u8 *pdata);
#if 0
static HAL_Status USBD_FlushEP(USBD_HandleTypeDef *pdev, u8 ep_addr);
#endif
static HAL_Status USBD_StallEP(USBD_HandleTypeDef *pdev, u8 ep_addr);
static HAL_Status USBD_ClearStallEP(USBD_HandleTypeDef *pdev, u8 ep_addr);
static u8 USBD_IsStallEP(USBD_HandleTypeDef *pdev, u8 ep_addr);
static HAL_Status USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
static HAL_Status  USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
static HAL_Status USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
static HAL_Status  USBD_RunTestMode(USBD_HandleTypeDef  *pdev);
static HAL_Status USBD_SetClassConfig(USBD_HandleTypeDef  *pdev, u8 cfgidx);
static HAL_Status USBD_ClrClassConfig(USBD_HandleTypeDef  *pdev, u8 cfgidx);

/* Private variables ---------------------------------------------------------*/

/* USB device handler */
USBD_HandleTypeDef USBD_Device;
/* PCD handler */
static USB_PCD_HandleTypeDef USBD_PCDHandle;
static u8 USBD_ConfigIndex;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handle Get Descriptor requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	u16 len;
	u8 *pbuf;

	switch (req->wValue >> 8) {
	case USB_DESC_TYPE_DEVICE:
		pbuf = pdev->pClass->GetDeviceDescriptor(pdev, &len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		pbuf = (u8 *)pdev->pClass->GetConfigDescriptor(pdev, &len);
		break;

	case USB_DESC_TYPE_STRING:
		switch ((u8)(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			pbuf = pdev->pClass->GetLangIDStrDescriptor(pdev, &len);
			break;

		case USBD_IDX_MFC_STR:
			pbuf = pdev->pClass->GetManufacturerStrDescriptor(pdev, &len);
			break;

		case USBD_IDX_PRODUCT_STR:
			pbuf = pdev->pClass->GetProductStrDescriptor(pdev, &len);
			break;

		case USBD_IDX_SERIAL_STR:
			pbuf = pdev->pClass->GetSerialStrDescriptor(pdev, &len);
			break;

		default:
			//USBD_CtlError(pdev, req);
			USBD_StallEP(pdev, 0x80U);
			return;
		}
		break;
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		pbuf = (u8 *)pdev->pClass->GetDeviceQualifierDescriptor(pdev, &len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		pbuf = (u8 *)pdev->pClass->GetOtherSpeedConfigDescriptor(pdev, &len);
		break;
#endif
	default:
		USBD_CtlError(pdev, req);
		return;
	}

	if ((len != 0U) && (req->wLength != 0U)) {

		len = MIN(len, req->wLength);

		USBD_CtlSendData(pdev, pbuf, len);
	}

	if (req->wLength == 0U) {
		USBD_CtlSendStatus(pdev);
	}
}

/**
  * @brief  Set device address
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	u8  dev_addr;

	if ((req->wIndex == 0U) && (req->wLength == 0U) && (req->wValue < 128U)) {
		dev_addr = (u8)(req->wValue) & 0x7FU;

		if (pdev->dev_state == USBD_STATE_CONFIGURED) {
			USBD_CtlError(pdev, req);
		} else {
			pdev->dev_address = dev_addr;
			USBD_SetUSBAddress(pdev, dev_addr);
			USBD_CtlSendStatus(pdev);

			if (dev_addr != 0U) {
				pdev->dev_state = USBD_STATE_ADDRESSED;
			} else {
				pdev->dev_state = USBD_STATE_DEFAULT;
			}
		}
	} else {
		USBD_CtlError(pdev, req);
	}
}

/**
  * @brief  Handle Set device configuration request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_SetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_ConfigIndex = (u8)(req->wValue);

	if (USBD_ConfigIndex > USBD_MAX_NUM_CONFIGURATION) {
		USBD_CtlError(pdev, req);
	} else {
		switch (pdev->dev_state) {
		case USBD_STATE_ADDRESSED:
			if (USBD_ConfigIndex) {
				pdev->dev_config = USBD_ConfigIndex;
				pdev->dev_state = USBD_STATE_CONFIGURED;
				if (USBD_SetClassConfig(pdev, USBD_ConfigIndex) == HAL_ERR_HW) {
					USBD_CtlError(pdev, req);
					return;
				}
				USBD_CtlSendStatus(pdev);
			} else {
				USBD_CtlSendStatus(pdev);
			}
			break;

		case USBD_STATE_CONFIGURED:
			if (USBD_ConfigIndex == 0U) {
				pdev->dev_state = USBD_STATE_ADDRESSED;
				pdev->dev_config = USBD_ConfigIndex;
				USBD_ClrClassConfig(pdev, USBD_ConfigIndex);
				USBD_CtlSendStatus(pdev);
			} else if (USBD_ConfigIndex != pdev->dev_config) {
				/* Clear old configuration */
				USBD_ClrClassConfig(pdev, (u8)pdev->dev_config);

				/* set new configuration */
				pdev->dev_config = USBD_ConfigIndex;
				if (USBD_SetClassConfig(pdev, USBD_ConfigIndex) == HAL_ERR_HW) {
					USBD_CtlError(pdev, req);
					return;
				}
				USBD_CtlSendStatus(pdev);
			} else {
				USBD_CtlSendStatus(pdev);
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			USBD_ClrClassConfig(pdev, USBD_ConfigIndex);
			break;
		}
	}
}

/**
  * @brief  Handle Get device configuration request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	if (req->wLength != 1U) {
		USBD_CtlError(pdev, req);
	} else {
		switch (pdev->dev_state) {
		case USBD_STATE_DEFAULT:
		case USBD_STATE_ADDRESSED:
			pdev->dev_default_config = 0U;
			USBD_CtlSendData(pdev, (u8 *)(void *)&pdev->dev_default_config, 1U);
			break;

		case USBD_STATE_CONFIGURED:
			USBD_CtlSendData(pdev, (u8 *)(void *)&pdev->dev_config, 1U);
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
		}
	}
}

/**
  * @brief  Handle Get Status request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	switch (pdev->dev_state) {
	case USBD_STATE_DEFAULT:
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:
		if (req->wLength != 0x2U) {
			USBD_CtlError(pdev, req);
			break;
		}

#if (USBD_SELF_POWERED == 1U)
		pdev->dev_config_status = USB_CONFIG_SELF_POWERED;
#else
		pdev->dev_config_status = 0U;
#endif

		if (pdev->dev_remote_wakeup) {
			pdev->dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;
		}

		USBD_CtlSendData(pdev, (u8 *)(void *)&pdev->dev_config_status, 2U);
		break;

	default :
		USBD_CtlError(pdev, req);
		break;
	}
}

/**
  * @brief  Handle Set device feature request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{

	if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
		pdev->dev_remote_wakeup = 1U;
		USBD_CtlSendStatus(pdev);
	}

}

/**
  * @brief  Handle clear device feature request
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	switch (pdev->dev_state) {
	case USBD_STATE_DEFAULT:
	case USBD_STATE_ADDRESSED:
	case USBD_STATE_CONFIGURED:
		if (req->wValue == USB_FEATURE_REMOTE_WAKEUP) {
			pdev->dev_remote_wakeup = 0U;
			USBD_CtlSendStatus(pdev);
		}
		break;

	default :
		USBD_CtlError(pdev, req);
		break;
	}
}

/**
  * @brief  Return the string length
  * @param  buf : pointer to the ascii string buffer
  * @retval string length
  */
static u8 USBD_GetStrLen(u8 *buf)
{
	u8  len = 0U;

	while (*buf != '\0') {
		len++;
		buf++;
	}

	return len;
}

/**
  * @brief  Assigns an USB address to the device
  * @param  pdev: Device handle
  * @param  dev_addr: USB address
  * @retval USBD Status
  */
static HAL_Status USBD_SetUSBAddress(USBD_HandleTypeDef *pdev, u8 dev_addr)
{
	USB_PCD_SetAddress(pdev->pData, dev_addr);
	return HAL_OK;
}

/**
  * @brief  Continue receive data on the ctl pipe
  * @param  pdev: device instance
  * @param  buff: pointer to data buffer
  * @param  len: length of data to be received
  * @retval status
  */
static HAL_Status USBD_CtlContinueRx(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len)
{
	USBD_PrepareReceive(pdev, 0U, pbuf, len);

	return HAL_OK;
}

/**
  * @brief  Continue sending data on the ctl pipe
  * @param  pdev: device instance
  * @param  buff: pointer to data buffer
  * @param  len: length of data to be sent
  * @retval status
  */
static HAL_Status USBD_CtlContinueSendData(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len)
{
	/* Start the next transfer */
	USBD_Transmit(pdev, 0x00U, pbuf, len);

	return HAL_OK;
}

/**
  * @brief  Send zero lzngth packet on the ctl pipe
  * @param  pdev: device instance
  * @retval status
  */
static HAL_Status USBD_CtlSendStatus(USBD_HandleTypeDef *pdev)
{
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_STATUS_IN;

	/* Start the transfer */
	USBD_Transmit(pdev, 0x00U, NULL, 0U);

	return HAL_OK;
}

/**
  * @brief  Receive zero lzngth packet on the ctl pipe
  * @param  pdev: device instance
  * @retval status
  */
static HAL_Status USBD_CtlReceiveStatus(USBD_HandleTypeDef *pdev)
{
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_STATUS_OUT;

	/* Start the transfer */
	USBD_PrepareReceive(pdev, 0U, NULL, 0U);

	return HAL_OK;
}

/**
  * @brief  Copy buffer into setup structure
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
static void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, u8 *pdata)
{
	req->bmRequest     = *(u8 *)(pdata);
	req->bRequest      = *(u8 *)(pdata +  1);
	req->wValue        = ReadEF2Byte(pdata +  2);
	req->wIndex        = ReadEF2Byte(pdata +  4);
	req->wLength       = ReadEF2Byte(pdata +  6);

}

#if 0
/**
  * @brief  Flushes an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
static HAL_Status USBD_FlushEP(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	USB_PCD_EP_Flush(pdev->pData, ep_addr);
	return HAL_OK;
}
#endif

/**
  * @brief  Sets a Stall condition on an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
static HAL_Status USBD_StallEP(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	//RTK_LOGS(NOTAG, RTK_LOG_DEBUG,"Stall EP%d\n", ep_addr);
	USB_PCD_EP_SetStall(pdev->pData, ep_addr);
	return HAL_OK;
}

/**
  * @brief  Clears a Stall condition on an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
static HAL_Status USBD_ClearStallEP(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	USB_PCD_EP_ClrStall(pdev->pData, ep_addr);
	return HAL_OK;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: yes, 0: No)
  */
static u8 USBD_IsStallEP(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	USB_PCD_HandleTypeDef *hpcd = pdev->pData;

	if ((ep_addr & 0x80) == 0x80) {
		return hpcd->IN_ep[ep_addr & 0xF].is_stall;
	} else {
		return hpcd->OUT_ep[ep_addr & 0xF].is_stall;
	}
}

/**
* @brief  Handle standard usb endpoint requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static HAL_Status USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req)
{
	u8 ep_addr;
	HAL_Status ret = HAL_OK;
	USBD_EndpointTypeDef   *pep;
	ep_addr  = USB_LOW_BYTE(req->wIndex);

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {

	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
		pdev->pClass->Setup(pdev, req);
		break;

	case USB_REQ_TYPE_STANDARD:
		/* Check if it is a class request */
		if ((req->bmRequest & 0x60U) == 0x20U) {
			ret = (HAL_Status)pdev->pClass->Setup(pdev, req);

			return ret;
		}

		switch (req->bRequest) {

		case USB_REQ_SET_FEATURE :

			switch (pdev->dev_state) {
			case USBD_STATE_ADDRESSED:
				if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
					USBD_StallEP(pdev, ep_addr);
					USBD_StallEP(pdev, 0x80U);
				} else {
					USBD_CtlError(pdev, req);
				}
				break;

			case USBD_STATE_CONFIGURED:
				if (req->wValue == USB_FEATURE_EP_HALT) {
					if ((ep_addr != 0x00U) && (ep_addr != 0x80U) && (req->wLength == 0x00U)) {
						USBD_StallEP(pdev, ep_addr);
					}
				}
				USBD_CtlSendStatus(pdev);

				break;

			default:
				USBD_CtlError(pdev, req);
				break;
			}
			break;

		case USB_REQ_CLEAR_FEATURE :

			switch (pdev->dev_state) {
			case USBD_STATE_ADDRESSED:
				if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
					USBD_StallEP(pdev, ep_addr);
					USBD_StallEP(pdev, 0x80U);
				} else {
					USBD_CtlError(pdev, req);
				}
				break;

			case USBD_STATE_CONFIGURED:
				if (req->wValue == USB_FEATURE_EP_HALT) {
					if ((ep_addr & 0x7FU) != 0x00U) {
						USBD_ClearStallEP(pdev, ep_addr);
					}
					USBD_CtlSendStatus(pdev);
				}
				break;

			default:
				USBD_CtlError(pdev, req);
				break;
			}
			break;

		case USB_REQ_GET_STATUS:
			switch (pdev->dev_state) {
			case USBD_STATE_ADDRESSED:
				if ((ep_addr != 0x00U) && (ep_addr != 0x80U)) {
					USBD_CtlError(pdev, req);
					break;
				}
				pep = ((ep_addr & 0x80U) == 0x80U) ? &pdev->ep_in[ep_addr & 0x7FU] : \
					  &pdev->ep_out[ep_addr & 0x7FU];

				pep->status = 0x0000U;

				USBD_CtlSendData(pdev, (u8 *)(void *)&pep->status, 2U);
				break;

			case USBD_STATE_CONFIGURED:
				if ((ep_addr & 0x80U) == 0x80U) {
					if (pdev->ep_in[ep_addr & 0xFU].is_used == 0U) {
						USBD_CtlError(pdev, req);
						break;
					}
				} else {
					if (pdev->ep_out[ep_addr & 0xFU].is_used == 0U) {
						USBD_CtlError(pdev, req);
						break;
					}
				}

				pep = ((ep_addr & 0x80U) == 0x80U) ? &pdev->ep_in[ep_addr & 0x7FU] : \
					  &pdev->ep_out[ep_addr & 0x7FU];

				if ((ep_addr == 0x00U) || (ep_addr == 0x80U)) {
					pep->status = 0x0000U;
				} else if (USBD_IsStallEP(pdev, ep_addr)) {
					pep->status = 0x0001U;
				} else {
					pep->status = 0x0000U;
				}

				USBD_CtlSendData(pdev, (u8 *)(void *)&pep->status, 2U);
				break;

			default:
				USBD_CtlError(pdev, req);
				break;
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		break;
	}

	return ret;
}

/**
* @brief  Handle standard usb interface requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static HAL_Status  USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req)
{
	HAL_Status ret = HAL_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
	case USB_REQ_TYPE_STANDARD:
		switch (pdev->dev_state) {
		case USBD_STATE_DEFAULT:
		case USBD_STATE_ADDRESSED:
		case USBD_STATE_CONFIGURED:

			if (USB_LOW_BYTE(req->wIndex) <= USBD_MAX_NUM_INTERFACES) {
				ret = (HAL_Status)pdev->pClass->Setup(pdev, req);

				if ((req->wLength == 0U) && (ret == HAL_OK)) {
					USBD_CtlSendStatus(pdev);
				}
			} else {
				USBD_CtlError(pdev, req);
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		break;
	}

	return HAL_OK;
}


/**
  * @brief  Handle standard usb device requests
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval status
  */
static HAL_Status USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req)
{
	HAL_Status ret = HAL_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS:
	case USB_REQ_TYPE_VENDOR:
		pdev->pClass->Setup(pdev, req);
		break;

	case USB_REQ_TYPE_STANDARD:

		switch (req->bRequest) {
		case USB_REQ_GET_DESCRIPTOR:
			USBD_GetDescriptor(pdev, req);
			break;

		case USB_REQ_SET_ADDRESS:
			USBD_SetAddress(pdev, req);
			break;

		case USB_REQ_SET_CONFIGURATION:
			USBD_SetConfig(pdev, req);
			break;

		case USB_REQ_GET_CONFIGURATION:
			USBD_GetConfig(pdev, req);
			break;

		case USB_REQ_GET_STATUS:
			USBD_GetStatus(pdev, req);
			break;


		case USB_REQ_SET_FEATURE:
			USBD_SetFeature(pdev, req);
			break;

		case USB_REQ_CLEAR_FEATURE:
			USBD_ClrFeature(pdev, req);
			break;

		default:
			//RTK_LOGS(NOTAG, RTK_LOG_DEBUG,"USBD_StdDevReq unexpected request: 0x%02X\n", req->bRequest);
			USBD_CtlError(pdev, req);
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		break;
	}

	return ret;
}

/**
  * @brief  Launch test mode process
  * @param  pdev: device instance
  * @retval status
  */
static HAL_Status USBD_RunTestMode(USBD_HandleTypeDef  *pdev)
{
	/* Prevent unused argument compilation warning */
	UNUSED(pdev);

	return HAL_OK;
}

/**
  * @brief  Configure device and start the interface
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status
  */
static HAL_Status USBD_SetClassConfig(USBD_HandleTypeDef  *pdev, u8 cfgidx)
{
	HAL_Status   ret = HAL_ERR_HW;

	if (pdev->pClass != NULL) {
		/* Set configuration  and Start the Class*/
		if (pdev->pClass->Init(pdev, cfgidx) == 0U) {
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Clear current configuration
  * @param  pdev: device instance
  * @param  cfgidx: configuration index
  * @retval status: HAL_Status
  */
static HAL_Status USBD_ClrClassConfig(USBD_HandleTypeDef  *pdev, u8 cfgidx)
{
	/* Clear configuration  and De-initialize the Class process*/
	pdev->pClass->DeInit(pdev, cfgidx);
	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initializes the device stack and load the class driver
  * @param  pdev: device instance
  * @param  pdesc: Descriptor structure address
  * @param  id: Low level core index
  * @retval None
  */
HAL_Status USBD_Core_Init(USBD_ClassTypeDef *driver, void *fops, u8 speed, void *data)
{
	USBD_HandleTypeDef *pdev = &USBD_Device;
	u8 ret;

	/* Set Device initial State */
	pdev->dev_state  = USBD_STATE_DEFAULT;

	/* Set LL Driver parameters */
	USBD_PCDHandle.Init.dev_endpoints = USB_MAX_ENDPOINTS;

	/* Be aware that enabling USB-DMA mode will result in data being sent only by
	 * multiple of 4 packet sizes. This is due to the fact that USB DMA does not
	 * allow sending data from non word-aligned addresses. For this specific
	 * application, it is advised to not enable this option unless required. */
	USBD_PCDHandle.Init.dma_enable = USBD_DMA_ENABLE;
	USBD_PCDHandle.Init.speed = speed;
	USBD_PCDHandle.Init.is_ptx_intr = 1U;

	/* Link The driver to the stack */
	USBD_PCDHandle.pData = pdev;
	pdev->pData = &USBD_PCDHandle;

	pdev->pClassData = data;

	/* Initialize LL Driver */
	ret = USB_PCD_Init(&USBD_PCDHandle);
	if (ret == HAL_OK) {
		// Use default FIFO configurations
		//USB_PCD_SetRxFiFo(&USBD_PCDHandle, USBD_RX_FIFO_SIZE);
		//USB_PCD_SetTxFiFo(&USBD_PCDHandle, 0, USBD_NPTX_FIFO_SIZE);
		//USB_PCD_SetTxFiFo(&USBD_PCDHandle, 1, USBD_PTX_FIFO_SIZE);

		pdev->pClass = driver;

		if (fops != NULL) {
			pdev->pUserData = fops;
		}

		USB_PCD_Start(&USBD_PCDHandle);
		ret = HAL_OK;
	} else {
		ret = HAL_ERR_HW;
	}

	return ret;
}

/**
  * @brief  De-Initialize th device library
  * @param  pdev: device instance
  * @retval status: status
  */
HAL_Status USBD_Core_DeInit(void)
{
	u8 result = HAL_OK;
	USBD_HandleTypeDef *pdev = &USBD_Device;

	/* Set Default State */
	pdev->dev_state  = USBD_STATE_DEFAULT;

	if (pdev->pClass != NULL) {
		/* Free Class Resources */
		pdev->pClass->DeInit(pdev, (u8)pdev->dev_config);
		pdev->pClass = NULL;
	}

	if (pdev->pData != NULL) {
		/* Initialize low level driver */
		result = USB_PCD_DeInit(pdev->pData);
		if (result == HAL_OK) {
			pdev->pData = NULL;
		} else {
			result = HAL_ERR_HW;
		}
	}

	return result;
}

/**
  * @brief  USB device IRQ handler
  * @param  None
  * @retval void
  */
void USBD_IRQHandler(void)
{
	usb_hal_driver.disable_interrupt();
	USB_PCD_IRQHandler(&USBD_PCDHandle);
	usb_hal_driver.enable_interrupt(USBD_IRQ_PRI);
}

/**
  * @brief  Opens an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  ep_type: Endpoint Type
  * @param  ep_mps: Endpoint Max Packet Size
  * @retval USBD Status
  */
HAL_Status USBD_OpenEP(USBD_HandleTypeDef *pdev, u8 ep_addr, u8 ep_type, u16 ep_mps)
{
	USB_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
	return HAL_OK;
}

/**
  * @brief  Closes an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval USBD Status
  */
HAL_Status USBD_CloseEP(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	USB_PCD_EP_Close(pdev->pData, ep_addr);
	return HAL_OK;
}

/**
  * @brief  Transmits data over an endpoint
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD Status
  */
HAL_Status USBD_Transmit(USBD_HandleTypeDef *pdev, u8 ep_addr, u8 *pbuf, u16 size)
{
	USB_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
	return HAL_OK;
}

/**
  * @brief  Prepares an endpoint for reception
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @param  pbuf:pointer to data to be received
  * @param  size: data size
  * @retval USBD Status
  */
HAL_Status USBD_PrepareReceive(USBD_HandleTypeDef *pdev, u8 ep_addr, u8 *pbuf, u16 size)
{
	USB_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
	return HAL_OK;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint Number
  * @retval Recived Data Size
  */
u32 USBD_GetRxDataSize(USBD_HandleTypeDef *pdev, u8 ep_addr)
{
	return USB_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
  * @brief  USBD_SetupStage
  *         Handle the setup stage
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_SetupStage(USBD_HandleTypeDef *pdev, u8 *psetup)
{
	USBD_ParseSetupRequest(&pdev->request, psetup);

	pdev->ep0_state = USBD_EP0_SETUP;

	pdev->ep0_data_len = pdev->request.wLength;

	switch (pdev->request.bmRequest & 0x1FU) {
	case USB_REQ_RECIPIENT_DEVICE:
		USBD_StdDevReq(pdev, &pdev->request);
		break;

	case USB_REQ_RECIPIENT_INTERFACE:
		USBD_StdItfReq(pdev, &pdev->request);
		break;

	case USB_REQ_RECIPIENT_ENDPOINT:
		USBD_StdEPReq(pdev, &pdev->request);
		break;

	default:
		//RTK_LOGS(NOTAG, RTK_LOG_DEBUG,"EP0 unexpected request: 0x%08X\n", pdev->request.bmRequest);
		USBD_StallEP(pdev, (pdev->request.bmRequest & 0x80U));
		break;
	}

	return HAL_OK;
}

/**
  * @brief  USBD_DataOutStage
  *         Handle data OUT stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
HAL_Status USBD_DataOutStage(USBD_HandleTypeDef *pdev, u8 epnum, u8 *pdata)
{
	USBD_EndpointTypeDef *pep;
	u32 len;

	if (epnum == 0U) {
		pep = &pdev->ep_out[0];

		if (pdev->ep0_state == USBD_EP0_DATA_OUT) {
			if (pep->rem_length > pep->maxpacket) {
				pep->rem_length -=  pep->maxpacket;

				USBD_CtlContinueRx(pdev,
								   pdata,
								   (u16)MIN(pep->rem_length, pep->maxpacket));
			} else {
				if ((pdev->pClass->EP0_RxReady != NULL) &&
					(pdev->dev_state == USBD_STATE_CONFIGURED)) {
					pdev->pClass->EP0_RxReady(pdev);
				}
				USBD_CtlSendStatus(pdev);
			}
		} else {
			if (pdev->ep0_state == USBD_EP0_STATUS_OUT) {
				/*
				 * STATUS PHASE completed, update ep0_state to idle
				 */
				pdev->ep0_state = USBD_EP0_IDLE;
				//USBD_StallEP(pdev, 0U);  // FIXME
			}
		}
	} else if ((pdev->pClass->DataOut != NULL) &&
			   (pdev->dev_state == USBD_STATE_CONFIGURED)) {
		len = USBD_GetRxDataSize(pdev, epnum);
		pdev->pClass->DataOut(pdev, epnum, len);
	} else {
		/* should never be in this condition */
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
  * @brief  USBD_DataInStage
  *         Handle data in stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
HAL_Status USBD_DataInStage(USBD_HandleTypeDef *pdev, u8 epnum, u8 *pdata)
{
	USBD_EndpointTypeDef *pep;

	if (epnum == 0U) {
		pep = &pdev->ep_in[0];

		if (pdev->ep0_state == USBD_EP0_DATA_IN) {
			if (pep->rem_length > pep->maxpacket) {
				pep->rem_length -= pep->maxpacket;

				USBD_CtlContinueSendData(pdev, pdata, (u16)pep->rem_length);

				/* Prepare endpoint for premature end of transfer */
				USBD_PrepareReceive(pdev, 0U, NULL, 0U);
			} else {
				/* last packet is MPS multiple, so send ZLP packet */
				if ((pep->total_length % pep->maxpacket == 0U) &&
					(pep->total_length >= pep->maxpacket) &&
					(pep->total_length < pdev->ep0_data_len)) {
					USBD_CtlContinueSendData(pdev, NULL, 0U);
					pdev->ep0_data_len = 0U;

					/* Prepare endpoint for premature end of transfer */
					USBD_PrepareReceive(pdev, 0U, NULL, 0U);
				} else {
					if ((pdev->pClass->EP0_TxSent != NULL) &&
						(pdev->dev_state == USBD_STATE_CONFIGURED)) {
						pdev->pClass->EP0_TxSent(pdev);
					}
					//USBD_StallEP(pdev, 0x80U);  // FIXME
					USBD_CtlReceiveStatus(pdev);
				}
			}
		} else {
			if ((pdev->ep0_state == USBD_EP0_STATUS_IN) ||
				(pdev->ep0_state == USBD_EP0_IDLE)) {
				//USBD_StallEP(pdev, 0x80U);
			}
		}

		if (pdev->dev_test_mode == 1U) {
			USBD_RunTestMode(pdev);
			pdev->dev_test_mode = 0U;
		}
	} else if ((pdev->pClass->DataIn != NULL) &&
			   (pdev->dev_state == USBD_STATE_CONFIGURED)) {
		pdev->pClass->DataIn(pdev, epnum);
	} else {
		/* should never be in this condition */
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
  * @brief  USBD_Reset
  *         Handle Reset event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_Reset(USBD_HandleTypeDef *pdev)
{
	/* Open EP0 OUT */
	USBD_OpenEP(pdev, 0x00U, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
	pdev->ep_out[0x00U & 0xFU].is_used = 1U;

	pdev->ep_out[0].maxpacket = USB_MAX_EP0_SIZE;

	/* Open EP0 IN */
	USBD_OpenEP(pdev, 0x80U, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
	pdev->ep_in[0x80U & 0xFU].is_used = 1U;

	pdev->ep_in[0].maxpacket = USB_MAX_EP0_SIZE;
	/* Upon Reset call user call back */
	pdev->dev_state = USBD_STATE_DEFAULT;
	pdev->ep0_state = USBD_EP0_IDLE;
	pdev->dev_config = 0U;
	pdev->dev_remote_wakeup = 0U;

#if 0
	if (pdev->pClass) {
		pdev->pClass->DeInit(pdev, (u8)pdev->dev_config);
	}
#endif

	return HAL_OK;
}

/**
  * @brief  USBD_SetSpeed
  *         Set USB device speed
  * @param  pdev: device instance
  * @param  speed: device speed
  * @retval status
  */
HAL_Status USBD_SetSpeed(USBD_HandleTypeDef *pdev, u8 speed)
{
	pdev->dev_speed = speed;
	return HAL_OK;
}

/**
  * @brief  USBD_Suspend
  *         Handle Suspend event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_Suspend(USBD_HandleTypeDef *pdev)
{
	pdev->dev_old_state =  pdev->dev_state;
	pdev->dev_state  = USBD_STATE_SUSPENDED;
	return HAL_OK;
}

/**
  * @brief  USBD_Resume
  *         Handle Resume event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_Resume(USBD_HandleTypeDef *pdev)
{
	pdev->dev_state = pdev->dev_old_state;
	return HAL_OK;
}

/**
  * @brief  USBD_SOF
  *         Handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_SOF(USBD_HandleTypeDef *pdev)
{
	if (pdev->dev_state == USBD_STATE_CONFIGURED) {
		if (pdev->pClass->SOF != NULL) {
			pdev->pClass->SOF(pdev);
		}
	}
	return HAL_OK;
}

/**
  * @brief  USBD_DevConnected
  *         Handle device connection event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_DevConnected(USBD_HandleTypeDef *pdev)
{
	/* Prevent unused argument compilation warning */
	UNUSED(pdev);

	return HAL_OK;
}

/**
  * @brief  USBD_DevDisconnected
  *         Handle device disconnection event
  * @param  pdev: device instance
  * @retval status
  */
HAL_Status USBD_DevDisconnected(USBD_HandleTypeDef *pdev)
{
	/* Free Class Resources */
	pdev->dev_state = USBD_STATE_DEFAULT;
	pdev->pClass->DeInit(pdev, (u8)pdev->dev_config);

	return HAL_OK;
}

/**
  * @brief  USBD_CtlSendData
  *         send data on the ctl pipe
  * @param  pdev: device instance
  * @param  buff: pointer to data buffer
  * @param  len: length of data to be sent
  * @retval status
  */
HAL_Status USBD_CtlSendData(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len)
{
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_DATA_IN;
	pdev->ep_in[0].total_length = len;
	pdev->ep_in[0].rem_length   = len;

	/* Start the transfer */
	USBD_Transmit(pdev, 0x00U, pbuf, len);

	return HAL_OK;
}

/**
  * @brief  USBD_CtlPrepareRx
  *         receive data on the ctl pipe
  * @param  pdev: device instance
  * @param  buff: pointer to data buffer
  * @param  len: length of data to be received
  * @retval status
  */
HAL_Status USBD_CtlPrepareRx(USBD_HandleTypeDef *pdev, u8 *pbuf, u16 len)
{
	/* Set EP0 State */
	pdev->ep0_state = USBD_EP0_DATA_OUT;
	pdev->ep_out[0].total_length = len;
	pdev->ep_out[0].rem_length   = len;

	/* Start the transfer */
	USBD_PrepareReceive(pdev, 0U, pbuf, len);

	return HAL_OK;
}

/**
  * @brief  USBD_CtlError
  *         Handle USB low level Error
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
void USBD_CtlError(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	UNUSED(req);
	USBD_StallEP(pdev, 0x80U);
	USBD_StallEP(pdev, 0U);
}


/**
  * @brief  USBD_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
void USBD_GetString(u8 *desc, u8 *unicode, u16 *len)
{
	u8 idx = 0U;

	if (desc != NULL) {
		*len = (u16)USBD_GetStrLen(desc) * 2U + 2U;
		unicode[idx++] = *(u8 *)(void *)len;
		unicode[idx++] = USB_DESC_TYPE_STRING;

		while (*desc != '\0') {
			unicode[idx++] = *desc++;
			unicode[idx++] =  0U;
		}
	}
}

