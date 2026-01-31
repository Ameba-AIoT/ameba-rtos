/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_VENDOR_H
#define USBD_VENDOR_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/*  Vendor Test parameters */
#define USBD_VENDOR_BULK_OUT_XFER_CNT					100
#define USBD_VENDOR_INTR_OUT_XFER_CNT					100
#define USBD_VENDOR_ISOC_OUT_XFER_CNT					100

/* Vendor Device parameters */
#define USBD_VENDOR_VID									USB_VID
#define USBD_VENDOR_PID									USB_PID
#define USBD_VENDOR_CLASS_CODE							0xFFU
#define USBD_VENDOR_SUBCLASS_CODE						0x00U
#define USBD_VENDOR_PROTOCOL							0x00U
#define USBD_VENDOR_SELF_POWERED						1U
#define USBD_VENDOR_REMOTE_WAKEUP_EN					1U
#define USBD_VENDOR_CONFIG_DESC_SIZE					83U
#define USBD_VENDOR_LANGID_STRING						0x0409U
#define USBD_VENDOR_MFG_STRING							"Realtek"
#define USBD_VENDOR_PROD_HS_STRING						"Realtek Vendor (HS)"
#define USBD_VENDOR_PROD_FS_STRING						"Realtek Vendor (FS)"
#define USBD_VENDOR_SN_STRING							"1234567890"

/* Vendor Endpoint parameters */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_VENDOR_BULK_IN_EP                          0x86U  /* EP6 for BULK IN */
#define USBD_VENDOR_BULK_OUT_EP                         0x03U  /* EP3 for BULK OUT */
#define USBD_VENDOR_ISOC_IN_EP                          0x82U  /* EP2 for ISOC IN */
#define USBD_VENDOR_ISOC_OUT_EP                         0x02U  /* EP2 for ISOC OUT */
#define USBD_VENDOR_INTR_IN_EP                          0x84U  /* EP4 for INTR IN */
#define USBD_VENDOR_INTR_OUT_EP                         0x05U  /* EP5 for INTR OUT */
#elif defined (CONFIG_AMEBAL2)
#define USBD_VENDOR_BULK_IN_EP                          0x84U  /* EP4 for BULK IN */
#define USBD_VENDOR_BULK_OUT_EP                         0x04U  /* EP4 for BULK OUT */
#define USBD_VENDOR_ISOC_IN_EP                          0x81U  /* EP1 for ISOC IN */
#define USBD_VENDOR_ISOC_OUT_EP                         0x01U  /* EP1 for ISOC OUT */
#define USBD_VENDOR_INTR_IN_EP                          0x82U  /* EP2 for INTR IN */
#define USBD_VENDOR_INTR_OUT_EP                         0x02U  /* EP2 for INTR OUT */
#else
#define USBD_VENDOR_BULK_IN_EP							0x81U  /* EP1 for BULK IN */
#define USBD_VENDOR_BULK_OUT_EP							0x02U  /* EP2 for BULK OUT */
#define USBD_VENDOR_INTR_IN_EP							0x85U  /* EP1 for INTR IN */
#define USBD_VENDOR_INTR_OUT_EP							0x05U  /* EP2 for INTR OUT */
#define USBD_VENDOR_ISOC_IN_EP							0x83U  /* EP3 for ISOC IN */
#define USBD_VENDOR_ISOC_OUT_EP							0x04U  /* EP4 for ISOC OUT */
#endif

#define USBD_VENDOR_HS_BULK_MPS							512U   /* High speed BULK IN & OUT max packet size */
#define USBD_VENDOR_FS_BULK_MPS							64U   /* Full speed BULK IN & OUT max packet size */
#define USBD_VENDOR_HS_INTR_MPS							1024U   /* High speed INTR IN & OUT max packet size */
#define USBD_VENDOR_FS_INTR_MPS							64U   /* Full speed INTR IN & OUT max packet size */
#define USBD_VENDOR_HS_ISOC_MPS							1024U   /* High speed ISOC IN & OUT max packet size */
#define USBD_VENDOR_FS_ISOC_MPS							1023U   /* Full speed ISOC IN & OUT max packet size */

#define USBD_VENDOR_HS_INTR_IN_INTERVAL					1U    /* High speed INTR IN interval */
#define USBD_VENDOR_HS_INTR_OUT_INTERVAL				1U    /* High speed INTR OUT interval */
#define USBD_VENDOR_FS_INTR_IN_INTERVAL					2U    /* Full speed INTR IN interval */
#define USBD_VENDOR_FS_INTR_OUT_INTERVAL				2U    /* Full speed INTR OUT interval */

#define USBD_VENDOR_HS_ISOC_IN_INTERVAL					1U    /* High speed ISOC IN interval */
#define USBD_VENDOR_HS_ISOC_OUT_INTERVAL				1U    /* High speed ISOC OUT interval */
#define USBD_VENDOR_FS_ISOC_IN_INTERVAL					2U    /* Full speed ISOC IN interval */
#define USBD_VENDOR_FS_ISOC_OUT_INTERVAL				2U    /* Full speed ISOC OUT interval */

/* Exported types ------------------------------------------------------------*/

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* bulk_received)(u8 *buf, u32 len);
	int(* intr_received)(u8 *buf, u32 len);
	int(* isoc_received)(u8 *buf, u32 len);
	void(* bulk_transmitted)(u8 status);
	void(* intr_transmitted)(u8 status);
	void(* isoc_transmitted)(u8 status);
	void (*status_changed)(u8 old_status, u8 status);
} usbd_vendor_cb_t;

typedef struct {
	usb_setup_req_t ctrl_req;
	usbd_ep_t ep_isoc_in;
	usbd_ep_t ep_isoc_out;
	usbd_ep_t ep_bulk_in;
	usbd_ep_t ep_bulk_out;
	usbd_ep_t ep_intr_in;
	usbd_ep_t ep_intr_out;
	usb_dev_t *dev;
	usbd_vendor_cb_t *cb;
	u8 alt_setting;
} usbd_vendor_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_vendor_init(usbd_vendor_cb_t *cb);
int usbd_vendor_deinit(void);
int usbd_vendor_transmit_ctrl_data(u8 *buf, u32 len);
int usbd_vendor_transmit_bulk_data(u8 *buf, u32 len);
int usbd_vendor_receive_bulk_data(void);
int usbd_vendor_transmit_intr_data(u8 *buf, u32 len);
int usbd_vendor_receive_intr_data(void);
int usbd_vendor_transmit_isoc_data(u8 *buf, u32 len);
int usbd_vendor_receive_isoc_data(void);

#endif  /* USBD_VENDOR_H */

