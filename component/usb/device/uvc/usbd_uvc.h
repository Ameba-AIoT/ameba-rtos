/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UVCD_H
#define USBD_UVCD_H

/* Includes ------------------------------------------------------------------*/
#include "dlist.h"
#include "usbd_video.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_UVC_FPS(fps) (10000000/fps)

#define USBD_UVC_FORMAT_TYPE_YUY2        0
#define USBD_UVC_FORMAT_TYPE_NV12        1
#define USBD_UVC_FORMAT_TYPE_MJPEG       2
#define USBD_UVC_FORMAT_TYPE_H264        3
#define USBD_UVC_FORMAT_TYPE_H265        4

#define USBD_UVC_INTF_CONTROL        0
#define USBD_UVC_INTF_STREAMING      1

#define USBD_UVC_EVENT_FIRST			0
#define USBD_UVC_EVENT_CONNECT			1
#define USBD_UVC_EVENT_DISCONNECT		2
#define USBD_UVC_EVENT_STREAMON			3
#define USBD_UVC_EVENT_STREAMOFF		4
#define USBD_UVC_EVENT_SETUP			5
#define USBD_UVC_EVENT_DATA				6
#define USBD_UVC_EVENT_LAST				7

#define USBD_UVC_CMD_THREAD_PRIORITY	2

#define USBD_UVC_NUM_REQUESTS			2
#define USBD_UVC_MAX_REQUEST_SIZE		64
#define USBD_UVC_MAX_EVENTS				4

//Tuning tool
#define USBD_UVC_VendorCommand   	0X0A
#define USBD_UVC_VendorData      	0X0B
#define USBD_UVC_VendorLength    	0X08
#define USBD_UVC_UserDefineCommand  	0X0E
#define USBD_UVC_UserDownloadData   	0X0F
#define USBD_UVC_UserDefineLength   	0X40


#define USBD_UVC_WEBCAM_VENDOR_ID		USB_VID
#define USBD_UVC_WEBCAM_PRODUCT_ID		USB_PID
#define USBD_UVC_WEBCAM_DEVICE_BCD		0x0010
#define USBD_UVC_LANGID_STRING 0x0409U

#define USBD_UVC_MFG_STRING                       "Realtek"
#define USBD_UVC_MFG_HS_STRING               	  "USB UVC CLASS"
#define USBD_UVC_MFG_FS_STRING                    "USB UVC CLASS"
#define USBD_UVC_SN_STRING                        "1234567890"

#define USBD_UVC_MAKE_SUBCMD(data,dir,subcmd) ((u8)(data)<<7|(u8)(dir)<<6|subcmd)

#define USBD_UVC_STRING_CONTROL_IDX			0
#define USBD_UVC_STRING_STREAMING_IDX		1

#define USBD_UVC_INTF_VIDEO_CONTROL			0
#define USBD_UVC_INTF_VIDEO_STREAMING		1

#define USBD_UVC_STATUS_MAX_PACKET_SIZE		64	/* 16 bytes status */

#define USBD_UVC_IN_BUF_SIZE 1024//1000//768//512//256

#define USBD_UVC_ISO_IN_EP                       0x83U
/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief UVC frame information structure.
 * @details
 * Describes a single video frame size and its supported frame intervals.
 */
typedef  struct {
	unsigned int width;
	unsigned int height;
	unsigned int intervals[4];
} usbd_uvc_frame_info_t;

/**
 * @brief UVC video format information.
 * @details
 * Defines a video format and its supported frame resolutions.
 */
typedef  struct {
	unsigned int fcc;
	usbd_uvc_frame_info_t *frames;
} usbd_uvc_format_info_t;

/**
 * @brief UVC vendor-specific command structure.
 * @details
 * Used for vendor-defined control or data transfer commands over USB.
 */
typedef  struct {
	unsigned char cmd;
	unsigned char subcmd;
	unsigned short address;
	unsigned short length;
	unsigned short reserved;
	unsigned char buffer[8];
} usbd_uvc_vendorcmd_t;

/**
 * @brief UVC Processing Unit control data.
 * @details
 * Stores control values returned by standard UVC GET requests
 * (e.g. brightness, contrast, saturation).
 */
typedef  struct {
	int cur;//0X81
	int min;//0X82
	int max;//0X83
	int res;//0X84
	int len;//0X85
	int info;//0X86
	int def;//0X87
} usbd_uvc_process_unit_t;

/**
 * @brief ISP USB device command data structure.
 * @details
 * Used for communication between USB and ISP firmware logic.
 */
typedef  struct {
	u16 cmdcode;
	u8 index;
	u8 length;
	u16 param;
	u16 addr;
	u8 buf[0x40];//The maximum buffer size
	u32 offset;//to recode the index for read and write
} usbd_uvc_isp_usbd_cmd_data_t;

/**
 * @brief UVC request payload data.
 * @details
 * Used to store data associated with a UVC control request.
 */
typedef  struct {
	int length;
	u8 data[64];
} usbd_uvc_request_data_t;

/**
 * @brief USB UVC video buffer structure.
 * @details
 * Represents a single video buffer used for streaming.
 */
typedef  struct {
	struct list_head buffer_list;
	unsigned char *mem;
	unsigned int bytesused;
	void *exbuf;
	int index;
} usbd_uvc_buffer_t;

/**
 * @brief UVC request data container.
 * @details
 * Wraps USB setup requests and UVC-specific request payloads.
 */
typedef  struct {
	u32				type;
	union {
		usb_setup_req_t req;
		usbd_uvc_request_data_t uvc_data;
		unsigned char buf[64];
	};
	struct list_head	list;
} usbd_uvc_req_data_t;

/**
 * @brief UVC video streaming context.
 * @details
 * Holds all runtime information related to UVC video streaming,
 * including format, buffers, payload handling, and synchronization.
 */
typedef  struct {
	usbd_uvc_buffer_t uvc_buffer;
	int stream_index;
	/* Frame parameters */
	u8 bpp;
	u32 fcc;
	unsigned int width;
	unsigned int height;
	unsigned int imagesize;
	unsigned int frm_type;

	/* Requests */
	unsigned int req_size;

	/* Context data used by the completion handler */
	u32 payload_size;
	u32 max_payload_size;

	//struct uvc_video_queue queue;
	unsigned int fid;
	unsigned int format;
	unsigned int buf_used;
	unsigned int end_flag;

	// payload list
	struct list_head input_queue;
	struct list_head output_queue;
	usb_os_lock_t input_lock;
	usb_os_lock_t output_lock;
	usb_os_sema_t output_queue_sema;
	usb_os_sema_t output_frame_sema;
	usb_os_queue_t complete_bf_req;
	void *complete_bf_task;
} usbd_uvc_video_t;

/**
 * @brief UVC device context structure.
 * @details
 * Represents the complete UVC device state, including control,
 * streaming, USB endpoints, and synchronization primitives.
 */
typedef  struct {
	usbd_uvc_video_t video;

	usbd_uvc_streaming_control_t probe;
	usbd_uvc_streaming_control_t commit;

	u16 interface_number;
	u8 config;

	usbd_uvc_req_data_t req_data;
	/* Events */
	unsigned int event_length;
	unsigned int event_setup_out : 1;
	int control;		   //control selector
	int command_interface; //Interface and entiny 0x00 control 0x01 streaming &0xff
	int command_entity;	   //0x02 process unit 0x03 extension unit >>8&0xff
	unsigned char result[64];
	void (*change_parm_cb)(void *);
	usb_dev_t *dev;

	usb_os_sema_t uvc_Cmd_wakeup_sema;
	u32 uvc_Cmd_blocked;
	usb_os_lock_t  bod_mutex;
	struct list_head	bod_list;
	usb_os_lock_t lock;
	u32 running;
	u8  ctrl_req;
	u8  ctrl_data_len;
	u8 *uvc_in_buf;
	usbd_ep_t ep_isoc_in;
	rtos_queue_t uvc_cmd_queue;
	u32 frame_done;
} usbd_uvc_dev_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


extern usbd_uvc_streaming_control_t usbd_uvc_probe;
extern usbd_uvc_streaming_control_t usbd_uvc_commit;
extern usbd_uvc_format_info_t uvcd_formats[];
extern u8 usbd_uvc_descriptors[];
extern int usbd_uvc_descriptors_size;

void usbd_uvc_events_process(usbd_uvc_dev_t *dev, usbd_uvc_req_data_t *uvc_event);
usbd_uvc_dev_t *get_private_usbd_uvcd(void);
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload);
void usbd_uvc_video_put_in_stream_queue(usbd_uvc_buffer_t *payload);
usbd_uvc_buffer_t *usbd_uvc_video_out_stream_queue(void);
int usbd_uvc_init(void);
int usbd_uvc_uvcd_get_status(void);
void usbd_uvc_uvcd_wait_frame_down(void);
int usbd_uvc_parameter_init(void);
void usbd_uvc_deinit(void);
#endif /* USBD_UVC_H */