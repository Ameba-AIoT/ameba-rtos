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
#include "usbh_composite_cdc_acm_ecm.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "Eth";

/* lwip api */
extern void rltk_usb_eth_init(void);
extern void rltk_usb_eth_deinit(void);

static int composite_ecm_cb_detach(void);
static int composite_ecm_cb_rxdata(u8 *buf, u32 len);
static int composite_acm_cb_rxdata(u8 *buf, u32 len);

/*enable this used to check ecm init/deinit memory leakage*/
#define CONFIG_USBH_COMPOSITE_MEM_CHECK         0
#define CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST     1     /* Hot plug test */
#define ENABLE_DUMP_FILE                        0
#define ENABLE_REMOTE_FILE_DOWNLOAD             0
#define ENABLE_USER_SET_DONGLE_MAC              0

#define PBUF_MAX_LEN                            (128)
#define NETWORK_INFO_MAX_STR                    (128)

/* while do mem check, disable download & hotplug */
#if CONFIG_USBH_COMPOSITE_MEM_CHECK
#undef  ENABLE_REMOTE_FILE_DOWNLOAD
#define ENABLE_REMOTE_FILE_DOWNLOAD             0
#undef  CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST
#define CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST     0
#endif

#if ENABLE_REMOTE_FILE_DOWNLOAD
#define MD5_CHECK_BUFFER_LEN                    (2)
/* socket server info */
#define SERVER_HOST                             "www.baidu.com"
#define SERVER_PORT                             80
#define RESOURCE                                "/"
#define BUFFER_SIZE                             1000      //download test buffer length
#define RECV_TO                                 60*1000   // ms

static unsigned char dl_buf[BUFFER_SIZE + 1];
static mbedtls_md5_context ctx;
#if ENABLE_DUMP_FILE
#define configTOTAL_PSRAM_HEAP_SIZE_TEST        (29000)
static unsigned char dump_psRAMHeap[configTOTAL_PSRAM_HEAP_SIZE_TEST];
#endif
#endif  //ENABLE_REMOTE_FILE_DOWNLOAD

#if ENABLE_USER_SET_DONGLE_MAC
static u16 ecm_led_color[1] = {0x1122};
static u8 ecm_mac_str[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif

static rtos_sema_t usb_detach_sema;
static rtos_task_t eth_link_status_check_task;
static rtos_task_t ecm_init_task;
static u8 acm_ctrl_buf[PBUF_MAX_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 dhcp_done = 0;

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
	u8 ip[NETWORK_INFO_MAX_STR];
	u8 mask[NETWORK_INFO_MAX_STR];
	u8 gw[NETWORK_INFO_MAX_STR];
	u8 dns[NETWORK_INFO_MAX_STR];
} network_info_t;

static u8 acm_rx_busy = 0;

static usbh_composite_cdc_acm_param_t dongle_array[] = {
	{USB_QUECTEL_DONGLE_VID, QUECTEL_DONGLE_PID_EG915, 2},
	{USB_QUECTEL_DONGLE_VID, QUECTEL_DONGLE_PID_EG91,  2},
	{USB_FIBOCOM_DONGLE_VID, FIBOCOM_DONGLE_PID_LE271, 2},
	{0, 0, 0},
};

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
} dongle_ctx_t;

static dongle_ctx_t dongle_ctx;

static usbh_composite_cdc_ecm_priv_data_t ecm_priv = {
#if ENABLE_USER_SET_DONGLE_MAC
	ecm_mac_str,
	ecm_led_color,
	sizeof(ecm_led_color) / sizeof(ecm_led_color[0]),
#else
	NULL,
	NULL,
	0,
#endif
};

static usbh_composite_cdc_acm_usr_cb_t usbh_acm_cfg = {
	.bulk_received = composite_acm_cb_rxdata,
	.priv = dongle_array,
};

static usbh_composite_cdc_ecm_usr_cb_t usbh_ecm_cfg = {
	.bulk_received = composite_ecm_cb_rxdata,
	.detach = composite_ecm_cb_detach,
	.priv = &ecm_priv,
};

static u32 composite_dev_strlen(u8 *pbuf)
{
	return strlen((char *)pbuf);
}

static void composite_dev_dongle_set_netinfo(u8 *pbuf, u8 *name)
{
	const char *pname = (const char *)name;
	u8 len = 0;

	if (pbuf[0] == '"') {
		pbuf ++;
	}
	len = composite_dev_strlen(pbuf);
	if (pbuf[len - 1] == '"') {
		pbuf[len - 1] = 0;
	}
	len = composite_dev_strlen(pbuf);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type(%s)=[%s(%d)]\n", name, pbuf, len);

	//whether support netinfo
	if (_strcmp(pname, "gw") == 0) {
		memcpy(dongle_ctx.quectel.network.gw, pbuf, len);
	} else if (_strcmp(pname, "ip") == 0) {
		memcpy(dongle_ctx.quectel.network.ip, pbuf, len);
	} else if (_strcmp(pname, "mask") == 0) {
		memcpy(dongle_ctx.quectel.network.mask, pbuf, len);
	} else if (_strcmp(pname, "dns") == 0) {
		memcpy(dongle_ctx.quectel.network.dns, pbuf, len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}
}
static u8 *composite_dev_dongle_get_netinfo(u8 *name)
{
	const char *pname = (const char *)name;

	//whether support netinfo
	if (_strcmp(pname, "gw") == 0) {
		return dongle_ctx.quectel.network.gw;
	} else if (_strcmp(pname, "ip") == 0) {
		return dongle_ctx.quectel.network.ip;
	} else if (_strcmp(pname, "mask") == 0) {
		return dongle_ctx.quectel.network.mask;
	} else if (_strcmp(pname, "dns") == 0) {
		return dongle_ctx.quectel.network.dns;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}

	return NULL;
}

/**
  * @brief  Composite ACM bulk receive callback
  * @param  pbuf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int composite_acm_cb_rxdata(u8 *pbuf, u32 len) //type is usb transfer type
{
	u32 i;
	u16 vid = dongle_ctx.vid;
	acm_rx_busy = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "ACM data received len(%d) \n", len);

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

	if (USB_QUECTEL_DONGLE_VID == vid) {
		/* EG91 */
		if (strstr((char *)pbuf, "+CEREG")) { ///parse to get the network information
			char *buff = (char *)pbuf;
			char *p;
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			if (p && ((*p == '1' || *p == '5'))) {
				dongle_ctx.quectel.cereg_ready = 1;
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
			//parse to get the ip
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"ip");
			p = strsep(&buff, ",");
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			//parse to get the gw
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"mask");
			p = strsep(&buff, ",");
			//parse to get the dns
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
			p = strsep(&buff, ",");

			dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
		if (strstr((char *)pbuf, "+QNETIFMAC")) { ///parse to get the mac info
			/*
				+QNETIFMAC:"ea:24:64:58:ca:83"
			*/
			char *buff = (char *)pbuf;
			char *p = strsep(&buff, "\"");
			p = strsep(&buff, "\"");
			if (p) {
				for (i = 0; i < 6; i++) {
					dongle_ctx.quectel.mac[i] = usbh_composite_cdc_ecm_hex_to_char(p[3 * i + 0]) * 16 + usbh_composite_cdc_ecm_hex_to_char(p[3 * i + 1]) ;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", dongle_ctx.quectel.mac[0], dongle_ctx.quectel.mac[1], dongle_ctx.quectel.mac[2],
						 dongle_ctx.quectel.mac[3], dongle_ctx.quectel.mac[4], dongle_ctx.quectel.mac[5]);
				dongle_ctx.quectel.mac_ready = 1;
			}
		}

		/* EG915 */
		if (strstr((char *)pbuf, "+CGCONTRDP")) { ///parse to get the network information
			char *buff = (char *)pbuf;
			u8 *tail ;
			char *p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			p = strsep(&buff, ",");
			//parse to get the ip/mask
			if (p) {
				u8 dotcount = 0;
				u8 ptmp[100];
				memset(ptmp, 0, 100);
				memcpy(ptmp, p, strlen(p));
				tail = ptmp ;
				do {
					if (*tail == '.') {
						dotcount ++;
						if (dotcount == 4) {
							*tail = 0;
							tail++;
							break;
						}
					}
					tail++;
				} while (1);
				composite_dev_dongle_set_netinfo(ptmp, (u8 *)"ip");
				composite_dev_dongle_set_netinfo(tail, (u8 *)"mask");
			}
			p = strsep(&buff, ",");
			//parse to get the gw
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"gw");
			p = strsep(&buff, ",");
			//parse to get the dns
			composite_dev_dongle_set_netinfo((u8 *)p, (u8 *)"dns");
			p = strsep(&buff, ",");

			dongle_ctx.quectel.ip_ready = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
		}
	} else if (USB_FIBOCOM_DONGLE_VID == vid) {
		/* LE271 (Fibocom) - parse the responses listed in the LE271 ECM dial flow.
		   Responses arrive line-by-line; we set per-marker flags consumed by dongle_fibocom_diag_ctrl(). */
		if (strstr((char *)pbuf, "+GTUSBMODE:")) {
			char *p = strstr((char *)pbuf, "+GTUSBMODE:") + sizeof("+GTUSBMODE:") - 1;
			while (*p == ' ') {
				p++;
			}
			dongle_ctx.fibocom.usbmode_is_ecm = (atoi(p) == 32) ? 1 : 0;
			dongle_ctx.fibocom.usbmode_seen = 1;
		}
		if (strstr((char *)pbuf, "+CGSN:")) {
			dongle_ctx.fibocom.cgsn_ok = 1;
		}
		if (strstr((char *)pbuf, "+CFSN:")) {
			dongle_ctx.fibocom.cfsn_ok = 1;
		}
		/* AT+CFUN? returns "+CFUN: 1" / "+CFUN: 4" etc.; PDF expects functional state 1. */
		if (strstr((char *)pbuf, "+CFUN:")) {
			char *p = strstr((char *)pbuf, "+CFUN:") + sizeof("+CFUN:") - 1;
			while (*p == ' ') {
				p++;
			}
			if (*p == '1') {
				dongle_ctx.fibocom.cfun_ok = 1;
			}
		}
		if (strstr((char *)pbuf, "+CGMR:")) {
			dongle_ctx.fibocom.cgmr_ok = 1;
		}
		if (strstr((char *)pbuf, "+CPIN: READY")) {
			dongle_ctx.fibocom.cpin_ready = 1;
		}
		if (strstr((char *)pbuf, "+CSQ:")) {
			char *p = strstr((char *)pbuf, "+CSQ:") + sizeof("+CSQ:") - 1;
			while (*p == ' ') {
				p++;
			}
			dongle_ctx.fibocom.csq_has_signal = (atoi(p) == 99) ? 0 : 1;
			dongle_ctx.fibocom.csq_seen = 1;
		}
		/* CIMI returns the IMSI digits followed by OK; treat any "460" prefix as success.
		   Generic OK (set below) also unblocks Step10 for non-CHN MNC. */
		if (strstr((char *)pbuf, "460")) {
			dongle_ctx.fibocom.cimi_ok = 1;
		}
		if (strstr((char *)pbuf, "+CGREG:")) {
			/* +CGREG: <n>,<stat>[,...]  - stat==1 (home) or stat==5 (roaming) means PS attached. */
			char *buff = (char *)strstr((char *)pbuf, "+CGREG:");
			char *p = strsep(&buff, ":");
			p = strsep(&buff, ",");           /* <n> */
			p = strsep(&buff, ",\r\n");       /* <stat> */
			if (p) {
				while (*p == ' ') {
					p++;
				}
				if (*p == '1' || *p == '5') {
					dongle_ctx.fibocom.ps_registered = 1;
				}
			}
			dongle_ctx.fibocom.cgreg_seen = 1;
		}
		if (strstr((char *)pbuf, "+GTRNDIS:")) {
			char *p = strstr((char *)pbuf, "+GTRNDIS:") + sizeof("+GTRNDIS:") - 1;
			while (*p == ' ') {
				p++;
			}
			/* Empty-state form "+GTRNDIS: 0" => no IPv4 yet; otherwise the response carries an IP. */
			if (*p == '0' && (*(p + 1) == '\r' || *(p + 1) == '\n' || *(p + 1) == 0)) {
				dongle_ctx.fibocom.gtrndis_has_ip = 0;
			} else {
				dongle_ctx.fibocom.gtrndis_has_ip = 1;
			}
			dongle_ctx.fibocom.gtrndis_seen = 1;
		}
		/* Generic OK - covers Step1/Step2/Step10/Step13 acks. Place last so it does not pre-empt
		   marker matches above for "<marker>...OK" multi-line replies. */
		if (strstr((char *)pbuf, "OK")) {
			dongle_ctx.fibocom.at_ok = 1;
		}
	}

	return HAL_OK;
}

/*
	for appx cmd test entry
*/
static u32 composite_acm_cmd_test(u16 argc, u8 *argv[])
{
	u8 *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (u8 *)argv[0];

	RTK_LOGS(TAG, RTK_LOG_INFO, "At cmd(%s)\n", cmd);
	if (USB_QUECTEL_DONGLE_VID == dongle_ctx.vid) {
		memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
		memcpy(acm_ctrl_buf, cmd, composite_dev_strlen(cmd));
		acm_ctrl_buf[composite_dev_strlen(cmd) + 0] = 0x0D;
		acm_ctrl_buf[composite_dev_strlen(cmd) + 1] = 0x0A;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Pre AtCmd\n");
		while (1) {
			if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
				break;
			}
			rtos_time_delay_ms(1000);
		}
	} else {
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "At cmd(%s) finish!\n", cmd);
	return 0;
}

static u8 dongle_quectel_eg915_diag_ctrl(void)
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

	while (1) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter state(%d)--------------------------------------------\n", state);
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_IDLE: //for debug issue
			while (acm_rx_busy == 0) {
				rtos_time_delay_ms(10);
			}
			memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
			memcpy(acm_ctrl_buf, pdata0, composite_dev_strlen(pdata0));
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 0] = 0x0D;
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 1] = 0x0A;
			usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata1, composite_dev_strlen(pdata1));
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_SEARCH:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata2, composite_dev_strlen(pdata2));
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_SET_APN:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata3, composite_dev_strlen(pdata3));
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata4, composite_dev_strlen(pdata4));
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
				rtos_time_delay_ms(15 * 1000);
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
			if (0 == dongle_ctx.quectel.ip_ready) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata5, composite_dev_strlen(pdata5));
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			idx = 0;
			do {
				idx ++;
				if (dongle_ctx.quectel.ip_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ctx.quectel.ip_ready) {
				state = QUECTEL_DONGLE_STATUS_MAX; ///maybe should add a check for AT RX msg,and allow to switch to next stage
			}
			break;
		case QUECTEL_DONGLE_STATUS_MAX:
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get all network info !\n");
			rtos_time_delay_ms(10);
			return 0;
			break;
		}
		rtos_time_delay_ms(1000);///force sleep to wait the reply
	}

	return 1;
}
static u8 dongle_quectel_eg91_diag_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	quectel_dongle_state_t state = QUECTEL_DONGLE_STATUS_ECM_CFG;

	u8 *pdata0 = (u8 *)"AT+qdbgcfg=\"tracecfg\",0,1";
	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\"";
	u8 *pdata4 = (u8 *)"AT+CEREG?"; //
	u8 *pdata5 = (u8 *)"at+qwwaninfo=0,\"IP\"";
	u8 *pdata6 = (u8 *)"AT+qnetifmac=\"ecm0\"";

	while (1) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter state(%d)--------------------------------------------\n", state);
		}
		switch (state) {
		case QUECTEL_DONGLE_STATUS_IDLE: //for debug issue
			while (acm_rx_busy == 0) {
				rtos_time_delay_ms(10);
			}
			memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
			memcpy(acm_ctrl_buf, pdata0, composite_dev_strlen(pdata0));
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 0] = 0x0D;
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 1] = 0x0A;
			if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
				break;
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_ECM_CFG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata1, composite_dev_strlen(pdata1));
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_SEARCH:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata2, composite_dev_strlen(pdata2));
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_SET_APN:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata3, composite_dev_strlen(pdata3));
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			state ++;
			break;
		case QUECTEL_DONGLE_STATUS_DIAG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata4, composite_dev_strlen(pdata4));
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			idx = 0;
			do {
				idx ++;
				if (dongle_ctx.quectel.cereg_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ctx.quectel.cereg_ready) {
				//EG 91 use DHCP to get mac
				state = QUECTEL_DONGLE_STATUS_GET_MAC;
			}
			break;
		case QUECTEL_DONGLE_STATUS_DIAG_STATUS:
			if (1) {
				rtos_time_delay_ms(15 * 1000);
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata5, composite_dev_strlen(pdata5));
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			idx = 0;
			do {
				idx ++;
				if (dongle_ctx.quectel.ip_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ctx.quectel.ip_ready) {
				state = QUECTEL_DONGLE_STATUS_GET_MAC; ///maybe should add a check for AT RX msg,and allow to switch to next stage
			}
			break;
		case QUECTEL_DONGLE_STATUS_GET_MAC:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata6, composite_dev_strlen(pdata6));
				acm_ctrl_buf[composite_dev_strlen(pdata6) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata6) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			idx = 0;
			do {
				idx ++;
				if (dongle_ctx.quectel.mac_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ctx.quectel.mac_ready) {
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
		rtos_time_delay_ms(1000);///force sleep to wait the reply
	}
	return 1;
}

/*
 * LE271 (Fibocom) ECM dial helper.
 * Sends a single AT command (auto-appends CRLF) then polls *ready_flag for up to timeout_ms.
 * Caller is responsible for clearing the flag before invoking this so we don't latch on stale data.
 * Returns 0 on flag set, -1 on tx failure or timeout.
 */
static int fibocom_send_at_wait(const char *cmd, volatile u8 *ready_flag, u32 timeout_ms)
{
	u32 cmd_len = composite_dev_strlen((u8 *)cmd);
	u32 elapsed = 0;

	if (cmd_len + 2 > PBUF_MAX_LEN) {
		return -1;
	}

	memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
	memcpy(acm_ctrl_buf, cmd, cmd_len);
	acm_ctrl_buf[cmd_len + 0] = 0x0D;
	acm_ctrl_buf[cmd_len + 1] = 0x0A;
	if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
		return -1;
	}

	while (elapsed < timeout_ms) {
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
static u8 dongle_fibocom_diag_ctrl(void)
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

	while (1) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter LE271 state(%d)--------------------------------------------\n", state);
		}
		switch (state) {
		case FIBOCOM_DONGLE_STATUS_AT_PROBE:
			/* Step1: probe the module with bare AT, retry every 2s, give up after ~60s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step1: AT probe\n");
			for (retry = 0; retry < 30; retry++) {
				dongle_ctx.fibocom.at_ok = 0;
				if (fibocom_send_at_wait((const char *)pdata0, &dongle_ctx.fibocom.at_ok, 1000) == 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 AT no reply, retry %d\n", retry + 1);
				rtos_time_delay_ms(2000);
			}
			if (retry >= 30) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 Step1 timeout\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_ATE0:
			/* Step2: disable AT echo. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step2: ATE0\n");
			dongle_ctx.fibocom.at_ok = 0;
			if (fibocom_send_at_wait((const char *)pdata1, &dongle_ctx.fibocom.at_ok, 2000) != 0) {
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
			dongle_ctx.fibocom.usbmode_seen = 0;
			dongle_ctx.fibocom.usbmode_is_ecm = 0;
			if (fibocom_send_at_wait((const char *)pdata2, &dongle_ctx.fibocom.usbmode_seen, 3000) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 GTUSBMODE? no reply\n");
				return 1;
			}
			if (!dongle_ctx.fibocom.usbmode_is_ecm) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 not in ECM mode, switch and reboot\n");
				dongle_ctx.fibocom.at_ok = 0;
				(void)fibocom_send_at_wait((const char *)pdata3, &dongle_ctx.fibocom.at_ok, 3000);
				dongle_ctx.fibocom.at_ok = 0;
				(void)fibocom_send_at_wait((const char *)pdata4, &dongle_ctx.fibocom.at_ok, 3000);
				/* USB will detach; let example_usbh_comp_acm_ecm_hotplug_thread re-init and re-enter this flow. */
				rtos_time_delay_ms(1000);
				return 0;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMEI:
			/* Step4: IMEI - non-fatal, continue even on timeout (info-only step). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step4: AT+CGSN?\n");
			dongle_ctx.fibocom.cgsn_ok = 0;
			(void)fibocom_send_at_wait((const char *)pdata5, &dongle_ctx.fibocom.cgsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_SN:
			/* Step5: device serial number (informational). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step5: AT+CFSN?\n");
			dongle_ctx.fibocom.cfsn_ok = 0;
			(void)fibocom_send_at_wait((const char *)pdata6, &dongle_ctx.fibocom.cfsn_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CFUN:
			/* Step6: functional state (informational, expect +CFUN: 1). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step6: AT+CFUN?\n");
			dongle_ctx.fibocom.cfun_ok = 0;
			(void)fibocom_send_at_wait((const char *)pdata7, &dongle_ctx.fibocom.cfun_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_CGMR:
			/* Step7: firmware version (informational). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step7: AT+CGMR?\n");
			dongle_ctx.fibocom.cgmr_ok = 0;
			(void)fibocom_send_at_wait((const char *)pdata8, &dongle_ctx.fibocom.cgmr_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIM:
			/* Step8: SIM detection - poll until +CPIN: READY (cap at ~30s to avoid hangs). */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step8: AT+CPIN? (wait SIM)\n");
			for (retry = 0; retry < 15; retry++) {
				dongle_ctx.fibocom.cpin_ready = 0;
				(void)fibocom_send_at_wait((const char *)pdata9, &dongle_ctx.fibocom.cpin_ready, 2000);
				if (dongle_ctx.fibocom.cpin_ready) {
					break;
				}
				rtos_time_delay_ms(2000);
			}
			if (!dongle_ctx.fibocom.cpin_ready) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 SIM not ready\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_SIGNAL:
			/* Step9: signal strength - retry while CSQ==99 (no signal), cap at ~60s. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step9: AT+CSQ? (wait signal)\n");
			for (retry = 0; retry < 30; retry++) {
				dongle_ctx.fibocom.csq_seen = 0;
				dongle_ctx.fibocom.csq_has_signal = 0;
				(void)fibocom_send_at_wait((const char *)pdata10, &dongle_ctx.fibocom.csq_seen, 2000);
				if (dongle_ctx.fibocom.csq_has_signal) {
					break;
				}
				rtos_time_delay_ms(2000);
			}
			if (!dongle_ctx.fibocom.csq_has_signal) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 no signal\n");
				return 1;
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_QUERY_IMSI:
			/* Step10: read CIMI (IMSI) - operator-aware logic could pick APN here; we keep it
			   informational and rely on the module's default APN as the PDF's APN setup is operator-specific. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step10: AT+CIMI\n");
			dongle_ctx.fibocom.at_ok = 0;
			dongle_ctx.fibocom.cimi_ok = 0;
			(void)fibocom_send_at_wait((const char *)pdata11, &dongle_ctx.fibocom.at_ok, 3000);
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_WAIT_PS_REG:
			/* Step11: PS-domain registration via +CGREG. PDF: continuous query timeout 90s -> reset module. */
			RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step11: AT+CGREG? (wait PS attach)\n");
			reg_elapsed_ms = 0;
			while (reg_elapsed_ms < 90000) {
				dongle_ctx.fibocom.cgreg_seen = 0;
				dongle_ctx.fibocom.ps_registered = 0;
				(void)fibocom_send_at_wait((const char *)pdata12, &dongle_ctx.fibocom.cgreg_seen, 2000);
				if (dongle_ctx.fibocom.ps_registered) {
					break;
				}
				rtos_time_delay_ms(2000);
				reg_elapsed_ms += 4000;
			}
			if (!dongle_ctx.fibocom.ps_registered) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "LE271 PS attach timeout, reset module\n");
				dongle_ctx.fibocom.at_ok = 0;
				(void)fibocom_send_at_wait((const char *)pdata4, &dongle_ctx.fibocom.at_ok, 3000);
				rtos_time_delay_ms(1000);
				return 0;  /* let hot-plug re-init */
			}
			state ++;
			break;
		case FIBOCOM_DONGLE_STATUS_DIAL_LOOP:
			/* Step12 + Step13: check IP, dial if missing, loop until IPv4 is allocated. Cap loops to 30. */
			for (retry = 0; retry < 30; retry++) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step12: AT+GTRNDIS? (check IP)\n");
				dongle_ctx.fibocom.gtrndis_seen = 0;
				dongle_ctx.fibocom.gtrndis_has_ip = 0;
				(void)fibocom_send_at_wait((const char *)pdata13, &dongle_ctx.fibocom.gtrndis_seen, 3000);
				if (dongle_ctx.fibocom.gtrndis_has_ip) {
					state = FIBOCOM_DONGLE_STATUS_DONE;
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271 Step13: AT+GTRNDIS=1,1 (dial)\n");
				dongle_ctx.fibocom.at_ok = 0;
				(void)fibocom_send_at_wait((const char *)pdata14, &dongle_ctx.fibocom.at_ok, 5000);
				rtos_time_delay_ms(1000);
			}
			if (state != FIBOCOM_DONGLE_STATUS_DONE) {
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

static int composite_dev_dongle_netif_init(void)
{
	char *pipaddr;
	char *pgw;
	char *pnetmask;
	char *pdnsserver;
	ip4_addr_t ipaddr, gw, netmask, dnsserver;
	int ret;

	pipaddr = (char *)composite_dev_dongle_get_netinfo((u8 *)"ip");
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

	pnetmask = (char *)composite_dev_dongle_get_netinfo((u8 *)"mask");
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

	pgw = (char *)composite_dev_dongle_get_netinfo((u8 *)"gw");
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

	pdnsserver = (char *)composite_dev_dongle_get_netinfo((u8 *)"dns");
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
static int composite_dev_dongle_diag_cmd(void)
{
	u16 pid = dongle_ctx.pid;
	u16 vid = dongle_ctx.vid;

	if (USB_DEFAULT_VID == vid) {
		return 0;
	} else if (USB_QUECTEL_DONGLE_VID == vid) {
		//should check the status finish
		if (pid == QUECTEL_DONGLE_PID_EG915) {
			return dongle_quectel_eg915_diag_ctrl();
		} else if (pid == QUECTEL_DONGLE_PID_EG91) {
			return dongle_quectel_eg91_diag_ctrl();
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the pid(%d)\n", pid);
		}
	} else if ((USB_FIBOCOM_DONGLE_VID == vid) && (pid == FIBOCOM_DONGLE_PID_LE271)) {
		return dongle_fibocom_diag_ctrl();
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the vid(%d)\n", vid);
	}
	return 0;
}

/**
  * @brief  Composite ECM bulk receive callback
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int composite_ecm_cb_rxdata(u8 *buf, u32 len)
{
	netif_adapter_usb_eth_recv(buf, len);
	return HAL_OK;
}

/**
  * @brief  Composite ECM detach callback
  * @retval Status
  */
static int composite_ecm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST
	rtos_sema_give(usb_detach_sema);
#endif
	return HAL_OK;
}

static void composite_dev_do_init(void)
{
	/* Clear all dongle state from the previous session before re-init so that
	 * stale flags (ip_ready, cereg_ready, mac_ready, at_ok, ��) cannot be seen
	 * by the new session's state machines or the RX callback. */
	memset(&dongle_ctx, 0, sizeof(dongle_ctx));

	usbh_composite_acm_ecm_init(&usbh_acm_cfg, &usbh_ecm_cfg);

	do {
		if (usbh_composite_acm_ecm_is_ready() == HAL_OK) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success

	/* Cache vid/pid before diag so the RX callback can use dongle_ctx.vid
	 * without calling the getter on every received packet. */
	dongle_ctx.vid = usbh_composite_acm_ecm_get_device_vid();
	dongle_ctx.pid = usbh_composite_acm_ecm_get_device_pid();

	/*
		pprepare for ecm transfer:
		4G dongle, send AT cmd to the dongle before the network is ready
		other dongle, parepare for the ecm transfer
	*/
	composite_dev_dongle_diag_cmd();

	usbh_composite_cdc_ecm_prepare_done();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Example Pid 0x%x/Vid 0x%x\n", dongle_ctx.pid, dongle_ctx.vid);
}

static void example_usbh_comp_acm_ecm_link_change_thread(void *param)
{
	u8 *mac;
	u32 dhcp_status = 0;
	u8 link_is_up = 0;
	u16 vid, pid;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter link status task!\n");

	do {
		if (usbh_composite_acm_ecm_is_ready() == HAL_OK) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success


	pid = usbh_composite_acm_ecm_get_device_pid();
	vid = usbh_composite_acm_ecm_get_device_vid();

	while (1) {
		link_is_up = usbh_composite_cdc_ecm_get_connect_status();

		if (USB_DEFAULT_VID == vid) { //rtk
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {  // unlink -> link
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				mac = (u8 *)usbh_composite_cdc_ecm_process_mac_str();
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to ether netif
					dhcp_done = 1;
					ethernet_unplug = ETH_STATUS_INIT;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "DHCP Fail\n");
				}
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to unlink\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else if ((USB_QUECTEL_DONGLE_VID == vid) && (pid == QUECTEL_DONGLE_PID_EG91)) {
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				ethernet_unplug = ETH_STATUS_INIT;
				mac = dongle_ctx.quectel.mac;
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);	//Set default gw to ether netif
					dhcp_done = 1;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link\n");
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to unlink\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else if ((USB_QUECTEL_DONGLE_VID == vid) && (pid == QUECTEL_DONGLE_PID_EG915)) { //EG915
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Pid 0x%x/Vid 0x%x, EG915 mac\n", vid, pid);
				mac = (u8 *)usbh_composite_cdc_ecm_process_mac_str();
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				memcpy(xnetif[NET_IF_NUM - 1].hwaddr, mac, 6);
				if (composite_dev_dongle_netif_init() == 0) {
					ethernet_unplug = ETH_STATUS_INIT;
					netif_set_up(&xnetif[NET_IF_NUM - 1]);
					netif_set_link_up(&xnetif[NET_IF_NUM - 1]);
					netif_set_default(&xnetif[NET_IF_NUM - 1]); //choose the route path
					dhcp_done = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "Switch to link !!\n");
				}
			} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {
				ethernet_unplug = ETH_STATUS_DEINIT;
				netif_set_default(pnetif_sta);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to unlink !!\n");
			} else {
				rtos_time_delay_ms(1000);
			}
		} else if ((USB_FIBOCOM_DONGLE_VID == vid) && (pid == FIBOCOM_DONGLE_PID_LE271)) {
			/* LE271: cellular dial done in dongle_fibocom_diag_ctrl(); ECM brings up the link
			   and the host obtains IPv4 via DHCP - same path as EG91. */
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "LE271: Do DHCP\n");
				ethernet_unplug = ETH_STATUS_INIT;
				mac = (u8 *)usbh_composite_cdc_ecm_process_mac_str();
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n",
						 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = lwip_request_ip(NETIF_USB_ETH_INDEX);
				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netifapi_netif_set_default(pnetif_usb_eth);
					dhcp_done = 1;
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "Error, the Pid 0x%x/Vid 0x%x can not match! \n", vid, pid);
			rtos_time_delay_ms(5000);
		}
	}

	rtos_task_delete(NULL);
}

#if ENABLE_REMOTE_FILE_DOWNLOAD
static void composite_cdc_ecm_save_data_to_memory(char *pdata, unsigned int length)
{
#if ENABLE_DUMP_FILE
	static unsigned int psram_pos = 0;
	if (configTOTAL_PSRAM_HEAP_SIZE_TEST >= psram_pos + length) {
		memcpy((void *)&dump_psRAMHeap[psram_pos], pdata, length);
		psram_pos += length;
	}
#else
	UNUSED(length);
	UNUSED(pdata);
#endif
}

static int composite_cdc_ecm_check_download_data(char *pdata, unsigned int length)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Data len %d\n", length);
	if (0 == length) {
		return 0;
	}
	composite_cdc_ecm_save_data_to_memory(pdata, length);
	mbedtls_md5_update(&ctx, (const unsigned char *)pdata, length);
	return  0;
}

static void composite_cdc_ecm_write_data_to_flash(void)
{
#if ENABLE_DUMP_FILE
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash start\n");
	FLASH_WriteStream(0x100000, configTOTAL_PSRAM_HEAP_SIZE_TEST, dump_psRAMHeap);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump mem to flash done\n");
#endif
}

static void example_usbh_comp_acm_ecm_download_thread(void *param)
{
	int server_fd = -1;
	u8 heart_beat = 0;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	u32 resource_size = 0;
	u32 content_len = 0;
	unsigned char output[8 * MD5_CHECK_BUFFER_LEN];
	int pos = 0, read_size = 0,  header_removed = 0;
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter donwload example\n");
	memset(output, 0x00, 8 * MD5_CHECK_BUFFER_LEN);

	while (0 == dhcp_done) {
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
		int recv_timeout_ms = RECV_TO;
		// lwip 1.5.0
		struct timeval recv_timeout;
		recv_timeout.tv_sec = recv_timeout_ms / 1000;
		recv_timeout.tv_usec = recv_timeout_ms % 1000 * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
		// lwip 1.4.1
		//setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms));
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Server_fd=%d\n", server_fd);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(SERVER_HOST);
	if (server_host != NULL) {
		memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Server host\n");
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Will do connect %s\n", SERVER_HOST);
	if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		pos = 0, read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Connect success\n");
		sprintf((char *)dl_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		u32 max = composite_dev_strlen((char const *)dl_buf);

		if (0) {
			u32 index = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get Msg %d", max);
			for (index = 0; index < max; index++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "0x%x ", dl_buf[index]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n\n");
		}

		write(server_fd, (char const *)dl_buf, composite_dev_strlen((char const *)dl_buf));
		RTK_LOGS(TAG, RTK_LOG_INFO, "Will call read\n");
		mbedtls_md5_init(&ctx);
		mbedtls_md5_starts(&ctx);

		while (((content_len == 0) || (resource_size < content_len)) /**/
			   && ((read_size = read(server_fd, dl_buf + pos, BUFFER_SIZE - pos)) > 0)) {
			if (header_removed == 0) {
				char *header = NULL;

				pos += read_size;
				dl_buf[pos] = 0;
				header = strstr((char const *)dl_buf, "\r\n\r\n");

				if (header) {
					char *body, *content_len_pos;

					body = header + composite_dev_strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					RTK_LOGS(TAG, RTK_LOG_INFO, "HTTP Header: %s\n", dl_buf);

					// Remove header size to get first read size of data from body head
					read_size = pos - ((unsigned char *) body - dl_buf);
					composite_cdc_ecm_check_download_data(body, read_size);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Body=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", body[0], body[1], body[2], body[3], body[4], body[5], body[6], body[7]);
					pos = 0;
					content_len_pos = strstr((char const *)dl_buf, "Content-Length: ");
					if (content_len_pos) {
						content_len_pos += composite_dev_strlen("Content-Length: ");
						*(char *)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
						RTK_LOGS(TAG, RTK_LOG_INFO, "Content len: %d\n", content_len);
					}
				} else {
					if (pos >= BUFFER_SIZE) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "HTTP header\n");
						goto exit;
					}
					continue;
				}
			} else {
				composite_cdc_ecm_check_download_data((char *)(dl_buf + pos), (unsigned int)read_size);
			}

			resource_size += read_size;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Read resource %dB/rx=%d/total=%d\n",
					 read_size, resource_size, content_len);
		}

		composite_cdc_ecm_write_data_to_flash();
		mbedtls_md5_finish(&ctx, output);
		mbedtls_md5_free(&ctx);

		RTK_LOGS(TAG, RTK_LOG_INFO, "mbedtls_md5_finish md5\n");
		for (u8 hh = 0; hh < MD5_CHECK_BUFFER_LEN; hh++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "md5 %d=%02x%02x%02x%02x%02x%02x%02x%02x\n\n", hh,
					 output[8 * hh + 0], output[8 * hh + 1], output[8 * hh + 2], output[8 * hh + 3],
					 output[8 * hh + 4], output[8 * hh + 5], output[8 * hh + 6], output[8 * hh + 7]);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Exit read. ret = %d\n", read_size);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Http content-len = %dB, download resource size = %dB\n", content_len, resource_size);
		if (0) {
			u32 index = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "2get Msg %d", max);
			for (index = 0; index < max; index++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "0x%x ", dl_buf[index]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\n will call write \n");
		}
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

static void example_usbh_comp_acm_ecm_init_task(void *param)
{
	UNUSED(param);
	composite_dev_do_init();

	do {
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success

	rtos_task_delete(NULL);
}

static void composite_dev_init(void)
{
	int status;
	status = rtos_task_create(&eth_link_status_check_task, "example_usbh_comp_acm_ecm_link_change_thread", example_usbh_comp_acm_ecm_link_change_thread, NULL,
							  1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

	status = rtos_task_create(&ecm_init_task, "example_usbh_comp_acm_ecm_init_task", example_usbh_comp_acm_ecm_init_task, NULL, 1024U * 2, 5U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB init start\r\n");
}

#if CONFIG_USBH_COMPOSITE_MEM_CHECK
// extern void vPortGetTaskHeapInfo(void);
// extern void mmeory_array_dump(void);
static void example_usbh_comp_acm_ecm_mem_check_thread(void *param)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "[test]ecm_link_toggle_thread \n");
	int loop = 0;

	UNUSED(param);

	while (1) {
		composite_dev_init();
		rtos_time_delay_ms(10000);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop create %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
		RTK_LOGS(TAG, RTK_LOG_INFO, "Del link_state task\n");

		usbh_composite_acm_ecm_deinit();
		rtos_task_delete(eth_link_status_check_task);
		rtos_task_delete(ecm_init_task);

		rtos_time_delay_ms(3000);
		// mmeory_array_dump();
		// vPortGetTaskHeapInfo();
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loop delete %d: all_free:0x%08x\r\n", loop, usb_os_get_free_heap_size());
		loop++;
	}

	rtos_task_delete(NULL);
}
#endif

#if CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST
static void example_usbh_comp_acm_ecm_hotplug_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usb_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbh_composite_acm_ecm_deinit();
			rtos_time_delay_ms(100);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			//init
			composite_dev_do_init();
		}
	}

	rtos_task_delete(NULL);
}
#endif

/* Exported functions --------------------------------------------------------*/
#if defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET

int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_composite_cdc_ecm_send_data(buf, len, block);
}

void example_usbh_composite_cdc_acm_ecm(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH Composite ECM demo start\n");

	rltk_usb_eth_init();
	rtos_sema_create(&usb_detach_sema, 0U, 1U);

#if CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST
	rtos_task_t hot_plug_task;
	int status = rtos_task_create(&hot_plug_task, "example_usbh_comp_acm_ecm_hotplug_thread", example_usbh_comp_acm_ecm_hotplug_thread, NULL, 1024U, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hoptplag thread fail\n");
	}
#endif

#if CONFIG_USBH_COMPOSITE_MEM_CHECK
	rtos_task_t memory_monitor_task;
	int ret = rtos_task_create(&memory_monitor_task, "example_usbh_comp_acm_ecm_mem_check_thread", example_usbh_comp_acm_ecm_mem_check_thread, NULL, 1024U * 2, 3U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
#else

	composite_dev_init();

#if ENABLE_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
	status = rtos_task_create(&download_task, "example_usbh_comp_acm_ecm_download_thread", example_usbh_comp_acm_ecm_download_thread, NULL, 10 * 512U, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create download thread fail\n");
	}
#endif

#endif
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_dongle_atcmd[] = {
	{"ecm_cmd", composite_acm_cmd_test},
};

#else
#error "No Lwip USB Ethernet Configuration"
#endif
