/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_VENDOR_H
#define USBD_VENDOR_H

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
 * @{
 */
/** @addtogroup Device_Vendor_Constants Device Vendor Constants
 * @{
 */

#define USBD_VENDOR_CLASS_CODE           0xFFU					/**< Device class code (0xFF = Vendor Specific) */
#define USBD_VENDOR_SUBCLASS_CODE        0x00U					/**< Device subclass code (0x00 = No specific subclass) */
#define USBD_VENDOR_PROTOCOL             0x00U					/**< Device protocol code (0x00 = No specific protocol) */

#define USBD_VENDOR_HS_BULK_MPS          512U   /**< High speed BULK IN & OUT maximum packet size */
#define USBD_VENDOR_FS_BULK_MPS          64U    /**< Full speed BULK IN & OUT maximum packet size */
#define USBD_VENDOR_HS_INTR_MPS          1024U  /**< High speed INTR IN & OUT maximum packet size */
#define USBD_VENDOR_FS_INTR_MPS          64U    /**< Full speed INTR IN & OUT maximum packet size */
#define USBD_VENDOR_HS_ISOC_MPS          1024U  /**< High speed ISOC IN & OUT maximum packet size */
#define USBD_VENDOR_FS_ISOC_MPS          1023U  /**< Full speed ISOC IN & OUT maximum packet size */

/* Default applied when the matching @ref usbd_vendor_config_t interval field is left 0. */
#define USBD_VENDOR_DEFAULT_EP_INTERVAL  2U    /**< Default INTR/ISOC bInterval, both speeds. */

/** @} End of Device_Vendor_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Configuration for the Vendor class.
 * @details Carries every Vendor option: the endpoint allocation, which the top-level application
 *          owns, plus the per-endpoint transfer sizes and intervals. Used by both standalone and
 *          composite modes. Device-global parameters (VID/PID, strings, power attributes) live in
 *          @ref usbd_dev_info_t instead, and application-level options such as hotplug or task
 *          priorities belong in the example's own configuration structure.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbd_vendor_deinit.
 */
typedef struct {
	u32 bulk_in_xfer_size;   /**< BULK IN transfer buffer size */
	u32 bulk_out_xfer_size;  /**< BULK OUT transfer buffer size */
	u32 isoc_in_xfer_size;   /**< ISOC IN transfer buffer size */
	u32 isoc_out_xfer_size;  /**< ISOC OUT transfer buffer size */
	u32 intr_in_xfer_size;   /**< INTR IN transfer buffer size */
	u32 intr_out_xfer_size;  /**< INTR OUT transfer buffer size */
	const char *itf_str;     /**< iInterface string of the Vendor interface, NULL for none. */
	u8 bulk_in_addr;    /**< BULK IN endpoint address */
	u8 bulk_out_addr;   /**< BULK OUT endpoint address */
	u8 isoc_in_addr;    /**< ISOC IN endpoint address */
	u8 isoc_out_addr;   /**< ISOC OUT endpoint address */
	u8 intr_in_addr;    /**< INTR IN endpoint address */
	u8 intr_out_addr;   /**< INTR OUT endpoint address */
	u8 intr_in_interval_hs;  /**< INTR IN bInterval at High Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 intr_in_interval_fs;  /**< INTR IN bInterval at Full Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 intr_out_interval_hs; /**< INTR OUT bInterval at High Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 intr_out_interval_fs; /**< INTR OUT bInterval at Full Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 isoc_in_interval_hs;  /**< ISOC IN bInterval at High Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 isoc_in_interval_fs;  /**< ISOC IN bInterval at Full Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 isoc_out_interval_hs; /**< ISOC OUT bInterval at High Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
	u8 isoc_out_interval_fs; /**< ISOC OUT bInterval at Full Speed, 0 uses @ref USBD_VENDOR_DEFAULT_EP_INTERVAL. */
} usbd_vendor_config_t;

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_Vendor_Types Device Vendor Types
 * @{
 */

/**
 * @brief Structure containing callback functions for the Vendor class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Called when the USB class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called when the USB device is de-initialized for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*deinit)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @note   The handler shall validate every field of the request and return non-zero for
	 *         any request it does not support, which makes the core STALL EP0.
	 * @param[in] req Pointer to the setup request packet.
	 * @param[out] buf Pointer to a buffer for data stage of control transfers. For a D2H
	 *         request it is the zero-initialized response window, min(wLength, EP0 buffer
	 *         size) bytes, and all of it is sent to the host when the handler returns 0.
	 *         For an H2D request it holds the received payload, and it is NULL if the
	 *         request has no data stage.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Called when the USB device is configured.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*set_config)(void);

	/**
	 * @brief Called when new data is received from the host on the BULK OUT endpoint.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*bulk_received)(u8 *buf, u32 len);

	/**
	 * @brief Called when new data is received from the host on the INTR OUT endpoint.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*intr_received)(u8 *buf, u32 len);

	/**
	 * @brief Called when new data is received from the host on the ISOC OUT endpoint.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*isoc_received)(u8 *buf, u32 len);

	/**
	 * @brief Called when a data transmission to the host on the BULK IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status: The status of the transmission.
	 */
	void (*bulk_transmitted)(u8 status);

	/**
	 * @brief Called when a data transmission to the host on the INTR IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status: The status of the transmission.
	 */
	void (*intr_transmitted)(u8 status);

	/**
	 * @brief Called when a data transmission to the host on the ISOC IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status: The status of the transmission.
	 */
	void (*isoc_transmitted)(u8 status);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_vendor_cb_t;

/**
 * @brief Structure representing the Vendor device instance.
 */
typedef struct {
	const usbd_vendor_config_t *cfg;	/**< Pointer to the class configuration (set by init). */
	const usbd_vendor_cb_t *cb;		/**< Pointer to the user-defined callback structure */
	usb_dev_t *dev;				/**< Pointer to the USB device instance */
	usbd_ep_t ep_isoc_in;		/**< ISOC IN endpoint structure */
	usbd_ep_t ep_isoc_out;		/**< ISOC OUT endpoint structure */
	usbd_ep_t ep_bulk_in;		/**< BULK IN endpoint structure */
	usbd_ep_t ep_bulk_out;		/**< BULK OUT endpoint structure */
	usbd_ep_t ep_intr_in;		/**< INTR IN endpoint structure */
	usbd_ep_t ep_intr_out;		/**< INTR OUT endpoint structure */
	usb_setup_req_t ctrl_req;	/**< Stores the current control request (byte-packed) */
	u8 alt_setting;				/**< Alternate setting number of the current interface */
	u8 ctrl_req_pending;		/**< 1 if ctrl_req is waiting for its EP0 OUT data stage */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
	u8 itf_str_idx;				/**< iInterface string index from usbd_add_string(), 0 for none. */
} usbd_vendor_dev_t;

/** @} End of Device_Vendor_Types group */
/** @} End of USB_Device_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_Vendor_Functions Device Vendor Functions
 * @{
 */

/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_vendor_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_vendor_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_config_t *cfg);
#endif

/**
 * @brief De-initializes the Vendor class driver.
 * @note  Call it in task context, once no other task is inside usbd_vendor_transmit_bulk_data(),
 *        usbd_vendor_transmit_intr_data() or usbd_vendor_transmit_isoc_data(): the TX buffers
 *        they write to are released here.
 * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
 *        still reachable from the USB ISR, so the call returns at once and releases nothing.
 * @return None. This is a teardown path: apart from the composite case above, the class is always
 *         unregistered and every resource it owns released, nothing for the caller to recover from.
 */
void usbd_vendor_deinit(void);

/**
 * @brief Transmits data to the host over the BULK IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_bulk_data(u8 *buf, u32 len);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_intr_data(u8 *buf, u32 len);

/**
 * @brief Transmits data to the host over the ISOC IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_isoc_data(u8 *buf, u32 len);

/** @} End of Device_Vendor_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_VENDOR_H */
