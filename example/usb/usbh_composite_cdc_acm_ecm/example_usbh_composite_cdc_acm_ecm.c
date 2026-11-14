/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "os_wrapper.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "lwip/tcpip.h"
#include "usbh_cdc_acm.h"
#include "usbh_cdc_ecm.h"
#include "atcmd_service.h"
#include "usb_app.h"
#include "example_usbh_composite_cdc_acm_ecm.h"

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_COMP_HOT_PLUG_TEST                          1     /* Hot plug test */
#define CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC             1

#define CONFIG_USBH_COMP_PBUF_MAX_LEN                           (1024)
#define CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR                   (128)

/* Thread priorities */
#define CONFIG_USBH_COMP_INIT_THREAD_PRIORITY                   5U
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY                6U
#define CONFIG_USBH_COMP_LINK_THREAD_PRIORITY                   3U

/* Thread stack sizes */
#define CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE                 (1536U)
#define CONFIG_USBH_COMP_START_THREAD_STACK_SIZE                (1536U)
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE              (1536U)
#define CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE                 (1600U)

/**
 * @brief Enable GPIO-driven power control for the attached USB device
 * When enabled, a GPIO pin (CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO) is wired to the device's power switch
 * and registered as usbh_user_cb_t::set_vbus: the host core then owns VBUS and power cycles a device that
 * no longer answers a port reset. AT+USBHACMECM=hotplug requests such a power cycle.
 * This is distinct from CONFIG_USBH_COMP_HOT_PLUG_TEST, which only re-initializes the
 * host stack in software after a detach event.
 */
#define CONFIG_USBH_COMP_GPIO_POWER_CTRL                        0

/* Private types -------------------------------------------------------------*/

/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_DEINIT,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} eth_state_t;

typedef enum {
	QUECTEL_DONGLE_STATUS_IDLE = 0U,
	QUECTEL_DONGLE_STATUS_ECM_CFG,
	QUECTEL_DONGLE_STATUS_SEARCH,
	QUECTEL_DONGLE_STATUS_SET_APN,
	QUECTEL_DONGLE_STATUS_DIAG,
	QUECTEL_DONGLE_STATUS_DIAG_STATUS,
	QUECTEL_DONGLE_STATUS_GET_MAC,
	QUECTEL_DONGLE_STATUS_MAX,
} quectel_dongle_state_t;

typedef enum {
	FIBOCOM_DONGLE_STATUS_AT_PROBE = 0U,
	FIBOCOM_DONGLE_STATUS_ATE0,
	FIBOCOM_DONGLE_STATUS_QUERY_USBMODE,
	FIBOCOM_DONGLE_STATUS_QUERY_IMEI,
	FIBOCOM_DONGLE_STATUS_QUERY_SN,
	FIBOCOM_DONGLE_STATUS_QUERY_CFUN,
	FIBOCOM_DONGLE_STATUS_QUERY_CGMR,
	FIBOCOM_DONGLE_STATUS_WAIT_SIM,
	FIBOCOM_DONGLE_STATUS_WAIT_SIGNAL,
	FIBOCOM_DONGLE_STATUS_QUERY_IMSI,
	FIBOCOM_DONGLE_STATUS_WAIT_PS_REG,
	FIBOCOM_DONGLE_STATUS_DIAL_LOOP,
	FIBOCOM_DONGLE_STATUS_DONE,
} fibocom_dongle_state_t;

/* SIMCom SIM767X dial flow states - see SIM767X_AT_CMD.txt reference log.
 * Standard 3GPP registration + SIMCom's AT+NETOPEN to enter data-pass-through mode. */
typedef enum {
	SIMCOM_DONGLE_STATUS_AT_PROBE = 0U,
	SIMCOM_DONGLE_STATUS_ATE0,
	SIMCOM_DONGLE_STATUS_WAIT_SIM,            /* AT+CPIN? until READY */
	SIMCOM_DONGLE_STATUS_WAIT_SIGNAL,         /* AT+CSQ  until rssi != 99 */
	SIMCOM_DONGLE_STATUS_QUERY_OPERATOR,      /* AT+COPS? informational */
	SIMCOM_DONGLE_STATUS_WAIT_PS_ATTACH,      /* AT+CGATT? until 1 */
	SIMCOM_DONGLE_STATUS_ACTIVATE_PDP,        /* AT+CGACT=1,1 */
	SIMCOM_DONGLE_STATUS_NETOPEN,             /* AT+NETOPEN -> +NETOPEN: 0 */
	SIMCOM_DONGLE_STATUS_QUERY_USBNETIP,      /* AT+USBNETIP? -> ensure mode=0; set if not */
	SIMCOM_DONGLE_STATUS_DONE,
} simcom_dongle_state_t;

typedef struct {
	u8 ip[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 mask[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 gw[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 dns[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
} network_info_t;

/*
 * Unified dongle context.
 * vid/pid identify the active dongle and are set once after enumeration.
 * The anonymous union holds per-vendor state; only the active vendor's view
 * is valid. A memset to zero at the start of each init cycle resets everything.
 */
typedef struct {
	union {
		struct {
			network_info_t network;
			u8 mac[6];
			u8 ip_ready;
			u8 cereg_ready;
			u8 mac_ready;
		} quectel;

		struct {
			volatile u8 at_ok;
			volatile u8 usbmode_seen;
			volatile u8 usbmode_is_ecm;
			volatile u8 cgsn_ok;
			volatile u8 cfsn_ok;
			volatile u8 cfun_ok;
			volatile u8 cgmr_ok;
			volatile u8 cpin_ready;
			volatile u8 csq_seen;
			volatile u8 csq_has_signal;
			volatile u8 cgreg_seen;
			volatile u8 ps_registered;
			volatile u8 gtrndis_seen;
			volatile u8 gtrndis_has_ip;
		} fibocom;

		/* SIMCom SIM767X flags: mirror the AT reply markers that
		 * usbh_composite_simcom_ctrl() polls between commands. */
		struct {
			volatile u8 at_ok;             /* Any bare "OK" line. */
			volatile u8 cpin_ready;        /* +CPIN: READY */
			volatile u8 csq_seen;          /* +CSQ line parsed */
			volatile u8 csq_has_signal;    /* +CSQ rssi != 99 */
			volatile u8 cops_ok;           /* +COPS line seen (informational) */
			volatile u8 cgatt_seen;
			volatile u8 cgatt_attached;    /* +CGATT: 1 */
			volatile u8 netopen_ok;           /* AT+NETOPEN execute success: +NETOPEN: 0 (err=0) */
			volatile u8 netopen_query_seen;   /* AT+NETOPEN? response received */
			volatile u8 netopen_query_state;  /* AT+NETOPEN? result: 0=closed 1=open */
			volatile u8 usbnetip_seen;     /* +USBNETIP: line parsed */
			volatile u8 usbnetip_mode;     /* 0=Private(DHCP) 1=Public(static) */
		} simcom;
	};
	u16 vid;
	u16 pid;
	volatile u8 in_detach;
} dongle_ctx_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_composite_cmd(u16 argc, char *argv[]);
static void usbh_composite_cmd_at(u16 argc, char **argv);
static int usbh_composite_hotplug_test(void);

static int usbh_composite_cb_init(void);
static void usbh_composite_cb_deinit(void);
static void usbh_composite_cb_attached(void);
static void usbh_composite_detached(void);
static void usbh_composite_ecm_received(u8 *buf, u32 len);
static void usbh_composite_acm_received(u8 *buf, u32 len, int status);
static void usbh_composite_acm_transmitted(int status);
static void usbh_composite_worker_exit(u8 fatal);
static void usbh_composite_cb_process(usb_host_t *host, u8 msg);
static int usbh_composite_cb_device_check(usb_host_t *host, u8 cfg_max);
#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
static int usbh_composite_set_vbus(usb_host_t *host, u8 on);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

#if CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC
static const u16 usbh_composite_ecm_led_color[1] = {0x1122};
static const u8 usbh_composite_ecm_mac_str[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
/* Given by cb_detached and by the stop owner, taken by the hot-plug thread. Guarded because
   every use of it already sits inside a CONFIG_USBH_COMP_HOT_PLUG_TEST block, so with
   hot-plug disabled the object is unreferenced and the build rejects it
   (-Werror=unused-variable). */
static rtos_sema_t usbh_composite_detach_sema;
/* 1 from USBH_MSG_RESET / USBH_MSG_POWER_CYCLE / USBH_MSG_ERROR until the hot-plug thread takes the
   detach it causes: the core brings the device back itself or waits for a replug, so the stack is
   kept, not rebuilt. A rebuild would cut VBUS again and renew the core reset budgets. Cleared by
   USBH_MSG_DISCONNECTED, a real unplug. Written by the host task, read by the hot-plug thread. */
static volatile u8 usbh_comp_keep_stack = 0;
#endif

static u8 usbh_composite_acm_tx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] USB_DMA_ALIGNED;
static u8 usbh_composite_acm_rx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] USB_DMA_ALIGNED;

static rtos_sema_t usbh_composite_acm_rx_done_sema;
static rtos_sema_t usbh_composite_acm_send_sema;

static usbh_cdc_acm_param_t usbh_composite_dongle_array[] = {
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID, USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID, 2},
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID, USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID,  2},
	{USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID, USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID, 2},
	{USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID, USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID, 2},
	{0, 0, 0},
};

static dongle_ctx_t usbh_dongle_ctx;

static const usbh_cdc_ecm_config_t usbh_composite_ecm_class_cfg = {
#if CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC
	usbh_composite_ecm_led_color,
	usbh_composite_ecm_mac_str,
	sizeof(usbh_composite_ecm_led_color) / sizeof(usbh_composite_ecm_led_color[0]),
#else
	NULL,
	NULL,
	0,
#endif
	0,
};

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1792U,
	.main_task_priority = CONFIG_USBH_COMP_INIT_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.class_num = 2U,   /* CDC-ACM + CDC-ECM */
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

static const usbh_cdc_acm_cb_t usbh_composite_acm_cfg = {
	.received = usbh_composite_acm_received,
	.transmitted = usbh_composite_acm_transmitted,
};

static const usbh_cdc_acm_config_t usbh_composite_acm_class_cfg = {
	.dongle_param = usbh_composite_dongle_array,
	/* Line coding and control line state are left 0 to take the class defaults;
	 * notify_en stays 0 because this path only uses the AT and BULK pipes. */
};

static const usbh_cdc_ecm_state_cb_t usbh_composite_ecm_cfg = {
	.init = usbh_composite_cb_init,
	.deinit = usbh_composite_cb_deinit,
	.attached = usbh_composite_cb_attached,
	.received = usbh_composite_ecm_received,
	.detached = usbh_composite_detached,
};

static const usbh_user_cb_t usbh_composite_usr_cb = {
	.process = usbh_composite_cb_process,
	.validate = usbh_composite_cb_device_check,
#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
	.set_vbus = usbh_composite_set_vbus,
#endif
};

/* Serializes the USB stack bring up / tear down against the class API callers.
   usbh_cdc_acm_deinit() frees the line-coding buffers and drops its host handle,
   usbh_cdc_ecm_deinit() frees the TX/RX buffers the lwIP path walks, and
   usbh_deinit() memset()s the whole host handle. Each class API only checks its
   own state on entry, so under SMP a caller on core 1 can be past that check while
   core 0 frees what it dereferences. The callers must be excluded by lock, not by
   core.
   in_detach already parks the AT state machine and the ACM RX thread, but it is a
   flag, not a barrier: a thread that passed the check is still inside the class
   when the teardown runs. The lock closes that window.
   Split two ways - ACM for the AT command path and the RX thread, ECM for the lwIP
   TX path - so a long AT exchange does not stall network transmit. The teardown
   takes both, always in the order acm, ecm.
   Created before the stack comes up and never deleted: the lwIP thread and the ACM
   RX thread can not be joined by the example, so there is no point in time at which
   these mutexes are provably unreferenced. usbh_comp_stack_ready is the gate that
   keeps callers off the stack instead. */
static rtos_mutex_t usbh_comp_acm_mutex = NULL;

static rtos_mutex_t usbh_comp_ecm_mutex = NULL;

/* Serializes the AT command senders (console send and the dial state machines) on the single
   usbh_composite_acm_tx_buf. Taken before usbh_comp_acm_mutex, never by the teardown. Same
   lifecycle as the class mutexes: created by the first start and never deleted. */
static rtos_mutex_t usbh_comp_at_tx_mutex = NULL;

/* 1: the whole stack is up and the class APIs may be called. Cleared before any
   tear down, so a non-zero value also implies both mutexes are valid. */
static volatile u8 usbh_comp_stack_ready = 0;

/* 1: usbh_init() and both class inits are done, the stack needs a deinit. Changed only by the
   thread owning the stack (start, then the hotplug thread) and the teardown owner. */
static u8 usbh_comp_stack_up = 0;

/* 1: the link thread is inside its IP section (DHCP or the static setup of the EG915) and may
   still bring the netif up or switch the default route. Set only if quit was clear, under the
   same lock usb_app_ctx_claim_stop() sets quit with, so the teardown owner either sees it or
   the link thread sees quit. */
static volatile u8 usbh_comp_net_busy = 0;
/* 1: pnetif_usb_eth got an address since start, the teardown has to clear it */
static volatile u8 usbh_comp_net_ran = 0;
/* 1: an AT+USBHACMECM=send command is inside the class API, entered like usbh_comp_net_busy */
static volatile u8 usbh_comp_at_busy = 0;

/* Number of worker threads created by start, all joined by the teardown */
static u8 usbh_comp_workers = 0;

/* Start/stop state: example_usbh_composite_cdc_acm_ecm_stop(), or the hotplug thread when the
   stack can not be rebuilt, sets quit, takes the network down, wakes and joins the workers */
static usb_app_ctx_t usbh_comp_app;

/* Exported variables --------------------------------------------------------*/

/* AT commands "AT+<cmd>=<p1>,<p2>...": the command name is case-sensitive, the parameters are not */
ATCMD_TABLE_DATA_SECTION
const log_item_t usbh_composite_dongle_atcmd[] = {
	{"+USBHACMECM", usbh_composite_cmd_at},
};

/* Private functions ---------------------------------------------------------*/

static u8 usbh_composite_hex_to_char(u8 hex_num)
{
	if (hex_num >= '0' && hex_num <= '9') {
		return hex_num - '0';
	}
	if (hex_num >= 'a' && hex_num <= 'f') {
		return hex_num - 'a' + 10;
	}
	if (hex_num >= 'A' && hex_num <= 'F') {
		return hex_num - 'A' + 10;
	}
	return 0;
}

static u32 usbh_composite_strlen(u8 *pbuf)
{
	return strlen((char *)pbuf);
}

/**
  * @brief  Parse the decimal number at the head of a device response field
  * @note   Only the leading digits are taken: an AT field goes on with ',', '\r' and so on.
  * @param  str: Field start
  * @param  val: Parsed value, written on success only
  * @retval RTK_SUCCESS, or -RTK_ERR_BADARG when the field does not start with a digit
  */
static int usbh_composite_parse_uint(const char *str, u32 *val)
{
	char *end = NULL;
	u32 num;

	if ((str == NULL) || (str[0] < '0') || (str[0] > '9')) {
		return -RTK_ERR_BADARG;
	}
	num = (u32)_strtoul(str, &end, 10);
	if (end == str) {
		return -RTK_ERR_BADARG;
	}
	*val = num;
	return RTK_SUCCESS;
}

/**
  * @brief  Store one network info field parsed from an AT response
  * @param  pbuf: Token from the response, quotes stripped here
  * @param  pdst: One of the usbh_dongle_ctx.quectel.network arrays
  * @param  name: Field name, for the log only
  * @retval None
  */
static void usbh_composite_dongle_set_netinfo(u8 *pbuf, u8 *pdst, const char *name)
{
	u32 len;

	/* A fragmented/short AT response can yield a NULL token from strsep(); reject it
	   here instead of dereferencing pbuf below. */
	if (pbuf == NULL) {
		return;
	}

	if (pbuf[0] == '"') {
		pbuf ++;
	}
	len = usbh_composite_strlen(pbuf);
	if ((len > 0U) && (pbuf[len - 1U] == '"')) {
		len--;
		pbuf[len] = 0;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type(%s)=[%s(%u)]\n", name, pbuf, len);

	/* The token points into the device-supplied AT response, so len is bounded only by
	   CONFIG_USBH_COMP_PBUF_MAX_LEN-1 while the destination is a fixed
	   CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR array. A dotted-quad address never exceeds
	   15 chars, so an over-long token is a malformed response: drop it rather than store
	   a truncated value that inet_aton() would silently misread. */
	if (len >= CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Type(%s) len %u too long\n", name, len);
		pdst[0] = 0;
		return;
	}

	memcpy((void *)pdst, (const void *)pbuf, len);
	pdst[len] = 0;  /* keep the field a valid C string for inet_aton() */
}

/**
  * @brief  Class-independent user process callback.
  * @details For a multi-config 4G dongle the host FSM raises
  *          USBH_MSG_USER_SET_CONFIG so the application picks which
  *          configuration to activate. The ECM data/control interfaces and the
  *          ACM AT interface live in the same configuration, so selecting the
  *          config that carries ECM also brings up ACM. Reuse the ECM helper
  *          usbh_cdc_ecm_choose_config() to locate and set that config.
  * @param  host: USB host handle
  * @param  msg: @ref usbh_msg_type_t
  * @retval Status
  */
static void usbh_composite_cb_process(usb_host_t *host, u8 msg)
{
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		if (usbh_cdc_ecm_choose_config(host) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Choose config fail\n");
		}
		break;
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	case USBH_MSG_RESET:
	case USBH_MSG_POWER_CYCLE:
	case USBH_MSG_ERROR:
		/* Reported before the class detach it causes, see usbh_comp_keep_stack */
		usbh_comp_keep_stack = 1U;
		break;
	case USBH_MSG_DISCONNECTED:
		usbh_comp_keep_stack = 0U;
		break;
#endif
	case USBH_MSG_CONNECTED:
	default:
		break;
	}
}

/**
  * @brief  Device validate callback (hub multi-device scenario).
  * @details Accept the device only if its descriptors expose the ECM
  *          control + data interfaces this composite example drives.
  * @param  host: USB host handle
  * @param  cfg_max: bNumConfigurations
  * @retval RTK_SUCCESS to accept the device, non-zero to discard it.
  */
static int usbh_composite_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

static int usbh_composite_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return RTK_SUCCESS;
}

static void usbh_composite_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

static void usbh_composite_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
}

/**
  * @brief  Composite detach callback.
  *
  * Registered in the ECM class callback table only; the ACM table has no detached
  * callback. The in_detach latch is kept anyway: a class whose setup was skipped
  * still detaches, and a second signal would make the hot-plug thread run its
  * whole deinit sequence twice (double free). Latch on the first fire and drop
  * subsequent ones until usbh_composite_bring_up() clears the flag after
  * re-enumeration.
  */
static void usbh_composite_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	/* Idempotent: only signal the hot-plug thread once per detach cycle. */
	if (usbh_dongle_ctx.in_detach == 0U) {
		usbh_dongle_ctx.in_detach = 1U;
		rtos_sema_give(usbh_composite_detach_sema);
	}
#endif
}

/**
  * @brief  ACM bulk OUT (transmit) completion callback.
  * @details Gives usbh_composite_acm_send_sema so usbh_composite_acm_transmit() can wait for the
  *          bulk OUT transfer to finish before the caller reuses usbh_composite_acm_tx_buf.
  */
static void usbh_composite_acm_transmitted(int status)
{
	if (status == RTK_SUCCESS) {
		rtos_sema_give(usbh_composite_acm_send_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ACM TX fail: %d\n", status);
	}
}

/**
  * @brief  Take one of the per-class stack locks if the class API may be used
  * @note   Gates on usbh_comp_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already cleared
  *         if a teardown is pending. A detach right after this check is harmless,
  *         the mutex stays valid and the re-check under it catches the loss.
  * @param  lock: usbh_comp_acm_mutex or usbh_comp_ecm_mutex
  * @retval RTK_SUCCESS when the lock is held and the stack is usable, -RTK_ERR_IO when
  *         the caller must give up without unlocking
  */
static int usbh_comp_lock(rtos_mutex_t lock)
{
	if ((usbh_comp_stack_ready == 0U) || (usbh_dongle_ctx.in_detach != 0U) || (lock == NULL)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(lock, RTOS_MAX_TIMEOUT);

	if ((usbh_comp_stack_ready == 0U) || (usbh_dongle_ctx.in_detach != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(lock);
		return -RTK_ERR_IO;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Release a lock taken by usbh_comp_lock()
  * @param  lock: The same handle that was passed to usbh_comp_lock()
  * @retval None
  */
static void usbh_comp_unlock(rtos_mutex_t lock)
{
	rtos_mutex_give(lock);
}

/**
  * @brief  Send an AT command over ACM bulk OUT and wait for TX completion.
  * @param  buf: data buffer (typically usbh_composite_acm_tx_buf)
  * @param  len: number of bytes to send
  * @retval RTK_SUCCESS on success (TX issued and completed), error otherwise.
  * @note   Waiting for completion (via usbh_composite_acm_send_sema, given by
  *         usbh_composite_acm_transmitted) prevents the caller from overwriting buf
  *         while the previous transfer is still in flight.
  */
static int usbh_composite_acm_transmit(u8 *buf, u32 len)
{
	int ret;

	/* Arm under the lock, then release it before waiting for the completion: the
	   callback never comes for a device that was just unplugged, and the teardown
	   must not be blocked for the whole wait. */
	if (usbh_comp_lock(usbh_comp_acm_mutex) != RTK_SUCCESS) {
		return -RTK_ERR_IO;
	}
	/* A transfer that timed out may still complete and leave its token: drain it,
	   or the wait below would take it for this transfer's completion */
	while (rtos_sema_take(usbh_composite_acm_send_sema, 0) == RTK_SUCCESS) {
	}
	ret = usbh_cdc_acm_transmit(buf, len);
	usbh_comp_unlock(usbh_comp_acm_mutex);

	if (ret == RTK_SUCCESS) {
		/* Bounded wait so a lost completion / detach cannot hang forever. Not done
		   in time means not sent: the caller stays on its step and sends again. */
		if (rtos_sema_take(usbh_composite_acm_send_sema, 1000) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "ACM TX TO\n");
			ret = -RTK_ERR_TIMEOUT;
		}
	}
	return ret;
}

/**
  * @brief  Send one AT command, CR LF appended, through usbh_composite_acm_tx_buf
  * @note   Serialized on usbh_comp_at_tx_mutex: the console and the dial state machines run in
  *         different threads and share the one TX buffer.
  * @param  cmd: Command without CR LF
  * @param  cmd_len: Command length in bytes
  * @retval RTK_SUCCESS once the transfer completed, error otherwise
  */
static int usbh_composite_acm_send_cmd(const u8 *cmd, u32 cmd_len)
{
	int ret;

	if (cmd_len + 2U > CONFIG_USBH_COMP_PBUF_MAX_LEN) {
		return -RTK_ERR_BADARG;
	}
	if (rtos_mutex_take(usbh_comp_at_tx_mutex, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
		return -RTK_ERR_IO;
	}

	memset((void *)usbh_composite_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
	memcpy((void *)usbh_composite_acm_tx_buf, (const void *)cmd, cmd_len);
	usbh_composite_acm_tx_buf[cmd_len + 0U] = 0x0D;
	usbh_composite_acm_tx_buf[cmd_len + 1U] = 0x0A;
	ret = usbh_composite_acm_transmit(usbh_composite_acm_tx_buf, cmd_len + 2U);
	if (ret == -RTK_ERR_TIMEOUT) {
		/* The bulk OUT may still read the buffer: the next sender must not rewrite it before the
		   completion. A detach closes the pipe, which halts the channel and so stops the DMA.
		   ret stays a timeout, the caller still sees the command as not sent. */
		while ((rtos_sema_take(usbh_composite_acm_send_sema, 100) != RTK_SUCCESS) && (usbh_dongle_ctx.in_detach == 0U) &&
			   (usbh_comp_stack_ready != 0U) && (usbh_comp_app.quit == 0U)) {
		}
	}

	rtos_mutex_give(usbh_comp_at_tx_mutex);
	return ret;
}

/**
  * @brief  Composite ACM bulk receive callback
  * @param  pbuf: RX buffer
  * @param  len: RX data length (in bytes)
  */
static void usbh_composite_acm_received(u8 *pbuf, u32 len, int status)  /* type is usb transfer type */
{
	UNUSED(status);
	u32 i;
	u32 rx_len = len;
	u16 vid = usbh_dongle_ctx.vid;

	/* Runs in the host task on every packet: the dumps stay at DEBUG */
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "ACM data received len(%u)\n", len);

	if (pbuf == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ACM data received NULL or len=0\n");
		return;
	}

	/* The bulk IN transfer delivers raw stream bytes with no NUL terminator, but the
	   AT-response parsing below scans pbuf as a C string (strstr/strsep/strlen/atoi).
	   pbuf is usbh_composite_acm_rx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN], the only buffer armed
	   with usbh_cdc_acm_receive(), so len <= CONFIG_USBH_COMP_PBUF_MAX_LEN. Cap len at the last
	   index so the NUL always lands inside the buffer; a full-size transfer gives up its last byte.
	   Done on a local copy, len itself is left as delivered. */
	if (rx_len > (CONFIG_USBH_COMP_PBUF_MAX_LEN - 1U)) {
		rx_len = CONFIG_USBH_COMP_PBUF_MAX_LEN - 1U;
	}
	pbuf[rx_len] = '\0';

	/* pbuf is NUL terminated above: print the AT response in one call */
	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "%s\n", (char *)pbuf);

	if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) {
		/* EG91 */
		if (strstr((char *)pbuf, "+CEREG")) {  /* /parse to get the network information */
			char *buff = (char *)pbuf;
			char *p;
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			if (p != NULL && ((*p == '1' || *p == '5'))) {
				usbh_dongle_ctx.quectel.cereg_ready = 1;
			}
		}
		if (strstr((char *)pbuf, "+WWANINFO")) {
			/*		 Apn:ipv4:ipaddr:GW:Mask:DNS:back DNS
				+WWANINFO:0,"IP","10.16.120.241","10.16.120.242","255.255.255.252","218.2.2.2","218.4.4.4"
			*/
			char *buff = (char *)pbuf;
			char *p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			/* parse to get the ip */
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.ip, "ip");
			p = strsep(&buff, ",");
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.gw, "gw");
			p = strsep(&buff, ",");
			/* parse to get the gw */
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.mask, "mask");
			p = strsep(&buff, ",");
			/* parse to get the dns */
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.dns, "dns");
			p = strsep(&buff, ",");

			usbh_dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
		if (strstr((char *)pbuf, "+QNETIFMAC")) {  /* /parse to get the mac info */
			/*
				+QNETIFMAC:"ea:24:64:58:ca:83"
			*/
			char *buff = (char *)pbuf;
			char *p = strsep(&buff, "\"");
			p = strsep(&buff, "\"");
			/* The token comes from the device: a short one ("xx:xx:xx:xx:xx:xx" is 17 chars)
			   would make the loop below read past its NUL */
			if ((p != NULL) && (usbh_composite_strlen((u8 *)p) < 17U)) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Bad MAC rsp\n");
				p = NULL;
			}
			if (p != NULL) {
				for (i = 0; i < 6; i++) {
					usbh_dongle_ctx.quectel.mac[i] = usbh_composite_hex_to_char(p[3 * i + 0]) * 16 + usbh_composite_hex_to_char(p[3 * i + 1]) ;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", usbh_dongle_ctx.quectel.mac[0], usbh_dongle_ctx.quectel.mac[1],
						 usbh_dongle_ctx.quectel.mac[2],
						 usbh_dongle_ctx.quectel.mac[3], usbh_dongle_ctx.quectel.mac[4], usbh_dongle_ctx.quectel.mac[5]);
				usbh_dongle_ctx.quectel.mac_ready = 1;
			}
		}

		/* EG915 */
		if (strstr((char *)pbuf, "+CGCONTRDP")) {  /* /parse to get the network information */
			char *buff = (char *)pbuf;
			u8 *tail ;
			char *p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			/* parse to get the ip/mask */
			if (p != NULL) {
				u8 dotcount = 0;
				u8 ptmp[100];
				u32 plen = strlen(p);
				if (plen >= sizeof(ptmp)) {
					plen = sizeof(ptmp) - 1;
				}
				memset((void *)ptmp, 0, sizeof(ptmp));
				memcpy((void *)ptmp, (const void *)p, plen);
				tail = ptmp ;
				for (;;) {
					if (*tail == '.') {
						dotcount ++;
						if (dotcount == 4) {
							*tail = 0;
							tail++;
							break;
						}
					} else if (*tail == 0) {
						/* reached end of the bounded, NUL-terminated buffer */
						break;
					}
					tail++;
				}
				usbh_composite_dongle_set_netinfo(ptmp, usbh_dongle_ctx.quectel.network.ip, "ip");
				usbh_composite_dongle_set_netinfo(tail, usbh_dongle_ctx.quectel.network.mask, "mask");
			}
			p = strsep(&buff, ",");
			/* parse to get the gw */
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.gw, "gw");
			p = strsep(&buff, ",");
			/* parse to get the dns */
			usbh_composite_dongle_set_netinfo((u8 *)p, usbh_dongle_ctx.quectel.network.dns, "dns");
			p = strsep(&buff, ",");

			usbh_dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
	} else if (USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) {
		/* LE271 (Fibocom) - parse the responses listed in the LE271 ECM dial flow.
		   Responses arrive line-by-line; we set per-marker flags consumed by usbh_composite_fibocom_ctrl(). */
		if (strstr((char *)pbuf, "+GTUSBMODE:")) {
			char *p = strstr((char *)pbuf, "+GTUSBMODE:") + sizeof("+GTUSBMODE:") - 1;
			while (*p == ' ') {
				p++;
			}
			u32 mode;

			/* A malformed field leaves the mode unseen, so the query is sent again */
			if (usbh_composite_parse_uint(p, &mode) == RTK_SUCCESS) {
				usbh_dongle_ctx.fibocom.usbmode_is_ecm = (mode == 32U) ? 1 : 0;
				usbh_dongle_ctx.fibocom.usbmode_seen = 1;
			}
		}
		if (strstr((char *)pbuf, "+CGSN:")) {
			usbh_dongle_ctx.fibocom.cgsn_ok = 1;
		}
		if (strstr((char *)pbuf, "+CFSN:")) {
			usbh_dongle_ctx.fibocom.cfsn_ok = 1;
		}
		/* AT+CFUN? returns "+CFUN: 1" / "+CFUN: 4" etc.; PDF expects functional state 1. */
		if (strstr((char *)pbuf, "+CFUN:")) {
			char *p = strstr((char *)pbuf, "+CFUN:") + sizeof("+CFUN:") - 1;
			while (*p == ' ') {
				p++;
			}
			if (*p == '1') {
				usbh_dongle_ctx.fibocom.cfun_ok = 1;
			}
		}
		if (strstr((char *)pbuf, "+CGMR:")) {
			usbh_dongle_ctx.fibocom.cgmr_ok = 1;
		}
		if (strstr((char *)pbuf, "+CPIN: READY")) {
			usbh_dongle_ctx.fibocom.cpin_ready = 1;
		}
		if (strstr((char *)pbuf, "+CSQ:")) {
			char *p = strstr((char *)pbuf, "+CSQ:") + sizeof("+CSQ:") - 1;
			while (*p == ' ') {
				p++;
			}
			u32 rssi;

			if (usbh_composite_parse_uint(p, &rssi) == RTK_SUCCESS) {
				usbh_dongle_ctx.fibocom.csq_has_signal = (rssi == 99U) ? 0 : 1;
				usbh_dongle_ctx.fibocom.csq_seen = 1;
			}
		}
		if (strstr((char *)pbuf, "+CGREG:")) {
			/* +CGREG: <n>,<stat>[,...]  - stat==1 (home) or stat==5 (roaming) means PS attached. */
			char *buff = (char *)strstr((char *)pbuf, "+CGREG:");
			char *p = strsep(&buff, ":");
			p = strsep(&buff, ",");           /* <n> */
			p = strsep(&buff, ",\r\n");       /* <stat> */
			if (p != NULL) {
				while (*p == ' ') {
					p++;
				}
				if (*p == '1' || *p == '5') {
					usbh_dongle_ctx.fibocom.ps_registered = 1;
				}
			}
			usbh_dongle_ctx.fibocom.cgreg_seen = 1;
		}
		if (strstr((char *)pbuf, "+GTRNDIS:")) {
			char *p = strstr((char *)pbuf, "+GTRNDIS:") + sizeof("+GTRNDIS:") - 1;
			while (*p == ' ') {
				p++;
			}
			/* Empty-state form "+GTRNDIS: 0" => no IPv4 yet; otherwise the response carries an IP. */
			if (*p == '0' && (*(p + 1) == '\r' || *(p + 1) == '\n' || *(p + 1) == 0)) {
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 0;
			} else {
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 1;
			}
			usbh_dongle_ctx.fibocom.gtrndis_seen = 1;
		}
		/* Generic OK - covers Step1/Step2/Step10/Step13 acks. Place last so it does not pre-empt
		   marker matches above for "<marker>...OK" multi-line replies. */
		if (strstr((char *)pbuf, "OK")) {
			usbh_dongle_ctx.fibocom.at_ok = 1;
		}
	} else if (USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID == vid) {
		/* SIMCom SIM767X (Qualcomm-based) - parse the markers of the SIM767X_AT_CMD.txt flow.
		 * Lines arrive one at a time; each marker sets a flag consumed by usbh_composite_simcom_ctrl(). */
		if (strstr((char *)pbuf, "+CPIN: READY")) {
			usbh_dongle_ctx.simcom.cpin_ready = 1;
		}
		if (strstr((char *)pbuf, "+CSQ:")) {
			/* +CSQ: <rssi>,<ber>  - rssi==99 means "not known / not detectable". */
			char *p = strstr((char *)pbuf, "+CSQ:") + sizeof("+CSQ:") - 1;
			while (*p == ' ') {
				p++;
			}
			u32 rssi;

			if (usbh_composite_parse_uint(p, &rssi) == RTK_SUCCESS) {
				usbh_dongle_ctx.simcom.csq_has_signal = (rssi == 99U) ? 0 : 1;
				usbh_dongle_ctx.simcom.csq_seen = 1;
			}
		}
		if (strstr((char *)pbuf, "+COPS:")) {
			usbh_dongle_ctx.simcom.cops_ok = 1;
		}
		if (strstr((char *)pbuf, "+CGATT:")) {
			/* +CGATT: <state>  - state==1 means attached to the PS domain. */
			char *p = strstr((char *)pbuf, "+CGATT:") + sizeof("+CGATT:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.cgatt_attached = (*p == '1') ? 1 : 0;
			usbh_dongle_ctx.simcom.cgatt_seen = 1;
		}
		if (strstr((char *)pbuf, "+NETOPEN:")) {
			/* +NETOPEN: <value> is shared by two commands with OPPOSITE semantics
			 * (per SIM767XX AT manual 13.2.1):
			 *   AT+NETOPEN?  (Read)    -> <net_state>: 0=closed, 1=open
			 *   AT+NETOPEN   (Execute) -> <err>:       0=success, non-zero=fail
			 * Use separate flags so ctrl() can distinguish which step produced
			 * the response. */
			char *p = strstr((char *)pbuf, "+NETOPEN:") + sizeof("+NETOPEN:") - 1;
			while (*p == ' ') {
				p++;
			}
			/* Query response (AT+NETOPEN?): record state unconditionally. */
			usbh_dongle_ctx.simcom.netopen_query_state = (*p == '1') ? 1U : 0U;
			usbh_dongle_ctx.simcom.netopen_query_seen  = 1;
			/* Execute response (AT+NETOPEN): err==0 means just-opened OK. */
			if (*p == '0') {
				usbh_dongle_ctx.simcom.netopen_ok = 1;
			}
		}
		if (strstr((char *)pbuf, "+IP ERROR: Network is already opened")) {
			/* Per SIM767XX AT manual 13.2.1: when the session is already active,
			 * AT+NETOPEN responds with this error. Treat as success for
			 * netopen_ok (execute path). */
			usbh_dongle_ctx.simcom.netopen_ok = 1;
		}
		if (strstr((char *)pbuf, "+USBNETIP:")) {
			/* +USBNETIP: 0,<third>,<fourth>  -> mode=0 (Private IP / DHCP)
			 * +USBNETIP: 1                   -> mode=1 (Public IP / static)
			 * Per SIM767XX AT manual 28.2.3. */
			char *p = strstr((char *)pbuf, "+USBNETIP:") + sizeof("+USBNETIP:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.usbnetip_mode = (*p == '1') ? 1 : 0;
			usbh_dongle_ctx.simcom.usbnetip_seen = 1;
		}
		/* Generic OK - covers bare AT probes and terminates every AT response. Place last so
		 * it does not pre-empt "<marker>...OK" multi-line replies parsed above. */
		if (strstr((char *)pbuf, "OK")) {
			usbh_dongle_ctx.simcom.at_ok = 1;
		}
	}

	/* Wake the ACM RX task so it re-arms the next receive. */
	rtos_sema_give(usbh_composite_acm_rx_done_sema);
}

static void usbh_composite_cmd_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBHACMECM=<cmd>[,<args>], 4G dongle (CDC ACM + ECM) host\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  send,<at>   Send one AT command, CR LF appended, to the dongle on its ACM port\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "              <at> is taken as typed, commas included, e.g. AT+USBHACMECM=send,AT+CGDCONT=1,\"IP\",\"cmnet\"\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "              At most %d chars, and the whole line must fit the console buffer\n", CONFIG_USBH_COMP_PBUF_MAX_LEN - 2);
	RTK_LOGS(TAG, RTK_LOG_INFO, "              Quectel / Fibocom LE271 / SIMCOM SIM767X dongles only; retried up to 10 times\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "              at 1 s interval, so the shell can be blocked for about 10 s\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  hotplug     Power-cycle the dongle via VBUS, needs GPIO power control\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  help        Print this help\n");
}

/**
  * @brief  Shell command of the example: send an AT command to the dongle, or power-cycle it
  * @param  argc: Argument count
  * @param  argv: Arguments, argv[0] is the sub command
  * @retval RTK_SUCCESS, or the error of the sub command
  */
static int usbh_composite_cmd(u16 argc, char *argv[])
{
	u8 *cmd;
	u32 cmd_len;
	u16 i;
	u8 try_cnt = 10;
	u8 enter = 0U;
	int ret = -RTK_ERR_IO;

	if ((argc == 0U) || (_stricmp(argv[0], "help") == 0) || (_stricmp(argv[0], "?") == 0)) {
		usbh_composite_cmd_usage();
		return (argc == 0U) ? -RTK_ERR_BADARG : RTK_SUCCESS;
	}

	/* Refused while the app is not running, a hotplug reinit is fine. send checks again with
	   usbh_comp_at_busy, under the lock a stop is claimed in. */
	if (usbh_comp_app.state != USB_APP_STATE_RUNNING) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}

	if (_stricmp(argv[0], "hotplug") == 0) {
		if (argc != 1U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBHACMECM=hotplug\n");
			return -RTK_ERR_BADARG;
		}
		return usbh_composite_hotplug_test();
	}

	if (_stricmp(argv[0], "send") != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd: %s, try AT+USBHACMECM=help\n", argv[0]);
		return -RTK_ERR_BADARG;
	}
	if ((argc < 2U) || (argv[1][0] == '\0')) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: AT+USBHACMECM=send,<at>\n");
		return -RTK_ERR_BADARG;
	}

	/* The AT parser split <at> at its commas, in place: each comma became the NUL ending one
	   argv, the next argv starts right after it. Put the commas back to get <at> as typed. */
	cmd = (u8 *)argv[1];
	for (i = 2U; i < argc; i++) {
		*(argv[i] - 1) = ',';
	}
	cmd_len = usbh_composite_strlen(cmd);
	/* The shell line can exceed the TX buffer with CONFIG_LONGER_CMD; keep room for CR LF */
	if (cmd_len + 2U > CONFIG_USBH_COMP_PBUF_MAX_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cmd too long %u\n", cmd_len);
		return -RTK_ERR_BADARG;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "At cmd(%s)\n", cmd);
	if ((USBH_CDC_ACM_QUECTEL_DONGLE_VID != usbh_dongle_ctx.vid) &&
		(USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID != usbh_dongle_ctx.vid) &&
		(USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID != usbh_dongle_ctx.vid)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No supported dongle, VID %04x\n", usbh_dongle_ctx.vid);
		return -RTK_ERR_IO;
	}

	/* Only while the app runs: a teardown waits for usbh_comp_at_busy to clear before it frees
	   the send sema and the stack, see usbh_composite_teardown_all() */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (usbh_comp_app.state == USB_APP_STATE_RUNNING) {
		usbh_comp_at_busy = 1U;
		enter = 1U;
	}
	rtos_critical_exit(RTOS_CRITICAL_USB);
	if (enter == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not running\n");
		return -RTK_ERR_IO;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Pre AtCmd\n");
	while ((try_cnt >  0) && (usbh_comp_app.quit == 0U)) {
		ret = usbh_composite_acm_send_cmd(cmd, cmd_len);
		if (RTK_SUCCESS == ret) {
			break;
		}
		rtos_time_delay_ms(1000);

		try_cnt --;
	}
	usbh_comp_at_busy = 0U;
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "AT cmd(%s) TX fail\n", cmd);
		return -RTK_ERR_IO;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "AT cmd(%s) done\n", cmd);
	return RTK_SUCCESS;
}

/* AT entry: argv[0] is reserved by the AT parser and argv[1] holds the first parameter */
static void usbh_composite_cmd_at(u16 argc, char **argv)
{
	int ret = usbh_composite_cmd((argc != 0U) ? (u16)(argc - 1U) : 0U, &argv[1]);

	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, (int)ret);
	}
}

/**
  * @brief  Wait for a reply flag of the dial state machines, polled every 100 ms
  * @param  flag: Flag set by usbh_composite_acm_received(), or NULL for a plain delay
  * @param  ms: Wait time in ms
  * @retval 0 once *flag is set (NULL: after the full delay), -1 on detach or timeout
  */
static int usbh_composite_wait_flag(volatile u8 *flag, u32 ms)
{
	u32 elapsed;

	for (elapsed = 0U; elapsed < ms; elapsed += 100U) {
		if (usbh_dongle_ctx.in_detach != 0) {
			return -1;
		}
		if ((flag != NULL) && (*flag != 0U)) {
			return 0;
		}
		rtos_time_delay_ms(100);
	}
	if (usbh_dongle_ctx.in_detach != 0) {
		return -1;
	}

	return ((flag == NULL) || (*flag != 0U)) ? 0 : -1;
}

/*
 * Dial helper of the LE271 / SIM767X flows.
 * Sends a single AT command (auto-appends CRLF) then polls *ready_flag for up to timeout_ms.
 * Caller is responsible for clearing the flag before invoking this so we don't latch on stale data.
 * Returns 0 on flag set, -1 on tx failure, detach or timeout.
 */
static int usbh_composite_send_wait(const char *cmd, volatile u8 *ready_flag, u32 timeout_ms)
{
	/* A command too long for the TX buffer is refused by the helper */
	if (RTK_SUCCESS != usbh_composite_acm_send_cmd((const u8 *)cmd, usbh_composite_strlen((u8 *)cmd))) {
		return -1;
	}

	return usbh_composite_wait_flag(ready_flag, timeout_ms);
}

static u8 usbh_composite_quectel_eg915_ctrl(void)
{
	u8 heart_beat = 0;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\",\"orange.m2m.spec\"";
	u8 *pdata4 = (u8 *)"AT+QNETDEVCTL=1,1,1";
	u8 *pdata5 = (u8 *)"AT+CGCONTRDP";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Enter state(%d)--------------------------------------------\n", state);
		}
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_ECM_CFG: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata1, usbh_composite_strlen(pdata1))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SEARCH: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata2, usbh_composite_strlen(pdata2))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SET_APN: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata3, usbh_composite_strlen(pdata3))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata4, usbh_composite_strlen(pdata4))) {
				break;
			}
			if (usbh_composite_wait_flag(NULL, 15000) != 0) {
				return 1;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
			if (0 == usbh_dongle_ctx.quectel.ip_ready) {
				if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata5, usbh_composite_strlen(pdata5))) {
					break;
				}
			}
			if (usbh_composite_wait_flag(&usbh_dongle_ctx.quectel.ip_ready, 10000) == 0) {
				state = QUECTEL_DONGLE_STATUS_MAX;
			} else if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			break;
		case QUECTEL_DONGLE_STATUS_MAX:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get all network info\n");
			rtos_time_delay_ms(10);
			return 0;
			break;
		}
		rtos_time_delay_ms(1000);  /* /force sleep to wait the reply */
	}

	return 1;
}
static u8 usbh_composite_quectel_eg91_ctrl(void)
{
	u8 heart_beat = 0;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\"";
	u8 *pdata4 = (u8 *)"AT+CEREG?";
	u8 *pdata5 = (u8 *)"at+qwwaninfo=0,\"IP\"";
	u8 *pdata6 = (u8 *)"AT+qnetifmac=\"ecm0\"";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter state(%d)--------------------------------------------\n", state);
		}
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_ECM_CFG: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata1, usbh_composite_strlen(pdata1))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SEARCH: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata2, usbh_composite_strlen(pdata2))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SET_APN: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata3, usbh_composite_strlen(pdata3))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata4, usbh_composite_strlen(pdata4))) {
				break;
			}
		}
		if (usbh_composite_wait_flag(&usbh_dongle_ctx.quectel.cereg_ready, 10000) == 0) {
			/* EG 91 use DHCP to get mac */
			state = QUECTEL_DONGLE_STATUS_GET_MAC;
		} else if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS: {
			if (usbh_composite_wait_flag(NULL, 15000) != 0) {
				return 1;
			}
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata5, usbh_composite_strlen(pdata5))) {
				break;
			}
		}
		if (usbh_composite_wait_flag(&usbh_dongle_ctx.quectel.ip_ready, 10000) == 0) {
			state = QUECTEL_DONGLE_STATUS_GET_MAC;
		} else if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		break;
		case QUECTEL_DONGLE_STATUS_GET_MAC: {
			if (RTK_SUCCESS != usbh_composite_acm_send_cmd(pdata6, usbh_composite_strlen(pdata6))) {
				break;
			}
		}
		if (usbh_composite_wait_flag(&usbh_dongle_ctx.quectel.mac_ready, 10000) == 0) {
			state = QUECTEL_DONGLE_STATUS_MAX;
		} else if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		break;
		case QUECTEL_DONGLE_STATUS_MAX:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Send request done\n");
			rtos_time_delay_ms(10);
			return 0;
			break;
		}
		rtos_time_delay_ms(1000);  /* /force sleep to wait the reply */
	}
	return 1;
}

/*
 * LE271 CAT1 ECM dial flow - implementation of Fibocom LE271_CAT1 (Step1..Step13).
 *
 * Returns 0 once the cellular side has an IPv4 (caller can then proceed to ECM link / DHCP).
 * Returns 1 on unrecoverable failure (wrong USB mode triggers AT+CFUN=15 and we fall through
 * to the hot-plug detach path which re-runs the flow on re-enumeration).
 */
static u8 usbh_composite_fibocom_ctrl(void)
{
	u8 heart_beat = 0;
	int retry;
	int reg_elapsed_ms;
	fibocom_dongle_state_t state = FIBOCOM_DONGLE_STATUS_AT_PROBE;

	u8 *pdata0 = (u8 *)"AT";
	u8 *pdata1 = (u8 *)"ATE0";
	u8 *pdata2 = (u8 *)"AT+GTUSBMODE?";
	u8 *pdata3 = (u8 *)"AT+GTUSBMODE=32";
	u8 *pdata4 = (u8 *)"AT+CFUN=15";
	u8 *pdata5 = (u8 *)"AT+CGSN?";
	u8 *pdata6 = (u8 *)"AT+CFSN?";
	u8 *pdata7 = (u8 *)"AT+CFUN?";
	u8 *pdata8 = (u8 *)"AT+CGMR?";
	u8 *pdata9 = (u8 *)"AT+CPIN?";
	u8 *pdata10 = (u8 *)"AT+CSQ?";
	u8 *pdata11 = (u8 *)"AT+CIMI";
	u8 *pdata12 = (u8 *)"AT+CGREG?";
	u8 *pdata13 = (u8 *)"AT+GTRNDIS?";
	u8 *pdata14 = (u8 *)"AT+GTRNDIS=1,1";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter LE271 state(%d)--------------------------------------------\n", state);
		}
		switch (state) {
		case FIBOCOM_DONGLE_STATUS_AT_PROBE:
			/* Step1: probe the module with bare AT, retry every 2s, give up after ~60s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step1: AT probe\n");
			for (retry = 0; (retry < 30) && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.fibocom.at_ok = 0;
				if (usbh_composite_send_wait((const char *)pdata0, &usbh_dongle_ctx.fibocom.at_ok, 1500) == 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 AT no reply, retry %d\n", retry + 1);
				rtos_time_delay_ms(1500);
			}
			if (retry >= 30 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 Step1 timeout or detached\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_ATE0:
			/* Step2: disable AT echo. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step2: ATE0\n");
			usbh_dongle_ctx.fibocom.at_ok = 0;
			if (usbh_composite_send_wait((const char *)pdata1, &usbh_dongle_ctx.fibocom.at_ok, 2000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 ATE0 failed\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_USBMODE:
			/* Step3: query AT+GTUSBMODE? - expected "+GTUSBMODE: 32" (ECM mode).
			   If the value is not 32, set it then issue AT+CFUN=15 to soft-reset and let the
			   hot-plug path re-enumerate the device in ECM mode. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step3: query USB mode\n");
			usbh_dongle_ctx.fibocom.usbmode_seen = 0;
			usbh_dongle_ctx.fibocom.usbmode_is_ecm = 0;
			if (usbh_composite_send_wait((const char *)pdata2, &usbh_dongle_ctx.fibocom.usbmode_seen, 3000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 GTUSBMODE? no reply\n");
				return 1;
			}
			if (usbh_dongle_ctx.fibocom.usbmode_is_ecm == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 not in ECM mode, switch and reboot\n");
				/* No reply check: the module reboots and this flow is left either way */
				usbh_dongle_ctx.fibocom.at_ok = 0;
				usbh_composite_send_wait((const char *)pdata3, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				usbh_dongle_ctx.fibocom.at_ok = 0;
				usbh_composite_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				/* USB will detach; let example_usbh_composite_hotplug_thread re-init and re-enter this flow. */
				rtos_time_delay_ms(1000);
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMEI:
			/* Step4: IMEI - non-fatal, continue even on timeout (info-only step). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step4: AT+CGSN?\n");
			usbh_dongle_ctx.fibocom.cgsn_ok = 0;
			usbh_composite_send_wait((const char *)pdata5, &usbh_dongle_ctx.fibocom.cgsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_SN:
			/* Step5: device serial number (informational, a failure continues). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step5: AT+CFSN?\n");
			usbh_dongle_ctx.fibocom.cfsn_ok = 0;
			usbh_composite_send_wait((const char *)pdata6, &usbh_dongle_ctx.fibocom.cfsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CFUN:
			/* Step6: functional state (informational, expect +CFUN: 1, a failure continues). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step6: AT+CFUN?\n");
			usbh_dongle_ctx.fibocom.cfun_ok = 0;
			usbh_composite_send_wait((const char *)pdata7, &usbh_dongle_ctx.fibocom.cfun_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CGMR:
			/* Step7: firmware version (informational, a failure continues). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step7: AT+CGMR?\n");
			usbh_dongle_ctx.fibocom.cgmr_ok = 0;
			usbh_composite_send_wait((const char *)pdata8, &usbh_dongle_ctx.fibocom.cgmr_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIM:
			/* Step8: SIM detection - poll until +CPIN: READY (cap at ~30s to avoid hangs). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step8: AT+CPIN? (wait SIM)\n");
			for (retry = 0; retry < 15 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.fibocom.cpin_ready = 0;
				usbh_composite_send_wait((const char *)pdata9, &usbh_dongle_ctx.fibocom.cpin_ready, 2000);
				if (usbh_dongle_ctx.fibocom.cpin_ready != 0) {
					break;
				}
				/* inter-retry gap: 2 s, cut short by a detach */
				usbh_composite_wait_flag(NULL, 2000);
			}
			if ((usbh_dongle_ctx.fibocom.cpin_ready == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 SIM not ready\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIGNAL:
			/* Step9: signal strength - retry while CSQ==99 (no signal), cap at ~60s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step9: AT+CSQ? (wait signal)\n");
			for (retry = 0; retry < 30 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.fibocom.csq_seen = 0;
				usbh_dongle_ctx.fibocom.csq_has_signal = 0;
				usbh_composite_send_wait((const char *)pdata10, &usbh_dongle_ctx.fibocom.csq_seen, 2000);
				if (usbh_dongle_ctx.fibocom.csq_has_signal != 0) {
					break;
				}
				/* inter-retry gap: 2 s, cut short by a detach */
				usbh_composite_wait_flag(NULL, 2000);
			}
			if ((usbh_dongle_ctx.fibocom.csq_has_signal == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 no signal\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMSI:
			/* Step10: read CIMI (IMSI) - operator-aware logic could pick APN here; we keep it
			   informational and rely on the module's default APN as the PDF's APN setup is operator-specific. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step10: AT+CIMI\n");
			usbh_dongle_ctx.fibocom.at_ok = 0;
			usbh_composite_send_wait((const char *)pdata11, &usbh_dongle_ctx.fibocom.at_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_PS_REG:
			/* Step11: PS-domain registration via +CGREG. PDF: continuous query timeout 90s -> reset module. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step11: AT+CGREG? (wait PS attach)\n");
			reg_elapsed_ms = 0;
			while (reg_elapsed_ms < 90000 && (usbh_dongle_ctx.in_detach == 0)) {
				usbh_dongle_ctx.fibocom.cgreg_seen = 0;
				usbh_dongle_ctx.fibocom.ps_registered = 0;
				usbh_composite_send_wait((const char *)pdata12, &usbh_dongle_ctx.fibocom.cgreg_seen, 2000);
				if (usbh_dongle_ctx.fibocom.ps_registered != 0) {
					break;
				}
				/* inter-query gap: 2 s, cut short by a detach */
				usbh_composite_wait_flag(NULL, 2000);
				reg_elapsed_ms += 4000;
			}
			if ((usbh_dongle_ctx.fibocom.ps_registered == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 PS attach timeout, reset module\n");
				if (usbh_dongle_ctx.in_detach == 0) {
					/* Device still attached: issue soft-reset and wait 1 s for it to
					 * begin detaching before returning (1 s total guard delay). */
					usbh_dongle_ctx.fibocom.at_ok = 0;
					usbh_composite_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
					rtos_time_delay_ms(1000);
				}
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_DIAL_LOOP:
			/* Step12 + Step13: check IP, dial if missing, loop until IPv4 is allocated. Cap loops to 30. */
			for (retry = 0; (retry < 30) && (usbh_dongle_ctx.in_detach == 0); retry++) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step12: AT+GTRNDIS? (check IP)\n");
				usbh_dongle_ctx.fibocom.gtrndis_seen = 0;
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 0;
				usbh_composite_send_wait((const char *)pdata13, &usbh_dongle_ctx.fibocom.gtrndis_seen, 3000);
				if (usbh_dongle_ctx.fibocom.gtrndis_has_ip != 0) {
					state = FIBOCOM_DONGLE_STATUS_DONE;
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step13: AT+GTRNDIS=1,1 (dial)\n");
				usbh_dongle_ctx.fibocom.at_ok = 0;
				usbh_composite_send_wait((const char *)pdata14, &usbh_dongle_ctx.fibocom.at_ok, 5000);
				rtos_time_delay_ms(1000);
			}
			if ((state != FIBOCOM_DONGLE_STATUS_DONE) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 dial loop exhausted\n");
				return 1;
			}
			break;
		case FIBOCOM_DONGLE_STATUS_DONE:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 has IP, dial OK\n");
			rtos_time_delay_ms(10);
			return 0;
		}
	}

	return 1;
}

/*
 * SIMCom SIM767X dial flow - mirrors the sequence captured in SIM767X_AT_CMD.txt:
 *   AT -> ATE0 -> CPIN? -> CSQ -> COPS? -> CGATT? -> CGDCONT? -> CGACT=1,1
 *   -> NETOPEN -> IPADDR.
 * Returns 0 once +IPADDR reports a non-zero IPv4; 1 on unrecoverable failure
 * (the hot-plug detach path re-runs the flow on re-enumeration).
 */
static u8 usbh_composite_simcom_ctrl(void)
{
	u8 heart_beat = 0;
	int retry;
	simcom_dongle_state_t state = SIMCOM_DONGLE_STATUS_AT_PROBE;

	u8 *pdata_at        = (u8 *)"AT";
	u8 *pdata_ate0      = (u8 *)"ATE0";
	u8 *pdata_cpin      = (u8 *)"AT+CPIN?";
	u8 *pdata_csq       = (u8 *)"AT+CSQ";
	u8 *pdata_cops      = (u8 *)"AT+COPS?";
	u8 *pdata_cgatt     = (u8 *)"AT+CGATT?";
	u8 *pdata_cgact     = (u8 *)"AT+CGACT=1,1";
	u8 *pdata_netopen   = (u8 *)"AT+NETOPEN";
	u8 *pdata_netopen_q = (u8 *)"AT+NETOPEN?";
	u8 *pdata_usbnetip_q = (u8 *)"AT+USBNETIP?";
	u8 *pdata_usbnetip0  = (u8 *)"AT+USBNETIP=0";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter SIM767X state(%d)--------------------------------------------\n", state);
		}
		switch (state) {
		case SIMCOM_DONGLE_STATUS_AT_PROBE:
			/* Step1: probe with bare AT, retry every 2 s, cap ~60 s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step1: AT probe\n");
			for (retry = 0; (retry < 30) && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.simcom.at_ok = 0;
				if (usbh_composite_send_wait((const char *)pdata_at, &usbh_dongle_ctx.simcom.at_ok, 1000) == 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X AT no reply, retry %d\n", retry + 1);
				rtos_time_delay_ms(2000);
			}
			if (retry >= 30 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X Step1 timeout or detached\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_ATE0:
			/* Step2: disable AT echo so response parsing does not see command echoes. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step2: ATE0\n");
			usbh_dongle_ctx.simcom.at_ok = 0;
			if (usbh_composite_send_wait((const char *)pdata_ate0, &usbh_dongle_ctx.simcom.at_ok, 2000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X ATE0 failed\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_SIM:
			/* Step3: wait for +CPIN: READY, cap ~30 s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step3: AT+CPIN? (wait SIM)\n");
			for (retry = 0; retry < 15 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.simcom.cpin_ready = 0;
				usbh_composite_send_wait((const char *)pdata_cpin, &usbh_dongle_ctx.simcom.cpin_ready, 2000);
				if (usbh_dongle_ctx.simcom.cpin_ready != 0) {
					break;
				}
				usbh_composite_wait_flag(NULL, 2000);
			}
			if ((usbh_dongle_ctx.simcom.cpin_ready == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X SIM not ready\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_SIGNAL:
			/* Step4: wait for signal (rssi != 99), cap ~60 s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step4: AT+CSQ (wait signal)\n");
			for (retry = 0; retry < 30 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.simcom.csq_seen = 0;
				usbh_dongle_ctx.simcom.csq_has_signal = 0;
				usbh_composite_send_wait((const char *)pdata_csq, &usbh_dongle_ctx.simcom.csq_seen, 2000);
				if (usbh_dongle_ctx.simcom.csq_has_signal != 0) {
					break;
				}
				usbh_composite_wait_flag(NULL, 2000);
			}
			if ((usbh_dongle_ctx.simcom.csq_has_signal == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X no signal\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_QUERY_OPERATOR:
			/* Step5: operator query (informational; failure is non-fatal). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step5: AT+COPS?\n");
			usbh_dongle_ctx.simcom.cops_ok = 0;
			usbh_composite_send_wait((const char *)pdata_cops, &usbh_dongle_ctx.simcom.cops_ok, 3000);
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_PS_ATTACH:
			/* Step6: wait until +CGATT: 1, cap ~60 s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step6: AT+CGATT? (wait PS attach)\n");
			for (retry = 0; retry < 30 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.simcom.cgatt_seen = 0;
				usbh_dongle_ctx.simcom.cgatt_attached = 0;
				usbh_composite_send_wait((const char *)pdata_cgatt, &usbh_dongle_ctx.simcom.cgatt_seen, 2000);
				if (usbh_dongle_ctx.simcom.cgatt_attached != 0) {
					break;
				}
				usbh_composite_wait_flag(NULL, 2000);
			}
			if ((usbh_dongle_ctx.simcom.cgatt_attached == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X PS attach timeout\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_ACTIVATE_PDP:
			/* Step6b: activate PDP context 1. Failure is non-fatal (may already be active). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step6b: AT+CGACT=1,1\n");
			usbh_dongle_ctx.simcom.at_ok = 0;
			usbh_composite_send_wait((const char *)pdata_cgact, &usbh_dongle_ctx.simcom.at_ok, 15000);
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_NETOPEN:
			/* Step7: open data pass-through session.
			 *
			 * Query state first with AT+NETOPEN? (Read command).
			 * Response: +NETOPEN: <net_state>  0=closed, 1=open  (manual 13.2.1)
			 * This uses a SEPARATE flag (netopen_query_seen / netopen_query_state)
			 * from the execute flag (netopen_ok) because the two commands share
			 * the same +NETOPEN: prefix but with opposite value semantics:
			 *   AT+NETOPEN?  0=closed  1=open
			 *   AT+NETOPEN   0=success non-zero=fail
			 *
			 * If already open: skip AT+NETOPEN (avoids +IP ERROR noisy log).
			 * If not open: send AT+NETOPEN; success = +NETOPEN: 0 or
			 *   "+IP ERROR: Network is already opened" (both handled in RX parse). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step7: AT+NETOPEN\n");
			usbh_dongle_ctx.simcom.netopen_query_seen  = 0;
			usbh_dongle_ctx.simcom.netopen_query_state = 0;
			usbh_composite_send_wait((const char *)pdata_netopen_q, &usbh_dongle_ctx.simcom.netopen_query_seen, 3000);
			if (usbh_dongle_ctx.simcom.netopen_query_seen &&
				usbh_dongle_ctx.simcom.netopen_query_state == 1) {
				/* Session already open per AT+NETOPEN? -> 1 (open). Skip. */
				RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step7: already open, skip\n");
				usbh_dongle_ctx.simcom.netopen_ok = 1;
			} else {
				/* Not open: send AT+NETOPEN, wait for +NETOPEN: 0 or already-opened. */
				for (retry = 0; retry < 3 && (usbh_dongle_ctx.in_detach == 0); retry++) {
					usbh_dongle_ctx.simcom.netopen_ok = 0;
					usbh_composite_send_wait((const char *)pdata_netopen, &usbh_dongle_ctx.simcom.netopen_ok, 15000);
					if (usbh_dongle_ctx.simcom.netopen_ok != 0) {
						break;
					}
					rtos_time_delay_ms(1000);
				}
			}
			if ((usbh_dongle_ctx.simcom.netopen_ok == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X NETOPEN failed\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_QUERY_USBNETIP:
			/* Step8: query USBNET IP mode (AT+USBNETIP? per manual 28.2.3).
			 * mode=0 = Private IP (default): module runs DHCP server on ECM interface,
			 *          host gets 192.168.x.x + GW + DNS automatically.
			 * mode=1 = Public IP: module passes operator IP directly to ECM interface,
			 *          host must configure static IP.
			 * We always want mode=0 (DHCP) for simplicity. If mode=1 is found,
			 * switch it with AT+USBNETIP=0 (AUTO_SAVE, takes effect immediately -
			 * no reboot required per manual 28.2.3 which has no restart NOTE). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step8: AT+USBNETIP?\n");
			usbh_dongle_ctx.simcom.usbnetip_seen = 0;
			usbh_dongle_ctx.simcom.usbnetip_mode = 0;
			usbh_composite_send_wait((const char *)pdata_usbnetip_q, &usbh_dongle_ctx.simcom.usbnetip_seen, 3000);
			if (usbh_dongle_ctx.simcom.usbnetip_seen && usbh_dongle_ctx.simcom.usbnetip_mode != 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step8: mode=1, switching to mode=0\n");
				usbh_dongle_ctx.simcom.at_ok = 0;
				usbh_composite_send_wait((const char *)pdata_usbnetip0, &usbh_dongle_ctx.simcom.at_ok, 9000);
				if (usbh_dongle_ctx.simcom.at_ok == 0) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X AT+USBNETIP=0 failed\n");
					return 1;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step8: mode switched to 0, takes effect immediately\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X Step8: already mode=0\n");
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_DONE:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X dial OK, waiting for ECM link-up + DHCP\n");
			rtos_time_delay_ms(10);
			return 0;
		}
	}

	return 1;
}

static int usbh_composite_dongle_netif_init(void)
{
	network_info_t *net = &usbh_dongle_ctx.quectel.network;
	ip4_addr_t ipaddr, gw, netmask, dnsserver;

	if (inet_aton((char *)net->ip, &ipaddr) == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Inet_aton ip fail\n");
		return -1;
	}
	/* App thread: the address setters are core calls, see LOCK_TCPIP_CORE() */
	LOCK_TCPIP_CORE();
	netif_set_ipaddr(pnetif_usb_eth, &ipaddr);
	UNLOCK_TCPIP_CORE();

	if (inet_aton((char *)net->mask, &netmask) == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Inet_aton netmask fail\n");
		return -1;
	}
	LOCK_TCPIP_CORE();
	netif_set_netmask(pnetif_usb_eth, &netmask);
	UNLOCK_TCPIP_CORE();

	if (inet_aton((char *)net->gw, &gw) == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Inet_aton gw fail\n");
		return -1;
	}
	LOCK_TCPIP_CORE();
	netif_set_gw(pnetif_usb_eth, &gw);
	UNLOCK_TCPIP_CORE();

	if (inet_aton((char *)net->dns, &dnsserver) == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Inet_aton dns fail\n");
		return -1;
	}
	dns_setserver(0, &dnsserver);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Run %s ok\n", __func__);
	return 0;
}

/*
	for different dongle, do the prepare for the ecm
	default:no nothing
	EG915/EG91/xx send cmd to AT port
*/
static int usbh_composite_dongle_diag_cmd(void)
{
	u16 pid = usbh_dongle_ctx.pid;
	u16 vid = usbh_dongle_ctx.vid;

	if (USB_VID == vid) {
		return 0;
	} else if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) {
		/* should check the status finish */
		if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID) {
			return usbh_composite_quectel_eg915_ctrl();
		} else if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID) {
			return usbh_composite_quectel_eg91_ctrl();
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Error happen, check the pid(%d)\n", pid);
		}
	} else if ((USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) && (pid == USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID)) {
		return usbh_composite_fibocom_ctrl();
	} else if ((USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID == vid) && (pid == USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID)) {
		return usbh_composite_simcom_ctrl();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Error happen, check the vid(%d)\n", vid);
	}

	return 1;
}

/**
  * @brief  Composite ECM bulk receive callback
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  */
static void usbh_composite_ecm_received(u8 *buf, u32 len)
{
	netif_adapter_usb_eth_recv(buf, len);
}

#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
#define CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO                  _PA_3

/**
  * @brief  Configure the device power-control GPIO as a push-pull output.
  * @param  GPIO_Pin: Pin wired to the USB device power switch (CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO).
  * @retval None
  */
static void usbh_composite_gpio_power_init(uint32_t GPIO_Pin)
{
	GPIO_InitTypeDef usbh_composite_gpio_initstruct_temp;
	usbh_composite_gpio_initstruct_temp.GPIO_Pin = GPIO_Pin;
	usbh_composite_gpio_initstruct_temp.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&usbh_composite_gpio_initstruct_temp);
}

/**
  * @brief  Drive the device power-control GPIO. Only the LSB is used:
  *         0 cuts power to the device, 1 restores it.
  * @param  level: Output level to write; masked to BIT0 (0 = off, 1 = on).
  * @retval None
  */
static void usbh_composite_gpio_trigger(u8 level)
{
	level = level & BIT0;
	GPIO_WriteBit(CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO, level);
}

/**
  * @brief  One-time init of the power-control GPIO, with the device unpowered.
  *         VBUS is switched on by usbh_start() through usbh_composite_set_vbus(),
  *         so the device always starts from a power-on reset. Guarded by a static
  *         flag so repeated calls are no-ops.
  * @retval None
  */
static void usbh_composite_gpio_init(void)
{
	static int init_ok = 0;
	if (init_ok == 0) {
		init_ok = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "GPIO init\n");
		usbh_composite_gpio_power_init(CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO);
		usbh_composite_gpio_trigger(0);
	}
}

/**
  * @brief  VBUS switch of the host core, see usbh_user_cb_t::set_vbus
  * @param  host: USB host handle
  * @param  on: 1 to power the device, 0 to cut its power
  * @retval RTK_SUCCESS
  */
static int usbh_composite_set_vbus(usb_host_t *host, u8 on)
{
	UNUSED(host);

	RTK_LOGS(TAG, RTK_LOG_INFO, "VBUS %d\n", on);
	usbh_composite_gpio_trigger(on);

	return RTK_SUCCESS;
}
#endif

/**
  * @brief  Have the host core power-cycle the attached USB device through VBUS,
  *         the recovery it falls back on for a device no longer answering a port
  *         reset. The device drops off the bus and is enumerated again; the stack
  *         is kept, see usbh_comp_keep_stack.
  *         Only available when CONFIG_USBH_COMP_GPIO_POWER_CTRL is enabled.
  * @retval RTK_SUCCESS if the power cycle was scheduled, an error otherwise
  */
static int usbh_composite_hotplug_test(void)
{
#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
	int ret;

	if (usbh_comp_stack_ready == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Not ready\n");
		return -RTK_ERR_IO;
	}
	ret = usbh_power_cycle();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VBUS cycle fail %d\n", ret);
	}

	return ret;
#else
	RTK_LOGS(TAG, RTK_LOG_WARN, "Not support\n");

	return RTK_SUCCESS;
#endif
}

/**
  * @brief  ACM bulk IN receive loop - lives across detach/re-attach until a stop.
  * @details The CDC-ACM driver only performs a bulk IN transfer when the
  *          application calls usbh_cdc_acm_receive(). This task keeps one
  *          receive armed at all times so AT-command responses are pulled in
  *          continuously; each completion is delivered via
  *          usbh_composite_acm_received(), which then wakes this task to re-arm.
  *
  *          The outer loop waits for a device to be enumerated (in_detach == 0);
  *          during detach it polls until the next attach clears in_detach to 0
  *          again. A stop sets quit and gives usbh_composite_acm_rx_done_sema.
  */
static void usbh_composite_acm_rx_thread(void *param)
{
	UNUSED(param);

	while (usbh_comp_app.quit == 0U) {
		/* Wait for a device to be enumerated */
		while ((usbh_dongle_ctx.in_detach != 0) && (usbh_comp_app.quit == 0U)) {
			rtos_time_delay_ms(10);
		}

		/* ACM RX loop for the current session */
		while ((usbh_dongle_ctx.in_detach == 0) && (usbh_comp_app.quit == 0U)) {
			int armed = 0;

			/* Lock covers the arming only, see usbh_composite_acm_transmit(). */
			if (usbh_comp_lock(usbh_comp_acm_mutex) == RTK_SUCCESS) {
				armed = (usbh_cdc_acm_receive(usbh_composite_acm_rx_buf, CONFIG_USBH_COMP_PBUF_MAX_LEN) == RTK_SUCCESS) ? 1 : 0;
				usbh_comp_unlock(usbh_comp_acm_mutex);
			}

			if (armed != 0) {
				/* Wait for completion (callback gives the sema).
				 * Use a timeout so detach is noticed promptly. */
				rtos_sema_take(usbh_composite_acm_rx_done_sema, 1000);
			} else {
				/* Pipe not ready yet, or the stack is down: back off and retry. */
				rtos_time_delay_ms(10);
			}
		}
	}

	usbh_composite_worker_exit(0U);
}

/**
 * @brief  Tear down the USB host stack, in the reverse order of
 *         usbh_composite_stack_init(): TRX -> classes -> core.
 * @note   in_detach is asserted first so acm_rx_thread parks before the pipes
 *         go away. No return value: a teardown failure has no recoverable path,
 *         so the only sequence of release calls in this example lives here.
 *         Skipped if the stack is not up, e.g. after a failed re-init.
 * @retval None
 */
static void usbh_composite_deinit_stack(void)
{
	/* in_detach first, so a caller inside a loop gives up its round instead of
	   re-taking a lock between messages and starving this caller out. */
	usbh_dongle_ctx.in_detach = 1;

	/* Both locks, always in this order (acm, ecm). Each caller takes exactly one,
	   never two, so a fixed order here is enough to rule out a cycle. */
	rtos_mutex_take(usbh_comp_acm_mutex, RTOS_MAX_TIMEOUT);
	rtos_mutex_take(usbh_comp_ecm_mutex, RTOS_MAX_TIMEOUT);
	/* Close the gate under the locks: no class API call is in flight here, and any
	   later one gives up on the cleared flag. */
	usbh_comp_stack_ready = 0U;

	if (usbh_comp_stack_up != 0U) {
		usbh_stop();
		usbh_cdc_acm_deinit();
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		usbh_comp_stack_up = 0U;
	}

	rtos_mutex_give(usbh_comp_ecm_mutex);
	rtos_mutex_give(usbh_comp_acm_mutex);
}

/**
  * @brief  Bring the USB stack up, the class API gate stays closed
  * @note   Called by start, then only by the thread owning the stack (hotplug)
  * @retval Status
  */
static int usbh_composite_stack_init(void)
{
	int ret;

	/* Clear all dongle state from the previous session before re-init so that
	 * stale flags (ip_ready, cereg_ready, mac_ready, at_ok, ) cannot be seen
	 * by the new session's state machines or the RX callback. */
	memset((void *)&usbh_dongle_ctx, 0, sizeof(usbh_dongle_ctx));
	/* Keep in_detach asserted while stack is coming up so acm_rx_thread
	 * stays parked and does not race an unattached pipe. */
	usbh_dongle_ctx.in_detach = 1;

	if (usbh_init(&usbh_cfg, &usbh_composite_usr_cb) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		return RTK_FAIL;
	}
	if (usbh_cdc_acm_init(&usbh_composite_acm_cfg, &usbh_composite_acm_class_cfg) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ACM init fail\n");
		usbh_deinit();
		return RTK_FAIL;
	}
	if (usbh_cdc_ecm_init(&usbh_composite_ecm_cfg, &usbh_composite_ecm_class_cfg) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM init fail\n");
		usbh_cdc_acm_deinit();
		usbh_deinit();
		return RTK_FAIL;
	}
	ret = usbh_start();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start USBH fail\n");
		usbh_cdc_ecm_deinit();
		usbh_cdc_acm_deinit();
		usbh_deinit();
		return ret;
	}
	usbh_comp_stack_up = 1U;

	return RTK_SUCCESS;
}

/**
  * @brief  Wait for the dongle, open the gate and prepare it for the ECM transfer
  * @note   Run by the thread owning the stack after every stack init.
  *         On a diag/dial failure the gate stays open on purpose (stack_ready 1, in_detach 0):
  *         a dongle such as the LE271 reboots itself after switching to ECM mode, and the next
  *         detach rebuilds the stack. Meanwhile the AT port stays usable for debugging.
  * @retval RTK_SUCCESS, or RTK_FAIL if a stop came in or the dongle diag failed
  */
static int usbh_composite_bring_up(void)
{
	u8 enter = 0U;

	/* Wait for ECM enumeration. No timeout: the dongle may be inserted at any
	 * time and we must not give up. in_detach is held at 1 by us (set above)
	 * while the stack is coming up, so it must NOT be used as an exit
	 * condition here -- it would immediately false-trigger and abort before
	 * the dongle is even detected. The only exits are a successful
	 * enumeration (usbh_cdc_ecm_usb_is_ready) and a stop. */
	while (!usbh_cdc_ecm_usb_is_ready()) {
		if (usbh_comp_app.quit != 0U) {
			return RTK_FAIL;
		}
		rtos_time_delay_ms(1000);
	}
	/* Cache vid/pid before diag so the RX callback can use usbh_dongle_ctx.vid
	 * without calling the getter on every received packet. */
	usbh_dongle_ctx.vid = usbh_cdc_ecm_get_device_vid();
	usbh_dongle_ctx.pid = usbh_cdc_ecm_get_device_pid();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Example Pid 0x%x/Vid 0x%x\n", usbh_dongle_ctx.pid, usbh_dongle_ctx.vid);

	/* Open the gate before releasing the RX pump: usbh_composite_dongle_diag_cmd()
	   below issues ACM transfers through usbh_composite_acm_transmit(), which is
	   gated on this flag, so it has to be set first or every AT command would be
	   rejected. Ordered gate-then-in_detach because a caller checks both and the
	   reverse order would let one through while the gate was still shut.
	   Only while no stop is claimed: the stop owner sets in_detach after the claim to
	   end the diag below, it must not be cleared behind its back. */
	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (usbh_comp_app.quit == 0U) {
		usbh_comp_stack_ready = 1U;
		/* Release acm_rx_thread now that a device is enumerated and the pipe
		 * is available; diag AT commands need the RX pump running. */
		usbh_dongle_ctx.in_detach = 0;
		enter = 1U;
	}
	rtos_critical_exit(RTOS_CRITICAL_USB);
	if (enter == 0U) {
		return RTK_FAIL;
	}

	/*
		prepare for ecm transfer:
		4G dongle, send AT cmd to the dongle before the network is ready
		other dongle, prepare for the ecm transfer
	*/
	if (usbh_composite_dongle_diag_cmd() == 0) {
		/* Only call prepare_done() on clean success (return 0 = ECM ready with IP).
		 * Any non-zero return - whether a reboot was triggered (DONGLE_DIAG_RET_REBOOT_PENDING)
		 * or a fatal AT error - means ECM is not ready; skip prepare_done(). */
		if (usbh_comp_lock(usbh_comp_ecm_mutex) == RTK_SUCCESS) {
			usbh_cdc_ecm_prepare_done();
			usbh_comp_unlock(usbh_comp_ecm_mutex);
			RTK_LOGS(TAG, RTK_LOG_INFO, "TRX start\n");
			return RTK_SUCCESS;
		}
	}

	/* Gate left open: the next detach rebuilds the stack, the AT port stays usable */
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Dongle diag fail\n");
	return RTK_FAIL;
}

/**
  * @brief  Enter the IP section of the link thread, see usbh_comp_net_busy
  * @retval 1 if entered, 0 if a stop is claimed
  */
static u8 usbh_composite_net_enter(void)
{
	u8 enter = 0U;

	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (usbh_comp_app.quit == 0U) {
		usbh_comp_net_busy = 1U;
		usbh_comp_net_ran = 1U;
		enter = 1U;
	}
	rtos_critical_exit(RTOS_CRITICAL_USB);

	return enter;
}

/**
  * @brief  Take pnetif_usb_eth down so lwIP stops sending to this app
  * @note   Called by the stop owner, quit is already set
  * @retval None
  */
static void usbh_composite_net_down(void)
{
	u32 waited = 0U;

	/* The link thread may be inside lwip_request_ip(): stop the DHCP client until it
	   leaves, a dhcp_stop() that lands before its dhcp_start() would be undone by it */
	while ((usbh_comp_net_busy != 0U) && (waited < USB_APP_JOIN_TIMEOUT_MS)) {
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		rtos_time_delay_ms(10);
		waited += 10U;
	}
	if (usbh_comp_net_ran != 0U) {
		/* Also clears the static address of the EG915 */
		lwip_dhcp_stop(NETIF_USB_ETH_INDEX);
		usbh_comp_net_ran = 0U;
	}
	netifapi_netif_set_link_down(pnetif_usb_eth);
	netifapi_netif_set_down(pnetif_usb_eth);
	if (netif_default == pnetif_usb_eth) {
		netifapi_netif_set_default(pnetif_sta);
	}
}

/**
  * @brief  Finish a claimed stop: network down, workers joined, stack and OS objects released
  * @note   The two class mutexes are kept, see their declaration
  * @param  workers: Number of workers to join
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT with everything kept if a worker did not leave
  */
static int usbh_composite_teardown_all(u32 workers)
{
	u32 waited = 0U;

	/* Ends the dongle diag, parks the RX pump and turns every class API caller away */
	usbh_dongle_ctx.in_detach = 1;

	usbh_composite_net_down();

	/* The hotplug thread waits on the detach sema, the RX pump on its done sema, the others poll quit */
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	if (usbh_composite_detach_sema != NULL) {
		rtos_sema_give(usbh_composite_detach_sema);
	}
#endif
	if (usbh_composite_acm_rx_done_sema != NULL) {
		rtos_sema_give(usbh_composite_acm_rx_done_sema);
	}
	if (usb_app_ctx_join(&usbh_comp_app, workers) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}
	/* An AT+USBHACMECM=send command may still wait for its TX completion on the send sema */
	while ((usbh_comp_at_busy != 0U) && (waited < USB_APP_JOIN_TIMEOUT_MS)) {
		rtos_time_delay_ms(10);
		waited += 10U;
	}
	if (usbh_comp_at_busy != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone. The stack is down already if the hotplug thread left mid-rebuild. */
	usbh_composite_deinit_stack();

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	if (usbh_composite_detach_sema != NULL) {
		rtos_sema_delete(usbh_composite_detach_sema);
		usbh_composite_detach_sema = NULL;
	}
#endif
	if (usbh_composite_acm_send_sema != NULL) {
		rtos_sema_delete(usbh_composite_acm_send_sema);
		usbh_composite_acm_send_sema = NULL;
	}
	if (usbh_composite_acm_rx_done_sema != NULL) {
		rtos_sema_delete(usbh_composite_acm_rx_done_sema);
		usbh_composite_acm_rx_done_sema = NULL;
	}
	usbh_comp_workers = 0U;
	usbh_comp_net_busy = 0U;
	usb_app_ctx_end(&usbh_comp_app);

	return RTK_SUCCESS;
}

/**
  * @brief  Last call of every worker
  * @param  fatal: 1 if the stack can not be rebuilt, the app has to go down
  * @retval None
  */
static void usbh_composite_worker_exit(u8 fatal)
{
	if ((fatal != 0U) && (usb_app_ctx_claim_stop(&usbh_comp_app) != 0U)) {
		/* No stop is in flight: finish the teardown here, this thread is not joined */
		if (usbh_composite_teardown_all(usbh_comp_workers - 1U) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBH ACM+ECM comp demo stop\n");
		}
	} else {
		usb_app_ctx_leave(&usbh_comp_app);
	}
	rtos_task_delete(NULL);
}

static void example_usbh_composite_link_change_thread(void *param)
{
	u8 *mac;
	u8 link_is_up = 0;
	u8 supported;
	u8 use_static;
	u8 ok;
	u16 vid, pid;
	u32 link_down_log_cnt = 0;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task\n");

	/* Wait for the USB stack to be ready before touching ECM APIs. During
	 * detach/re-init phases in_detach is 1 and the stack has no active
	 * pipes, so we must not call usbh_cdc_ecm_get_* helpers. */
	while (usbh_comp_app.quit == 0U) {
		if (usbh_dongle_ctx.in_detach == 0 && usbh_cdc_ecm_usb_is_ready()) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB init done\n");
			break;
		}
		rtos_time_delay_ms(1000);
	}

	while (usbh_comp_app.quit == 0U) {
		/* Park while USB stack is being torn down / re-initialized. Once
		 * in_detach clears, the next iteration re-queries ECM state. */
		if (usbh_dongle_ctx.in_detach != 0) {
			ethernet_unplug = ETH_STATUS_IDLE;
			rtos_time_delay_ms(500);
			continue;
		}

		pid = usbh_cdc_ecm_get_device_pid();
		vid = usbh_cdc_ecm_get_device_vid();

		link_is_up = usbh_cdc_ecm_get_link_status();

		/* MAC source and IP setup per dongle: the EG91 reports its MAC over AT, the others in the
		   ECM iMACAddress string. The EG915 takes the static IP of AT+CGCONTRDP, the others DHCP;
		   LE271 / SIM767X get it from the module's built-in DHCP server (mode=0, 192.168.x.x),
		   usbh_composite_simcom_ctrl() ensures mode=0 before returning. */
		supported = 1U;
		use_static = 0U;
		if ((USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) && (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID)) {
			mac = usbh_dongle_ctx.quectel.mac;
		} else if ((USB_VID == vid) || ((USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) && (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID)) ||
				   ((USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) && (pid == USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID)) ||
				   ((USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID == vid) && (pid == USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID))) {
			mac = (u8 *)usbh_cdc_ecm_process_mac_str();
			use_static = ((USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) && (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID)) ? 1U : 0U;
		} else {
			mac = NULL;
			supported = 0U;
		}

		if ((supported != 0U) && (1 == link_is_up) && (ethernet_unplug < ETH_STATUS_INIT)) {  /* unlink -> link */
			if (mac == NULL) {
				/* iMACAddress string descriptor not readable yet; retry next round.
				   ethernet_unplug stays below ETH_STATUS_INIT so this branch is re-entered. */
				RTK_LOGS(TAG, RTK_LOG_WARN, "MAC not ready\n");
				rtos_time_delay_ms(1000);
				continue;
			}
			/* Enter the IP section only while no stop is claimed, see usbh_comp_net_busy */
			if (usbh_composite_net_enter() == 0U) {
				break;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "%s\n", (use_static != 0U) ? "Set static IP" : "Do DHCP");
			/* hwaddr is read by the tcpip thread, written under its core lock */
			LOCK_TCPIP_CORE();
			memcpy((void *)pnetif_usb_eth->hwaddr, (const void *)mac, 6);
			UNLOCK_TCPIP_CORE();
			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

			if (use_static != 0U) {
				ok = (usbh_composite_dongle_netif_init() == 0) ? 1U : 0U;
				if (ok != 0U) {
					netifapi_netif_set_up(pnetif_usb_eth);
					netifapi_netif_set_link_up(pnetif_usb_eth);
				}
			} else {
				netifapi_netif_set_link_up(pnetif_usb_eth);
				ok = ((DHCP_ADDRESS_ASSIGNED == lwip_request_ip(NETIF_USB_ETH_INDEX)) && (usbh_comp_app.quit == 0U)) ? 1U : 0U;
			}
			if (ok != 0U) {
				netifapi_netif_set_default(pnetif_usb_eth);  /* Set default gw to ether netif */
				/* Only once the IP is set: on failure the state stays below INIT and the next round asks again */
				ethernet_unplug = ETH_STATUS_INIT;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
			}
			/* Left the IP section: a stop owner waiting for it takes the netif down now */
			usbh_comp_net_busy = 0U;
			if ((ok == 0U) && (usbh_comp_app.quit == 0U)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "%s fail\n", (use_static != 0U) ? "Static IP" : "DHCP");
				rtos_time_delay_ms(1000);
			}
		} else if ((supported != 0U) && (0 == link_is_up) && (ethernet_unplug >= ETH_STATUS_INIT)) {  /* link -> unlink */
			ethernet_unplug = ETH_STATUS_DEINIT;
			netifapi_netif_set_default(pnetif_sta);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
		} else {
			if ((link_is_up == 0) && (++link_down_log_cnt % 10 == 1)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ECM link is down (%u)\n", link_down_log_cnt);
			}
			if ((supported == 0U) && (link_is_up != 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Vid 0x%x/Pid 0x%x not match\n", vid, pid);
			}
			rtos_time_delay_ms(1000);
		}
	}

	usbh_composite_worker_exit(0U);
}

/**
  * @brief  First bring-up after start: waits for the dongle, then runs its diag once
  * @note   Run by the thread owning the stack, so no two threads ever run the diag together
  * @retval None
  */
static void usbh_composite_first_bring_up(void)
{
	/* On failure the gate stays open and the next detach rebuilds the stack, see usbh_composite_bring_up() */
	if ((usbh_composite_bring_up() != RTK_SUCCESS) && (usbh_comp_app.quit == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail, check hardware/cellular\n");
	}
}

#if !CONFIG_USBH_COMP_HOT_PLUG_TEST
/* Bring-up worker when no hotplug thread owns the stack */
static void example_usbh_composite_init_task(void *param)
{
	UNUSED(param);

	usbh_composite_first_bring_up();
	usbh_composite_worker_exit(0U);
}
#endif

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
/**
 * @brief  Hot-plug worker thread. Blocks on detach sema, tears down the USB
 *         stack, re-initializes it. usbh_composite_bring_up() manages in_detach on its own,
 *         releasing it only after a device is enumerated so the RX pump does
 *         not race an unattached pipe. A stop gives the sema with quit set.
 */
static void example_usbh_composite_hotplug_thread(void *param)
{
	u8 fatal = 0U;

	UNUSED(param);

	/* A detach or a stop during the first bring-up is waiting on the sema below */
	usbh_composite_first_bring_up();

	for (;;) {
		if (rtos_sema_take(usbh_composite_detach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (usbh_comp_app.quit != 0U) {
			/* Stop requested: the stop owner tears the stack down */
			break;
		}
		if (usbh_comp_keep_stack != 0U) {
			/* The core resets the device and enumerates it again, or waits for a replug: only
			   the dongle bring-up is redone, on the stack already up */
			usbh_comp_keep_stack = 0U;
			usbh_dongle_ctx.in_detach = 1;
			usbh_comp_stack_ready = 0U;
			if (usbh_composite_bring_up() != RTK_SUCCESS) {
				if (usbh_comp_app.quit != 0U) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reset bring up fail, wait for next detach\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Reset done\n");
			}
			continue;
		}
		usbh_composite_deinit_stack();
		rtos_time_delay_ms(100);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
		if (usbh_comp_app.quit != 0U) {
			break;
		}

		if (usbh_composite_stack_init() != RTK_SUCCESS) {
			/* The stack is down for good: usbh_comp_stack_up stays 0, so a stop does not deinit it twice */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
			fatal = 1U;
			break;
		}
		/* usbh_composite_bring_up() clears in_detach once the dongle is enumerated. If the diag
		 * fails after that, the gate stays open: the next detach rebuilds the stack, the AT port
		 * stays usable. */
		if (usbh_composite_bring_up() != RTK_SUCCESS) {
			if (usbh_comp_app.quit != 0U) {
				break;
			}
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail, wait for next detach\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		}
	}

	if (fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	}
	usbh_composite_worker_exit(fatal);
}
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

/* Exported functions --------------------------------------------------------*/

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	int ret;

	if (usbh_comp_lock(usbh_comp_ecm_mutex) != RTK_SUCCESS) {
		return -RTK_ERR_IO;
	}
	ret = usbh_cdc_ecm_send_data(buf, len, block);
	usbh_comp_unlock(usbh_comp_ecm_mutex);

	return ret;
}

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int usbh_composite_start(void)
{
	int ret;
	rtos_task_t task;

	ret = usb_app_ctx_begin(&usbh_comp_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	usbh_comp_stack_ready = 0U;
	usbh_comp_net_busy = 0U;
	usbh_comp_net_ran = 0U;
	usbh_comp_workers = 0U;
	memset((void *)&usbh_dongle_ctx, 0, sizeof(usbh_dongle_ctx));
	usbh_dongle_ctx.in_detach = 1;

	/* Created by the first start and never deleted, see the declarations. Nothing else exists
	   yet, and the teardown needs both class mutexes. */
	if (usbh_comp_acm_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_comp_acm_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm mutex fail\n");
			usb_app_ctx_end(&usbh_comp_app);
			return ret;
		}
	}
	if (usbh_comp_ecm_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_comp_ecm_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ecm mutex fail\n");
			usb_app_ctx_end(&usbh_comp_app);
			return ret;
		}
	}
	if (usbh_comp_at_tx_mutex == NULL) {
		ret = rtos_mutex_create(&usbh_comp_at_tx_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create at tx mutex fail\n");
			usb_app_ctx_end(&usbh_comp_app);
			return ret;
		}
	}

	rltk_usb_eth_init();

#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
	usbh_composite_gpio_init();
#endif

	/* Freed by the teardown once every worker is joined and the stack is down */
	ret = rtos_sema_create(&usbh_composite_acm_rx_done_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm rx done sema fail\n");
		goto exit_stop;
	}
	ret = rtos_sema_create(&usbh_composite_acm_send_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm send sema fail\n");
		goto exit_stop;
	}
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	ret = rtos_sema_create(&usbh_composite_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto exit_stop;
	}
#endif

	ret = usbh_composite_stack_init();
	if (ret != RTK_SUCCESS) {
		goto exit_stop;
	}

	ret = rtos_task_create(&task, "usbh_composite_acm_rx_thread", usbh_composite_acm_rx_thread, NULL, CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm_rx thread fail\n");
		goto exit_stop;
	}
	usbh_comp_workers++;
	ret = rtos_task_create(&task, "usbh_composite_link_change_thread", example_usbh_composite_link_change_thread, NULL, CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_LINK_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create link_change thread fail\n");
		goto exit_stop;
	}
	usbh_comp_workers++;
#if !CONFIG_USBH_COMP_HOT_PLUG_TEST
	/* The first bring-up waits for the dongle, so it runs in a worker and start returns */
	ret = rtos_task_create(&task, "usbh_composite_init_thread", example_usbh_composite_init_task, NULL, CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
		goto exit_stop;
	}
	usbh_comp_workers++;
#endif

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&task, "usbh_composite_hotplug_thread", example_usbh_composite_hotplug_thread, NULL, CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto exit_stop;
	}
	usbh_comp_workers++;
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning the
	   hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so the
	   local interrupt disables inside the teardown are meaningful again. The lwIP TX
	   thread and the ACM RX thread stay unaffined: a pending teardown is excluded
	   from them by the per-class mutexes, not by core. */
	rtos_task_set_affinity(task, 0);
#endif
#endif
	RTK_LOGS(TAG, RTK_LOG_INFO, "USB init start\n");

	return RTK_SUCCESS;

exit_stop:
	/* No hotplug thread runs yet, so nothing else can claim the stop */
	if (usb_app_ctx_claim_stop(&usbh_comp_app) != 0U) {
		if (usbh_composite_teardown_all(usbh_comp_workers) != RTK_SUCCESS) {
			/* A worker may still run on the stack: keep it, the app stays not idle */
			return -RTK_ERR_TIMEOUT;
		}
	} else if (usb_app_ctx_wait_idle(&usbh_comp_app) != RTK_SUCCESS) {
		return -RTK_ERR_TIMEOUT;
	}
#ifndef USB_APP_AGGREGATE
	/* Not in the usb_drd image: another app may hold usb_eth_tx_mutex there */
	rltk_usb_eth_deinit();
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH ACM+ECM comp demo aborted\n");

	return ret;
}

/* Standalone entry thread: brings the demo up and leaves */
static void example_usbh_composite_thread(void *param)
{
	UNUSED(param);

	if (usbh_composite_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/**
  * @brief  Start the ACM + ECM composite host, used by the USB app manager
  * @note   Task context only. Returns once the stack is up, the dongle diag and DHCP run in workers.
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbh_composite_cdc_acm_ecm_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH ACM+ECM comp demo start\n");

	return usbh_composite_start();
}

/**
  * @brief  Stop the ACM + ECM composite host and release everything it owns
  * @note   Task context only. Safe to call when not running. pnetif_usb_eth is taken down first.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbh_composite_cdc_acm_ecm_stop(void)
{
	int ret;

	if (usb_app_ctx_claim_stop(&usbh_comp_app) == 0U) {
		/* Idle, or a worker found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&usbh_comp_app);
	}

	ret = usbh_composite_teardown_all(usbh_comp_workers);
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH ACM+ECM comp demo stop\n");
	}

	return ret;
}

/**
  * @brief  Example entry: start the ACM + ECM composite host from its own thread
  * @retval None
  */
void example_usbh_composite_cdc_acm_ecm(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH ACM+ECM comp demo start\n");

	ret = rtos_task_create(&task, "usbh_composite_start_thread", example_usbh_composite_thread, NULL,
						   CONFIG_USBH_COMP_START_THREAD_STACK_SIZE, CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init thread fail\n");
	}
}

#else
#error "No Lwip USB Ethernet Configuration"
#endif
