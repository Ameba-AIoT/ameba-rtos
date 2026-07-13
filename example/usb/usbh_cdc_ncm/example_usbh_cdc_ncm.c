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
#include "wifi_api.h"
#include "mbedtls/md5.h"
#include "lwip_netconf.h"
#include "usbh_cdc_ncm.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "NCM";

extern void rltk_usb_eth_init(void);
extern void rltk_usb_eth_deinit(void);

#define CONFIG_USBH_CDC_NCM_HOTPLUG                            1     /* Hot plug */

#define CONFIG_USBH_CDC_NCM_MAIN_THREAD_PRIORITY               5
#define CONFIG_USBH_CDC_NCM_HOTPLUG_THREAD_PRIORITY            6
#define CONFIG_USBH_CDC_NCM_ISR_PRIORITY                       INT_PRI_MIDDLE

/*enable this used to check ncm init/deinit memory leakage*/
#define CONFIG_USBH_CDC_NCM_MEM_CHECK                          0

#define CONFIG_USBH_CDC_NCM_DUMP_FILE                          0
#define CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD               0

#define CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC                1

#if CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD
#define MD5_CHECK_BUFFER_LEN                    (2)
/* socket server info */
#define SERVER_HOST                             "www.baidu.com"
#define SERVER_PORT                             80
#define RESOURCE                                "/"
#define BUFFER_SIZE                             1000      //download test buffer length
#define RECV_TO                                 60*1000   // ms
#endif


/* Private variables ---------------------------------------------------------*/
#if CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD
static unsigned char cdc_ncm_dl_buf[BUFFER_SIZE + 1];
static mbedtls_md5_context ctx;
#if CONFIG_USBH_CDC_NCM_DUMP_FILE
#define CONFIG_USBH_CDC_NCM_DUMP_PSRAM_HEAP_SIZE               (29000)
static unsigned char cdc_ncm_dump_psram_heap[CONFIG_USBH_CDC_NCM_DUMP_PSRAM_HEAP_SIZE];
#endif
#endif  //CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD

#if CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC
static const u8 cdc_ncm_mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

static u8 cdc_ncm_dhcp_done = 0;
static __IO u8 cdc_ncm_detach_pending = 0;
static usb_os_sema_t cdc_ncm_detach_sema;
#if CONFIG_USBH_CDC_NCM_HOTPLUG
rtos_task_t 	            cdc_ncm_hotplug_task;
#endif

/* Private types -------------------------------------------------------------*/
/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} eth_state_t;

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ncm_cb_init(void);
static int usbh_cdc_ncm_cb_deinit(void);
static int usbh_cdc_ncm_cb_attach(void);
static int usbh_cdc_ncm_cb_detach(void);
static int usbh_cdc_ncm_cb_setup(void);
static int usbh_cdc_ncm_cb_process(usb_host_t *host, u8 msg);
static int usbh_cdc_ncm_cb_bulk_receive(u8 *pbuf, u32 Len);
static int usbh_cdc_ncm_cb_device_check(usb_host_t *host, u8 cfg_max);

static const usbh_config_t usbh_ncm_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = CONFIG_USBH_CDC_NCM_ISR_PRIORITY,
	.main_task_stack_size = 1280U,
	.main_task_priority = CONFIG_USBH_CDC_NCM_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.hub_support = 1U,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static const usbh_cdc_ncm_state_cb_t cdc_ncm_usb_cb = {
	.init   = usbh_cdc_ncm_cb_init,
	.deinit = usbh_cdc_ncm_cb_deinit,
	.attach = usbh_cdc_ncm_cb_attach,
	.detach = usbh_cdc_ncm_cb_detach,
	.setup  = usbh_cdc_ncm_cb_setup,
	.bulk_received = usbh_cdc_ncm_cb_bulk_receive,
};

static const usbh_user_cb_t usbh_ncm_usr_cb = {
	.process = usbh_cdc_ncm_cb_process,
	.validate = usbh_cdc_ncm_cb_device_check,
};

static const usbh_cdc_ncm_priv_data_t cdc_ncm_priv = {
#if CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC
	NULL,       /* led_array - not used in NCM */
	cdc_ncm_mac_valid,  /* mac_value */
	0,          /* led_cnt */
#else
	NULL,
	NULL,
	0,
#endif
};

/**
  * @brief  Validates whether the connected USB device supports CDC NCM.
  * @note   Called by the USB host stack during enumeration when multiple devices
  *         are connected to a hub.  Delegates to usbh_cdc_ncm_check_config_desc()
  *         to verify the configuration descriptor contains both the NCM
  *         Communication Interface and the NCM Data Interface.
  * @param[in] host: USB host handle.
  * @param[in] cfg_max: Number of configurations (unused).
  * @return HAL_OK if the device is a valid CDC NCM device, non-zero otherwise.
  */
static int usbh_cdc_ncm_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ncm_check_config_desc(host);
}

/**
  * @brief  CDC NCM init callback
  * @retval Status
  */
static int usbh_cdc_ncm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

/**
  * @brief  CDC NCM deinit callback
  * @retval Status
  */
static int usbh_cdc_ncm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

/**
  * @brief  CDC NCM attach callback
  * @retval Status
  */
static int usbh_cdc_ncm_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	return HAL_OK;
}

/**
  * @brief  CDC NCM detach callback
  * @retval Status
  */
static int usbh_cdc_ncm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");

	cdc_ncm_detach_pending = 1;
#if CONFIG_USBH_CDC_NCM_HOTPLUG
	usb_os_sema_give(cdc_ncm_detach_sema);
#endif
	return HAL_OK;
}

/**
  * @brief  CDC NCM setup callback
  * @retval Status
  */
static int usbh_cdc_ncm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	return HAL_OK;
}

/**
  * @brief  CDC NCM bulk receive callback
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  * @retval Status
  */
static int usbh_cdc_ncm_cb_bulk_receive(u8 *buf, u32 length)
{
	if (length > 0) {
		netif_adapter_usb_eth_recv(buf, length);
	}

	return HAL_OK;
}

static int usbh_cdc_ncm_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_cdc_ncm_choose_config(host);
		break;
	case USBH_MSG_DISCONNECTED:
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static int usbh_cdc_ncm_do_init(void)
{
	int ret = 0;

	ret = usbh_init(&usbh_ncm_cfg, &usbh_ncm_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return 0;
	}

	ret = usbh_cdc_ncm_init(&cdc_ncm_usb_cb, &cdc_ncm_priv);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC NCM fail\n");
		usbh_deinit();
		return 0;
	}

	do {
		if (usbh_cdc_ncm_is_configured()) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success

	return 1;
}

static void usbh_ncm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	eth_state_t ethernet_state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	if (usbh_cdc_ncm_do_init() == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB init fail\n");

		rtos_task_delete(NULL);
	}

	while (1) {
		if (cdc_ncm_detach_pending) {
			cdc_ncm_detach_pending = 0;
			if (ethernet_state >= ETH_STATUS_INIT) {
				ethernet_state = ETH_STATUS_IDLE;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink (detach event)\n");
			}
		}

		link_is_up = usbh_cdc_ncm_get_link_status();

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {	// unlink -> link
			mac = (u8 *)usbh_cdc_ncm_get_mac_str();
			if (mac == NULL) {
				rtos_time_delay_ms(1000);
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				ethernet_state = ETH_STATUS_INIT;
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to ether netif
					cdc_ncm_dhcp_done = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Fail\n");
				}
			}
		} else if (0 == link_is_up && (ethernet_state >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_state = ETH_STATUS_IDLE;
			netif_set_default(pnetif_sta);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

#if CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD
static void usbh_cdc_ncm_save_to_memory(char *pdata, unsigned int length)
{
#if CONFIG_USBH_CDC_NCM_DUMP_FILE
	static unsigned int psram_pos = 0;
	if (CONFIG_USBH_CDC_NCM_DUMP_PSRAM_HEAP_SIZE >= psram_pos + length) {
		memcpy((void *)&cdc_ncm_dump_psram_heap[psram_pos], pdata, length);
		psram_pos += length;
	}
#else
	UNUSED(length);
	UNUSED(pdata);
#endif
}

static int usbh_cdc_ncm_update_data_check(char *pdata, unsigned int length)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d\n", length);
	if (0 == length) {
		return 0;
	}
	usbh_cdc_ncm_save_to_memory(pdata, length);
	mbedtls_md5_update(&ctx, (const unsigned char *)pdata, length);
	return  0;
}

static void usbh_cdc_ncm_write_data_to_flash(void)
{
#if CONFIG_USBH_CDC_NCM_DUMP_FILE
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash start\n");
	FLASH_WriteStream(0x100000, CONFIG_USBH_CDC_NCM_DUMP_PSRAM_HEAP_SIZE, cdc_ncm_dump_psram_heap);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash done\n");
#endif
}

static void usbh_ncm_download_thread(void *param)
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

	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter download example\n");
	memset(output, 0x00, 8 * MD5_CHECK_BUFFER_LEN);

	while (0 == cdc_ncm_dhcp_done) {
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
		struct timeval recv_timeout;
		recv_timeout.tv_sec = recv_timeout_ms / 1000;
		recv_timeout.tv_usec = recv_timeout_ms % 1000 * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Server_fd=%d\n", server_fd);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

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
		sprintf((char *)cdc_ncm_dl_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		u32 max = strlen((char const *)cdc_ncm_dl_buf);

		write(server_fd, (char const *)cdc_ncm_dl_buf, strlen((char const *)cdc_ncm_dl_buf));
		RTK_LOGS(TAG, RTK_LOG_INFO, "Will call read\n");
		mbedtls_md5_init(&ctx);
		mbedtls_md5_starts(&ctx);

		while (((content_len == 0) || (resource_size < content_len))
			   && ((read_size = read(server_fd, cdc_ncm_dl_buf + pos, BUFFER_SIZE - pos)) > 0)) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				cdc_ncm_dl_buf[pos] = 0;
				header = strstr((char const *)cdc_ncm_dl_buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "HTTP Header: %s\n", cdc_ncm_dl_buf);

					read_size = pos - ((unsigned char *) body - cdc_ncm_dl_buf);
					usbh_cdc_ncm_update_data_check(body, read_size);
					pos = 0;
					content_len_pos = strstr((char const *)cdc_ncm_dl_buf, "Content-Length: ");
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
				usbh_cdc_ncm_update_data_check((char *)(cdc_ncm_dl_buf + pos), (unsigned int)read_size);
			}

			resource_size += read_size;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Read resource %dB/rx=%d/total=%d\n",
					 read_size, resource_size, content_len);
		}

		usbh_cdc_ncm_write_data_to_flash();
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

#if CONFIG_USBH_CDC_NCM_MEM_CHECK
static void usbh_ncm_mem_check_thread(void *param)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "[test]ncm_link_toggle_thread \n");
	rtos_task_t monitor_task;
	int loop = 0;
	int status;

	UNUSED(param);

	while (1) {
		status = rtos_task_create(&monitor_task, "usbh_ncm_link_change_thread", usbh_ncm_link_change_thread, NULL, 1024U * 2, 3U);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop create %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
		rtos_time_delay_ms(5000);

		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Fail to create NCM monitor_link_change thread: %d\n", status);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Del monitor_task\n");
			rtos_task_delete(monitor_task);

			usbh_cdc_ncm_deinit();
			usbh_deinit();

			rtos_time_delay_ms(3000);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Loop delete %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
			loop++;
		}
	}
}
#endif

#if CONFIG_USBH_CDC_NCM_HOTPLUG
static void usbh_ncm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		usb_os_sema_take(cdc_ncm_detach_sema, USB_OS_SEMA_TIMEOUT);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Hot plug\n");
		//stop isr
		usbh_cdc_ncm_deinit();
		usbh_deinit();

		rtos_time_delay_ms(10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap size: 0x%08x\n", usb_os_get_free_heap_size());

		ret = usbh_init(&usbh_ncm_cfg, &usbh_ncm_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
			break;
		}

		ret = usbh_cdc_ncm_init(&cdc_ncm_usb_cb, &cdc_ncm_priv);
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC NCM fail\n");
			usbh_deinit();
			break;
		}
	}
}
#endif

/* Exported functions --------------------------------------------------------*/
#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ncm_transmit(buf, len, block);
}

void example_usbh_cdc_ncm(void)
{
	int status;
	rtos_task_t monitor_task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH NCM demo start\n");

	usb_os_sema_create(&cdc_ncm_detach_sema);
	rltk_usb_eth_init();

#if CONFIG_USBH_CDC_NCM_HOTPLUG
	status = rtos_task_create(&cdc_ncm_hotplug_task, "usbh_ncm_hotplug_thread", usbh_ncm_hotplug_thread, NULL, 1024U,
							  CONFIG_USBH_CDC_NCM_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
	}
#endif

#if CONFIG_USBH_CDC_NCM_MEM_CHECK
	status = rtos_task_create(&monitor_task, "usbh_ncm_mem_check_thread", usbh_ncm_mem_check_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
#else
	status = rtos_task_create(&monitor_task, "usbh_ncm_link_change_thread", usbh_ncm_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

#if CONFIG_USBH_CDC_NCM_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
	status = rtos_task_create(&download_task, "usbh_ncm_download_thread", usbh_ncm_download_thread, NULL, 10 * 512U, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create download thread fail\n");
	}
#endif

#endif
}
#else
void example_usbh_cdc_ncm(void)
{
	RTK_LOGS(TAG, RTK_LOG_WARN, "USBH NCM example: CONFIG_LWIP_USB_ETHERNET not enabled, skipping\n");
}
#endif /* defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET */
