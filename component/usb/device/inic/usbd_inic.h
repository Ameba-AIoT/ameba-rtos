/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_INIC_H
#define USBD_INIC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_inic_otp.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_INIC_VID									USB_VID
#define USBD_INIC_PID									USB_PID

#define USBD_INIC_LANGID_STRING							0x409
#define USBD_INIC_IDX_INTERFACE_STR						0x04U

#define USBD_INIC_ITF_NUM								3U
#define USBD_INIC_ITF_BT								0U
#define USBD_INIC_ITF_BT_SCO							1U
#define USBD_INIC_ITF_WIFI								2U

#define USBD_INIC_HS_BULK_MPS							512U   /* High speed BULK IN & OUT max packet size */
#define USBD_INIC_FS_BULK_MPS							64U   /* Full speed BULK IN & OUT max packet size */
#define USBD_INIC_HS_INTR_MPS							16U   /* High speed INTR IN & OUT max packet size */
#define USBD_INIC_FS_INTR_MPS							16U   /* Full speed INTR IN & OUT max packet size */

#define USBD_INIT_BT_EP1_INTR_IN						0x81U
#define USBD_INIC_BT_EP2_BULK_IN						0x82U
#define USBD_INIC_BT_EP2_BULK_OUT						0x02U
#define USBD_WHC_WIFI_EP4_BULK_IN 						0x84U
#define USBD_WHC_WIFI_EP5_BULK_OUT						0x05U
#define USBD_WHC_WIFI_EP6_BULK_OUT						0x06U
#define USBD_WHC_WIFI_EP7_BULK_OUT						0x07U

#define USBD_INIC_CTRL_BUF_SIZE							512U  /* CTRL buffer size */

#define USBD_INIC_EP_STATE_IDLE							0U
#define USBD_INIC_EP_STATE_BUSY							1U

/* Exported types ------------------------------------------------------------*/

typedef struct {
	usbd_ep_t ep;
	void *userdata; // userdata for each ep
} usbd_inic_ep_t;

typedef struct {
	// Common callbacks
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* clear_config)(void);
	void(* transmitted)(usbd_inic_ep_t *in_ep, u8 status);
	int(* received)(usbd_inic_ep_t *out_ep, u16 len);
	void (*status_changed)(u8 old_status, u8 status);
	void(*suspend)(void);
	void(*resume)(void);
} usbd_inic_cb_t;

typedef struct {
	usbd_inic_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_inic_ep_t out_ep[USB_MAX_ENDPOINTS];
	usbd_otp_t otp;
	usb_dev_t *dev;
	usbd_inic_cb_t *cb;
	usb_setup_req_t ctrl_req;
	u8  bt_alt;
	u8  bt_sco_alt;
	u8  wifi_alt;
} usbd_inic_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_inic_init(usbd_inic_cb_t *cb);
int usbd_inic_deinit(void);
int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len);
int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u16 len, void *userdata);
int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u16 len, void *userdata);
u8 usbd_inic_is_bt_en(void);
#endif  /* USBD_INIC_H */

