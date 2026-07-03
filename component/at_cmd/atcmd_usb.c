/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"
#include "atcmd_service.h"

#if defined(CONFIG_USB_HOST_EN)
#include "usbh.h"
#endif
#if defined(CONFIG_USB_DEVICE_EN)
#include "usbd.h"
#endif

#if defined(CONFIG_USB_HOST_EN) || defined(CONFIG_USB_DEVICE_EN)

/* Private defines -----------------------------------------------------------*/

static const char *const TAG = "ATUSB";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#if defined(CONFIG_USB_HOST_EN)
static void at_usbh(u16 argc, char **argv);
#endif
#if defined(CONFIG_USB_DEVICE_EN)
static void at_usbd(u16 argc, char **argv);
#endif
static void at_usb_help(void);

/* Private variables ---------------------------------------------------------*/

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_usb_items[] = {
#if defined(CONFIG_USB_HOST_EN)
	{"+USBH", at_usbh},
#endif
#if defined(CONFIG_USB_DEVICE_EN)
	{"+USBD", at_usbd},
#endif
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Print USBH help text.
 */
static void at_usb_help(void)
{
	at_printf("\r\n");
#if defined(CONFIG_USB_HOST_EN)
	at_printf("AT+USBH=<cmd>[,<param>]\r\n");
	at_printf("  suspend\tSuspend USB bus\r\n");
	at_printf("  resume\tResume USB bus\r\n");
	at_printf("  tmod,<val>\tSet USB test mode\r\n");
#endif
#if defined(CONFIG_USB_DEVICE_EN)
	at_printf("AT+USBD=<cmd>\r\n");
	at_printf("  wakehost\tSend remote wakeup\r\n");
#endif
}

#if defined(CONFIG_USB_HOST_EN)

/**
 * @brief  USB host AT command dispatcher.
 * @note   Usage:
 *           AT+USBH=suspend        — Suspend USB bus
 *           AT+USBH=resume         — Resume USB bus
 *           AT+USBH=tmod,<value>   — Set USB test mode
 */
static void at_usbh(u16 argc, char **argv)
{
	u32 mode;

	if (argc < 2U) {
		at_usb_help();
		return;
	}

	if (_stricmp(argv[1], "suspend") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB suspend\n");
		usbh_suspend();
	} else if (_stricmp(argv[1], "resume") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB resume\n");
		usbh_resume();
	} else if (_stricmp(argv[1], "tmod") == 0) {
		if (argc < 3U) {
			at_usb_help();
			return;
		}
		mode = _strtoul(argv[2], (char **)NULL, 10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB test mode=%lu\n", mode);
		usbh_select_test_mode((u8)(mode & 0xFF));
	} else {
		at_usb_help();
	}
}

#endif /* CONFIG_USB_HOST_EN */

#if defined(CONFIG_USB_DEVICE_EN)

/**
 * @brief  USB device AT command dispatcher.
 * @note   Usage:
 *           AT+USBD=wakehost  — Send remote wakeup
 */
static void at_usbd(u16 argc, char **argv)
{
	int ret;

	if (argc < 2U) {
		at_usb_help();
		return;
	}

	if (_stricmp(argv[1], "wakehost") == 0) {
		ret = usbd_wake_host();
		if (ret == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBD remote wakeup OK\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD remote wakeup fail\n");
		}
	} else {
		at_usb_help();
	}
}

#endif /* CONFIG_USB_DEVICE_EN */

/**
 * @brief  Print available USB AT commands.
 * @note   Called from help/command listing, matching print_wifi_at() pattern.
 */
void print_usb_at(void)
{
	int cmdSize;
	int i;

	cmdSize = sizeof(at_usb_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_usb_items[i].log_cmd);
	}
}

#endif /* CONFIG_USB_HOST_EN || CONFIG_USB_DEVICE_EN */

/* Exported functions --------------------------------------------------------*/
