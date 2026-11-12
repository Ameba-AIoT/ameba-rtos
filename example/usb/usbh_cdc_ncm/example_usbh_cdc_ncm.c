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
#include "usbh_cdc_ncm.h"
#include "usb_app.h"
#include "example_usbh_cdc_ncm.h"

/* Includes ------------------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_NCM_HOTPLUG                            1     /* Hot plug */

#define CONFIG_USBH_CDC_NCM_MAIN_THREAD_PRIORITY               5
#define CONFIG_USBH_CDC_NCM_HOTPLUG_THREAD_PRIORITY            6
#define CONFIG_USBH_CDC_NCM_ISR_PRIORITY                       INT_PRI_MIDDLE

#define CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC                1

/* Private types -------------------------------------------------------------*/

/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_INIT,
} eth_state_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_ncm_cb_init(void);
static void usbh_cdc_ncm_cb_deinit(void);
static void usbh_cdc_ncm_cb_attached(void);
static void usbh_cdc_ncm_cb_detached(void);
static void usbh_cdc_ncm_cb_setup(void);
static void usbh_cdc_ncm_cb_process(usb_host_t *host, u8 msg);
static void usbh_cdc_ncm_cb_received(u8 *pbuf, u32 Len);
static int usbh_cdc_ncm_cb_device_check(usb_host_t *host, u8 cfg_max);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "NCM";

#if CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC
static const u8 cdc_ncm_mac_valid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

static __IO u8 cdc_ncm_detach_pending = 0;
static rtos_sema_t cdc_ncm_detach_sema;

/* Serializes the USB stack bring up / tear down against the lwIP TX path.
   The class driver already keeps usbh_cdc_ncm_transmit() off the buffers that
   usbh_cdc_ncm_deinit() frees; this lock is an application level guard on top of
   it, so the whole teardown and re-init window is closed to TX deterministically.
   Created before the stack comes up and never deleted: TX is driven by the lwIP
   thread, which lives as long as the firmware and can not be joined by the
   example, so there is no point in time at which this mutex is provably
   unreferenced. cdc_ncm_stack_ready is the gate that keeps callers off the stack
   instead. */
static rtos_mutex_t cdc_ncm_state_mutex = NULL;

/* 1: the stack is fully up and the class API may be called. Cleared before any
   tear down, so a non-zero value also implies cdc_ncm_state_mutex is valid. */
static volatile u8 cdc_ncm_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so a TX queued behind it
   gives up instead of re-taking the lock between lwIP packets and starving this
   thread out of a teardown it must perform. */
static volatile u8 cdc_ncm_teardown = 0;

/* 1: usbh_init() and usbh_cdc_ncm_init() are done, whether the gate is open yet or not. Only
   changed by the stack owner: start, the hotplug thread, or the stop owner once it joined them. */
static u8 cdc_ncm_stack_up = 0;

/* Start/stop state: example_usbh_cdc_ncm_stop(), or the hotplug thread when the stack can not be
   rebuilt, owns the teardown; every worker leaves through quit */
static usb_app_ctx_t cdc_ncm_app;

/* Worker threads of the running app, joined by the teardown owner */
static u32 cdc_ncm_workers = 0;

/* 1 while the link change thread runs, cleared by it once it took the netif down */
static volatile u8 cdc_ncm_link_alive = 0;

/* 1 while the link change thread is in lwip_request_ip(): the teardown owner keeps stopping DHCP
   so the request returns, even one started just after quit was set */
static volatile u8 cdc_ncm_dhcp_pending = 0;

static const usbh_config_t usbh_ncm_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = CONFIG_USBH_CDC_NCM_ISR_PRIORITY,
	.main_task_stack_size = 1280U,
	.main_task_priority = CONFIG_USBH_CDC_NCM_MAIN_THREAD_PRIORITY,
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

static const usbh_cdc_ncm_state_cb_t cdc_ncm_usb_cb = {
	.init   = usbh_cdc_ncm_cb_init,
	.deinit = usbh_cdc_ncm_cb_deinit,
	.attached = usbh_cdc_ncm_cb_attached,
	.detached = usbh_cdc_ncm_cb_detached,
	.setup  = usbh_cdc_ncm_cb_setup,
	.received = usbh_cdc_ncm_cb_received,
};

static const usbh_user_cb_t usbh_ncm_usr_cb = {
	.process = usbh_cdc_ncm_cb_process,
	.validate = usbh_cdc_ncm_cb_device_check,
};

static const usbh_cdc_ncm_config_t cdc_ncm_class_cfg = {
#if CONFIG_USBH_CDC_NCM_USER_SET_DONGLE_MAC
	.mac_value = cdc_ncm_mac_valid,
#else
	/* mac_value left NULL: the MAC is taken from the dongle during enumeration. */
#endif
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Validates whether the connected USB device supports CDC NCM.
  * @note   Called by the USB host stack during enumeration when multiple devices
  *         are connected to a hub.  Delegates to usbh_cdc_ncm_check_config_desc()
  *         to verify the configuration descriptor contains both the NCM
  *         Communication Interface and the NCM Data Interface.
  * @param[in] host: USB host handle.
  * @param[in] cfg_max: Number of configurations (unused).
  * @return RTK_SUCCESS if the device is a valid CDC NCM device, non-zero otherwise.
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
	return RTK_SUCCESS;
}

/**
  * @brief  CDC NCM deinit callback
  */
static void usbh_cdc_ncm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

/**
  * @brief  CDC NCM attach callback
  */
static void usbh_cdc_ncm_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
}

/**
  * @brief  CDC NCM detach callback
  */
static void usbh_cdc_ncm_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");

	cdc_ncm_detach_pending = 1;
#if CONFIG_USBH_CDC_NCM_HOTPLUG
	rtos_sema_give(cdc_ncm_detach_sema);
#endif
}

/**
  * @brief  CDC NCM setup callback
  */
static void usbh_cdc_ncm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
}

/**
  * @brief  CDC NCM bulk receive callback
  * @param  buf: RX buffer
  * @param  length: RX data length (in bytes)
  */
static void usbh_cdc_ncm_cb_received(u8 *buf, u32 length)
{
	netif_adapter_usb_eth_recv(buf, length);
}

static void usbh_cdc_ncm_cb_process(usb_host_t *host, u8 msg)
{
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		if (usbh_cdc_ncm_choose_config(host) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Choose config fail\n");
		}
		break;
	case USBH_MSG_DISCONNECTED:
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}
}

/**
  * @brief  Bring the USB host stack and the CDC NCM class up, the TX gate stays closed
  * @retval RTK_SUCCESS, or the init error with nothing left initialized
  */
static int usbh_cdc_ncm_stack_init(void)
{
	int ret = 0;

	ret = usbh_init(&usbh_ncm_cfg, &usbh_ncm_usr_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return ret;
	}

	ret = usbh_cdc_ncm_init(&cdc_ncm_usb_cb, &cdc_ncm_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC NCM fail\n");
		usbh_deinit();
		return ret;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	ret = usbh_start();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start USBH fail\n");
		usbh_cdc_ncm_deinit();
		usbh_deinit();
		return ret;
	}
	cdc_ncm_stack_up = 1U;

	return RTK_SUCCESS;
}

/**
  * @brief  Close the TX gate under the lock and take the stack down if it is up
  * @note   Stack owner only: the hotplug thread, or start/stop once no worker can rebuild it
  * @retval None
  */
static void usbh_cdc_ncm_stack_down(void)
{
	/* Announce the teardown BEFORE contending for the lock, so a TX queued
	 * behind it gives up instead of re-taking the lock between lwIP packets
	 * and starving this thread out of a teardown it must perform. */
	cdc_ncm_teardown = 1U;

	rtos_mutex_take(cdc_ncm_state_mutex, RTOS_MAX_TIMEOUT);
	/* Close the gate under the lock: no TX is in flight here, and any later
	 * one gives up on the cleared flag. */
	cdc_ncm_stack_ready = 0U;

	if (cdc_ncm_stack_up != 0U) {
		//stop isr
		usbh_stop();
		usbh_cdc_ncm_deinit();
		usbh_deinit();
		cdc_ncm_stack_up = 0U;
	}
	rtos_mutex_give(cdc_ncm_state_mutex);
}

/**
  * @brief  Wait for the device to be configured, then open the TX gate
  * @param  rebuild: 1 from the hotplug thread after its re-init, 0 from the link change thread
  * @retval 1 once configured, 0 if a stop came first
  */
static u8 usbh_cdc_ncm_open_gate(u8 rebuild)
{
	while (usbh_cdc_ncm_is_configured() == 0U) {
		if (cdc_ncm_app.quit != 0U) {
			return 0U;
		}
		rtos_time_delay_ms(1000);
	} //wait usb init success

	/* Open the gate as the very last step, so no TX runs against a half built
	   stack. Teardown is cleared alongside it because a TX that saw neither flag
	   would slip through. The link change thread leaves the gate to a rebuild
	   that started meanwhile. */
	rtos_mutex_take(cdc_ncm_state_mutex, RTOS_MAX_TIMEOUT);
	if ((rebuild != 0U) || (cdc_ncm_teardown == 0U)) {
		cdc_ncm_stack_ready = 1U;
		cdc_ncm_teardown = 0U;
	}
	rtos_mutex_give(cdc_ncm_state_mutex);

	return 1U;
}

/**
  * @brief  Take the netif down so lwIP stops sending to the USB link
  * @note   Link change thread only, on its way out
  * @param  dhcp_ran: 1 if DHCP ran on the netif and the default route may point to it
  * @retval None
  */
static void usbh_ncm_net_down(u8 dhcp_ran)
{
	if (dhcp_ran != 0U) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
	}
	netifapi_netif_set_link_down(pnetif_usb_eth);
	netifapi_netif_set_down(pnetif_usb_eth);
	if (dhcp_ran != 0U) {
		/* As on detach: give the default route back to the STA netif */
		netifapi_netif_set_default(pnetif_sta);
	}
}

/**
  * @brief  Wait for the link change thread to take the netif down and leave
  * @note   Teardown owner only, quit already set
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT if it did not leave in time
  */
static int usbh_ncm_wait_link_exit(void)
{
	u32 waited = 0U;

	while (cdc_ncm_link_alive != 0U) {
		if (waited >= USB_APP_JOIN_TIMEOUT_MS) {
			return -RTK_ERR_TIMEOUT;
		}
		/* A DHCP request only ends on its own after many retries, or never with the link down:
		   stop it, again and again since the thread may start one after the previous stop */
		if (cdc_ncm_dhcp_pending != 0U) {
			lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		}
		rtos_time_delay_ms(100);
		waited += 100U;
	}

	return RTK_SUCCESS;
}

static void usbh_ncm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	u8 dhcp_ran = 0U;
	eth_state_t ethernet_state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task\n");

	usbh_cdc_ncm_open_gate(0U);

	while (cdc_ncm_app.quit == 0U) {
		if (cdc_ncm_detach_pending) {
			cdc_ncm_detach_pending = 0;
			if (ethernet_state >= ETH_STATUS_INIT) {
				ethernet_state = ETH_STATUS_IDLE;
				/* Not the tcpip thread: netifapi takes the core lock */
				netifapi_netif_set_default(pnetif_sta);
				netifapi_netif_set_link_down(pnetif_usb_eth);
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
				/* hwaddr is read by the tcpip thread, written under its core lock */
				LOCK_TCPIP_CORE();
				memcpy((void *)pnetif_usb_eth->hwaddr, (const void *)mac, 6);
				UNLOCK_TCPIP_CORE();
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netifapi_netif_set_link_up(pnetif_usb_eth);

				/* A stop meanwhile keeps stopping DHCP until this returns, see usbh_ncm_wait_link_exit() */
				dhcp_ran = 1U;
				cdc_ncm_dhcp_pending = 1U;
				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				cdc_ncm_dhcp_pending = 0U;
				if (cdc_ncm_app.quit != 0U) {
					break;
				}
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to ether netif
					/* From here AT+IPERF/AT+IPERF3 run over the dongle. iperf3 needs -l (at most 2920, 1460
					 * recommended) on both the board and the PC side, see README "Throughput test" */
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				} else {
					/* Back to IDLE so the next round retries: while the link stays up no link-down edge
					   would ever bring this branch back */
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
			rtos_time_delay_ms(1000);
		}
	}

	/* Stop requested: take the netif down while the USB stack is still up, before the stop owner tears it down */
	usbh_ncm_net_down(dhcp_ran);
	cdc_ncm_link_alive = 0U;
	usb_app_ctx_leave(&cdc_ncm_app);
	rtos_task_delete(NULL);
}

/**
  * @brief  Wait for the workers to leave, the link change thread taking the netif down first
  * @note   Teardown owner only, quit already set
  * @param  count: Number of workers to join
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT if one did not leave in time
  */
static int usbh_ncm_join_workers(u32 count)
{
	if (usbh_ncm_wait_link_exit() != RTK_SUCCESS) {
		return -RTK_ERR_TIMEOUT;
	}

	return usb_app_ctx_join(&cdc_ncm_app, count);
}

/**
  * @brief  Release what the app owns once every worker is gone, cdc_ncm_state_mutex is kept
  * @retval None
  */
static void usbh_ncm_release(void)
{
	usbh_cdc_ncm_stack_down();
	rtos_sema_delete(cdc_ncm_detach_sema);
	cdc_ncm_detach_sema = NULL;
	usb_app_ctx_end(&cdc_ncm_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC NCM demo stop\n");
}

#if CONFIG_USBH_CDC_NCM_HOTPLUG
static void usbh_ncm_hotplug_thread(void *param)
{
	u8 fatal = 0U;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_ncm_detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (cdc_ncm_app.quit != 0U) {
			/* Stop requested: the stack is still up, example_usbh_cdc_ncm_stop() tears it down */
			break;
		}

		usbh_cdc_ncm_stack_down();

		rtos_time_delay_ms(10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

		if (cdc_ncm_app.quit != 0U) {
			/* Stop requested during the teardown: leave the stack down */
			break;
		}

		/* Re-run the full bring-up, including the wait for the device to be configured,
		 * so the gate re-opens only once the class can carry traffic, as on the initial
		 * bring-up. TX stays out meanwhile: the gate is closed and teardown is set. */
		if (usbh_cdc_ncm_stack_init() != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
			fatal = 1U;
			break;
		}
		if (usbh_cdc_ncm_open_gate(1U) == 0U) {
			/* Stop requested before the device came up: the stop owner tears the stack down */
			break;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
	}

	if (fatal != 0U) {
		/* A re-init failed, so the stack is gone for good: the gate stays closed and teardown
		 * set, so TX is kept out, and no class callback can give the detach sema any more. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
		if (usb_app_ctx_claim_stop(&cdc_ncm_app) != 0U) {
			/* No stop is in flight: the link change thread, woken through quit, takes the
			   netif down before it leaves, then this thread finishes the teardown */
			if (usbh_ncm_join_workers(cdc_ncm_workers - 1U) != RTK_SUCCESS) {
				/* A worker may still use the netif: keep everything, the app stays STOPPING */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
				rtos_task_delete(NULL);
			}
			usbh_ncm_release();
			rtos_task_delete(NULL);
		}
	}

	usb_app_ctx_leave(&cdc_ncm_app);
	rtos_task_delete(NULL);
}
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int usbh_ncm_start(void)
{
	int status;
	u32 workers = 0U;
	rtos_task_t task;

	status = usb_app_ctx_begin(&cdc_ncm_app);
	if (status != RTK_SUCCESS) {
		return status;
	}

	/* A previous run may have left them set */
	cdc_ncm_detach_pending = 0;
	cdc_ncm_stack_ready = 0U;
	cdc_ncm_teardown = 0U;
	cdc_ncm_dhcp_pending = 0U;

	/* Created before the stack comes up and never deleted, see the declaration */
	if (cdc_ncm_state_mutex == NULL) {
		status = rtos_mutex_create(&cdc_ncm_state_mutex);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto exit_end;
		}
	}

	status = rtos_sema_create(&cdc_ncm_detach_sema, 0U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto exit_end;
	}

	rltk_usb_eth_init();

	status = usbh_cdc_ncm_stack_init();
	if (status != RTK_SUCCESS) {
		goto exit_release;
	}

	/* Waits for the device to be configured, opens the TX gate, then runs DHCP */
	cdc_ncm_link_alive = 1U;
	status = rtos_task_create(&task, "usbh_ncm_link_change_thread", usbh_ncm_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		cdc_ncm_link_alive = 0U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
		goto exit_join;
	}
	workers++;

#if CONFIG_USBH_CDC_NCM_HOTPLUG
	/* Created last: it owns the stack once running, so nothing after it can fail. Counted
	   before it runs, its own fatal teardown joins the others through this. */
	cdc_ncm_workers = workers + 1U;
	status = rtos_task_create(&task, "usbh_ncm_hotplug_thread", usbh_ncm_hotplug_thread, NULL, 1536U,
							  CONFIG_USBH_CDC_NCM_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
		goto exit_join;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The lwIP
	   TX thread stays unaffined: a pending teardown is excluded from it by
	   cdc_ncm_state_mutex, not by core. */
	rtos_task_set_affinity(task, 0);
#endif
#endif

#if !CONFIG_USBH_CDC_NCM_HOTPLUG
	cdc_ncm_workers = workers;
#endif
	return RTK_SUCCESS;

exit_join:
	/* Join the workers, the link change thread takes the netif down, before the stack goes away */
	cdc_ncm_app.quit = 1U;
	if (usbh_ncm_join_workers(workers) != RTK_SUCCESS) {
		/* A worker may still use the stack or the netif: keep everything, the app stays not idle */
		cdc_ncm_app.state = USB_APP_STATE_STOPPING;
		return -RTK_ERR_TIMEOUT;
	}
	usbh_cdc_ncm_stack_down();

exit_release:
#ifndef USB_APP_AGGREGATE
	/* Standalone only: in the usb_drd image lwIP may hold usb_eth_tx_mutex, which outlives the app */
	rltk_usb_eth_deinit();
#endif
	rtos_sema_delete(cdc_ncm_detach_sema);
	cdc_ncm_detach_sema = NULL;

exit_end:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH CDC NCM demo aborted\n");
	usb_app_ctx_end(&cdc_ncm_app);

	return status;
}

/* Standalone entry thread: brings the demo up and leaves */
static void usbh_ncm_init_thread(void *param)
{
	UNUSED(param);

	if (usbh_ncm_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	int ret;

	/* Gate on cdc_ncm_stack_ready BEFORE touching the mutex: the handle is still
	   NULL if the example never started, and the flag is already cleared if a
	   teardown is pending. A hotplug right after this check is harmless, the mutex
	   stays valid and the re-check under it skips the TX. A stop moves the app out
	   of RUNNING before it tears anything down. */
	if ((cdc_ncm_stack_ready == 0U) || (cdc_ncm_teardown != 0U) || (cdc_ncm_app.state != USB_APP_STATE_RUNNING)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(cdc_ncm_state_mutex, RTOS_MAX_TIMEOUT);
	if ((cdc_ncm_stack_ready == 0U) || (cdc_ncm_teardown != 0U) || (cdc_ncm_app.state != USB_APP_STATE_RUNNING)) {
		/* Lost the race against a teardown */
		ret = -RTK_ERR_IO;
	} else {
		ret = usbh_cdc_ncm_transmit(buf, len, block);
	}
	rtos_mutex_give(cdc_ncm_state_mutex);

	return ret;
}

/**
  * @brief  Start the CDC NCM host, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbh_cdc_ncm_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC NCM demo start\n");

	return usbh_ncm_start();
}

/**
  * @brief  Stop the CDC NCM host and release everything it owns
  * @note   Task context only. Safe to call when not running. The lwIP netif, usb_eth_tx_mutex
  *         and cdc_ncm_state_mutex outlive the app: lwIP may still hold or take them.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbh_cdc_ncm_stop(void)
{
	if (usb_app_ctx_claim_stop(&cdc_ncm_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&cdc_ncm_app);
	}

	/* quit is set and TX refused: the link change thread stops DHCP, restores the default route and
	   takes the netif down within its 1 s poll, before the stack goes; the others leave through quit */
#if CONFIG_USBH_CDC_NCM_HOTPLUG
	rtos_sema_give(cdc_ncm_detach_sema);
#endif
	if (usbh_ncm_join_workers(cdc_ncm_workers) != RTK_SUCCESS) {
		/* A worker is still on the stack or the netif: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone: this caller is the only one left, apart from TX under the lock */
	usbh_ncm_release();

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the CDC NCM host from its own thread
  * @retval None
  */
void example_usbh_cdc_ncm(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC NCM demo start\n");

	status = rtos_task_create(&task, "usbh_ncm_init_thread", usbh_ncm_init_thread, NULL, 1536U, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}
#else
#ifdef USB_APP_AGGREGATE
int example_usbh_cdc_ncm_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_WARN, "USBH CDC NCM demo aborted: LWIP USB ETH disabled\n");
	return -RTK_ERR_IO;
}

int example_usbh_cdc_ncm_stop(void)
{
	return RTK_SUCCESS;
}
#endif

void example_usbh_cdc_ncm(void)
{
	RTK_LOGS(TAG, RTK_LOG_WARN, "USBH CDC NCM demo aborted: LWIP USB ETH disabled\n");
}
#endif /* defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET */
