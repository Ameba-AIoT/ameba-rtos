/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_H
#define USBD_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_TP_TRACE_DEBUG             1U  /**<This define is used to trace transfer performance */

/* USB descriptor configurations */
#define USBD_MAX_NUM_INTERFACES			16U
#define USBD_MAX_NUM_CONFIGURATION		16U

/**
 * @brief USB device string descriptor index.
 * @{
 */
#define USBD_IDX_LANGID_STR				0x00U
#define USBD_IDX_MFC_STR				0x01U
#define USBD_IDX_PRODUCT_STR			0x02U
#define USBD_IDX_SERIAL_STR				0x03U
#define USBD_IDX_MS_OS_STR				0xEEU
/** @} */

/**
 * @brief USB device interrupt enable flag.
 * @{
 */
#define USBD_SOF_INTR                   (BIT0) /**< Start of (micro)Frame Interrupt (GINTSTS.Sof). */
#define USBD_EOPF_INTR                  (BIT1) /**< End of Periodic Frame Interrupt (GINTSTS.EOPF). */
#define USBD_EPMIS_INTR                 (BIT2) /**< Endpoint Mismatch Interrupt (GINTSTS.EPMis). */
/** @} */

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief USB device state.
 */
typedef enum {
	USBD_STATE_INIT       = 0U,                /**< Initial state, before enumeration. */
	USBD_STATE_DEFAULT    = 1U,                /**< Default state, after reset. */
	USBD_STATE_ADDRESSED  = 2U,                /**< Addressed state, after `SET_ADDRESS`. */
	USBD_STATE_CONFIGURED = 3U,                /**< Configured state, after `SET_CONFIGURATION`. */
	USBD_STATE_SUSPENDED  = 4U,                /**< Suspended state. */
} usbd_state_t;

/**
 * @brief  USB device attach status.
 */
typedef enum {
	USBD_ATTACH_STATUS_INIT     = 0U,          /**< Initial status. */
	USBD_ATTACH_STATUS_ATTACHED = 1U,          /**< Device is attached to the host. */
	USBD_ATTACH_STATUS_DETACHED = 2U,          /**< Device is detached from the host. */
} usbd_attach_status_t;

// Forward declarations to resolve circular dependencies.
struct _usbd_class_driver_t;

/**
 * @brief Defines the structure for an USB endpoint.
 */
typedef struct {
	u8 *xfer_buf;                             /**< Pointer to the transfer buffer. */
	u8 *rem_xfer_buf;                         /**< Pointer to the remaining part of the transfer buffer. */
	u32 xfer_len;                             /**< Total length of the data to transfer. */
	u32 rem_xfer_len;                         /**< Remaining length of data to transfer (used for EP0). */
	u32 xfer_buf_len;                         /**< Total length of the class transfer buffer. */
	u16 mps;                                  /**< Maximum Packet Size for this endpoint (0-64KB). */
	u8 addr;                                  /**< Endpoint address (includes direction). */
	u8 num;                                   /**< Endpoint number (0-15). */
	u8 type;                                  /**< Endpoint type (Control, Bulk, Isochronous, Interrupt). */
	u8 binterval;                             /**< Polling interval for the endpoint. */
	u8 skip_dcache_pre_clean;                 /**< Skip `DCache_Clean` in TRX API and it will be called in class. */
	u8 skip_dcache_post_invalidate;           /**< Skip `DCache_Invalidate` when RX complete and it will be called in class. */
	__IO u8 xfer_state;                       /**< Current state of the class transfer. */
	__IO u8 tx_zlp;                           /**< Flag to indicate if a Zero-Length Packet should be sent. */
	__IO u8 dis_zlp;                          /**< Flag to disable Zero-Length Packet for the current transfer. */
	__IO u8 is_busy;                          /**< Flag indicating if the endpoint is currently busy. */
} usbd_ep_t;

/**
 * @brief Defines the core driver configuration parameters for the USB device.
 */
typedef struct {
	/**
	 * @brief Threshold count of EPMis interrupts for non-periodic IN transfers.
	 * @details If the mismatch count exceeds this value, the GINTSTS.EPMis interrupt is handled.
	 *          This is only active if `USBD_EPMIS_INTR` is enabled (Shared FIFO mode only).
	 */
	u32 nptx_max_epmis_cnt;

	/**
	 * @brief Enables extra interrupts.
	 * @details Optional USB interrupt enable flags:
	 * - `USBD_SOF_INTR`: For SOF-based timing synchronization.
	 * - `USBD_EOPF_INTR`: For ISOC transfers parity setting in slave mode.
	 * - `USBD_EPMIS_INTR`: To handle endpoint mismatches in shared FIFO mode.
	 *    For restarting IN transfers, applicable to scenarios with multiple non-periodic IN endpoints.
	 */
	u32 ext_intr_enable;

	u16 rx_fifo_depth;                        /**< RxFIFO depth in dwords which must not exceed hardware limits(Dedicated FIFO mode only). */

	/**
	 * @brief Depth of TxFIFO n (for n=1 to USB_MAX_ENDPOINTS-1) in dwords which must not exceed hardware limits.
	 * @details ptx_fifo_depth[i] corresponds to TxFIFO i+1 (Dedicated FIFO mode only).
	 * @note
	 *    - For SoCs with shared USB FIFO, no FIFO depth configuration is required.
	 *    - For SoCs with dedicated USB FIFO, observe the following constraint:
	 *      rx_fifo_depth + all ptx_fifo_depth[n] <= Hardware total FIFO depth
	 */
	u16 ptx_fifo_depth[USB_MAX_ENDPOINTS - 1];

	/**
	 * @brief USB device speed mode. See @ref usb_speed_type_t.
	 * - `USB_SPEED_HIGH`: USB 2.0 High-Speed PHY (for HS-capable SoCs).
	 * - `USB_SPEED_HIGH_IN_FULL`: USB 2.0 PHY operating in Full-Speed mode (for HS-capable SoCs with low bandwidth applcations like UAC).
	 * - `USB_SPEED_FULL`: USB 1.1 Full-Speed transceiver (for FS-only SoCs).
	 */
	u8 speed;

	u8 isr_priority;                          /**< Priority of the USB interrupt. */
	u8 isr_in_critical;                       /**< Flag to process USB ISR within a critical section. */
	u8 intr_use_ptx_fifo;                     /**< Use Periodic TxFIFO for Interrupt IN transfers (Shared TxFIFO mode only). */
#if USBD_TP_TRACE_DEBUG
	u8 tp_trace;                              /**< Enable/disable tracing for transfer performance. */
#endif
} usbd_config_t;

/**
 * @brief Defines the main USB device structure.
 */
typedef struct {
#if USBD_TP_TRACE_DEBUG
	/* Device task time debug params, enable by `tp_trace` in @ref usbd_config_t */
	__IO u64 isr_process_time_max;           /**< [Debug] Maximum ISR handler processing time */
	__IO u64 isr_process_time;               /**< [Debug] Current ISR handler processing time */
	__IO u64 isr_enter_period_max;           /**< [Debug] Maximum interval between two consecutive ISR processing */
	__IO u64 isr_enter_period;               /**< [Debug] Current time between last two consecutive ISR processing */
	__IO u64 isr_enter_time;                 /**< [Debug] Timestamp of current ISR handler entry */
#endif
	usbd_ep_t ep0_in;                        /**< Control endpoint 0 IN. */
	usbd_ep_t ep0_out;                       /**< Control endpoint 0 OUT. */
	struct _usbd_class_driver_t *driver;     /**< Pointer to the active class driver. */
	void *pcd;                               /**< Pointer to the low-level PCD (Platform Controller Driver) handle. */
	u32 ep0_out_intr;                        /**< Previous interrupt status for EP0 OUT. */
	u16 ep0_data_len;                        /**< Data length for the current EP0 transfer. */
	u8 ep0_state;                            /**< Current state of the EP0 state machine. */
	u8 ep0_old_state;                        /**< Previous state of the EP0 state machine. */
	u8 dev_config;                           /**< Current device configuration index. */
	u8 dev_speed;                            /**< Current device speed. See @ref usb_speed_type_t. */
	u8 dev_state;                            /**< Current device state. See @ref usbd_state_t. */
	u8 dev_old_state;                        /**< Previous device state. */
	u8 dev_attach_status;                    /**< Current device attach status. See @ref usbd_attach_status_t. */
	u8 dev_old_attach_status;                /**< Previous device attach status. */
	u8 test_mode;                            /**< Flag indicating if the device is in a test mode. */
	u8 self_powered;                         /**< Power source status: 0 for bus-powered, 1 for self-powered. */
	u8 remote_wakeup_en;                     /**< Remote wakeup enable or not, 0-disabled, 1-enabled */
	u8 remote_wakeup;                        /**< Remote wakeup flag. */
	u8 is_ready;                             /**< Device ready or not, 0-disabled, 1-enabled */
	u8 is_connected;                         /**< Device connected or not,0-disabled, 1-enabled */
} usb_dev_t;

/**
 * @brief Defines the structure for the USB class driver callbacks.
 * @note
 *    All the device class driver callback functions are invoked in interrupt context.
 *    Time-consuming or blocking operations must not be executed within these callback functions.
 */
typedef struct _usbd_class_driver_t {
	/**
	 * @brief Callback to get a descriptor during enumeration when host requests USB descriptors.
	 * @note
	 *    The class driver must return the descriptor data buffer and length againt the `wValue` value of host setup request:
	 *     - `USB_DESC_TYPE_DEVICE`: Device Descriptor, mandatory.
	 *     - `USB_DESC_TYPE_CONFIGURATION`: Configuration Descriptor, mandatory.
	 *     - `USB_DESC_TYPE_DEVICE_QUALIFIER`: Device Qualifier Descriptor, required for dual-speed (FS/HS) devices                                                                                                     |
	 *     - `USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION`：Other Speed Configuration Descriptor, required for dual-speed (FS/HS) devices.
	 *     - `USB_DESC_TYPE_STRING`: String descriptors (language ID, manufacturer, product, serial), optional.
	 * @param[in] dev: USB device.
	 * @param[in] req: USB setup request.
	 * @param[out] buf: Buffer to store the requested descriptor.
	 * @return The actual length of the descriptor.
	 */
	u16(*get_descriptor)(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Callback to set the device configuration.
	 * @note
	 *    Called upon `SET_CONFIGURATION` request in ADDRESSED state. The device class driver must:
	 *     - Initialize endpoints via @ref usbd_ep_init.
	 *     - Prepare first OUT transfer via @ref usbd_ep_receive.
	 * @param[in] dev: USB device.
	 * @param[in] config: The configuration index to be set.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*set_config)(usb_dev_t *dev, u8 config);

	/**
	 * @brief Callback to clear the device configuration.
	 * @note
	 *    Called upon:
	 *      - Core driver receives `SET_CONFIGURATION` control request in CONFIGURED state and needs to switch configuration.
	 *      - Core driver receives `SET_CONFIGURATION` control request in non-ADDRESSED/CONFIGURED state.
	 *      - Core driver processes enumeration complete interrupt (GINTSTS.EnumDone).
	 *      - Core driver processes Session end interrupt (GOTGINT.SesEndDet).
	 *      - USB core driver API @ref usbd_deinit is called.
	 *    Typically, the device class driver shall call @ref usbd_ep_deinit to deinitialize the endpoints.
	 * @param[in] dev: USB device.
	 * @param[in] config: The configuration index to be cleared.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*clear_config)(usb_dev_t *dev, u8 config);

	/**
	 * @brief Callback to handle class-specific or vendor-specific setup requests at control transfer setup phase.
	 * @details
	 *     - Standard interface requests (`SET_INTERFACE/GET_INTERFACE/GET_STATUS`).
	 *     - Class-specific requests per relevant class specifications.
	 *     - Vendor-defined requests for custom devices.
	 * @param[in] dev: USB device.
	 * @param[in] req: USB setup request.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(usb_dev_t *dev, usb_setup_req_t *req);

	/**
	 * @brief Callback invoked at the Start-of-Frame (SOF).
	 * @details Called upon SOF interrupt (GINTSTS.Sof) for timing-sensitive operations (e.g. UAC clock synchronization).
	 * @param[in] dev: USB device.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*sof)(usb_dev_t *dev);

	/**
	 * @brief Callback invoked when an IN data transfer on EP0 is complete.
	 * @param[in] dev: USB device.
	 * @param[in] status: Status of the completed transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*ep0_data_in)(usb_dev_t *dev, u8 status);

	/**
	 * @brief Callback invoked when an OUT data transfer on EP0 is complete.
	 * @note
	 *    The device driver does not need to re-enable the control OUT transfer since it is automatically done in core driver.
	 * @param[in] dev: USB device.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*ep0_data_out)(usb_dev_t *dev);

	/**
	 * @brief Callback invoked when a bulk/interrupt/isochronous IN data transfer on EP is complete.
	 * @note
	 *    The device class driver may mark endpoint ready for next transfer or retry failed transfers.
	 * @param[in] dev: USB device structure.
	 * @param[in] ep_addr: Endpoint address.
	 * @param[in] status: Status of the completed transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int(*ep_data_in)(usb_dev_t *dev, u8 ep_addr, u8 status);

	/**
	 * @brief Callback invoked when a bulk/interrupt/isochronous OUT data transfer on EP is complete.
	 * @note
	 *    Typically, the device class driver shall:
	 *      - Process received data via application callback.
	 *      - Prepare next OUT transfer via @ref usbd_ep_receive.
	 * @param[in] dev: USB device.
	 * @param[in] ep_addr: Endpoint address.
	 * @param[in] status: Status of the completed transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int(*ep_data_out)(usb_dev_t *dev, u8 ep_addr, u16 len);

	/**
	 * @brief Callback invoked when USB status change. See @ref usbd_attach_status_t.
	 * @details Called upon connection state changed for hot-plug support (e.g. do reinitialization on host disconnection)
	 * @param[in] dev: USB device.
	 * @param[in] old_status: Previous status of USB device.
	 * @param[in] status: Current status of USB device.
	 * @return None
	 */
	void (*status_changed)(usb_dev_t *dev, u8 old_status, u8 status);
} usbd_class_driver_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* API for application */
/**
 * @brief Initialize USB device core driver with configuration.
 * @param[in] cfg: USB device configuration.
 * @return 0 on success, non-zero on failure.
 */
int usbd_init(usbd_config_t *cfg);

/**
 * @brief Deinitialize USB device core driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_deinit(void);

/**
 * @brief Get USB device attach status.
 * @return See @ref usbd_attach_status_t.
 */
int usbd_get_status(void);

/**
 * @brief Get USB device bus status.
 * @param[out] status: Return 0 on success, with the status parameter containing USB bus status
 *        represented as a bitwise combination of enumeration values:
 *          - `USB_DEV_BUS_STATUS_DN` = BIT0,       // D- line status bit.
 *          - `USB_DEV_BUS_STATUS_DP` = BIT1,       // D+ line status bit.
 *          - `USB_DEV_BUS_STATUS_SUSPEND` = BIT2,  //Suspend indication bit.
 * @return 0 on success, non-zero on failure.
 */
int usbd_get_bus_status(u32 *status);

/**
 * @brief Sends a remote wakeup signal to the USB host to resume communication.
 * @return 0 on success, non-zero on failure.
 */
int usbd_wake_host(void);

/* API for class */
/**
 * @brief Register a device class driver, called in class initialization function.
 * @param[in] driver: USB class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_register_class(const usbd_class_driver_t *driver);

/**
 * @brief Un-register a class, called in class de-initialization function.
 * @return 0 on success, non-zero on failure.
 */
int usbd_unregister_class(void);

/**
 * @brief Initialize an endpoint.
 * @note
 *    Typically called in following scenarios:
 *     - In the set_config callback function of the @ref usbd_class_driver_t.
 *     - In the setup callback function of the @ref usbd_class_driver_t, when receiving specific
 *      requests (such as `SET_INTERFACE`) that require endpoint (re-)initialization.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_init(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Deinitialize an endpoint.
 * @note
 *    Typically called in following scenarios:
 *     - In the clear_config callback function of the @ref usbd_class_driver_t.
 *     - In the setup callback function of the @ref usbd_class_driver_t, when receiving specific
 *       requests (such as `SET_INTERFACE`) that require endpoint deinitialization.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_deinit(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Initiates an IN transfer to the USB host through a specified endpoint.
 * @note
 *     - The transfer executes asynchronously: function return doesn't indicate completion.
 *     - Check transfer status via ep_data_in/ep0_data_in callback of the @ref usbd_class_driver_t.
 *     - Zero-Length Packet will be automatically transmitted as needed by device core driver.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_transmit(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief  Prepares to receive OUT transfer data from the USB host through a specified endpoint.
 * @note
 *     - The transfer executes asynchronously: function return doesn't indicate completion.
 *     - Retrieved data is available via the ep_data_out/ep0_data_out callback of the @ref usbd_class_driver_t.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_receive(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Sets the specified endpoint to STALL state.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_set_stall(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Clears the STALL state of the specified endpoint.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_ep_clear_stall(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Checks whether the specified endpoint is in STALL state.
 * @param[in] dev: USB device.
 * @param[in] ep: USB endpoint.
 * @return
 *        - 1: The endpoint is stalled.
 *        - 0: The endpoint is not stalled.
 */
int usbd_ep_is_stall(usb_dev_t *dev, usbd_ep_t *ep);

/**
 * @brief Converts ASCII strings to UNICODE16-encoded USB string descriptors.
 * @param[in] str: Pointer to the null-terminated source ASCII string.
 * @param[out] desc: Formatted unicode string descriptor buffer where the USB descriptor will be written.
 * @note  The destination buffer must accommodate twice the source length plus 2 bytes
 *        (for the length, and type fields of string descriptor)
 * @return The total length of the generated descriptor in bytes.
 */
u16 usbd_get_str_desc(const char *str, u8 *desc);

#endif /* USBD_H */

