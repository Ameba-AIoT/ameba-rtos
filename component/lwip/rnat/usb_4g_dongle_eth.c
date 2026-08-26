/**
 * @file usb_4g_dongle_eth.c
 * @brief Temporary USB host CDC-ACM + CDC-ECM (4G dongle) network management for R-NAT.
 */

#include "platform_autoconf.h"

#if defined(CONFIG_LWIP_USB_ETHERNET) && defined(CONFIG_RNAT) && defined(CONFIG_USBH_CDC_ACM_4G_DONGLE)

#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "usbh_cdc_acm.h"
#include "usbh_cdc_ecm.h"

#ifndef TAG
#define TAG "USB-4G"
#endif

/* ========================================================================== */
/*                              Config                                        */
/* ========================================================================== */

#define CONFIG_USBH_COMP_PBUF_MAX_LEN           (1024)
#define CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR   (128)

#define CONFIG_USBH_COMP_INIT_THREAD_PRIORITY    5U
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY 6U
#define CONFIG_USBH_COMP_LINK_THREAD_PRIORITY    3U

#define CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE     (1024U)
#define CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE  (1024U)
#define CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE     (1600U)

/* ========================================================================== */
/*                              Types                                         */
/* ========================================================================== */

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

typedef enum {
	SIMCOM_DONGLE_STATUS_AT_PROBE = 0U,
	SIMCOM_DONGLE_STATUS_ATE0,
	SIMCOM_DONGLE_STATUS_WAIT_SIM,
	SIMCOM_DONGLE_STATUS_WAIT_SIGNAL,
	SIMCOM_DONGLE_STATUS_QUERY_OPERATOR,
	SIMCOM_DONGLE_STATUS_WAIT_PS_ATTACH,
	SIMCOM_DONGLE_STATUS_ACTIVATE_PDP,
	SIMCOM_DONGLE_STATUS_NETOPEN,
	SIMCOM_DONGLE_STATUS_QUERY_USBNETIP,
	SIMCOM_DONGLE_STATUS_DONE,
} simcom_dongle_state_t;

typedef struct {
	u8 ip[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 mask[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 gw[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
	u8 dns[CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR];
} network_info_t;

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

		struct {
			volatile u8 at_ok;
			volatile u8 cpin_ready;
			volatile u8 csq_seen;
			volatile u8 csq_has_signal;
			volatile u8 cops_ok;
			volatile u8 cgatt_seen;
			volatile u8 cgatt_attached;
			volatile u8 netopen_ok;
			volatile u8 netopen_query_seen;
			volatile u8 netopen_query_state;
			volatile u8 usbnetip_seen;
			volatile u8 usbnetip_mode;
		} simcom;
	};
	u16 vid;
	u16 pid;
	volatile u8 in_detach;
	u8 dhcp_done;
} dongle_ctx_t;

/* ========================================================================== */
/*               Forward declarations (only for struct initializers)          */
/* ========================================================================== */

extern struct netif *pnetif_usb_eth;
extern void rltk_usb_eth_init(void);
extern void netif_adapter_usb_eth_recv(u8 *buf, u32 len);

static int usbh_comp_detach(void);
static int usbh_comp_ecm_rxdata(u8 *buf, u32 len);
static int usbh_comp_acm_rxdata(u8 *buf, u32 len, u8 status);
static int usbh_comp_acm_transmit_cb(u8 status);
static void usbh_comp_acm_rx_thread(void *param);
static int usbh_comp_cb_process(usb_host_t *host, u8 msg);
static int usbh_comp_cb_device_check(usb_host_t *host, u8 cfg_max);

/* ========================================================================== */
/*                              Variables                                     */
/* ========================================================================== */

static rtos_sema_t usbh_comp_detach_sema;
static rtos_task_t usbh_comp_link_check_task;
static rtos_task_t usbh_comp_ecm_init_task;
static rtos_task_t usbh_comp_acm_rx_task;

static u8 usbh_comp_acm_tx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 usbh_comp_acm_rx_buf[CONFIG_USBH_COMP_PBUF_MAX_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));

static rtos_sema_t usbh_comp_acm_rx_done_sema;
static rtos_sema_t usbh_comp_acm_send_sema;

static usbh_cdc_acm_param_t usbh_comp_dongle_array[] = {
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID,        USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID,    2},
	{USBH_CDC_ACM_QUECTEL_DONGLE_VID,        USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID,     2},
	{USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID,  USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID,    2},
	{USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID, USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID,   2},
	{0, 0, 0},
};

static dongle_ctx_t usbh_dongle_ctx;

static const usbh_cdc_ecm_priv_data_t usbh_comp_ecm_priv = {
	NULL,
	NULL,
	0,
};

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 1792U,
	.main_task_priority = CONFIG_USBH_COMP_INIT_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.class_num = 2U,
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

static const usbh_cdc_acm_cb_t usbh_comp_acm_cfg = {
	.receive  = usbh_comp_acm_rxdata,
	.transmit = usbh_comp_acm_transmit_cb,
	.priv     = usbh_comp_dongle_array,
};

static const usbh_cdc_ecm_state_cb_t usbh_comp_ecm_cfg = {
	.bulk_received = usbh_comp_ecm_rxdata,
	.detach        = usbh_comp_detach,
};

static const usbh_user_cb_t usbh_comp_usr_cb = {
	.process  = usbh_comp_cb_process,
	.validate = usbh_comp_cb_device_check,
};

static void (*g_usb_eth_link_cb)(int link_up) = NULL;

/* ========================================================================== */
/*                          Helper Utilities                                  */
/* ========================================================================== */

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

	if (pbuf == NULL) {
		return;
	}
	if (pbuf[0] == '"') {
		pbuf++;
	}
	len = usbh_comp_strlen(pbuf);
	if (len > 0 && pbuf[len - 1] == '"') {
		pbuf[len - 1] = 0;
	}
	len = usbh_comp_strlen(pbuf);
	if (len >= CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR) {
		len = CONFIG_USBH_COMP_NETWORK_INFO_MAX_STR - 1;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type(%s)=[%s(%d)]\n", name, pbuf, len);

	if (_strcmp(pname, "gw") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.gw, pbuf, len);
		usbh_dongle_ctx.quectel.network.gw[len] = '\0';
	} else if (_strcmp(pname, "ip") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.ip, pbuf, len);
		usbh_dongle_ctx.quectel.network.ip[len] = '\0';
	} else if (_strcmp(pname, "mask") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.mask, pbuf, len);
		usbh_dongle_ctx.quectel.network.mask[len] = '\0';
	} else if (_strcmp(pname, "dns") == 0) {
		memcpy(usbh_dongle_ctx.quectel.network.dns, pbuf, len);
		usbh_dongle_ctx.quectel.network.dns[len] = '\0';
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name\n");
	}
}

static u8 *usbh_comp_dongle_get_netinfo(u8 *name)
{
	const char *pname = (const char *)name;

	if (_strcmp(pname, "gw") == 0) {
		return usbh_dongle_ctx.quectel.network.gw;
	}
	if (_strcmp(pname, "ip") == 0) {
		return usbh_dongle_ctx.quectel.network.ip;
	}
	if (_strcmp(pname, "mask") == 0) {
		return usbh_dongle_ctx.quectel.network.mask;
	}
	if (_strcmp(pname, "dns") == 0) {
		return usbh_dongle_ctx.quectel.network.dns;
	}
	return NULL;
}

/* ========================================================================== */
/*                          USB Callbacks                                     */
/* ========================================================================== */

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

static int usbh_comp_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

static int usbh_comp_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	if (usbh_dongle_ctx.in_detach == 0U) {
		usbh_dongle_ctx.in_detach = 1U;
		rtos_sema_give(usbh_comp_detach_sema);
	}
	return HAL_OK;
}

static int usbh_comp_ecm_rxdata(u8 *buf, u32 len)
{
	netif_adapter_usb_eth_recv(buf, len);
	return HAL_OK;
}

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

static int usbh_comp_acm_transmit(u8 *buf, u32 len)
{
	int ret = usbh_cdc_acm_transmit(buf, len);
	if (ret == HAL_OK) {
		if (usbh_comp_acm_send_sema != NULL) {
			rtos_sema_take(usbh_comp_acm_send_sema, 1000);
		}
	}
	return ret;
}

static int usbh_comp_acm_rxdata(u8 *pbuf, u32 len, u8 status)
{
	u32 i;
	u16 vid = usbh_dongle_ctx.vid;

	UNUSED(status);

	RTK_LOGS(TAG, RTK_LOG_INFO, "ACM len(%u)\n", len);

	if (pbuf == NULL || len == 0) {
		goto done;
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
			break;
		}
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");

	if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) {
		if (strstr((char *)pbuf, "+CEREG")) {
			char *buff = (char *)pbuf;
			char *p;
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			if (p != NULL && (*p == '1' || *p == '5')) {
				usbh_dongle_ctx.quectel.cereg_ready = 1;
			}
		}
		if (strstr((char *)pbuf, "+WWANINFO")) {
			char *buff = (char *)pbuf;
			char *p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"ip");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"mask");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
			usbh_dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
		if (strstr((char *)pbuf, "+QNETIFMAC")) {
			char *buff = (char *)pbuf;
			char *p = strsep(&buff, "\"");
			p = strsep(&buff, "\"");
			if (p != NULL) {
				for (i = 0; i < 6; i++) {
					usbh_dongle_ctx.quectel.mac[i] =
						usbh_comp_hex_to_char(p[3 * i]) * 16 + usbh_comp_hex_to_char(p[3 * i + 1]);
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
						 usbh_dongle_ctx.quectel.mac[0], usbh_dongle_ctx.quectel.mac[1],
						 usbh_dongle_ctx.quectel.mac[2], usbh_dongle_ctx.quectel.mac[3],
						 usbh_dongle_ctx.quectel.mac[4], usbh_dongle_ctx.quectel.mac[5]);
				usbh_dongle_ctx.quectel.mac_ready = 1;
			}
		}
		if (strstr((char *)pbuf, "+CGCONTRDP")) {
			char *buff = (char *)pbuf;
			u8 *tail;
			char *p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			if (p != NULL) {
				u8 dotcount = 0;
				u8 ptmp[100];
				u32 plen = strlen(p);
				if (plen >= sizeof(ptmp)) {
					plen = sizeof(ptmp) - 1;
				}
				memset(ptmp, 0, sizeof(ptmp));
				memcpy(ptmp, p, plen);
				tail = ptmp;
				for (;;) {
					if (*tail == '.') {
						dotcount++;
						if (dotcount == 4) {
							*tail = 0;
							tail++;
							break;
						}
					} else if (*tail == 0) {
						break;
					}
					tail++;
				}
				usbh_comp_dongle_set_netinfo(ptmp, (u8 *)"ip");
				usbh_comp_dongle_set_netinfo(tail, (u8 *)"mask");
			}
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			usbh_comp_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
			usbh_dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
	} else if (USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid) {
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
		if (strstr((char *)pbuf, "460")) {
			usbh_dongle_ctx.fibocom.cimi_ok = 1;
		}
		if (strstr((char *)pbuf, "+CGREG:")) {
			char *buff = (char *)strstr((char *)pbuf, "+CGREG:");
			char *p = strsep(&buff, ":");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",\r\n");
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
			if (*p == '0' && (*(p + 1) == '\r' || *(p + 1) == '\n' || *(p + 1) == 0)) {
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 0;
			} else {
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 1;
			}
			usbh_dongle_ctx.fibocom.gtrndis_seen = 1;
		}
		if (strstr((char *)pbuf, "OK")) {
			usbh_dongle_ctx.fibocom.at_ok = 1;
		}
	} else if (USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID == vid) {
		if (strstr((char *)pbuf, "+CPIN: READY")) {
			usbh_dongle_ctx.simcom.cpin_ready = 1;
		}
		if (strstr((char *)pbuf, "+CSQ:")) {
			char *p = strstr((char *)pbuf, "+CSQ:") + sizeof("+CSQ:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.csq_has_signal = (atoi(p) == 99) ? 0 : 1;
			usbh_dongle_ctx.simcom.csq_seen = 1;
		}
		if (strstr((char *)pbuf, "+COPS:")) {
			usbh_dongle_ctx.simcom.cops_ok = 1;
		}
		if (strstr((char *)pbuf, "+CGATT:")) {
			char *p = strstr((char *)pbuf, "+CGATT:") + sizeof("+CGATT:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.cgatt_attached = (*p == '1') ? 1 : 0;
			usbh_dongle_ctx.simcom.cgatt_seen = 1;
		}
		if (strstr((char *)pbuf, "+NETOPEN:")) {
			char *p = strstr((char *)pbuf, "+NETOPEN:") + sizeof("+NETOPEN:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.netopen_query_state = (*p == '1') ? 1U : 0U;
			usbh_dongle_ctx.simcom.netopen_query_seen  = 1;
			if (*p == '0') {
				usbh_dongle_ctx.simcom.netopen_ok = 1;
			}
		}
		if (strstr((char *)pbuf, "+IP ERROR: Network is already opened")) {
			usbh_dongle_ctx.simcom.netopen_ok = 1;
		}
		if (strstr((char *)pbuf, "+USBNETIP:")) {
			char *p = strstr((char *)pbuf, "+USBNETIP:") + sizeof("+USBNETIP:") - 1;
			while (*p == ' ') {
				p++;
			}
			usbh_dongle_ctx.simcom.usbnetip_mode = (*p == '1') ? 1 : 0;
			usbh_dongle_ctx.simcom.usbnetip_seen = 1;
		}
		if (strstr((char *)pbuf, "OK")) {
			usbh_dongle_ctx.simcom.at_ok = 1;
		}
	}

done:
	if (usbh_comp_acm_rx_done_sema != NULL) {
		rtos_sema_give(usbh_comp_acm_rx_done_sema);
	}
	return HAL_OK;
}

static void usbh_comp_acm_rx_thread(void *param)
{
	UNUSED(param);
	for (;;) {
		while (usbh_dongle_ctx.in_detach != 0) {
			rtos_time_delay_ms(10);
		}
		while (usbh_dongle_ctx.in_detach == 0) {
			if (usbh_cdc_acm_receive(usbh_comp_acm_rx_buf, CONFIG_USBH_COMP_PBUF_MAX_LEN) == HAL_OK) {
				rtos_sema_take(usbh_comp_acm_rx_done_sema, 1000);
			} else {
				rtos_time_delay_ms(10);
			}
		}
	}
}

/* ========================================================================== */
/*                       Dongle Network Config (EG915)                       */
/* ========================================================================== */

static int usbh_comp_dongle_netif_init(void)
{
	char *pipaddr, *pgw, *pnetmask, *pdnsserver;
	ip4_addr_t ipaddr, gw, netmask, dnsserver;
	int ret;

	pipaddr = (char *)usbh_comp_dongle_get_netinfo((u8 *)"ip");
	if (pipaddr == NULL || (ret = inet_aton(pipaddr, &ipaddr)) == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get ip failed\n");
		return -1;
	}

	pnetmask = (char *)usbh_comp_dongle_get_netinfo((u8 *)"mask");
	if (pnetmask == NULL || (ret = inet_aton(pnetmask, &netmask)) == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get mask failed\n");
		return -1;
	}

	pgw = (char *)usbh_comp_dongle_get_netinfo((u8 *)"gw");
	if (pgw == NULL || (ret = inet_aton(pgw, &gw)) == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get gw failed\n");
		return -1;
	}

	pdnsserver = (char *)usbh_comp_dongle_get_netinfo((u8 *)"dns");
	if (pdnsserver == NULL || (ret = inet_aton(pdnsserver, &dnsserver)) == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get dns failed\n");
		return -1;
	}

	/* DNS before IP: mirrors lwIP DHCP order so rnat can cache
	 * valid DNS in the LWIP_NSC_IPV4_ADDRESS_CHANGED callback. */
	dns_setserver(0, (const ip_addr_t *)&dnsserver);
	netif_set_addr(&xnetif[NET_IF_NUM - 1], &ipaddr, &netmask, &gw);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Netif init ok\n");
	return 0;
}

/* ========================================================================== */
/*                          AT State Machines                                 */
/* ========================================================================== */

static u8 usbh_comp_quectel_eg915_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\",\"orange.m2m.spec\"";
	u8 *pdata4 = (u8 *)"AT+QNETDEVCTL=1,1,1";
	u8 *pdata5 = (u8 *)"AT+CGCONTRDP";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "EG915 state(%d)\n", state);
		}
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_IDLE:
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata1, usbh_comp_strlen(pdata1));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_SEARCH:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata2, usbh_comp_strlen(pdata2));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_SET_APN:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata3, usbh_comp_strlen(pdata3));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG:
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
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
			if (usbh_dongle_ctx.quectel.ip_ready == 0) {
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
				if (usbh_dongle_ctx.quectel.ip_ready != 0 || usbh_dongle_ctx.in_detach != 0) {
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "EG915 diag ok\n");
			return 0;
		}
		rtos_time_delay_ms(1000);
	}
	return 1;
}

static u8 usbh_comp_quectel_eg91_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\"";
	u8 *pdata4 = (u8 *)"AT+CEREG?";
	u8 *pdata5 = (u8 *)"at+qwwaninfo=0,\"IP\"";
	u8 *pdata6 = (u8 *)"AT+qnetifmac=\"ecm0\"";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "EG91 state(%d)\n", state);
		}
		if (usbh_dongle_ctx.in_detach != 0) {
			return 1;
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_IDLE:
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata1, usbh_comp_strlen(pdata1));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata1) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_SEARCH:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata2, usbh_comp_strlen(pdata2));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata2) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_SET_APN:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata3, usbh_comp_strlen(pdata3));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata3) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			state++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata4, usbh_comp_strlen(pdata4));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata4) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			idx = 0;
			do {
				idx++;
				if (usbh_dongle_ctx.quectel.cereg_ready != 0 || usbh_dongle_ctx.in_detach != 0) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (usbh_dongle_ctx.in_detach != 0) {
				return 1;
			}
			if (usbh_dongle_ctx.quectel.cereg_ready != 0) {
				state = QUECTEL_DONGLE_STATUS_GET_MAC;
			}
			break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
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
			idx = 0;
			do {
				idx++;
				if (usbh_dongle_ctx.quectel.ip_ready != 0 || usbh_dongle_ctx.in_detach != 0) {
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
		case QUECTEL_DONGLE_STATUS_GET_MAC:
			memset(usbh_comp_acm_tx_buf, 0x00, CONFIG_USBH_COMP_PBUF_MAX_LEN);
			memcpy(usbh_comp_acm_tx_buf, pdata6, usbh_comp_strlen(pdata6));
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata6) + 0] = 0x0D;
			usbh_comp_acm_tx_buf[usbh_comp_strlen(pdata6) + 1] = 0x0A;
			if (HAL_OK != usbh_comp_acm_transmit(usbh_comp_acm_tx_buf, usbh_comp_strlen(usbh_comp_acm_tx_buf))) {
				break;
			}
			idx = 0;
			do {
				idx++;
				if (usbh_dongle_ctx.quectel.mac_ready != 0 || usbh_dongle_ctx.in_detach != 0) {
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "EG91 diag ok\n");
			return 0;
		}
		rtos_time_delay_ms(1000);
	}
	return 1;
}

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

static u8 usbh_comp_fibocom_ctrl(void)
{
	u8 heart_beat = 0;
	int retry, reg_elapsed_ms, d;
	fibocom_dongle_state_t state = FIBOCOM_DONGLE_STATUS_AT_PROBE;

	u8 *pdata0  = (u8 *)"AT";
	u8 *pdata1  = (u8 *)"ATE0";
	u8 *pdata2  = (u8 *)"AT+GTUSBMODE?";
	u8 *pdata3  = (u8 *)"AT+GTUSBMODE=32";
	u8 *pdata4  = (u8 *)"AT+CFUN=15";
	u8 *pdata5  = (u8 *)"AT+CGSN?";
	u8 *pdata6  = (u8 *)"AT+CFSN?";
	u8 *pdata7  = (u8 *)"AT+CFUN?";
	u8 *pdata8  = (u8 *)"AT+CGMR?";
	u8 *pdata9  = (u8 *)"AT+CPIN?";
	u8 *pdata10 = (u8 *)"AT+CSQ?";
	u8 *pdata11 = (u8 *)"AT+CIMI";
	u8 *pdata12 = (u8 *)"AT+CGREG?";
	u8 *pdata13 = (u8 *)"AT+GTRNDIS?";
	u8 *pdata14 = (u8 *)"AT+GTRNDIS=1,1";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 state(%d)\n", state);
		}
		switch (state) {
		case FIBOCOM_DONGLE_STATUS_AT_PROBE:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.fibocom.at_ok = 0;
				if (usbh_comp_fibocom_send_wait((const char *)pdata0, &usbh_dongle_ctx.fibocom.at_ok, 1500) == 0) {
					break;
				}
				rtos_time_delay_ms(1500);
			}
			if (retry >= 30 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 Step1 timeout\n");
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_ATE0:
			usbh_dongle_ctx.fibocom.at_ok = 0;
			if (usbh_comp_fibocom_send_wait((const char *)pdata1, &usbh_dongle_ctx.fibocom.at_ok, 2000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 ATE0 fail\n");
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_USBMODE:
			usbh_dongle_ctx.fibocom.usbmode_seen = 0;
			usbh_dongle_ctx.fibocom.usbmode_is_ecm = 0;
			if (usbh_comp_fibocom_send_wait((const char *)pdata2, &usbh_dongle_ctx.fibocom.usbmode_seen, 3000) != 0) {
				return 1;
			}
			if (usbh_dongle_ctx.fibocom.usbmode_is_ecm == 0) {
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata3, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
				rtos_time_delay_ms(1000);
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMEI:
			usbh_dongle_ctx.fibocom.cgsn_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata5, &usbh_dongle_ctx.fibocom.cgsn_ok, 3000);
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_SN:
			usbh_dongle_ctx.fibocom.cfsn_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata6, &usbh_dongle_ctx.fibocom.cfsn_ok, 3000);
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CFUN:
			usbh_dongle_ctx.fibocom.cfun_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata7, &usbh_dongle_ctx.fibocom.cfun_ok, 3000);
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CGMR:
			usbh_dongle_ctx.fibocom.cgmr_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata8, &usbh_dongle_ctx.fibocom.cgmr_ok, 3000);
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIM:
			for (retry = 0; retry < 15 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.fibocom.cpin_ready = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata9, &usbh_dongle_ctx.fibocom.cpin_ready, 2000);
				if (usbh_dongle_ctx.fibocom.cpin_ready != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
			}
			if (usbh_dongle_ctx.fibocom.cpin_ready == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 SIM not ready\n");
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIGNAL:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.fibocom.csq_seen = 0;
				usbh_dongle_ctx.fibocom.csq_has_signal = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata10, &usbh_dongle_ctx.fibocom.csq_seen, 2000);
				if (usbh_dongle_ctx.fibocom.csq_has_signal != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
			}
			if (usbh_dongle_ctx.fibocom.csq_has_signal == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 no signal\n");
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMSI:
			usbh_dongle_ctx.fibocom.at_ok = 0;
			usbh_dongle_ctx.fibocom.cimi_ok = 0;
			(void)usbh_comp_fibocom_send_wait((const char *)pdata11, &usbh_dongle_ctx.fibocom.at_ok, 3000);
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_PS_REG:
			reg_elapsed_ms = 0;
			while (reg_elapsed_ms < 90000 && usbh_dongle_ctx.in_detach == 0) {
				usbh_dongle_ctx.fibocom.cgreg_seen = 0;
				usbh_dongle_ctx.fibocom.ps_registered = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata12, &usbh_dongle_ctx.fibocom.cgreg_seen, 2000);
				if (usbh_dongle_ctx.fibocom.ps_registered != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
				reg_elapsed_ms += 4000;
			}
			if (usbh_dongle_ctx.fibocom.ps_registered == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 PS attach timeout\n");
				if (usbh_dongle_ctx.in_detach == 0) {
					usbh_dongle_ctx.fibocom.at_ok = 0;
					(void)usbh_comp_fibocom_send_wait((const char *)pdata4, &usbh_dongle_ctx.fibocom.at_ok, 3000);
					rtos_time_delay_ms(1000);
				}
				return 1;
			}
			state++;
			break;
		case FIBOCOM_DONGLE_STATUS_DIAL_LOOP:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.fibocom.gtrndis_seen = 0;
				usbh_dongle_ctx.fibocom.gtrndis_has_ip = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata13, &usbh_dongle_ctx.fibocom.gtrndis_seen, 3000);
				if (usbh_dongle_ctx.fibocom.gtrndis_has_ip != 0) {
					state = FIBOCOM_DONGLE_STATUS_DONE;
					break;
				}
				usbh_dongle_ctx.fibocom.at_ok = 0;
				(void)usbh_comp_fibocom_send_wait((const char *)pdata14, &usbh_dongle_ctx.fibocom.at_ok, 5000);
				rtos_time_delay_ms(1000);
			}
			if (state != FIBOCOM_DONGLE_STATUS_DONE || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 dial loop exhausted\n");
				return 1;
			}
			break;
		case FIBOCOM_DONGLE_STATUS_DONE:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 dial ok\n");
			return 0;
		}
	}
	return 1;
}

static int usbh_comp_simcom_send_wait(const char *cmd, volatile u8 *ready_flag, u32 timeout_ms)
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

static u8 usbh_comp_simcom_ctrl(void)
{
	u8 heart_beat = 0;
	int retry, d;
	simcom_dongle_state_t state = SIMCOM_DONGLE_STATUS_AT_PROBE;

	u8 *pdata_at         = (u8 *)"AT";
	u8 *pdata_ate0       = (u8 *)"ATE0";
	u8 *pdata_cpin       = (u8 *)"AT+CPIN?";
	u8 *pdata_csq        = (u8 *)"AT+CSQ";
	u8 *pdata_cops       = (u8 *)"AT+COPS?";
	u8 *pdata_cgatt      = (u8 *)"AT+CGATT?";
	u8 *pdata_cgact      = (u8 *)"AT+CGACT=1,1";
	u8 *pdata_netopen    = (u8 *)"AT+NETOPEN";
	u8 *pdata_netopen_q  = (u8 *)"AT+NETOPEN?";
	u8 *pdata_usbnetip_q = (u8 *)"AT+USBNETIP?";
	u8 *pdata_usbnetip0  = (u8 *)"AT+USBNETIP=0";

	for (;;) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X state(%d)\n", state);
		}
		switch (state) {
		case SIMCOM_DONGLE_STATUS_AT_PROBE:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.simcom.at_ok = 0;
				if (usbh_comp_simcom_send_wait((const char *)pdata_at, &usbh_dongle_ctx.simcom.at_ok, 1000) == 0) {
					break;
				}
				rtos_time_delay_ms(2000);
			}
			if (retry >= 30 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X Step1 timeout\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_ATE0:
			usbh_dongle_ctx.simcom.at_ok = 0;
			if (usbh_comp_simcom_send_wait((const char *)pdata_ate0, &usbh_dongle_ctx.simcom.at_ok, 2000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X ATE0 fail\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_SIM:
			for (retry = 0; retry < 15 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.simcom.cpin_ready = 0;
				(void)usbh_comp_simcom_send_wait((const char *)pdata_cpin, &usbh_dongle_ctx.simcom.cpin_ready, 2000);
				if (usbh_dongle_ctx.simcom.cpin_ready != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
			}
			if (usbh_dongle_ctx.simcom.cpin_ready == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X SIM not ready\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_SIGNAL:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.simcom.csq_seen = 0;
				usbh_dongle_ctx.simcom.csq_has_signal = 0;
				(void)usbh_comp_simcom_send_wait((const char *)pdata_csq, &usbh_dongle_ctx.simcom.csq_seen, 2000);
				if (usbh_dongle_ctx.simcom.csq_has_signal != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
			}
			if (usbh_dongle_ctx.simcom.csq_has_signal == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X no signal\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_QUERY_OPERATOR:
			usbh_dongle_ctx.simcom.cops_ok = 0;
			(void)usbh_comp_simcom_send_wait((const char *)pdata_cops, &usbh_dongle_ctx.simcom.cops_ok, 3000);
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_WAIT_PS_ATTACH:
			for (retry = 0; retry < 30 && usbh_dongle_ctx.in_detach == 0; retry++) {
				usbh_dongle_ctx.simcom.cgatt_seen = 0;
				usbh_dongle_ctx.simcom.cgatt_attached = 0;
				(void)usbh_comp_simcom_send_wait((const char *)pdata_cgatt, &usbh_dongle_ctx.simcom.cgatt_seen, 2000);
				if (usbh_dongle_ctx.simcom.cgatt_attached != 0) {
					break;
				}
				for (d = 0; d < 20 && usbh_dongle_ctx.in_detach == 0; d++) {
					rtos_time_delay_ms(100);
				}
			}
			if (usbh_dongle_ctx.simcom.cgatt_attached == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X PS attach timeout\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_ACTIVATE_PDP:
			usbh_dongle_ctx.simcom.at_ok = 0;
			(void)usbh_comp_simcom_send_wait((const char *)pdata_cgact, &usbh_dongle_ctx.simcom.at_ok, 15000);
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_NETOPEN:
			usbh_dongle_ctx.simcom.netopen_query_seen = 0;
			usbh_dongle_ctx.simcom.netopen_query_state = 0;
			(void)usbh_comp_simcom_send_wait((const char *)pdata_netopen_q,
											 &usbh_dongle_ctx.simcom.netopen_query_seen, 3000);
			if (usbh_dongle_ctx.simcom.netopen_query_seen &&
				usbh_dongle_ctx.simcom.netopen_query_state == 1) {
				usbh_dongle_ctx.simcom.netopen_ok = 1;
			} else {
				for (retry = 0; retry < 3 && usbh_dongle_ctx.in_detach == 0; retry++) {
					usbh_dongle_ctx.simcom.netopen_ok = 0;
					(void)usbh_comp_simcom_send_wait((const char *)pdata_netopen,
													 &usbh_dongle_ctx.simcom.netopen_ok, 15000);
					if (usbh_dongle_ctx.simcom.netopen_ok != 0) {
						break;
					}
					rtos_time_delay_ms(1000);
				}
			}
			if (usbh_dongle_ctx.simcom.netopen_ok == 0 || usbh_dongle_ctx.in_detach != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X NETOPEN fail\n");
				return 1;
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_QUERY_USBNETIP:
			usbh_dongle_ctx.simcom.usbnetip_seen = 0;
			usbh_dongle_ctx.simcom.usbnetip_mode = 0;
			(void)usbh_comp_simcom_send_wait((const char *)pdata_usbnetip_q,
											 &usbh_dongle_ctx.simcom.usbnetip_seen, 3000);
			if (usbh_dongle_ctx.simcom.usbnetip_seen && usbh_dongle_ctx.simcom.usbnetip_mode != 0) {
				usbh_dongle_ctx.simcom.at_ok = 0;
				(void)usbh_comp_simcom_send_wait((const char *)pdata_usbnetip0,
												 &usbh_dongle_ctx.simcom.at_ok, 9000);
				if (usbh_dongle_ctx.simcom.at_ok == 0) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SIM767X USBNETIP=0 fail\n");
					return 1;
				}
			}
			state++;
			break;
		case SIMCOM_DONGLE_STATUS_DONE:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "SIM767X dial ok\n");
			return 0;
		}
	}
	return 1;
}

static int usbh_comp_dongle_diag_cmd(void)
{
	u16 pid = usbh_dongle_ctx.pid;
	u16 vid = usbh_dongle_ctx.vid;

	if (USB_VID == vid) {
		return 0;
	} else if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid) {
		if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID) {
			return usbh_comp_quectel_eg915_ctrl();
		} else if (pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID) {
			return usbh_comp_quectel_eg91_ctrl();
		}
	} else if (USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID == vid &&
			   pid == USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID) {
		return usbh_comp_fibocom_ctrl();
	} else if (USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID == vid &&
			   pid == USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID) {
		return usbh_comp_simcom_ctrl();
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown VID 0x%x, skip diag\n", vid);
	return 0;
}

/* ========================================================================== */
/*                          USB Lifecycle                                     */
/* ========================================================================== */

static int usbh_comp_do_init(void)
{
	memset(&usbh_dongle_ctx, 0, sizeof(usbh_dongle_ctx));
	usbh_dongle_ctx.in_detach = 1;

	if (usbh_init(&usbh_cfg, &usbh_comp_usr_cb) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init fail\n");
		return HAL_ERR_UNKNOWN;
	}
	usbh_cdc_acm_init(&usbh_comp_acm_cfg);
	usbh_cdc_ecm_init(&usbh_comp_ecm_cfg, &usbh_comp_ecm_priv);
	usbh_start();

	while (!usbh_cdc_ecm_usb_is_ready()) {
		rtos_time_delay_ms(1000);
	}

	usbh_dongle_ctx.vid = usbh_cdc_ecm_get_device_vid();
	usbh_dongle_ctx.pid = usbh_cdc_ecm_get_device_pid();

	RTK_LOGS(TAG, RTK_LOG_INFO, "VID 0x%x PID 0x%x\n", usbh_dongle_ctx.vid, usbh_dongle_ctx.pid);

	usbh_dongle_ctx.in_detach = 0;

	if (usbh_comp_dongle_diag_cmd() == 0) {
		usbh_cdc_ecm_prepare_done();
		RTK_LOGS(TAG, RTK_LOG_INFO, "TRX start\n");
		return HAL_OK;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Dongle diag err\n");
	return HAL_ERR_UNKNOWN;
}

static void usb_4g_hotplug_thread(void *param)
{
	UNUSED(param);
	for (;;) {
		if (rtos_sema_take(usbh_comp_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbh_dongle_ctx.in_detach = 1;
			usbh_stop();
			usbh_cdc_acm_deinit();
			usbh_cdc_ecm_deinit();
			usbh_deinit();
			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug reinit heap:0x%x\n", rtos_mem_get_free_heap_size());
			if (usbh_comp_do_init() != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug reinit fail\n");
			}
		}
	}
}

static void usb_4g_init_task(void *param)
{
	UNUSED(param);
	if (usbh_comp_do_init() != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Initial USB bring-up failed\n");
	}
	rtos_task_delete(NULL);
}

/* ========================================================================== */
/*                  Link Change Thread (Adapted for rnat callback)           */
/* ========================================================================== */

static void usb_4g_link_thread(void *param)
{
	u8 *mac;
	u8 link_is_up = 0;
	u16 vid, pid;
	eth_state_t state = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link thread\n");

	for (;;) {
		if (usbh_dongle_ctx.in_detach != 0 || !usbh_cdc_ecm_usb_is_ready()) {
			/* Treat detach/not-ready as forced link-down without calling USB APIs. */
			link_is_up = 0;
			vid = 0;
			pid = 0;
		} else {
			link_is_up = usbh_cdc_ecm_get_link_status();
			vid = usbh_cdc_ecm_get_device_vid();
			pid = usbh_cdc_ecm_get_device_pid();
		}

		if (link_is_up && state < ETH_STATUS_INIT) {
			state = ETH_STATUS_INIT;

			if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid &&
				pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID) {
				/* EG91: MAC queried via AT+qnetifmac */
				memcpy(pnetif_usb_eth->hwaddr, usbh_dongle_ctx.quectel.mac, 6);
				pnetif_usb_eth->hwaddr_len = 6;
				RTK_LOGS(TAG, RTK_LOG_INFO, "EG91 MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
						 pnetif_usb_eth->hwaddr[0], pnetif_usb_eth->hwaddr[1],
						 pnetif_usb_eth->hwaddr[2], pnetif_usb_eth->hwaddr[3],
						 pnetif_usb_eth->hwaddr[4], pnetif_usb_eth->hwaddr[5]);
			} else if (USBH_CDC_ACM_QUECTEL_DONGLE_VID == vid &&
					   pid == USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID) {
				/* EG915: ECM MAC + static IP pre-set on netif (rnat skips DHCP) */
				mac = (u8 *)usbh_cdc_ecm_process_mac_str();
				if (mac) {
					memcpy(xnetif[NET_IF_NUM - 1].hwaddr, mac, 6);
					xnetif[NET_IF_NUM - 1].hwaddr_len = 6;
				}
				usbh_comp_dongle_netif_init();
				RTK_LOGS(TAG, RTK_LOG_INFO, "EG915 static IP set\n");
			} else {
				/* RTK device / LE271 / SIM767X: standard ECM MAC, DHCP by upper layer */
				mac = (u8 *)usbh_cdc_ecm_process_mac_str();
				if (mac) {
					memcpy(pnetif_usb_eth->hwaddr, mac, 6);
					pnetif_usb_eth->hwaddr_len = 6;
					RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x Vid 0x%x\n",
							 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid);
				}
			}

			if (g_usb_eth_link_cb) {
				g_usb_eth_link_cb(1);
			}
		} else if (!link_is_up && state >= ETH_STATUS_INIT) {
			state = ETH_STATUS_DEINIT;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Link down Vid 0x%x\n", vid);
			if (g_usb_eth_link_cb) {
				g_usb_eth_link_cb(0);
			}
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

/* ========================================================================== */
/*                              Public API                                    */
/* ========================================================================== */

void usb_eth_register_link_cb(void (*cb)(int link_up))
{
	g_usb_eth_link_cb = cb;
}

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_cdc_ecm_send_data(buf, len, block);
}

void usb_eth_init(void)
{
	int ret;
	rtos_task_t hotplug_task;

	rltk_usb_eth_init();

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

	ret = rtos_task_create(&usbh_comp_acm_rx_task, "usb_4g_acm_rx",
						   usbh_comp_acm_rx_thread, NULL,
						   CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create acm rx task fail\n");
		goto err_send_sema;
	}

	ret = rtos_sema_create(&usbh_comp_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create detach sema fail\n");
		goto err_rx_task;
	}

	ret = rtos_task_create(&hotplug_task, "usb_4g_hotplug",
						   usb_4g_hotplug_thread, NULL,
						   CONFIG_USBH_COMP_HOTPLUG_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug task fail\n");
		goto err_detach_sema;
	}

	ret = rtos_task_create(&usbh_comp_link_check_task, "usb_4g_link",
						   usb_4g_link_thread, NULL,
						   CONFIG_USBH_COMP_LINK_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_LINK_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create link task fail\n");
		goto err_detach_sema;
	}

	ret = rtos_task_create(&usbh_comp_ecm_init_task, "usb_4g_init",
						   usb_4g_init_task, NULL,
						   CONFIG_USBH_COMP_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBH_COMP_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create init task fail\n");
		goto err_detach_sema;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB 4G dongle ETH started\n");
	return;

err_detach_sema:
	rtos_sema_delete(usbh_comp_detach_sema);
err_rx_task:
	rtos_task_delete(usbh_comp_acm_rx_task);
err_send_sema:
	rtos_sema_delete(usbh_comp_acm_send_sema);
err_rx_done_sema:
	rtos_sema_delete(usbh_comp_acm_rx_done_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USB 4G dongle ETH init fail\n");
}

#endif /* CONFIG_LWIP_USB_ETHERNET && CONFIG_RNAT && CONFIG_USBH_CDC_ACM_4G_DONGLE */
