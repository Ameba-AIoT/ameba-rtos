/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "basic_types.h"
#include "usbd_cdc_ncm.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "dhcp/dhcps.h"

/* Private defines -----------------------------------------------------------*/

/*
	This configuration is used to enable a thread to check hotplug event
	and reset USB stack to avoid memory leak, only for example.
*/
#define CONFIG_USBD_CDC_NCM_HOTPLUG                            1
// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_CDC_NCM_SPEED                              USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_NCM_SPEED                              USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_USBD_CDC_NCM_INIT_THREAD_PRIORITY               5
#define CONFIG_USBD_CDC_NCM_LINK_STATE_THREAD_PRIORITY         4
#define CONFIG_USBD_CDC_NCM_HOTPLUG_THREAD_PRIORITY            8

// Thread stack sizes
#define CONFIG_USBD_CDC_NCM_INIT_THREAD_STACK_SIZE             2048U
#define CONFIG_USBD_CDC_NCM_HOTPLUG_THREAD_STACK_SIZE          2048U
#define CONFIG_USBD_CDC_NCM_LINK_STATE_THREAD_STACK_SIZE       2048U

// USB NCM Device IP Configuration
#define CONFIG_USBD_CDC_NCM_IP_ADDR0                           192
#define CONFIG_USBD_CDC_NCM_IP_ADDR1                           168
#define CONFIG_USBD_CDC_NCM_IP_ADDR2                           45
#define CONFIG_USBD_CDC_NCM_IP_ADDR3                           1

#define CONFIG_USBD_CDC_NCM_NETMASK_ADDR0                      255
#define CONFIG_USBD_CDC_NCM_NETMASK_ADDR1                      255
#define CONFIG_USBD_CDC_NCM_NETMASK_ADDR2                      255
#define CONFIG_USBD_CDC_NCM_NETMASK_ADDR3                      0

#define CONFIG_USBD_CDC_NCM_GW_ADDR0                           192
#define CONFIG_USBD_CDC_NCM_GW_ADDR1                           168
#define CONFIG_USBD_CDC_NCM_GW_ADDR2                           45
#define CONFIG_USBD_CDC_NCM_GW_ADDR3                           1

/* Private types -------------------------------------------------------------*/
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_DEINIT,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} eth_state_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
extern void rltk_usb_eth_init(void);
extern void rltk_usb_eth_deinit(void);

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block);

static int usbd_cdc_ncm_cb_init(void);
static int usbd_cdc_ncm_cb_deinit(void);
static int usbd_cdc_ncm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int usbd_cdc_ncm_cb_received(u8 *buf, u32 Len);
static void usbd_cdc_ncm_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "NCM";

extern struct netif *pnetif_usb_eth;
static u8 cdc_ncm_dhcp_server_started = 0;
/*
 * cdc_ncm_dongle_mac: MAC address reported to the USB host side of the CDC-NCM link.
 * It is passed into cdc_ncm_priv and advertised through the NCM functional descriptor
 * (iMACAddress string), so the host's virtual Ethernet adapter is assigned this MAC.
 * In other words, it identifies the "host-facing" end of the USB Ethernet dongle.
 */
static const u8 cdc_ncm_dongle_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
/*
 * cdc_ncm_dhcp_server_mac: MAC address used by the device's local lwIP netif (pnetif_usb_eth).
 * It is copied into pnetif_usb_eth->hwaddr in usbd_ncm_link_change_thread() before the
 * DHCP server is started, so it is the source MAC for frames the device sends to the
 * host (DHCP offers, ARP replies, gateway traffic). This is the "device-facing" end.
 *
 * Note: cdc_ncm_dongle_mac and cdc_ncm_dhcp_server_mac must differ (here only the last byte: 0x55 vs
 * 0x56) so the two ends of the point-to-point USB Ethernet link have distinct MACs.
 */
static const u8 cdc_ncm_dhcp_server_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x56};
static __IO u8 cdc_ncm_link_disconnected = 0;

static const usbd_cdc_ncm_priv_data_t cdc_ncm_priv = {
	cdc_ncm_dongle_mac,
};

static const usbd_cdc_ncm_cb_t cdc_ncm_cb = {
	.priv = &cdc_ncm_priv,
	.init = usbd_cdc_ncm_cb_init,
	.deinit = usbd_cdc_ncm_cb_deinit,
	.setup = usbd_cdc_ncm_cb_setup,
	.received = usbd_cdc_ncm_cb_received,
	.status_changed = usbd_cdc_ncm_cb_status_changed,
};

static const usbd_config_t cdc_ncm_cfg = {
	.speed = CONFIG_USBD_CDC_NCM_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 644U,
	.ptx_fifo_depth = {16U, 256U, 32U, 16U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#endif
};

#if CONFIG_USBD_CDC_NCM_HOTPLUG
static __IO u8 cdc_ncm_attach_status = USBD_ATTACH_STATUS_INIT;
static __IO u8 cdc_ncm_attach_old_status = USBD_ATTACH_STATUS_INIT;
static rtos_sema_t cdc_ncm_attach_status_changed_sema = NULL;
static __IO u8 cdc_ncm_hotplug_thread_running = 0;
#endif

/* Private functions ---------------------------------------------------------*/
static void usbd_ncm_link_change_thread(void *param)
{
	eth_state_t ethernet_state = ETH_STATUS_IDLE;
	u8 link_is_up = 0;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	while (1) {
		link_is_up = usbd_cdc_ncm_get_link_status();

		if (cdc_ncm_link_disconnected) {
			cdc_ncm_link_disconnected = 0;
			link_is_up = 0;
		}

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {
			if (pnetif_usb_eth == NULL) {
				rtos_time_delay_ms(1000);
			} else {
				if (!cdc_ncm_dhcp_server_started) {
					// RTK_LOGS(TAG, RTK_LOG_INFO, "Starting USB NCM DHCP Server...\n");

					// 1. Set netif MAC address
					memcpy(pnetif_usb_eth->hwaddr, cdc_ncm_dhcp_server_mac, 6);
					pnetif_usb_eth->hwaddr_len = ETHARP_HWADDR_LEN;
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server MAC: " MAC_FMT "\n", MAC_ARG(pnetif_usb_eth->hwaddr));

					// 2. Deinit DHCP server
					dhcps_deinit(pnetif_usb_eth);

					// 3. Set Device IP address
					u32 ip_addr = CONCAT_TO_UINT32(CONFIG_USBD_CDC_NCM_IP_ADDR0, CONFIG_USBD_CDC_NCM_IP_ADDR1, CONFIG_USBD_CDC_NCM_IP_ADDR2, CONFIG_USBD_CDC_NCM_IP_ADDR3);
					u32 netmask = CONCAT_TO_UINT32(CONFIG_USBD_CDC_NCM_NETMASK_ADDR0, CONFIG_USBD_CDC_NCM_NETMASK_ADDR1, CONFIG_USBD_CDC_NCM_NETMASK_ADDR2,
												   CONFIG_USBD_CDC_NCM_NETMASK_ADDR3);
					u32 gw = CONCAT_TO_UINT32(CONFIG_USBD_CDC_NCM_GW_ADDR0, CONFIG_USBD_CDC_NCM_GW_ADDR1, CONFIG_USBD_CDC_NCM_GW_ADDR2, CONFIG_USBD_CDC_NCM_GW_ADDR3);
					lwip_set_ip(NETIF_USB_ETH_INDEX, ip_addr, netmask, gw);

					RTK_LOGS(TAG, RTK_LOG_INFO, "Device IP: %d.%d.%d.%d\n", CONFIG_USBD_CDC_NCM_IP_ADDR0, CONFIG_USBD_CDC_NCM_IP_ADDR1, CONFIG_USBD_CDC_NCM_IP_ADDR2,
							 CONFIG_USBD_CDC_NCM_IP_ADDR3);

					// 4. Activate network interface link
					netifapi_netif_set_link_up(pnetif_usb_eth);
					netifapi_netif_set_up(pnetif_usb_eth);
					netifapi_netif_set_default(pnetif_usb_eth);

					// 5. Initialize and start DHCP server
					dhcps_init(pnetif_usb_eth);
					dhcps_start(pnetif_usb_eth);

					cdc_ncm_dhcp_server_started = 1;
					ethernet_state = ETH_STATUS_INIT;

					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server started\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server already started\n");
					rtos_time_delay_ms(1000);
				}
			}
		} else if (0 == link_is_up && (ethernet_state >= ETH_STATUS_INIT)) {
			ethernet_state = ETH_STATUS_DEINIT;
			// USB disconnected, stop DHCP server
			if (cdc_ncm_dhcp_server_started) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Stopping USB NCM DHCP Server...\n");

				// 1. Stop DHCP service first
				dhcps_stop(pnetif_usb_eth);
				dhcps_deinit(pnetif_usb_eth);

				// 2. Bring down network interface (similar to WHC stop ap)
				netifapi_netif_set_down(pnetif_usb_eth);
				netifapi_netif_set_link_down(pnetif_usb_eth);

				cdc_ncm_dhcp_server_started = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server stopped\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server not started\n");
				rtos_time_delay_ms(1000);
			}
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

/**
  * @brief  Initializes the CDC NCM media layer
  * @retval Status
  */
static int usbd_cdc_ncm_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC NCM media layer
  * @retval Status
  */
static int usbd_cdc_ncm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint (ethernet frame extracted from NTB)
  * @param  buf: RX buffer (raw ethernet frame, NTB already parsed by driver)
  * @param  length: RX data length (in bytes)
  * @retval Status
  */
static int usbd_cdc_ncm_cb_received(u8 *buf, u32 length)
{
	if (buf == NULL || length == 0) {
		return HAL_ERR_PARA;
	}

#if 0
	RTK_LOGS(TAG, RTK_LOG_INFO, "RX Len = %d\n", length);
	// for (u32 i = 0; i < length; i++) {
	// 	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", (u8)buf[i]);
	// }
	// RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	/* Forward to network stack */
	if (length > 0) {
		netif_adapter_usb_eth_recv(buf, length);
	}

	return HAL_OK;
}

/**
  * @brief  Handle the CDC NCM class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing command data
  * @retval Status
  */
static int usbd_cdc_ncm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	UNUSED(buf);

	if (req == NULL) {
		return HAL_ERR_PARA;
	}

	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;
	u8 req_code = req->bRequest;

	if (req_type == USB_REQ_TYPE_CLASS) {
		switch (req_code) {
		case 0x43: /* SET_ETHERNET_PACKET_FILTER (standard CDC Ethernet request) */
			break;
		case 0x40: /* SET_ETHERNET_MULTICAST_FILTERS */
			break;
		case 0x44: /* GET_ETHERNET_STATISTIC */
			break;
		default:
			/*
			 * NCM-specific requests (GET/SET NTB_PARAMETERS, SET_NTB_FORMAT, etc.)
			 * are handled by the class driver. Unrecognized requests return HAL_OK
			 * to avoid stalling the control endpoint.
			 */
			break;
		}
	}

	return ret;
}

/**
  * @brief  USB attach status changed callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: Current attach status
  */
static void usbd_cdc_ncm_cb_status_changed(u8 old_status, u8 status)
{
	/*
	The scenario of state change is as follows:
		Status 0 to 1: Indicates the initialization of the USB device from a cold boot, transitioning it
		to an attached state.
		Status 1 to 2: Represents transition from attached to detached state; for example, when the device
		is hot-plugged out, the host suspends, or the system enters sleep mode.
		Status 2 to 1: Represents transition from detached to attached state; for example, when the device
		is hot-plugged in, performs a remote wakeup, or the host resumes.
	*/

#if CONFIG_USBD_CDC_NCM_HOTPLUG
	cdc_ncm_attach_old_status = old_status;
	cdc_ncm_attach_status = status;
	if (cdc_ncm_attach_status_changed_sema != NULL) {
		rtos_sema_give(cdc_ncm_attach_status_changed_sema);
	}
#else
	UNUSED(status);
	UNUSED(old_status);
#endif

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdc_ncm_link_disconnected = 1;
	}
}

#if CONFIG_USBD_CDC_NCM_HOTPLUG
/**
  * @brief  USB hotplug detection and handling thread
  * @param  param: Thread parameter (unused)
  */
static void usbd_ncm_hotplug_thread(void *param)
{
	int ret = 0;
	u8 current_status;
	UNUSED(param);

	cdc_ncm_hotplug_thread_running = 1;

	while (cdc_ncm_hotplug_thread_running) {
		// Wait for attach status change notification
		if (rtos_sema_take(cdc_ncm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Status change %d -> %d \n", cdc_ncm_attach_old_status, cdc_ncm_attach_status);
		current_status = cdc_ncm_attach_status;

		if (current_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");

			// Deinitialize CDC NCM
			ret = usbd_cdc_ncm_deinit();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit fail %d\n", ret);
			}

			// Deinitialize USB device
			ret = usbd_deinit();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit core fail %d\n", ret);
				break;
			}

			// Small delay to ensure proper cleanup
			rtos_time_delay_ms(100);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap 0x%x\n", rtos_mem_get_free_heap_size());

			// Re-initialize USB device
			ret = usbd_init(&cdc_ncm_cfg);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
				break;
			}

			// Re-initialize CDC NCM
			ret = usbd_cdc_ncm_init(&cdc_ncm_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init NCM fail %d\n", ret);
				usbd_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");

		} else if (current_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Attached\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Thread exit\n");
	cdc_ncm_hotplug_thread_running = 0;
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_CDC_NCM_HOTPLUG

/**
  * @brief  USB CDC NCM initialization thread
  * @param  param: Thread parameter (unused)
  */
static void usbd_ncm_init_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_CDC_NCM_HOTPLUG
	rtos_task_t hotplug_task = NULL;
#endif

	UNUSED(param);

	rltk_usb_eth_init();

#if CONFIG_USBD_CDC_NCM_HOTPLUG
	// Create semaphore for hotplug detection
	ret = rtos_sema_create(&cdc_ncm_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create attach sema fail\n");
		goto exit_cleanup;
	}
#endif

	// Initialize USB device
	ret = usbd_init(&cdc_ncm_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
		goto exit_usbd_init_fail;
	}

	// Initialize CDC NCM
	ret = usbd_cdc_ncm_init(&cdc_ncm_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init NCM fail %d\n", ret);
		goto exit_usbd_cdc_ncm_init_fail;
	}

#if CONFIG_USBD_CDC_NCM_HOTPLUG
	// Create hotplug detection thread
	ret = rtos_task_create(&hotplug_task,
						   "usbd_ncm_hotplug_thread",
						   usbd_ncm_hotplug_thread,
						   NULL,
						   CONFIG_USBD_CDC_NCM_HOTPLUG_THREAD_STACK_SIZE,
						   CONFIG_USBD_CDC_NCM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_hotplug_task_fail;
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD NCM demo start\n");

	// Keep init thread alive briefly then exit
	rtos_time_delay_ms(100);
	rtos_task_delete(NULL);
	return;

#if CONFIG_USBD_CDC_NCM_HOTPLUG
exit_create_hotplug_task_fail:
	usbd_cdc_ncm_deinit();
#endif

exit_usbd_cdc_ncm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
#if CONFIG_USBD_CDC_NCM_HOTPLUG
	if (cdc_ncm_attach_status_changed_sema != NULL) {
		rtos_sema_delete(cdc_ncm_attach_status_changed_sema);
		cdc_ncm_attach_status_changed_sema = NULL;
	}
exit_cleanup:
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail\n");
	rtos_task_delete(NULL);
}

/* Shell command: ncm_link <0|1> -- report the upper-layer network link state
 * to the host. Mirrors the ECM example's `ecm_link` cmd. Edge-triggered:
 * calling repeatedly with the same value is a no-op inside the class driver. */
static u32 usbd_cdc_ncm_cmd_link(u16 argc, u8 *argv[])
{
	if (argc == 0 || argv[0] == NULL) {
		return HAL_ERR_PARA;
	}
	u8 link_up = (u8)(_strtoul((const char *)argv[0], NULL, 10) ? 1 : 0);
	return (u32)usbd_cdc_ncm_set_link_status(link_up);
}

/* Exported functions --------------------------------------------------------*/

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ncm_transmit(buf, len, block);
}

/**
  * @brief  Start USB CDC NCM device demo
  * @retval None
  */
void example_usbd_cdc_ncm(void)
{
	int ret;
	rtos_task_t init_task = NULL;
	rtos_task_t monitor_task = NULL;

	ret = rtos_task_create(&init_task,
						   "usbd_ncm_init_thread",
						   usbd_ncm_init_thread,
						   NULL,
						   CONFIG_USBD_CDC_NCM_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBD_CDC_NCM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
		return;
	}

	ret = rtos_task_create(&monitor_task,
						   "usbd_ncm_link_change_thread",
						   usbd_ncm_link_change_thread,
						   NULL,
						   CONFIG_USBD_CDC_NCM_LINK_STATE_THREAD_STACK_SIZE,
						   CONFIG_USBD_CDC_NCM_LINK_STATE_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_cdc_ncm_cmd_table[] = {
	{"usbd_ncm_link", usbd_cdc_ncm_cmd_link},
};
