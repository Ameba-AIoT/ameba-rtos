/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_RTL815X_H
#define USB_RTL815X_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"
#include "usb_ch9.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 * @{
 */
/** @defgroup USB_Common_RTL815X_Constants USB RTL815X Constants
 *  @brief Vendor register access of the Realtek RTL8152/RTL8153/RTL8156 USB Ethernet chips, shared by
 *         the USBH CDC ECM and CDC NCM drivers. Names and values follow the Realtek r8152 driver v2.22.1
 *         (r8152.c), the vendor driver of these chips.
 *  @{
 */

/* Vendor register access request (Ref r8152 get_registers()/set_registers()): wValue = register
 * address, wLength = register bytes. wIndex = MCU type for a dword read (ocp_read_dword()),
 * MCU type | byte enable for a write (generic_ocp_write()) */
#define USB_RTL815X_REQT_READ               (USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE) /**< bmRequestType of a register read */
#define USB_RTL815X_REQT_WRITE              (USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE) /**< bmRequestType of a register write */
#define USB_RTL815X_REQ_GET_REGS            0x05U   /**< bRequest of a register read */
#define USB_RTL815X_REQ_SET_REGS            0x05U   /**< bRequest of a register write */
#define USB_RTL815X_REG_DWORD_LEN           4U      /**< wLength of one register dword access */

/* MCU type, high byte of wIndex */
#define USB_RTL815X_MCU_TYPE_PLA            0x0100U /**< PLA (MAC) register space */
#define USB_RTL815X_MCU_TYPE_USB            0x0000U /**< USB register space */

/* Byte enable of a write, low byte of wIndex: one bit per byte of the addressed dword, the low
 * nibble for the first dword and the high nibble for the last one of the access */
#define USB_RTL815X_BYTE_EN_DWORD           0xFFU   /**< All 4 bytes */
#define USB_RTL815X_BYTE_EN_WORD            0x33U   /**< Bytes 0..1 */
#define USB_RTL815X_BYTE_EN_BYTE            0x11U   /**< Byte 0 */

/* PLA registers */
#define USB_RTL815X_PLA_IDR                 0xC000U /**< MAC address, 6 bytes */
#define USB_RTL815X_PLA_IDR_HIGH            (USB_RTL815X_PLA_IDR + USB_RTL815X_REG_DWORD_LEN) /**< MAC address bytes 4..5 */
#define USB_RTL815X_PLA_RCR                 0xC010U /**< Receive configuration */
#define USB_RTL815X_PLA_RXFIFO_CTRL1        0xC0A4U /**< RX FIFO threshold 2 */
#define USB_RTL815X_PLA_RXFIFO_CTRL2        0xC0A8U /**< RX FIFO threshold 3 */
#define USB_RTL815X_PLA_LEDSEL              0xDD90U /**< LED selection */
#define USB_RTL815X_PLA_CRWECR              0xE81CU /**< Configuration register write enable, guards PLA_IDR */

/* PLA_RCR */
#define USB_RTL815X_RCR_AAP                 0x00000001U /**< Accept all physical (unicast to any MAC) */
#define USB_RTL815X_RCR_APM                 0x00000002U /**< Accept physical match (unicast to own MAC) */
#define USB_RTL815X_RCR_AM                  0x00000004U /**< Accept multicast */
#define USB_RTL815X_RCR_AB                  0x00000008U /**< Accept broadcast */
#define USB_RTL815X_RCR_ACPT_ALL            (USB_RTL815X_RCR_AAP | USB_RTL815X_RCR_APM | USB_RTL815X_RCR_AM | USB_RTL815X_RCR_AB) /**< Accept all */

/* PLA_RXFIFO_CTRL1 */
#define USB_RTL815X_RXFIFO_THR2_FULL        0x00000060U /**< RX FIFO threshold 2 at Full/Low Speed */
#define USB_RTL815X_RXFIFO_THR2_HIGH        0x00000038U /**< RX FIFO threshold 2 at High Speed */

/* PLA_RXFIFO_CTRL2 */
#define USB_RTL815X_RXFIFO_THR3_FULL        0x00000078U /**< RX FIFO threshold 3 at Full/Low Speed */
#define USB_RTL815X_RXFIFO_THR3_HIGH        0x00000048U /**< RX FIFO threshold 3 at High Speed */

/* PLA_CRWECR, r8152 CRWECR_NORAML/CRWECR_CONFIG */
#define USB_RTL815X_CRWECR_NORMAL           0x00U   /**< Configuration registers locked */
#define USB_RTL815X_CRWECR_CONFIG           0xC0U   /**< Configuration registers writable */

/** @} End of USB_Common_RTL815X_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_RTL815X_H */
