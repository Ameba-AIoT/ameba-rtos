/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CONFIG_H
#define USBH_COMPOSITE_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

// #define  CONFIG_USBH_COMPOSITE_HID_UAC 1

/* Exported defines ----------------------------------------------------------*/
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)

#define USBH_COMPOSITE_HID_UAC_DEBUG              1

#else
#error "No composite class configured"
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct {
	u8 bLength;
	u8 bDescriptorType;
} usb_descriptor_header_t;

typedef enum {
	USBH_COMPOSITE_IDLE = 0U,
	USBH_COMPOSITE_DETACHED,
	USBH_COMPOSITE_ATTACH,
	USBH_COMPOSITE_SETUP,
	USBH_COMPOSITE_MAX,
} usbh_compsoite_state_t;

typedef struct {
	void (*status_changed)(u8 old_status, u8 status);
	int (* set_config)(void);
} usbh_composite_cb_t;

typedef struct {
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)
	usbh_class_driver_t *hid;
	usbh_class_driver_t *uac;
#endif

	usbh_composite_cb_t *cb;
	usb_host_t *host;
	__IO usbh_compsoite_state_t state;
} usbh_composite_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif // USBH_COMPOSITE_CONFIG_H

