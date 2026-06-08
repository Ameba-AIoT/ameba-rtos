/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "basic_types.h"
#include "usbd_cdc_ecm.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "dhcp/dhcps.h"

/* Private defines -----------------------------------------------------------*/

/*
	This configuration is used to enable a thread to check hotplug event
 	and reset USB stack to avoid memory leak, only for example.
*/
#define CONFIG_USBD_CDC_ECM_HOTPLUG         1

#define CONFIG_USBD_CDC_ECM_TX_DEBUG        0

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_CDC_ECM_SPEED           USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_ECM_SPEED           USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_CDC_ECM_INIT_THREAD_PRIORITY         5
#define CONFIG_CDC_ECM_XFER_THREAD_PRIORITY         5
#define CONFIG_CDC_ECM_LINK_STATE_THREAD_PRIORITY   4
#define CONFIG_CDC_ECM_HOTPLUG_THREAD_PRIORITY      8

// Thread stack sizes
#define CONFIG_CDC_ECM_INIT_THREAD_STACK_SIZE         2048U
#define CONFIG_CDC_ECM_HOTPLUG_THREAD_STACK_SIZE      2048U
#define CONFIG_CDC_ECM_LINK_STATE_THREAD_STACK_SIZE   2048U

// USB ECM Device IP Configuration
#define USB_ECM_IP_ADDR0   192
#define USB_ECM_IP_ADDR1   168
#define USB_ECM_IP_ADDR2   45
#define USB_ECM_IP_ADDR3   1

#define USB_ECM_NETMASK_ADDR0   255
#define USB_ECM_NETMASK_ADDR1   255
#define USB_ECM_NETMASK_ADDR2   255
#define USB_ECM_NETMASK_ADDR3   0

#define USB_ECM_GW_ADDR0   192
#define USB_ECM_GW_ADDR1   168
#define USB_ECM_GW_ADDR2   45
#define USB_ECM_GW_ADDR3   1

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

static int cdc_ecm_cb_init(void);
static int cdc_ecm_cb_deinit(void);
static int cdc_ecm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_ecm_cb_received(u8 *buf, u32 Len);
static void cdc_ecm_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ECM";

extern struct netif *pnetif_usb_eth;
static u8 dhcp_server_started = 0;
/*
 * dongle_mac: MAC address reported to the USB host side of the CDC-ECM link.
 * It is passed into ecm_priv and advertised through the ECM functional descriptor
 * (iMACAddress string), so the host's virtual Ethernet adapter is assigned this MAC.
 * In other words, it identifies the "host-facing" end of the USB Ethernet dongle.
 */
static u8 dongle_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
/*
 * dhcp_server_mac: MAC address used by the device's local lwIP netif (pnetif_usb_eth).
 * It is copied into pnetif_usb_eth->hwaddr in ecm_link_change_thread() before the
 * DHCP server is started, so it is the source MAC for frames the device sends to the
 * host (DHCP offers, ARP replies, gateway traffic). This is the "device-facing" end.
 *
 * Note: dongle_mac and dhcp_server_mac must differ (here only the last byte: 0x55 vs
 * 0x56) so the two ends of the point-to-point USB Ethernet link have distinct MACs.
 */
static u8 dhcp_server_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x56};
static __IO u8 cdc_ecm_link_disconnected = 0;

static usbd_cdc_ecm_priv_data_t ecm_priv = {
	dongle_mac,
};

static usbd_cdc_ecm_cb_t cdc_ecm_cb = {
	.priv = &ecm_priv,
	.init = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.setup = cdc_ecm_cb_setup,
	.received = cdc_ecm_cb_received,
	.status_changed = cdc_ecm_cb_status_changed,
};

static usbd_config_t cdc_ecm_cfg = {
	.speed = CONFIG_USBD_CDC_ECM_SPEED,
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

#if CONFIG_USBD_CDC_ECM_TX_DEBUG
/*
 * USB ECM Network Packet: DHCP Offer
 * Total Size: 368 Bytes
 * Description: A DHCP Offer packet sent from the server (Router) to the client.
 */
static const uint8_t usb_cdc_ecm_dhcp_offer_pkt[368] = {
	/* --- Ethernet Header (14 Bytes) --- */
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  // Dest MAC: Client (00:11:22:33:44:55)
	0x78, 0x11, 0xDC, 0x55, 0x6A, 0x56, // Src MAC: Server (78:11:DC:55:6A:56)
	0x08, 0x00,                         // EtherType: IPv4 (0x0800)

	/* --- IP Header (20 Bytes) --- */
	0x45,                               // Version (4) & IHL (5)
	0x00,                               // DSCP & ECN
	0x01, 0x62,                         // Total Length: 354 bytes (IP Payload)
	0xBE, 0xCD,                         // Identification
	0x00, 0x00,                         // Flags & Fragment Offset
	0x40,                               // TTL: 64
	0x11,                               // Protocol: UDP (17)
	0xFB, 0x09,                         // Header Checksum
	0xC0, 0xA8, 0x1F, 0x01,             // Src IP: 192.168.31.1
	0xC0, 0xA8, 0x1F, 0x62,             // Dest IP: 192.168.31.98

	/* --- UDP Header (8 Bytes) --- */
	0x00, 0x43,                         // Src Port: 67 (DHCP Server)
	0x00, 0x44,                         // Dest Port: 68 (DHCP Client)
	0x01, 0x4E,                         // Length: 334 bytes
	0xDD, 0xC9,                         // Checksum

	/* --- DHCP Header (Fixed 236 Bytes) --- */
	0x02,                               // Op: DHCP Offer (2)
	0x01,                               // HType: Ethernet
	0x06,                               // HLen: 6
	0x00,                               // Hops: 0
	0x23, 0xEC, 0xE7, 0x3A,             // XID: Transaction ID
	0x00, 0x00,                         // Secs: 0
	0x00, 0x00,                         // Flags: 0
	0xC0, 0xA8, 0x1F, 0x62,             // Client IP (Your IP): 192.168.31.98
	0xC0, 0xA8, 0x1F, 0x01,             // Next Server IP: 192.168.31.1
	0x00, 0x00, 0x00, 0x00,             // Gateway IP: 0.0.0.0
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, // Client MAC Address
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Padding (Client MAC padding)

	/* --- DHCP sname & file fields (Padding Area) --- */
	/* These 128+ bytes are mostly zero-padding in this packet */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	/* --- DHCP Options (Variable Length) --- */
	0x63, 0x82, 0x53, 0x63,             // Magic Cookie: DHCP Option Start
	0x35, 0x01, 0x02,                   // Option 53: DHCP Offer
	0x36, 0x04, 0xC0, 0xA8, 0x1F, 0x01, // Option 36: Server Identifier (192.168.31.1)
	0x33, 0x04, 0x00, 0x00, 0xA8, 0xC0, // Option 51: Lease Time (43200s / 12h)
	0x3A, 0x04, 0x00, 0x00, 0x54, 0x60, // Option 58: Renewal Time (21600s)
	0x3B, 0x04, 0x00, 0x00, 0x93, 0xA8, // Option 59: Rebinding Time (37800s)
	0x01, 0x04, 0xFF, 0xFF, 0xFF, 0x00, // Option 1: Subnet Mask (255.255.255.0)
	0x1C, 0x04, 0xC0, 0xA8, 0x1F, 0xFF, // Option 28: Broadcast Addr (192.168.31.255)
	0x03, 0x04, 0xC0, 0xA8, 0x1F, 0x01, // Option 3: Router/Gateway (192.168.31.1)
	0x06, 0x04, 0xC0, 0xA8, 0x1F, 0x01, // Option 6: DNS Server (192.168.31.1)

	/* --- Vendor Specific / Hostname Options --- */
	0x2B, 0x11,                         // Option 43: Vendor Specific Info (Len 17)
	0x6D, 0x69, 0x77, 0x69, 0x66, 0x69, // Data: "miwifi"
	0x2D, 0x52, 0x33, 0x2D,             // Data: "-R3-"
	0x32, 0x2E, 0x32, 0x36, 0x2E, 0x33, // Data: "2.26.3"
	0x39,                               // Data: "9"

	0x0C, 0x0D,                         // Option 12: Hostname (Len 13)
	0x4D, 0x69, 0x57, 0x69, 0x46, 0x69, // Data: "MiWiFi"
	0x2D, 0x52, 0x33, 0x2D,             // Data: "-R3-"
	0x73, 0x72, 0x76,                   // Data: "srv"

	0xFF                                // Option 255: End of Options
	// Note: The remaining bytes up to 368 are implicit padding (0x00)
	// required by the USB CDC-ECM frame size or MTU alignment.
};
#endif

#if CONFIG_USBD_CDC_ECM_HOTPLUG
static __IO u8 cdc_ecm_attach_status = USBD_ATTACH_STATUS_INIT;
static __IO u8 cdc_ecm_attach_old_status = USBD_ATTACH_STATUS_INIT;
static rtos_sema_t cdc_ecm_attach_status_changed_sema = NULL;
static __IO u8 cdc_ecm_hotplug_thread_running = 0;
#endif

/* Private functions ---------------------------------------------------------*/
static void ecm_link_change_thread(void *param)
{
	eth_state_t ethernet_state = ETH_STATUS_IDLE;
	u8 link_is_up = 0;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	while (1) {
		link_is_up = usbd_cdc_ecm_get_connect_status();

		if (cdc_ecm_link_disconnected) {
			cdc_ecm_link_disconnected = 0;
			link_is_up = 0;
		}

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {
			if (pnetif_usb_eth == NULL) {
				rtos_time_delay_ms(1000);
			} else {
				if (!dhcp_server_started) {
					// RTK_LOGS(TAG, RTK_LOG_INFO, "Starting USB ECM DHCP Server...\n");

					// 1. Set netif MAC address
					memcpy(pnetif_usb_eth->hwaddr, dhcp_server_mac, 6);
					pnetif_usb_eth->hwaddr_len = ETHARP_HWADDR_LEN;
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server MAC: " MAC_FMT "\n", MAC_ARG(pnetif_usb_eth->hwaddr));

					// 2. Deinit DHCP server
					dhcps_deinit(pnetif_usb_eth);

					// 3. Set Device IP address
					u32 ip_addr = CONCAT_TO_UINT32(USB_ECM_IP_ADDR0, USB_ECM_IP_ADDR1, USB_ECM_IP_ADDR2, USB_ECM_IP_ADDR3);
					u32 netmask = CONCAT_TO_UINT32(USB_ECM_NETMASK_ADDR0, USB_ECM_NETMASK_ADDR1, USB_ECM_NETMASK_ADDR2, USB_ECM_NETMASK_ADDR3);
					u32 gw = CONCAT_TO_UINT32(USB_ECM_GW_ADDR0, USB_ECM_GW_ADDR1, USB_ECM_GW_ADDR2, USB_ECM_GW_ADDR3);
					lwip_set_ip(NETIF_USB_ETH_INDEX, ip_addr, netmask, gw);

					RTK_LOGS(TAG, RTK_LOG_INFO, "Device IP: %d.%d.%d.%d\n", USB_ECM_IP_ADDR0, USB_ECM_IP_ADDR1, USB_ECM_IP_ADDR2, USB_ECM_IP_ADDR3);

					// 4. Activate network interface link
					netifapi_netif_set_link_up(pnetif_usb_eth);
					netifapi_netif_set_up(pnetif_usb_eth);
					netifapi_netif_set_default(pnetif_usb_eth);

					// 5. Initialize and start DHCP server
					dhcps_init(pnetif_usb_eth);
					dhcps_start(pnetif_usb_eth);

					dhcp_server_started = 1;
					ethernet_state = ETH_STATUS_INIT;

					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server started\n");
				}
			}
		} else if (0 == link_is_up && (ethernet_state >= ETH_STATUS_INIT)) {
			ethernet_state = ETH_STATUS_DEINIT;
			// USB disconnected, stop DHCP server
			if (dhcp_server_started) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Stopping USB ECM DHCP Server...\n");

				// 1. Stop DHCP service first
				dhcps_stop(pnetif_usb_eth);
				dhcps_deinit(pnetif_usb_eth);

				// 2. Bring down network interface (similar to WHC stop ap)
				netifapi_netif_set_down(pnetif_usb_eth);
				netifapi_netif_set_link_down(pnetif_usb_eth);

				dhcp_server_started = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server stopped\n");
			}
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

/**
  * @brief  Initializes the CDC ECM media layer
  * @retval Status
  */
static int cdc_ecm_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC ECM media layer
  * @retval Status
  */
static int cdc_ecm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  * @retval Status
  */
static int cdc_ecm_cb_received(u8 *buf, u32 length)
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
  * @brief  Handle the CDC ECM class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing command data
  * @retval Status
  */
static int cdc_ecm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	UNUSED(buf);

	if (req == NULL) {
		return HAL_ERR_PARA;
	}

	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;
	u8 req_code = req->bRequest;

	// Handle standard SET_INTERFACE request
	if ((req_code == USB_REQ_SET_INTERFACE) && (req_type == USB_REQ_TYPE_STANDARD)) {
#if CONFIG_USBD_CDC_ECM_TX_DEBUG
		u16 alt_setting = req->wValue;
		u16 interface = req->wIndex;

		// When data interface alternate setting 1 is selected, send DHCP offer
		if ((alt_setting == 1) && (interface == USBD_CDC_ECM_DATA_INTERFACE_NUM)) {
			ret = usb_ethernet_transmit((u8 *)usb_cdc_ecm_dhcp_offer_pkt, sizeof(usb_cdc_ecm_dhcp_offer_pkt), 1);
		}
#endif
	} else if (req_type == USB_REQ_TYPE_CLASS) {
		switch (req_code) {
		case USB_CDC_ECM_SET_ETHERNET_PACKET_FILTER:
			break;
		case USB_CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS:
			break;
		case USB_CDC_ECM_GET_ETHERNET_STATISTIC:
			break;
		default:
			ret = HAL_ERR_UNKNOWN;
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
static void cdc_ecm_cb_status_changed(u8 old_status, u8 status)
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

#if CONFIG_USBD_CDC_ECM_HOTPLUG
	cdc_ecm_attach_old_status = old_status;
	cdc_ecm_attach_status = status;
	if (cdc_ecm_attach_status_changed_sema != NULL) {
		rtos_sema_give(cdc_ecm_attach_status_changed_sema);
	}
#else
	UNUSED(status);
	UNUSED(old_status);
#endif

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdc_ecm_link_disconnected = 1;
	}
}

#if CONFIG_USBD_CDC_ECM_HOTPLUG
/**
  * @brief  USB hotplug detection and handling thread
  * @param  param: Thread parameter (unused)
  */
static void cdc_ecm_hotplug_thread(void *param)
{
	int ret = 0;
	u8 current_status;
	UNUSED(param);

	cdc_ecm_hotplug_thread_running = 1;

	while (cdc_ecm_hotplug_thread_running) {
		// Wait for attach status change notification
		if (rtos_sema_take(cdc_ecm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Status change %d -> %d \n", cdc_ecm_attach_old_status, cdc_ecm_attach_status);
		current_status = cdc_ecm_attach_status;

		if (current_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");

			// Deinitialize CDC ECM
			ret = usbd_cdc_ecm_deinit();
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
			ret = usbd_init(&cdc_ecm_cfg);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
				break;
			}

			// Re-initialize CDC ECM
			ret = usbd_cdc_ecm_init(&cdc_ecm_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init ECM fail %d\n", ret);
				usbd_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");

		} else if (current_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Attached\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Thread exit\n");
	cdc_ecm_hotplug_thread_running = 0;
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_CDC_ECM_HOTPLUG

/**
  * @brief  USB CDC ECM initialization thread
  * @param  param: Thread parameter (unused)
  */
static void example_usbd_cdc_ecm_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_CDC_ECM_HOTPLUG
	rtos_task_t hotplug_task = NULL;
#endif

	UNUSED(param);

	rltk_usb_eth_init();

#if CONFIG_USBD_CDC_ECM_HOTPLUG
	// Create semaphore for hotplug detection
	ret = rtos_sema_create(&cdc_ecm_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create attach sema fail\n");
		goto exit_cleanup;
	}
#endif

	// Initialize USB device
	ret = usbd_init(&cdc_ecm_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
		goto exit_usbd_init_fail;
	}

	// Initialize CDC ECM
	ret = usbd_cdc_ecm_init(&cdc_ecm_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init ECM fail %d\n", ret);
		goto exit_usbd_cdc_ecm_init_fail;
	}

#if CONFIG_USBD_CDC_ECM_HOTPLUG
	// Create hotplug detection thread
	ret = rtos_task_create(&hotplug_task,
						   "cdc_ecm_hotplug",
						   cdc_ecm_hotplug_thread,
						   NULL,
						   CONFIG_CDC_ECM_HOTPLUG_THREAD_STACK_SIZE,
						   CONFIG_CDC_ECM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_hotplug_task_fail;
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "ECM demo start\n");

	// Keep init thread alive briefly then exit
	rtos_time_delay_ms(100);
	rtos_task_delete(NULL);
	return;

#if CONFIG_USBD_CDC_ECM_HOTPLUG
exit_create_hotplug_task_fail:
	usbd_cdc_ecm_deinit();
#endif

exit_usbd_cdc_ecm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
#if CONFIG_USBD_CDC_ECM_HOTPLUG
	if (cdc_ecm_attach_status_changed_sema != NULL) {
		rtos_sema_delete(cdc_ecm_attach_status_changed_sema);
		cdc_ecm_attach_status_changed_sema = NULL;
	}
exit_cleanup:
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ecm_transmit(buf, len, block);
}

/**
  * @brief  Start USB CDC ECM device demo
  * @retval None
  */
void example_usbd_cdc_ecm(void)
{
	int ret;
	rtos_task_t init_task = NULL;
	rtos_task_t monitor_task = NULL;

	ret = rtos_task_create(&init_task,
						   "cdc_ecm_init",
						   example_usbd_cdc_ecm_thread,
						   NULL,
						   CONFIG_CDC_ECM_INIT_THREAD_STACK_SIZE,
						   CONFIG_CDC_ECM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
		return;
	}

	ret = rtos_task_create(&monitor_task,
						   "usbd_ecm_link_thread",
						   ecm_link_change_thread,
						   NULL,
						   CONFIG_CDC_ECM_LINK_STATE_THREAD_STACK_SIZE,
						   CONFIG_CDC_ECM_LINK_STATE_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
}
