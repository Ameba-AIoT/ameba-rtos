/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CDC_ACM_H
#define USB_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/
#include "basic_types.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */
/** @addtogroup USB_Common_Constants USB Common Constants
 * @{
 */
/* CDC Class Codes */
#define USB_CDC_CLASS_CODE                              0x02U /**< USB Communication Device Class (CDC) Code */
#define USB_CDC_COMM_INTERFACE_CLASS_CODE               0x02U /**< CDC Communication Interface Class Code */
#define USB_CDC_DATA_INTERFACE_CLASS_CODE               0x0AU /**< CDC Data Interface Class Code */

/* CDC Communication Subclass Codes */
#define USB_CDC_SUBCLASS_RESERVED                       0x00U /**< CDC Subclass Code: Reserved */
#define USB_CDC_SUBCLASS_ACM                            0x02U /**< CDC Subclass Code: Abstract Control Model (ACM) */

/* Communication Interface Class Control Protocol Codes */
#define USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC         0x00U  /**< CDC Protocol Code: No class */
#define USB_CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND         0x01U  /**< CDC Protocol Code: AT cpmmand */
#define USB_CDC_CTRL_PROTOCOL_VENDOR_SPECIFIC           0xFFU  /**< CDC Protocol Code: Vendor */

/* CDC Class-Specific Request Codes */
#define USB_CDC_ACM_SEND_ENCAPSULATED_COMMAND           0x00U /**< CDC request to send an encapsulated command. */
#define USB_CDC_ACM_GET_ENCAPSULATED_RESPONSE           0x01U /**< CDC request to get an encapsulated response. */
#define USB_CDC_ACM_SET_COMM_FEATURE                    0x02U /**< CDC request to set a communication feature. */
#define USB_CDC_ACM_GET_COMM_FEATURE                    0x03U /**< CDC request to get a communication feature. */
#define USB_CDC_ACM_CLEAR_COMM_FEATURE                  0x04U /**< CDC request to clear a communication feature. */
#define USB_CDC_ACM_SET_LINE_CODING                     0x20U /**< CDC request to set the line coding properties. */
#define USB_CDC_ACM_GET_LINE_CODING                     0x21U /**< CDC request to get the line coding properties. */
#define USB_CDC_ACM_SET_CONTROL_LINE_STATE              0x22U /**< CDC request to set the control line state. */
#define USB_CDC_ACM_SEND_BREAK                          0x23U /**< CDC request to send a break condition. */

/* CDC Notification Codes */
#define USB_CDC_ACM_NOTIFY_NETWORK_CONNECTION           0x00U /**< Notification: Network Connection */
#define USB_CDC_ACM_NOTIFY_RESPONSE_AVAILABLE           0x01U /**< Notification: Response Available */
#define USB_CDC_ACM_NOTIFY_SERIAL_STATE                 0x20U /**< Notification: Serial State */

/* Defines the bitmask for the SERIAL_STATE notification */
#define USB_CDC_ACM_CTRL_DCD                            (1 << 0) /**< bRxCarrier: State of DCD signal. */
#define USB_CDC_ACM_CTRL_DSR                            (1 << 1) /**< bTxCarrier: State of DSR signal. */
#define USB_CDC_ACM_CTRL_BRK                            (1 << 2) /**< bBreak: Break condition detected. */
#define USB_CDC_ACM_CTRL_RI                             (1 << 3) /**< bRingSignal: State of ring signal detection. */
#define USB_CDC_ACM_CTRL_FRAMING                        (1 << 4) /**< bFraming: A framing error has occurred. */
#define USB_CDC_ACM_CTRL_PARITY                         (1 << 5) /**< bParity: A parity error has occurred. */
#define USB_CDC_ACM_CTRL_OVERRUN                        (1 << 6) /**< bOverRun: Received data has been discarded. */

/* Standard Packet Sizes and Lengths */
#define USB_CDC_ACM_INTR_IN_PACKET_SIZE                 10U      /**< Maximum packet size for Interrupt IN endpoint */
#define USB_CDC_ACM_INTR_IN_REQUEST_SIZE                8U       /**< Notification request header size (bytes) */
#define USB_CDC_ACM_INTR_IN_DATA_SIZE                   2U       /**< Notification data payload size (bytes) */
#define USB_CDC_ACM_LINE_CODING_SIZE                    7U       /**< Line Coding structure size (Set/Get Line Coding data length) */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 * @{
 */
/**
 * @brief  USB CDC Parity Type Definition
 * @note   Corresponds to the bParityType field in the Line Coding structure
 */
typedef enum {
	LINE_CODING_PARITY_NO    = 0U, /**< No Parity */
	LINE_CODING_PARITY_ODD   = 1U, /**< Odd Parity */
	LINE_CODING_PARITY_EVEN  = 2U, /**< Even Parity */
	LINE_CODING_PARITY_MARK  = 3U, /**< Mark Parity */
	LINE_CODING_PARITY_SPACE = 4U  /**< Space Parity */
} usb_cdc_acm_line_coding_parity_t;

/**
 * @brief  USB CDC Stop Bits Type Definition
 * @note   Corresponds to the bCharFormat field in the Line Coding structure
 */
typedef enum {
	LINE_CODING_CHAR_FORMAT_1_STOP_BITS   = 0U, /**< 1 Stop Bit */
	LINE_CODING_CHAR_FORMAT_1_5_STOP_BITS = 1U, /**< 1.5 Stop Bits */
	LINE_CODING_CHAR_FORMAT_2_STOP_BITS   = 2U  /**< 2 Stop Bits */
} usb_cdc_acm_line_coding_char_format_t;

/**
 * @brief USB CDC Line Coding Structure (Standard Union)
 * @details Corresponds to the 7-byte data sent over USB Class Requests.
 */
typedef union {
	u8 d8[USB_CDC_ACM_LINE_CODING_SIZE]; /**< Raw byte array access */
	struct {
		u32 dwDteRate;   /**< Data Terminal Rate (Baud Rate), unit: bps */
		u8  bCharFormat; /**< Stop Bits, see @ref usb_cdc_acm_line_coding_char_format_t */
		u8  bParityType; /**< Parity Type, see @ref usb_cdc_acm_line_coding_parity_t */
		u8  bDataBits;   /**< Data Bits (usually 5, 6, 7, 8, or 16) */
	} b;                 /**< Structured member access */
} usb_cdc_line_coding_t;
/** @} End of USB_Common_Types group */
/** @} End of USB_Common_API group */

#endif  /* USB_CDC_ACM_H */
