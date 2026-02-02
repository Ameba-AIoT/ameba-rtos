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
/**
 * @brief CDC Class Codes
 * @{
 */
#define USB_CDC_CLASS_CODE                              0x02U
#define USB_CDC_COMM_INTERFACE_CLASS_CODE               0x02U
#define USB_CDC_DATA_INTERFACE_CLASS_CODE               0x0AU
/** @} */

/**
 * @brief CDC Communication Subclass Codes
 * @{
 */
#define USB_CDC_SUBCLASS_RESERVED                       0x00U
#define USB_CDC_SUBCLASS_ACM                            0x02U
/** @} */

/**
 * @brief Communication Interface Class Control Protocol Codes
 * @{
 */
#define USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC         0x00U
#define USB_CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND         0x01U
#define USB_CDC_CTRL_PROTOCOL_VENDOR_SPECIFIC           0xFFU
/** @} */

/**
 * @brief CDC Class-Specific Request Codes
 * @{
 */
#define USB_CDC_ACM_SEND_ENCAPSULATED_COMMAND           0x00U
#define USB_CDC_ACM_GET_ENCAPSULATED_RESPONSE           0x01U
#define USB_CDC_ACM_SET_COMM_FEATURE                    0x02U
#define USB_CDC_ACM_GET_COMM_FEATURE                    0x03U
#define USB_CDC_ACM_CLEAR_COMM_FEATURE                  0x04U
#define USB_CDC_ACM_SET_LINE_CODING                     0x20U
#define USB_CDC_ACM_GET_LINE_CODING                     0x21U
#define USB_CDC_ACM_SET_CONTROL_LINE_STATE              0x22U
#define USB_CDC_ACM_SEND_BREAK                          0x23U
/** @} */

/**
 * @brief CDC Notification Codes
 * @{
 */
#define USB_CDC_ACM_NOTIFY_NETWORK_CONNECTION           0x00U
#define USB_CDC_ACM_NOTIFY_RESPONSE_AVAILABLE           0x01U
#define USB_CDC_ACM_NOTIFY_SERIAL_STATE                 0x20U
/** @} */

/**
 * @brief Serial State Notification Bitmask
 * @{
 */
#define USB_CDC_ACM_CTRL_DCD                            (1 << 0)
#define USB_CDC_ACM_CTRL_DSR                            (1 << 1)
#define USB_CDC_ACM_CTRL_BRK                            (1 << 2)
#define USB_CDC_ACM_CTRL_RI                             (1 << 3)
#define USB_CDC_ACM_CTRL_FRAMING                        (1 << 4)
#define USB_CDC_ACM_CTRL_PARITY                         (1 << 5)
#define USB_CDC_ACM_CTRL_OVERRUN                        (1 << 6)
/** @} */

/**
 * @brief Standard Packet Sizes and Lengths
 * @{
 */
#define USB_CDC_ACM_INTR_IN_PACKET_SIZE                 10U
#define USB_CDC_ACM_INTR_IN_REQUEST_SIZE                8U
#define USB_CDC_ACM_INTR_IN_DATA_SIZE                   2U
#define USB_CDC_ACM_LINE_CODING_SIZE                    7U
/** @} */

/* Line coding parity type */
typedef enum {
	LINE_CODING_PARITY_NO = 0U,
	LINE_CODING_PARITY_ODD,
	LINE_CODING_PARITY_EVEN,
	LINE_CODING_PARITY_MARK,
	LINE_CODING_PARITY_SPACE
} usb_cdc_acm_line_coding_parity_t;

/* Line coding char format */
typedef enum {
	LINE_CODING_CHAR_FORMAT_1_STOP_BITS = 0U,
	LINE_CODING_CHAR_FORMAT_1_5_STOP_BITS,
	LINE_CODING_CHAR_FORMAT_2_STOP_BITS
} usb_cdc_acm_line_coding_char_format_t;

/**
 * @brief USB CDC Line Coding Structure (Standard Union)
 * @details Corresponds to the 7-byte data sent over USB Class Requests.
 */
typedef union {
	u8 d8[USB_CDC_ACM_LINE_CODING_SIZE];
	struct {
		u32 dwDteRate;    /**< Data terminal rate, in bits per second */
		u8  bCharFormat;  /**< Stop bits (usb_cdc_char_format_t) */
		u8  bParityType;  /**< Parity (usb_cdc_parity_t) */
		u8  bDataBits;    /**< Data bits (5, 6, 7, 8 or 16) */
	} b;
} usb_cdc_line_coding_t;

#endif  /* USB_CDC_ACM_H */