/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "os_wrapper.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "lwip/tcpip.h"
#include "usbh_cdc_ecm.h"
#include "usb_app.h"
#include "example_usbh_cdc_ecm.h"

/* Includes ------------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_ECM_HOT_PLUG                               1     /* Hot plug */

// Thread priorities
#define CONFIG_USBH_CDC_ECM_INIT_THREAD_PRIORITY                   3
#define CONFIG_USBH_CDC_ECM_MAIN_THREAD_PRIORITY                   5
#define CONFIG_USBH_CDC_ECM_HOTPLUG_THREAD_PRIORITY                6
#define CONFIG_USBH_CDC_ECM_MONITOR_THREAD_PRIORITY                3

// Thread stack sizes
#define CONFIG_USBH_CDC_ECM_INIT_THREAD_STACK_SIZE                 1536U
#define CONFIG_USBH_CDC_ECM_HOTPLUG_THREAD_STACK_SIZE              1536U
#define CONFIG_USBH_CDC_ECM_MONITOR_THREAD_STACK_SIZE              1600U

#define CONFIG_USBH_CDC_ECM_MAIN_TASK_STACK_SIZE                   1280U

#define CONFIG_USBH_CDC_ECM_ENABLE_USER_SET_DONGLE_MAC             1

/* Private types -------------------------------------------------------------*/

/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_INIT,
} eth_state_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_ecm_cb_init(void);
static void cdc_ecm_cb_deinit(void);
static void cdc_ecm_cb_attached(void);
static void cdc_ecm_cb_detached(void);
static void cdc_ecm_cb_setup(void);
static void cdc_ecm_cb_process(usb_host_t *host, u8 msg);
static void cdc_ecm_cb_received(u8 *pbuf, u32 Len);
static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECM";

#if CONFIG_USBH_CDC_ECM_ENABLE_USER_SET_DONGLE_MAC
static const u16 led_color[1] = {0x1122};
static const u8 mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

static __IO u8 cdc_ecm_detach_pending = 0;
static rtos_sema_t cdc_ecm_detach_sema;

/* Serializes the USB stack bring up / tear down against the lwIP TX path.
   The class driver already keeps usbh_cdc_ecm_send_data() off the buffers that
   usbh_cdc_ecm_deinit() frees; this lock is an application level guard on top of
   it, so the whole teardown and re-init window is closed to TX deterministically.
   Created by the first start and never deleted: TX is driven by the lwIP
   thread, which lives as long as the firmware and can not be joined by the
   example, so there is no point in time at which this mutex is provably
   unreferenced. cdc_ecm_stack_ready is the gate that keeps callers off the
   stack instead. */
static rtos_mutex_t cdc_ecm_state_mutex = NULL;

/* 1: the stack is fully up and the class API may be called. Cleared before any
   tear down, so a non-zero value also implies cdc_ecm_state_mutex is valid. */
static volatile u8 cdc_ecm_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so a TX queued behind it
   gives up instead of re-taking the lock between lwIP packets and starving this
   thread out of a teardown it must perform. */
static volatile u8 cdc_ecm_teardown = 0;

/* 1: usbh_init() and usbh_cdc_ecm_init() are done, the stack needs a deinit. Changed under
   cdc_ecm_state_mutex by start, the rebuilding worker and the teardown owner only. */
static u8 cdc_ecm_stack_up = 0;

/* 1: the link thread is inside its DHCP section and may still bring the netif up or switch the
   default route. Set only if quit was clear, under the same lock usb_app_ctx_claim_stop() sets
   quit with, so the teardown owner either sees it or the link thread sees quit. */
static volatile u8 cdc_ecm_dhcp_busy = 0;
/* 1: DHCP was requested on pnetif_usb_eth since start, the teardown has to stop it */
static volatile u8 cdc_ecm_dhcp_ran = 0;

/* Number of worker threads created by start, all joined by the teardown */
static u8 cdc_ecm_workers = 0;

/* Start/stop state: example_usbh_cdc_ecm_stop(), or the hotplug thread when the stack can not
   be rebuilt, sets quit, takes the network down, wakes and joins the workers */
static usb_app_ctx_t cdc_ecm_app;

static const usbh_config_t usbh_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = CONFIG_USBH_CDC_ECM_MAIN_TASK_STACK_SIZE,
	.main_task_priority = CONFIG_USBH_CDC_ECM_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.hub_support = 1U,
#if defined(CONFIG_AMEBAGREEN2)
	/*FIFO total 1024 DWORD, resv 12 DWORD for DMA*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	/*FIFO total 1024 DWORD, resv 48 DWORD */
	.rx_fifo_depth = 464,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif !defined(CONFIG_SUPPORT_USB_SHARED_DFIFO)
#error "Set the USB host FIFO depths for this SoC"
#endif
};

static const usbh_cdc_ecm_state_cb_t cdc_ecm_usb_cb = {
	.init   = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.attached = cdc_ecm_cb_attached,
	.detached = cdc_ecm_cb_detached,
	.setup  = cdc_ecm_cb_setup,
	.received = cdc_ecm_cb_received,
};

static const usbh_user_cb_t usbh_ecm_usr_cb = {
	.process = cdc_ecm_cb_process,
	.validate = cdc_ecm_cb_device_check,
};

static const usbh_cdc_ecm_config_t ecm_class_cfg = {
#if CONFIG_USBH_CDC_ECM_ENABLE_USER_SET_DONGLE_MAC
	led_color,
	mac_valid,
	sizeof(led_color) / sizeof(led_color[0]),
#else
	NULL,
	NULL,
	0,
#endif
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

/**
  * @brief  CDC ECM init callback
  * @retval Status
  */
static int cdc_ecm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return RTK_SUCCESS;
}

/**
  * @brief  CDC ECM deinit callback
  */
static void cdc_ecm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

/**
  * @brief  CDC ECM attach callback
  */
static void cdc_ecm_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
}

/**
  * @brief  CDC ECM detach callback
  */
static void cdc_ecm_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");

	cdc_ecm_detach_pending = 1;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG
	rtos_sema_give(cdc_ecm_detach_sema);
#endif
}

/**
  * @brief  CDC ECM setup callback
  */
static void cdc_ecm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
}

/**
  * @brief  CDC ECM bulk receive callback
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  */
static void cdc_ecm_cb_received(u8 *buf, u32 length)
{
	netif_adapter_usb_eth_recv(buf, length);
}

static void cdc_ecm_cb_process(usb_host_t *host, u8 msg)
{
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		if (usbh_cdc_ecm_choose_config(host) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Choose config fail\n");
		}
		break;
	case USBH_MSG_DISCONNECTED:
		// usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}
}

/**
  * @brief  Bring the USB stack up in the documented order, the TX gate stays closed
  * @retval Status
  */
static int cdc_ecm_stack_init(void)
{
	int ret;

	ret = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return ret;
	}

	ret = usbh_cdc_ecm_init(&cdc_ecm_usb_cb, &ecm_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ECM fail\n");
		usbh_deinit();
		return ret;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	ret = usbh_start();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start USBH fail\n");
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		return ret;
	}
	cdc_ecm_stack_up = 1U;

	return RTK_SUCCESS;
}

/**
  * @brief  Tear the stack down with the TX gate closed under the lock
  * @retval None
  */
static void cdc_ecm_stack_deinit(void)
{
	/* Announce the teardown BEFORE contending for the lock, so a TX queued
	 * behind it gives up instead of re-taking the lock between lwIP packets
	 * and starving this thread out of a teardown it must perform. */
	cdc_ecm_teardown = 1U;

	rtos_mutex_take(cdc_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	/* Close the gate under the lock: no TX is in flight here, and any later
	 * one gives up on the cleared flag. */
	cdc_ecm_stack_ready = 0U;
	if (cdc_ecm_stack_up != 0U) {
		usbh_stop();
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		cdc_ecm_stack_up = 0U;
	}
	rtos_mutex_give(cdc_ecm_state_mutex);
}

/**
  * @brief  Open the TX gate once the device is enumerated, polled by the link thread
  * @retval None
  */
static void cdc_ecm_open_gate(void)
{
	rtos_mutex_take(cdc_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	if ((cdc_ecm_stack_ready == 0U) && (cdc_ecm_stack_up != 0U) && (cdc_ecm_teardown == 0U) && (cdc_ecm_app.quit == 0U) &&
		(usbh_cdc_ecm_usb_is_ready() != 0U)) {
		/* USB enumerated and upper layer is ready: release the SOF data-transfer
		 * gate so bulk/intr scheduling can begin (see usbh_cdc_ecm_sof()).
		 * Runs on both the initial bring-up and every hot-plug re-init, re-arming
		 * the gate that detach() cleared. For devices needing upper-layer prep
		 * (e.g. 4G AT config), gate this on the real ready signal instead of usb_is_ready(). */
		usbh_cdc_ecm_prepare_done();
		/* Open the gate as the very last step, so no TX runs against a half built stack */
		cdc_ecm_stack_ready = 1U;
	}
	rtos_mutex_give(cdc_ecm_state_mutex);
}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG
/**
  * @brief  Tear the stack down and bring it up again, unless a stop came in meanwhile
  * @param  down_ms: Time the stack stays down
  * @retval Status of the re-init, RTK_SUCCESS if skipped for a stop
  */
static int cdc_ecm_rebuild(u32 down_ms)
{
	int ret = RTK_SUCCESS;

	cdc_ecm_stack_deinit();

	rtos_time_delay_ms(down_ms);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

	/* Under the lock, so the stop owner either sees the stack up or this thread sees quit */
	rtos_mutex_take(cdc_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	if ((cdc_ecm_app.quit == 0U) && (cdc_ecm_stack_up == 0U)) {
		ret = cdc_ecm_stack_init();
		if (ret == RTK_SUCCESS) {
			/* The link thread opens the gate once the device is enumerated again */
			cdc_ecm_teardown = 0U;
		}
	}
	rtos_mutex_give(cdc_ecm_state_mutex);

	return ret;
}
#endif

/**
  * @brief  Take pnetif_usb_eth down so lwIP stops sending to this app
  * @note   Called by the stop owner, quit is already set
  * @retval None
  */
static void cdc_ecm_net_down(void)
{
	u32 waited = 0U;

	/* The link thread may be inside lwip_request_ip(): stop the DHCP client until it
	   leaves, a dhcp_stop() that lands before its dhcp_start() would be undone by it */
	while ((cdc_ecm_dhcp_busy != 0U) && (waited < USB_APP_JOIN_TIMEOUT_MS)) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		rtos_time_delay_ms(10);
		waited += 10U;
	}
	if (cdc_ecm_dhcp_ran != 0U) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		cdc_ecm_dhcp_ran = 0U;
	}
	netifapi_netif_set_link_down(pnetif_usb_eth);
	netifapi_netif_set_down(pnetif_usb_eth);
	if (netif_default == pnetif_usb_eth) {
		netifapi_netif_set_default(pnetif_sta);
	}
}

/**
  * @brief  Finish a claimed stop: network down, workers joined, stack and OS objects released
  * @note   cdc_ecm_state_mutex is kept, see its declaration
  * @param  workers: Number of workers to join
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT with everything kept if a worker did not leave
  */
static int cdc_ecm_teardown_all(u32 workers)
{
	cdc_ecm_net_down();

	/* The hotplug thread waits on the detach sema, the others poll quit */
	rtos_sema_give(cdc_ecm_detach_sema);
	if (usb_app_ctx_join(&cdc_ecm_app, workers) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone. The stack is down already if the hotplug thread left mid-rebuild. */
	cdc_ecm_stack_deinit();

	rtos_sema_delete(cdc_ecm_detach_sema);
	cdc_ecm_detach_sema = NULL;
	cdc_ecm_workers = 0U;
	cdc_ecm_detach_pending = 0U;
	cdc_ecm_dhcp_busy = 0U;
	usb_app_ctx_end(&cdc_ecm_app);

	return RTK_SUCCESS;
}

/**
  * @brief  Last call of every worker
  * @param  fatal: 1 if the stack can not be rebuilt, the app has to go down
  * @retval None
  */
static void cdc_ecm_worker_exit(u8 fatal)
{
	if ((fatal != 0U) && (usb_app_ctx_claim_stop(&cdc_ecm_app) != 0U)) {
		/* No stop is in flight: finish the teardown here, this thread is not joined */
		if (cdc_ecm_teardown_all(cdc_ecm_workers - 1U) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ECM demo stop\n");
		}
	} else {
		usb_app_ctx_leave(&cdc_ecm_app);
	}
	rtos_task_delete(NULL);
}

static void example_usbh_ecm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u32 link_down_log_cnt = 0;
	u8 link_is_up = 0;
	u8 enter;
	eth_state_t ethernet_state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task\n");

	while (cdc_ecm_app.quit == 0U) {
		/* Start and the hotplug thread only bring the stack up: open the TX gate here once the
		   device is enumerated, so neither of them blocks on the attach */
		cdc_ecm_open_gate();

		/* Consume detach events first: USB callback context only flags this
		 * bit, so even if the dongle has already been re-plugged by the time
		 * we poll (link_is_up reads back as 1), we still tear down the eth
		 * default route here. Without this, branch 2 below would miss the
		 * transition and netif_set_default(pnetif_sta) would never run. */
		if (cdc_ecm_detach_pending) {
			cdc_ecm_detach_pending = 0;
			if (ethernet_state >= ETH_STATUS_INIT) {
				ethernet_state = ETH_STATUS_IDLE;
				/* Not the tcpip thread: netifapi takes the core lock */
				netifapi_netif_set_default(pnetif_sta);
				netifapi_netif_set_link_down(pnetif_usb_eth);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink (detach event)\n");
			}
		}

		link_is_up = usbh_cdc_ecm_get_link_status();

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {	// unlink -> link
			mac = (u8 *)usbh_cdc_ecm_process_mac_str();
			/* Enter the DHCP section only while no stop is claimed, see cdc_ecm_dhcp_busy */
			enter = 0U;
			if (mac != NULL) {
				rtos_critical_enter(RTOS_CRITICAL_USB);
				if (cdc_ecm_app.quit == 0U) {
					cdc_ecm_dhcp_busy = 1U;
					cdc_ecm_dhcp_ran = 1U;
					enter = 1U;
				}
				rtos_critical_exit(RTOS_CRITICAL_USB);
			}
			if (mac == NULL) {
				rtos_time_delay_ms(1000);
			} else if (enter != 0U) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				ethernet_state = ETH_STATUS_INIT;
				/* hwaddr is read by the tcpip thread, written under its core lock */
				LOCK_TCPIP_CORE();
				memcpy((void *)pnetif_usb_eth->hwaddr, (const void *)mac, 6);
				UNLOCK_TCPIP_CORE();
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netifapi_netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if ((DHCP_ADDRESS_ASSIGNED == dhcp_status) && (cdc_ecm_app.quit == 0U)) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to ether netif
					/* From here AT+IPERF/AT+IPERF3 run over the dongle. iperf3 needs -l (at most 2920, 1460
					 * recommended) on both the board and the PC side, see README "Throughput test" */
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				}
				/* Left the DHCP section: a stop owner waiting for it takes the netif down now */
				cdc_ecm_dhcp_busy = 0U;
				if ((DHCP_ADDRESS_ASSIGNED != dhcp_status) && (cdc_ecm_app.quit == 0U)) {
					/* Back to IDLE so the next round retries: a 4G dongle reports the link up for good,
					   so no link-down edge would ever bring this branch back */
					RTK_LOGS(TAG, RTK_LOG_ERROR, "DHCP fail, retry\n");
					ethernet_state = ETH_STATUS_IDLE;
					rtos_time_delay_ms(1000);
				}
			}
		} else if (0 == link_is_up && (ethernet_state >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_state = ETH_STATUS_IDLE;
			netifapi_netif_set_default(pnetif_sta);
			/* Clear the link flag and the ARP entries of the gone USB interface */
			netifapi_netif_set_link_down(pnetif_usb_eth);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
		} else {
			if ((link_is_up == 0) && (++link_down_log_cnt % 10 == 1)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ECM link is down (%u)\n", link_down_log_cnt);
			}
			rtos_time_delay_ms(1000);
		}
	}

	cdc_ecm_worker_exit(0U);
}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG
static void example_usbh_ecm_hotplug_thread(void *param)
{
	u8 fatal = 0U;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_ecm_detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (cdc_ecm_app.quit != 0U) {
			/* Stop requested: the stop owner tears the stack down */
			break;
		}

		/* Re-run the init sequence used by start (usbh_init + ecm_init + usbh_start). The
		 * link thread re-opens the TX gate once the device is enumerated again. */
		if (cdc_ecm_rebuild(10U) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
			fatal = 1U;
			break;
		}
		if (cdc_ecm_app.quit != 0U) {
			/* Stop requested during the rebuild: the stop owner deinits whatever is up */
			break;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
	}

	if (fatal != 0U) {
		/* A re-init failed, so the stack is gone for good: cdc_ecm_teardown keeps TX out */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	}
	cdc_ecm_worker_exit(fatal);
}
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int cdc_ecm_start(void)
{
	int ret;
	rtos_task_t task;

	ret = usb_app_ctx_begin(&cdc_ecm_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	cdc_ecm_detach_pending = 0U;
	cdc_ecm_stack_ready = 0U;
	cdc_ecm_teardown = 0U;
	cdc_ecm_dhcp_busy = 0U;
	cdc_ecm_dhcp_ran = 0U;
	cdc_ecm_workers = 0U;

	/* Created by the first start and never deleted, see the declaration */
	if (cdc_ecm_state_mutex == NULL) {
		ret = rtos_mutex_create(&cdc_ecm_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			usb_app_ctx_end(&cdc_ecm_app);
			return ret;
		}
	}

	ret = rtos_sema_create(&cdc_ecm_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		usb_app_ctx_end(&cdc_ecm_app);
		return ret;
	}

	rltk_usb_eth_init();

	ret = cdc_ecm_stack_init();
	if (ret != RTK_SUCCESS) {
		goto exit_stop;
	}

	ret = rtos_task_create(&task, "usbh_ecm_link_change_thread",
						   example_usbh_ecm_link_change_thread, NULL,
						   CONFIG_USBH_CDC_ECM_MONITOR_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ECM_MONITOR_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
		goto exit_stop;
	}
	cdc_ecm_workers++;

#if CONFIG_USBH_CDC_ECM_HOT_PLUG
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&task, "usbh_ecm_hotplug_thread",
						   example_usbh_ecm_hotplug_thread, NULL,
						   CONFIG_USBH_CDC_ECM_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ECM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
		goto exit_stop;
	}
	cdc_ecm_workers++;
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The lwIP
	   TX thread stays unaffined: a pending teardown is excluded from it by
	   cdc_ecm_state_mutex, not by core. */
	rtos_task_set_affinity(task, 0);
#endif
#endif

	return RTK_SUCCESS;

exit_stop:
	/* No hotplug thread runs yet, the only worker that can claim the stop on a fatal error,
	   so this claim is normally won here */
	if (usb_app_ctx_claim_stop(&cdc_ecm_app) != 0U) {
		if (cdc_ecm_teardown_all(cdc_ecm_workers) != RTK_SUCCESS) {
			/* A worker may still run on the stack: keep it, the app stays not idle */
			return -RTK_ERR_TIMEOUT;
		}
	} else if (usb_app_ctx_wait_idle(&cdc_ecm_app) != RTK_SUCCESS) {
		return -RTK_ERR_TIMEOUT;
	}
#ifndef USB_APP_AGGREGATE
	rltk_usb_eth_deinit();
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH CDC ECM demo aborted\n");

	return ret;
}

/* Standalone entry thread: brings the demo up and leaves */
static void example_usbh_cdc_ecm_thread(void *param)
{
	UNUSED(param);

	if (cdc_ecm_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  lwIP USB Ethernet TX
  * @param  buf: Frame
  * @param  len: Frame length in bytes
  * @param  block: Wait for the transfer to complete
  * @retval Status, -RTK_ERR_IO if the stack is not up or the app is stopping
  */
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	int ret;

	/* Gate on cdc_ecm_stack_ready BEFORE touching the mutex: the handle is still
	   NULL if the example never started, and the flag is already cleared if a
	   teardown is pending. A hotplug right after this check is harmless, the mutex
	   stays valid and the re-check under it skips the TX. */
	if ((cdc_ecm_stack_ready == 0U) || (cdc_ecm_teardown != 0U) || (cdc_ecm_app.quit != 0U)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(cdc_ecm_state_mutex, RTOS_MAX_TIMEOUT);
	if ((cdc_ecm_stack_ready == 0U) || (cdc_ecm_teardown != 0U) || (cdc_ecm_app.quit != 0U)) {
		/* Lost the race against a teardown */
		ret = -RTK_ERR_IO;
	} else {
		ret = usbh_cdc_ecm_send_data(buf, len, block);
	}
	rtos_mutex_give(cdc_ecm_state_mutex);

	return ret;
}

/**
  * @brief  Start the CDC ECM host, used by the USB app manager
  * @note   Task context only. Returns once the stack is up, DHCP runs in the link thread.
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbh_cdc_ecm_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ECM demo start\n");

	return cdc_ecm_start();
}

/**
  * @brief  Stop the CDC ECM host and release everything it owns
  * @note   Task context only. Safe to call when not running. pnetif_usb_eth is taken down first.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbh_cdc_ecm_stop(void)
{
	int ret;

	if (usb_app_ctx_claim_stop(&cdc_ecm_app) == 0U) {
		/* Idle, or a worker found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&cdc_ecm_app);
	}

	ret = cdc_ecm_teardown_all(cdc_ecm_workers);
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ECM demo stop\n");
	}

	return ret;
}

/**
  * @brief  Example entry: start the CDC ECM host from its own thread
  * @retval None
  */
void example_usbh_cdc_ecm(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ECM demo start\n");

	ret = rtos_task_create(&task, "usbh_ecm_init_thread", example_usbh_cdc_ecm_thread, NULL,
						   CONFIG_USBH_CDC_ECM_INIT_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ECM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
#else
#error "No Lwip USB Ethernet Configuration"
#endif
