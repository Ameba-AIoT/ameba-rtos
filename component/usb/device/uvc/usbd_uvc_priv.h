/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UVC_PRIV_H
#define USBD_UVC_PRIV_H

/* Includes ------------------------------------------------------------------*/

#include "usbd_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_UVC_Priv_Functions Device UVC Private Functions
 *  @brief Functions shared between the UVC class driver and its event handlers.
 *  @{
 */

/**
	* @brief  Dispatch a UVC event (SETUP / DATA / STREAMON / STREAMOFF) to the event handlers.
	* @param[in]  dev        UVC device context.
	* @param[in]  uvc_event  Event container (type + setup request or data payload).
	*/
void usbd_uvc_events_process(usbd_uvc_dev_t *dev, usbd_uvc_req_data_t *uvc_event);

/**
	* @brief  Initialize UVC processing-unit and extension-unit control parameters.
	* @retval RTK_SUCCESS on success.
	*/
int usbd_uvc_parameter_init(void);

/** @} End of Device_UVC_Priv_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_UVC_PRIV_H */
