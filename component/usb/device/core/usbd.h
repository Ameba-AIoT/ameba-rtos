/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_H
#define USBD_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"
#include "usb_hal.h"

/* Exported defines ----------------------------------------------------------*/
/* This define used to debug the isr time issue */
#define USBD_ISR_TASK_TIME_DEBUG		0U

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
#define USBD_SOF_INTR                 (BIT0) /* Start of (micro)Frame, GINTSTS.Sof */
#define USBD_EOPF_INTR                (BIT1) /* End of Periodic Frame Interrupt, GINTSTS.EOPF */
#define USBD_EPMIS_INTR               (BIT2) /* Endpoint Mismatch Interrupt, GINTSTS.EPMis*/

/* Exported types ------------------------------------------------------------*/

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

/* USB ep structure */
typedef struct {
	u8 *xfer_buf;               /* Pointer to transfer buffer */
	u8 *rem_xfer_buf;           /* Pointer to remain transfer buffer */
	u32 xfer_len;               /* transfer data length */
	u32 rem_xfer_len;           /* remain transfer len, only for ep0 in driver */
	u32 xfer_buf_len;           /* class transfer buffer len */
	u16 mps;                    /* Endpoint Max packet size, 0~64KB */
	u8 addr;                    /* Endpoint address */
	u8 num;                     /* Endpoint number */
	u8 type;                    /* Endpoint type, USB_EP_Type_XXX */
	u8 binterval;               /* Endpoint binterval */
	__IO u8 tx_zlp : 1;         /* class tz ZLP packet or not */
	__IO u8 dis_zlp : 1;        /* class xfer ZLP enable or disable */
	__IO u8 is_busy : 1;        /* class is busy or not */
	__IO u8 xfer_state : 1;     /* class xfer state */
} usbd_ep_t;

/* USB configuration structure */
typedef struct {
	u32 nptx_max_err_cnt[USB_MAX_ENDPOINTS]; /* Max Non-Periodical TX transfer error count allowed, if transfer
								error count is higher than this value, the transfer status will be determined as failed */
	u32 nptx_max_epmis_cnt;		/* Max Non-Periodical TX transfer epmis count allowed, if epmis count is higher than this
								value, the GINTSTS.EPMis interrupt will be handled. This parameter is enabled only when
								USBD_EPMIS_INTR is enabled in ext_intr_en. */
	u32 ext_intr_en;			/* Enable extra interrupts:
								USBD_SOF_INTR: used for timing synchronization with SOF.
								USBD_EOPF_INTR: used to toggle frame parity for ISOC transfers, only for slave mode.
								USBD_EPMIS_INTR: used to re-activate the transfers of multiple non-periodic endpoints when
								Endpoint Mismatch Interrupt happens, only for shared FIFO mode.	*/
	u16 rx_fifo_depth;			/* Only for dedicated FIFO mode, RxFIFO depth in size of dword. */
	u16 ptx_fifo_depth[USB_MAX_ENDPOINTS - 1]; /* Only for dedicated FIFO mode. Depth of TxFIFO n# (for n=1; n<OTG_NUM_IN_EPS) in size of dword,
								where tx_fifo_depth[n] is for TxFIFO n+1 normally used by IN EP n+1, specially for AmebaGreen2,
								tx_fifo_depth[4] is for IN EP6. TxFIFO 0# depth in not user configurable in dedicated FIFO mode. */
	u8 speed;					/* USB speed:
							   	USB_SPEED_HIGH: USB 2.0 PHY, e.g. AmebaD/AmebaSmart.
							   	USB_SPEED_HIGH_IN_FULL: USB 2.0 PHY in full speed mode, e.g. AmebaD/AmebaSmart.
							   	USB_SPEED_FULL: USB 1.1 transceiver, e.g. AmebaDPlus. */
	u8 isr_priority;			/* USB ISR priority */
	u8 isr_in_critical : 1;		/* Process USB ISR in critical state. */
	u8 dma_enable : 1;			/* Enable USB internal DMA mode, 0-Disable, 1-Enable. */
	u8 intr_use_ptx_fifo : 1;	/* Use Periodic TX FIFO for INTR IN transfer, only for shared TxFIFO mode. */
} usbd_config_t;

struct _usbd_class_driver_t;

/* USB device */
typedef struct {
	usbd_ep_t ep0_in;
	usbd_ep_t ep0_out;
	struct _usbd_class_driver_t *driver;	/* Class driver */
	u32 ep0_out_intr;						/* EP0 old out interrupt */
	u16 ep0_data_len;						/* EP0 data length */
#if USBD_ISR_TASK_TIME_DEBUG
	__IO u32 isr_func_time_cost_max;
	__IO u32 isr_func_time_cost;
	__IO u32 isr_trigger_time_diff_max;
	__IO u32 isr_trigger_time_diff;
	u32 isr_trigger_last_time;
#endif
	void *pcd;								/* PCD handle */
	u8 ep0_state;							/* EP0 state */
	u8 ep0_old_state;						/* EP0 old state */
	u8 dev_config;							/* Device config index */
	u8 dev_speed;							/* Device speed, usb_speed_type_t */
	u8 dev_state;							/* Device state, usbd_state_t */
	u8 dev_old_state;						/* Device old state, usbd_state_t */
	u8 dev_attach_status;					/* Device attach status, usbd_attach_status_t */
	u8 dev_old_attach_status;				/* Device old attach status, usbd_attach_status_t */
	u8 test_mode;							/* Test mode */
	u8 self_powered : 1;						/* Self powered or not, 0-bus powered, 1-self powered */
	u8 remote_wakeup_en : 1;					/* Remote wakeup enable or not, 0-disabled, 1-enabled */
	u8 remote_wakeup : 1;						/* Remote wakeup */
	u8 is_ready : 1;							/* Device ready or not, 0-disabled, 1-enabled */
} usb_dev_t;

/* USB class driver */
typedef struct _usbd_class_driver_t {
	u16(*get_descriptor)(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
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

	void (*status_changed)(usb_dev_t *dev, u8 old_status, u8 status);
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
int usbd_register_class(const usbd_class_driver_t *driver);
int usbd_unregister_class(void);
int usbd_ep_init(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_deinit(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_transmit(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_receive(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_set_stall(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_clear_stall(usb_dev_t *dev, usbd_ep_t *ep);
int usbd_ep_is_stall(usb_dev_t *dev, usbd_ep_t *ep);
u16 usbd_get_str_desc(const char *str, u8 *desc);

#endif /* USBD_H */

