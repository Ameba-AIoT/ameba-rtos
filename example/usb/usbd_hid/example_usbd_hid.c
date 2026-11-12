/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd_hid.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "usb_app.h"
#include "example_usbd_hid.h"

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

// Send mouse data through monitor
#define HID_MOUSE_CMD                            1

#ifdef CONFIG_USBD_HID_VENDOR
// Vendor reports carry no fixed data table: every Output report from the host is echoed back
// as an Input report instead, and the "tx" command sends one on demand
#define HID_CONSTANT_DATA                        0
#define HID_VENDOR_LOOPBACK                      1
#define HID_VENDOR_CMD                           1
#else
// Send hid device data table. Once connected to PC, cursor of PC will do
// process according to array mdata[].
#define HID_CONSTANT_DATA                        1
#define HID_VENDOR_LOOPBACK                      0
#define HID_VENDOR_CMD                           0
#endif
#define HID_CONSTANT_LOOP                        10

// The AT command: the mouse report or the vendor report, headset and keyboard have none
#if (HID_MOUSE_CMD && defined(CONFIG_USBD_HID_MOUSE)) || HID_VENDOR_CMD
#define HID_SHELL_CMD_EN                         1
#else
#define HID_SHELL_CMD_EN                         0
#endif

// hid_send_device_data() is referenced by the shell commands, by the constant
// data test and by the vendor loopback only
#if HID_CONSTANT_DATA || HID_SHELL_CMD_EN || HID_VENDOR_LOOPBACK
#define HID_TX_EN                                1
#else
#define HID_TX_EN                                0
#endif

// The constant data test or the vendor loopback, one worker thread either way
#define HID_XFER_THREAD_EN                       (HID_CONSTANT_DATA || HID_VENDOR_LOOPBACK)

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
// Longest sleep slice of the xfer thread, bounds how long a stop waits for it
#define HID_QUIT_POLL_MS                         100U

/* Private types -------------------------------------------------------------*/

typedef struct {
	u8 left;			//left button. 0: release, 1: press
	u8 right;			//right button. 0: release, 1: press
	u8 middle;			//wheel button. 0: release, 1: press
	s8 x_axis;			//x-axis pixels. relative value from -127 to 127, positive for right and negative for left
	s8 y_axis;			//y-axis pixels. relative value from -127 to 127, positive for up and negative for down
	s8 wheel;			//scrolling units. relative value from -127 to 127, positive for up and negative for down.
} usbd_hid_mouse_data_t;

typedef struct {
	u8 func_cmd;			//left & right : alt win shift alt command
	u8 padding;				//constant panding
	u8 cmd[6];				//6 command key values
} usbd_hid_keyboard_data_t;

typedef struct {
	u8 keys;				//pressed headset buttons, USBD_HID_HEADSET_KEY_*, 0: all released
} usbd_hid_headset_data_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void hid_cb_init(void);
static void hid_cb_deinit(void);
static void hid_cb_setup(void);
static void hid_cb_transmitted(int status);
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
static void hid_cb_received(u8 *buf, u32 len);
#endif
static void hid_cb_status_changed(u8 old_status, u8 status);

#if HID_SHELL_CMD_EN
static int hid_cmd(u16 argc, char *argv[]);
static void hid_cmd_at(u16 argc, char **argv);
#endif

#if HID_TX_EN
static int hid_send_device_data(const void *data);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

/* Serializes the USB stack bring up/tear down against the TX path, taken by
 * the hotplug thread, the xfer thread, the mouse command and the stop entry.
 * Created by the first start and never deleted: the mouse command runs on the shell thread,
 * which lives as long as the firmware and can not be joined by the example, so
 * there is no point in time at which this mutex is provably unreferenced.
 * hid_stack_ready is the gate that keeps callers off the stack instead. */
static rtos_mutex_t hid_state_mutex;
/* 1: usbd_init() and usbd_hid_init() both done, TX allowed. Cleared before any
 * tear down, so a non-zero value also implies hid_state_mutex is valid */
static volatile u8 hid_stack_ready;

#if HID_HOTPLUG
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 hid_attach_status;
static rtos_sema_t hid_attach_status_changed_sema;
/* Increased on every successful (re)init, identifies one test session */
static volatile u32 hid_session_id;
#endif

/* Start/stop state: the hotplug and xfer threads own the running stack, example_usbd_hid_stop()
   (or the hotplug thread when re-init fails) sets quit and joins them */
static usb_app_ctx_t hid_app;

#if HID_CONSTANT_DATA
static rtos_sema_t hid_connect_sema;
static rtos_sema_t hid_transmit_sema;
#if defined(CONFIG_USBD_HID_HEADSET)
static const usbd_hid_headset_data_t mdata[] = {
	{USBD_HID_HEADSET_KEY_VOLUME_UP},	//Volume Up pressed
	{0},								//released
	{USBD_HID_HEADSET_KEY_VOLUME_DOWN},	//Volume Down pressed
	{0},								//released
};
#elif defined(CONFIG_USBD_HID_MOUSE)
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
#else /* CONFIG_USBD_HID_KEYBOARD */
static usbd_hid_keyboard_data_t mdata[] = {
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//all released

	{0,   0,   {4,	0,	0,	0,	0,	0} },	//enter a pressed
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//enter a released

	{2,   0,   {4,	0,	0,	0,	0,	0} },	//left Shift pressed + enter a pressed = A
	{0,   0,   {0,	0,	0,	0,	0,	0} },	//left Shift & a released
};
#endif
#endif

#if HID_VENDOR_LOOPBACK
/* Last Output report from the host, Report ID first: filled by the ISR, emptied by the
   loopback thread, possibly on another core. 0 length means empty. */
static u8 hid_rx_report[USBD_HID_VENDOR_REPORT_LEN];
static volatile u32 hid_rx_report_len;
static rtos_sema_t hid_rx_sema;
#endif

static const usbd_config_t hid_cfg = {
	.info = {
		.prod_str = "Realtek HID Device",
	},
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 724U,
	.ptx_fifo_depth = {0U, 256U, 0U, 0U, 0U},
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 688U,
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

static const usbd_hid_config_t hid_class_cfg = {
	.intr_in_xfer_size = USBD_HID_INTR_IN_BUF_SIZE,
	.intr_in_addr  = HID_INTR_IN_EP,
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	.intr_out_addr = HID_INTR_OUT_EP,
#endif
};

static const usbd_hid_usr_cb_t hid_usr_cb = {
	.init = hid_cb_init,
	.deinit = hid_cb_deinit,
	.setup = hid_cb_setup,
	.transmitted = hid_cb_transmitted,
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	.received = hid_cb_received,
#endif
	.status_changed = hid_cb_status_changed,
};

/* Exported variables --------------------------------------------------------*/

#if HID_SHELL_CMD_EN
/* AT commands "AT+<cmd>=<p1>,<p2>...": the command name is case-sensitive, the parameters are not */
ATCMD_TABLE_DATA_SECTION
const log_item_t usbd_hid_cmd_table[] = {
	{"+USBDHID", hid_cmd_at},
};
#endif

/* Private functions ---------------------------------------------------------*/

#if HID_CONSTANT_DATA
/**
  * @brief  Check whether the running test session is still the valid one
  * @param  session: Session id recorded when the test started
  * @retval 1 if a hotplug happened or the example is stopping else 0
  */
static u8 hid_session_changed(u32 session)
{
#if HID_HOTPLUG
	return ((hid_app.quit != 0U) || (hid_session_id != session)) ? 1U : 0U;
#else
	UNUSED(session);
	return hid_app.quit;
#endif
}

/**
  * @brief  Sleep in short slices so that the xfer thread sees a stop within HID_QUIT_POLL_MS
  * @param  ms: Sleep time
  * @retval None
  */
static void hid_xfer_sleep(u32 ms)
{
	u32 step;

	while ((ms != 0U) && (hid_app.quit == 0U)) {
		step = (ms > HID_QUIT_POLL_MS) ? HID_QUIT_POLL_MS : ms;
		rtos_time_delay_ms(step);
		ms -= step;
	}
}
#endif // HID_CONSTANT_DATA

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called once every worker thread is gone and the USB stack is fully
  *         deinited, so that no ISR callback can touch these objects any more.
  *         hid_state_mutex is deliberately kept: the shell thread may be holding
  *         it inside hid_send_device_data(), and deleting a held mutex is UB
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
#if HID_VENDOR_LOOPBACK
	rtos_sema_delete(hid_rx_sema);
	hid_rx_sema = NULL;
#endif
#if HID_HOTPLUG
	rtos_sema_delete(hid_attach_status_changed_sema);
	hid_attach_status_changed_sema = NULL;
#endif
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
static void hid_cb_transmitted(int status)
{
	UNUSED(status);
#if HID_CONSTANT_DATA
	rtos_sema_give(hid_transmit_sema);
#endif
}

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
/**
  * @brief  Data received over USB HID OUT endpoint, or the data stage of a SET_REPORT
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer, the vendor Output report starts with its Report ID
  * @param  len: RX data length (in bytes)
  * @retval None
  */
static void hid_cb_received(u8 *buf, u32 len)
{
#if HID_VENDOR_LOOPBACK
	/* Ref HID 1.11 5.6: the report descriptor declares the Report ID, a report with any other
	   leading byte is not one of ours. A report arriving before the previous one is echoed is
	   dropped: the loopback keeps a single slot, a customer protocol queues as it needs. */
	if ((len == 0U) || (buf[0] != USBD_HID_VENDOR_OUT_REPORT_ID) || (hid_rx_report_len != 0U)) {
		return;
	}
	len = (len > USBD_HID_VENDOR_REPORT_LEN) ? USBD_HID_VENDOR_REPORT_LEN : len;
	/* The loopback thread copies the slot under the same lock, possibly on another core */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	memcpy((void *)hid_rx_report, (const void *)buf, len);
	hid_rx_report_len = len;
	rtos_critical_exit(RTOS_CRITICAL_USB);
	rtos_sema_give(hid_rx_sema);
#else
	/* ISR context: no logging per report, it would stretch the interrupt for every OUT packet */
	UNUSED(buf);
	UNUSED(len);
#endif
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
/* Parse a whole-string decimal in [min, max], an optional leading '-' allowed for a negative min */
static int hid_parse_s32(const char *str, s32 min, s32 max, s32 *val)
{
	const char *p = str;
	char *end = NULL;
	u32 mag;
	s32 num;
	u8 neg = 0U;

	if (p == NULL) {
		return -RTK_ERR_BADARG;
	}
	if (*p == '-') {
		neg = 1U;
		p++;
	}
	if ((*p < '0') || (*p > '9')) {
		return -RTK_ERR_BADARG;
	}
	mag = (u32)strtoul(p, &end, 10);
	if ((end == NULL) || (*end != '\0') || (mag > 0x7FFFFFFFU)) {
		return -RTK_ERR_BADARG;
	}
	num = (neg != 0U) ? -(s32)mag : (s32)mag;
	if ((num < min) || (num > max)) {
		return -RTK_ERR_BADARG;
	}

	*val = num;
	return RTK_SUCCESS;
}

#endif
#endif  //HID_MOUSE_CMD

#if HID_SHELL_CMD_EN
static void hid_cmd_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBDHID=<cmd>[,<args>]\n");
#ifdef CONFIG_USBD_HID_MOUSE
	RTK_LOGS(TAG, RTK_LOG_INFO, "  mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "        Send one mouse report. left/right/middle: 0|1 (1 = pressed)\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "        x/y/wheel: relative move -127..127; omitted args are 0. Needs the host to have configured the device\n");
#else
	RTK_LOGS(TAG, RTK_LOG_INFO, "  tx[,<len>]  Send one vendor Input report, Report ID 0x%02x, carrying len bytes of value len & 0xFF\n",
			 USBD_HID_VENDOR_IN_REPORT_ID);
	RTK_LOGS(TAG, RTK_LOG_INFO, "        len: 1..%d, default 10, the rest of the report is 0. Needs the host to have configured the device\n",
			 USBD_HID_VENDOR_REPORT_PAYLOAD_LEN);
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "  help  Print this help\n");
}
#endif // HID_SHELL_CMD_EN

#if HID_MOUSE_CMD
#ifdef CONFIG_USBD_HID_MOUSE

/* Fill a mouse report from "mouse <left> [<right> <middle> <x> <y> <wheel>]", argv[0] being "mouse" */
static int hid_parse_mouse(u16 argc, char *argv[], usbd_hid_mouse_data_t *data)
{
	s32 val[6] = {0};
	s32 min;
	u16 i;

	if ((argc < 2U) || (argc > 7U)) {
		return -RTK_ERR_BADARG;
	}
	for (i = 1U; i < argc; i++) {
		/* Buttons are 1-bit fields, the axes LOGICAL_MINIMUM(-127)/LOGICAL_MAXIMUM(127), Ref HID 1.11 6.2.2.7 */
		min = (i <= 3U) ? 0 : -127;
		if (hid_parse_s32(argv[i], min, (i <= 3U) ? 1 : 127, &val[i - 1U]) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arg %d: %s\n", i, argv[i]);
			return -RTK_ERR_BADARG;
		}
	}

	memset((void *)data, 0, sizeof(*data));
	data->left = (u8)val[0];
	data->right = (u8)val[1];
	data->middle = (u8)val[2];
	data->x_axis = (s8)val[3];
	data->y_axis = (s8)val[4];
	data->wheel = (s8)val[5];
	return RTK_SUCCESS;
}

static int hid_cmd_mouse_data(u16 argc, char *argv[])
{
	usbd_hid_mouse_data_t data;

	if (hid_parse_mouse(argc, argv, &data) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBDHID=mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]\n");
		return -RTK_ERR_BADARG;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Send mouse data\n");

	return hid_send_device_data(&data);
}
#endif
#endif  //HID_MOUSE_CMD

#if HID_VENDOR_CMD
/* Whole-string decimal in [min, max] */
static int hid_parse_u32(const char *str, u32 min, u32 max, u32 *val)
{
	char *end = NULL;
	u32 num;

	if ((str == NULL) || (str[0] < '0') || (str[0] > '9')) {
		return -RTK_ERR_BADARG;
	}
	num = (u32)strtoul(str, &end, 10);
	if ((end == NULL) || (*end != '\0') || (num < min) || (num > max)) {
		return -RTK_ERR_BADARG;
	}
	*val = num;
	return RTK_SUCCESS;
}

/* Send one vendor Input report from "tx [<len>]", argv[0] being "tx" */
static int hid_cmd_vendor_tx(u16 argc, char *argv[])
{
	u8 report[USBD_HID_VENDOR_REPORT_LEN];
	u32 len = 10U;

	if ((argc > 2U) ||
		((argc == 2U) && (hid_parse_u32(argv[1], 1U, USBD_HID_VENDOR_REPORT_PAYLOAD_LEN, &len) != RTK_SUCCESS))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBDHID=tx[,<len>], len: 1..%d\n", USBD_HID_VENDOR_REPORT_PAYLOAD_LEN);
		return -RTK_ERR_BADARG;
	}

	/* Ref HID 1.11 5.6: every report has the fixed length its descriptor declares */
	memset((void *)report, 0, sizeof(report));
	report[0] = USBD_HID_VENDOR_IN_REPORT_ID;
	memset((void *)&report[1], (int)(len & 0xFFU), len);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Send vendor report, %u bytes of payload\n", len);

	return hid_send_device_data(report);
}
#endif // HID_VENDOR_CMD

#if HID_SHELL_CMD_EN
static int hid_cmd(u16 argc, char *argv[])
{
	if ((argc == 0U) || (_stricmp(argv[0], "help") == 0) || (_stricmp(argv[0], "?") == 0)) {
		hid_cmd_usage();
		return (argc == 0U) ? -RTK_ERR_BADARG : RTK_SUCCESS;
	}
	/* Refused while the app is not running, a hotplug reinit is fine */
	if (hid_app.state != USB_APP_STATE_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}
#ifdef CONFIG_USBD_HID_MOUSE
	if (_stricmp(argv[0], "mouse") == 0) {
		return hid_cmd_mouse_data(argc, argv);
	}
#else
	if (_stricmp(argv[0], "tx") == 0) {
		return hid_cmd_vendor_tx(argc, argv);
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd: %s, try AT+USBDHID=help\n", argv[0]);
	return -RTK_ERR_BADARG;
}

/* AT entry: argv[0] is reserved by the AT parser and argv[1] holds the first parameter */
static void hid_cmd_at(u16 argc, char **argv)
{
	int ret = hid_cmd((argc != 0U) ? (u16)(argc - 1U) : 0U, &argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}
#endif // HID_SHELL_CMD_EN

#if HID_TX_EN
/**
  * @brief  Send device data (wrapper function of usbd_hid_send_data())
  * @note   The stack status check and the transfer share one critical section,
  *         otherwise the hotplug thread could free the endpoint buffers between
  *         the check and the transfer
  * @note   Also reachable from the shell thread through the AT command, i.e.
  *         before the example is started, while it stops and after it has stopped
  * @param  pdata: Mouse, keyboard or headset data, or a whole vendor Input report, to send
  * @retval Result of the operation: RTK_SUCCESS if success else fail
  */
static int hid_send_device_data(const void *pdata)
{
	int ret;
#if defined(CONFIG_USBD_HID_MOUSE)
	u8 byte[4];
	const usbd_hid_mouse_data_t *data = (const usbd_hid_mouse_data_t *)pdata;

	memset((void *)byte, 0, 4);

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
	/* Two's complement on the wire, Ref HID 1.11 Appendix B.2 */
	byte[1] = (u8)data->x_axis;
	byte[2] = (u8)data->y_axis;
	byte[3] = (u8)data->wheel;

#elif defined(CONFIG_USBD_HID_HEADSET)
	const usbd_hid_headset_data_t *data = (const usbd_hid_headset_data_t *)pdata;
#elif defined(CONFIG_USBD_HID_VENDOR)
	const u8 *data = (const u8 *)pdata;
#else /* CONFIG_USBD_HID_KEYBOARD */
	const usbd_hid_keyboard_data_t *data = (const usbd_hid_keyboard_data_t *)pdata;
#endif

	/* Gate on hid_stack_ready BEFORE touching the mutex: it is still NULL if the
	 * example was never started, and already cleared if the init failed or the
	 * stack is gone for good. A hotplug right after this check is harmless, the
	 * mutex stays valid and the re-check below skips the TX. quit: a stop is
	 * tearing the example down, no new TX */
	if ((hid_stack_ready == 0U) || (hid_app.quit != 0U)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Stack not ready, skip TX\n");
		return -RTK_ERR_IO;
	}

	/* Never enter the gated section, nor give, without the mutex */
	ret = rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
	if (ret != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}
	if ((hid_stack_ready == 0U) || (hid_app.quit != 0U)) {
		/* Lost the race against a hotplug or stop tear down */
		RTK_LOGS(TAG, RTK_LOG_WARN, "Stack torn down, skip TX\n");
		ret = -RTK_ERR_IO;
	} else {
#if defined(CONFIG_USBD_HID_MOUSE)
		ret = usbd_hid_send_data((const u8 *)byte, 4);
#elif defined(CONFIG_USBD_HID_HEADSET)
		ret = usbd_hid_send_data(&data->keys, USBD_HID_HEADSET_REPORT_LEN);
#elif defined(CONFIG_USBD_HID_VENDOR)
		ret = usbd_hid_send_data(data, USBD_HID_VENDOR_REPORT_LEN);
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
	u8 fatal = 0U;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(hid_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (hid_app.quit != 0U) {
			/* Stop requested: the stack is left as it is, the stop entry tears it down */
			break;
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
			if (hid_app.quit != 0U) {
				/* Stop requested while the stack is down: skip the re-init, hid_stack_ready stays 0 */
				break;
			}

			rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
			ret = usbd_init(&hid_cfg);
			if (ret == 0) {
				ret = usbd_hid_init(&hid_usr_cb, &hid_class_cfg);
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
#if HID_VENDOR_LOOPBACK
				/* A report of the last session is not echoed into the new one */
				hid_rx_report_len = 0U;
#endif
				/* Open the gate at the very last step, so that the xfer thread
				 * never runs before the re-init is fully done */
				hid_session_id++;
				hid_stack_ready = 1U;
			}
			rtos_mutex_give(hid_state_mutex);

			if (ret != 0) {
				fatal = 1U;
				break;
			}
		} else if (hid_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
		}
	}

	if (fatal != 0U) {
		/* The stack is fully deinited here, no more ISR callback */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
		if (usb_app_ctx_claim_stop(&hid_app) != 0U) {
			/* No stop is in flight: this thread finishes the teardown on its own */
#if HID_XFER_THREAD_EN
			/* The xfer thread polls quit, set by the claim above */
			if (usb_app_ctx_join(&hid_app, 1U) != RTK_SUCCESS) {
				/* The xfer thread may still touch the shared objects: leak them */
				rtos_task_delete(NULL);
			}
#endif
			hid_free_resource();
			usb_app_ctx_end(&hid_app);
			rtos_task_delete(NULL);
		}
	}
	usb_app_ctx_leave(&hid_app);
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

	array_len = sizeof(mdata) / sizeof(mdata[0]);
#if defined(CONFIG_USBD_HID_MOUSE) || defined(CONFIG_USBD_HID_HEADSET)
	delaytime = 1000;
#else
	delaytime = 50;
#endif

	for (;;) {
		/* Wait for a usable session: stack inited and enumerated by the host */
		while ((hid_app.quit == 0U)
			   && ((hid_stack_ready == 0U) || (usbd_get_status() != USBD_ATTACH_STATUS_ATTACHED))) {
			rtos_time_delay_ms(HID_QUIT_POLL_MS);
		}
		if (hid_app.quit != 0U) {
			break;
		}

#if HID_HOTPLUG
		session = hid_session_id;
#endif
		if (rtos_sema_take(hid_connect_sema, HID_CONNECT_TIMEOUT_MS) != RTK_SUCCESS) {
			continue; /* Not configured by the host yet, re-check the state */
		}
		if (hid_app.quit != 0U) {
			break;
		}

#if defined(CONFIG_USBD_HID_MOUSE)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Mouse data TX test start\n");
#elif defined(CONFIG_USBD_HID_HEADSET)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Headset key TX test start\n");
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
				if (hid_send_device_data(&mdata[i]) != RTK_SUCCESS) {
					goto session_end;
				}
				hid_xfer_sleep(delaytime);
			}
			hid_xfer_sleep(5 * 1000); //next loop
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Test done\n");

session_end:
#if HID_HOTPLUG
		/* Idle until the hotplug thread finishes the next re-init, the test is
		 * restarted only on a brand new session */
		while ((hid_app.quit == 0U) && (hid_session_id == session)) {
			rtos_time_delay_ms(HID_QUIT_POLL_MS);
		}
		if (hid_app.quit != 0U) {
			break;
		}
#else
		break;
#endif
	}

	/* Never deinits the stack: the stop owner does, once every worker is gone */
	usb_app_ctx_leave(&hid_app);
	rtos_task_delete(NULL);
}
#endif // HID_CONSTANT_DATA

#if HID_VENDOR_LOOPBACK
/* Echo every vendor Output report back to the host as an Input report, payload unchanged */
static void example_usbd_hid_xfer_thread(void *param)
{
	u8 report[USBD_HID_VENDOR_REPORT_LEN];
	u32 len;
	int ret;

	UNUSED(param);

	while (hid_app.quit == 0U) {
		/* Bounded wait: polls quit, a stop also gives the semaphore to cut it short */
		if (rtos_sema_take(hid_rx_sema, HID_QUIT_POLL_MS) != RTK_SUCCESS) {
			continue;
		}
		if (hid_app.quit != 0U) {
			continue;
		}

		/* Take the report out of the slot and hand the slot back to the ISR in one go */
		memset((void *)report, 0, sizeof(report));
		rtos_critical_enter(RTOS_CRITICAL_USB);
		len = hid_rx_report_len;
		memcpy((void *)report, (const void *)hid_rx_report, len);
		hid_rx_report_len = 0U;
		rtos_critical_exit(RTOS_CRITICAL_USB);
		if (len == 0U) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "RX report ID 0x%02x, %u bytes, payload[0] 0x%02x\n", report[0], len, report[1]);

		/* Same payload, Input Report ID: the host reads back what it wrote */
		report[0] = USBD_HID_VENDOR_IN_REPORT_ID;
		ret = hid_send_device_data(report);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Echo fail %d\n", ret);
		}
	}

	/* Never deinits the stack: the stop owner does, once every worker is gone */
	usb_app_ctx_leave(&hid_app);
	rtos_task_delete(NULL);
}
#endif // HID_VENDOR_LOOPBACK

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int hid_start(void)
{
	int ret;
#if HID_XFER_THREAD_EN
	u32 workers = 0U;
	rtos_task_t xfer_task;
#endif
#if HID_HOTPLUG
	rtos_task_t hotplug_task;
#endif

	ret = usb_app_ctx_begin(&hid_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	if (hid_state_mutex == NULL) {
		ret = rtos_mutex_create(&hid_state_mutex);
		if (ret != RTK_SUCCESS) {
			goto exit_create_obj_fail;
		}
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

#if HID_VENDOR_LOOPBACK
	ret = rtos_sema_create(&hid_rx_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_create_obj_fail;
	}
	hid_rx_report_len = 0U;
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

	ret = usbd_hid_init(&hid_usr_cb, &hid_class_cfg);
	if (ret != 0) {
		goto exit_usbd_init_fail;
	}

#if HID_HOTPLUG
	hid_session_id = 1U;
#endif
	hid_stack_ready = 1U;

#if HID_XFER_THREAD_EN
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_hid_xfer_thread",
						   example_usbd_hid_xfer_thread, NULL,
						   HID_XFER_THREAD_STACK_SIZE, HID_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_task_fail;
	}
	workers++;
#endif

#if HID_HOTPLUG
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&hotplug_task, "usbd_hid_hotplug_thread",
						   example_usbd_hid_hotplug_thread, NULL,
						   HID_HOTPLUG_THREAD_STACK_SIZE, HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_task_fail;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to
	   core 0). Pinning the hotplug/deinit thread to CPU0 puts it on the same core
	   as the ISR, so deinit's local interrupt disable is meaningful again under
	   SMP. The xfer thread stays unaffined: hid_state_mutex already excludes TX
	   from a pending deinit, so it does not need to share the ISR's core. */
	rtos_task_set_affinity(hotplug_task, 0);
#endif
#endif // HID_HOTPLUG

	return RTK_SUCCESS;

#if HID_HOTPLUG || HID_XFER_THREAD_EN
exit_create_task_fail:
#if HID_XFER_THREAD_EN
	/* Wake and join the xfer thread if already running, it polls quit */
	hid_app.quit = 1U;
	if (usb_app_ctx_join(&hid_app, workers) != RTK_SUCCESS) {
		/* The xfer thread may still run on the stack: keep it, the app stays not idle */
		hid_app.state = USB_APP_STATE_STOPPING;
		return -RTK_ERR_TIMEOUT;
	}
#endif
	/* Same exclusion as the hotplug teardown: once the mutex is held no shell TX is inside
	   the class API, so closing the gate and deiniting cannot race it */
	rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
	hid_stack_ready = 0U;
	usbd_hid_deinit();
	usbd_deinit();
	rtos_mutex_give(hid_state_mutex);
	goto exit_create_obj_fail;
#endif

exit_usbd_init_fail:
	usbd_deinit();

exit_create_obj_fail:
	hid_free_resource();
	usb_app_ctx_end(&hid_app);

	return ret;
}

static void example_usbd_hid_thread(void *param)
{
	UNUSED(param);

	if (hid_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the HID device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_hid_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID demo start\n");

	return hid_start();
}

/**
  * @brief  Stop the HID device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbd_hid_stop(void)
{
	u32 workers = 0U;

	if (usb_app_ctx_claim_stop(&hid_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&hid_app);
	}

#if HID_HOTPLUG
	rtos_sema_give(hid_attach_status_changed_sema);
	workers++;
#endif
#if HID_CONSTANT_DATA
	/* The xfer thread polls quit, these only cut its bounded waits short */
	rtos_sema_give(hid_connect_sema);
	rtos_sema_give(hid_transmit_sema);
	workers++;
#endif
#if HID_VENDOR_LOOPBACK
	/* The loopback thread polls quit, this only cuts its bounded wait short */
	rtos_sema_give(hid_rx_sema);
	workers++;
#endif
	if (usb_app_ctx_join(&hid_app, workers) != RTK_SUCCESS) {
		/* A worker is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone. The stack is still up unless a hotplug left it down, the mutex
	   keeps the shell TX out of the class API while the gate closes */
	if (hid_stack_ready != 0U) {
		rtos_mutex_take(hid_state_mutex, RTOS_MAX_TIMEOUT);
		hid_stack_ready = 0U;
		usbd_hid_deinit();
		usbd_deinit();
		rtos_mutex_give(hid_state_mutex);
	}
	hid_free_resource();
	usb_app_ctx_end(&hid_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the HID device from its own thread
  * @retval None
  */
void example_usbd_hid(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD HID demo start\n");

	ret = rtos_task_create(&task, "usbd_hid_thread", example_usbd_hid_thread, NULL,
						   HID_INIT_THREAD_STACK_SIZE, HID_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD HID thread fail\n");
	}
}
