/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_CLASS_H
#define USBH_UVC_CLASS_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/** @addtogroup USB_Host_Variables USB Host Variables
 *  @{
 */
/** @defgroup Host_UVC_Class_Variables Host UVC Class Variables
 *  @brief Class driver instance registered with the host core by the UVC class.
 *  @{
 */

/** @brief The UVC host class driver, registered with the core by @ref usbh_uvc_class_init. */
extern const usbh_class_driver_t usbh_uvc_driver;

/** @} End of Host_UVC_Class_Variables group */
/** @} End of USB_Host_Variables group */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 *  @{
 */
/** @defgroup Host_UVC_Class_Functions Host UVC Class Functions
 *  @brief Registration and teardown of the UVC host class driver, internal to the UVC class.
 *  @{
 */

/**
 * @brief  Register the UVC class driver with the host core.
 * @return RTK_SUCCESS on success, a RTK error code otherwise.
 */
int usbh_uvc_class_init(void);

/**
 * @brief  Unregister the UVC class driver from the host core.
 * @return Result of usbh_unregister_class(): on anything but RTK_SUCCESS nothing was released.
 */
int usbh_uvc_class_deinit(void);

/** @} End of Host_UVC_Class_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_UVC_CLASS_H */
