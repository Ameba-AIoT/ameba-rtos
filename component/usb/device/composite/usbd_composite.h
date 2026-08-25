/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_H
#define USBD_COMPOSITE_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @addtogroup Device_Composite_Constants Device Composite Constants
 *  @{
 */

/** Maximum number of sub-functions in one composite device. */
#define USBD_COMP_MAX_FUNCS     4U

#define USBD_COMP_VID              USB_VID
#define USBD_COMP_PID              USB_PID
#define USBD_COMP_LANGID           0x0409U

#define USBD_COMP_STRING_COUNT     3U
#define USBD_COMP_MFG_STRING       "Realtek"
#define USBD_COMP_PROD_STRING      "Realtek Composite Device"
#define USBD_COMP_SN_STRING        "1234567890"

/** @} End of Device_Composite_Constants group */
/** @} End of USB_Device_Constants group */
/** @} End of USB_Device_API group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 *  @{
 */
/** @addtogroup Device_Composite_Types Device Composite Types
 *  @{
 */

/**
 * @brief Composite user callback structure.
 */
typedef struct {
	void (*status_changed)(u8 old_status, u8 new_status); /**< USB attach status change callback. */
} usbd_composite_cb_t;

/**
 * @brief Composite device structure (internal).
 */
typedef struct {
	const usbd_class_driver_t *drivers[USBD_COMP_MAX_FUNCS]; /**< Registered class driver array. */
	const usbd_composite_cb_t *cb;          /**< User callback. */
	usb_dev_t *dev;                         /**< USB device instance. */
	u8 *desc_buf;                           /**< Dynamically allocated scratch buffer for descriptor assembly. */
	u32 desc_buf_size;                      /**< Size of desc_buf in bytes. */
	u8 func_count;                          /**< Number of registered sub-functions. */
	u8 if_counts[USBD_COMP_MAX_FUNCS]; /**< Interface count for each sub-function (parsed from desc). */
	u8 active_func;                         /**< Index of sub-function handling the current setup with data OUT, 0xFF if none. */
} usbd_composite_dev_t;

/** @} End of Device_Composite_Types group */
/** @} End of USB_Device_Types group */
/** @} End of USB_Device_API group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Register a class driver with the composite framework.
 * @param[in] driver  Pointer to the class driver.
 * @return HAL_OK on success, error code on failure.
 * @note  Must be called AFTER the class driver's composite_init (e.g.
 *        @ref usbd_composite_cdc_acm_init) and BEFORE @ref usbd_composite_init.
 */
int usbd_composite_register_driver(const usbd_class_driver_t *driver);

/**
 * @brief Unregister a class driver from the composite framework.
 * @param[in] driver  Pointer to the class driver.
 * @note  Called from within each class driver's deinit function when
 *        the driver was registered as part of a composite device.
 */
void usbd_composite_unregister_driver(const usbd_class_driver_t *driver);

/**
 * @brief Initialize composite USB device.
 * @param[in] cb  Optional user callback. Can be NULL.
 * @note  All class drivers MUST be registered via @ref usbd_composite_register_driver
 *        BEFORE calling this function.
 *        The internal descriptor scratch buffer is allocated automatically using
 *        the EP0 control transfer buffer size from the core (see @ref usbd_get_ctrl_xfer_buf_len).
 * @return HAL_OK on success, error code otherwise.
 */
int usbd_composite_init(const usbd_composite_cb_t *cb);

/**
 * @brief De-initialize composite USB device.
 */
void usbd_composite_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* USBD_COMPOSITE_H */
