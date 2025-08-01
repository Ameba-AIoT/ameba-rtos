/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sys_api.h"
#include "ameba_rtos_version.h"
#include "build_info.h"

#include "at_intf_spi.h"
#include "at_intf_sdio.h"
#include "at_intf_usbd.h"
#include "ameba_ota.h"

#include "atcmd_service.h"
#ifndef CONFIG_MP_SHRINK
#include "atcmd_wifi.h"
#ifdef CONFIG_LWIP_LAYER
#include "atcmd_mqtt.h"
#include "atcmd_sockets.h"
#include "atcmd_http.h"
#include "atcmd_websocket.h"
#include "atcmd_network.h"
#endif
#endif
#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
#include "atcmd_bt_mp.h"
#endif
#endif
#ifndef CONFIG_MP_INCLUDED
#if defined(CONFIG_BT_COEXIST)
#include "atcmd_coex.h"
#endif
#endif
#include "atcmd_sys.h"
#include "atcmd_sys_common.h"

#include "FreeRTOS.h"
#if (configGENERATE_RUN_TIME_STATS == 1)
#include "task.h"
#endif

#ifdef CONFIG_MP_INCLUDED
#if defined(configUSE_TRACE_FACILITY) && (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
extern void vTaskList(char *pcWriteBuffer);
#endif
#endif

static const char *const TAG = "AT-SYS";

#ifndef CONFIG_INIC_NO_FLASH

/****************************************************************
AT command process:
	AT+OTACLEAR
	Clear OTA signature.
	[+OTACLEAR]: OK
****************************************************************/
void at_otaclear(void *arg)
{
	UNUSED(arg);
	sys_clear_ota_signature();
	at_printf(ATCMD_OK_END_STR);
}

/****************************************************************
AT command process:
	AT+OTARECOVER
	Recover OTA signature.
	[+OTARECOVER]: OK
****************************************************************/
void at_otarecover(void *arg)
{
	UNUSED(arg);
	sys_recover_ota_signature();
	at_printf(ATCMD_OK_END_STR);
}

#endif /* CONFIG_INIC_NO_FLASH */

static void at_test_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+test=<mode>,<string/length>\r\n");
	RTK_LOGI(TAG, "\t<mode>:\t0-echo, 1-tt mode test\r\n");
	RTK_LOGI(TAG, "\t<string/length>:\tstring-mode 0 echo string, length-tt mode length\r\n");
}

/****************************************************************
AT command process:
	AT+TEST
	[+TEST]: OK
****************************************************************/
void at_test(void *arg)
{
	u8 *buffer = NULL;
	u8 error_no = 0;
	u32 start_time, end_time, tt_len;
	int argc = 0, mode = 0;
	char *argv[MAX_ARGC] = {0};

	argc = parse_param(arg, argv);
	if (argc == 1) {
		goto end;
	}

	if (argc != 3) {
		error_no = 1;
		goto end;
	}

	mode = (int)atoi(argv[1]);

	if (mode == 0) {
		at_printf("\r\n arg len = %d \r\n", strlen((char *)argv[2]));
		at_printf("\r\n arg = %s \r\n", (char *)argv[2]);
	} else if (mode == 1) {
		buffer = (u8 *)rtos_mem_malloc(10 * 1024);
		if (buffer == NULL) {
			error_no = 2;
			goto end;
		}

		tt_len = (u32)atoi(argv[2]);

		if (atcmd_tt_mode_start(tt_len) != 0) {
			error_no = 2;
			goto end;
		}

		u8 *buffer_ptr = buffer;
		int get_len = 0, remain_len = tt_len;

		get_len = atcmd_tt_mode_get(buffer_ptr, 1);
		remain_len -= get_len;
		start_time = rtos_time_get_current_system_time_ms();

		while (remain_len > 0) {
			get_len = atcmd_tt_mode_get(buffer_ptr, remain_len);
			if (get_len == 0) {
				RTK_LOGI(TAG, "host stops tt mode\r\n");
				break;
			}
			remain_len -= get_len;
		}

		end_time = rtos_time_get_current_system_time_ms();
		atcmd_tt_mode_end();

		at_printf("upstream test(tt mode): Send %d KBytes in %d ms, %d Kbits/sec\n\r", (int)((tt_len - remain_len) / 1024), (int)(end_time - start_time),
				  (int)(((tt_len - remain_len) * 8) / (end_time - start_time)));
	} else if (mode == 2) {
		u32 at_len = (u32)atoi(argv[2]);
		u32 send_len = 0;
		u32 malloc_size = 256;
#ifdef CONFIG_ATCMD_HOST_CONTROL
		if (g_host_control_mode == AT_HOST_CONTROL_SPI) {
			malloc_size = ATCMD_SPI_DMA_SIZE - 8;
		} else if (g_host_control_mode == AT_HOST_CONTROL_SDIO) {
			malloc_size = ATCMD_SDIO_MAX_SIZE;
		} else if (g_host_control_mode == AT_HOST_CONTROL_USB) {
			malloc_size = ATCMD_USBD_MAX_SIZE;
		}
#endif
		buffer = (u8 *)rtos_mem_malloc(malloc_size);
		memset(buffer, 1, malloc_size);
		at_printf(ATCMD_DOWNSTREAM_TEST_START_STR);
		while (at_len > 0) {
			send_len = at_len > malloc_size ? malloc_size : at_len;
			at_printf_data((char *)buffer, send_len);
			at_len -= send_len;
		}
		at_printf(ATCMD_DOWNSTREAM_TEST_END_STR);
	} else {
		error_no = 1;
		goto end;
	}

end:
	if (buffer != NULL) {
		rtos_mem_free(buffer);
	}

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_test_help();
	}
}

/****************************************************************
AT command process:
	AT+LIST
	Common AT Command:
	Then pending all the AT common commands supported.
	Wi-Fi AT Command:
	Then pending all the Wi-Fi common commands supported.
	......
	[+LIST]:OK
****************************************************************/
void at_list(void *arg)
{
	UNUSED(arg);

	/* System commands. */
	at_printf("Common AT Command:\r\n");
	print_system_common_at();

	/* System ap commands. */
	at_printf("AP Common AT Command:\r\n");
	print_system_at();

#ifndef CONFIG_MP_SHRINK
#ifdef CONFIG_WLAN
	/* Wifi commands. */
	at_printf("Wi-Fi AT Command:\r\n");
	print_wifi_at();
#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_SOCKET) && (CONFIG_ATCMD_SOCKET == 1)
	/* Socket AT Commands. */
	at_printf("Socket AT Commands:\r\n");
	print_socket_at();
#endif
#if defined(CONFIG_ATCMD_HTTP) && (CONFIG_ATCMD_HTTP == 1)
	/* HTTP commands. */
	at_printf("HTTP AT Command:\r\n");
	print_http_at();
#endif
#if defined(CONFIG_ATCMD_WEBSOCKET) && (CONFIG_ATCMD_WEBSOCKET == 1)
	/* WEBSOCKET commands. */
	at_printf("WEBSOCKET AT command:\r\n");
	print_websocket_at();
#endif
#if defined(CONFIG_ATCMD_NETWORK) && (CONFIG_ATCMD_NETWORK == 1)
	/* NETWORK commands. */
	at_printf("NETWORK AT command:\r\n");
	print_network_at();
#endif
#if defined(CONFIG_ATCMD_MQTT) && (CONFIG_ATCMD_MQTT == 1)
	/* MQTT commands. */
	at_printf("MQTT AT command:\r\n");
	print_mqtt_at();
#endif
#endif // CONFIG_LWIP_LAYER
#endif // CONFIG_WLAN
#endif // CONFIG_MP_SHRINK

#if defined(CONFIG_BT) && CONFIG_BT
	/* Bluetooth commands. */
	at_printf("BT AT command:\r\n");
	print_bt_ext_at();
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
	print_bt_mp_at();
#endif
#endif

#ifndef CONFIG_MP_INCLUDED
#if defined(CONFIG_BT_COEXIST)
	/* COEX commands. */
	at_printf("COEX AT command:\r\n");
	print_coex_at();
#endif
#endif

	at_printf(ATCMD_OK_END_STR);
}

#ifndef CONFIG_AMEBAD
static Certificate_TypeDef cert[2];
static s64 ver[2] = {0};  //32-bit full version
static u32 ota_region[3][2] = {0};
static const u32 image_pattern[2] = {
	0x35393138, 0x31313738,
};

static u8 at_get_ota_version(void)
{
	u16 MajorVer[2] = {0}; //16-bit major
	u16 MinorVer[2] = {0}; //16-bit minor
	u32 Vertemp;
	u8 ImgIndex, i;

	flash_get_layout_info(IMG_APP_OTA1, &ota_region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &ota_region[IMG_CERT][1], NULL);

	ota_region[IMG_IMG2][0] = ota_region[IMG_CERT][0] + 0x1000;
	ota_region[IMG_IMG2][1] = ota_region[IMG_CERT][1] + 0x1000;

	for (i = 0; i < 2; i++) {
		_memcpy((void *)&cert[i], (void *)ota_region[IMG_CERT][i], sizeof(Certificate_TypeDef));

		if (_memcmp(cert[i].Pattern, image_pattern, sizeof(image_pattern)) == 0) {
			MajorVer[i] = (u16)cert[i].MajorKeyVer;
			MinorVer[i] = (u16)cert[i].MinorKeyVer;
			Vertemp = (MajorVer[i] << 16) | MinorVer[i]; // get 32-bit full version number
			ver[i] = (s64)Vertemp;
		} else {
			ver[i] = -1;
		}
	}

	ImgIndex = ota_get_cur_index(1);

	return ImgIndex;
}
#endif


/****************************************************************
AT command process:
	AT+GMR
	+GMR:<at-version>,<sdk-version>(<compile_time>)
	[+GMR]: OK
****************************************************************/
void at_gmr(void *arg)
{
	UNUSED(arg);
	u32 buflen = 1024;
	char *buf = rtos_mem_malloc(buflen);
	at_printf("AMEBA-RTOS SDK VERSION: %d.%d.%d\n", AMEBA_RTOS_VERSION_MAJOR, AMEBA_RTOS_VERSION_MINOR, AMEBA_RTOS_VERSION_PATCH);
	at_printf("ATCMD VERSION: %d.%d.%d\r\n", ATCMD_VERSION, ATCMD_SUBVERSION, ATCMD_REVISION);

#ifndef CONFIG_AMEBAD
	u8 image_id = at_get_ota_version();
	u32 version = (u32)(ver[image_id] & 0xFFFFFFFF);
	at_printf("IMAGE VERSION: %d.%d\r\n", ((version >> 16) & 0xFFFF), (version & 0xFFFF));
#endif

	ChipInfo_GetSocName_ToBuf(buf, buflen - 1);
	at_printf("%s", buf);
	rtos_mem_free(buf);

	at_printf("COMPILE TIME: %s\r\n", RTL_FW_COMPILE_TIME);
	at_printf("COMPILE USER: %s@%s\r\n", RTL_FW_COMPILE_BY, RTL_FW_COMPILE_HOST);
	at_printf("COMPILE ENV : %s\r\n", RTL_FW_COMPILER);

	at_printf(ATCMD_OK_END_STR);
}

log_item_t at_sys_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
	{"+OTACLEAR", at_otaclear, {NULL, NULL}},
	{"+OTARECOVER", at_otarecover, {NULL, NULL}},
#endif
	{"+TEST", at_test, {NULL, NULL}},
	{"+LIST", at_list, {NULL, NULL}},
	{"+GMR", at_gmr, {NULL, NULL}},
};

void print_system_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_sys_items) / sizeof(at_sys_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_sys_items[index].log_cmd);
	}
}

void at_sys_init(void)
{
	atcmd_service_add_table(at_sys_items, sizeof(at_sys_items) / sizeof(at_sys_items[0]));
}
