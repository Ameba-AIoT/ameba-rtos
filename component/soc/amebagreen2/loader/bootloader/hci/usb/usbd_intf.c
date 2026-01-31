/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "usbd_hal.h"
#include "usbd_intf.h"
#include "usbd_whc_class.h"
#include "usbd_core.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_SPEED					USB_SPEED_HIGH

#define USBD_CTRL_REQ_TYPE			0xF0U
#define USBD_CTRL_REQ_QUERY			0x01U
#define USBD_CTRL_ACK_MASK			0x80U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 USBD_Intf_Setup(HCI_AdapterTypeDef *adapter, USBD_SetupReqTypedef *req, u8 *buf);
static u8 USBD_Intf_Receive(HCI_AdapterTypeDef *adapter, u32 size);

static int USBD_Intf_Init(HCI_AdapterTypeDef *adapter);
static void USBD_Intf_DeInit(HCI_AdapterTypeDef *adapter);
static int USBD_Intf_Transmit(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USB";

static USBD_WHC_ClassCallbackTypeDef USBD_WHC_ClassCallback = {
	USBD_Intf_Setup,
	USBD_Intf_Receive
};

HCI_InterfaceTypeDef USBD_Interface = {
	.Init = USBD_Intf_Init,
	.DeInit = USBD_Intf_DeInit,
	.Transmit = USBD_Intf_Transmit,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Receive callback for INIC class
  * @param  adapter: HCI adapter
  * @param  size: RX data size in bytes
  * @retval Status
  */
static u8 USBD_Intf_Receive(HCI_AdapterTypeDef *adapter, u32 size)
{
	return HCI_RxProcess(adapter, adapter->RxBuffer, size);
}

/**
  * @brief  Setup callback for INIC class
  * @param  adapter: HCI adapter
  * @param  req: Control setup request
  * @param  buf: Buffer to hold the tx/rx data
  * @retval Status
  */
static u8 USBD_Intf_Setup(HCI_AdapterTypeDef *adapter, USBD_SetupReqTypedef *req, u8 *buf)
{
	u8 ret = HAL_ERR_PARA;
	u32 len = req->wLength;
	HCI_PacketDescTypeDef *txDesc;

	UNUSED(adapter);

	if (req->bRequest != USBD_CTRL_REQ_TYPE) {
		return HAL_ERR_PARA;
	}

	switch (req->wIndex) {
	case USBD_CTRL_REQ_QUERY:
		if ((len == HCI_PKT_DESC_SIZE) && (buf != NULL)) {
			_memset((void *)buf, 0, HCI_PKT_DESC_SIZE);
			txDesc = (HCI_PacketDescTypeDef *)buf;
			txDesc->DataLen = 0;
			txDesc->DataOffset = HCI_PKT_DESC_SIZE;
			txDesc->PacketType = USBD_CTRL_REQ_QUERY | USBD_CTRL_ACK_MASK;
			txDesc->XferStatus = HAL_OK;
			txDesc->Version = (u8)(EFUSE_GetChipVersion() & 0xFF);
			txDesc->DevMode = HCI_FW_TYPE_BOOTLOADER;
			ret = HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "QUERY: protocol err\n");
		}
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid request index 0x%02X\n", req->wIndex);
		break;
	}
	return ret;
}

/**
  * @brief  USB download interface initialize
  * @param  adapter: HCI adapter
  * @retval Status
  */
static int USBD_Intf_Init(HCI_AdapterTypeDef *adapter)
{
	int ret;

	USBD_WHC_ClassHandle.Adapter = adapter;

	ret = USBD_Core_Init(&USBD_WHC_ClassDriver, &USBD_WHC_ClassCallback, USBD_SPEED, (void *)&USBD_WHC_ClassHandle);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail: %d\n", ret);
	}

	return ret;
}

/**
  * @brief  USB download interface de-initialize
  * @param  adapter: HCI adapter
  * @retval void
  */
static void USBD_Intf_DeInit(HCI_AdapterTypeDef *adapter)
{
	UNUSED(adapter);

	USBD_Core_DeInit();

	USBD_WHC_ClassHandle.Adapter = NULL;
}

/**
  * @brief  Transmit data to USB host
  * @param  adapter: HCI adapter
  * @param  buf: Data buffer
  * @param  size: Data size in byte
  * @retval Status
  */
static int USBD_Intf_Transmit(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size)
{
	UNUSED(adapter);
	return USBD_Class_Transmit(buf, size);
}

/* Exported functions --------------------------------------------------------*/
