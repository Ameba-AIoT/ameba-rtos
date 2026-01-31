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

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup USB
* @brief USB driver modules
* @{
*/

/** @defgroup USB_Register_Definitions USB Register Definitions
 * @{
 *****************************************************************************/

//#define USB_REG_BASE
#define USB_ADDON_REG_BASE                                  (USB_REG_BASE + 0x30000UL)

/** @defgroup USB_ADDON_REG_AUTOLOAD_CTRL
 * @brief USB AddOn Autoload Control Register
 * @{
 **/

#define USB_ADDON_REG_AUTOLOAD_CTRL                         (USB_ADDON_REG_BASE + 0xA4UL)

#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM           BIT(0)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP       BIT(1)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN       BIT(2)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_UPHY_EN    BIT(3)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN    BIT(4)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_BT_INTR_INTERVAL    BIT(5)

/* USB OTG addon autoload registers */
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0E0                    (USB_ADDON_REG_BASE + 0xE0UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0E4                    (USB_ADDON_REG_BASE + 0xE4UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1E0                    (USB_ADDON_REG_BASE + 0xE8UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1E4                    (USB_ADDON_REG_BASE + 0xECUL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0F0                    (USB_ADDON_REG_BASE + 0xF0UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0F4                    (USB_ADDON_REG_BASE + 0xF4UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1F4                    (USB_ADDON_REG_BASE + 0xF8UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P2E4                    (USB_ADDON_REG_BASE + 0xFCUL)
/** @} */

/** @} */

/** @defgroup USB_Exported_Constants USB Exported Constants
  * @{
  */

/** @defgroup USB_Hardware_Configurations
  * @{
  */

#define USB_MAX_ENDPOINTS                                   8U
#define USB_MAX_PIPES                                       12U

#define USB_PIN_DM                                          _PA_4
#define USB_PIN_DP                                          _PA_5
/** @} */

/** @defgroup USB_ID
  * @{
  */
#define USB_VID                                             0x0BDA
#define USB_PID                                             0x8006
/** @} */

/** @} */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} */

/** @} */


#ifdef __cplusplus
}
#endif
#endif
