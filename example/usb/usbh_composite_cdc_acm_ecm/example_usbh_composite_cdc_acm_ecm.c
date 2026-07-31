/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
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
#include "usbh_cdc_acm.h"
#include "usbh_cdc_ecm.h"

/* Private defines -----------------------------------------------------------*/

/*enable this used to check ecm init/deinit memory leakage*/
#define CONFIG_USBH_COMP_HOT_PLUG_TEST                          1     /* Hot plug test */
#define CONFIG_USBH_COMP_MEM_CHECK                              0
#define CONFIG_USBH_COMP_ENABLE_DUMP_FILE                       0
#define CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD            0
#define CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC             1

#define CONFIG_USBH_COMP_PBUF_MAX_LEN                           (1024)
#define CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR                   (128)

/* Thread priorities */
#define CONFIG_USBH_COMP_INIT_THREAD_PRIORITY                   5U
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY                6U
#define CONFIG_USBH_COMP_LINK_THREAD_PRIORITY                   3U
#define CONFIG_USBH_COMP_MEM_CHECK_THREAD_PRIORITY              3U
#define CONFIG_USBH_COMP_DOWNLOAD_THREAD_PRIORITY               2U

/* Thread stack sizes */
#define CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE                 (1024U)
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE              (1024U)
#define CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE                 (1600U)
#if CONFIG_USBH_COMP_MEM_CHECK
#define CONFIG_USBH_COMP_MEM_CHECK_THREAD_STACK_SIZE            (2048U)
#endif
#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
#define CONFIG_USBH_COMP_DOWNLOAD_THREAD_STACK_SIZE             (1024U * 5)
#endif

/**
 * @brief Enable GPIO-driven power control for the attached USB device
 * When enabled, a GPIO pin (CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO) is wired to the device's power switch
 * so firmware can power-cycle the device to physically trigger a hot-plug.
 * This is distinct from CONFIG_USBH_COMP_HOT_PLUG_TEST, which only re-initializes the
 * host stack in software after a detach event.
 */
#define CONFIG_USBH_COMP_GPIO_POWER_CTRL                        0

/* while do mem check, disable download & hotplug */
#if CONFIG_USBH_COMP_MEM_CHECK
#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
#undef  CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
#define CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD            0
#endif
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
#undef  CONFIG_USBH_COMP_HOT_PLUG_TEST
#define CONFIG_USBH_COMP_HOT_PLUG_TEST                          0
#endif
#endif

#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
#define CONFIG_USBH_COMP_MD5_CHECK_BUFFER_LEN                   (2)
/* socket server info */
#define CONFIG_USBH_COMP_SERVER_HOST                            "www.baidu.com"
#define CONFIG_USBH_COMP_SERVER_PORT                            80
#define CONFIG_USBH_COMP_RESOURCE                               "/"
#define CONFIG_USBH_COMP_BUFFER_SIZE                            1000  /* download test buffer length */
#define CONFIG_USBH_COMP_RECV_TO                                (60*1000)  /* ms */

#if CONFIG_USBH_COMP_ENABLE_DUMP_FILE
#define CONFIG_USBH_COMP_PSRAM_HEAP_SIZE_TEST                   (29000)
#endif  /* DUMP_FILE */
#endif  /* CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD */

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
			volatile u8 cimi_ok;
			volatile u8 cgreg_seen;
			volatile u8 ps_registered;
			volatile u8 gtrndis_seen;
			volatile u8 gtrndis_has_ip;
		} fibocom;
	};
	u16 vid;
	u16 pid;
	volatile u8 in_detach;
	volatile u8 acm_rx_busy;
	u8 dhcp_done;
} dongle_ctx_t;

/* Private function prototypes -----------------------------------------------*/
extern void rltk_usb_eth_init(void);
extern void rltk_usb_eth_deinit(void);

static int usbh_comp_detach(void);
static int usbh_comp_ecm_rxdata(u8 *buf, u32 len);
static int usbh_comp_acm_rxdata(u8 *buf, u32 len, u8 status);
static int usbh_comp_acm_transmit_cb(u8 status);
static void usbh_comp_acm_rx_thread(void *param);
static int usbh_comp_cb_process(usb_host_t *host, u8 msg);
static int usbh_comp_cb_device_check(usb_host_t *host, u8 cfg_max);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
static unsigned char dl_buf[CONFIG_USBH_COMP_BUFFER_SIZE + 1];
static mbedtls_md5_context ctx;
#if CONFIG_USBH_COMP_ENABLE_DUMP_FILE
static unsigned char dump_psRAMHeap[CONFIG_USBH_COMP_PSRAM_HEAP_SIZE_TEST];
#endif
#endif  /* CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD */

#if CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC
static const u16 usbh_comp_ecm_led_color[1] = {0x1122};
static const u8 usbh_comp_ecm_mac_str[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

static rtos_sema_t usbh_comp_detach_sema;
static rtos_task_t usbh_comp_link_check_task;  /* link up down task */
static rtos_task_t usbh_comp_ecm_init_task;  /* delete while init done */
static rtos_task_t usbh_comp_acm_rx_task;  /* acm rx task */

static u8 usbh_comp_acm_tx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 usbh_comp_acm_rx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));

static rtos_sema_t usbh_comp_acm_rx_done_sema;
static rtos_sema_t usbh_comp_acm_send_sema;

static usbh_cdc_acm_param_t usbh_comp_dongle_array[] = {
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID, USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID, 2},
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID, USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID,  2},
	{USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID, USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID, 2},
	{0, 0, 0},
};

static dongle_ctx_t usbh_dongle_ctx;

static const usbh_cdc_ecm_priv_data_t usbh_comp_ecm_priv = {
#if CONFIG_USBH_COMP_ENABLE_USER_SET_DONGLE_MAC
	usbh_comp_ecm_led_color,
	usbh_comp_ecm_mac_str,
	sizeof(usbh_comp_ecm_led_color) / sizeof(usbh_comp_ecm_led_color[0]),
#else
	NULL,
	NULL,
	0,
#endif
};

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1792U,
	.main_task_priority = CONFIG_USBH_COMP_INIT_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.class_num = 2U,   /* CDC-ACM + CDC-ECM */
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

static const usbh_cdc_acm_cb_t usbh_comp_acm_cfg = {
	.receive = usbh_comp_acm_rxdata,
	.transmit = usbh_comp_acm_transmit_cb,
	.priv = usbh_comp_dongle_array,
};

static const usbh_cdc_ecm_state_cb_t usbh_comp_ecm_cfg = {
	.bulk_received = usbh_comp_ecm_rxdata,
	.detach = usbh_comp_detach,
};

static const usbh_user_cb_t usbh_comp_usr_cb = {
	.process = usbh_comp_cb_process,
	.validate = usbh_comp_cb_device_check,
};

/* Private functions ---------------------------------------------------------*/
static u8 usbh_comp_hex_to_char(u8 hex_num)
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

static u32 usbh_comp_strlen(u8 *pbuf)
{
	return strlen((char *)pbuf);
}

static void usbh_comp_dongle_set_netinfo(u8 *pbuf, u8 *name)
{
	const char *pname = (const char *)name;
	u8 len = 0;

	if (pbuf[0] == '"') {
		pbuf ++;
	}
	len = usbh_comp_strlen(pbuf);
	if (pbuf[len - 1] == '"') {
		pbuf[len - 1] = 0;
	}
	len = usbh_comp_strlen(pbuf);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type(%s)=[%s(%d)]\n", name, pbuf, len);

	/* whether support netinfo */
	if (_strcmp(pname, "gw") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.gw, pbuf, len);
	} else if (_strcmp(pname, "ip") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.ip, pbuf, len);
	} else if (_strcmp(pname, "mask") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.mask, pbuf, len);
	} else if (_strcmp(pname, "dns") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.dns, pbuf, len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}
}
static u8 *usbh_comp_dongle_get_netinfo(u8 *name)
{
	const char *pname = (const char *)name;

	/* whether support netinfo */
	if (_strcmp(pname, "gw") == 0) {
		return usbh_dongle_ctx.quectel.network.gw;
	} else if (_strcmp(pname, "ip") == 0) {
		return usbh_dongle_ctx.quectel.network.ip;
	} else if (_strcmp(pname, "mask") == 0) {
		return usbh_dongle_ctx.quectel.network.mask;
	} else if (_strcmp(pname, "dns") == 0) {
		return usbh_dongle_ctx.quectel.network.dns;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}

	return NULL;
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
  * @param  msg: @ref usbh_msg_t
  * @retval Status
  */
static int usbh_comp_cb_process(usb_host_t *host, u8 msg)
{
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_cdc_ecm_choose_config(host);
		break;
	case USBH_MSG_CONNECTED:
	case USBH_MSG_DISCONNECTED:
	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  Device validate callback (hub multi-device scenario).
  * @details Accept the device only if its descriptors expose the ECM
  *          control + data interfaces this composite example drives.
  * @param  host: USB host handle
  * @param  cfg_max: bNumConfigurations
  * @retval HAL_OK to accept the device, non-zero to discard it.
  */
static int usbh_comp_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

/**
  * @brief  Composite detach callback
  * @retval Status
  */
static int usbh_comp_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	/* Idempotent: only signal the hot-plug thread once per detach cycle. */
	if (usbh_dongle_ctx.in_detach == 0U) {
		usbh_dongle_ctx.in_detach = 1U;
		rtos_sema_give(usbh_comp_detach_sema);
	}
#endif
	return HAL_OK;
}

/**
  * @brief  ACM bulk OUT (transmit) completion callback.
  * @details Gives usbh_comp_acm_send_sema so usbh_comp_acm_transmit() can wait for the
  *          bulk OUT transfer to finish before the caller reuses usbh_comp_acm_tx_buf.
  */
static int usbh_comp_acm_transmit_cb(u8 status)
{
	if (status == HAL_OK) {
		if (usbh_comp_acm_send_sema != NULL) {
			rtos_sema_give(usbh_comp_acm_send_sema);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ACM TX fail: %d\n", status);
	}
	return HAL_OK;
}

/**
  * @brief  Send an AT command over ACM bulk OUT and wait for TX completion.
  * @param  buf: data buffer (typically usbh_comp_acm_tx_buf)
  * @param  len: number of bytes to send
  * @retval HAL_OK on success (TX issued and completed), error otherwise.
  * @note   Waiting for completion (via usbh_comp_acm_send_sema, given by
  *         usbh_comp_acm_transmit_cb) prevents the caller from overwriting buf
  *         while the previous transfer is still in flight.
  */
static int usbh_comp_acm_transmit(u8 *buf, u32 len)
{
	int ret = usbh_cdc_acm_transmit(buf, len);
	if (ret == HAL_OK) {
		if (usbh_comp_acm_send_sema != NULL) {
			/* Bounded wait so a lost completion / detach cannot hang forever. */
			rtos_sema_take(usbh_comp_acm_send_sema, 1000);
		}
	}
	return ret;
}

/**
  * @brief  Composite ACM bulk receive callback
  * @param  pbuf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int usbh_comp_acm_rxdata(u8 *pbuf, u32 len, u8 status)  /* type is usb transfer type */
{
	UNUSED(status);
	u32 i;
	u16 vid = usbh_dongle_ctx.vid;
	usbh_dongle_ctx.acm_rx_busy = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "ACM data received len(%u) \n", len);

	if (pbuf == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ACM data received NULL or len=0\n");
		return HAL_OK;
	}

	for (i = 0; i < len;) {
		if (i + 10 <= len) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%c%c%c%c%c%c%c%c%c%c",
					 pbuf[i], pbuf[i + 1], pbuf[i + 2], pbuf[i + 3], pbuf[i + 4],
					 pbuf[i + 5], pbuf[i + 6], pbuf[i + 7], pbuf[i + 8], pbuf[i + 9]);
			i += 10;
		} else {
			for (; i < len; i++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "%c", pbuf[i]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n");
			break;
		}
	}

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
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"ip");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			/* parse to get the gw */
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"mask");
			p = strsep(&buff, ",");
			/* parse to get the dns */
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
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
			if (p != NULL) {
				for (i = 0; i < 6; i++) {
					usbh_dongle_ctx.quectel.mac[i] = usbh_comp_hex_to_char(p[3 * i + 0]) * 16 + usbh_comp_hex_to_char(p[3 * i + 1]) ;
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
				memset(ptmp, 0, sizeof(ptmp));
				memcpy(ptmp, p, plen);
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
				usbh_comp_dongle_set_netinfo(ptmp, (u8 *)"ip");
				usbh_comp_dongle_set_netinfo(tail, (u8 *)"mask");
			}
			p = strsep(&buff, ",");
			/* parse to get the gw */
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			/* parse to get the dns */
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
			p = strsep(&buff, ",");

			usbh_dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
	} else if (USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) {
		/* LE271 (Fibocom) - parse the responses listed in the LE271 ECM dial flow.
		   Responses arrive line-by-line; we set per-marker flags consumed by usbh_comp_fibocom_ctrl(). */
		if (strstr((char *)pbuf, "+GTUSBMODE:")) {
			char *p = strstr((char *)pbuf, "+GTUSBMODE:") + sizeof("+GTUSBMODE:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.fibocom.usbmode_is_ecm = (atoi(p) == 32) ? 1 : 0;
			usbh_dongle_ctx.fibocom.usbmode_seen = 1;
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
			usbh_dongle_ctx.fibocom.csq_has_signal = (atoi(p) == 99) ? 0 : 1;
			usbh_dongle_ctx.fibocom.csq_seen = 1;
		}
		/* CIMI returns the IMSI digits followed by OK; treat any "460" prefix as success.
		   Generic OK (set below) also unblocks Step10 for non-CHN MNC. */
		if (strstr((char *)pbuf, "460")) {
			usbh_dongle_ctx.fibocom.cimi_ok = 1;
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
	}

	/* Wake the ACM RX task so it re-arms the next receive. */
	if (usbh_comp_acm_rx_done_sema != NULL) {
		rtos_sema_give(usbh_comp_acm_rx_done_sema);
	}

	return HAL_OK;
}

/*
	for appx cmd test entry
*/
static u32 usbh_comp_acm_cmd_test(u16 argc, u8 *argv[])
{
	u8 *cmd;
	u8 try_cnt = 10;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (u8 *)argv[0];

	RTK_LOGS(TAG, RTK_LOG_INFO, "At cmd(%s)\n", cmd);
	if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == usbh_dongle_ctx.vid ||
		USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == usbh_dongle_ctx.vid) {
		memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
		memcpy(usbh_comp_acm_tx_buf, cmd, usbh_comp_strlen(cmd));
		usbh_comp_acm_tx_buf[usbh_comp_strlen(cmd) + 0] = 0x0D;
		usbh_comp_acm_tx_buf[usbh_comp_strlen(cmd) + 1] = 0x0A;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Pre AtCmd\n");
		while (try_cnt >  0) {
			if (HAL_OK == usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			rtos_time_delay_ms(1000);

			try_cnt --;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "At cmd(%s) finish!\n", cmd);
	return 0;
}

static u8 usbh_comp_quectel_eg915_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata0 = (u8 *)"AT+qdbgcfg=\"tracecfg\",0,1";
	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\",\"orange.m2m.spec\"";
	u8 *pdata4 = (u8 *)"AT+QNETDEVCTL=1,1,1";
	u8 *pdata5 = (u8 *)"AT+CGCONTRDP";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter state(%d)--------------------------------------------\n", state);
		}
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_IDLE:  /* for debug issue */
			while (usbh_dongle_ctx.acm_rx_busy == 0 && usbh_dongle_ctx.in_detach == 0) {
				rtos_time_delay_ms(10);
			}
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata0, usbh_comp_strlen(pdata0));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata0) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata0) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata1, usbh_comp_strlen(pdata1));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SEARCH: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata2, usbh_comp_strlen(pdata2));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SET_APN: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata3, usbh_comp_strlen(pdata3));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata4, usbh_comp_strlen(pdata4));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			for (idx = 0; idx < 150 && usbh_dongle_ctx.in_detach == 0; idx++) {
				rtos_time_delay_ms(100);
			}
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
			if (0 == usbh_dongle_ctx.quectel.ip_ready) {
				memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
				memcpy(usbh_comp_acm_tx_buf, pdata5, usbh_comp_strlen(pdata5));
				usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata5) + 0] = 0x0D;
				usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata5) + 1] = 0x0A;
				if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
					break;
				}
			}
			idx = 0;
			do {
				idx++;
				if ((usbh_dongle_ctx.quectel.ip_ready != 0) || (usbh_dongle_ctx.in_detach != 0)) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			if (usbh_dongle_ctx.quectel.ip_ready != 0) {
				state = QUECTEL_DONGLE_STATUS_MAX;
			}
			break;
		case QUECTEL_DONGLE_STATUS_MAX:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get all network info !\n");
			rtos_time_delay_ms(10);
			return 0;
			break;
		}
		rtos_time_delay_ms(1000);  /* /force sleep to wait the reply */
	}

	return 1;
}
static u8 usbh_comp_quectel_eg91_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata0 = (u8 *)"AT+qdbgcfg=\"tracecfg\",0,1";
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
		case QUECTEL_DONGLE_STATUS_IDLE:  /* for debug issue */
			while (usbh_dongle_ctx.acm_rx_busy == 0 && usbh_dongle_ctx.in_detach == 0) {
				rtos_time_delay_ms(10);
			}
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata0, usbh_comp_strlen(pdata0));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata0) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata0) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata1, usbh_comp_strlen(pdata1));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SEARCH: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata2, usbh_comp_strlen(pdata2));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_SET_APN: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata3, usbh_comp_strlen(pdata3));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		state ++;
		break;
		case QUECTEL_DONGLE_STATUS_DIAG: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata4, usbh_comp_strlen(pdata4));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		idx = 0;
		do {
			idx++;
			if ((usbh_dongle_ctx.quectel.cereg_ready != 0) || (usbh_dongle_ctx.in_detach != 0)) {
				break;
			}
			rtos_time_delay_ms(1000);
		} while (idx < 10);
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		if (usbh_dongle_ctx.quectel.cereg_ready != 0) {
			/* EG 91 use DHCP to get mac */
			state = QUECTEL_DONGLE_STATUS_GET_MAC;
		}
		break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS: {
			for (idx = 0; idx < 150 && usbh_dongle_ctx.in_detach == 0; idx++) {
				rtos_time_delay_ms(100);
			}
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata5, usbh_comp_strlen(pdata5));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata5) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata5) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		idx = 0;
		do {
			idx++;
			if ((usbh_dongle_ctx.quectel.ip_ready != 0) || (usbh_dongle_ctx.in_detach != 0)) {
				break;
			}
			rtos_time_delay_ms(1000);
		} while (idx < 10);
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		if (usbh_dongle_ctx.quectel.ip_ready != 0) {
			state = QUECTEL_DONGLE_STATUS_GET_MAC;
		}
		break;
		case QUECTEL_DONGLE_STATUS_GET_MAC: {
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata6, usbh_comp_strlen(pdata6));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata6) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata6) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
		}
		idx = 0;
		do {
			idx++;
			if ((usbh_dongle_ctx.quectel.mac_ready != 0) || (usbh_dongle_ctx.in_detach != 0)) {
				break;
			}
			rtos_time_delay_ms(1000);
		} while (idx < 10);
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		if (usbh_dongle_ctx.quectel.mac_ready != 0) {
			state = QUECTEL_DONGLE_STATUS_MAX;
		}
		break;
		case QUECTEL_DONGLE_STATUS_MAX:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Send request finish!\n");
			rtos_time_delay_ms(10);
			return 0;
			break;
		}
		rtos_time_delay_ms(1000);  /* /force sleep to wait the reply */
	}
	return 1;
}

/*
 * LE271 (Fibocom) ECM dial helper.
 * Sends a single AT command (auto-appends CRLF) then polls *ready_flag for up to timeout_ms.
 * Caller is responsible for clearing the flag before invoking this so we don't latch on stale data.
 * Returns 0 on flag set, -1 on tx failure or timeout.
 */
static int usbh_comp_fibocom_send_wait(const char *cmd, volatile u8 *ready_flag, u32 timeout_ms)
{
	u32 cmd_len = usbh_comp_strlen((u8 *)cmd);
	u32 elapsed = 0;

	if (cmd_len + 2 > CONFIG_USBH_COMP_PBUF_MAX_LEN) {
		return -1;
	}

	memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
	memcpy(usbh_comp_acm_tx_buf, cmd, cmd_len);
	usbh_comp_acm_tx_buf[cmd_len + 0] = 0x0D;
	usbh_comp_acm_tx_buf[cmd_len + 1] = 0x0A;
	if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
		return -1;
	}

	while (elapsed < timeout_ms) {
		if (usbh_dongle_ctx.in_detach != 0) {
			return -1;
		}
		if (ready_flag == NULL || *ready_flag) {
			return 0;
		}
		rtos_time_delay_ms(100);
		elapsed += 100;
	}
	return -1;
}

/*
 * LE271 CAT1 ECM dial flow - implementation of Fibocom LE271_CAT1 (Step1..Step13).
 *
 * Returns 0 once the cellular side has an IPv4 (caller can then proceed to ECM link / DHCP).
 * Returns 1 on unrecoverable failure (wrong USB mode triggers AT+CFUN=15 and we fall through
 * to the hot-plug detach path which re-runs the flow on re-enumeration).
 */
static u8 usbh_comp_fibocom_ctrl(void)
{
	u8 heart_beat = 0;
	int retry;
	int reg_elapsed_ms;
	int d;
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
				if (usbh_comp_fibocom_send_wait((const char *)pdata0, &usbh_dongle_ctx.fibocom.at_ok, 1000) == 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 AT no reply, retry %d\n", retry + 1);
				rtos_time_delay_ms(2000);
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
			if (usbh_comp_fibocom_send_wait((const char *)pdata1, &usbh_dongle_ctx.fibocom.at_ok, 2000) != 0) {
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
			if (usbh_comp_fibocom_send_wait((const char *)pdata2, &usbh_dongle_ctx.fibocom.usbmode_seen, 3000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 GTUSBMODE? no reply\n");
				return 1;
			}
			if (usbh_dongle_ctx.fibocom.usbmode_is_ecm == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 not in ECM mode, switch and reboot\n");
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata3, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				/* USB will detach; let example_usbh_comp_hotplug_thread re-init and re-enter this flow. */
				rtos_time_delay_ms(1000);
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMEI:
			/* Step4: IMEI - non-fatal, continue even on timeout (info-only step). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step4: AT+CGSN?\n");
			usbh_dongle_ctx.fibocom.cgsn_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata5, &usbh_dongle_ctx.fibocom.cgsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_SN:
			/* Step5: device serial number (informational). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step5: AT+CFSN?\n");
			usbh_dongle_ctx.fibocom.cfsn_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata6, &usbh_dongle_ctx.fibocom.cfsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CFUN:
			/* Step6: functional state (informational, expect +CFUN: 1). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step6: AT+CFUN?\n");
			usbh_dongle_ctx.fibocom.cfun_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata7, &usbh_dongle_ctx.fibocom.cfun_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CGMR:
			/* Step7: firmware version (informational). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step7: AT+CGMR?\n");
			usbh_dongle_ctx.fibocom.cgmr_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata8, &usbh_dongle_ctx.fibocom.cgmr_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIM:
			/* Step8: SIM detection - poll until +CPIN: READY (cap at ~30s to avoid hangs). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step8: AT+CPIN? (wait SIM)\n");
			for (retry = 0; retry < 15 && (usbh_dongle_ctx.in_detach == 0); retry++) {
				usbh_dongle_ctx.fibocom.cpin_ready = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata9, &usbh_dongle_ctx.fibocom.cpin_ready, 2000);
				if (usbh_dongle_ctx.fibocom.cpin_ready != 0) {
					break;
				}
				/* inter-retry gap: 20 * 100 ms = 2 s total; exits early on detach */
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
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
				(void)usbh_comp_fibocom_send_wait((const char *)pdata10, &usbh_dongle_ctx.fibocom.csq_seen, 2000);
				if (usbh_dongle_ctx.fibocom.csq_has_signal != 0) {
					break;
				}
				/* inter-retry gap: 20 * 100 ms = 2 s total; exits early on detach */
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
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
			usbh_dongle_ctx.fibocom.cimi_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata11, &usbh_dongle_ctx.fibocom.at_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_PS_REG:
			/* Step11: PS-domain registration via +CGREG. PDF: continuous query timeout 90s -> reset module. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step11: AT+CGREG? (wait PS attach)\n");
			reg_elapsed_ms = 0;
			while (reg_elapsed_ms < 90000 && (usbh_dongle_ctx.in_detach == 0)) {
				usbh_dongle_ctx.fibocom.cgreg_seen = 0;
				usbh_dongle_ctx.fibocom.ps_registered = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata12, &usbh_dongle_ctx.fibocom.cgreg_seen, 2000);
				if (usbh_dongle_ctx.fibocom.ps_registered != 0) {
					break;
				}
				/* inter-query gap: 20 * 100 ms = 2 s total; exits early on detach */
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
				reg_elapsed_ms += 4000;
			}
			if ((usbh_dongle_ctx.fibocom.ps_registered == 0) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 PS attach timeout, reset module\n");
				if (usbh_dongle_ctx.in_detach == 0) {
					/* Device still attached: issue soft-reset and wait 1 s for it to
					 * begin detaching before returning (1 s total guard delay). */
					usbh_dongle_ctx.fibocom.at_ok = 0;
					(void)usbh_comp_fibocom_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
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
				(void)usbh_comp_fibocom_send_wait((const char *)pdata13, &usbh_dongle_ctx.fibocom.gtrndis_seen, 3000);
				if (usbh_dongle_ctx.fibocom.gtrndis_has_ip != 0) {
					state = FIBOCOM_DONGLE_STATUS_DONE;
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step13: AT+GTRNDIS=1,1 (dial)\n");
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata14, &usbh_dongle_ctx.fibocom.at_ok, 5000);
				rtos_time_delay_ms(1000);
			}
			if ((state != FIBOCOM_DONGLE_STATUS_DONE) || (usbh_dongle_ctx.in_detach == 1)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 dial loop exhausted\n");
				return 1;
			}
			break;
		case FIBOCOM_DONGLE_STATUS_DONE:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 has IP, dial OK!\n");
			rtos_time_delay_ms(10);
			return 0;
		}
	}

	return 1;
}

static int usbh_comp_dongle_netif_init(void)
{
	char *pipaddr;
	char *pgw;
	char *pnetmask;
	char *pdnsserver;
	ip4_addr_t ipaddr, gw, netmask, dnsserver;
	int ret;

	pipaddr = (char *)usbh_comp_dongle_get_netinfo((u8 *)"ip");
	if (NULL == pipaddr) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get ip failed !\n");
		return -1;
	} else {
		ret = inet_aton(pipaddr, &ipaddr);
		if (ret == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Inet_aton ip failed !\n");
			return -1;
		}
		netif_set_ipaddr(&xnetif[NET_IF_NUM - 1], &ipaddr);
	}

	pnetmask = (char *)usbh_comp_dongle_get_netinfo((u8 *)"mask");
	if (NULL == pnetmask) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get mask failed !\n");
		return -1;
	} else {
		ret = inet_aton(pnetmask, &netmask);
		if (ret == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Inet_aton netmask failed !\n");
			return -1;
		}
		netif_set_netmask(&xnetif[NET_IF_NUM - 1], &netmask);
	}

	pgw = (char *)usbh_comp_dongle_get_netinfo((u8 *)"gw");
	if (NULL == pgw) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get gw failed !\n");
		return -1;
	} else {
		ret = inet_aton(pgw, &gw);
		if (ret == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Inet_aton gw failed !\n");
			return -1;
		}
		netif_set_gw(&xnetif[NET_IF_NUM - 1], &gw);
	}

	pdnsserver = (char *)usbh_comp_dongle_get_netinfo((u8 *)"dns");
	if (NULL == pdnsserver) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get dns failed !\n");
		return -1;
	} else {
		ret = inet_aton(pdnsserver, &dnsserver);
		if (ret == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Inet_aton dns failed !\n");
			return -1;
		}
		dns_setserver(0, &dnsserver);
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Run %s ok. \n", __func__);
	return 0;
}

/*
	for different dongle, do the prepare for the ecm
	default:no nothing
	EG915/EG91/xx send cmd to AT port
*/
static int usbh_comp_dongle_diag_cmd(void)
{
	u16 pid = usbh_dongle_ctx.pid;
	u16 vid = usbh_dongle_ctx.vid;

	if (USB_VID == vid) {
		return 0;
	} else if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) {
		/* should check the status finish */
		if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID) {
			return usbh_comp_quectel_eg915_ctrl();
		} else if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID) {
			return usbh_comp_quectel_eg91_ctrl();
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the pid(%d)\n", pid);
		}
	} else if ((USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) && (pid == USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID)) {
		return usbh_comp_fibocom_ctrl();
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the vid(%d)\n", vid);
	}

	return 1;
}

/**
  * @brief  Composite ECM bulk receive callback
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int usbh_comp_ecm_rxdata(u8 *buf, u32 len)
{
	netif_adapter_usb_eth_recv(buf, len);
	return HAL_OK;
}

#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
#define CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO                  _PA_3

/**
  * @brief  Configure the device power-control GPIO as a push-pull output.
  * @param  GPIO_Pin: Pin wired to the USB device power switch (CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO).
  * @retval None
  */
static void usbh_comp_gpio_power_init(uint32_t GPIO_Pin)
{
	GPIO_InitTypeDef usbh_comp_gpio_initstruct_temp;
	usbh_comp_gpio_initstruct_temp.GPIO_Pin = GPIO_Pin;
	usbh_comp_gpio_initstruct_temp.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&usbh_comp_gpio_initstruct_temp);
}

/**
  * @brief  Drive the device power-control GPIO. Only the LSB is used:
  *         0 cuts power to the device, 1 restores it.
  * @param  level: Output level to write; masked to BIT0 (0 = off, 1 = on).
  * @retval None
  */
static void usbh_comp_gpio_trigger(u8 level)
{
	level = level & BIT0;
	GPIO_WriteBit(CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO, level);
}

/**
  * @brief  One-time init of the power-control GPIO. Configures the pin, then
  *         power-cycles the device once (off -> settle -> on) so it always
  *         starts from a known powered state. Guarded by a static flag so
  *         repeated calls are no-ops.
  * @retval None
  */
static void usbh_comp_gpio_init(void)
{
	static int init_ok = 0;
	if (init_ok == 0) {
		init_ok = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "[USB] gpio init\n");
		usbh_comp_gpio_power_init(CONFIG_USBH_COMP_USB_DEV_PWR_CTRL_GPIO);

		usbh_comp_gpio_trigger(0);      /* power off */
		usb_os_sleep_ms(20);        /* let the power rail discharge */
		usbh_comp_gpio_trigger(1);      /* power on */
	}
}
#endif

/**
  * @brief  Physically power-cycle the attached USB device via the power-control
  *         GPIO: drop power for 500 ms, then restore it. This forces a real
  *         detach/attach so the hot-plug re-enumeration path can be exercised.
  *         Only available when CONFIG_USBH_COMP_GPIO_POWER_CTRL is enabled.
  * @retval HAL_OK
  */
static u32 usbh_comp_hotplug_test(void)
{
#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
	usbh_comp_gpio_trigger(0);
	rtos_time_delay_ms(500);
	usbh_comp_gpio_trigger(1);
#else
	RTK_LOGS(TAG, RTK_LOG_WARN, "Not support\n");
#endif

	return HAL_OK;
}

/**
  * @brief  ACM bulk IN receive loop - lives forever across detach/re-attach.
  * @details The CDC-ACM driver only performs a bulk IN transfer when the
  *          application calls usbh_cdc_acm_receive(). This task keeps one
  *          receive armed at all times so AT-command responses are pulled in
  *          continuously; each completion is delivered via
  *          usbh_comp_acm_rxdata(), which then wakes this task to re-arm.
  *
  *          The outer loop waits for a device to be enumerated (in_detach == 0);
  *          during detach it polls until the next attach clears in_detach to 0
  *          again. The task is created once in the entry function and never
  *          self-deletes - no need to track a task handle.
  */
static void usbh_comp_acm_rx_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		/* Wait for a device to be enumerated */
		while (usbh_dongle_ctx.in_detach != 0) {
			rtos_time_delay_ms(10);
		}

		/* ACM RX loop for the current session */
		while (usbh_dongle_ctx.in_detach == 0) {
			if (usbh_cdc_acm_receive(usbh_comp_acm_rx_buf, CONFIG_USBH_COMP_PBUF_MAX_LEN) == HAL_OK) {
				/* Wait for completion (callback gives the sema).
				 * Use a timeout so detach is noticed promptly. */
				rtos_sema_take(usbh_comp_acm_rx_done_sema, 1000);
			} else {
				/* Pipe not ready yet: back off and retry. */
				rtos_time_delay_ms(10);
			}
		}
	}
}

static int usbh_comp_do_init(void)
{
	u32 wait_ms;

	/* Clear all dongle state from the previous session before re-init so that
	 * stale flags (ip_ready, cereg_ready, mac_ready, at_ok, ) cannot be seen
	 * by the new session's state machines or the RX callback. */
	memset(&usbh_dongle_ctx, 0, sizeof(usbh_dongle_ctx));
	/* Keep in_detach asserted while stack is coming up so acm_rx_thread
	 * stays parked and does not race an unattached pipe. */
	usbh_dongle_ctx.in_detach = 1;

	if (usbh_init(&usbh_cfg, &usbh_comp_usr_cb) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		return HAL_ERR_UNKNOWN;
	}
	usbh_cdc_acm_init(&usbh_comp_acm_cfg);
	usbh_cdc_ecm_init(&usbh_comp_ecm_cfg, &usbh_comp_ecm_priv);
	usbh_start();

	/* Wait for ECM enumeration, bounded to 30 s so a missing / bad device
	 * cannot hang this thread forever. */
	wait_ms = 0;
	for (;;) {
		if (usbh_cdc_ecm_usb_is_ready()) {
			break;
		}
		if (wait_ms >= 30000U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USB ecm ready timeout\n");
			return HAL_ERR_UNKNOWN;
		}
		rtos_time_delay_ms(1000);
		wait_ms += 1000U;
	}

	/* Cache vid/pid before diag so the RX callback can use usbh_dongle_ctx.vid
	 * without calling the getter on every received packet. */
	usbh_dongle_ctx.vid = usbh_cdc_ecm_get_device_vid();
	usbh_dongle_ctx.pid = usbh_cdc_ecm_get_device_pid();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Example Pid 0x%x/Vid 0x%x\n", usbh_dongle_ctx.pid, usbh_dongle_ctx.vid);

	/* Release acm_rx_thread now that a device is enumerated and the pipe
	 * is available; diag AT commands need the RX pump running. */
	usbh_dongle_ctx.in_detach = 0;

	/*
		prepare for ecm transfer:
		4G dongle, send AT cmd to the dongle before the network is ready
		other dongle, prepare for the ecm transfer
	*/
	if (usbh_comp_dongle_diag_cmd() == 0) {
		/* Only call prepare_done() on clean success (return 0 = ECM ready with IP).
		 * Any non-zero return - whether a reboot was triggered (DONGLE_DIAG_RET_REBOOT_PENDING)
		 * or a fatal AT error - means ECM is not ready; skip prepare_done(). */
		usbh_cdc_ecm_prepare_done();
		RTK_LOGS(TAG, RTK_LOG_INFO, "TRX start\n");
		return HAL_OK;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Dongle Diag Err\n");
	return HAL_ERR_UNKNOWN;
}

static void example_usbh_comp_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	u16 vid, pid;
	u32 link_down_log_cnt = 0;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	/* Wait for the USB stack to be ready before touching ECM APIs. During
	 * detach/re-init phases in_detach is 1 and the stack has no active
	 * pipes, so we must not call usbh_cdc_ecm_get_* helpers. */
	for (;;) {
		if (usbh_dongle_ctx.in_detach == 0 && usbh_cdc_ecm_usb_is_ready()) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB Init success!\n");
	for (;;) {
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

		if (USB_VID == vid) {  /* rtk */
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {  /* unlink -> link */
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				mac = (u8 *)usbh_cdc_ecm_process_mac_str();
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);  /* Set default gw to ether netif */
					usbh_dongle_ctx.dhcp_done = 1;
					ethernet_unplug = ETH_STATUS_INIT;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Fail\n");
				}
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {  /* link -> unlink */
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
			} else {
				if ((link_is_up == 0) && (++link_down_log_cnt % 10 == 1)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "ECM link is down (%u)\n", link_down_log_cnt);
				}
				rtos_time_delay_ms(1000);
			}
		} else if ((USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) && (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID)) {
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {  /* unlink -> link */
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				ethernet_unplug = ETH_STATUS_INIT;
				mac = usbh_dongle_ctx.quectel.mac;
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);  /* Set default gw to ether netif */
					usbh_dongle_ctx.dhcp_done = 1;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {  /* link -> unlink */
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else if ((USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) && (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID)) {  /* EG915 */
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Pid 0x%x/Vid 0x%x, EG915 mac\n", vid, pid);
				mac = (u8 *)usbh_cdc_ecm_process_mac_str();
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				memcpy(xnetif[NET_IF_NUM - 1].hwaddr, mac, 6);
				if (usbh_comp_dongle_netif_init() == 0) {
					ethernet_unplug = ETH_STATUS_INIT;
					netif_set_up(&xnetif[NET_IF_NUM - 1]);
					netif_set_link_up(&xnetif[NET_IF_NUM - 1]);
					netif_set_default(&xnetif[NET_IF_NUM - 1]);  /* choose the route path */
					usbh_dongle_ctx.dhcp_done = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link !!\n");
				}
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to unlink\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else if ((USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) && (pid == USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID)) {
			/* LE271: cellular dial done in usbh_comp_fibocom_ctrl(); ECM brings up the link
			   and the host obtains IPv4 via DHCP - same path as EG91. */
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271: Do DHCP\n");
				ethernet_unplug = ETH_STATUS_INIT;
				mac = (u8 *)usbh_cdc_ecm_process_mac_str();
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n",
						 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);
					usbh_dongle_ctx.dhcp_done = 1;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271: Switch to link\n");
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271: Switch to unlink\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else {
			if ((link_is_up == 0) && (++link_down_log_cnt % 10 == 1)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ECM link is down (%u)\n", link_down_log_cnt);
			}
			if (link_is_up != 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Error, the Pid 0x%x/Vid 0x%x can not match! \n", vid, pid);
			}
			rtos_time_delay_ms(1000);
		}
	}
	/* Not reached - the for(;;) above never breaks. */
}

#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
static void usbh_comp_ecm_save_data(char *pdata, unsigned int length)
{
#if CONFIG_USBH_COMP_ENABLE_DUMP_FILE
	static unsigned int psram_pos = 0;
	if (CONFIG_USBH_COMP_PSRAM_HEAP_SIZE_TEST >= psram_pos + length) {
		memcpy((void *)&dump_psRAMHeap[psram_pos], pdata, length);
		psram_pos += length;
	}
#else
	UNUSED(length);
	UNUSED(pdata);
#endif
}

static int usbh_comp_ecm_check_download(char *pdata, unsigned int length)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d\n", length);
	if (0 == length) {
		return 0;
	}
	usbh_comp_ecm_save_data(pdata, length);
	mbedtls_md5_update(&ctx, (const unsigned char *)pdata, length);
	return  0;
}

static void usbh_comp_ecm_write_flash(void)
{
#if CONFIG_USBH_COMP_ENABLE_DUMP_FILE
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash start\n");
	FLASH_WriteStream(0x100000, CONFIG_USBH_COMP_PSRAM_HEAP_SIZE_TEST, dump_psRAMHeap);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash done\n");
#endif
}

static void example_usbh_comp_download_thread(void *param)
{
	int server_fd = -1;
	u8 heart_beat = 0;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	u32 resource_size = 0;
	u32 content_len = 0;
	u32 max = 0;
	u8 hh = 0;
	unsigned char output[8 * CONFIG_USBH_COMP_MD5_CHECK_BUFFER_LEN];
	int pos = 0, read_size = 0,  header_removed = 0;
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter download example\n");
	memset(output, 0x00, 8 * CONFIG_USBH_COMP_MD5_CHECK_BUFFER_LEN);

	while (0 == usbh_dongle_ctx.dhcp_done) {
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
		int recv_timeout_ms = CONFIG_USBH_COMP_RECV_TO;
		/* lwip 1.5.0 */
		struct timeval recv_timeout;
		recv_timeout.tv_sec = recv_timeout_ms / 1000;
		recv_timeout.tv_usec = recv_timeout_ms % 1000 * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
		/* lwip 1.4.1 */
		/* setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms)); */
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Server_fd=%d\n", server_fd);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CONFIG_USBH_COMP_SERVER_PORT);

	/* Support CONFIG_USBH_COMP_SERVER_HOST in IP or domain name */
	server_host = gethostbyname(CONFIG_USBH_COMP_SERVER_HOST);
	if (server_host != NULL) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Server host\n");
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Will do connect %s\n", CONFIG_USBH_COMP_SERVER_HOST);
	if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Connect success\n");
		sprintf((char *)dl_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", CONFIG_USBH_COMP_RESOURCE, CONFIG_USBH_COMP_SERVER_HOST);
		max = usbh_comp_strlen((u8 *)dl_buf);


		write(server_fd, (char const *)dl_buf, usbh_comp_strlen((u8 *)dl_buf));
		RTK_LOGS(TAG, RTK_LOG_INFO, "Will call read\n");
		mbedtls_md5_init(&ctx);
		mbedtls_md5_starts(&ctx);

		while (((content_len == 0) || (resource_size < content_len)) /**/
			   && ((read_size = read(server_fd, dl_buf + pos, CONFIG_USBH_COMP_BUFFER_SIZE - pos)) > 0)) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				dl_buf[pos] = 0;
				header = strstr((char const *)dl_buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + usbh_comp_strlen((u8 *)"\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "HTTP Header: %s\n", dl_buf);

					/* Remove header size to get first read size of data from body head */
					read_size = pos - ((unsigned char *) body - dl_buf);
					usbh_comp_ecm_check_download(body, read_size);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Body=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", body[0], body[1], body[2], body[3], body[4], body[5], body[6], body[7]);
					pos = 0;
					content_len_pos = strstr((char const *)dl_buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += usbh_comp_strlen((u8 *)"Content-Length: ");
						*(char *)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
						RTK_LOGS(TAG, RTK_LOG_INFO, "Content len: %d\n", content_len);
					}
				} else {
					if (pos >= CONFIG_USBH_COMP_BUFFER_SIZE) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "HTTP header pos=%d-%s\n", pos, dl_buf);
						goto exit;
					}
					continue;
				}
			} else {
				usbh_comp_ecm_check_download((char *)(dl_buf + pos), (unsigned int)read_size);
			}

			resource_size += read_size;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Read resource %dB/rx=%d/total=%d\n",
					 read_size, resource_size, content_len);
		}

		usbh_comp_ecm_write_flash();
		mbedtls_md5_finish(&ctx, output);
		mbedtls_md5_free(&ctx);

		RTK_LOGS(TAG, RTK_LOG_INFO, "mbedtls_md5_finish md5\n");
		for (hh = 0; hh < CONFIG_USBH_COMP_MD5_CHECK_BUFFER_LEN; hh++) {
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

static void example_usbh_comp_init_task(void *param)
{
	int ret;

	UNUSED(param);
	ret = usbh_comp_do_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Initial USB bring-up failed; check hardware / cellular\n");
	}
	/* Bootstrap task complete: self-delete. */
	rtos_task_delete(NULL);
}

static void usbh_comp_init(void)
{
	int status;
	status = rtos_task_create(&usbh_comp_link_check_task,
							  "example_usbh_comp_link_change_thread",
							  example_usbh_comp_link_change_thread, NULL,
							  CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE, CONFIG_USBH_COMP_LINK_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

	status = rtos_task_create(&usbh_comp_ecm_init_task, "example_usbh_comp_init_task",
							  example_usbh_comp_init_task, NULL,
							  CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE, CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init task fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB init start\r\n");
}

#if CONFIG_USBH_COMP_MEM_CHECK
/* extern void vPortGetTaskHeapInfo(void); */
/* extern void mmeory_array_dump(void); */
/**
 * @brief  Memory leak stress test: repeatedly deinit / re-init the USB stack.
 * @note   Worker tasks (link_check, init_task) are only created on the first
 *         iteration via usbh_comp_init(); subsequent iterations only reset
 *         the USB stack itself via do_init() so we don't leak task handles.
 */
static void example_usbh_comp_mem_check_thread(void *param)
{
	int loop = 0;
	int ret;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "[test] mem_check start\n");

	/* First iteration: full init (creates link_check + init_task worker threads).
	 * Later iterations only deinit/reinit the USB stack; the worker threads
	 * are always-alive and are reused across cycles to avoid leaking a task
	 * handle per iteration. */
	usbh_comp_init();

	for (;;) {
		rtos_time_delay_ms(10000);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop create %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());

		/* Tear down USB stack (worker tasks stay alive; they'll wait on the
		 * next ready state after re-init). */
		usbh_dongle_ctx.in_detach = 1;
		usbh_stop();
		usbh_cdc_acm_deinit();
		usbh_cdc_ecm_deinit();
		usbh_deinit();

		rtos_time_delay_ms(3000);
		/* mmeory_array_dump(); */
		/* vPortGetTaskHeapInfo(); */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop delete %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());

		/* Re-init the USB stack; do_init() clears in_detach on success so
		 * RX pump can resume. */
		ret = usbh_comp_do_init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "mem_check loop %d re-init failed, stopping\n", loop);
			break;
		}
		loop++;
	}
}
#endif

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
/**
 * @brief  Hot-plug worker thread. Blocks on detach sema, tears down the USB
 *         stack, re-initializes it. do_init() manages in_detach on its own,
 *         releasing it only after a device is enumerated so the RX pump does
 *         not race an unattached pipe.
 */
static void example_usbh_comp_hotplug_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_comp_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			/* Signal RX pump to park before we tear down the pipes */
			usbh_dongle_ctx.in_detach = 1;
			usbh_stop();
			usbh_cdc_acm_deinit();
			usbh_cdc_ecm_deinit();
			usbh_deinit();
			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			/* do_init() clears in_detach on successful enumeration; on
			 * failure it stays asserted so RX pump stays parked. */
			if (usbh_comp_do_init() != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Hot-plug re-init failed; waiting for next detach\n");
			}
		}
	}
}
#endif

/**
  * @brief  Shell command: trigger a physical hot-plug by power-cycling the
  *         USB device via GPIO. Requires CONFIG_USBH_COMP_GPIO_POWER_CTRL.
  * @param  argc: Argument count (unused).
  * @param  argv: Argument vector (unused).
  * @retval HAL_OK
  */
static u32 usbh_comp_ecm_hotplug_cmd(u16 argc, u8 *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	return usbh_comp_hotplug_test();
}

/* Exported functions --------------------------------------------------------*/
#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ecm_send_data(buf, len, block);
}

void example_usbh_composite_cdc_acm_ecm(void)
{
	int ret;
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	rtos_task_t hot_plug_task;
#endif
#if CONFIG_USBH_COMP_MEM_CHECK
	rtos_task_t memory_monitor_task;
#endif
#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH Composite ECM demo start\n");

	rltk_usb_eth_init();

#if CONFIG_USBH_COMP_GPIO_POWER_CTRL
	usbh_comp_gpio_init();
#endif

	/* ---------------------------------------------------------------------
	 * Resource creation with cascaded goto-cleanup on failure.
	 *
	 * Dependency order (created first - cleaned last):
	 *   1. acm_rx_done_sema     - used by acm_rx_thread (no NULL guard)
	 *   2. acm_send_sema        - used by acm_transmit (has NULL guard)
	 *   3. acm_rx_task          - pumps AT-command responses
	 *   4. detach_sema          - cb_detach() gives, hotplug_thread takes
	 *                             (only needed when HOT_PLUG_TEST is enabled)
	 *   5. hot_plug_task / mem_check_task (mutually exclusive)
	 *   6. usbh_comp_init()     - starts link/init worker threads
	 *   7. download_task        - optional, failure is non-fatal
	 *
	 * Both HOT_PLUG_TEST and MEM_CHECK invoke usbh_comp_init() - do_init() -
	 * dongle_diag_cmd() - acm_transmit(), so ACM infrastructure (semas + rx
	 * task) is mandatory in every build config, not just the #else branch.
	 * ------------------------------------------------------------------- */

	ret = rtos_sema_create(&usbh_comp_acm_rx_done_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm rx done sema fail\n");
		return;
	}

	ret = rtos_sema_create(&usbh_comp_acm_send_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm send sema fail\n");
		goto err_rx_done_sema;
	}

	ret = rtos_task_create(&usbh_comp_acm_rx_task, "usbh_comp_acm_rx_thread",
						   usbh_comp_acm_rx_thread, NULL,
						   CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm rx task fail\n");
		goto err_send_sema;
	}

#if CONFIG_USBH_COMP_HOT_PLUG_TEST
	ret = rtos_sema_create(&usbh_comp_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto err_rx_task;
	}

	ret = rtos_task_create(&hot_plug_task, "example_usbh_comp_hotplug_thread",
						   example_usbh_comp_hotplug_thread, NULL,
						   CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
		goto err_detach_sema;
	}
#endif

#if CONFIG_USBH_COMP_MEM_CHECK
	ret = rtos_task_create(&memory_monitor_task, "example_usbh_comp_mem_check_thread",
						   example_usbh_comp_mem_check_thread, NULL,
						   CONFIG_USBH_COMP_MEM_CHECK_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_MEM_CHECK_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create mem check thread fail\n");
		goto err_rx_task;
	}
	/* mem_check_thread drives its own usbh_comp_init() loop */
#else
	usbh_comp_init();

#if CONFIG_USBH_COMP_ENABLE_REMOTE_FILE_DOWNLOAD
	/* Download thread failure is tolerable: ECM still works, just no HTTP demo */
	ret = rtos_task_create(&download_task, "example_usbh_comp_download_thread",
						   example_usbh_comp_download_thread, NULL,
						   CONFIG_USBH_COMP_DOWNLOAD_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_DOWNLOAD_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create download thread fail\n");
	}
#endif
#endif /* CONFIG_USBH_COMP_MEM_CHECK */

	return;

	/* -------- error unwind path (labels in reverse construction order) --
	 * Each label is only referenced from build configs that reach the
	 * corresponding creation step; guard the ones that might be unused so
	 * every build config compiles cleanly. */
#if CONFIG_USBH_COMP_HOT_PLUG_TEST
err_detach_sema:
	rtos_sema_delete(usbh_comp_detach_sema);
#endif
#if CONFIG_USBH_COMP_HOT_PLUG_TEST || CONFIG_USBH_COMP_MEM_CHECK
err_rx_task:
	/* acm_rx_thread is always-alive; force-delete since it has no exit path */
	rtos_task_delete(usbh_comp_acm_rx_task);
#endif
err_send_sema:
	rtos_sema_delete(usbh_comp_acm_send_sema);
err_rx_done_sema:
	rtos_sema_delete(usbh_comp_acm_rx_done_sema);
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_dongle_atcmd[] = {
	{"ecm_cmd", usbh_comp_acm_cmd_test},
	{"ecm_hotplug", usbh_comp_ecm_hotplug_cmd},
};

#else
#error "No Lwip USB Ethernet Configuration"
#endif
