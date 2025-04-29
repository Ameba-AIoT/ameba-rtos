// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

/* Cipher suite type: params->cipher in add key. */
/*
 * OUI		Suite type	Meaning
 * 00-0F-AC	0		Use group cipher suite
 * 00-0F-AC	1		WEP-40
 * 00-0F-AC	2		TKIP
 * 00-0F-AC	3		Reserved
 * 00-0F-AC	4		CCMP - default in an RSNA
 * 00-0F-AC	5		WEP-104
 * 00-0F-AC	6-255		Reserved
 * Vendor OUI	Other		Vendor specific
*/

/* AKM suites: crypto.akm_suites while connect. */
/* OUI		Suite type	Authentication type					Key management type
 * 00-0F-AC	0		Reserved						Reserved
 * 00-0F-AC	1		IEEE 802.1X or using PMKSA caching - RSNA default	RSNA key management or using PMKSA caching - RSNA default
 * 00-0F-AC	2		PSK							RSNA key management, using PSK
 * 00-0F-AC	3-255		Reserved						Reserved
 * Vendor OUI	Any		Vendor specific						Vendor specific
*/

static int whc_fullmac_host_add_key_ops(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
										, int link_id
#endif
										, u8 key_index
										, bool pairwise
										, const u8 *mac_addr, struct key_params *params)
{
	struct rtw_crypt_info *crypt;
	int ret = 0;
	struct xmit_priv_t *xmit_priv = &global_idev.xmit_priv;

	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);
	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	crypt = kmalloc(sizeof(struct rtw_crypt_info), GFP_KERNEL);
	if (!crypt) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	memset(crypt, 0, sizeof(struct rtw_crypt_info));
	if (ndev) {
		crypt->wlan_idx = rtw_netdev_idx(ndev);
	} else {
		ret = -EINVAL;
		goto exit;
	}

	dev_dbg(global_idev.fullmac_dev, "[fullmac]: netdev = %d", crypt->wlan_idx);
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: key_index = %d", key_index);
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: pairwise= %d", pairwise);
	if (pairwise) {
		dev_dbg(global_idev.fullmac_dev, "[fullmac]: mac addr = %x %x %x %x %x %x", *mac_addr, *(mac_addr + 1), *(mac_addr + 2), *(mac_addr + 3), *(mac_addr + 4),
				*(mac_addr + 5));
	}
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: key material. key len = %d.\n", params->key_len);
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: cipher suite = %08x", params->cipher);
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: key mode = %d", params->mode);

	if (((params->cipher & 0xff) == 1) || ((params->cipher & 0xff) == 5)) {
		/* Set WEP key by rtos. */
		dev_dbg(global_idev.fullmac_dev, "--- %s --- return: set key by rtos self. ", __func__);
		kfree(crypt);
		return 0;
	}

	if (params->key_len && params->key) {
		crypt->key_len = params->key_len;
		memcpy(crypt->key, (u8 *)params->key, params->key_len);
	}
	crypt->pairwise = pairwise;
	crypt->key_idx = key_index;
	crypt->driver_cipher = rtw_80211_cipher_suite_to_driver(params->cipher);
	if (pairwise && mac_addr) {
		memcpy(crypt->mac_addr, mac_addr, 6);
	}

#ifndef CONFIG_FULLMAC_HCI_IPC
	if (pairwise) {	/*https://jira.realtek.com/browse/RSWLANDIOT-9403*/
		while (1) {
			if (list_empty(&(xmit_priv->queue_head)) == true) {
				break;
			}
			msleep(1);
		}
	}
#endif

	ret = whc_fullmac_host_add_key(crypt);
exit:

	kfree(crypt);
	return ret;
}

static int whc_fullmac_host_get_key(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
									, int link_id
#endif
									, u8 keyid
									, bool pairwise
									, const u8 *mac_addr, void *cookie
									, void (*callback)(void *cookie, struct key_params *))
{
	dev_dbg(global_idev.fullmac_dev, "--- %s --- !!!!!!!!!!", __func__);
	return 0;
}

static int whc_fullmac_host_del_key(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
									, int link_id
#endif
									, u8 key_index, bool pairwise, const u8 *mac_addr)

{
	dev_dbg(global_idev.fullmac_dev, "--- %s --- !!!!!!!!!!!!", __func__);
	return 0;
}

static int whc_fullmac_host_set_default_key(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
		, int link_id
#endif
		, u8 key_index, bool unicast, bool multicast)
{
	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);
	return 0;
}

static int whc_fullmac_host_set_default_mgmt_key(struct wiphy *wiphy, struct net_device *ndev
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
		, int link_id
#endif
		, u8 key_index)
{
	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);
	return 0;
}

#if defined(CONFIG_GTK_OL)
static int whc_fullmac_host_set_rekey_data(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_gtk_rekey_data *data)
{
	dev_dbg(global_idev.fullmac_dev, "[fullmac]: %s", __func__);
	return 0;
}
#endif /*CONFIG_GTK_OL*/

static int whc_fullmac_host_set_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa)
{
	struct rtw_pmksa_ops_t *pmksa_ops_vir = NULL;
	dma_addr_t		pmksa_ops_phy;

	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	pmksa_ops_vir = rtw_malloc(sizeof(struct rtw_pmksa_ops_t), &pmksa_ops_phy);
	if (!pmksa_ops_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	memcpy(pmksa_ops_vir->pmkid, pmksa->pmkid, 16);
	if (pmksa->pmk) {
		memcpy(pmksa_ops_vir->pmk, pmksa->pmk, 32);
	}
	if (pmksa->bssid) {
		dev_dbg(global_idev.fullmac_dev, "set [%02x:%02x:%02x:%02x:%02x:%02x] pmksa",
				*pmksa->bssid, *(pmksa->bssid + 1), *(pmksa->bssid + 2), *(pmksa->bssid + 3), *(pmksa->bssid + 4), *(pmksa->bssid + 5));
		memcpy(pmksa_ops_vir->mac_addr, pmksa->bssid, 6);
	}

	pmksa_ops_vir->wlan_idx = rtw_netdev_idx(ndev);
	pmksa_ops_vir->ops_id = PMKSA_SET;
	whc_fullmac_host_pmksa_ops(pmksa_ops_phy);

	if (pmksa_ops_vir) {
		rtw_mfree(sizeof(struct rtw_pmksa_ops_t), pmksa_ops_vir, pmksa_ops_phy);
	}

	return 0;
}

static int whc_fullmac_host_del_pmksa(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_pmksa *pmksa)
{
	struct rtw_pmksa_ops_t	*pmksa_ops_vir = NULL;
	dma_addr_t		pmksa_ops_phy;

	dev_dbg(global_idev.fullmac_dev, "--- %s ---", __func__);

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	pmksa_ops_vir = rtw_malloc(sizeof(struct rtw_pmksa_ops_t), &pmksa_ops_phy);
	if (!pmksa_ops_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	if (pmksa->pmkid) {
		memcpy(pmksa_ops_vir->pmkid, pmksa->pmkid, 16);
	}
	if (pmksa->bssid) {
		dev_dbg(global_idev.fullmac_dev, "delete [%02x:%02x:%02x:%02x:%02x:%02x] pmksa",
				*pmksa->bssid, *(pmksa->bssid + 1), *(pmksa->bssid + 2), *(pmksa->bssid + 3), *(pmksa->bssid + 4), *(pmksa->bssid + 5));
		memcpy(pmksa_ops_vir->mac_addr, pmksa->bssid, 6);
	}

	pmksa_ops_vir->wlan_idx = rtw_netdev_idx(ndev);
	pmksa_ops_vir->ops_id = PMKSA_DEL;
	whc_fullmac_host_pmksa_ops(pmksa_ops_phy);

	if (pmksa_ops_vir) {
		rtw_mfree(sizeof(struct rtw_pmksa_ops_t), pmksa_ops_vir, pmksa_ops_phy);
	}

	return 0;
}

static int whc_fullmac_host_flush_pmksa(struct wiphy *wiphy, struct net_device *ndev)
{
	struct rtw_pmksa_ops_t *pmksa_ops_vir = NULL;
	dma_addr_t		pmksa_ops_phy;

	dev_dbg(global_idev.fullmac_dev, "--- %s --- ", __func__);

	if (whc_fullmac_host_dev_driver_is_mp()) {
		return -EPERM;
	}

	pmksa_ops_vir = rtw_malloc(sizeof(struct rtw_pmksa_ops_t), &pmksa_ops_phy);
	if (!pmksa_ops_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	pmksa_ops_vir->wlan_idx = rtw_netdev_idx(ndev);
	pmksa_ops_vir->ops_id = PMKSA_FLUSH;
	whc_fullmac_host_pmksa_ops(pmksa_ops_phy);

	if (pmksa_ops_vir) {
		rtw_mfree(sizeof(struct rtw_pmksa_ops_t), pmksa_ops_vir, pmksa_ops_phy);
	}

	return 0;
}

void whc_fullmac_host_ops_key_init(void)
{
	struct cfg80211_ops *ops = &global_idev.rtw_cfg80211_ops;

	ops->add_key = whc_fullmac_host_add_key_ops;
	ops->get_key = whc_fullmac_host_get_key;
	ops->del_key = whc_fullmac_host_del_key;
	ops->set_default_key = whc_fullmac_host_set_default_key;
	ops->set_default_mgmt_key = whc_fullmac_host_set_default_mgmt_key;
	ops->set_pmksa = whc_fullmac_host_set_pmksa;
	ops->del_pmksa = whc_fullmac_host_del_pmksa;
	ops->flush_pmksa = whc_fullmac_host_flush_pmksa;
#if defined(CONFIG_GTK_OL)
	ops->set_rekey_data = whc_fullmac_host_set_rekey_data;
#endif /*CONFIG_GTK_OL*/
}
