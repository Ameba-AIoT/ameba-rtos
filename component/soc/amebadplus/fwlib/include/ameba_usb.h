/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_USB_H
#define _AMEBA_USB_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_vector.h"
#include "basic_types.h"
#include "hal_platform.h"
#include "section_config.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

//#define USB_REG_BASE

/** @defgroup USB
* @brief USB driver modules
* @{
*/

/** @defgroup USB_Exported_Constants USB Exported Constants
  * @{
  */

/** @defgroup USB_Hardware_Configurations
  * @{
  */
#define CONFIG_USB_OTG						0U
#define CONFIG_USB_PHY						0U

#define USB_MAX_ENDPOINTS					6U
#define USB_MAX_PIPES						0U
#define USB_IN_TOKEN_QUEUE_DEPTH			8U
/** @} */

/** @defgroup USB_ID
  * @{
  */
#define USB_VID								0x0BDA
#define USB_PID								0x8722
/** @} */

/** @defgroup USB_IRQ_Configurations
  * @{
  */
#define USB_IRQ								USB_INT_IRQ
#define USB_IRQ_PRI							INT_PRI_MIDDLE
/** @} */

/** @} */

/* Exported types ------------------------------------------------------------*/

/** @defgroup USB_Exported_Types USB Exported Types
  * @{
  */

typedef IRQ_FUN usb_irq_fun_t;

/** @} */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @defgroup USB_Exported_Functions USB HAL Exported Functions
  * @{
  */

int usb_chip_init(u8 mode);
int usb_chip_deinit(void);

/** @} */

/** @} */

/** @} */

#endif
