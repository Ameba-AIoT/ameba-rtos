/**
  ******************************************************************************
  * @file    example_usbh_cdc_ecm.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2023-1-13
  * @brief   This file provides the demo code for USB CDC ECM host class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
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
extern void rltk_mii_init(void);

#define ENABLE_DUMP_FILE                        0
#define ENABLE_REMOTE_FILE_DOWNLOAD             0

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

static int cdc_ecm_do_init(void)
{
	return usbh_cdc_ecm_do_init(ethernetif_mii_recv);
}

static void ecm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	UNUSED(param);
	printf("\n[Link]enter link status task !\n");
	cdc_ecm_do_init();

	rltk_mii_init();

	while (1) {
		link_is_up = usbh_cdc_ecm_get_connect_status();

		if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
			printf("will do dhcp \n");
			ethernet_unplug = ETH_STATUS_INIT;
			mac = (u8 *)usbh_cdc_ecm_process_mac_str();
			memcpy(xnetif[NET_IF_NUM - 1].hwaddr, mac, 6);
			printf("mac[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			netif_set_link_up(&xnetif[NET_IF_NUM - 1]);

			dhcp_status = LwIP_DHCP(NET_IF_NUM - 1, DHCP_START);
			if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
				netifapi_netif_set_default(&xnetif[NET_IF_NUM - 1]);	//Set default gw to ether netif
				dhcp_done = 1;
			}
			printf("switch to link !!\n");
		} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_unplug = ETH_STATUS_DEINIT;
			netif_set_default(&xnetif[0]);
			printf("swicth to unlink !!\n");
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
	printf("Data length = %d\n", length);
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
	printf("Dump mem to flash start\n");
	FLASH_WriteStream(0x100000, configTOTAL_PSRAM_HEAP_SIZE_TEST, dump_psRAMHeap);
	printf("Dump mem to flash done\n");
#endif
}

static void ecm_download_thread(void *param)
{
	int server_fd = -1;
	u8 heart_beat = 0;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	u32 resource_size = 0 ;
	u32 content_len = 0;
	unsigned char output[8 * MD5_CHECK_BUFFER_LEN];
	int pos = 0, read_size = 0,  header_removed = 0;
	UNUSED(param);

	printf("\n[Download]enter example \n");
	memset(output, 0x00, 8 * MD5_CHECK_BUFFER_LEN);

	while (0 == dhcp_done) {
		if (++heart_beat % 30 == 0) {
			printf("Wait for ethernet connection ...\n");
		}
		usb_os_sleep_ms(1000);
	}

	printf("\n[Download]start HTTP download \n");

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("ERROR: socket\n");
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
	printf("[Download]%s Line %d server_fd=%d \n", __func__, __LINE__, server_fd);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(SERVER_HOST);
	if (server_host != NULL) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		printf("ERROR: server host\n");
		goto exit;
	}

	printf("[Download]will do connect %s\n", SERVER_HOST);
	if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;
		printf("[Download]connect success \n");
		sprintf((char *)dl_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		u32 max = strlen((char const *)dl_buf);

		if (0) {
			u32 index = 0 ;
			printf("\n[Download][get Msg %ld]", max);
			for (index = 0; index < max; index++) {
				printf("0x%x ", dl_buf[index]);
			}
			printf("\n\n\n");
		}

		write(server_fd, (char const *)dl_buf, strlen((char const *)dl_buf));
		printf("[Download]will call read \n");
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
					printf("\nHTTP Header: %s\n", dl_buf);

					// Remove header size to get first read size of data from body head
					read_size = pos - ((unsigned char *) body - dl_buf);
					update_data_check(body, read_size);
					printf("[Download]body=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", body[0], body[1], body[2], body[3], body[4], body[5], body[6], body[7]);
					pos = 0;
					content_len_pos = strstr((char const *)dl_buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(char *)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
						printf("[Download]content len: %ld\n", content_len);
					}
				} else {
					if (pos >= BUFFER_SIZE) {
						printf("ERROR: HTTP header\n");
						goto exit;
					}
					continue;
				}
			} else {
				update_data_check((char *)(dl_buf + pos), (unsigned int)read_size);
			}

			resource_size += read_size;
			printf("[Download]read resource %d bytes/receive=%ld/total=%ld ------------------------------------------------\n",
				   read_size, resource_size, content_len);
		}

		write_data_to_flash();
		mbedtls_md5_finish(&ctx, output);
		mbedtls_md5_free(&ctx);

		printf("[Download]mbedtls_md5_finish md5 \n");
		for (u8 hh = 0; hh < MD5_CHECK_BUFFER_LEN; hh++) {
			printf("[Download]md5 %d=%02x%02x%02x%02x%02x%02x%02x%02x\n\n", hh,
				   output[8 * hh + 0], output[8 * hh + 1], output[8 * hh + 2], output[8 * hh + 3],
				   output[8 * hh + 4], output[8 * hh + 5], output[8 * hh + 6], output[8 * hh + 7]);
		}

		printf("[Download]exit read. ret = %d/%d\n", read_size, __LINE__);
		printf("[Download]http content-length = %ld bytes, download resource size = %ld bytes/%d\n", content_len, resource_size, __LINE__);
		if (0) {
			u32 index = 0 ;
			printf("\n[Download][2get Msg %ld]", max);
			for (index = 0; index < max; index++) {
				printf("0x%x ", dl_buf[index]);
			}
			printf("\n\nwill call write \n");
		}
	} else {
		printf("[Download]ERROR: connect\n");
	}

exit:
	if (server_fd >= 0) {
		close(server_fd);
	}

	rtos_task_delete(NULL);
}
#endif

/* Exported functions --------------------------------------------------------*/
void example_usbh_cdc_ecm(void)
{
	int status;
	rtos_task_t monitor_task;
#if ENABLE_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
#endif
	printf("\n[CDC_ECM] USB host cdc_ecm demo started...\n");

	status = rtos_task_create(&monitor_task, "ecm_link_thread", ecm_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != SUCCESS) {
		printf("\n[CDC_ECM] Fail to create USB host monitor_link_change thread: %d\n", status);
	}
#if ENABLE_REMOTE_FILE_DOWNLOAD
	status = rtos_task_create(&download_task, "ecm_download_thread", ecm_download_thread, NULL, 10 * 512U, 2U);
	if (status != SUCCESS) {
		printf("\n[USBH] Fail to create USBH download cdc_ecm_test_task thread\n");
	}
#endif
}
