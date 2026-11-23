/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
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
#include "usbh_rndis.h"
#include "usb_app.h"
#include "example_usbh_rndis.h"

/* Includes ------------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define USBH_RNDIS_APP_HOT_PLUG                               1     /* Hot plug */

// Thread priorities
#define USBH_RNDIS_APP_INIT_THREAD_PRIORITY                   3
#define USBH_RNDIS_APP_MAIN_THREAD_PRIORITY                   5
#define USBH_RNDIS_APP_HOTPLUG_THREAD_PRIORITY                6
#define USBH_RNDIS_APP_MONITOR_THREAD_PRIORITY                3

// Thread stack sizes
#define USBH_RNDIS_APP_INIT_THREAD_STACK_SIZE                 1536U
#define USBH_RNDIS_APP_HOTPLUG_THREAD_STACK_SIZE              1536U
#define USBH_RNDIS_APP_MONITOR_THREAD_STACK_SIZE              1600U

#define USBH_RNDIS_APP_MAIN_TASK_STACK_SIZE                   1280U

/* Private types -------------------------------------------------------------*/

/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_INIT,
} eth_state_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int rndis_app_cb_init(void);
static void rndis_app_cb_deinit(void);
static void rndis_app_cb_attached(void);
static void rndis_app_cb_detached(void);
static void rndis_app_cb_setup(void);
static void rndis_app_cb_process(usb_host_t *host, u8 msg);
static void rndis_app_cb_received(u8 *pbuf, u32 Len);
static int rndis_app_cb_device_check(usb_host_t *host, u8 cfg_max);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "RNDIS";

static __IO u8 rndis_app_detach_pending = 0;
static rtos_sema_t rndis_app_detach_sema;

/* Serializes the USB stack bring up / tear down against the lwIP TX path.
   The class driver already keeps usbh_rndis_send_data() off the buffers that
   usbh_rndis_deinit() frees; this lock is an application level guard on top of
   it, so the whole teardown and re-init window is closed to TX deterministically.
   Created by the first start and never deleted: TX is driven by the lwIP
   thread, which lives as long as the firmware and can not be joined by the
   example, so there is no point in time at which this mutex is provably
   unreferenced. rndis_app_stack_ready is the gate that keeps callers off the
   stack instead. */
static rtos_mutex_t rndis_app_state_mutex = NULL;

/* 1: the stack is fully up and the class API may be called. Cleared before any
   tear down, so a non-zero value also implies rndis_app_state_mutex is valid. */
static volatile u8 rndis_app_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so a TX queued behind it
   gives up instead of re-taking the lock between lwIP packets and starving this
   thread out of a teardown it must perform. */
static volatile u8 rndis_app_teardown = 0;

/* 1: usbh_init() and usbh_rndis_init() are done, the stack needs a deinit. Changed under
   rndis_app_state_mutex by start, the rebuilding worker and the teardown owner only. */
static u8 rndis_app_stack_up = 0;

/* 1: the link thread is inside its DHCP section and may still bring the netif up or switch the
   default route. Set only if quit was clear, under the same lock usb_app_ctx_claim_stop() sets
   quit with, so the teardown owner either sees it or the link thread sees quit. */
static volatile u8 rndis_app_dhcp_busy = 0;
/* 1: DHCP was requested on pnetif_usb_eth since start, the teardown has to stop it */
static volatile u8 rndis_app_dhcp_ran = 0;

/* Number of worker threads created by start, all joined by the teardown */
static u8 rndis_app_workers = 0;

/* Start/stop state: example_usbh_rndis_stop(), or the hotplug thread when the stack can not
   be rebuilt, sets quit, takes the network down, wakes and joins the workers */
static usb_app_ctx_t rndis_app_ctx;

static const usbh_config_t usbh_rndis_app_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_RNDIS_APP_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_RNDIS_APP_MAIN_THREAD_PRIORITY,
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

static const usbh_rndis_state_cb_t rndis_app_usb_cb = {
	.init   = rndis_app_cb_init,
	.deinit = rndis_app_cb_deinit,
	.attached = rndis_app_cb_attached,
	.detached = rndis_app_cb_detached,
	.setup  = rndis_app_cb_setup,
	.received = rndis_app_cb_received,
};

static const usbh_user_cb_t usbh_rndis_app_usr_cb = {
	.process = rndis_app_cb_process,
	.validate = rndis_app_cb_device_check,
};

static const usbh_rndis_config_t rndis_class_cfg = {
	/* Every field is left 0 to take the class default packet filter */
	0,
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static int rndis_app_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_rndis_check_config_desc(host);
}

/**
  * @brief  RNDIS init callback
  * @retval Status
  */
static int rndis_app_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return RTK_SUCCESS;
}

/**
  * @brief  RNDIS deinit callback
  */
static void rndis_app_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

/**
  * @brief  RNDIS attach callback
  */
static void rndis_app_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
}

/**
  * @brief  RNDIS detach callback
  */
static void rndis_app_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");

	rndis_app_detach_pending = 1;
#if USBH_RNDIS_APP_HOT_PLUG
	rtos_sema_give(rndis_app_detach_sema);
#endif
}

/**
  * @brief  RNDIS setup callback
  */
static void rndis_app_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
}

/**
  * @brief  RNDIS bulk receive callback
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  */
static void rndis_app_cb_received(u8 *buf, u32 length)
{
	netif_adapter_usb_eth_recv(buf, length);
}

static void rndis_app_cb_process(usb_host_t *host, u8 msg)
{
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		if (usbh_rndis_choose_config(host) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Choose config fail\n");
		}
		break;
	case USBH_MSG_DISCONNECTED:
		// usbh_rndis_host_user.rndis_app_is_ready = 0;
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
static int rndis_app_stack_init(void)
{
	int ret;

	ret = usbh_init(&usbh_rndis_app_cfg, &usbh_rndis_app_usr_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return ret;
	}

	ret = usbh_rndis_init(&rndis_app_usb_cb, &rndis_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init RNDIS fail\n");
		usbh_deinit();
		return ret;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	ret = usbh_start();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start USBH fail\n");
		usbh_rndis_deinit();
		usbh_deinit();
		return ret;
	}
	rndis_app_stack_up = 1U;

	return RTK_SUCCESS;
}

/**
  * @brief  Tear the stack down with the TX gate closed under the lock
  * @retval None
  */
static void rndis_app_stack_deinit(void)
{
	/* Announce the teardown BEFORE contending for the lock, so a TX queued
	 * behind it gives up instead of re-taking the lock between lwIP packets
	 * and starving this thread out of a teardown it must perform. */
	rndis_app_teardown = 1U;

	rtos_mutex_take(rndis_app_state_mutex, RTOS_MAX_TIMEOUT);
	/* Close the gate under the lock: no TX is in flight here, and any later
	 * one gives up on the cleared flag. */
	rndis_app_stack_ready = 0U;
	if (rndis_app_stack_up != 0U) {
		usbh_stop();
		usbh_rndis_deinit();
		usbh_deinit();
		rndis_app_stack_up = 0U;
	}
	rtos_mutex_give(rndis_app_state_mutex);
}

/**
  * @brief  Open the TX gate once the device is enumerated, polled by the link thread
  * @retval None
  */
static void rndis_app_open_gate(void)
{
	rtos_mutex_take(rndis_app_state_mutex, RTOS_MAX_TIMEOUT);
	if ((rndis_app_stack_ready == 0U) && (rndis_app_stack_up != 0U) && (rndis_app_teardown == 0U) && (rndis_app_ctx.quit == 0U) &&
		(usbh_rndis_usb_is_ready() != 0U)) {
		/* Open the gate as the very last step, so no TX runs against a half built stack */
		rndis_app_stack_ready = 1U;
	}
	rtos_mutex_give(rndis_app_state_mutex);
}

/**
  * @brief  Make the link thread leave its DHCP section
  * @note   lwip_request_ip() only ends on its own after all its retries, which takes minutes with no device:
  *         stop the DHCP client until the thread leaves, a dhcp_stop() that lands before its dhcp_start()
  *         would be undone by it
  * @retval None
  */
static void rndis_app_abort_dhcp(void)
{
	u32 waited = 0U;

	while ((rndis_app_dhcp_busy != 0U) && (waited < USB_APP_JOIN_TIMEOUT_MS)) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		rtos_time_delay_ms(10);
		waited += 10U;
	}
}

#if USBH_RNDIS_APP_HOT_PLUG
/**
  * @brief  Tear the stack down and bring it up again, unless a stop came in meanwhile
  * @param  down_ms: Time the stack stays down
  * @retval Status of the re-init, RTK_SUCCESS if skipped for a stop
  */
static int rndis_app_rebuild(u32 down_ms)
{
	int ret = RTK_SUCCESS;

	/* The device is gone: a DHCP round still running for it would hold the link thread, and so the
	   re-opening of the TX gate, until it times out */
	rndis_app_abort_dhcp();
	rndis_app_stack_deinit();

	rtos_time_delay_ms(down_ms);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

	/* Under the lock, so the stop owner either sees the stack up or this thread sees quit */
	rtos_mutex_take(rndis_app_state_mutex, RTOS_MAX_TIMEOUT);
	if ((rndis_app_ctx.quit == 0U) && (rndis_app_stack_up == 0U)) {
		ret = rndis_app_stack_init();
		if (ret == RTK_SUCCESS) {
			/* The link thread opens the gate once the device is enumerated again */
			rndis_app_teardown = 0U;
		}
	}
	rtos_mutex_give(rndis_app_state_mutex);

	return ret;
}
#endif

/**
  * @brief  Take pnetif_usb_eth down so lwIP stops sending to this app
  * @note   Called by the stop owner, quit is already set
  * @retval None
  */
static void rndis_app_net_down(void)
{
	rndis_app_abort_dhcp();
	if (rndis_app_dhcp_ran != 0U) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		rndis_app_dhcp_ran = 0U;
	}
	netifapi_netif_set_link_down(pnetif_usb_eth);
	netifapi_netif_set_down(pnetif_usb_eth);
	if (netif_default == pnetif_usb_eth) {
		netifapi_netif_set_default(pnetif_sta);
	}
}

/**
  * @brief  Finish a claimed stop: network down, workers joined, stack and OS objects released
  * @note   rndis_app_state_mutex is kept, see its declaration
  * @param  workers: Number of workers to join
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT with everything kept if a worker did not leave
  */
static int rndis_app_teardown_all(u32 workers)
{
	rndis_app_net_down();

	/* The hotplug thread waits on the detach sema, the others poll quit */
	rtos_sema_give(rndis_app_detach_sema);
	if (usb_app_ctx_join(&rndis_app_ctx, workers) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone. The stack is down already if the hotplug thread left mid-rebuild. */
	rndis_app_stack_deinit();

	rtos_sema_delete(rndis_app_detach_sema);
	rndis_app_detach_sema = NULL;
	rndis_app_workers = 0U;
	rndis_app_detach_pending = 0U;
	rndis_app_dhcp_busy = 0U;
	usb_app_ctx_end(&rndis_app_ctx);

	return RTK_SUCCESS;
}

/**
  * @brief  Last call of every worker
  * @param  fatal: 1 if the stack can not be rebuilt, the app has to go down
  * @retval None
  */
static void rndis_app_worker_exit(u8 fatal)
{
	if ((fatal != 0U) && (usb_app_ctx_claim_stop(&rndis_app_ctx) != 0U)) {
		/* No stop is in flight: finish the teardown here, this thread is not joined */
		if (rndis_app_teardown_all(rndis_app_workers - 1U) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBH RNDIS demo stop\n");
		}
	} else {
		usb_app_ctx_leave(&rndis_app_ctx);
	}
	rtos_task_delete(NULL);
}

static void example_usbh_rndis_app_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u32 link_down_log_cnt = 0;
	u8 link_is_up = 0;
	u8 enter;
	eth_state_t ethernet_state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task\n");

	while (rndis_app_ctx.quit == 0U) {
		/* Start and the hotplug thread only bring the stack up: open the TX gate here once the
		   device is enumerated, so neither of them blocks on the attach */
		rndis_app_open_gate();

		/* Consume detach events first: USB callback context only flags this
		 * bit, so even if the device has already been re-plugged by the time
		 * we poll (link_is_up reads back as 1), we still tear down the eth
		 * default route here. Without this, branch 2 below would miss the
		 * transition and netif_set_default(pnetif_sta) would never run. */
		if (rndis_app_detach_pending) {
			rndis_app_detach_pending = 0;
			if (ethernet_state >= ETH_STATUS_INIT) {
				ethernet_state = ETH_STATUS_IDLE;
				/* Not the tcpip thread: netifapi takes the core lock */
				netifapi_netif_set_default(pnetif_sta);
				netifapi_netif_set_link_down(pnetif_usb_eth);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink (detach event)\n");
			}
		}

		link_is_up = usbh_rndis_get_link_status();

		if (1 == link_is_up && (ethernet_state < ETH_STATUS_INIT)) {	// unlink -> link
			mac = (u8 *)usbh_rndis_get_mac();
			/* Enter the DHCP section only while no stop is claimed, see rndis_app_dhcp_busy */
			enter = 0U;
			if (mac != NULL) {
				rtos_critical_enter(RTOS_CRITICAL_USB);
				if (rndis_app_ctx.quit == 0U) {
					rndis_app_dhcp_busy = 1U;
					rndis_app_dhcp_ran = 1U;
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
				if ((DHCP_ADDRESS_ASSIGNED == dhcp_status) && (rndis_app_ctx.quit == 0U)) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to RNDIS netif
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				}
				/* Left the DHCP section: a stop owner waiting for it takes the netif down now */
				rndis_app_dhcp_busy = 0U;
				if ((DHCP_ADDRESS_ASSIGNED != dhcp_status) && (rndis_app_ctx.quit == 0U)) {
					/* Back to IDLE so the next round retries: a device keeping the link up
					   brings no link-down edge that would bring this branch back */
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
				RTK_LOGS(TAG, RTK_LOG_INFO, "RNDIS link is down (%u)\n", link_down_log_cnt);
			}
			rtos_time_delay_ms(1000);
		}
	}

	rndis_app_worker_exit(0U);
}

#if USBH_RNDIS_APP_HOT_PLUG
static void example_usbh_rndis_app_hotplug_thread(void *param)
{
	u8 fatal = 0U;

	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The lwIP
	   TX thread stays unaffined: a pending teardown is excluded from it by
	   rndis_app_state_mutex, not by core.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	for (;;) {
		if (rtos_sema_take(rndis_app_detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (rndis_app_ctx.quit != 0U) {
			/* Stop requested: the stop owner tears the stack down */
			break;
		}

		/* Re-run the init sequence used by start (usbh_init + rndis_init + usbh_start). The
		 * link thread re-opens the TX gate once the device is enumerated again. */
		if (rndis_app_rebuild(10U) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
			fatal = 1U;
			break;
		}
		if (rndis_app_ctx.quit != 0U) {
			/* Stop requested during the rebuild: the stop owner deinits whatever is up */
			break;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
	}

	if (fatal != 0U) {
		/* A re-init failed, so the stack is gone for good: rndis_app_teardown keeps TX out */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	}
	rndis_app_worker_exit(fatal);
}
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int rndis_app_start(void)
{
	int ret;
	rtos_task_t task;

	ret = usb_app_ctx_begin(&rndis_app_ctx);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	rndis_app_detach_pending = 0U;
	rndis_app_stack_ready = 0U;
	rndis_app_teardown = 0U;
	rndis_app_dhcp_busy = 0U;
	rndis_app_dhcp_ran = 0U;
	rndis_app_workers = 0U;

	/* Created by the first start and never deleted, see the declaration */
	if (rndis_app_state_mutex == NULL) {
		ret = rtos_mutex_create(&rndis_app_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			usb_app_ctx_end(&rndis_app_ctx);
			return ret;
		}
	}

	ret = rtos_sema_create(&rndis_app_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		usb_app_ctx_end(&rndis_app_ctx);
		return ret;
	}

	rltk_usb_eth_init();

	ret = rndis_app_stack_init();
	if (ret != RTK_SUCCESS) {
		goto exit_stop;
	}

	ret = rtos_task_create(&task, "usbh_rndis_app_link_change_thread",
						   example_usbh_rndis_app_link_change_thread, NULL,
						   USBH_RNDIS_APP_MONITOR_THREAD_STACK_SIZE, USBH_RNDIS_APP_MONITOR_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
		goto exit_stop;
	}
	rndis_app_workers++;

#if USBH_RNDIS_APP_HOT_PLUG
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&task, "usbh_rndis_app_hotplug_thread",
						   example_usbh_rndis_app_hotplug_thread, NULL,
						   USBH_RNDIS_APP_HOTPLUG_THREAD_STACK_SIZE, USBH_RNDIS_APP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
		goto exit_stop;
	}
	rndis_app_workers++;
#endif

	return RTK_SUCCESS;

exit_stop:
	/* No hotplug thread runs yet, the only worker that can claim the stop on a fatal error,
	   so this claim is normally won here */
	if (usb_app_ctx_claim_stop(&rndis_app_ctx) != 0U) {
		if (rndis_app_teardown_all(rndis_app_workers) != RTK_SUCCESS) {
			/* A worker may still run on the stack: keep it, the app stays not idle */
			return -RTK_ERR_TIMEOUT;
		}
	} else if (usb_app_ctx_wait_idle(&rndis_app_ctx) != RTK_SUCCESS) {
		return -RTK_ERR_TIMEOUT;
	}
#ifndef USB_APP_AGGREGATE
	rltk_usb_eth_deinit();
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH RNDIS demo aborted\n");

	return ret;
}

/* Standalone entry thread: brings the demo up and leaves */
static void example_usbh_rndis_thread(void *param)
{
	UNUSED(param);

#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* A start that fails rolls back through usbh_deinit(), which waits for no ISR on another
	   core: run it on CPU0, where the USB IRQ is delivered, as the hotplug thread does.
	   Done by the thread itself, first: a created thread may already run on another core, and its
	   creator pinning it afterwards could come too late or reach a task that has deleted itself. */
	rtos_task_set_affinity(rtos_task_handle_get(), 0);
#endif

	if (rndis_app_start() != RTK_SUCCESS) {
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

	/* Gate on rndis_app_stack_ready BEFORE touching the mutex: the handle is still
	   NULL if the example never started, and the flag is already cleared if a
	   teardown is pending. A hotplug right after this check is harmless, the mutex
	   stays valid and the re-check under it skips the TX. */
	if ((rndis_app_stack_ready == 0U) || (rndis_app_teardown != 0U) || (rndis_app_ctx.quit != 0U)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(rndis_app_state_mutex, RTOS_MAX_TIMEOUT);
	if ((rndis_app_stack_ready == 0U) || (rndis_app_teardown != 0U) || (rndis_app_ctx.quit != 0U)) {
		/* Lost the race against a teardown */
		ret = -RTK_ERR_IO;
	} else {
		ret = usbh_rndis_send_data(buf, len, block);
	}
	rtos_mutex_give(rndis_app_state_mutex);

	return ret;
}

/**
  * @brief  Start the RNDIS host, used by the USB app manager
  * @note   Task context only. Returns once the stack is up, DHCP runs in the link thread.
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbh_rndis_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH RNDIS demo start\n");

	return rndis_app_start();
}

/**
  * @brief  Stop the RNDIS host and release everything it owns
  * @note   Task context only. Safe to call when not running. pnetif_usb_eth is taken down first.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbh_rndis_stop(void)
{
	int ret;

	if (usb_app_ctx_claim_stop(&rndis_app_ctx) == 0U) {
		/* Idle, or a worker found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&rndis_app_ctx);
	}

	ret = rndis_app_teardown_all(rndis_app_workers);
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH RNDIS demo stop\n");
	}

	return ret;
}

/**
  * @brief  Example entry: start the RNDIS host from its own thread
  * @retval None
  */
void example_usbh_rndis(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH RNDIS demo start\n");

	ret = rtos_task_create(&task, "usbh_rndis_app_init_thread", example_usbh_rndis_thread, NULL,
						   USBH_RNDIS_APP_INIT_THREAD_STACK_SIZE, USBH_RNDIS_APP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
#else
#error "No Lwip USB Ethernet Configuration"
#endif
