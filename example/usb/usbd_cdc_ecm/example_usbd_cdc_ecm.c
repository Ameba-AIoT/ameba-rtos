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

// Endpoint address
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define CDC_ECM_BULK_IN_EP                        0x82U
#define CDC_ECM_BULK_OUT_EP                       0x02U
#else
#define CDC_ECM_BULK_IN_EP                        0x81U
#define CDC_ECM_BULK_OUT_EP                       0x02U
#endif
#define CDC_ECM_INTR_IN_EP                        0x83U

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define CDC_ECM_HOTPLUG                           1

#define CDC_ECM_TX_DEBUG                          0

// Thread priorities
#define CDC_ECM_INIT_THREAD_PRIORITY              5
#define CDC_ECM_LINK_STATE_THREAD_PRIORITY        4
#define CDC_ECM_HOTPLUG_THREAD_PRIORITY           8

// Thread stack sizes
#define CDC_ECM_INIT_THREAD_STACK_SIZE            1024U
#define CDC_ECM_HOTPLUG_THREAD_STACK_SIZE         1024U
#define CDC_ECM_LINK_STATE_THREAD_STACK_SIZE      1600U

// USB ECM Device IP Configuration
#define CDC_ECM_IP_ADDR0                          192
#define CDC_ECM_IP_ADDR1                          168
#define CDC_ECM_IP_ADDR2                          45
#define CDC_ECM_IP_ADDR3                          1

#define CDC_ECM_NETMASK_ADDR0                     255
#define CDC_ECM_NETMASK_ADDR1                     255
#define CDC_ECM_NETMASK_ADDR2                     255
#define CDC_ECM_NETMASK_ADDR3                     0

#define CDC_ECM_GW_ADDR0                          192
#define CDC_ECM_GW_ADDR1                          168
#define CDC_ECM_GW_ADDR2                          45
#define CDC_ECM_GW_ADDR3                          1

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

static int usbd_ecm_cb_init(void);
static int usbd_ecm_cb_deinit(void);
static int usbd_ecm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int usbd_ecm_cb_received(u8 *buf, u32 Len);
static void usbd_ecm_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ECM";

extern struct netif *pnetif_usb_eth;
static u8 usbd_ecm_dhcp_server_started = 0;
/*
 * dongle_mac: MAC address reported to the USB host side of the CDC-ECM link.
 * It is passed into usbd_ecm_priv and advertised through the ECM functional descriptor
 * (iMACAddress string), so the host's virtual Ethernet adapter is assigned this MAC.
 * In other words, it identifies the "host-facing" end of the USB Ethernet dongle.
 */
static const u8 dongle_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
/*
 * dhcp_server_mac: MAC address used by the device's local lwIP netif (pnetif_usb_eth).
 * It is copied into pnetif_usb_eth->hwaddr in usbd_ecm_link_change_thread() before the
 * DHCP server is started, so it is the source MAC for frames the device sends to the
 * host (DHCP offers, ARP replies, gateway traffic). This is the "device-facing" end.
 *
 * Note: dongle_mac and dhcp_server_mac must differ (here only the last byte: 0x55 vs
 * 0x56) so the two ends of the point-to-point USB Ethernet link have distinct MACs.
 */
static const u8 dhcp_server_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x56};
static __IO u8 usbd_ecm_link_disconnected = 0;

static const usbd_cdc_ecm_priv_data_t usbd_ecm_priv = {
	dongle_mac,
};

static const usbd_cdc_ecm_cb_t usbd_ecm_cb = {
	.priv = &usbd_ecm_priv,
	.init = usbd_ecm_cb_init,
	.deinit = usbd_ecm_cb_deinit,
	.setup = usbd_ecm_cb_setup,
	.received = usbd_ecm_cb_received,
	.status_changed = usbd_ecm_cb_status_changed,
};

static const usbd_cdc_ecm_ep_cfg_t usbd_ecm_ep_cfg = {
	.bulk_in_addr  = CDC_ECM_BULK_IN_EP,
	.bulk_out_addr = CDC_ECM_BULK_OUT_EP,
	.intr_in_addr  = CDC_ECM_INTR_IN_EP,
};

static const usbd_config_t usbd_ecm_cfg = {
	.info = {
		.prod_str = "Realtek CDC ECM Device",
	},
	.isr_priority = INT_PRI_MIDDLE,
	.ext_intr_enable = USBD_SOF_INTR,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 692U,
	.ptx_fifo_depth = {0U, 256U, 32U, 0U, 0U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 656U,
	.ptx_fifo_depth = {0U, 256U, 32U, 0U, 0U, },
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U, },
#endif
};

#if CDC_ECM_TX_DEBUG
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

#if CDC_ECM_HOTPLUG
static __IO u8 usbd_ecm_attach_status = USBD_ATTACH_STATUS_INIT;
static __IO u8 usbd_ecm_attach_old_status = USBD_ATTACH_STATUS_INIT;
static rtos_sema_t usbd_ecm_attach_status_changed_sema = NULL;
static __IO u8 usbd_ecm_hotplug_thread_running = 0;
#endif

/* Serializes the USB stack bring up/tear down against the TX path, taken by the
 * hotplug thread and by usb_ethernet_transmit().
 * Created once and never deleted: TX is driven by the lwIP thread, which lives as
 * long as the firmware and can not be joined by the example, so there is no point
 * in time at which this mutex is provably unreferenced. usbd_ecm_stack_ready is
 * the gate that keeps callers off the stack instead. */
static rtos_mutex_t usbd_ecm_state_mutex = NULL;
/* 1: usbd_init() and usbd_cdc_ecm_init() both done, TX allowed. Cleared before any
 * tear down, so a non-zero value also implies usbd_ecm_state_mutex is valid.
 * usbd_cdc_ecm_deinit() frees the TX ring buffer, the BULK IN DMA buffer and the
 * TX slot sema, all of which usbd_cdc_ecm_transmit() dereferences; its internal
 * data_alt_setting check only guards the entry and retry points, so under SMP the
 * lwIP thread must be excluded from the whole teardown, not just narrowed. */
static volatile u8 usbd_ecm_stack_ready = 0;

/* Private functions ---------------------------------------------------------*/
static void usbd_ecm_link_change_thread(void *param)
{
	eth_state_t ethernet_state = ETH_STATUS_IDLE;
	u8 link_is_up = 0;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	while (1) {
		link_is_up = usbd_cdc_ecm_get_link_status();

		if (usbd_ecm_link_disconnected) {
			usbd_ecm_link_disconnected = 0;
			link_is_up = 0;
		}

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {
			if (pnetif_usb_eth == NULL) {
				rtos_time_delay_ms(1000);
			} else {
				if (!usbd_ecm_dhcp_server_started) {
					// RTK_LOGS(TAG, RTK_LOG_INFO, "Starting USB ECM DHCP Server...\n");

					// 1. Set netif MAC address
					usb_os_memcpy((void *)pnetif_usb_eth->hwaddr, (const void *)dhcp_server_mac, 6);
					pnetif_usb_eth->hwaddr_len = ETHARP_HWADDR_LEN;
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server MAC: " MAC_FMT "\n", MAC_ARG(pnetif_usb_eth->hwaddr));

					// 2. Deinit DHCP server
					dhcps_deinit(pnetif_usb_eth);

					// 3. Set Device IP address
					u32 ip_addr = CONCAT_TO_UINT32(CDC_ECM_IP_ADDR0, CDC_ECM_IP_ADDR1, CDC_ECM_IP_ADDR2, CDC_ECM_IP_ADDR3);
					u32 netmask = CONCAT_TO_UINT32(CDC_ECM_NETMASK_ADDR0, CDC_ECM_NETMASK_ADDR1, CDC_ECM_NETMASK_ADDR2,
												   CDC_ECM_NETMASK_ADDR3);
					u32 gw = CONCAT_TO_UINT32(CDC_ECM_GW_ADDR0, CDC_ECM_GW_ADDR1, CDC_ECM_GW_ADDR2, CDC_ECM_GW_ADDR3);
					lwip_set_ip(NETIF_USB_ETH_INDEX, ip_addr, netmask, gw);

					RTK_LOGS(TAG, RTK_LOG_INFO, "Device IP: %d.%d.%d.%d\n", CDC_ECM_IP_ADDR0, CDC_ECM_IP_ADDR1, CDC_ECM_IP_ADDR2,
							 CDC_ECM_IP_ADDR3);

					// 4. Activate network interface link
					netifapi_netif_set_link_up(pnetif_usb_eth);
					netifapi_netif_set_up(pnetif_usb_eth);
					netifapi_netif_set_default(pnetif_usb_eth);

					// 5. Initialize and start DHCP server
					dhcps_init(pnetif_usb_eth);
					dhcps_start(pnetif_usb_eth);

					usbd_ecm_dhcp_server_started = 1;
					ethernet_state = ETH_STATUS_INIT;

					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Server started\n");
				}
			}
		} else if (0 == link_is_up && (ethernet_state >= ETH_STATUS_INIT)) {
			ethernet_state = ETH_STATUS_DEINIT;
			// USB disconnected, stop DHCP server
			if (usbd_ecm_dhcp_server_started) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Stopping USB ECM DHCP Server...\n");

				// 1. Stop DHCP service first
				dhcps_stop(pnetif_usb_eth);
				dhcps_deinit(pnetif_usb_eth);

				// 2. Bring down network interface (similar to WHC stop ap)
				netifapi_netif_set_down(pnetif_usb_eth);
				netifapi_netif_set_link_down(pnetif_usb_eth);

				usbd_ecm_dhcp_server_started = 0;
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
static int usbd_ecm_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC ECM media layer
  * @retval Status
  */
static int usbd_ecm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  * @retval Status
  */
static int usbd_ecm_cb_received(u8 *buf, u32 length)
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
static int usbd_ecm_cb_setup(usb_setup_req_t *req, u8 *buf)
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
#if CDC_ECM_TX_DEBUG
		u16 alt_setting = req->wValue;
		u16 interface = req->wIndex;

		// When data interface alternate setting 1 is selected, send DHCP offer.
		// block = 0: this callback runs in ISR context, so it must never wait on
		// a ring buffer slot.
		if ((alt_setting == 1) && (interface == USBD_CDC_ECM_DATA_INTERFACE_NUM)) {
			ret = usb_ethernet_transmit((u8 *)usb_cdc_ecm_dhcp_offer_pkt, sizeof(usb_cdc_ecm_dhcp_offer_pkt), 0);
		}
#endif
	} else if (req_type == USB_REQ_TYPE_CLASS) {
		switch (req_code) {
		case USB_CDC_SET_ETHERNET_PACKET_FILTER:
			/* CDC Ethernet subclass request (Ref CDC 1.2 Table 13), mandatory for
			 * ECM and carrying no data stage.  Accepted and ignored - this device
			 * does no packet filtering. */
			break;
		case USB_CDC_SET_ETHERNET_MULTICAST_FILTERS:
			/* Accepted and ignored: no multicast filter table on this device. */
			break;
		default:
			/*
			 * Anything left here is unsupported and must be answered with a
			 * request error so the core STALLs EP0 (Ref USB 2.0 9.2.7).
			 * HAL_ERR_PARA matches the ECM class driver's own convention
			 * (every unsupported branch in usbd_cdc_ecm.c returns it) and the
			 * NCM example.  Returning HAL_OK would be actively harmful for a
			 * device-to-host request such as GET_ETHERNET_STATISTIC: the class
			 * driver would transmit req->wLength bytes of whatever the EP0
			 * buffer happened to hold, leaking stale buffer content.
			 */
			ret = HAL_ERR_PARA;
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
static void usbd_ecm_cb_status_changed(u8 old_status, u8 status)
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

#if CDC_ECM_HOTPLUG
	usbd_ecm_attach_old_status = old_status;
	usbd_ecm_attach_status = status;
	if (usbd_ecm_attach_status_changed_sema != NULL) {
		rtos_sema_give(usbd_ecm_attach_status_changed_sema);
	}
#else
	UNUSED(status);
	UNUSED(old_status);
#endif

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		usbd_ecm_link_disconnected = 1;
	}
}

#if CDC_ECM_HOTPLUG
/**
  * @brief  USB hotplug detection and handling thread
  * @param  param: Thread parameter (unused)
  */
static void usbd_ecm_hotplug_thread(void *param)
{
	int ret = 0;
	u8 current_status;
	UNUSED(param);

	usbd_ecm_hotplug_thread_running = 1;

	while (usbd_ecm_hotplug_thread_running) {
		// Wait for attach status change notification
		if (rtos_sema_take(usbd_ecm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Status change %d -> %d \n", usbd_ecm_attach_old_status, usbd_ecm_attach_status);
		current_status = usbd_ecm_attach_status;

		if (current_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");

			/* Close the gate before tearing down. usb_ethernet_transmit() contends
			 * for the same mutex, so no TX is in flight here and any later one sees
			 * usbd_ecm_stack_ready == 0 and gives up. */
			rtos_mutex_take(usbd_ecm_state_mutex, RTOS_MAX_TIMEOUT);
			usbd_ecm_stack_ready = 0;

			// Deinitialize CDC ECM
			usbd_cdc_ecm_deinit();

			// Deinitialize USB device
			usbd_deinit();
			rtos_mutex_give(usbd_ecm_state_mutex);

			// Small delay to ensure proper cleanup
			rtos_time_delay_ms(100);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap 0x%x\n", rtos_mem_get_free_heap_size());

			rtos_mutex_take(usbd_ecm_state_mutex, RTOS_MAX_TIMEOUT);

			// Re-initialize USB device
			ret = usbd_init(&usbd_ecm_cfg);
			if (ret != HAL_OK) {
				rtos_mutex_give(usbd_ecm_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
				break;
			}

			// Re-initialize CDC ECM
			ret = usbd_cdc_ecm_init(&usbd_ecm_cb, &usbd_ecm_ep_cfg);
			if (ret != HAL_OK) {
				usbd_deinit();
				rtos_mutex_give(usbd_ecm_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init ECM fail %d\n", ret);
				break;
			}

			/* Open the gate at the very last step, so that no TX runs before the
			 * re-init is fully done. */
			usbd_ecm_stack_ready = 1;
			rtos_mutex_give(usbd_ecm_state_mutex);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");

		} else if (current_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Attached\n");
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Thread exit\n");
	usbd_ecm_hotplug_thread_running = 0;
	/* Either the loop was asked to stop or a re-init failed; in both cases the
	 * stack must be treated as gone, so keep TX out for good. */
	usbd_ecm_stack_ready = 0;

	/* The stack is fully deinited here, no ISR callback can give the sema any more.
	   This thread is its only user left: free it as the last owner. */
	rtos_sema_delete(usbd_ecm_attach_status_changed_sema);
	usbd_ecm_attach_status_changed_sema = NULL;

	rtos_task_delete(NULL);
}
#endif // CDC_ECM_HOTPLUG

/**
  * @brief  USB CDC ECM initialization thread
  * @param  param: Thread parameter (unused)
  */
static void usbd_ecm_init_thread(void *param)
{
	int ret = 0;
#if CDC_ECM_HOTPLUG
	rtos_task_t hotplug_task = NULL;
#endif

	UNUSED(param);

	rltk_usb_eth_init();

	/* Created before the stack comes up and never deleted, see the declaration */
	if (usbd_ecm_state_mutex == NULL) {
		ret = rtos_mutex_create(&usbd_ecm_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto exit_cleanup;
		}
	}

#if CDC_ECM_HOTPLUG
	// Create semaphore for hotplug detection
	ret = rtos_sema_create(&usbd_ecm_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create attach sema fail\n");
		goto exit_cleanup;
	}
#endif

	// Initialize USB device
	ret = usbd_init(&usbd_ecm_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
		goto exit_usbd_init_fail;
	}

	// Initialize CDC ECM
	ret = usbd_cdc_ecm_init(&usbd_ecm_cb, &usbd_ecm_ep_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init ECM fail %d\n", ret);
		goto exit_usbd_cdc_ecm_init_fail;
	}

	/* The stack is fully up: allow TX */
	usbd_ecm_stack_ready = 1;

#if CDC_ECM_HOTPLUG
	// Create hotplug detection thread
	ret = rtos_task_create(&hotplug_task,
						   "usbd_cdc_ecm_hotplug_thread",
						   usbd_ecm_hotplug_thread,
						   NULL,
						   CDC_ECM_HOTPLUG_THREAD_STACK_SIZE,
						   CDC_ECM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_hotplug_task_fail;
	}
#if defined(CONFIG_SMP)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to
	   core 0). Pinning the hotplug thread to CPU0 makes its deinit/reinit single-core
	   against the ISR, so deinit's local interrupt disable is meaningful under SMP. */
	rtos_task_set_affinity(hotplug_task, 0);
#endif
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ECM demo start\n");

	// Keep init thread alive briefly then exit
	rtos_time_delay_ms(100);
	rtos_task_delete(NULL);
	return;

#if CDC_ECM_HOTPLUG
exit_create_hotplug_task_fail:
	/* The gate was already opened above, so the lwIP thread may be inside
	 * usbd_cdc_ecm_transmit() right now: take the lock here too, exactly as the
	 * hotplug thread does, instead of only closing the gate. */
	rtos_mutex_take(usbd_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	usbd_ecm_stack_ready = 0;
	usbd_cdc_ecm_deinit();
	rtos_mutex_give(usbd_ecm_state_mutex);
#endif

exit_usbd_cdc_ecm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
#if CDC_ECM_HOTPLUG
	if (usbd_ecm_attach_status_changed_sema != NULL) {
		rtos_sema_delete(usbd_ecm_attach_status_changed_sema);
		usbd_ecm_attach_status_changed_sema = NULL;
	}
#endif
	/* usbd_ecm_state_mutex is deliberately kept, see its declaration */
exit_cleanup:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	int ret;

	/* Gate on usbd_ecm_stack_ready BEFORE touching the mutex: it is still NULL if
	 * the example was never started, and already cleared if the init failed or the
	 * stack is gone for good. A hotplug right after this check is harmless, the
	 * mutex stays valid and the re-check below skips the TX. */
	if (usbd_ecm_stack_ready == 0U) {
		return HAL_ERR_HW;
	}

	rtos_mutex_take(usbd_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	if (usbd_ecm_stack_ready == 0U) {
		/* Lost the race against a hotplug tear down */
		ret = HAL_ERR_HW;
	} else {
		ret = usbd_cdc_ecm_transmit(buf, len, block);
	}
	rtos_mutex_give(usbd_ecm_state_mutex);

	return ret;
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
						   "usbd_cdc_ecm_init_thread",
						   usbd_ecm_init_thread,
						   NULL,
						   CDC_ECM_INIT_THREAD_STACK_SIZE,
						   CDC_ECM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
		return;
	}

	ret = rtos_task_create(&monitor_task,
						   "usbd_cdc_ecm_link_change_thread",
						   usbd_ecm_link_change_thread,
						   NULL,
						   CDC_ECM_LINK_STATE_THREAD_STACK_SIZE,
						   CDC_ECM_LINK_STATE_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
}

/**
 * @brief  CLI command: report uplink (e.g. Wi-Fi / cellular) link state to host.
 *
 * Usage: ecm_link <0|1>
 *   1: notify host that the upper-layer network link is UP
 *   0: notify host that the upper-layer network link is DOWN
 *
 * Wraps usbd_cdc_ecm_set_link_status(), which is edge-triggered: calling
 * with the same value twice is a no-op. This is the call site documented
 * in usbd_cdc_ecm.h as belonging to the layer that owns the real uplink.
 * Exposing it as a shell command lets you manually exercise the
 * NETWORK_CONNECTION notification path on the USB host side (Linux/Windows
 * virtual NIC carrier up/down) without waiting for a real link event.
 *
 * Note: this does NOT touch the example's internal DHCP/lwIP teardown
 * path (usbd_ecm_link_disconnected); it only sends the USB-level
 * notification. Add a second command if you want to drive both at once.
 */
static u32 usbd_ecm_cmd_link(u16 argc, u8 *argv[])
{
	u8 link_up;

	if (argc == 0 || argv[0] == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: ecm_link <0|1>\n");
		return HAL_ERR_PARA;
	}

	link_up = (u8)(_strtoul((const char *)argv[0], (char **)NULL, 10) ? 1U : 0U);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Set ECM link status -> %s\n",
			 link_up ? "UP" : "DOWN");

	return (u32)usbd_cdc_ecm_set_link_status(link_up);
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_cdc_ecm_cmd_table[] = {
	{"usbd_ecm_link", usbd_ecm_cmd_link},
};
