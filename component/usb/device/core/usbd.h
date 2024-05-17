/**
  ******************************************************************************
  * @file    usbd.h
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the API for USB device library
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBD_H
#define USBD_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"

/* Exported defines ----------------------------------------------------------*/

/* USB descriptor configurations */
#define USBD_MAX_NUM_INTERFACES			16U
#define USBD_MAX_NUM_CONFIGURATION		16U

/* USB device string descriptor index */
#define USBD_IDX_LANGID_STR				0x00U
#define USBD_IDX_MFC_STR				0x01U
#define USBD_IDX_PRODUCT_STR			0x02U
#define USBD_IDX_SERIAL_STR				0x03U
#define USBD_IDX_MS_OS_STR				0xEEU

/* USB device interrupt enable flag*/
/* GINTSTS */
#define USBD_SOF_INTR                 (BIT0) /* Start of (micro)Frame GINTSTS.bit3 */
#define USBD_EOPF_INTR                (BIT1) /* End of Periodic Frame Interrupt GINTSTS.bit15 */
#define USBD_EPMIS_INTR               (BIT2) /* Endpoint Mismatch Interrupt GINTSTS.bit17*/
#define USBD_ICII_INTR                (BIT3) /* Incomplete Isochronous IN Transfer GINTSTS.bit20*/

/* Exported types ------------------------------------------------------------*/
/* USB device bus state */
typedef enum {
	USBD_BUS_STATUS_DN       = BIT0,  // D-
	USBD_BUS_STATUS_DP    	 = BIT1,  // D+
	USBD_BUS_STATUS_SUSPEND  = BIT2,  // suspend indication
} usbd_bus_state_t;

/* USB device state */
typedef enum {
	USBD_STATE_INIT       = 0U,
	USBD_STATE_DEFAULT    = 1U,
	USBD_STATE_ADDRESSED  = 2U,
	USBD_STATE_CONFIGURED = 3U,
	USBD_STATE_SUSPENDED  = 4U,
} usbd_state_t;

/* USB device attach status */
typedef enum {
	USBD_ATTACH_STATUS_INIT     = 0U,
	USBD_ATTACH_STATUS_ATTACHED = 1U,
	USBD_ATTACH_STATUS_DETACHED = 2U
} usbd_attach_status_t;

/* USB configuration structure */
typedef struct {
	u8 speed;				/* USB speed:
							   USB_SPEED_HIGH: USB 2.0 PHY, e.g. AmebaD/AmebaSmart
							   USB_SPEED_HIGH_IN_FULL: USB 2.0 PHY in full speed mode, e.g. AmebaD/AmebaSmart
							   USB_SPEED_FULL: USB 1.1 transceiver, e.g. AmebaDPlus */
	u8 dma_enable;			/* Enable USB internal DMA mode, 0-Disable, 1-Enable */
	u8 isr_priority;		/* USB ISR thread priority */
	u8 intr_use_ptx_fifo;	/* Use Periodic TX FIFO for INTR IN transfer, only for shared TxFIFO mode */

	/* For shared FIFO mode, e.g. AmabeD, AmebaSmart and AmebaDplus, the DFIFO limitation:
			RxFIFO <= 512, default 512 if set to 0 or no specified, for all the OUT transfers
			NPTxFIFO <=256, default 256 if set to 0 or no specified, for all the non-periodic CTRL/BULK/INTR IN transfers
			PTxFIFO <= 256, default 256 if set to 0 or no specified, for periodic ISOC/INTR IN transfers
			Total DFIFO = RxFIFO + NPTxFIFO + PTxFIFO = 1016
	   Specially, if rx_fifo_depth, nptx_fifo_depth and ptx_fifo_depth are all set to 0 or not specified:
			RxFIFO = 512
			NPTxFIFO = 256
			PTxFIFO = 248   // Total DFIFO - RxFIFO - NPTxFIFO
	*/
	u32 rx_fifo_depth;		/* RX FIFO depth in dword, for shared FIFO mode */
	u32 nptx_fifo_depth;	/* Non-Periodical TX FIFO depth in dword, for shared FIFO mode */
	u32 ptx_fifo_depth;		/* Periodical TX FIFO depth in dword, for shared FIFO mode */

	u32 ext_intr_en;		/* allow class to enable some interrupts*/
	u32 nptx_max_epmis_cnt; /* Max Non-Periodical TX transfer epmis count allowed, if transfer
							   epmis count is higher than this value,the EMIPS interrupt will be handled.
							   This param works with the USB_OTG_GINTMSK_EPMISM interrupt which enable by USBD_EPMIS_INTR,
							   make sure you has configed the appropriate value,
							   a few epmis are possible and do not need to handle, it is not error
							   but when we get a lot of epmis, it is a true Endpoint Mismatch. */
	u32 nptx_max_err_cnt[USB_MAX_ENDPOINTS]; /* Max Non-Periodical TX transfer error count allowed, if transfer
							   error count is higher than this value, the transfer status will be determined as failed */
} usbd_config_t;

struct _usbd_class_driver_t;

/* USB device */
typedef struct {
	u8 dev_config;							/* Device config index */
	u8 dev_speed;							/* Device speed, usb_speed_type_t */
	u8 dev_state;							/* Device state, usbd_state_t */
	u8 dev_old_state;						/* Device old state, usbd_state_t */
	u8 dev_attach_status;					/* Device attach status, usbd_attach_status_t */

	u8 ep0_state;							/* EP0 state */
	u16 ep0_data_len;						/* EP0 data length */
	u32 ep0_xfer_total_len;					/* The total data length to transfer */
	u32 ep0_xfer_rem_len;					/* The remain data length to transfer */
	u32 ep0_recv_rem_len;					/* The remain data length to receive */

	u8 test_mode;							/* Test mode */

	u8 self_powered;						/* Self powered or not, 0-bus powered, 1-self powered */
	u8 remote_wakeup_en;					/* Remote wakeup enable or not, 0-disabled, 1-enabled */
	u8 remote_wakeup;						/* Remote wakeup */

	u8 *ctrl_buf;							/* Buffer for control transfer */

	void *pcd;								/* PCD handle */
	struct _usbd_class_driver_t *driver;	/* Class driver */
} usb_dev_t;

/* USB class driver */
typedef struct _usbd_class_driver_t {
	u8 *(*get_descriptor)(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);

	int(*set_config)(usb_dev_t *dev, u8 config);
	int(*clear_config)(usb_dev_t *dev, u8 config);
	int(*setup)(usb_dev_t *dev, usb_setup_req_t *req);

	int(*sof)(usb_dev_t *dev);
	int(*suspend)(usb_dev_t *dev);
	int(*resume)(usb_dev_t *dev);

	int(*ep0_data_in)(usb_dev_t *dev, u8 status);
	int(*ep0_data_out)(usb_dev_t *dev);

	int(*ep_data_in)(usb_dev_t *dev, u8 ep_addr, u8 status);
	int(*ep_data_out)(usb_dev_t *dev, u8 ep_addr, u16 len);

	void (*status_changed)(usb_dev_t *dev, u8 status);
} usbd_class_driver_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* API for application */
int usbd_init(usbd_config_t *cfg);
int usbd_deinit(void);
int usbd_get_status(void);
int usbd_get_bus_status(u32 *status);
int usbd_wake_host(void);

/* API for class */
int usbd_register_class(usbd_class_driver_t *driver);
int usbd_unregister_class(void);
int usbd_ep_init(usb_dev_t *dev, u8 ep_addr, u8 ep_type, u16 ep_mps);
int usbd_ep_deinit(usb_dev_t *dev, u8 ep_addr);
int usbd_ep_transmit(usb_dev_t *dev, u8 ep_addr, u8 *buf, u16  len);
int usbd_ep_receive(usb_dev_t *dev, u8 ep_addr, u8 *buf, u16  len);
int usbd_ep_set_stall(usb_dev_t *dev, u8 ep_addr);
int usbd_ep_clear_stall(usb_dev_t *dev, u8 ep_addr);
int usbd_ep_is_stall(usb_dev_t *dev, u8 ep_addr);
int usbd_ep0_transmit(usb_dev_t *dev, u8 *buf, u16 len);
int usbd_ep0_receive(usb_dev_t *dev, u8 *buf, u16 len);
int usbd_ep0_transmit_status(usb_dev_t *dev);
int usbd_ep0_receive_status(usb_dev_t *dev);
int usbd_ep0_set_stall(usb_dev_t *dev);
void usbd_get_str_desc(const char *str, u8 *desc, u16 *len);

#endif /* USBD_H */

