/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include <platform_autoconf.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "os_wrapper.h"
#include "usbh.h"
#include "mbedtls/md5.h"
#include "lwip_netconf.h"
#include "usbh_cdc_ecm_hal.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "ECM";

extern void rltk_mii_init(void);
extern void rltk_mii_deinit(void);


/*enable this used to check ecm init/deinit memory leakage*/
#define ENABLE_ECM_MEM_CHECK                    0

#define ENABLE_DUMP_FILE                        0
#define ENABLE_REMOTE_FILE_DOWNLOAD             0

#define ENABLE_USER_SET_DONGLE_MAC              0

#if ENABLE_REMOTE_FILE_DOWNLOAD
#define MD5_CHECK_BUFFER_LEN                    (2)
/* socket server info */
#define SERVER_HOST                             "www.baidu.com"
#define SERVER_PORT                             80
#define RESOURCE                                "/"
#define BUFFER_SIZE                             1000      //download test buffer length
#define RECV_TO                                 60*1000   // ms
#endif

/* Private types -------------------------------------------------------------*/
/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_DEINIT,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} eth_state_t;

/* Private variables ---------------------------------------------------------*/
#if ENABLE_REMOTE_FILE_DOWNLOAD
static unsigned char dl_buf[BUFFER_SIZE + 1];
static mbedtls_md5_context ctx;
#if ENABLE_DUMP_FILE
#define configTOTAL_PSRAM_HEAP_SIZE_TEST        (29000)
static unsigned char dump_psRAMHeap[configTOTAL_PSRAM_HEAP_SIZE_TEST];
#endif
#endif  //ENABLE_REMOTE_FILE_DOWNLOAD

static u8 dhcp_done = 0;
extern struct netif xnetif[NET_IF_NUM];
extern struct netif eth_netif;

#if ENABLE_USER_SET_DONGLE_MAC
static u16 led_color[1] = {0x1122};
static u8 mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static usbh_cdc_ecm_priv_data_t ecm_priv = {
	mac_valid,
	led_color,
	sizeof(led_color) / sizeof(led_color[0]),
};
#endif
static int cdc_ecm_do_init(void)
{
#if ENABLE_USER_SET_DONGLE_MAC
	return usbh_cdc_ecm_do_init(ethernetif_mii_recv, &ecm_priv);
#else
	return usbh_cdc_ecm_do_init(ethernetif_mii_recv, NULL);
#endif
}

static void ecm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");
	cdc_ecm_do_init();

	rltk_mii_init();

	while (1) {
		link_is_up = usbh_cdc_ecm_get_connect_status();

		if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
			RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
			ethernet_unplug = ETH_STATUS_INIT;
			mac = (u8 *)usbh_cdc_ecm_process_mac_str();
			memcpy(eth_netif.hwaddr, mac, 6);
			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			netif_set_link_up(&eth_netif);

			dhcp_status = LwIP_DHCP(2, DHCP_START);
			if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
				netifapi_netif_set_default(&eth_netif);	//Set default gw to ether netif
				dhcp_done = 1;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
		} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_unplug = ETH_STATUS_DEINIT;
			netif_set_default(&xnetif[0]);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to unlink\n");
			//usbh_cdc_ecm_do_deinit();
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

#if ENABLE_REMOTE_FILE_DOWNLOAD
static void save_to_memory(char *pdata, unsigned int length)
{
#if ENABLE_DUMP_FILE
	static unsigned int psram_pos = 0;
	if (configTOTAL_PSRAM_HEAP_SIZE_TEST >= psram_pos + length) {
		memcpy((void *)&dump_psRAMHeap[psram_pos], pdata, length);
		psram_pos += length;
	}
#else
	UNUSED(length);
	UNUSED(pdata);
#endif
}

static int update_data_check(char *pdata, unsigned int length)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d\n", length);
	if (0 == length) {
		return 0;
	}
	save_to_memory(pdata, length);
	mbedtls_md5_update(&ctx, (const unsigned char *)pdata, length);
	return  0;
}

static void write_data_to_flash(void)
{
#if ENABLE_DUMP_FILE
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash start\n");
	FLASH_WriteStream(0x100000, configTOTAL_PSRAM_HEAP_SIZE_TEST, dump_psRAMHeap);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash done\n");
#endif
}

static void ecm_download_thread(void *param)
{
	int server_fd = -1;
	u8 heart_beat = 0;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	u32 resource_size = 0;
	u32 content_len = 0;
	unsigned char output[8 * MD5_CHECK_BUFFER_LEN];
	int pos = 0, read_size = 0,  header_removed = 0;
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter donwload example\n");
	memset(output, 0x00, 8 * MD5_CHECK_BUFFER_LEN);

	while (0 == dhcp_done) {
		if (++heart_beat % 30 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for ethernet connect ...\n");
		}
		usb_os_sleep_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start HTTP download\n");

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Socket\n");
		goto exit;
	} else {
		int recv_timeout_ms = RECV_TO;
		// lwip 1.5.0
		struct timeval recv_timeout;
		recv_timeout.tv_sec = recv_timeout_ms / 1000;
		recv_timeout.tv_usec = recv_timeout_ms % 1000 * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
		// lwip 1.4.1
		//setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms));
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Server_fd=%d\n", server_fd);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(SERVER_HOST);
	if (server_host != NULL) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Server host\n");
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Will do connect %s\n", SERVER_HOST);
	if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Connect success\n");
		sprintf((char *)dl_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		u32 max = strlen((char const *)dl_buf);

		if (0) {
			u32 index = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get Msg %d", max);
			for (index = 0; index < max; index++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "0x%x ", dl_buf[index]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n\n");
		}

		write(server_fd, (char const *)dl_buf, strlen((char const *)dl_buf));
		RTK_LOGS(TAG, RTK_LOG_INFO, "Will call read\n");
		mbedtls_md5_init(&ctx);
		mbedtls_md5_starts(&ctx);

		while (((content_len == 0) || (resource_size < content_len)) /**/
			   && ((read_size = read(server_fd, dl_buf + pos, BUFFER_SIZE - pos)) > 0)) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				dl_buf[pos] = 0;
				header = strstr((char const *)dl_buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "HTTP Header: %s\n", dl_buf);

					// Remove header size to get first read size of data from body head
					read_size = pos - ((unsigned char *) body - dl_buf);
					update_data_check(body, read_size);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Body=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", body[0], body[1], body[2], body[3], body[4], body[5], body[6], body[7]);
					pos = 0;
					content_len_pos = strstr((char const *)dl_buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(char *)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
						RTK_LOGS(TAG, RTK_LOG_INFO, "Content len: %d\n", content_len);
					}
				} else {
					if (pos >= BUFFER_SIZE) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "HTTP header\n");
						goto exit;
					}
					continue;
				}
			} else {
				update_data_check((char *)(dl_buf + pos), (unsigned int)read_size);
			}

			resource_size += read_size;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Read resource %dB/rx=%d/total=%d\n",
					 read_size, resource_size, content_len);
		}

		write_data_to_flash();
		mbedtls_md5_finish(&ctx, output);
		mbedtls_md5_free(&ctx);

		RTK_LOGS(TAG, RTK_LOG_INFO, "mbedtls_md5_finish md5\n");
		for (u8 hh = 0; hh < MD5_CHECK_BUFFER_LEN; hh++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "md5 %d=%02x%02x%02x%02x%02x%02x%02x%02x\n\n", hh,
					 output[8 * hh + 0], output[8 * hh + 1], output[8 * hh + 2], output[8 * hh + 3],
					 output[8 * hh + 4], output[8 * hh + 5], output[8 * hh + 6], output[8 * hh + 7]);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Exit read. ret = %d\n", read_size);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Http content-len = %dB, download resource size = %dB\n", content_len, resource_size);
		if (0) {
			u32 index = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "2get Msg %d", max);
			for (index = 0; index < max; index++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "0x%x ", dl_buf[index]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n will call write \n");
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Connect\n");
	}

exit:
	if (server_fd >= 0) {
		close(server_fd);
	}

	rtos_task_delete(NULL);
}
#endif

#if ENABLE_ECM_MEM_CHECK
static void usbh_ecm_mem_check_thread(void *param)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "[test]ecm_link_toggle_thread \n");
	rtos_task_t monitor_task;
	int loop = 0;
	int status;

	UNUSED(param);

	while (1) {
		status = rtos_task_create(&monitor_task, "usbh_ecm_link_thread", ecm_link_change_thread, NULL, 1024U * 2, 3U);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop create %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
		rtos_time_delay_ms(5000);

		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Fail to create ECM monitor_link_change thread: %d\n", status);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Del monitor_task\n");
			rtos_task_delete(monitor_task);
			rltk_mii_deinit();

			usbh_cdc_ecm_do_deinit();
			rtos_time_delay_ms(3000);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Loop delete %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
			loop++;
		}
	}
}
#endif

/* Exported functions --------------------------------------------------------*/
#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET
void example_usbh_cdc_ecm(void)
{
	int status;
	rtos_task_t monitor_task;
#if ENABLE_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH ECM demo start\n");

#if ENABLE_ECM_MEM_CHECK
	status = rtos_task_create(&monitor_task, "usbh_ecm_mem_check_thread", usbh_ecm_mem_check_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
#else
	status = rtos_task_create(&monitor_task, "usbh_ecm_link_thread", ecm_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
#endif

#if ENABLE_REMOTE_FILE_DOWNLOAD
	status = rtos_task_create(&download_task, "usbh_ecm_download_thread", ecm_download_thread, NULL, 10 * 512U, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create download thread fail\n");
	}
#endif

#if ECM_STATE_DEBUG_ENABLE
	extern void ecm_debug_thread(void *param);
	rtos_task_t ecm_debug_task;
	status = rtos_task_create(&ecm_debug_task, "usbh_ecm_debug_task", ecm_debug_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[ECM] Create monitor_link thread fail\n");
	}
#endif
}
#else
#error "No Lwip USB Ethernet Configuration"
#endif
