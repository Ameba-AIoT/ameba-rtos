/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_DIAG_H
#define USB_DIAG_H

/* Includes ------------------------------------------------------------------*/
#include "ameba_soc.h"

/* Exported defines ----------------------------------------------------------*/

#ifndef CONFIG_NON_OS
/** @brief Default ring buffer depth for USB diag entries, used when diag_depth is 0 in the config. */
#define USB_DIAG_DEFAULT_DEPTH            10U
/** @brief Default diag task polling interval in ms, used when diag_poll_ms is 0 in the config. */
#define USB_DIAG_DEFAULT_POLL_MS          200U
#endif /* CONFIG_NON_OS */

/* Exported types ------------------------------------------------------------*/
typedef enum {
	USB_LAYER_CORE  = 0,       /* core / HCD / PCD */
	USB_LAYER_CLASS = 1,       /* class driver */
	USB_LAYER_APP   = 2,       /* example / application */
} usb_diag_layer_t;

typedef enum {
	/* Default */
	USB_EVT_DEFAULT  = 0,

	/* Normal events (1–9) */
	USB_EVT_LINK     = 1,       /* link event (connect/disconnect) */

	/* Error events (10+) */
	USB_EVT_ERR_SETUP    = 10,  /* setup callback error */
	USB_EVT_ERR_GET_DESC = 11,  /* get descriptor callback error */
	USB_EVT_ERR_XFER     = 12,  /* transfer error */

	USB_EVT_MAX      = 63,      /* max value */
} usb_diag_event_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Record a USB diagnostic event into the diag ring buffer.
 *
 * @warning For internal use inside USB interrupt handlers ONLY. It exists to
 *          capture diagnostic events from ISR context without printing there.
 *          Do NOT call it from task/thread context or any other code path: the
 *          ring buffer write is a non-atomic read-modify-write on a shared
 *          index, with no locking (locking is intentionally avoided to keep
 *          ISR latency minimal). A caller racing with the USB ISR writer will
 *          corrupt the buffer state. Use it exclusively within the USB ISR.
 *
 * @param[in] layer  @ref usb_diag_layer_t
 * @param[in] event  @ref usb_diag_event_t
 * @param[in] id     endpoint addr / pipe number / parameter (bits[7:0])
 */
#ifndef CONFIG_NON_OS
void usb_diag(usb_diag_layer_t layer, usb_diag_event_t event, u8 id);
#define USB_DIAG(layer, event, id)  usb_diag(layer, event, id)
#else
#define USB_DIAG(layer, event, id)  do {} while (0)
#endif /* CONFIG_NON_OS */

#endif /* USB_DIAG_H */
