// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC (Inter Process
*   Communication) bus, SPI, SDIO and USB.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#include <whc_fullmac_host_ft.h>

#ifdef CONFIG_IEEE80211R

#define MAX_FT_AUTH_LEN (300)
#define FT_REAUTH_LIMIT (4)
#define REAUTH_TO (800)

#define _FT_CHECK_STATUS(s) (lacal_ft_priv.ft_status == (s))
#define _FT_SET_STATUS(s) \
	do { \
		dev_dbg(global_idev.fullmac_dev, "Ft status: %s => %s.\n", _ft_str_status[lacal_ft_priv.ft_status], _ft_str_status[s]); \
		lacal_ft_priv.ft_status = (s); \
	} while (0)

struct whc_fullmac_host_ft_priv_t {
	struct timer_list ft_auth_timer;
	u8 reauth_count;
	u8 ft_status;
	u8 target_roam_bssid[ETH_ALEN];
	u16 mdid;
	u8 ie[5 + MAX_WPA_IE_LEN + MAX_FT_IE_LEN];
	u32 ielen;
};

enum _rtw_ft_sta_status {
	_FT_UNASSOCIATED_STA = 0,
	_FT_AUTHENTICATING_STA,
	_FT_AUTHENTICATED_STA,
	_FT_ASSOCIATING_STA,
	_FT_ASSOCIATED_STA,
};

static const char *_ft_str_status[] = {
	"ft unassociated",
	"ft authenticating",
	"ft authenticated",
	"ft associating",
	"ft associated",
};

static struct whc_fullmac_host_ft_priv_t lacal_ft_priv = {0};

static int _whc_fullmac_host_ft_auth_tx(void);

static void _whc_fullmac_host_ft_auth_timer_hdl(struct timer_list *t)
{
	if (_FT_CHECK_STATUS(_FT_AUTHENTICATING_STA)) {
		if (++lacal_ft_priv.reauth_count > FT_REAUTH_LIMIT) {
			_FT_SET_STATUS(_FT_UNASSOCIATED_STA);
			cfg80211_disconnected(global_idev.pndev[WHC_STA_PORT], 0, NULL, 0, 1, GFP_ATOMIC);
			whc_fullmac_host_ft_status_indicate(NULL, WLAN_STATUS_AUTH_TIMEOUT);
		}
		_whc_fullmac_host_ft_auth_tx();
		mod_timer(&lacal_ft_priv.ft_auth_timer, jiffies + msecs_to_jiffies(REAUTH_TO));
	}
}

static int _whc_fullmac_host_ft_auth_tx(void)
{
	struct whc_fullmac_host_ft_priv_t *ft_priv = &lacal_ft_priv;
	unsigned char buf[MAX_FT_AUTH_LEN] = {0};
	struct ieee80211_mgmt *auth = (void *)buf;
	u32 pkt_len = 0;
	struct element *pelem = NULL;
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "%s enter\n", __func__);
	if (!ft_priv->target_roam_bssid) {
		ret = -EINVAL;
		dev_err(global_idev.fullmac_dev, "%s, FT no target_roam_bssid!\n", __func__);
		goto func_exit;
	}
	memcpy(auth->da, ft_priv->target_roam_bssid, ETH_ALEN);
	memcpy(auth->sa, global_idev.pndev[WHC_STA_PORT]->dev_addr, ETH_ALEN);
	memcpy(auth->bssid, ft_priv->target_roam_bssid, ETH_ALEN);

	auth->frame_control = cpu_to_le16(IEEE80211_STYPE_AUTH | IEEE80211_FTYPE_MGMT);
	auth->u.auth.auth_alg = cpu_to_le16(2);  /*auth algo*/
	auth->u.auth.auth_transaction = cpu_to_le16(1);  /*auth algo*/
	auth->u.auth.status_code = cpu_to_le16(0);  /*status*/

	memcpy(auth->u.auth.variable, ft_priv->ie, ft_priv->ielen);
	pkt_len = offsetof(struct ieee80211_mgmt, u.auth.variable) + ft_priv->ielen;

	ret = whc_fullmac_host_tx_mgnt(WHC_STA_PORT, buf, pkt_len, 0);

func_exit:
	dev_dbg(global_idev.fullmac_dev, "%s exit %d\n", __func__, ret);
	return ret;
}

static int _whc_fullmac_host_ft_auth_resp_process(u8 *pframe, u32 pkt_len)
{
	struct ieee80211_mgmt *mgmt = (void *)pframe;
	u16 status_code = le16_to_cpu(mgmt->u.auth.status_code);
	struct cfg80211_ft_event_params ft_evt_parms = {0};
	struct element *pelem = NULL;
	u8 *ies = NULL;
	u32 ies_len = 0;

	dev_dbg(global_idev.fullmac_dev, "%s===>\n", __func__);
	if (status_code != WLAN_STATUS_SUCCESS) {
		dev_dbg(global_idev.fullmac_dev, "%s, FT recv auth status: %d\n", __func__, status_code);
		return -ENOPROTOOPT;
	}
	ies = mgmt->u.auth.variable;
	ies_len = pkt_len - offsetof(struct ieee80211_mgmt, u.auth.variable);

	ft_evt_parms.target_ap = mgmt->sa;
	ft_evt_parms.ies = ies;
	ft_evt_parms.ies_len = ies_len;

	cfg80211_ft_event(global_idev.pndev[WHC_STA_PORT], &ft_evt_parms);

	return 0;
}

static int _whc_fullmac_host_ft_auth_rx(u8 *pframe, u32 pkt_len)
{
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "%s===>\n", __func__);
	if (_FT_CHECK_STATUS(_FT_AUTHENTICATING_STA)) {
		del_timer_sync(&lacal_ft_priv.ft_auth_timer);
		ret = _whc_fullmac_host_ft_auth_resp_process(pframe, pkt_len);
		if (ret < 0) {
			_FT_SET_STATUS(_FT_UNASSOCIATED_STA);
			cfg80211_disconnected(global_idev.pndev[WHC_STA_PORT], 0, NULL, 0, 1, GFP_ATOMIC);
			whc_fullmac_host_ft_status_indicate(NULL, WLAN_STATUS_CHALLENGE_FAIL);
		} else {
			_FT_SET_STATUS(_FT_AUTHENTICATED_STA);
		}
	}

	return ret;
}

static int _whc_fullmac_host_ft_auth_start(void)
{
	struct whc_fullmac_host_ft_priv_t *ft_priv = &lacal_ft_priv;
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "%s FT start auth to 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", \
			__func__, ft_priv->target_roam_bssid[0], ft_priv->target_roam_bssid[1], ft_priv->target_roam_bssid[2], \
			ft_priv->target_roam_bssid[3], ft_priv->target_roam_bssid[4], ft_priv->target_roam_bssid[5]);
	_FT_SET_STATUS(_FT_AUTHENTICATING_STA);
	_whc_fullmac_host_ft_auth_tx();

	lacal_ft_priv.reauth_count = 0;
	timer_setup(&lacal_ft_priv.ft_auth_timer, _whc_fullmac_host_ft_auth_timer_hdl, 0);
	mod_timer(&lacal_ft_priv.ft_auth_timer, jiffies + msecs_to_jiffies(REAUTH_TO));

func_exit:
	return ret;
}

static int _whc_fullmac_host_ft_rx_mgnt(char *evt_info)
{
	struct rtw_event_report_frame *evt_rpt_frm = (struct rtw_event_report_frame *)evt_info;
	struct ieee80211_mgmt *mgmt = (void *)(evt_rpt_frm->frame);
	int ret = 0;

	if (evt_rpt_frm->frame_len < 2) {
		dev_err(global_idev.fullmac_dev, "%s, FT length of frame < 2.\n", __func__);
		return -EINVAL;
	}

	dev_dbg(global_idev.fullmac_dev, "%s===>\n", __func__);
	if (ieee80211_is_auth(mgmt->frame_control)) {
		ret = _whc_fullmac_host_ft_auth_rx(evt_rpt_frm->frame, evt_rpt_frm->frame_len);
	} else if (ieee80211_is_assoc_resp(mgmt->frame_control) || \
			   ieee80211_is_reassoc_resp(mgmt->frame_control)) {
		_FT_SET_STATUS(_FT_ASSOCIATED_STA);
	} else {
		dev_err(global_idev.fullmac_dev, "%s, FT receive unexpected mgnt. fc %02x.\n", __func__, mgmt->frame_control);
	}

	return ret;
}

static void _whc_fullmac_host_ft_rx_join_status(unsigned int join_status)
{
	dev_dbg(global_idev.fullmac_dev, "FT join_status is changed to %d.\n", join_status);

	switch (join_status) {
	case RTW_JOINSTATUS_UNKNOWN:
	case RTW_JOINSTATUS_FAIL:
	case RTW_JOINSTATUS_DISCONNECT:
		_FT_SET_STATUS(_FT_UNASSOCIATED_STA);
		break;
	default:
		dev_dbg(global_idev.fullmac_dev, "FT status %d do nothing.\n", join_status);
		break;
	}
}

int whc_fullmac_host_ft_set_bssid(const u8 *target_bssid)
{
	struct whc_fullmac_host_ft_priv_t *ft_priv = &lacal_ft_priv;
	int ret = 0;

	if (!target_bssid) {
		dev_err(global_idev.fullmac_dev, "%s, target_bssid is NULL!\n", __func__);
		return -EINVAL;
	}
	dev_dbg(global_idev.fullmac_dev, "%s [0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x]\n", \
			__func__, target_bssid[0], target_bssid[1], target_bssid[2], target_bssid[3], target_bssid[4], target_bssid[5]);
	memcpy(ft_priv->target_roam_bssid, target_bssid, ETH_ALEN);

	return 0;
}

int whc_fullmac_host_ft_event(u32 event, char *evt_info, unsigned int join_status)
{
	int ret = 0;
	dev_dbg(global_idev.fullmac_dev, "%s ===>\n", __func__);

	switch (event) {
	case RTW_EVENT_FT_AUTH_START:
		_whc_fullmac_host_ft_auth_start();
		break;
	case RTW_EVENT_FT_RX_MGNT:
		_whc_fullmac_host_ft_rx_mgnt(evt_info);
		break;
	case RTW_EVENT_JOIN_STATUS:
		_whc_fullmac_host_ft_rx_join_status(join_status);
		break;
	default:
		ret = -EINVAL;
		dev_err(global_idev.fullmac_dev, "%s, event %d is not ft event!\n", __func__, event);
		break;
	}

	return ret;
}

int whc_fullmac_host_update_ft_ies(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_update_ft_ies_params *ftie)
{
	int i = 0;
	struct element *pelem = NULL;
	struct whc_fullmac_host_ft_priv_t *ft_priv = &lacal_ft_priv;
	struct rtw_kvr_param_t kvr_param = {0};
	u8 *pos = NULL;
	int ret = 0;

	dev_dbg(global_idev.fullmac_dev, "%s===>\n", __func__);
	/* store all ft ies to whc_dev_kvr_param */
	ft_priv->mdid = ftie->md;
	ft_priv->ielen = ftie->ie_len;
	memcpy(&ft_priv->ie[0], ftie->ie, ftie->ie_len);

	if (_FT_CHECK_STATUS(_FT_UNASSOCIATED_STA)) {
		/* first assoc, update ies for assoc request except rsnie */
		pos = &kvr_param.ie[0];
		kvr_param.ielen = 0;

		/* fill mdie */
		pelem = cfg80211_find_elem(WLAN_EID_MOBILITY_DOMAIN, ftie->ie, ftie->ie_len);
		if (pelem) {
			memcpy(pos, (u8 *)pelem, pelem->datalen + 2);
			pos += pelem->datalen + 2;
			kvr_param.ielen += pelem->datalen + 2;
		}

		/* fill ftie */
		pelem = cfg80211_find_elem(WLAN_EID_FAST_BSS_TRANSITION, ftie->ie, ftie->ie_len);
		if (pelem) {
			memcpy(pos, (u8 *)pelem, pelem->datalen + 2);
			pos += pelem->datalen + 2;
			kvr_param.ielen += pelem->datalen + 2;
		}
		/* not WLAN_STATUS_UNSPECIFIED_FAILURE, only ro transfer kvr_param */
		ret = whc_fullmac_host_ft_status_indicate(&kvr_param, WLAN_STATUS_UNSPECIFIED_FAILURE);
	} else if (_FT_CHECK_STATUS(_FT_AUTHENTICATED_STA)) {
		/* reassoc, update all ies for reassoc request */
		pos = &kvr_param.ie[0];
		kvr_param.ielen = 0;

		memcpy(pos, ftie->ie, ftie->ie_len);
		kvr_param.ielen = ftie->ie_len;

		ret = whc_fullmac_host_ft_status_indicate(&kvr_param, WLAN_STATUS_SUCCESS);
		_FT_SET_STATUS(_FT_ASSOCIATING_STA);
	}

	return ret;
}

void whc_fullmac_host_ft_set_unassociated(void)
{
	_FT_SET_STATUS(_FT_UNASSOCIATED_STA);
}

void whc_fullmac_host_ft_init(void)
{
	struct whc_fullmac_host_ft_priv_t *ft_priv = &lacal_ft_priv;

	dev_dbg(global_idev.fullmac_dev, "%s===>\n", __func__);
	memset(ft_priv, 0, sizeof(struct whc_fullmac_host_ft_priv_t));
	_FT_SET_STATUS(_FT_UNASSOCIATED_STA);
}

#endif /* CONFIG_IEEE80211R */
