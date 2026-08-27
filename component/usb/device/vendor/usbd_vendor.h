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

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_Vendor_Constants Device Vendor Constants
 * @{
 */
#define USBD_VENDOR_VID                  USB_VID					/**< Vendor ID */
#define USBD_VENDOR_PID                  USB_PID					/**< Product ID */
#define USBD_VENDOR_CLASS_CODE           0xFFU					/**< Device class code (0xFF = Vendor Specific) */
#define USBD_VENDOR_SUBCLASS_CODE        0x00U					/**< Device subclass code (0x00 = No specific subclass) */
#define USBD_VENDOR_PROTOCOL             0x00U					/**< Device protocol code (0x00 = No specific protocol) */
#define USBD_VENDOR_LANGID_STRING        0x0409U					/**< Language ID for string descriptors (0x0409 = English) */
#define USBD_VENDOR_MFG_STRING           "Realtek"				/**< Manufacturer string */
#define USBD_VENDOR_PROD_HS_STRING       "Realtek Vendor (HS)"	/**< Product string for High-Speed mode */
#define USBD_VENDOR_PROD_FS_STRING       "Realtek Vendor (FS)"	/**< Product string for Full-Speed mode */
#define USBD_VENDOR_SN_STRING            "1234567890"			/**< Serial number string */

#define USBD_VENDOR_HS_BULK_MPS          512U   /**< High speed BULK IN & OUT maximum packet size */
#define USBD_VENDOR_FS_BULK_MPS          64U    /**< Full speed BULK IN & OUT maximum packet size */
#define USBD_VENDOR_HS_INTR_MPS          1024U  /**< High speed INTR IN & OUT maximum packet size */
#define USBD_VENDOR_FS_INTR_MPS          64U    /**< Full speed INTR IN & OUT maximum packet size */
#define USBD_VENDOR_HS_ISOC_MPS          1024U  /**< High speed ISOC IN & OUT maximum packet size */
#define USBD_VENDOR_FS_ISOC_MPS          1023U  /**< Full speed ISOC IN & OUT maximum packet size */

#define USBD_VENDOR_HS_INTR_IN_INTERVAL  1U    /**< High speed INTR IN interval */
#define USBD_VENDOR_HS_INTR_OUT_INTERVAL 1U    /**< High speed INTR OUT interval */
#define USBD_VENDOR_FS_INTR_IN_INTERVAL  2U    /**< Full speed INTR IN interval */
#define USBD_VENDOR_FS_INTR_OUT_INTERVAL 2U    /**< Full speed INTR OUT interval */

#define USBD_VENDOR_HS_ISOC_IN_INTERVAL  2U    /**< High speed ISOC IN interval */
#define USBD_VENDOR_HS_ISOC_OUT_INTERVAL 2U    /**< High speed ISOC OUT interval */
#define USBD_VENDOR_FS_ISOC_IN_INTERVAL  2U    /**< Full speed ISOC IN interval */
#define USBD_VENDOR_FS_ISOC_OUT_INTERVAL 2U    /**< Full speed ISOC OUT interval */
/** @} End of Device_Vendor_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Vendor endpoint configuration structure.
 * @details Used to configure endpoint addresses and buffer sizes at runtime.
 */
typedef struct {
	u32 bulk_in_xfer_size;   /**< BULK IN transfer buffer size */
	u32 bulk_out_xfer_size;  /**< BULK OUT transfer buffer size */
	u32 isoc_in_xfer_size;   /**< ISOC IN transfer buffer size */
	u32 isoc_out_xfer_size;  /**< ISOC OUT transfer buffer size */
	u32 intr_in_xfer_size;   /**< INTR IN transfer buffer size */
	u32 intr_out_xfer_size;  /**< INTR OUT transfer buffer size */
	u8 bulk_in_addr;    /**< BULK IN endpoint address */
	u8 bulk_out_addr;   /**< BULK OUT endpoint address */
	u8 isoc_in_addr;    /**< ISOC IN endpoint address */
	u8 isoc_out_addr;   /**< ISOC OUT endpoint address */
	u8 intr_in_addr;    /**< INTR IN endpoint address */
	u8 intr_out_addr;   /**< INTR OUT endpoint address */
} usbd_vendor_ep_cfg_t;

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
	 * @param[in] req Pointer to the setup request packet.
	 * @param[out] buf Pointer to a buffer for data stage of control transfers.
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
	const usbd_vendor_ep_cfg_t *ep_cfg;	/**< Pointer to the EP configuration. */
	const usbd_vendor_cb_t *cb;		/**< Pointer to the user-defined callback structure */
	usb_dev_t *dev;				/**< Pointer to the USB device instance */
	usb_setup_req_t ctrl_req;	/**< Stores the current control request */
	usbd_ep_t ep_isoc_in;		/**< ISOC IN endpoint structure */
	usbd_ep_t ep_isoc_out;		/**< ISOC OUT endpoint structure */
	usbd_ep_t ep_bulk_in;		/**< BULK IN endpoint structure */
	usbd_ep_t ep_bulk_out;		/**< BULK OUT endpoint structure */
	usbd_ep_t ep_intr_in;		/**< INTR IN endpoint structure */
	usbd_ep_t ep_intr_out;		/**< INTR OUT endpoint structure */
	u8 alt_setting;				/**< Alternate setting number of the current interface */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
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
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to EP configuration.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_vendor_init(const usbd_vendor_cb_t *cb, const usbd_vendor_ep_cfg_t *ep_cfg);
#endif

/**
 * @brief De-initializes the Vendor class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_deinit(void);

/**
 * @brief Transmits data to the host over the CTRL endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_ctrl_data(u8 *buf, u32 len);

/**
 * @brief Transmits data to the host over the BULK IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_bulk_data(u8 *buf, u32 len);

/**
 * @brief Receives data to the host over the BULK OUT endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_receive_bulk_data(void);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_intr_data(u8 *buf, u32 len);

/**
 * @brief Receives data to the host over the INTR OUT endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_receive_intr_data(void);

/**
 * @brief Transmits data to the host over the ISOC IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_transmit_isoc_data(u8 *buf, u32 len);

/**
 * @brief Receives data to the host over the ISOC OUT endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_vendor_receive_isoc_data(void);
/** @} End of Device_Vendor_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif  /* USBD_VENDOR_H */
