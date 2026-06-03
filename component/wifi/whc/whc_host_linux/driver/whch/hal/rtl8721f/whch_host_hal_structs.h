
#ifndef __WHCH_HOST_HAL_STRUCTS_H__
#define __WHCH_HOST_HAL_STRUCTS_H__

#define BIT_LEN_MASK_32(__BitLen)	(0xFFFFFFFFUL >> (32 - (__BitLen)))
#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
	((le32_to_cpu(*((u32 *)(__pStart))) >> (__BitOffset)) & BIT_LEN_MASK_32(__BitLen))

/* RX status descriptor accessors (Offset 0) */
#define GET_RX_STATUS_DESC_PKT_LEN(p)		LE_BITS_TO_4BYTE(p,      0, 14)
#define GET_RX_STATUS_DESC_CRC32(p)		LE_BITS_TO_4BYTE(p,     14,  1)
#define GET_RX_STATUS_DESC_ICV(p)		LE_BITS_TO_4BYTE(p,     15,  1)
#define GET_RX_STATUS_DESC_SECURITY(p)		LE_BITS_TO_4BYTE(p,     20,  3)
#define GET_RX_STATUS_DESC_SHIFT(p)		LE_BITS_TO_4BYTE(p,     23,  2)
#define GET_RX_STATUS_DESC_SWDEC(p)		LE_BITS_TO_4BYTE(p,     25,  1)
/* RX status descriptor accessors (Offset 4) */
#define GET_RX_STATUS_DESC_TID(p)		LE_BITS_TO_4BYTE((p)+4,  8,  4)
#define GET_RX_STATUS_DESC_PKT_INDICATE(p)	LE_BITS_TO_4BYTE((p)+4, 16,  3)
#define GET_RX_STATUS_DESC_PAM(p)		LE_BITS_TO_4BYTE((p)+4, 24,  1)
#define GET_RX_STATUS_DESC_MORE_DATA(p)		LE_BITS_TO_4BYTE((p)+4, 26,  1)
#define GET_RX_STATUS_DESC_MORE_FRAG(p)		LE_BITS_TO_4BYTE((p)+4, 27,  1)
#define GET_RX_STATUS_DESC_TYPE(p)		LE_BITS_TO_4BYTE((p)+4, 28,  2)
#define GET_RX_STATUS_DESC_BC(p)		LE_BITS_TO_4BYTE((p)+4, 31,  1)
/* RX status descriptor accessors (Offset 8) */
#define GET_RX_STATUS_DESC_SEQ(p)		LE_BITS_TO_4BYTE((p)+8,  0, 12)
#define GET_RX_STATUS_DESC_FRAG(p)		LE_BITS_TO_4BYTE((p)+8, 12,  4)
#define GET_RX_STATUS_DESC_QOS(p)		LE_BITS_TO_4BYTE((p)+8, 16,  1)
#define GET_RX_STATUS_DESC_GI_LTF(p)		LE_BITS_TO_4BYTE((p)+8, 25,  3)
#define GET_RX_STATUS_DESC_C2H(p)		LE_BITS_TO_4BYTE((p)+8, 28,  1)
/* RX status descriptor accessors (Offset 12) */
#define GET_RX_STATUS_DESC_RX_RATE(p)		LE_BITS_TO_4BYTE((p)+12, 0,  9)
#define GET_RX_STATUS_DESC_PWR_LV(p)		LE_BITS_TO_4BYTE((p)+12, 12, 9)
#define GET_RX_STATUS_DESC_CH_INFO_CSI_LEN(p)	LE_BITS_TO_4BYTE((p)+12, 16, 10)
#define GET_RX_STATUS_DESC_CH_INFO_BW(p)	LE_BITS_TO_4BYTE((p)+12, 26,  2)
#define GET_RX_STATUS_DESC_PHYSTS_LEN(p)	LE_BITS_TO_4BYTE((p)+12, 16,  8)
/* RX status descriptor accessors (Offset 16) */
#define GET_RX_STATUS_DESC_TXRPTMID_SRCH(p)	LE_BITS_TO_4BYTE((p)+16, 17,  7)
/* DRV info size is always 0 for this chip */
#define GET_RX_STATUS_DESC_DRV_INFO_SIZE(p)	0

/* RX buffer descriptor accessors */
#define GET_RX_BUFFER_DESC_FS_92E(rxbd)	((le32_to_cpu(*((u32 *)(rxbd))) >> 15) & 0x1)
#define GET_RX_BUFFER_DESC_LS_92E(rxbd)	((le32_to_cpu(*((u32 *)(rxbd))) >> 14) & 0x1)

#define QSLT_BK							0x2//0x01
#define QSLT_BE							0x0
#define QSLT_VI							0x5//0x4
#define QSLT_VO							0x7//0x6
#define QSLT_HIGH						0x11

#define VO_QUEUE_INX		0
#define VI_QUEUE_INX		1
#define BE_QUEUE_INX		2
#define BK_QUEUE_INX		3
#define BCN_QUEUE_INX		4
#define MGT_QUEUE_INX		5
#define HIGH_QUEUE_INX		6
#define TXCMD_QUEUE_INX		7

#define GEN_HT_RATE_IDX(MCS) (0x80 | ((MCS) & 0x1f))
#define GEN_VHT_RATE_IDX(SS, MCS) (0x100 | (((SS) & 0x3) << 4) | ((MCS) & 0xf))
#define GEN_HE_RATE_IDX(SS, MCS) (0x180 | (((SS) & 0x3) << 4) | ((MCS) & 0xf))


#define TXDESC_SIZE	40
#define RXDESC_SIZE 24

#define RX_BUFFER_DESC_SIZE	8

struct txdesc_priv {
	// Offset 0
	u32 pktlen: 16;
	u32 offset: 8;
	u32 bmc: 1;
	u32 darf_tc_idx: 1; // 1: data_fb_time1
	u32 tx_swap: 1;
	u32 ul_swap: 1;
	u32 bcnpkt_tsf_ctrl: 1;
	u32 noacm: 1;
	u32 gf: 1;
	u32 rsvd0031: 1;

	// Offset 4
	u32 macid: 7;
	u32 bip_keyid_sel: 1;
	u32 qsel: 5;
	u32 force_txop: 1;
	u32 lsig_txop_en: 1;
	u32 pifs: 1;
	u32 a_ctrl_cas: 1;
	u32 a_ctrl_bsr: 1;
	u32 a_ctrl_uph: 1;
	u32 a_ctrl_bqr: 1;
	u32 htc: 1;
	u32 en_desc_id: 1;
	u32 sectype: 2;
	u32 pkt_offset: 5; // unit: 8 bytes
	u32 moredata: 1;
	u32 ftm_en: 1;
	u32 hw_aesiv: 1;

	// Offset 8
	u32 p_aid: 9;
	u32 tri_frame: 1;
	u32 cca_rts: 2;
	u32 agg_en: 1;
	u32 ack_ch_info: 1;
	u32 null_0: 1;
	u32 null_1: 1;
	u32 bk: 1;
	u32 tm_en: 1;
	u32 raw: 1;
	u32 ccx: 1;
	u32 ampdu_density: 3;
	u32 bt_int: 1;
	u32 spe_frame_ind: 3;
	u32 mu_snd_flag: 1;
	u32 sw_use_pkt_type: 2;
	u32 sw_use_eapol4: 1;
	u32 sw_use_addba: 1;

	// Offset 12
	u32 wheader_len: 5;
	u32 txdesc_info_en: 1;
	u32 hw_ssn_sel: 2;
	u32 userate: 1;
	u32 disrtsfb: 1;
	u32 disdatafb: 1;
	u32 cts2self: 1;
	u32 rtsen: 1;
	u32 hw_rts_en: 1;
	u32 chk_en: 1;
	u32 navusehdr: 1;
	u32 use_max_time: 1;
	u32 max_agg_num: 5;
	u32 ndpa: 2;
	u32 ampdu_max_time: 8;

	// Offset 16
	u32 datarate: 9;
	u32 data_rty_lowest_rate: 9;
	u32 retry_limit: 6;
	u32 obw_cts2self_dup_type: 4;
	u32 try_rate: 1;
	u32 data_gi: 3;

	// Offset 20
	u32 data_sc: 4;
	u32 data_dcm: 1;
	u32 data_bw: 2;
	u32 data_ldpc: 1;
	u32 data_stbc: 2;
	u32 rts_stbc: 2;
	u32 rts_short: 1;
	u32 rts_sc: 4;
	u32 signaling_tapkt: 1;
	u32 data_bw_er: 1;
	u32 data_er: 1;
	u32 mbssid: 4;
	u32 txpwr_offset: 5;
	u32 port_id: 2;
	u32 polluted: 1;

	// Offset 24
	u32 rtsrate: 9;
	u32 rts_rty_lowest_rate: 9;
	u32 rsvd2418: 1;
	u32 rty_use_des: 1;
	u32 rsvd2420: 12;

	// Offset 28
	u32 checksum: 16;
	u32 sw_define_l: 7;
	u32 usb_txagg_num: 9;

	// Offset 32
	u32 rts_rc: 6;
	u32 bar_rty_th: 2;
	u32 data_rc: 6;
	u32 vendor_adv: 1;
	u32 en_hwseq: 1;
	u32 nextheadpage_l: 8;
	u32 tailpage_l: 8;

	// Offset 36
	u32 group_bit_ie: 8;
	u32 sw_define_h: 4;
	u32 seq: 12;
	u32 nextheadpage_h: 4;
	u32 tailpage_h: 4;
};

#endif  //__WHCH_HOST_HAL_STRUCTS_H__