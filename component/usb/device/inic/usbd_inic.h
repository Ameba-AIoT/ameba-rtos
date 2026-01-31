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

#define USBD_INIC_HS_BULK_MPS							512U  /* High speed BULK IN & OUT maximum packet size */
#define USBD_INIC_FS_BULK_MPS							64U   /* Full speed BULK IN & OUT maximum packet size */
#define USBD_INIC_HS_INTR_MPS							16U   /* High speed INTR IN & OUT maximum packet size */
#define USBD_INIC_FS_INTR_MPS							16U   /* Full speed INTR IN & OUT maximum packet size */

#define USBD_INIT_BT_EP1_INTR_IN						0x81U
#define USBD_INIC_BT_EP2_BULK_IN						0x82U
#define USBD_INIC_BT_EP2_BULK_OUT						0x02U
#define USBD_WHC_WIFI_EP4_BULK_IN 						0x84U
#define USBD_WHC_WIFI_EP5_BULK_OUT						0x05U
#define USBD_WHC_WIFI_EP6_BULK_OUT						0x06U
#define USBD_WHC_WIFI_EP7_BULK_OUT						0x07U

#define USBD_INIC_CTRL_BUF_SIZE							512U  /* Control buffer size */

#define USBD_INIC_EP_STATE_IDLE							0U
#define USBD_INIC_EP_STATE_BUSY							1U

// Vendor requests
#define USBD_INIC_VENDOR_REQ_BT_HCI_CMD					0x00U
#define USBD_INIC_VENDOR_REQ_FW_DOWNLOAD				0xF0U
#define USBD_INIC_VENDOR_QUERY_CMD						0x01U
#define USBD_INIC_VENDOR_QUERY_ACK						0x81U
#define USBD_INIC_VENDOR_RESET_CMD						0x02U
#define USBD_INIC_VENDOR_RESET_ACK						0x82U

#define USBD_INIC_FW_TYPE_APPLICATION					0xF2U

#define USBD_INIC_RESET_THREAD_PRIORITY					6

/* Exported types ------------------------------------------------------------*/

typedef struct {
	// DWORD 0
	u32	data_len: 16;		// Data payload length
	u32	data_offset: 8;		// Data payload offset i.e. header length
	u32	data_checksum: 8;	// Checksum of the data payload

	// DWORD 1
	u32	pkt_type: 8;		// Packet type
	u32	xfer_status: 8;		// Xfer status
	u32	rl_version: 8;		// RL Version
	u32	dev_mode: 8;		// Device mode

	// DWORD 2
	u32	mem_addr;			// Memory address

	// DWORD 3
	u32	mem_size;			// Memory size

	// DWORD 4
	union {
		u32	d32;
		u16	d16[2];
		u8	d8[4];
	} value;				// Target value

	// DWORD 5
	u32	reserved;
} __PACKED usbd_inic_query_packet_t;

typedef struct {
	usbd_ep_t ep;
	void *userdata; // userdata for each ep
} usbd_inic_ep_t;

/**
 * @brief INIC user callback structure.
 * @details This structure holds pointers to user-defined callback functions
 *          that are invoked on various INIC events.
 */
typedef struct {
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
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Notifies application layer when INIC driver becomes operational.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* set_config)(void);

	/**
	 * @brief Notifies application layer when INIC driver becomes non-operational.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* clear_config)(void);

	/**
	 * @brief Called when non-control IN transfer done, for asynchronous non-control IN transfer status notification.
	 * @param[in] in_ep: Pointer to the INIC IN endpoint.
	 * @param[in] status: The status of the transmission.
	 */
	void(* transmitted)(usbd_inic_ep_t *in_ep, u8 status);

	/**
	 * @brief Called when non-control OUT transfer done, for application to handle the received host command/data.
	 * @param[in] out_ep: Pointer to the INIC OUT endpoint.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* received)(usbd_inic_ep_t *out_ep, u32 len);

	/**
	 * @brief Called when the USB device status changes for application to support USB hot-plug events.
	 * @param[in] old_status: The previous USB device status.
	 * @param[in] status: The new USB device status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_inic_cb_t;

typedef struct {
	usbd_inic_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_inic_ep_t out_ep[USB_MAX_ENDPOINTS];
	usbd_otp_t otp;
	usb_dev_t *dev;
	usbd_inic_cb_t *cb;
	usb_setup_req_t ctrl_req;
	rtos_task_t reset_task;
	rtos_sema_t reset_sema;
	u8  bt_alt;
	u8  bt_sco_alt;
	u8  wifi_alt;
} usbd_inic_dev_t;

/* Exported macros -----------------------------------------------------------*/

#define	USBD_INIC_QUERY_PACKET_SIZE			(sizeof(usbd_inic_query_packet_t))

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
* @brief Initializes class driver with application callback handler.
* @param[in] cb: Pointer to the user-defined callback structure.
* @return 0 on success, non-zero on failure.
*/
int usbd_inic_init(usbd_inic_cb_t *cb);

/**
 * @brief DeInitialize HID device
 * @return 0 on success, non-zero on failure.
 */
int usbd_inic_deinit(void);

/**
 * @brief Transmits control IN data
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_inic_transmit_ctrl_data(u8 *buf, u16 len);

/**
 * @brief Transmits data to the host.
 * @param[in] ep_addr: Endpoint address.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @param[in] userdata: userdata from application for each endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_inic_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);

/**
 * @brief Prepares to receive non-control OUT data.
 * @param[in] ep_addr: Endpoint address.
 * @param[in] buf: Pointer to the data buffer.
 * @param[in] len: Length of the data in bytes.
 * @param[in] userdata: userdata from application for each endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_inic_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);

/**
 * @brief Checks whether the BT is enbaled.
 * @return
 *        - 1: BT is enbaled.
 *        - 0: BT is disabled.
 */
u8 usbd_inic_is_bt_en(void);
#endif  /* USBD_INIC_H */

