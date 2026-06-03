#include <whc_host_linux.h>

#ifdef CONFIG_WHCH

u32 whc_host_hal_hwqueue_get(u8 qsel)
{
	u32 addr;

	switch (qsel) {
	case 0:
	case 3:
		addr = BE_QUEUE_INX;
		break;
	case 1:
	case 2:
		addr = BK_QUEUE_INX;
		break;
	case 4:
	case 5:
		addr = VI_QUEUE_INX;
		break;
	case 6:
	case 7:
		addr = VO_QUEUE_INX;
		break;
	case QSLT_HIGH://BC/MC in PS (HIQ)
		addr = HIGH_QUEUE_INX;
		break;
	default:
		addr = MGT_QUEUE_INX;
		break;
	}

	return addr;
}

int whc_host_hal_txbd_enough_check(void)
{
	return 1;
}

u8 whc_host_hal_txdesc_sc_mapping(u8 bwmode, enum channel_width CurrentChannelBW, u8 pri_ch, u8 central_ch)
{
	u8	SCSettingOfDesc = 0;

	if (CurrentChannelBW == CHANNEL_WIDTH_40) {
		if (bwmode == CHANNEL_WIDTH_40) {
			SCSettingOfDesc = 0;
		} else if (bwmode == CHANNEL_WIDTH_20) {
			SCSettingOfDesc = pri_ch > central_ch ?  1 : 2;
		}
	} else if (CurrentChannelBW == CHANNEL_WIDTH_80) {
		if (bwmode == CHANNEL_WIDTH_80) {
			SCSettingOfDesc = 0;
		} else if (bwmode == CHANNEL_WIDTH_40) {
			SCSettingOfDesc = pri_ch > central_ch ? 9 : 10;
		} else {
			if (pri_ch > central_ch) {
				SCSettingOfDesc = (pri_ch - central_ch) >> 1;
			} else {
				SCSettingOfDesc = ((central_ch - pri_ch) >> 1) + 1;
			}
		}
	} else {
		SCSettingOfDesc = 0;
	}

	return SCSettingOfDesc;
}

u8 whc_host_hal_txdesc_bw_mapping(u8 bwmode, enum channel_width CurrentChannelBW)
{
	u8	BWSettingOfDesc = 0;

	if (CurrentChannelBW == CHANNEL_WIDTH_40) {
		if ((bwmode == CHANNEL_WIDTH_40) || (bwmode == CHANNEL_WIDTH_80)) {
			BWSettingOfDesc = 1;
		} else {
			BWSettingOfDesc = 0;
		}
	} else if (CurrentChannelBW == CHANNEL_WIDTH_80) {
		if (bwmode == CHANNEL_WIDTH_80) {
			BWSettingOfDesc = 2;
		} else if (bwmode == CHANNEL_WIDTH_40) {
			BWSettingOfDesc = 1;
		} else {
			BWSettingOfDesc = 0;
		}
	} else {
		BWSettingOfDesc = 0;
	}

	return BWSettingOfDesc;
}

u8 whc_host_hal_txdesc_fill_sectype(u8 encrypt, u8 bswenc)
{
	u8 sectype = 0;

	if ((encrypt > 0) && !bswenc) {
		switch (encrypt) {
		// SEC_TYPE
		case _WEP40_:
		case _WEP104_:
		case _TKIP_:
		case _TKIP_WTMIC_:
			sectype = 1;
			break;
		case _SMS4_:
		case _GCMP_256_:
			sectype = 2;
			break;
		case _AES_:
			sectype = 3;
			break;

		case _NO_PRIVACY_:
		default:
			break;
		}
	}
	return sectype;
}

void whc_host_hal_txdesc_fill_vcs(struct txdesc_priv *ptxdesc, u8 vcs_mode, u8 cur_channel, u8 preamble_mode)
{
	if (vcs_mode) {
		switch (vcs_mode) {
		case RTS_CTS_BY_HW:
			ptxdesc->rtsen = 1;
			// ENABLE HW RTS
			ptxdesc->hw_rts_en = 1;
			break;
		case RTS_CTS_FORCE:
			ptxdesc->rtsen = 1;
			break;
		case CTS_TO_SELF_FORCE:
			ptxdesc->cts2self = 1;
			break;

		case NONE_VCS:
		default:
			break;
		}

		ptxdesc->rtsrate = 8; // RTS Rate=24M

		if (cur_channel > 14) {
			/* for 5G. OFDM 6M */
			ptxdesc->rts_rty_lowest_rate = 4;
		} else {
			/* for 2.4G. CCK 1M */
			ptxdesc->rts_rty_lowest_rate = 0;
		}

		if (preamble_mode == PREAMBLE_SHORT) {
			ptxdesc->rts_short = 1;
		}
	}
}

void whc_host_hal_txdesc_fill(struct xmit_frame *pxmitframe, u8 *pbuf)
{
	struct mlme_info *pmlmeinfo = &global_idev.mlmeinfo[pxmitframe->iface_type];
	struct security_priv *psecuritypriv = &global_idev.securitypriv[pxmitframe->iface_type];
	struct pkt_attrib *pattrib;
	struct txdesc_priv *ptxdesc;
	s32 bmcst;
	u8 auth_seq_num = 0;
	u8 iface_type = pxmitframe->iface_type;

	memset(pbuf, 0, TXDESC_SIZE);

	pattrib = &pxmitframe->attrib;
	bmcst = IS_MCAST(pattrib->ra);

	ptxdesc = (struct txdesc_priv *)pbuf;

	/* sw use field */
	ptxdesc->sw_use_pkt_type = pattrib->pkt_type;
	if (pattrib->tx_done_id == 1) {
		ptxdesc->sw_use_eapol4 = 1;
	}
	if (pattrib->issue_addba == 1) {
		ptxdesc->sw_use_addba = 1;
	}

	if (pxmitframe->frame_tag == DATA_FRAMETAG) {
		//softAP encrypted bcmc using EN_DESC_ID and CAMID. When EN_DESC_ID is set, no CAM search is done.
		if ((iface_type == WHC_AP_PORT) && pattrib->encrypt &&
			(bmcst || psecuritypriv->dot11PrivacyAlgrthm == _WEP40_ || psecuritypriv->dot11PrivacyAlgrthm == _WEP104_)) {
			ptxdesc->en_desc_id = 1;
			ptxdesc->macid = AP_GTK_TX_KEYCAM_ID;  //CAM_ID
		} else if (pattrib->b_tx_raw && pattrib->encrypt) {
			ptxdesc->en_desc_id = 1;
			ptxdesc->macid = (pattrib->mac_id == 0) ? STA_PTK_TRX_KEYCAM_ID : (pattrib->mac_id + AP_PTK_TRX_KEYCAM_ID_OFFSET);  //CAM_ID
		} else if ((iface_type == WHC_NAN_PORT) && pattrib->encrypt && bmcst && !pattrib->b_swenc) {
			/* NAN bcmc hw encrypt */
			ptxdesc->en_desc_id = 1;
			ptxdesc->macid = NAN_GTK_TX_KEYCAM_ID;  //CAM_ID
		} else {
			ptxdesc->macid = pattrib->mac_id;  //MAC_ID
		}
		ptxdesc->port_id = iface_type;
		ptxdesc->qsel = pattrib->qsel;
		ptxdesc->seq = pattrib->seqnum;

		ptxdesc->sectype = whc_host_hal_txdesc_fill_sectype(pattrib->encrypt, pattrib->b_swenc);

		whc_host_hal_txdesc_fill_vcs(ptxdesc, pattrib->vcs_mode, pmlmeinfo->cur_channel, pmlmeinfo->b_preamble_mode);

		if (pattrib->b_ht_en) {
			ptxdesc->rts_sc = whc_host_hal_txdesc_sc_mapping(pattrib->bwmode, global_idev.cur_chandef.bw,
							  global_idev.cur_chandef.chan, global_idev.cur_chandef.center_ch);
		}

		if ((pattrib->ether_type != ETH_P_PAE) &&
			(pattrib->ether_type != ETH_P_ARP) &&
			(pattrib->ether_type != 0x88B4) &&
			(pattrib->pkt_type != PACKET_DHCP)
		   ) {
			if (pattrib->b_ht_en) {
				ptxdesc->data_bw = whc_host_hal_txdesc_bw_mapping(pattrib->bwmode, global_idev.cur_chandef.bw);
				ptxdesc->data_sc = whc_host_hal_txdesc_sc_mapping(pattrib->bwmode, global_idev.cur_chandef.bw,
								   global_idev.cur_chandef.chan, global_idev.cur_chandef.center_ch);
			}

			if (!bmcst) {
				if (pmlmeinfo->cur_channel > 14) {
					/* for 5G. OFDM 6M */
					ptxdesc->data_rty_lowest_rate = 4;
				} else {
					/* for 2.4G. CCK 1M */
					ptxdesc->data_rty_lowest_rate = 0;
				}
			} else {
				/* Avoid indexing wrong rate according to macid 31 from encrypted broadcast data frames
				https://jira.realtek.com/browse/RSWLANDIOT-8185 */
				ptxdesc->userate = 1;
				ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(global_idev.mlmepriv.user_tx_rate);
			}
#if 0
			if (pmlmeinfo->fix_rate != 0xFF) {
				ptxdesc->datarate = GET_FIX_RATE(pmlmeinfo->fix_rate);
				ptxdesc->data_gi = GET_FIX_RATE_SGI(pmlmeinfo->fix_rate);
				ptxdesc->data_er = GET_FIX_RATE_ER_EN(pmlmeinfo->fix_rate);
				ptxdesc->data_bw_er = GET_FIX_RATE_ER_BW(pmlmeinfo->fix_rate);
				ptxdesc->disdatafb = 1;
				ptxdesc->userate = 1;
			}
#endif
			if (global_idev.wifi_user_config.rtw_trp_tis_cert_en == 1) { //disable date fallback under trp tis test
				ptxdesc->disdatafb = 1;
			}

			if (pattrib->b_tx_navusehdr) {
				ptxdesc->navusehdr = 1;
			}
		} else {
			/*
			*  EAP & ARP & DHCP packet.
			* Use the 6M data rate to send the EAP/ARP packet.
			* This will maybe make the handshake smooth.
			*/
			ptxdesc->bk = 1; // AGG BK
			ptxdesc->userate = 1; // driver uses rate
			ptxdesc->rty_use_des = 1;

			if (pmlmeinfo->b_preamble_mode == PREAMBLE_SHORT) {
				ptxdesc->data_gi = 1;	// DATA_SHORT
			}

			ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(global_idev.mlmepriv.user_tx_rate);
			ptxdesc->retry_limit = 48;

			/*https://jira.realtek.com/browse/RSWLANDIOT-12164*/
			ptxdesc->rtsen = 0;
			ptxdesc->hw_rts_en = 0;
			ptxdesc->cts2self = 0;

		}
	} else if (pxmitframe->frame_tag == MGNT_FRAMETAG) {
		/* TODO: check whether need mgnt flow */
#ifdef CONFIG_PMF_USE_HW_CRYPTO
		struct sta_security_priv *psta_security = wifi_rom_rtw_sta_get_secpriv(iface_type, pattrib->ra);
#endif
		//softAP encrypted bcmc using EN_DESC_ID and CAMID. When EN_DESC_ID is set, no CAM search is done.
		if ((iface_type == WHC_AP_PORT) && (pattrib->encrypt && bmcst)) {
			ptxdesc->en_desc_id = 1;
			ptxdesc->macid = 30;			//CAM_ID
		} else {
			ptxdesc->macid = pattrib->mac_id;	 // CAM_ID(MAC_ID)
		}

		ptxdesc->qsel = pattrib->qsel;
		ptxdesc->seq = pattrib->seqnum;
		ptxdesc->userate = 1; // driver uses rate, 1M

		ptxdesc->rty_use_des = 1;

		ptxdesc->retry_limit = 12;

#ifdef CONFIG_PMF_USE_HW_CRYPTO
		/* when enable 802.11w, need enc mgnt frame: please ensure correct settings for pattrib->encrypt and pattrib->b_swenc */
		if ((psecuritypriv->ieee80211w != NO_MGMT_FRAME_PROTECTION)
			&& (psecuritypriv->b_installBIPkey == true)
			&& (psta_security && psta_security->b_pairwise_key_installed == true)) {
			ptxdesc->sectype = whc_host_hal_txdesc_fill_sectype(pattrib->encrypt, pattrib->b_swenc);
		}
#endif

		/* enlarge retry limit for sae confirm-auth(auth seq=2) */
		if (pattrib->type_subtype == RTW_AUTH) {
			auth_seq_num = *(u8 *)(pxmitframe->pkt->data + pattrib->hdrlen + 2);
			if (auth_seq_num == 2) {
				ptxdesc->retry_limit = 24;
			}
		}

		/* if user config retry limit, overwrite with user config's value*/
		if (pattrib->retry_limit_by_user) {
			ptxdesc->retry_limit = pattrib->retry_limit_by_user;
		}

		if (pattrib->rate) {
			ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(pattrib->rate);
		} else

		{
			ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(global_idev.mlmepriv.user_tx_rate);
		}

		if (pattrib->b_tx_navusehdr) {
			ptxdesc->navusehdr = 1;
		}
	} else {
		//softAP encrypted bcmc using EN_DESC_ID and CAMID. When EN_DESC_ID is set, no CAM search is done.
		ptxdesc->macid = pattrib->mac_id;	 // CAM_ID(MAC_ID)
		ptxdesc->qsel = pattrib->qsel;
		ptxdesc->seq = pattrib->seqnum;
		ptxdesc->userate = 1; // driver uses rate
		ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(global_idev.mlmepriv.user_tx_rate);
	}

	ptxdesc->pktlen = pattrib->last_txcmdsz;
	ptxdesc->offset = TXDESC_SIZE;

	if (bmcst) {
		ptxdesc->bmc = 1;
	}

	/*
	* 2009.11.05. tynli_test. Suggested by SD4 Filen for FW LPS.
	* (1) The sequence number of each non-Qos frame / non-Qos bmcast frame /
	* mgnt frame should be controled by Hw because Fw will also send null data
	* which we cannot control when Fw LPS enable.
	* --> default enable non-Qos data sequense number. 2010.06.23. by tynli.
	* (2) Enable HW SEQ control for beacon packet, because we use Hw beacon.
	* (3) Use HW Qos SEQ to control the seq num of Ext port non-Qos packets.
	* 2010.06.23. Added by tynli.
	*/
	if (!pattrib->b_qos_en) {
		// Hw set sequence number
		ptxdesc->en_hwseq = 1; // HWSEQ_EN
		if (iface_type == WHC_AP_PORT) {
			ptxdesc->hw_ssn_sel = 1;
		}
	}

#ifdef CONFIG_NAN
	wifi_hal_nan_update_txdesc(pxmitframe, pbuf);
#endif

#ifdef CONFIG_WIFI_TUNNEL
	if (pattrib->wtunnel_tx_swap) {
		ptxdesc->retry_limit = 24;
		ptxdesc->en_hwseq = 0;
		ptxdesc->tx_swap = 1;
		ptxdesc->ul_swap = pattrib->wtunnel_ul_swap;
		dev_dbg(global_idev.pwhc_dev, "[whc] relay pkt: tx_swap=%d ul_swap=%d qsel=%d en_hwseq=%d seqnum=%d\n",
				ptxdesc->tx_swap, ptxdesc->ul_swap, ptxdesc->qsel, ptxdesc->en_hwseq, pattrib->seqnum);
	}
	wifi_tunnelapi_update_txdata_macswap(ptxdesc, (u8 *)(pxmitframe->pkt->data), 0);
#endif

	/*don't use cck rate in 5G*/
	if ((global_idev.cur_chandef.chan > 14) && ptxdesc->userate && ptxdesc->datarate < DESC_RATE6M) {
		ptxdesc->datarate = DESC_RATE6M;
	}

	if (pattrib->b_tx_raw) {
		ptxdesc->userate = 1;
		ptxdesc->datarate = whc_host_hal_mrate_to_hwrate(pattrib->rate);
		ptxdesc->rty_use_des = 1;
		ptxdesc->retry_limit = pattrib->retry_limit_by_user;
		ptxdesc->data_gi = pattrib->b_tx_sgi;
		ptxdesc->en_hwseq = 0;  /* seq maintain by sw when raw tx */
	}
}

#endif