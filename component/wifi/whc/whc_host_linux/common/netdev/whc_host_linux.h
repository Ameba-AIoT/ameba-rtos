// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef _RTW_TOP_HEADER_
#define _RTW_TOP_HEADER_

/******************************************************************/
/********** MACROS for FULLMAC. **************/
/******************************************************************/
/* Allow print debug info at runtime by control log level. */
/* Runtime order: echo 8 4 1 7 > /proc/sys/kernel/printk   */
#define DEBUG 1

#ifdef CONFIG_NAN
#define CONFIG_NAN_PAIRING
// #define NAN_CUSTOMER_NANDOW
#endif

/******************************************************************/
/******************** Linux platform related. *********************/
/******************************************************************/
#include <linux/version.h>
#include <linux/stddef.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/kref.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include <net/cfg80211.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/inetdevice.h>
#include <linux/skbuff.h>
#include <linux/circ_buf.h>
#include <asm/uaccess.h>
#include <asm/byteorder.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm-generic/io.h>
#include <linux/semaphore.h>
#include <linux/sem.h>
#include <linux/sched.h>
#include <linux/etherdevice.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <net/addrconf.h>
#include <linux/if_arp.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ip.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/vmalloc.h>
#include <linux/sysrq.h>
#include <linux/ioport.h>
#include <asm/string.h>
#include <linux/workqueue.h>
#include <uapi/linux/limits.h>
#include <net/ieee80211_radiotap.h>
#include <linux/ieee80211.h>
#include <net/cfg80211.h>
#include <linux/of_gpio.h>

/* fullmac headers. */
#include "autoconf.h"
#include "whc_host_wiphy.h"
#include "wifi_api_types.h"
#include "wifi_api_event.h"
#include "wifi_intf_drv_to_app_internal.h"
#include "whc_host_trx.h"
#include "ameba_wificfg_common.h"

#ifdef CONFIG_FULLMAC_HCI_IPC
/* ipc driver. */
#include <linux/ameba/ameba_ipc.h>
#include "whc_ipc.h"
#include "whc_fullmac_ipc_host_msg.h"
#include "whc_fullmac_ipc_host_ops.h"
#include "whc_fullmac_ipc_host_mem.h"
#elif defined(CONFIG_FULLMAC_HCI_SDIO)
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include "whc_dev.h"
#include "whc_sdio_host.h"
#ifdef CONFIG_BT_INIC
#include "rtb_sdio.h"
#endif
#elif defined(CONFIG_FULLMAC_HCI_SPI)
#include <linux/spi/spi.h>
#include "whc_dev.h"
#include "whc_spi_host.h"
#ifdef CONFIG_BT_INIC
#include "rtb_spi.h"
#endif
#elif defined(CONFIG_FULLMAC_HCI_USB)
#include <linux/usb.h>
#include "whc_dev.h"
#include "whc_usb_host.h"
#endif

#ifdef NAN_CUSTOMER_NANDOW
#include "WFPAL.h"
#endif

#if !defined(CONFIG_FULLMAC_HCI_IPC)
#include "whc_fullmac_host_ioctl.h"
#include "whc_fullmac_host_cust_evt.h"
#endif

#include "whc_fullmac_host_regd.h"
#ifdef CONFIG_IEEE80211R
#include "whc_fullmac_host_ft.h"
#endif
#include "whc_fullmac_host_cfgvendor.h"
#include "whc_fullmac_host_proc.h"
#include "whc_fullmac_host_acs.h"
#include "whc_fullmac_host_promisc.h"

#include "whc_host_event.h"
#include "whc_host_drv_probe.h"
#include "whc_host_netdev_ops.h"
#include "whc_host_ethtool_ops.h"
#include "whc_host_hci.h"
#include "whc_host_function.h"


/******************************************************************/
/********** Definitions between Linux and FULLMAC. **************/
/******************************************************************/
#define TOTAL_IFACE_NUM			2
#define ETH_ALEN			6
#define FUNC_NDEV_FMT			"%s(%s)"
#define FUNC_NDEV_ARG(ndev)		__func__, ndev->name
#define WHC_HOST_NAME "whc_fullmac"

#define WHC_HOST_PORT_NAME "wlan%d"

#ifdef CONFIG_FULLMAC_HCI_SDIO
#define CONFIG_WOWLAN
#endif

/******************************************************************/
/***************** Definitions for cfg80211_ops. ******************/
/******************************************************************/

struct rtw_ieee80211_hdr_3addr {
	u16				frame_ctl;
	u16				duration_id;
	u8				addr1[ETH_ALEN];
	u8				addr2[ETH_ALEN];
	u8				addr3[ETH_ALEN];
	u16				seq_ctl;
} __attribute__((packed));

struct wps_str {
	u8 wps_probe_ie[258];
	u16 wps_probe_ielen;
	u8 wps_phase;
};

#define _memset(x, y, z) memset(x, y, z)

#define set_frame_sub_type(pbuf, type) \
	do {    \
		*(unsigned short *)(pbuf) &= cpu_to_le16(~(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2))); \
		*(unsigned short *)(pbuf) |= cpu_to_le16(type); \
	} while (0)

#define get_addr2_ptr(pbuf)    ((unsigned char *)((unsigned char *)(pbuf) + 10))

#define RTW_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define RSN_HEADER_LEN 4
#define RSN_SELECTOR_LEN 4

#define MFPR_BIT BIT(0)
#define MFPC_BIT BIT(1)
#define _WEAK           __attribute__ ((weak))

/*cipher suite from 802.11-2016 p884*/
#define WIFI_CIPHER_SUITE_WEP_40			0x000FAC01
#define WIFI_CIPHER_SUITE_TKIP				0x000FAC02
#define WIFI_CIPHER_SUITE_CCMP_128		0x000FAC04
#define WIFI_CIPHER_SUITE_WEP_104			0x000FAC05
#define WIFI_CIPHER_SUITE_BIP_CMAC_128	0x000FAC06
#define WIFI_CIPHER_SUITE_GCMP				0x000FAC08
#define WIFI_CIPHER_SUITE_GCMP_256		0x000FAC09
#define WIFI_CIPHER_SUITE_CCMP_256		0x000FAC0A
#define WIFI_CIPHER_SUITE_BIP_GMAC_128	0x000FAC0B
#define WIFI_CIPHER_SUITE_BIP_GMAC_256	0x000FAC0C
#define WIFI_CIPHER_SUITE_BIP_CMAC_256	0x000FAC0D

enum ENCRYP_PROTOCOL_E {
	ENCRYP_PROTOCOL_OPENSYS,   //open system
	ENCRYP_PROTOCOL_WEP,       //WEP
	ENCRYP_PROTOCOL_WPA,       //WPA
	ENCRYP_PROTOCOL_WPA2,      //WPA2
	ENCRYP_PROTOCOL_WPA_WPA2,  //WPA & WPA2
	ENCRYP_PROTOCOL_WAPI,      //WAPI: Not support in this version
	ENCRYP_PROTOCOL_MAX
};

/******************************************************************/
/***************** inline functions for fullmac. *****************/
/******************************************************************/
static inline int rtw_ch2freq(int chan)
{
	if (chan >= 1 && chan <= 14) {
		if (chan == 14) {
			return 2484;
		} else if (chan < 14) {
			return 2407 + chan * 5;
		}
	} else if (chan >= 36 && chan <= 177) {
		return 5000 + chan * 5;
	}

	return 0; /* not supported */
}

static inline int rtw_freq2ch(int freq)
{
	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484) {
		return 14;
	} else if (freq < 2484) {
		return (freq - 2407) / 5;
	} else if (freq >= 4910 && freq <= 4980) {
		return (freq - 4000) / 5;
	} else if (freq >= 5000 && freq < 5950) {
		return (freq - 5000) / 5;
	} else if (freq >= 5950 && freq <= 7215) {
		return (freq - 5950) / 5;
	} else if (freq >= 58320 && freq <= 64800) {
		return (freq - 56160) / 2160;
	} else {
		return 0;
	}
}

static inline u8 *rtw_get_ie(const u8 *pbuf, int element_id, int *element_len, int total_len)
{
	int tmp_element_len, used_len;
	const u8 *pie;

	pie = pbuf;
	used_len = 0;
	*element_len = 0;
	while (total_len - used_len > 2) {
		if (*pie == element_id) {
			*element_len = *(pie + 1);
			if (*element_len + 2 + used_len > total_len) {
				return NULL;
			} else {
				return (u8 *)pie;
			}
		} else {
			tmp_element_len = *(pie + 1);
			pie += (tmp_element_len + 2);
			used_len += (tmp_element_len + 2);
		}
	}
	return NULL;
}

static inline unsigned char *rtw_get_wpa2_ie(unsigned char *pie, u32 *rsn_ie_len, int limit)
{
	return rtw_get_ie(pie, WLAN_EID_RSN, rsn_ie_len, limit);
}

static inline unsigned char *rtw_get_wpa_ie(unsigned char *pie, u32 *wpa_ie_len, int limit)
{
	u32 len;
	u16 val16;
	u8 *pbuf = pie;
	int limit_new = limit;
	const u8 RTW_WPA_OUI_TYPE[] = { 0x00, 0x50, 0xf2, 1 };

	while (1) {
		pbuf = rtw_get_ie(pbuf, WLAN_EID_WPA, &len, limit_new);

		if (pbuf) {

			//check if oui matches...
			if (memcmp((pbuf + 2), (void *)RTW_WPA_OUI_TYPE, sizeof(RTW_WPA_OUI_TYPE)) != 0) {

				goto check_next_ie;
			}

			//check version...
			memcpy((u8 *)&val16, (pbuf + 6), sizeof(val16));

			val16 = le16_to_cpu(val16);
			if (val16 != 0x0001) {
				goto check_next_ie;
			}

			*wpa_ie_len = *(pbuf + 1);

			return pbuf;

		} else {

			*wpa_ie_len = 0;
			return NULL;
		}

check_next_ie:

		limit_new = limit - (pbuf - pie) - 2 - len;

		if (limit_new <= 0) {
			break;
		}

		pbuf += (2 + len);

	}

	*wpa_ie_len = 0;

	return NULL;

}

static inline u8 rtw_get_pmf_option(const u8 *ie, u32 ie_len)
{
	const u8 *pos = ie;
	u16 cnt;
	if (ie + ie_len < pos + RSN_HEADER_LEN) {
		goto err;
	}
	pos += RSN_HEADER_LEN;

	/* Group CS processing */
	if (ie + ie_len < pos + RSN_SELECTOR_LEN) {
		goto err;
	}
	pos += RSN_SELECTOR_LEN;

	/* Pairwise CS processing */
	if (ie + ie_len < pos + 2) {
		goto err;
	}
	cnt = RTW_GET_LE16(pos);
	pos += 2;

	if (!cnt || (ie + ie_len < pos + cnt * RSN_SELECTOR_LEN)) {
		goto err;
	}
	pos += cnt * RSN_SELECTOR_LEN;

	/* AKM processing */
	if (ie + ie_len < pos + 2) {
		goto err;
	}
	cnt = RTW_GET_LE16(pos);
	pos += 2;

	if (!cnt || (ie + ie_len < pos + cnt * RSN_SELECTOR_LEN)) {
		goto err;
	}
	pos += cnt * RSN_SELECTOR_LEN;

	/* RSN Capability processing */
	if (ie + ie_len < pos + 2) {
		goto err;
	}
	return pos[0] >> 6;
err:
	return 0xff;
}

static inline void rtw_dump_buf(u8 *_titlestring, const u8 *_hexdata, int _hexdatalen)
{
	int __i;
	u8 *ptr = (u8 *)_hexdata;

	if (_titlestring) {
		printk("%s", _titlestring);
		if (_hexdatalen >= 16) {
			printk("\n");
		}
	}

	for (__i = 0; __i < _hexdatalen; __i++) {
		if (((__i % 16) == 0) && (_hexdatalen >= 16)) {
			printk("0x%03X: ", __i);
		}
		printk("%02X%s", ptr[__i], (((__i + 1) % 4) == 0) ? "  " : " ");
		if ((__i + 1 < _hexdatalen) && ((__i + 1) % 16) == 0) {
			printk("\n");
		}
	}
	printk("\n");
}

static inline u8 rtw_80211_cipher_suite_to_driver(const u32 _80211_suite)
{
	switch (_80211_suite) {
	case WIFI_CIPHER_SUITE_WEP_40:
		return _WEP40_;
	case WIFI_CIPHER_SUITE_TKIP:
		return _TKIP_;
	case WIFI_CIPHER_SUITE_CCMP_128:
		return _AES_;
	case WIFI_CIPHER_SUITE_WEP_104:
		return _WEP104_;
	case WIFI_CIPHER_SUITE_BIP_CMAC_128:
	case WIFI_CIPHER_SUITE_BIP_CMAC_256:
	case WIFI_CIPHER_SUITE_BIP_GMAC_128:
	case WIFI_CIPHER_SUITE_BIP_GMAC_256:
		return _BIP_;
	case WIFI_CIPHER_SUITE_GCMP:
		return _GCMP_;
	case WIFI_CIPHER_SUITE_GCMP_256:
		return _GCMP_256_;
	case WIFI_CIPHER_SUITE_CCMP_256:
		return _CCMP_256_;
	}
	return _NO_PRIVACY_;
}


#endif // _RTW_TOP_HEADER_
