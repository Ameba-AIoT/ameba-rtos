/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_INIC_H
#define USBD_INIC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_INIC_VID									USB_VID
#define USBD_INIC_PID									USB_PID

#define USBD_INIC_LANGID_STRING							0x409
#define USBD_INIC_IDX_INTERFACE_STR						0x04U

#define USBD_INIC_ITF_NUM								1U
#define USBD_INIC_ITF_WIFI								0U

#define USBD_INIC_FS_BULK_MPS							64U   /* Full speed BULK IN & OUT maximum packet size */

#define USBD_WHC_WIFI_EP3_BULK_IN 						0x83U
#define USBD_WHC_WIFI_EP2_BULK_OUT						0x02U
#define USBD_WHC_WIFI_EP4_BULK_OUT						0x04U

#define USBD_INIC_CTRL_BUF_SIZE							512U  /* Control buffer size */
#define USBD_INIC_EP_STATE_IDLE							0U
#define USBD_INIC_EP_STATE_BUSY							1U

/* Exported types ------------------------------------------------------------*/

typedef struct {
	usbd_ep_t ep;
	void *userdata; /* userdata for each ep */
} usbd_inic_ep_t;

typedef struct {
	/* Common callbacks */
	/**
	 * @brief Called during class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called during class driver deinitialization for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Called during control transfer SETUP/DATA phases to handle application-specific control requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Notifies application layer when INIC driver becomes operational.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* set_config)(void);

	/**
	 * @brief Notifies application layer when INIC driver becomes non-operational.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* clear_config)(void);

	/**
	 * @brief Called when non-control IN transfer done, for asynchronous non-control IN transfer status notification.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] in_ep: Pointer to the INIC IN endpoint.
	 * @param[in] status: The status of the transmission.
	 */
	void(* transmitted)(usbd_inic_ep_t *in_ep, u8 status);

	/**
	 * @brief Called when non-control OUT transfer done, for application to handle the received host command/data.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] out_ep: Pointer to the INIC OUT endpoint.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* received)(usbd_inic_ep_t *out_ep, u32 len);

	/**
	 * @brief Called when the USB device status changes for application to support USB hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous USB device status.
	 * @param[in] status: The new USB device status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_inic_cb_t;

typedef struct {
	usbd_inic_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_inic_ep_t out_ep[USB_MAX_ENDPOINTS];
	usb_dev_t *dev;
	const usbd_inic_cb_t *cb;
	usb_setup_req_t ctrl_req;
	u8  bt_alt;
	u8  bt_sco_alt;
	u8  wifi_alt;
} usbd_inic_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_inic_init(const usbd_inic_cb_t *cb);
int usbd_inic_deinit(void);
int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len);
int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);
int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);

#endif  /* USBD_INIC_H */
