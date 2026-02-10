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
#define USBH_COMPOSITE_HID_UAC_DEBUG             0

#if USBH_COMPOSITE_HID_UAC_DEBUG && (USBH_TP_TRACE_DEBUG == 0)
#error "Please set USBH_TP_TRACE_DEBUG in usbh.h"
#endif

#define USBH_COMPOSITE_HID_UAC_EVENT              BIT0
#define USBH_COMPOSITE_HID_EVENT                  BIT1
#define USBH_COMPOSITE_UAC_EVENT                  BIT2
#elif defined(CONFIG_USBH_COMPOSITE_ACM_ECM)

#define USB_4G_DONGLE_SUPPORT                     1

#define USB_EF_DONGLE_VID        0x2C7C
#define EF_DONGLE_PID_EG915      0x0901
#define EF_DONGLE_PID_EG91       0x0191
#define USB_DEFAULT_VID          USB_VID

#else
#error "No composite class configured"
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct {
	void (*status_changed)(u8 old_status, u8 status);
	int (* set_config)(void);
} usbh_composite_cb_t;

typedef struct {
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)
	usbh_class_driver_t *hid;
	usbh_class_driver_t *uac;
#elif defined(CONFIG_USBH_COMPOSITE_ACM_ECM)
	usbh_class_driver_t *acm;
	usbh_class_driver_t *ecm;
#endif
	usbh_composite_cb_t *cb;
	usb_host_t *host;
} usbh_composite_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif // USBH_COMPOSITE_CONFIG_H
