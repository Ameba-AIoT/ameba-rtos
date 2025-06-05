/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_HID_H
#define USBD_COMPOSITE_HID_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/* HID requests */
#define COMP_HID_GET_REPORT							0x01
#define COMP_HID_GET_IDLE							0x02
#define COMP_HID_GET_PROTOCOL						0x03
#define COMP_HID_SET_REPORT							0x09
#define COMP_HID_SET_IDLE							0x0A
#define COMP_HID_SET_PROTOCOL						0x0B

#define COMP_HID_INTR_IN_PACKET_SIZE				64

/* wValue of HID report request */
#define COMP_HID_DESC								0x21
#define COMP_HID_REPORT_DESC						0x22

#define COMP_HID_DESC_SIZE							9
#define COMP_HID_DESC_ITEM_LENGTH_OFFSET			7
#define COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET		16

/* Mouse buttons */
#define COMP_HID_MOUSE_BUTTON_LEFT					0x01	// left button. 0: release, 1: press
#define COMP_HID_MOUSE_BUTTON_RIGHT					0x02	// right button. 0: release, 1: press
#define COMP_HID_MOUSE_BUTTON_MIDDLE				0x04	// wheel button. 0: release, 1: press
#define COMP_HID_MOUSE_BUTTON_RESERVED				0xF8	// reserved. all bit should set to 1.

/* Exported types ------------------------------------------------------------*/

typedef struct {
	int(* init)(void);
	void(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	void(* transmitted)(u8 status);
} usbd_composite_hid_usr_cb_t;

typedef struct {
	usbd_ep_t ep_intr_in;
	usbd_composite_hid_usr_cb_t *cb;
	usbd_composite_dev_t *cdev;
} usbd_composite_hid_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usbd_class_driver_t usbd_composite_hid_driver;

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, u16 tx_buf_len, usbd_composite_hid_usr_cb_t *cb);
int usbd_composite_hid_deinit(void);
int usbd_composite_hid_send_data(u8 *data, u16 len);

#endif // USBD_COMPOSITE_HID_H