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

/** @defgroup USB_REG_BASE
 * @brief USB Register Base Address
 * @{
 **/

#define USB_REG_BASE              USB_OTG_REG_BASE
#define USB_ADDON_REG_BASE        (USB_REG_BASE + 0x30000UL)

/** @} */

/** @} */

/** @defgroup USB_Exported_Constants USB Exported Constants
  * @{
  */

/** @defgroup USB_Hardware_Configurations
  * @{
  */

#define USB_MAX_ENDPOINTS         6U
#define USB_MAX_PIPES             8U

/** @} */

/** @defgroup USB_ID
  * @{
  */
#define USB_VID                   0x0BDA
#define USB_PID                   0x8730
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
