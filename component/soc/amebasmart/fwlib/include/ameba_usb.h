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

#define USB_REG_BASE								USB_OTG_REG_BASE

/** @} */

/** @defgroup USB_ADDON_REG_VND_STS_OUT
 * @brief USB AddOn Vendor Status Out Register
 * @{
 **/

#define USB_ADDON_REG_VND_STS_OUT					(USB_OTG_REG_BASE + 0x3001CUL)

#define USB_ADDON_REG_VND_STS_OUT_DATA_POS			0U
#define USB_ADDON_REG_VND_STS_OUT_DATA_MASK			(0xFFU << USB_ADDON_REG_VND_STS_OUT_DATA_POS)
#define USB_ADDON_REG_VND_STS_OUT_DATA				(USB_ADDON_REG_VND_STS_OUT_DATA_MASK)

/** @} */

/** @} */

/** @defgroup USB_Exported_Constants USB Exported Constants
  * @{
  */

/** @defgroup USB_Hardware_Configurations
  * @{
  */
#define CONFIG_USB_OTG								0U
#define CONFIG_USB_PHY								1U

#define USB_MAX_ENDPOINTS							6U
#define USB_MAX_PIPES								8U
#define USB_IN_TOKEN_QUEUE_DEPTH					8U

/** @} */

/** @defgroup USB_ID
  * @{
  */
#define USB_VID										0x0BDA
#define USB_PID										0x8730
/** @} */

/** @defgroup USB_IRQ_Configurations
  * @{
  */
#define USB_IRQ										USB_OTG_IRQ
#define USB_IRQ_PRI									INT_PRI_MIDDLE
/** @} */

/** @} */

/* Exported types ------------------------------------------------------------*/

/** @defgroup USB_Exported_Types USB Exported Types
  * @{
  */

typedef struct {
	u8 page; /*!< Page number */
	u8 addr; /*!< Register address */
	u8 val;  /*!< Register value */
} usb_cal_data_t;

typedef IRQ_FUN usb_irq_fun_t;

/** @} */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @defgroup USB_Exported_Functions USB HAL Exported Functions
  * @{
  */

int usb_chip_init(void);
int usb_chip_deinit(void);
usb_cal_data_t *usb_chip_get_cal_data(u8 mode);

/** @} */

/** @} */

/** @} */

#endif
