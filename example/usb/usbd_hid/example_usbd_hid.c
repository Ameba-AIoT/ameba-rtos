/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd_hid.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// Endpoint address
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define HID_INTR_IN_EP                           0x82U
#else
#define HID_INTR_IN_EP                           0x81U
#endif
#define HID_INTR_OUT_EP                          0x02U

// Hotplug: enables a thread to check hotplug event and reset USB stack to
// avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define HID_HOTPLUG                              1

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define HID_USB_SPEED                            USB_SPEED_FULL
#else
#define HID_USB_SPEED                            USB_SPEED_HIGH
#endif

// Send mouse data through monitor
#define HID_MOUSE_CMD                            1

// Send hid device data table. Once connected to PC, cursor of PC will do
// process according to array mdata[].
#define HID_CONSTANT_DATA                        1
#define HID_CONSTANT_LOOP                        10

// hid_send_device_data() is referenced by the mouse command and by the constant
// data test only
#if HID_CONSTANT_DATA || (HID_MOUSE_CMD && defined(CONFIG_USBD_HID_MOUSE))
#define HID_TX_EN                                1
#else
#define HID_TX_EN                                0
#endif

// Thread priorities
#define HID_INIT_THREAD_PRIORITY                 5
#define HID_HOTPLUG_THREAD_PRIORITY              8
#define HID_XFER_THREAD_PRIORITY                 5

// Thread stack sizes
#define HID_INIT_THREAD_STACK_SIZE               1024U
#define HID_HOTPLUG_THREAD_STACK_SIZE            1024U
#define HID_XFER_THREAD_STACK_SIZE               1024U

// Bounded waits, a hotplug in the middle of a transfer/enumeration means the
// corresponding callback never comes, never block forever on it
#define HID_TX_TIMEOUT_MS                        1000U
#define HID_CONNECT_TIMEOUT_MS                   1000U

/* Private types -------------------------------------------------------------*/

typedef struct {
	u8 left;			//left button. 0: release, 1: press
	u8 right;			//right button. 0: release, 1: press
	u8 middle;			//wheel button. 0: release, 1: press
	char x_axis;		//x-axis pixels. relative value from -127 to 127, positive for right and negative for left
	char y_axis;		//y-axis pixels. relative value from -127 to 127, positive for up and negative for down
	char wheel;			//scrolling units. relative value from -127 to 127, positive for up and negative for down.
} usbd_hid_mouse_data_t;

typedef struct {
	u8 func_cmd;			//left & right : alt win shift alt command
	u8 padding;				//constant panding
	u8 cmd[6];				//6 command key values
} usbd_hid_keyboard_data_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void hid_cb_init(void);
static void hid_cb_deinit(void);
static void hid_cb_setup(void);
static void hid_cb_transmitted(u8 status);
#ifdef CONFIG_USBD_HID_KEYBOARD
static void hid_cb_received(u8 *buf, u32 len);
#endif
static void hid_cb_status_changed(u8 old_status, u8 status);

#if HID_MOUSE_CMD
#ifdef CONFIG_USBD_HID_MOUSE
static u32 hid_cmd_mouse_data(u16 argc, u8  *argv[]);
#endif
#endif

#if HID_TX_EN
static int hid_send_device_data(const void *data);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

/* Serializes the USB stack bring up/tear down against the TX path, taken by
 * the hotplug thread, the xfer thread and the mouse command */
static rtos_mutex_t hid_state_mutex;
/* 1: usbd_init() and usbd_hid_init() both done, TX allowed */
static volatile u8 hid_stack_ready;

#if HID_HOTPLUG
static u8 hid_attach_status;
static rtos_sema_t hid_attach_status_changed_sema;
/* 1: hotplug re-init failed, all example threads shall quit */
static volatile u8 hid_stack_fatal;
/* Increased on every successful (re)init, identifies one test session */
static volatile u32 hid_session_id;
#endif

#if HID_CONSTANT_DATA
static rtos_sema_t hid_connect_sema;
static rtos_sema_t hid_transmit_sema;
#ifdef CONFIG_USBD_HID_MOUSE
static const usbd_hid_mouse_data_t mdata[] = {
	{0,   0,   0,  50,   0,   0},	//move the cursor 50 pixels to the right
	{0,   0,   0,   0,  50,   0},	//move the cursor down 50 pixels
	{0,   0,   0, -50,   0,   0},	//move the cursor 50 pixels to the left
	{0,   0,   0,   0, -50,   0},	//move the cursor up 50 pixels
	{0,   0,   0,   0,   0,   5},	//scroll up for 5 units
	{0,   0,   0,   0,   0,  -5},	//scroll down for 5 units
	{0,   0,   1,   0,   0,   0},	//middle button pressed
	{0,   0,   0,   0,   0,   0},	//middle button released
	{0,   1,   0,   0,   0,   0},	//right button pressed
	{0,   0,   0,   0,   0,   0},	//right button released
	{0,   0,   0,  -5,   0,   0},	//move the cursor 5 pixels to the left
	{1,   0,   0,   0,   0,   0},	//left button pressed
	{0,   0,   0,   0,   0,   0},	//left button released
};
#else
usbd_hid_keyboard_data_t mdata[] = {
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//all released

	{0,   0,   {4,	0,	0,	0,	0,	0} },	//enter a pressed
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//enter a released

	{2,   0,   {4,	0,	0,	0,	0,	0} },	//left Shift pressed + enter a pressed = A
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//left Shift & a released
};
#endif
#endif

#if HID_MOUSE_CMD
#ifdef CONFIG_USBD_HID_MOUSE
/*exmaple cmd: mouse  0   0   0   50   0   0
	left button release,
	right button release,
	middle button release,
	x_axis: move the cursor 50 pixels to the right,
	y_axos: no movement,
	wheel: no scrolling.
*/
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_hid_mouse_data_cmd[] = {
	{"mouse", hid_cmd_mouse_data},
};
#endif
#endif  //HID_MOUSE_CMD

static const usbd_config_t hid_cfg = {
	.speed = HID_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
	.rx_fifo_depth = 724U,
	.ptx_fifo_depth = {0U, 256U, 0U, 0U, 0U},
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 677U,
	.ptx_fifo_depth = {256U, 16U, 16U, 16U},
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1680U,
	.ptx_fifo_depth = {256U, 16U, 16U, },
#endif
};

static const usbd_hid_ep_cfg_t hid_ep = {
	.intr_in_xfer_size = USBD_HID_INTR_IN_BUF_SIZE,
	.intr_in_addr  = HID_INTR_IN_EP,
#ifdef CONFIG_USBD_HID_KEYBOARD
	.intr_out_addr = HID_INTR_OUT_EP,
#endif
};

static const usbd_hid_usr_cb_t hid_usr_cb = {
	.init = hid_cb_init,
	.deinit = hid_cb_deinit,
	.setup = hid_cb_setup,
	.transmitted = hid_cb_transmitted,
#ifdef CONFIG_USBD_HID_KEYBOARD
	.received = hid_cb_received,
#endif
	.status_changed = hid_cb_status_changed,
};

/* Private functions ---------------------------------------------------------*/

#if HID_TX_EN || HID_CONSTANT_DATA
/**
  * @brief  Check whether the example shall abort
  * @retval 1 if the USB stack can not be recovered any more else 0
  */
static u8 hid_stack_broken(void)
{
#if HID_HOTPLUG
	return hid_stack_fatal;
#else
	return 0U;
#endif
}
#endif

#if HID_CONSTANT_DATA
/**
  * @brief  Check whether the running test session is still the valid one
  * @param  session: Session id recorded when the test started
  * @retval 1 if a hotplug happened or the stack is broken else 0
  */
static u8 hid_session_changed(u32 session)
{
#if HID_HOTPLUG
	return ((hid_stack_fatal != 0U) || (hid_session_id != session)) ? 1U : 0U;
#else
	UNUSED(session);
	return 0U;
#endif
}
#endif // HID_CONSTANT_DATA

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the last running thread, after the USB stack is fully
  *         deinited, so that no ISR callback can touch these objects any more
  * @retval None
  */
static void hid_free_resource(void)
{
#if HID_CONSTANT_DATA
	rtos_sema_delete(hid_connect_sema);
	rtos_sema_delete(hid_transmit_sema);
	hid_connect_sema = NULL;
	hid_transmit_sema = NULL;
#endif
#if HID_HOTPLUG
	rtos_sema_delete(hid_attach_status_changed_sema);
	hid_attach_status_changed_sema = NULL;
#endif
	rtos_mutex_delete(hid_state_mutex);
	hid_state_mutex = NULL;
}

static void hid_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
}

static void hid_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

/**
  * @brief  Handle HID class control (setup) requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @retval None
  */
static void hid_cb_setup(void)
{
#if HID_CONSTANT_DATA
	rtos_sema_give(hid_connect_sema);
#endif
}

/**
  * @brief  Notify completion of an HID IN transfer
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  status: Transfer completion status
  * @retval None
  */
static void hid_cb_transmitted(u8 status)
{
	UNUSED(status);
#if HID_CONSTANT_DATA
	rtos_sema_give(hid_transmit_sema);
#endif
}

#ifdef CONFIG_USBD_HID_KEYBOARD
/**
  * @brief  Data received over USB HID OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval None
  */
static void hid_cb_received(u8 *buf, u32 len)
{
	UNUSED(buf);
	UNUSED(len);
	if (len > 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "RX %d byte(s): 0x%02x\n", len, buf[0]);
	}
}
#endif

/**
  * @brief  Handle HID attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void hid_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if HID_HOTPLUG
	hid_attach_status = status;
	rtos_sema_give(hid_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

#if HID_MOUSE_CMD
#ifdef CONFIG_USBD_HID_MOUSE
static u32 hid_cmd_mouse_data(u16 argc, u8  *argv[])
{
	usbd_hid_mouse_data_t data;

	if (argc == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n"
				 "mouse <left> [<right> <middle> <x_axis> <y_axis> <wheel>]\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset((void *)&data, 0, sizeof(data));

	if (argc > 0) {
		data.left = strtoul((const char *)(argv[0]), (char **)NULL, 10);
	}

	if (argc > 1) {
		data.right = strtoul((const char *)(argv[1]), (char **)NULL, 10);
	}

	if (argc > 2) {
		data.middle = strtoul((const char *)(argv[2]), (char **)NULL, 10);
	}

	if (argc > 3) {
		data.x_axis = strtoul((const char *)(argv[3]), (char **)NULL, 10);
	}

	if (argc > 4) {
		data.y_axis = strtoul((const char *)(argv[4]), (char **)NULL, 10);
	}

	if (argc > 5) {
		data.wheel = strtoul((const char *)(argv[5]), (char **)NULL, 10);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Send mouse data\n");

	return (u32)hid_send_device_data(&data);
}
#endif
#endif  //HID_MOUSE_CMD

#if HID_TX_EN
/**
  * @brief  Send device data (wrapper function of usbd_hid_send_data())
  * @note   The stack status check and the transfer share one critical section,
  *         otherwise the hotplug thread could free the endpoint buffers between
  *         the check and the transfer
  * @param  pdata: Mouse or keyboard data to send
  * @retval Result of the operation: HAL_OK if success else fail
  */
static int hid_send_device_data(const void *pdata)
{
	int ret;
#ifdef CONFIG_USBD_HID_MOUSE
	u8 byte[4];
	const usbd_hid_mouse_data_t *data = (const usbd_hid_mouse_data_t *)pdata;

	usb_os_memset((void *)byte, 0, 4);

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
		byte[0] |= USBD_HID_MOUSE_BUTTON_LEFT;
	}
	if (data->right != 0) {
		byte[0] |= USBD_HID_MOUSE_BUTTON_RIGHT;
	}
	if (data->middle != 0) {
		byte[0] |= USBD_HID_MOUSE_BUTTON_MIDDLE;
	}

	byte[0] |= USBD_HID_MOUSE_BUTTON_RESERVED;
	byte[1] = data->x_axis;
	byte[2] = data->y_axis;
	byte[3] = data->wheel;

#else /* CONFIG_USBD_HID_KEYBOARD */
	const usbd_hid_keyboard_data_t *data = (const usbd_hid_keyboard_data_t *)pdata;
#endif

	/* The stack is gone and hid_state_mutex is freed, refuse to touch it */
	if (hid_stack_broken() != 0U) {
		return HAL_ERR_HW;
	}

	rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
	if (hid_stack_ready == 0U) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Stack not ready, skip TX\n");
		ret = HAL_ERR_HW;
	} else {
#ifdef CONFIG_USBD_HID_MOUSE
		ret = usbd_hid_send_data((const u8 *)byte, 4);
#else
		ret = usbd_hid_send_data((const u8 *)data, 8);
#endif
	}
	rtos_mutex_give(hid_state_mutex);

	return ret;
}
#endif // HID_TX_EN

#if HID_HOTPLUG
static void example_usbd_hid_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(hid_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		if (hid_attach_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");

			/* Close the gate before tearing down. hid_send_device_data()
			 * contends for the same mutex, so no TX is in flight here and any
			 * later one sees hid_stack_ready == 0 and gives up */
			rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
			hid_stack_ready = 0U;
			usbd_hid_deinit();
			usbd_deinit();
			rtos_mutex_give(hid_state_mutex);

			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
			ret = usbd_init(&hid_cfg);
			if (ret == 0) {
				ret = usbd_hid_init(&hid_usr_cb, &hid_ep);
				if (ret != 0) {
					usbd_deinit();
				}
			}
			if (ret == 0) {
#if HID_CONSTANT_DATA
				/* Drop the counts left by the last session and rebuild the TX
				 * flow control initial state */
				while (rtos_sema_take(hid_transmit_sema, 0) == RTK_SUCCESS) {
					/* drain */
				}
				while (rtos_sema_take(hid_connect_sema, 0) == RTK_SUCCESS) {
					/* drain */
				}
				rtos_sema_give(hid_transmit_sema);
#endif
				/* Open the gate at the very last step, so that the xfer thread
				 * never runs before the re-init is fully done */
				hid_session_id++;
				hid_stack_ready = 1U;
			}
			rtos_mutex_give(hid_state_mutex);

			if (ret != 0) {
				break;
			}
		} else if (hid_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
		}
	}

	/* The stack is fully deinited here, no more ISR callback. Notify the xfer
	 * thread to quit, it frees the shared objects as the last thread standing */
	hid_stack_fatal = 1U;
#if !HID_CONSTANT_DATA
	hid_free_resource();
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // HID_HOTPLUG

#if HID_CONSTANT_DATA
static void example_usbd_hid_xfer_thread(void *param)
{
	u32 i;
	u32 session = 0;
	u32 delaytime;
	u8 array_len;
	int loop;

	UNUSED(param);

#ifdef CONFIG_USBD_HID_MOUSE
	array_len = sizeof(mdata) / sizeof(usbd_hid_mouse_data_t);
	delaytime = 1000;
#else
	array_len = sizeof(mdata) / sizeof(usbd_hid_keyboard_data_t);
	delaytime = 50;
#endif

	for (;;) {
		/* Wait for a usable session: stack inited and enumerated by the host */
		while ((hid_stack_broken() == 0U)
			   && ((hid_stack_ready == 0U) || (usbd_get_status() != USBD_ATTACH_STATUS_ATTACHED))) {
			rtos_time_delay_ms(100);
		}
		if (hid_stack_broken() != 0U) {
			break;
		}

#if HID_HOTPLUG
		session = hid_session_id;
#endif
		if (rtos_sema_take(hid_connect_sema, HID_CONNECT_TIMEOUT_MS) != RTK_SUCCESS) {
			continue; /* Not configured by the host yet, re-check the state */
		}

#ifdef CONFIG_USBD_HID_MOUSE
		RTK_LOGS(TAG, RTK_LOG_INFO, "Mouse data TX test start\n");
#else
		RTK_LOGS(TAG, RTK_LOG_INFO, "Keyboard data TX test start\n");
#endif

		for (loop = 0; loop < HID_CONSTANT_LOOP; loop++) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Test round %d/%d\n", loop + 1, HID_CONSTANT_LOOP);
			for (i = 0; i < array_len; i++) {
				/* A hotplug aborts the round, the stack may be down or under
				 * re-init, no TX allowed until the new session shows up */
				if (hid_session_changed(session) != 0U) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Test aborted by hotplug\n");
					goto session_end;
				}
				if (rtos_sema_take(hid_transmit_sema, HID_TX_TIMEOUT_MS) != RTK_SUCCESS) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "TX timeout\n");
					goto session_end;
				}
				if (hid_send_device_data(&mdata[i]) != HAL_OK) {
					goto session_end;
				}
				rtos_time_delay_ms(delaytime);
			}
			rtos_time_delay_ms(5 * 1000); //next loop
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Test done\n");

session_end:
#if HID_HOTPLUG
		/* Idle until the hotplug thread finishes the next re-init, the test is
		 * restarted only on a brand new session */
		while ((hid_stack_fatal == 0U) && (hid_session_id == session)) {
			rtos_time_delay_ms(500);
		}
		if (hid_stack_fatal != 0U) {
			break;
		}
#else
		break;
#endif
	}

#if HID_HOTPLUG
	if (hid_stack_fatal != 0U) {
		/* The hotplug thread already deinited the stack and quit, free the
		 * shared objects here as the last thread standing */
		hid_free_resource();
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Xfer thread abort\n");
	}
#endif
	rtos_task_delete(NULL);
}
#endif // HID_CONSTANT_DATA

static void example_usbd_hid_thread(void *param)
{
	int ret;
#if HID_HOTPLUG
	rtos_task_t hotplug_task;
#endif
#if HID_CONSTANT_DATA
	rtos_task_t xfer_task;
#endif

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID demo start\n");

	ret = rtos_mutex_create(&hid_state_mutex);
	if (ret != RTK_SUCCESS) {
		goto exit_create_obj_fail;
	}

#if HID_CONSTANT_DATA
	ret = rtos_sema_create(&hid_connect_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_create_obj_fail;
	}

	ret = rtos_sema_create(&hid_transmit_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_create_obj_fail;
	}
	rtos_sema_give(hid_transmit_sema);
#endif

#if HID_HOTPLUG
	ret = rtos_sema_create(&hid_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_create_obj_fail;
	}
#endif

	ret = usbd_init(&hid_cfg);
	if (ret != 0) {
		goto exit_create_obj_fail;
	}

	ret = usbd_hid_init(&hid_usr_cb, &hid_ep);
	if (ret != 0) {
		goto exit_usbd_init_fail;
	}

#if HID_HOTPLUG
	hid_session_id = 1U;
#endif
	hid_stack_ready = 1U;

#if HID_HOTPLUG
	ret = rtos_task_create(&hotplug_task, "usbd_hid_hotplug_thread",
						   example_usbd_hid_hotplug_thread, NULL,
						   HID_HOTPLUG_THREAD_STACK_SIZE, HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_hid_init_fail;
	}
#endif // HID_HOTPLUG

#if HID_CONSTANT_DATA
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_hid_xfer_thread",
						   example_usbd_hid_xfer_thread, NULL,
						   HID_XFER_THREAD_STACK_SIZE, HID_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_hotplug_task_fail;
	}
#endif

	/* Device stays enumerated, the hotplug thread and the xfer thread own the
	 * shared objects from now on and free them if the stack can not recover */
	rtos_task_delete(NULL);
	return;

#if HID_CONSTANT_DATA
exit_create_hotplug_task_fail:
#if HID_HOTPLUG
	rtos_task_delete(hotplug_task);
#endif
#endif

#if HID_HOTPLUG
exit_usbd_hid_init_fail:
#endif
	hid_stack_ready = 0U;
	usbd_hid_deinit();

exit_usbd_init_fail:
	usbd_deinit();

exit_create_obj_fail:
	hid_free_resource();
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD HID demo aborted\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_hid(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_hid_thread", example_usbd_hid_thread, NULL,
						   HID_INIT_THREAD_STACK_SIZE, HID_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD HID thread fail\n");
	}
}
