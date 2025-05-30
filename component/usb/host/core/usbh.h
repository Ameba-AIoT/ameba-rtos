/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_H
#define USBH_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"
#include "usb_hal.h"

/* Exported defines ----------------------------------------------------------*/
#define USBH_DEFAULT_MAX_ALT_NUM			3
#define USBH_MAX_CLASSES_NUM				1


/* USB Host interrupt enable flag*/
/* GINTSTS */
#define USBH_SOF_INTR                 (BIT0) /* Start of (micro)Frame GINTSTS.bit3 */

/* Exported types ------------------------------------------------------------*/

/* USB host state */
typedef enum {
	USBH_IDLE = 0U,
	USBH_WAIT_DEV_ATTACH,
	USBH_DEV_ATTACHED,
	USBH_DEV_DETACHED,
	USBH_ENUMERATION,
	USBH_CLASS_REQUEST,
	USBH_USER_SET_CONFIG,
	USBH_SET_CONFIG,
	USBH_SET_WAKEUP_FEATURE,
	USBH_CLASS_PROBE,
	USBH_CLASS_READY,
	USBH_ABORTED,
} usbh_state_t;

/* URB state */
typedef enum {
	USBH_URB_IDLE = 0U,
	USBH_URB_DONE,
	USBH_URB_BUSY,
	USBH_URB_ERROR,
	USBH_URB_STALL
} usbh_urb_state_t;

/* USB message */
typedef enum {
	USBH_MSG_USER_SET_CONFIG = 0U,
	USBH_MSG_CONNECTED,
	USBH_MSG_DISCONNECTED,
	USBH_MSG_ERROR,
} usbh_msg_t;

/* USB device descriptor */
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 bcdUSB;        									/* USB specification version which device complies to */
	u8  bDeviceClass;									/* Class code (assigned by USB Org) */
	u8  bDeviceSubClass;								/* Subclass code (assigned by USB Org) */
	u8  bDeviceProtocol;								/* If equal to 0, each interface specifies its own class
							   								code if equal to 0xFF, the class code is vendor specified.
							   								Otherwise field is valid class code.*/
	u8  bMaxPacketSize;
	u16 idVendor;           							/* Vendor ID (assigned by USB Org) */
	u16 idProduct;          							/* Product ID (assigned by manufacturer) */
	u16 bcdDevice;          							/* Device release number */
	u8  iManufacturer;      							/* Index of manufacturer string descriptor */
	u8  iProduct;           							/* Index of product string descriptor */
	u8  iSerialNumber;      							/* Index of serial number string descriptor */
	u8  bNumConfigurations; 							/* Number of possible configurations */
} usbh_dev_desc_t;

/* USB endpoint descriptor */
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bEndpointAddress;								/* Endpoint address */
	u8  bmAttributes;									/* Specifies the transfer type. */
	u16 wMaxPacketSize;									/* Maximum packet size this endpoint is capable of sending or receiving */
	u8  bInterval;										/* The polling interval of certain transfers. */
} usbh_ep_desc_t;

/* USB interface descriptor */
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bInterfaceNumber;								/* Interface number */
	u8  bAlternateSetting;								/* Value used to select alternative setting */
	u8  bNumEndpoints;									/* Number of endpoints used for this interface */
	u8  bInterfaceClass;								/* Class code (assigned by USB Org) */
	u8  bInterfaceSubClass;								/* Subclass code (assigned by USB Org) */
	u8  bInterfaceProtocol;								/* Protocol code */
	u8  iInterface;										/* Index of string descriptor describing this interface */
	usbh_ep_desc_t *ep_desc_array;							/* Endpoint descriptors */
} usbh_if_desc_t;

/* USB configuration descriptor */
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u16 wTotalLength;									/* Total length of Data Returned */
	u8  bNumInterfaces;									/* Interface number */
	u8  bConfigurationValue;							/* Value to use as an argument to select this configuration*/
	u8  iConfiguration;									/* Index of string descriptor describing this configuration */
	u8  bmAttributes;									/* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0)*/
	u8  bMaxPower;										/* Maximum power consumption */
	usbh_if_desc_t **if_desc_array;						/* Interface descriptors save all interfaces infor in this struct
							   								the interface is index by bInterfaceNumber & bAlternateSetting
							   								:bInterfaceNumber define the main interface Id
							   								:bAlternateSetting define the alternate setting Id.*/
} usbh_cfg_desc_t;

/* USB setup request */
typedef union {
	u32 d32[2];
	usb_setup_req_t b;
} usbh_setup_req_t;

/* USB user configuration */
typedef struct {
	u32 ext_intr_en;                                    /* allow class to enable some interrupts*/

	u16 rx_fifo_depth;                                  /* RxFIFO depth in size of dword*/
	u16 nptx_fifo_depth;                                /* npTxFIFO depth in size of dword*/
	u16 ptx_fifo_depth;                                 /* pTxFIFO depth in size of dword*/

	u8 isr_priority;									/* USB ISR priority */
	u8 isr_task_priority;								/* USB ISR thread priority */
	u8 main_task_priority;								/* USB main thread priority */

	u8 alt_max;											/* USB support max alt setting num */

	u8 speed : 2;										/* USB speed, USB_SPEED_HIGH, USB_SPEED_HIGH_IN_FULL or USB_SPEED_LOW 0~3*/
	u8 dma_enable : 1;									/* Enable USB internal DMA mode, 0-Disable, 1-Enable */

	/* 	used for get the usb host tick
		if sof_tick_en = 1, usbh_get_tick will return the tick which support by sof interrupt(should enable sof interrupt)
		if sof_tick_en = 0, usbh_get_tick will return the tick which got from the timestamp
	*/
	u8 sof_tick_en : 1;
} usbh_config_t;

struct _usb_host_t;

/* USB class driver */
typedef struct {
	u8 class_code;										/* Class code assigned by USB Org */
	int(*attach)(struct _usb_host_t *host);				/* Called after set configuration */
	int(*detach)(struct _usb_host_t *host);				/* Called when device disconnected */
	int(*setup)(struct _usb_host_t *host);				/* Called after class attached to process class standard control requests */
	int(*process)(struct _usb_host_t *host, u32 msg);		/* Called after class setup to process class specific transfers */
	int(*sof)(struct _usb_host_t *host);					/* Called at SOF interrupt */
	int(*nak)(struct _usb_host_t *host, u8 pipe_num);		/* Called at NAK interrupt of specific pipe */
} usbh_class_driver_t;

/* USB host user callback */
typedef struct {
	int(*process)(struct _usb_host_t *host, u8 id);		/* Allow usesr to handle class-independent events in application level */
} usbh_user_cb_t;

/* USB host */
typedef struct _usb_host_t {
	usbh_class_driver_t  *class_driver[USBH_MAX_CLASSES_NUM];	/* Class drivers */
	usbh_config_t         config;          				/* User configuration  */

	usbh_class_driver_t  *active_class_driver;			/* Active class driver */
	usbh_user_cb_t       *cb;							/* Host user callback */
	void                 *private;						/* Host private data */

	u32                   class_num;					/* Registered class number */
	__IO u32              tick;							/* Host timer tick */
	__IO usbh_state_t     state;						/* Host state */
} usb_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/*
 * API for application
 */

/* Init with user configuration and callback */
int usbh_init(usbh_config_t *cfg, usbh_user_cb_t *cb);
/* De-init */
int usbh_deinit(void);
/* Re-enumerate */
int usbh_reenumerate(void);
/* Get device connection status: 0 - Disconnected 1 - Connected */
int usbh_get_status(void);

/*
 * API for class
 */

/* Register class driver */
int  usbh_register_class(usbh_class_driver_t *driver);
/* Un-Register class driver */
int  usbh_unregister_class(usbh_class_driver_t *driver);

/* Config operations */
/* Set the config index while bNumConfigurations > 1 */
int usbh_set_configuration(usb_host_t *host, u8 cfg_id);
u8 usbh_get_configuration(usb_host_t *host, u8 subclass);

/* Pipe operations */
int usbh_alloc_pipe(usb_host_t *host, u8 ep_addr); /* 0xFF means no available pipe */
int usbh_free_pipe(usb_host_t *host, u8 pipe_num);
int usbh_open_pipe(usb_host_t *host, u8 pipe_num, u8 ep_num, u8 ep_type, u16 mps);
int usbh_close_pipe(usb_host_t *host, u8 pipe_num);
/* Get ep type in a specific pipe */
u8 usbh_get_ep_type(usb_host_t *host, u8 pipe_num);
/* Reactivate the request in a specific pipe */
int usbh_reactivate_pipe(usb_host_t *host, u8 pipe_num);

/* Interface operations */
u8 usbh_get_interface(usb_host_t *host, u8 class_code, u8 sub_class_code, u8 protocol); /* 0xFF means interface not found */
int usbh_set_interface(usb_host_t *host, u8 if_num);
usbh_if_desc_t *usbh_get_interface_descriptor(usb_host_t *host, u8 if_num, u8 alt_num);

/* Get the interval value */
u32 usbh_get_interval(usb_host_t *host, u8 ep_type, u8 binterval);

/* Get the tick difference */
u32 usbh_get_elapsed_ticks(usb_host_t *host, u32 start_tick);

/* Get current tick count, based SOF */
u32 usbh_get_tick(usb_host_t *host);
/* Get current timestamp in ms*/
u32 usbh_get_timestamp(usb_host_t *host);

/* Get raw configuration descriptor data */
u8 *usbh_get_raw_configuration_descriptor(usb_host_t *host);

/* Get device descriptor data */
usbh_dev_desc_t *usbh_get_device_descriptor(usb_host_t *host);

/* Get device speed */
u32 usbh_get_device_speed(usb_host_t *host);

/* Get current sof number */
u32 usbh_get_current_frame(usb_host_t *host);

/* Toggle operations */
int usbh_set_toggle(usb_host_t *host, u8 pipe_num, u8 toggle);
u8 usbh_get_toggle(usb_host_t *host, u8 pipe_num);

/* Get URB state */
usbh_urb_state_t usbh_get_urb_state(usb_host_t *host, u8 pipe_num);

/* Notify host core that class state has been changed */
void usbh_notify_class_state_change(usb_host_t *host, u32 param);
/* Notify host core that URB state has been changed */
void usbh_notify_urb_state_change(usb_host_t *host, u32 param);

/* Transfer operations */
int usbh_ctrl_set_interface(usb_host_t *host, u8 if_num, u8 if_alt);
int usbh_ctrl_set_feature(usb_host_t *host, u8 value);
int usbh_ctrl_clear_feature(usb_host_t *host, u8 ep_num);
int usbh_ctrl_request(usb_host_t *host, usbh_setup_req_t *req, u8 *buf);
int usbh_bulk_receive_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
int usbh_bulk_send_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
int usbh_intr_receive_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
int usbh_intr_send_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
int usbh_isoc_receive_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
int usbh_isoc_send_data(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);
u32 usbh_get_last_transfer_size(usb_host_t *host, u8 pipe);
u32 usbh_get_dev_address(void);

/* Usbh CTS test operations */
int usbh_enter_suspend(u8 suspend);
int usbh_port_test_ctrl(u8 type);

#endif /* USBD_H */

