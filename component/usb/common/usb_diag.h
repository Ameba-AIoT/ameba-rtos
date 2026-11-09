/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_DIAG_H
#define USB_DIAG_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */
/** @defgroup USB_Common_Diag_Constants USB Diag Constants
 *  @brief Default sizing of the USB diagnostics ring buffer and dumper task.
 *  @{
 */

#ifndef CONFIG_NON_OS
/** @brief Default ring buffer depth for USB diag entries, used when diag_depth is 0 in the config. */
#define USB_DIAG_DEFAULT_DEPTH            10U
/** @brief Default diag task polling interval in ms, used when diag_poll_ms is 0 in the config. */
#define USB_DIAG_DEFAULT_POLL_MS          200U
#endif /* CONFIG_NON_OS */

/** @} End of USB_Common_Diag_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */
/** @defgroup USB_Common_Diag_Types USB Diag Types
 *  @brief Layer and event identifiers recorded in a USB diagnostics entry.
 *  @{
 */

/** @brief Stack layer that reported a diagnostics event. */
typedef enum {
	USB_LAYER_CORE  = 0,       /**< Core, HCD or PCD. */
	USB_LAYER_CLASS = 1,       /**< Class driver. */
	USB_LAYER_APP   = 2,       /**< Example or application. */
} usb_diag_layer_t;

/** @brief Event recorded in a USB diagnostics entry. */
typedef enum {
	/* Default */
	USB_EVT_DEFAULT  = 0,       /**< Unclassified event. */

	/* Normal events (1–9) */
	USB_EVT_LINK     = 1,       /**< Link event (connect or disconnect). */

	/* Error events (10+) */
	USB_EVT_ERR_SETUP    = 10,  /**< Setup callback error. */
	USB_EVT_ERR_GET_DESC = 11,  /**< Get descriptor callback error. */
	USB_EVT_ERR_XFER     = 12,  /**< Transfer error. */

	USB_EVT_MAX      = 63,      /**< Maximum event value that fits the entry bit-field. */
} usb_diag_event_t;

/** @} End of USB_Common_Diag_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Common_Functions USB Common Functions
 *  @{
 */
/** @defgroup USB_Common_Diag_Functions USB Diag Functions
 *  @brief ISR-side recording entry point of the USB diagnostics facility.
 *  @{
 */

/**
 * @brief Record a USB diagnostic event into the diag ring buffer.
 *
 * @warning USB ISR context ONLY: the core interrupt handlers and the class driver
 *          callbacks they call. It exists to capture diagnostic events from ISR
 *          context without printing there. The ring buffer has a single writer
 *          and no lock, to keep ISR latency minimal: a call from task context or
 *          from another interrupt races the USB ISR and corrupts entries. Use
 *          RTK_LOGS in task context instead.
 *          The diag task is created before the USB IRQ is registered and deleted
 *          after it is unregistered, so the context stays valid while the ISR runs.
 *
 * @param[in] layer  @ref usb_diag_layer_t
 * @param[in] event  @ref usb_diag_event_t
 * @param[in] id     endpoint addr / pipe number / parameter (bits[7:0])
 */
#ifndef CONFIG_NON_OS
void usb_diag(usb_diag_layer_t layer, usb_diag_event_t event, u8 id);
/** @brief Record a USB diagnostics event, compiled out when the OS is absent. */
#define USB_DIAG(layer, event, id)  usb_diag(layer, event, id)
#else
/** @brief No-op diagnostics record for non-OS builds. */
#define USB_DIAG(layer, event, id)  do {} while (0)
#endif /* CONFIG_NON_OS */

/** @} End of USB_Common_Diag_Functions group */
/** @} End of USB_Common_Functions group */

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_DIAG_H */
