#include "rtw_cfg80211_fullmac.h"

struct proxy_priv proxypriv;

static struct war_mdns_service_info default_sinfo[] = {
	RTW_MDNS_SRV_INFO("_ipp", 4, "_tcp", 4, "local", 5, 0x02, 0x77, 7200, "KM1", 3, 0),
	RTW_MDNS_SRV_INFO("_ipps", 5, "_tcp", 4, "local", 5, 0x02, 0x77, 7200, "KM2", 3, 0),
	RTW_MDNS_SRV_INFO("_http", 5, "_tcp", 4, "local", 5, 0x00, 0x50, 7200, "KM3", 3, 2),
	RTW_MDNS_SRV_INFO("_privet", 7, "_tcp", 4, "local", 5, 0x00, 0x50, 7200, "KM4", 3, 3),
	RTW_MDNS_SRV_INFO("_https", 6, "_tcp", 4, "local", 5, 0x01, 0xbb, 7200, "KM5", 3, 2),
	RTW_MDNS_SRV_INFO("_uscan", 6, "_tcp", 4, "local", 5, 0x1f, 0x91, 7200, "KM6", 3, 4),
	RTW_MDNS_SRV_INFO("_printer", 8, "_tcp", 4, "local", 5, 0x23, 0x8c, 7200, "KM7", 3, 1),
	RTW_MDNS_SRV_INFO("_pdl-datastream", 15, "_tcp", 4, "local", 5, 0x23, 0x8c, 7200, "KM8", 3, 1)
};

void rtw_proxy_init(void)
{
	/* init mDNS parameters */
	rtw_proxy_mdns_parms_init(1);
}

void rtw_proxy_mdns_parms_init(u8 is_set_default)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	u8 i = 0;
	u16	offset = 0;
	u8 default_domain_name[] = "Generic";
	u8 default_machine_name[] = { 0x0a, 0x5f, 0x75, 0x6e, 0x69, 0x76, 0x65, 0x72, 0x73, 0x61, 0x6c}; /* length : 10 name : _universal */
	u8 default_machine_name_len = 11;
#if 0
	//char *default_txt_rsp_0_for_serive[2] = { "_ipp", "_ipps" };
	char *default_txt_rsp_0[25] = {
		"txtvers=1", "qtotal=1", "usb_MFG=KONICA MINOLTA", "usb_MDL=C754Series",
		"rp=ipp/print", "priority=54", "tr=Generic 35c-4", "product=DriverName",
		"pdl=application/postscript,image/urf,application/octet-stream,image/jpeg",
		"adminurl=http://KM00D91C.local./wcd/a_network.xml",
		"note=Copy Room", "Transparent=T", "Binary=T", "TBCP=T",
		"URF=V1,4,w8,SRGB24,ADOBERGB24-48,DEVW8,DEVRGB24,DEVCMYK32,RS150000000,IS19-20-21,MT1-3,OB1,PQ4,DM1,FN3-14,CP255",
		"rfo=ipp/faxout", "Fax=T", "Scan=T", "Duplex=T", "Color=T", "air=none",
		"Kind=document,envelope,photo",
		"PaperMax=tabloid-A3", "UUID=6c183832-69ba-541b-baf6-6d947c144325", "TLS=1.2"
	};
#endif
	//char *default_txt_rsp_1_for_serive[2] = { "_printer", "_pdl-datastream" };
	char *default_txt_rsp_1[13] = {
		"txtvers=1", "qtotal=1", "usb_MFG=KONICA MINOLTA", "usb_MDL=C754Series",
		"rp=print", "priority=51", "tr=Generic 35c-4", "product=DriverName",
		"pdl=application/postscript", "note=Copy Room", "Transparent=T", "Binary=T", "TBCP=F"
	};

	//char *default_txt_rsp_2_for_serive[2] = { "_http", "_https" };
	char *default_txt_rsp_2[1] = {
		"Path=/"
	};

	//char *default_txt_rsp_3_for_serive[1] = { "_privet" };
	char *default_txt_rsp_3[5] = {
		"txtvers=1", "url=https://www.google.com/cloudprint",
		"type=printer", "cs=not-configured", "note=Copy Room"
	};

	//char *default_txt_rsp_4_for_serive[1] = { "_uscan" };
	char *default_txt_rsp_4[11] = {
		"txtvers=1", "vers=2.5", "adminurl=http://KM00D91C.local./wsd/a_network_airprint.xml",
		"representation=http://KM00D91C.local./wcd/DeviceIcon_1283png",
		"rs=eSCL", "ty=KONICA MINOLTA bishub C287", "note=japan",
		"pdl=image/jpeg,image/tiff,application/pdf",
		"UUID=dd5454cc-e196-5711-aa1f-35be49a6ca9f",
		"cs=color,grayscale,binary", "is=platen,adf,duplex=T"
	};

	/* reset ===>  */

	memset(pwrpriv->wowlan_war_offload_mdns_domain_name, 0, MAX_MDNS_DOMAIN_NAME_LEN);
	memset(pwrpriv->wowlan_war_offload_mdns_mnane, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_mnane));
	memset(pwrpriv->wowlan_war_offload_mdns_service, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_service));
	memset(pwrpriv->wowlan_war_offload_mdns_txt_rsp, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_txt_rsp));

	pwrpriv->wowlan_war_offload_mdns_domain_name_len = 0;
	pwrpriv->wowlan_war_offload_mdns_mnane_num = 0;
	pwrpriv->wowlan_war_offload_mdns_service_info_num = 0;
	pwrpriv->wowlan_war_offload_mdns_txt_rsp_num = 0;

	/* init  ===>  */

	if (is_set_default) {
		// domain_name
		pwrpriv->wowlan_war_offload_mdns_domain_name_len = strlen(default_domain_name);
		memcpy(pwrpriv->wowlan_war_offload_mdns_domain_name, default_domain_name, sizeof(default_domain_name));

		// machine name
		pwrpriv->wowlan_war_offload_mdns_mnane_num = 1;
		pwrpriv->wowlan_war_offload_mdns_mnane[0].name_len = default_machine_name_len;
		memcpy(pwrpriv->wowlan_war_offload_mdns_mnane[0].name, default_machine_name, default_machine_name_len);

		// service info
		pwrpriv->wowlan_war_offload_mdns_service_info_num = 8;
		memcpy(pwrpriv->wowlan_war_offload_mdns_service, default_sinfo, sizeof(default_sinfo));

		// type txt rsp 0~5
#if 0
		// 0
		for (offset = 0, i = 0; i < 25; i++) {
			pwrpriv->wowlan_war_offload_mdns_txt_rsp[0].txt[offset++] = strlen(default_txt_rsp_0[i]);
			memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[0].txt + offset, default_txt_rsp_0[i], strlen(default_txt_rsp_0[i]));
			offset += strlen(default_txt_rsp_0[i]);
			dev_info(global_idev.fullmac_dev, "==> default_txt_rsp_0[%d]: [%s](%zu), offset(%d)\n", i, default_txt_rsp_0[i], strlen(default_txt_rsp_0[i]), offset);
		}
		pwrpriv->wowlan_war_offload_mdns_txt_rsp[0].txt_len = offset;
		dev_info(global_idev.fullmac_dev, "==> offset = %d\n\n", offset);
#endif
		// 1
		for (offset = 0, i = 0; i < 13; i++) {
			pwrpriv->wowlan_war_offload_mdns_txt_rsp[1].txt[offset++] = strlen(default_txt_rsp_1[i]);
			memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[1].txt + offset, default_txt_rsp_1[i], strlen(default_txt_rsp_1[i]));
			offset += strlen(default_txt_rsp_1[i]);
		}
		pwrpriv->wowlan_war_offload_mdns_txt_rsp[1].txt_len = offset;
		dev_info(global_idev.fullmac_dev, "==> offset = %d\n\n", offset);

		// 2
		for (offset = 0, i = 0; i < 1; i++) {
			pwrpriv->wowlan_war_offload_mdns_txt_rsp[2].txt[offset++] = strlen(default_txt_rsp_2[i]);
			memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[2].txt + offset, default_txt_rsp_2[i], strlen(default_txt_rsp_2[i]));
			offset += strlen(default_txt_rsp_2[i]);
		}
		pwrpriv->wowlan_war_offload_mdns_txt_rsp[2].txt_len = offset;
		dev_info(global_idev.fullmac_dev, "==> offset = %d\n\n", offset);

		// 3
		for (offset = 0, i = 0; i < 5; i++) {
			pwrpriv->wowlan_war_offload_mdns_txt_rsp[3].txt[offset++] = strlen(default_txt_rsp_3[i]);
			memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[3].txt + offset, default_txt_rsp_3[i], strlen(default_txt_rsp_3[i]));
			offset += strlen(default_txt_rsp_3[i]);
		}
		pwrpriv->wowlan_war_offload_mdns_txt_rsp[3].txt_len = offset;
		dev_info(global_idev.fullmac_dev, "==> offset = %d\n\n", offset);

		// 4
		for (offset = 0, i = 0; i < 11; i++) {
			pwrpriv->wowlan_war_offload_mdns_txt_rsp[4].txt[offset++] = strlen(default_txt_rsp_4[i]);
			memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[4].txt + offset, default_txt_rsp_4[i], strlen(default_txt_rsp_4[i]));
			offset += strlen(default_txt_rsp_4[i]);
		}
		pwrpriv->wowlan_war_offload_mdns_txt_rsp[4].txt_len = offset;
		dev_info(global_idev.fullmac_dev, "==> offset = %d\n\n", offset);

		/* txt_rsp_num is always as MAX_MDNS_TXT_NUM because the input mechanism(new/append) makes the entities are not in order */
		pwrpriv->wowlan_war_offload_mdns_txt_rsp_num = MAX_MDNS_TXT_NUM;
	}
}

void rtw_set_offload_ctrl(u32 value)
{
	struct proxy_priv *pwrctl = &proxypriv;
	u32 mdns_para_len;
	u8 *mdns_para;

	pwrctl->wowlan_war_offload_ctrl = value;
	pwrctl->wowlan_war_offload_mode = value ? true : false;

	/* send offload control bits to device, enable/disable offload */
	llhw_war_offload_ctrl(pwrctl->wowlan_war_offload_mode, pwrctl->wowlan_war_offload_ctrl);

	/* update ip address */
	llhw_wifi_update_ip_addr();

	if (pwrctl->wowlan_war_offload_mode) {
		/* for mdns, update mdns parameters to device */
		if (pwrctl->wowlan_war_offload_ctrl & WAR_MDNS_EN) {
			mdns_para = (u8 *)kzalloc(4096, GFP_KERNEL);

			/* prepare mdns parameters */
			rtw_wow_prepare_mdns_para(mdns_para, &mdns_para_len);

			/* send mdns para to device*/
			llhw_war_set_mdns_param(mdns_para, mdns_para_len);

			kfree((void *)mdns_para);
		}
	}
}

void rtw_wow_prepare_mdns_para(u8 *pframe, u32 *plen)
{
	struct proxy_priv *pwrctl = &proxypriv;

	if (pwrctl->wowlan_war_offload_ctrl & WAR_MDNS_EN) {

		struct war_mdns_service_info *psinfo = pwrctl->wowlan_war_offload_mdns_service;
		u8 txt_in_ptr[31] = { 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
							  0x00, 0x13, 0x09, 0x74, 0x78, 0x74, 0x76, 0x65, 0x72, 0x73,
							  0x3d, 0x31, 0x08, 0x71, 0x74, 0x6f, 0x74, 0x61, 0x6c, 0x3d, 0x31
							};
		u32 mdns_offset = 0;
		u8 i = 0;

		/* 1. service info */
		pframe[mdns_offset] = 0x01;  // TLV(T)rl, mdns_para_len:98
		mdns_offset += 1;
		memcpy(pframe + mdns_offset, &pwrctl->wowlan_war_offload_mdns_service_info_num, 1);
		mdns_offset += 1;

		for (i = 0; i < pwrctl->wowlan_war_offload_mdns_service_info_num ; i++) {
			// 1.1 : construct service name string
			//	   : length of total service name string (service+transport+domain)
			pframe[mdns_offset] = psinfo[i].service_len + psinfo[i].transport_len + psinfo[i].domain_len + 4;
			mdns_offset += 1;

			//	   :  service name
			pframe[mdns_offset] = psinfo[i].service_len;
			mdns_offset += 1;
			memcpy(pframe + mdns_offset, &psinfo[i].service, psinfo[i].service_len);
			mdns_offset += psinfo[i].service_len;

			//	   : transport name
			pframe[mdns_offset] = psinfo[i].transport_len;
			mdns_offset += 1;
			memcpy(pframe + mdns_offset, &psinfo[i].transport, psinfo[i].transport_len);
			mdns_offset += psinfo[i].transport_len;

			//	   : domain name
			pframe[mdns_offset] = psinfo[i].domain_len;
			mdns_offset += 1;
			memcpy(pframe + mdns_offset, &psinfo[i].domain, psinfo[i].domain_len);
			mdns_offset += psinfo[i].domain_len;

			//	   : delimiter
			mdns_offset += 1;

			// 1.2 : construct type srv rsp
			pframe[mdns_offset] = psinfo[i].target_len + 19; // length
			pframe[mdns_offset + 2] = 0x21; // rsp type (srv)
			pframe[mdns_offset + 4] = 0x01; // cache flush + class
			memcpy(pframe + mdns_offset + 5, &psinfo[i].ttl, 4); // ttl
			pframe[mdns_offset + 5] = (u8)((psinfo[i].ttl & 0xff000000) >> 24);	// ttl - byte0
			pframe[mdns_offset + 6] = (u8)((psinfo[i].ttl & 0x00ff0000) >> 16);	// ttl - byte1
			pframe[mdns_offset + 7] = (u8)((psinfo[i].ttl & 0x0000ff00) >> 8);		// ttl - byte2
			pframe[mdns_offset + 8] = (u8)(psinfo[i].ttl & 0x000000ff);			// ttl - byte3
			pframe[mdns_offset + 10] = psinfo[i].target_len + 9;	  // data length
			memcpy(pframe + mdns_offset + 15, &psinfo[i].port, 2); // port
			memcpy(pframe + mdns_offset + 17, &psinfo[i].target_len, 1); // target len
			memcpy(pframe + mdns_offset + 18, &psinfo[i].target, psinfo[i].target_len); // target
			pframe[mdns_offset + 18 + psinfo[i].target_len] = 0xc0; // message compresion, offset will be filled by fw.
			mdns_offset += (1 + psinfo[i].target_len + 19);

			// 1.3 : set the idx of txt rsp
			pframe[mdns_offset] = psinfo[i].txt_rsp_idx;
			mdns_offset += 1;
		}

		/* 2. machine name */
		pframe[mdns_offset] = 0x02; // TLV(T)
		mdns_offset += 1;
		memcpy(pframe + mdns_offset, &pwrctl->wowlan_war_offload_mdns_mnane_num, 1); // NUM
		mdns_offset += 1;

		for (i = 0; i < pwrctl->wowlan_war_offload_mdns_mnane_num; i++) {
			pframe[mdns_offset] = pwrctl->wowlan_war_offload_mdns_mnane[i].name_len;
			memcpy(pframe + mdns_offset + 1, pwrctl->wowlan_war_offload_mdns_mnane[i].name,
				   pwrctl->wowlan_war_offload_mdns_mnane[i].name_len); // machine name
			mdns_offset += (1 + pwrctl->wowlan_war_offload_mdns_mnane[i].name_len);
		}

		/* 3. A rsp */
		pframe[mdns_offset] = 0x03; // TLV(T)
		pframe[mdns_offset + 1] = 14;	// TLV(L)
		pframe[mdns_offset + 3] = 0x01; // rsp type (a)
		pframe[mdns_offset + 5] = 0x01; // cache flush + class
		pframe[mdns_offset + 9] = 0xf0; // ttl (240 sec)
		pframe[mdns_offset + 11] = 4;	// length of ipv4 addr.
		memcpy(pframe + mdns_offset + 12, global_idev.ip_addr, 4);
		mdns_offset += (2 + 14);

		/* 4. AAAA rsp */
		pframe[mdns_offset] = 0x04; // TLV(T)
		pframe[mdns_offset + 1] = 26;	// TLV(L)
		pframe[mdns_offset + 3] = 0x1c; // rsp type (aaaa)
		pframe[mdns_offset + 5] = 0x01; // cache flush + class
		pframe[mdns_offset + 9] = 0xf0; // ttl (240 sec)
		pframe[mdns_offset + 11] = 16;	// length of ipv6 addr.
		memcpy(pframe + mdns_offset + 12, global_idev.ipv6_addr, 16);
		mdns_offset += (2 + 26);

		/* 5. PTR rsp */
		pframe[mdns_offset] = 0x05; // TLV(T)
		pframe[mdns_offset + 1] = 13 + pwrctl->wowlan_war_offload_mdns_domain_name_len; // TLV(L)
		pframe[mdns_offset + 3] = 0x0c; // rsp type (ptr)
		pframe[mdns_offset + 5] = 0x01; // cache flush + class
		pframe[mdns_offset + 8] = 0x1c; // ttl
		pframe[mdns_offset + 9] = 0x20; // ttl (7200 sec)
		pframe[mdns_offset + 11] = 3 + pwrctl->wowlan_war_offload_mdns_domain_name_len; // data length
		pframe[mdns_offset + 12] = pwrctl->wowlan_war_offload_mdns_domain_name_len; // domain name length
		memcpy(pframe + mdns_offset + 13, &pwrctl->wowlan_war_offload_mdns_domain_name,
			   pwrctl->wowlan_war_offload_mdns_domain_name_len);
		pframe[mdns_offset + 13 + pwrctl->wowlan_war_offload_mdns_domain_name_len] = 0xc0; // message compression
		mdns_offset += (2 + 13 + pwrctl->wowlan_war_offload_mdns_domain_name_len);

		/* 6. TXT in PTR rsp */
		pframe[mdns_offset] = 0x06; 		// TLV(T)
		pframe[mdns_offset + 1] = 31;	// TLV(L)
		memcpy(pframe + mdns_offset + 2, &txt_in_ptr, 31);
		mdns_offset += (2 + 31);

		/* 7. TXT rsp */
		pframe[mdns_offset] = 0x07; // TLV(T)
		mdns_offset += 1;
		memcpy(pframe + mdns_offset, &pwrctl->wowlan_war_offload_mdns_txt_rsp_num, 1); // NUM
		mdns_offset += 1;

		for (i = 0; i < pwrctl->wowlan_war_offload_mdns_txt_rsp_num; i++) {
			u16 txt_rsp_len = pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt_len;

			if (pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt_len == 0) {
				memcpy(pframe + mdns_offset, &txt_rsp_len,  2);
				mdns_offset += (2 + txt_rsp_len);
				continue;
			}

			txt_rsp_len += 10;
			memcpy(pframe + mdns_offset, &txt_rsp_len,  2);
			pframe[mdns_offset + 3] = 0x10; // rsp type (txt)
			pframe[mdns_offset + 5] = 0x01; // cache flush + class
			pframe[mdns_offset + 8] = 0x1c; // ttl
			pframe[mdns_offset + 9] = 0x20; // ttl (7200 sec)
			pframe[mdns_offset + 10] = (u8)((pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt_len & 0xff00) >> 8);
			pframe[mdns_offset + 11] = (u8)(pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt_len & 0x00ff);
			memcpy(pframe + mdns_offset + 12, &pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt,
				   pwrctl->wowlan_war_offload_mdns_txt_rsp[i].txt_len);
			mdns_offset  += (2 + txt_rsp_len);
		}

		*plen = mdns_offset;
	}
}


void rtw_wow_war_mdns_dump_buf(struct seq_file *m, u8 *title, u8 *buf, u32 len)
{
	u32 i;

	seq_printf(m, "\t%s (%d)\n\t\t", title, len);
	for (i = 1; i <= len; i++) {
		seq_printf(m, "%2.2x-", *(buf + i - 1));
		if ((i % 16 == 0) && (len != i)) {
			seq_printf(m, "\n\t\t");
		}
	}
	seq_printf(m, "\n\n");
}

void rtw_wow_war_mdns_dump_txt(struct seq_file *m, u8 *title, u8 *buf, u32 len)
{
	u16 idx = 1, offset = 0; /* offset = the location of L in the Length.Value */

	seq_printf(m, "\t%s (%d)\n\t", title, len);
	for (; offset < len; idx++) {
		int item_len = buf[offset];
		u8 item_buf[256] = {0};

		memcpy(item_buf, (buf + offset + 1), item_len);
		seq_printf(m, "\t[%d] => %s (%d)\n\t", idx, item_buf, item_len);
		memset(item_buf, 0, sizeof(item_buf));
		offset += (1 + item_len);
	}
	seq_printf(m, "\n\n");
}

inline bool IsHexDigit(char chTmp)
{
	if ((chTmp >= '0' && chTmp <= '9') ||
		(chTmp >= 'a' && chTmp <= 'f') ||
		(chTmp >= 'A' && chTmp <= 'F')) {
		return true;
	} else {
		return false;
	}
}

bool rtw_wow_war_mdns_parser_pattern(u8 *input, char *target,
									 u32 *target_len, u32 type)
{
	char *cp = NULL;
	size_t len = 0;
	int pos = 0, mask_pos = 0, res = 0;
	u8 member[3] = {0};

	/* reset */
	memset(target, '\0', type);
	(*target_len) = 0;

	cp = strchr(input, '=');
	if (cp) {
		*cp = 0;
		cp++;
		input = cp;
	}

	while (1) {
		cp = strchr(input, ':');

		if (cp) {
			len = strlen(input) - strlen(cp);
			*cp = 0;
			cp++;
		} else {
			len = 2;
		}

		{
			u8 hex, idx = 0, pos_in_unit_as_4bit = 0;

			strncpy(member, input, len);
			res = sscanf(member, "%02hhx", &hex);

			target[pos] = hex;

			/* RTW_INFO("==> in; input-member = %s, hex = %x,  target[%d] = %x\n", member, hex, target[pos], pos); */

			for (idx = 0; idx < 2; idx++) {
				pos_in_unit_as_4bit =  pos * 2 + (1 - idx);
				mask_pos = (pos_in_unit_as_4bit / 8);

				if (!IsHexDigit(member[idx])) {
					dev_err(global_idev.fullmac_dev, "%s:[ERROR] pattern is invalid!!(%c)\n", __func__, member[idx]);
					goto error;
				}

				/* RTW_INFO("==> in; pos = %d, pos_in_unit_as_4bit = %d, mask-pos = %d \n", pos, pos_in_unit_as_4bit, mask_pos);
				RTW_INFO("==> in; hex(0x%02x), member(%c%c) \n", pattern[pos], member[1], member[0]); */
			}
			/* RTW_INFO_DUMP("Pattern Mask: ",bit_mask, 6); */
		}

		pos++;
		if (!cp) {
			break;
		}
		input = cp;
	}

	(*target_len) = pos;

	return true;
error:
	return false;

}

int proc_get_offload_enable(struct seq_file *m, void *v)
{
	struct proxy_priv *pwrctl = &proxypriv;

	if (true == pwrctl->wowlan_war_offload_mode) {
		seq_printf(m, "\n[ Offload Feature Enabled ]\n");

		if (WAR_ARP_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n ARP Reponse offload enabled\n");
		}
#ifdef CONFIG_OFFLOAD_ICMP_NS
		if (WAR_ICMPV6_NS_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n IPv6 ICMP NS Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_ICMP_NS */
#ifdef CONFIG_OFFLOAD_ICMP_V4
		if (WAR_ICMPV4_ECHO_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n IPv4 ICMP Echo Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_ICMP_V4 */
#ifdef CONFIG_OFFLOAD_ICMP_V6
		if (WAR_ICMPV6_ECHO_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n IPv6 ICMP Echo Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_ICMP_V6 */
#ifdef CONFIG_OFFLOAD_NETBIOS
		if (WAR_NETBIOS_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n NetBIOS Name Resolution Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_NETBIOS */
#ifdef CONFIG_OFFLOAD_LLMNR_V4
		if (WAR_LLMNR_V4_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n LLMNR v4 Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_LLMNR_v4 */
#ifdef CONFIG_OFFLOAD_LLMNR_V6
		if (WAR_LLMNR_V6_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n LLMNR v6 Reponse offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_LLMNR_V6 */

#ifdef CONFIG_OFFLOAD_SNMP_V4
		if (WAR_SNMP_V4_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SNMP v4 Reponse offload enabled\n");
		}
		if (WAR_SNMP_V4_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SNMP v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SNMP_v4 */
#ifdef CONFIG_OFFLOAD_SNMP_V6
		if (WAR_SNMP_V6_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SNMP v6 Reponse offload enabled\n");
		}
		if (WAR_SNMP_V6_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SNMP v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SNMP_V6 */
#ifdef CONFIG_OFFLOAD_MDNS_V4
		if (WAR_MDNS_V4_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n MDNS v4 Reponse offload enabled\n");
		}
		if (WAR_MDNS_V4_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n MDNS v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_MDNS_v4 */
#ifdef CONFIG_OFFLOAD_MDNS_V6
		if (WAR_MDNS_V6_RSP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n MDNS v6 Reponse offload enabled\n");
		}
		if (WAR_MDNS_V6_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n MDNS v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_MDNS_V6 */

#ifdef CONFIG_OFFLOAD_SSDP_V4
		if (WAR_SSDP_V4_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SSDP v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SSDP_V4 */
#ifdef CONFIG_OFFLOAD_SSDP_V6
		if (WAR_SSDP_V6_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SSDP v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SSDP_V6 */
#ifdef CONFIG_OFFLOAD_WSD_V4
		if (WAR_WSD_V4_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n WSD v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_WSD_V4 */
#ifdef CONFIG_OFFLOAD_WSD_V6
		if (WAR_WSD_V6_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n WSD v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_WSD_V6 */
#ifdef CONFIG_OFFLOAD_SLP_V4
		if (WAR_SLP_V4_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SLP v4 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SLP_V4 */
#ifdef CONFIG_OFFLOAD_SLP_V6
		if (WAR_SLP_V6_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n SLP v6 Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_SLP_V6 */
#ifdef CONFIG_OFFLOAD_DESIGNATED_MAC
		if (WAR_DESIGNATED_MAC_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n Designated MAC Wakeup offload enabled\n");
		}
#endif /* CONFIG_OFFLOAD_DESIGNATED_MAC */
#ifdef CONFIG_OFFLOAD_LLTD
		if (WAR_LLTD_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n LLTD Wakeup offload enabled\n");
		}
#endif
		if (WAR_ARP_WAKEUP_EN & pwrctl->wowlan_war_offload_ctrl) {
			seq_printf(m, "\n ARP Request wakeup enabled\n");
		}

	} else {
		seq_printf(m, "\n[ Offload Feature Disabled ]\n");
	}

	return 0;
}

ssize_t proc_set_offload_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[10];
	u32 offload_cfg = 0;

	if (NULL == buffer) {
		dev_err(global_idev.fullmac_dev, "input buffer is NULL");
		return -EFAULT;
	}

	if (count < 1) {
		dev_err(global_idev.fullmac_dev, "input length is 0!\n");
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		sscanf(tmp, "%x", &offload_cfg);
		rtw_set_offload_ctrl(offload_cfg);
	}

	return count;

}

int proc_get_offload_mdns_domain_name(struct seq_file *m, void *v)
{
	struct proxy_priv *pwrpriv = &proxypriv;

	if (true == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			seq_printf(m, "\nDomain Name:[%s](%d)\n\n",
					   pwrpriv->wowlan_war_offload_mdns_domain_name, pwrpriv->wowlan_war_offload_mdns_domain_name_len);
		} else {
			seq_printf(m, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		seq_printf(m, "\nOffload Not enabled\n\n");
	}

	return 0;
}

ssize_t proc_set_offload_mdns_domain_name(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	char tmp[MAX_MDNS_DOMAIN_NAME_LEN + 1];
	char domain_name[MAX_MDNS_DOMAIN_NAME_LEN + 1];

	if (NULL == buffer) {
		dev_err(global_idev.fullmac_dev, "input buffer is NULL");
		return -EFAULT;
	}

	if (count < 1) {
		dev_err(global_idev.fullmac_dev, "input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		dev_err(global_idev.fullmac_dev, "input length is large than MAX_MDNS_DOMAIN_NAME_LEN(%d)\n", MAX_MDNS_DOMAIN_NAME_LEN);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, MAX_MDNS_DOMAIN_NAME_LEN_FOR_SSCAN, domain_name);
		if (1 == num) {
			pwrpriv->wowlan_war_offload_mdns_domain_name_len = strlen(domain_name);
			memset(pwrpriv->wowlan_war_offload_mdns_domain_name, 0x00, MAX_MDNS_DOMAIN_NAME_LEN);
			memcpy(pwrpriv->wowlan_war_offload_mdns_domain_name, domain_name, strlen(domain_name));
		}
	}

	return count;
}


int proc_get_offload_mdns_machine_name(struct seq_file *m, void *v)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	int i = 0;

	if (true == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl)  ||
			(WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			for (i = 0; i < pwrpriv->wowlan_war_offload_mdns_mnane_num; i++) {
				seq_printf(m, "[%d]", i);
				rtw_wow_war_mdns_dump_buf(m, "Machine Name",
										  pwrpriv->wowlan_war_offload_mdns_mnane[i].name, pwrpriv->wowlan_war_offload_mdns_mnane[i].name_len);
			}
			seq_printf(m, "\n");
		} else {
			seq_printf(m, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		seq_printf(m, "\nOffload Not enabled\n\n");
	}

	return 0;
}


ssize_t proc_set_offload_mdns_machine_name(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	char tmp[MAX_MDNS_MACHINE_NAME_LEN * 3 - 1 + 1];

	if (NULL == buffer) {
		dev_err(global_idev.fullmac_dev, "input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		dev_err(global_idev.fullmac_dev, "input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		dev_err(global_idev.fullmac_dev, "input length, %lu, is large than MAX_MDNS_MACHINE_NAME_LEN(%d)\n", (count + 1) / 3,
				MAX_MDNS_MACHINE_NAME_LEN);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		if (strncmp(tmp, "clean", 5) == 0) {
			memset(pwrpriv->wowlan_war_offload_mdns_mnane, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_mnane));
			pwrpriv->wowlan_war_offload_mdns_mnane_num = 0;
		} else {
			int idx = pwrpriv->wowlan_war_offload_mdns_mnane_num;
			if (idx == MAX_MDNS_MACHINE_NAME_NUM) {
				dev_err(global_idev.fullmac_dev, "the num of machine name is already %d(MAX_MDNS_MACHINE_NAME_NUM)!\n", MAX_MDNS_MACHINE_NAME_NUM);
				return -EFAULT;
			}
			if (rtw_wow_war_mdns_parser_pattern(tmp, pwrpriv->wowlan_war_offload_mdns_mnane[idx].name,
												(u32 *) &pwrpriv->wowlan_war_offload_mdns_mnane[idx].name_len, MAX_MDNS_MACHINE_NAME_LEN)) {
				pwrpriv->wowlan_war_offload_mdns_mnane_num++;
			}
		}
	}

	return count;
}


int proc_get_offload_mdns_service_info(struct seq_file *m, void *v)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	struct war_mdns_service_info *psinfo = pwrpriv->wowlan_war_offload_mdns_service;
	int i = 0, j = 0;

	if (true == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V4_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_WAKEUP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			for (i = 0; i < pwrpriv->wowlan_war_offload_mdns_service_info_num; i++) {
				seq_printf(m, "[%d] service info ===> \n", i + 1);
				seq_printf(m, "\tservice-transport-domain : %s(%d)- %s(%d)- %s(%d)\n",
						   psinfo[i].service, psinfo[i].service_len,
						   psinfo[i].transport, psinfo[i].transport_len,
						   psinfo[i].domain, psinfo[i].domain_len);
				seq_printf(m, "\ttarget for srv rsp : %s(%d)\n", psinfo[i].target, psinfo[i].target_len);
				seq_printf(m, "\tport : %x-%x, ttl : %d \n", psinfo[i].port[0], psinfo[i].port[1], psinfo[i].ttl);
				j = psinfo[i].txt_rsp_idx;
				seq_printf(m, "\ttype txt rsp. [%d] \n", j);
				rtw_wow_war_mdns_dump_txt(m, "type txt rsp. (Str)",
										  pwrpriv->wowlan_war_offload_mdns_txt_rsp[j].txt, pwrpriv->wowlan_war_offload_mdns_txt_rsp[j].txt_len);

			}
			seq_printf(m, "\n");
		} else {
			seq_printf(m, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		seq_printf(m, "\nOffload Not enabled\n\n");
	}

	return 0;
}

ssize_t proc_set_offload_mdns_service_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	struct war_mdns_service_info *psinfo = pwrpriv->wowlan_war_offload_mdns_service;
	u8 port[2];
	char *tmp = NULL;
	char srv[MAX_MDNS_SERVICE_NAME_LEN + 1], trans[MAX_MDNS_TRANS_LEN + 1], domain[MAX_MDNS_DOMAIN_LEN + 1];
	char target[MAX_MDNS_TARGET_LEN + 1];
	u32 ttl, port0 = 0, port1 = 0;
	u16 max_input_size = (MAX_MDNS_SERVICE_NAME_LEN + MAX_MDNS_TRANS_LEN + MAX_MDNS_DOMAIN_LEN + MAX_MDNS_TARGET_LEN + 2);
	int txt_idx;

	if (NULL == buffer) {
		dev_err(global_idev.fullmac_dev, "input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		dev_err(global_idev.fullmac_dev, "input length is 0!\n");
		return -EFAULT;
	}

	if (count > (sizeof(char) * (max_input_size))) {
		dev_err(global_idev.fullmac_dev, "input length is too large\n");
		return -EFAULT;
	}

	tmp = rtw_malloc(sizeof(char) * (max_input_size), NULL);
	if (NULL == tmp) {
		dev_err(global_idev.fullmac_dev, "tmp buffer allocate fail!!\n");
		count = -EFAULT;
		goto exit;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%15s %4s %5s %63s %x %x %u %d", srv, trans, domain, target, &port0, &port1, &ttl, &txt_idx);
		/*  MAX_MDNS_SERVICE_NAME_LEN(15),  MAX_MDNS_TRANS_LEN(4), MAX_MDNS_DOMAIN_LEN(5), MAX_MDNS_TARGET_LEN(63) */
		int idx = pwrpriv->wowlan_war_offload_mdns_service_info_num;
		u16 sscanf_parameter_length = strlen(srv) + strlen(trans) + strlen(domain) + strlen(target) + 2 + 2 + 4 + 1 + num;

		if (strncmp(srv, "clean", 5) == 0) {
			memset(pwrpriv->wowlan_war_offload_mdns_service, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_service));
			pwrpriv->wowlan_war_offload_mdns_service_info_num = 0;
		} else if (count != sscanf_parameter_length) {
			dev_err(global_idev.fullmac_dev, "Length of total parameters does not match the input buffer. (%d != %lu)\n",
					sscanf_parameter_length, count);
			dev_err(global_idev.fullmac_dev, "Please check the content and length of each parameter.\n");
			dev_err(global_idev.fullmac_dev, "input buffer = (%s)(%lu)!\n\n", tmp, count);
			dev_err(global_idev.fullmac_dev, "srv = %s (%lu)!\n", srv, strlen(srv));
			dev_err(global_idev.fullmac_dev, "trans = %s (%lu)!\n", trans, strlen(trans));
			dev_err(global_idev.fullmac_dev, "domain = %s (%lu)!\n", domain, strlen(domain));
			dev_err(global_idev.fullmac_dev, "target = %s (%lu)!\n", target, strlen(target));
			dev_err(global_idev.fullmac_dev, "port = %x-%x, ttl = %d!\n", port0, port1, ttl);
			dev_err(global_idev.fullmac_dev, "txt idx = %d!\n", txt_idx);
			count = -EFAULT;
			goto exit;
		} else {
			port[0] = (u8)port0;
			port[1] = (u8)port1;

			if (txt_idx >= MAX_MDNS_TXT_NUM) {
				dev_err(global_idev.fullmac_dev, "input txt idx, %d, is out of range (0~%d)!\n", txt_idx, MAX_MDNS_TXT_NUM - 1);
				count = -EFAULT;
				goto exit;
			}

			if (pwrpriv->wowlan_war_offload_mdns_txt_rsp[txt_idx].txt_len == 0) {
				dev_err(global_idev.fullmac_dev, "wowlan_war_offload_mdns_txt_rsp[%d] is null! Please initiate it first.\n", txt_idx);
				count = -EFAULT;
				goto exit;
			}

			// 1. set the value of members for this new service
			psinfo[idx].service_len = strlen(srv);
			memcpy(psinfo[idx].service, srv, psinfo[idx].service_len);
			psinfo[idx].transport_len = strlen(trans);
			memcpy(psinfo[idx].transport, trans, psinfo[idx].transport_len);
			psinfo[idx].domain_len = strlen(domain);
			memcpy(psinfo[idx].domain, domain, psinfo[idx].domain_len);
			psinfo[idx].target_len = strlen(target);
			memcpy(psinfo[idx].target, target, psinfo[idx].target_len);
			memcpy(psinfo[idx].port, port, 2);
			psinfo[idx].ttl = ttl;
			psinfo[idx].txt_rsp_idx = txt_idx;
			pwrpriv->wowlan_war_offload_mdns_service_info_num++;
		}
	}

exit:
	if (tmp) {
		rtw_mfree(sizeof(char) * (max_input_size), tmp, (dma_addr_t)NULL);
	}
	return count;

}

int proc_get_offload_mdns_txt_rsp(struct seq_file *m, void *v)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	int i = 0;

	if (true == pwrpriv->wowlan_war_offload_mode) {
		if ((WAR_MDNS_V4_RSP_EN & pwrpriv->wowlan_war_offload_ctrl) ||
			(WAR_MDNS_V6_RSP_EN & pwrpriv->wowlan_war_offload_ctrl)) {
			for (i = 0; i < pwrpriv->wowlan_war_offload_mdns_txt_rsp_num; i++) {
				seq_printf(m, "[%d]", i);

				if (pwrpriv->wowlan_war_offload_mdns_txt_rsp[i].txt_len == 0) {
					seq_printf(m, " (null)\n");
					continue;
				}

				rtw_wow_war_mdns_dump_txt(m, "type txt rsp. (Str)",
										  pwrpriv->wowlan_war_offload_mdns_txt_rsp[i].txt, pwrpriv->wowlan_war_offload_mdns_txt_rsp[i].txt_len);
				rtw_wow_war_mdns_dump_buf(m, "type txt rsp. (Hex)",
										  pwrpriv->wowlan_war_offload_mdns_txt_rsp[i].txt, pwrpriv->wowlan_war_offload_mdns_txt_rsp[i].txt_len);
			}
			seq_printf(m, "\n");
		} else {
			seq_printf(m, "\nMSND RSP Not enabled\n\n");
		}
	} else {
		seq_printf(m, "\nOffload Not enabled\n\n");
	}

	return 0;
}

ssize_t proc_set_offload_mdns_txt_rsp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct proxy_priv *pwrpriv = &proxypriv;
	u16 max_input_size = (1 + 6 + MAX_MDNS_TXT_SINGLE_LEN + 2);
	char *tmp = NULL;
	char op[7] = {0}, txt_str[MAX_MDNS_TXT_SINGLE_LEN + 1] = {0};
	int idx;

	if (NULL == buffer) {
		dev_err(global_idev.fullmac_dev, "input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		dev_err(global_idev.fullmac_dev, "input length is 0!\n");
		return -EFAULT;
	}

	tmp = rtw_malloc(sizeof(char) * (max_input_size), NULL);
	if (NULL == tmp) {
		dev_err(global_idev.fullmac_dev, "tmp buffer allocate fail!!\n");
		count = -EFAULT;
		goto exit;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		if (strncmp(tmp, "clean", 5) == 0) {
			/* clean ==> */
			if (pwrpriv->wowlan_war_offload_mdns_service_info_num == 0) {
				memset(pwrpriv->wowlan_war_offload_mdns_txt_rsp, 0, sizeof(pwrpriv->wowlan_war_offload_mdns_txt_rsp));
			} else {
				dev_err(global_idev.fullmac_dev, "Txt rsp are refered! (Current service_info_num = %d)\n", pwrpriv->wowlan_war_offload_mdns_service_info_num);
				count = -EFAULT;
				goto exit;
			}

		} else {
			/* set ==> */
			int num = sscanf(tmp, "%d %6s %256c", &idx, op, txt_str);
			u16 sscanf_parameter_length = 0, txt_len = 0;

			txt_len = (strlen(txt_str) > MAX_MDNS_TXT_SINGLE_LEN) ? MAX_MDNS_TXT_SINGLE_LEN : (strlen(txt_str) - 1);
			txt_str[txt_len] = '\0';
			sscanf_parameter_length = 1 + strlen(op) + txt_len + num;

			if (count != sscanf_parameter_length) {
				dev_err(global_idev.fullmac_dev, "Length of total parameters does not match the input buffer. (%d != %lu)(num=%d)\n",
						sscanf_parameter_length, count, num);
				dev_err(global_idev.fullmac_dev, "Please check the content and length of each parameter.\n");
				dev_err(global_idev.fullmac_dev, "input buffer = (%s)(%lu)!\n\n", tmp, count);
				dev_err(global_idev.fullmac_dev, "op. = %s (%lu)!\n", op, strlen(op));
				dev_err(global_idev.fullmac_dev, "txt = %s (%lu)!\n", txt_str, strlen(txt_str));
				count = -EFAULT;
				goto exit;
			} else {

				u16 offset;

				if (idx >= MAX_MDNS_TXT_NUM) {
					dev_err(global_idev.fullmac_dev, "the index, %d, is over the range of txt rsp(0~%d)!\n", idx, MAX_MDNS_TXT_NUM - 1);
					count = -EFAULT;
					goto exit;
				}

				if (strncmp(op, "new", 3) == 0) {
					memset(pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt, 0, pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len);
					pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len = 0;
				} else if (strncmp(op, "append", 6) == 0) {
					if ((pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len + strlen(txt_str) + 1) > MAX_MDNS_TXT_LEN) {
						dev_err(global_idev.fullmac_dev, "the txt rsp(%d) will be over the limitation(%d) if append input string(%lu)!\n",
								pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len,
								MAX_MDNS_TXT_LEN, strlen(txt_str) + 1);
						count = -EFAULT;
						goto exit;
					}
				} else {
					dev_err(global_idev.fullmac_dev, "Invaild op str %s (new/append only)!\n", op);
					count = -EFAULT;
					goto exit;
				}

				offset = pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len;
				pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt[offset++] = strlen(txt_str);
				memcpy(pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt + offset, txt_str, strlen(txt_str));
				pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len += (strlen(txt_str) + 1);  /* actul len with length field */

				/* Dump ==> */
				//RTW_PRINT_SEL(RTW_DBGDUMP, "[%d]", idx);
				//rtw_wow_war_mdns_dump_txt(RTW_DBGDUMP, "type txt rsp. (Str)",
				//		pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt, pwrpriv->wowlan_war_offload_mdns_txt_rsp[idx].txt_len);
			}
		}
	}

exit:
	if (tmp) {
		rtw_mfree(sizeof(char) * (max_input_size), tmp, (dma_addr_t)NULL);
	}
	return count;

}

