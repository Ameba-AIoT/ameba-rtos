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

/* Report size */
#define COMP_HID_DESC_SIZE                             9
#define COMP_HID_DESC_ITEM_LENGTH_OFFSET               7
#define COMP_HID_ITF_DESC_ITEM_LENGTH_OFFSET           16
#define COMP_HID_ITF_VEND_DESC_ITEM_LENGTH_OFFSET      48

/* Exported types ------------------------------------------------------------*/

/**
 * @brief User callback structure for HID events.
 * @details This structure allows the application layer to handle key HID events.
 */
typedef struct {
	int(* init)(void);                             /**< Called during class driver initialization for application resource setup. */
	int(* deinit)(void);                           /**< Called during class driver deinitialization for resource cleanup. */
	int(* setup)(usb_setup_req_t *req, u8 *buf);   /**< Called during control transfer SETUP/DATA phases to handle application-specific control requests. */
	int(* set_config)(void);                       /**< Notifies application layer that the class driver becomes operational. */
	int(* sof)(void);                              /**< Called upon SOF interrupt (GINTSTS.Sof) for timing-sensitive operations. */
	int(* transmitted)(u8 status);                 /**< Called when interrupt IN transfer done, for asynchronous interrupt IN transfer status notification . */
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
	usbd_composite_hid_buf_ctrl_t hid_priv_out;
	usbd_ep_t ep_hid_priv_in;
	usbd_ep_t ep_hid_in;
	usbd_ep_t ep_intr_out;
	usbd_composite_hid_usr_cb_t *cb;
	usbd_composite_dev_t *cdev;

	/*
	    Windows Hid Tool adds reportID for the transfer data,and the transfer len = ReportSize
	    the TV does not add the reportID, and it just report the use data,the size may < reportSize
	*/
	__IO u8 report_id_append : 1;
} usbd_composite_hid_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usbd_class_driver_t usbd_composite_hid_driver;

/* Exported functions --------------------------------------------------------*/

int usbd_composite_hid_init(usbd_composite_dev_t *cdev, usbd_composite_hid_usr_cb_t *cb);
int usbd_composite_hid_deinit(void);

/**
 * @brief Transmits HID data via interrupt IN endpoint.
 * @param[in] data: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_send_data(u8 *data, u32 len);

/**
 * @brief Transmits volume control request to USB host via interrupt IN endpoint.
 * @param[in] vol_up: 1 if volume up, else 0.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_volume_ctrl(u8 vol_up);

/**
 * @brief Transmits power control request to USB host via interrupt IN endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_power_ctrl(void);

/**
 * @brief Reads HID data from ring buffer.
 * @param[in] buffer: Pointer to the data buffer.
 * @param[in] size: Length of the data in bytes.
 * @param[in] time_out_ms:Time out.
 * @return copy len.
 */
u32 usbd_composite_hid_read(u8 *buffer, u32 size, u32 time_out_ms);

/**
 * @brief Gets queued HID frames count.
  * @retval read buffer cnt
 */
u32 usbd_composite_hid_get_read_buf_cnt(void);

/**
 * @brief Check whether the HID received ring buffer is full.
  * @retval 1 if the ring buf is full, else 0
 */
u32 usbd_composite_hid_ring_buf_is_full(void);
#endif // USBD_COMPOSITE_HID_BI_DIR_H
