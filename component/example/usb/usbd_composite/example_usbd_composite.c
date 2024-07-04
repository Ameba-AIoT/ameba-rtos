/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_composite.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "COMP";
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_COMPOSITE_HOTPLUG							1

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_FULL
#else
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_HIGH
#endif

#define CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_IN_XFER_SIZE		2048U
#define CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_OUT_XFER_SIZE	2048U
#define CONFIG_USBD_COMPOSITE_HID_HID_INTR_IN_XFER_SIZE			512U

// Thread priorities
#define CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY				5U
#define CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY				7U
#define CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY			8U // Should be higher than CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY

/* Private types -------------------------------------------------------------*/

typedef struct {
	u8 left;			//left button. 0: release, 1: press
	u8 right;			//right button. 0: release, 1: press
	u8 middle;			//wheel button. 0: release, 1: press
	char x_axis;		//x-axis pixels. relative value from -127 to 127, positive for right and negative for left
	char y_axis;		//y-axis pixels. relative value from -127 to 127, positive for up and negative for down
	char wheel;			//scrolling units. relative value from -127 to 127, positive for up and negative for down.
} composite_hid_mouse_data_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 Len);

static void composite_hid_send_device_data(void *pdata);
static int composite_hid_cb_setup(usb_setup_req_t *req, u8 *buf);

static void composite_cb_status_changed(u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t composite_cfg = {
	.speed = CONFIG_USBD_COMPOSITE_SPEED,
	.dma_enable = 1U,
	.isr_priority = CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY,
	.intr_use_ptx_fifo = 0U,
	.nptx_max_epmis_cnt = 100U,
};

static usbd_composite_cdc_acm_usr_cb_t composite_cdc_acm_usr_cb = {
	.init = composite_cdc_acm_cb_init,
	.deinit = composite_cdc_acm_cb_deinit,
	.setup = composite_cdc_acm_cb_setup,
	.received = composite_cdc_acm_cb_received
};

static usbd_composite_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

static u16 composite_cdc_acm_ctrl_line_state;

static u16 composite_hid_protocol;

static u16 composite_hid_idle_state;

static usbd_composite_hid_usr_cb_t composite_hid_usr_cb = {
	.setup = composite_hid_cb_setup,
};

static usbd_composite_cb_t composite_cb = {
	.status_changed = composite_cb_status_changed,
};

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static u8 composite_attach_status;
static rtos_sema_t composite_attach_status_changed_sema;
#endif

static u32 composite_cmd_mouse_data(u16 argc, u8 *argv[])
{
	composite_hid_mouse_data_t data;

	if (argc == 0U) {
		RTK_LOGS(TAG, "[COMP] Invalid arguments, usage:\n"
				 "mouse <left> [<right> <middle> <x_axis> <y_axis> <wheel>]\n");
		return HAL_ERR_PARA;
	}

	memset((void *)&data, 0, sizeof(data));

	if (argc > 0) {
		data.left = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
	}

	if (argc > 1) {
		data.right = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	}

	if (argc > 2) {
		data.middle = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
	}

	if (argc > 3) {
		data.x_axis = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
	}

	if (argc > 4) {
		data.y_axis = _strtoul((const char *)(argv[4]), (char **)NULL, 10);
	}

	if (argc > 5) {
		data.wheel = _strtoul((const char *)(argv[5]), (char **)NULL, 10);
	}

	RTK_LOGS(TAG, "[COMP] Send mouse data\n");

	composite_hid_send_device_data(&data);

	return HAL_OK;
}

/*exmaple cmd: mouse  0   0   0   50   0   0
	left button release,
	right button release,
	middle button release,
	x_axis: move the cursor 50 pixels to the right,
	y_axos: no movement,
	wheel: no scrolling.
*/
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE  usbd_composite_mouse_data_cmd[] = {
	{(const u8 *)"mouse",		8, composite_cmd_mouse_data,		NULL},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	lc->bitrate = 150000;
	lc->format = 0x00;
	lc->parity_type = 0x00;
	lc->data_type = 0x08;

	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	return usbd_composite_cdc_acm_transmit(buf, len);
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	switch (req->bRequest) {
	case COMP_CDC_SEND_ENCAPSULATED_COMMAND:
		/* Do nothing */
		break;

	case COMP_CDC_GET_ENCAPSULATED_RESPONSE:
		/* Do nothing */
		break;

	case COMP_CDC_SET_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_GET_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_CLEAR_COMM_FEATURE:
		/* Do nothing */
		break;

	case COMP_CDC_SET_LINE_CODING:
		if (req->wLength == COMP_CDC_ACM_LINE_CODING_SIZE) {
			lc->bitrate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->format = buf[4];
			lc->parity_type = buf[5];
			lc->data_type = buf[6];
		}
		break;

	case COMP_CDC_GET_LINE_CODING:
		buf[0] = (u8)(lc->bitrate & 0xFF);
		buf[1] = (u8)((lc->bitrate >> 8) & 0xFF);
		buf[2] = (u8)((lc->bitrate >> 16) & 0xFF);
		buf[3] = (u8)((lc->bitrate >> 24) & 0xFF);
		buf[4] = lc->format;
		buf[5] = lc->parity_type;
		buf[6] = lc->data_type;
		break;

	case COMP_CDC_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		composite_cdc_acm_ctrl_line_state = req->wValue;
		if (composite_cdc_acm_ctrl_line_state & 0x01) {
			RTK_LOGS(TAG, "[COMP] VCOM port activate\n");
		}
		break;

	case COMP_CDC_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		RTK_LOGS(TAG, "[COMP] Invalid CDC bRequest 0x%02x\n", req->bRequest);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

static int composite_hid_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;

	switch (req->bRequest) {
	case COMP_HID_SET_PROTOCOL:
		composite_hid_protocol = req->wValue;
		break;
	case COMP_HID_GET_PROTOCOL:
		buf[0] = (u8)(composite_hid_protocol & 0xFF);
		break;
	case COMP_HID_SET_REPORT:
		break;
	case COMP_HID_SET_IDLE:
		composite_hid_idle_state = req->wValue >> 8;
		break;
	case COMP_HID_GET_IDLE:
		buf[0] = (u8)(composite_hid_idle_state & 0xFF);
		break;
	default:
		RTK_LOGS(TAG, "[COMP] Invalid HID bRequest 0x%02x\n", req->bRequest);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

/*brief: send device data.(wrapper function usbd_composite_hid_send_data())*/
static void composite_hid_send_device_data(void *pdata)
{
	u8 byte[4];
	composite_hid_mouse_data_t *data = (composite_hid_mouse_data_t *)pdata;

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

	if (data->left != 0) {
		byte[0] |= COMP_HID_MOUSE_BUTTON_LEFT;
	}
	if (data->right != 0) {
		byte[0] |= COMP_HID_MOUSE_BUTTON_RIGHT;
	}
	if (data->middle != 0) {
		byte[0] |= COMP_HID_MOUSE_BUTTON_MIDDLE;
	}

	byte[0] |= COMP_HID_MOUSE_BUTTON_RESERVED;
	byte[1] = data->x_axis;
	byte[2] = data->y_axis;
	byte[3] = data->wheel;

	usbd_composite_hid_send_data(byte, 4);
}

static void composite_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, "[COMP] Status change: %d\n", status);
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	composite_attach_status = status;
	rtos_sema_give(composite_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static void composite_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(composite_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, "[COMP] DETACHED\n");
				usbd_composite_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, "[COMP] Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&composite_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_OUT_XFER_SIZE,
										  CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_IN_XFER_SIZE,
										  &composite_cdc_acm_usr_cb,
										  CONFIG_USBD_COMPOSITE_HID_HID_INTR_IN_XFER_SIZE,
										  &composite_hid_usr_cb,
										  &composite_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (composite_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, "[COMP] ATTACHED\n");
			} else {
				RTK_LOGS(TAG, "[COMP] INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, "[COMP] Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_COMPOSITE_HOTPLUG

static void example_usbd_composite_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_task_t task;
#endif

	UNUSED(param);

#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_sema_create(&composite_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_init(&composite_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_OUT_XFER_SIZE,
							  CONFIG_USBD_COMPOSITE_HID_CDC_ACM_BULK_IN_XFER_SIZE,
							  &composite_cdc_acm_usr_cb,
							  CONFIG_USBD_COMPOSITE_HID_HID_INTR_IN_XFER_SIZE,
							  &composite_hid_usr_cb,
							  &composite_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_composite_init_fail;
	}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
	ret = rtos_task_create(&task, "composite_hotplug_thread", composite_hotplug_thread, NULL,
						   1024, CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, "[COMP] USBD COMP demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_COMPOSITE_HOTPLUG
exit_create_check_task_fail:
	usbd_composite_deinit();
#endif

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, "[COMP] USBD COMP demo stop\n");
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_sema_delete(composite_attach_status_changed_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_usbd_composite(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_composite_thread", example_usbd_composite_thread, NULL, 1024, CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[COMP] Create USBD COMP thread fail\n");
	}
}

