/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "sdio_intf.h"
#include "hci_core.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int SDIO_Intf_Init(HCI_AdapterTypeDef *adapter);
static void SDIO_Intf_DeInit(HCI_AdapterTypeDef *adapter);
static int SDIO_Intf_Transmit(HCI_AdapterTypeDef *adapter, u8 *buf, u32 len);

/* Private variables ---------------------------------------------------------*/

//static const char *const TAG = "SDIO";

HCI_InterfaceTypeDef SDIO_Interface = {
	.Init = SDIO_Intf_Init,
	.DeInit = SDIO_Intf_DeInit,
	.Transmit = SDIO_Intf_Transmit,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  USB download interface initialize
  * @param  adapter: HCI adapter
  * @retval Status
  */
static int SDIO_Intf_Init(HCI_AdapterTypeDef *adapter)
{
	UNUSED(adapter);

	/* TODO */

	return HAL_OK;
}

/**
  * @brief  USB download interface de-initialize
  * @param  adapter: HCI adapter
  * @retval Status
  */
static void SDIO_Intf_DeInit(HCI_AdapterTypeDef *adapter)
{
	UNUSED(adapter);

	/* TODO */
}

/**
  * @brief  Transmit data to SDIO host
  * @param  adapter: HCI adapter
  * @param  buf: Data buffer
  * @param  size: Data size in byte
  * @retval Status
  */
static int SDIO_Intf_Transmit(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size)
{
	UNUSED(adapter);
	UNUSED(buf);
	UNUSED(size);

	/* TODO */

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/
