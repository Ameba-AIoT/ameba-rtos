/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_CDC_ACM_H
#define USBD_COMPOSITE_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/**
 * @brief Configuration macro to enable or disable the CDC ACM notification endpoint.
 * @details Set to 1 to enable the Interrupt IN endpoint for sending notifications
 *          like SERIAL_STATE. Set to 0 to disable.
 */
#define CONFIG_COMP_CDC_ACM_NOTIFY                       1     /**< Enable/Disable notification feature. */

/*  CDC Class-Specific Request Codes */
#define COMP_CDC_SEND_ENCAPSULATED_COMMAND               0x00U  /**< CDC request to send an encapsulated command. */
#define COMP_CDC_GET_ENCAPSULATED_RESPONSE               0x01U  /**< CDC request to get an encapsulated response. */
#define COMP_CDC_SET_COMM_FEATURE                        0x02U  /**< CDC request to set a communication feature. */
#define COMP_CDC_GET_COMM_FEATURE                        0x03U  /**< CDC request to get a communication feature. */
#define COMP_CDC_CLEAR_COMM_FEATURE                      0x04U  /**< CDC request to clear a communication feature. */
#define COMP_CDC_SET_LINE_CODING                         0x20U  /**< CDC request to set line coding parameters (baud rate, etc.). */
#define COMP_CDC_GET_LINE_CODING                         0x21U  /**< CDC request to get current line coding parameters. */
#define COMP_CDC_SET_CONTROL_LINE_STATE                  0x22U  /**< CDC request from host to control DTR/RTS signals. */
#define COMP_CDC_SEND_BREAK                              0x23U  /**< CDC request to send a break condition. */

/* CDC ACM class-specific notification type */
#define COMP_CDC_NOTIFY_SERIAL_STATE                     0x20   /**< Notification type for serial state changes. */

/*  CDC ACM Serial State Bitmap, These bits are used in the SERIAL_STATE notification message. */
#define COMP_CDC_ACM_CTRL_OVERRUN                        (1 << 6)  /**< Indicates a receive data overrun error. */
#define COMP_CDC_ACM_CTRL_PARITY                         (1 << 5)  /**< Indicates a parity error. */
#define COMP_CDC_ACM_CTRL_FRAMING                        (1 << 4)  /**< Indicates a framing error. */
#define COMP_CDC_ACM_CTRL_RI                             (1 << 3)  /**< State of the Ring Indicator signal. */
#define COMP_CDC_ACM_CTRL_BRK                            (1 << 2)  /**< State of the break detection mechanism. */
#define COMP_CDC_ACM_CTRL_DSR                            (1 << 1)  /**< State of the Data Set Ready signal. */
#define COMP_CDC_ACM_CTRL_DCD                            (1 << 0)  /**< State of the Data Carrier Detect signal. */

/* Endpoint Packet Size Definitions*/
#define COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE             512U /**< High-speed Bulk endpoint maximum packet size. */
#define COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE             64U  /**< Full-speed Bulk endpoint maximum packet size. */

#define COMP_CDC_ACM_HS_BULK_IN_PACKET_SIZE              COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE /**< HS Bulk IN packet size. */
#define COMP_CDC_ACM_HS_BULK_OUT_PACKET_SIZE             COMP_CDC_ACM_HS_BULK_MAX_PACKET_SIZE /**< HS Bulk OUT packet size. */
#define COMP_CDC_ACM_FS_BULK_IN_PACKET_SIZE              COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE /**< FS Bulk IN packet size. */
#define COMP_CDC_ACM_FS_BULK_OUT_PACKET_SIZE             COMP_CDC_ACM_FS_BULK_MAX_PACKET_SIZE /**< FS Bulk OUT packet size. */

#define COMP_CDC_ACM_INTR_IN_PACKET_SIZE                 10U /**< Interrupt IN endpoint packet size. */
#define COMP_CDC_ACM_INTR_IN_REQUEST_SIZE                8U  /**< Interrupt IN request header size for notifications. */
#define COMP_CDC_ACM_INTR_IN_DATA_SIZE                   2U  /**< Interrupt IN data payload size for SERIAL_STATE notification. */
#define COMP_CDC_ACM_HS_INTR_IN_INTERVAL                 8U  /**< High-speed Interrupt IN endpoint polling interval. */
#define COMP_CDC_ACM_FS_INTR_IN_INTERVAL                 8U  /**< Full-speed Interrupt IN endpoint polling interval. */

#define COMP_CDC_ACM_LINE_CODING_SIZE                    7U  /**< Size of the Line Coding structure in bytes. */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief CDC ACM Line Coding Structure.
 * @details This structure holds the settings for the virtual serial port, such as
 *          baud rate, stop bits, parity, and data bits.
 */
typedef struct {
	u32 bitrate;       /**< Data terminal rate, in bits per second. */
	u8 format;         /**< Stop bits: 0=1 Stop bit, 1=1.5 Stop bits, 2=2 Stop bits. */
	u8 parity_type;    /**< Parity: 0=None, 1=Odd, 2=Even, 3=Mark, 4=Space. */
	u8 data_type;      /**< Data bits: 5, 6, 7, 8, or 16. */
} usbd_composite_cdc_acm_line_coding_t;

/**
 * @brief User callback structure for CDC ACM events.
 * @details This structure allows the application layer to handle key CDC ACM events.
 */
typedef struct {
	int(* init)(void);                             /**< Called during class driver initialization for application resource setup. */
	int(* deinit)(void);                           /**< Called during class driver deinitialization for resource cleanup. */
	int(* setup)(usb_setup_req_t *req, u8 *buf);   /**< Called during control transfer SETUP/DATA phases to handle application-specific control requests. */
	int(* received)(u8 *buf, u32 len);             /**< Called when new data is received on the Bulk OUT endpoint. */
	void(* transmitted)(u8 status);                /**< Called after data transmission on the Bulk IN endpoint is complete. */
} usbd_composite_cdc_acm_usr_cb_t;

#if CONFIG_COMP_CDC_ACM_NOTIFY
/**
 * @brief CDC ACM Notification Structure.
 * @details This structure defines the format for sending notifications (like SERIAL_STATE)
 *          on the Interrupt IN endpoint. This is only available if CONFIG_COMP_CDC_ACM_NOTIFY is enabled.
 */
typedef struct {
	u8 bmRequestType;     /**< D7: 0=Host-to-Device, 1=Device-to-Host. D6-5: 1=Class. D4-0: 0=Device, 1=Interface, 2=Endpoint. */
	u8 bNotificationType; /**< Notification code, e.g., COMP_CDC_NOTIFY_SERIAL_STATE. */
	u16 wValue;           /**< Typically 0. */
	u16 wIndex;           /**< The interface number. */
	u16 wLength;          /**< Size of the notification data payload. */
	u8 buf[COMP_CDC_ACM_INTR_IN_DATA_SIZE]; /**< Notification data payload. */
} __PACKED usbd_composite_cdc_acm_ntf_t;
#endif

/**
 * @brief CDC ACM device context structure.
 * @details This main structure holds the state and configuration for the CDC ACM function.
 */
typedef struct {
	usbd_composite_dev_t *cdev;           /**< Pointer to the parent composite device structure. */
	usbd_composite_cdc_acm_usr_cb_t *cb;  /**< Pointer to the user-registered callback structure. */
	usbd_ep_t ep_bulk_in;                 /**< Bulk IN endpoint handler. */
	usbd_ep_t ep_bulk_out;                /**< Bulk OUT endpoint handler. */
#if CONFIG_COMP_CDC_ACM_NOTIFY
	usbd_ep_t ep_intr_in;                 /**< Interrupt IN endpoint handler (for notifications). */
#endif
} usbd_composite_cdc_acm_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usbd_class_driver_t usbd_composite_cdc_acm_driver;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes the CDC ACM composite function.
 * @param[in] cdev: Pointer to the composite device context.
 * @param[in] bulk_out_xfer_size: The desired transfer size for the Bulk OUT buffer.
 * @param[in] bulk_in_xfer_size: The desired transfer size for the Bulk IN buffer.
 * @param[in] cb: Pointer to the user callback structure.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_init(usbd_composite_dev_t *cdev, u16 bulk_out_xfer_size, u16 bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cb);

/**
 * @brief De-initializes the CDC ACM composite function.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_deinit(void);

/**
 * @brief Transmits data over the CDC ACM Bulk IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_transmit(u8 *buf, u16 len);

#if CONFIG_COMP_CDC_ACM_NOTIFY
/**
 * @brief Sends a SERIAL_STATE notification to the host over the CDC ACM Interrupt IN endpoint..
 * @param[in] serial_state: A bitmap of the serial line states (see @ref COMP_CDC_ACM_CTRL_DCD, etc.).
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_notify_serial_state(u16 serial_state);
#endif

#endif  /* USBD_COMPOSITE_CDC_ACM_H */

