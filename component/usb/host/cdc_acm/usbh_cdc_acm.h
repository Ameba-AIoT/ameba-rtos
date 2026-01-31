/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ACM_H
#define USBH_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_cdc_acm.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief CDC ACM state machine.
 */
typedef enum {
	USBH_CDC_ACM_STATE_IDLE = 0U,                /**< State IDLE: Ready for operation. */
	USBH_CDC_ACM_STATE_SET_LINE_CODING,          /**< State SET_LINE_CODING: Configuring line coding. */
	USBH_CDC_ACM_STATE_GET_LINE_CODING,          /**< State GET_LINE_CODING: Retrieving line coding. */
	USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE,   /**< State SET_CONTROL_LINE_STATE: Setting control line state. */
	USBH_CDC_ACM_STATE_TRANSFER,                 /**< State TRANSFER: Data transfer in progress. */
	USBH_CDC_ACM_STATE_ERROR,                    /**< State ERROR: Error occurred. */
} usbh_cdc_acm_state_t;

/**
 * @brief Structure containing callback functions for the CDC ACM host class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Called when the CDC ACM host driver initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the CDC ACM host driver de-initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Called when a CDC ACM device is attached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* attach)(void);

	/**
	 * @brief Called when a CDC ACM device is detached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests completion.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(void);

	/**
	 * @brief Called when interrupt data is received from the device (e.g. Serial State).
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @param[in] status: The status of the transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* notify)(u8 *buf, u32 len, u8 status);

	/**
	 * @brief Called when data is received from the device on the BULK IN pipe.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @param[in] status: The status of the transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* receive)(u8 *buf, u32 len, u8 status);

	/**
	 * @brief Called when a data transmission to the device on the BULK OUT pipe is complete.
	 * @param[in] status: The status of the transmission.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* transmit)(u8 status);

	/**
	 * @brief Called when the line coding parameters have changed.
	 * @param[in] line_coding: Pointer to the new line coding structure.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* line_coding_changed)(usb_cdc_line_coding_t *line_coding);
} usbh_cdc_acm_cb_t;

/**
 * @brief Structure representing the CDC ACM host instance.
 */
typedef struct {
	usbh_pipe_t bulk_in;                        /**< BULK IN pipe structure. */
	usbh_pipe_t bulk_out;                       /**< BULK OUT pipe structure. */
	usbh_pipe_t intr_in;                        /**< INTERRUPT IN pipe structure. */
	usb_host_t *host;                           /**< Pointer to the USB host instance. */
	usbh_cdc_acm_cb_t *cb;                      /**< Pointer to the user-defined callback structure. */
	usb_cdc_line_coding_t *line_coding;    /**< Current line coding of the device. */
	usb_cdc_line_coding_t *user_line_coding; /**< User requested line coding. */
	usbh_cdc_acm_state_t state;                 /**< Current state of the CDC ACM host driver. */
} usbh_cdc_acm_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes the CDC ACM host class driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_init(usbh_cdc_acm_cb_t *cb);

/**
 * @brief De-initializes the CDC ACM host class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_deinit(void);

/**
 * @brief Sets the line coding parameters for the device.
 * @param[in] lc: Pointer to the line coding structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_set_line_coding(usb_cdc_line_coding_t *lc);

/**
 * @brief Gets the current line coding parameters from the device.
 * @param[out] lc: Pointer to the structure where the line coding will be stored.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_get_line_coding(usb_cdc_line_coding_t *lc);

/**
 * @brief Sets the control line state (DTR, RTS).
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_set_control_line_state(void);

/**
 * @brief Transmits data to the device over the BULK OUT pipe.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_transmit(u8 *buf, u32 len);

/**
 * @brief Prepares to receive data from the device over the BULK IN pipe.
 * @param[in] buf: Pointer to the buffer where received data will be stored.
 * @param[in] len: Length of the buffer in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_receive(u8 *buf, u32 len);

/**
 * @brief Prepares to receive notification data (e.g. Serial State) over the INTERRUPT IN pipe.
 * @param[in] buf: Pointer to the buffer where notification data will be stored.
 * @param[in] len: Length of the buffer in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len);

/**
 * @brief Gets the Maximum Packet Size (MPS) of the BULK endpoint.
 * @return The MPS in bytes.
 */
u16 usbh_cdc_acm_get_bulk_ep_mps(void);

#endif  /* USBH_CDC_ACM_H */
