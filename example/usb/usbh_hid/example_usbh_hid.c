/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "usbh.h"
#include "usbh_hid.h"
#include "usb_app.h"
#include "example_usbh_hid.h"

/* Private defines -----------------------------------------------------------*/

/* Applications, one per device type of the usbd_hid example */
#define USBH_HID_APP_MEDIA                          1   /* Media keys: volume up/down, mute, play/pause, stop (usbd_hid HEADSET) */
#define USBH_HID_APP_KEYBOARD                       2   /* Boot keyboard: typed keys and LED control (usbd_hid KEYBOARD) */
#define USBH_HID_APP_MOUSE                          3   /* Boot mouse: buttons, move and wheel (usbd_hid MOUSE) */
#define USBH_HID_APP_VENDOR                         4   /* Vendor reports: loopback against usbd_hid VENDOR */

/* Choose which application example */
#define CONFIG_USBH_HID_APP                         USBH_HID_APP_MEDIA

/**
 * @brief Enable hot-plug support.
 * When enabled, the hotplug thread waits on detach_sema; on each detach it
 * tears down the HID class driver and the USB host core, then reinitializes
 * so a freshly plugged device is re-enumerated automatically.
 */
#define CONFIG_USBH_HID_HOTPLUG                     1

/* Thread priorities */
#define USBH_HID_INIT_THREAD_PRIORITY               4U
#define USBH_HID_HOTPLUG_THREAD_PRIORITY            6U

/* Thread stack sizes */
#define USBH_HID_MAIN_TASK_STACK_SIZE               1024U
#define USBH_HID_MAIN_THREAD_PRIORITY               4U
#define USBH_HID_INIT_THREAD_STACK_SIZE             768U
#define USBH_HID_HOTPLUG_THREAD_STACK_SIZE          768U

/* Consumer Page usages, Ref HID Usage Tables 1.4 chapter 15 */
#define USBH_HID_CONSUMER_PLAY_PAUSE                0xCDU
#define USBH_HID_CONSUMER_STOP                      0xB7U
#define USBH_HID_CONSUMER_MUTE                      0xE2U
#define USBH_HID_CONSUMER_VOLUME_UP                 0xE9U
#define USBH_HID_CONSUMER_VOLUME_DOWN               0xEAU
#define USBH_HID_MEDIA_KEY_CNT                      5U

/* Boot reports, Ref HID 1.11 Appendix B */
#define USBH_HID_BOOT_KBD_REPORT_LEN                8U      /* Modifiers, reserved, 6 key codes */
#define USBH_HID_BOOT_KBD_KEY_CNT                   6U
#define USBH_HID_BOOT_MOUSE_REPORT_LEN              3U      /* Buttons, X, Y; a wheel byte may follow */
#define USBH_HID_KBD_LED_MASK                       0x1FU   /* Num/Caps/Scroll Lock, Compose, Kana, Ref HID Usage Tables 11 */
#define USBH_HID_KEY_ERR_ROLLOVER                   0x01U   /* ErrorRollOver, Ref HID Usage Tables 10 */

/* Vendor reports of the usbd_hid VENDOR device: Report ID first, 64 bytes each */
#define USBH_HID_VENDOR_IN_REPORT_ID                0x01U
#define USBH_HID_VENDOR_OUT_REPORT_ID               0x02U
#define USBH_HID_VENDOR_REPORT_LEN                  64U
#define USBH_HID_VENDOR_PAYLOAD_LEN                 (USBH_HID_VENDOR_REPORT_LEN - 1U)
#define USBH_HID_VENDOR_USAGE_PAGE                  0xFF00U

/* Private types -------------------------------------------------------------*/

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
/* One media key and where the device reports it */
typedef struct {
	usbh_hid_field_t field;     /* Field declaring the key */
	const char *name;           /* Printed on a press */
	u16 usage;                  /* Consumer usage */
	u8 found;                   /* The device declares the key */
} usbh_hid_media_key_t;
#endif

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void usbh_hid_cb_report(u8 idx, const u8 *buf, u32 len);
static void usbh_hid_cb_attached(void);
static void usbh_hid_cb_detached(void);
static u8 usbh_hid_cb_setup(u8 idx, const usbh_hid_itf_info_t *info);
static void usbh_hid_cb_process(usb_host_t *host, u8 msg);
static void usbh_hid_cmd_at(u16 argc, char **argv);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

#if CONFIG_USBH_HID_HOTPLUG
/**
 * @brief Application-level context for the HID demo.
 * @note  Both members exist only to run the hot-plug worker, and every use of
 *        them is already inside a CONFIG_USBH_HID_HOTPLUG block, so the whole
 *        context is compiled out with it. Without this guard the instance is
 *        unreferenced when hot-plug is disabled, which the build treats as an
 *        error (-Werror=unused-variable).
 */
typedef struct {
	rtos_sema_t detach_sema;    /*!< Signaled by detach callback; consumed by hotplug thread */
	rtos_task_t hotplug_task;   /*!< Hot-plug monitoring thread handle */
} usbh_hid_ctx_t;

static usbh_hid_ctx_t usbh_hid_ctx;
#endif

/* Serializes the USB stack bring up / tear down against the callers of the HID class API: the AT
   command runs on the shell thread, possibly on another core, and usbh_hid_deinit() frees what
   usbh_hid_send_report() and the EP0 requests use. Excluding such a caller has to be done by lock,
   not by core.
   Created by the first start and never deleted: the shell thread can not be joined by the example,
   so there is no point in time at which this mutex is provably unreferenced. */
static rtos_mutex_t usbh_hid_state_mutex = NULL;

/* 1: usbh_init(), usbh_hid_init() and usbh_start() are all done. Set by the bring up
   and cleared by every tear down under usbh_hid_state_mutex. */
static volatile u8 usbh_hid_stack_ready = 0U;

/* Start/stop state: example_usbh_hid_stop(), or the hotplug thread when the stack can
   not be rebuilt, sets quit, wakes the hotplug thread and joins it */
static usb_app_ctx_t usbh_hid_app;

/* Interface the application drives, written by setup() in the host task and read by the report
   thread and the AT command. 0xFF while none. */
static volatile u8 usbh_hid_app_itf = 0xFFU;

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
/* Written in setup(), before the gate opens, read by the report thread */
static usbh_hid_media_key_t usbh_hid_media_keys[USBH_HID_MEDIA_KEY_CNT] = {
	{.usage = USBH_HID_CONSUMER_VOLUME_UP,   .name = "Volume Up"},
	{.usage = USBH_HID_CONSUMER_VOLUME_DOWN, .name = "Volume Down"},
	{.usage = USBH_HID_CONSUMER_MUTE,        .name = "Mute"},
	{.usage = USBH_HID_CONSUMER_PLAY_PAUSE,  .name = "Play/Pause"},
	{.usage = USBH_HID_CONSUMER_STOP,        .name = "Stop"},
};
/* Keys held in the previous report, bit n for usbh_hid_media_keys[n]: a press is printed once */
static u8 usbh_hid_media_held;
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
/* Key codes held in the previous report, a key is printed on its press only */
static u8 usbh_hid_kbd_prev[USBH_HID_BOOT_KBD_KEY_CNT];
/* LED Output report, DMA source of the OUT pipe */
static u8 usbh_hid_kbd_led[CACHE_LINE_SIZE] USB_DMA_ALIGNED;
/* Usage IDs 0x04..0x38 of the Keyboard Page in US layout, without and with Shift */
static const char usbh_hid_kbd_ascii[2][0x39 - 0x04] = {
	"abcdefghijklmnopqrstuvwxyz1234567890\n\x1b\b\t -=[]\\#;'`,./",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()\n\x1b\b\t _+{}|~:\"~<>?",
};
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
/* Vendor Output report, DMA source of the OUT pipe */
static u8 usbh_hid_vendor_tx[USBH_HID_VENDOR_REPORT_LEN] USB_DMA_ALIGNED;
/* Payload of the last Output report, compared with the Input report the device loops back */
static u8 usbh_hid_vendor_expect;
static volatile u32 usbh_hid_vendor_expect_len;
#endif

/* USB host stack configuration */
static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_FULL,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_HID_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_HID_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	/*FIFO total 1024 DWORD, resv 12 DWORD for DMA*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	/*FIFO total 1024 DWORD, resv 48 DWORD */
	.rx_fifo_depth = 464,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

/* HID user callback configuration */
static const usbh_hid_usr_cb_t usbh_hid_cfg = {
	.attached = usbh_hid_cb_attached,
	.detached = usbh_hid_cb_detached,
	.setup  = usbh_hid_cb_setup,
	.report = usbh_hid_cb_report,
};

/* General USB host event callbacks */
static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_hid_cb_process,
};

/* Exported variables --------------------------------------------------------*/

/* AT command "AT+USBHHID=<cmd>[,<args>]": the command name is case-sensitive, the parameters are not */
ATCMD_TABLE_DATA_SECTION
const log_item_t usbh_hid_cmd_table[] = {
	{"+USBHHID", usbh_hid_cmd_at},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Whether an interface is the one this application drives
  * @param  info: Interface
  * @retval 1 if so, 0 otherwise
  */
static u8 usbh_hid_app_match(const usbh_hid_itf_info_t *info)
{
	usbh_hid_field_t field;

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
	for (u8 i = 0U; i < USBH_HID_MEDIA_KEY_CNT; i++) {
		if ((info->report_desc != NULL) && (usbh_hid_find_field(info->report_desc, info->report_desc_len, USBH_HID_MAIN_ITEM_TAG_INPUT,
											USBH_HID_UP_CONSUMER, usbh_hid_media_keys[i].usage, &field) == RTK_SUCCESS)) {
			return 1U;
		}
	}
	return 0U;
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
	UNUSED(field);
	/* Boot protocol gives a fixed report layout, Ref HID 1.11 Appendix B.1 */
	return ((info->subclass == USBH_HID_SUBCLASS_BOOT) && (info->protocol == USB_HID_PROTOCOL_KEYBOARD)) ? 1U : 0U;
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_MOUSE)
	UNUSED(field);
	/* Boot protocol gives a fixed report layout, Ref HID 1.11 Appendix B.2 */
	return ((info->subclass == USBH_HID_SUBCLASS_BOOT) && (info->protocol == USB_HID_PROTOCOL_MOUSE)) ? 1U : 0U;
#else
	/* The vendor collection declares Usage 0x02 for its Input report */
	return ((info->report_desc != NULL) && (usbh_hid_find_field(info->report_desc, info->report_desc_len, USBH_HID_MAIN_ITEM_TAG_INPUT,
											USBH_HID_VENDOR_USAGE_PAGE, 0x02U, &field) == RTK_SUCCESS)) ? 1U : 0U;
#endif
}

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
/**
  * @brief  Print the media keys pressed in a report
  * @param  buf: Report
  * @param  len: Report length
  * @retval None
  */
static void usbh_hid_media_report(const u8 *buf, u32 len)
{
	u8 held = 0U;

	for (u8 i = 0U; i < USBH_HID_MEDIA_KEY_CNT; i++) {
		/* A report of another Report ID leaves the key as it was */
		if ((usbh_hid_media_keys[i].found == 0U) ||
			((usbh_hid_media_keys[i].field.report_id != 0U) && ((len == 0U) || (buf[0] != usbh_hid_media_keys[i].field.report_id)))) {
			held |= (u8)(usbh_hid_media_held & (1U << i));
			continue;
		}
		if (usbh_hid_usage_active(buf, len, &(usbh_hid_media_keys[i].field)) != 0U) {
			held |= (u8)(1U << i);
			if ((usbh_hid_media_held & (1U << i)) == 0U) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s\n", usbh_hid_media_keys[i].name);
			}
		}
	}
	usbh_hid_media_held = held;
}
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
/**
  * @brief  Print the keys pressed in a boot keyboard report, Ref HID 1.11 Appendix B.1
  * @param  buf: Report
  * @param  len: Report length
  * @retval None
  */
static void usbh_hid_kbd_report(const u8 *buf, u32 len)
{
	const u8 *keys = &buf[2];
	u8 shift;
	u8 held;

	if (len < USBH_HID_BOOT_KBD_REPORT_LEN) {
		return;
	}
	/* More keys than the report holds: all six read ErrorRollOver, nothing to tell */
	if (keys[0] == USBH_HID_KEY_ERR_ROLLOVER) {
		return;
	}

	/* Left or right Shift */
	shift = ((buf[0] & 0x22U) != 0U) ? 1U : 0U;
	for (u8 i = 0U; i < USBH_HID_BOOT_KBD_KEY_CNT; i++) {
		if (keys[i] == 0U) {
			continue;
		}
		held = 0U;
		for (u8 j = 0U; j < USBH_HID_BOOT_KBD_KEY_CNT; j++) {
			if (usbh_hid_kbd_prev[j] == keys[i]) {
				held = 1U;
				break;
			}
		}
		if (held != 0U) {
			continue;
		}
		if ((keys[i] >= 0x04U) && (keys[i] < 0x39U)) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Key %02x '%c' mod %02x\n", keys[i], usbh_hid_kbd_ascii[shift][keys[i] - 0x04U], buf[0]);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Key %02x mod %02x\n", keys[i], buf[0]);
		}
	}
	memcpy((void *)usbh_hid_kbd_prev, (const void *)keys, USBH_HID_BOOT_KBD_KEY_CNT);
}
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MOUSE)
/**
  * @brief  Print a boot mouse report, Ref HID 1.11 Appendix B.2
  * @param  buf: Report
  * @param  len: Report length
  * @retval None
  */
static void usbh_hid_mouse_report(const u8 *buf, u32 len)
{
	s8 wheel = 0;

	if (len < USBH_HID_BOOT_MOUSE_REPORT_LEN) {
		return;
	}
	/* Boot mice commonly add the wheel as a fourth byte */
	if (len > USBH_HID_BOOT_MOUSE_REPORT_LEN) {
		wheel = (s8)buf[3];
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Mouse L%d R%d M%d x %d y %d wheel %d\n", buf[0] & 0x01U, (buf[0] >> 1) & 0x01U, (buf[0] >> 2) & 0x01U,
			 (s8)buf[1], (s8)buf[2], wheel);
}
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
/**
  * @brief  Check an Input report the device looped back
  * @param  buf: Report
  * @param  len: Report length
  * @retval None
  */
static void usbh_hid_vendor_report(const u8 *buf, u32 len)
{
	u32 expect_len = usbh_hid_vendor_expect_len;
	u32 i;

	if ((len == 0U) || (buf[0] != USBH_HID_VENDOR_IN_REPORT_ID)) {
		return;
	}
	if (expect_len == 0U) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Vendor RX %d bytes\n", len);
		return;
	}
	usbh_hid_vendor_expect_len = 0U;

	for (i = 0U; (i < expect_len) && ((i + 1U) < len); i++) {
		if (buf[i + 1U] != usbh_hid_vendor_expect) {
			break;
		}
	}
	if (i == expect_len) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Vendor loopback %d bytes OK\n", expect_len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Vendor loopback mismatch at %d\n", i);
	}
}
#endif

/**
  * @brief  HID report callback, invoked by the HID class report thread for every INTR IN report
  * @param  idx: Interface index
  * @param  buf: Report, Report ID first if the device uses one
  * @param  len: Report length
  * @retval None
  */
static void usbh_hid_cb_report(u8 idx, const u8 *buf, u32 len)
{
	if (idx != usbh_hid_app_itf) {
		return;
	}

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
	usbh_hid_media_report(buf, len);
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
	usbh_hid_kbd_report(buf, len);
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_MOUSE)
	usbh_hid_mouse_report(buf, len);
#else
	usbh_hid_vendor_report(buf, len);
#endif
}

/**
  * @brief  HID attach callback, invoked when a HID device is enumerated.
  */
static void usbh_hid_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	usbh_hid_app_itf = 0xFFU;
#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
	usbh_hid_media_held = 0U;
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
	memset((void *)usbh_hid_kbd_prev, 0, sizeof(usbh_hid_kbd_prev));
#elif (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
	usbh_hid_vendor_expect_len = 0U;
#endif
}

/**
  * @brief  HID detach callback, invoked when the HID device is removed.
  *         In hot-plug mode, signals the hotplug thread to reinitialize.
  */
static void usbh_hid_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	usbh_hid_app_itf = 0xFFU;

#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_give(usbh_hid_ctx.detach_sema);
#endif
}

/**
  * @brief  HID setup callback, invoked by the host task once per HID interface
  * @note   Host task: no blocking call, the protocol wanted is returned instead
  * @param  idx: Interface index
  * @param  info: Interface
  * @retval Protocol the interface shall run
  */
static u8 usbh_hid_cb_setup(u8 idx, const usbh_hid_itf_info_t *info)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP itf %d sub %d proto %d desc %d OUT %d\n", info->itf_num, info->subclass, info->protocol,
			 info->report_desc_len, info->has_out_ep);

	if ((usbh_hid_app_itf != 0xFFU) || (usbh_hid_app_match(info) == 0U)) {
		return USBH_HID_PROTOCOL_REPORT;
	}

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_MEDIA)
	for (u8 i = 0U; i < USBH_HID_MEDIA_KEY_CNT; i++) {
		usbh_hid_media_keys[i].found = (usbh_hid_find_field(info->report_desc, info->report_desc_len, USBH_HID_MAIN_ITEM_TAG_INPUT,
										USBH_HID_UP_CONSUMER, usbh_hid_media_keys[i].usage, &(usbh_hid_media_keys[i].field)) == RTK_SUCCESS) ? 1U : 0U;
	}
#endif

	usbh_hid_app_itf = idx;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Use itf %d\n", info->itf_num);

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD) || (CONFIG_USBH_HID_APP == USBH_HID_APP_MOUSE)
	/* The reports are decoded in the fixed boot layout, Ref HID 1.11 Appendix B */
	return USBH_HID_PROTOCOL_BOOT;
#else
	return USBH_HID_PROTOCOL_REPORT;
#endif
}

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD) || (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
/* Whole-string decimal in [min, max] */
static int usbh_hid_parse_u32(const char *str, u32 min, u32 max, u32 *val)
{
	char *end = NULL;
	u32 num;

	if ((str == NULL) || (str[0] < '0') || (str[0] > '9')) {
		return -RTK_ERR_BADARG;
	}
	num = (u32)strtoul(str, &end, 0);
	if ((end == NULL) || (*end != '\0') || (num < min) || (num > max)) {
		return -RTK_ERR_BADARG;
	}
	*val = num;
	return RTK_SUCCESS;
}
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
/* Set the keyboard LEDs from "led,<mask>": the OUT pipe if any, SET_REPORT(Output) otherwise, Ref HID 1.11 Appendix B.1 */
static int usbh_hid_cmd_led(u16 argc, char *argv[])
{
	u32 mask;
	int ret;

	if ((argc != 2U) || (usbh_hid_parse_u32(argv[1], 0U, USBH_HID_KBD_LED_MASK, &mask) != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBHHID=led,<mask>, mask: 0..0x%02x\n", USBH_HID_KBD_LED_MASK);
		return -RTK_ERR_BADARG;
	}

	usbh_hid_kbd_led[0] = (u8)mask;
	ret = usbh_hid_send_report(usbh_hid_app_itf, usbh_hid_kbd_led, 1U);
	if (ret == -RTK_ERR_BADARG) {
		/* No Interrupt OUT endpoint, the case of most keyboards */
		ret = usbh_hid_set_report(usbh_hid_app_itf, USBH_HID_REPORT_OUTPUT, 0U, usbh_hid_kbd_led, 1U);
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "LED %02x: %d\n", mask, ret);

	return ret;
}
#endif

#if (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
/* Send one vendor Output report from "tx[,<len>]", the device loops it back as an Input report */
static int usbh_hid_cmd_vendor_tx(u16 argc, char *argv[])
{
	u32 len = 10U;
	int ret;

	if ((argc > 2U) || ((argc == 2U) && (usbh_hid_parse_u32(argv[1], 1U, USBH_HID_VENDOR_PAYLOAD_LEN, &len) != RTK_SUCCESS))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBHHID=tx[,<len>], len: 1..%d\n", USBH_HID_VENDOR_PAYLOAD_LEN);
		return -RTK_ERR_BADARG;
	}

	/* Every report has the fixed length its descriptor declares, Ref HID 1.11 5.6 */
	memset((void *)usbh_hid_vendor_tx, 0, sizeof(usbh_hid_vendor_tx));
	usbh_hid_vendor_tx[0] = USBH_HID_VENDOR_OUT_REPORT_ID;
	memset((void *)&usbh_hid_vendor_tx[1], (int)(len & 0xFFU), len);
	usbh_hid_vendor_expect = (u8)(len & 0xFFU);
	usbh_hid_vendor_expect_len = len;

	ret = usbh_hid_send_report(usbh_hid_app_itf, usbh_hid_vendor_tx, USBH_HID_VENDOR_REPORT_LEN);
	if (ret == -RTK_ERR_BADARG) {
		ret = usbh_hid_set_report(usbh_hid_app_itf, USBH_HID_REPORT_OUTPUT, USBH_HID_VENDOR_OUT_REPORT_ID, usbh_hid_vendor_tx,
								  USBH_HID_VENDOR_REPORT_LEN);
	}
	if (ret != RTK_SUCCESS) {
		usbh_hid_vendor_expect_len = 0U;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Vendor TX %d bytes: %d\n", len, ret);

	return ret;
}
#endif

static void usbh_hid_cmd_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBHHID=<cmd>[,<args>]\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  status       Show the interface in use, needs a HID device set up\n");
#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
	RTK_LOGS(TAG, RTK_LOG_INFO, "  led,<mask>   Set the keyboard LEDs, mask 0..0x%02x: bit0 Num, bit1 Caps, bit2 Scroll Lock\n", USBH_HID_KBD_LED_MASK);
#endif
#if (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
	RTK_LOGS(TAG, RTK_LOG_INFO, "  tx[,<len>]   Send one Output report, Report ID 0x%02x, carrying len bytes of value len & 0xFF\n",
			 USBH_HID_VENDOR_OUT_REPORT_ID);
	RTK_LOGS(TAG, RTK_LOG_INFO, "               len: 1..%d, default 10; the device loops it back as Input report 0x%02x\n",
			 USBH_HID_VENDOR_PAYLOAD_LEN, USBH_HID_VENDOR_IN_REPORT_ID);
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "  help         Print this help\n");
}

static int usbh_hid_cmd(u16 argc, char *argv[])
{
	int ret;

	if ((argc == 0U) || (_stricmp(argv[0], "help") == 0) || (_stricmp(argv[0], "?") == 0)) {
		usbh_hid_cmd_usage();
		return (argc == 0U) ? -RTK_ERR_BADARG : RTK_SUCCESS;
	}
	/* Refused while the app is not running, the command stays callable while another app runs */
	if (usbh_hid_app.state != USB_APP_STATE_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}

	/* The class API is called under the lock the teardown takes, see usbh_hid_state_mutex */
	rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);
	if ((usbh_hid_stack_ready == 0U) || (usbh_hid_app_itf == 0xFFU)) {
		rtos_mutex_give(usbh_hid_state_mutex);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No HID device\n");
		return -RTK_ERR_IO;
	}

	if (_stricmp(argv[0], "status") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "App %d, itf idx %d\n", CONFIG_USBH_HID_APP, usbh_hid_app_itf);
		ret = RTK_SUCCESS;
#if (CONFIG_USBH_HID_APP == USBH_HID_APP_KEYBOARD)
	} else if (_stricmp(argv[0], "led") == 0) {
		ret = usbh_hid_cmd_led(argc, argv);
#endif
#if (CONFIG_USBH_HID_APP == USBH_HID_APP_VENDOR)
	} else if (_stricmp(argv[0], "tx") == 0) {
		ret = usbh_hid_cmd_vendor_tx(argc, argv);
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd: %s, try AT+USBHHID=help\n", argv[0]);
		ret = -RTK_ERR_BADARG;
	}
	rtos_mutex_give(usbh_hid_state_mutex);

	return ret;
}

/* AT entry: argv[0] is reserved by the AT parser and argv[1] holds the first parameter */
static void usbh_hid_cmd_at(u16 argc, char **argv)
{
	int ret = usbh_hid_cmd((argc != 0U) ? (u16)(argc - 1U) : 0U, &argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}

/**
  * @brief  General USB host process callback (attach/detach state machine).
  * @param  host: Host handle
  * @param  msg:  Host state message
  * @retval Status
  */
static void usbh_hid_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);
	UNUSED(msg);
}

/**
  * @brief  Tear the stack down under the lock if it is up
  * @note   Only called once no other thread can rebuild the stack
  * @retval None
  */
static void usbh_hid_stack_deinit(void)
{
	rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);
	if (usbh_hid_stack_ready != 0U) {
		usbh_hid_stack_ready = 0U;
		usbh_stop();
		usbh_hid_deinit();
		usbh_deinit();
	}
	rtos_mutex_give(usbh_hid_state_mutex);
}

#if CONFIG_USBH_HID_HOTPLUG
/**
  * @brief  Hot-plug worker thread.
  *
  *         Sleeps on usbh_hid_ctx.detach_sema.  On each detach event it tears
  *         down the HID class driver and the USB host core, then reinitializes
  *         the stack so a freshly plugged device is re-enumerated.
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_hid_hotplug_thread(void *param)
{
	int ret;
	u32 hotplug_count = 0;
	u8 fatal = 0U;

	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning
	   the hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so
	   the local interrupt disables inside the teardown are meaningful again. Any
	   class API caller stays unaffined: a pending teardown is excluded from it by
	   usbh_hid_state_mutex, not by core.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	for (;;) {
		if (rtos_sema_take(usbh_hid_ctx.detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (usbh_hid_app.quit != 0U) {
			/* Stop requested: the stack is still up, example_usbh_hid_stop() tears it down */
			break;
		}

		hotplug_count++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "\n========== Hotplug #%u ==========\n", hotplug_count);

		rtos_time_delay_ms(200);

		/* Tear down under the lock, see usbh_hid_state_mutex */
		usbh_hid_stack_deinit();

		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		if (usbh_hid_app.quit != 0U) {
			/* Stop requested: leave the stack down, stack_ready tells stop it is gone */
			break;
		}

		/* Re-initialize */
		rtos_mutex_take(usbh_hid_state_mutex, RTOS_MAX_TIMEOUT);

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != RTK_SUCCESS) {
			rtos_mutex_give(usbh_hid_state_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail: %d\n", ret);
			fatal = 1U;
			break;
		}

		ret = usbh_hid_init(&usbh_hid_cfg);
		if (ret != RTK_SUCCESS) {
			usbh_deinit();
			rtos_mutex_give(usbh_hid_state_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail: %d\n", ret);
			fatal = 1U;
			break;
		}

		ret = usbh_start();
		if (ret != RTK_SUCCESS) {
			usbh_hid_deinit();
			usbh_deinit();
			rtos_mutex_give(usbh_hid_state_mutex);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail: %d\n", ret);
			fatal = 1U;
			break;
		}
		usbh_hid_stack_ready = 1U;
		rtos_mutex_give(usbh_hid_state_mutex);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
	}

	usbh_hid_ctx.hotplug_task = NULL;
	if (fatal != 0U) {
		/* A re-init failed, so the stack is gone for good (stack_ready stays 0) */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
		if (usb_app_ctx_claim_stop(&usbh_hid_app) != 0U) {
			/* No stop is in flight and no other worker exists: finish the teardown here.
			   The stack is down, so the detach callback can no longer give the sema. */
			rtos_sema_delete(usbh_hid_ctx.detach_sema);
			usbh_hid_ctx.detach_sema = NULL;
			usb_app_ctx_end(&usbh_hid_app);
			rtos_task_delete(NULL);
		}
	}
	usb_app_ctx_leave(&usbh_hid_app);
	rtos_task_delete(NULL);
}
#endif /* CONFIG_USBH_HID_HOTPLUG */

/**
  * @brief  Bring up the USB host stack, then start the hotplug thread
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int usbh_hid_start(void)
{
	int ret;

	ret = usb_app_ctx_begin(&usbh_hid_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Created by the first start and never deleted, see the declaration */
	if (usbh_hid_state_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_hid_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto abort_exit;
		}
	}

#if CONFIG_USBH_HID_HOTPLUG
	ret = rtos_sema_create(&usbh_hid_ctx.detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto abort_exit;
	}
#endif

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail: %d\n", ret);
		goto free_sema_exit;
	}

	ret = usbh_hid_init(&usbh_hid_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID init fail: %d\n", ret);
		goto usb_deinit_exit;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	ret = usbh_start();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB start fail: %d\n", ret);
		usbh_hid_deinit();
		goto usb_deinit_exit;
	}
	usbh_hid_stack_ready = 1U;

#if CONFIG_USBH_HID_HOTPLUG
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&usbh_hid_ctx.hotplug_task, "usbh_hid_hotplug_thread",
						   example_usbh_hid_hotplug_thread, NULL,
						   USBH_HID_HOTPLUG_THREAD_STACK_SIZE, USBH_HID_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		/* Roll back under the lock, exactly as the hotplug thread does */
		usbh_hid_stack_deinit();
		goto free_sema_exit;
	}
#endif

	return RTK_SUCCESS;

usb_deinit_exit:
	/* usbh_hid_init() or usbh_start() failed, so only the host core is up and the gate was
	   never opened: no lock needed here. */
	usbh_deinit();

free_sema_exit:
#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_delete(usbh_hid_ctx.detach_sema);
	usbh_hid_ctx.detach_sema = NULL;
#endif

abort_exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH HID demo aborted\n");
	usb_app_ctx_end(&usbh_hid_app);

	return ret;
}

/**
  * @brief  Standalone entry thread: brings the demo up and leaves
  * @param  param: Unused.
  * @retval None
  */
static void example_usbh_hid_init_thread(void *param)
{
	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* A start that fails rolls back through usbh_deinit(), which waits for no ISR on another
	   core: run it on CPU0, where the USB IRQ is delivered, as the hotplug thread does.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	if (usbh_hid_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the HID host, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbh_hid_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID demo start\n");

	return usbh_hid_start();
}

/**
  * @brief  Stop the HID host and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if the hotplug thread did not leave in time
  */
int example_usbh_hid_stop(void)
{
	if (usb_app_ctx_claim_stop(&usbh_hid_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&usbh_hid_app);
	}

#if CONFIG_USBH_HID_HOTPLUG
	/* Hotplug thread: parked on detach, or inside a rebuild that checks quit */
	rtos_sema_give(usbh_hid_ctx.detach_sema);
	if (usb_app_ctx_join(&usbh_hid_app, 1U) != RTK_SUCCESS) {
		/* The hotplug thread is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}
#endif

	/* No worker is left. The stack is down already if the hotplug thread left mid-rebuild. */
	usbh_hid_stack_deinit();
#if CONFIG_USBH_HID_HOTPLUG
	rtos_sema_delete(usbh_hid_ctx.detach_sema);
	usbh_hid_ctx.detach_sema = NULL;
#endif
	usb_app_ctx_end(&usbh_hid_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  USB Host HID example entry point.
  * @retval None
  */
void example_usbh_hid(void)
{
	int ret;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH HID demo start\n");

	ret = rtos_task_create(NULL, "usbh_hid_init_thread",
						   example_usbh_hid_init_thread, NULL,
						   USBH_HID_INIT_THREAD_STACK_SIZE, USBH_HID_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH HID thread fail\n");
	}
}
