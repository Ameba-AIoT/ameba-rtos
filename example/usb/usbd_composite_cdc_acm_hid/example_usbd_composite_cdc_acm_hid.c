/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_cdc_acm.h"
#include "usbd_hid.h"
#include "usbd_composite.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* CDC ACM endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_CDC_BULK_IN_EP                           0x82U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#elif defined (CONFIG_AMEBAL2)
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x01U
#define COMP_CDC_INTR_IN_EP                           0x82U
#else
#define COMP_CDC_BULK_IN_EP                           0x81U
#define COMP_CDC_BULK_OUT_EP                          0x02U
#define COMP_CDC_INTR_IN_EP                           0x83U
#endif

/* HID endpoint addresses */
#if defined (CONFIG_AMEBAGREEN2)
#define COMP_HID_INTR_IN_EP                           0x84U
#elif defined (CONFIG_AMEBAL2)
#define COMP_HID_INTR_IN_EP                           0x83U
#else
#define COMP_HID_INTR_IN_EP                           0x85U
#endif

#define CDC_BULK_IN_XFER_SIZE                         2048U
#define CDC_BULK_OUT_XFER_SIZE                        2048U
#define HID_INTR_IN_XFER_SIZE                         512U

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define COMP_USB_SPEED                                USB_SPEED_FULL
#else
#define COMP_USB_SPEED                                USB_SPEED_HIGH
#endif

#define HID_SET_PROTOCOL                              0x01
#define HID_GET_PROTOCOL                              0x03
#define HID_SET_REPORT                                0x09
#define HID_SET_IDLE                                  0x0A
#define HID_GET_IDLE                                  0x02

#define HID_MOUSE_BUTTON_LEFT                         0x01
#define HID_MOUSE_BUTTON_RIGHT                        0x02
#define HID_MOUSE_BUTTON_MIDDLE                       0x04
#define HID_MOUSE_BUTTON_RESERVED                     0x00

/* Private types -------------------------------------------------------------*/

typedef struct {
	u8 left;			//left button. 0: release, 1: press
	u8 right;			//right button. 0: release, 1: press
	u8 middle;			//wheel button. 0: release, 1: press
	char x_axis;		//x-axis pixels. relative value from -127 to 127, positive for right and negative for left
	char y_axis;		//y-axis pixels. relative value from -127 to 127, positive for up and negative for down
	char wheel;			//scrolling units. relative value from -127 to 127, positive for up and negative for down.
} composite_hid_mouse_data_t;

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 len);

static void composite_hid_cb_setup(void);
static u32 composite_cmd_mouse_data(u16 argc, u8 *argv[]);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static const usbd_config_t composite_cfg = {
	.speed = COMP_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 100U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 404U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 16U, },
#elif defined(CONFIG_AMEBAL2)
	.rx_fifo_depth = 645U,
	.ptx_fifo_depth = {256U, 32U, 32U, 16U, },
#elif defined(CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 256U, 32U},
#endif
};

/* CDC ACM endpoint configuration */
static const usbd_cdc_acm_ep_cfg_t cdc_acm_ep = {
	.bulk_in_addr  = COMP_CDC_BULK_IN_EP,
	.bulk_out_addr = COMP_CDC_BULK_OUT_EP,
	.intr_in_addr  = COMP_CDC_INTR_IN_EP,
	.bulk_in_xfer_size  = CDC_BULK_IN_XFER_SIZE,
	.bulk_out_xfer_size = CDC_BULK_OUT_XFER_SIZE,
};

/* HID endpoint configuration */
static const usbd_hid_ep_cfg_t hid_ep = {
	.intr_in_xfer_size  = HID_INTR_IN_XFER_SIZE,
	.intr_in_addr       = COMP_HID_INTR_IN_EP,
};

/* CDC ACM user callbacks */
static const usbd_cdc_acm_cb_t composite_cdc_acm_usr_cb = {
	.init            = composite_cdc_acm_cb_init,
	.deinit          = composite_cdc_acm_cb_deinit,
	.setup           = composite_cdc_acm_cb_setup,
	.received        = composite_cdc_acm_cb_received,
	.transmitted     = NULL,
	.status_changed  = NULL,
	.wakeup          = NULL,
};

/* HID user callbacks */
static const usbd_hid_usr_cb_t composite_hid_usr_cb = {
	.init            = NULL,
	.deinit          = NULL,
	.setup           = composite_hid_cb_setup,
	.transmitted     = NULL,
	.status_changed  = NULL,
};



static usb_cdc_acm_line_coding_t composite_cdc_acm_line_coding;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	lc->b.dwDteRate = 150000;
	lc->b.bCharFormat = 0x00;
	lc->b.bParityType = 0x00;
	lc->b.bDataBits = 0x08;

	return 0;
}

/**
  * @brief  DeInitializes the CDC ACM media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_deinit(void)
{
	return 0;
}

/**
  * @brief  Handle the CDC class control (setup) requests for the composite device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing the setup data payload
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usb_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	switch (req->bRequest) {
	case USB_CDC_ACM_SET_LINE_CODING:
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
		}
		break;

	case USB_CDC_ACM_GET_LINE_CODING:
		buf[0] = (u8)(lc->b.dwDteRate & 0xFF);
		buf[1] = (u8)((lc->b.dwDteRate >> 8) & 0xFF);
		buf[2] = (u8)((lc->b.dwDteRate >> 16) & 0xFF);
		buf[3] = (u8)((lc->b.dwDteRate >> 24) & 0xFF);
		buf[4] = lc->b.bCharFormat;
		buf[5] = lc->b.bParityType;
		buf[6] = lc->b.bDataBits;
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		if (req->wValue & 0x01) {
			/* VCOM port activate */
			USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
			usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
		}
		break;

	case USB_CDC_ACM_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		break;
	}

	return 0;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC ACM interface through this function.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	if (buf && len) {
		usbd_cdc_acm_transmit(buf, len);
	}
	return 0;
}

/**
  * @brief  Handle the HID class control (setup) requests for the composite device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing the setup data payload
  * @retval None
  */
static void composite_hid_cb_setup(void)
{
}

/*brief: send device data.(wrapper function usbd_hid_send_data())*/
static void composite_hid_send_device_data(composite_hid_mouse_data_t *data)
{
	u8 byte[4];

	memset(byte, 0, 4);

	/* mouse protocol:
		BYTE0
			|-- bit7~bit3: RSVD
			|-- bit2: middle button press
			|-- bit1: right button press
			|-- bit0: left button press
		BYTE1: x-axis value, -128~127
		BYTE2: y-axis value, -128~127
		BYTE3: wheel value, -128~127
	*/
	if (data->left) {
		byte[0] |= HID_MOUSE_BUTTON_LEFT;
	}
	if (data->right) {
		byte[0] |= HID_MOUSE_BUTTON_RIGHT;
	}
	if (data->middle) {
		byte[0] |= HID_MOUSE_BUTTON_MIDDLE;
	}

	byte[0] |= HID_MOUSE_BUTTON_RESERVED;
	byte[1] = data->x_axis;
	byte[2] = data->y_axis;
	byte[3] = data->wheel;

	usbd_hid_send_data(byte, 4);
}

static u32 composite_cmd_mouse_data(u16 argc, u8 *argv[])
{
	composite_hid_mouse_data_t data;

	if (argc == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n"
				 "mouse <left> [<right> <middle> <x_axis> <y_axis> <wheel>]\n");
		return HAL_ERR_PARA;
	}

	memset(&data, 0, sizeof(data));

	if (argc > 0) {
		data.left = _strtoul((const char *)argv[0], (char **)NULL, 10);
	}
	if (argc > 1) {
		data.right = _strtoul((const char *)argv[1], (char **)NULL, 10);
	}
	if (argc > 2) {
		data.middle = _strtoul((const char *)argv[2], (char **)NULL, 10);
	}
	if (argc > 3) {
		data.x_axis = _strtoul((const char *)argv[3], (char **)NULL, 10);
	}
	if (argc > 4) {
		data.y_axis = _strtoul((const char *)argv[4], (char **)NULL, 10);
	}
	if (argc > 5) {
		data.wheel = _strtoul((const char *)argv[5], (char **)NULL, 10);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Send mouse data\n");
	composite_hid_send_device_data(&data);

	return HAL_OK;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_composite_mouse_data_cmd[] = {
	{"mouse", composite_cmd_mouse_data},
};

/* Exported functions --------------------------------------------------------*/

void example_usbd_composite(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start\r\n");

	/* Step 1: Start USB device stack */
	if (usbd_init(&composite_cfg) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd_init failed\r\n");
		return;
	}

	/* Step 2: Initialize each class driver with its EP config */
	if (usbd_composite_cdc_acm_init(&composite_cdc_acm_usr_cb, &cdc_acm_ep) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "CDC ACM init failed\r\n");
		usbd_deinit();
		return;
	}

	if (usbd_composite_hid_init(&composite_hid_usr_cb, &hid_ep) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID init failed\r\n");
		usbd_cdc_acm_deinit();
		usbd_deinit();
		return;
	}

	/* Step 3: Start composite framework (drivers already registered via composite_init calls above) */
	if (usbd_composite_init(NULL) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Composite init failed\r\n");
		usbd_hid_deinit();
		usbd_cdc_acm_deinit();
		usbd_deinit();
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo ready\r\n");
}
