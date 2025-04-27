/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_HID_BI_DIR_H
#define USBD_COMPOSITE_HID_BI_DIR_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

#define COMP_HID_BUF_MAX_CNT                           10

/* HID requests */
#define COMP_HID_GET_REPORT                            0x01
#define COMP_HID_GET_IDLE                              0x02
#define COMP_HID_GET_PROTOCOL                          0x03
#define COMP_HID_SET_REPORT                            0x09
#define COMP_HID_SET_IDLE                              0x0A
#define COMP_HID_SET_PROTOCOL                          0x0B

/* wValue of HID report request */
#define COMP_HID_DESC                                  0x21
#define COMP_HID_REPORT_DESC                           0x22

/* Exported types ------------------------------------------------------------*/

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* sof)(void);
	int(* transmitted)(u8 status);
} usbd_composite_hid_usr_cb_t;

typedef struct {
	u8 *buf_raw;
	__IO u16 buf_valid_len;
} usbd_composite_hid_buf_t;

typedef struct {
	rtos_sema_t uac_hid_sema;
	usbd_composite_hid_buf_t *buf_array;
	u8 *hid_buf;

	u16 buf_array_cnt;
	u16 hid_mps; //mps

	u8 hid_sema_valid;
	__IO u8 read_wait_sema;
	__IO u8 read_idx;  /*  */
	__IO u8 write_idx; /* wr_idx means the next write pos, the wr_idx data is not valid */
	__IO u8 is_intr_out_busy;
} usbd_composite_hid_buf_ctrl_t;

typedef struct {
	u8 *intr_in_buf;
	u16 intr_in_buf_size;

	__IO u8 intr_tx_zlp : 1;
	__IO u8 intr_in_state : 1;
	__IO u8 is_intr_in_busy : 1;
} usbd_composite_hid_ep_t;

typedef struct {
	usbd_composite_hid_usr_cb_t *cb;
	usbd_composite_dev_t *cdev;

	usbd_composite_hid_buf_ctrl_t hid_priv_out;
	usbd_composite_hid_ep_t hid_priv_in;

	usbd_composite_hid_ep_t hid_in;

	__IO u8 is_ready : 1;

	/*
	    Windows Hid Tool adds reportID for the transfer data,and the transfer len = ReportSize
	    the TV does not add the reportID, and it just report the use data,the size may < reportSize
	*/
	__IO u8 report_id_append : 1;
} usbd_composite_hid_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern usbd_class_driver_t usbd_composite_hid_driver;

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, usbd_composite_hid_usr_cb_t *cb);
int usbd_composite_hid_deinit(void);
int usbd_composite_hid_send_data(u8 *data, u16 len);
int usbd_composite_hid_volume_ctrl(u8 vol_up);
int usbd_composite_hid_power_ctrl(void);

u32 usbd_composite_hid_read(u8 *buffer, u32 size, u32 time_out_ms);
u32 usbd_composite_hid_get_read_buf_cnt(void);
u32 usbd_composite_hid_ring_buf_is_full(void);

#endif // USBD_COMPOSITE_HID_BI_DIR_H