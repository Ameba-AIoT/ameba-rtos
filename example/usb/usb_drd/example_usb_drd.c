/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "basic_types.h"
#include "ameba.h"
#include "os_wrapper.h"
#include "usb_app_mgr.h"
#include "example_usb_drd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "DRD";

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Example entry: bring up the USB app manager, the apps are run with AT+USBAPP
  * @retval None
  */
void example_usb_drd(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USB DRD demo start\n");

	if (usb_app_mgr_init() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB DRD demo aborted\n");
	}
}
