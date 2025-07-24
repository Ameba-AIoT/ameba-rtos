/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "platform_autoconf.h"

#ifdef CONFIG_SUPPORT_ATCMD

#include "lwip_netconf.h"
#include "at_intf_uart.h"
#include "at_intf_spi.h"
#include "at_intf_sdio.h"
#include "at_intf_usbd.h"
#include "atcmd_service.h"
#include "atcmd_sys_common.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
#include "vfs.h"
#include "kv.h"
#ifndef CONFIG_ZEPHYR_SDK
#include "cJSON.h"
#endif
#endif

#include "atcmd_sys.h"
#include "atcmd_diag.h"

#ifndef CONFIG_MP_SHRINK
#ifdef CONFIG_WLAN
#include "atcmd_wifi.h"
#ifdef CONFIG_LWIP_LAYER
#include "atcmd_mqtt.h"
#include "atcmd_sockets.h"
#include "atcmd_http.h"
#include "atcmd_websocket.h"
#include "atcmd_network.h"
#include "atcmd_ota.h"
#endif
#endif
#include "atcmd_fs.h"
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

#ifndef CONFIG_AMEBAD
#include "atcmd_otp.h"
#endif

//======================================================
struct list_head log_hash[ATC_INDEX_NUM];

rtos_mutex_t at_printf_mutex = NULL;

static const char *const TAG = "AT";

log_init_t log_init_table[] = {
	at_sys_init_common,

#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
#ifndef CONFIG_MP_SHRINK
#ifdef CONFIG_WLAN
	at_wifi_init,
#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_MQTT) && (CONFIG_ATCMD_MQTT == 1)
	at_mqtt_init,
#endif
#if defined(CONFIG_ATCMD_SOCKET) && (CONFIG_ATCMD_SOCKET == 1)
	at_socket_init,
#endif
#if defined(CONFIG_ATCMD_HTTP) && (CONFIG_ATCMD_HTTP == 1)
	at_http_init,
#endif
#if defined(CONFIG_ATCMD_WEBSOCKET) && (CONFIG_ATCMD_WEBSOCKET == 1)
	at_websocket_init,
#endif
#if defined(CONFIG_ATCMD_NETWORK) && (CONFIG_ATCMD_NETWORK == 1)
	at_network_init,
#endif
#if defined(CONFIG_ATCMD_OTA) && (CONFIG_ATCMD_OTA == 1)
	at_ota_init,
#endif
#endif  //CONFIG_LWIP_LAYER
#endif  //CONFIG_WLAN
	at_fs_init,
#endif  //CONFIG_MP_SHRINK

#if defined(CONFIG_BT) && CONFIG_BT
	at_bt_init,
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED
	at_mp_init,
#endif
#endif


#ifndef CONFIG_MP_INCLUDED
#if defined(CONFIG_BT_COEXIST)
	at_coex_init,
#endif
#endif
	at_sys_init,
	at_diag_init,
#ifndef CONFIG_AMEBAD
	at_otp_init,
#endif
#endif
};


//======================================================
#if (defined CONFIG_ATCMD_HOST_CONTROL && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE))
#ifdef CONFIG_SUPPORT_SDIO_DEVICE
extern u8 SDIO_Pin_Grp;
extern const u8 SDIO_PAD[5][6];
#endif
RingBuffer *atcmd_tt_mode_rx_ring_buf = NULL;
char g_tt_mode = 0;
char g_tt_mode_check_watermark = 0;
char g_tt_mode_indicate_high_watermark = 0;
char g_tt_mode_indicate_low_watermark = 1;
char g_host_control_mode = AT_HOST_CONTROL_UART;
volatile char g_tt_mode_stop_flag = 0;
volatile u8 g_tt_mode_stop_char_cnt = 0;
rtos_timer_t xTimers_TT_Mode;
char pin_name[5];
char global_buf[SMALL_BUF];
/* Out callback function */
at_write out_buffer;
rtos_sema_t atcmd_tt_mode_sema;

extern s32 wifi_set_countrycode(u8 *cntcode);

/**
 * @brief Output format strings, like printf.
 * @param fmt: format string.
 * @return The length of the output string or error code.
 * @retval -1: encoding error or format string is tool long.
 * @retval others: The length of the output string.
 */
int at_printf(const char *fmt, ...)
{
	int ret = -1;
	int len_fmt = 0;
	char *buf;

	at_printf_lock();

	va_list ap;
	va_start(ap, fmt);
	len_fmt = vsnprintf(global_buf, SMALL_BUF, fmt, ap);
	va_end(ap);

	if (len_fmt < 0) {
		goto fail;
	}

	if (len_fmt < SMALL_BUF) {
		buf = global_buf;
	} else if (len_fmt < MAX_BUF_LEN) {
		buf = (char *)rtos_mem_malloc(len_fmt + 1);
		if (buf == NULL) {
			goto fail;
		}

		va_start(ap, fmt);
		vsnprintf(buf, len_fmt + 1, fmt, ap);
		va_end(ap);
	} else {
		RTK_LOGE(TAG, "print string len %d exceed max buffer length : %d\n", (int)len_fmt, MAX_BUF_LEN);
		goto fail;
	}

	ret = len_fmt;

	if (out_buffer) {
		out_buffer(buf, len_fmt);
	}

	if (len_fmt >= SMALL_BUF) {
		rtos_mem_free(buf);
	}

fail:
	at_printf_unlock();
	return ret;
}

/**
 * @brief Output format strings reported by system.
 * @param fmt: format string.
 * @return The length of the output string or error code.
 * @retval -1: encoding error or format string is tool long.
 * @retval others: The length of the output string.
 */
int at_printf_indicate(const char *fmt, ...)
{
	int ret = -1;
	int len_fmt = 0;
	char *buf;

	at_printf_lock();

	va_list ap;
	va_start(ap, fmt);
	len_fmt = vsnprintf(global_buf, SMALL_BUF, fmt, ap);
	va_end(ap);

	if (len_fmt < 0) {
		goto fail;
	}

	if (len_fmt < SMALL_BUF - 3) {
		buf = global_buf;
	} else if (len_fmt < MAX_BUF_LEN - 3) {
		buf = (char *)rtos_mem_malloc(len_fmt + 4);
		if (buf == NULL) {
			goto fail;
		}

		va_start(ap, fmt);
		vsnprintf(buf, len_fmt + 4, fmt, ap);
		va_end(ap);
	} else {
		RTK_LOGE(TAG, "print string len %d exceed max buffer length : %d\n", (int)len_fmt, MAX_BUF_LEN - 1);
		goto fail;
	}

	ret = len_fmt;

	for (int i = len_fmt; i > 0; i--) {
		buf[i + 2] = buf[i - 1];
	}

	buf[0] = '[';
	buf[1] = '$';
	buf[2] = ']';

	if (out_buffer) {
		out_buffer(buf, len_fmt + 3);
	}

	if (len_fmt >= SMALL_BUF - 3) {
		rtos_mem_free(buf);
	}

fail:
	at_printf_unlock();
	return ret;
}

/**
 * @brief Output data reported by system.
 * @param data: pointer to data.
 * @param len: data len
 * @return The length of the output data or error code.
 * @retval -1: output interface not exist.
 * @retval others: The length of the output data.
 */
int at_printf_data(char *data, u32 len)
{
	if (out_buffer) {
		at_printf_lock();
		out_buffer(data, len);
		at_printf_unlock();
	} else {
		return -1;
	}

	return len;
}

int atcmd_tt_mode_start(u32 len)
{
	u32 ring_buf_size = len >= MAX_TT_HEAP_SIZE ? MAX_TT_HEAP_SIZE : len + 1;

	if (rtos_mem_get_free_heap_size() < ring_buf_size) {
		RTK_LOGE(TAG, "free heap size(%u) is not enough, exit tt mode\n", rtos_mem_get_free_heap_size());
		return -1;
	}

	if (ring_buf_size == MAX_TT_HEAP_SIZE && g_host_control_mode != AT_HOST_CONTROL_SPI) {
		g_tt_mode_check_watermark = 1;
	}

	atcmd_tt_mode_rx_ring_buf = RingBuffer_Create(NULL, ring_buf_size, LOCAL_RINGBUFF, 1);
	if (atcmd_tt_mode_rx_ring_buf == NULL) {
		RTK_LOGE(TAG, "create tt mode ring buffer fail\n");
		return -1;
	}

	// info HOST we enter tt mode now
	at_printf(ATCMD_ENTER_TT_MODE_STR);
	g_tt_mode = 1;
	RTK_LOGI(TAG, "enter tt mode\n");

	return 0;
}

int atcmd_tt_mode_get(u8 *buf, u32 len)
{
	u32 get_len = len;
	u8 *buf_temp = buf;
	u32 actual_len = 0;

	if (get_len > MAX_TT_BUF_LEN) {
		RTK_LOGD(TAG, "TT mode len exceeds MAX_TT_BUF_LEN\n");
		get_len = MAX_TT_BUF_LEN;
	}

	while (get_len != 0) {
		if (g_tt_mode_stop_flag && RingBuffer_Available(atcmd_tt_mode_rx_ring_buf) == 0) {
			break;
		}

		while (RingBuffer_Available(atcmd_tt_mode_rx_ring_buf) == 0) {
			rtos_sema_take(atcmd_tt_mode_sema, 0xFFFFFFFF);
			if (g_tt_mode_stop_flag) {
				break;
			}
		}

		actual_len = RingBuffer_Available(atcmd_tt_mode_rx_ring_buf);

		if (actual_len == 0) {
			continue;
		}

		if (g_tt_mode_check_watermark) {
			if (actual_len < MAX_TT_HEAP_SIZE * TT_MODE_LOW_WATERMARK && g_tt_mode_indicate_low_watermark == 0) {
				g_tt_mode_indicate_low_watermark = 1;
				g_tt_mode_indicate_high_watermark = 0;
				at_printf(ATCMD_TT_MODE_LOW_WATERMARK_STR);
			}
		}

		actual_len = actual_len > get_len ? get_len : actual_len;
		RingBuffer_Read(atcmd_tt_mode_rx_ring_buf, buf_temp, actual_len);
		get_len -= actual_len;
		buf_temp += actual_len;
	}

	return (buf_temp - buf);
}

void atcmd_tt_mode_end(void)
{
	g_tt_mode = 0;
	g_tt_mode_stop_flag = 0;
	g_tt_mode_check_watermark = 0;
	RingBuffer_Destroy(atcmd_tt_mode_rx_ring_buf);
	RTK_LOGI(TAG, "exit tt mode\n");
	// info HOST we exit tt mode now if needed
	//at_printf(ATCMD_EXIT_TT_MODE_STR);
}

int atcmd_wifi_config_setting(void)
{
	int ret = 0;
	char *path = NULL;
	struct stat *stat_buf = NULL;
	char *wifi_config = NULL;

	if (lfs_mount_fail) {
		ret = -1;
		goto EXIT;
	}

	char *prefix = find_vfs_tag(VFS_REGION_1);
	path = (char *)rtos_mem_zmalloc(MAX_KEY_LENGTH + 2);
	stat_buf = (struct stat *)rtos_mem_zmalloc(sizeof(struct stat));
	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:%s", prefix, "wifi_config.json");
	ret = stat(path, stat_buf);
	if (ret < 0) {
		RTK_LOGI(TAG, "wifi_config.json is not exist \r\n");
		ret = 0;
		goto EXIT;
	}

	FILE *finfo;
	finfo = fopen(path, "r");
	if (finfo == NULL) {
		RTK_LOGI(TAG, "get wifi_config.json fail \r\n");
		ret = -1;
		goto EXIT;
	}


	cJSON *wifi_ob, *country_code_ob;
	wifi_config = (char *)rtos_mem_zmalloc(stat_buf->st_size);
	ret = fread(wifi_config, stat_buf->st_size, 1, finfo);
	if (ret < 0) {
		RTK_LOGI(TAG, "get wifi_config.json fail \r\n");
		fclose(finfo);
		goto EXIT;
	}

	fclose(finfo);

	if ((wifi_ob = cJSON_Parse(wifi_config)) != NULL) {
		country_code_ob = cJSON_GetObjectItem(wifi_ob, "country_code");
		if (country_code_ob) {
			if (strlen(country_code_ob->valuestring) != 2) {
				RTK_LOGE(TAG, "country code format error \r\n");
				ret = -1;
				goto EXIT;
			} else {
				ret = wifi_set_countrycode((u8 *)country_code_ob->valuestring);
				RTK_LOGI(TAG, "wifi_set_countrycode : %s %s\r\n", country_code_ob->valuestring, ret == 0 ? "OK" : "ERROR");
			}
		}
	}

EXIT:
	if (path) {
		rtos_mem_free(path);
	}
	if (stat_buf) {
		rtos_mem_free(stat_buf);
	}
	if (wifi_config) {
		rtos_mem_free(wifi_config);
	}

	return ret;
}

void atcmd_get_pin_from_json(const cJSON *const object, const char *const string, u8 *value)
{
	cJSON *pin_ob = cJSON_GetObjectItem(object, string);
	if (pin_ob) {
		if (strstr(pin_ob->valuestring, "PA")) {
			*value = _PA_0 + atoi(&(pin_ob->valuestring[2]));
		} else if (strstr(pin_ob->valuestring, "PB")) {
			*value = _PB_0 + atoi(&(pin_ob->valuestring[2]));
		}
	}
}

int atcmd_host_control_config_setting(void)
{
	int ret;
	char *path = NULL;
	struct stat *stat_buf = NULL;
	char *atcmd_config = NULL;
	cJSON *atcmd_ob = NULL, *interface_ob;

	if (lfs_mount_fail) {
		goto DEFAULT;
	}

	char *prefix = find_vfs_tag(VFS_REGION_1);
	path = (char *)rtos_mem_zmalloc(MAX_KEY_LENGTH + 2);
	stat_buf = (struct stat *)rtos_mem_zmalloc(sizeof(struct stat));
	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:%s", prefix, "atcmd_config.json");
	ret = stat(path, stat_buf);
	if (ret < 0) {
		RTK_LOGI(TAG, "atcmd_config.json is not exist \r\n");
		goto DEFAULT;
	}

	FILE *finfo;
	finfo = fopen(path, "r");
	if (finfo == NULL) {
		RTK_LOGI(TAG, "get atcmd_config.json fail \r\n");
		goto DEFAULT;
	}

	atcmd_config = (char *)rtos_mem_zmalloc(stat_buf->st_size);
	ret = fread(atcmd_config, stat_buf->st_size, 1, finfo);
	if (ret < 0) {
		RTK_LOGI(TAG, "get atcmd_config.json fail \r\n");
		fclose(finfo);
		goto DEFAULT;
	}

	fclose(finfo);

	if ((atcmd_ob = cJSON_Parse(atcmd_config)) != NULL) {
		interface_ob = cJSON_GetObjectItem(atcmd_ob, "interface");

		if (interface_ob) {
			if (strncmp(interface_ob->valuestring, "uart", strlen(interface_ob->valuestring)) == 0)	{
				g_host_control_mode = AT_HOST_CONTROL_UART;
			} else if (strncmp(interface_ob->valuestring, "spi", strlen(interface_ob->valuestring)) == 0) {
				g_host_control_mode = AT_HOST_CONTROL_SPI;
			} else if (strncmp(interface_ob->valuestring, "sdio", strlen(interface_ob->valuestring)) == 0) {
				g_host_control_mode = AT_HOST_CONTROL_SDIO;
			} else if (strncmp(interface_ob->valuestring, "usb", strlen(interface_ob->valuestring)) == 0) {
				g_host_control_mode = AT_HOST_CONTROL_USB;
			}
		} else {
			goto DEFAULT;
		}

		if (g_host_control_mode == AT_HOST_CONTROL_UART) {
			cJSON *uart_ob, *baudrate_ob;
			if ((uart_ob = cJSON_GetObjectItem(atcmd_ob, "uart")) != NULL) {
				baudrate_ob = cJSON_GetObjectItem(uart_ob, "baudrate");
				if (baudrate_ob) {
					UART_BAUD = baudrate_ob->valueint;
				}
				atcmd_get_pin_from_json(uart_ob, "tx", &UART_TX);
				atcmd_get_pin_from_json(uart_ob, "rx", &UART_RX);
			}
		} else if (g_host_control_mode == AT_HOST_CONTROL_SPI) {
			cJSON *spi_ob, *spi_index_ob;
			if ((spi_ob = cJSON_GetObjectItem(atcmd_ob, "spi")) != NULL) {
				spi_index_ob = cJSON_GetObjectItem(spi_ob, "index");
				if (spi_index_ob) {
					SPI_INDEX = spi_index_ob->valueint == 0 ? MBED_SPI0 : MBED_SPI1;
				}

				atcmd_get_pin_from_json(spi_ob, "mosi", &SPI0_MOSI);
				atcmd_get_pin_from_json(spi_ob, "miso", &SPI0_MISO);
				atcmd_get_pin_from_json(spi_ob, "clk", &SPI0_SCLK);
				atcmd_get_pin_from_json(spi_ob, "cs", &SPI0_CS);
				atcmd_get_pin_from_json(spi_ob, "master_sync_pin", &AT_SYNC_FROM_MASTER_GPIO);
				atcmd_get_pin_from_json(spi_ob, "slave_sync_pin", &AT_SYNC_TO_MASTER_GPIO);
			}
		} else if (g_host_control_mode == AT_HOST_CONTROL_SDIO) {
#ifdef CONFIG_SUPPORT_SDIO_DEVICE
			cJSON *sdio_ob, *group_ob;
			if ((sdio_ob = cJSON_GetObjectItem(atcmd_ob, "sdio")) != NULL) {
				group_ob = cJSON_GetObjectItem(sdio_ob, "group");
				if (group_ob) {
					SDIO_Pin_Grp = group_ob->valueint;
				}
			}
#endif
		} else if (g_host_control_mode == AT_HOST_CONTROL_USB) {
			// nothing to config
		} else {
			RTK_LOGE(TAG, "g_host_control_mode is invalid\r\n");
		}
	}

DEFAULT:
	if (g_host_control_mode == AT_HOST_CONTROL_UART) {
		RTK_LOGI(TAG, "ATCMD HOST Control Mode : UART, tx:%s, ", PIN_VAL_TO_NAME_STR(UART_TX));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "rx:%s, ", PIN_VAL_TO_NAME_STR(UART_RX));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "baudrate:%d\r\n", (int)UART_BAUD);
		ret = atio_uart_init();
	} else if (g_host_control_mode == AT_HOST_CONTROL_SPI) {
		RTK_LOGI(TAG, "ATCMD HOST Control Mode : SPI, mosi:%s, ", PIN_VAL_TO_NAME_STR(SPI0_MOSI));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "miso:%s, ", PIN_VAL_TO_NAME_STR(SPI0_MISO));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "clk:%s, ", PIN_VAL_TO_NAME_STR(SPI0_SCLK));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "cs:%s, ", PIN_VAL_TO_NAME_STR(SPI0_CS));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "master_sync_pin:%s, ", PIN_VAL_TO_NAME_STR(AT_SYNC_FROM_MASTER_GPIO));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "slave_sync_pin:%s, ", PIN_VAL_TO_NAME_STR(AT_SYNC_TO_MASTER_GPIO));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "spi_index:%d\r\n", SPI_INDEX - MBED_SPI0);
		ret = atio_spi_init();
	} else if (g_host_control_mode == AT_HOST_CONTROL_SDIO) {
#ifdef CONFIG_SUPPORT_SDIO_DEVICE
		RTK_LOGI(TAG, "ATCMD HOST Control Mode : SDIO, group:%d, ", SDIO_Pin_Grp);
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "clk:%s, ", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][0]));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "cmd:%s, ", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][1]));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "d3:%s, ", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][2]));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "d2:%s, ", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][3]));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "d1:%s, ", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][4]));
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "d0:%s\r\n", PIN_VAL_TO_NAME_STR(SDIO_PAD[SDIO_Pin_Grp][5]));
		ret = atio_sdio_init();
#else
		ret = -1;
		RTK_LOGI(TAG, "NOT Support SDIO Interface!\r\n");
#endif
	} else if (g_host_control_mode == AT_HOST_CONTROL_USB) {
#ifdef CONFIG_SUPPORT_USB
		RTK_LOGI(TAG, "ATCMD HOST Control Mode : USB");
		ret = atio_usbd_init();
#else
		ret = -1;
		RTK_LOGI(TAG, "NOT Support USB Interface!\r\n");
#endif
	} else {
		RTK_LOGE(TAG, "g_host_control_mode is invalid\r\n");
	}

	if (path) {
		rtos_mem_free(path);
	}

	if (stat_buf) {
		rtos_mem_free(stat_buf);
	}

	if (atcmd_config) {
		rtos_mem_free(atcmd_config);
	}

	if (atcmd_ob) {
		rtos_mem_free(atcmd_ob);
	}

	return ret;
}

void tt_mode_timeout_handler(void *arg)
{
	// indicate tt_mode_get stop tt mode
	(void) arg;
	g_tt_mode_stop_flag = 1;
	g_tt_mode_stop_char_cnt = 0;
	rtos_sema_give(atcmd_tt_mode_sema);
}

#else

int at_printf_data(char *data, u32 len)
{
	u32 cur_len = 0;
	if (len > 0) {
		for (u32 i = 0; i < len; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%c", data[cur_len + i]);
		}
	}

	return len;
}

int atcmd_tt_mode_start(u32 len)
{
	(void) len;
	RTK_LOGI(TAG, "NOT SUPPORT TT MODE\n");
	return -1;
}

int atcmd_tt_mode_get(u8 *buf, u32 len)
{
	(void) buf;
	(void) len;
	RTK_LOGI(TAG, "NOT SUPPORT TT MODE\n");
	return 0;
}

void atcmd_tt_mode_end(void)
{
	RTK_LOGI(TAG, "NOT SUPPORT TT MODE\n");
}

#endif

void at_printf_lock()
{
	rtos_mutex_recursive_take(at_printf_mutex, RTOS_MAX_DELAY);
}

void at_printf_unlock()
{
	rtos_mutex_recursive_give(at_printf_mutex);
}


int hash_index(const char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str) {
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

void log_add_new_command(log_item_t *new)
{
	int index = hash_index(new->log_cmd) % ATC_INDEX_NUM;

	list_add(&new->node, &log_hash[index]);
}

void atcmd_service_init(void)
{
	unsigned int i;

	for (i = 0; i < ATC_INDEX_NUM; i++) {
		INIT_LIST_HEAD(&log_hash[i]);
	}

	for (i = 0; i < sizeof(log_init_table) / sizeof(log_init_t); i++) {
		log_init_table[i]();
	}

	rtos_mutex_recursive_create(&at_printf_mutex);

#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)

#if (defined CONFIG_ATCMD_HOST_CONTROL && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE))
	rtos_timer_create(&xTimers_TT_Mode, "TT_Mode_Timer", NULL, 30, FALSE, tt_mode_timeout_handler);

	//initialize tt mode ring sema
	rtos_sema_create(&atcmd_tt_mode_sema, 0, 0xFFFF);

#ifndef CONFIG_WHC_BRIDGE
	int ret = atcmd_wifi_config_setting();
	if (ret < 0) {
		RTK_LOGE(TAG, "atcmd wifi config setting fail\n");
		return;
	}

	ret = atcmd_host_control_config_setting();

	if (ret < 0) {
		RTK_LOGI(TAG, "atcmd host control config setting fail\n");
		return;
	}
#endif

	char *path = rtos_mem_zmalloc(MAX_KEY_LENGTH);
	char *prefix = find_vfs_tag(VFS_REGION_1);
	DiagSnPrintf(path, MAX_KEY_LENGTH, "%s:AT", prefix);
	mkdir(path, 0);
	rtos_mem_free(path);

	RTK_LOGI(TAG, ATCMD_HOST_CONTROL_INIT_STR);
	at_printf(ATCMD_HOST_CONTROL_INIT_STR);
#endif
#endif
}

//sizeof(log_items)/sizeof(log_items[0])
void atcmd_service_add_table(log_item_t *tbl, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		log_add_new_command(&tbl[i]);
	}
}

void *atcmd_action(char *cmd)
{
	int index = hash_index(cmd) % ATC_INDEX_NUM;
	struct list_head *head = &log_hash[index];
	struct list_head *iterator;
	log_item_t *item;
	void *act = NULL;

	list_for_each(iterator, head) {
		item = list_entry(iterator, log_item_t, node);
		if (strcmp(item->log_cmd, cmd) == 0) {
			act = (void *)item->at_act;
			break;
		}
	}

	return act;
}

void *atcmd_handler(char *cmd)
{
	const char *atcmd_prefix = "AT";
	log_act_t action = NULL;
	char *copy = cmd;
	char *token = NULL;
	char *param = NULL;
	char tok[33] = {0};//'\0'
	char *tokSearch = NULL;
	int prefix_length = strlen(atcmd_prefix);

	if (strncmp(cmd, atcmd_prefix, prefix_length) != 0) {
		return NULL;
	}

	token = strsep(&copy, "=");
	param = strsep(&copy, "\0");

	/* The length of command header should be 4 bytes at least */
	if (token && (strlen(token) > 3)) {
		strncpy(tok, token, sizeof(tok) - 1);
		tokSearch = &tok[2];
	} else {
		return NULL;
	}
	action = (log_act_t)atcmd_action(tokSearch);

	if (action) {
		action(param);
	}
	return (void *)action;

}

/****************************************************************
  Single parameter divided by comma.
  The argv[0] is reserved as NULL, argv[1] is the 1st paramerter, argv[2] is the 2nd
  parameter, and son on.
  If there are consecutive commas, it means there are parameter(s) ignored.
  So that, the mandatory parameters' length should be checked not equal to 0.
  The optional parameters' length can be 0, which means absent.
  e.g.
  AT+XXX=param1,param2		<-	There are 2 parameters.
  AT+XXX=param1,,param3		<-	The param2 has length == 0, but argv[2] is not NULL.
  If you need let the comma(s) be part(s) of parameter, use parse_param_advance instead.
****************************************************************/
int parse_param(char *buf, char **argv)
{
	int argc = 1;
	char *temp = NULL;

	if (buf == NULL) {
		goto exit;
	}
	temp = buf;

	for (; argc < MAX_ARGC; argc++) {
		argv[argc] = strsep(&temp, ",");
		if (argv[argc] == NULL) {
			goto exit;
		}
	}

exit:
	return argc;
}

/****************************************************************
  If the string parameter need include comma(s), use this one instead of parse_param.
  The argv[0] is reserved as NULL, argv[1] is the 1st paramerter, argv[2] is the 2nd
  parameter, and son on.
  If return value argc == -1, it means there are invalid datas inside.
  e.g.
    AT+XXX=param1,head\,tail,param3		<- The 2nd parameter is string "head,tail".
    AT+XXX=param1,head\\tail,param3		<- The 2nd parameter is string "head\tail".
  The single backslash is not allowed, in other words, you should use double backslashes
  such as "\\" to express single backslash.
****************************************************************/
int parse_param_advance(char *buf, char **argv)
{
	/* The last charactor should be '\0'. */
	const int most_size = UART_LOG_CMD_BUFLEN - 1;
	int argc = 1, pos = 0, i = 0, j = 0, offset = 1;

	if (buf == NULL) {
		goto exit;
	}

	for (i = 0; most_size > i && '\0' != buf[i] && MAX_ARGC > argc; i += offset) {
		/* Escape charactor. */
		if ('\\' == buf[i]) {
			/* The next charactor should be comma or backslash. */
			if (',' != buf[i + 1] && '\\' != buf[i + 1]) {
				argc = -1;
				goto exit;
			}
			buf[j] = buf[i + 1];
			offset = 2;
		}
		/* Delimiter. */
		else if (',' == buf[i]) {
			buf[j] = '\0';
			argv[argc] = &buf[pos];
			argc++;
			pos = j + 1;
			offset = 1;
		}
		/* Other charactor. */
		else {
			buf[j] = buf[i];
			offset = 1;
		}
		j++;
	}

	/* The final arg. */
	if (MAX_ARGC > argc) {
		buf[j] = '\0';
		argv[argc] = &buf[pos];
		argc++;
	} else {
		argc = -1;
		goto exit;
	}

exit:
	return argc;
}

#ifdef CONFIG_WLAN
int mp_command_handler(char *cmd)
{
	char *start = "iwpriv ";
	int len = strlen(start);
	if (strncmp(cmd, start, len) == 0) {
#ifdef CONFIG_MP_INCLUDED
#if defined(CONFIG_WHC_HOST)
		char *cmdbuf = NULL;
		cmdbuf = rtos_mem_malloc(strlen(cmd + len) + 1);
		strcpy(cmdbuf, (const char *)(cmd + len));
		whc_ipc_host_api_mp_command(cmdbuf, strlen(cmd + len) + 1, 1);
		rtos_mem_free(cmdbuf);
#elif defined(CONFIG_WHC_NONE)
		wext_private_command(cmd + len, 1, NULL);
#endif
#endif
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n\n#\r\n"); //"#" is needed for mp tool
		return TRUE;
	}

	return FALSE;
}
#endif

int atcmd_service(char *line_buf)
{
	if (atcmd_handler((char *)line_buf) == NULL) {
		return FALSE;
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[MEM] After do cmd, available heap %d\n\r", rtos_mem_get_free_heap_size());
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n\n#\r\n"); //"#" is needed for mp tool
	return TRUE;
}
#endif /* CONFIG_SUPPORT_ATCMD */
