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
#include "usbh.h"
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
	DONGLE_STATUS_IDLE = 0U,
	DONGLE_STATUS_ECM_CFG,
	DONGLE_STATUS_SEARCH,
	DONGLE_STATUS_SET_APN,
	DONGLE_STATUS_DIAG,
	DONGLE_STATUS_DIAG_STATUS,
	DONGLE_STATUS_GET_MAC,
	DONGLE_STATUS_MAX,
} dongle_state_t;

typedef struct {
	u8 ip[NETWORK_INFO_MAX_STR];
	u8 mask[NETWORK_INFO_MAX_STR];
	u8 gw[NETWORK_INFO_MAX_STR];
	u8 dns[NETWORK_INFO_MAX_STR];
} network_info_t;

static u8 acm_rx_busy = 0;
static u8 dongle_cereg_ready = 0; //
static u8 dongle_mac_ready = 0;   //
static u8 dongle_ip_ready = 0;    //get all the ip info
static u8 dongle_mac[6];
static network_info_t network_info;

static usbh_composite_cdc_acm_param_t dongle_array[] = {
	{USB_EF_DONGLE_VID, EF_DONGLE_PID_EG915, 2},
	{USB_EF_DONGLE_VID, EF_DONGLE_PID_EG91,  2},
	{0, 0, 0},
};

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
		memcpy(network_info.gw, pbuf, len);
	} else if (_strcmp(pname, "ip") == 0) {
		memcpy(network_info.ip, pbuf, len);
	} else if (_strcmp(pname, "mask") == 0) {
		memcpy(network_info.mask, pbuf, len);
	} else if (_strcmp(pname, "dns") == 0) {
		memcpy(network_info.dns, pbuf, len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}
}
static u8 *composite_dev_dongle_get_netinfo(u8 *name)
{
	const char *pname = (const char *)name;

	//whether support netinfo
	if (_strcmp(pname, "gw") == 0) {
		return network_info.gw;
	} else if (_strcmp(pname, "ip") == 0) {
		return network_info.ip;
	} else if (_strcmp(pname, "mask") == 0) {
		return network_info.mask;
	} else if (_strcmp(pname, "dns") == 0) {
		return network_info.dns;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown name(%s)\n", name);
	}

	return NULL;
}

static int composite_acm_cb_rxdata(u8 *pbuf, u32 len) //type is usb transfer type
{
	u32 i;
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

	/* EG91 */
	if (strstr((char *)pbuf, "+CEREG")) { ///parse to get the network information
		char *buff = (char *)pbuf;
		char *p;
		p = strsep(&buff, ",");
		p = strsep(&buff, ",");
		if (p && ((*p == '1' || *p == '5'))) {
			dongle_cereg_ready = 1;
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

		dongle_ip_ready = 1;
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
				dongle_mac[i] = usbh_composite_cdc_ecm_hex_to_char(p[3 * i + 0]) * 16 + usbh_composite_cdc_ecm_hex_to_char(p[3 * i + 1]) ;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", dongle_mac[0], dongle_mac[1], dongle_mac[2], dongle_mac[3], dongle_mac[4], dongle_mac[5]);
			dongle_mac_ready = 1;
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

		dongle_ip_ready = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "IP Ready\n");
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
	if (USB_EF_DONGLE_VID == usbh_composite_acm_ecm_get_device_vid()) {
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

static u8 dongle_EG915_diag_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	dongle_state_t dongle = DONGLE_STATUS_ECM_CFG;

	u8 *pdata0 = (u8 *)"AT+qdbgcfg=\"tracecfg\",0,1";
	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\",\"orange.m2m.spec\"";
	u8 *pdata4 = (u8 *)"AT+QNETDEVCTL=1,1,1";
	u8 *pdata5 = (u8 *)"AT+CGCONTRDP";

	while (1) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter dongle(%d)--------------------------------------------\n", dongle);
		}
		switch (dongle) {
		case DONGLE_STATUS_IDLE: //for debug issue
			while (acm_rx_busy == 0) {
				rtos_time_delay_ms(10);
			}
			memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
			memcpy(acm_ctrl_buf, pdata0, composite_dev_strlen(pdata0));
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 0] = 0x0D;
			acm_ctrl_buf[composite_dev_strlen(pdata0) + 1] = 0x0A;
			usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			dongle ++;
			break;
		case DONGLE_STATUS_ECM_CFG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata1, composite_dev_strlen(pdata1));
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			dongle ++;
			break;
		case DONGLE_STATUS_SEARCH:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata2, composite_dev_strlen(pdata2));
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			dongle ++;
			break;
		case DONGLE_STATUS_SET_APN:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata3, composite_dev_strlen(pdata3));
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			dongle ++;
			break;
		case DONGLE_STATUS_DIAG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata4, composite_dev_strlen(pdata4));
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata4) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
				rtos_time_delay_ms(15 * 1000);
			}
			dongle ++;
			break;
		case DONGLE_STATUS_DIAG_STATUS:
			if (0 == dongle_ip_ready) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata5, composite_dev_strlen(pdata5));
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata5) + 1] = 0x0A;
				usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf));
			}
			idx = 0;
			do {
				idx ++;
				if (dongle_ip_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ip_ready) {
				dongle = DONGLE_STATUS_MAX; ///maybe should add a check for AT RX msg,and allow to switch to next stage
			}
			break;
		case DONGLE_STATUS_MAX:
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
static u8 dongle_EG91_diag_ctrl(void)
{
	u8 heart_beat = 0;
	u8 idx;
	dongle_state_t dongle = DONGLE_STATUS_ECM_CFG;

	u8 *pdata0 = (u8 *)"AT+qdbgcfg=\"tracecfg\",0,1";
	u8 *pdata1 = (u8 *)"AT+QCFG=\"usbnet\",1";
	u8 *pdata2 = (u8 *)"AT+QNWINFO;+CEREG?;+CREG?;+QCSQ;+CSQ;+COPS?;+CPIN?;+QCCID";
	u8 *pdata3 = (u8 *)"AT+CGDCONT=1,\"IP\"";
	u8 *pdata4 = (u8 *)"AT+CEREG?"; //
	u8 *pdata5 = (u8 *)"at+qwwaninfo=0,\"IP\"";
	u8 *pdata6 = (u8 *)"AT+qnetifmac=\"ecm0\"";

	while (1) {
		if (heart_beat++ % 10 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n\nEnter dongle(%d)--------------------------------------------\n", dongle);
		}
		switch (dongle) {
		case DONGLE_STATUS_IDLE: //for debug issue
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
			dongle ++;
			break;
		case DONGLE_STATUS_ECM_CFG:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata1, composite_dev_strlen(pdata1));
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata1) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			dongle ++;
			break;
		case DONGLE_STATUS_SEARCH:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata2, composite_dev_strlen(pdata2));
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata2) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			dongle ++;
			break;
		case DONGLE_STATUS_SET_APN:
			if (1) {
				memset(acm_ctrl_buf, 0x00, PBUF_MAX_LEN);
				memcpy(acm_ctrl_buf, pdata3, composite_dev_strlen(pdata3));
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 0] = 0x0D;
				acm_ctrl_buf[composite_dev_strlen(pdata3) + 1] = 0x0A;
				if (HAL_OK != usbh_composite_cdc_acm_transmit(acm_ctrl_buf, composite_dev_strlen(acm_ctrl_buf))) {
					break;
				}
			}
			dongle ++;
			break;
		case DONGLE_STATUS_DIAG:
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
				if (dongle_cereg_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_cereg_ready) {
				//EG 91 use DHCP to get mac
				dongle = DONGLE_STATUS_GET_MAC;
			}
			break;
		case DONGLE_STATUS_DIAG_STATUS:
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
				if (dongle_ip_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_ip_ready) {
				dongle = DONGLE_STATUS_GET_MAC; ///maybe should add a check for AT RX msg,and allow to switch to next stage
			}
			break;
		case DONGLE_STATUS_GET_MAC:
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
				if (dongle_mac_ready) {
					break;
				}
				rtos_time_delay_ms(1000);
			} while (idx < 10);
			if (dongle_mac_ready) {
				dongle = DONGLE_STATUS_MAX;
			}
			break;
		case DONGLE_STATUS_MAX:
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
	u16 pid = usbh_composite_acm_ecm_get_device_pid();
	u16 vid = usbh_composite_acm_ecm_get_device_vid();

	if (USB_DEFAULT_VID == vid) {
		return 0;
	} else if (USB_EF_DONGLE_VID == vid) {
		//should check the status finish
		if (pid == EF_DONGLE_PID_EG915) {
			return dongle_EG915_diag_ctrl();
		} else if (pid == EF_DONGLE_PID_EG91) {
			return dongle_EG91_diag_ctrl();
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the pid(%d)\n", pid);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Error happen, check the vid(%d)\n", vid);
	}
	return 0;
}

static int composite_ecm_cb_rxdata(u8 *buf, u32 len)
{
	ethernetif_usb_eth_recv(buf, len);
	return HAL_OK;
}

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
	usbh_composite_acm_ecm_init(&usbh_acm_cfg, &usbh_ecm_cfg);

	do {
		if (usbh_composite_acm_ecm_is_ready() == HAL_OK) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success

	/*
		pprepare for ecm transfer:
		4G dongle, send AT cmd to the dongle before the network is ready
		other dongle, parepare for the ecm transfer
	*/
	composite_dev_dongle_diag_cmd();

	usbh_composite_cdc_ecm_prepare_done();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Example Pid 0x%x/Vid 0x%x\n", usbh_composite_acm_ecm_get_device_pid(), usbh_composite_acm_ecm_get_device_vid());
}

static void composite_eth_link_change_thread(void *param)
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

				dhcp_status = LwIP_IP_Address_Request(NETIF_USB_ETH_INDEX);
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
		} else if ((USB_EF_DONGLE_VID == vid) && (pid == EF_DONGLE_PID_EG91)) {
			if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
				RTK_LOGS(TAG, RTK_LOG_INFO, "Do DHCP\n");
				ethernet_unplug = ETH_STATUS_INIT;
				mac = dongle_mac;
				memcpy(pnetif_usb_eth->hwaddr, mac, 6);
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				netif_set_link_up(pnetif_usb_eth);

				dhcp_status = LwIP_IP_Address_Request(NETIF_USB_ETH_INDEX);
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
		} else if ((USB_EF_DONGLE_VID == vid) && (pid == EF_DONGLE_PID_EG915)) { //EG915
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

static void composite_cdc_ecm_download_thread(void *param)
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

static void composite_dev_doinit_task(void *param)
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
	status = rtos_task_create(&eth_link_status_check_task, "usbh_ecm_link_thread", composite_eth_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

	status = rtos_task_create(&ecm_init_task, "ecm_init_task", composite_dev_doinit_task, NULL, 1024U * 2, 5U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB init start\r\n");
}

#if CONFIG_USBH_COMPOSITE_MEM_CHECK
// extern void vPortGetTaskHeapInfo(void);
// extern void mmeory_array_dump(void);
static void composite_dev_mem_check_thread(void *param)
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
static void composite_dev_hotplug_thread(void *param)
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
void example_usbh_composite_cdc_acm_ecm(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH Composite ECM demo start\n");

	rltk_usb_eth_init();
	rtos_sema_create(&usb_detach_sema, 0U, 1U);

#if CONFIG_USBH_COMPOSITE_HOT_PLUG_TEST
	rtos_task_t hot_plug_task;
	int status = rtos_task_create(&hot_plug_task, "usb_hotplug_thread", composite_dev_hotplug_thread, NULL, 1024U, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hoptplag thread fail\n");
	}
#endif

#if CONFIG_USBH_COMPOSITE_MEM_CHECK
	rtos_task_t memory_monitor_task;
	int ret = rtos_task_create(&memory_monitor_task, "mem_check_thread", composite_dev_mem_check_thread, NULL, 1024U * 2, 3U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create monitor_link thread fail\n");
	}
#else

	composite_dev_init();

#if ENABLE_REMOTE_FILE_DOWNLOAD
	rtos_task_t download_task;
	status = rtos_task_create(&download_task, "usbh_ecm_download_thread", composite_cdc_ecm_download_thread, NULL, 10 * 512U, 2U);
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