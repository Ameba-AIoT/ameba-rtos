/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_VENDOR_H
#define USBH_VENDOR_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

/* Vendor Class Codes */
#define VENDOR_CLASS_CODE					0xFFU
/* Vendor SubClass Codes */
#define VENDOR_SUBCLASS_CODE				0x00U
/* Vendor Protocol */
#define VENDOR_PROTOCOL						0x00U

#define USBH_VENDOR_MASK_ALL_EPS			0x3FU
#define USBH_VENDOR_MASK_INTR_IN			0x01U
#define USBH_VENDOR_MASK_INTR_OUT		 	0x02U
#define USBH_VENDOR_MASK_BULK_IN			0x04U
#define USBH_VENDOR_MASK_BULK_OUT			0x08U
#define USBH_VENDOR_MASK_ISOC_IN			0x10U
#define USBH_VENDOR_MASK_ISOC_OUT			0x20U
/* Exported types ------------------------------------------------------------*/
#define USB_FIFO_READ_SIZE_ALIGNED4B(x)     ((((u32)x + 3U) / 4U) * 4U ) /* FIFO read data set u32 as basic data length */
#define USB_CTRL_BUF_LENGTH                 (18)

/* States for Vendor State Machine */
typedef enum {
	VENDOR_STATE_IDLE = 0U,
	VENDOR_STATE_XFER,
	VENDOR_STATE_BUSY,
	VENDOR_STATE_ERROR,
} usbh_vendor_state_t;

/* Vendor user callback interface */
typedef struct {
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* receive)(u8 ep_type, u8 *buf, u32 len, u8 status);
	int(* transmit)(u8 ep_type);
} usbh_vendor_cb_t;

typedef struct {
	usbh_pipe_t pipe;
	u8 *xfer_bk_buf;
	u8 *test_buf;
	u32 test_mask;
	u32 cur_frame;
	__IO u32 error_cnt;
	u16 xfer_max_len;
	u8 xfer_cnt;
	u8 xfer_max_cnt;
} usbh_vendor_xfer_t;

/* Vendor host */
typedef struct {
	usbh_vendor_xfer_t bulk_in_xfer;
	usbh_vendor_xfer_t bulk_out_xfer;
	usbh_vendor_xfer_t intr_in_xfer;
	usbh_vendor_xfer_t intr_out_xfer;
	usbh_vendor_xfer_t isoc_in_xfer;
	usbh_vendor_xfer_t isoc_out_xfer;
	usbh_vendor_cb_t *cb;
	usb_host_t *host;
	u8 *ctrl_buf;
	u32 ep_mask;
	usbh_vendor_state_t state;
} usbh_vendor_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_vendor_init(usbh_vendor_cb_t *cb);
int usbh_vendor_deinit(void);
int usbh_vendor_bulk_transmit(u8 *buf, u32 len, u32 test_cnt);
int usbh_vendor_bulk_receive(u8 *buf, u32 len, u32 test_cnt);
int usbh_vendor_intr_transmit(u8 *buf, u32 len, u32 test_cnt);
int usbh_vendor_intr_receive(u8 *buf, u32 len, u32 test_cnt);
int usbh_vendor_isoc_transmit(u8 *buf, u32 len, u32 test_cnt);
int usbh_vendor_isoc_receive(u8 *buf, u32 len, u32 test_cnt);
u16 usbh_vendor_get_bulk_ep_mps(void);
u16 usbh_vendor_get_intr_ep_mps(void);
#endif  /* USBH_VENDOR_H */

