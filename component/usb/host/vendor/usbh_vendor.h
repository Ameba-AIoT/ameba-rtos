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

/* Vendor test configuration */
#define CONFIG_USBH_VENDOR_CTRL_TEST		1
#define CONFIG_USBH_VENDOR_BULK_TEST		0
#define CONFIG_USBH_VENDOR_INTR_TEST		0
#define CONFIG_USBH_VENDOR_ISOC_TEST		0

#define USBH_VENDOR_BULK_IN_BUF_SIZE		64
#define USBH_VENDOR_BULK_OUT_BUF_SIZE		64
#define USBH_VENDOR_INTR_IN_BUF_SIZE		64
#define USBH_VENDOR_INTR_OUT_BUF_SIZE		64
#define USBH_VENDOR_ISOC_IN_BUF_SIZE		64
#define USBH_VENDOR_ISOC_OUT_BUF_SIZE		64

#define USBH_VENDOR_BULK_XFER_CNT			100
#define USBH_VENDOR_INTR_XFER_CNT			100
#define USBH_VENDOR_ISOC_XFER_CNT			100

/* Exported types ------------------------------------------------------------*/
#define USB_FIFO_READ_SIZE_ALIGNED4B(x)     ((((u32)x + 3U) / 4U) * 4U ) /*FIFO read data set u32 as basic data length*/
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
	int(* detach)(void);
	int(* setup)(void);
} usbh_vendor_cb_t;

typedef struct {
	u8  pipe_num;
	u8  ep_num;
	u8  ep_type;
	u16 ep_mps;
	u32 ep_interval;
	u8 *xfer_buf;
	u8 *xfer_bk_buf;
	u16 xfer_len;
	u16 xfer_max_len;
	u8  xfer_cnt;
	u8  xfer_max_cnt;
	u8 *test_buf;
	__IO u32 tick;
	usbh_vendor_state_t	state;
} usbh_vendor_xfer_t;

/* Vendor host */
typedef struct {
	u8  ctrl_buf[USB_FIFO_READ_SIZE_ALIGNED4B(USB_CTRL_BUF_LENGTH)];
	usbh_vendor_state_t state;
	usbh_vendor_xfer_t bulk_in_xfer;
	usbh_vendor_xfer_t bulk_out_xfer;
	usbh_vendor_xfer_t intr_in_xfer;
	usbh_vendor_xfer_t intr_out_xfer;
	usbh_vendor_xfer_t isoc_in_xfer;
	usbh_vendor_xfer_t isoc_out_xfer;
	u8                 ptx_busy;
	u8                 interface_id;
	usbh_vendor_cb_t   *cb;
	usb_host_t         *host;
} usbh_vendor_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_vendor_init(usbh_vendor_cb_t *cb);
int usbh_vendor_deinit(void);

#endif  /* USBH_VENDOR_H */

