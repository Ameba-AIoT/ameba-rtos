// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>

int dummy(struct net_device *dev, struct iw_request_info *a,
		  union iwreq_data *wrqu, char *b)
{
	return -1;
}

static iw_handler rtw_handlers[] = {
#ifdef TODO
	NULL,					/* SIOCSIWCOMMIT */
	rtw_wx_get_name,		/* SIOCGIWNAME */
	dummy,					/* SIOCSIWNWID */
	dummy,					/* SIOCGIWNWID */
	rtw_wx_set_freq,		/* SIOCSIWFREQ */
	rtw_wx_get_freq,		/* SIOCGIWFREQ */
	rtw_wx_set_mode,		/* SIOCSIWMODE */
	rtw_wx_get_mode,		/* SIOCGIWMODE */
	dummy,					/* SIOCSIWSENS */
	rtw_wx_get_sens,		/* SIOCGIWSENS */
	NULL,					/* SIOCSIWRANGE */
	rtw_wx_get_range,		/* SIOCGIWRANGE */
	rtw_wx_set_priv,		/* SIOCSIWPRIV */
	NULL,					/* SIOCGIWPRIV */
	NULL,					/* SIOCSIWSTATS */
	NULL,					/* SIOCGIWSTATS */
	dummy,					/* SIOCSIWSPY */
	dummy,					/* SIOCGIWSPY */
	NULL,					/* SIOCGIWTHRSPY */
	NULL,					/* SIOCWIWTHRSPY */
	rtw_wx_set_wap,		/* SIOCSIWAP */
	rtw_wx_get_wap,		/* SIOCGIWAP */
	rtw_wx_set_mlme,		/* request MLME operation; uses struct iw_mlme */
	dummy,					/* SIOCGIWAPLIST -- depricated */
	rtw_wx_set_scan,		/* SIOCSIWSCAN */
	rtw_wx_get_scan,		/* SIOCGIWSCAN */
	rtw_wx_set_essid,		/* SIOCSIWESSID */
	rtw_wx_get_essid,		/* SIOCGIWESSID */
	dummy,					/* SIOCSIWNICKN */
	rtw_wx_get_nick,		/* SIOCGIWNICKN */
	NULL,					/* -- hole -- */
	NULL,					/* -- hole -- */
	rtw_wx_set_rate,		/* SIOCSIWRATE */
	rtw_wx_get_rate,		/* SIOCGIWRATE */
	rtw_wx_set_rts,			/* SIOCSIWRTS */
	rtw_wx_get_rts,			/* SIOCGIWRTS */
	rtw_wx_set_frag,		/* SIOCSIWFRAG */
	rtw_wx_get_frag,		/* SIOCGIWFRAG */
	dummy,					/* SIOCSIWTXPOW */
	dummy,					/* SIOCGIWTXPOW */
	dummy,					/* SIOCSIWRETRY */
	rtw_wx_get_retry,		/* SIOCGIWRETRY */
	rtw_wx_set_enc,			/* SIOCSIWENCODE */
	rtw_wx_get_enc,			/* SIOCGIWENCODE */
	dummy,					/* SIOCSIWPOWER */
	rtw_wx_get_power,		/* SIOCGIWPOWER */
	NULL,					/*---hole---*/
	NULL,					/*---hole---*/
	rtw_wx_set_gen_ie,		/* SIOCSIWGENIE */
	NULL,					/* SIOCGWGENIE */
	rtw_wx_set_auth,		/* SIOCSIWAUTH */
	NULL,					/* SIOCGIWAUTH */
	rtw_wx_set_enc_ext,		/* SIOCSIWENCODEEXT */
	NULL,					/* SIOCGIWENCODEEXT */
	rtw_wx_set_pmkid,		/* SIOCSIWPMKSA */
	NULL,					/*---hole---*/
#endif
};

#ifdef CONFIG_WOWLAN
static int rtw_wowlan_ctrl(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	struct inic_sdio *priv = &inic_sdio_priv;

	ret = llhw_wifi_scan_abort(0);
	if (ret) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: %s abort wifi scan failed!\n", __func__);
		return -EPERM;
	}

	if (memcmp(extra, "enable", 6) == 0) {
		dev_info(global_idev.fullmac_dev, "enable wow_mode");
		rtw_sdio_suspend(&priv->func->dev);

	} else if (memcmp(extra, "disable", 7) == 0) {
		dev_info(global_idev.fullmac_dev, "disable wow_mode");
		rtw_sdio_resume(&priv->func->dev);

	} else {
		dev_err(global_idev.fullmac_dev, "[%s] Invalid Parameter.\n", __func__);
	}

	return ret;
}

/*
 * IP filter This pattern if for a frame containing a ip packet:
 * AA:AA:AA:AA:AA:AA:BB:BB:BB:BB:BB:BB:CC:CC:DD:-:-:-:-:-:-:-:-:EE:-:-:FF:FF:FF:FF:GG:GG:GG:GG:HH:HH:II:II
 *
 * A: Ethernet destination address
 * B: Ethernet source address
 * C: Ethernet protocol type
 * D: IP header VER+Hlen, use: 0x45 (4 is for ver 4, 5 is for len 20)
 * E: IP protocol
 * F: IP source address ( 192.168.0.4: C0:A8:00:2C )
 * G: IP destination address ( 192.168.0.4: C0:A8:00:2C )
 * H: Source port (1024: 04:00)
 * I: Destination port (1024: 04:00)
 */

static int rtw_wowlan_set_pattern(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wrqu, char *extra)
{
#ifdef TODO
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wowlan_ioctl_param poidparam;
	int ret = 0;
	u8 input[wrqu->data.length + 1];
	u8 index = 0;

	poidparam.subcode = 0;

	if (!check_fwstate(pmlmepriv, WIFI_ASOC_STATE) &&
		check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		ret = -EFAULT;
		RTW_INFO("Please Connect With AP First!!\n");
		goto _rtw_wowlan_set_pattern_exit;
	}

	if (wrqu->data.length <= 0) {
		ret = -EFAULT;
		RTW_INFO("ERROR: parameter length <= 0\n");
		goto _rtw_wowlan_set_pattern_exit;
	} else {
		/* set pattern */
		if (copy_from_user(input,
						   wrqu->data.pointer, wrqu->data.length)) {
			return -EFAULT;
		}

		input[wrqu->data.length] = '\0';

		/* leave PS first */
		rtw_ps_deny(padapter, PS_DENY_IOCTL);
		LeaveAllPowerSaveModeDirect(padapter);
		if (strncmp(input, "pattern=", 8) == 0) {
			if (pwrpriv->wowlan_pattern_idx >= MAX_WKFM_CAM_NUM) {
				RTW_INFO("WARNING: priv-pattern is full(idx: %d)\n",
						 pwrpriv->wowlan_pattern_idx);
				RTW_INFO("WARNING: please clean priv-pattern first\n");
				ret = -EINVAL;
				goto _rtw_wowlan_set_pattern_exit;
			} else {
				index = pwrpriv->wowlan_pattern_idx;
				ret = rtw_wowlan_parser_pattern_cmd(input,
													pwrpriv->patterns[index].content,
													&pwrpriv->patterns[index].len,
													pwrpriv->patterns[index].mask);

				if (ret == _TRUE) {
					pwrpriv->wowlan_pattern_idx++;
				}
			}
		} else if (strncmp(input, "clean", 5) == 0) {
			poidparam.subcode = WOWLAN_PATTERN_CLEAN;
			rtw_hal_set_hwreg(padapter,
							  HW_VAR_WOWLAN, (u8 *)&poidparam);
		} else if (strncmp(input, "show", 4) == 0) {
			rtw_wow_pattern_cam_dump(padapter);
			rtw_wow_pattern_sw_dump(padapter);
		} else {
			RTW_INFO("ERROR: incorrect parameter!\n");
			ret = -EINVAL;
		}
		rtw_ps_deny_cancel(padapter, PS_DENY_IOCTL);
	}
_rtw_wowlan_set_pattern_exit:
	return ret;
#else
	return 0;
#endif
}
#endif /* CONFIG_WOWLAN */

static int rtw_priv_set(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;

	if (extra == NULL) {
		wrqu->length = 0;
		return -EIO;
	}

	switch (subcmd) {
#ifdef CONFIG_WOWLAN
	case MP_WOW_ENABLE:
		rtw_wowlan_ctrl(dev, info, wdata, extra);
		break;
	case MP_WOW_SET_PATTERN:
		rtw_wowlan_set_pattern(dev, info, wdata, extra);
		break;
#endif
	default:
		return -EIO;
	}

	return 0;
}


static int rtw_priv_get(struct net_device *dev, struct iw_request_info *info, union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;

	if (extra == NULL) {
		wrqu->length = 0;
		return -EIO;
	}

	switch (subcmd) {
	default:
		return -EIO;
	}

	return 0;
}

static const struct iw_priv_args rtw_private_args[] = {

	{ SIOCIWFIRSTPRIV + 0x0, IW_PRIV_TYPE_CHAR | 1024, 0, ""},   /* set  */
	{ SIOCIWFIRSTPRIV + 0x1, IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, ""}, /* get */

	/* --- sub-ioctls definitions --- */
#ifdef CONFIG_WOWLAN
	{ MP_WOW_ENABLE, IW_PRIV_TYPE_CHAR | 1024, 0, "wow_mode" },
	{ MP_WOW_SET_PATTERN, IW_PRIV_TYPE_CHAR | 1024, 0, "wow_set_pattern" },
#endif

};

static iw_handler rtw_private_handler[] = {
	rtw_priv_set,					/* 0x00 */
	rtw_priv_get,					/* 0x01 */
};

#ifdef CONFIG_WIRELESS_EXT
struct iw_handler_def rtw_handlers_def = {
	.standard = rtw_handlers,
	.num_standard = sizeof(rtw_handlers) / sizeof(iw_handler),

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)) || defined(CONFIG_WEXT_PRIV)
	.private = rtw_private_handler,
	.private_args = rtw_private_args,
	.num_private = sizeof(rtw_private_handler) / sizeof(iw_handler),
	.num_private_args = sizeof(rtw_private_args) / sizeof(struct iw_priv_args),
#endif

#ifdef TODO
#if WIRELESS_EXT >= 17
	.get_wireless_stats = rtw_get_wireless_stats,
#endif
#endif
};
#endif

