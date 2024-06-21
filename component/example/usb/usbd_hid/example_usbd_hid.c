/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd.h"
#include "usbd_hid.h"
#include "os_wrapper.h"
#include "example_usbd_hid.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "HID";
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_HID_HOTPLUG						1

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_HID_SPEED						USB_SPEED_FULL
#else
#define CONFIG_USBD_HID_SPEED						USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_HID_INIT_THREAD_PRIORITY		5U
#define CONFIG_USBD_HID_ISR_THREAD_PRIORITY			7U
#define CONFIG_USBD_HID_HOTPLUG_THREAD_PRIORITY		8U // Should be higher than CONFIG_USBD_HID_ISR_THREAD_PRIORITY

// Send mouse data through monitor.
#define CONFIG_USBD_HID_MOUSE_CMD					1

// Send hid device data table. Once connected to PC, cursor of PC will do process according to array mdata[].
#define CONFIG_USBD_HID_CONSTANT_DATA				1
#define CONFIG_USBD_HID_CONSTANT_LOOP				10

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
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
static void hid_cb_received(u8 *buf, u32 len);
#endif
static void hid_cb_status_changed(u8 status);

#if CONFIG_USBD_HID_MOUSE_CMD
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
static u32 hid_cmd_mouse_data(u16 argc, u8  *argv[]);
#endif
#endif

static void hid_send_device_data(void *data);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t hid_connect_sema;
static rtos_sema_t hid_transmit_sema;

#if CONFIG_USBD_HID_HOTPLUG
static u8 hid_attach_status;
static rtos_sema_t hid_attach_status_changed_sema;
#endif

#if CONFIG_USBD_HID_CONSTANT_DATA
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
static usbd_hid_mouse_data_t mdata[] = {
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

#if CONFIG_USBD_HID_MOUSE_CMD
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
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
	{(const u8 *)"mouse",		8, hid_cmd_mouse_data,		NULL},
};
#endif
#endif  //CONFIG_USBD_HID_MOUSE_CMD

static usbd_config_t hid_cfg = {
	.speed = CONFIG_USBD_HID_SPEED,
	.dma_enable = 1U,
	.isr_priority = CONFIG_USBD_HID_ISR_THREAD_PRIORITY,
};

static usbd_hid_usr_cb_t hid_usr_cb = {
	.init = hid_cb_init,
	.deinit = hid_cb_deinit,
	.setup = hid_cb_setup,
	.transmitted = hid_cb_transmitted,
#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
	.received = hid_cb_received,
#endif
	.status_changed = hid_cb_status_changed,
};

/* Private functions ---------------------------------------------------------*/

static void hid_cb_init(void)
{
	RTK_LOGS(TAG, "[HID] INIT\n");
}

static void hid_cb_deinit(void)
{
	RTK_LOGS(TAG, "[HID] DEINIT\n");
}

static void hid_cb_setup(void)
{
	rtos_sema_give(hid_connect_sema);
}

static void hid_cb_transmitted(u8 status)
{
	UNUSED(status);

	rtos_sema_give(hid_transmit_sema);
}

#if USBD_HID_DEVICE_TYPE == USBD_HID_KEYBOARD_DEVICE
static void hid_cb_received(u8 *buf, u32 len)
{
	UNUSED(buf);
	u32 i = 0;
	RTK_LOGS(TAG, "[HID] RX %dB\n", len);
	for (i = 0; i < len ; i++) {
		RTK_LOGS(TAG, " 0x%x ", buf[i]);
		if ((i + 1) % 10 == 0) {
			RTK_LOGS(TAG, "\n");
		}
	}
}
#endif

static void hid_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, "[HID] Status change: %d\n", status);
#if CONFIG_USBD_HID_HOTPLUG
	hid_attach_status = status;
	rtos_sema_give(hid_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_HID_MOUSE_CMD
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
static u32 hid_cmd_mouse_data(u16 argc, u8  *argv[])
{
	usbd_hid_mouse_data_t data;

	if (argc == 0U) {
		RTK_LOGS(TAG, "[HID] Invalid arguments, usage:\n"
				 "mouse <left> [<right> <middle> <x_axis> <y_axis> <wheel>]\n");
		return HAL_ERR_PARA;
	}

	memset((void *)&data, 0, sizeof(data));

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

	RTK_LOGS(TAG, "[HID] Send mouse data\n");

	hid_send_device_data(&data);

	return HAL_OK;
}
#endif
#endif  //CONFIG_USBD_HID_MOUSE_CMD

/*brief: send device data.(wrapper function usbd_hid_send_data())*/
static void hid_send_device_data(void *pdata)
{
#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	u8 byte[4];
	usbd_hid_mouse_data_t *data = (usbd_hid_mouse_data_t *)pdata;

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

	usbd_hid_send_data(byte, 4);
#else
	usbd_hid_keyboard_data_t *data = (usbd_hid_keyboard_data_t *)pdata;
	usbd_hid_send_data((u8 *)data, 8);
#endif
}

#if CONFIG_USBD_HID_HOTPLUG
static void hid_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(hid_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if (hid_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, "[HID] DETACHED\n");
				usbd_hid_deinit();
				usbd_deinit();

				rtos_time_delay_ms(100);
				RTK_LOGS(TAG, "[HID] Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

				ret = usbd_init(&hid_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_hid_init(512, &hid_usr_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (hid_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, "[HID] ATTACHED\n");
			} else {
				RTK_LOGS(TAG, "[HID] INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, "[HID] Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_HID_HOTPLUG

static void example_usbd_hid_thread(void *param)
{
	int ret = 0;
	u32 i = 0;
	u32 delaytime = 0;
	u8 array_len = 0 ;
	int loop = 0;
#if CONFIG_USBD_HID_HOTPLUG
	rtos_task_t task;
#endif

	UNUSED(param);
	RTK_LOGS(TAG, "[HID] USBD HID demo start\n");

	rtos_sema_create(&hid_connect_sema, 0U, 1U);
	rtos_sema_create(&hid_transmit_sema, 0U, 1U);
	rtos_sema_give(hid_transmit_sema);
#if CONFIG_USBD_HID_HOTPLUG
	rtos_sema_create(&hid_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_init(&hid_cfg);
	if (ret != 0) {
		goto example_usbd_hid_device_thread_fail;
	}

	ret = usbd_hid_init(512, &hid_usr_cb);
	if (ret != 0) {
		usbd_deinit();
		goto example_usbd_hid_device_thread_fail;
	}

#if CONFIG_USBD_HID_HOTPLUG
	ret = rtos_task_create(&task, "hid_hotplug_thread", hid_hotplug_thread, NULL, 1024U, CONFIG_USBD_HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		usbd_hid_deinit();
		usbd_deinit();
		goto example_usbd_hid_device_thread_fail;
	}
#endif // CONFIG_USBD_HID_HOTPLUG

	while (usbd_get_status() != USBD_ATTACH_STATUS_ATTACHED) {
		rtos_time_delay_ms(100);
	}

#if CONFIG_USBD_HID_CONSTANT_DATA

	rtos_sema_take(hid_connect_sema, RTOS_SEMA_MAX_COUNT);

#if USBD_HID_DEVICE_TYPE == USBD_HID_MOUSE_DEVICE
	array_len = sizeof(mdata) / sizeof(usbd_hid_mouse_data_t);
	delaytime = 1000 ;
	RTK_LOGS(TAG, "[HID] Mouse data TX test start\n");
#else
	array_len = sizeof(mdata) / sizeof(usbd_hid_keyboard_data_t);
	delaytime = 50 ;
	RTK_LOGS(TAG, "[HID] Keyboard data TX test start\n");
#endif

	do {
		RTK_LOGS(TAG, "[HID] Test round %d/%d\n", loop + 1, CONFIG_USBD_HID_CONSTANT_LOOP);
		for (i = 0; i < array_len; i++) {
			rtos_sema_take(hid_transmit_sema, RTOS_SEMA_MAX_COUNT);
			hid_send_device_data(&mdata[i]);
			rtos_time_delay_ms(delaytime);
		}
		rtos_time_delay_ms(5 * 1000); //next loop
	} while (++loop < CONFIG_USBD_HID_CONSTANT_LOOP);

	RTK_LOGS(TAG, "[HID] Test done\n");

#endif

	rtos_time_delay_ms(100);

example_usbd_hid_device_thread_fail:
#if CONFIG_USBD_HID_HOTPLUG
	rtos_sema_delete(hid_attach_status_changed_sema);
#endif
	rtos_sema_delete(hid_connect_sema);
	rtos_sema_delete(hid_transmit_sema);
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_hid(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_hid_thread", example_usbd_hid_thread, NULL, 1024, CONFIG_USBD_HID_INIT_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[HID] Create USBD HID thread fail\n");
	}
}
