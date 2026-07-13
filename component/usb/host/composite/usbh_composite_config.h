/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CONFIG_H
#define USBH_COMPOSITE_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)
#define USBH_COMPOSITE_HID_UAC_DEBUG              0

#define USBH_COMPOSITE_HID_UAC_EVENT              BIT0
#define USBH_COMPOSITE_HID_EVENT                  BIT1
#define USBH_COMPOSITE_UAC_EVENT                  BIT2
#elif defined(CONFIG_USBH_COMPOSITE_ACM_ECM)

#define USBH_COMPOSITE_4G_DONGLE_SUPPORT          1

/* Quectel CAT1 in ECM mode */
#define USBH_COMPOSITE_QUECTEL_DONGLE_VID         (0x2C7C)
#define USBH_COMPOSITE_QUECTEL_DONGLE_EG915_PID   (0x0901)
#define USBH_COMPOSITE_QUECTEL_DONGLE_EG91_PID    (0x0191)

/* Fibocom LE271 CAT1 in ECM mode */
#define USBH_COMPOSITE_FIBOCOM_DONGLE_LE271_VID   (0x2CB7)
#define USBH_COMPOSITE_FIBOCOM_DONGLE_LE271_PID   (0x0D01)

#define USBH_COMPOSITE_DEVICE_VID                 (USB_VID)

#else
#error "No composite class configured"
#endif

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_Composite_Types Host Composite Types
 * @{
 */
typedef struct {
	/**
	 * @brief Callback invoked when a device is detached.
	 *        Used to report the disconnection status to the application.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);
} usbh_composite_cb_t;

typedef struct {
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)
	usbh_class_driver_t *hid;    /**< HID class. */
	usbh_class_driver_t *uac;    /**< UAC class. */
#elif defined(CONFIG_USBH_COMPOSITE_ACM_ECM)
	usbh_class_driver_t *acm;    /**< CDC ACM class. */
	usbh_class_driver_t *ecm;    /**< CDC ECM class. */
#endif
	const usbh_composite_cb_t *cb;      /**< Pointer to the user-defined callback structure. */
	usb_host_t *host;             /**< Pointer to the host structure. */
} usbh_composite_host_t;
/** @} End of Host_Composite_Types group */
/** @} End of USB_Host_Types group */
/** @} End of USB_Host_API group */
/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* USBH_COMPOSITE_CONFIG_H */
