/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_WHC_H
#define USBD_WHC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_whc_otp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @defgroup Device_WHC_Constants Device WHC Constants
 *  @brief Identity defaults, endpoint sizing and firmware type codes of the WHC class.
 *  @{
 */

#define USBD_WHC_VID                     USB_VID
#define USBD_WHC_PID                     USB_PID
#define USBD_NIC_PID                     0x8006U  /**< NIC mode PID (used to patch device descriptor at runtime). */

#define USBD_WHC_MFG_STRING              "Realtek"               /**< Manufacturer string. */
#define USBD_WHC_PROD_STRING             "802.11ax WLAN Adapter" /**< Product string. */
#define USBD_WHC_SN_STRING               "00E04C000001"          /**< Serial number string. */
#define USBD_WHC_BT_STRING               "Bluetooth Radio"       /**< Bluetooth interface string. */
#ifdef CONFIG_WHC_ETH
#define USBD_WHC_ETH_STRING              "Ethernet Adapter"      /**< Ethernet interface string. */
#endif

#define USBD_WHC_HS_BULK_MPS             512U  /* High speed BULK IN & OUT maximum packet size */
#define USBD_WHC_FS_BULK_MPS             64U   /* Full speed BULK IN & OUT maximum packet size */
#ifdef CONFIG_WHC_ETH
/* High speed Ethernet BULK IN maximum packet size.
Per USB 2.0 spec §5.8.3, the wMaxPacketSize of high-speed BULK IN EP should be 512, otherwise USB-IF Chapter 9 / USB3CV will fail. */
#define USBD_WHC_ETH_HS_IN_MPS            512U
#endif
#define USBD_WHC_HS_INTR_MPS             16U   /* High speed INTR IN & OUT maximum packet size */
#define USBD_WHC_FS_INTR_MPS             16U   /* Full speed INTR IN & OUT maximum packet size */

/* Endpoint addresses.
 *
 * The macro names describe the function and the transfer type only, never the
 * endpoint address, so that the upper layers (WHC, BT) can be shared by SoCs
 * whose address assignment differs. This header is the only place to change
 * when the assignment changes.
 * The WiFi BULK OUT pipes are equivalent, they are numbered from 1 up to
 * USBD_WHC_WIFI_BULK_OUT_EP_NUM.
 */
#define USBD_WHC_BT_EP_INTR_IN            0x81U
#define USBD_WHC_BT_EP_BULK_OUT           0x02U
#ifdef CONFIG_WHC_ETH
/* Ethernet reuses the BT SCO ISOC OUT EP as its BULK OUT. Its BULK IN takes the BT BULK IN EP, whose TxFIFO
 * holds two HS packets for ping-pong, while BT BULK IN moves to an EP whose TxFIFO holds one HS packet, enough for
 * the BT throughput. Each IN EP owns a dedicated TxFIFO, so the IN EP number sets the FIFO depth. */
#define USBD_WHC_BT_EP_BULK_IN            0x86U
#define USBD_WHC_ETH_EP_BULK_OUT          0x03U
#define USBD_WHC_ETH_EP_BULK_IN           0x82U
#else
#define USBD_WHC_BT_EP_BULK_IN            0x82U
/* BT SCO ISOC EPs, only available when Ethernet is disabled, EP3 is taken by Ethernet otherwise */
#define USBD_WHC_BT_EP_ISOC_IN            0x83U
#define USBD_WHC_BT_EP_ISOC_OUT           0x03U
#endif
#define USBD_WHC_WIFI_EP_BULK_IN          0x84U
#define USBD_WHC_WIFI_EP_BULK_OUT_1       0x05U
#define USBD_WHC_WIFI_EP_BULK_OUT_2       0x06U
#define USBD_WHC_WIFI_EP_BULK_OUT_3       0x07U
#define USBD_WHC_WIFI_BULK_OUT_EP_NUM     3     /* Number of WiFi BULK OUT pipes */

/* Interface class/subclass/protocol triples
 *
 * BT uses the USB-IF assigned Wireless Controller / RF Controller / Bluetooth
 * Programming Interface triple. The others are vendor specific (class 0xFF),
 * for which the USB spec leaves subclass and protocol to the vendor:
 *   bInterfaceSubClass = 0x00 : Realtek WHC function family
 *   bInterfaceProtocol        : function id within the family
 *       0x00      - reserved, invalid
 *       0x01      - WiFi
 *       0x02      - Ethernet
 *       0x03~0xFF - reserved for future functions
 * The host driver matches an interface by this triple, so keep it in sync with
 * the host side.
 */
#define USBD_WHC_BT_ITF_CLASS            0xE0U
#define USBD_WHC_BT_ITF_SUBCLASS         0x01U
#define USBD_WHC_BT_ITF_PROTOCOL         0x01U
#define USBD_WHC_WIFI_ITF_CLASS          0xFFU
#define USBD_WHC_WIFI_ITF_SUBCLASS       0x00U
#define USBD_WHC_WIFI_ITF_PROTOCOL       0x01U
#define USBD_WHC_ETH_ITF_CLASS           0xFFU
#define USBD_WHC_ETH_ITF_SUBCLASS        0x00U
#define USBD_WHC_ETH_ITF_PROTOCOL        0x02U

/* Vendor requests */
#define USBD_WHC_VENDOR_REQ_BT_HCI_CMD   0x00U
#define USBD_WHC_VENDOR_REQ_FW_DOWNLOAD  0xF0U
#define USBD_WHC_VENDOR_QUERY_CMD        0x01U
#define USBD_WHC_VENDOR_QUERY_ACK        0x81U
#define USBD_WHC_VENDOR_RESET_CMD        0x02U
#define USBD_WHC_VENDOR_RESET_ACK        0x82U

#define USBD_WHC_FW_TYPE_APPLICATION     0xF2U

/** @} End of Device_WHC_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 *  @{
 */
/** @defgroup Device_WHC_Types Device WHC Types
 *  @brief Packet header, endpoint, callback and device context types of the WHC class.
 *  @{
 */

typedef struct {
	/* DWORD 0 */
	u32	data_len: 16;		/* Data payload length */
	u32	data_offset: 8;		/* Data payload offset i.e. header length */
	u32	data_checksum: 8;	/* Checksum of the data payload */

	/* DWORD 1 */
	u32	pkt_type: 8;		/* Packet type */
	u32	xfer_status: 8;		/* Xfer status */
	u32	rl_version: 8;		/* RL Version */
	u32	dev_mode: 8;		/* Device mode */

	/* DWORD 2 */
	u32	mem_addr;			/* Memory address */

	/* DWORD 3 */
	u32	mem_size;			/* Memory size */

	/* DWORD 4 */
	union {
		u32	d32;
		u16	d16[2];
		u8	d8[4];
	} value;				/* Target value */

	/* DWORD 5 */
	u32	reserved;
} __PACKED usbd_whc_query_packet_t;

typedef struct {
	void *userdata; /* userdata for each ep */
	usbd_ep_t ep;
} usbd_whc_ep_t;

/**
 * @brief WHC user callback structure.
 * @details This structure holds pointers to user-defined callback functions
 *          that are invoked on various WHC events.
 */
typedef struct {
	/**
	 * @brief Called during class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called during class driver deinitialization for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*deinit)(void);

	/**
	 * @brief Called during control transfer SETUP/DATA phases to handle application-specific control requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Notifies application layer when WHC driver becomes operational.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*set_config)(void);

	/**
	 * @brief Notifies application layer when WHC driver becomes non-operational.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*clear_config)(void);

	/**
	 * @brief Called when non-control IN transfer done, for asynchronous non-control IN transfer status notification.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] in_ep: Pointer to the WHC IN endpoint.
	 * @param[in] status: The status of the transmission.
	 */
	void (*transmitted)(usbd_whc_ep_t *in_ep, int status);

	/**
	 * @brief Called when non-control OUT transfer done, for application to handle the received host command/data.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] out_ep: Pointer to the WHC OUT endpoint.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*received)(usbd_whc_ep_t *out_ep, u32 len);

	/**
	 * @brief Called when the USB device status changes for application to support USB hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous USB device status.
	 * @param[in] status: The new USB device status.
	 */
	void (*status_changed)(u8 old_status, u8 status);

	/**
	 * @brief Called when the USB device resumes from suspend (wakeup).
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @details Indicates that the USB bus is active again and the upper layer can resume normal TRX.
	 */
	void (*wakeup)(void);
} usbd_whc_cb_t;

typedef struct {
	usb_dev_t *dev;
	const usbd_whc_cb_t *cb;
	rtos_task_t reset_task;
	rtos_sema_t reset_sema;
	usbd_whc_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_whc_ep_t out_ep[USB_MAX_ENDPOINTS];
	usbd_otp_t otp;
	usb_setup_req_t ctrl_req;
	u8 ctrl_req_pending; /* 1 if ctrl_req is waiting for its EP0 OUT data stage */
	u8 bt_str_idx;       /* BT string index from usbd_add_string(), 0 if none */
#ifdef CONFIG_WHC_ETH
	u8 eth_str_idx;      /* Ethernet string index from usbd_add_string(), 0 if none */
#endif
#ifndef CONFIG_WHC_ETH
	u8  bt_sco_alt; /* BT SCO is the only interface which owns alternate settings */
#endif
} usbd_whc_dev_t;

/** @} End of Device_WHC_Types group */
/** @} End of USB_Device_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_WHC_Functions Device WHC Functions
 *  @brief Lifecycle and transfer operations of the WHC class.
 *  @{
 */

/**
* @brief Initializes class driver with application callback handler.
* @param[in] cb: Pointer to the user-defined callback structure.
* @return 0 on success, non-zero on failure.
*/
int usbd_whc_init(const usbd_whc_cb_t *cb);

/**
 * @brief DeInitialize HID device
 * @return None. This is a teardown path: the class is always unregistered and every resource it
 *         owns released, so there is nothing for the caller to recover from.
 */
void usbd_whc_deinit(void);

/**
 * @brief Transmits control IN data
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_whc_transmit_ctrl_data(u8 *buf, u16 len);

/**
 * @brief Transmits data to the host.
 * @param[in] ep_addr: Endpoint address.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @param[in] userdata: userdata from application for each endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_whc_transmit_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);

/**
 * @brief Prepares to receive non-control OUT data.
 * @note  The controller writes whole packets, so the DMA window is usb_get_dma_len(len, MPS),
 *        which exceeds len when len is not a multiple of the MPS. The buffer shall hold that
 *        window: the class does not report its capacity, so the core cannot reject or clamp an
 *        oversized OUT transfer and a host sending more than len bytes overruns a smaller buffer.
 * @param[in] ep_addr: Endpoint address.
 * @param[in] buf: Pointer to the data buffer, at least usb_get_dma_len(len, MPS) bytes.
 * @param[in] len: Length of the data in bytes.
 * @param[in] userdata: userdata from application for each endpoint.
 * @return 0 on success, non-zero on failure.
 */
int usbd_whc_receive_data(u8 ep_addr, u8 *buf, u32 len, void *userdata);

/**
 * @brief Checks whether the BT is enbaled.
 * @return
 *        - 1: BT is enbaled.
 *        - 0: BT is disabled.
 */
u8 usbd_whc_is_bt_en(void);

/** @} End of Device_WHC_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_WHC_H */
