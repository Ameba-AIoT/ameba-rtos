/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_DEF_H
#define USB_DEF_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */
/** @addtogroup USB_Common_Types USB Common Types
 * @{
 */

/**
 * @brief Defines the operational speeds for the USB controller.
 * @note  This is a controller operating mode rather than a Chapter 9 definition:
 *        `USB_SPEED_HIGH_IN_FULL` has no counterpart on the bus, it describes a
 *        High-Speed capable PHY driven in Full-Speed mode.
 */
typedef enum {
	USB_SPEED_HIGH = 0,                 /**< High Speed (480 Mbps). */
	USB_SPEED_HIGH_IN_FULL,             /**< High Speed core running in Full Speed mode. */
	USB_SPEED_LOW,                      /**< Low Speed (1.5 Mbps). */
	USB_SPEED_FULL                      /**< Full Speed (12 Mbps). */
} usb_speed_type_t;

/**
 * @brief USB endpoint information structure.
 * @details Contains basic endpoint configuration: interval, address, max packet size, and type.
 */
typedef struct {
	u16 interval;                       /**< Endpoint polling interval in ticks, High-speed means 2^(binterval-1). */
	u16 mps : 11;                       /**< Maximum Packet Size for this endpoint (0-1024). */
	u16 type : 2;                       /**< Endpoint type (Control, Bulk, Isochronous, Interrupt). */
	u8 binterval;                       /**< Polling interval for the endpoint. Full Speed: 1-255, High Speed: 1-16 */
	u8 addr;                            /**< Endpoint address (includes direction). */
} usb_ep_info_t;

/** @} End of USB_Common_Types group */

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Common_Functions USB Common Functions
 * @{
 */
/**
  * @brief  Get the per-packet stride of an OUT transfer, adjusted to the DWORD boundary
  * @note   DWC OTG Programming Guide, Application Requirements of the OUT data transfers:
  *         the transfer size programmed into DOEPTSIZn.XferSize shall be a multiple of the
  *         MPS adjusted to the DWORD boundary, because in DMA mode the controller always
  *         starts a received packet on a DWORD boundary and pads a max-packet-size packet
  *         up to the end of the DWORD.
  *             if ((mps mod 4) == 0) stride = mps
  *             else                  stride = mps + 4 - (mps mod 4)
  * @param  mps: Max packet size of the endpoint, in bytes
  * @retval Per-packet stride in bytes
  */
static inline u32 usb_get_dword_aligned_mps(u32 mps)
{
	return (mps + 3U) & (~3U);
}

/**
  * @brief  Get the DMA window length of an OUT transfer
  * @note   The controller receives whole packets only, so it is programmed with
  *         XFRSIZ = usb_get_dword_aligned_mps(MPS) * PKTCNT, refer to
  *         usbd_hal_ep_start_transfer. The RX buffer shall be able to hold this length,
  *         and the cache maintenance before the transfer shall cover it.
  * @param  xfer_len: Requested transfer length, in bytes
  * @param  mps: Max packet size of the endpoint, in bytes
  * @retval DMA window length in bytes, xfer_len if mps is invalid
  */
static inline u32 usb_get_dma_len(u32 xfer_len, u32 mps)
{
	u32 pktcnt;

	if (mps == 0U) {
		return xfer_len;
	}

	pktcnt = (xfer_len == 0U) ? 1U : ((xfer_len + mps - 1U) / mps);

	return pktcnt * usb_get_dword_aligned_mps(mps);
}

/**
  * @brief  Get the largest OUT transfer length whose DMA window still fits in a given buffer
  * @note   This is the inverse of @ref usb_get_dma_len: it answers how long a transfer may be
  *         requested before the window the core derives from it overflows the buffer.
  *         Solving usb_get_dma_len(len, mps) <= buf_len gives
  *             n = buf_len / usb_get_dword_aligned_mps(mps)    whole packet slots that fit
  *             len = n * mps                                   payload carried by those n packets
  *         The divisor is the DWORD adjusted MPS because that is what a packet costs in memory,
  *         while the multiplier is the raw MPS because that is what a packet carries on the wire.
  *         Bounding the payload by buf_len instead is NOT enough: one extra payload byte costs a
  *         whole extra padded packet, e.g. with MPS 1023 and a 1024 byte buffer a 1024 byte
  *         request needs 2 packets, so a 2048 byte window.
  * @param  buf_len: Capacity of the transfer buffer, in bytes
  * @param  mps: Max packet size of the endpoint, in bytes
  * @retval Largest requestable transfer length in bytes, 0 if the buffer cannot hold one packet,
  *         buf_len if mps is invalid
  */
static inline u32 usb_get_max_payload_len(u32 buf_len, u32 mps)
{
	u32 aligned_mps;

	if (mps == 0U) {
		return buf_len;
	}

	aligned_mps = usb_get_dword_aligned_mps(mps);

	return (buf_len / aligned_mps) * mps;
}

/** @} End of USB_Common_Functions group */

/** @} End of USB_Common_API group */

#endif /* USB_DEF_H */
