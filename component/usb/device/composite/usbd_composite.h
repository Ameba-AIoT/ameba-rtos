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

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @addtogroup Device_Composite_Constants Device Composite Constants
 *  @{
 */

/** Maximum number of sub-functions in one composite device. */
#define USBD_COMP_MAX_FUNCS     4U

/** @} End of Device_Composite_Constants group */
/** @} End of USB_Device_Constants group */

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
	u8 func_count;                          /**< Number of registered sub-functions. */
	u8 if_counts[USBD_COMP_MAX_FUNCS]; /**< Interface count for each sub-function (parsed from desc). */
	u8 active_func;                         /**< Index of sub-function handling the current setup with data OUT, 0xFF if none. */
	u8 active_func_in;                      /**< Index of sub-function handling the current setup with data IN, 0xFF if none. */
	u8 registered;                          /**< 1 between @ref usbd_composite_init and @ref usbd_composite_deinit. */
} usbd_composite_dev_t;

/** @} End of Device_Composite_Types group */
/** @} End of USB_Device_Types group */

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
 *        Removing a single function from a running composite device is not supported: a
 *        class deinit shall only run after @ref usbd_composite_deinit, or before
 *        @ref usbd_composite_init succeeded. @ref usbd_composite_deinit stops the endpoints
 *        of every function, which a class deinit relies on before freeing its buffers.
 *        The call is refused while the composite device is registered.
 *        The class deinit then returns without releasing anything.
 * @return HAL_OK on success, HAL_BUSY while the composite device is registered.
 */
int usbd_composite_unregister_driver(const usbd_class_driver_t *driver);

/**
 * @brief Initialize composite USB device.
 * @param[in] cb  Optional user callback. Can be NULL.
 * @note  All class drivers MUST be registered via @ref usbd_composite_register_driver
 *        BEFORE calling this function.
 *        The configuration descriptor is assembled from all sub-functions directly in the
 *        EP0 control transfer buffer and cannot be truncated, so @ref usbd_config_t::ctrl_xfer_buf_len
 *        must cover its total length at every supported speed, otherwise enumeration fails.
 * @return HAL_OK on success, error code otherwise.
 */
int usbd_composite_init(const usbd_composite_cb_t *cb);

/**
 * @brief De-initialize composite USB device.
 * @note  Call it in task context before the deinit of each class, then @ref usbd_deinit:
 *        it blocks the ISR and clears the configuration of every registered function.
 * @return HAL_OK on success. HAL_ERR_PARA when called in interrupt context: nothing was done
 *         and the deinit of each class keeps refusing, refer to @ref usbd_unregister_class.
 */
int usbd_composite_deinit(void);

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_COMPOSITE_H */
