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
#include "rom_rtw_defs.h"
#include "rtw_wifi_defs.h"
#include "rtw_wiphy.h"
#include "wifi_intf_drv_to_app_basic.h"
#include "rtw_llhw_trx.h"

#ifdef CONFIG_FULLMAC_HCI_IPC
/* ipc driver. */
#include <ameba_ipc/ameba_ipc.h>
#include "inic_ipc.h"
#include "rtw_llhw_msg.h"
#include "rtw_llhw_ops.h"
#elif defined(CONFIG_FULLMAC_HCI_SDIO)
#include <linux/mmc/sdio_func.h>
#include "inic_dev.h"
#include "rtw_sdio.h"
#include "rtw_ioctl.h"
#elif defined(CONFIG_FULLMAC_HCI_SPI)
#include <linux/spi/spi.h>
#include "inic_dev.h"
#include "rtw_spi.h"
#include "rtw_ioctl.h"
#elif defined(CONFIG_FULLMAC_HCI_USB)
#include <linux/usb.h>
#include "inic_dev.h"
#include "rtw_usb.h"
#include "rtw_ioctl.h"
#endif

#include "rtw_llhw_event.h"
#include "rtw_drv_probe.h"
#include "rtw_regd.h"
#include "rtw_netdev_ops.h"
#include "rtw_ethtool_ops.h"
#include "rtw_llhw_hci.h"
#include "rtw_functions.h"
#include "rtw_cfgvendor.h"
#include "rtw_proc.h"
#include "rtw_acs.h"

/******************************************************************/
/********** Definitions between Linux and FULLMAC. **************/
/******************************************************************/
#ifdef CONFIG_SDIO_BRIDGE
#define TOTAL_IFACE_NUM			1
#else
#define TOTAL_IFACE_NUM			2
#endif
#define ETH_ALEN			6
#define FUNC_NDEV_FMT			"%s(%s)"
#define FUNC_NDEV_ARG(ndev)		__func__, ndev->name

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
