/**
 * @file usb_ethernet.c
 * @brief USB Ethernet Network Management for R-NAT
 */

#include "platform_autoconf.h"

#if defined(CONFIG_LWIP_USB_ETHERNET) && defined(CONFIG_RNAT)
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "usb_ethernet.h"

#if defined(CONFIG_USBD_CDC_ECM)
#include "usbd_cdc_ecm.h"
#elif defined(CONFIG_USBH_CDC_ECM)
#include "usbh_cdc_ecm.h"
#endif

#ifndef TAG
#define TAG "USB-ETH"
#endif

/* ========== Common ========== */

extern struct netif *pnetif_usb_eth;

extern void rltk_usb_eth_init(void);

extern void netif_adapter_usb_eth_recv(u8 *buf, u32 len);

static void (*g_usb_eth_link_cb)(int link_up) = NULL;

typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_DEINIT,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} usb_eth_state_t;

#define USB_ETH_LINK_THREAD_PRIORITY       4
#define USB_ETH_LINK_THREAD_STACK_SIZE     2048U
#define USB_ETH_HOTPLUG_THREAD_PRIORITY    6
#define USB_ETH_HOTPLUG_THREAD_STACK_SIZE  2048U

static rtos_task_t g_link_task = NULL;
static rtos_task_t g_hotplug_task = NULL;

/* ========== TX ========== */

#if defined(CONFIG_USBD_CDC_ECM)
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbd_cdc_ecm_transmit(buf, len, block);
}
#elif defined(CONFIG_USBH_CDC_ECM)
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ecm_send_data(buf, len, block);
}
#endif

/* ========================================================================== */
/*                                 USBD                                       */
/* ========================================================================== */

#if defined(CONFIG_USBD_CDC_ECM)

static const u8 mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
static const u8 dhcp_server_mac[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x56};

static const usbd_cdc_ecm_priv_data_t ecm_priv = {
	mac_valid,
};

static int cdc_ecm_cb_init(void);
static int cdc_ecm_cb_deinit(void);
static int cdc_ecm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_ecm_cb_received(u8 *buf, u32 length);
static void cdc_ecm_cb_status_changed(u8 old_status, u8 status);

static const usbd_cdc_ecm_cb_t cdc_ecm_cb = {
	.priv = &ecm_priv,
	.init = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.setup = cdc_ecm_cb_setup,
	.received = cdc_ecm_cb_received,
	.status_changed = cdc_ecm_cb_status_changed,
};

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USB_ETH_SPEED  USB_SPEED_FULL
#else
#define USB_ETH_SPEED  USB_SPEED_HIGH
#endif

static const usbd_config_t cdc_ecm_cfg = {
	.speed = USB_ETH_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 644U,
	.ptx_fifo_depth = {16U, 256U, 32U, 16U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined(CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#elif defined(CONFIG_AMEBAPRO3)
	.rx_fifo_depth = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U, },
	.ext_intr_enable = USBD_SOF_INTR,
#endif
};

static rtos_sema_t cdc_ecm_attach_sema = NULL;
static volatile u8 cdc_ecm_attach_status = 0;
static volatile u8 cdc_ecm_hotplug_running = 0;
static volatile u8 cdc_ecm_link_disconnected = 0;

static int cdc_ecm_cb_init(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	UNUSED(buf);

	if (req == NULL) {
		return HAL_ERR_PARA;
	}

	u8 req_type = req->bmRequestType & USB_REQ_TYPE_MASK;
	u8 req_code = req->bRequest;

	if ((req_code == USB_REQ_SET_INTERFACE) && (req_type == USB_REQ_TYPE_STANDARD)) {
		/* Standard SET_INTERFACE */
	} else if (req_type == USB_REQ_TYPE_CLASS) {
		switch (req_code) {
		case USB_CDC_ECM_SET_ETHERNET_PACKET_FILTER:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set filter 0x%04x\n", req->wValue);
			break;
		case USB_CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set mul filter\n");
			break;
		case USB_CDC_ECM_GET_ETHERNET_STATISTIC:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get eth stat 0x%04x\n", req->wValue);
			break;
		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Unknown req 0x%02x\n", req_code);
			ret = HAL_ERR_UNKNOWN;
			break;
		}
	}

	return ret;
}

static int cdc_ecm_cb_received(u8 *buf, u32 length)
{
	if (buf == NULL || length == 0) {
		return HAL_ERR_PARA;
	}

	if (length > 0) {
		netif_adapter_usb_eth_recv(buf, length);
	}

	return HAL_OK;
}

static void cdc_ecm_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d -> %d\n", old_status, status);
	cdc_ecm_attach_status = status;
	if (cdc_ecm_attach_sema != NULL) {
		rtos_sema_give(cdc_ecm_attach_sema);
	}
	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdc_ecm_link_disconnected = 1;
	}
}

static void usb_eth_hotplug_thread(void *param)
{
	int ret;
	u8 current_status;
	UNUSED(param);

	cdc_ecm_hotplug_running = 1;

	while (cdc_ecm_hotplug_running) {
		if (rtos_sema_take(cdc_ecm_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		current_status = cdc_ecm_attach_status;

		if (current_status == USBD_ATTACH_STATUS_DETACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");

			ret = usbd_cdc_ecm_deinit();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM deinit fail %d\n", ret);
			}

			ret = usbd_deinit();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Core deinit fail %d\n", ret);
				break;
			}

			rtos_time_delay_ms(100);

			ret = usbd_init(&cdc_ecm_cfg);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Core init fail %d\n", ret);
				break;
			}

			ret = usbd_cdc_ecm_init(&cdc_ecm_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM init fail %d\n", ret);
				usbd_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		} else if (current_status == USBD_ATTACH_STATUS_ATTACHED) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Attached\n");
		}
	}

	cdc_ecm_hotplug_running = 0;
	rtos_task_delete(NULL);
}

static int usb_eth_do_usb_init(void)
{
	int ret;

	ret = rtos_sema_create(&cdc_ecm_attach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create attach sema fail\n");
		return -1;
	}

	ret = usbd_init(&cdc_ecm_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
		rtos_sema_delete(cdc_ecm_attach_sema);
		cdc_ecm_attach_sema = NULL;
		return -1;
	}

	ret = usbd_cdc_ecm_init(&cdc_ecm_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM init fail %d\n", ret);
		usbd_deinit();
		rtos_sema_delete(cdc_ecm_attach_sema);
		cdc_ecm_attach_sema = NULL;
		return -1;
	}

	return 0;
}

/* ========================================================================== */
/*                                 USBH                                       */
/* ========================================================================== */

#elif defined(CONFIG_USBH_CDC_ECM)

static const u16 led_color[1] = {0x1122};
static const u8 mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

static int cdc_ecm_cb_init(void);
static int cdc_ecm_cb_deinit(void);
static int cdc_ecm_cb_attach(void);
static int cdc_ecm_cb_detach(void);
static int cdc_ecm_cb_setup(void);
static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length);
static int cdc_ecm_cb_process(usb_host_t *host, u8 msg);
static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max);

static const usbh_cdc_ecm_state_cb_t cdc_ecm_usb_cb = {
	.init   = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.attach = cdc_ecm_cb_attach,
	.detach = cdc_ecm_cb_detach,
	.setup  = cdc_ecm_cb_setup,
	.bulk_received = cdc_ecm_cb_bulk_receive,
};

static const usbh_user_cb_t usbh_ecm_usr_cb = {
	.process = cdc_ecm_cb_process,
	.validate = cdc_ecm_cb_device_check,
};

static const usbh_cdc_ecm_priv_data_t ecm_priv = {
	led_color,
	mac_valid,
	sizeof(led_color) / sizeof(led_color[0]),
};

static const usbh_config_t usbh_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1280U,
	.main_task_priority = 5,
	.tick_source = USBH_SOF_TICK,
	.hub_support = 1U,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_AMEBAL2)
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_AMEBAPRO3)
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usb_os_sema_t cdc_ecm_detach_sema;
static volatile u8 cdc_ecm_detach_pending = 0;

static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

static int cdc_ecm_cb_init(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_attach(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_detach(void)
{
	cdc_ecm_detach_pending = 1;
	usb_os_sema_give(cdc_ecm_detach_sema);
	return HAL_OK;
}

static int cdc_ecm_cb_setup(void)
{
	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
	if (length > 0) {
		netif_adapter_usb_eth_recv(buf, length);
	}

	return HAL_OK;
}

static int cdc_ecm_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_cdc_ecm_choose_config(host);
		break;
	case USBH_MSG_DISCONNECTED:
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

static int usb_eth_do_usb_init(void)
{
	int ret;

	ret = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return -1;
	}

	ret = usbh_cdc_ecm_init(&cdc_ecm_usb_cb, &ecm_priv);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ECM fail\n");
		usbh_deinit();
		return -1;
	}

	do {
		if (usbh_cdc_ecm_usb_is_ready()) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1);

	return 0;
}

static void usb_eth_hotplug_thread(void *param)
{
	int ret;
	UNUSED(param);

	for (;;) {
		usb_os_sema_take(cdc_ecm_detach_sema, USB_OS_SEMA_TIMEOUT);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Hot plug\n");

		usbh_cdc_ecm_deinit();
		usbh_deinit();

		rtos_time_delay_ms(10);

		ret = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
			break;
		}

		ret = usbh_cdc_ecm_init(&cdc_ecm_usb_cb, &ecm_priv);
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ECM fail\n");
			usbh_deinit();
			break;
		}
	}

	rtos_task_delete(NULL);
}

#endif /* CONFIG_USBD_CDC_ECM / CONFIG_USBH_CDC_ECM */

/* ========================================================================== */
/*                     Common: Link Change Thread                              */
/* ========================================================================== */

static void usb_eth_link_change_thread(void *param)
{
	u8 *mac;
	u8 link_is_up = 0;
	usb_eth_state_t state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task\n");

#if defined(CONFIG_USBH_CDC_ECM)
	/* Host mode: USB init blocks until device is ready */
	if (usb_eth_do_usb_init() != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		rtos_task_delete(NULL);
		return;
	}
#endif

	while (1) {
#if defined(CONFIG_USBD_CDC_ECM)
		link_is_up = usbd_cdc_ecm_get_link_status();
		mac = (u8 *)dhcp_server_mac;
		if (cdc_ecm_link_disconnected) {
			cdc_ecm_link_disconnected = 0;
			link_is_up = 0;
		}
#elif defined(CONFIG_USBH_CDC_ECM)
		link_is_up = usbh_cdc_ecm_get_connect_status();
		mac = (u8 *)usbh_cdc_ecm_process_mac_str();
		if (cdc_ecm_detach_pending) {
			cdc_ecm_detach_pending = 0;
			link_is_up = 0;
		}
#endif

		if (link_is_up && state < ETH_STATUS_INIT) {
			if (mac == NULL || pnetif_usb_eth == NULL) {
				rtos_time_delay_ms(1000);
				continue;
			}

			memcpy(pnetif_usb_eth->hwaddr, mac, 6);
			pnetif_usb_eth->hwaddr_len = 6;
			state = ETH_STATUS_INIT;

			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

			if (g_usb_eth_link_cb) {
				g_usb_eth_link_cb(link_is_up);
			}
		} else if (!link_is_up && state >= ETH_STATUS_INIT) {
			state = ETH_STATUS_DEINIT;

			if (g_usb_eth_link_cb) {
				g_usb_eth_link_cb(link_is_up);
			}
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

/* ========================================================================== */
/*                             Public API                                      */
/* ========================================================================== */

void usb_eth_register_link_cb(void (*cb)(int link_up))
{
	g_usb_eth_link_cb = cb;
}

void usb_eth_init(void)
{
	int status;

	rltk_usb_eth_init();

#if defined(CONFIG_USBD_CDC_ECM)
	/* Device mode: init USB controller + ECM class driver synchronously */
	if (usb_eth_do_usb_init() != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB device init fail\n");
		return;
	}
#elif defined(CONFIG_USBH_CDC_ECM)
	/* Host mode: create detach semaphore; USB init deferred to link thread */
	usb_os_sema_create(&cdc_ecm_detach_sema);
#endif

	/* Create hotplug thread */
	status = rtos_task_create(&g_hotplug_task, "usb_eth_hotplug",
							  usb_eth_hotplug_thread, NULL,
							  USB_ETH_HOTPLUG_THREAD_STACK_SIZE,
							  USB_ETH_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
	}

	/* Create link monitoring thread */
	status = rtos_task_create(&g_link_task, "usb_eth_link",
							  usb_eth_link_change_thread, NULL,
							  USB_ETH_LINK_THREAD_STACK_SIZE,
							  USB_ETH_LINK_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create link thread fail\n");
	}
}

#endif /* CONFIG_LWIP_USB_ETHERNET && CONFIG_RNAT */
